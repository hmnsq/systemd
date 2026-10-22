/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <sys/signalfd.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <sys/uio.h>
#অন্তর্ভাৱ <threads.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "sd-messages.h"

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "ansi-color.h"
#অন্তর্ভাৱ "argv-util.h"
#অন্তর্ভাৱ "env-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "format-util.h"
#অন্তর্ভাৱ "iovec-util.h"
#অন্তর্ভাৱ "list.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "log-context.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "proc-cmdline.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "ratelimit.h"
#অন্তর্ভাৱ "signal-util.h"
#অন্তর্ভাৱ "socket-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-table.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "syslog-util.h"
#অন্তর্ভাৱ "terminal-util.h"
#অন্তর্ভাৱ "time-util.h"
#অন্তর্ভাৱ "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

স্থির log_syntax_callback_t log_syntax_callback = রিক্ত;
স্থির শূন্য *log_syntax_callback_userdata = রিক্ত;

স্থির LogTarget log_target = LOG_TARGET_CONSOLE;
স্থির পূর্ণাঙ্ক log_max_level = LOG_INFO;
স্থির পূর্ণাঙ্ক log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
স্থির পূর্ণাঙ্ক log_facility = LOG_DAEMON;
স্থির bool ratelimit_kmsg = true;

স্থির পূর্ণাঙ্ক console_fd = STDERR_FILENO;
স্থির পূর্ণাঙ্ক console_fd_is_tty = -1; /* tri-state: -1 means don't know */
স্থির পূর্ণাঙ্ক syslog_fd = -EBADF;
স্থির পূর্ণাঙ্ক kmsg_fd = -EBADF;
স্থির পূর্ণাঙ্ক journal_fd = -EBADF;

স্থির bool syslog_is_stream = false;

স্থির পূর্ণাঙ্ক show_color = -1; /* tristate */
স্থির bool show_location = false;
স্থির bool show_time = false;
স্থির bool show_tid = false;
স্থির bool show_executable = false;

স্থির bool upgrade_syslog_to_journal = false;
স্থির bool always_reopen_console = false;
স্থির bool open_when_needed = false;
স্থির bool prohibit_ipc = false;

স্থির thread_local ধ্রুৱ ৱর্ণ *log_prefix = রিক্ত;

