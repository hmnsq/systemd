/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/signalfd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/uio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-messages.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ansi-color.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "argv-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log-context.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc-cmdline.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ratelimit.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "signal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socket-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "syslog-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "terminal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SNDBUF_ਮਾਪ (8*1024*1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IOVEC_MAX 256U

ਸਥਿਰ log_syntax_callback_t log_syntax_callback = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ *log_syntax_callback_userdata = ਨਿਰਾ;

ਸਥਿਰ Log_ਨਿਸ਼ਾਨਾ log_ਨਿਸ਼ਾਨਾ = LOG_ਨਿਸ਼ਾਨਾ_CONSOLE;
ਸਥਿਰ ਪੂਰਨ log_max_level = LOG_INFO;
ਸਥਿਰ ਪੂਰਨ log_ਨਿਸ਼ਾਨਾ_max_level[LOG_ਨਿਸ਼ਾਨਾ_SINGLE_MAX] = {
        [LOG_ਨਿਸ਼ਾਨਾ_CONSOLE] = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ,
        [LOG_ਨਿਸ਼ਾਨਾ_KMSG]    = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ,
        [LOG_ਨਿਸ਼ਾਨਾ_SYSLOG]  = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ,
        [LOG_ਨਿਸ਼ਾਨਾ_JOURNAL] = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ,
};
ਸਥਿਰ ਪੂਰਨ log_facility = LOG_DAEMON;
ਸਥਿਰ bool ratelimit_kmsg = true;

ਸਥਿਰ ਪੂਰਨ console_fd = STDERR_FILENO;
ਸਥਿਰ ਪੂਰਨ console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ਸਥਿਰ ਪੂਰਨ syslog_fd = -EBADF;
ਸਥਿਰ ਪੂਰਨ kmsg_fd = -EBADF;
ਸਥਿਰ ਪੂਰਨ journal_fd = -EBADF;

ਸਥਿਰ bool syslog_is_ਧਾਰਾ = false;

ਸਥਿਰ ਪੂਰਨ show_color = -1; /* tristate */
ਸਥਿਰ bool show_location = false;
ਸਥਿਰ bool show_ਸਮਾਂ = false;
ਸਥਿਰ bool show_tid = false;
ਸਥਿਰ bool show_executable = false;

ਸਥਿਰ bool upgrade_syslog_ਮੰਜ਼ਿਲ_journal = false;
ਸਥਿਰ bool always_reopen_console = false;
ਸਥਿਰ bool open_when_needed = false;
ਸਥਿਰ bool prohibit_ipc = false;

ਸਥਿਰ thread_local ਅਚਲ ਅੱਖਰ *log_ਅਗੇਤਰ = ਨਿਰਾ;

#ਜੇ LOG_ਸੁਨੇਹਾ_VERIFICATION || defined(__COVERITY__)
bool log_ਸੁਨੇਹਾ_dummy = false; /* Always false */
#ਜੇ_ਅੰਤ

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ assert_raw(ਵਿਅੰਜਕ)                                                \
        ਕਰੋ {                                                            \
                ਜੇ (_unlikely_(!(ਵਿਅੰਜਕ))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ਜਦੋਂ (false)

ਖਾਲੀ log_ਅਗੇਤਰ_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ ਅੱਖਰ **ਅਗੇਤਰ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

        ਅਦਲਾ_ਬਦਲੀ_TWO(log_ਅਗੇਤਰ, *ਅਗੇਤਰ);
}

ਸਥਿਰ ਖਾਲੀ log_close_console(ਖਾਲੀ) {
        /* See comment in log_close_journal() */
        (ਖਾਲੀ) ਸੁਰੱਖਿਅਤ_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ਸਥਿਰ ਪੂਰਨ log_open_console(ਖਾਲੀ) {

        ਜੇ (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ਵਾਪਸ 0;
        }

        ਜੇ (console_fd < 3) {
                ਪੂਰਨ fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ਜੇ (fd < 0)
                        ਵਾਪਸ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ log_close_kmsg(ਖਾਲੀ) {
        /* See comment in log_close_journal() */
        (ਖਾਲੀ) ਸੁਰੱਖਿਅਤ_close(TAKE_FD(kmsg_fd));
}

ਸਥਿਰ ਪੂਰਨ log_open_kmsg(ਖਾਲੀ) {

        ਜੇ (kmsg_fd >= 0)
                ਵਾਪਸ 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ਜੇ (kmsg_fd < 0)
                ਵਾਪਸ -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ log_close_syslog(ਖਾਲੀ) {
        /* See comment in log_close_journal() */
        (ਖਾਲੀ) ਸੁਰੱਖਿਅਤ_close(TAKE_FD(syslog_fd));
}

ਸਥਿਰ ਪੂਰਨ create_log_socket(ਪੂਰਨ type) {
        ਬਣਤਰ timeval tv;
        ਪੂਰਨ fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        fd = fd_move_above_stdio(fd);
        (ਖਾਲੀ) fd_inc_sndbuf(fd, SNDBUF_ਮਾਪ);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ਜੇ (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_ਪ੍ਰਤੀ_MSEC);
        ਨਹੀਂਤਾਂ
                timeval_store(&tv, 10 * USEC_ਪ੍ਰਤੀ_SEC);
        (ਖਾਲੀ) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, ਆਕਾਰ(tv));

        ਵਾਪਸ fd;
}

ਸਥਿਰ ਪੂਰਨ log_open_syslog(ਖਾਲੀ) {
        ਪੂਰਨ r;

        ਜੇ (syslog_fd >= 0)
                ਵਾਪਸ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ਜੇ (syslog_fd < 0) {
                r = syslog_fd;
                ਜਾਓ fail;
        }

        r = connect_unix_ਰਸਤਾ(syslog_fd, AT_FDCWD, "/dev/log");
        ਜੇ (r < 0) {
                ਸੁਰੱਖਿਅਤ_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ਜੇ (syslog_fd < 0) {
                        r = syslog_fd;
                        ਜਾਓ fail;
                }

                r = connect_unix_ਰਸਤਾ(syslog_fd, AT_FDCWD, "/dev/log");
                ਜੇ (r < 0)
                        ਜਾਓ fail;

                syslog_is_ਧਾਰਾ = true;
        } ਨਹੀਂਤਾਂ
                syslog_is_ਧਾਰਾ = false;

        ਵਾਪਸ 0;

fail:
        log_close_syslog();
        ਵਾਪਸ r;
}

ਸਥਿਰ ਖਾਲੀ log_close_journal(ਖਾਲੀ) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (ਖਾਲੀ) ਸੁਰੱਖਿਅਤ_close(TAKE_FD(journal_fd));
}

ਸਥਿਰ ਪੂਰਨ log_open_journal(ਖਾਲੀ) {
        ਪੂਰਨ r;

        ਜੇ (journal_fd >= 0)
                ਵਾਪਸ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ਜੇ (journal_fd < 0) {
                r = journal_fd;
                ਜਾਓ fail;
        }

        r = connect_unix_ਰਸਤਾ(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ਜੇ (r < 0)
                ਜਾਓ fail;

        ਵਾਪਸ 0;

fail:
        log_close_journal();
        ਵਾਪਸ r;
}

bool stderr_is_journal(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *w = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e;
        uint64_t dev, ino;
        ਬਣਤਰ stat st;

        e = getenv("JOURNAL_STREAM");
        ਜੇ (!e)
                ਵਾਪਸ false;

        ਜੇ (ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&e, &w, ":", ਕੱਢੋ_DONT_COALESCE_SEPARATORS) <= 0)
                ਵਾਪਸ false;
        ਜੇ (!e)
                ਵਾਪਸ false;

        ਜੇ (ਸੁਰੱਖਿਅਤ_atou64(w, &dev) < 0)
                ਵਾਪਸ false;
        ਜੇ (ਸੁਰੱਖਿਅਤ_atou64(e, &ino) < 0)
                ਵਾਪਸ false;

        ਜੇ (fstat(STDERR_FILENO, &st) < 0)
                ਵਾਪਸ false;

        ਵਾਪਸ st.st_dev == dev && st.st_ino == ino;
}

ਪੂਰਨ log_open(ਖਾਲੀ) {
        ਪੂਰਨ r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ਜੇ (log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                ਵਾਪਸ 0;
        }

        ਜੇ (getpid_cached() == 1 ||
            stderr_is_journal() ||
            ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ,
                   LOG_ਨਿਸ਼ਾਨਾ_KMSG,
                   LOG_ਨਿਸ਼ਾਨਾ_JOURNAL,
                   LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                   LOG_ਨਿਸ਼ਾਨਾ_SYSLOG,
                   LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG)) {

                ਜੇ (!prohibit_ipc) {
                        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ,
                                   LOG_ਨਿਸ਼ਾਨਾ_AUTO,
                                   LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                                   LOG_ਨਿਸ਼ਾਨਾ_JOURNAL)) {

                                r = log_open_journal();
                                ਜੇ (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        ਵਾਪਸ r;
                                }
                        }

                        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ,
                                   LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG,
                                   LOG_ਨਿਸ਼ਾਨਾ_SYSLOG)) {

                                r = log_open_syslog();
                                ਜੇ (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ਵਾਪਸ r;
                                }
                        }
                }

                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ, LOG_ਨਿਸ਼ਾਨਾ_AUTO,
                                       LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                                       LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG,
                                       LOG_ਨਿਸ਼ਾਨਾ_KMSG)) {
                        r = log_open_kmsg();
                        ਜੇ (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                ਵਾਪਸ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ਵਾਪਸ log_open_console();
}

ਖਾਲੀ log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ < LOG_ਨਿਸ਼ਾਨਾ_MAX);

        ਜੇ (upgrade_syslog_ਮੰਜ਼ਿਲ_journal) {
                ਜੇ (ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_SYSLOG)
                        ਨਿਸ਼ਾਨਾ = LOG_ਨਿਸ਼ਾਨਾ_JOURNAL;
                ਨਹੀਂਤਾਂ ਜੇ (ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG)
                        ਨਿਸ਼ਾਨਾ = LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG;
        }

        log_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ;
}

