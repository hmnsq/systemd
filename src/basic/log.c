/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <threads.h>
#include <unistd.h>

#include "sd-messages.h"

#include "alloc-util.h"
#include "ansi-color.h"
#include "argv-util.h"
#include "env-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "format-util.h"
#include "iovec-util.h"
#include "list.h"
#include "log.h"
#include "log-context.h"
#include "parse-util.h"
#include "proc-cmdline.h"
#include "process-util.h"
#include "ratelimit.h"
#include "signal-util.h"
#include "socket-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "syslog-util.h"
#include "terminal-util.h"
#include "time-util.h"
#include "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

static log_syntax_callback_t log_syntax_callback = NULL;
static void *log_syntax_callback_userdata = NULL;

static LogTarget log_target = LOG_TARGET_CONSOLE;
static целое log_max_level = LOG_INFO;
static целое log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
static целое log_facility = LOG_DAEMON;
static bool ratelimit_kmsg = true;

static целое console_fd = STDERR_FILENO;
static целое console_fd_is_tty = -1; /* tri-state: -1 means don't know */
static целое syslog_fd = -EBADF;
static целое kmsg_fd = -EBADF;
static целое journal_fd = -EBADF;

static bool syslog_is_stream = false;

static целое show_color = -1; /* tristate */
static bool show_location = false;
static bool show_time = false;
static bool show_tid = false;
static bool show_executable = false;

static bool upgrade_syslog_to_journal = false;
static bool always_reopen_console = false;
static bool open_when_needed = false;
static bool prohibit_ipc = false;

static thread_local const char *log_prefix = NULL;