#যদি LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(ৰাশিমালা)                                                \
        কৰা {                                                            \
                যদি (_unlikely_(!(ৰাশিমালা))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } যেতিয়ালৈ (false)

শূন্য log_prefix_swap(ধ্রুৱ ৱর্ণ **prefix) {
        প্রতিজ্ঞা(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

স্থির শূন্য log_close_console(শূন্য) {
        /* See comment in log_close_journal() */
        (শূন্য) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

স্থির পূর্ণাঙ্ক log_open_console(শূন্য) {

        যদি (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ঘূৰাই 0;
        }

        যদি (console_fd < 3) {
                পূর্ণাঙ্ক fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                যদি (fd < 0)
                        ঘূৰাই fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ঘূৰাই 0;
}

স্থির শূন্য log_close_kmsg(শূন্য) {
        /* See comment in log_close_journal() */
        (শূন্য) safe_close(TAKE_FD(kmsg_fd));
}

স্থির পূর্ণাঙ্ক log_open_kmsg(শূন্য) {

        যদি (kmsg_fd >= 0)
                ঘূৰাই 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        যদি (kmsg_fd < 0)
                ঘূৰাই -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ঘূৰাই 0;
}

স্থির শূন্য log_close_syslog(শূন্য) {
        /* See comment in log_close_journal() */
        (শূন্য) safe_close(TAKE_FD(syslog_fd));
}

স্থির পূর্ণাঙ্ক create_log_socket(পূর্ণাঙ্ক type) {
        সংরচনা timeval tv;
        পূর্ণাঙ্ক fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        যদি (fd < 0)
                ঘূৰাই -errno;

        fd = fd_move_above_stdio(fd);
        (শূন্য) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        যদি (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        অন্যথা
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (শূন্য) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, পরিমাণ(tv));

        ঘূৰাই fd;
}

স্থির পূর্ণাঙ্ক log_open_syslog(শূন্য) {
        পূর্ণাঙ্ক r;

        যদি (syslog_fd >= 0)
                ঘূৰাই 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        যদি (syslog_fd < 0) {
                r = syslog_fd;
                গচ্ছ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        যদি (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                যদি (syslog_fd < 0) {
                        r = syslog_fd;
                        গচ্ছ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                যদি (r < 0)
                        গচ্ছ fail;

                syslog_is_stream = true;
        } অন্যথা
                syslog_is_stream = false;

        ঘূৰাই 0;

fail:
        log_close_syslog();
        ঘূৰাই r;
}

স্থির শূন্য log_close_journal(শূন্য) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (শূন্য) safe_close(TAKE_FD(journal_fd));
}

স্থির পূর্ণাঙ্ক log_open_journal(শূন্য) {
        পূর্ণাঙ্ক r;

        যদি (journal_fd >= 0)
                ঘূৰাই 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        যদি (journal_fd < 0) {
                r = journal_fd;
                গচ্ছ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        যদি (r < 0)
                গচ্ছ fail;

        ঘূৰাই 0;

fail:
        log_close_journal();
        ঘূৰাই r;
}

bool stderr_is_journal(শূন্য) {
        _cleanup_free_ ৱর্ণ *w = রিক্ত;
        ধ্রুৱ ৱর্ণ *e;
        uint64_t dev, ino;
        সংরচনা stat st;

        e = getenv("JOURNAL_STREAM");
        যদি (!e)
                ঘূৰাই false;

        যদি (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                ঘূৰাই false;
        যদি (!e)
                ঘূৰাই false;

        যদি (safe_atou64(w, &dev) < 0)
                ঘূৰাই false;
        যদি (safe_atou64(e, &ino) < 0)
                ঘূৰাই false;

        যদি (fstat(STDERR_FILENO, &st) < 0)
                ঘূৰাই false;

        ঘূৰাই st.st_dev == dev && st.st_ino == ino;
}

পূর্ণাঙ্ক log_open(শূন্য) {
        পূর্ণাঙ্ক r;

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
                ঘূৰাই 0;
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
                                        ঘূৰাই r;
                                }
                        }

                        যদি (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                যদি (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ঘূৰাই r;
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
                                ঘূৰাই r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ঘূৰাই log_open_console();
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

পূর্ণাঙ্ক log_set_max_level(পূর্ণাঙ্ক level) {
        প্রতিজ্ঞা(level == LOG_NULL || log_level_is_valid(level));

        পূর্ণাঙ্ক old = log_max_level;
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

        ঘূৰাই old;
}

শূন্য log_set_facility(পূর্ণাঙ্ক facility) {
        log_facility = facility;
}

স্থির bool check_console_fd_is_tty(শূন্য) {
        যদি (console_fd < 0)
                ঘূৰাই false;

        যদি (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        ঘূৰাই console_fd_is_tty;
}

স্থির পূর্ণাঙ্ক write_to_console(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *buffer) {

        স্থির পূর্ণাঙ্ক dumb = -1;

        ৱর্ণ location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(পূর্ণাঙ্ক) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        সংরচনা iovec iovec[13];
        ধ্রুৱ ৱর্ণ *on = রিক্ত, *off = রিক্ত;
        size_t n = 0;

        যদি (console_fd < 0)
                ঘূৰাই 0;

        যদি (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                ঘূৰাই 0;

        যদি (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        যদি (show_time &&
            format_timestamp(header_time, পরিমাণ(header_time), now(CLOCK_REALTIME))) {
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
                get_log_colors(LOG_PRI(level), &on, &off, রিক্ত);

        যদি (show_location) {
                ধ্রুৱ ৱর্ণ *lon = "", *loff = "";
                যদি (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (শূন্য) snprintf(location, পরিমাণ location, "%s%s:%i%s: ", lon, file, সাৰি, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        যদি (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        যদি (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
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
                                ঘূৰাই 0;

                        যদি (writev(console_fd, iovec, n) < 0)
                                ঘূৰাই -errno;
                } অন্যথা
                        ঘূৰাই -errno;
        }

        ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক write_to_syslog(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *buffer) {

        ৱর্ণ header_priority[2 + DECIMAL_STR_MAX(পূর্ণাঙ্ক) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        সংরচনা tm tm;
        পূর্ণাঙ্ক r;

        যদি (syslog_fd < 0)
                ঘূৰাই 0;

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                ঘূৰাই 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (strftime(header_time, পরিমাণ(header_time), "%h %e %T ", &tm) <= 0)
                ঘূৰাই -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        সংরচনা iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        ধ্রুৱ সংরচনা msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        যদি (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        বাবে (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                যদি (n < 0)
                        ঘূৰাই -errno;

                যদি (!syslog_is_stream)
                        ভঙ্গ;

                যদি (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        ভঙ্গ;
        }

        ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক write_to_kmsg(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        স্থির thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        ৱর্ণ header_priority[2 + DECIMAL_STR_MAX(পূর্ণাঙ্ক) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        যদি (kmsg_fd < 0)
                ঘূৰাই 0;

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                ঘূৰাই 0;

        যদি (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                যদি (ratelimit_অঙ্ক_dropped(&ratelimit) > 1)
                        ঘূৰাই 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ধ্রুৱ সংরচনা iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        যদি (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ঘূৰাই -errno;

        ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক log_do_header(
                ৱর্ণ *header,
                size_t size,
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *object_field, ধ্রুৱ ৱর্ণ *object,
                ধ্রুৱ ৱর্ণ *extra_field, ধ্রুৱ ৱর্ণ *extra) {
        পূর্ণাঙ্ক r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_মান(error);

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
                     সাৰি ? "CODE_LINE=" : "",
                     সাৰি ? 1 : 0, সাৰি, /* %.0d means no output too, special case for 0 */
                     সাৰি ? "\n" : "",
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

        ঘূৰাই 0;
}

স্থির শূন্য log_do_context(সংরচনা iovec *iovec, size_t iovec_len, size_t *n) {
        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        যদি (*n + 2 >= iovec_len)
                                ঘূৰাই;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                বাবে (size_t i = 0; i < c->n_প্ৰবেশ_iovec; i++) {
                        যদি (*n + 2 >= iovec_len)
                                ঘূৰাই;

                        iovec[(*n)++] = c->প্ৰবেশ_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                যদি (c->চাবি && c->মান) {
                        যদি (*n + 3 >= iovec_len)
                                ঘূৰাই;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->চাবি);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->মান);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

স্থির পূর্ণাঙ্ক write_to_journal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *object_field,
                ধ্রুৱ ৱর্ণ *object,
                ধ্রুৱ ৱর্ণ *extra_field,
                ধ্রুৱ ৱর্ণ *extra,
                ধ্রুৱ ৱর্ণ *buffer) {

        ৱর্ণ header[LINE_MAX];
        size_t n = 0, iovec_len;
        সংরচনা iovec *iovec;

        যদি (journal_fd < 0)
                ঘূৰাই 0;

        যদি (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                ঘূৰাই 0;

        যদি (!MUL_SAFE(&iovec_len, log_context_অঙ্ক_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        অন্যথা
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(সংরচনা iovec, iovec_len);

        log_do_header(header, পরিমাণ(header), level, error, file, সাৰি, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        যদি (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ধ্রুৱ সংরচনা msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        যদি (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ঘূৰাই -errno;

        ঘূৰাই 1;
}

পূর্ণাঙ্ক log_dispatch_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *object_field,
                ধ্রুৱ ৱর্ণ *object,
                ধ্রুৱ ৱর্ণ *extra_field,
                ধ্রুৱ ৱর্ণ *extra,
                ৱর্ণ *buffer) {

        assert_raw(buffer);

        যদি (log_target == LOG_TARGET_NULL)
                ঘূৰাই -ERRNO_মান(error);

        /* Patch in LOG_DAEMON facility if necessary */
        যদি (LOG_FAC(level) == 0)
                level |= log_facility;

        যদি (open_when_needed)
                (শূন্য) log_open();

        কৰা {
                ৱর্ণ *e;
                পূর্ণাঙ্ক k = 0;

                buffer += strspn(buffer, NEWLINE);

                যদি (buffer[0] == 0)
                        ভঙ্গ;

                যদি ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                যদি (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, সাৰি, func, object_field, object, extra_field, extra, buffer);
                        যদি (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                যদি (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, সাৰি, func, buffer);
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

                        k = write_to_kmsg(level, error, file, সাৰি, func, buffer);
                        যদি (k < 0) {
                                log_close_kmsg();
                                (শূন্য) log_open_console();
                        }
                }

                যদি (k <= 0)
                        (শূন্য) write_to_console(level, error, file, সাৰি, func, buffer);

                buffer = e;
        } যেতিয়ালৈ (buffer);

        যদি (open_when_needed)
                log_close();

        ঘূৰাই -ERRNO_মান(error);
}

পূর্ণাঙ্ক log_dump_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ৱর্ণ *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        যদি (_likely_(LOG_PRI(level) > log_max_level))
                ঘূৰাই -ERRNO_মান(error);

        ঘূৰাই log_dispatch_internal(level, error, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত, buffer);
}

পূর্ণাঙ্ক log_internalv(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *format,
                va_list ap) {

        যদি (_likely_(LOG_PRI(level) > log_max_level))
                ঘূৰাই -ERRNO_মান(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        ৱর্ণ buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_মান(error));

        (শূন্য) vsnprintf(buffer, পরিমাণ buffer, format, ap);

        ঘূৰাই log_dispatch_internal(level, error, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত, buffer);
}

পূর্ণাঙ্ক log_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *format, ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = log_internalv(level, error, file, সাৰি, func, format, ap);
        va_end(ap);

        ঘূৰাই r;
}

পূর্ণাঙ্ক log_object_internalv(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *object_field,
                ধ্রুৱ ৱর্ণ *object,
                ধ্রুৱ ৱর্ণ *extra_field,
                ধ্রুৱ ৱর্ণ *extra,
                ধ্রুৱ ৱর্ণ *format,
                va_list ap) {

        ৱর্ণ *buffer, *b;

        যদি (_likely_(LOG_PRI(level) > log_max_level))
                ঘূৰাই -ERRNO_মান(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_মান(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(ৱর্ণ, LINE_MAX);
        (শূন্য) vsnprintf(b, LINE_MAX, format, ap);

        ঘূৰাই log_dispatch_internal(level, error, file, সাৰি, func,
                                     object_field, object, extra_field, extra, buffer);
}

পূর্ণাঙ্ক log_object_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *object_field,
                ধ্রুৱ ৱর্ণ *object,
                ধ্রুৱ ৱর্ণ *extra_field,
                ধ্রুৱ ৱর্ণ *extra,
                ধ্রুৱ ৱর্ণ *format, ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, সাৰি, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        ঘূৰাই r;
}

পূর্ণাঙ্ক log_oom_internal(পূর্ণাঙ্ক level, ধ্রুৱ ৱর্ণ *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *func) {
        ঘূৰাই log_internal(level, ENOMEM, file, সাৰি, func, "Out of memory.");
}

পূর্ণাঙ্ক log_format_iovec(
                সংরচনা iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *format,
                va_list ap) {

        প্রতিজ্ঞা(iovec);
        প্রতিজ্ঞা(n);

        যেতিয়ালৈ (format && *n + 1 < iovec_len) {
                va_list aq;
                ৱর্ণ *m;
                পূর্ণাঙ্ক r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_মান(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                যদি (r < 0)
                        ঘূৰাই -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                যদি (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, ৱর্ণ *);
        }
        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_struct_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *format, ...) {

        ৱর্ণ buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        যদি (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ঘূৰাই -ERRNO_মান(error);

        যদি (LOG_FAC(level) == 0)
                level |= log_facility;

        যদি (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                যদি (open_when_needed)
                        log_open_journal();

                যদি (journal_fd >= 0) {
                        ৱর্ণ header[LINE_MAX];
                        সংরচনা iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        পূর্ণাঙ্ক r;
                        bool fallback = false;

                        যদি (!MUL_SAFE(&iovec_len, log_context_অঙ্ক_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        অন্যথা
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(সংরচনা iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, পরিমাণ(header), level, error, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত);
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

                                ধ্রুৱ সংরচনা msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (শূন্য) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        বাবে (size_t i = 1; i < m; i += 2)
                                মুক্ত(iovec[i].iov_base);

                        যদি (!fallback) {
                                যদি (open_when_needed)
                                        log_close();

                                ঘূৰাই -ERRNO_মান(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        যেতিয়ালৈ (format) {
                va_list aq;

                errno = ERRNO_মান(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (শূন্য) vsnprintf(buf, পরিমাণ buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                যদি (startswith(buf, "MESSAGE=")) {
                        found = true;
                        ভঙ্গ;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, ৱর্ণ *);
        }
        va_end(ap);

        যদি (!found) {
                যদি (open_when_needed)
                        log_close();

                ঘূৰাই -ERRNO_মান(error);
        }

        ঘূৰাই log_dispatch_internal(level, error, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত, buf + 8);
}

পূর্ণাঙ্ক log_struct_iovec_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ সংরচনা iovec প্ৰবেশ_iovec[],
                size_t n_প্ৰবেশ_iovec) {

        PROTECT_ERRNO;

        যদি (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ঘূৰাই -ERRNO_মান(error);

        যদি (LOG_FAC(level) == 0)
                level |= log_facility;

        যদি (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                ৱর্ণ header[LINE_MAX];
                সংরচনা iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_প্ৰবেশ_iovec_expanded;

                যদি (!MUL_SAFE(&n_প্ৰবেশ_iovec_expanded, n_প্ৰবেশ_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_অঙ্ক_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_প্ৰবেশ_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                অন্যথা
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(সংরচনা iovec, iovec_len);

                log_do_header(header, পরিমাণ(header), level, error, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                বাবে (size_t i = 0; i < n_প্ৰবেশ_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = প্ৰবেশ_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ধ্রুৱ সংরচনা msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                যদি (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ঘূৰাই -ERRNO_মান(error);
        }

        বাবে (size_t i = 0; i < n_প্ৰবেশ_iovec; i++)
                যদি (memory_startswith(প্ৰবেশ_iovec[i].iov_base, প্ৰবেশ_iovec[i].iov_len, "MESSAGE=")) {
                        ৱর্ণ *m;

                        m = strndupa_safe((ৱর্ণ*) প্ৰবেশ_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          প্ৰবেশ_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ঘূৰাই log_dispatch_internal(level, error, file, সাৰি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত, m);
                }

        /* Couldn't find MESSAGE=. */
        ঘূৰাই -ERRNO_মান(error);
}

পূর্ণাঙ্ক log_set_target_from_string(ধ্রুৱ ৱর্ণ *e) {
        LogTarget t;

        t = log_target_from_string(e);
        যদি (t < 0)
                ঘূৰাই t;

        log_set_target(t);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_set_max_level_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *word = রিক্ত, *prefix = রিক্ত;
                LogTarget target;
                ধ্রুৱ ৱর্ণ *colon;

                r = extract_first_word(&e, &word, ",", 0);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ভঙ্গ;

                colon = strchr(word, ':');
                যদি (!colon) {
                        r = log_level_from_string(word);
                        যদি (r < 0)
                                ঘূৰাই r;

                        log_set_max_level(r);
                        অনুৱর্ত;
                }

                prefix = strndup(word, colon - word);
                যদি (!prefix)
                        ঘূৰাই -ENOMEM;

                target = log_target_from_string(prefix);
                যদি (target < 0)
                        ঘূৰাই target;

                যদি (target >= _LOG_TARGET_SINGLE_MAX)
                        ঘূৰাই -EINVAL;

                r = log_level_from_string(colon + 1);
                যদি (r < 0)
                        ঘূৰাই r;

                log_target_max_level[target] = r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_max_levels_to_string(পূর্ণাঙ্ক level, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = log_level_to_string_alloc(level, &s);
        যদি (r < 0)
                ঘূৰাই r;

        বাবে (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ ৱর্ণ *l = রিক্ত;

                যদি (log_target_max_level[target] == INT_MAX)
                        অনুৱর্ত;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                যদি (r < 0)
                        ঘূৰাই r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(s);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক log_set_ratelimit_kmsg_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        r = parse_boolean(e);
        যদি (r < 0)
                ঘূৰাই r;

        ratelimit_kmsg = r;
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক parse_proc_cmdline_item(ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান, শূন্য *data) {

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
                        ঘূৰাই 0;

                যদি (log_set_target_from_string(মান) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", মান);

        } অন্যথা যদি (proc_cmdline_চাবি_streq(চাবি, "systemd.log_level")) {

                যদি (proc_cmdline_মান_missing(চাবি, মান))
                        ঘূৰাই 0;

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

        ঘূৰাই 0;
}

স্থির bool should_parse_proc_cmdline(শূন্য) {
        /* PID1 always reads the kernel command line. */
        যদি (getpid_cached() == 1)
                ঘূৰাই true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ঘূৰাই invoked_by_systemd();
}

শূন্য log_parse_environment_variables(শূন্য) {
        ধ্রুৱ ৱর্ণ *e;
        পূর্ণাঙ্ক r;

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
                (শূন্য) proc_cmdline_parse(parse_proc_cmdline_item, রিক্ত, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(শূন্য) {
        ঘূৰাই log_target;
}

শূন্য log_settle_target(শূন্য) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        যদি (t != LOG_TARGET_AUTO)
                ঘূৰাই;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

পূর্ণাঙ্ক log_get_max_level(শূন্য) {
        ঘূৰাই log_max_level;
}

পূর্ণাঙ্ক log_get_target_max_level(LogTarget target) {
        প্রতিজ্ঞা(target >= 0);
        প্রতিজ্ঞা(target < _LOG_TARGET_SINGLE_MAX);
        ঘূৰাই log_target_max_level[target];
}

শূন্য log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(শূন্য) {
        ঘূৰাই show_color > 0; /* Defaults to false. */
}

শূন্য log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(শূন্য) {
        ঘূৰাই show_location;
}

শূন্য log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(শূন্য) {
        ঘূৰাই show_time;
}

শূন্য log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(শূন্য) {
        ঘূৰাই show_tid;
}

শূন্য log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(শূন্য) {
        ঘূৰাই show_executable;
}

পূর্ণাঙ্ক log_show_color_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        r = parse_boolean(e);
        যদি (r < 0)
                ঘূৰাই r;

        log_show_color(r);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_show_location_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        r = parse_boolean(e);
        যদি (r < 0)
                ঘূৰাই r;

        log_show_location(r);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_show_time_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        r = parse_boolean(e);
        যদি (r < 0)
                ঘূৰাই r;

        log_show_time(r);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_show_tid_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        r = parse_boolean(e);
        যদি (r < 0)
                ঘূৰাই r;

        log_show_tid(r);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক log_show_executable_from_string(ধ্রুৱ ৱর্ণ *e) {
        পূর্ণাঙ্ক r;

        r = parse_boolean(e);
        যদি (r < 0)
                ঘূৰাই r;

        log_show_executable(r);
        ঘূৰাই 0;
}

bool log_on_console(শূন্য) {
        যদি (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                ঘূৰাই true;

        ঘূৰাই syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

স্থির ধ্রুৱ ৱর্ণ *ধ্রুৱ log_target_table[_LOG_TARGET_MAX] = {
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

শূন্য log_received_signal(পূর্ণাঙ্ক level, ধ্রুৱ সংরচনা signalfd_siginfo *si) {
        প্রতিজ্ঞা(si);

        যদি (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ ৱর্ণ *p = রিক্ত;

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

পূর্ণাঙ্ক log_syntax_internal(
                ধ্রুৱ ৱর্ণ *unit,
                পূর্ণাঙ্ক level,
                ধ্রুৱ ৱর্ণ *config_file,
                unsigned config_সাৰি,
                পূর্ণাঙ্ক error,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *format, ...) {

        PROTECT_ERRNO;

        যদি (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        যদি (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ঘূৰাই -ERRNO_মান(error);

        ৱর্ণ buffer[LINE_MAX];
        va_list ap;
        ধ্রুৱ ৱর্ণ *unit_fmt = রিক্ত;

        errno = ERRNO_মান(error);

        va_start(ap, format);
        (শূন্য) vsnprintf(buffer, পরিমাণ buffer, format, ap);
        va_end(ap);

        যদি (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        যদি (config_file) {
                যদি (config_সাৰি > 0)
                        ঘূৰাই log_struct_internal(
                                        level,
                                        error,
                                        file, সাৰি, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_সাৰি),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_সাৰি, buffer),
                                        unit_fmt, unit,
                                        রিক্ত);
                অন্যথা
                        ঘূৰাই log_struct_internal(
                                        level,
                                        error,
                                        file, সাৰি, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        রিক্ত);
        } অন্যথা যদি (unit)
                ঘূৰাই log_struct_internal(
                                level,
                                error,
                                file, সাৰি, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                রিক্ত);
        অন্যথা
                ঘূৰাই log_struct_internal(
                                level,
                                error,
                                file, সাৰি, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                রিক্ত);
}

পূর্ণাঙ্ক log_syntax_invalid_utf8_internal(
                ধ্রুৱ ৱর্ণ *unit,
                পূর্ণাঙ্ক level,
                ধ্রুৱ ৱর্ণ *config_file,
                unsigned config_সাৰি,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ৱর্ণ *p = রিক্ত;

        যদি (rvalue)
                p = utf8_escape_invalid(rvalue);

        ঘূৰাই log_syntax_internal(unit, level, config_file, config_সাৰি,
                                   SYNTHETIC_ERRNO(EINVAL), file, সাৰি, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

পূর্ণাঙ্ক log_syntax_parse_error_internal(
                ধ্রুৱ ৱর্ণ *unit,
                ধ্রুৱ ৱর্ণ *config_file,
                unsigned config_সাৰি,
                পূর্ণাঙ্ক error,
                bool critical,
                ধ্রুৱ ৱর্ণ *file,
                পূর্ণাঙ্ক সাৰি,
                ধ্রুৱ ৱর্ণ *func,
                ধ্রুৱ ৱর্ণ *lvalue,
                ধ্রুৱ ৱর্ণ *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ৱর্ণ *escaped = রিক্ত;

        /* OOM is always handled as critical. */
        যদি (ERRNO_মান(error) == ENOMEM)
                ঘূৰাই log_oom_internal(LOG_ERR, file, সাৰি, func);

        যদি (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                যদি (!escaped)
                        rvalue = "(oom)";
                অন্যথা
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_সাৰি, error,
                            file, সাৰি, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ঘূৰাই critical ? -ERRNO_মান(error) : 0;
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

পূর্ণাঙ্ক log_emergency_level(শূন্য) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ঘূৰাই getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

পূর্ণাঙ্ক log_dup_console(শূন্য) {
        পূর্ণাঙ্ক copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        যদি (console_fd < 0 || console_fd >= 3)
                ঘূৰাই 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        যদি (copy < 0)
                ঘূৰাই -errno;

        console_fd = copy;
        ঘূৰাই 0;
}

শূন্য log_setup(শূন্য) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (শূন্য) log_open();
        যদি (log_on_console() && show_color < 0)
                log_show_color(true);
}

ধ্রুৱ ৱর্ণ* _log_set_prefix(ধ্রুৱ ৱর্ণ *prefix, bool force) {
        ধ্রুৱ ৱর্ণ *old = log_prefix;

        যদি (prefix || force)
                log_prefix = prefix;

        ঘূৰাই old;
}