ਖਾਲੀ log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_and_open(Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ) {
        log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(ਨਿਸ਼ਾਨਾ);
        log_open();
}

ਖਾਲੀ log_close(ਖਾਲੀ) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

ਖਾਲੀ log_forget_fds(ਖਾਲੀ) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

ਪੂਰਨ log_ਸੈੱਟ_max_level(ਪੂਰਨ level) {
        ਪ੍ਰਤਿਜ੍ਞਾ(level == LOG_NULL || log_level_is_valid(level));

        ਪੂਰਨ old = log_max_level;
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

        ਵਾਪਸ old;
}

ਖਾਲੀ log_ਸੈੱਟ_facility(ਪੂਰਨ facility) {
        log_facility = facility;
}

ਸਥਿਰ bool check_console_fd_is_tty(ਖਾਲੀ) {
        ਜੇ (console_fd < 0)
                ਵਾਪਸ false;

        ਜੇ (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_ਸੁਰੱਖਿਅਤ(console_fd);

        ਵਾਪਸ console_fd_is_tty;
}

ਸਥਿਰ ਪੂਰਨ write_ਮੰਜ਼ਿਲ_console(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *ਬਫਰ) {

        ਸਥਿਰ ਪੂਰਨ dumb = -1;

        ਅੱਖਰ location[256],
             ਸਿਰਲੇਖ_ਸਮਾਂ[FORMAT_TIMESTAMP_MAX],
             ਅਗੇਤਰ[1 + DECIMAL_STR_MAX(ਪੂਰਨ) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ਬਣਤਰ iovec iovec[13];
        ਅਚਲ ਅੱਖਰ *on = ਨਿਰਾ, *off = ਨਿਰਾ;
        size_t n = 0;

        ਜੇ (console_fd < 0)
                ਵਾਪਸ 0;

        ਜੇ (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ਜੇ (LOG_PRI(level) > log_ਨਿਸ਼ਾਨਾ_max_level[LOG_ਨਿਸ਼ਾਨਾ_CONSOLE])
                ਵਾਪਸ 0;

        ਜੇ (log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_CONSOLE_PREFIXED) {
                xsprintf(ਅਗੇਤਰ, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(ਅਗੇਤਰ);
        }

        ਜੇ (show_ਸਮਾਂ &&
            format_timestamp(ਸਿਰਲੇਖ_ਸਮਾਂ, ਆਕਾਰ(ਸਿਰਲੇਖ_ਸਮਾਂ), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(ਸਿਰਲੇਖ_ਸਮਾਂ);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ਜੇ (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(ਪ੍ਰੋਗਰਾਮ_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ਜੇ (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ਜੇ (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ਨਿਰਾ);

        ਜੇ (show_location) {
                ਅਚਲ ਅੱਖਰ *lon = "", *loff = "";
                ਜੇ (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_ਸਾਧਾਰਨ();
                }

                (ਖਾਲੀ) snprintf(location, ਆਕਾਰ location, "%s%s:%i%s: ", lon, file, ਸਤਰ, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ਜੇ (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ਜੇ (log_ਅਗੇਤਰ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_ਅਗੇਤਰ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(ਬਫਰ);
        ਜੇ (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ਜੇ (writev(console_fd, iovec, n) < 0) {

                ਜੇ (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (ਖਾਲੀ) log_open_console();
                        ਜੇ (console_fd < 0)
                                ਵਾਪਸ 0;

                        ਜੇ (writev(console_fd, iovec, n) < 0)
                                ਵਾਪਸ -errno;
                } ਨਹੀਂਤਾਂ
                        ਵਾਪਸ -errno;
        }

        ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ write_ਮੰਜ਼ਿਲ_syslog(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *ਬਫਰ) {

        ਅੱਖਰ ਸਿਰਲੇਖ_ਤਰਜੀਹ[2 + DECIMAL_STR_MAX(ਪੂਰਨ) + 1],
             ਸਿਰਲੇਖ_ਸਮਾਂ[64],
             ਸਿਰਲੇਖ_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ਬਣਤਰ tm tm;
        ਪੂਰਨ r;

        ਜੇ (syslog_fd < 0)
                ਵਾਪਸ 0;

        ਜੇ (LOG_PRI(level) > log_ਨਿਸ਼ਾਨਾ_max_level[LOG_ਨਿਸ਼ਾਨਾ_SYSLOG])
                ਵਾਪਸ 0;

        xsprintf(ਸਿਰਲੇਖ_ਤਰਜੀਹ, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (strftime(ਸਿਰਲੇਖ_ਸਮਾਂ, ਆਕਾਰ(ਸਿਰਲੇਖ_ਸਮਾਂ), "%h %e %T ", &tm) <= 0)
                ਵਾਪਸ -EINVAL;

        xsprintf(ਸਿਰਲੇਖ_pid, "["PID_FMT"]: ", getpid_cached());

        ਬਣਤਰ iovec iovec[] = {
                IOVEC_MAKE_STRING(ਸਿਰਲੇਖ_ਤਰਜੀਹ),
                IOVEC_MAKE_STRING(ਸਿਰਲੇਖ_ਸਮਾਂ),
                IOVEC_MAKE_STRING(ਪ੍ਰੋਗਰਾਮ_invocation_short_name),
                IOVEC_MAKE_STRING(ਸਿਰਲੇਖ_pid),
                IOVEC_MAKE_STRING(strempty(log_ਅਗੇਤਰ)),
                IOVEC_MAKE_STRING(log_ਅਗੇਤਰ ? ": " : ""),
                IOVEC_MAKE_STRING(ਬਫਰ),
        };
        ਅਚਲ ਬਣਤਰ msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        ਜੇ (syslog_is_ਧਾਰਾ)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        ਲਈ (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (!syslog_is_ਧਾਰਾ)
                        ਰੋਕੋ;

                ਜੇ (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        ਰੋਕੋ;
        }

        ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ write_ਮੰਜ਼ਿਲ_kmsg(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *ਬਫਰ) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ਸਥਿਰ thread_local ਦਰ_ਹੱਦਬੰਦੀ ratelimit = { 5 * USEC_ਪ੍ਰਤੀ_SEC, 200 };

        ਅੱਖਰ ਸਿਰਲੇਖ_ਤਰਜੀਹ[2 + DECIMAL_STR_MAX(ਪੂਰਨ) + 1],
             ਸਿਰਲੇਖ_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ਜੇ (kmsg_fd < 0)
                ਵਾਪਸ 0;

        ਜੇ (LOG_PRI(level) > log_ਨਿਸ਼ਾਨਾ_max_level[LOG_ਨਿਸ਼ਾਨਾ_KMSG])
                ਵਾਪਸ 0;

        ਜੇ (ratelimit_kmsg && !ratelimit_ਹੇਠਾਂ(&ratelimit)) {
                ਜੇ (ratelimit_ਅੰਕ_dropped(&ratelimit) > 1)
                        ਵਾਪਸ 0;

                ਬਫਰ = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(ਸਿਰਲੇਖ_ਤਰਜੀਹ, "<%i>", level);
        xsprintf(ਸਿਰਲੇਖ_pid, "["PID_FMT"]: ", getpid_cached());

        ਅਚਲ ਬਣਤਰ iovec iovec[] = {
                IOVEC_MAKE_STRING(ਸਿਰਲੇਖ_ਤਰਜੀਹ),
                IOVEC_MAKE_STRING(ਪ੍ਰੋਗਰਾਮ_invocation_short_name),
                IOVEC_MAKE_STRING(ਸਿਰਲੇਖ_pid),
                IOVEC_MAKE_STRING(strempty(log_ਅਗੇਤਰ)),
                IOVEC_MAKE_STRING(log_ਅਗੇਤਰ ? ": " : ""),
                IOVEC_MAKE_STRING(ਬਫਰ),
                IOVEC_MAKE_STRING("\n"),
        };

        ਜੇ (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ log_do_ਸਿਰਲੇਖ(
                ਅੱਖਰ *ਸਿਰਲੇਖ,
                size_t ਮਾਪ,
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *extra) {
        ਪੂਰਨ r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_ਮੁੱਲ(error);

        r = snprintf(ਸਿਰਲੇਖ, ਮਾਪ,
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
                     ਸਤਰ ? "CODE_LINE=" : "",
                     ਸਤਰ ? 1 : 0, ਸਤਰ, /* %.0d means no output too, special case for 0 */
                     ਸਤਰ ? "\n" : "",
                     isempty(func) ? "" : "CODE_FUNC=",
                     isempty(func) ? "" : func,
                     isempty(func) ? "" : "\n",
                     error ? "ERRNO=" : "",
                     error ? 1 : 0, error,
                     error ? "\n" : "",
                     isempty(object) ? "" : ASSERT_PTR(object_ਖੇਤਰ),
                     isempty(object) ? "" : object,
                     isempty(object) ? "" : "\n",
                     isempty(extra) ? "" : ASSERT_PTR(extra_ਖੇਤਰ),
                     isempty(extra) ? "" : extra,
                     isempty(extra) ? "" : "\n",
                     ਪ੍ਰੋਗਰਾਮ_invocation_short_name);
        assert_raw((size_t) r < ਮਾਪ);

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ log_do_context(ਬਣਤਰ iovec *iovec, size_t iovec_len, size_t *n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);
        ਪ੍ਰਤਿਜ੍ਞਾ(n);

        LIST_FOREACH(ll, c, log_context_ਸਿਰ()) {
                STRV_FOREACH(s, c->fields) {
                        ਜੇ (*n + 2 >= iovec_len)
                                ਵਾਪਸ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ਲਈ (size_t i = 0; i < c->n_ਨਿਵੇਸ਼_iovec; i++) {
                        ਜੇ (*n + 2 >= iovec_len)
                                ਵਾਪਸ;

                        iovec[(*n)++] = c->ਨਿਵੇਸ਼_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ਜੇ (c->ਕੁੰਜੀ && c->ਮੁੱਲ) {
                        ਜੇ (*n + 3 >= iovec_len)
                                ਵਾਪਸ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ਕੁੰਜੀ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ਮੁੱਲ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ਸਥਿਰ ਪੂਰਨ write_ਮੰਜ਼ਿਲ_journal(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅਚਲ ਅੱਖਰ *ਬਫਰ) {

        ਅੱਖਰ ਸਿਰਲੇਖ[LINE_MAX];
        size_t n = 0, iovec_len;
        ਬਣਤਰ iovec *iovec;

        ਜੇ (journal_fd < 0)
                ਵਾਪਸ 0;

        ਜੇ (LOG_PRI(level) > log_ਨਿਸ਼ਾਨਾ_max_level[LOG_ਨਿਸ਼ਾਨਾ_JOURNAL])
                ਵਾਪਸ 0;

        ਜੇ (!MUL_ਸੁਰੱਖਿਅਤ(&iovec_len, log_context_ਅੰਕ_fields_2(), 3) ||
            !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        ਨਹੀਂਤਾਂ
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ਬਣਤਰ iovec, iovec_len);

        log_do_ਸਿਰਲੇਖ(ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ), level, error, file, ਸਤਰ, func, object_ਖੇਤਰ, object, extra_ਖੇਤਰ, extra);

        iovec[n++] = IOVEC_MAKE_STRING(ਸਿਰਲੇਖ);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ਜੇ (log_ਅਗੇਤਰ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_ਅਗੇਤਰ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(ਬਫਰ);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ਅਚਲ ਬਣਤਰ msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        ਜੇ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 1;
}

ਪੂਰਨ log_dispatch_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅੱਖਰ *ਬਫਰ) {

        assert_raw(ਬਫਰ);

        ਜੇ (log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_NULL)
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ਜੇ (LOG_FAC(level) == 0)
                level |= log_facility;

        ਜੇ (open_when_needed)
                (ਖਾਲੀ) log_open();

        ਕਰੋ {
                ਅੱਖਰ *e;
                ਪੂਰਨ k = 0;

                ਬਫਰ += strspn(ਬਫਰ, NEWLINE);

                ਜੇ (ਬਫਰ[0] == 0)
                        ਰੋਕੋ;

                ਜੇ ((e = strpbrk(ਬਫਰ, NEWLINE)))
                        *(e++) = 0;

                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ, LOG_ਨਿਸ਼ਾਨਾ_AUTO,
                                       LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                                       LOG_ਨਿਸ਼ਾਨਾ_JOURNAL)) {

                        k = write_ਮੰਜ਼ਿਲ_journal(level, error, file, ਸਤਰ, func, object_ਖੇਤਰ, object, extra_ਖੇਤਰ, extra, ਬਫਰ);
                        ਜੇ (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ, LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG,
                                       LOG_ਨਿਸ਼ਾਨਾ_SYSLOG)) {

                        k = write_ਮੰਜ਼ਿਲ_syslog(level, error, file, ਸਤਰ, func, ਬਫਰ);
                        ਜੇ (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ਜੇ (k <= 0 &&
                    ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ, LOG_ਨਿਸ਼ਾਨਾ_AUTO,
                                       LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG,
                                       LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                                       LOG_ਨਿਸ਼ਾਨਾ_KMSG)) {

                        ਜੇ (k < 0)
                                log_open_kmsg();

                        k = write_ਮੰਜ਼ਿਲ_kmsg(level, error, file, ਸਤਰ, func, ਬਫਰ);
                        ਜੇ (k < 0) {
                                log_close_kmsg();
                                (ਖਾਲੀ) log_open_console();
                        }
                }

                ਜੇ (k <= 0)
                        (ਖਾਲੀ) write_ਮੰਜ਼ਿਲ_console(level, error, file, ਸਤਰ, func, ਬਫਰ);

                ਬਫਰ = e;
        } ਜਦੋਂ (ਬਫਰ);

        ਜੇ (open_when_needed)
                log_close();

        ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);
}

ਪੂਰਨ log_dump_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅੱਖਰ *ਬਫਰ) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ਜੇ (_likely_(LOG_PRI(level) > log_max_level))
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        ਵਾਪਸ log_dispatch_ਅੰਦਰੂਨੀ(level, error, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਬਫਰ);
}

ਪੂਰਨ log_internalv(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format,
                va_list ap) {

        ਜੇ (_likely_(LOG_PRI(level) > log_max_level))
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        ਅੱਖਰ ਬਫਰ[LINE_MAX];
        LOCAL_ERRNO(ERRNO_ਮੁੱਲ(error));

        (ਖਾਲੀ) vsnprintf(ਬਫਰ, ਆਕਾਰ ਬਫਰ, format, ap);

        ਵਾਪਸ log_dispatch_ਅੰਦਰੂਨੀ(level, error, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਬਫਰ);
}

ਪੂਰਨ log_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format, ...) {

        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = log_internalv(level, error, file, ਸਤਰ, func, format, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ log_object_internalv(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅਚਲ ਅੱਖਰ *format,
                va_list ap) {

        ਅੱਖਰ *ਬਫਰ, *b;

        ਜੇ (_likely_(LOG_PRI(level) > log_max_level))
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_ਮੁੱਲ(error));

        LOG_ਸੈੱਟ_ਅਗੇਤਰ(object);

        b = ਬਫਰ = newa(ਅੱਖਰ, LINE_MAX);
        (ਖਾਲੀ) vsnprintf(b, LINE_MAX, format, ap);

        ਵਾਪਸ log_dispatch_ਅੰਦਰੂਨੀ(level, error, file, ਸਤਰ, func,
                                     object_ਖੇਤਰ, object, extra_ਖੇਤਰ, extra, ਬਫਰ);
}

ਪੂਰਨ log_object_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅਚਲ ਅੱਖਰ *format, ...) {

        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, ਸਤਰ, func, object_ਖੇਤਰ, object, extra_ਖੇਤਰ, extra, format, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ log_oom_ਅੰਦਰੂਨੀ(ਪੂਰਨ level, ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *func) {
        ਵਾਪਸ log_ਅੰਦਰੂਨੀ(level, ENOMEM, file, ਸਤਰ, func, "Out of memory.");
}

ਪੂਰਨ log_format_iovec(
                ਬਣਤਰ iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *format,
                va_list ap) {

        ਪ੍ਰਤਿਜ੍ਞਾ(iovec);
        ਪ੍ਰਤਿਜ੍ਞਾ(n);

        ਜਦੋਂ (format && *n + 1 < iovec_len) {
                va_list aq;
                ਅੱਖਰ *m;
                ਪੂਰਨ r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_ਮੁੱਲ(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ਜੇ (r < 0)
                        ਵਾਪਸ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ਜੇ (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, ਅੱਖਰ *);
        }
        ਵਾਪਸ 0;
}

ਪੂਰਨ log_struct_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format, ...) {

        ਅੱਖਰ buf[LINE_MAX];
        bool ਮਿਲਿਆ = false;
        PROTECT_ERRNO;
        va_list ap;

        ਜੇ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_NULL)
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        ਜੇ (LOG_FAC(level) == 0)
                level |= log_facility;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ,
                   LOG_ਨਿਸ਼ਾਨਾ_AUTO,
                   LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                   LOG_ਨਿਸ਼ਾਨਾ_JOURNAL)) {

                ਜੇ (open_when_needed)
                        log_open_journal();

                ਜੇ (journal_fd >= 0) {
                        ਅੱਖਰ ਸਿਰਲੇਖ[LINE_MAX];
                        ਬਣਤਰ iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        ਪੂਰਨ r;
                        bool fallback = false;

                        ਜੇ (!MUL_ਸੁਰੱਖਿਅਤ(&iovec_len, log_context_ਅੰਕ_fields_2(), 3) ||
                            !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        ਨਹੀਂਤਾਂ
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ਬਣਤਰ iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_ਸਿਰਲੇਖ(ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ), level, error, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
                        iovec[n++] = IOVEC_MAKE_STRING(ਸਿਰਲੇਖ);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        ਜੇ (r < 0)
                                fallback = true;
                        ਨਹੀਂਤਾਂ {
                                log_do_context(iovec, iovec_len, &n);

                                ਅਚਲ ਬਣਤਰ msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (ਖਾਲੀ) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        ਲਈ (size_t i = 1; i < m; i += 2)
                                ਮੁਕਤ(iovec[i].iov_base);

                        ਜੇ (!fallback) {
                                ਜੇ (open_when_needed)
                                        log_close();

                                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ਜਦੋਂ (format) {
                va_list aq;

                errno = ERRNO_ਮੁੱਲ(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (ਖਾਲੀ) vsnprintf(buf, ਆਕਾਰ buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ਜੇ (startswith(buf, "MESSAGE=")) {
                        ਮਿਲਿਆ = true;
                        ਰੋਕੋ;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, ਅੱਖਰ *);
        }
        va_end(ap);

        ਜੇ (!ਮਿਲਿਆ) {
                ਜੇ (open_when_needed)
                        log_close();

                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);
        }

        ਵਾਪਸ log_dispatch_ਅੰਦਰੂਨੀ(level, error, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, buf + 8);
}

ਪੂਰਨ log_struct_iovec_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਬਣਤਰ iovec ਨਿਵੇਸ਼_iovec[],
                size_t n_ਨਿਵੇਸ਼_iovec) {

        PROTECT_ERRNO;

        ਜੇ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_NULL)
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        ਜੇ (LOG_FAC(level) == 0)
                level |= log_facility;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ, LOG_ਨਿਸ਼ਾਨਾ_AUTO,
                               LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
                               LOG_ਨਿਸ਼ਾਨਾ_JOURNAL) &&
            journal_fd >= 0) {

                ਅੱਖਰ ਸਿਰਲੇਖ[LINE_MAX];
                ਬਣਤਰ iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_ਨਿਵੇਸ਼_iovec_expanded;

                ਜੇ (!MUL_ਸੁਰੱਖਿਅਤ(&n_ਨਿਵੇਸ਼_iovec_expanded, n_ਨਿਵੇਸ਼_iovec, 2) ||
                    !MUL_ਸੁਰੱਖਿਅਤ(&n_context_iovec, log_context_ਅੰਕ_fields_2(), 3) ||
                    !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&iovec_len, 1, n_ਨਿਵੇਸ਼_iovec_expanded) ||
                    !ਜੋੜੋ_ਸੁਰੱਖਿਅਤ(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                ਨਹੀਂਤਾਂ
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ਬਣਤਰ iovec, iovec_len);

                log_do_ਸਿਰਲੇਖ(ਸਿਰਲੇਖ, ਆਕਾਰ(ਸਿਰਲੇਖ), level, error, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

                iovec[n++] = IOVEC_MAKE_STRING(ਸਿਰਲੇਖ);
                ਲਈ (size_t i = 0; i < n_ਨਿਵੇਸ਼_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = ਨਿਵੇਸ਼_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ਅਚਲ ਬਣਤਰ msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ਜੇ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);
        }

        ਲਈ (size_t i = 0; i < n_ਨਿਵੇਸ਼_iovec; i++)
                ਜੇ (ਸਮ੍ਰਿਤੀ_startswith(ਨਿਵੇਸ਼_iovec[i].iov_base, ਨਿਵੇਸ਼_iovec[i].iov_len, "MESSAGE=")) {
                        ਅੱਖਰ *m;

                        m = strndupa_ਸੁਰੱਖਿਅਤ((ਅੱਖਰ*) ਨਿਵੇਸ਼_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          ਨਿਵੇਸ਼_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ਵਾਪਸ log_dispatch_ਅੰਦਰੂਨੀ(level, error, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, m);
                }

        /* Couldn't find MESSAGE=. */
        ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);
}

ਪੂਰਨ log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_from_string(ਅਚਲ ਅੱਖਰ *e) {
        Log_ਨਿਸ਼ਾਨਾ t;

        t = log_ਨਿਸ਼ਾਨਾ_from_string(e);
        ਜੇ (t < 0)
                ਵਾਪਸ t;

        log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(t);
        ਵਾਪਸ 0;
}

ਪੂਰਨ log_ਸੈੱਟ_max_level_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸ਼ਬਦ = ਨਿਰਾ, *ਅਗੇਤਰ = ਨਿਰਾ;
                Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ;
                ਅਚਲ ਅੱਖਰ *colon;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&e, &ਸ਼ਬਦ, ",", 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                colon = strchr(ਸ਼ਬਦ, ':');
                ਜੇ (!colon) {
                        r = log_level_from_string(ਸ਼ਬਦ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        log_ਸੈੱਟ_max_level(r);
                        ਜਾਰੀ;
                }

                ਅਗੇਤਰ = strndup(ਸ਼ਬਦ, colon - ਸ਼ਬਦ);
                ਜੇ (!ਅਗੇਤਰ)
                        ਵਾਪਸ -ENOMEM;

                ਨਿਸ਼ਾਨਾ = log_ਨਿਸ਼ਾਨਾ_from_string(ਅਗੇਤਰ);
                ਜੇ (ਨਿਸ਼ਾਨਾ < 0)
                        ਵਾਪਸ ਨਿਸ਼ਾਨਾ;

                ਜੇ (ਨਿਸ਼ਾਨਾ >= LOG_ਨਿਸ਼ਾਨਾ_SINGLE_MAX)
                        ਵਾਪਸ -EINVAL;

                r = log_level_from_string(colon + 1);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                log_ਨਿਸ਼ਾਨਾ_max_level[ਨਿਸ਼ਾਨਾ] = r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ log_max_ਪੱਧਰ_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ level, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = log_level_ਮੰਜ਼ਿਲ_string_alloc(level, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ = 0; ਨਿਸ਼ਾਨਾ < LOG_ਨਿਸ਼ਾਨਾ_SINGLE_MAX; ਨਿਸ਼ਾਨਾ++) {
                _cleanup_free_ ਅੱਖਰ *l = ਨਿਰਾ;

                ਜੇ (log_ਨਿਸ਼ਾਨਾ_max_level[ਨਿਸ਼ਾਨਾ] == ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                        ਜਾਰੀ;

                r = log_level_ਮੰਜ਼ਿਲ_string_alloc(log_ਨਿਸ਼ਾਨਾ_max_level[ਨਿਸ਼ਾਨਾ], &l);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_ਨਿਸ਼ਾਨਾ_ਮੰਜ਼ਿਲ_string(ਨਿਸ਼ਾਨਾ), l);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ log_ਸੈੱਟ_ratelimit_kmsg_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        r = parse_boolean(e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ratelimit_kmsg = r;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ parse_proc_cmdline_ਵਸਤੂ(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, ਖਾਲੀ *ਅੰਕੜੇ) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ਜੇ (streq(ਕੁੰਜੀ, "debug") && !ਮੁੱਲ)
                log_ਸੈੱਟ_max_level(LOG_DEBUG);

        ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_target")) {

                ਜੇ (proc_cmdline_ਮੁੱਲ_missing(ਕੁੰਜੀ, ਮੁੱਲ))
                        ਵਾਪਸ 0;

                ਜੇ (log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_from_string(ਮੁੱਲ) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", ਮੁੱਲ);

        } ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_level")) {

                ਜੇ (proc_cmdline_ਮੁੱਲ_missing(ਕੁੰਜੀ, ਮੁੱਲ))
                        ਵਾਪਸ 0;

                ਜੇ (log_ਸੈੱਟ_max_level_from_string(ਮੁੱਲ) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", ਮੁੱਲ);

        } ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_color")) {

                ਜੇ (log_show_color_from_string(ਮੁੱਲ ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", ਮੁੱਲ);

        } ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_location")) {

                ਜੇ (log_show_location_from_string(ਮੁੱਲ ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", ਮੁੱਲ);

        } ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_tid")) {

                ਜੇ (log_show_tid_from_string(ਮੁੱਲ ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", ਮੁੱਲ);

        } ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_time")) {

                ਜੇ (log_show_ਸਮਾਂ_from_string(ਮੁੱਲ ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", ਮੁੱਲ);

        } ਨਹੀਂਤਾਂ ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਕੁੰਜੀ, "systemd.log_ratelimit_kmsg")) {

                ਜੇ (log_ਸੈੱਟ_ratelimit_kmsg_from_string(ਮੁੱਲ ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", ਮੁੱਲ);
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ bool should_parse_proc_cmdline(ਖਾਲੀ) {
        /* PID1 always reads the kernel command line. */
        ਜੇ (getpid_cached() == 1)
                ਵਾਪਸ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ਵਾਪਸ invoked_by_systemd();
}

ਖਾਲੀ log_parse_environment_variables(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *e;
        ਪੂਰਨ r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ਜੇ (e && log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ਜੇ (e) {
                r = log_ਸੈੱਟ_max_level_from_string(e);
                ਜੇ (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } ਨਹੀਂਤਾਂ {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ਜੇ (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                ਨਹੀਂਤਾਂ ਜੇ (r > 0)
                        log_ਸੈੱਟ_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ਜੇ (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ਜੇ (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ਜੇ (e && log_show_ਸਮਾਂ_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ਜੇ (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ਜੇ (e && log_ਸੈੱਟ_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ਜੇ (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

ਖਾਲੀ log_parse_environment(ਖਾਲੀ) {
        /* Do not call from library code. */

        ਜੇ (should_parse_proc_cmdline())
                (ਖਾਲੀ) proc_cmdline_parse(parse_proc_cmdline_ਵਸਤੂ, ਨਿਰਾ, PROC_CMDLINE_STRIP_RD_ਅਗੇਤਰ);

        log_parse_environment_variables();
}

Log_ਨਿਸ਼ਾਨਾ log_get_ਨਿਸ਼ਾਨਾ(ਖਾਲੀ) {
        ਵਾਪਸ log_ਨਿਸ਼ਾਨਾ;
}

ਖਾਲੀ log_settle_ਨਿਸ਼ਾਨਾ(ਖਾਲੀ) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        Log_ਨਿਸ਼ਾਨਾ t = log_get_ਨਿਸ਼ਾਨਾ();

        ਜੇ (t != LOG_ਨਿਸ਼ਾਨਾ_AUTO)
                ਵਾਪਸ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_ਨਿਸ਼ਾਨਾ_KMSG : LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG)
                                                        : LOG_ਨਿਸ਼ਾਨਾ_CONSOLE;
        log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(t);
}

ਪੂਰਨ log_get_max_level(ਖਾਲੀ) {
        ਵਾਪਸ log_max_level;
}

ਪੂਰਨ log_get_ਨਿਸ਼ਾਨਾ_max_level(Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ < LOG_ਨਿਸ਼ਾਨਾ_SINGLE_MAX);
        ਵਾਪਸ log_ਨਿਸ਼ਾਨਾ_max_level[ਨਿਸ਼ਾਨਾ];
}

ਖਾਲੀ log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(ਖਾਲੀ) {
        ਵਾਪਸ show_color > 0; /* Defaults to false. */
}

ਖਾਲੀ log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(ਖਾਲੀ) {
        ਵਾਪਸ show_location;
}

ਖਾਲੀ log_show_ਸਮਾਂ(bool b) {
        show_ਸਮਾਂ = b;
}

bool log_get_show_ਸਮਾਂ(ਖਾਲੀ) {
        ਵਾਪਸ show_ਸਮਾਂ;
}

ਖਾਲੀ log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(ਖਾਲੀ) {
        ਵਾਪਸ show_tid;
}

ਖਾਲੀ log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(ਖਾਲੀ) {
        ਵਾਪਸ show_executable;
}

ਪੂਰਨ log_show_color_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        r = parse_boolean(e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_show_color(r);
        ਵਾਪਸ 0;
}

ਪੂਰਨ log_show_location_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        r = parse_boolean(e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_show_location(r);
        ਵਾਪਸ 0;
}

ਪੂਰਨ log_show_ਸਮਾਂ_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        r = parse_boolean(e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_show_ਸਮਾਂ(r);
        ਵਾਪਸ 0;
}

ਪੂਰਨ log_show_tid_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        r = parse_boolean(e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_show_tid(r);
        ਵਾਪਸ 0;
}

ਪੂਰਨ log_show_executable_from_string(ਅਚਲ ਅੱਖਰ *e) {
        ਪੂਰਨ r;

        r = parse_boolean(e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        log_show_executable(r);
        ਵਾਪਸ 0;
}

bool log_on_console(ਖਾਲੀ) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(log_ਨਿਸ਼ਾਨਾ, LOG_ਨਿਸ਼ਾਨਾ_CONSOLE,
                               LOG_ਨਿਸ਼ਾਨਾ_CONSOLE_PREFIXED))
                ਵਾਪਸ true;

        ਵਾਪਸ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ log_ਨਿਸ਼ਾਨਾ_ਸਾਰਣੀ[LOG_ਨਿਸ਼ਾਨਾ_MAX] = {
        [LOG_ਨਿਸ਼ਾਨਾ_CONSOLE]          = "console",
        [LOG_ਨਿਸ਼ਾਨਾ_CONSOLE_PREFIXED] = "console-prefixed",
        [LOG_ਨਿਸ਼ਾਨਾ_KMSG]             = "kmsg",
        [LOG_ਨਿਸ਼ਾਨਾ_JOURNAL]          = "journal",
        [LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG]  = "journal-or-kmsg",
        [LOG_ਨਿਸ਼ਾਨਾ_SYSLOG]           = "syslog",
        [LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG]   = "syslog-or-kmsg",
        [LOG_ਨਿਸ਼ਾਨਾ_AUTO]             = "auto",
        [LOG_ਨਿਸ਼ਾਨਾ_NULL]             = "null",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(log_ਨਿਸ਼ਾਨਾ, Log_ਨਿਸ਼ਾਨਾ);

ਖਾਲੀ log_received_signal(ਪੂਰਨ level, ਅਚਲ ਬਣਤਰ signalfd_siginfo *si) {
        ਪ੍ਰਤਿਜ੍ਞਾ(si);

        ਜੇ (si_ਕੋਡ_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                (ਖਾਲੀ) pid_get_comm(si->ssi_pid, &p);

                log_ਭਰਿਆ(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_ਮੰਜ਼ਿਲ_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } ਨਹੀਂਤਾਂ
                log_ਭਰਿਆ(level,
                         "Received SIG%s.",
                         signal_ਮੰਜ਼ਿਲ_string(si->ssi_signo));
}

ਖਾਲੀ ਸੈੱਟ_log_syntax_callback(log_syntax_callback_t cb, ਖਾਲੀ *userdata) {
        ਪ੍ਰਤਿਜ੍ਞਾ(!log_syntax_callback || !cb);
        ਪ੍ਰਤਿਜ੍ਞਾ(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

ਪੂਰਨ log_syntax_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *unit,
                ਪੂਰਨ level,
                ਅਚਲ ਅੱਖਰ *config_file,
                ਬਿਨਾਨਿਸ਼ਾਨੀ config_ਸਤਰ,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format, ...) {

        PROTECT_ERRNO;

        ਜੇ (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ਜੇ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_NULL)
                ਵਾਪਸ -ERRNO_ਮੁੱਲ(error);

        ਅੱਖਰ ਬਫਰ[LINE_MAX];
        va_list ap;
        ਅਚਲ ਅੱਖਰ *unit_fmt = ਨਿਰਾ;

        errno = ERRNO_ਮੁੱਲ(error);

        va_start(ap, format);
        (ਖਾਲੀ) vsnprintf(ਬਫਰ, ਆਕਾਰ ਬਫਰ, format, ap);
        va_end(ap);

        ਜੇ (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ਜੇ (config_file) {
                ਜੇ (config_ਸਤਰ > 0)
                        ਵਾਪਸ log_struct_ਅੰਦਰੂਨੀ(
                                        level,
                                        error,
                                        file, ਸਤਰ, func,
                                        LOG_ਸੁਨੇਹਾ_ID(SD_ਸੁਨੇਹਾ_INVALID_CONFIGURATION_STR),
                                        LOG_ਵਸਤੂ("CONFIG_FILE=%s", config_file),
                                        LOG_ਵਸਤੂ("CONFIG_LINE=%u", config_ਸਤਰ),
                                        LOG_ਸੁਨੇਹਾ("%s:%u: %s", config_file, config_ਸਤਰ, ਬਫਰ),
                                        unit_fmt, unit,
                                        ਨਿਰਾ);
                ਨਹੀਂਤਾਂ
                        ਵਾਪਸ log_struct_ਅੰਦਰੂਨੀ(
                                        level,
                                        error,
                                        file, ਸਤਰ, func,
                                        LOG_ਸੁਨੇਹਾ_ID(SD_ਸੁਨੇਹਾ_INVALID_CONFIGURATION_STR),
                                        LOG_ਵਸਤੂ("CONFIG_FILE=%s", config_file),
                                        LOG_ਸੁਨੇਹਾ("%s: %s", config_file, ਬਫਰ),
                                        unit_fmt, unit,
                                        ਨਿਰਾ);
        } ਨਹੀਂਤਾਂ ਜੇ (unit)
                ਵਾਪਸ log_struct_ਅੰਦਰੂਨੀ(
                                level,
                                error,
                                file, ਸਤਰ, func,
                                LOG_ਸੁਨੇਹਾ_ID(SD_ਸੁਨੇਹਾ_INVALID_CONFIGURATION_STR),
                                LOG_ਸੁਨੇਹਾ("%s: %s", unit, ਬਫਰ),
                                unit_fmt, unit,
                                ਨਿਰਾ);
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ log_struct_ਅੰਦਰੂਨੀ(
                                level,
                                error,
                                file, ਸਤਰ, func,
                                LOG_ਸੁਨੇਹਾ_ID(SD_ਸੁਨੇਹਾ_INVALID_CONFIGURATION_STR),
                                LOG_ਸੁਨੇਹਾ("%s", ਬਫਰ),
                                ਨਿਰਾ);
}

ਪੂਰਨ log_syntax_invalid_utf8_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *unit,
                ਪੂਰਨ level,
                ਅਚਲ ਅੱਖਰ *config_file,
                ਬਿਨਾਨਿਸ਼ਾਨੀ config_ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

        ਜੇ (rvalue)
                p = utf8_escape_invalid(rvalue);

        ਵਾਪਸ log_syntax_ਅੰਦਰੂਨੀ(unit, level, config_file, config_ਸਤਰ,
                                   SYNTHETIC_ERRNO(EINVAL), file, ਸਤਰ, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

ਪੂਰਨ log_syntax_parse_error_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *unit,
                ਅਚਲ ਅੱਖਰ *config_file,
                ਬਿਨਾਨਿਸ਼ਾਨੀ config_ਸਤਰ,
                ਪੂਰਨ error,
                bool critical,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *lvalue,
                ਅਚਲ ਅੱਖਰ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ਅੱਖਰ *escaped = ਨਿਰਾ;

        /* OOM is always handled as critical. */
        ਜੇ (ERRNO_ਮੁੱਲ(error) == ENOMEM)
                ਵਾਪਸ log_oom_ਅੰਦਰੂਨੀ(LOG_ERR, file, ਸਤਰ, func);

        ਜੇ (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ਜੇ (!escaped)
                        rvalue = "(oom)";
                ਨਹੀਂਤਾਂ
                        rvalue = " (escaped)";
        }

        log_syntax_ਅੰਦਰੂਨੀ(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_ਸਤਰ, error,
                            file, ਸਤਰ, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ਵਾਪਸ critical ? -ERRNO_ਮੁੱਲ(error) : 0;
}

ਖਾਲੀ log_ਸੈੱਟ_upgrade_syslog_ਮੰਜ਼ਿਲ_journal(bool b) {
        upgrade_syslog_ਮੰਜ਼ਿਲ_journal = b;

        /* Make the change effective immediately */
        ਜੇ (b) {
                ਜੇ (log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_SYSLOG)
                        log_ਨਿਸ਼ਾਨਾ = LOG_ਨਿਸ਼ਾਨਾ_JOURNAL;
                ਨਹੀਂਤਾਂ ਜੇ (log_ਨਿਸ਼ਾਨਾ == LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG)
                        log_ਨਿਸ਼ਾਨਾ = LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG;
        }
}

ਖਾਲੀ log_ਸੈੱਟ_always_reopen_console(bool b) {
        always_reopen_console = b;
}

ਖਾਲੀ log_ਸੈੱਟ_open_when_needed(bool b) {
        open_when_needed = b;
}

ਖਾਲੀ log_ਸੈੱਟ_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

ਪੂਰਨ log_emergency_level(ਖਾਲੀ) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ਵਾਪਸ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

ਪੂਰਨ log_dup_console(ਖਾਲੀ) {
        ਪੂਰਨ copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ਜੇ (console_fd < 0 || console_fd >= 3)
                ਵਾਪਸ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ਜੇ (copy < 0)
                ਵਾਪਸ -errno;

        console_fd = copy;
        ਵਾਪਸ 0;
}

ਖਾਲੀ log_ਤਿਆਰੀ(ਖਾਲੀ) {
        log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(LOG_ਨਿਸ਼ਾਨਾ_AUTO);
        log_parse_environment();
        (ਖਾਲੀ) log_open();
        ਜੇ (log_on_console() && show_color < 0)
                log_show_color(true);
}

ਅਚਲ ਅੱਖਰ* log_ਸੈੱਟ_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, bool force) {
        ਅਚਲ ਅੱਖਰ *old = log_ਅਗੇਤਰ;

        ਜੇ (ਅਗੇਤਰ || force)
                log_ਅਗੇਤਰ = ਅਗੇਤਰ;

        ਵਾਪਸ old;
}
