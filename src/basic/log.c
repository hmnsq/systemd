/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/signalfd.h>
#شامل <sys/stat.h>
#شامل <sys/uio.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "sd-messages.h"

#شامل "alloc-util.h"
#شامل "ansi-color.h"
#شامل "argv-util.h"
#شامل "env-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "format-util.h"
#شامل "iovec-util.h"
#شامل "list.h"
#شامل "log.h"
#شامل "log-context.h"
#شامل "parse-util.h"
#شامل "proc-cmdline.h"
#شامل "process-util.h"
#شامل "ratelimit.h"
#شامل "signal-util.h"
#شامل "socket-util.h"
#شامل "stdio-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "syslog-util.h"
#شامل "terminal-util.h"
#شامل "time-util.h"
#شامل "utf8.h"

#تعریف_کرو SNDBUF_سائز (8*1024*1024)
#تعریف_کرو IOVEC_MAX 256U

ساکن log_syntax_callback_t log_syntax_callback = تہی;
ساکن خالی *log_syntax_callback_userdata = تہی;

ساکن Log_ہدف log_ہدف = LOG_ہدف_CONSOLE;
ساکن صحیح log_max_level = LOG_INFO;
ساکن صحیح log_ہدف_max_level[LOG_ہدف_SINGLE_MAX] = {
        [LOG_ہدف_CONSOLE] = زیادہ_سے_زیادہ_عدد,
        [LOG_ہدف_KMSG]    = زیادہ_سے_زیادہ_عدد,
        [LOG_ہدف_SYSLOG]  = زیادہ_سے_زیادہ_عدد,
        [LOG_ہدف_JOURNAL] = زیادہ_سے_زیادہ_عدد,
};
ساکن صحیح log_facility = LOG_DAEMON;
ساکن bool ratelimit_kmsg = true;

ساکن صحیح console_fd = STDERR_FILENO;
ساکن صحیح console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ساکن صحیح syslog_fd = -EBADF;
ساکن صحیح kmsg_fd = -EBADF;
ساکن صحیح journal_fd = -EBADF;

ساکن bool syslog_is_دھارا = false;

ساکن صحیح show_color = -1; /* tristate */
ساکن bool show_location = false;
ساکن bool show_وقت = false;
ساکن bool show_tid = false;
ساکن bool show_executable = false;

ساکن bool upgrade_syslog_مقصد_journal = false;
ساکن bool always_reopen_console = false;
ساکن bool open_when_needed = false;
ساکن bool prohibit_ipc = false;

ساکن thread_local ثابت حرف *log_سابقہ = تہی;

