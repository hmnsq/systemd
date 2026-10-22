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

#ವ್ಯಾಖ್ಯಾನಿಸು SNDBUF_ಗಾತ್ರ (8*1024*1024)
#ವ್ಯಾಖ್ಯಾನಿಸು IOVEC_MAX 256U

ಸ್ಥಿರ log_syntax_callback_t log_syntax_callback = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ *log_syntax_callback_userdata = ಖಾಲಿ;

ಸ್ಥಿರ Log_ಗುರಿ log_ಗುರಿ = LOG_ಗುರಿ_CONSOLE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_max_level = LOG_INFO;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_ಗುರಿ_max_level[LOG_ಗುರಿ_SINGLE_MAX] = {
        [LOG_ಗುರಿ_CONSOLE] = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ,
        [LOG_ಗುರಿ_KMSG]    = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ,
        [LOG_ಗುರಿ_SYSLOG]  = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ,
        [LOG_ಗುರಿ_JOURNAL] = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ,
};
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_facility = LOG_DAEMON;
ಸ್ಥಿರ bool ratelimit_kmsg = true;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ console_fd = STDERR_FILENO;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ syslog_fd = -EBADF;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ kmsg_fd = -EBADF;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ journal_fd = -EBADF;

ಸ್ಥಿರ bool syslog_is_ಹರಿವು = false;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ show_color = -1; /* tristate */
ಸ್ಥಿರ bool show_location = false;
ಸ್ಥಿರ bool show_ಸಮಯ = false;
ಸ್ಥಿರ bool show_tid = false;
ಸ್ಥಿರ bool show_executable = false;

ಸ್ಥಿರ bool upgrade_syslog_ಗಮ್ಯ_journal = false;
ಸ್ಥಿರ bool always_reopen_console = false;
ಸ್ಥಿರ bool open_when_needed = false;
ಸ್ಥಿರ bool prohibit_ipc = false;

ಸ್ಥಿರ thread_local ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *log_ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;