#если LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(expr)                                                \
        do {                                                            \
                если (_unlikely_(!(expr))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } пока (false)

void log_prefix_swap(const char **prefix) {
        assert(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

static void log_close_console(void) {
        /* See comment in log_close_journal() */
        (void) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

static целое log_open_console(void) {

        если (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                вернуть 0;
        }

        если (console_fd < 3) {
                целое fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                если (fd < 0)
                        вернуть fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        вернуть 0;
}

static void log_close_kmsg(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(kmsg_fd));
}

static целое log_open_kmsg(void) {

        если (kmsg_fd >= 0)
                вернуть 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        если (kmsg_fd < 0)
                вернуть -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        вернуть 0;
}

static void log_close_syslog(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(syslog_fd));
}

static целое create_log_socket(целое type) {
        struct timeval tv;
        целое fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        если (fd < 0)
                вернуть -errno;

        fd = fd_move_above_stdio(fd);
        (void) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        если (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        иначе
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (void) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

        вернуть fd;
}

static целое log_open_syslog(void) {
        целое r;

        если (syslog_fd >= 0)
                вернуть 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        если (syslog_fd < 0) {
                r = syslog_fd;
                goto fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        если (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                если (syslog_fd < 0) {
                        r = syslog_fd;
                        goto fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                если (r < 0)
                        goto fail;

                syslog_is_stream = true;
        } иначе
                syslog_is_stream = false;

        вернуть 0;

fail:
        log_close_syslog();
        вернуть r;
}

static void log_close_journal(void) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (void) safe_close(TAKE_FD(journal_fd));
}

static целое log_open_journal(void) {
        целое r;

        если (journal_fd >= 0)
                вернуть 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        если (journal_fd < 0) {
                r = journal_fd;
                goto fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        если (r < 0)
                goto fail;

        вернуть 0;

fail:
        log_close_journal();
        вернуть r;
}

bool stderr_is_journal(void) {
        _cleanup_free_ char *w = NULL;
        const char *e;
        uint64_t dev, ino;
        struct stat st;

        e = getenv("JOURNAL_STREAM");
        если (!e)
                вернуть false;

        если (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                вернуть false;
        если (!e)
                вернуть false;

        если (safe_atou64(w, &dev) < 0)
                вернуть false;
        если (safe_atou64(e, &ino) < 0)
                вернуть false;

        если (fstat(STDERR_FILENO, &st) < 0)
                вернуть false;

        вернуть st.st_dev == dev && st.st_ino == ino;
}

целое log_open(void) {
        целое r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        если (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                вернуть 0;
        }

        если (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                если (!prohibit_ipc) {
                        если (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                если (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        вернуть r;
                                }
                        }

                        если (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                если (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        вернуть r;
                                }
                        }
                }

                если (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        если (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                вернуть r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        вернуть log_open_console();
}

void log_set_target(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_MAX);

        если (upgrade_syslog_to_journal) {
                если (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                иначе если (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

void log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

void log_close(void) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

void log_forget_fds(void) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

целое log_set_max_level(целое level) {
        assert(level == LOG_NULL || log_level_is_valid(level));

        целое old = log_max_level;
        log_max_level = level;

        /* Also propagate max log level to libc's syslog(), just in case some other component loaded into our
         * process logs directly via syslog(). You might wonder why we maintain our own log level variable if
         * libc has the same functionality. This has multiple reasons, first and foremost that we want to
         * apply this to all our log targets, not just syslog and console. Moreover, we cannot query the
         * current log mask from glibc without changing it, but that's useful for testing the current log
         * level before even entering the log functions like we do in our macros. */
        setlogmask(LOG_UPTO(level));

        /* Ensure that our own LOG_NULL define maps sanely to the log mask */
        assert_cc(LOG_UPTO(LOG_NULL) == 0);

        вернуть old;
}

void log_set_facility(целое facility) {
        log_facility = facility;
}

static bool check_console_fd_is_tty(void) {
        если (console_fd < 0)
                вернуть false;

        если (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        вернуть console_fd_is_tty;
}

static целое write_to_console(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *buffer) {

        static целое dumb = -1;

        char location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(целое) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        struct iovec iovec[13];
        const char *on = NULL, *off = NULL;
        size_t n = 0;

        если (console_fd < 0)
                вернуть 0;

        если (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        если (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                вернуть 0;

        если (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        если (show_time &&
            format_timestamp(header_time, sizeof(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        если (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        если (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        если (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, NULL);

        если (show_location) {
                const char *lon = "", *loff = "";
                если (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (void) snprintf(location, sizeof location, "%s%s:%i%s: ", lon, file, line, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        если (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        если (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        если (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        если (writev(console_fd, iovec, n) < 0) {

                если (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (void) log_open_console();
                        если (console_fd < 0)
                                вернуть 0;

                        если (writev(console_fd, iovec, n) < 0)
                                вернуть -errno;
                } иначе
                        вернуть -errno;
        }

        вернуть 1;
}

static целое write_to_syslog(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *buffer) {

        char header_priority[2 + DECIMAL_STR_MAX(целое) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        struct tm tm;
        целое r;

        если (syslog_fd < 0)
                вернуть 0;

        если (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                вернуть 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        если (r < 0)
                вернуть r;

        если (strftime(header_time, sizeof(header_time), "%h %e %T ", &tm) <= 0)
                вернуть -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        struct iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        const struct msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        если (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        для (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                если (n < 0)
                        вернуть -errno;

                если (!syslog_is_stream)
                        break;

                если (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        break;
        }

        вернуть 1;
}

static целое write_to_kmsg(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        static thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        char header_priority[2 + DECIMAL_STR_MAX(целое) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        если (kmsg_fd < 0)
                вернуть 0;

        если (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                вернуть 0;

        если (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                если (ratelimit_num_dropped(&ratelimit) > 1)
                        вернуть 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        const struct iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        если (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                вернуть -errno;

        вернуть 1;
}

static целое log_do_header(
                char *header,
                size_t size,
                целое level,
                целое error,
                const char *file, целое line, const char *func,
                const char *object_field, const char *object,
                const char *extra_field, const char *extra) {
        целое r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_VALUE(error);

        r = snprintf(header, size,
                     "PRIORITY=%i\n"
                     "SYSLOG_FACILITY=%i\n"
                     "TID=" PID_FMT "\n"
                     "%s%.256s%s"        /* CODE_FILE */
                     "%s%.*i%s"          /* CODE_LINE */
                     "%s%.256s%s"        /* CODE_FUNC */
                     "%s%.*i%s"          /* ERRNO */
                     "%s%.256s%s"        /* object */
                     "%s%.256s%s"        /* extra */
                     "SYSLOG_IDENTIFIER=%.256s\n",
                     LOG_PRI(level),
                     LOG_FAC(level),
                     gettid(),
                     isempty(file) ? "" : "CODE_FILE=",
                     isempty(file) ? "" : file,
                     isempty(file) ? "" : "\n",
                     line ? "CODE_LINE=" : "",
                     line ? 1 : 0, line, /* %.0d means no output too, special case for 0 */
                     line ? "\n" : "",
                     isempty(func) ? "" : "CODE_FUNC=",
                     isempty(func) ? "" : func,
                     isempty(func) ? "" : "\n",
                     error ? "ERRNO=" : "",
                     error ? 1 : 0, error,
                     error ? "\n" : "",
                     isempty(object) ? "" : ASSERT_PTR(object_field),
                     isempty(object) ? "" : object,
                     isempty(object) ? "" : "\n",
                     isempty(extra) ? "" : ASSERT_PTR(extra_field),
                     isempty(extra) ? "" : extra,
                     isempty(extra) ? "" : "\n",
                     program_invocation_short_name);
        assert_raw((size_t) r < size);

        вернуть 0;
}

static void log_do_context(struct iovec *iovec, size_t iovec_len, size_t *n) {
        assert(iovec);
        assert(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        если (*n + 2 >= iovec_len)
                                вернуть;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                для (size_t i = 0; i < c->n_input_iovec; i++) {
                        если (*n + 2 >= iovec_len)
                                вернуть;

                        iovec[(*n)++] = c->input_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                если (c->key && c->value) {
                        если (*n + 3 >= iovec_len)
                                вернуть;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->key);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->value);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

static целое write_to_journal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                const char *buffer) {

        char header[LINE_MAX];
        size_t n = 0, iovec_len;
        struct iovec *iovec;

        если (journal_fd < 0)
                вернуть 0;

        если (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                вернуть 0;

        если (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        иначе
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(struct iovec, iovec_len);

        log_do_header(header, sizeof(header), level, error, file, line, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        если (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        const struct msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        если (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                вернуть -errno;

        вернуть 1;
}

целое log_dispatch_internal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                char *buffer) {

        assert_raw(buffer);

        если (log_target == LOG_TARGET_NULL)
                вернуть -ERRNO_VALUE(error);

        /* Patch in LOG_DAEMON facility if necessary */
        если (LOG_FAC(level) == 0)
                level |= log_facility;

        если (open_when_needed)
                (void) log_open();

        do {
                char *e;
                целое k = 0;

                buffer += strspn(buffer, NEWLINE);

                если (buffer[0] == 0)
                        break;

                если ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                если (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, line, func, object_field, object, extra_field, extra, buffer);
                        если (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                если (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, line, func, buffer);
                        если (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                если (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        если (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, line, func, buffer);
                        если (k < 0) {
                                log_close_kmsg();
                                (void) log_open_console();
                        }
                }

                если (k <= 0)
                        (void) write_to_console(level, error, file, line, func, buffer);

                buffer = e;
        } пока (buffer);

        если (open_when_needed)
                log_close();

        вернуть -ERRNO_VALUE(error);
}

целое log_dump_internal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                char *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        если (_likely_(LOG_PRI(level) > log_max_level))
                вернуть -ERRNO_VALUE(error);

        вернуть log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

целое log_internalv(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *format,
                va_list ap) {

        если (_likely_(LOG_PRI(level) > log_max_level))
                вернуть -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        char buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_VALUE(error));

        (void) vsnprintf(buffer, sizeof buffer, format, ap);

        вернуть log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

целое log_internal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *format, ...) {

        va_list ap;
        целое r;

        va_start(ap, format);
        r = log_internalv(level, error, file, line, func, format, ap);
        va_end(ap);

        вернуть r;
}

целое log_object_internalv(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                const char *format,
                va_list ap) {

        char *buffer, *b;

        если (_likely_(LOG_PRI(level) > log_max_level))
                вернуть -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_VALUE(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(char, LINE_MAX);
        (void) vsnprintf(b, LINE_MAX, format, ap);

        вернуть log_dispatch_internal(level, error, file, line, func,
                                     object_field, object, extra_field, extra, buffer);
}

целое log_object_internal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                const char *format, ...) {

        va_list ap;
        целое r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, line, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        вернуть r;
}

целое log_oom_internal(целое level, const char *file, целое line, const char *func) {
        вернуть log_internal(level, ENOMEM, file, line, func, "Out of memory.");
}

целое log_format_iovec(
                struct iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                целое error,
                const char *format,
                va_list ap) {

        assert(iovec);
        assert(n);

        пока (format && *n + 1 < iovec_len) {
                va_list aq;
                char *m;
                целое r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                если (r < 0)
                        вернуть -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                если (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, char *);
        }
        вернуть 0;
}

целое log_struct_internal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *format, ...) {

        char buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        если (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                вернуть -ERRNO_VALUE(error);

        если (LOG_FAC(level) == 0)
                level |= log_facility;

        если (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                если (open_when_needed)
                        log_open_journal();

                если (journal_fd >= 0) {
                        char header[LINE_MAX];
                        struct iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        целое r;
                        bool fallback = false;

                        если (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        иначе
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(struct iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, sizeof(header), level, error, file, line, func, NULL, NULL, NULL, NULL);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        если (r < 0)
                                fallback = true;
                        иначе {
                                log_do_context(iovec, iovec_len, &n);

                                const struct msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (void) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        для (size_t i = 1; i < m; i += 2)
                                free(iovec[i].iov_base);

                        если (!fallback) {
                                если (open_when_needed)
                                        log_close();

                                вернуть -ERRNO_VALUE(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        пока (format) {
                va_list aq;

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (void) vsnprintf(buf, sizeof buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                если (startswith(buf, "MESSAGE=")) {
                        found = true;
                        break;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, char *);
        }
        va_end(ap);

        если (!found) {
                если (open_when_needed)
                        log_close();

                вернуть -ERRNO_VALUE(error);
        }

        вернуть log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buf + 8);
}

целое log_struct_iovec_internal(
                целое level,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const struct iovec input_iovec[],
                size_t n_input_iovec) {

        PROTECT_ERRNO;

        если (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                вернуть -ERRNO_VALUE(error);

        если (LOG_FAC(level) == 0)
                level |= log_facility;

        если (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                char header[LINE_MAX];
                struct iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_input_iovec_expanded;

                если (!MUL_SAFE(&n_input_iovec_expanded, n_input_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_num_fields(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_input_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                иначе
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(struct iovec, iovec_len);

                log_do_header(header, sizeof(header), level, error, file, line, func, NULL, NULL, NULL, NULL);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                для (size_t i = 0; i < n_input_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = input_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                const struct msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                если (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        вернуть -ERRNO_VALUE(error);
        }

        для (size_t i = 0; i < n_input_iovec; i++)
                если (memory_startswith(input_iovec[i].iov_base, input_iovec[i].iov_len, "MESSAGE=")) {
                        char *m;

                        m = strndupa_safe((char*) input_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          input_iovec[i].iov_len - STRLEN("MESSAGE="));

                        вернуть log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, m);
                }

        /* Couldn't find MESSAGE=. */
        вернуть -ERRNO_VALUE(error);
}

целое log_set_target_from_string(const char *e) {
        LogTarget t;

        t = log_target_from_string(e);
        если (t < 0)
                вернуть t;

        log_set_target(t);
        вернуть 0;
}

целое log_set_max_level_from_string(const char *e) {
        целое r;

        для (;;) {
                _cleanup_free_ char *word = NULL, *prefix = NULL;
                LogTarget target;
                const char *colon;

                r = extract_first_word(&e, &word, ",", 0);
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        break;

                colon = strchr(word, ':');
                если (!colon) {
                        r = log_level_from_string(word);
                        если (r < 0)
                                вернуть r;

                        log_set_max_level(r);
                        continue;
                }

                prefix = strndup(word, colon - word);
                если (!prefix)
                        вернуть -ENOMEM;

                target = log_target_from_string(prefix);
                если (target < 0)
                        вернуть target;

                если (target >= _LOG_TARGET_SINGLE_MAX)
                        вернуть -EINVAL;

                r = log_level_from_string(colon + 1);
                если (r < 0)
                        вернуть r;

                log_target_max_level[target] = r;
        }

        вернуть 0;
}

целое log_max_levels_to_string(целое level, char **ret) {
        _cleanup_free_ char *s = NULL;
        целое r;

        assert(ret);

        r = log_level_to_string_alloc(level, &s);
        если (r < 0)
                вернуть r;

        для (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ char *l = NULL;

                если (log_target_max_level[target] == INT_MAX)
                        continue;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                если (r < 0)
                        вернуть r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                если (r < 0)
                        вернуть r;
        }

        *ret = TAKE_PTR(s);
        вернуть 0;
}

static целое log_set_ratelimit_kmsg_from_string(const char *e) {
        целое r;

        r = parse_boolean(e);
        если (r < 0)
                вернуть r;

        ratelimit_kmsg = r;
        вернуть 0;
}

static целое parse_proc_cmdline_item(const char *key, const char *value, void *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        если (streq(key, "debug") && !value)
                log_set_max_level(LOG_DEBUG);

        иначе если (proc_cmdline_key_streq(key, "systemd.log_target")) {

                если (proc_cmdline_value_missing(key, value))
                        вернуть 0;

                если (log_set_target_from_string(value) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", value);

        } иначе если (proc_cmdline_key_streq(key, "systemd.log_level")) {

                если (proc_cmdline_value_missing(key, value))
                        вернуть 0;

                если (log_set_max_level_from_string(value) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", value);

        } иначе если (proc_cmdline_key_streq(key, "systemd.log_color")) {

                если (log_show_color_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", value);

        } иначе если (proc_cmdline_key_streq(key, "systemd.log_location")) {

                если (log_show_location_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", value);

        } иначе если (proc_cmdline_key_streq(key, "systemd.log_tid")) {

                если (log_show_tid_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", value);

        } иначе если (proc_cmdline_key_streq(key, "systemd.log_time")) {

                если (log_show_time_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", value);

        } иначе если (proc_cmdline_key_streq(key, "systemd.log_ratelimit_kmsg")) {

                если (log_set_ratelimit_kmsg_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", value);
        }

        вернуть 0;
}

static bool should_parse_proc_cmdline(void) {
        /* PID1 always reads the kernel command line. */
        если (getpid_cached() == 1)
                вернуть true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        вернуть invoked_by_systemd();
}

void log_parse_environment_variables(void) {
        const char *e;
        целое r;

        e = getenv("SYSTEMD_LOG_TARGET");
        если (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        если (e) {
                r = log_set_max_level_from_string(e);
                если (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } иначе {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                если (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                иначе если (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        если (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        если (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        если (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        если (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        если (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        если (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

void log_parse_environment(void) {
        /* Do not call from library code. */

        если (should_parse_proc_cmdline())
                (void) proc_cmdline_parse(parse_proc_cmdline_item, NULL, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(void) {
        вернуть log_target;
}

void log_settle_target(void) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        если (t != LOG_TARGET_AUTO)
                вернуть;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

целое log_get_max_level(void) {
        вернуть log_max_level;
}

целое log_get_target_max_level(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_SINGLE_MAX);
        вернуть log_target_max_level[target];
}

void log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(void) {
        вернуть show_color > 0; /* Defaults to false. */
}

void log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(void) {
        вернуть show_location;
}

void log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(void) {
        вернуть show_time;
}

void log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(void) {
        вернуть show_tid;
}

void log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(void) {
        вернуть show_executable;
}

целое log_show_color_from_string(const char *e) {
        целое r;

        r = parse_boolean(e);
        если (r < 0)
                вернуть r;

        log_show_color(r);
        вернуть 0;
}

целое log_show_location_from_string(const char *e) {
        целое r;

        r = parse_boolean(e);
        если (r < 0)
                вернуть r;

        log_show_location(r);
        вернуть 0;
}

целое log_show_time_from_string(const char *e) {
        целое r;

        r = parse_boolean(e);
        если (r < 0)
                вернуть r;

        log_show_time(r);
        вернуть 0;
}

целое log_show_tid_from_string(const char *e) {
        целое r;

        r = parse_boolean(e);
        если (r < 0)
                вернуть r;

        log_show_tid(r);
        вернуть 0;
}

целое log_show_executable_from_string(const char *e) {
        целое r;

        r = parse_boolean(e);
        если (r < 0)
                вернуть r;

        log_show_executable(r);
        вернуть 0;
}

bool log_on_console(void) {
        если (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                вернуть true;

        вернуть syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

static const char *const log_target_table[_LOG_TARGET_MAX] = {
        [LOG_TARGET_CONSOLE]          = "console",
        [LOG_TARGET_CONSOLE_PREFIXED] = "console-prefixed",
        [LOG_TARGET_KMSG]             = "kmsg",
        [LOG_TARGET_JOURNAL]          = "journal",
        [LOG_TARGET_JOURNAL_OR_KMSG]  = "journal-or-kmsg",
        [LOG_TARGET_SYSLOG]           = "syslog",
        [LOG_TARGET_SYSLOG_OR_KMSG]   = "syslog-or-kmsg",
        [LOG_TARGET_AUTO]             = "auto",
        [LOG_TARGET_NULL]             = "null",
};

DEFINE_STRING_TABLE_LOOKUP(log_target, LogTarget);

void log_received_signal(целое level, const struct signalfd_siginfo *si) {
        assert(si);

        если (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ char *p = NULL;

                (void) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } иначе
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

void set_log_syntax_callback(log_syntax_callback_t cb, void *userdata) {
        assert(!log_syntax_callback || !cb);
        assert(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

целое log_syntax_internal(
                const char *unit,
                целое level,
                const char *config_file,
                unsigned config_line,
                целое error,
                const char *file,
                целое line,
                const char *func,
                const char *format, ...) {

        PROTECT_ERRNO;

        если (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        если (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                вернуть -ERRNO_VALUE(error);

        char buffer[LINE_MAX];
        va_list ap;
        const char *unit_fmt = NULL;

        errno = ERRNO_VALUE(error);

        va_start(ap, format);
        (void) vsnprintf(buffer, sizeof buffer, format, ap);
        va_end(ap);

        если (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        если (config_file) {
                если (config_line > 0)
                        вернуть log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_line),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_line, buffer),
                                        unit_fmt, unit,
                                        NULL);
                иначе
                        вернуть log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        NULL);
        } иначе если (unit)
                вернуть log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                NULL);
        иначе
                вернуть log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                NULL);
}

целое log_syntax_invalid_utf8_internal(
                const char *unit,
                целое level,
                const char *config_file,
                unsigned config_line,
                const char *file,
                целое line,
                const char *func,
                const char *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ char *p = NULL;

        если (rvalue)
                p = utf8_escape_invalid(rvalue);

        вернуть log_syntax_internal(unit, level, config_file, config_line,
                                   SYNTHETIC_ERRNO(EINVAL), file, line, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

целое log_syntax_parse_error_internal(
                const char *unit,
                const char *config_file,
                unsigned config_line,
                целое error,
                bool critical,
                const char *file,
                целое line,
                const char *func,
                const char *lvalue,
                const char *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ char *escaped = NULL;

        /* OOM is always handled as critical. */
        если (ERRNO_VALUE(error) == ENOMEM)
                вернуть log_oom_internal(LOG_ERR, file, line, func);

        если (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                если (!escaped)
                        rvalue = "(oom)";
                иначе
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_line, error,
                            file, line, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        вернуть critical ? -ERRNO_VALUE(error) : 0;
}

void log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        если (b) {
                если (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                иначе если (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

void log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

void log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

void log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

целое log_emergency_level(void) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        вернуть getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

целое log_dup_console(void) {
        целое copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        если (console_fd < 0 || console_fd >= 3)
                вернуть 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        если (copy < 0)
                вернуть -errno;

        console_fd = copy;
        вернуть 0;
}

void log_setup(void) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (void) log_open();
        если (log_on_console() && show_color < 0)
                log_show_color(true);
}

const char* _log_set_prefix(const char *prefix, bool force) {
        const char *old = log_prefix;

        если (prefix || force)
                log_prefix = prefix;

        вернуть old;
}
