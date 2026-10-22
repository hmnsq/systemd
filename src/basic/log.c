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
static จำนวนเต็ม log_max_level = LOG_INFO;
static จำนวนเต็ม log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
static จำนวนเต็ม log_facility = LOG_DAEMON;
static bool ratelimit_kmsg = true;

static จำนวนเต็ม console_fd = STDERR_FILENO;
static จำนวนเต็ม console_fd_is_tty = -1; /* tri-state: -1 means don't know */
static จำนวนเต็ม syslog_fd = -EBADF;
static จำนวนเต็ม kmsg_fd = -EBADF;
static จำนวนเต็ม journal_fd = -EBADF;

static bool syslog_is_stream = false;

static จำนวนเต็ม show_color = -1; /* tristate */
static bool show_location = false;
static bool show_time = false;
static bool show_tid = false;
static bool show_executable = false;

static bool upgrade_syslog_to_journal = false;
static bool always_reopen_console = false;
static bool open_when_needed = false;
static bool prohibit_ipc = false;

static thread_local const var_na *log_prefix = NULL;

#ถ้า LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(expr)                                                \
        do {                                                            \
                ถ้า (_unlikely_(!(expr))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ขณะ (false)

void log_prefix_swap(const var_na **prefix) {
        assert(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

static void log_close_console(void) {
        /* See comment in log_close_journal() */
        (void) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

static จำนวนเต็ม log_open_console(void) {

        ถ้า (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                คืน 0;
        }

        ถ้า (console_fd < 3) {
                จำนวนเต็ม fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ถ้า (fd < 0)
                        คืน fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        คืน 0;
}

static void log_close_kmsg(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(kmsg_fd));
}

static จำนวนเต็ม log_open_kmsg(void) {

        ถ้า (kmsg_fd >= 0)
                คืน 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ถ้า (kmsg_fd < 0)
                คืน -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        คืน 0;
}

static void log_close_syslog(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(syslog_fd));
}

static จำนวนเต็ม create_log_socket(จำนวนเต็ม type) {
        sa_nracanaa timeval tv;
        จำนวนเต็ม fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ถ้า (fd < 0)
                คืน -errno;

        fd = fd_move_above_stdio(fd);
        (void) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ถ้า (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        มิฉะนั้น
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (void) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, parimaa_na(tv));

        คืน fd;
}

static จำนวนเต็ม log_open_syslog(void) {
        จำนวนเต็ม r;

        ถ้า (syslog_fd >= 0)
                คืน 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ถ้า (syslog_fd < 0) {
                r = syslog_fd;
                goto fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        ถ้า (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ถ้า (syslog_fd < 0) {
                        r = syslog_fd;
                        goto fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                ถ้า (r < 0)
                        goto fail;

                syslog_is_stream = true;
        } มิฉะนั้น
                syslog_is_stream = false;

        คืน 0;

fail:
        log_close_syslog();
        คืน r;
}

static void log_close_journal(void) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (void) safe_close(TAKE_FD(journal_fd));
}

static จำนวนเต็ม log_open_journal(void) {
        จำนวนเต็ม r;

        ถ้า (journal_fd >= 0)
                คืน 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ถ้า (journal_fd < 0) {
                r = journal_fd;
                goto fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ถ้า (r < 0)
                goto fail;

        คืน 0;

fail:
        log_close_journal();
        คืน r;
}

bool stderr_is_journal(void) {
        _cleanup_free_ var_na *w = NULL;
        const var_na *e;
        uint64_t dev, ino;
        sa_nracanaa stat st;

        e = getenv("JOURNAL_STREAM");
        ถ้า (!e)
                คืน false;

        ถ้า (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                คืน false;
        ถ้า (!e)
                คืน false;

        ถ้า (safe_atou64(w, &dev) < 0)
                คืน false;
        ถ้า (safe_atou64(e, &ino) < 0)
                คืน false;

        ถ้า (fstat(STDERR_FILENO, &st) < 0)
                คืน false;

        คืน st.st_dev == dev && st.st_ino == ino;
}

จำนวนเต็ม log_open(void) {
        จำนวนเต็ม r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ถ้า (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                คืน 0;
        }

        ถ้า (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                ถ้า (!prohibit_ipc) {
                        ถ้า (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                ถ้า (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        คืน r;
                                }
                        }

                        ถ้า (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                ถ้า (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        คืน r;
                                }
                        }
                }

                ถ้า (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        ถ้า (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                คืน r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        คืน log_open_console();
}

void log_set_target(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_MAX);

        ถ้า (upgrade_syslog_to_journal) {
                ถ้า (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                มิฉะนั้น ถ้า (target == LOG_TARGET_SYSLOG_OR_KMSG)
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

จำนวนเต็ม log_set_max_level(จำนวนเต็ม level) {
        assert(level == LOG_NULL || log_level_is_valid(level));

        จำนวนเต็ม old = log_max_level;
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

        คืน old;
}

void log_set_facility(จำนวนเต็ม facility) {
        log_facility = facility;
}

static bool check_console_fd_is_tty(void) {
        ถ้า (console_fd < 0)
                คืน false;

        ถ้า (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        คืน console_fd_is_tty;
}

static จำนวนเต็ม write_to_console(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *buffer) {

        static จำนวนเต็ม dumb = -1;

        var_na location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(จำนวนเต็ม) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        sa_nracanaa iovec iovec[13];
        const var_na *on = NULL, *off = NULL;
        size_t n = 0;

        ถ้า (console_fd < 0)
                คืน 0;

        ถ้า (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ถ้า (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                คืน 0;

        ถ้า (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        ถ้า (show_time &&
            format_timestamp(header_time, parimaa_na(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ถ้า (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ถ้า (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ถ้า (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, NULL);

        ถ้า (show_location) {
                const var_na *lon = "", *loff = "";
                ถ้า (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (void) snprintf(location, parimaa_na location, "%s%s:%i%s: ", lon, file, line, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ถ้า (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ถ้า (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        ถ้า (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ถ้า (writev(console_fd, iovec, n) < 0) {

                ถ้า (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (void) log_open_console();
                        ถ้า (console_fd < 0)
                                คืน 0;

                        ถ้า (writev(console_fd, iovec, n) < 0)
                                คืน -errno;
                } มิฉะนั้น
                        คืน -errno;
        }

        คืน 1;
}

static จำนวนเต็ม write_to_syslog(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *buffer) {

        var_na header_priority[2 + DECIMAL_STR_MAX(จำนวนเต็ม) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        sa_nracanaa tm tm;
        จำนวนเต็ม r;

        ถ้า (syslog_fd < 0)
                คืน 0;

        ถ้า (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                คืน 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ถ้า (r < 0)
                คืน r;

        ถ้า (strftime(header_time, parimaa_na(header_time), "%h %e %T ", &tm) <= 0)
                คืน -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        sa_nracanaa iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        const sa_nracanaa msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        ถ้า (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        สำหรับ (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ถ้า (n < 0)
                        คืน -errno;

                ถ้า (!syslog_is_stream)
                        bha_nga;

                ถ้า (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        bha_nga;
        }

        คืน 1;
}

static จำนวนเต็ม write_to_kmsg(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        static thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        var_na header_priority[2 + DECIMAL_STR_MAX(จำนวนเต็ม) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ถ้า (kmsg_fd < 0)
                คืน 0;

        ถ้า (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                คืน 0;

        ถ้า (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                ถ้า (ratelimit_num_dropped(&ratelimit) > 1)
                        คืน 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        const sa_nracanaa iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        ถ้า (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                คืน -errno;

        คืน 1;
}

static จำนวนเต็ม log_do_header(
                var_na *header,
                size_t size,
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file, จำนวนเต็ม line, const var_na *func,
                const var_na *object_field, const var_na *object,
                const var_na *extra_field, const var_na *extra) {
        จำนวนเต็ม r;

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

        คืน 0;
}

static void log_do_context(sa_nracanaa iovec *iovec, size_t iovec_len, size_t *n) {
        assert(iovec);
        assert(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        ถ้า (*n + 2 >= iovec_len)
                                คืน;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                สำหรับ (size_t i = 0; i < c->n_input_iovec; i++) {
                        ถ้า (*n + 2 >= iovec_len)
                                คืน;

                        iovec[(*n)++] = c->input_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ถ้า (c->key && c->value) {
                        ถ้า (*n + 3 >= iovec_len)
                                คืน;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->key);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->value);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

static จำนวนเต็ม write_to_journal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *buffer) {

        var_na header[LINE_MAX];
        size_t n = 0, iovec_len;
        sa_nracanaa iovec *iovec;

        ถ้า (journal_fd < 0)
                คืน 0;

        ถ้า (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                คืน 0;

        ถ้า (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        มิฉะนั้น
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(sa_nracanaa iovec, iovec_len);

        log_do_header(header, parimaa_na(header), level, error, file, line, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ถ้า (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        const sa_nracanaa msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        ถ้า (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                คืน -errno;

        คืน 1;
}

จำนวนเต็ม log_dispatch_internal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                var_na *buffer) {

        assert_raw(buffer);

        ถ้า (log_target == LOG_TARGET_NULL)
                คืน -ERRNO_VALUE(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ถ้า (LOG_FAC(level) == 0)
                level |= log_facility;

        ถ้า (open_when_needed)
                (void) log_open();

        do {
                var_na *e;
                จำนวนเต็ม k = 0;

                buffer += strspn(buffer, NEWLINE);

                ถ้า (buffer[0] == 0)
                        bha_nga;

                ถ้า ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                ถ้า (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, line, func, object_field, object, extra_field, extra, buffer);
                        ถ้า (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ถ้า (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, line, func, buffer);
                        ถ้า (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ถ้า (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        ถ้า (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, line, func, buffer);
                        ถ้า (k < 0) {
                                log_close_kmsg();
                                (void) log_open_console();
                        }
                }

                ถ้า (k <= 0)
                        (void) write_to_console(level, error, file, line, func, buffer);

                buffer = e;
        } ขณะ (buffer);

        ถ้า (open_when_needed)
                log_close();

        คืน -ERRNO_VALUE(error);
}

จำนวนเต็ม log_dump_internal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                var_na *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ถ้า (_likely_(LOG_PRI(level) > log_max_level))
                คืน -ERRNO_VALUE(error);

        คืน log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

จำนวนเต็ม log_internalv(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *format,
                va_list ap) {

        ถ้า (_likely_(LOG_PRI(level) > log_max_level))
                คืน -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        var_na buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_VALUE(error));

        (void) vsnprintf(buffer, parimaa_na buffer, format, ap);

        คืน log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

จำนวนเต็ม log_internal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *format, ...) {

        va_list ap;
        จำนวนเต็ม r;

        va_start(ap, format);
        r = log_internalv(level, error, file, line, func, format, ap);
        va_end(ap);

        คืน r;
}

จำนวนเต็ม log_object_internalv(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *format,
                va_list ap) {

        var_na *buffer, *b;

        ถ้า (_likely_(LOG_PRI(level) > log_max_level))
                คืน -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_VALUE(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(var_na, LINE_MAX);
        (void) vsnprintf(b, LINE_MAX, format, ap);

        คืน log_dispatch_internal(level, error, file, line, func,
                                     object_field, object, extra_field, extra, buffer);
}

จำนวนเต็ม log_object_internal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *format, ...) {

        va_list ap;
        จำนวนเต็ม r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, line, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        คืน r;
}

จำนวนเต็ม log_oom_internal(จำนวนเต็ม level, const var_na *file, จำนวนเต็ม line, const var_na *func) {
        คืน log_internal(level, ENOMEM, file, line, func, "Out of memory.");
}

จำนวนเต็ม log_format_iovec(
                sa_nracanaa iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                จำนวนเต็ม error,
                const var_na *format,
                va_list ap) {

        assert(iovec);
        assert(n);

        ขณะ (format && *n + 1 < iovec_len) {
                va_list aq;
                var_na *m;
                จำนวนเต็ม r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ถ้า (r < 0)
                        คืน -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ถ้า (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, var_na *);
        }
        คืน 0;
}

จำนวนเต็ม log_struct_internal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *format, ...) {

        var_na buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        ถ้า (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                คืน -ERRNO_VALUE(error);

        ถ้า (LOG_FAC(level) == 0)
                level |= log_facility;

        ถ้า (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                ถ้า (open_when_needed)
                        log_open_journal();

                ถ้า (journal_fd >= 0) {
                        var_na header[LINE_MAX];
                        sa_nracanaa iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        จำนวนเต็ม r;
                        bool fallback = false;

                        ถ้า (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        มิฉะนั้น
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(sa_nracanaa iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, parimaa_na(header), level, error, file, line, func, NULL, NULL, NULL, NULL);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        ถ้า (r < 0)
                                fallback = true;
                        มิฉะนั้น {
                                log_do_context(iovec, iovec_len, &n);

                                const sa_nracanaa msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (void) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        สำหรับ (size_t i = 1; i < m; i += 2)
                                free(iovec[i].iov_base);

                        ถ้า (!fallback) {
                                ถ้า (open_when_needed)
                                        log_close();

                                คืน -ERRNO_VALUE(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ขณะ (format) {
                va_list aq;

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (void) vsnprintf(buf, parimaa_na buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ถ้า (startswith(buf, "MESSAGE=")) {
                        found = true;
                        bha_nga;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, var_na *);
        }
        va_end(ap);

        ถ้า (!found) {
                ถ้า (open_when_needed)
                        log_close();

                คืน -ERRNO_VALUE(error);
        }

        คืน log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buf + 8);
}

จำนวนเต็ม log_struct_iovec_internal(
                จำนวนเต็ม level,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const sa_nracanaa iovec input_iovec[],
                size_t n_input_iovec) {

        PROTECT_ERRNO;

        ถ้า (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                คืน -ERRNO_VALUE(error);

        ถ้า (LOG_FAC(level) == 0)
                level |= log_facility;

        ถ้า (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                var_na header[LINE_MAX];
                sa_nracanaa iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_input_iovec_expanded;

                ถ้า (!MUL_SAFE(&n_input_iovec_expanded, n_input_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_num_fields(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_input_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                มิฉะนั้น
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(sa_nracanaa iovec, iovec_len);

                log_do_header(header, parimaa_na(header), level, error, file, line, func, NULL, NULL, NULL, NULL);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                สำหรับ (size_t i = 0; i < n_input_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = input_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                const sa_nracanaa msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ถ้า (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        คืน -ERRNO_VALUE(error);
        }

        สำหรับ (size_t i = 0; i < n_input_iovec; i++)
                ถ้า (memory_startswith(input_iovec[i].iov_base, input_iovec[i].iov_len, "MESSAGE=")) {
                        var_na *m;

                        m = strndupa_safe((var_na*) input_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          input_iovec[i].iov_len - STRLEN("MESSAGE="));

                        คืน log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, m);
                }

        /* Couldn't find MESSAGE=. */
        คืน -ERRNO_VALUE(error);
}

จำนวนเต็ม log_set_target_from_string(const var_na *e) {
        LogTarget t;

        t = log_target_from_string(e);
        ถ้า (t < 0)
                คืน t;

        log_set_target(t);
        คืน 0;
}

จำนวนเต็ม log_set_max_level_from_string(const var_na *e) {
        จำนวนเต็ม r;

        สำหรับ (;;) {
                _cleanup_free_ var_na *word = NULL, *prefix = NULL;
                LogTarget target;
                const var_na *colon;

                r = extract_first_word(&e, &word, ",", 0);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        bha_nga;

                colon = strchr(word, ':');
                ถ้า (!colon) {
                        r = log_level_from_string(word);
                        ถ้า (r < 0)
                                คืน r;

                        log_set_max_level(r);
                        continue;
                }

                prefix = strndup(word, colon - word);
                ถ้า (!prefix)
                        คืน -ENOMEM;

                target = log_target_from_string(prefix);
                ถ้า (target < 0)
                        คืน target;

                ถ้า (target >= _LOG_TARGET_SINGLE_MAX)
                        คืน -EINVAL;

                r = log_level_from_string(colon + 1);
                ถ้า (r < 0)
                        คืน r;

                log_target_max_level[target] = r;
        }

        คืน 0;
}

จำนวนเต็ม log_max_levels_to_string(จำนวนเต็ม level, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r;

        assert(ret);

        r = log_level_to_string_alloc(level, &s);
        ถ้า (r < 0)
                คืน r;

        สำหรับ (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ var_na *l = NULL;

                ถ้า (log_target_max_level[target] == INT_MAX)
                        continue;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                ถ้า (r < 0)
                        คืน r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                ถ้า (r < 0)
                        คืน r;
        }

        *ret = TAKE_PTR(s);
        คืน 0;
}

static จำนวนเต็ม log_set_ratelimit_kmsg_from_string(const var_na *e) {
        จำนวนเต็ม r;

        r = parse_boolean(e);
        ถ้า (r < 0)
                คืน r;

        ratelimit_kmsg = r;
        คืน 0;
}

static จำนวนเต็ม parse_proc_cmdline_item(const var_na *key, const var_na *value, void *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ถ้า (streq(key, "debug") && !value)
                log_set_max_level(LOG_DEBUG);

        มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_target")) {

                ถ้า (proc_cmdline_value_missing(key, value))
                        คืน 0;

                ถ้า (log_set_target_from_string(value) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", value);

        } มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_level")) {

                ถ้า (proc_cmdline_value_missing(key, value))
                        คืน 0;

                ถ้า (log_set_max_level_from_string(value) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", value);

        } มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_color")) {

                ถ้า (log_show_color_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", value);

        } มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_location")) {

                ถ้า (log_show_location_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", value);

        } มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_tid")) {

                ถ้า (log_show_tid_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", value);

        } มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_time")) {

                ถ้า (log_show_time_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", value);

        } มิฉะนั้น ถ้า (proc_cmdline_key_streq(key, "systemd.log_ratelimit_kmsg")) {

                ถ้า (log_set_ratelimit_kmsg_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", value);
        }

        คืน 0;
}

static bool should_parse_proc_cmdline(void) {
        /* PID1 always reads the kernel command line. */
        ถ้า (getpid_cached() == 1)
                คืน true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        คืน invoked_by_systemd();
}

void log_parse_environment_variables(void) {
        const var_na *e;
        จำนวนเต็ม r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ถ้า (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ถ้า (e) {
                r = log_set_max_level_from_string(e);
                ถ้า (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } มิฉะนั้น {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ถ้า (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                มิฉะนั้น ถ้า (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ถ้า (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ถ้า (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ถ้า (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ถ้า (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ถ้า (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ถ้า (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

void log_parse_environment(void) {
        /* Do not call from library code. */

        ถ้า (should_parse_proc_cmdline())
                (void) proc_cmdline_parse(parse_proc_cmdline_item, NULL, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(void) {
        คืน log_target;
}

void log_settle_target(void) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        ถ้า (t != LOG_TARGET_AUTO)
                คืน;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

จำนวนเต็ม log_get_max_level(void) {
        คืน log_max_level;
}

จำนวนเต็ม log_get_target_max_level(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_SINGLE_MAX);
        คืน log_target_max_level[target];
}

void log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(void) {
        คืน show_color > 0; /* Defaults to false. */
}

void log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(void) {
        คืน show_location;
}

void log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(void) {
        คืน show_time;
}

void log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(void) {
        คืน show_tid;
}

void log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(void) {
        คืน show_executable;
}

จำนวนเต็ม log_show_color_from_string(const var_na *e) {
        จำนวนเต็ม r;

        r = parse_boolean(e);
        ถ้า (r < 0)
                คืน r;

        log_show_color(r);
        คืน 0;
}

จำนวนเต็ม log_show_location_from_string(const var_na *e) {
        จำนวนเต็ม r;

        r = parse_boolean(e);
        ถ้า (r < 0)
                คืน r;

        log_show_location(r);
        คืน 0;
}

จำนวนเต็ม log_show_time_from_string(const var_na *e) {
        จำนวนเต็ม r;

        r = parse_boolean(e);
        ถ้า (r < 0)
                คืน r;

        log_show_time(r);
        คืน 0;
}

จำนวนเต็ม log_show_tid_from_string(const var_na *e) {
        จำนวนเต็ม r;

        r = parse_boolean(e);
        ถ้า (r < 0)
                คืน r;

        log_show_tid(r);
        คืน 0;
}

จำนวนเต็ม log_show_executable_from_string(const var_na *e) {
        จำนวนเต็ม r;

        r = parse_boolean(e);
        ถ้า (r < 0)
                คืน r;

        log_show_executable(r);
        คืน 0;
}

bool log_on_console(void) {
        ถ้า (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                คืน true;

        คืน syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

static const var_na *const log_target_table[_LOG_TARGET_MAX] = {
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

void log_received_signal(จำนวนเต็ม level, const sa_nracanaa signalfd_siginfo *si) {
        assert(si);

        ถ้า (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ var_na *p = NULL;

                (void) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } มิฉะนั้น
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

จำนวนเต็ม log_syntax_internal(
                const var_na *unit,
                จำนวนเต็ม level,
                const var_na *config_file,
                unsigned config_line,
                จำนวนเต็ม error,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *format, ...) {

        PROTECT_ERRNO;

        ถ้า (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ถ้า (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                คืน -ERRNO_VALUE(error);

        var_na buffer[LINE_MAX];
        va_list ap;
        const var_na *unit_fmt = NULL;

        errno = ERRNO_VALUE(error);

        va_start(ap, format);
        (void) vsnprintf(buffer, parimaa_na buffer, format, ap);
        va_end(ap);

        ถ้า (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ถ้า (config_file) {
                ถ้า (config_line > 0)
                        คืน log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_line),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_line, buffer),
                                        unit_fmt, unit,
                                        NULL);
                มิฉะนั้น
                        คืน log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        NULL);
        } มิฉะนั้น ถ้า (unit)
                คืน log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                NULL);
        มิฉะนั้น
                คืน log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                NULL);
}

จำนวนเต็ม log_syntax_invalid_utf8_internal(
                const var_na *unit,
                จำนวนเต็ม level,
                const var_na *config_file,
                unsigned config_line,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ var_na *p = NULL;

        ถ้า (rvalue)
                p = utf8_escape_invalid(rvalue);

        คืน log_syntax_internal(unit, level, config_file, config_line,
                                   SYNTHETIC_ERRNO(EINVAL), file, line, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

จำนวนเต็ม log_syntax_parse_error_internal(
                const var_na *unit,
                const var_na *config_file,
                unsigned config_line,
                จำนวนเต็ม error,
                bool critical,
                const var_na *file,
                จำนวนเต็ม line,
                const var_na *func,
                const var_na *lvalue,
                const var_na *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ var_na *escaped = NULL;

        /* OOM is always handled as critical. */
        ถ้า (ERRNO_VALUE(error) == ENOMEM)
                คืน log_oom_internal(LOG_ERR, file, line, func);

        ถ้า (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ถ้า (!escaped)
                        rvalue = "(oom)";
                มิฉะนั้น
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_line, error,
                            file, line, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        คืน critical ? -ERRNO_VALUE(error) : 0;
}

void log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        ถ้า (b) {
                ถ้า (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                มิฉะนั้น ถ้า (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
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

จำนวนเต็ม log_emergency_level(void) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        คืน getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

จำนวนเต็ม log_dup_console(void) {
        จำนวนเต็ม copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ถ้า (console_fd < 0 || console_fd >= 3)
                คืน 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ถ้า (copy < 0)
                คืน -errno;

        console_fd = copy;
        คืน 0;
}

void log_setup(void) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (void) log_open();
        ถ้า (log_on_console() && show_color < 0)
                log_show_color(true);
}

const var_na* _log_set_prefix(const var_na *prefix, bool force) {
        const var_na *old = log_prefix;

        ถ้า (prefix || force)
                log_prefix = prefix;

        คืน old;
}
