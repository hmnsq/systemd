/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sys/signalfd.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <sys/uio.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "sd-messages.h"

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-color.h"
#अन्तर्भाव "argv-util.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "list.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "log-context.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "proc-cmdline.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "signal-util.h"
#अन्तर्भाव "socket-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "syslog-util.h"
#अन्तर्भाव "terminal-util.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

स्थिर log_syntax_callback_t log_syntax_callback = रिक्त;
स्थिर शून्य *log_syntax_callback_userdata = रिक्त;

स्थिर LogTarget log_target = LOG_TARGET_CONSOLE;
स्थिर पूर्णाङ्क log_max_level = LOG_INFO;
स्थिर पूर्णाङ्क log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
स्थिर पूर्णाङ्क log_facility = LOG_DAEMON;
स्थिर bool ratelimit_kmsg = true;

स्थिर पूर्णाङ्क console_fd = STDERR_FILENO;
स्थिर पूर्णाङ्क console_fd_is_tty = -1; /* tri-state: -1 means don't know */
स्थिर पूर्णाङ्क syslog_fd = -EBADF;
स्थिर पूर्णाङ्क kmsg_fd = -EBADF;
स्थिर पूर्णाङ्क journal_fd = -EBADF;

स्थिर bool syslog_is_stream = false;

स्थिर पूर्णाङ्क show_color = -1; /* tristate */
स्थिर bool show_location = false;
स्थिर bool show_time = false;
स्थिर bool show_tid = false;
स्थिर bool show_executable = false;

स्थिर bool upgrade_syslog_to_journal = false;
स्थिर bool always_reopen_console = false;
स्थिर bool open_when_needed = false;
स्थिर bool prohibit_ipc = false;

स्थिर thread_local ध्रुव वर्ण *log_prefix = रिक्त;

