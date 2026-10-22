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
static цяло log_max_level = LOG_INFO;
static цяло log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
static цяло log_facility = LOG_DAEMON;
static bool ratelimit_kmsg = true;

static цяло console_fd = STDERR_FILENO;
static цяло console_fd_is_tty = -1; /* tri-state: -1 means don't know */
static цяло syslog_fd = -EBADF;
static цяло kmsg_fd = -EBADF;
static цяло journal_fd = -EBADF;

static bool syslog_is_stream = false;

static цяло show_color = -1; /* tristate */
static bool show_location = false;
static bool show_time = false;
static bool show_tid = false;
static bool show_executable = false;

static bool upgrade_syslog_to_journal = false;
static bool always_reopen_console = false;
static bool open_when_needed = false;
static bool prohibit_ipc = false;

static thread_local const char *log_prefix = NULL;

#ако LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(expr)                                                \
        do {                                                            \
                ако (_unlikely_(!(expr))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } докато (false)

void log_prefix_swap(const char **prefix) {
        assert(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

static void log_close_console(void) {
        /* See comment in log_close_journal() */
        (void) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

static цяло log_open_console(void) {

        ако (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                върни 0;
        }

        ако (console_fd < 3) {
                цяло fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ако (fd < 0)
                        върни fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        върни 0;
}

static void log_close_kmsg(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(kmsg_fd));
}

static цяло log_open_kmsg(void) {

        ако (kmsg_fd >= 0)
                върни 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ако (kmsg_fd < 0)
                върни -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        върни 0;
}

static void log_close_syslog(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(syslog_fd));
}

static цяло create_log_socket(цяло type) {
        struct timeval tv;
        цяло fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ако (fd < 0)
                върни -errno;

        fd = fd_move_above_stdio(fd);
        (void) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ако (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        иначе
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (void) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

        върни fd;
}

static цяло log_open_syslog(void) {
        цяло r;

        ако (syslog_fd >= 0)
                върни 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ако (syslog_fd < 0) {
                r = syslog_fd;
                goto fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        ако (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ако (syslog_fd < 0) {
                        r = syslog_fd;
                        goto fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                ако (r < 0)
                        goto fail;

                syslog_is_stream = true;
        } иначе
                syslog_is_stream = false;

        върни 0;

fail:
        log_close_syslog();
        върни r;
}

static void log_close_journal(void) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (void) safe_close(TAKE_FD(journal_fd));
}

static цяло log_open_journal(void) {
        цяло r;

        ако (journal_fd >= 0)
                върни 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ако (journal_fd < 0) {
                r = journal_fd;
                goto fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ако (r < 0)
                goto fail;

        върни 0;

fail:
        log_close_journal();
        върни r;
}

bool stderr_is_journal(void) {
        _cleanup_free_ char *w = NULL;
        const char *e;
        uint64_t dev, ino;
        struct stat st;

        e = getenv("JOURNAL_STREAM");
        ако (!e)
                върни false;

        ако (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                върни false;
        ако (!e)
                върни false;

        ако (safe_atou64(w, &dev) < 0)
                върни false;
        ако (safe_atou64(e, &ino) < 0)
                върни false;

        ако (fstat(STDERR_FILENO, &st) < 0)
                върни false;

        върни st.st_dev == dev && st.st_ino == ino;
}

цяло log_open(void) {
        цяло r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ако (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                върни 0;
        }

        ако (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                ако (!prohibit_ipc) {
                        ако (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                ако (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        върни r;
                                }
                        }

                        ако (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                ако (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        върни r;
                                }
                        }
                }

                ако (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        ако (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                върни r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        върни log_open_console();
}

void log_set_target(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_MAX);

        ако (upgrade_syslog_to_journal) {
                ако (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                иначе ако (target == LOG_TARGET_SYSLOG_OR_KMSG)
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

цяло log_set_max_level(цяло level) {
        assert(level == LOG_NULL || log_level_is_valid(level));

        цяло old = log_max_level;
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

        върни old;
}

void log_set_facility(цяло facility) {
        log_facility = facility;
}

static bool check_console_fd_is_tty(void) {
        ако (console_fd < 0)
                върни false;

        ако (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        върни console_fd_is_tty;
}

static цяло write_to_console(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *buffer) {

        static цяло dumb = -1;

        char location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(цяло) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        struct iovec iovec[13];
        const char *on = NULL, *off = NULL;
        size_t n = 0;

        ако (console_fd < 0)
                върни 0;

        ако (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ако (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                върни 0;

        ако (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        ако (show_time &&
            format_timestamp(header_time, sizeof(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ако (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ако (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ако (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, NULL);

        ако (show_location) {
                const char *lon = "", *loff = "";
                ако (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (void) snprintf(location, sizeof location, "%s%s:%i%s: ", lon, file, line, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ако (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ако (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        ако (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ако (writev(console_fd, iovec, n) < 0) {

                ако (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (void) log_open_console();
                        ако (console_fd < 0)
                                върни 0;

                        ако (writev(console_fd, iovec, n) < 0)
                                върни -errno;
                } иначе
                        върни -errno;
        }

        върни 1;
}

static цяло write_to_syslog(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *buffer) {

        char header_priority[2 + DECIMAL_STR_MAX(цяло) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        struct tm tm;
        цяло r;

        ако (syslog_fd < 0)
                върни 0;

        ако (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                върни 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ако (r < 0)
                върни r;

        ако (strftime(header_time, sizeof(header_time), "%h %e %T ", &tm) <= 0)
                върни -EINVAL;

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
        ако (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        за (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ако (n < 0)
                        върни -errno;

                ако (!syslog_is_stream)
                        break;

                ако (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        break;
        }

        върни 1;
}

static цяло write_to_kmsg(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
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

        char header_priority[2 + DECIMAL_STR_MAX(цяло) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ако (kmsg_fd < 0)
                върни 0;

        ако (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                върни 0;

        ако (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                ако (ratelimit_num_dropped(&ratelimit) > 1)
                        върни 0;

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

        ако (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                върни -errno;

        върни 1;
}

static цяло log_do_header(
                char *header,
                size_t size,
                цяло level,
                цяло error,
                const char *file, цяло line, const char *func,
                const char *object_field, const char *object,
                const char *extra_field, const char *extra) {
        цяло r;

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

        върни 0;
}

static void log_do_context(struct iovec *iovec, size_t iovec_len, size_t *n) {
        assert(iovec);
        assert(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        ако (*n + 2 >= iovec_len)
                                върни;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                за (size_t i = 0; i < c->n_input_iovec; i++) {
                        ако (*n + 2 >= iovec_len)
                                върни;

                        iovec[(*n)++] = c->input_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ако (c->key && c->value) {
                        ако (*n + 3 >= iovec_len)
                                върни;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->key);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->value);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

static цяло write_to_journal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                const char *buffer) {

        char header[LINE_MAX];
        size_t n = 0, iovec_len;
        struct iovec *iovec;

        ако (journal_fd < 0)
                върни 0;

        ако (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                върни 0;

        ако (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        иначе
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(struct iovec, iovec_len);

        log_do_header(header, sizeof(header), level, error, file, line, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ако (log_prefix) {
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

        ако (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                върни -errno;

        върни 1;
}

цяло log_dispatch_internal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                char *buffer) {

        assert_raw(buffer);

        ако (log_target == LOG_TARGET_NULL)
                върни -ERRNO_VALUE(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ако (LOG_FAC(level) == 0)
                level |= log_facility;

        ако (open_when_needed)
                (void) log_open();

        do {
                char *e;
                цяло k = 0;

                buffer += strspn(buffer, NEWLINE);

                ако (buffer[0] == 0)
                        break;

                ако ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                ако (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, line, func, object_field, object, extra_field, extra, buffer);
                        ако (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ако (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, line, func, buffer);
                        ако (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ако (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        ако (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, line, func, buffer);
                        ако (k < 0) {
                                log_close_kmsg();
                                (void) log_open_console();
                        }
                }

                ако (k <= 0)
                        (void) write_to_console(level, error, file, line, func, buffer);

                buffer = e;
        } докато (buffer);

        ако (open_when_needed)
                log_close();

        върни -ERRNO_VALUE(error);
}

цяло log_dump_internal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                char *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ако (_likely_(LOG_PRI(level) > log_max_level))
                върни -ERRNO_VALUE(error);

        върни log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

цяло log_internalv(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *format,
                va_list ap) {

        ако (_likely_(LOG_PRI(level) > log_max_level))
                върни -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        char buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_VALUE(error));

        (void) vsnprintf(buffer, sizeof buffer, format, ap);

        върни log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

цяло log_internal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *format, ...) {

        va_list ap;
        цяло r;

        va_start(ap, format);
        r = log_internalv(level, error, file, line, func, format, ap);
        va_end(ap);

        върни r;
}

цяло log_object_internalv(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                const char *format,
                va_list ap) {

        char *buffer, *b;

        ако (_likely_(LOG_PRI(level) > log_max_level))
                върни -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_VALUE(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(char, LINE_MAX);
        (void) vsnprintf(b, LINE_MAX, format, ap);

        върни log_dispatch_internal(level, error, file, line, func,
                                     object_field, object, extra_field, extra, buffer);
}

цяло log_object_internal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *object_field,
                const char *object,
                const char *extra_field,
                const char *extra,
                const char *format, ...) {

        va_list ap;
        цяло r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, line, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        върни r;
}

цяло log_oom_internal(цяло level, const char *file, цяло line, const char *func) {
        върни log_internal(level, ENOMEM, file, line, func, "Out of memory.");
}

цяло log_format_iovec(
                struct iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                цяло error,
                const char *format,
                va_list ap) {

        assert(iovec);
        assert(n);

        докато (format && *n + 1 < iovec_len) {
                va_list aq;
                char *m;
                цяло r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ако (r < 0)
                        върни -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ако (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, char *);
        }
        върни 0;
}

цяло log_struct_internal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *format, ...) {

        char buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        ако (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                върни -ERRNO_VALUE(error);

        ако (LOG_FAC(level) == 0)
                level |= log_facility;

        ако (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                ако (open_when_needed)
                        log_open_journal();

                ако (journal_fd >= 0) {
                        char header[LINE_MAX];
                        struct iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        цяло r;
                        bool fallback = false;

                        ако (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
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
                        ако (r < 0)
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
                        за (size_t i = 1; i < m; i += 2)
                                free(iovec[i].iov_base);

                        ако (!fallback) {
                                ако (open_when_needed)
                                        log_close();

                                върни -ERRNO_VALUE(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        докато (format) {
                va_list aq;

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (void) vsnprintf(buf, sizeof buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ако (startswith(buf, "MESSAGE=")) {
                        found = true;
                        break;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, char *);
        }
        va_end(ap);

        ако (!found) {
                ако (open_when_needed)
                        log_close();

                върни -ERRNO_VALUE(error);
        }

        върни log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buf + 8);
}

цяло log_struct_iovec_internal(
                цяло level,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const struct iovec input_iovec[],
                size_t n_input_iovec) {

        PROTECT_ERRNO;

        ако (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                върни -ERRNO_VALUE(error);

        ако (LOG_FAC(level) == 0)
                level |= log_facility;

        ако (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                char header[LINE_MAX];
                struct iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_input_iovec_expanded;

                ако (!MUL_SAFE(&n_input_iovec_expanded, n_input_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_num_fields(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_input_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                иначе
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(struct iovec, iovec_len);

                log_do_header(header, sizeof(header), level, error, file, line, func, NULL, NULL, NULL, NULL);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                за (size_t i = 0; i < n_input_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = input_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                const struct msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ако (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        върни -ERRNO_VALUE(error);
        }

        за (size_t i = 0; i < n_input_iovec; i++)
                ако (memory_startswith(input_iovec[i].iov_base, input_iovec[i].iov_len, "MESSAGE=")) {
                        char *m;

                        m = strndupa_safe((char*) input_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          input_iovec[i].iov_len - STRLEN("MESSAGE="));

                        върни log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, m);
                }

        /* Couldn't find MESSAGE=. */
        върни -ERRNO_VALUE(error);
}

цяло log_set_target_from_string(const char *e) {
        LogTarget t;

        t = log_target_from_string(e);
        ако (t < 0)
                върни t;

        log_set_target(t);
        върни 0;
}

цяло log_set_max_level_from_string(const char *e) {
        цяло r;

        за (;;) {
                _cleanup_free_ char *word = NULL, *prefix = NULL;
                LogTarget target;
                const char *colon;

                r = extract_first_word(&e, &word, ",", 0);
                ако (r < 0)
                        върни r;
                ако (r == 0)
                        break;

                colon = strchr(word, ':');
                ако (!colon) {
                        r = log_level_from_string(word);
                        ако (r < 0)
                                върни r;

                        log_set_max_level(r);
                        continue;
                }

                prefix = strndup(word, colon - word);
                ако (!prefix)
                        върни -ENOMEM;

                target = log_target_from_string(prefix);
                ако (target < 0)
                        върни target;

                ако (target >= _LOG_TARGET_SINGLE_MAX)
                        върни -EINVAL;

                r = log_level_from_string(colon + 1);
                ако (r < 0)
                        върни r;

                log_target_max_level[target] = r;
        }

        върни 0;
}

цяло log_max_levels_to_string(цяло level, char **ret) {
        _cleanup_free_ char *s = NULL;
        цяло r;

        assert(ret);

        r = log_level_to_string_alloc(level, &s);
        ако (r < 0)
                върни r;

        за (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ char *l = NULL;

                ако (log_target_max_level[target] == INT_MAX)
                        continue;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                ако (r < 0)
                        върни r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                ако (r < 0)
                        върни r;
        }

        *ret = TAKE_PTR(s);
        върни 0;
}

static цяло log_set_ratelimit_kmsg_from_string(const char *e) {
        цяло r;

        r = parse_boolean(e);
        ако (r < 0)
                върни r;

        ratelimit_kmsg = r;
        върни 0;
}

static цяло parse_proc_cmdline_item(const char *key, const char *value, void *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ако (streq(key, "debug") && !value)
                log_set_max_level(LOG_DEBUG);

        иначе ако (proc_cmdline_key_streq(key, "systemd.log_target")) {

                ако (proc_cmdline_value_missing(key, value))
                        върни 0;

                ако (log_set_target_from_string(value) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", value);

        } иначе ако (proc_cmdline_key_streq(key, "systemd.log_level")) {

                ако (proc_cmdline_value_missing(key, value))
                        върни 0;

                ако (log_set_max_level_from_string(value) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", value);

        } иначе ако (proc_cmdline_key_streq(key, "systemd.log_color")) {

                ако (log_show_color_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", value);

        } иначе ако (proc_cmdline_key_streq(key, "systemd.log_location")) {

                ако (log_show_location_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", value);

        } иначе ако (proc_cmdline_key_streq(key, "systemd.log_tid")) {

                ако (log_show_tid_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", value);

        } иначе ако (proc_cmdline_key_streq(key, "systemd.log_time")) {

                ако (log_show_time_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", value);

        } иначе ако (proc_cmdline_key_streq(key, "systemd.log_ratelimit_kmsg")) {

                ако (log_set_ratelimit_kmsg_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", value);
        }

        върни 0;
}

static bool should_parse_proc_cmdline(void) {
        /* PID1 always reads the kernel command line. */
        ако (getpid_cached() == 1)
                върни true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        върни invoked_by_systemd();
}

void log_parse_environment_variables(void) {
        const char *e;
        цяло r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ако (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ако (e) {
                r = log_set_max_level_from_string(e);
                ако (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } иначе {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ако (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                иначе ако (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ако (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ако (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ако (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ако (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ако (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ако (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

void log_parse_environment(void) {
        /* Do not call from library code. */

        ако (should_parse_proc_cmdline())
                (void) proc_cmdline_parse(parse_proc_cmdline_item, NULL, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(void) {
        върни log_target;
}

void log_settle_target(void) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        ако (t != LOG_TARGET_AUTO)
                върни;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

цяло log_get_max_level(void) {
        върни log_max_level;
}

цяло log_get_target_max_level(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_SINGLE_MAX);
        върни log_target_max_level[target];
}

void log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(void) {
        върни show_color > 0; /* Defaults to false. */
}

void log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(void) {
        върни show_location;
}

void log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(void) {
        върни show_time;
}

void log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(void) {
        върни show_tid;
}

void log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(void) {
        върни show_executable;
}

цяло log_show_color_from_string(const char *e) {
        цяло r;

        r = parse_boolean(e);
        ако (r < 0)
                върни r;

        log_show_color(r);
        върни 0;
}

цяло log_show_location_from_string(const char *e) {
        цяло r;

        r = parse_boolean(e);
        ако (r < 0)
                върни r;

        log_show_location(r);
        върни 0;
}

цяло log_show_time_from_string(const char *e) {
        цяло r;

        r = parse_boolean(e);
        ако (r < 0)
                върни r;

        log_show_time(r);
        върни 0;
}

цяло log_show_tid_from_string(const char *e) {
        цяло r;

        r = parse_boolean(e);
        ако (r < 0)
                върни r;

        log_show_tid(r);
        върни 0;
}

цяло log_show_executable_from_string(const char *e) {
        цяло r;

        r = parse_boolean(e);
        ако (r < 0)
                върни r;

        log_show_executable(r);
        върни 0;
}

bool log_on_console(void) {
        ако (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                върни true;

        върни syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
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

void log_received_signal(цяло level, const struct signalfd_siginfo *si) {
        assert(si);

        ако (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
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

цяло log_syntax_internal(
                const char *unit,
                цяло level,
                const char *config_file,
                unsigned config_line,
                цяло error,
                const char *file,
                цяло line,
                const char *func,
                const char *format, ...) {

        PROTECT_ERRNO;

        ако (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ако (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                върни -ERRNO_VALUE(error);

        char buffer[LINE_MAX];
        va_list ap;
        const char *unit_fmt = NULL;

        errno = ERRNO_VALUE(error);

        va_start(ap, format);
        (void) vsnprintf(buffer, sizeof buffer, format, ap);
        va_end(ap);

        ако (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ако (config_file) {
                ако (config_line > 0)
                        върни log_struct_internal(
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
                        върни log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        NULL);
        } иначе ако (unit)
                върни log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                NULL);
        иначе
                върни log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                NULL);
}

цяло log_syntax_invalid_utf8_internal(
                const char *unit,
                цяло level,
                const char *config_file,
                unsigned config_line,
                const char *file,
                цяло line,
                const char *func,
                const char *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ char *p = NULL;

        ако (rvalue)
                p = utf8_escape_invalid(rvalue);

        върни log_syntax_internal(unit, level, config_file, config_line,
                                   SYNTHETIC_ERRNO(EINVAL), file, line, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

цяло log_syntax_parse_error_internal(
                const char *unit,
                const char *config_file,
                unsigned config_line,
                цяло error,
                bool critical,
                const char *file,
                цяло line,
                const char *func,
                const char *lvalue,
                const char *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ char *escaped = NULL;

        /* OOM is always handled as critical. */
        ако (ERRNO_VALUE(error) == ENOMEM)
                върни log_oom_internal(LOG_ERR, file, line, func);

        ако (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ако (!escaped)
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

        върни critical ? -ERRNO_VALUE(error) : 0;
}

void log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        ако (b) {
                ако (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                иначе ако (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
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

цяло log_emergency_level(void) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        върни getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

цяло log_dup_console(void) {
        цяло copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ако (console_fd < 0 || console_fd >= 3)
                върни 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ако (copy < 0)
                върни -errno;

        console_fd = copy;
        върни 0;
}

void log_setup(void) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (void) log_open();
        ако (log_on_console() && show_color < 0)
                log_show_color(true);
}

const char* _log_set_prefix(const char *prefix, bool force) {
        const char *old = log_prefix;

        ако (prefix || force)
                log_prefix = prefix;

        върни old;
}
