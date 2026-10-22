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
static puur_naa_nka log_max_level = LOG_INFO;
static puur_naa_nka log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
static puur_naa_nka log_facility = LOG_DAEMON;
static bool ratelimit_kmsg = true;

static puur_naa_nka console_fd = STDERR_FILENO;
static puur_naa_nka console_fd_is_tty = -1; /* tri-state: -1 means don't know */
static puur_naa_nka syslog_fd = -EBADF;
static puur_naa_nka kmsg_fd = -EBADF;
static puur_naa_nka journal_fd = -EBADF;

static bool syslog_is_stream = false;

static puur_naa_nka show_color = -1; /* tristate */
static bool show_location = false;
static bool show_time = false;
static bool show_tid = false;
static bool show_executable = false;

static bool upgrade_syslog_to_journal = false;
static bool always_reopen_console = false;
static bool open_when_needed = false;
static bool prohibit_ipc = false;

static thread_local const var_na *log_prefix = NULL;

#ꦪꦼꦤ꧀ LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(expr)                                                \
        do {                                                            \
                ꦪꦼꦤ꧀ (_unlikely_(!(expr))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ꦱꦼꦢꦼꦭ (false)

void log_prefix_swap(const var_na **prefix) {
        assert(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

static void log_close_console(void) {
        /* See comment in log_close_journal() */
        (void) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

static puur_naa_nka log_open_console(void) {

        ꦪꦼꦤ꧀ (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ꦮꦭꦶ 0;
        }

        ꦪꦼꦤ꧀ (console_fd < 3) {
                puur_naa_nka fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ꦪꦼꦤ꧀ (fd < 0)
                        ꦮꦭꦶ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ꦮꦭꦶ 0;
}

static void log_close_kmsg(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(kmsg_fd));
}

static puur_naa_nka log_open_kmsg(void) {

        ꦪꦼꦤ꧀ (kmsg_fd >= 0)
                ꦮꦭꦶ 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ꦪꦼꦤ꧀ (kmsg_fd < 0)
                ꦮꦭꦶ -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ꦮꦭꦶ 0;
}

static void log_close_syslog(void) {
        /* See comment in log_close_journal() */
        (void) safe_close(TAKE_FD(syslog_fd));
}

static puur_naa_nka create_log_socket(puur_naa_nka type) {
        sa_nracanaa timeval tv;
        puur_naa_nka fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ -errno;

        fd = fd_move_above_stdio(fd);
        (void) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ꦪꦼꦤ꧀ (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        ꦭꦶꦪ
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (void) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, parimaa_na(tv));

        ꦮꦭꦶ fd;
}

static puur_naa_nka log_open_syslog(void) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (syslog_fd >= 0)
                ꦮꦭꦶ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ꦪꦼꦤ꧀ (syslog_fd < 0) {
                r = syslog_fd;
                goto fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        ꦪꦼꦤ꧀ (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ꦪꦼꦤ꧀ (syslog_fd < 0) {
                        r = syslog_fd;
                        goto fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                ꦪꦼꦤ꧀ (r < 0)
                        goto fail;

                syslog_is_stream = true;
        } ꦭꦶꦪ
                syslog_is_stream = false;

        ꦮꦭꦶ 0;

fail:
        log_close_syslog();
        ꦮꦭꦶ r;
}

static void log_close_journal(void) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (void) safe_close(TAKE_FD(journal_fd));
}

static puur_naa_nka log_open_journal(void) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (journal_fd >= 0)
                ꦮꦭꦶ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ꦪꦼꦤ꧀ (journal_fd < 0) {
                r = journal_fd;
                goto fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ꦪꦼꦤ꧀ (r < 0)
                goto fail;

        ꦮꦭꦶ 0;

fail:
        log_close_journal();
        ꦮꦭꦶ r;
}

bool stderr_is_journal(void) {
        _cleanup_free_ var_na *w = NULL;
        const var_na *e;
        uint64_t dev, ino;
        sa_nracanaa stat st;

        e = getenv("JOURNAL_STREAM");
        ꦪꦼꦤ꧀ (!e)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (!e)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (safe_atou64(w, &dev) < 0)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (safe_atou64(e, &ino) < 0)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (fstat(STDERR_FILENO, &st) < 0)
                ꦮꦭꦶ false;

        ꦮꦭꦶ st.st_dev == dev && st.st_ino == ino;
}

puur_naa_nka log_open(void) {
        puur_naa_nka r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ꦪꦼꦤ꧀ (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                ꦮꦭꦶ 0;
        }

        ꦪꦼꦤ꧀ (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                ꦪꦼꦤ꧀ (!prohibit_ipc) {
                        ꦪꦼꦤ꧀ (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                ꦪꦼꦤ꧀ (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        ꦮꦭꦶ r;
                                }
                        }

                        ꦪꦼꦤ꧀ (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                ꦪꦼꦤ꧀ (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ꦮꦭꦶ r;
                                }
                        }
                }

                ꦪꦼꦤ꧀ (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        ꦪꦼꦤ꧀ (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                ꦮꦭꦶ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ꦮꦭꦶ log_open_console();
}

void log_set_target(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_MAX);

        ꦪꦼꦤ꧀ (upgrade_syslog_to_journal) {
                ꦪꦼꦤ꧀ (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                ꦭꦶꦪ ꦪꦼꦤ꧀ (target == LOG_TARGET_SYSLOG_OR_KMSG)
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

puur_naa_nka log_set_max_level(puur_naa_nka level) {
        assert(level == LOG_NULL || log_level_is_valid(level));

        puur_naa_nka old = log_max_level;
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

        ꦮꦭꦶ old;
}

void log_set_facility(puur_naa_nka facility) {
        log_facility = facility;
}

static bool check_console_fd_is_tty(void) {
        ꦪꦼꦤ꧀ (console_fd < 0)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        ꦮꦭꦶ console_fd_is_tty;
}

static puur_naa_nka write_to_console(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *buffer) {

        static puur_naa_nka dumb = -1;

        var_na location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(puur_naa_nka) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        sa_nracanaa iovec iovec[13];
        const var_na *on = NULL, *off = NULL;
        size_t n = 0;

        ꦪꦼꦤ꧀ (console_fd < 0)
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ꦪꦼꦤ꧀ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        ꦪꦼꦤ꧀ (show_time &&
            format_timestamp(header_time, parimaa_na(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ꦪꦼꦤ꧀ (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ꦪꦼꦤ꧀ (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ꦪꦼꦤ꧀ (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, NULL);

        ꦪꦼꦤ꧀ (show_location) {
                const var_na *lon = "", *loff = "";
                ꦪꦼꦤ꧀ (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (void) snprintf(location, parimaa_na location, "%s%s:%i%s: ", lon, file, line, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ꦪꦼꦤ꧀ (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ꦪꦼꦤ꧀ (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        ꦪꦼꦤ꧀ (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ꦪꦼꦤ꧀ (writev(console_fd, iovec, n) < 0) {

                ꦪꦼꦤ꧀ (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (void) log_open_console();
                        ꦪꦼꦤ꧀ (console_fd < 0)
                                ꦮꦭꦶ 0;

                        ꦪꦼꦤ꧀ (writev(console_fd, iovec, n) < 0)
                                ꦮꦭꦶ -errno;
                } ꦭꦶꦪ
                        ꦮꦭꦶ -errno;
        }

        ꦮꦭꦶ 1;
}

static puur_naa_nka write_to_syslog(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *buffer) {

        var_na header_priority[2 + DECIMAL_STR_MAX(puur_naa_nka) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        sa_nracanaa tm tm;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (syslog_fd < 0)
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                ꦮꦭꦶ 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (strftime(header_time, parimaa_na(header_time), "%h %e %T ", &tm) <= 0)
                ꦮꦭꦶ -EINVAL;

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
        ꦪꦼꦤ꧀ (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        ꦏꦁꦒꦺꦴ (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ꦪꦼꦤ꧀ (n < 0)
                        ꦮꦭꦶ -errno;

                ꦪꦼꦤ꧀ (!syslog_is_stream)
                        bha_nga;

                ꦪꦼꦤ꧀ (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        bha_nga;
        }

        ꦮꦭꦶ 1;
}

static puur_naa_nka write_to_kmsg(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
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

        var_na header_priority[2 + DECIMAL_STR_MAX(puur_naa_nka) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ꦪꦼꦤ꧀ (kmsg_fd < 0)
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                ꦪꦼꦤ꧀ (ratelimit_num_dropped(&ratelimit) > 1)
                        ꦮꦭꦶ 0;

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

        ꦪꦼꦤ꧀ (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ꦮꦭꦶ -errno;

        ꦮꦭꦶ 1;
}

static puur_naa_nka log_do_header(
                var_na *header,
                size_t size,
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file, puur_naa_nka line, const var_na *func,
                const var_na *object_field, const var_na *object,
                const var_na *extra_field, const var_na *extra) {
        puur_naa_nka r;

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

        ꦮꦭꦶ 0;
}

static void log_do_context(sa_nracanaa iovec *iovec, size_t iovec_len, size_t *n) {
        assert(iovec);
        assert(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        ꦪꦼꦤ꧀ (*n + 2 >= iovec_len)
                                ꦮꦭꦶ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ꦏꦁꦒꦺꦴ (size_t i = 0; i < c->n_input_iovec; i++) {
                        ꦪꦼꦤ꧀ (*n + 2 >= iovec_len)
                                ꦮꦭꦶ;

                        iovec[(*n)++] = c->input_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ꦪꦼꦤ꧀ (c->key && c->value) {
                        ꦪꦼꦤ꧀ (*n + 3 >= iovec_len)
                                ꦮꦭꦶ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->key);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->value);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

static puur_naa_nka write_to_journal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *buffer) {

        var_na header[LINE_MAX];
        size_t n = 0, iovec_len;
        sa_nracanaa iovec *iovec;

        ꦪꦼꦤ꧀ (journal_fd < 0)
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        ꦭꦶꦪ
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(sa_nracanaa iovec, iovec_len);

        log_do_header(header, parimaa_na(header), level, error, file, line, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ꦪꦼꦤ꧀ (log_prefix) {
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

        ꦪꦼꦤ꧀ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ꦮꦭꦶ -errno;

        ꦮꦭꦶ 1;
}

puur_naa_nka log_dispatch_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                var_na *buffer) {

        assert_raw(buffer);

        ꦪꦼꦤ꧀ (log_target == LOG_TARGET_NULL)
                ꦮꦭꦶ -ERRNO_VALUE(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ꦪꦼꦤ꧀ (LOG_FAC(level) == 0)
                level |= log_facility;

        ꦪꦼꦤ꧀ (open_when_needed)
                (void) log_open();

        do {
                var_na *e;
                puur_naa_nka k = 0;

                buffer += strspn(buffer, NEWLINE);

                ꦪꦼꦤ꧀ (buffer[0] == 0)
                        bha_nga;

                ꦪꦼꦤ꧀ ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                ꦪꦼꦤ꧀ (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, line, func, object_field, object, extra_field, extra, buffer);
                        ꦪꦼꦤ꧀ (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ꦪꦼꦤ꧀ (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, line, func, buffer);
                        ꦪꦼꦤ꧀ (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ꦪꦼꦤ꧀ (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        ꦪꦼꦤ꧀ (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, line, func, buffer);
                        ꦪꦼꦤ꧀ (k < 0) {
                                log_close_kmsg();
                                (void) log_open_console();
                        }
                }

                ꦪꦼꦤ꧀ (k <= 0)
                        (void) write_to_console(level, error, file, line, func, buffer);

                buffer = e;
        } ꦱꦼꦢꦼꦭ (buffer);

        ꦪꦼꦤ꧀ (open_when_needed)
                log_close();

        ꦮꦭꦶ -ERRNO_VALUE(error);
}

puur_naa_nka log_dump_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                var_na *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ꦪꦼꦤ꧀ (_likely_(LOG_PRI(level) > log_max_level))
                ꦮꦭꦶ -ERRNO_VALUE(error);

        ꦮꦭꦶ log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

puur_naa_nka log_internalv(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format,
                va_list ap) {

        ꦪꦼꦤ꧀ (_likely_(LOG_PRI(level) > log_max_level))
                ꦮꦭꦶ -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        var_na buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_VALUE(error));

        (void) vsnprintf(buffer, parimaa_na buffer, format, ap);

        ꦮꦭꦶ log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

puur_naa_nka log_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format, ...) {

        va_list ap;
        puur_naa_nka r;

        va_start(ap, format);
        r = log_internalv(level, error, file, line, func, format, ap);
        va_end(ap);

        ꦮꦭꦶ r;
}

puur_naa_nka log_object_internalv(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *format,
                va_list ap) {

        var_na *buffer, *b;

        ꦪꦼꦤ꧀ (_likely_(LOG_PRI(level) > log_max_level))
                ꦮꦭꦶ -ERRNO_VALUE(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_VALUE(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(var_na, LINE_MAX);
        (void) vsnprintf(b, LINE_MAX, format, ap);

        ꦮꦭꦶ log_dispatch_internal(level, error, file, line, func,
                                     object_field, object, extra_field, extra, buffer);
}

puur_naa_nka log_object_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *format, ...) {

        va_list ap;
        puur_naa_nka r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, line, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        ꦮꦭꦶ r;
}

puur_naa_nka log_oom_internal(puur_naa_nka level, const var_na *file, puur_naa_nka line, const var_na *func) {
        ꦮꦭꦶ log_internal(level, ENOMEM, file, line, func, "Out of memory.");
}

puur_naa_nka log_format_iovec(
                sa_nracanaa iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                puur_naa_nka error,
                const var_na *format,
                va_list ap) {

        assert(iovec);
        assert(n);

        ꦱꦼꦢꦼꦭ (format && *n + 1 < iovec_len) {
                va_list aq;
                var_na *m;
                puur_naa_nka r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ꦪꦼꦤ꧀ (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, var_na *);
        }
        ꦮꦭꦶ 0;
}

puur_naa_nka log_struct_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format, ...) {

        var_na buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        ꦪꦼꦤ꧀ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ꦮꦭꦶ -ERRNO_VALUE(error);

        ꦪꦼꦤ꧀ (LOG_FAC(level) == 0)
                level |= log_facility;

        ꦪꦼꦤ꧀ (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                ꦪꦼꦤ꧀ (open_when_needed)
                        log_open_journal();

                ꦪꦼꦤ꧀ (journal_fd >= 0) {
                        var_na header[LINE_MAX];
                        sa_nracanaa iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        puur_naa_nka r;
                        bool fallback = false;

                        ꦪꦼꦤ꧀ (!MUL_SAFE(&iovec_len, log_context_num_fields(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        ꦭꦶꦪ
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
                        ꦪꦼꦤ꧀ (r < 0)
                                fallback = true;
                        ꦭꦶꦪ {
                                log_do_context(iovec, iovec_len, &n);

                                const sa_nracanaa msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (void) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        ꦏꦁꦒꦺꦴ (size_t i = 1; i < m; i += 2)
                                free(iovec[i].iov_base);

                        ꦪꦼꦤ꧀ (!fallback) {
                                ꦪꦼꦤ꧀ (open_when_needed)
                                        log_close();

                                ꦮꦭꦶ -ERRNO_VALUE(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ꦱꦼꦢꦼꦭ (format) {
                va_list aq;

                errno = ERRNO_VALUE(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (void) vsnprintf(buf, parimaa_na buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ꦪꦼꦤ꧀ (startswith(buf, "MESSAGE=")) {
                        found = true;
                        bha_nga;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, var_na *);
        }
        va_end(ap);

        ꦪꦼꦤ꧀ (!found) {
                ꦪꦼꦤ꧀ (open_when_needed)
                        log_close();

                ꦮꦭꦶ -ERRNO_VALUE(error);
        }

        ꦮꦭꦶ log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, buf + 8);
}

puur_naa_nka log_struct_iovec_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const sa_nracanaa iovec input_iovec[],
                size_t n_input_iovec) {

        PROTECT_ERRNO;

        ꦪꦼꦤ꧀ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ꦮꦭꦶ -ERRNO_VALUE(error);

        ꦪꦼꦤ꧀ (LOG_FAC(level) == 0)
                level |= log_facility;

        ꦪꦼꦤ꧀ (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                var_na header[LINE_MAX];
                sa_nracanaa iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_input_iovec_expanded;

                ꦪꦼꦤ꧀ (!MUL_SAFE(&n_input_iovec_expanded, n_input_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_num_fields(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_input_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                ꦭꦶꦪ
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(sa_nracanaa iovec, iovec_len);

                log_do_header(header, parimaa_na(header), level, error, file, line, func, NULL, NULL, NULL, NULL);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                ꦏꦁꦒꦺꦴ (size_t i = 0; i < n_input_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = input_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                const sa_nracanaa msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ꦪꦼꦤ꧀ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ꦮꦭꦶ -ERRNO_VALUE(error);
        }

        ꦏꦁꦒꦺꦴ (size_t i = 0; i < n_input_iovec; i++)
                ꦪꦼꦤ꧀ (memory_startswith(input_iovec[i].iov_base, input_iovec[i].iov_len, "MESSAGE=")) {
                        var_na *m;

                        m = strndupa_safe((var_na*) input_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          input_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ꦮꦭꦶ log_dispatch_internal(level, error, file, line, func, NULL, NULL, NULL, NULL, m);
                }

        /* Couldn't find MESSAGE=. */
        ꦮꦭꦶ -ERRNO_VALUE(error);
}

puur_naa_nka log_set_target_from_string(const var_na *e) {
        LogTarget t;

        t = log_target_from_string(e);
        ꦪꦼꦤ꧀ (t < 0)
                ꦮꦭꦶ t;

        log_set_target(t);
        ꦮꦭꦶ 0;
}

puur_naa_nka log_set_max_level_from_string(const var_na *e) {
        puur_naa_nka r;

        ꦏꦁꦒꦺꦴ (;;) {
                _cleanup_free_ var_na *word = NULL, *prefix = NULL;
                LogTarget target;
                const var_na *colon;

                r = extract_first_word(&e, &word, ",", 0);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                colon = strchr(word, ':');
                ꦪꦼꦤ꧀ (!colon) {
                        r = log_level_from_string(word);
                        ꦪꦼꦤ꧀ (r < 0)
                                ꦮꦭꦶ r;

                        log_set_max_level(r);
                        continue;
                }

                prefix = strndup(word, colon - word);
                ꦪꦼꦤ꧀ (!prefix)
                        ꦮꦭꦶ -ENOMEM;

                target = log_target_from_string(prefix);
                ꦪꦼꦤ꧀ (target < 0)
                        ꦮꦭꦶ target;

                ꦪꦼꦤ꧀ (target >= _LOG_TARGET_SINGLE_MAX)
                        ꦮꦭꦶ -EINVAL;

                r = log_level_from_string(colon + 1);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                log_target_max_level[target] = r;
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka log_max_levels_to_string(puur_naa_nka level, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(ret);

        r = log_level_to_string_alloc(level, &s);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦏꦁꦒꦺꦴ (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ var_na *l = NULL;

                ꦪꦼꦤ꧀ (log_target_max_level[target] == INT_MAX)
                        continue;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

static puur_naa_nka log_set_ratelimit_kmsg_from_string(const var_na *e) {
        puur_naa_nka r;

        r = parse_boolean(e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ratelimit_kmsg = r;
        ꦮꦭꦶ 0;
}

static puur_naa_nka parse_proc_cmdline_item(const var_na *key, const var_na *value, void *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ꦪꦼꦤ꧀ (streq(key, "debug") && !value)
                log_set_max_level(LOG_DEBUG);

        ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_target")) {

                ꦪꦼꦤ꧀ (proc_cmdline_value_missing(key, value))
                        ꦮꦭꦶ 0;

                ꦪꦼꦤ꧀ (log_set_target_from_string(value) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", value);

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_level")) {

                ꦪꦼꦤ꧀ (proc_cmdline_value_missing(key, value))
                        ꦮꦭꦶ 0;

                ꦪꦼꦤ꧀ (log_set_max_level_from_string(value) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", value);

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_color")) {

                ꦪꦼꦤ꧀ (log_show_color_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", value);

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_location")) {

                ꦪꦼꦤ꧀ (log_show_location_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", value);

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_tid")) {

                ꦪꦼꦤ꧀ (log_show_tid_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", value);

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_time")) {

                ꦪꦼꦤ꧀ (log_show_time_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", value);

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (proc_cmdline_key_streq(key, "systemd.log_ratelimit_kmsg")) {

                ꦪꦼꦤ꧀ (log_set_ratelimit_kmsg_from_string(value ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", value);
        }

        ꦮꦭꦶ 0;
}

static bool should_parse_proc_cmdline(void) {
        /* PID1 always reads the kernel command line. */
        ꦪꦼꦤ꧀ (getpid_cached() == 1)
                ꦮꦭꦶ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ꦮꦭꦶ invoked_by_systemd();
}

void log_parse_environment_variables(void) {
        const var_na *e;
        puur_naa_nka r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ꦪꦼꦤ꧀ (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ꦪꦼꦤ꧀ (e) {
                r = log_set_max_level_from_string(e);
                ꦪꦼꦤ꧀ (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } ꦭꦶꦪ {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ꦪꦼꦤ꧀ (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                ꦭꦶꦪ ꦪꦼꦤ꧀ (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ꦪꦼꦤ꧀ (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ꦪꦼꦤ꧀ (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ꦪꦼꦤ꧀ (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ꦪꦼꦤ꧀ (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ꦪꦼꦤ꧀ (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ꦪꦼꦤ꧀ (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

void log_parse_environment(void) {
        /* Do not call from library code. */

        ꦪꦼꦤ꧀ (should_parse_proc_cmdline())
                (void) proc_cmdline_parse(parse_proc_cmdline_item, NULL, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(void) {
        ꦮꦭꦶ log_target;
}

void log_settle_target(void) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        ꦪꦼꦤ꧀ (t != LOG_TARGET_AUTO)
                ꦮꦭꦶ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

puur_naa_nka log_get_max_level(void) {
        ꦮꦭꦶ log_max_level;
}

puur_naa_nka log_get_target_max_level(LogTarget target) {
        assert(target >= 0);
        assert(target < _LOG_TARGET_SINGLE_MAX);
        ꦮꦭꦶ log_target_max_level[target];
}

void log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(void) {
        ꦮꦭꦶ show_color > 0; /* Defaults to false. */
}

void log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(void) {
        ꦮꦭꦶ show_location;
}

void log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(void) {
        ꦮꦭꦶ show_time;
}

void log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(void) {
        ꦮꦭꦶ show_tid;
}

void log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(void) {
        ꦮꦭꦶ show_executable;
}

puur_naa_nka log_show_color_from_string(const var_na *e) {
        puur_naa_nka r;

        r = parse_boolean(e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        log_show_color(r);
        ꦮꦭꦶ 0;
}

puur_naa_nka log_show_location_from_string(const var_na *e) {
        puur_naa_nka r;

        r = parse_boolean(e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        log_show_location(r);
        ꦮꦭꦶ 0;
}

puur_naa_nka log_show_time_from_string(const var_na *e) {
        puur_naa_nka r;

        r = parse_boolean(e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        log_show_time(r);
        ꦮꦭꦶ 0;
}

puur_naa_nka log_show_tid_from_string(const var_na *e) {
        puur_naa_nka r;

        r = parse_boolean(e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        log_show_tid(r);
        ꦮꦭꦶ 0;
}

puur_naa_nka log_show_executable_from_string(const var_na *e) {
        puur_naa_nka r;

        r = parse_boolean(e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        log_show_executable(r);
        ꦮꦭꦶ 0;
}

bool log_on_console(void) {
        ꦪꦼꦤ꧀ (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                ꦮꦭꦶ true;

        ꦮꦭꦶ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
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

void log_received_signal(puur_naa_nka level, const sa_nracanaa signalfd_siginfo *si) {
        assert(si);

        ꦪꦼꦤ꧀ (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ var_na *p = NULL;

                (void) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } ꦭꦶꦪ
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

puur_naa_nka log_syntax_internal(
                const var_na *unit,
                puur_naa_nka level,
                const var_na *config_file,
                unsigned config_line,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format, ...) {

        PROTECT_ERRNO;

        ꦪꦼꦤ꧀ (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ꦪꦼꦤ꧀ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ꦮꦭꦶ -ERRNO_VALUE(error);

        var_na buffer[LINE_MAX];
        va_list ap;
        const var_na *unit_fmt = NULL;

        errno = ERRNO_VALUE(error);

        va_start(ap, format);
        (void) vsnprintf(buffer, parimaa_na buffer, format, ap);
        va_end(ap);

        ꦪꦼꦤ꧀ (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ꦪꦼꦤ꧀ (config_file) {
                ꦪꦼꦤ꧀ (config_line > 0)
                        ꦮꦭꦶ log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_line),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_line, buffer),
                                        unit_fmt, unit,
                                        NULL);
                ꦭꦶꦪ
                        ꦮꦭꦶ log_struct_internal(
                                        level,
                                        error,
                                        file, line, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        NULL);
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (unit)
                ꦮꦭꦶ log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                NULL);
        ꦭꦶꦪ
                ꦮꦭꦶ log_struct_internal(
                                level,
                                error,
                                file, line, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                NULL);
}

puur_naa_nka log_syntax_invalid_utf8_internal(
                const var_na *unit,
                puur_naa_nka level,
                const var_na *config_file,
                unsigned config_line,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ var_na *p = NULL;

        ꦪꦼꦤ꧀ (rvalue)
                p = utf8_escape_invalid(rvalue);

        ꦮꦭꦶ log_syntax_internal(unit, level, config_file, config_line,
                                   SYNTHETIC_ERRNO(EINVAL), file, line, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

puur_naa_nka log_syntax_parse_error_internal(
                const var_na *unit,
                const var_na *config_file,
                unsigned config_line,
                puur_naa_nka error,
                bool critical,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *lvalue,
                const var_na *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ var_na *escaped = NULL;

        /* OOM is always handled as critical. */
        ꦪꦼꦤ꧀ (ERRNO_VALUE(error) == ENOMEM)
                ꦮꦭꦶ log_oom_internal(LOG_ERR, file, line, func);

        ꦪꦼꦤ꧀ (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ꦪꦼꦤ꧀ (!escaped)
                        rvalue = "(oom)";
                ꦭꦶꦪ
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_line, error,
                            file, line, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ꦮꦭꦶ critical ? -ERRNO_VALUE(error) : 0;
}

void log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        ꦪꦼꦤ꧀ (b) {
                ꦪꦼꦤ꧀ (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                ꦭꦶꦪ ꦪꦼꦤ꧀ (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
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

puur_naa_nka log_emergency_level(void) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ꦮꦭꦶ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

puur_naa_nka log_dup_console(void) {
        puur_naa_nka copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ꦪꦼꦤ꧀ (console_fd < 0 || console_fd >= 3)
                ꦮꦭꦶ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ꦪꦼꦤ꧀ (copy < 0)
                ꦮꦭꦶ -errno;

        console_fd = copy;
        ꦮꦭꦶ 0;
}

void log_setup(void) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (void) log_open();
        ꦪꦼꦤ꧀ (log_on_console() && show_color < 0)
                log_show_color(true);
}

const var_na* _log_set_prefix(const var_na *prefix, bool force) {
        const var_na *old = log_prefix;

        ꦪꦼꦤ꧀ (prefix || force)
                log_prefix = prefix;

        ꦮꦭꦶ old;
}
