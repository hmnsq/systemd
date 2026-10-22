/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/signalfd.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <sys/uio.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "sd-messages.h"

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-color.h"
#ಅನ್ತರ್ಭಾವ "argv-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "list.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "log-context.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "proc-cmdline.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "ratelimit.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "syslog-util.h"
#ಅನ್ತರ್ಭಾವ "terminal-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

ಸ್ಥಿರ log_syntax_callback_t log_syntax_callback = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ *log_syntax_callback_userdata = ರಿಕ್ತ;

ಸ್ಥಿರ LogTarget log_target = LOG_TARGET_CONSOLE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_max_level = LOG_INFO;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_facility = LOG_DAEMON;
ಸ್ಥಿರ bool ratelimit_kmsg = true;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ console_fd = STDERR_FILENO;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ syslog_fd = -EBADF;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ kmsg_fd = -EBADF;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ journal_fd = -EBADF;

ಸ್ಥಿರ bool syslog_is_stream = false;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ show_color = -1; /* tristate */
ಸ್ಥಿರ bool show_location = false;
ಸ್ಥಿರ bool show_time = false;
ಸ್ಥಿರ bool show_tid = false;
ಸ್ಥಿರ bool show_executable = false;

ಸ್ಥಿರ bool upgrade_syslog_to_journal = false;
ಸ್ಥಿರ bool always_reopen_console = false;
ಸ್ಥಿರ bool open_when_needed = false;
ಸ್ಥಿರ bool prohibit_ipc = false;

ಸ್ಥಿರ thread_local ಧ್ರುವ ವರ್ಣ *log_prefix = ರಿಕ್ತ;

