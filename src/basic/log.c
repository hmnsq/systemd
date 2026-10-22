/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <sys/signalfd.h>
#ضمن <sys/stat.h>
#ضمن <sys/uio.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "sd-messages.h"

#ضمن "alloc-util.h"
#ضمن "ansi-color.h"
#ضمن "argv-util.h"
#ضمن "env-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "format-util.h"
#ضمن "iovec-util.h"
#ضمن "list.h"
#ضمن "log.h"
#ضمن "log-context.h"
#ضمن "parse-util.h"
#ضمن "proc-cmdline.h"
#ضمن "process-util.h"
#ضمن "ratelimit.h"
#ضمن "signal-util.h"
#ضمن "socket-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "syslog-util.h"
#ضمن "terminal-util.h"
#ضمن "time-util.h"
#ضمن "utf8.h"

#تعریف_کړه SNDBUF_اندازه (8*1024*1024)
#تعریف_کړه IOVEC_MAX 256U

ساكن log_syntax_callback_t log_syntax_callback = عدم;
ساكن فراغ *log_syntax_callback_userdata = عدم;

ساكن Log_موخه log_موخه = LOG_موخه_CONSOLE;
ساكن صحيح log_max_level = LOG_INFO;
ساكن صحيح log_موخه_max_level[LOG_موخه_SINGLE_MAX] = {
        [LOG_موخه_CONSOLE] = لوی_عدد,
        [LOG_موخه_KMSG]    = لوی_عدد,
        [LOG_موخه_SYSLOG]  = لوی_عدد,
        [LOG_موخه_JOURNAL] = لوی_عدد,
};
ساكن صحيح log_facility = LOG_DAEMON;
ساكن bool ratelimit_kmsg = true;

ساكن صحيح console_fd = STDERR_FILENO;
ساكن صحيح console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ساكن صحيح syslog_fd = -EBADF;
ساكن صحيح kmsg_fd = -EBADF;
ساكن صحيح journal_fd = -EBADF;

ساكن bool syslog_is_بهیر = false;

ساكن صحيح show_color = -1; /* tristate */
ساكن bool show_location = false;
ساكن bool show_وخت = false;
ساكن bool show_tid = false;
ساكن bool show_executable = false;

ساكن bool upgrade_syslog_منزل_journal = false;
ساكن bool always_reopen_console = false;
ساكن bool open_when_needed = false;
ساكن bool prohibit_ipc = false;

ساكن thread_local ثابت حرف *log_مختاړی = عدم;