#ಆದರೆ LOG_ಸಂದೇಶ_VERIFICATION || defined(__COVERITY__)
bool log_ಸಂದೇಶ_dummy = false; /* Always false */
#ಆದರೆ_ಅಂತ್ಯ

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#ವ್ಯಾಖ್ಯಾನಿಸು assert_raw(ಅಭಿವ್ಯಕ್ತಿ)                                                \
        ಮಾಡು {                                                            \
                ಆದರೆ (_unlikely_(!(ಅಭಿವ್ಯಕ್ತಿ))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ಆಗ (false)

ಶೂನ್ಯ log_ಪೂರ್ವಪ್ರತ್ಯಯ_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಪೂರ್ವಪ್ರತ್ಯಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

        ಅದಲುಬದಲು_TWO(log_ಪೂರ್ವಪ್ರತ್ಯಯ, *ಪೂರ್ವಪ್ರತ್ಯಯ);
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_console(ಶೂನ್ಯ) {
        /* See comment in log_close_journal() */
        (ಶೂನ್ಯ) ಸುರಕ್ಷಿತ_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_open_console(ಶೂನ್ಯ) {

        ಆದರೆ (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (console_fd < 3) {
                ಪೂರ್ಣಾಂಕ fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_kmsg(ಶೂನ್ಯ) {
        /* See comment in log_close_journal() */
        (ಶೂನ್ಯ) ಸುರಕ್ಷಿತ_close(TAKE_FD(kmsg_fd));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_open_kmsg(ಶೂನ್ಯ) {

        ಆದರೆ (kmsg_fd >= 0)
                ಹಿಂತಿರುಗಿ 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ಆದರೆ (kmsg_fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_syslog(ಶೂನ್ಯ) {
        /* See comment in log_close_journal() */
        (ಶೂನ್ಯ) ಸುರಕ್ಷಿತ_close(TAKE_FD(syslog_fd));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ create_log_socket(ಪೂರ್ಣಾಂಕ type) {
        ರಚನೆ timeval tv;
        ಪೂರ್ಣಾಂಕ fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        fd = fd_move_above_stdio(fd);
        (ಶೂನ್ಯ) fd_inc_sndbuf(fd, SNDBUF_ಗಾತ್ರ);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ಆದರೆ (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_ಪ್ರತಿ_MSEC);
        ಇಲ್ಲದಿದ್ದರೆ
                timeval_store(&tv, 10 * USEC_ಪ್ರತಿ_SEC);
        (ಶೂನ್ಯ) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, ಗಾತ್ರ(tv));

        ಹಿಂತಿರುಗಿ fd;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_open_syslog(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (syslog_fd >= 0)
                ಹಿಂತಿರುಗಿ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ಆದರೆ (syslog_fd < 0) {
                r = syslog_fd;
                ಹೋಗು fail;
        }

        r = connect_unix_ಮಾರ್ಗ(syslog_fd, AT_FDCWD, "/dev/log");
        ಆದರೆ (r < 0) {
                ಸುರಕ್ಷಿತ_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ಆದರೆ (syslog_fd < 0) {
                        r = syslog_fd;
                        ಹೋಗು fail;
                }

                r = connect_unix_ಮಾರ್ಗ(syslog_fd, AT_FDCWD, "/dev/log");
                ಆದರೆ (r < 0)
                        ಹೋಗು fail;

                syslog_is_ಹರಿವು = true;
        } ಇಲ್ಲದಿದ್ದರೆ
                syslog_is_ಹರಿವು = false;

        ಹಿಂತಿರುಗಿ 0;

fail:
        log_close_syslog();
        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_close_journal(ಶೂನ್ಯ) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (ಶೂನ್ಯ) ಸುರಕ್ಷಿತ_close(TAKE_FD(journal_fd));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_open_journal(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (journal_fd >= 0)
                ಹಿಂತಿರುಗಿ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ಆದರೆ (journal_fd < 0) {
                r = journal_fd;
                ಹೋಗು fail;
        }

        r = connect_unix_ಮಾರ್ಗ(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ಆದರೆ (r < 0)
                ಹೋಗು fail;

        ಹಿಂತಿರುಗಿ 0;

fail:
        log_close_journal();
        ಹಿಂತಿರುಗಿ r;
}

bool stderr_is_journal(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *w = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        uint64_t dev, ino;
        ರಚನೆ stat st;

        e = getenv("JOURNAL_STREAM");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&e, &w, ":", ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS) <= 0)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಸುರಕ್ಷಿತ_atou64(w, &dev) < 0)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (ಸುರಕ್ಷಿತ_atou64(e, &ino) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (fstat(STDERR_FILENO, &st) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ st.st_dev == dev && st.st_ino == ino;
}

ಪೂರ್ಣಾಂಕ log_open(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ಆದರೆ (log_ಗುರಿ == LOG_ಗುರಿ_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (getpid_cached() == 1 ||
            stderr_is_journal() ||
            ಒಳ_ಗಣ(log_ಗುರಿ,
                   LOG_ಗುರಿ_KMSG,
                   LOG_ಗುರಿ_JOURNAL,
                   LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                   LOG_ಗುರಿ_SYSLOG,
                   LOG_ಗುರಿ_SYSLOG_OR_KMSG)) {

                ಆದರೆ (!prohibit_ipc) {
                        ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ,
                                   LOG_ಗುರಿ_AUTO,
                                   LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                                   LOG_ಗುರಿ_JOURNAL)) {

                                r = log_open_journal();
                                ಆದರೆ (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        ಹಿಂತಿರುಗಿ r;
                                }
                        }

                        ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ,
                                   LOG_ಗುರಿ_SYSLOG_OR_KMSG,
                                   LOG_ಗುರಿ_SYSLOG)) {

                                r = log_open_syslog();
                                ಆದರೆ (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ಹಿಂತಿರುಗಿ r;
                                }
                        }
                }

                ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ, LOG_ಗುರಿ_AUTO,
                                       LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                                       LOG_ಗುರಿ_SYSLOG_OR_KMSG,
                                       LOG_ಗುರಿ_KMSG)) {
                        r = log_open_kmsg();
                        ಆದರೆ (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                ಹಿಂತಿರುಗಿ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ಹಿಂತಿರುಗಿ log_open_console();
}

ಶೂನ್ಯ log_ಗಣ_ಗುರಿ(Log_ಗುರಿ ಗುರಿ) {
        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ < LOG_ಗುರಿ_MAX);

        ಆದರೆ (upgrade_syslog_ಗಮ್ಯ_journal) {
                ಆದರೆ (ಗುರಿ == LOG_ಗುರಿ_SYSLOG)
                        ಗುರಿ = LOG_ಗುರಿ_JOURNAL;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗುರಿ == LOG_ಗುರಿ_SYSLOG_OR_KMSG)
                        ಗುರಿ = LOG_ಗುರಿ_JOURNAL_OR_KMSG;
        }

        log_ಗುರಿ = ಗುರಿ;
}

ಶೂನ್ಯ log_ಗಣ_ಗುರಿ_and_open(Log_ಗುರಿ ಗುರಿ) {
        log_ಗಣ_ಗುರಿ(ಗುರಿ);
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

ಪೂರ್ಣಾಂಕ log_ಗಣ_max_level(ಪೂರ್ಣಾಂಕ level) {
        ಪ್ರತಿಜ್ಞಾ(level == LOG_NULL || log_level_is_valid(level));

        ಪೂರ್ಣಾಂಕ old = log_max_level;
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

        ಹಿಂತಿರುಗಿ old;
}

ಶೂನ್ಯ log_ಗಣ_facility(ಪೂರ್ಣಾಂಕ facility) {
        log_facility = facility;
}

ಸ್ಥಿರ bool check_console_fd_is_tty(ಶೂನ್ಯ) {
        ಆದರೆ (console_fd < 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_ಸುರಕ್ಷಿತ(console_fd);

        ಹಿಂತಿರುಗಿ console_fd_is_tty;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_ಗಮ್ಯ_console(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್) {

        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dumb = -1;

        ಅಕ್ಷರ location[256],
             ಶೀರ್ಷಿಕೆ_ಸಮಯ[FORMAT_TIMESTAMP_MAX],
             ಪೂರ್ವಪ್ರತ್ಯಯ[1 + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ರಚನೆ iovec iovec[13];
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *on = ಖಾಲಿ, *off = ಖಾಲಿ;
        size_t n = 0;

        ಆದರೆ (console_fd < 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ಆದರೆ (LOG_PRI(level) > log_ಗುರಿ_max_level[LOG_ಗುರಿ_CONSOLE])
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (log_ಗುರಿ == LOG_ಗುರಿ_CONSOLE_PREFIXED) {
                xsprintf(ಪೂರ್ವಪ್ರತ್ಯಯ, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(ಪೂರ್ವಪ್ರತ್ಯಯ);
        }

        ಆದರೆ (show_ಸಮಯ &&
            format_timestamp(ಶೀರ್ಷಿಕೆ_ಸಮಯ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_ಸಮಯ), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ_ಸಮಯ);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ಆದರೆ (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(ಕಾರ್ಯಕ್ರಮ_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ಆದರೆ (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ಆದರೆ (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ಖಾಲಿ);

        ಆದರೆ (show_location) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lon = "", *loff = "";
                ಆದರೆ (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_ಸಾಮಾನ್ಯ();
                }

                (ಶೂನ್ಯ) snprintf(location, ಗಾತ್ರ location, "%s%s:%i%s: ", lon, file, ಸಾಲು, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ಆದರೆ (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ಆದರೆ (log_ಪೂರ್ವಪ್ರತ್ಯಯ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_ಪೂರ್ವಪ್ರತ್ಯಯ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(ಬಫರ್);
        ಆದರೆ (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ಆದರೆ (writev(console_fd, iovec, n) < 0) {

                ಆದರೆ (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (ಶೂನ್ಯ) log_open_console();
                        ಆದರೆ (console_fd < 0)
                                ಹಿಂತಿರುಗಿ 0;

                        ಆದರೆ (writev(console_fd, iovec, n) < 0)
                                ಹಿಂತಿರುಗಿ -errno;
                } ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_ಗಮ್ಯ_syslog(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್) {

        ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ_ಆದ್ಯತೆ[2 + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ) + 1],
             ಶೀರ್ಷಿಕೆ_ಸಮಯ[64],
             ಶೀರ್ಷಿಕೆ_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ರಚನೆ tm tm;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (syslog_fd < 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (LOG_PRI(level) > log_ಗುರಿ_max_level[LOG_ಗುರಿ_SYSLOG])
                ಹಿಂತಿರುಗಿ 0;

        xsprintf(ಶೀರ್ಷಿಕೆ_ಆದ್ಯತೆ, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (strftime(ಶೀರ್ಷಿಕೆ_ಸಮಯ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_ಸಮಯ), "%h %e %T ", &tm) <= 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        xsprintf(ಶೀರ್ಷಿಕೆ_pid, "["PID_FMT"]: ", getpid_cached());

        ರಚನೆ iovec iovec[] = {
                IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ_ಆದ್ಯತೆ),
                IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ_ಸಮಯ),
                IOVEC_MAKE_STRING(ಕಾರ್ಯಕ್ರಮ_invocation_short_name),
                IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ_pid),
                IOVEC_MAKE_STRING(strempty(log_ಪೂರ್ವಪ್ರತ್ಯಯ)),
                IOVEC_MAKE_STRING(log_ಪೂರ್ವಪ್ರತ್ಯಯ ? ": " : ""),
                IOVEC_MAKE_STRING(ಬಫರ್),
        };
        ಸ್ಥಿರಾಂಕ ರಚನೆ msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        ಆದರೆ (syslog_is_ಹರಿವು)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        ಗಾಗಿ (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (!syslog_is_ಹರಿವು)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        ನಿಲ್ಲಿಸು;
        }

        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_ಗಮ್ಯ_kmsg(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ಸ್ಥಿರ thread_local ದರ_ಎಲ್ಲೆ ratelimit = { 5 * USEC_ಪ್ರತಿ_SEC, 200 };

        ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ_ಆದ್ಯತೆ[2 + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ) + 1],
             ಶೀರ್ಷಿಕೆ_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ಆದರೆ (kmsg_fd < 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (LOG_PRI(level) > log_ಗುರಿ_max_level[LOG_ಗುರಿ_KMSG])
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (ratelimit_kmsg && !ratelimit_ಕೆಳಗೆ(&ratelimit)) {
                ಆದರೆ (ratelimit_ಅಂಕೆ_dropped(&ratelimit) > 1)
                        ಹಿಂತಿರುಗಿ 0;

                ಬಫರ್ = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(ಶೀರ್ಷಿಕೆ_ಆದ್ಯತೆ, "<%i>", level);
        xsprintf(ಶೀರ್ಷಿಕೆ_pid, "["PID_FMT"]: ", getpid_cached());

        ಸ್ಥಿರಾಂಕ ರಚನೆ iovec iovec[] = {
                IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ_ಆದ್ಯತೆ),
                IOVEC_MAKE_STRING(ಕಾರ್ಯಕ್ರಮ_invocation_short_name),
                IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ_pid),
                IOVEC_MAKE_STRING(strempty(log_ಪೂರ್ವಪ್ರತ್ಯಯ)),
                IOVEC_MAKE_STRING(log_ಪೂರ್ವಪ್ರತ್ಯಯ ? ": " : ""),
                IOVEC_MAKE_STRING(ಬಫರ್),
                IOVEC_MAKE_STRING("\n"),
        };

        ಆದರೆ (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_do_ಶೀರ್ಷಿಕೆ(
                ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ,
                size_t ಗಾತ್ರ_2,
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra) {
        ಪೂರ್ಣಾಂಕ r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_ಮೌಲ್ಯ(error);

        r = snprintf(ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ_2,
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
                     isempty(object) ? "" : ASSERT_PTR(object_ಕ್ಷೇತ್ರ),
                     isempty(object) ? "" : object,
                     isempty(object) ? "" : "\n",
                     isempty(extra) ? "" : ASSERT_PTR(extra_ಕ್ಷೇತ್ರ),
                     isempty(extra) ? "" : extra,
                     isempty(extra) ? "" : "\n",
                     ಕಾರ್ಯಕ್ರಮ_invocation_short_name);
        assert_raw((size_t) r < ಗಾತ್ರ_2);

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ log_do_context(ರಚನೆ iovec *iovec, size_t iovec_len, size_t *n) {
        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n);

        LIST_FOREACH(ll, c, log_context_ತಲೆ()) {
                STRV_FOREACH(s, c->fields) {
                        ಆದರೆ (*n + 2 >= iovec_len)
                                ಹಿಂತಿರುಗಿ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ಗಾಗಿ (size_t i = 0; i < c->n_ಒಳಹರಿವು_iovec; i++) {
                        ಆದರೆ (*n + 2 >= iovec_len)
                                ಹಿಂತಿರುಗಿ;

                        iovec[(*n)++] = c->ಒಳಹರಿವು_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ಆದರೆ (c->ಕೀಲಿ && c->ಮೌಲ್ಯ) {
                        ಆದರೆ (*n + 3 >= iovec_len)
                                ಹಿಂತಿರುಗಿ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ಕೀಲಿ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ಮೌಲ್ಯ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_ಗಮ್ಯ_journal(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್) {

        ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ[LINE_MAX];
        size_t n = 0, iovec_len;
        ರಚನೆ iovec *iovec;

        ಆದರೆ (journal_fd < 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (LOG_PRI(level) > log_ಗುರಿ_max_level[LOG_ಗುರಿ_JOURNAL])
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!MUL_ಸುರಕ್ಷಿತ(&iovec_len, log_context_ಅಂಕೆ_fields_2(), 3) ||
            !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        ಇಲ್ಲದಿದ್ದರೆ
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ರಚನೆ iovec, iovec_len);

        log_do_ಶೀರ್ಷಿಕೆ(ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ), level, error, file, ಸಾಲು, func, object_ಕ್ಷೇತ್ರ, object, extra_ಕ್ಷೇತ್ರ, extra);

        iovec[n++] = IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ಆದರೆ (log_ಪೂರ್ವಪ್ರತ್ಯಯ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_ಪೂರ್ವಪ್ರತ್ಯಯ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(ಬಫರ್);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ಸ್ಥಿರಾಂಕ ರಚನೆ msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        ಆದರೆ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ log_dispatch_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಅಕ್ಷರ *ಬಫರ್) {

        assert_raw(ಬಫರ್);

        ಆದರೆ (log_ಗುರಿ == LOG_ಗುರಿ_NULL)
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ಆದರೆ (LOG_FAC(level) == 0)
                level |= log_facility;

        ಆದರೆ (open_when_needed)
                (ಶೂನ್ಯ) log_open();

        ಮಾಡು {
                ಅಕ್ಷರ *e;
                ಪೂರ್ಣಾಂಕ k = 0;

                ಬಫರ್ += strspn(ಬಫರ್, NEWLINE);

                ಆದರೆ (ಬಫರ್[0] == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ ((e = strpbrk(ಬಫರ್, NEWLINE)))
                        *(e++) = 0;

                ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ, LOG_ಗುರಿ_AUTO,
                                       LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                                       LOG_ಗುರಿ_JOURNAL)) {

                        k = write_ಗಮ್ಯ_journal(level, error, file, ಸಾಲು, func, object_ಕ್ಷೇತ್ರ, object, extra_ಕ್ಷೇತ್ರ, extra, ಬಫರ್);
                        ಆದರೆ (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ, LOG_ಗುರಿ_SYSLOG_OR_KMSG,
                                       LOG_ಗುರಿ_SYSLOG)) {

                        k = write_ಗಮ್ಯ_syslog(level, error, file, ಸಾಲು, func, ಬಫರ್);
                        ಆದರೆ (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ಆದರೆ (k <= 0 &&
                    ಒಳ_ಗಣ(log_ಗುರಿ, LOG_ಗುರಿ_AUTO,
                                       LOG_ಗುರಿ_SYSLOG_OR_KMSG,
                                       LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                                       LOG_ಗುರಿ_KMSG)) {

                        ಆದರೆ (k < 0)
                                log_open_kmsg();

                        k = write_ಗಮ್ಯ_kmsg(level, error, file, ಸಾಲು, func, ಬಫರ್);
                        ಆದರೆ (k < 0) {
                                log_close_kmsg();
                                (ಶೂನ್ಯ) log_open_console();
                        }
                }

                ಆದರೆ (k <= 0)
                        (ಶೂನ್ಯ) write_ಗಮ್ಯ_console(level, error, file, ಸಾಲು, func, ಬಫರ್);

                ಬಫರ್ = e;
        } ಆಗ (ಬಫರ್);

        ಆದರೆ (open_when_needed)
                log_close();

        ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);
}

ಪೂರ್ಣಾಂಕ log_dump_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಅಕ್ಷರ *ಬಫರ್) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ಆದರೆ (_likely_(LOG_PRI(level) > log_max_level))
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        ಹಿಂತಿರುಗಿ log_dispatch_ಆಂತರಿಕ(level, error, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಬಫರ್);
}

ಪೂರ್ಣಾಂಕ log_internalv(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                va_list ap) {

        ಆದರೆ (_likely_(LOG_PRI(level) > log_max_level))
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        ಅಕ್ಷರ ಬಫರ್[LINE_MAX];
        LOCAL_ERRNO(ERRNO_ಮೌಲ್ಯ(error));

        (ಶೂನ್ಯ) vsnprintf(ಬಫರ್, ಗಾತ್ರ ಬಫರ್, format, ap);

        ಹಿಂತಿರುಗಿ log_dispatch_ಆಂತರಿಕ(level, error, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಬಫರ್);
}

ಪೂರ್ಣಾಂಕ log_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = log_internalv(level, error, file, ಸಾಲು, func, format, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ log_object_internalv(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                va_list ap) {

        ಅಕ್ಷರ *ಬಫರ್, *b;

        ಆದರೆ (_likely_(LOG_PRI(level) > log_max_level))
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_ಮೌಲ್ಯ(error));

        LOG_ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ(object);

        b = ಬಫರ್ = newa(ಅಕ್ಷರ, LINE_MAX);
        (ಶೂನ್ಯ) vsnprintf(b, LINE_MAX, format, ap);

        ಹಿಂತಿರುಗಿ log_dispatch_ಆಂತರಿಕ(level, error, file, ಸಾಲು, func,
                                     object_ಕ್ಷೇತ್ರ, object, extra_ಕ್ಷೇತ್ರ, extra, ಬಫರ್);
}

ಪೂರ್ಣಾಂಕ log_object_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, ಸಾಲು, func, object_ಕ್ಷೇತ್ರ, object, extra_ಕ್ಷೇತ್ರ, extra, format, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ log_oom_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func) {
        ಹಿಂತಿರುಗಿ log_ಆಂತರಿಕ(level, ENOMEM, file, ಸಾಲು, func, "Out of memory.");
}

ಪೂರ್ಣಾಂಕ log_format_iovec(
                ರಚನೆ iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                va_list ap) {

        ಪ್ರತಿಜ್ಞಾ(iovec);
        ಪ್ರತಿಜ್ಞಾ(n);

        ಆಗ (format && *n + 1 < iovec_len) {
                va_list aq;
                ಅಕ್ಷರ *m;
                ಪೂರ್ಣಾಂಕ r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_ಮೌಲ್ಯ(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ಆದರೆ (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, ಅಕ್ಷರ *);
        }
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_struct_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {

        ಅಕ್ಷರ buf[LINE_MAX];
        bool ಸಿಕ್ಕಿತು = false;
        PROTECT_ERRNO;
        va_list ap;

        ಆದರೆ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ಗುರಿ == LOG_ಗುರಿ_NULL)
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        ಆದರೆ (LOG_FAC(level) == 0)
                level |= log_facility;

        ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ,
                   LOG_ಗುರಿ_AUTO,
                   LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                   LOG_ಗುರಿ_JOURNAL)) {

                ಆದರೆ (open_when_needed)
                        log_open_journal();

                ಆದರೆ (journal_fd >= 0) {
                        ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ[LINE_MAX];
                        ರಚನೆ iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        ಪೂರ್ಣಾಂಕ r;
                        bool fallback = false;

                        ಆದರೆ (!MUL_ಸುರಕ್ಷಿತ(&iovec_len, log_context_ಅಂಕೆ_fields_2(), 3) ||
                            !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        ಇಲ್ಲದಿದ್ದರೆ
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ರಚನೆ iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_ಶೀರ್ಷಿಕೆ(ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ), level, error, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
                        iovec[n++] = IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        ಆದರೆ (r < 0)
                                fallback = true;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                log_do_context(iovec, iovec_len, &n);

                                ಸ್ಥಿರಾಂಕ ರಚನೆ msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (ಶೂನ್ಯ) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        ಗಾಗಿ (size_t i = 1; i < m; i += 2)
                                ಬಿಡುಗಡೆ(iovec[i].iov_base);

                        ಆದರೆ (!fallback) {
                                ಆದರೆ (open_when_needed)
                                        log_close();

                                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ಆಗ (format) {
                va_list aq;

                errno = ERRNO_ಮೌಲ್ಯ(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (ಶೂನ್ಯ) vsnprintf(buf, ಗಾತ್ರ buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ಆದರೆ (startswith(buf, "MESSAGE=")) {
                        ಸಿಕ್ಕಿತು = true;
                        ನಿಲ್ಲಿಸು;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, ಅಕ್ಷರ *);
        }
        va_end(ap);

        ಆದರೆ (!ಸಿಕ್ಕಿತು) {
                ಆದರೆ (open_when_needed)
                        log_close();

                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);
        }

        ಹಿಂತಿರುಗಿ log_dispatch_ಆಂತರಿಕ(level, error, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, buf + 8);
}

ಪೂರ್ಣಾಂಕ log_struct_iovec_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ರಚನೆ iovec ಒಳಹರಿವು_iovec[],
                size_t n_ಒಳಹರಿವು_iovec) {

        PROTECT_ERRNO;

        ಆದರೆ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ಗುರಿ == LOG_ಗುರಿ_NULL)
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        ಆದರೆ (LOG_FAC(level) == 0)
                level |= log_facility;

        ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ, LOG_ಗುರಿ_AUTO,
                               LOG_ಗುರಿ_JOURNAL_OR_KMSG,
                               LOG_ಗುರಿ_JOURNAL) &&
            journal_fd >= 0) {

                ಅಕ್ಷರ ಶೀರ್ಷಿಕೆ[LINE_MAX];
                ರಚನೆ iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_ಒಳಹರಿವು_iovec_expanded;

                ಆದರೆ (!MUL_ಸುರಕ್ಷಿತ(&n_ಒಳಹರಿವು_iovec_expanded, n_ಒಳಹರಿವು_iovec, 2) ||
                    !MUL_ಸುರಕ್ಷಿತ(&n_context_iovec, log_context_ಅಂಕೆ_fields_2(), 3) ||
                    !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&iovec_len, 1, n_ಒಳಹರಿವು_iovec_expanded) ||
                    !ಕೂಡಿಸು_ಸುರಕ್ಷಿತ(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                ಇಲ್ಲದಿದ್ದರೆ
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ರಚನೆ iovec, iovec_len);

                log_do_ಶೀರ್ಷಿಕೆ(ಶೀರ್ಷಿಕೆ, ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ), level, error, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

                iovec[n++] = IOVEC_MAKE_STRING(ಶೀರ್ಷಿಕೆ);
                ಗಾಗಿ (size_t i = 0; i < n_ಒಳಹರಿವು_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = ಒಳಹರಿವು_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ಸ್ಥಿರಾಂಕ ರಚನೆ msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ಆದರೆ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);
        }

        ಗಾಗಿ (size_t i = 0; i < n_ಒಳಹರಿವು_iovec; i++)
                ಆದರೆ (ಸ್ಮರಣೆ_startswith(ಒಳಹರಿವು_iovec[i].iov_base, ಒಳಹರಿವು_iovec[i].iov_len, "MESSAGE=")) {
                        ಅಕ್ಷರ *m;

                        m = strndupa_ಸುರಕ್ಷಿತ((ಅಕ್ಷರ*) ಒಳಹರಿವು_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          ಒಳಹರಿವು_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ಹಿಂತಿರುಗಿ log_dispatch_ಆಂತರಿಕ(level, error, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, m);
                }

        /* Couldn't find MESSAGE=. */
        ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);
}

ಪೂರ್ಣಾಂಕ log_ಗಣ_ಗುರಿ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        Log_ಗುರಿ t;

        t = log_ಗುರಿ_from_string(e);
        ಆದರೆ (t < 0)
                ಹಿಂತಿರುಗಿ t;

        log_ಗಣ_ಗುರಿ(t);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_ಗಣ_max_level_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಶಬ್ದ = ಖಾಲಿ, *ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;
                Log_ಗುರಿ ಗುರಿ;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *colon;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&e, &ಶಬ್ದ, ",", 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                colon = strchr(ಶಬ್ದ, ':');
                ಆದರೆ (!colon) {
                        r = log_level_from_string(ಶಬ್ದ);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        log_ಗಣ_max_level(r);
                        ಮುಂದುವರಿ;
                }

                ಪೂರ್ವಪ್ರತ್ಯಯ = strndup(ಶಬ್ದ, colon - ಶಬ್ದ);
                ಆದರೆ (!ಪೂರ್ವಪ್ರತ್ಯಯ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಗುರಿ = log_ಗುರಿ_from_string(ಪೂರ್ವಪ್ರತ್ಯಯ);
                ಆದರೆ (ಗುರಿ < 0)
                        ಹಿಂತಿರುಗಿ ಗುರಿ;

                ಆದರೆ (ಗುರಿ >= LOG_ಗುರಿ_SINGLE_MAX)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                r = log_level_from_string(colon + 1);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                log_ಗುರಿ_max_level[ಗುರಿ] = r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_max_ಮಟ್ಟಗಳು_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ level, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = log_level_ಗಮ್ಯ_string_alloc(level, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (Log_ಗುರಿ ಗುರಿ = 0; ಗುರಿ < LOG_ಗುರಿ_SINGLE_MAX; ಗುರಿ++) {
                _cleanup_free_ ಅಕ್ಷರ *l = ಖಾಲಿ;

                ಆದರೆ (log_ಗುರಿ_max_level[ಗುರಿ] == ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                        ಮುಂದುವರಿ;

                r = log_level_ಗಮ್ಯ_string_alloc(log_ಗುರಿ_max_level[ಗುರಿ], &l);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_ಗುರಿ_ಗಮ್ಯ_string(ಗುರಿ), l);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_ಗಣ_ratelimit_kmsg_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        r = parse_boolean(e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ratelimit_kmsg = r;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_proc_cmdline_ಅಂಶ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, ಶೂನ್ಯ *ದತ್ತಾಂಶ) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ಆದರೆ (streq(ಕೀಲಿ, "debug") && !ಮೌಲ್ಯ)
                log_ಗಣ_max_level(LOG_DEBUG);

        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_target")) {

                ಆದರೆ (proc_cmdline_ಮೌಲ್ಯ_missing(ಕೀಲಿ, ಮೌಲ್ಯ))
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (log_ಗಣ_ಗುರಿ_from_string(ಮೌಲ್ಯ) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", ಮೌಲ್ಯ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_level")) {

                ಆದರೆ (proc_cmdline_ಮೌಲ್ಯ_missing(ಕೀಲಿ, ಮೌಲ್ಯ))
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (log_ಗಣ_max_level_from_string(ಮೌಲ್ಯ) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_color")) {

                ಆದರೆ (log_show_color_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_location")) {

                ಆದರೆ (log_show_location_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_tid")) {

                ಆದರೆ (log_show_tid_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_time")) {

                ಆದರೆ (log_show_ಸಮಯ_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", ಮೌಲ್ಯ);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಕೀಲಿ, "systemd.log_ratelimit_kmsg")) {

                ಆದರೆ (log_ಗಣ_ratelimit_kmsg_from_string(ಮೌಲ್ಯ ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", ಮೌಲ್ಯ);
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ bool should_parse_proc_cmdline(ಶೂನ್ಯ) {
        /* PID1 always reads the kernel command line. */
        ಆದರೆ (getpid_cached() == 1)
                ಹಿಂತಿರುಗಿ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ಹಿಂತಿರುಗಿ invoked_by_systemd();
}

ಶೂನ್ಯ log_parse_environment_variables(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ಆದರೆ (e && log_ಗಣ_ಗುರಿ_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ಆದರೆ (e) {
                r = log_ಗಣ_max_level_from_string(e);
                ಆದರೆ (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ಆದರೆ (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r > 0)
                        log_ಗಣ_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ಆದರೆ (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ಆದರೆ (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ಆದರೆ (e && log_show_ಸಮಯ_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ಆದರೆ (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ಆದರೆ (e && log_ಗಣ_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ಆದರೆ (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

ಶೂನ್ಯ log_parse_environment(ಶೂನ್ಯ) {
        /* Do not call from library code. */

        ಆದರೆ (should_parse_proc_cmdline())
                (ಶೂನ್ಯ) proc_cmdline_parse(parse_proc_cmdline_ಅಂಶ, ಖಾಲಿ, PROC_CMDLINE_STRIP_RD_ಪೂರ್ವಪ್ರತ್ಯಯ);

        log_parse_environment_variables();
}

Log_ಗುರಿ log_get_ಗುರಿ(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ log_ಗುರಿ;
}

ಶೂನ್ಯ log_settle_ಗುರಿ(ಶೂನ್ಯ) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        Log_ಗುರಿ t = log_get_ಗುರಿ();

        ಆದರೆ (t != LOG_ಗುರಿ_AUTO)
                ಹಿಂತಿರುಗಿ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_ಗುರಿ_KMSG : LOG_ಗುರಿ_JOURNAL_OR_KMSG)
                                                        : LOG_ಗುರಿ_CONSOLE;
        log_ಗಣ_ಗುರಿ(t);
}

ಪೂರ್ಣಾಂಕ log_get_max_level(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ log_max_level;
}

ಪೂರ್ಣಾಂಕ log_get_ಗುರಿ_max_level(Log_ಗುರಿ ಗುರಿ) {
        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ < LOG_ಗುರಿ_SINGLE_MAX);
        ಹಿಂತಿರುಗಿ log_ಗುರಿ_max_level[ಗುರಿ];
}

ಶೂನ್ಯ log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ show_color > 0; /* Defaults to false. */
}

ಶೂನ್ಯ log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ show_location;
}

ಶೂನ್ಯ log_show_ಸಮಯ(bool b) {
        show_ಸಮಯ = b;
}

bool log_get_show_ಸಮಯ(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ show_ಸಮಯ;
}

ಶೂನ್ಯ log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ show_tid;
}

ಶೂನ್ಯ log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ show_executable;
}

ಪೂರ್ಣಾಂಕ log_show_color_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        r = parse_boolean(e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        log_show_color(r);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_show_location_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        r = parse_boolean(e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        log_show_location(r);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_show_ಸಮಯ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        r = parse_boolean(e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        log_show_ಸಮಯ(r);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_show_tid_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        r = parse_boolean(e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        log_show_tid(r);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ log_show_executable_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಪೂರ್ಣಾಂಕ r;

        r = parse_boolean(e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        log_show_executable(r);
        ಹಿಂತಿರುಗಿ 0;
}

bool log_on_console(ಶೂನ್ಯ) {
        ಆದರೆ (ಒಳ_ಗಣ(log_ಗುರಿ, LOG_ಗುರಿ_CONSOLE,
                               LOG_ಗುರಿ_CONSOLE_PREFIXED))
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ log_ಗುರಿ_ಕೋಷ್ಟಕ[LOG_ಗುರಿ_MAX] = {
        [LOG_ಗುರಿ_CONSOLE]          = "console",
        [LOG_ಗುರಿ_CONSOLE_PREFIXED] = "console-prefixed",
        [LOG_ಗುರಿ_KMSG]             = "kmsg",
        [LOG_ಗುರಿ_JOURNAL]          = "journal",
        [LOG_ಗುರಿ_JOURNAL_OR_KMSG]  = "journal-or-kmsg",
        [LOG_ಗುರಿ_SYSLOG]           = "syslog",
        [LOG_ಗುರಿ_SYSLOG_OR_KMSG]   = "syslog-or-kmsg",
        [LOG_ಗುರಿ_AUTO]             = "auto",
        [LOG_ಗುರಿ_NULL]             = "null",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(log_ಗುರಿ, Log_ಗುರಿ);

ಶೂನ್ಯ log_received_signal(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ರಚನೆ signalfd_siginfo *si) {
        ಪ್ರತಿಜ್ಞಾ(si);

        ಆದರೆ (si_ಸಂಕೇತ_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                (ಶೂನ್ಯ) pid_get_comm(si->ssi_pid, &p);

                log_ತುಂಬಿದ(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_ಗಮ್ಯ_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } ಇಲ್ಲದಿದ್ದರೆ
                log_ತುಂಬಿದ(level,
                         "Received SIG%s.",
                         signal_ಗಮ್ಯ_string(si->ssi_signo));
}

ಶೂನ್ಯ ಗಣ_log_syntax_callback(log_syntax_callback_t cb, ಶೂನ್ಯ *userdata) {
        ಪ್ರತಿಜ್ಞಾ(!log_syntax_callback || !cb);
        ಪ್ರತಿಜ್ಞಾ(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

ಪೂರ್ಣಾಂಕ log_syntax_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಪೂರ್ಣಾಂಕ level,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config_file,
                ಚಿಹ್ನೆರಹಿತ config_ಸಾಲು,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {

        PROTECT_ERRNO;

        ಆದರೆ (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ಆದರೆ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ಗುರಿ == LOG_ಗುರಿ_NULL)
                ಹಿಂತಿರುಗಿ -ERRNO_ಮೌಲ್ಯ(error);

        ಅಕ್ಷರ ಬಫರ್[LINE_MAX];
        va_list ap;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit_fmt = ಖಾಲಿ;

        errno = ERRNO_ಮೌಲ್ಯ(error);

        va_start(ap, format);
        (ಶೂನ್ಯ) vsnprintf(ಬಫರ್, ಗಾತ್ರ ಬಫರ್, format, ap);
        va_end(ap);

        ಆದರೆ (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ಆದರೆ (config_file) {
                ಆದರೆ (config_ಸಾಲು > 0)
                        ಹಿಂತಿರುಗಿ log_struct_ಆಂತರಿಕ(
                                        level,
                                        error,
                                        file, ಸಾಲು, func,
                                        LOG_ಸಂದೇಶ_ID(SD_ಸಂದೇಶ_INVALID_CONFIGURATION_STR),
                                        LOG_ಅಂಶ("CONFIG_FILE=%s", config_file),
                                        LOG_ಅಂಶ("CONFIG_LINE=%u", config_ಸಾಲು),
                                        LOG_ಸಂದೇಶ("%s:%u: %s", config_file, config_ಸಾಲು, ಬಫರ್),
                                        unit_fmt, unit,
                                        ಖಾಲಿ);
                ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ log_struct_ಆಂತರಿಕ(
                                        level,
                                        error,
                                        file, ಸಾಲು, func,
                                        LOG_ಸಂದೇಶ_ID(SD_ಸಂದೇಶ_INVALID_CONFIGURATION_STR),
                                        LOG_ಅಂಶ("CONFIG_FILE=%s", config_file),
                                        LOG_ಸಂದೇಶ("%s: %s", config_file, ಬಫರ್),
                                        unit_fmt, unit,
                                        ಖಾಲಿ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unit)
                ಹಿಂತಿರುಗಿ log_struct_ಆಂತರಿಕ(
                                level,
                                error,
                                file, ಸಾಲು, func,
                                LOG_ಸಂದೇಶ_ID(SD_ಸಂದೇಶ_INVALID_CONFIGURATION_STR),
                                LOG_ಸಂದೇಶ("%s: %s", unit, ಬಫರ್),
                                unit_fmt, unit,
                                ಖಾಲಿ);
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ log_struct_ಆಂತರಿಕ(
                                level,
                                error,
                                file, ಸಾಲು, func,
                                LOG_ಸಂದೇಶ_ID(SD_ಸಂದೇಶ_INVALID_CONFIGURATION_STR),
                                LOG_ಸಂದೇಶ("%s", ಬಫರ್),
                                ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ log_syntax_invalid_utf8_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಪೂರ್ಣಾಂಕ level,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config_file,
                ಚಿಹ್ನೆರಹಿತ config_ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

        ಆದರೆ (rvalue)
                p = utf8_escape_invalid(rvalue);

        ಹಿಂತಿರುಗಿ log_syntax_ಆಂತರಿಕ(unit, level, config_file, config_ಸಾಲು,
                                   SYNTHETIC_ERRNO(EINVAL), file, ಸಾಲು, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

ಪೂರ್ಣಾಂಕ log_syntax_parse_error_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config_file,
                ಚಿಹ್ನೆರಹಿತ config_ಸಾಲು,
                ಪೂರ್ಣಾಂಕ error,
                bool critical,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lvalue,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ಅಕ್ಷರ *escaped = ಖಾಲಿ;

        /* OOM is always handled as critical. */
        ಆದರೆ (ERRNO_ಮೌಲ್ಯ(error) == ENOMEM)
                ಹಿಂತಿರುಗಿ log_oom_ಆಂತರಿಕ(LOG_ERR, file, ಸಾಲು, func);

        ಆದರೆ (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ಆದರೆ (!escaped)
                        rvalue = "(oom)";
                ಇಲ್ಲದಿದ್ದರೆ
                        rvalue = " (escaped)";
        }

        log_syntax_ಆಂತರಿಕ(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_ಸಾಲು, error,
                            file, ಸಾಲು, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ಹಿಂತಿರುಗಿ critical ? -ERRNO_ಮೌಲ್ಯ(error) : 0;
}

ಶೂನ್ಯ log_ಗಣ_upgrade_syslog_ಗಮ್ಯ_journal(bool b) {
        upgrade_syslog_ಗಮ್ಯ_journal = b;

        /* Make the change effective immediately */
        ಆದರೆ (b) {
                ಆದರೆ (log_ಗುರಿ == LOG_ಗುರಿ_SYSLOG)
                        log_ಗುರಿ = LOG_ಗುರಿ_JOURNAL;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (log_ಗುರಿ == LOG_ಗುರಿ_SYSLOG_OR_KMSG)
                        log_ಗುರಿ = LOG_ಗುರಿ_JOURNAL_OR_KMSG;
        }
}

ಶೂನ್ಯ log_ಗಣ_always_reopen_console(bool b) {
        always_reopen_console = b;
}

ಶೂನ್ಯ log_ಗಣ_open_when_needed(bool b) {
        open_when_needed = b;
}

ಶೂನ್ಯ log_ಗಣ_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

ಪೂರ್ಣಾಂಕ log_emergency_level(ಶೂನ್ಯ) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ಹಿಂತಿರುಗಿ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

ಪೂರ್ಣಾಂಕ log_dup_console(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ಆದರೆ (console_fd < 0 || console_fd >= 3)
                ಹಿಂತಿರುಗಿ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ಆದರೆ (copy < 0)
                ಹಿಂತಿರುಗಿ -errno;

        console_fd = copy;
        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ log_ಸಿದ್ಧತೆ(ಶೂನ್ಯ) {
        log_ಗಣ_ಗುರಿ(LOG_ಗುರಿ_AUTO);
        log_parse_environment();
        (ಶೂನ್ಯ) log_open();
        ಆದರೆ (log_on_console() && show_color < 0)
                log_show_color(true);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* log_ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, bool force) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *old = log_ಪೂರ್ವಪ್ರತ್ಯಯ;

        ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ || force)
                log_ಪೂರ್ವಪ್ರತ್ಯಯ = ಪೂರ್ವಪ್ರತ್ಯಯ;

        ಹಿಂತಿರುಗಿ old;
}
