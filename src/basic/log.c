/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <sys/signalfd.h>
#समावेश <sys/stat.h>
#समावेश <sys/uio.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "sd-messages.h"

#समावेश "alloc-util.h"
#समावेश "ansi-color.h"
#समावेश "argv-util.h"
#समावेश "env-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "format-util.h"
#समावेश "iovec-util.h"
#समावेश "list.h"
#समावेश "log.h"
#समावेश "log-context.h"
#समावेश "parse-util.h"
#समावेश "proc-cmdline.h"
#समावेश "process-util.h"
#समावेश "ratelimit.h"
#समावेश "signal-util.h"
#समावेश "socket-util.h"
#समावेश "stdio-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "syslog-util.h"
#समावेश "terminal-util.h"
#समावेश "time-util.h"
#समावेश "utf8.h"

#घोषणा SNDBUF_आकार (8*1024*1024)
#घोषणा IOVEC_MAX 256U

अटल log_syntax_callback_t log_syntax_callback = शून्य;
अटल व्योम *log_syntax_callback_userdata = शून्य;

अटल LogTarget log_target = LOG_TARGET_CONSOLE;
अटल पूर्णांक log_max_level = LOG_INFO;
अटल पूर्णांक log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = पूर्णांक_उच्च,
        [LOG_TARGET_KMSG]    = पूर्णांक_उच्च,
        [LOG_TARGET_SYSLOG]  = पूर्णांक_उच्च,
        [LOG_TARGET_JOURNAL] = पूर्णांक_उच्च,
};
अटल पूर्णांक log_facility = LOG_DAEMON;
अटल bool ratelimit_kmsg = true;

अटल पूर्णांक console_fd = STDERR_FILENO;
अटल पूर्णांक console_fd_is_tty = -1; /* tri-state: -1 means don't know */
अटल पूर्णांक syslog_fd = -EBADF;
अटल पूर्णांक kmsg_fd = -EBADF;
अटल पूर्णांक journal_fd = -EBADF;

अटल bool syslog_is_stream = false;

अटल पूर्णांक show_color = -1; /* tristate */
अटल bool show_location = false;
अटल bool show_time = false;
अटल bool show_tid = false;
अटल bool show_executable = false;

अटल bool upgrade_syslog_to_journal = false;
अटल bool always_reopen_console = false;
अटल bool open_when_needed = false;
अटल bool prohibit_ipc = false;

अटल thread_local स्थिर अक्षर *log_prefix = शून्य;