#اگر LOG_پیغام_VERIFICATION || defined(__COVERITY__)
bool log_پیغام_dummy = false; /* Always false */
#اگر_ختم

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#تعریف_کرو assert_raw(اظہاریہ)                                                \
        کرو {                                                            \
                اگر (_unlikely_(!(اظہاریہ))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } جب (false)

خالی log_سابقہ_تبادلہ(ثابت حرف **سابقہ) {
        أكد(سابقہ);

        تبادلہ_TWO(log_سابقہ, *سابقہ);
}

ساکن خالی log_close_console(خالی) {
        /* See comment in log_close_journal() */
        (خالی) محفوظ_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ساکن صحیح log_open_console(خالی) {

        اگر (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                واپس 0;
        }

        اگر (console_fd < 3) {
                صحیح fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                اگر (fd < 0)
                        واپس fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        واپس 0;
}

ساکن خالی log_close_kmsg(خالی) {
        /* See comment in log_close_journal() */
        (خالی) محفوظ_close(TAKE_FD(kmsg_fd));
}

ساکن صحیح log_open_kmsg(خالی) {

        اگر (kmsg_fd >= 0)
                واپس 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        اگر (kmsg_fd < 0)
                واپس -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        واپس 0;
}

ساکن خالی log_close_syslog(خالی) {
        /* See comment in log_close_journal() */
        (خالی) محفوظ_close(TAKE_FD(syslog_fd));
}

ساکن صحیح create_log_socket(صحیح type) {
        ساخت timeval tv;
        صحیح fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        اگر (fd < 0)
                واپس -errno;

        fd = fd_move_above_stdio(fd);
        (خالی) fd_inc_sndbuf(fd, SNDBUF_سائز);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        اگر (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_فی_MSEC);
        ورنہ
                timeval_store(&tv, 10 * USEC_فی_SEC);
        (خالی) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, حجم(tv));

        واپس fd;
}

ساکن صحیح log_open_syslog(خالی) {
        صحیح r;

        اگر (syslog_fd >= 0)
                واپس 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        اگر (syslog_fd < 0) {
                r = syslog_fd;
                جاؤ fail;
        }

        r = connect_unix_راستہ(syslog_fd, AT_FDCWD, "/dev/log");
        اگر (r < 0) {
                محفوظ_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                اگر (syslog_fd < 0) {
                        r = syslog_fd;
                        جاؤ fail;
                }

                r = connect_unix_راستہ(syslog_fd, AT_FDCWD, "/dev/log");
                اگر (r < 0)
                        جاؤ fail;

                syslog_is_دھارا = true;
        } ورنہ
                syslog_is_دھارا = false;

        واپس 0;

fail:
        log_close_syslog();
        واپس r;
}

ساکن خالی log_close_journal(خالی) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (خالی) محفوظ_close(TAKE_FD(journal_fd));
}

ساکن صحیح log_open_journal(خالی) {
        صحیح r;

        اگر (journal_fd >= 0)
                واپس 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        اگر (journal_fd < 0) {
                r = journal_fd;
                جاؤ fail;
        }

        r = connect_unix_راستہ(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        اگر (r < 0)
                جاؤ fail;

        واپس 0;

fail:
        log_close_journal();
        واپس r;
}

bool stderr_is_journal(خالی) {
        _cleanup_free_ حرف *w = تہی;
        ثابت حرف *e;
        uint64_t dev, ino;
        ساخت stat st;

        e = getenv("JOURNAL_STREAM");
        اگر (!e)
                واپس false;

        اگر (نکالو_پہلا_لفظ(&e, &w, ":", نکالو_DONT_COALESCE_SEPARATORS) <= 0)
                واپس false;
        اگر (!e)
                واپس false;

        اگر (محفوظ_atou64(w, &dev) < 0)
                واپس false;
        اگر (محفوظ_atou64(e, &ino) < 0)
                واپس false;

        اگر (fstat(STDERR_FILENO, &st) < 0)
                واپس false;

        واپس st.st_dev == dev && st.st_ino == ino;
}

صحیح log_open(خالی) {
        صحیح r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        اگر (log_ہدف == LOG_ہدف_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                واپس 0;
        }

        اگر (getpid_cached() == 1 ||
            stderr_is_journal() ||
            اندر_سیٹ(log_ہدف,
                   LOG_ہدف_KMSG,
                   LOG_ہدف_JOURNAL,
                   LOG_ہدف_JOURNAL_OR_KMSG,
                   LOG_ہدف_SYSLOG,
                   LOG_ہدف_SYSLOG_OR_KMSG)) {

                اگر (!prohibit_ipc) {
                        اگر (اندر_سیٹ(log_ہدف,
                                   LOG_ہدف_AUTO,
                                   LOG_ہدف_JOURNAL_OR_KMSG,
                                   LOG_ہدف_JOURNAL)) {

                                r = log_open_journal();
                                اگر (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        واپس r;
                                }
                        }

                        اگر (اندر_سیٹ(log_ہدف,
                                   LOG_ہدف_SYSLOG_OR_KMSG,
                                   LOG_ہدف_SYSLOG)) {

                                r = log_open_syslog();
                                اگر (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        واپس r;
                                }
                        }
                }

                اگر (اندر_سیٹ(log_ہدف, LOG_ہدف_AUTO,
                                       LOG_ہدف_JOURNAL_OR_KMSG,
                                       LOG_ہدف_SYSLOG_OR_KMSG,
                                       LOG_ہدف_KMSG)) {
                        r = log_open_kmsg();
                        اگر (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                واپس r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        واپس log_open_console();
}

خالی log_سیٹ_ہدف(Log_ہدف ہدف) {
        أكد(ہدف >= 0);
        أكد(ہدف < LOG_ہدف_MAX);

        اگر (upgrade_syslog_مقصد_journal) {
                اگر (ہدف == LOG_ہدف_SYSLOG)
                        ہدف = LOG_ہدف_JOURNAL;
                ورنہ اگر (ہدف == LOG_ہدف_SYSLOG_OR_KMSG)
                        ہدف = LOG_ہدف_JOURNAL_OR_KMSG;
        }

        log_ہدف = ہدف;
}

خالی log_سیٹ_ہدف_and_open(Log_ہدف ہدف) {
        log_سیٹ_ہدف(ہدف);
        log_open();
}

خالی log_close(خالی) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

خالی log_forget_fds(خالی) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

صحیح log_سیٹ_max_level(صحیح level) {
        أكد(level == LOG_NULL || log_level_is_valid(level));

        صحیح old = log_max_level;
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

        واپس old;
}

خالی log_سیٹ_facility(صحیح facility) {
        log_facility = facility;
}

ساکن bool check_console_fd_is_tty(خالی) {
        اگر (console_fd < 0)
                واپس false;

        اگر (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_محفوظ(console_fd);

        واپس console_fd_is_tty;
}

ساکن صحیح write_مقصد_console(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *بفر) {

        ساکن صحیح dumb = -1;

        حرف location[256],
             سرخی_وقت[FORMAT_TIMESTAMP_MAX],
             سابقہ[1 + DECIMAL_STR_MAX(صحیح) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ساخت iovec iovec[13];
        ثابت حرف *on = تہی, *off = تہی;
        size_t n = 0;

        اگر (console_fd < 0)
                واپس 0;

        اگر (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        اگر (LOG_PRI(level) > log_ہدف_max_level[LOG_ہدف_CONSOLE])
                واپس 0;

        اگر (log_ہدف == LOG_ہدف_CONSOLE_PREFIXED) {
                xsprintf(سابقہ, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(سابقہ);
        }

        اگر (show_وقت &&
            format_timestamp(سرخی_وقت, حجم(سرخی_وقت), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(سرخی_وقت);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        اگر (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(پروگرام_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        اگر (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        اگر (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, تہی);

        اگر (show_location) {
                ثابت حرف *lon = "", *loff = "";
                اگر (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_معمول();
                }

                (خالی) snprintf(location, حجم location, "%s%s:%i%s: ", lon, file, سطر, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        اگر (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        اگر (log_سابقہ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_سابقہ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(بفر);
        اگر (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        اگر (writev(console_fd, iovec, n) < 0) {

                اگر (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (خالی) log_open_console();
                        اگر (console_fd < 0)
                                واپس 0;

                        اگر (writev(console_fd, iovec, n) < 0)
                                واپس -errno;
                } ورنہ
                        واپس -errno;
        }

        واپس 1;
}

ساکن صحیح write_مقصد_syslog(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *بفر) {

        حرف سرخی_ترجیح[2 + DECIMAL_STR_MAX(صحیح) + 1],
             سرخی_وقت[64],
             سرخی_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ساخت tm tm;
        صحیح r;

        اگر (syslog_fd < 0)
                واپس 0;

        اگر (LOG_PRI(level) > log_ہدف_max_level[LOG_ہدف_SYSLOG])
                واپس 0;

        xsprintf(سرخی_ترجیح, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        اگر (r < 0)
                واپس r;

        اگر (strftime(سرخی_وقت, حجم(سرخی_وقت), "%h %e %T ", &tm) <= 0)
                واپس -EINVAL;

        xsprintf(سرخی_pid, "["PID_FMT"]: ", getpid_cached());

        ساخت iovec iovec[] = {
                IOVEC_MAKE_STRING(سرخی_ترجیح),
                IOVEC_MAKE_STRING(سرخی_وقت),
                IOVEC_MAKE_STRING(پروگرام_invocation_short_name),
                IOVEC_MAKE_STRING(سرخی_pid),
                IOVEC_MAKE_STRING(strempty(log_سابقہ)),
                IOVEC_MAKE_STRING(log_سابقہ ? ": " : ""),
                IOVEC_MAKE_STRING(بفر),
        };
        ثابت ساخت msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        اگر (syslog_is_دھارا)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        برائے (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                اگر (n < 0)
                        واپس -errno;

                اگر (!syslog_is_دھارا)
                        توڑو;

                اگر (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        توڑو;
        }

        واپس 1;
}

ساکن صحیح write_مقصد_kmsg(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *بفر) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ساکن thread_local شرح_حد_بندی ratelimit = { 5 * USEC_فی_SEC, 200 };

        حرف سرخی_ترجیح[2 + DECIMAL_STR_MAX(صحیح) + 1],
             سرخی_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        اگر (kmsg_fd < 0)
                واپس 0;

        اگر (LOG_PRI(level) > log_ہدف_max_level[LOG_ہدف_KMSG])
                واپس 0;

        اگر (ratelimit_kmsg && !ratelimit_نیچے(&ratelimit)) {
                اگر (ratelimit_عدد_dropped(&ratelimit) > 1)
                        واپس 0;

                بفر = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(سرخی_ترجیح, "<%i>", level);
        xsprintf(سرخی_pid, "["PID_FMT"]: ", getpid_cached());

        ثابت ساخت iovec iovec[] = {
                IOVEC_MAKE_STRING(سرخی_ترجیح),
                IOVEC_MAKE_STRING(پروگرام_invocation_short_name),
                IOVEC_MAKE_STRING(سرخی_pid),
                IOVEC_MAKE_STRING(strempty(log_سابقہ)),
                IOVEC_MAKE_STRING(log_سابقہ ? ": " : ""),
                IOVEC_MAKE_STRING(بفر),
                IOVEC_MAKE_STRING("\n"),
        };

        اگر (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                واپس -errno;

        واپس 1;
}

ساکن صحیح log_do_سرخی(
                حرف *سرخی,
                size_t سائز,
                صحیح level,
                صحیح error,
                ثابت حرف *file, صحیح سطر, ثابت حرف *func,
                ثابت حرف *object_میدان, ثابت حرف *object,
                ثابت حرف *extra_میدان, ثابت حرف *extra) {
        صحیح r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_قدر(error);

        r = snprintf(سرخی, سائز,
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
                     سطر ? "CODE_LINE=" : "",
                     سطر ? 1 : 0, سطر, /* %.0d means no output too, special case for 0 */
                     سطر ? "\n" : "",
                     isempty(func) ? "" : "CODE_FUNC=",
                     isempty(func) ? "" : func,
                     isempty(func) ? "" : "\n",
                     error ? "ERRNO=" : "",
                     error ? 1 : 0, error,
                     error ? "\n" : "",
                     isempty(object) ? "" : ASSERT_PTR(object_میدان),
                     isempty(object) ? "" : object,
                     isempty(object) ? "" : "\n",
                     isempty(extra) ? "" : ASSERT_PTR(extra_میدان),
                     isempty(extra) ? "" : extra,
                     isempty(extra) ? "" : "\n",
                     پروگرام_invocation_short_name);
        assert_raw((size_t) r < سائز);

        واپس 0;
}

ساکن خالی log_do_context(ساخت iovec *iovec, size_t iovec_len, size_t *n) {
        أكد(iovec);
        أكد(n);

        LIST_FOREACH(ll, c, log_context_سر()) {
                STRV_FOREACH(s, c->fields) {
                        اگر (*n + 2 >= iovec_len)
                                واپس;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                برائے (size_t i = 0; i < c->n_داخلہ_iovec; i++) {
                        اگر (*n + 2 >= iovec_len)
                                واپس;

                        iovec[(*n)++] = c->داخلہ_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                اگر (c->کلید && c->قدر) {
                        اگر (*n + 3 >= iovec_len)
                                واپس;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->کلید);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->قدر);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ساکن صحیح write_مقصد_journal(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *بفر) {

        حرف سرخی[LINE_MAX];
        size_t n = 0, iovec_len;
        ساخت iovec *iovec;

        اگر (journal_fd < 0)
                واپس 0;

        اگر (LOG_PRI(level) > log_ہدف_max_level[LOG_ہدف_JOURNAL])
                واپس 0;

        اگر (!MUL_محفوظ(&iovec_len, log_context_عدد_fields_2(), 3) ||
            !جوڑو_محفوظ(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        ورنہ
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ساخت iovec, iovec_len);

        log_do_سرخی(سرخی, حجم(سرخی), level, error, file, سطر, func, object_میدان, object, extra_میدان, extra);

        iovec[n++] = IOVEC_MAKE_STRING(سرخی);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        اگر (log_سابقہ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_سابقہ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(بفر);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ثابت ساخت msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        اگر (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                واپس -errno;

        واپس 1;
}

صحیح log_dispatch_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                حرف *بفر) {

        assert_raw(بفر);

        اگر (log_ہدف == LOG_ہدف_NULL)
                واپس -ERRNO_قدر(error);

        /* Patch in LOG_DAEMON facility if necessary */
        اگر (LOG_FAC(level) == 0)
                level |= log_facility;

        اگر (open_when_needed)
                (خالی) log_open();

        کرو {
                حرف *e;
                صحیح k = 0;

                بفر += strspn(بفر, NEWLINE);

                اگر (بفر[0] == 0)
                        توڑو;

                اگر ((e = strpbrk(بفر, NEWLINE)))
                        *(e++) = 0;

                اگر (اندر_سیٹ(log_ہدف, LOG_ہدف_AUTO,
                                       LOG_ہدف_JOURNAL_OR_KMSG,
                                       LOG_ہدف_JOURNAL)) {

                        k = write_مقصد_journal(level, error, file, سطر, func, object_میدان, object, extra_میدان, extra, بفر);
                        اگر (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                اگر (اندر_سیٹ(log_ہدف, LOG_ہدف_SYSLOG_OR_KMSG,
                                       LOG_ہدف_SYSLOG)) {

                        k = write_مقصد_syslog(level, error, file, سطر, func, بفر);
                        اگر (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                اگر (k <= 0 &&
                    اندر_سیٹ(log_ہدف, LOG_ہدف_AUTO,
                                       LOG_ہدف_SYSLOG_OR_KMSG,
                                       LOG_ہدف_JOURNAL_OR_KMSG,
                                       LOG_ہدف_KMSG)) {

                        اگر (k < 0)
                                log_open_kmsg();

                        k = write_مقصد_kmsg(level, error, file, سطر, func, بفر);
                        اگر (k < 0) {
                                log_close_kmsg();
                                (خالی) log_open_console();
                        }
                }

                اگر (k <= 0)
                        (خالی) write_مقصد_console(level, error, file, سطر, func, بفر);

                بفر = e;
        } جب (بفر);

        اگر (open_when_needed)
                log_close();

        واپس -ERRNO_قدر(error);
}

صحیح log_dump_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                حرف *بفر) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        اگر (_likely_(LOG_PRI(level) > log_max_level))
                واپس -ERRNO_قدر(error);

        واپس log_dispatch_اندرونی(level, error, file, سطر, func, تہی, تہی, تہی, تہی, بفر);
}

صحیح log_internalv(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format,
                va_list ap) {

        اگر (_likely_(LOG_PRI(level) > log_max_level))
                واپس -ERRNO_قدر(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        حرف بفر[LINE_MAX];
        LOCAL_ERRNO(ERRNO_قدر(error));

        (خالی) vsnprintf(بفر, حجم بفر, format, ap);

        واپس log_dispatch_اندرونی(level, error, file, سطر, func, تہی, تہی, تہی, تہی, بفر);
}

صحیح log_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) {

        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = log_internalv(level, error, file, سطر, func, format, ap);
        va_end(ap);

        واپس r;
}

صحیح log_object_internalv(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *format,
                va_list ap) {

        حرف *بفر, *b;

        اگر (_likely_(LOG_PRI(level) > log_max_level))
                واپس -ERRNO_قدر(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_قدر(error));

        LOG_سیٹ_سابقہ(object);

        b = بفر = newa(حرف, LINE_MAX);
        (خالی) vsnprintf(b, LINE_MAX, format, ap);

        واپس log_dispatch_اندرونی(level, error, file, سطر, func,
                                     object_میدان, object, extra_میدان, extra, بفر);
}

صحیح log_object_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *format, ...) {

        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, سطر, func, object_میدان, object, extra_میدان, extra, format, ap);
        va_end(ap);

        واپس r;
}

صحیح log_oom_اندرونی(صحیح level, ثابت حرف *file, صحیح سطر, ثابت حرف *func) {
        واپس log_اندرونی(level, ENOMEM, file, سطر, func, "Out of memory.");
}

صحیح log_format_iovec(
                ساخت iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحیح error,
                ثابت حرف *format,
                va_list ap) {

        أكد(iovec);
        أكد(n);

        جب (format && *n + 1 < iovec_len) {
                va_list aq;
                حرف *m;
                صحیح r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_قدر(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                اگر (r < 0)
                        واپس -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                اگر (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, حرف *);
        }
        واپس 0;
}

صحیح log_struct_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) {

        حرف buf[LINE_MAX];
        bool ملا = false;
        PROTECT_ERRNO;
        va_list ap;

        اگر (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ہدف == LOG_ہدف_NULL)
                واپس -ERRNO_قدر(error);

        اگر (LOG_FAC(level) == 0)
                level |= log_facility;

        اگر (اندر_سیٹ(log_ہدف,
                   LOG_ہدف_AUTO,
                   LOG_ہدف_JOURNAL_OR_KMSG,
                   LOG_ہدف_JOURNAL)) {

                اگر (open_when_needed)
                        log_open_journal();

                اگر (journal_fd >= 0) {
                        حرف سرخی[LINE_MAX];
                        ساخت iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        صحیح r;
                        bool fallback = false;

                        اگر (!MUL_محفوظ(&iovec_len, log_context_عدد_fields_2(), 3) ||
                            !جوڑو_محفوظ(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        ورنہ
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ساخت iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_سرخی(سرخی, حجم(سرخی), level, error, file, سطر, func, تہی, تہی, تہی, تہی);
                        iovec[n++] = IOVEC_MAKE_STRING(سرخی);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        اگر (r < 0)
                                fallback = true;
                        ورنہ {
                                log_do_context(iovec, iovec_len, &n);

                                ثابت ساخت msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (خالی) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        برائے (size_t i = 1; i < m; i += 2)
                                آزاد(iovec[i].iov_base);

                        اگر (!fallback) {
                                اگر (open_when_needed)
                                        log_close();

                                واپس -ERRNO_قدر(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        جب (format) {
                va_list aq;

                errno = ERRNO_قدر(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (خالی) vsnprintf(buf, حجم buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                اگر (startswith(buf, "MESSAGE=")) {
                        ملا = true;
                        توڑو;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, حرف *);
        }
        va_end(ap);

        اگر (!ملا) {
                اگر (open_when_needed)
                        log_close();

                واپس -ERRNO_قدر(error);
        }

        واپس log_dispatch_اندرونی(level, error, file, سطر, func, تہی, تہی, تہی, تہی, buf + 8);
}

صحیح log_struct_iovec_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت ساخت iovec داخلہ_iovec[],
                size_t n_داخلہ_iovec) {

        PROTECT_ERRNO;

        اگر (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ہدف == LOG_ہدف_NULL)
                واپس -ERRNO_قدر(error);

        اگر (LOG_FAC(level) == 0)
                level |= log_facility;

        اگر (اندر_سیٹ(log_ہدف, LOG_ہدف_AUTO,
                               LOG_ہدف_JOURNAL_OR_KMSG,
                               LOG_ہدف_JOURNAL) &&
            journal_fd >= 0) {

                حرف سرخی[LINE_MAX];
                ساخت iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_داخلہ_iovec_expanded;

                اگر (!MUL_محفوظ(&n_داخلہ_iovec_expanded, n_داخلہ_iovec, 2) ||
                    !MUL_محفوظ(&n_context_iovec, log_context_عدد_fields_2(), 3) ||
                    !جوڑو_محفوظ(&iovec_len, 1, n_داخلہ_iovec_expanded) ||
                    !جوڑو_محفوظ(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                ورنہ
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ساخت iovec, iovec_len);

                log_do_سرخی(سرخی, حجم(سرخی), level, error, file, سطر, func, تہی, تہی, تہی, تہی);

                iovec[n++] = IOVEC_MAKE_STRING(سرخی);
                برائے (size_t i = 0; i < n_داخلہ_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = داخلہ_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ثابت ساخت msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                اگر (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        واپس -ERRNO_قدر(error);
        }

        برائے (size_t i = 0; i < n_داخلہ_iovec; i++)
                اگر (حافظہ_startswith(داخلہ_iovec[i].iov_base, داخلہ_iovec[i].iov_len, "MESSAGE=")) {
                        حرف *m;

                        m = strndupa_محفوظ((حرف*) داخلہ_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          داخلہ_iovec[i].iov_len - STRLEN("MESSAGE="));

                        واپس log_dispatch_اندرونی(level, error, file, سطر, func, تہی, تہی, تہی, تہی, m);
                }

        /* Couldn't find MESSAGE=. */
        واپس -ERRNO_قدر(error);
}

صحیح log_سیٹ_ہدف_from_string(ثابت حرف *e) {
        Log_ہدف t;

        t = log_ہدف_from_string(e);
        اگر (t < 0)
                واپس t;

        log_سیٹ_ہدف(t);
        واپس 0;
}

صحیح log_سیٹ_max_level_from_string(ثابت حرف *e) {
        صحیح r;

        برائے (;;) {
                _cleanup_free_ حرف *لفظ = تہی, *سابقہ = تہی;
                Log_ہدف ہدف;
                ثابت حرف *colon;

                r = نکالو_پہلا_لفظ(&e, &لفظ, ",", 0);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                colon = strchr(لفظ, ':');
                اگر (!colon) {
                        r = log_level_from_string(لفظ);
                        اگر (r < 0)
                                واپس r;

                        log_سیٹ_max_level(r);
                        جاری;
                }

                سابقہ = strndup(لفظ, colon - لفظ);
                اگر (!سابقہ)
                        واپس -ENOMEM;

                ہدف = log_ہدف_from_string(سابقہ);
                اگر (ہدف < 0)
                        واپس ہدف;

                اگر (ہدف >= LOG_ہدف_SINGLE_MAX)
                        واپس -EINVAL;

                r = log_level_from_string(colon + 1);
                اگر (r < 0)
                        واپس r;

                log_ہدف_max_level[ہدف] = r;
        }

        واپس 0;
}

صحیح log_max_سطحیں_مقصد_string(صحیح level, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        أكد(ret);

        r = log_level_مقصد_string_alloc(level, &s);
        اگر (r < 0)
                واپس r;

        برائے (Log_ہدف ہدف = 0; ہدف < LOG_ہدف_SINGLE_MAX; ہدف++) {
                _cleanup_free_ حرف *l = تہی;

                اگر (log_ہدف_max_level[ہدف] == زیادہ_سے_زیادہ_عدد)
                        جاری;

                r = log_level_مقصد_string_alloc(log_ہدف_max_level[ہدف], &l);
                اگر (r < 0)
                        واپس r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_ہدف_مقصد_string(ہدف), l);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(s);
        واپس 0;
}

ساکن صحیح log_سیٹ_ratelimit_kmsg_from_string(ثابت حرف *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                واپس r;

        ratelimit_kmsg = r;
        واپس 0;
}

ساکن صحیح parse_proc_cmdline_شے(ثابت حرف *کلید, ثابت حرف *قدر, خالی *ڈیٹا) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        اگر (streq(کلید, "debug") && !قدر)
                log_سیٹ_max_level(LOG_DEBUG);

        ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_target")) {

                اگر (proc_cmdline_قدر_missing(کلید, قدر))
                        واپس 0;

                اگر (log_سیٹ_ہدف_from_string(قدر) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", قدر);

        } ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_level")) {

                اگر (proc_cmdline_قدر_missing(کلید, قدر))
                        واپس 0;

                اگر (log_سیٹ_max_level_from_string(قدر) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", قدر);

        } ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_color")) {

                اگر (log_show_color_from_string(قدر ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", قدر);

        } ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_location")) {

                اگر (log_show_location_from_string(قدر ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", قدر);

        } ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_tid")) {

                اگر (log_show_tid_from_string(قدر ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", قدر);

        } ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_time")) {

                اگر (log_show_وقت_from_string(قدر ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", قدر);

        } ورنہ اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_ratelimit_kmsg")) {

                اگر (log_سیٹ_ratelimit_kmsg_from_string(قدر ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", قدر);
        }

        واپس 0;
}

ساکن bool should_parse_proc_cmdline(خالی) {
        /* PID1 always reads the kernel command line. */
        اگر (getpid_cached() == 1)
                واپس true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        واپس invoked_by_systemd();
}

خالی log_parse_environment_variables(خالی) {
        ثابت حرف *e;
        صحیح r;

        e = getenv("SYSTEMD_LOG_TARGET");
        اگر (e && log_سیٹ_ہدف_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        اگر (e) {
                r = log_سیٹ_max_level_from_string(e);
                اگر (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } ورنہ {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                اگر (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                ورنہ اگر (r > 0)
                        log_سیٹ_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        اگر (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        اگر (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        اگر (e && log_show_وقت_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        اگر (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        اگر (e && log_سیٹ_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        اگر (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

خالی log_parse_environment(خالی) {
        /* Do not call from library code. */

        اگر (should_parse_proc_cmdline())
                (خالی) proc_cmdline_parse(parse_proc_cmdline_شے, تہی, PROC_CMDLINE_STRIP_RD_سابقہ);

        log_parse_environment_variables();
}

Log_ہدف log_get_ہدف(خالی) {
        واپس log_ہدف;
}

خالی log_settle_ہدف(خالی) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        Log_ہدف t = log_get_ہدف();

        اگر (t != LOG_ہدف_AUTO)
                واپس;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_ہدف_KMSG : LOG_ہدف_JOURNAL_OR_KMSG)
                                                        : LOG_ہدف_CONSOLE;
        log_سیٹ_ہدف(t);
}

صحیح log_get_max_level(خالی) {
        واپس log_max_level;
}

صحیح log_get_ہدف_max_level(Log_ہدف ہدف) {
        أكد(ہدف >= 0);
        أكد(ہدف < LOG_ہدف_SINGLE_MAX);
        واپس log_ہدف_max_level[ہدف];
}

خالی log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(خالی) {
        واپس show_color > 0; /* Defaults to false. */
}

خالی log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(خالی) {
        واپس show_location;
}

خالی log_show_وقت(bool b) {
        show_وقت = b;
}

bool log_get_show_وقت(خالی) {
        واپس show_وقت;
}

خالی log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(خالی) {
        واپس show_tid;
}

خالی log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(خالی) {
        واپس show_executable;
}

صحیح log_show_color_from_string(ثابت حرف *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                واپس r;

        log_show_color(r);
        واپس 0;
}

صحیح log_show_location_from_string(ثابت حرف *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                واپس r;

        log_show_location(r);
        واپس 0;
}

صحیح log_show_وقت_from_string(ثابت حرف *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                واپس r;

        log_show_وقت(r);
        واپس 0;
}

صحیح log_show_tid_from_string(ثابت حرف *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                واپس r;

        log_show_tid(r);
        واپس 0;
}

صحیح log_show_executable_from_string(ثابت حرف *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                واپس r;

        log_show_executable(r);
        واپس 0;
}

bool log_on_console(خالی) {
        اگر (اندر_سیٹ(log_ہدف, LOG_ہدف_CONSOLE,
                               LOG_ہدف_CONSOLE_PREFIXED))
                واپس true;

        واپس syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ساکن ثابت حرف *ثابت log_ہدف_جدول[LOG_ہدف_MAX] = {
        [LOG_ہدف_CONSOLE]          = "console",
        [LOG_ہدف_CONSOLE_PREFIXED] = "console-prefixed",
        [LOG_ہدف_KMSG]             = "kmsg",
        [LOG_ہدف_JOURNAL]          = "journal",
        [LOG_ہدف_JOURNAL_OR_KMSG]  = "journal-or-kmsg",
        [LOG_ہدف_SYSLOG]           = "syslog",
        [LOG_ہدف_SYSLOG_OR_KMSG]   = "syslog-or-kmsg",
        [LOG_ہدف_AUTO]             = "auto",
        [LOG_ہدف_NULL]             = "null",
};

DEFINE_STRING_جدول_LOOKUP(log_ہدف, Log_ہدف);

خالی log_received_signal(صحیح level, ثابت ساخت signalfd_siginfo *si) {
        أكد(si);

        اگر (si_رمز_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ حرف *p = تہی;

                (خالی) pid_get_comm(si->ssi_pid, &p);

                log_بھرا(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_مقصد_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } ورنہ
                log_بھرا(level,
                         "Received SIG%s.",
                         signal_مقصد_string(si->ssi_signo));
}

خالی سیٹ_log_syntax_callback(log_syntax_callback_t cb, خالی *userdata) {
        أكد(!log_syntax_callback || !cb);
        أكد(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

صحیح log_syntax_اندرونی(
                ثابت حرف *unit,
                صحیح level,
                ثابت حرف *config_file,
                غیرعلامتی config_سطر,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) {

        PROTECT_ERRNO;

        اگر (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        اگر (_likely_(LOG_PRI(level) > log_max_level) ||
            log_ہدف == LOG_ہدف_NULL)
                واپس -ERRNO_قدر(error);

        حرف بفر[LINE_MAX];
        va_list ap;
        ثابت حرف *unit_fmt = تہی;

        errno = ERRNO_قدر(error);

        va_start(ap, format);
        (خالی) vsnprintf(بفر, حجم بفر, format, ap);
        va_end(ap);

        اگر (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        اگر (config_file) {
                اگر (config_سطر > 0)
                        واپس log_struct_اندرونی(
                                        level,
                                        error,
                                        file, سطر, func,
                                        LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                        LOG_شے("CONFIG_FILE=%s", config_file),
                                        LOG_شے("CONFIG_LINE=%u", config_سطر),
                                        LOG_پیغام("%s:%u: %s", config_file, config_سطر, بفر),
                                        unit_fmt, unit,
                                        تہی);
                ورنہ
                        واپس log_struct_اندرونی(
                                        level,
                                        error,
                                        file, سطر, func,
                                        LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                        LOG_شے("CONFIG_FILE=%s", config_file),
                                        LOG_پیغام("%s: %s", config_file, بفر),
                                        unit_fmt, unit,
                                        تہی);
        } ورنہ اگر (unit)
                واپس log_struct_اندرونی(
                                level,
                                error,
                                file, سطر, func,
                                LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                LOG_پیغام("%s: %s", unit, بفر),
                                unit_fmt, unit,
                                تہی);
        ورنہ
                واپس log_struct_اندرونی(
                                level,
                                error,
                                file, سطر, func,
                                LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                LOG_پیغام("%s", بفر),
                                تہی);
}

صحیح log_syntax_invalid_utf8_اندرونی(
                ثابت حرف *unit,
                صحیح level,
                ثابت حرف *config_file,
                غیرعلامتی config_سطر,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ حرف *p = تہی;

        اگر (rvalue)
                p = utf8_escape_invalid(rvalue);

        واپس log_syntax_اندرونی(unit, level, config_file, config_سطر,
                                   SYNTHETIC_ERRNO(EINVAL), file, سطر, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

صحیح log_syntax_parse_error_اندرونی(
                ثابت حرف *unit,
                ثابت حرف *config_file,
                غیرعلامتی config_سطر,
                صحیح error,
                bool critical,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *lvalue,
                ثابت حرف *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ حرف *escaped = تہی;

        /* OOM is always handled as critical. */
        اگر (ERRNO_قدر(error) == ENOMEM)
                واپس log_oom_اندرونی(LOG_ERR, file, سطر, func);

        اگر (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                اگر (!escaped)
                        rvalue = "(oom)";
                ورنہ
                        rvalue = " (escaped)";
        }

        log_syntax_اندرونی(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_سطر, error,
                            file, سطر, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        واپس critical ? -ERRNO_قدر(error) : 0;
}

خالی log_سیٹ_upgrade_syslog_مقصد_journal(bool b) {
        upgrade_syslog_مقصد_journal = b;

        /* Make the change effective immediately */
        اگر (b) {
                اگر (log_ہدف == LOG_ہدف_SYSLOG)
                        log_ہدف = LOG_ہدف_JOURNAL;
                ورنہ اگر (log_ہدف == LOG_ہدف_SYSLOG_OR_KMSG)
                        log_ہدف = LOG_ہدف_JOURNAL_OR_KMSG;
        }
}

خالی log_سیٹ_always_reopen_console(bool b) {
        always_reopen_console = b;
}

خالی log_سیٹ_open_when_needed(bool b) {
        open_when_needed = b;
}

خالی log_سیٹ_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

صحیح log_emergency_level(خالی) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        واپس getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

صحیح log_dup_console(خالی) {
        صحیح copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        اگر (console_fd < 0 || console_fd >= 3)
                واپس 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        اگر (copy < 0)
                واپس -errno;

        console_fd = copy;
        واپس 0;
}

خالی log_تیاری(خالی) {
        log_سیٹ_ہدف(LOG_ہدف_AUTO);
        log_parse_environment();
        (خالی) log_open();
        اگر (log_on_console() && show_color < 0)
                log_show_color(true);
}

ثابت حرف* log_سیٹ_سابقہ(ثابت حرف *سابقہ, bool force) {
        ثابت حرف *old = log_سابقہ;

        اگر (سابقہ || force)
                log_سابقہ = سابقہ;

        واپس old;
}