#ಯದಿ LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(ಅಭಿವ್ಯಕ್ತಿ)                                                \
        ಕುರು {                                                            \
                ಯದಿ (_unlikely_(!(ಅಭಿವ್ಯಕ್ತಿ))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ಯಾವತ್ (false)

ಶೂನ್ಯ log_prefix_swap(ಧ್ರುವ ವರ್ಣ **prefix) {
        ಪ್ರತಿಜ್ಞಾ(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_console(ಶೂನ್ಯ) {
        /* See comment in log_close_journal() */
        (ಶೂನ್ಯ) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_open_console(ಶೂನ್ಯ) {

        ಯದಿ (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (console_fd < 3) {
                ಪೂರ್ಣಾಙ್ಕ fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_kmsg(ಶೂನ್ಯ) {
        /* See comment in log_close_journal() */
        (ಶೂನ್ಯ) safe_close(TAKE_FD(kmsg_fd));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_open_kmsg(ಶೂನ್ಯ) {

        ಯದಿ (kmsg_fd >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ಯದಿ (kmsg_fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_syslog(ಶೂನ್ಯ) {
        /* See comment in log_close_journal() */
        (ಶೂನ್ಯ) safe_close(TAKE_FD(syslog_fd));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ create_log_socket(ಪೂರ್ಣಾಙ್ಕ type) {
        ಸಂರಚನಾ timeval tv;
        ಪೂರ್ಣಾಙ್ಕ fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        fd = fd_move_above_stdio(fd);
        (ಶೂನ್ಯ) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ಯದಿ (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        ಅನ್ಯಥಾ
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (ಶೂನ್ಯ) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, ಪರಿಮಾಣ(tv));

        ಪ್ರತ್ಯಾಗಮ fd;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_open_syslog(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (syslog_fd >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ಯದಿ (syslog_fd < 0) {
                r = syslog_fd;
                ಗಚ್ಛ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        ಯದಿ (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ಯದಿ (syslog_fd < 0) {
                        r = syslog_fd;
                        ಗಚ್ಛ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                ಯದಿ (r < 0)
                        ಗಚ್ಛ fail;

                syslog_is_stream = true;
        } ಅನ್ಯಥಾ
                syslog_is_stream = false;

        ಪ್ರತ್ಯಾಗಮ 0;

fail:
        log_close_syslog();
        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_journal(ಶೂನ್ಯ) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (ಶೂನ್ಯ) safe_close(TAKE_FD(journal_fd));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_open_journal(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (journal_fd >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ಯದಿ (journal_fd < 0) {
                r = journal_fd;
                ಗಚ್ಛ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ಯದಿ (r < 0)
                ಗಚ್ಛ fail;

        ಪ್ರತ್ಯಾಗಮ 0;

fail:
        log_close_journal();
        ಪ್ರತ್ಯಾಗಮ r;
}

bool stderr_is_journal(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *w = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *e;
        uint64_t dev, ino;
        ಸಂರಚನಾ stat st;

        e = getenv("JOURNAL_STREAM");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (safe_atou64(w, &dev) < 0)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (safe_atou64(e, &ino) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (fstat(STDERR_FILENO, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ st.st_dev == dev && st.st_ino == ino;
}

ಪೂರ್ಣಾಙ್ಕ log_open(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ಯದಿ (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                ಯದಿ (!prohibit_ipc) {
                        ಯದಿ (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                ಯದಿ (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        ಪ್ರತ್ಯಾಗಮ r;
                                }
                        }

                        ಯದಿ (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                ಯದಿ (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ಪ್ರತ್ಯಾಗಮ r;
                                }
                        }
                }

                ಯದಿ (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        ಯದಿ (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                ಪ್ರತ್ಯಾಗಮ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ಪ್ರತ್ಯಾಗಮ log_open_console();
}

ಶೂನ್ಯ log_set_target(LogTarget target) {
        ಪ್ರತಿಜ್ಞಾ(target >= 0);
        ಪ್ರತಿಜ್ಞಾ(target < _LOG_TARGET_MAX);

        ಯದಿ (upgrade_syslog_to_journal) {
                ಯದಿ (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                ಅನ್ಯಥಾ ಯದಿ (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

ಶೂನ್ಯ log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

ಶೂನ್ಯ log_close(ಶೂನ್ಯ) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

ಶೂನ್ಯ log_forget_fds(ಶೂನ್ಯ) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

ಪೂರ್ಣಾಙ್ಕ log_set_max_level(ಪೂರ್ಣಾಙ್ಕ level) {
        ಪ್ರತಿಜ್ಞಾ(level == LOG_NULL || log_level_is_valid(level));

        ಪೂರ್ಣಾಙ್ಕ old = log_max_level;
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

        ಪ್ರತ್ಯಾಗಮ old;
}

ಶೂನ್ಯ log_set_facility(ಪೂರ್ಣಾಙ್ಕ facility) {
        log_facility = facility;
}

ಸ್ಥಿರ bool check_console_fd_is_tty(ಶೂನ್ಯ) {
        ಯದಿ (console_fd < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        ಪ್ರತ್ಯಾಗಮ console_fd_is_tty;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_to_console(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *buffer) {

        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dumb = -1;

        ವರ್ಣ location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ಸಂರಚನಾ iovec iovec[13];
        ಧ್ರುವ ವರ್ಣ *on = ರಿಕ್ತ, *off = ರಿಕ್ತ;
        size_t n = 0;

        ಯದಿ (console_fd < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ಯದಿ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        ಯದಿ (show_time &&
            format_timestamp(header_time, ಪರಿಮಾಣ(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ಯದಿ (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ಯದಿ (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ಯದಿ (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ರಿಕ್ತ);

        ಯದಿ (show_location) {
                ಧ್ರುವ ವರ್ಣ *lon = "", *loff = "";
                ಯದಿ (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (ಶೂನ್ಯ) snprintf(location, ಪರಿಮಾಣ location, "%s%s:%i%s: ", lon, file, ಸಾಲು, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ಯದಿ (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ಯದಿ (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        ಯದಿ (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ಯದಿ (writev(console_fd, iovec, n) < 0) {

                ಯದಿ (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (ಶೂನ್ಯ) log_open_console();
                        ಯದಿ (console_fd < 0)
                                ಪ್ರತ್ಯಾಗಮ 0;

                        ಯದಿ (writev(console_fd, iovec, n) < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                } ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_to_syslog(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *buffer) {

        ವರ್ಣ header_priority[2 + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ಸಂರಚನಾ tm tm;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (syslog_fd < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                ಪ್ರತ್ಯಾಗಮ 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (strftime(header_time, ಪರಿಮಾಣ(header_time), "%h %e %T ", &tm) <= 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ಸಂರಚನಾ iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        ಧ್ರುವ ಸಂರಚನಾ msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        ಯದಿ (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        ಪ್ರತಿ (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (!syslog_is_stream)
                        ಭಙ್ಗ;

                ಯದಿ (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        ಭಙ್ಗ;
        }

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_to_kmsg(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ಸ್ಥಿರ thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        ವರ್ಣ header_priority[2 + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ಯದಿ (kmsg_fd < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                ಯದಿ (ratelimit_ಅಂಕೆ_dropped(&ratelimit) > 1)
                        ಪ್ರತ್ಯಾಗಮ 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ಧ್ರುವ ಸಂರಚನಾ iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        ಯದಿ (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_do_header(
                ವರ್ಣ *header,
                size_t size,
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field, ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field, ಧ್ರುವ ವರ್ಣ *extra) {
        ಪೂರ್ಣಾಙ್ಕ r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_ಮೌಲ್ಯ(error);

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
                     ಸಾಲು ? "CODE_LINE=" : "",
                     ಸಾಲು ? 1 : 0, ಸಾಲು, /* %.0d means no output too, special case for 0 */
                     ಸಾಲು ? "\n" : "",
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

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_do_context(ಸಂರಚನಾ iovec *iovec, size_t iovec_len, size_t *n) {
        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        ಯದಿ (*n + 2 >= iovec_len)
                                ಪ್ರತ್ಯಾಗಮ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ಪ್ರತಿ (size_t i = 0; i < c->n_ಒಳಹರಿವು_iovec; i++) {
                        ಯದಿ (*n + 2 >= iovec_len)
                                ಪ್ರತ್ಯಾಗಮ;

                        iovec[(*n)++] = c->ಒಳಹರಿವು_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ಯದಿ (c->ಕೀಲಿ && c->ಮೌಲ್ಯ) {
                        ಯದಿ (*n + 3 >= iovec_len)
                                ಪ್ರತ್ಯಾಗಮ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ಕೀಲಿ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ಮೌಲ್ಯ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_to_journal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ಧ್ರುವ ವರ್ಣ *buffer) {

        ವರ್ಣ header[LINE_MAX];
        size_t n = 0, iovec_len;
        ಸಂರಚನಾ iovec *iovec;

        ಯದಿ (journal_fd < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!MUL_SAFE(&iovec_len, log_context_ಅಂಕೆ_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        ಅನ್ಯಥಾ
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ಸಂರಚನಾ iovec, iovec_len);

        log_do_header(header, ಪರಿಮಾಣ(header), level, error, file, ಸಾಲು, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ಯದಿ (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ಧ್ರುವ ಸಂರಚನಾ msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        ಯದಿ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ log_dispatch_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ವರ್ಣ *buffer) {

        assert_raw(buffer);

        ಯದಿ (log_target == LOG_TARGET_NULL)
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ಯದಿ (LOG_FAC(level) == 0)
                level |= log_facility;

        ಯದಿ (open_when_needed)
                (ಶೂನ್ಯ) log_open();

        ಕುರು {
                ವರ್ಣ *e;
                ಪೂರ್ಣಾಙ್ಕ k = 0;

                buffer += strspn(buffer, NEWLINE);

                ಯದಿ (buffer[0] == 0)
                        ಭಙ್ಗ;

                ಯದಿ ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                ಯದಿ (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, ಸಾಲು, func, object_field, object, extra_field, extra, buffer);
                        ಯದಿ (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ಯದಿ (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, ಸಾಲು, func, buffer);
                        ಯದಿ (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ಯದಿ (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        ಯದಿ (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, ಸಾಲು, func, buffer);
                        ಯದಿ (k < 0) {
                                log_close_kmsg();
                                (ಶೂನ್ಯ) log_open_console();
                        }
                }

                ಯದಿ (k <= 0)
                        (ಶೂನ್ಯ) write_to_console(level, error, file, ಸಾಲು, func, buffer);

                buffer = e;
        } ಯಾವತ್ (buffer);

        ಯದಿ (open_when_needed)
                log_close();

        ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);
}

ಪೂರ್ಣಾಙ್ಕ log_dump_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ವರ್ಣ *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ಯದಿ (_likely_(LOG_PRI(level) > log_max_level))
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        ಪ್ರತ್ಯಾಗಮ log_dispatch_internal(level, error, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, buffer);
}

ಪೂರ್ಣಾಙ್ಕ log_internalv(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format,
                va_list ap) {

        ಯದಿ (_likely_(LOG_PRI(level) > log_max_level))
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        ವರ್ಣ buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_ಮೌಲ್ಯ(error));

        (ಶೂನ್ಯ) vsnprintf(buffer, ಪರಿಮಾಣ buffer, format, ap);

        ಪ್ರತ್ಯಾಗಮ log_dispatch_internal(level, error, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, buffer);
}

ಪೂರ್ಣಾಙ್ಕ log_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format, ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = log_internalv(level, error, file, ಸಾಲು, func, format, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ log_object_internalv(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ಧ್ರುವ ವರ್ಣ *format,
                va_list ap) {

        ವರ್ಣ *buffer, *b;

        ಯದಿ (_likely_(LOG_PRI(level) > log_max_level))
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_ಮೌಲ್ಯ(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(ವರ್ಣ, LINE_MAX);
        (ಶೂನ್ಯ) vsnprintf(b, LINE_MAX, format, ap);

        ಪ್ರತ್ಯಾಗಮ log_dispatch_internal(level, error, file, ಸಾಲು, func,
                                     object_field, object, extra_field, extra, buffer);
}

ಪೂರ್ಣಾಙ್ಕ log_object_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ಧ್ರುವ ವರ್ಣ *format, ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, ಸಾಲು, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ log_oom_internal(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *func) {
        ಪ್ರತ್ಯಾಗಮ log_internal(level, ENOMEM, file, ಸಾಲು, func, "Out of memory.");
}

ಪೂರ್ಣಾಙ್ಕ log_format_iovec(
                ಸಂರಚನಾ iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *format,
                va_list ap) {

        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n);

        ಯಾವತ್ (format && *n + 1 < iovec_len) {
                va_list aq;
                ವರ್ಣ *m;
                ಪೂರ್ಣಾಙ್ಕ r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_ಮೌಲ್ಯ(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ಯದಿ (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, ವರ್ಣ *);
        }
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_struct_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format, ...) {

        ವರ್ಣ buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        ಯದಿ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        ಯದಿ (LOG_FAC(level) == 0)
                level |= log_facility;

        ಯದಿ (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                ಯದಿ (open_when_needed)
                        log_open_journal();

                ಯದಿ (journal_fd >= 0) {
                        ವರ್ಣ header[LINE_MAX];
                        ಸಂರಚನಾ iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        ಪೂರ್ಣಾಙ್ಕ r;
                        bool fallback = false;

                        ಯದಿ (!MUL_SAFE(&iovec_len, log_context_ಅಂಕೆ_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        ಅನ್ಯಥಾ
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ಸಂರಚನಾ iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, ಪರಿಮಾಣ(header), level, error, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        ಯದಿ (r < 0)
                                fallback = true;
                        ಅನ್ಯಥಾ {
                                log_do_context(iovec, iovec_len, &n);

                                ಧ್ರುವ ಸಂರಚನಾ msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (ಶೂನ್ಯ) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        ಪ್ರತಿ (size_t i = 1; i < m; i += 2)
                                ಮುಕ್ತ(iovec[i].iov_base);

                        ಯದಿ (!fallback) {
                                ಯದಿ (open_when_needed)
                                        log_close();

                                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ಯಾವತ್ (format) {
                va_list aq;

                errno = ERRNO_ಮೌಲ್ಯ(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (ಶೂನ್ಯ) vsnprintf(buf, ಪರಿಮಾಣ buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ಯದಿ (startswith(buf, "MESSAGE=")) {
                        found = true;
                        ಭಙ್ಗ;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, ವರ್ಣ *);
        }
        va_end(ap);

        ಯದಿ (!found) {
                ಯದಿ (open_when_needed)
                        log_close();

                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);
        }

        ಪ್ರತ್ಯಾಗಮ log_dispatch_internal(level, error, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, buf + 8);
}

ಪೂರ್ಣಾಙ್ಕ log_struct_iovec_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ಸಂರಚನಾ iovec ಒಳಹರಿವು_iovec[],
                size_t n_ಒಳಹರಿವು_iovec) {

        PROTECT_ERRNO;

        ಯದಿ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        ಯದಿ (LOG_FAC(level) == 0)
                level |= log_facility;

        ಯದಿ (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                ವರ್ಣ header[LINE_MAX];
                ಸಂರಚನಾ iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_ಒಳಹರಿವು_iovec_expanded;

                ಯದಿ (!MUL_SAFE(&n_ಒಳಹರಿವು_iovec_expanded, n_ಒಳಹರಿವು_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_ಅಂಕೆ_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_ಒಳಹರಿವು_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                ಅನ್ಯಥಾ
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ಸಂರಚನಾ iovec, iovec_len);

                log_do_header(header, ಪರಿಮಾಣ(header), level, error, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                ಪ್ರತಿ (size_t i = 0; i < n_ಒಳಹರಿವು_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = ಒಳಹರಿವು_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ಧ್ರುವ ಸಂರಚನಾ msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ಯದಿ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);
        }

        ಪ್ರತಿ (size_t i = 0; i < n_ಒಳಹರಿವು_iovec; i++)
                ಯದಿ (memory_startswith(ಒಳಹರಿವು_iovec[i].iov_base, ಒಳಹರಿವು_iovec[i].iov_len, "MESSAGE=")) {
                        ವರ್ಣ *m;

                        m = strndupa_safe((ವರ್ಣ*) ಒಳಹರಿವು_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          ಒಳಹರಿವು_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ಪ್ರತ್ಯಾಗಮ log_dispatch_internal(level, error, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, m);
                }

        /* Couldn't find MESSAGE=. */
        ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);
}

ಪೂರ್ಣಾಙ್ಕ log_set_target_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        LogTarget t;

        t = log_target_from_string(e);
        ಯದಿ (t < 0)
                ಪ್ರತ್ಯಾಗಮ t;

        log_set_target(t);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_set_max_level_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *word = ರಿಕ್ತ, *prefix = ರಿಕ್ತ;
                LogTarget target;
                ಧ್ರುವ ವರ್ಣ *colon;

                r = extract_first_word(&e, &word, ",", 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                colon = strchr(word, ':');
                ಯದಿ (!colon) {
                        r = log_level_from_string(word);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        log_set_max_level(r);
                        ಅನುವರ್ತ;
                }

                prefix = strndup(word, colon - word);
                ಯದಿ (!prefix)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                target = log_target_from_string(prefix);
                ಯದಿ (target < 0)
                        ಪ್ರತ್ಯಾಗಮ target;

                ಯದಿ (target >= _LOG_TARGET_SINGLE_MAX)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                r = log_level_from_string(colon + 1);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                log_target_max_level[target] = r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_max_levels_to_string(ಪೂರ್ಣಾಙ್ಕ level, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = log_level_to_string_alloc(level, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ ವರ್ಣ *l = ರಿಕ್ತ;

                ಯದಿ (log_target_max_level[target] == INT_MAX)
                        ಅನುವರ್ತ;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_set_ratelimit_kmsg_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = parse_boolean(e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ratelimit_kmsg = r;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_proc_cmdline_item(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, ಶೂನ್ಯ *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ಯದಿ (streq(ಕೀಲಿ, "debug") && !ಮೌಲ್ಯ)
                log_set_max_level(LOG_DEBUG);

        ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_target")) {

                ಯದಿ (proc_cmdline_ಮೌಲ್ಯ_missing(ಕೀಲಿ, ಮೌಲ್ಯ))
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (log_set_target_from_string(ಮೌಲ್ಯ) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", ಮೌಲ್ಯ);

        } ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_level")) {

                ಯದಿ (proc_cmdline_ಮೌಲ್ಯ_missing(ಕೀಲಿ, ಮೌಲ್ಯ))
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (log_set_max_level_from_string(ಮೌಲ್ಯ) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_color")) {

                ಯದಿ (log_show_color_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_location")) {

                ಯದಿ (log_show_location_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_tid")) {

                ಯದಿ (log_show_tid_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_time")) {

                ಯದಿ (log_show_time_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಅನ್ಯಥಾ ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_ratelimit_kmsg")) {

                ಯದಿ (log_set_ratelimit_kmsg_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", ಮೌಲ್ಯ);
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ bool should_parse_proc_cmdline(ಶೂನ್ಯ) {
        /* PID1 always reads the kernel command line. */
        ಯದಿ (getpid_cached() == 1)
                ಪ್ರತ್ಯಾಗಮ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ಪ್ರತ್ಯಾಗಮ invoked_by_systemd();
}

ಶೂನ್ಯ log_parse_environment_variables(ಶೂನ್ಯ) {
        ಧ್ರುವ ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ಯದಿ (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ಯದಿ (e) {
                r = log_set_max_level_from_string(e);
                ಯದಿ (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } ಅನ್ಯಥಾ {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ಯದಿ (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                ಅನ್ಯಥಾ ಯದಿ (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ಯದಿ (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ಯದಿ (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ಯದಿ (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ಯದಿ (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ಯದಿ (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ಯದಿ (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

ಶೂನ್ಯ log_parse_environment(ಶೂನ್ಯ) {
        /* Do not call from library code. */

        ಯದಿ (should_parse_proc_cmdline())
                (ಶೂನ್ಯ) proc_cmdline_parse(parse_proc_cmdline_item, ರಿಕ್ತ, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ log_target;
}

ಶೂನ್ಯ log_settle_target(ಶೂನ್ಯ) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        ಯದಿ (t != LOG_TARGET_AUTO)
                ಪ್ರತ್ಯಾಗಮ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

ಪೂರ್ಣಾಙ್ಕ log_get_max_level(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ log_max_level;
}

ಪೂರ್ಣಾಙ್ಕ log_get_target_max_level(LogTarget target) {
        ಪ್ರತಿಜ್ಞಾ(target >= 0);
        ಪ್ರತಿಜ್ಞಾ(target < _LOG_TARGET_SINGLE_MAX);
        ಪ್ರತ್ಯಾಗಮ log_target_max_level[target];
}

ಶೂನ್ಯ log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ show_color > 0; /* Defaults to false. */
}

ಶೂನ್ಯ log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ show_location;
}

ಶೂನ್ಯ log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ show_time;
}

ಶೂನ್ಯ log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ show_tid;
}

ಶೂನ್ಯ log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ show_executable;
}

ಪೂರ್ಣಾಙ್ಕ log_show_color_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = parse_boolean(e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        log_show_color(r);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_show_location_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = parse_boolean(e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        log_show_location(r);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_show_time_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = parse_boolean(e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        log_show_time(r);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_show_tid_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = parse_boolean(e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        log_show_tid(r);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ log_show_executable_from_string(ಧ್ರುವ ವರ್ಣ *e) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = parse_boolean(e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        log_show_executable(r);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool log_on_console(ಶೂನ್ಯ) {
        ಯದಿ (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ log_target_table[_LOG_TARGET_MAX] = {
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

ಶೂನ್ಯ log_received_signal(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ಸಂರಚನಾ signalfd_siginfo *si) {
        ಪ್ರತಿಜ್ಞಾ(si);

        ಯದಿ (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                (ಶೂನ್ಯ) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } ಅನ್ಯಥಾ
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

ಶೂನ್ಯ set_log_syntax_callback(log_syntax_callback_t cb, ಶೂನ್ಯ *userdata) {
        ಪ್ರತಿಜ್ಞಾ(!log_syntax_callback || !cb);
        ಪ್ರತಿಜ್ಞಾ(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

ಪೂರ್ಣಾಙ್ಕ log_syntax_internal(
                ಧ್ರುವ ವರ್ಣ *unit,
                ಪೂರ್ಣಾಙ್ಕ level,
                ಧ್ರುವ ವರ್ಣ *config_file,
                unsigned config_ಸಾಲು,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format, ...) {

        PROTECT_ERRNO;

        ಯದಿ (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ಯದಿ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ಪ್ರತ್ಯಾಗಮ -ERRNO_ಮೌಲ್ಯ(error);

        ವರ್ಣ buffer[LINE_MAX];
        va_list ap;
        ಧ್ರುವ ವರ್ಣ *unit_fmt = ರಿಕ್ತ;

        errno = ERRNO_ಮೌಲ್ಯ(error);

        va_start(ap, format);
        (ಶೂನ್ಯ) vsnprintf(buffer, ಪರಿಮಾಣ buffer, format, ap);
        va_end(ap);

        ಯದಿ (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ಯದಿ (config_file) {
                ಯದಿ (config_ಸಾಲು > 0)
                        ಪ್ರತ್ಯಾಗಮ log_struct_internal(
                                        level,
                                        error,
                                        file, ಸಾಲು, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_ಸಾಲು),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_ಸಾಲು, buffer),
                                        unit_fmt, unit,
                                        ರಿಕ್ತ);
                ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ log_struct_internal(
                                        level,
                                        error,
                                        file, ಸಾಲು, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        ರಿಕ್ತ);
        } ಅನ್ಯಥಾ ಯದಿ (unit)
                ಪ್ರತ್ಯಾಗಮ log_struct_internal(
                                level,
                                error,
                                file, ಸಾಲು, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                ರಿಕ್ತ);
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ log_struct_internal(
                                level,
                                error,
                                file, ಸಾಲು, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ log_syntax_invalid_utf8_internal(
                ಧ್ರುವ ವರ್ಣ *unit,
                ಪೂರ್ಣಾಙ್ಕ level,
                ಧ್ರುವ ವರ್ಣ *config_file,
                unsigned config_ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

        ಯದಿ (rvalue)
                p = utf8_escape_invalid(rvalue);

        ಪ್ರತ್ಯಾಗಮ log_syntax_internal(unit, level, config_file, config_ಸಾಲು,
                                   SYNTHETIC_ERRNO(EINVAL), file, ಸಾಲು, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

ಪೂರ್ಣಾಙ್ಕ log_syntax_parse_error_internal(
                ಧ್ರುವ ವರ್ಣ *unit,
                ಧ್ರುವ ವರ್ಣ *config_file,
                unsigned config_ಸಾಲು,
                ಪೂರ್ಣಾಙ್ಕ error,
                bool critical,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *lvalue,
                ಧ್ರುವ ವರ್ಣ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ವರ್ಣ *escaped = ರಿಕ್ತ;

        /* OOM is always handled as critical. */
        ಯದಿ (ERRNO_ಮೌಲ್ಯ(error) == ENOMEM)
                ಪ್ರತ್ಯಾಗಮ log_oom_internal(LOG_ERR, file, ಸಾಲು, func);

        ಯದಿ (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ಯದಿ (!escaped)
                        rvalue = "(oom)";
                ಅನ್ಯಥಾ
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_ಸಾಲು, error,
                            file, ಸಾಲು, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ಪ್ರತ್ಯಾಗಮ critical ? -ERRNO_ಮೌಲ್ಯ(error) : 0;
}

ಶೂನ್ಯ log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        ಯದಿ (b) {
                ಯದಿ (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                ಅನ್ಯಥಾ ಯದಿ (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

ಶೂನ್ಯ log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

ಶೂನ್ಯ log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

ಶೂನ್ಯ log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

ಪೂರ್ಣಾಙ್ಕ log_emergency_level(ಶೂನ್ಯ) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ಪ್ರತ್ಯಾಗಮ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

ಪೂರ್ಣಾಙ್ಕ log_dup_console(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ಯದಿ (console_fd < 0 || console_fd >= 3)
                ಪ್ರತ್ಯಾಗಮ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ಯದಿ (copy < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        console_fd = copy;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ log_setup(ಶೂನ್ಯ) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (ಶೂನ್ಯ) log_open();
        ಯದಿ (log_on_console() && show_color < 0)
                log_show_color(true);
}

ಧ್ರುವ ವರ್ಣ* _log_set_prefix(ಧ್ರುವ ವರ್ಣ *prefix, bool force) {
        ಧ್ರುವ ವರ್ಣ *old = log_prefix;

        ಯದಿ (prefix || force)
                log_prefix = prefix;

        ಪ್ರತ್ಯಾಗಮ old;
}