#अगर LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#पूर्ण_अगर

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#घोषणा assert_raw(व्यंजक)                                                \
        करो {                                                            \
                अगर (_unlikely_(!(व्यंजक))) {                              \
                        ख_माला_दो(#expr "\n", मानक_त्रुटि);                      \
                        पात();                                        \
                }                                                       \
        } जबतक (false)

व्योम log_prefix_swap(स्थिर अक्षर **prefix) {
        निश्चित(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

अटल व्योम log_close_console(व्योम) {
        /* See comment in log_close_journal() */
        (व्योम) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

अटल पूर्णांक log_open_console(व्योम) {

        अगर (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                वापस 0;
        }

        अगर (console_fd < 3) {
                पूर्णांक fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                अगर (fd < 0)
                        वापस fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        वापस 0;
}

अटल व्योम log_close_kmsg(व्योम) {
        /* See comment in log_close_journal() */
        (व्योम) safe_close(TAKE_FD(kmsg_fd));
}

अटल पूर्णांक log_open_kmsg(व्योम) {

        अगर (kmsg_fd >= 0)
                वापस 0;

        kmsg_fd = खोलो("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        अगर (kmsg_fd < 0)
                वापस -त्रुटि_सं;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        वापस 0;
}

अटल व्योम log_close_syslog(व्योम) {
        /* See comment in log_close_journal() */
        (व्योम) safe_close(TAKE_FD(syslog_fd));
}

अटल पूर्णांक create_log_socket(पूर्णांक type) {
        काष्ठा timeval tv;
        पूर्णांक fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        fd = fd_move_above_stdio(fd);
        (व्योम) fd_inc_sndbuf(fd, SNDBUF_आकार);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        अगर (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        अन्यथा
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (व्योम) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, माप(tv));

        वापस fd;
}

अटल पूर्णांक log_open_syslog(व्योम) {
        पूर्णांक r;

        अगर (syslog_fd >= 0)
                वापस 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        अगर (syslog_fd < 0) {
                r = syslog_fd;
                जाओ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        अगर (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                अगर (syslog_fd < 0) {
                        r = syslog_fd;
                        जाओ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                अगर (r < 0)
                        जाओ fail;

                syslog_is_stream = true;
        } अन्यथा
                syslog_is_stream = false;

        वापस 0;

fail:
        log_close_syslog();
        वापस r;
}

अटल व्योम log_close_journal(व्योम) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (व्योम) safe_close(TAKE_FD(journal_fd));
}

अटल पूर्णांक log_open_journal(व्योम) {
        पूर्णांक r;

        अगर (journal_fd >= 0)
                वापस 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        अगर (journal_fd < 0) {
                r = journal_fd;
                जाओ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        अगर (r < 0)
                जाओ fail;

        वापस 0;

fail:
        log_close_journal();
        वापस r;
}

bool stderr_is_journal(व्योम) {
        _cleanup_free_ अक्षर *w = शून्य;
        स्थिर अक्षर *e;
        uint64_t dev, ino;
        काष्ठा stat st;

        e = दो_पर्या("JOURNAL_STREAM");
        अगर (!e)
                वापस false;

        अगर (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                वापस false;
        अगर (!e)
                वापस false;

        अगर (safe_atou64(w, &dev) < 0)
                वापस false;
        अगर (safe_atou64(e, &ino) < 0)
                वापस false;

        अगर (ख_स्थिति(STDERR_FILENO, &st) < 0)
                वापस false;

        वापस st.st_dev == dev && st.st_ino == ino;
}

पूर्णांक log_open(व्योम) {
        पूर्णांक r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        अगर (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                वापस 0;
        }

        अगर (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                अगर (!prohibit_ipc) {
                        अगर (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                अगर (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        वापस r;
                                }
                        }

                        अगर (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                अगर (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        वापस r;
                                }
                        }
                }

                अगर (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        अगर (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                वापस r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        वापस log_open_console();
}

व्योम log_set_target(LogTarget target) {
        निश्चित(target >= 0);
        निश्चित(target < _LOG_TARGET_MAX);

        अगर (upgrade_syslog_to_journal) {
                अगर (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                अन्यथा अगर (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

व्योम log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

व्योम log_close(व्योम) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

व्योम log_forget_fds(व्योम) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

पूर्णांक log_set_max_level(पूर्णांक level) {
        निश्चित(level == LOG_NULL || log_level_is_valid(level));

        पूर्णांक old = log_max_level;
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

        वापस old;
}

व्योम log_set_facility(पूर्णांक facility) {
        log_facility = facility;
}

अटल bool check_console_fd_is_tty(व्योम) {
        अगर (console_fd < 0)
                वापस false;

        अगर (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        वापस console_fd_is_tty;
}

अटल पूर्णांक write_to_console(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *बफर) {

        अटल पूर्णांक dumb = -1;

        अक्षर location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(पूर्णांक) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        काष्ठा iovec iovec[13];
        स्थिर अक्षर *on = शून्य, *off = शून्य;
        माप_प्रकार n = 0;

        अगर (console_fd < 0)
                वापस 0;

        अगर (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        अगर (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                वापस 0;

        अगर (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        अगर (show_time &&
            format_timestamp(header_time, माप(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        अगर (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        अगर (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        अगर (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, शून्य);

        अगर (show_location) {
                स्थिर अक्षर *lon = "", *loff = "";
                अगर (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (व्योम) snprintf(location, माप location, "%s%s:%i%s: ", lon, file, पंक्ति, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        अगर (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        अगर (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(बफर);
        अगर (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        अगर (writev(console_fd, iovec, n) < 0) {

                अगर (त्रुटि_सं == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (व्योम) log_open_console();
                        अगर (console_fd < 0)
                                वापस 0;

                        अगर (writev(console_fd, iovec, n) < 0)
                                वापस -त्रुटि_सं;
                } अन्यथा
                        वापस -त्रुटि_सं;
        }

        वापस 1;
}

अटल पूर्णांक write_to_syslog(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *बफर) {

        अक्षर header_priority[2 + DECIMAL_STR_MAX(पूर्णांक) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        काष्ठा पंचांग पंचांग;
        पूर्णांक r;

        अगर (syslog_fd < 0)
                वापस 0;

        अगर (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                वापस 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &पंचांग);
        अगर (r < 0)
                वापस r;

        अगर (स_माला(header_time, माप(header_time), "%h %e %T ", &पंचांग) <= 0)
                वापस -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        काष्ठा iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(बफर),
        };
        स्थिर काष्ठा msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        अगर (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        क्रम (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                अगर (n < 0)
                        वापस -त्रुटि_सं;

                अगर (!syslog_is_stream)
                        अवरोध;

                अगर (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        अवरोध;
        }

        वापस 1;
}

अटल पूर्णांक write_to_kmsg(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *बफर) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        अटल thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        अक्षर header_priority[2 + DECIMAL_STR_MAX(पूर्णांक) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        अगर (kmsg_fd < 0)
                वापस 0;

        अगर (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                वापस 0;

        अगर (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                अगर (ratelimit_अंक_dropped(&ratelimit) > 1)
                        वापस 0;

                बफर = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        स्थिर काष्ठा iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(बफर),
                IOVEC_MAKE_STRING("\n"),
        };

        अगर (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                वापस -त्रुटि_सं;

        वापस 1;
}

अटल पूर्णांक log_do_header(
                अक्षर *header,
                माप_प्रकार आकार,
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file, पूर्णांक पंक्ति, स्थिर अक्षर *func,
                स्थिर अक्षर *object_field, स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field, स्थिर अक्षर *extra) {
        पूर्णांक r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_मूल्य(error);

        r = snprintf(header, आकार,
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
                     पंक्ति ? "CODE_LINE=" : "",
                     पंक्ति ? 1 : 0, पंक्ति, /* %.0d means no output too, special case for 0 */
                     पंक्ति ? "\n" : "",
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
        assert_raw((माप_प्रकार) r < आकार);

        वापस 0;
}

अटल व्योम log_do_context(काष्ठा iovec *iovec, माप_प्रकार iovec_len, माप_प्रकार *n) {
        निश्चित(iovec);
        निश्चित(n);

        LIST_FOREACH(ll, c, log_context_शीर्ष()) {
                STRV_FOREACH(s, c->fields) {
                        अगर (*n + 2 >= iovec_len)
                                वापस;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                क्रम (माप_प्रकार i = 0; i < c->n_निवेश_iovec; i++) {
                        अगर (*n + 2 >= iovec_len)
                                वापस;

                        iovec[(*n)++] = c->निवेश_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                अगर (c->कुंजी && c->मूल्य) {
                        अगर (*n + 3 >= iovec_len)
                                वापस;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->कुंजी);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->मूल्य);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

अटल पूर्णांक write_to_journal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                स्थिर अक्षर *बफर) {

        अक्षर header[LINE_MAX];
        माप_प्रकार n = 0, iovec_len;
        काष्ठा iovec *iovec;

        अगर (journal_fd < 0)
                वापस 0;

        अगर (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                वापस 0;

        अगर (!MUL_SAFE(&iovec_len, log_context_अंक_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        अन्यथा
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(काष्ठा iovec, iovec_len);

        log_do_header(header, माप(header), level, error, file, पंक्ति, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        अगर (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(बफर);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        स्थिर काष्ठा msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        अगर (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                वापस -त्रुटि_सं;

        वापस 1;
}

पूर्णांक log_dispatch_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                अक्षर *बफर) {

        assert_raw(बफर);

        अगर (log_target == LOG_TARGET_NULL)
                वापस -ERRNO_मूल्य(error);

        /* Patch in LOG_DAEMON facility if necessary */
        अगर (LOG_FAC(level) == 0)
                level |= log_facility;

        अगर (open_when_needed)
                (व्योम) log_open();

        करो {
                अक्षर *e;
                पूर्णांक k = 0;

                बफर += म_अखोज(बफर, NEWLINE);

                अगर (बफर[0] == 0)
                        अवरोध;

                अगर ((e = strpbrk(बफर, NEWLINE)))
                        *(e++) = 0;

                अगर (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, पंक्ति, func, object_field, object, extra_field, extra, बफर);
                        अगर (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                अगर (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, पंक्ति, func, बफर);
                        अगर (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                अगर (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        अगर (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, पंक्ति, func, बफर);
                        अगर (k < 0) {
                                log_close_kmsg();
                                (व्योम) log_open_console();
                        }
                }

                अगर (k <= 0)
                        (व्योम) write_to_console(level, error, file, पंक्ति, func, बफर);

                बफर = e;
        } जबतक (बफर);

        अगर (open_when_needed)
                log_close();

        वापस -ERRNO_मूल्य(error);
}

पूर्णांक log_dump_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                अक्षर *बफर) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        अगर (_likely_(LOG_PRI(level) > log_max_level))
                वापस -ERRNO_मूल्य(error);

        वापस log_dispatch_internal(level, error, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य, बफर);
}

पूर्णांक log_internalv(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format,
                बहु_सूची ap) {

        अगर (_likely_(LOG_PRI(level) > log_max_level))
                वापस -ERRNO_मूल्य(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        अक्षर बफर[LINE_MAX];
        LOCAL_ERRNO(ERRNO_मूल्य(error));

        (व्योम) vsnprintf(बफर, माप बफर, format, ap);

        वापस log_dispatch_internal(level, error, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य, बफर);
}

पूर्णांक log_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format, ...) {

        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = log_internalv(level, error, file, पंक्ति, func, format, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक log_object_internalv(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                स्थिर अक्षर *format,
                बहु_सूची ap) {

        अक्षर *बफर, *b;

        अगर (_likely_(LOG_PRI(level) > log_max_level))
                वापस -ERRNO_मूल्य(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_मूल्य(error));

        LOG_SET_PREFIX(object);

        b = बफर = newa(अक्षर, LINE_MAX);
        (व्योम) vsnprintf(b, LINE_MAX, format, ap);

        वापस log_dispatch_internal(level, error, file, पंक्ति, func,
                                     object_field, object, extra_field, extra, बफर);
}

पूर्णांक log_object_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                स्थिर अक्षर *format, ...) {

        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = log_object_internalv(level, error, file, पंक्ति, func, object_field, object, extra_field, extra, format, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक log_oom_internal(पूर्णांक level, स्थिर अक्षर *file, पूर्णांक पंक्ति, स्थिर अक्षर *func) {
        वापस log_internal(level, ENOMEM, file, पंक्ति, func, "Out of memory.");
}

पूर्णांक log_format_iovec(
                काष्ठा iovec *iovec,
                माप_प्रकार iovec_len,
                माप_प्रकार *n,
                bool newline_separator,
                पूर्णांक error,
                स्थिर अक्षर *format,
                बहु_सूची ap) {

        निश्चित(iovec);
        निश्चित(n);

        जबतक (format && *n + 1 < iovec_len) {
                बहु_सूची aq;
                अक्षर *m;
                पूर्णांक r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                त्रुटि_सं = ERRNO_मूल्य(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                बहु_पूर्ण(aq);
                अगर (r < 0)
                        वापस -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                अगर (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = बहु_तर्क(ap, अक्षर *);
        }
        वापस 0;
}

पूर्णांक log_struct_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format, ...) {

        अक्षर buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        बहु_सूची ap;

        अगर (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                वापस -ERRNO_मूल्य(error);

        अगर (LOG_FAC(level) == 0)
                level |= log_facility;

        अगर (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                अगर (open_when_needed)
                        log_open_journal();

                अगर (journal_fd >= 0) {
                        अक्षर header[LINE_MAX];
                        काष्ठा iovec *iovec;
                        माप_प्रकार n = 0, m, iovec_len;
                        पूर्णांक r;
                        bool fallback = false;

                        अगर (!MUL_SAFE(&iovec_len, log_context_अंक_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        अन्यथा
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(काष्ठा iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, माप(header), level, error, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        बहु_शुरू(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        अगर (r < 0)
                                fallback = true;
                        अन्यथा {
                                log_do_context(iovec, iovec_len, &n);

                                स्थिर काष्ठा msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (व्योम) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        बहु_पूर्ण(ap);
                        क्रम (माप_प्रकार i = 1; i < m; i += 2)
                                मुक्त(iovec[i].iov_base);

                        अगर (!fallback) {
                                अगर (open_when_needed)
                                        log_close();

                                वापस -ERRNO_मूल्य(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        बहु_शुरू(ap, format);
        जबतक (format) {
                बहु_सूची aq;

                त्रुटि_सं = ERRNO_मूल्य(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (व्योम) vsnprintf(buf, माप buf, format, aq);
                REENABLE_WARNING;
                बहु_पूर्ण(aq);

                अगर (startswith(buf, "MESSAGE=")) {
                        found = true;
                        अवरोध;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = बहु_तर्क(ap, अक्षर *);
        }
        बहु_पूर्ण(ap);

        अगर (!found) {
                अगर (open_when_needed)
                        log_close();

                वापस -ERRNO_मूल्य(error);
        }

        वापस log_dispatch_internal(level, error, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य, buf + 8);
}

पूर्णांक log_struct_iovec_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर काष्ठा iovec निवेश_iovec[],
                माप_प्रकार n_निवेश_iovec) {

        PROTECT_ERRNO;

        अगर (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                वापस -ERRNO_मूल्य(error);

        अगर (LOG_FAC(level) == 0)
                level |= log_facility;

        अगर (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                अक्षर header[LINE_MAX];
                काष्ठा iovec *iovec;
                माप_प्रकार n = 0, iovec_len, n_context_iovec, n_निवेश_iovec_expanded;

                अगर (!MUL_SAFE(&n_निवेश_iovec_expanded, n_निवेश_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_अंक_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_निवेश_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                अन्यथा
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(काष्ठा iovec, iovec_len);

                log_do_header(header, माप(header), level, error, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                क्रम (माप_प्रकार i = 0; i < n_निवेश_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = निवेश_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                स्थिर काष्ठा msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                अगर (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        वापस -ERRNO_मूल्य(error);
        }

        क्रम (माप_प्रकार i = 0; i < n_निवेश_iovec; i++)
                अगर (memory_startswith(निवेश_iovec[i].iov_base, निवेश_iovec[i].iov_len, "MESSAGE=")) {
                        अक्षर *m;

                        m = strndupa_safe((अक्षर*) निवेश_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          निवेश_iovec[i].iov_len - STRLEN("MESSAGE="));

                        वापस log_dispatch_internal(level, error, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य, m);
                }

        /* Couldn't find MESSAGE=. */
        वापस -ERRNO_मूल्य(error);
}

पूर्णांक log_set_target_from_string(स्थिर अक्षर *e) {
        LogTarget t;

        t = log_target_from_string(e);
        अगर (t < 0)
                वापस t;

        log_set_target(t);
        वापस 0;
}

पूर्णांक log_set_max_level_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *word = शून्य, *prefix = शून्य;
                LogTarget target;
                स्थिर अक्षर *colon;

                r = extract_first_word(&e, &word, ",", 0);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                colon = म_अक्षर(word, ':');
                अगर (!colon) {
                        r = log_level_from_string(word);
                        अगर (r < 0)
                                वापस r;

                        log_set_max_level(r);
                        जारी;
                }

                prefix = strndup(word, colon - word);
                अगर (!prefix)
                        वापस -ENOMEM;

                target = log_target_from_string(prefix);
                अगर (target < 0)
                        वापस target;

                अगर (target >= _LOG_TARGET_SINGLE_MAX)
                        वापस -EINVAL;

                r = log_level_from_string(colon + 1);
                अगर (r < 0)
                        वापस r;

                log_target_max_level[target] = r;
        }

        वापस 0;
}

पूर्णांक log_max_levels_to_string(पूर्णांक level, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = log_level_to_string_alloc(level, &s);
        अगर (r < 0)
                वापस r;

        क्रम (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ अक्षर *l = शून्य;

                अगर (log_target_max_level[target] == पूर्णांक_उच्च)
                        जारी;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                अगर (r < 0)
                        वापस r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(s);
        वापस 0;
}

अटल पूर्णांक log_set_ratelimit_kmsg_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        r = parse_boolean(e);
        अगर (r < 0)
                वापस r;

        ratelimit_kmsg = r;
        वापस 0;
}

अटल पूर्णांक parse_proc_cmdline_item(स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य, व्योम *तथ्य) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        अगर (streq(कुंजी, "debug") && !मूल्य)
                log_set_max_level(LOG_DEBUG);

        अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_target")) {

                अगर (proc_cmdline_मूल्य_missing(कुंजी, मूल्य))
                        वापस 0;

                अगर (log_set_target_from_string(मूल्य) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", मूल्य);

        } अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_level")) {

                अगर (proc_cmdline_मूल्य_missing(कुंजी, मूल्य))
                        वापस 0;

                अगर (log_set_max_level_from_string(मूल्य) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", मूल्य);

        } अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_color")) {

                अगर (log_show_color_from_string(मूल्य ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", मूल्य);

        } अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_location")) {

                अगर (log_show_location_from_string(मूल्य ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", मूल्य);

        } अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_tid")) {

                अगर (log_show_tid_from_string(मूल्य ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", मूल्य);

        } अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_time")) {

                अगर (log_show_time_from_string(मूल्य ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", मूल्य);

        } अन्यथा अगर (proc_cmdline_कुंजी_streq(कुंजी, "systemd.log_ratelimit_kmsg")) {

                अगर (log_set_ratelimit_kmsg_from_string(मूल्य ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", मूल्य);
        }

        वापस 0;
}

अटल bool should_parse_proc_cmdline(व्योम) {
        /* PID1 always reads the kernel command line. */
        अगर (getpid_cached() == 1)
                वापस true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        वापस invoked_by_systemd();
}

व्योम log_parse_environment_variables(व्योम) {
        स्थिर अक्षर *e;
        पूर्णांक r;

        e = दो_पर्या("SYSTEMD_LOG_TARGET");
        अगर (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_LEVEL");
        अगर (e) {
                r = log_set_max_level_from_string(e);
                अगर (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } अन्यथा {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                अगर (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                अन्यथा अगर (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = दो_पर्या("SYSTEMD_LOG_COLOR");
        अगर (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_LOCATION");
        अगर (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_TIME");
        अगर (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_TID");
        अगर (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_RATELIMIT_KMSG");
        अगर (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = दो_पर्या("SYSTEMD_LOG_EXECUTABLE");
        अगर (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

व्योम log_parse_environment(व्योम) {
        /* Do not call from library code. */

        अगर (should_parse_proc_cmdline())
                (व्योम) proc_cmdline_parse(parse_proc_cmdline_item, शून्य, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(व्योम) {
        वापस log_target;
}

व्योम log_settle_target(व्योम) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        अगर (t != LOG_TARGET_AUTO)
                वापस;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

पूर्णांक log_get_max_level(व्योम) {
        वापस log_max_level;
}

पूर्णांक log_get_target_max_level(LogTarget target) {
        निश्चित(target >= 0);
        निश्चित(target < _LOG_TARGET_SINGLE_MAX);
        वापस log_target_max_level[target];
}

व्योम log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(व्योम) {
        वापस show_color > 0; /* Defaults to false. */
}

व्योम log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(व्योम) {
        वापस show_location;
}

व्योम log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(व्योम) {
        वापस show_time;
}

व्योम log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(व्योम) {
        वापस show_tid;
}

व्योम log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(व्योम) {
        वापस show_executable;
}

पूर्णांक log_show_color_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        r = parse_boolean(e);
        अगर (r < 0)
                वापस r;

        log_show_color(r);
        वापस 0;
}

पूर्णांक log_show_location_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        r = parse_boolean(e);
        अगर (r < 0)
                वापस r;

        log_show_location(r);
        वापस 0;
}

पूर्णांक log_show_time_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        r = parse_boolean(e);
        अगर (r < 0)
                वापस r;

        log_show_time(r);
        वापस 0;
}

पूर्णांक log_show_tid_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        r = parse_boolean(e);
        अगर (r < 0)
                वापस r;

        log_show_tid(r);
        वापस 0;
}

पूर्णांक log_show_executable_from_string(स्थिर अक्षर *e) {
        पूर्णांक r;

        r = parse_boolean(e);
        अगर (r < 0)
                वापस r;

        log_show_executable(r);
        वापस 0;
}

bool log_on_console(व्योम) {
        अगर (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                वापस true;

        वापस syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

अटल स्थिर अक्षर *स्थिर log_target_table[_LOG_TARGET_MAX] = {
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

व्योम log_received_signal(पूर्णांक level, स्थिर काष्ठा signalfd_siginfo *si) {
        निश्चित(si);

        अगर (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ अक्षर *p = शून्य;

                (व्योम) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } अन्यथा
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

व्योम set_log_syntax_callback(log_syntax_callback_t cb, व्योम *userdata) {
        निश्चित(!log_syntax_callback || !cb);
        निश्चित(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

पूर्णांक log_syntax_internal(
                स्थिर अक्षर *unit,
                पूर्णांक level,
                स्थिर अक्षर *config_file,
                अचिन्हित config_पंक्ति,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format, ...) {

        PROTECT_ERRNO;

        अगर (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        अगर (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                वापस -ERRNO_मूल्य(error);

        अक्षर बफर[LINE_MAX];
        बहु_सूची ap;
        स्थिर अक्षर *unit_fmt = शून्य;

        त्रुटि_सं = ERRNO_मूल्य(error);

        बहु_शुरू(ap, format);
        (व्योम) vsnprintf(बफर, माप बफर, format, ap);
        बहु_पूर्ण(ap);

        अगर (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        अगर (config_file) {
                अगर (config_पंक्ति > 0)
                        वापस log_struct_internal(
                                        level,
                                        error,
                                        file, पंक्ति, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_पंक्ति),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_पंक्ति, बफर),
                                        unit_fmt, unit,
                                        शून्य);
                अन्यथा
                        वापस log_struct_internal(
                                        level,
                                        error,
                                        file, पंक्ति, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, बफर),
                                        unit_fmt, unit,
                                        शून्य);
        } अन्यथा अगर (unit)
                वापस log_struct_internal(
                                level,
                                error,
                                file, पंक्ति, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, बफर),
                                unit_fmt, unit,
                                शून्य);
        अन्यथा
                वापस log_struct_internal(
                                level,
                                error,
                                file, पंक्ति, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", बफर),
                                शून्य);
}

पूर्णांक log_syntax_invalid_utf8_internal(
                स्थिर अक्षर *unit,
                पूर्णांक level,
                स्थिर अक्षर *config_file,
                अचिन्हित config_पंक्ति,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ अक्षर *p = शून्य;

        अगर (rvalue)
                p = utf8_escape_invalid(rvalue);

        वापस log_syntax_internal(unit, level, config_file, config_पंक्ति,
                                   SYNTHETIC_ERRNO(EINVAL), file, पंक्ति, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

पूर्णांक log_syntax_parse_error_internal(
                स्थिर अक्षर *unit,
                स्थिर अक्षर *config_file,
                अचिन्हित config_पंक्ति,
                पूर्णांक error,
                bool critical,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *lvalue,
                स्थिर अक्षर *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ अक्षर *escaped = शून्य;

        /* OOM is always handled as critical. */
        अगर (ERRNO_मूल्य(error) == ENOMEM)
                वापस log_oom_internal(LOG_ERR, file, पंक्ति, func);

        अगर (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                अगर (!escaped)
                        rvalue = "(oom)";
                अन्यथा
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_पंक्ति, error,
                            file, पंक्ति, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        वापस critical ? -ERRNO_मूल्य(error) : 0;
}

व्योम log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        अगर (b) {
                अगर (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                अन्यथा अगर (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

व्योम log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

व्योम log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

व्योम log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

पूर्णांक log_emergency_level(व्योम) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        वापस getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

पूर्णांक log_dup_console(व्योम) {
        पूर्णांक copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        अगर (console_fd < 0 || console_fd >= 3)
                वापस 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        अगर (copy < 0)
                वापस -त्रुटि_सं;

        console_fd = copy;
        वापस 0;
}

व्योम log_setup(व्योम) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (व्योम) log_open();
        अगर (log_on_console() && show_color < 0)
                log_show_color(true);
}

स्थिर अक्षर* _log_set_prefix(स्थिर अक्षर *prefix, bool force) {
        स्थिर अक्षर *old = log_prefix;

        अगर (prefix || force)
                log_prefix = prefix;

        वापस old;
}