#सचे LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(व्यञ्जकम्)                                                \
        करोहि {                                                            \
                सचे (_unlikely_(!(व्यञ्जकम्))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } याव (false)

शून्य log_prefix_swap(ध्रुव वर्ण **prefix) {
        प्रतिज्ञा(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

स्थिर शून्य log_close_console(शून्य) {
        /* See comment in log_close_journal() */
        (शून्य) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

स्थिर पूर्णाङ्क log_open_console(शून्य) {

        सचे (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                पटिनिवत्त 0;
        }

        सचे (console_fd < 3) {
                पूर्णाङ्क fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                सचे (fd < 0)
                        पटिनिवत्त fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        पटिनिवत्त 0;
}

स्थिर शून्य log_close_kmsg(शून्य) {
        /* See comment in log_close_journal() */
        (शून्य) safe_close(TAKE_FD(kmsg_fd));
}

स्थिर पूर्णाङ्क log_open_kmsg(शून्य) {

        सचे (kmsg_fd >= 0)
                पटिनिवत्त 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        सचे (kmsg_fd < 0)
                पटिनिवत्त -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        पटिनिवत्त 0;
}

स्थिर शून्य log_close_syslog(शून्य) {
        /* See comment in log_close_journal() */
        (शून्य) safe_close(TAKE_FD(syslog_fd));
}

स्थिर पूर्णाङ्क create_log_socket(पूर्णाङ्क type) {
        संरचना timeval tv;
        पूर्णाङ्क fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        fd = fd_move_above_stdio(fd);
        (शून्य) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        सचे (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        अञ्ञथा
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (शून्य) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, परिमाण(tv));

        पटिनिवत्त fd;
}

स्थिर पूर्णाङ्क log_open_syslog(शून्य) {
        पूर्णाङ्क r;

        सचे (syslog_fd >= 0)
                पटिनिवत्त 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        सचे (syslog_fd < 0) {
                r = syslog_fd;
                गच्छ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        सचे (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                सचे (syslog_fd < 0) {
                        r = syslog_fd;
                        गच्छ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                सचे (r < 0)
                        गच्छ fail;

                syslog_is_stream = true;
        } अञ्ञथा
                syslog_is_stream = false;

        पटिनिवत्त 0;

fail:
        log_close_syslog();
        पटिनिवत्त r;
}

स्थिर शून्य log_close_journal(शून्य) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (शून्य) safe_close(TAKE_FD(journal_fd));
}

स्थिर पूर्णाङ्क log_open_journal(शून्य) {
        पूर्णाङ्क r;

        सचे (journal_fd >= 0)
                पटिनिवत्त 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        सचे (journal_fd < 0) {
                r = journal_fd;
                गच्छ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        सचे (r < 0)
                गच्छ fail;

        पटिनिवत्त 0;

fail:
        log_close_journal();
        पटिनिवत्त r;
}

bool stderr_is_journal(शून्य) {
        _cleanup_free_ वर्ण *w = रिक्त;
        ध्रुव वर्ण *e;
        uint64_t dev, ino;
        संरचना stat st;

        e = getenv("JOURNAL_STREAM");
        सचे (!e)
                पटिनिवत्त false;

        सचे (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                पटिनिवत्त false;
        सचे (!e)
                पटिनिवत्त false;

        सचे (safe_atou64(w, &dev) < 0)
                पटिनिवत्त false;
        सचे (safe_atou64(e, &ino) < 0)
                पटिनिवत्त false;

        सचे (fstat(STDERR_FILENO, &st) < 0)
                पटिनिवत्त false;

        पटिनिवत्त st.st_dev == dev && st.st_ino == ino;
}

पूर्णाङ्क log_open(शून्य) {
        पूर्णाङ्क r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        सचे (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                पटिनिवत्त 0;
        }

        सचे (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                सचे (!prohibit_ipc) {
                        सचे (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                सचे (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        पटिनिवत्त r;
                                }
                        }

                        सचे (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                सचे (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        पटिनिवत्त r;
                                }
                        }
                }

                सचे (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        सचे (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                पटिनिवत्त r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        पटिनिवत्त log_open_console();
}

शून्य log_set_target(LogTarget target) {
        प्रतिज्ञा(target >= 0);
        प्रतिज्ञा(target < _LOG_TARGET_MAX);

        सचे (upgrade_syslog_to_journal) {
                सचे (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                अञ्ञथा सचे (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

शून्य log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

शून्य log_close(शून्य) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

शून्य log_forget_fds(शून्य) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

पूर्णाङ्क log_set_max_level(पूर्णाङ्क level) {
        प्रतिज्ञा(level == LOG_NULL || log_level_is_valid(level));

        पूर्णाङ्क old = log_max_level;
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

        पटिनिवत्त old;
}

शून्य log_set_facility(पूर्णाङ्क facility) {
        log_facility = facility;
}

स्थिर bool check_console_fd_is_tty(शून्य) {
        सचे (console_fd < 0)
                पटिनिवत्त false;

        सचे (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        पटिनिवत्त console_fd_is_tty;
}

स्थिर पूर्णाङ्क write_to_console(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *buffer) {

        स्थिर पूर्णाङ्क dumb = -1;

        वर्ण location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(पूर्णाङ्क) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        संरचना iovec iovec[13];
        ध्रुव वर्ण *on = रिक्त, *off = रिक्त;
        size_t n = 0;

        सचे (console_fd < 0)
                पटिनिवत्त 0;

        सचे (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        सचे (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                पटिनिवत्त 0;

        सचे (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        सचे (show_time &&
            format_timestamp(header_time, परिमाण(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        सचे (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        सचे (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        सचे (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, रिक्त);

        सचे (show_location) {
                ध्रुव वर्ण *lon = "", *loff = "";
                सचे (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (शून्य) snprintf(location, परिमाण location, "%s%s:%i%s: ", lon, file, पङ्क्तिः, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        सचे (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        सचे (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        सचे (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        सचे (writev(console_fd, iovec, n) < 0) {

                सचे (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (शून्य) log_open_console();
                        सचे (console_fd < 0)
                                पटिनिवत्त 0;

                        सचे (writev(console_fd, iovec, n) < 0)
                                पटिनिवत्त -errno;
                } अञ्ञथा
                        पटिनिवत्त -errno;
        }

        पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क write_to_syslog(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *buffer) {

        वर्ण header_priority[2 + DECIMAL_STR_MAX(पूर्णाङ्क) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        संरचना tm tm;
        पूर्णाङ्क r;

        सचे (syslog_fd < 0)
                पटिनिवत्त 0;

        सचे (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                पटिनिवत्त 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (strftime(header_time, परिमाण(header_time), "%h %e %T ", &tm) <= 0)
                पटिनिवत्त -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        संरचना iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        ध्रुव संरचना msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        सचे (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        अत्थाय (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                सचे (n < 0)
                        पटिनिवत्त -errno;

                सचे (!syslog_is_stream)
                        भङ्ग;

                सचे (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        भङ्ग;
        }

        पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क write_to_kmsg(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        स्थिर thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        वर्ण header_priority[2 + DECIMAL_STR_MAX(पूर्णाङ्क) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        सचे (kmsg_fd < 0)
                पटिनिवत्त 0;

        सचे (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                पटिनिवत्त 0;

        सचे (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                सचे (ratelimit_अङ्कः_dropped(&ratelimit) > 1)
                        पटिनिवत्त 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ध्रुव संरचना iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        सचे (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क log_do_header(
                वर्ण *header,
                size_t size,
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field, ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field, ध्रुव वर्ण *extra) {
        पूर्णाङ्क r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_मानम्(error);

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
                     पङ्क्तिः ? "CODE_LINE=" : "",
                     पङ्क्तिः ? 1 : 0, पङ्क्तिः, /* %.0d means no output too, special case for 0 */
                     पङ्क्तिः ? "\n" : "",
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

        पटिनिवत्त 0;
}

स्थिर शून्य log_do_context(संरचना iovec *iovec, size_t iovec_len, size_t *n) {
        प्रतिज्ञा(iovec);
        प्रतिज्ञा(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        सचे (*n + 2 >= iovec_len)
                                पटिनिवत्त;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                अत्थाय (size_t i = 0; i < c->n_निवेशः_iovec; i++) {
                        सचे (*n + 2 >= iovec_len)
                                पटिनिवत्त;

                        iovec[(*n)++] = c->निवेशः_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                सचे (c->कुञ्चिका && c->मानम्) {
                        सचे (*n + 3 >= iovec_len)
                                पटिनिवत्त;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->कुञ्चिका);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->मानम्);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

स्थिर पूर्णाङ्क write_to_journal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *buffer) {

        वर्ण header[LINE_MAX];
        size_t n = 0, iovec_len;
        संरचना iovec *iovec;

        सचे (journal_fd < 0)
                पटिनिवत्त 0;

        सचे (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                पटिनिवत्त 0;

        सचे (!MUL_SAFE(&iovec_len, log_context_अङ्कः_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        अञ्ञथा
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(संरचना iovec, iovec_len);

        log_do_header(header, परिमाण(header), level, error, file, पङ्क्तिः, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        सचे (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ध्रुव संरचना msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        सचे (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त 1;
}

पूर्णाङ्क log_dispatch_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                वर्ण *buffer) {

        assert_raw(buffer);

        सचे (log_target == LOG_TARGET_NULL)
                पटिनिवत्त -ERRNO_मानम्(error);

        /* Patch in LOG_DAEMON facility if necessary */
        सचे (LOG_FAC(level) == 0)
                level |= log_facility;

        सचे (open_when_needed)
                (शून्य) log_open();

        करोहि {
                वर्ण *e;
                पूर्णाङ्क k = 0;

                buffer += strspn(buffer, NEWLINE);

                सचे (buffer[0] == 0)
                        भङ्ग;

                सचे ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                सचे (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, पङ्क्तिः, func, object_field, object, extra_field, extra, buffer);
                        सचे (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                सचे (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, पङ्क्तिः, func, buffer);
                        सचे (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                सचे (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        सचे (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, पङ्क्तिः, func, buffer);
                        सचे (k < 0) {
                                log_close_kmsg();
                                (शून्य) log_open_console();
                        }
                }

                सचे (k <= 0)
                        (शून्य) write_to_console(level, error, file, पङ्क्तिः, func, buffer);

                buffer = e;
        } याव (buffer);

        सचे (open_when_needed)
                log_close();

        पटिनिवत्त -ERRNO_मानम्(error);
}

पूर्णाङ्क log_dump_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                वर्ण *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        सचे (_likely_(LOG_PRI(level) > log_max_level))
                पटिनिवत्त -ERRNO_मानम्(error);

        पटिनिवत्त log_dispatch_internal(level, error, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

पूर्णाङ्क log_internalv(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format,
                va_list ap) {

        सचे (_likely_(LOG_PRI(level) > log_max_level))
                पटिनिवत्त -ERRNO_मानम्(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        वर्ण buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_मानम्(error));

        (शून्य) vsnprintf(buffer, परिमाण buffer, format, ap);

        पटिनिवत्त log_dispatch_internal(level, error, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

पूर्णाङ्क log_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) {

        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = log_internalv(level, error, file, पङ्क्तिः, func, format, ap);
        va_end(ap);

        पटिनिवत्त r;
}

पूर्णाङ्क log_object_internalv(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *format,
                va_list ap) {

        वर्ण *buffer, *b;

        सचे (_likely_(LOG_PRI(level) > log_max_level))
                पटिनिवत्त -ERRNO_मानम्(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_मानम्(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(वर्ण, LINE_MAX);
        (शून्य) vsnprintf(b, LINE_MAX, format, ap);

        पटिनिवत्त log_dispatch_internal(level, error, file, पङ्क्तिः, func,
                                     object_field, object, extra_field, extra, buffer);
}

पूर्णाङ्क log_object_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *format, ...) {

        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, पङ्क्तिः, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        पटिनिवत्त r;
}

पूर्णाङ्क log_oom_internal(पूर्णाङ्क level, ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *func) {
        पटिनिवत्त log_internal(level, ENOMEM, file, पङ्क्तिः, func, "Out of memory.");
}

पूर्णाङ्क log_format_iovec(
                संरचना iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                पूर्णाङ्क error,
                ध्रुव वर्ण *format,
                va_list ap) {

        प्रतिज्ञा(iovec);
        प्रतिज्ञा(n);

        याव (format && *n + 1 < iovec_len) {
                va_list aq;
                वर्ण *m;
                पूर्णाङ्क r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_मानम्(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                सचे (r < 0)
                        पटिनिवत्त -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                सचे (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, वर्ण *);
        }
        पटिनिवत्त 0;
}

पूर्णाङ्क log_struct_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) {

        वर्ण buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        सचे (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                पटिनिवत्त -ERRNO_मानम्(error);

        सचे (LOG_FAC(level) == 0)
                level |= log_facility;

        सचे (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                सचे (open_when_needed)
                        log_open_journal();

                सचे (journal_fd >= 0) {
                        वर्ण header[LINE_MAX];
                        संरचना iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        पूर्णाङ्क r;
                        bool fallback = false;

                        सचे (!MUL_SAFE(&iovec_len, log_context_अङ्कः_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        अञ्ञथा
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(संरचना iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, परिमाण(header), level, error, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        सचे (r < 0)
                                fallback = true;
                        अञ्ञथा {
                                log_do_context(iovec, iovec_len, &n);

                                ध्रुव संरचना msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (शून्य) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        अत्थाय (size_t i = 1; i < m; i += 2)
                                मुक्त(iovec[i].iov_base);

                        सचे (!fallback) {
                                सचे (open_when_needed)
                                        log_close();

                                पटिनिवत्त -ERRNO_मानम्(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        याव (format) {
                va_list aq;

                errno = ERRNO_मानम्(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (शून्य) vsnprintf(buf, परिमाण buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                सचे (startswith(buf, "MESSAGE=")) {
                        found = true;
                        भङ्ग;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, वर्ण *);
        }
        va_end(ap);

        सचे (!found) {
                सचे (open_when_needed)
                        log_close();

                पटिनिवत्त -ERRNO_मानम्(error);
        }

        पटिनिवत्त log_dispatch_internal(level, error, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त, buf + 8);
}

पूर्णाङ्क log_struct_iovec_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव संरचना iovec निवेशः_iovec[],
                size_t n_निवेशः_iovec) {

        PROTECT_ERRNO;

        सचे (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                पटिनिवत्त -ERRNO_मानम्(error);

        सचे (LOG_FAC(level) == 0)
                level |= log_facility;

        सचे (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                वर्ण header[LINE_MAX];
                संरचना iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_निवेशः_iovec_expanded;

                सचे (!MUL_SAFE(&n_निवेशः_iovec_expanded, n_निवेशः_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_अङ्कः_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_निवेशः_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                अञ्ञथा
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(संरचना iovec, iovec_len);

                log_do_header(header, परिमाण(header), level, error, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                अत्थाय (size_t i = 0; i < n_निवेशः_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = निवेशः_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ध्रुव संरचना msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                सचे (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        पटिनिवत्त -ERRNO_मानम्(error);
        }

        अत्थाय (size_t i = 0; i < n_निवेशः_iovec; i++)
                सचे (memory_startswith(निवेशः_iovec[i].iov_base, निवेशः_iovec[i].iov_len, "MESSAGE=")) {
                        वर्ण *m;

                        m = strndupa_safe((वर्ण*) निवेशः_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          निवेशः_iovec[i].iov_len - STRLEN("MESSAGE="));

                        पटिनिवत्त log_dispatch_internal(level, error, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त, m);
                }

        /* Couldn't find MESSAGE=. */
        पटिनिवत्त -ERRNO_मानम्(error);
}

पूर्णाङ्क log_set_target_from_string(ध्रुव वर्ण *e) {
        LogTarget t;

        t = log_target_from_string(e);
        सचे (t < 0)
                पटिनिवत्त t;

        log_set_target(t);
        पटिनिवत्त 0;
}

पूर्णाङ्क log_set_max_level_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        अत्थाय (;;) {
                _cleanup_free_ वर्ण *word = रिक्त, *prefix = रिक्त;
                LogTarget target;
                ध्रुव वर्ण *colon;

                r = extract_first_word(&e, &word, ",", 0);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        भङ्ग;

                colon = strchr(word, ':');
                सचे (!colon) {
                        r = log_level_from_string(word);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        log_set_max_level(r);
                        अनुवर्त;
                }

                prefix = strndup(word, colon - word);
                सचे (!prefix)
                        पटिनिवत्त -ENOMEM;

                target = log_target_from_string(prefix);
                सचे (target < 0)
                        पटिनिवत्त target;

                सचे (target >= _LOG_TARGET_SINGLE_MAX)
                        पटिनिवत्त -EINVAL;

                r = log_level_from_string(colon + 1);
                सचे (r < 0)
                        पटिनिवत्त r;

                log_target_max_level[target] = r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क log_max_levels_to_string(पूर्णाङ्क level, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = log_level_to_string_alloc(level, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        अत्थाय (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ वर्ण *l = रिक्त;

                सचे (log_target_max_level[target] == INT_MAX)
                        अनुवर्त;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                सचे (r < 0)
                        पटिनिवत्त r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क log_set_ratelimit_kmsg_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        सचे (r < 0)
                पटिनिवत्त r;

        ratelimit_kmsg = r;
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क parse_proc_cmdline_item(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्, शून्य *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        सचे (streq(कुञ्चिका, "debug") && !मानम्)
                log_set_max_level(LOG_DEBUG);

        अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_target")) {

                सचे (proc_cmdline_मानम्_missing(कुञ्चिका, मानम्))
                        पटिनिवत्त 0;

                सचे (log_set_target_from_string(मानम्) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", मानम्);

        } अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_level")) {

                सचे (proc_cmdline_मानम्_missing(कुञ्चिका, मानम्))
                        पटिनिवत्त 0;

                सचे (log_set_max_level_from_string(मानम्) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", मानम्);

        } अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_color")) {

                सचे (log_show_color_from_string(मानम् ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", मानम्);

        } अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_location")) {

                सचे (log_show_location_from_string(मानम् ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", मानम्);

        } अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_tid")) {

                सचे (log_show_tid_from_string(मानम् ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", मानम्);

        } अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_time")) {

                सचे (log_show_time_from_string(मानम् ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", मानम्);

        } अञ्ञथा सचे (proc_cmdline_कुञ्चिका_streq(कुञ्चिका, "systemd.log_ratelimit_kmsg")) {

                सचे (log_set_ratelimit_kmsg_from_string(मानम् ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", मानम्);
        }

        पटिनिवत्त 0;
}

स्थिर bool should_parse_proc_cmdline(शून्य) {
        /* PID1 always reads the kernel command line. */
        सचे (getpid_cached() == 1)
                पटिनिवत्त true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        पटिनिवत्त invoked_by_systemd();
}

शून्य log_parse_environment_variables(शून्य) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        e = getenv("SYSTEMD_LOG_TARGET");
        सचे (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        सचे (e) {
                r = log_set_max_level_from_string(e);
                सचे (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } अञ्ञथा {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                सचे (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                अञ्ञथा सचे (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        सचे (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        सचे (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        सचे (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        सचे (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        सचे (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        सचे (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

शून्य log_parse_environment(शून्य) {
        /* Do not call from library code. */

        सचे (should_parse_proc_cmdline())
                (शून्य) proc_cmdline_parse(parse_proc_cmdline_item, रिक्त, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(शून्य) {
        पटिनिवत्त log_target;
}

शून्य log_settle_target(शून्य) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        सचे (t != LOG_TARGET_AUTO)
                पटिनिवत्त;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

पूर्णाङ्क log_get_max_level(शून्य) {
        पटिनिवत्त log_max_level;
}

पूर्णाङ्क log_get_target_max_level(LogTarget target) {
        प्रतिज्ञा(target >= 0);
        प्रतिज्ञा(target < _LOG_TARGET_SINGLE_MAX);
        पटिनिवत्त log_target_max_level[target];
}

शून्य log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(शून्य) {
        पटिनिवत्त show_color > 0; /* Defaults to false. */
}

शून्य log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(शून्य) {
        पटिनिवत्त show_location;
}

शून्य log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(शून्य) {
        पटिनिवत्त show_time;
}

शून्य log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(शून्य) {
        पटिनिवत्त show_tid;
}

शून्य log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(शून्य) {
        पटिनिवत्त show_executable;
}

पूर्णाङ्क log_show_color_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        सचे (r < 0)
                पटिनिवत्त r;

        log_show_color(r);
        पटिनिवत्त 0;
}

पूर्णाङ्क log_show_location_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        सचे (r < 0)
                पटिनिवत्त r;

        log_show_location(r);
        पटिनिवत्त 0;
}

पूर्णाङ्क log_show_time_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        सचे (r < 0)
                पटिनिवत्त r;

        log_show_time(r);
        पटिनिवत्त 0;
}

पूर्णाङ्क log_show_tid_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        सचे (r < 0)
                पटिनिवत्त r;

        log_show_tid(r);
        पटिनिवत्त 0;
}

पूर्णाङ्क log_show_executable_from_string(ध्रुव वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        सचे (r < 0)
                पटिनिवत्त r;

        log_show_executable(r);
        पटिनिवत्त 0;
}

bool log_on_console(शून्य) {
        सचे (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                पटिनिवत्त true;

        पटिनिवत्त syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

स्थिर ध्रुव वर्ण *ध्रुव log_target_table[_LOG_TARGET_MAX] = {
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

शून्य log_received_signal(पूर्णाङ्क level, ध्रुव संरचना signalfd_siginfo *si) {
        प्रतिज्ञा(si);

        सचे (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                (शून्य) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } अञ्ञथा
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

शून्य set_log_syntax_callback(log_syntax_callback_t cb, शून्य *userdata) {
        प्रतिज्ञा(!log_syntax_callback || !cb);
        प्रतिज्ञा(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

पूर्णाङ्क log_syntax_internal(
                ध्रुव वर्ण *unit,
                पूर्णाङ्क level,
                ध्रुव वर्ण *config_file,
                unsigned config_पङ्क्तिः,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) {

        PROTECT_ERRNO;

        सचे (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        सचे (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                पटिनिवत्त -ERRNO_मानम्(error);

        वर्ण buffer[LINE_MAX];
        va_list ap;
        ध्रुव वर्ण *unit_fmt = रिक्त;

        errno = ERRNO_मानम्(error);

        va_start(ap, format);
        (शून्य) vsnprintf(buffer, परिमाण buffer, format, ap);
        va_end(ap);

        सचे (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        सचे (config_file) {
                सचे (config_पङ्क्तिः > 0)
                        पटिनिवत्त log_struct_internal(
                                        level,
                                        error,
                                        file, पङ्क्तिः, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_पङ्क्तिः),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_पङ्क्तिः, buffer),
                                        unit_fmt, unit,
                                        रिक्त);
                अञ्ञथा
                        पटिनिवत्त log_struct_internal(
                                        level,
                                        error,
                                        file, पङ्क्तिः, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        रिक्त);
        } अञ्ञथा सचे (unit)
                पटिनिवत्त log_struct_internal(
                                level,
                                error,
                                file, पङ्क्तिः, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                रिक्त);
        अञ्ञथा
                पटिनिवत्त log_struct_internal(
                                level,
                                error,
                                file, पङ्क्तिः, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                रिक्त);
}

पूर्णाङ्क log_syntax_invalid_utf8_internal(
                ध्रुव वर्ण *unit,
                पूर्णाङ्क level,
                ध्रुव वर्ण *config_file,
                unsigned config_पङ्क्तिः,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ वर्ण *p = रिक्त;

        सचे (rvalue)
                p = utf8_escape_invalid(rvalue);

        पटिनिवत्त log_syntax_internal(unit, level, config_file, config_पङ्क्तिः,
                                   SYNTHETIC_ERRNO(EINVAL), file, पङ्क्तिः, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

पूर्णाङ्क log_syntax_parse_error_internal(
                ध्रुव वर्ण *unit,
                ध्रुव वर्ण *config_file,
                unsigned config_पङ्क्तिः,
                पूर्णाङ्क error,
                bool critical,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *lvalue,
                ध्रुव वर्ण *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ वर्ण *escaped = रिक्त;

        /* OOM is always handled as critical. */
        सचे (ERRNO_मानम्(error) == ENOMEM)
                पटिनिवत्त log_oom_internal(LOG_ERR, file, पङ्क्तिः, func);

        सचे (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                सचे (!escaped)
                        rvalue = "(oom)";
                अञ्ञथा
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_पङ्क्तिः, error,
                            file, पङ्क्तिः, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        पटिनिवत्त critical ? -ERRNO_मानम्(error) : 0;
}

शून्य log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        सचे (b) {
                सचे (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                अञ्ञथा सचे (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

शून्य log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

शून्य log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

शून्य log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

पूर्णाङ्क log_emergency_level(शून्य) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        पटिनिवत्त getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

पूर्णाङ्क log_dup_console(शून्य) {
        पूर्णाङ्क copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        सचे (console_fd < 0 || console_fd >= 3)
                पटिनिवत्त 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        सचे (copy < 0)
                पटिनिवत्त -errno;

        console_fd = copy;
        पटिनिवत्त 0;
}

शून्य log_setup(शून्य) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (शून्य) log_open();
        सचे (log_on_console() && show_color < 0)
                log_show_color(true);
}

ध्रुव वर्ण* _log_set_prefix(ध्रुव वर्ण *prefix, bool force) {
        ध्रुव वर्ण *old = log_prefix;

        सचे (prefix || force)
                log_prefix = prefix;

        पटिनिवत्त old;
}
