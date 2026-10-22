/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/signalfd.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <sys/uio.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "sd-messages.h"

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "ansi-color.h"
#অন্তর্ভাব "argv-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "list.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "log-context.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "proc-cmdline.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "ratelimit.h"
#অন্তর্ভাব "signal-util.h"
#অন্তর্ভাব "socket-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "syslog-util.h"
#অন্তর্ভাব "terminal-util.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "utf8.h"

#define SNDBUF_আকার (8*1024*1024)
#define IOVEC_MAX 256U

স্থির log_syntax_callback_t log_syntax_callback = ফাঁকা;
স্থির শূন্য *log_syntax_callback_userdata = ফাঁকা;

স্থির LogTarget log_target = LOG_TARGET_CONSOLE;
স্থির পূর্ণসংখ্যা log_max_level = LOG_INFO;
স্থির পূর্ণসংখ্যা log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
স্থির পূর্ণসংখ্যা log_facility = LOG_DAEMON;
স্থির bool ratelimit_kmsg = true;

স্থির পূর্ণসংখ্যা console_fd = STDERR_FILENO;
স্থির পূর্ণসংখ্যা console_fd_is_tty = -1; /* tri-state: -1 means don't know */
স্থির পূর্ণসংখ্যা syslog_fd = -EBADF;
স্থির পূর্ণসংখ্যা kmsg_fd = -EBADF;
স্থির পূর্ণসংখ্যা journal_fd = -EBADF;

স্থির bool syslog_is_stream = false;

স্থির পূর্ণসংখ্যা show_color = -1; /* tristate */
স্থির bool show_location = false;
স্থির bool show_time = false;
স্থির bool show_tid = false;
স্থির bool show_executable = false;

স্থির bool upgrade_syslog_to_journal = false;
স্থির bool always_reopen_console = false;
স্থির bool open_when_needed = false;
স্থির bool prohibit_ipc = false;

স্থির thread_local ধ্রুবক অক্ষর *log_prefix = ফাঁকা;

