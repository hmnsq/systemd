/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sys/signalfd.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <sys/uio.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "sd-messages.h"

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "ansi-color.h"
#அந்தர்பாவ "argv-util.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "list.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "log-context.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "proc-cmdline.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "ratelimit.h"
#அந்தர்பாவ "signal-util.h"
#அந்தர்பாவ "socket-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "syslog-util.h"
#அந்தர்பாவ "terminal-util.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

நிலையான log_syntax_callback_t log_syntax_callback = வெற்று;
நிலையான வெறுமை *log_syntax_callback_userdata = வெற்று;

நிலையான LogTarget log_target = LOG_TARGET_CONSOLE;
நிலையான முழு log_max_level = LOG_INFO;
நிலையான முழு log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
நிலையான முழு log_facility = LOG_DAEMON;
நிலையான bool ratelimit_kmsg = true;

நிலையான முழு console_fd = STDERR_FILENO;
நிலையான முழு console_fd_is_tty = -1; /* tri-state: -1 means don't know */
நிலையான முழு syslog_fd = -EBADF;
நிலையான முழு kmsg_fd = -EBADF;
நிலையான முழு journal_fd = -EBADF;

நிலையான bool syslog_is_stream = false;

நிலையான முழு show_color = -1; /* tristate */
நிலையான bool show_location = false;
நிலையான bool show_time = false;
நிலையான bool show_tid = false;
நிலையான bool show_executable = false;

நிலையான bool upgrade_syslog_to_journal = false;
நிலையான bool always_reopen_console = false;
நிலையான bool open_when_needed = false;
நிலையான bool prohibit_ipc = false;

நிலையான thread_local மாறிலி எழுத்து *log_prefix = வெற்று;

