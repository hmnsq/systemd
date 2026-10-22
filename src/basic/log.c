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
        [LOG_TARGET_CONSOLE] = पूर्णांक_उच्च,
        [LOG_TARGET_KMSG]    = पूर्णांक_उच्च,
        [LOG_TARGET_SYSLOG]  = पूर्णांक_उच्च,
        [LOG_TARGET_JOURNAL] = पूर्णांक_उच्च,
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

स्थिर thread_local अचर वर्ण *log_prefix = रिक्त;

#यदि LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(अभिव्यक्ति)                                                \
        गर {                                                            \
                यदि (_unlikely_(!(अभिव्यक्ति))) {                              \
                        ख_माला_दो(#expr "\n", मानक_त्रुटि);                      \
                        पात();                                        \
                }                                                       \
        } जबसम्म (false)

शून्य log_prefix_swap(अचर वर्ण **prefix) {
        प्रतिज्ञा(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

स्थिर शून्य log_close_console(शून्य) {
        /* See comment in log_close_journal() */
        (शून्य) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

स्थिर पूर्णाङ्क log_open_console(शून्य) {

        यदि (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                फर्काऊ 0;
        }

        यदि (console_fd < 3) {
                पूर्णाङ्क fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                यदि (fd < 0)
                        फर्काऊ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        फर्काऊ 0;
}

स्थिर शून्य log_close_kmsg(शून्य) {
        /* See comment in log_close_journal() */
        (शून्य) safe_close(TAKE_FD(kmsg_fd));
}

स्थिर पूर्णाङ्क log_open_kmsg(शून्य) {

        यदि (kmsg_fd >= 0)
                फर्काऊ 0;

        kmsg_fd = खोलो("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        यदि (kmsg_fd < 0)
                फर्काऊ -त्रुटि_सं;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        फर्काऊ 0;
}

स्थिर शून्य log_close_syslog(शून्य) {
        /* See comment in log_close_journal() */
        (शून्य) safe_close(TAKE_FD(syslog_fd));
}

स्थिर पूर्णाङ्क create_log_socket(पूर्णाङ्क type) {
        संरचना timeval tv;
        पूर्णाङ्क fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        यदि (fd < 0)
                फर्काऊ -त्रुटि_सं;

        fd = fd_move_above_stdio(fd);
        (शून्य) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        यदि (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        अन्यथा
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (शून्य) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, परिमाण(tv));

        फर्काऊ fd;
}

स्थिर पूर्णाङ्क log_open_syslog(शून्य) {
        पूर्णाङ्क r;

        यदि (syslog_fd >= 0)
                फर्काऊ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        यदि (syslog_fd < 0) {
                r = syslog_fd;
                गच्छ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        यदि (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                यदि (syslog_fd < 0) {
                        r = syslog_fd;
                        गच्छ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                यदि (r < 0)
                        गच्छ fail;

                syslog_is_stream = true;
        } अन्यथा
                syslog_is_stream = false;

        फर्काऊ 0;

fail:
        log_close_syslog();
        फर्काऊ r;
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

        यदि (journal_fd >= 0)
                फर्काऊ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        यदि (journal_fd < 0) {
                r = journal_fd;
                गच्छ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        यदि (r < 0)
                गच्छ fail;

        फर्काऊ 0;

fail:
        log_close_journal();
        फर्काऊ r;
}

bool stderr_is_journal(शून्य) {
        _cleanup_free_ वर्ण *w = रिक्त;
        अचर वर्ण *e;
        uint64_t dev, ino;
        संरचना stat st;

        e = दो_पर्या("JOURNAL_STREAM");
        यदि (!e)
                फर्काऊ false;

        यदि (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                फर्काऊ false;
        यदि (!e)
                फर्काऊ false;

        यदि (safe_atou64(w, &dev) < 0)
                फर्काऊ false;
        यदि (safe_atou64(e, &ino) < 0)
                फर्काऊ false;

        यदि (ख_स्थिति(STDERR_FILENO, &st) < 0)
                फर्काऊ false;

        फर्काऊ st.st_dev == dev && st.st_ino == ino;
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

        यदि (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                फर्काऊ 0;
        }

        यदि (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                यदि (!prohibit_ipc) {
                        यदि (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                यदि (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        फर्काऊ r;
                                }
                        }

                        यदि (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                यदि (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        फर्काऊ r;
                                }
                        }
                }

                यदि (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        यदि (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                फर्काऊ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        फर्काऊ log_open_console();
}

शून्य log_set_target(LogTarget target) {
        प्रतिज्ञा(target >= 0);
        प्रतिज्ञा(target < _LOG_TARGET_MAX);

        यदि (upgrade_syslog_to_journal) {
                यदि (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                अन्यथा यदि (target == LOG_TARGET_SYSLOG_OR_KMSG)
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

        फर्काऊ old;
}

शून्य log_set_facility(पूर्णाङ्क facility) {
        log_facility = facility;
}

स्थिर bool check_console_fd_is_tty(शून्य) {
        यदि (console_fd < 0)
                फर्काऊ false;

        यदि (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        फर्काऊ console_fd_is_tty;
}

स्थिर पूर्णाङ्क write_to_console(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *buffer) {

        स्थिर पूर्णाङ्क dumb = -1;

        वर्ण location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(पूर्णाङ्क) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        संरचना iovec iovec[13];
        अचर वर्ण *on = रिक्त, *off = रिक्त;
        माप_प्रकार n = 0;

        यदि (console_fd < 0)
                फर्काऊ 0;

        यदि (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        यदि (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                फर्काऊ 0;

        यदि (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        यदि (show_time &&
            format_timestamp(header_time, परिमाण(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        यदि (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        यदि (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        यदि (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, रिक्त);

        यदि (show_location) {
                अचर वर्ण *lon = "", *loff = "";
                यदि (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (शून्य) snprintf(location, परिमाण location, "%s%s:%i%s: ", lon, file, पङ्क्ति, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        यदि (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        यदि (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        यदि (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        यदि (writev(console_fd, iovec, n) < 0) {

                यदि (त्रुटि_सं == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (शून्य) log_open_console();
                        यदि (console_fd < 0)
                                फर्काऊ 0;

                        यदि (writev(console_fd, iovec, n) < 0)
                                फर्काऊ -त्रुटि_सं;
                } अन्यथा
                        फर्काऊ -त्रुटि_सं;
        }

        फर्काऊ 1;
}

स्थिर पूर्णाङ्क write_to_syslog(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *buffer) {

        वर्ण header_priority[2 + DECIMAL_STR_MAX(पूर्णाङ्क) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        संरचना पंचांग पंचांग;
        पूर्णाङ्क r;

        यदि (syslog_fd < 0)
                फर्काऊ 0;

        यदि (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                फर्काऊ 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &पंचांग);
        यदि (r < 0)
                फर्काऊ r;

        यदि (स_माला(header_time, परिमाण(header_time), "%h %e %T ", &पंचांग) <= 0)
                फर्काऊ -EINVAL;

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
        अचर संरचना msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        यदि (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        लागि (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                यदि (n < 0)
                        फर्काऊ -त्रुटि_सं;

                यदि (!syslog_is_stream)
                        रोक;

                यदि (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        रोक;
        }

        फर्काऊ 1;
}

स्थिर पूर्णाङ्क write_to_kmsg(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *buffer) {

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

        यदि (kmsg_fd < 0)
                फर्काऊ 0;

        यदि (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                फर्काऊ 0;

        यदि (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                यदि (ratelimit_अङ्क_dropped(&ratelimit) > 1)
                        फर्काऊ 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        अचर संरचना iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        यदि (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ 1;
}

स्थिर पूर्णाङ्क log_do_header(
                वर्ण *header,
                माप_प्रकार size,
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file, पूर्णाङ्क पङ्क्ति, अचर वर्ण *func,
                अचर वर्ण *object_field, अचर वर्ण *object,
                अचर वर्ण *extra_field, अचर वर्ण *extra) {
        पूर्णाङ्क r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_मान(error);

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
                     पङ्क्ति ? "CODE_LINE=" : "",
                     पङ्क्ति ? 1 : 0, पङ्क्ति, /* %.0d means no output too, special case for 0 */
                     पङ्क्ति ? "\n" : "",
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
        assert_raw((माप_प्रकार) r < size);

        फर्काऊ 0;
}

स्थिर शून्य log_do_context(संरचना iovec *iovec, माप_प्रकार iovec_len, माप_प्रकार *n) {
        प्रतिज्ञा(iovec);
        प्रतिज्ञा(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        यदि (*n + 2 >= iovec_len)
                                फर्काऊ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                लागि (माप_प्रकार i = 0; i < c->n_आगत_iovec; i++) {
                        यदि (*n + 2 >= iovec_len)
                                फर्काऊ;

                        iovec[(*n)++] = c->आगत_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                यदि (c->साँचो && c->मान) {
                        यदि (*n + 3 >= iovec_len)
                                फर्काऊ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->साँचो);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->मान);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

स्थिर पूर्णाङ्क write_to_journal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *object_field,
                अचर वर्ण *object,
                अचर वर्ण *extra_field,
                अचर वर्ण *extra,
                अचर वर्ण *buffer) {

        वर्ण header[LINE_MAX];
        माप_प्रकार n = 0, iovec_len;
        संरचना iovec *iovec;

        यदि (journal_fd < 0)
                फर्काऊ 0;

        यदि (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                फर्काऊ 0;

        यदि (!MUL_SAFE(&iovec_len, log_context_अङ्क_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        अन्यथा
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(संरचना iovec, iovec_len);

        log_do_header(header, परिमाण(header), level, error, file, पङ्क्ति, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        यदि (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        अचर संरचना msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        यदि (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ 1;
}

पूर्णाङ्क log_dispatch_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *object_field,
                अचर वर्ण *object,
                अचर वर्ण *extra_field,
                अचर वर्ण *extra,
                वर्ण *buffer) {

        assert_raw(buffer);

        यदि (log_target == LOG_TARGET_NULL)
                फर्काऊ -ERRNO_मान(error);

        /* Patch in LOG_DAEMON facility if necessary */
        यदि (LOG_FAC(level) == 0)
                level |= log_facility;

        यदि (open_when_needed)
                (शून्य) log_open();

        गर {
                वर्ण *e;
                पूर्णाङ्क k = 0;

                buffer += म_अखोज(buffer, NEWLINE);

                यदि (buffer[0] == 0)
                        रोक;

                यदि ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                यदि (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, पङ्क्ति, func, object_field, object, extra_field, extra, buffer);
                        यदि (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                यदि (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, पङ्क्ति, func, buffer);
                        यदि (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                यदि (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        यदि (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, पङ्क्ति, func, buffer);
                        यदि (k < 0) {
                                log_close_kmsg();
                                (शून्य) log_open_console();
                        }
                }

                यदि (k <= 0)
                        (शून्य) write_to_console(level, error, file, पङ्क्ति, func, buffer);

                buffer = e;
        } जबसम्म (buffer);

        यदि (open_when_needed)
                log_close();

        फर्काऊ -ERRNO_मान(error);
}

पूर्णाङ्क log_dump_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                वर्ण *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        यदि (_likely_(LOG_PRI(level) > log_max_level))
                फर्काऊ -ERRNO_मान(error);

        फर्काऊ log_dispatch_internal(level, error, file, पङ्क्ति, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

पूर्णाङ्क log_internalv(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *format,
                बहु_सूची ap) {

        यदि (_likely_(LOG_PRI(level) > log_max_level))
                फर्काऊ -ERRNO_मान(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        वर्ण buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_मान(error));

        (शून्य) vsnprintf(buffer, परिमाण buffer, format, ap);

        फर्काऊ log_dispatch_internal(level, error, file, पङ्क्ति, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

पूर्णाङ्क log_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *format, ...) {

        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, format);
        r = log_internalv(level, error, file, पङ्क्ति, func, format, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

पूर्णाङ्क log_object_internalv(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *object_field,
                अचर वर्ण *object,
                अचर वर्ण *extra_field,
                अचर वर्ण *extra,
                अचर वर्ण *format,
                बहु_सूची ap) {

        वर्ण *buffer, *b;

        यदि (_likely_(LOG_PRI(level) > log_max_level))
                फर्काऊ -ERRNO_मान(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_मान(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(वर्ण, LINE_MAX);
        (शून्य) vsnprintf(b, LINE_MAX, format, ap);

        फर्काऊ log_dispatch_internal(level, error, file, पङ्क्ति, func,
                                     object_field, object, extra_field, extra, buffer);
}

पूर्णाङ्क log_object_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *object_field,
                अचर वर्ण *object,
                अचर वर्ण *extra_field,
                अचर वर्ण *extra,
                अचर वर्ण *format, ...) {

        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, format);
        r = log_object_internalv(level, error, file, पङ्क्ति, func, object_field, object, extra_field, extra, format, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

पूर्णाङ्क log_oom_internal(पूर्णाङ्क level, अचर वर्ण *file, पूर्णाङ्क पङ्क्ति, अचर वर्ण *func) {
        फर्काऊ log_internal(level, ENOMEM, file, पङ्क्ति, func, "Out of memory.");
}

पूर्णाङ्क log_format_iovec(
                संरचना iovec *iovec,
                माप_प्रकार iovec_len,
                माप_प्रकार *n,
                bool newline_separator,
                पूर्णाङ्क error,
                अचर वर्ण *format,
                बहु_सूची ap) {

        प्रतिज्ञा(iovec);
        प्रतिज्ञा(n);

        जबसम्म (format && *n + 1 < iovec_len) {
                बहु_सूची aq;
                वर्ण *m;
                पूर्णाङ्क r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                त्रुटि_सं = ERRNO_मान(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                बहु_पूर्ण(aq);
                यदि (r < 0)
                        फर्काऊ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                यदि (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = बहु_तर्क(ap, वर्ण *);
        }
        फर्काऊ 0;
}

पूर्णाङ्क log_struct_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *format, ...) {

        वर्ण buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        बहु_सूची ap;

        यदि (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                फर्काऊ -ERRNO_मान(error);

        यदि (LOG_FAC(level) == 0)
                level |= log_facility;

        यदि (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                यदि (open_when_needed)
                        log_open_journal();

                यदि (journal_fd >= 0) {
                        वर्ण header[LINE_MAX];
                        संरचना iovec *iovec;
                        माप_प्रकार n = 0, m, iovec_len;
                        पूर्णाङ्क r;
                        bool fallback = false;

                        यदि (!MUL_SAFE(&iovec_len, log_context_अङ्क_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        अन्यथा
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(संरचना iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, परिमाण(header), level, error, file, पङ्क्ति, func, रिक्त, रिक्त, रिक्त, रिक्त);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        बहु_शुरू(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        यदि (r < 0)
                                fallback = true;
                        अन्यथा {
                                log_do_context(iovec, iovec_len, &n);

                                अचर संरचना msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (शून्य) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        बहु_पूर्ण(ap);
                        लागि (माप_प्रकार i = 1; i < m; i += 2)
                                मुक्त(iovec[i].iov_base);

                        यदि (!fallback) {
                                यदि (open_when_needed)
                                        log_close();

                                फर्काऊ -ERRNO_मान(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        बहु_शुरू(ap, format);
        जबसम्म (format) {
                बहु_सूची aq;

                त्रुटि_सं = ERRNO_मान(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (शून्य) vsnprintf(buf, परिमाण buf, format, aq);
                REENABLE_WARNING;
                बहु_पूर्ण(aq);

                यदि (startswith(buf, "MESSAGE=")) {
                        found = true;
                        रोक;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = बहु_तर्क(ap, वर्ण *);
        }
        बहु_पूर्ण(ap);

        यदि (!found) {
                यदि (open_when_needed)
                        log_close();

                फर्काऊ -ERRNO_मान(error);
        }

        फर्काऊ log_dispatch_internal(level, error, file, पङ्क्ति, func, रिक्त, रिक्त, रिक्त, रिक्त, buf + 8);
}

पूर्णाङ्क log_struct_iovec_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर संरचना iovec आगत_iovec[],
                माप_प्रकार n_आगत_iovec) {

        PROTECT_ERRNO;

        यदि (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                फर्काऊ -ERRNO_मान(error);

        यदि (LOG_FAC(level) == 0)
                level |= log_facility;

        यदि (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                वर्ण header[LINE_MAX];
                संरचना iovec *iovec;
                माप_प्रकार n = 0, iovec_len, n_context_iovec, n_आगत_iovec_expanded;

                यदि (!MUL_SAFE(&n_आगत_iovec_expanded, n_आगत_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_अङ्क_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_आगत_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                अन्यथा
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(संरचना iovec, iovec_len);

                log_do_header(header, परिमाण(header), level, error, file, पङ्क्ति, func, रिक्त, रिक्त, रिक्त, रिक्त);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                लागि (माप_प्रकार i = 0; i < n_आगत_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = आगत_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                अचर संरचना msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                यदि (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        फर्काऊ -ERRNO_मान(error);
        }

        लागि (माप_प्रकार i = 0; i < n_आगत_iovec; i++)
                यदि (memory_startswith(आगत_iovec[i].iov_base, आगत_iovec[i].iov_len, "MESSAGE=")) {
                        वर्ण *m;

                        m = strndupa_safe((वर्ण*) आगत_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          आगत_iovec[i].iov_len - STRLEN("MESSAGE="));

                        फर्काऊ log_dispatch_internal(level, error, file, पङ्क्ति, func, रिक्त, रिक्त, रिक्त, रिक्त, m);
                }

        /* Couldn't find MESSAGE=. */
        फर्काऊ -ERRNO_मान(error);
}

पूर्णाङ्क log_set_target_from_string(अचर वर्ण *e) {
        LogTarget t;

        t = log_target_from_string(e);
        यदि (t < 0)
                फर्काऊ t;

        log_set_target(t);
        फर्काऊ 0;
}

पूर्णाङ्क log_set_max_level_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        लागि (;;) {
                _cleanup_free_ वर्ण *word = रिक्त, *prefix = रिक्त;
                LogTarget target;
                अचर वर्ण *colon;

                r = extract_first_word(&e, &word, ",", 0);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                colon = म_अक्षर(word, ':');
                यदि (!colon) {
                        r = log_level_from_string(word);
                        यदि (r < 0)
                                फर्काऊ r;

                        log_set_max_level(r);
                        जारी;
                }

                prefix = strndup(word, colon - word);
                यदि (!prefix)
                        फर्काऊ -ENOMEM;

                target = log_target_from_string(prefix);
                यदि (target < 0)
                        फर्काऊ target;

                यदि (target >= _LOG_TARGET_SINGLE_MAX)
                        फर्काऊ -EINVAL;

                r = log_level_from_string(colon + 1);
                यदि (r < 0)
                        फर्काऊ r;

                log_target_max_level[target] = r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क log_max_levels_to_string(पूर्णाङ्क level, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = log_level_to_string_alloc(level, &s);
        यदि (r < 0)
                फर्काऊ r;

        लागि (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ वर्ण *l = रिक्त;

                यदि (log_target_max_level[target] == पूर्णांक_उच्च)
                        जारी;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                यदि (r < 0)
                        फर्काऊ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *ret = TAKE_PTR(s);
        फर्काऊ 0;
}

स्थिर पूर्णाङ्क log_set_ratelimit_kmsg_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        यदि (r < 0)
                फर्काऊ r;

        ratelimit_kmsg = r;
        फर्काऊ 0;
}

स्थिर पूर्णाङ्क parse_proc_cmdline_item(अचर वर्ण *साँचो, अचर वर्ण *मान, शून्य *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        यदि (streq(साँचो, "debug") && !मान)
                log_set_max_level(LOG_DEBUG);

        अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_target")) {

                यदि (proc_cmdline_मान_missing(साँचो, मान))
                        फर्काऊ 0;

                यदि (log_set_target_from_string(मान) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", मान);

        } अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_level")) {

                यदि (proc_cmdline_मान_missing(साँचो, मान))
                        फर्काऊ 0;

                यदि (log_set_max_level_from_string(मान) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", मान);

        } अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_color")) {

                यदि (log_show_color_from_string(मान ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", मान);

        } अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_location")) {

                यदि (log_show_location_from_string(मान ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", मान);

        } अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_tid")) {

                यदि (log_show_tid_from_string(मान ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", मान);

        } अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_time")) {

                यदि (log_show_time_from_string(मान ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", मान);

        } अन्यथा यदि (proc_cmdline_साँचो_streq(साँचो, "systemd.log_ratelimit_kmsg")) {

                यदि (log_set_ratelimit_kmsg_from_string(मान ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", मान);
        }

        फर्काऊ 0;
}

स्थिर bool should_parse_proc_cmdline(शून्य) {
        /* PID1 always reads the kernel command line. */
        यदि (getpid_cached() == 1)
                फर्काऊ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        फर्काऊ invoked_by_systemd();
}

शून्य log_parse_environment_variables(शून्य) {
        अचर वर्ण *e;
        पूर्णाङ्क r;

        e = दो_पर्या("SYSTEMD_LOG_TARGET");
        यदि (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_LEVEL");
        यदि (e) {
                r = log_set_max_level_from_string(e);
                यदि (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } अन्यथा {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                यदि (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                अन्यथा यदि (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = दो_पर्या("SYSTEMD_LOG_COLOR");
        यदि (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_LOCATION");
        यदि (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_TIME");
        यदि (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_TID");
        यदि (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_RATELIMIT_KMSG");
        यदि (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_EXECUTABLE");
        यदि (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

शून्य log_parse_environment(शून्य) {
        /* Do not call from library code. */

        यदि (should_parse_proc_cmdline())
                (शून्य) proc_cmdline_parse(parse_proc_cmdline_item, रिक्त, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(शून्य) {
        फर्काऊ log_target;
}

शून्य log_settle_target(शून्य) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        यदि (t != LOG_TARGET_AUTO)
                फर्काऊ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

पूर्णाङ्क log_get_max_level(शून्य) {
        फर्काऊ log_max_level;
}

पूर्णाङ्क log_get_target_max_level(LogTarget target) {
        प्रतिज्ञा(target >= 0);
        प्रतिज्ञा(target < _LOG_TARGET_SINGLE_MAX);
        फर्काऊ log_target_max_level[target];
}

शून्य log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(शून्य) {
        फर्काऊ show_color > 0; /* Defaults to false. */
}

शून्य log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(शून्य) {
        फर्काऊ show_location;
}

शून्य log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(शून्य) {
        फर्काऊ show_time;
}

शून्य log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(शून्य) {
        फर्काऊ show_tid;
}

शून्य log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(शून्य) {
        फर्काऊ show_executable;
}

पूर्णाङ्क log_show_color_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        यदि (r < 0)
                फर्काऊ r;

        log_show_color(r);
        फर्काऊ 0;
}

पूर्णाङ्क log_show_location_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        यदि (r < 0)
                फर्काऊ r;

        log_show_location(r);
        फर्काऊ 0;
}

पूर्णाङ्क log_show_time_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        यदि (r < 0)
                फर्काऊ r;

        log_show_time(r);
        फर्काऊ 0;
}

पूर्णाङ्क log_show_tid_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        यदि (r < 0)
                फर्काऊ r;

        log_show_tid(r);
        फर्काऊ 0;
}

पूर्णाङ्क log_show_executable_from_string(अचर वर्ण *e) {
        पूर्णाङ्क r;

        r = parse_boolean(e);
        यदि (r < 0)
                फर्काऊ r;

        log_show_executable(r);
        फर्काऊ 0;
}

bool log_on_console(शून्य) {
        यदि (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                फर्काऊ true;

        फर्काऊ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

स्थिर अचर वर्ण *अचर log_target_table[_LOG_TARGET_MAX] = {
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

शून्य log_received_signal(पूर्णाङ्क level, अचर संरचना signalfd_siginfo *si) {
        प्रतिज्ञा(si);

        यदि (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                (शून्य) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } अन्यथा
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
                अचर वर्ण *unit,
                पूर्णाङ्क level,
                अचर वर्ण *config_file,
                अचिन्हित config_पङ्क्ति,
                पूर्णाङ्क error,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *format, ...) {

        PROTECT_ERRNO;

        यदि (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        यदि (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                फर्काऊ -ERRNO_मान(error);

        वर्ण buffer[LINE_MAX];
        बहु_सूची ap;
        अचर वर्ण *unit_fmt = रिक्त;

        त्रुटि_सं = ERRNO_मान(error);

        बहु_शुरू(ap, format);
        (शून्य) vsnprintf(buffer, परिमाण buffer, format, ap);
        बहु_पूर्ण(ap);

        यदि (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        यदि (config_file) {
                यदि (config_पङ्क्ति > 0)
                        फर्काऊ log_struct_internal(
                                        level,
                                        error,
                                        file, पङ्क्ति, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_पङ्क्ति),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_पङ्क्ति, buffer),
                                        unit_fmt, unit,
                                        रिक्त);
                अन्यथा
                        फर्काऊ log_struct_internal(
                                        level,
                                        error,
                                        file, पङ्क्ति, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        रिक्त);
        } अन्यथा यदि (unit)
                फर्काऊ log_struct_internal(
                                level,
                                error,
                                file, पङ्क्ति, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                रिक्त);
        अन्यथा
                फर्काऊ log_struct_internal(
                                level,
                                error,
                                file, पङ्क्ति, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                रिक्त);
}

पूर्णाङ्क log_syntax_invalid_utf8_internal(
                अचर वर्ण *unit,
                पूर्णाङ्क level,
                अचर वर्ण *config_file,
                अचिन्हित config_पङ्क्ति,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ वर्ण *p = रिक्त;

        यदि (rvalue)
                p = utf8_escape_invalid(rvalue);

        फर्काऊ log_syntax_internal(unit, level, config_file, config_पङ्क्ति,
                                   SYNTHETIC_ERRNO(EINVAL), file, पङ्क्ति, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

पूर्णाङ्क log_syntax_parse_error_internal(
                अचर वर्ण *unit,
                अचर वर्ण *config_file,
                अचिन्हित config_पङ्क्ति,
                पूर्णाङ्क error,
                bool critical,
                अचर वर्ण *file,
                पूर्णाङ्क पङ्क्ति,
                अचर वर्ण *func,
                अचर वर्ण *lvalue,
                अचर वर्ण *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ वर्ण *escaped = रिक्त;

        /* OOM is always handled as critical. */
        यदि (ERRNO_मान(error) == ENOMEM)
                फर्काऊ log_oom_internal(LOG_ERR, file, पङ्क्ति, func);

        यदि (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                यदि (!escaped)
                        rvalue = "(oom)";
                अन्यथा
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_पङ्क्ति, error,
                            file, पङ्क्ति, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        फर्काऊ critical ? -ERRNO_मान(error) : 0;
}

शून्य log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        यदि (b) {
                यदि (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                अन्यथा यदि (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
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

        फर्काऊ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

पूर्णाङ्क log_dup_console(शून्य) {
        पूर्णाङ्क copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        यदि (console_fd < 0 || console_fd >= 3)
                फर्काऊ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        यदि (copy < 0)
                फर्काऊ -त्रुटि_सं;

        console_fd = copy;
        फर्काऊ 0;
}

शून्य log_setup(शून्य) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (शून्य) log_open();
        यदि (log_on_console() && show_color < 0)
                log_show_color(true);
}

अचर वर्ण* _log_set_prefix(अचर वर्ण *prefix, bool force) {
        अचर वर्ण *old = log_prefix;

        यदि (prefix || force)
                log_prefix = prefix;

        फर्काऊ old;
}