#যদি LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(রাশিমালা)                                                \
        করো {                                                            \
                যদি (_unlikely_(!(রাশিমালা))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } যখন (false)

শূন্য log_prefix_swap(ধ্রুবক অক্ষর **prefix) {
        প্রতিজ্ঞা(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

স্থির শূন্য log_close_console(শূন্য) {
        /* See comment in log_close_journal() */
        (শূন্য) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

স্থির পূর্ণসংখ্যা log_open_console(শূন্য) {

        যদি (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ফেরত 0;
        }

        যদি (console_fd < 3) {
                পূর্ণসংখ্যা fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                যদি (fd < 0)
                        ফেরত fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ফেরত 0;
}

স্থির শূন্য log_close_kmsg(শূন্য) {
        /* See comment in log_close_journal() */
        (শূন্য) safe_close(TAKE_FD(kmsg_fd));
}

স্থির পূর্ণসংখ্যা log_open_kmsg(শূন্য) {

        যদি (kmsg_fd >= 0)
                ফেরত 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        যদি (kmsg_fd < 0)
                ফেরত -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ফেরত 0;
}

স্থির শূন্য log_close_syslog(শূন্য) {
        /* See comment in log_close_journal() */
        (শূন্য) safe_close(TAKE_FD(syslog_fd));
}

স্থির পূর্ণসংখ্যা create_log_socket(পূর্ণসংখ্যা type) {
        কাঠামো timeval tv;
        পূর্ণসংখ্যা fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        যদি (fd < 0)
                ফেরত -errno;

        fd = fd_move_above_stdio(fd);
        (শূন্য) fd_inc_sndbuf(fd, SNDBUF_আকার);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        যদি (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        অন্যথা
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (শূন্য) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, আকার(tv));

        ফেরত fd;
}

স্থির পূর্ণসংখ্যা log_open_syslog(শূন্য) {
        পূর্ণসংখ্যা r;

        যদি (syslog_fd >= 0)
                ফেরত 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        যদি (syslog_fd < 0) {
                r = syslog_fd;
                যাও fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        যদি (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                যদি (syslog_fd < 0) {
                        r = syslog_fd;
                        যাও fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                যদি (r < 0)
                        যাও fail;

                syslog_is_stream = true;
        } অন্যথা
                syslog_is_stream = false;

        ফেরত 0;

fail:
        log_close_syslog();
        ফেরত r;
}

স্থির শূন্য log_close_journal(শূন্য) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (শূন্য) safe_close(TAKE_FD(journal_fd));
}

স্থির পূর্ণসংখ্যা log_open_journal(শূন্য) {
        পূর্ণসংখ্যা r;

        যদি (journal_fd >= 0)
                ফেরত 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        যদি (journal_fd < 0) {
                r = journal_fd;
                যাও fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        যদি (r < 0)
                যাও fail;

        ফেরত 0;

fail:
        log_close_journal();
        ফেরত r;
}

bool stderr_is_journal(শূন্য) {
        _cleanup_free_ অক্ষর *w = ফাঁকা;
        ধ্রুবক অক্ষর *e;
        uint64_t dev, ino;
        কাঠামো stat st;

        e = getenv("JOURNAL_STREAM");
        যদি (!e)
                ফেরত false;

        যদি (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                ফেরত false;
        যদি (!e)
                ফেরত false;

        যদি (safe_atou64(w, &dev) < 0)
                ফেরত false;
        যদি (safe_atou64(e, &ino) < 0)
                ফেরত false;

        যদি (fstat(STDERR_FILENO, &st) < 0)
                ফেরত false;

        ফেরত st.st_dev == dev && st.st_ino == ino;
}

পূর্ণসংখ্যা log_open(শূন্য) {
        পূর্ণসংখ্যা r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        যদি (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                ফেরত 0;
        }

        যদি (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                যদি (!prohibit_ipc) {
                        যদি (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                যদি (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        ফেরত r;
                                }
                        }

                        যদি (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                যদি (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ফেরত r;
                                }
                        }
                }

                যদি (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        যদি (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                ফেরত r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ফেরত log_open_console();
}

শূন্য log_set_target(LogTarget target) {
        প্রতিজ্ঞা(target >= 0);
        প্রতিজ্ঞা(target < _LOG_TARGET_MAX);

        যদি (upgrade_syslog_to_journal) {
                যদি (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                অন্যথা যদি (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

শূন্য log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

শূন্য log_close(শূন্য) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

শূন্য log_forget_fds(শূন্য) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

পূর্ণসংখ্যা log_set_max_level(পূর্ণসংখ্যা level) {
        প্রতিজ্ঞা(level == LOG_NULL || log_level_is_valid(level));

        পূর্ণসংখ্যা old = log_max_level;
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

        ফেরত old;
}

শূন্য log_set_facility(পূর্ণসংখ্যা facility) {
        log_facility = facility;
}

স্থির bool check_console_fd_is_tty(শূন্য) {
        যদি (console_fd < 0)
                ফেরত false;

        যদি (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        ফেরত console_fd_is_tty;
}

স্থির পূর্ণসংখ্যা write_to_console(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *বাফার) {

        স্থির পূর্ণসংখ্যা dumb = -1;

        অক্ষর location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(পূর্ণসংখ্যা) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        কাঠামো iovec iovec[13];
        ধ্রুবক অক্ষর *on = ফাঁকা, *off = ফাঁকা;
        size_t n = 0;

        যদি (console_fd < 0)
                ফেরত 0;

        যদি (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                ফেরত 0;

        যদি (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        যদি (show_time &&
            format_timestamp(header_time, আকার(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        যদি (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        যদি (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        যদি (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ফাঁকা);

        যদি (show_location) {
                ধ্রুবক অক্ষর *lon = "", *loff = "";
                যদি (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (শূন্য) snprintf(location, আকার location, "%s%s:%i%s: ", lon, file, সারি, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        যদি (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        যদি (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(বাফার);
        যদি (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        যদি (writev(console_fd, iovec, n) < 0) {

                যদি (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (শূন্য) log_open_console();
                        যদি (console_fd < 0)
                                ফেরত 0;

                        যদি (writev(console_fd, iovec, n) < 0)
                                ফেরত -errno;
                } অন্যথা
                        ফেরত -errno;
        }

        ফেরত 1;
}

স্থির পূর্ণসংখ্যা write_to_syslog(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *বাফার) {

        অক্ষর header_priority[2 + DECIMAL_STR_MAX(পূর্ণসংখ্যা) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        কাঠামো tm tm;
        পূর্ণসংখ্যা r;

        যদি (syslog_fd < 0)
                ফেরত 0;

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                ফেরত 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        যদি (r < 0)
                ফেরত r;

        যদি (strftime(header_time, আকার(header_time), "%h %e %T ", &tm) <= 0)
                ফেরত -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        কাঠামো iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(বাফার),
        };
        ধ্রুবক কাঠামো msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        যদি (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        জন্য (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                যদি (n < 0)
                        ফেরত -errno;

                যদি (!syslog_is_stream)
                        বিরতি;

                যদি (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        বিরতি;
        }

        ফেরত 1;
}

স্থির পূর্ণসংখ্যা write_to_kmsg(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *বাফার) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        স্থির thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        অক্ষর header_priority[2 + DECIMAL_STR_MAX(পূর্ণসংখ্যা) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        যদি (kmsg_fd < 0)
                ফেরত 0;

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                ফেরত 0;

        যদি (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                যদি (ratelimit_অঙ্ক_dropped(&ratelimit) > 1)
                        ফেরত 0;

                বাফার = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ধ্রুবক কাঠামো iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(বাফার),
                IOVEC_MAKE_STRING("\n"),
        };

        যদি (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ফেরত -errno;

        ফেরত 1;
}

স্থির পূর্ণসংখ্যা log_do_header(
                অক্ষর *header,
                size_t আকার_2,
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *object_field, ধ্রুবক অক্ষর *object,
                ধ্রুবক অক্ষর *extra_field, ধ্রুবক অক্ষর *extra) {
        পূর্ণসংখ্যা r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_মান(error);

        r = snprintf(header, আকার_2,
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
                     সারি ? "CODE_LINE=" : "",
                     সারি ? 1 : 0, সারি, /* %.0d means no output too, special case for 0 */
                     সারি ? "\n" : "",
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
        assert_raw((size_t) r < আকার_2);

        ফেরত 0;
}

স্থির শূন্য log_do_context(কাঠামো iovec *iovec, size_t iovec_len, size_t *n) {
        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n);

        LIST_FOREACH(ll, c, log_context_শীর্ষ()) {
                STRV_FOREACH(s, c->fields) {
                        যদি (*n + 2 >= iovec_len)
                                ফেরত;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                জন্য (size_t i = 0; i < c->n_প্রবেশ_iovec; i++) {
                        যদি (*n + 2 >= iovec_len)
                                ফেরত;

                        iovec[(*n)++] = c->প্রবেশ_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                যদি (c->চাবি && c->মান) {
                        যদি (*n + 3 >= iovec_len)
                                ফেরত;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->চাবি);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->মান);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

স্থির পূর্ণসংখ্যা write_to_journal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *object_field,
                ধ্রুবক অক্ষর *object,
                ধ্রুবক অক্ষর *extra_field,
                ধ্রুবক অক্ষর *extra,
                ধ্রুবক অক্ষর *বাফার) {

        অক্ষর header[LINE_MAX];
        size_t n = 0, iovec_len;
        কাঠামো iovec *iovec;

        যদি (journal_fd < 0)
                ফেরত 0;

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                ফেরত 0;

        যদি (!MUL_SAFE(&iovec_len, log_context_অঙ্ক_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        অন্যথা
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(কাঠামো iovec, iovec_len);

        log_do_header(header, আকার(header), level, error, file, সারি, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        যদি (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(বাফার);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ধ্রুবক কাঠামো msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        যদি (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ফেরত -errno;

        ফেরত 1;
}

পূর্ণসংখ্যা log_dispatch_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *object_field,
                ধ্রুবক অক্ষর *object,
                ধ্রুবক অক্ষর *extra_field,
                ধ্রুবক অক্ষর *extra,
                অক্ষর *বাফার) {

        assert_raw(বাফার);

        যদি (log_target == LOG_TARGET_NULL)
                ফেরত -ERRNO_মান(error);

        /* Patch in LOG_DAEMON facility if necessary */
        যদি (LOG_FAC(level) == 0)
                level |= log_facility;

        যদি (open_when_needed)
                (শূন্য) log_open();

        করো {
                অক্ষর *e;
                পূর্ণসংখ্যা k = 0;

                বাফার += strspn(বাফার, NEWLINE);

                যদি (বাফার[0] == 0)
                        বিরতি;

                যদি ((e = strpbrk(বাফার, NEWLINE)))
                        *(e++) = 0;

                যদি (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, সারি, func, object_field, object, extra_field, extra, বাফার);
                        যদি (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                যদি (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, সারি, func, বাফার);
                        যদি (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                যদি (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        যদি (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, সারি, func, বাফার);
                        যদি (k < 0) {
                                log_close_kmsg();
                                (শূন্য) log_open_console();
                        }
                }

                যদি (k <= 0)
                        (শূন্য) write_to_console(level, error, file, সারি, func, বাফার);

                বাফার = e;
        } যখন (বাফার);

        যদি (open_when_needed)
                log_close();

        ফেরত -ERRNO_মান(error);
}

পূর্ণসংখ্যা log_dump_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                অক্ষর *বাফার) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        যদি (_likely_(LOG_PRI(level) > log_max_level))
                ফেরত -ERRNO_মান(error);

        ফেরত log_dispatch_internal(level, error, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, বাফার);
}

পূর্ণসংখ্যা log_internalv(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *format,
                va_list ap) {

        যদি (_likely_(LOG_PRI(level) > log_max_level))
                ফেরত -ERRNO_মান(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        অক্ষর বাফার[LINE_MAX];
        LOCAL_ERRNO(ERRNO_মান(error));

        (শূন্য) vsnprintf(বাফার, আকার বাফার, format, ap);

        ফেরত log_dispatch_internal(level, error, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, বাফার);
}

পূর্ণসংখ্যা log_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *format, ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, format);
        r = log_internalv(level, error, file, সারি, func, format, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা log_object_internalv(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *object_field,
                ধ্রুবক অক্ষর *object,
                ধ্রুবক অক্ষর *extra_field,
                ধ্রুবক অক্ষর *extra,
                ধ্রুবক অক্ষর *format,
                va_list ap) {

        অক্ষর *বাফার, *b;

        যদি (_likely_(LOG_PRI(level) > log_max_level))
                ফেরত -ERRNO_মান(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_মান(error));

        LOG_SET_PREFIX(object);

        b = বাফার = newa(অক্ষর, LINE_MAX);
        (শূন্য) vsnprintf(b, LINE_MAX, format, ap);

        ফেরত log_dispatch_internal(level, error, file, সারি, func,
                                     object_field, object, extra_field, extra, বাফার);
}

পূর্ণসংখ্যা log_object_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *object_field,
                ধ্রুবক অক্ষর *object,
                ধ্রুবক অক্ষর *extra_field,
                ধ্রুবক অক্ষর *extra,
                ধ্রুবক অক্ষর *format, ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, সারি, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা log_oom_internal(পূর্ণসংখ্যা level, ধ্রুবক অক্ষর *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *func) {
        ফেরত log_internal(level, ENOMEM, file, সারি, func, "Out of memory.");
}

পূর্ণসংখ্যা log_format_iovec(
                কাঠামো iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *format,
                va_list ap) {

        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n);

        যখন (format && *n + 1 < iovec_len) {
                va_list aq;
                অক্ষর *m;
                পূর্ণসংখ্যা r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_মান(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                যদি (r < 0)
                        ফেরত -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                যদি (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, অক্ষর *);
        }
        ফেরত 0;
}

পূর্ণসংখ্যা log_struct_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *format, ...) {

        অক্ষর buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        যদি (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ফেরত -ERRNO_মান(error);

        যদি (LOG_FAC(level) == 0)
                level |= log_facility;

        যদি (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                যদি (open_when_needed)
                        log_open_journal();

                যদি (journal_fd >= 0) {
                        অক্ষর header[LINE_MAX];
                        কাঠামো iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        পূর্ণসংখ্যা r;
                        bool fallback = false;

                        যদি (!MUL_SAFE(&iovec_len, log_context_অঙ্ক_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        অন্যথা
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(কাঠামো iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, আকার(header), level, error, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        যদি (r < 0)
                                fallback = true;
                        অন্যথা {
                                log_do_context(iovec, iovec_len, &n);

                                ধ্রুবক কাঠামো msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (শূন্য) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        জন্য (size_t i = 1; i < m; i += 2)
                                মুক্ত(iovec[i].iov_base);

                        যদি (!fallback) {
                                যদি (open_when_needed)
                                        log_close();

                                ফেরত -ERRNO_মান(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        যখন (format) {
                va_list aq;

                errno = ERRNO_মান(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (শূন্য) vsnprintf(buf, আকার buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                যদি (startswith(buf, "MESSAGE=")) {
                        found = true;
                        বিরতি;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, অক্ষর *);
        }
        va_end(ap);

        যদি (!found) {
                যদি (open_when_needed)
                        log_close();

                ফেরত -ERRNO_মান(error);
        }

        ফেরত log_dispatch_internal(level, error, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, buf + 8);
}

পূর্ণসংখ্যা log_struct_iovec_internal(
                পূর্ণসংখ্যা level,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক কাঠামো iovec প্রবেশ_iovec[],
                size_t n_প্রবেশ_iovec) {

        PROTECT_ERRNO;

        যদি (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ফেরত -ERRNO_মান(error);

        যদি (LOG_FAC(level) == 0)
                level |= log_facility;

        যদি (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                অক্ষর header[LINE_MAX];
                কাঠামো iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_প্রবেশ_iovec_expanded;

                যদি (!MUL_SAFE(&n_প্রবেশ_iovec_expanded, n_প্রবেশ_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_অঙ্ক_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_প্রবেশ_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                অন্যথা
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(কাঠামো iovec, iovec_len);

                log_do_header(header, আকার(header), level, error, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                জন্য (size_t i = 0; i < n_প্রবেশ_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = প্রবেশ_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ধ্রুবক কাঠামো msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                যদি (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ফেরত -ERRNO_মান(error);
        }

        জন্য (size_t i = 0; i < n_প্রবেশ_iovec; i++)
                যদি (memory_startswith(প্রবেশ_iovec[i].iov_base, প্রবেশ_iovec[i].iov_len, "MESSAGE=")) {
                        অক্ষর *m;

                        m = strndupa_safe((অক্ষর*) প্রবেশ_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          প্রবেশ_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ফেরত log_dispatch_internal(level, error, file, সারি, func, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, m);
                }

        /* Couldn't find MESSAGE=. */
        ফেরত -ERRNO_মান(error);
}

পূর্ণসংখ্যা log_set_target_from_string(ধ্রুবক অক্ষর *e) {
        LogTarget t;

        t = log_target_from_string(e);
        যদি (t < 0)
                ফেরত t;

        log_set_target(t);
        ফেরত 0;
}

পূর্ণসংখ্যা log_set_max_level_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *word = ফাঁকা, *prefix = ফাঁকা;
                LogTarget target;
                ধ্রুবক অক্ষর *colon;

                r = extract_first_word(&e, &word, ",", 0);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                colon = strchr(word, ':');
                যদি (!colon) {
                        r = log_level_from_string(word);
                        যদি (r < 0)
                                ফেরত r;

                        log_set_max_level(r);
                        চালু;
                }

                prefix = strndup(word, colon - word);
                যদি (!prefix)
                        ফেরত -ENOMEM;

                target = log_target_from_string(prefix);
                যদি (target < 0)
                        ফেরত target;

                যদি (target >= _LOG_TARGET_SINGLE_MAX)
                        ফেরত -EINVAL;

                r = log_level_from_string(colon + 1);
                যদি (r < 0)
                        ফেরত r;

                log_target_max_level[target] = r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা log_max_levels_to_string(পূর্ণসংখ্যা level, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = log_level_to_string_alloc(level, &s);
        যদি (r < 0)
                ফেরত r;

        জন্য (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ অক্ষর *l = ফাঁকা;

                যদি (log_target_max_level[target] == INT_MAX)
                        চালু;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                যদি (r < 0)
                        ফেরত r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা log_set_ratelimit_kmsg_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        r = parse_boolean(e);
        যদি (r < 0)
                ফেরত r;

        ratelimit_kmsg = r;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা parse_proc_cmdline_item(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান, শূন্য *তথ্য) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        যদি (streq(চাবি, "debug") && !মান)
                log_set_max_level(LOG_DEBUG);

        অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_target")) {

                যদি (proc_cmdline_মান_missing(চাবি, মান))
                        ফেরত 0;

                যদি (log_set_target_from_string(মান) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_level")) {

                যদি (proc_cmdline_মান_missing(চাবি, মান))
                        ফেরত 0;

                যদি (log_set_max_level_from_string(মান) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_color")) {

                যদি (log_show_color_from_string(মান ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_location")) {

                যদি (log_show_location_from_string(মান ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_tid")) {

                যদি (log_show_tid_from_string(মান ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_time")) {

                যদি (log_show_time_from_string(মান ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_ratelimit_kmsg")) {

                যদি (log_set_ratelimit_kmsg_from_string(মান ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", মান);
        }

        ফেরত 0;
}

স্থির bool should_parse_proc_cmdline(শূন্য) {
        /* PID1 always reads the kernel command line. */
        যদি (getpid_cached() == 1)
                ফেরত true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ফেরত invoked_by_systemd();
}

শূন্য log_parse_environment_variables(শূন্য) {
        ধ্রুবক অক্ষর *e;
        পূর্ণসংখ্যা r;

        e = getenv("SYSTEMD_LOG_TARGET");
        যদি (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        যদি (e) {
                r = log_set_max_level_from_string(e);
                যদি (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } অন্যথা {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                যদি (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                অন্যথা যদি (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        যদি (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        যদি (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        যদি (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        যদি (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        যদি (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        যদি (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

শূন্য log_parse_environment(শূন্য) {
        /* Do not call from library code. */

        যদি (should_parse_proc_cmdline())
                (শূন্য) proc_cmdline_parse(parse_proc_cmdline_item, ফাঁকা, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(শূন্য) {
        ফেরত log_target;
}

শূন্য log_settle_target(শূন্য) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        যদি (t != LOG_TARGET_AUTO)
                ফেরত;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

পূর্ণসংখ্যা log_get_max_level(শূন্য) {
        ফেরত log_max_level;
}

পূর্ণসংখ্যা log_get_target_max_level(LogTarget target) {
        প্রতিজ্ঞা(target >= 0);
        প্রতিজ্ঞা(target < _LOG_TARGET_SINGLE_MAX);
        ফেরত log_target_max_level[target];
}

শূন্য log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(শূন্য) {
        ফেরত show_color > 0; /* Defaults to false. */
}

শূন্য log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(শূন্য) {
        ফেরত show_location;
}

শূন্য log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(শূন্য) {
        ফেরত show_time;
}

শূন্য log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(শূন্য) {
        ফেরত show_tid;
}

শূন্য log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(শূন্য) {
        ফেরত show_executable;
}

পূর্ণসংখ্যা log_show_color_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        r = parse_boolean(e);
        যদি (r < 0)
                ফেরত r;

        log_show_color(r);
        ফেরত 0;
}

পূর্ণসংখ্যা log_show_location_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        r = parse_boolean(e);
        যদি (r < 0)
                ফেরত r;

        log_show_location(r);
        ফেরত 0;
}

পূর্ণসংখ্যা log_show_time_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        r = parse_boolean(e);
        যদি (r < 0)
                ফেরত r;

        log_show_time(r);
        ফেরত 0;
}

পূর্ণসংখ্যা log_show_tid_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        r = parse_boolean(e);
        যদি (r < 0)
                ফেরত r;

        log_show_tid(r);
        ফেরত 0;
}

পূর্ণসংখ্যা log_show_executable_from_string(ধ্রুবক অক্ষর *e) {
        পূর্ণসংখ্যা r;

        r = parse_boolean(e);
        যদি (r < 0)
                ফেরত r;

        log_show_executable(r);
        ফেরত 0;
}

bool log_on_console(শূন্য) {
        যদি (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                ফেরত true;

        ফেরত syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক log_target_table[_LOG_TARGET_MAX] = {
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

শূন্য log_received_signal(পূর্ণসংখ্যা level, ধ্রুবক কাঠামো signalfd_siginfo *si) {
        প্রতিজ্ঞা(si);

        যদি (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ অক্ষর *p = ফাঁকা;

                (শূন্য) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } অন্যথা
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

শূন্য set_log_syntax_callback(log_syntax_callback_t cb, শূন্য *userdata) {
        প্রতিজ্ঞা(!log_syntax_callback || !cb);
        প্রতিজ্ঞা(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

পূর্ণসংখ্যা log_syntax_internal(
                ধ্রুবক অক্ষর *unit,
                পূর্ণসংখ্যা level,
                ধ্রুবক অক্ষর *config_file,
                unsigned config_সারি,
                পূর্ণসংখ্যা error,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *format, ...) {

        PROTECT_ERRNO;

        যদি (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        যদি (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ফেরত -ERRNO_মান(error);

        অক্ষর বাফার[LINE_MAX];
        va_list ap;
        ধ্রুবক অক্ষর *unit_fmt = ফাঁকা;

        errno = ERRNO_মান(error);

        va_start(ap, format);
        (শূন্য) vsnprintf(বাফার, আকার বাফার, format, ap);
        va_end(ap);

        যদি (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        যদি (config_file) {
                যদি (config_সারি > 0)
                        ফেরত log_struct_internal(
                                        level,
                                        error,
                                        file, সারি, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_সারি),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_সারি, বাফার),
                                        unit_fmt, unit,
                                        ফাঁকা);
                অন্যথা
                        ফেরত log_struct_internal(
                                        level,
                                        error,
                                        file, সারি, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, বাফার),
                                        unit_fmt, unit,
                                        ফাঁকা);
        } অন্যথা যদি (unit)
                ফেরত log_struct_internal(
                                level,
                                error,
                                file, সারি, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, বাফার),
                                unit_fmt, unit,
                                ফাঁকা);
        অন্যথা
                ফেরত log_struct_internal(
                                level,
                                error,
                                file, সারি, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", বাফার),
                                ফাঁকা);
}

পূর্ণসংখ্যা log_syntax_invalid_utf8_internal(
                ধ্রুবক অক্ষর *unit,
                পূর্ণসংখ্যা level,
                ধ্রুবক অক্ষর *config_file,
                unsigned config_সারি,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ অক্ষর *p = ফাঁকা;

        যদি (rvalue)
                p = utf8_escape_invalid(rvalue);

        ফেরত log_syntax_internal(unit, level, config_file, config_সারি,
                                   SYNTHETIC_ERRNO(EINVAL), file, সারি, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

পূর্ণসংখ্যা log_syntax_parse_error_internal(
                ধ্রুবক অক্ষর *unit,
                ধ্রুবক অক্ষর *config_file,
                unsigned config_সারি,
                পূর্ণসংখ্যা error,
                bool critical,
                ধ্রুবক অক্ষর *file,
                পূর্ণসংখ্যা সারি,
                ধ্রুবক অক্ষর *func,
                ধ্রুবক অক্ষর *lvalue,
                ধ্রুবক অক্ষর *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ অক্ষর *escaped = ফাঁকা;

        /* OOM is always handled as critical. */
        যদি (ERRNO_মান(error) == ENOMEM)
                ফেরত log_oom_internal(LOG_ERR, file, সারি, func);

        যদি (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                যদি (!escaped)
                        rvalue = "(oom)";
                অন্যথা
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_সারি, error,
                            file, সারি, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ফেরত critical ? -ERRNO_মান(error) : 0;
}

শূন্য log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        যদি (b) {
                যদি (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                অন্যথা যদি (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

শূন্য log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

শূন্য log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

শূন্য log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

পূর্ণসংখ্যা log_emergency_level(শূন্য) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ফেরত getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

পূর্ণসংখ্যা log_dup_console(শূন্য) {
        পূর্ণসংখ্যা copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        যদি (console_fd < 0 || console_fd >= 3)
                ফেরত 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        যদি (copy < 0)
                ফেরত -errno;

        console_fd = copy;
        ফেরত 0;
}

শূন্য log_setup(শূন্য) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (শূন্য) log_open();
        যদি (log_on_console() && show_color < 0)
                log_show_color(true);
}

ধ্রুবক অক্ষর* _log_set_prefix(ধ্রুবক অক্ষর *prefix, bool force) {
        ধ্রুবক অক্ষর *old = log_prefix;

        যদি (prefix || force)
                log_prefix = prefix;

        ফেরত old;
}