#எனில் LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(கோவை)                                                \
        செய் {                                                            \
                எனில் (_unlikely_(!(கோவை))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } போது (false)

வெறுமை log_prefix_swap(மாறிலி எழுத்து **prefix) {
        ப்ரதிஜ்ஞா(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

நிலையான வெறுமை log_close_console(வெறுமை) {
        /* See comment in log_close_journal() */
        (வெறுமை) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

நிலையான முழு log_open_console(வெறுமை) {

        எனில் (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                திரும்பு 0;
        }

        எனில் (console_fd < 3) {
                முழு fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                எனில் (fd < 0)
                        திரும்பு fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        திரும்பு 0;
}

நிலையான வெறுமை log_close_kmsg(வெறுமை) {
        /* See comment in log_close_journal() */
        (வெறுமை) safe_close(TAKE_FD(kmsg_fd));
}

நிலையான முழு log_open_kmsg(வெறுமை) {

        எனில் (kmsg_fd >= 0)
                திரும்பு 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        எனில் (kmsg_fd < 0)
                திரும்பு -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        திரும்பு 0;
}

நிலையான வெறுமை log_close_syslog(வெறுமை) {
        /* See comment in log_close_journal() */
        (வெறுமை) safe_close(TAKE_FD(syslog_fd));
}

நிலையான முழு create_log_socket(முழு type) {
        கட்டமைப்பு timeval tv;
        முழு fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        எனில் (fd < 0)
                திரும்பு -errno;

        fd = fd_move_above_stdio(fd);
        (வெறுமை) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        எனில் (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        இல்லையெனில்
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (வெறுமை) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, அளவு(tv));

        திரும்பு fd;
}

நிலையான முழு log_open_syslog(வெறுமை) {
        முழு r;

        எனில் (syslog_fd >= 0)
                திரும்பு 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        எனில் (syslog_fd < 0) {
                r = syslog_fd;
                செல் fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        எனில் (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                எனில் (syslog_fd < 0) {
                        r = syslog_fd;
                        செல் fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                எனில் (r < 0)
                        செல் fail;

                syslog_is_stream = true;
        } இல்லையெனில்
                syslog_is_stream = false;

        திரும்பு 0;

fail:
        log_close_syslog();
        திரும்பு r;
}

நிலையான வெறுமை log_close_journal(வெறுமை) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (வெறுமை) safe_close(TAKE_FD(journal_fd));
}

நிலையான முழு log_open_journal(வெறுமை) {
        முழு r;

        எனில் (journal_fd >= 0)
                திரும்பு 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        எனில் (journal_fd < 0) {
                r = journal_fd;
                செல் fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        எனில் (r < 0)
                செல் fail;

        திரும்பு 0;

fail:
        log_close_journal();
        திரும்பு r;
}

bool stderr_is_journal(வெறுமை) {
        _cleanup_free_ எழுத்து *w = வெற்று;
        மாறிலி எழுத்து *e;
        uint64_t dev, ino;
        கட்டமைப்பு stat st;

        e = getenv("JOURNAL_STREAM");
        எனில் (!e)
                திரும்பு false;

        எனில் (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                திரும்பு false;
        எனில் (!e)
                திரும்பு false;

        எனில் (safe_atou64(w, &dev) < 0)
                திரும்பு false;
        எனில் (safe_atou64(e, &ino) < 0)
                திரும்பு false;

        எனில் (fstat(STDERR_FILENO, &st) < 0)
                திரும்பு false;

        திரும்பு st.st_dev == dev && st.st_ino == ino;
}

முழு log_open(வெறுமை) {
        முழு r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        எனில் (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                திரும்பு 0;
        }

        எனில் (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                எனில் (!prohibit_ipc) {
                        எனில் (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                எனில் (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        திரும்பு r;
                                }
                        }

                        எனில் (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                எனில் (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        திரும்பு r;
                                }
                        }
                }

                எனில் (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        எனில் (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                திரும்பு r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        திரும்பு log_open_console();
}

வெறுமை log_set_target(LogTarget target) {
        ப்ரதிஜ்ஞா(target >= 0);
        ப்ரதிஜ்ஞா(target < _LOG_TARGET_MAX);

        எனில் (upgrade_syslog_to_journal) {
                எனில் (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                இல்லையெனில் எனில் (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

வெறுமை log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

வெறுமை log_close(வெறுமை) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

வெறுமை log_forget_fds(வெறுமை) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

முழு log_set_max_level(முழு level) {
        ப்ரதிஜ்ஞா(level == LOG_NULL || log_level_is_valid(level));

        முழு old = log_max_level;
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

        திரும்பு old;
}

வெறுமை log_set_facility(முழு facility) {
        log_facility = facility;
}

நிலையான bool check_console_fd_is_tty(வெறுமை) {
        எனில் (console_fd < 0)
                திரும்பு false;

        எனில் (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        திரும்பு console_fd_is_tty;
}

நிலையான முழு write_to_console(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *buffer) {

        நிலையான முழு dumb = -1;

        எழுத்து location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(முழு) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        கட்டமைப்பு iovec iovec[13];
        மாறிலி எழுத்து *on = வெற்று, *off = வெற்று;
        size_t n = 0;

        எனில் (console_fd < 0)
                திரும்பு 0;

        எனில் (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        எனில் (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                திரும்பு 0;

        எனில் (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        எனில் (show_time &&
            format_timestamp(header_time, அளவு(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        எனில் (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        எனில் (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        எனில் (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, வெற்று);

        எனில் (show_location) {
                மாறிலி எழுத்து *lon = "", *loff = "";
                எனில் (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (வெறுமை) snprintf(location, அளவு location, "%s%s:%i%s: ", lon, file, வரி, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        எனில் (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        எனில் (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        எனில் (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        எனில் (writev(console_fd, iovec, n) < 0) {

                எனில் (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (வெறுமை) log_open_console();
                        எனில் (console_fd < 0)
                                திரும்பு 0;

                        எனில் (writev(console_fd, iovec, n) < 0)
                                திரும்பு -errno;
                } இல்லையெனில்
                        திரும்பு -errno;
        }

        திரும்பு 1;
}

நிலையான முழு write_to_syslog(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *buffer) {

        எழுத்து header_priority[2 + DECIMAL_STR_MAX(முழு) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        கட்டமைப்பு tm tm;
        முழு r;

        எனில் (syslog_fd < 0)
                திரும்பு 0;

        எனில் (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                திரும்பு 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (strftime(header_time, அளவு(header_time), "%h %e %T ", &tm) <= 0)
                திரும்பு -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        கட்டமைப்பு iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        மாறிலி கட்டமைப்பு msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        எனில் (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        க்கு (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                எனில் (n < 0)
                        திரும்பு -errno;

                எனில் (!syslog_is_stream)
                        நிறுத்து;

                எனில் (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        நிறுத்து;
        }

        திரும்பு 1;
}

நிலையான முழு write_to_kmsg(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        நிலையான thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        எழுத்து header_priority[2 + DECIMAL_STR_MAX(முழு) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        எனில் (kmsg_fd < 0)
                திரும்பு 0;

        எனில் (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                திரும்பு 0;

        எனில் (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                எனில் (ratelimit_இலக்கம்_dropped(&ratelimit) > 1)
                        திரும்பு 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        மாறிலி கட்டமைப்பு iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        எனில் (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                திரும்பு -errno;

        திரும்பு 1;
}

நிலையான முழு log_do_header(
                எழுத்து *header,
                size_t size,
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file, முழு வரி, மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field, மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field, மாறிலி எழுத்து *extra) {
        முழு r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_மதிப்பு(error);

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
                     வரி ? "CODE_LINE=" : "",
                     வரி ? 1 : 0, வரி, /* %.0d means no output too, special case for 0 */
                     வரி ? "\n" : "",
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

        திரும்பு 0;
}

நிலையான வெறுமை log_do_context(கட்டமைப்பு iovec *iovec, size_t iovec_len, size_t *n) {
        ப்ரதிஜ்ஞா(iovec);
        ப்ரதிஜ்ஞா(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        எனில் (*n + 2 >= iovec_len)
                                திரும்பு;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                க்கு (size_t i = 0; i < c->n_உள்ளீடு_iovec; i++) {
                        எனில் (*n + 2 >= iovec_len)
                                திரும்பு;

                        iovec[(*n)++] = c->உள்ளீடு_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                எனில் (c->சாவி && c->மதிப்பு) {
                        எனில் (*n + 3 >= iovec_len)
                                திரும்பு;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->சாவி);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->மதிப்பு);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

நிலையான முழு write_to_journal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                மாறிலி எழுத்து *buffer) {

        எழுத்து header[LINE_MAX];
        size_t n = 0, iovec_len;
        கட்டமைப்பு iovec *iovec;

        எனில் (journal_fd < 0)
                திரும்பு 0;

        எனில் (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                திரும்பு 0;

        எனில் (!MUL_SAFE(&iovec_len, log_context_இலக்கம்_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        இல்லையெனில்
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(கட்டமைப்பு iovec, iovec_len);

        log_do_header(header, அளவு(header), level, error, file, வரி, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        எனில் (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        மாறிலி கட்டமைப்பு msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        எனில் (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                திரும்பு -errno;

        திரும்பு 1;
}

முழு log_dispatch_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                எழுத்து *buffer) {

        assert_raw(buffer);

        எனில் (log_target == LOG_TARGET_NULL)
                திரும்பு -ERRNO_மதிப்பு(error);

        /* Patch in LOG_DAEMON facility if necessary */
        எனில் (LOG_FAC(level) == 0)
                level |= log_facility;

        எனில் (open_when_needed)
                (வெறுமை) log_open();

        செய் {
                எழுத்து *e;
                முழு k = 0;

                buffer += strspn(buffer, NEWLINE);

                எனில் (buffer[0] == 0)
                        நிறுத்து;

                எனில் ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                எனில் (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, வரி, func, object_field, object, extra_field, extra, buffer);
                        எனில் (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                எனில் (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, வரி, func, buffer);
                        எனில் (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                எனில் (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        எனில் (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, வரி, func, buffer);
                        எனில் (k < 0) {
                                log_close_kmsg();
                                (வெறுமை) log_open_console();
                        }
                }

                எனில் (k <= 0)
                        (வெறுமை) write_to_console(level, error, file, வரி, func, buffer);

                buffer = e;
        } போது (buffer);

        எனில் (open_when_needed)
                log_close();

        திரும்பு -ERRNO_மதிப்பு(error);
}

முழு log_dump_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                எழுத்து *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        எனில் (_likely_(LOG_PRI(level) > log_max_level))
                திரும்பு -ERRNO_மதிப்பு(error);

        திரும்பு log_dispatch_internal(level, error, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று, buffer);
}

முழு log_internalv(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format,
                va_list ap) {

        எனில் (_likely_(LOG_PRI(level) > log_max_level))
                திரும்பு -ERRNO_மதிப்பு(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        எழுத்து buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_மதிப்பு(error));

        (வெறுமை) vsnprintf(buffer, அளவு buffer, format, ap);

        திரும்பு log_dispatch_internal(level, error, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று, buffer);
}

முழு log_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format, ...) {

        va_list ap;
        முழு r;

        va_start(ap, format);
        r = log_internalv(level, error, file, வரி, func, format, ap);
        va_end(ap);

        திரும்பு r;
}

முழு log_object_internalv(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                மாறிலி எழுத்து *format,
                va_list ap) {

        எழுத்து *buffer, *b;

        எனில் (_likely_(LOG_PRI(level) > log_max_level))
                திரும்பு -ERRNO_மதிப்பு(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_மதிப்பு(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(எழுத்து, LINE_MAX);
        (வெறுமை) vsnprintf(b, LINE_MAX, format, ap);

        திரும்பு log_dispatch_internal(level, error, file, வரி, func,
                                     object_field, object, extra_field, extra, buffer);
}

முழு log_object_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                மாறிலி எழுத்து *format, ...) {

        va_list ap;
        முழு r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, வரி, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        திரும்பு r;
}

முழு log_oom_internal(முழு level, மாறிலி எழுத்து *file, முழு வரி, மாறிலி எழுத்து *func) {
        திரும்பு log_internal(level, ENOMEM, file, வரி, func, "Out of memory.");
}

முழு log_format_iovec(
                கட்டமைப்பு iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                முழு error,
                மாறிலி எழுத்து *format,
                va_list ap) {

        ப்ரதிஜ்ஞா(iovec);
        ப்ரதிஜ்ஞா(n);

        போது (format && *n + 1 < iovec_len) {
                va_list aq;
                எழுத்து *m;
                முழு r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_மதிப்பு(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                எனில் (r < 0)
                        திரும்பு -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                எனில் (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, எழுத்து *);
        }
        திரும்பு 0;
}

முழு log_struct_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format, ...) {

        எழுத்து buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        எனில் (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                திரும்பு -ERRNO_மதிப்பு(error);

        எனில் (LOG_FAC(level) == 0)
                level |= log_facility;

        எனில் (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                எனில் (open_when_needed)
                        log_open_journal();

                எனில் (journal_fd >= 0) {
                        எழுத்து header[LINE_MAX];
                        கட்டமைப்பு iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        முழு r;
                        bool fallback = false;

                        எனில் (!MUL_SAFE(&iovec_len, log_context_இலக்கம்_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        இல்லையெனில்
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(கட்டமைப்பு iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, அளவு(header), level, error, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        எனில் (r < 0)
                                fallback = true;
                        இல்லையெனில் {
                                log_do_context(iovec, iovec_len, &n);

                                மாறிலி கட்டமைப்பு msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (வெறுமை) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        க்கு (size_t i = 1; i < m; i += 2)
                                விடுவி(iovec[i].iov_base);

                        எனில் (!fallback) {
                                எனில் (open_when_needed)
                                        log_close();

                                திரும்பு -ERRNO_மதிப்பு(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        போது (format) {
                va_list aq;

                errno = ERRNO_மதிப்பு(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (வெறுமை) vsnprintf(buf, அளவு buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                எனில் (startswith(buf, "MESSAGE=")) {
                        found = true;
                        நிறுத்து;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, எழுத்து *);
        }
        va_end(ap);

        எனில் (!found) {
                எனில் (open_when_needed)
                        log_close();

                திரும்பு -ERRNO_மதிப்பு(error);
        }

        திரும்பு log_dispatch_internal(level, error, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று, buf + 8);
}

முழு log_struct_iovec_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி கட்டமைப்பு iovec உள்ளீடு_iovec[],
                size_t n_உள்ளீடு_iovec) {

        PROTECT_ERRNO;

        எனில் (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                திரும்பு -ERRNO_மதிப்பு(error);

        எனில் (LOG_FAC(level) == 0)
                level |= log_facility;

        எனில் (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                எழுத்து header[LINE_MAX];
                கட்டமைப்பு iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_உள்ளீடு_iovec_expanded;

                எனில் (!MUL_SAFE(&n_உள்ளீடு_iovec_expanded, n_உள்ளீடு_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_இலக்கம்_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_உள்ளீடு_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                இல்லையெனில்
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(கட்டமைப்பு iovec, iovec_len);

                log_do_header(header, அளவு(header), level, error, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                க்கு (size_t i = 0; i < n_உள்ளீடு_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = உள்ளீடு_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                மாறிலி கட்டமைப்பு msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                எனில் (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        திரும்பு -ERRNO_மதிப்பு(error);
        }

        க்கு (size_t i = 0; i < n_உள்ளீடு_iovec; i++)
                எனில் (memory_startswith(உள்ளீடு_iovec[i].iov_base, உள்ளீடு_iovec[i].iov_len, "MESSAGE=")) {
                        எழுத்து *m;

                        m = strndupa_safe((எழுத்து*) உள்ளீடு_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          உள்ளீடு_iovec[i].iov_len - STRLEN("MESSAGE="));

                        திரும்பு log_dispatch_internal(level, error, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று, m);
                }

        /* Couldn't find MESSAGE=. */
        திரும்பு -ERRNO_மதிப்பு(error);
}

முழு log_set_target_from_string(மாறிலி எழுத்து *e) {
        LogTarget t;

        t = log_target_from_string(e);
        எனில் (t < 0)
                திரும்பு t;

        log_set_target(t);
        திரும்பு 0;
}

முழு log_set_max_level_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *word = வெற்று, *prefix = வெற்று;
                LogTarget target;
                மாறிலி எழுத்து *colon;

                r = extract_first_word(&e, &word, ",", 0);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                colon = strchr(word, ':');
                எனில் (!colon) {
                        r = log_level_from_string(word);
                        எனில் (r < 0)
                                திரும்பு r;

                        log_set_max_level(r);
                        தொடர்;
                }

                prefix = strndup(word, colon - word);
                எனில் (!prefix)
                        திரும்பு -ENOMEM;

                target = log_target_from_string(prefix);
                எனில் (target < 0)
                        திரும்பு target;

                எனில் (target >= _LOG_TARGET_SINGLE_MAX)
                        திரும்பு -EINVAL;

                r = log_level_from_string(colon + 1);
                எனில் (r < 0)
                        திரும்பு r;

                log_target_max_level[target] = r;
        }

        திரும்பு 0;
}

முழு log_max_levels_to_string(முழு level, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = log_level_to_string_alloc(level, &s);
        எனில் (r < 0)
                திரும்பு r;

        க்கு (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ எழுத்து *l = வெற்று;

                எனில் (log_target_max_level[target] == INT_MAX)
                        தொடர்;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                எனில் (r < 0)
                        திரும்பு r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

நிலையான முழு log_set_ratelimit_kmsg_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        r = parse_boolean(e);
        எனில் (r < 0)
                திரும்பு r;

        ratelimit_kmsg = r;
        திரும்பு 0;
}

நிலையான முழு parse_proc_cmdline_item(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு, வெறுமை *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        எனில் (streq(சாவி, "debug") && !மதிப்பு)
                log_set_max_level(LOG_DEBUG);

        இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_target")) {

                எனில் (proc_cmdline_மதிப்பு_missing(சாவி, மதிப்பு))
                        திரும்பு 0;

                எனில் (log_set_target_from_string(மதிப்பு) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", மதிப்பு);

        } இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_level")) {

                எனில் (proc_cmdline_மதிப்பு_missing(சாவி, மதிப்பு))
                        திரும்பு 0;

                எனில் (log_set_max_level_from_string(மதிப்பு) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", மதிப்பு);

        } இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_color")) {

                எனில் (log_show_color_from_string(மதிப்பு ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", மதிப்பு);

        } இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_location")) {

                எனில் (log_show_location_from_string(மதிப்பு ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", மதிப்பு);

        } இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_tid")) {

                எனில் (log_show_tid_from_string(மதிப்பு ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", மதிப்பு);

        } இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_time")) {

                எனில் (log_show_time_from_string(மதிப்பு ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", மதிப்பு);

        } இல்லையெனில் எனில் (proc_cmdline_சாவி_streq(சாவி, "systemd.log_ratelimit_kmsg")) {

                எனில் (log_set_ratelimit_kmsg_from_string(மதிப்பு ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", மதிப்பு);
        }

        திரும்பு 0;
}

நிலையான bool should_parse_proc_cmdline(வெறுமை) {
        /* PID1 always reads the kernel command line. */
        எனில் (getpid_cached() == 1)
                திரும்பு true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        திரும்பு invoked_by_systemd();
}

வெறுமை log_parse_environment_variables(வெறுமை) {
        மாறிலி எழுத்து *e;
        முழு r;

        e = getenv("SYSTEMD_LOG_TARGET");
        எனில் (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        எனில் (e) {
                r = log_set_max_level_from_string(e);
                எனில் (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } இல்லையெனில் {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                எனில் (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                இல்லையெனில் எனில் (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        எனில் (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        எனில் (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        எனில் (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        எனில் (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        எனில் (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        எனில் (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

வெறுமை log_parse_environment(வெறுமை) {
        /* Do not call from library code. */

        எனில் (should_parse_proc_cmdline())
                (வெறுமை) proc_cmdline_parse(parse_proc_cmdline_item, வெற்று, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(வெறுமை) {
        திரும்பு log_target;
}

வெறுமை log_settle_target(வெறுமை) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        எனில் (t != LOG_TARGET_AUTO)
                திரும்பு;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

முழு log_get_max_level(வெறுமை) {
        திரும்பு log_max_level;
}

முழு log_get_target_max_level(LogTarget target) {
        ப்ரதிஜ்ஞா(target >= 0);
        ப்ரதிஜ்ஞா(target < _LOG_TARGET_SINGLE_MAX);
        திரும்பு log_target_max_level[target];
}

வெறுமை log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(வெறுமை) {
        திரும்பு show_color > 0; /* Defaults to false. */
}

வெறுமை log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(வெறுமை) {
        திரும்பு show_location;
}

வெறுமை log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(வெறுமை) {
        திரும்பு show_time;
}

வெறுமை log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(வெறுமை) {
        திரும்பு show_tid;
}

வெறுமை log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(வெறுமை) {
        திரும்பு show_executable;
}

முழு log_show_color_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        r = parse_boolean(e);
        எனில் (r < 0)
                திரும்பு r;

        log_show_color(r);
        திரும்பு 0;
}

முழு log_show_location_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        r = parse_boolean(e);
        எனில் (r < 0)
                திரும்பு r;

        log_show_location(r);
        திரும்பு 0;
}

முழு log_show_time_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        r = parse_boolean(e);
        எனில் (r < 0)
                திரும்பு r;

        log_show_time(r);
        திரும்பு 0;
}

முழு log_show_tid_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        r = parse_boolean(e);
        எனில் (r < 0)
                திரும்பு r;

        log_show_tid(r);
        திரும்பு 0;
}

முழு log_show_executable_from_string(மாறிலி எழுத்து *e) {
        முழு r;

        r = parse_boolean(e);
        எனில் (r < 0)
                திரும்பு r;

        log_show_executable(r);
        திரும்பு 0;
}

bool log_on_console(வெறுமை) {
        எனில் (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                திரும்பு true;

        திரும்பு syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

நிலையான மாறிலி எழுத்து *மாறிலி log_target_table[_LOG_TARGET_MAX] = {
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

வெறுமை log_received_signal(முழு level, மாறிலி கட்டமைப்பு signalfd_siginfo *si) {
        ப்ரதிஜ்ஞா(si);

        எனில் (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ எழுத்து *p = வெற்று;

                (வெறுமை) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } இல்லையெனில்
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

வெறுமை set_log_syntax_callback(log_syntax_callback_t cb, வெறுமை *userdata) {
        ப்ரதிஜ்ஞா(!log_syntax_callback || !cb);
        ப்ரதிஜ்ஞா(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

முழு log_syntax_internal(
                மாறிலி எழுத்து *unit,
                முழு level,
                மாறிலி எழுத்து *config_file,
                unsigned config_வரி,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format, ...) {

        PROTECT_ERRNO;

        எனில் (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        எனில் (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                திரும்பு -ERRNO_மதிப்பு(error);

        எழுத்து buffer[LINE_MAX];
        va_list ap;
        மாறிலி எழுத்து *unit_fmt = வெற்று;

        errno = ERRNO_மதிப்பு(error);

        va_start(ap, format);
        (வெறுமை) vsnprintf(buffer, அளவு buffer, format, ap);
        va_end(ap);

        எனில் (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        எனில் (config_file) {
                எனில் (config_வரி > 0)
                        திரும்பு log_struct_internal(
                                        level,
                                        error,
                                        file, வரி, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_வரி),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_வரி, buffer),
                                        unit_fmt, unit,
                                        வெற்று);
                இல்லையெனில்
                        திரும்பு log_struct_internal(
                                        level,
                                        error,
                                        file, வரி, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        வெற்று);
        } இல்லையெனில் எனில் (unit)
                திரும்பு log_struct_internal(
                                level,
                                error,
                                file, வரி, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                வெற்று);
        இல்லையெனில்
                திரும்பு log_struct_internal(
                                level,
                                error,
                                file, வரி, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                வெற்று);
}

முழு log_syntax_invalid_utf8_internal(
                மாறிலி எழுத்து *unit,
                முழு level,
                மாறிலி எழுத்து *config_file,
                unsigned config_வரி,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ எழுத்து *p = வெற்று;

        எனில் (rvalue)
                p = utf8_escape_invalid(rvalue);

        திரும்பு log_syntax_internal(unit, level, config_file, config_வரி,
                                   SYNTHETIC_ERRNO(EINVAL), file, வரி, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

முழு log_syntax_parse_error_internal(
                மாறிலி எழுத்து *unit,
                மாறிலி எழுத்து *config_file,
                unsigned config_வரி,
                முழு error,
                bool critical,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *lvalue,
                மாறிலி எழுத்து *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ எழுத்து *escaped = வெற்று;

        /* OOM is always handled as critical. */
        எனில் (ERRNO_மதிப்பு(error) == ENOMEM)
                திரும்பு log_oom_internal(LOG_ERR, file, வரி, func);

        எனில் (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                எனில் (!escaped)
                        rvalue = "(oom)";
                இல்லையெனில்
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_வரி, error,
                            file, வரி, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        திரும்பு critical ? -ERRNO_மதிப்பு(error) : 0;
}

வெறுமை log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        எனில் (b) {
                எனில் (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                இல்லையெனில் எனில் (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

வெறுமை log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

வெறுமை log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

வெறுமை log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

முழு log_emergency_level(வெறுமை) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        திரும்பு getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

முழு log_dup_console(வெறுமை) {
        முழு copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        எனில் (console_fd < 0 || console_fd >= 3)
                திரும்பு 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        எனில் (copy < 0)
                திரும்பு -errno;

        console_fd = copy;
        திரும்பு 0;
}

வெறுமை log_setup(வெறுமை) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (வெறுமை) log_open();
        எனில் (log_on_console() && show_color < 0)
                log_show_color(true);
}

மாறிலி எழுத்து* _log_set_prefix(மாறிலி எழுத்து *prefix, bool force) {
        மாறிலி எழுத்து *old = log_prefix;

        எனில் (prefix || force)
                log_prefix = prefix;

        திரும்பு old;
}
