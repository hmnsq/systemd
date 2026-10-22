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

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

ایستا log_syntax_callback_t log_syntax_callback = پوچ;
ایستا تهی *log_syntax_callback_userdata = پوچ;

ایستا LogTarget log_target = LOG_TARGET_CONSOLE;
ایستا صحیح log_max_level = LOG_INFO;
ایستا صحیح log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
ایستا صحیح log_facility = LOG_DAEMON;
ایستا bool ratelimit_kmsg = true;

ایستا صحیح console_fd = STDERR_FILENO;
ایستا صحیح console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ایستا صحیح syslog_fd = -EBADF;
ایستا صحیح kmsg_fd = -EBADF;
ایستا صحیح journal_fd = -EBADF;

ایستا bool syslog_is_stream = false;

ایستا صحیح show_color = -1; /* tristate */
ایستا bool show_location = false;
ایستا bool show_time = false;
ایستا bool show_tid = false;
ایستا bool show_executable = false;

ایستا bool upgrade_syslog_to_journal = false;
ایستا bool always_reopen_console = false;
ایستا bool open_when_needed = false;
ایستا bool prohibit_ipc = false;

ایستا thread_local ثابت نویسه *log_prefix = پوچ;

#اگر LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(عبارت)                                                \
        انجامبده {                                                            \
                اگر (_unlikely_(!(عبارت))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } وقتی (false)

تهی log_prefix_swap(ثابت نویسه **prefix) {
        أكد(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

ایستا تهی log_close_console(تهی) {
        /* See comment in log_close_journal() */
        (تهی) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ایستا صحیح log_open_console(تهی) {

        اگر (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                بازگشت 0;
        }

        اگر (console_fd < 3) {
                صحیح fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                اگر (fd < 0)
                        بازگشت fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        بازگشت 0;
}

ایستا تهی log_close_kmsg(تهی) {
        /* See comment in log_close_journal() */
        (تهی) safe_close(TAKE_FD(kmsg_fd));
}

ایستا صحیح log_open_kmsg(تهی) {

        اگر (kmsg_fd >= 0)
                بازگشت 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        اگر (kmsg_fd < 0)
                بازگشت -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        بازگشت 0;
}

ایستا تهی log_close_syslog(تهی) {
        /* See comment in log_close_journal() */
        (تهی) safe_close(TAKE_FD(syslog_fd));
}

ایستا صحیح create_log_socket(صحیح type) {
        ساختار timeval tv;
        صحیح fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        اگر (fd < 0)
                بازگشت -errno;

        fd = fd_move_above_stdio(fd);
        (تهی) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        اگر (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        وگرنه
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (تهی) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, اندازه(tv));

        بازگشت fd;
}

ایستا صحیح log_open_syslog(تهی) {
        صحیح r;

        اگر (syslog_fd >= 0)
                بازگشت 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        اگر (syslog_fd < 0) {
                r = syslog_fd;
                برو fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        اگر (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                اگر (syslog_fd < 0) {
                        r = syslog_fd;
                        برو fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                اگر (r < 0)
                        برو fail;

                syslog_is_stream = true;
        } وگرنه
                syslog_is_stream = false;

        بازگشت 0;

fail:
        log_close_syslog();
        بازگشت r;
}

ایستا تهی log_close_journal(تهی) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (تهی) safe_close(TAKE_FD(journal_fd));
}

ایستا صحیح log_open_journal(تهی) {
        صحیح r;

        اگر (journal_fd >= 0)
                بازگشت 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        اگر (journal_fd < 0) {
                r = journal_fd;
                برو fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        اگر (r < 0)
                برو fail;

        بازگشت 0;

fail:
        log_close_journal();
        بازگشت r;
}

bool stderr_is_journal(تهی) {
        _cleanup_free_ نویسه *w = پوچ;
        ثابت نویسه *e;
        uint64_t dev, ino;
        ساختار stat st;

        e = getenv("JOURNAL_STREAM");
        اگر (!e)
                بازگشت false;

        اگر (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                بازگشت false;
        اگر (!e)
                بازگشت false;

        اگر (safe_atou64(w, &dev) < 0)
                بازگشت false;
        اگر (safe_atou64(e, &ino) < 0)
                بازگشت false;

        اگر (fstat(STDERR_FILENO, &st) < 0)
                بازگشت false;

        بازگشت st.st_dev == dev && st.st_ino == ino;
}

صحیح log_open(تهی) {
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

        اگر (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                بازگشت 0;
        }

        اگر (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                اگر (!prohibit_ipc) {
                        اگر (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                اگر (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        بازگشت r;
                                }
                        }

                        اگر (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                اگر (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        بازگشت r;
                                }
                        }
                }

                اگر (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        اگر (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                بازگشت r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        بازگشت log_open_console();
}

تهی log_set_target(LogTarget target) {
        أكد(target >= 0);
        أكد(target < _LOG_TARGET_MAX);

        اگر (upgrade_syslog_to_journal) {
                اگر (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                وگرنه اگر (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

تهی log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

تهی log_close(تهی) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

تهی log_forget_fds(تهی) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

صحیح log_set_max_level(صحیح level) {
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

        بازگشت old;
}

تهی log_set_facility(صحیح facility) {
        log_facility = facility;
}

ایستا bool check_console_fd_is_tty(تهی) {
        اگر (console_fd < 0)
                بازگشت false;

        اگر (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        بازگشت console_fd_is_tty;
}

ایستا صحیح write_to_console(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *buffer) {

        ایستا صحیح dumb = -1;

        نویسه location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(صحیح) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ساختار iovec iovec[13];
        ثابت نویسه *on = پوچ, *off = پوچ;
        size_t n = 0;

        اگر (console_fd < 0)
                بازگشت 0;

        اگر (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        اگر (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                بازگشت 0;

        اگر (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        اگر (show_time &&
            format_timestamp(header_time, اندازه(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        اگر (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        اگر (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        اگر (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, پوچ);

        اگر (show_location) {
                ثابت نویسه *lon = "", *loff = "";
                اگر (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (تهی) snprintf(location, اندازه location, "%s%s:%i%s: ", lon, file, سطر, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        اگر (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        اگر (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
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
                        (تهی) log_open_console();
                        اگر (console_fd < 0)
                                بازگشت 0;

                        اگر (writev(console_fd, iovec, n) < 0)
                                بازگشت -errno;
                } وگرنه
                        بازگشت -errno;
        }

        بازگشت 1;
}

ایستا صحیح write_to_syslog(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *buffer) {

        نویسه header_priority[2 + DECIMAL_STR_MAX(صحیح) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ساختار tm tm;
        صحیح r;

        اگر (syslog_fd < 0)
                بازگشت 0;

        اگر (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                بازگشت 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        اگر (r < 0)
                بازگشت r;

        اگر (strftime(header_time, اندازه(header_time), "%h %e %T ", &tm) <= 0)
                بازگشت -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ساختار iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        ثابت ساختار msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        اگر (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        برای (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                اگر (n < 0)
                        بازگشت -errno;

                اگر (!syslog_is_stream)
                        بشکن;

                اگر (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        بشکن;
        }

        بازگشت 1;
}

ایستا صحیح write_to_kmsg(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ایستا thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        نویسه header_priority[2 + DECIMAL_STR_MAX(صحیح) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        اگر (kmsg_fd < 0)
                بازگشت 0;

        اگر (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                بازگشت 0;

        اگر (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                اگر (ratelimit_رقم_dropped(&ratelimit) > 1)
                        بازگشت 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ثابت ساختار iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        اگر (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                بازگشت -errno;

        بازگشت 1;
}

ایستا صحیح log_do_header(
                نویسه *header,
                size_t size,
                صحیح level,
                صحیح error,
                ثابت نویسه *file, صحیح سطر, ثابت نویسه *func,
                ثابت نویسه *object_field, ثابت نویسه *object,
                ثابت نویسه *extra_field, ثابت نویسه *extra) {
        صحیح r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_مقدار(error);

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
                     سطر ? "CODE_LINE=" : "",
                     سطر ? 1 : 0, سطر, /* %.0d means no output too, special case for 0 */
                     سطر ? "\n" : "",
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

        بازگشت 0;
}

ایستا تهی log_do_context(ساختار iovec *iovec, size_t iovec_len, size_t *n) {
        أكد(iovec);
        أكد(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        اگر (*n + 2 >= iovec_len)
                                بازگشت;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                برای (size_t i = 0; i < c->n_درون_داد_iovec; i++) {
                        اگر (*n + 2 >= iovec_len)
                                بازگشت;

                        iovec[(*n)++] = c->درون_داد_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                اگر (c->کلید && c->مقدار) {
                        اگر (*n + 3 >= iovec_len)
                                بازگشت;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->کلید);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->مقدار);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ایستا صحیح write_to_journal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                ثابت نویسه *buffer) {

        نویسه header[LINE_MAX];
        size_t n = 0, iovec_len;
        ساختار iovec *iovec;

        اگر (journal_fd < 0)
                بازگشت 0;

        اگر (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                بازگشت 0;

        اگر (!MUL_SAFE(&iovec_len, log_context_رقم_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        وگرنه
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ساختار iovec, iovec_len);

        log_do_header(header, اندازه(header), level, error, file, سطر, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        اگر (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ثابت ساختار msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        اگر (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                بازگشت -errno;

        بازگشت 1;
}

صحیح log_dispatch_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                نویسه *buffer) {

        assert_raw(buffer);

        اگر (log_target == LOG_TARGET_NULL)
                بازگشت -ERRNO_مقدار(error);

        /* Patch in LOG_DAEMON facility if necessary */
        اگر (LOG_FAC(level) == 0)
                level |= log_facility;

        اگر (open_when_needed)
                (تهی) log_open();

        انجامبده {
                نویسه *e;
                صحیح k = 0;

                buffer += strspn(buffer, NEWLINE);

                اگر (buffer[0] == 0)
                        بشکن;

                اگر ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                اگر (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, سطر, func, object_field, object, extra_field, extra, buffer);
                        اگر (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                اگر (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, سطر, func, buffer);
                        اگر (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                اگر (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        اگر (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, سطر, func, buffer);
                        اگر (k < 0) {
                                log_close_kmsg();
                                (تهی) log_open_console();
                        }
                }

                اگر (k <= 0)
                        (تهی) write_to_console(level, error, file, سطر, func, buffer);

                buffer = e;
        } وقتی (buffer);

        اگر (open_when_needed)
                log_close();

        بازگشت -ERRNO_مقدار(error);
}

صحیح log_dump_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                نویسه *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        اگر (_likely_(LOG_PRI(level) > log_max_level))
                بازگشت -ERRNO_مقدار(error);

        بازگشت log_dispatch_internal(level, error, file, سطر, func, پوچ, پوچ, پوچ, پوچ, buffer);
}

صحیح log_internalv(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format,
                va_list ap) {

        اگر (_likely_(LOG_PRI(level) > log_max_level))
                بازگشت -ERRNO_مقدار(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        نویسه buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_مقدار(error));

        (تهی) vsnprintf(buffer, اندازه buffer, format, ap);

        بازگشت log_dispatch_internal(level, error, file, سطر, func, پوچ, پوچ, پوچ, پوچ, buffer);
}

صحیح log_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format, ...) {

        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = log_internalv(level, error, file, سطر, func, format, ap);
        va_end(ap);

        بازگشت r;
}

صحیح log_object_internalv(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                ثابت نویسه *format,
                va_list ap) {

        نویسه *buffer, *b;

        اگر (_likely_(LOG_PRI(level) > log_max_level))
                بازگشت -ERRNO_مقدار(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_مقدار(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(نویسه, LINE_MAX);
        (تهی) vsnprintf(b, LINE_MAX, format, ap);

        بازگشت log_dispatch_internal(level, error, file, سطر, func,
                                     object_field, object, extra_field, extra, buffer);
}

صحیح log_object_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                ثابت نویسه *format, ...) {

        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, سطر, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        بازگشت r;
}

صحیح log_oom_internal(صحیح level, ثابت نویسه *file, صحیح سطر, ثابت نویسه *func) {
        بازگشت log_internal(level, ENOMEM, file, سطر, func, "Out of memory.");
}

صحیح log_format_iovec(
                ساختار iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحیح error,
                ثابت نویسه *format,
                va_list ap) {

        أكد(iovec);
        أكد(n);

        وقتی (format && *n + 1 < iovec_len) {
                va_list aq;
                نویسه *m;
                صحیح r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_مقدار(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                اگر (r < 0)
                        بازگشت -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                اگر (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, نویسه *);
        }
        بازگشت 0;
}

صحیح log_struct_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format, ...) {

        نویسه buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        اگر (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                بازگشت -ERRNO_مقدار(error);

        اگر (LOG_FAC(level) == 0)
                level |= log_facility;

        اگر (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                اگر (open_when_needed)
                        log_open_journal();

                اگر (journal_fd >= 0) {
                        نویسه header[LINE_MAX];
                        ساختار iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        صحیح r;
                        bool fallback = false;

                        اگر (!MUL_SAFE(&iovec_len, log_context_رقم_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        وگرنه
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ساختار iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, اندازه(header), level, error, file, سطر, func, پوچ, پوچ, پوچ, پوچ);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        اگر (r < 0)
                                fallback = true;
                        وگرنه {
                                log_do_context(iovec, iovec_len, &n);

                                ثابت ساختار msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (تهی) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        برای (size_t i = 1; i < m; i += 2)
                                آزاد(iovec[i].iov_base);

                        اگر (!fallback) {
                                اگر (open_when_needed)
                                        log_close();

                                بازگشت -ERRNO_مقدار(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        وقتی (format) {
                va_list aq;

                errno = ERRNO_مقدار(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (تهی) vsnprintf(buf, اندازه buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                اگر (startswith(buf, "MESSAGE=")) {
                        found = true;
                        بشکن;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, نویسه *);
        }
        va_end(ap);

        اگر (!found) {
                اگر (open_when_needed)
                        log_close();

                بازگشت -ERRNO_مقدار(error);
        }

        بازگشت log_dispatch_internal(level, error, file, سطر, func, پوچ, پوچ, پوچ, پوچ, buf + 8);
}

صحیح log_struct_iovec_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت ساختار iovec درون_داد_iovec[],
                size_t n_درون_داد_iovec) {

        PROTECT_ERRNO;

        اگر (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                بازگشت -ERRNO_مقدار(error);

        اگر (LOG_FAC(level) == 0)
                level |= log_facility;

        اگر (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                نویسه header[LINE_MAX];
                ساختار iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_درون_داد_iovec_expanded;

                اگر (!MUL_SAFE(&n_درون_داد_iovec_expanded, n_درون_داد_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_رقم_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_درون_داد_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                وگرنه
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ساختار iovec, iovec_len);

                log_do_header(header, اندازه(header), level, error, file, سطر, func, پوچ, پوچ, پوچ, پوچ);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                برای (size_t i = 0; i < n_درون_داد_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = درون_داد_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ثابت ساختار msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                اگر (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        بازگشت -ERRNO_مقدار(error);
        }

        برای (size_t i = 0; i < n_درون_داد_iovec; i++)
                اگر (memory_startswith(درون_داد_iovec[i].iov_base, درون_داد_iovec[i].iov_len, "MESSAGE=")) {
                        نویسه *m;

                        m = strndupa_safe((نویسه*) درون_داد_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          درون_داد_iovec[i].iov_len - STRLEN("MESSAGE="));

                        بازگشت log_dispatch_internal(level, error, file, سطر, func, پوچ, پوچ, پوچ, پوچ, m);
                }

        /* Couldn't find MESSAGE=. */
        بازگشت -ERRNO_مقدار(error);
}

صحیح log_set_target_from_string(ثابت نویسه *e) {
        LogTarget t;

        t = log_target_from_string(e);
        اگر (t < 0)
                بازگشت t;

        log_set_target(t);
        بازگشت 0;
}

صحیح log_set_max_level_from_string(ثابت نویسه *e) {
        صحیح r;

        برای (;;) {
                _cleanup_free_ نویسه *word = پوچ, *prefix = پوچ;
                LogTarget target;
                ثابت نویسه *colon;

                r = extract_first_word(&e, &word, ",", 0);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                colon = strchr(word, ':');
                اگر (!colon) {
                        r = log_level_from_string(word);
                        اگر (r < 0)
                                بازگشت r;

                        log_set_max_level(r);
                        ادامه;
                }

                prefix = strndup(word, colon - word);
                اگر (!prefix)
                        بازگشت -ENOMEM;

                target = log_target_from_string(prefix);
                اگر (target < 0)
                        بازگشت target;

                اگر (target >= _LOG_TARGET_SINGLE_MAX)
                        بازگشت -EINVAL;

                r = log_level_from_string(colon + 1);
                اگر (r < 0)
                        بازگشت r;

                log_target_max_level[target] = r;
        }

        بازگشت 0;
}

صحیح log_max_levels_to_string(صحیح level, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(ret);

        r = log_level_to_string_alloc(level, &s);
        اگر (r < 0)
                بازگشت r;

        برای (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ نویسه *l = پوچ;

                اگر (log_target_max_level[target] == INT_MAX)
                        ادامه;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                اگر (r < 0)
                        بازگشت r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

ایستا صحیح log_set_ratelimit_kmsg_from_string(ثابت نویسه *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                بازگشت r;

        ratelimit_kmsg = r;
        بازگشت 0;
}

ایستا صحیح parse_proc_cmdline_item(ثابت نویسه *کلید, ثابت نویسه *مقدار, تهی *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        اگر (streq(کلید, "debug") && !مقدار)
                log_set_max_level(LOG_DEBUG);

        وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_target")) {

                اگر (proc_cmdline_مقدار_missing(کلید, مقدار))
                        بازگشت 0;

                اگر (log_set_target_from_string(مقدار) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", مقدار);

        } وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_level")) {

                اگر (proc_cmdline_مقدار_missing(کلید, مقدار))
                        بازگشت 0;

                اگر (log_set_max_level_from_string(مقدار) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", مقدار);

        } وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_color")) {

                اگر (log_show_color_from_string(مقدار ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", مقدار);

        } وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_location")) {

                اگر (log_show_location_from_string(مقدار ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", مقدار);

        } وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_tid")) {

                اگر (log_show_tid_from_string(مقدار ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", مقدار);

        } وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_time")) {

                اگر (log_show_time_from_string(مقدار ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", مقدار);

        } وگرنه اگر (proc_cmdline_کلید_streq(کلید, "systemd.log_ratelimit_kmsg")) {

                اگر (log_set_ratelimit_kmsg_from_string(مقدار ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", مقدار);
        }

        بازگشت 0;
}

ایستا bool should_parse_proc_cmdline(تهی) {
        /* PID1 always reads the kernel command line. */
        اگر (getpid_cached() == 1)
                بازگشت true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        بازگشت invoked_by_systemd();
}

تهی log_parse_environment_variables(تهی) {
        ثابت نویسه *e;
        صحیح r;

        e = getenv("SYSTEMD_LOG_TARGET");
        اگر (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        اگر (e) {
                r = log_set_max_level_from_string(e);
                اگر (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } وگرنه {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                اگر (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                وگرنه اگر (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        اگر (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        اگر (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        اگر (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        اگر (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        اگر (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        اگر (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

تهی log_parse_environment(تهی) {
        /* Do not call from library code. */

        اگر (should_parse_proc_cmdline())
                (تهی) proc_cmdline_parse(parse_proc_cmdline_item, پوچ, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(تهی) {
        بازگشت log_target;
}

تهی log_settle_target(تهی) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        اگر (t != LOG_TARGET_AUTO)
                بازگشت;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

صحیح log_get_max_level(تهی) {
        بازگشت log_max_level;
}

صحیح log_get_target_max_level(LogTarget target) {
        أكد(target >= 0);
        أكد(target < _LOG_TARGET_SINGLE_MAX);
        بازگشت log_target_max_level[target];
}

تهی log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(تهی) {
        بازگشت show_color > 0; /* Defaults to false. */
}

تهی log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(تهی) {
        بازگشت show_location;
}

تهی log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(تهی) {
        بازگشت show_time;
}

تهی log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(تهی) {
        بازگشت show_tid;
}

تهی log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(تهی) {
        بازگشت show_executable;
}

صحیح log_show_color_from_string(ثابت نویسه *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                بازگشت r;

        log_show_color(r);
        بازگشت 0;
}

صحیح log_show_location_from_string(ثابت نویسه *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                بازگشت r;

        log_show_location(r);
        بازگشت 0;
}

صحیح log_show_time_from_string(ثابت نویسه *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                بازگشت r;

        log_show_time(r);
        بازگشت 0;
}

صحیح log_show_tid_from_string(ثابت نویسه *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                بازگشت r;

        log_show_tid(r);
        بازگشت 0;
}

صحیح log_show_executable_from_string(ثابت نویسه *e) {
        صحیح r;

        r = parse_boolean(e);
        اگر (r < 0)
                بازگشت r;

        log_show_executable(r);
        بازگشت 0;
}

bool log_on_console(تهی) {
        اگر (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                بازگشت true;

        بازگشت syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ایستا ثابت نویسه *ثابت log_target_table[_LOG_TARGET_MAX] = {
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

تهی log_received_signal(صحیح level, ثابت ساختار signalfd_siginfo *si) {
        أكد(si);

        اگر (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ نویسه *p = پوچ;

                (تهی) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } وگرنه
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

تهی set_log_syntax_callback(log_syntax_callback_t cb, تهی *userdata) {
        أكد(!log_syntax_callback || !cb);
        أكد(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

صحیح log_syntax_internal(
                ثابت نویسه *unit,
                صحیح level,
                ثابت نویسه *config_file,
                unsigned config_سطر,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format, ...) {

        PROTECT_ERRNO;

        اگر (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        اگر (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                بازگشت -ERRNO_مقدار(error);

        نویسه buffer[LINE_MAX];
        va_list ap;
        ثابت نویسه *unit_fmt = پوچ;

        errno = ERRNO_مقدار(error);

        va_start(ap, format);
        (تهی) vsnprintf(buffer, اندازه buffer, format, ap);
        va_end(ap);

        اگر (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        اگر (config_file) {
                اگر (config_سطر > 0)
                        بازگشت log_struct_internal(
                                        level,
                                        error,
                                        file, سطر, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_سطر),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_سطر, buffer),
                                        unit_fmt, unit,
                                        پوچ);
                وگرنه
                        بازگشت log_struct_internal(
                                        level,
                                        error,
                                        file, سطر, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        پوچ);
        } وگرنه اگر (unit)
                بازگشت log_struct_internal(
                                level,
                                error,
                                file, سطر, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                پوچ);
        وگرنه
                بازگشت log_struct_internal(
                                level,
                                error,
                                file, سطر, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                پوچ);
}

صحیح log_syntax_invalid_utf8_internal(
                ثابت نویسه *unit,
                صحیح level,
                ثابت نویسه *config_file,
                unsigned config_سطر,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ نویسه *p = پوچ;

        اگر (rvalue)
                p = utf8_escape_invalid(rvalue);

        بازگشت log_syntax_internal(unit, level, config_file, config_سطر,
                                   SYNTHETIC_ERRNO(EINVAL), file, سطر, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

صحیح log_syntax_parse_error_internal(
                ثابت نویسه *unit,
                ثابت نویسه *config_file,
                unsigned config_سطر,
                صحیح error,
                bool critical,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *lvalue,
                ثابت نویسه *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ نویسه *escaped = پوچ;

        /* OOM is always handled as critical. */
        اگر (ERRNO_مقدار(error) == ENOMEM)
                بازگشت log_oom_internal(LOG_ERR, file, سطر, func);

        اگر (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                اگر (!escaped)
                        rvalue = "(oom)";
                وگرنه
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_سطر, error,
                            file, سطر, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        بازگشت critical ? -ERRNO_مقدار(error) : 0;
}

تهی log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        اگر (b) {
                اگر (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                وگرنه اگر (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

تهی log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

تهی log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

تهی log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

صحیح log_emergency_level(تهی) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        بازگشت getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

صحیح log_dup_console(تهی) {
        صحیح copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        اگر (console_fd < 0 || console_fd >= 3)
                بازگشت 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        اگر (copy < 0)
                بازگشت -errno;

        console_fd = copy;
        بازگشت 0;
}

تهی log_setup(تهی) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (تهی) log_open();
        اگر (log_on_console() && show_color < 0)
                log_show_color(true);
}

ثابت نویسه* _log_set_prefix(ثابت نویسه *prefix, bool force) {
        ثابت نویسه *old = log_prefix;

        اگر (prefix || force)
                log_prefix = prefix;

        بازگشت old;
}