#إذا LOG_پیغام_VERIFICATION || defined(__COVERITY__)
bool log_پیغام_dummy = false; /* Always false */
#د_که_پای

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#تعریف_کړه assert_raw(بیان)                                                \
        افعل {                                                            \
                إذا (_unlikely_(!(بیان))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } طالما (false)

فراغ log_مختاړی_بدلون(ثابت حرف **مختاړی) {
        أكد(مختاړی);

        بدلون_TWO(log_مختاړی, *مختاړی);
}

ساكن فراغ log_close_console(فراغ) {
        /* See comment in log_close_journal() */
        (فراغ) خوندي_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ساكن صحيح log_open_console(فراغ) {

        إذا (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                أرجع 0;
        }

        إذا (console_fd < 3) {
                صحيح fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                إذا (fd < 0)
                        أرجع fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        أرجع 0;
}

ساكن فراغ log_close_kmsg(فراغ) {
        /* See comment in log_close_journal() */
        (فراغ) خوندي_close(TAKE_FD(kmsg_fd));
}

ساكن صحيح log_open_kmsg(فراغ) {

        إذا (kmsg_fd >= 0)
                أرجع 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        إذا (kmsg_fd < 0)
                أرجع -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        أرجع 0;
}

ساكن فراغ log_close_syslog(فراغ) {
        /* See comment in log_close_journal() */
        (فراغ) خوندي_close(TAKE_FD(syslog_fd));
}

ساكن صحيح create_log_socket(صحيح type) {
        بنية timeval tv;
        صحيح fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        إذا (fd < 0)
                أرجع -errno;

        fd = fd_move_above_stdio(fd);
        (فراغ) fd_inc_sndbuf(fd, SNDBUF_اندازه);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        إذا (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_فی_MSEC);
        وإلا
                timeval_store(&tv, 10 * USEC_فی_SEC);
        (فراغ) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, حجم(tv));

        أرجع fd;
}

ساكن صحيح log_open_syslog(فراغ) {
        صحيح r;

        إذا (syslog_fd >= 0)
                أرجع 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        إذا (syslog_fd < 0) {
                r = syslog_fd;
                اذهب fail;
        }

        r = connect_unix_لار(syslog_fd, AT_FDCWD, "/dev/log");
        إذا (r < 0) {
                خوندي_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                إذا (syslog_fd < 0) {
                        r = syslog_fd;
                        اذهب fail;
                }

                r = connect_unix_لار(syslog_fd, AT_FDCWD, "/dev/log");
                إذا (r < 0)
                        اذهب fail;

                syslog_is_بهیر = true;
        } وإلا
                syslog_is_بهیر = false;

        أرجع 0;

fail:
        log_close_syslog();
        أرجع r;
}

ساكن فراغ log_close_journal(فراغ) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (فراغ) خوندي_close(TAKE_FD(journal_fd));
}

ساكن صحيح log_open_journal(فراغ) {
        صحيح r;

        إذا (journal_fd >= 0)
                أرجع 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        إذا (journal_fd < 0) {
                r = journal_fd;
                اذهب fail;
        }

        r = connect_unix_لار(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        إذا (r < 0)
                اذهب fail;

        أرجع 0;

fail:
        log_close_journal();
        أرجع r;
}

bool stderr_is_journal(فراغ) {
        _cleanup_free_ حرف *w = عدم;
        ثابت حرف *e;
        uint64_t dev, ino;
        بنية stat st;

        e = getenv("JOURNAL_STREAM");
        إذا (!e)
                أرجع false;

        إذا (ایستل_لومړی_کلمه(&e, &w, ":", ایستل_DONT_COALESCE_SEPARATORS) <= 0)
                أرجع false;
        إذا (!e)
                أرجع false;

        إذا (خوندي_atou64(w, &dev) < 0)
                أرجع false;
        إذا (خوندي_atou64(e, &ino) < 0)
                أرجع false;

        إذا (fstat(STDERR_FILENO, &st) < 0)
                أرجع false;

        أرجع st.st_dev == dev && st.st_ino == ino;
}

صحيح log_open(فراغ) {
        صحيح r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        إذا (log_موخه == LOG_موخه_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                أرجع 0;
        }

        إذا (getpid_cached() == 1 ||
            stderr_is_journal() ||
            دننه_سېټ(log_موخه,
                   LOG_موخه_KMSG,
                   LOG_موخه_JOURNAL,
                   LOG_موخه_JOURNAL_OR_KMSG,
                   LOG_موخه_SYSLOG,
                   LOG_موخه_SYSLOG_OR_KMSG)) {

                إذا (!prohibit_ipc) {
                        إذا (دننه_سېټ(log_موخه,
                                   LOG_موخه_AUTO,
                                   LOG_موخه_JOURNAL_OR_KMSG,
                                   LOG_موخه_JOURNAL)) {

                                r = log_open_journal();
                                إذا (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        أرجع r;
                                }
                        }

                        إذا (دننه_سېټ(log_موخه,
                                   LOG_موخه_SYSLOG_OR_KMSG,
                                   LOG_موخه_SYSLOG)) {

                                r = log_open_syslog();
                                إذا (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        أرجع r;
                                }
                        }
                }

                إذا (دننه_سېټ(log_موخه, LOG_موخه_AUTO,
                                       LOG_موخه_JOURNAL_OR_KMSG,
                                       LOG_موخه_SYSLOG_OR_KMSG,
                                       LOG_موخه_KMSG)) {
                        r = log_open_kmsg();
                        إذا (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                أرجع r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        أرجع log_open_console();
}

فراغ log_سېټ_موخه(Log_موخه موخه) {
        أكد(موخه >= 0);
        أكد(موخه < LOG_موخه_MAX);

        إذا (upgrade_syslog_منزل_journal) {
                إذا (موخه == LOG_موخه_SYSLOG)
                        موخه = LOG_موخه_JOURNAL;
                وإلا إذا (موخه == LOG_موخه_SYSLOG_OR_KMSG)
                        موخه = LOG_موخه_JOURNAL_OR_KMSG;
        }

        log_موخه = موخه;
}

فراغ log_سېټ_موخه_and_open(Log_موخه موخه) {
        log_سېټ_موخه(موخه);
        log_open();
}

فراغ log_close(فراغ) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

فراغ log_forget_fds(فراغ) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

صحيح log_سېټ_max_level(صحيح level) {
        أكد(level == LOG_NULL || log_level_is_valid(level));

        صحيح old = log_max_level;
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

        أرجع old;
}

فراغ log_سېټ_facility(صحيح facility) {
        log_facility = facility;
}

ساكن bool check_console_fd_is_tty(فراغ) {
        إذا (console_fd < 0)
                أرجع false;

        إذا (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_خوندي(console_fd);

        أرجع console_fd_is_tty;
}

ساكن صحيح write_منزل_console(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *بفر) {

        ساكن صحيح dumb = -1;

        حرف location[256],
             سرلیک_وخت[FORMAT_TIMESTAMP_MAX],
             مختاړی[1 + DECIMAL_STR_MAX(صحيح) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        بنية iovec iovec[13];
        ثابت حرف *on = عدم, *off = عدم;
        size_t n = 0;

        إذا (console_fd < 0)
                أرجع 0;

        إذا (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        إذا (LOG_PRI(level) > log_موخه_max_level[LOG_موخه_CONSOLE])
                أرجع 0;

        إذا (log_موخه == LOG_موخه_CONSOLE_PREFIXED) {
                xsprintf(مختاړی, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(مختاړی);
        }

        إذا (show_وخت &&
            format_timestamp(سرلیک_وخت, حجم(سرلیک_وخت), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(سرلیک_وخت);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        إذا (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(پروګرام_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        إذا (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        إذا (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, عدم);

        إذا (show_location) {
                ثابت حرف *lon = "", *loff = "";
                إذا (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_نورمال();
                }

                (فراغ) snprintf(location, حجم location, "%s%s:%i%s: ", lon, file, کرښه, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        إذا (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        إذا (log_مختاړی) {
                iovec[n++] = IOVEC_MAKE_STRING(log_مختاړی);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(بفر);
        إذا (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        إذا (writev(console_fd, iovec, n) < 0) {

                إذا (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (فراغ) log_open_console();
                        إذا (console_fd < 0)
                                أرجع 0;

                        إذا (writev(console_fd, iovec, n) < 0)
                                أرجع -errno;
                } وإلا
                        أرجع -errno;
        }

        أرجع 1;
}

ساكن صحيح write_منزل_syslog(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *بفر) {

        حرف سرلیک_لومړیتوب[2 + DECIMAL_STR_MAX(صحيح) + 1],
             سرلیک_وخت[64],
             سرلیک_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        بنية tm tm;
        صحيح r;

        إذا (syslog_fd < 0)
                أرجع 0;

        إذا (LOG_PRI(level) > log_موخه_max_level[LOG_موخه_SYSLOG])
                أرجع 0;

        xsprintf(سرلیک_لومړیتوب, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        إذا (r < 0)
                أرجع r;

        إذا (strftime(سرلیک_وخت, حجم(سرلیک_وخت), "%h %e %T ", &tm) <= 0)
                أرجع -EINVAL;

        xsprintf(سرلیک_pid, "["PID_FMT"]: ", getpid_cached());

        بنية iovec iovec[] = {
                IOVEC_MAKE_STRING(سرلیک_لومړیتوب),
                IOVEC_MAKE_STRING(سرلیک_وخت),
                IOVEC_MAKE_STRING(پروګرام_invocation_short_name),
                IOVEC_MAKE_STRING(سرلیک_pid),
                IOVEC_MAKE_STRING(strempty(log_مختاړی)),
                IOVEC_MAKE_STRING(log_مختاړی ? ": " : ""),
                IOVEC_MAKE_STRING(بفر),
        };
        ثابت بنية msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        إذا (syslog_is_بهیر)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        لكل (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                إذا (n < 0)
                        أرجع -errno;

                إذا (!syslog_is_بهیر)
                        اكسر;

                إذا (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        اكسر;
        }

        أرجع 1;
}

ساكن صحيح write_منزل_kmsg(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *بفر) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ساكن thread_local شرح_حد ratelimit = { 5 * USEC_فی_SEC, 200 };

        حرف سرلیک_لومړیتوب[2 + DECIMAL_STR_MAX(صحيح) + 1],
             سرلیک_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        إذا (kmsg_fd < 0)
                أرجع 0;

        إذا (LOG_PRI(level) > log_موخه_max_level[LOG_موخه_KMSG])
                أرجع 0;

        إذا (ratelimit_kmsg && !ratelimit_لاندې(&ratelimit)) {
                إذا (ratelimit_عدد_dropped(&ratelimit) > 1)
                        أرجع 0;

                بفر = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(سرلیک_لومړیتوب, "<%i>", level);
        xsprintf(سرلیک_pid, "["PID_FMT"]: ", getpid_cached());

        ثابت بنية iovec iovec[] = {
                IOVEC_MAKE_STRING(سرلیک_لومړیتوب),
                IOVEC_MAKE_STRING(پروګرام_invocation_short_name),
                IOVEC_MAKE_STRING(سرلیک_pid),
                IOVEC_MAKE_STRING(strempty(log_مختاړی)),
                IOVEC_MAKE_STRING(log_مختاړی ? ": " : ""),
                IOVEC_MAKE_STRING(بفر),
                IOVEC_MAKE_STRING("\n"),
        };

        إذا (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                أرجع -errno;

        أرجع 1;
}

ساكن صحيح log_do_سرلیک(
                حرف *سرلیک,
                size_t اندازه,
                صحيح level,
                صحيح error,
                ثابت حرف *file, صحيح کرښه, ثابت حرف *func,
                ثابت حرف *object_ډګر, ثابت حرف *object,
                ثابت حرف *extra_ډګر, ثابت حرف *extra) {
        صحيح r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_ارزښت(error);

        r = snprintf(سرلیک, اندازه,
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
                     کرښه ? "CODE_LINE=" : "",
                     کرښه ? 1 : 0, کرښه, /* %.0d means no output too, special case for 0 */
                     کرښه ? "\n" : "",
                     isempty(func) ? "" : "CODE_FUNC=",
                     isempty(func) ? "" : func,
                     isempty(func) ? "" : "\n",
                     error ? "ERRNO=" : "",
                     error ? 1 : 0, error,
                     error ? "\n" : "",
                     isempty(object) ? "" : ASSERT_PTR(object_ډګر),
                     isempty(object) ? "" : object,
                     isempty(object) ? "" : "\n",
                     isempty(extra) ? "" : ASSERT_PTR(extra_ډګر),
                     isempty(extra) ? "" : extra,
                     isempty(extra) ? "" : "\n",
                     پروګرام_invocation_short_name);
        assert_raw((size_t) r < اندازه);

        أرجع 0;
}

ساكن فراغ log_do_context(بنية iovec *iovec, size_t iovec_len, size_t *n) {
        أكد(iovec);
        أكد(n);

        LIST_FOREACH(ll, c, log_context_سر()) {
                STRV_FOREACH(s, c->fields) {
                        إذا (*n + 2 >= iovec_len)
                                أرجع;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                لكل (size_t i = 0; i < c->n_ننوت_iovec; i++) {
                        إذا (*n + 2 >= iovec_len)
                                أرجع;

                        iovec[(*n)++] = c->ننوت_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                إذا (c->کلی && c->ارزښت) {
                        إذا (*n + 3 >= iovec_len)
                                أرجع;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->کلی);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ارزښت);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ساكن صحيح write_منزل_journal(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *object_ډګر,
                ثابت حرف *object,
                ثابت حرف *extra_ډګر,
                ثابت حرف *extra,
                ثابت حرف *بفر) {

        حرف سرلیک[LINE_MAX];
        size_t n = 0, iovec_len;
        بنية iovec *iovec;

        إذا (journal_fd < 0)
                أرجع 0;

        إذا (LOG_PRI(level) > log_موخه_max_level[LOG_موخه_JOURNAL])
                أرجع 0;

        إذا (!MUL_خوندي(&iovec_len, log_context_عدد_fields_2(), 3) ||
            !جمع_کړه_خوندي(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        وإلا
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(بنية iovec, iovec_len);

        log_do_سرلیک(سرلیک, حجم(سرلیک), level, error, file, کرښه, func, object_ډګر, object, extra_ډګر, extra);

        iovec[n++] = IOVEC_MAKE_STRING(سرلیک);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        إذا (log_مختاړی) {
                iovec[n++] = IOVEC_MAKE_STRING(log_مختاړی);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(بفر);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ثابت بنية msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        إذا (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                أرجع -errno;

        أرجع 1;
}

صحيح log_dispatch_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *object_ډګر,
                ثابت حرف *object,
                ثابت حرف *extra_ډګر,
                ثابت حرف *extra,
                حرف *بفر) {

        assert_raw(بفر);

        إذا (log_موخه == LOG_موخه_NULL)
                أرجع -ERRNO_ارزښت(error);

        /* Patch in LOG_DAEMON facility if necessary */
        إذا (LOG_FAC(level) == 0)
                level |= log_facility;

        إذا (open_when_needed)
                (فراغ) log_open();

        افعل {
                حرف *e;
                صحيح k = 0;

                بفر += strspn(بفر, NEWLINE);

                إذا (بفر[0] == 0)
                        اكسر;

                إذا ((e = strpbrk(بفر, NEWLINE)))
                        *(e++) = 0;

                إذا (دننه_سېټ(log_موخه, LOG_موخه_AUTO,
                                       LOG_موخه_JOURNAL_OR_KMSG,
                                       LOG_موخه_JOURNAL)) {

                        k = write_منزل_journal(level, error, file, کرښه, func, object_ډګر, object, extra_ډګر, extra, بفر);
                        إذا (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                إذا (دننه_سېټ(log_موخه, LOG_موخه_SYSLOG_OR_KMSG,
                                       LOG_موخه_SYSLOG)) {

                        k = write_منزل_syslog(level, error, file, کرښه, func, بفر);
                        إذا (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                إذا (k <= 0 &&
                    دننه_سېټ(log_موخه, LOG_موخه_AUTO,
                                       LOG_موخه_SYSLOG_OR_KMSG,
                                       LOG_موخه_JOURNAL_OR_KMSG,
                                       LOG_موخه_KMSG)) {

                        إذا (k < 0)
                                log_open_kmsg();

                        k = write_منزل_kmsg(level, error, file, کرښه, func, بفر);
                        إذا (k < 0) {
                                log_close_kmsg();
                                (فراغ) log_open_console();
                        }
                }

                إذا (k <= 0)
                        (فراغ) write_منزل_console(level, error, file, کرښه, func, بفر);

                بفر = e;
        } طالما (بفر);

        إذا (open_when_needed)
                log_close();

        أرجع -ERRNO_ارزښت(error);
}

صحيح log_dump_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                حرف *بفر) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        إذا (_likely_(LOG_PRI(level) > log_max_level))
                أرجع -ERRNO_ارزښت(error);

        أرجع log_dispatch_دننی(level, error, file, کرښه, func, عدم, عدم, عدم, عدم, بفر);
}

صحيح log_internalv(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *format,
                va_list ap) {

        إذا (_likely_(LOG_PRI(level) > log_max_level))
                أرجع -ERRNO_ارزښت(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        حرف بفر[LINE_MAX];
        LOCAL_ERRNO(ERRNO_ارزښت(error));

        (فراغ) vsnprintf(بفر, حجم بفر, format, ap);

        أرجع log_dispatch_دننی(level, error, file, کرښه, func, عدم, عدم, عدم, عدم, بفر);
}

صحيح log_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *format, ...) {

        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = log_internalv(level, error, file, کرښه, func, format, ap);
        va_end(ap);

        أرجع r;
}

صحيح log_object_internalv(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *object_ډګر,
                ثابت حرف *object,
                ثابت حرف *extra_ډګر,
                ثابت حرف *extra,
                ثابت حرف *format,
                va_list ap) {

        حرف *بفر, *b;

        إذا (_likely_(LOG_PRI(level) > log_max_level))
                أرجع -ERRNO_ارزښت(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_ارزښت(error));

        LOG_سېټ_مختاړی(object);

        b = بفر = newa(حرف, LINE_MAX);
        (فراغ) vsnprintf(b, LINE_MAX, format, ap);

        أرجع log_dispatch_دننی(level, error, file, کرښه, func,
                                     object_ډګر, object, extra_ډګر, extra, بفر);
}

صحيح log_object_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *object_ډګر,
                ثابت حرف *object,
                ثابت حرف *extra_ډګر,
                ثابت حرف *extra,
                ثابت حرف *format, ...) {

        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, کرښه, func, object_ډګر, object, extra_ډګر, extra, format, ap);
        va_end(ap);

        أرجع r;
}

صحيح log_oom_دننی(صحيح level, ثابت حرف *file, صحيح کرښه, ثابت حرف *func) {
        أرجع log_دننی(level, ENOMEM, file, کرښه, func, "Out of memory.");
}

صحيح log_format_iovec(
                بنية iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحيح error,
                ثابت حرف *format,
                va_list ap) {

        أكد(iovec);
        أكد(n);

        طالما (format && *n + 1 < iovec_len) {
                va_list aq;
                حرف *m;
                صحيح r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_ارزښت(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                إذا (r < 0)
                        أرجع -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                إذا (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, حرف *);
        }
        أرجع 0;
}

صحيح log_struct_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *format, ...) {

        حرف buf[LINE_MAX];
        bool وموندل = false;
        PROTECT_ERRNO;
        va_list ap;

        إذا (_likely_(LOG_PRI(level) > log_max_level) ||
            log_موخه == LOG_موخه_NULL)
                أرجع -ERRNO_ارزښت(error);

        إذا (LOG_FAC(level) == 0)
                level |= log_facility;

        إذا (دننه_سېټ(log_موخه,
                   LOG_موخه_AUTO,
                   LOG_موخه_JOURNAL_OR_KMSG,
                   LOG_موخه_JOURNAL)) {

                إذا (open_when_needed)
                        log_open_journal();

                إذا (journal_fd >= 0) {
                        حرف سرلیک[LINE_MAX];
                        بنية iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        صحيح r;
                        bool fallback = false;

                        إذا (!MUL_خوندي(&iovec_len, log_context_عدد_fields_2(), 3) ||
                            !جمع_کړه_خوندي(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        وإلا
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(بنية iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_سرلیک(سرلیک, حجم(سرلیک), level, error, file, کرښه, func, عدم, عدم, عدم, عدم);
                        iovec[n++] = IOVEC_MAKE_STRING(سرلیک);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        إذا (r < 0)
                                fallback = true;
                        وإلا {
                                log_do_context(iovec, iovec_len, &n);

                                ثابت بنية msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (فراغ) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        لكل (size_t i = 1; i < m; i += 2)
                                حرر(iovec[i].iov_base);

                        إذا (!fallback) {
                                إذا (open_when_needed)
                                        log_close();

                                أرجع -ERRNO_ارزښت(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        طالما (format) {
                va_list aq;

                errno = ERRNO_ارزښت(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (فراغ) vsnprintf(buf, حجم buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                إذا (startswith(buf, "MESSAGE=")) {
                        وموندل = true;
                        اكسر;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, حرف *);
        }
        va_end(ap);

        إذا (!وموندل) {
                إذا (open_when_needed)
                        log_close();

                أرجع -ERRNO_ارزښت(error);
        }

        أرجع log_dispatch_دننی(level, error, file, کرښه, func, عدم, عدم, عدم, عدم, buf + 8);
}

صحيح log_struct_iovec_دننی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت بنية iovec ننوت_iovec[],
                size_t n_ننوت_iovec) {

        PROTECT_ERRNO;

        إذا (_likely_(LOG_PRI(level) > log_max_level) ||
            log_موخه == LOG_موخه_NULL)
                أرجع -ERRNO_ارزښت(error);

        إذا (LOG_FAC(level) == 0)
                level |= log_facility;

        إذا (دننه_سېټ(log_موخه, LOG_موخه_AUTO,
                               LOG_موخه_JOURNAL_OR_KMSG,
                               LOG_موخه_JOURNAL) &&
            journal_fd >= 0) {

                حرف سرلیک[LINE_MAX];
                بنية iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_ننوت_iovec_expanded;

                إذا (!MUL_خوندي(&n_ننوت_iovec_expanded, n_ننوت_iovec, 2) ||
                    !MUL_خوندي(&n_context_iovec, log_context_عدد_fields_2(), 3) ||
                    !جمع_کړه_خوندي(&iovec_len, 1, n_ننوت_iovec_expanded) ||
                    !جمع_کړه_خوندي(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                وإلا
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(بنية iovec, iovec_len);

                log_do_سرلیک(سرلیک, حجم(سرلیک), level, error, file, کرښه, func, عدم, عدم, عدم, عدم);

                iovec[n++] = IOVEC_MAKE_STRING(سرلیک);
                لكل (size_t i = 0; i < n_ننوت_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = ننوت_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ثابت بنية msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                إذا (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        أرجع -ERRNO_ارزښت(error);
        }

        لكل (size_t i = 0; i < n_ننوت_iovec; i++)
                إذا (حافظه_startswith(ننوت_iovec[i].iov_base, ننوت_iovec[i].iov_len, "MESSAGE=")) {
                        حرف *m;

                        m = strndupa_خوندي((حرف*) ننوت_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          ننوت_iovec[i].iov_len - STRLEN("MESSAGE="));

                        أرجع log_dispatch_دننی(level, error, file, کرښه, func, عدم, عدم, عدم, عدم, m);
                }

        /* Couldn't find MESSAGE=. */
        أرجع -ERRNO_ارزښت(error);
}

صحيح log_سېټ_موخه_from_string(ثابت حرف *e) {
        Log_موخه t;

        t = log_موخه_from_string(e);
        إذا (t < 0)
                أرجع t;

        log_سېټ_موخه(t);
        أرجع 0;
}

صحيح log_سېټ_max_level_from_string(ثابت حرف *e) {
        صحيح r;

        لكل (;;) {
                _cleanup_free_ حرف *کلمه = عدم, *مختاړی = عدم;
                Log_موخه موخه;
                ثابت حرف *colon;

                r = ایستل_لومړی_کلمه(&e, &کلمه, ",", 0);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                colon = strchr(کلمه, ':');
                إذا (!colon) {
                        r = log_level_from_string(کلمه);
                        إذا (r < 0)
                                أرجع r;

                        log_سېټ_max_level(r);
                        استمر;
                }

                مختاړی = strndup(کلمه, colon - کلمه);
                إذا (!مختاړی)
                        أرجع -ENOMEM;

                موخه = log_موخه_from_string(مختاړی);
                إذا (موخه < 0)
                        أرجع موخه;

                إذا (موخه >= LOG_موخه_SINGLE_MAX)
                        أرجع -EINVAL;

                r = log_level_from_string(colon + 1);
                إذا (r < 0)
                        أرجع r;

                log_موخه_max_level[موخه] = r;
        }

        أرجع 0;
}

صحيح log_max_پوړونه_منزل_string(صحيح level, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = log_level_منزل_string_alloc(level, &s);
        إذا (r < 0)
                أرجع r;

        لكل (Log_موخه موخه = 0; موخه < LOG_موخه_SINGLE_MAX; موخه++) {
                _cleanup_free_ حرف *l = عدم;

                إذا (log_موخه_max_level[موخه] == لوی_عدد)
                        استمر;

                r = log_level_منزل_string_alloc(log_موخه_max_level[موخه], &l);
                إذا (r < 0)
                        أرجع r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_موخه_منزل_string(موخه), l);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(s);
        أرجع 0;
}

ساكن صحيح log_سېټ_ratelimit_kmsg_from_string(ثابت حرف *e) {
        صحيح r;

        r = parse_boolean(e);
        إذا (r < 0)
                أرجع r;

        ratelimit_kmsg = r;
        أرجع 0;
}

ساكن صحيح parse_proc_cmdline_توکی(ثابت حرف *کلی, ثابت حرف *ارزښت, فراغ *معلومات) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        إذا (streq(کلی, "debug") && !ارزښت)
                log_سېټ_max_level(LOG_DEBUG);

        وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_target")) {

                إذا (proc_cmdline_ارزښت_missing(کلی, ارزښت))
                        أرجع 0;

                إذا (log_سېټ_موخه_from_string(ارزښت) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", ارزښت);

        } وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_level")) {

                إذا (proc_cmdline_ارزښت_missing(کلی, ارزښت))
                        أرجع 0;

                إذا (log_سېټ_max_level_from_string(ارزښت) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", ارزښت);

        } وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_color")) {

                إذا (log_show_color_from_string(ارزښت ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", ارزښت);

        } وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_location")) {

                إذا (log_show_location_from_string(ارزښت ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", ارزښت);

        } وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_tid")) {

                إذا (log_show_tid_from_string(ارزښت ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", ارزښت);

        } وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_time")) {

                إذا (log_show_وخت_from_string(ارزښت ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", ارزښت);

        } وإلا إذا (proc_cmdline_کلی_streq(کلی, "systemd.log_ratelimit_kmsg")) {

                إذا (log_سېټ_ratelimit_kmsg_from_string(ارزښت ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", ارزښت);
        }

        أرجع 0;
}

ساكن bool should_parse_proc_cmdline(فراغ) {
        /* PID1 always reads the kernel command line. */
        إذا (getpid_cached() == 1)
                أرجع true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        أرجع invoked_by_systemd();
}

فراغ log_parse_environment_variables(فراغ) {
        ثابت حرف *e;
        صحيح r;

        e = getenv("SYSTEMD_LOG_TARGET");
        إذا (e && log_سېټ_موخه_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        إذا (e) {
                r = log_سېټ_max_level_from_string(e);
                إذا (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } وإلا {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                إذا (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                وإلا إذا (r > 0)
                        log_سېټ_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        إذا (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        إذا (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        إذا (e && log_show_وخت_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        إذا (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        إذا (e && log_سېټ_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        إذا (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

فراغ log_parse_environment(فراغ) {
        /* Do not call from library code. */

        إذا (should_parse_proc_cmdline())
                (فراغ) proc_cmdline_parse(parse_proc_cmdline_توکی, عدم, PROC_CMDLINE_STRIP_RD_مختاړی);

        log_parse_environment_variables();
}

Log_موخه log_get_موخه(فراغ) {
        أرجع log_موخه;
}

فراغ log_settle_موخه(فراغ) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        Log_موخه t = log_get_موخه();

        إذا (t != LOG_موخه_AUTO)
                أرجع;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_موخه_KMSG : LOG_موخه_JOURNAL_OR_KMSG)
                                                        : LOG_موخه_CONSOLE;
        log_سېټ_موخه(t);
}

صحيح log_get_max_level(فراغ) {
        أرجع log_max_level;
}

صحيح log_get_موخه_max_level(Log_موخه موخه) {
        أكد(موخه >= 0);
        أكد(موخه < LOG_موخه_SINGLE_MAX);
        أرجع log_موخه_max_level[موخه];
}

فراغ log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(فراغ) {
        أرجع show_color > 0; /* Defaults to false. */
}

فراغ log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(فراغ) {
        أرجع show_location;
}

فراغ log_show_وخت(bool b) {
        show_وخت = b;
}

bool log_get_show_وخت(فراغ) {
        أرجع show_وخت;
}

فراغ log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(فراغ) {
        أرجع show_tid;
}

فراغ log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(فراغ) {
        أرجع show_executable;
}

صحيح log_show_color_from_string(ثابت حرف *e) {
        صحيح r;

        r = parse_boolean(e);
        إذا (r < 0)
                أرجع r;

        log_show_color(r);
        أرجع 0;
}

صحيح log_show_location_from_string(ثابت حرف *e) {
        صحيح r;

        r = parse_boolean(e);
        إذا (r < 0)
                أرجع r;

        log_show_location(r);
        أرجع 0;
}

صحيح log_show_وخت_from_string(ثابت حرف *e) {
        صحيح r;

        r = parse_boolean(e);
        إذا (r < 0)
                أرجع r;

        log_show_وخت(r);
        أرجع 0;
}

صحيح log_show_tid_from_string(ثابت حرف *e) {
        صحيح r;

        r = parse_boolean(e);
        إذا (r < 0)
                أرجع r;

        log_show_tid(r);
        أرجع 0;
}

صحيح log_show_executable_from_string(ثابت حرف *e) {
        صحيح r;

        r = parse_boolean(e);
        إذا (r < 0)
                أرجع r;

        log_show_executable(r);
        أرجع 0;
}

bool log_on_console(فراغ) {
        إذا (دننه_سېټ(log_موخه, LOG_موخه_CONSOLE,
                               LOG_موخه_CONSOLE_PREFIXED))
                أرجع true;

        أرجع syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ساكن ثابت حرف *ثابت log_موخه_جدول[LOG_موخه_MAX] = {
        [LOG_موخه_CONSOLE]          = "console",
        [LOG_موخه_CONSOLE_PREFIXED] = "console-prefixed",
        [LOG_موخه_KMSG]             = "kmsg",
        [LOG_موخه_JOURNAL]          = "journal",
        [LOG_موخه_JOURNAL_OR_KMSG]  = "journal-or-kmsg",
        [LOG_موخه_SYSLOG]           = "syslog",
        [LOG_موخه_SYSLOG_OR_KMSG]   = "syslog-or-kmsg",
        [LOG_موخه_AUTO]             = "auto",
        [LOG_موخه_NULL]             = "null",
};

DEFINE_STRING_جدول_LOOKUP(log_موخه, Log_موخه);

فراغ log_received_signal(صحيح level, ثابت بنية signalfd_siginfo *si) {
        أكد(si);

        إذا (si_کوډ_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ حرف *p = عدم;

                (فراغ) pid_get_comm(si->ssi_pid, &p);

                log_ډک(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_منزل_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } وإلا
                log_ډک(level,
                         "Received SIG%s.",
                         signal_منزل_string(si->ssi_signo));
}

فراغ سېټ_log_syntax_callback(log_syntax_callback_t cb, فراغ *userdata) {
        أكد(!log_syntax_callback || !cb);
        أكد(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

صحيح log_syntax_دننی(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بینښه config_کرښه,
                صحيح error,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *format, ...) {

        PROTECT_ERRNO;

        إذا (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        إذا (_likely_(LOG_PRI(level) > log_max_level) ||
            log_موخه == LOG_موخه_NULL)
                أرجع -ERRNO_ارزښت(error);

        حرف بفر[LINE_MAX];
        va_list ap;
        ثابت حرف *unit_fmt = عدم;

        errno = ERRNO_ارزښت(error);

        va_start(ap, format);
        (فراغ) vsnprintf(بفر, حجم بفر, format, ap);
        va_end(ap);

        إذا (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        إذا (config_file) {
                إذا (config_کرښه > 0)
                        أرجع log_struct_دننی(
                                        level,
                                        error,
                                        file, کرښه, func,
                                        LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                        LOG_توکی("CONFIG_FILE=%s", config_file),
                                        LOG_توکی("CONFIG_LINE=%u", config_کرښه),
                                        LOG_پیغام("%s:%u: %s", config_file, config_کرښه, بفر),
                                        unit_fmt, unit,
                                        عدم);
                وإلا
                        أرجع log_struct_دننی(
                                        level,
                                        error,
                                        file, کرښه, func,
                                        LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                        LOG_توکی("CONFIG_FILE=%s", config_file),
                                        LOG_پیغام("%s: %s", config_file, بفر),
                                        unit_fmt, unit,
                                        عدم);
        } وإلا إذا (unit)
                أرجع log_struct_دننی(
                                level,
                                error,
                                file, کرښه, func,
                                LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                LOG_پیغام("%s: %s", unit, بفر),
                                unit_fmt, unit,
                                عدم);
        وإلا
                أرجع log_struct_دننی(
                                level,
                                error,
                                file, کرښه, func,
                                LOG_پیغام_ID(SD_پیغام_INVALID_CONFIGURATION_STR),
                                LOG_پیغام("%s", بفر),
                                عدم);
}

صحيح log_syntax_invalid_utf8_دننی(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بینښه config_کرښه,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ حرف *p = عدم;

        إذا (rvalue)
                p = utf8_escape_invalid(rvalue);

        أرجع log_syntax_دننی(unit, level, config_file, config_کرښه,
                                   SYNTHETIC_ERRNO(EINVAL), file, کرښه, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

صحيح log_syntax_parse_error_دننی(
                ثابت حرف *unit,
                ثابت حرف *config_file,
                بینښه config_کرښه,
                صحيح error,
                bool critical,
                ثابت حرف *file,
                صحيح کرښه,
                ثابت حرف *func,
                ثابت حرف *lvalue,
                ثابت حرف *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ حرف *escaped = عدم;

        /* OOM is always handled as critical. */
        إذا (ERRNO_ارزښت(error) == ENOMEM)
                أرجع log_oom_دننی(LOG_ERR, file, کرښه, func);

        إذا (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                إذا (!escaped)
                        rvalue = "(oom)";
                وإلا
                        rvalue = " (escaped)";
        }

        log_syntax_دننی(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_کرښه, error,
                            file, کرښه, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        أرجع critical ? -ERRNO_ارزښت(error) : 0;
}

فراغ log_سېټ_upgrade_syslog_منزل_journal(bool b) {
        upgrade_syslog_منزل_journal = b;

        /* Make the change effective immediately */
        إذا (b) {
                إذا (log_موخه == LOG_موخه_SYSLOG)
                        log_موخه = LOG_موخه_JOURNAL;
                وإلا إذا (log_موخه == LOG_موخه_SYSLOG_OR_KMSG)
                        log_موخه = LOG_موخه_JOURNAL_OR_KMSG;
        }
}

فراغ log_سېټ_always_reopen_console(bool b) {
        always_reopen_console = b;
}

فراغ log_سېټ_open_when_needed(bool b) {
        open_when_needed = b;
}

فراغ log_سېټ_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

صحيح log_emergency_level(فراغ) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        أرجع getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

صحيح log_dup_console(فراغ) {
        صحيح copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        إذا (console_fd < 0 || console_fd >= 3)
                أرجع 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        إذا (copy < 0)
                أرجع -errno;

        console_fd = copy;
        أرجع 0;
}

فراغ log_تیاری(فراغ) {
        log_سېټ_موخه(LOG_موخه_AUTO);
        log_parse_environment();
        (فراغ) log_open();
        إذا (log_on_console() && show_color < 0)
                log_show_color(true);
}

ثابت حرف* log_سېټ_مختاړی(ثابت حرف *مختاړی, bool force) {
        ثابت حرف *old = log_مختاړی;

        إذا (مختاړی || force)
                log_مختاړی = مختاړی;

        أرجع old;
}
