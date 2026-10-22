/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sys/signalfd.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <sys/uio.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "sd-messages.h"

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "ansi-color.h"
#અન્તર્ભાવ "argv-util.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "list.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "log-context.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "proc-cmdline.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "ratelimit.h"
#અન્તર્ભાવ "signal-util.h"
#અન્તર્ભાવ "socket-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "syslog-util.h"
#અન્તર્ભાવ "terminal-util.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

સ્થિર log_syntax_callback_t log_syntax_callback = ખાલી;
સ્થિર શૂન્ય *log_syntax_callback_userdata = ખાલી;

સ્થિર LogTarget log_target = LOG_TARGET_CONSOLE;
સ્થિર પૂર્ણાંક log_max_level = LOG_INFO;
સ્થિર પૂર્ણાંક log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
સ્થિર પૂર્ણાંક log_facility = LOG_DAEMON;
સ્થિર bool ratelimit_kmsg = true;

સ્થિર પૂર્ણાંક console_fd = STDERR_FILENO;
સ્થિર પૂર્ણાંક console_fd_is_tty = -1; /* tri-state: -1 means don't know */
સ્થિર પૂર્ણાંક syslog_fd = -EBADF;
સ્થિર પૂર્ણાંક kmsg_fd = -EBADF;
સ્થિર પૂર્ણાંક journal_fd = -EBADF;

સ્થિર bool syslog_is_stream = false;

સ્થિર પૂર્ણાંક show_color = -1; /* tristate */
સ્થિર bool show_location = false;
સ્થિર bool show_time = false;
સ્થિર bool show_tid = false;
સ્થિર bool show_executable = false;

સ્થિર bool upgrade_syslog_to_journal = false;
સ્થિર bool always_reopen_console = false;
સ્થિર bool open_when_needed = false;
સ્થિર bool prohibit_ipc = false;

સ્થિર thread_local અચલ અક્ષર *log_prefix = ખાલી;

#જો LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(પદાવલિ)                                                \
        કરો {                                                            \
                જો (_unlikely_(!(પદાવલિ))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } જ્યારે (false)

શૂન્ય log_prefix_swap(અચલ અક્ષર **prefix) {
        પ્રતિજ્ઞા(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

સ્થિર શૂન્ય log_close_console(શૂન્ય) {
        /* See comment in log_close_journal() */
        (શૂન્ય) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

સ્થિર પૂર્ણાંક log_open_console(શૂન્ય) {

        જો (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                પાછા 0;
        }

        જો (console_fd < 3) {
                પૂર્ણાંક fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                જો (fd < 0)
                        પાછા fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        પાછા 0;
}

સ્થિર શૂન્ય log_close_kmsg(શૂન્ય) {
        /* See comment in log_close_journal() */
        (શૂન્ય) safe_close(TAKE_FD(kmsg_fd));
}

સ્થિર પૂર્ણાંક log_open_kmsg(શૂન્ય) {

        જો (kmsg_fd >= 0)
                પાછા 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        જો (kmsg_fd < 0)
                પાછા -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        પાછા 0;
}

સ્થિર શૂન્ય log_close_syslog(શૂન્ય) {
        /* See comment in log_close_journal() */
        (શૂન્ય) safe_close(TAKE_FD(syslog_fd));
}

સ્થિર પૂર્ણાંક create_log_socket(પૂર્ણાંક type) {
        સંરચના timeval tv;
        પૂર્ણાંક fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        જો (fd < 0)
                પાછા -errno;

        fd = fd_move_above_stdio(fd);
        (શૂન્ય) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        જો (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        નહીંતર
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (શૂન્ય) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, કદ(tv));

        પાછા fd;
}

સ્થિર પૂર્ણાંક log_open_syslog(શૂન્ય) {
        પૂર્ણાંક r;

        જો (syslog_fd >= 0)
                પાછા 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        જો (syslog_fd < 0) {
                r = syslog_fd;
                જાઓ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        જો (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                જો (syslog_fd < 0) {
                        r = syslog_fd;
                        જાઓ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                જો (r < 0)
                        જાઓ fail;

                syslog_is_stream = true;
        } નહીંતર
                syslog_is_stream = false;

        પાછા 0;

fail:
        log_close_syslog();
        પાછા r;
}

સ્થિર શૂન્ય log_close_journal(શૂન્ય) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (શૂન્ય) safe_close(TAKE_FD(journal_fd));
}

સ્થિર પૂર્ણાંક log_open_journal(શૂન્ય) {
        પૂર્ણાંક r;

        જો (journal_fd >= 0)
                પાછા 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        જો (journal_fd < 0) {
                r = journal_fd;
                જાઓ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        જો (r < 0)
                જાઓ fail;

        પાછા 0;

fail:
        log_close_journal();
        પાછા r;
}

bool stderr_is_journal(શૂન્ય) {
        _cleanup_free_ અક્ષર *w = ખાલી;
        અચલ અક્ષર *e;
        uint64_t dev, ino;
        સંરચના stat st;

        e = getenv("JOURNAL_STREAM");
        જો (!e)
                પાછા false;

        જો (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                પાછા false;
        જો (!e)
                પાછા false;

        જો (safe_atou64(w, &dev) < 0)
                પાછા false;
        જો (safe_atou64(e, &ino) < 0)
                પાછા false;

        જો (fstat(STDERR_FILENO, &st) < 0)
                પાછા false;

        પાછા st.st_dev == dev && st.st_ino == ino;
}

પૂર્ણાંક log_open(શૂન્ય) {
        પૂર્ણાંક r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        જો (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                પાછા 0;
        }

        જો (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                જો (!prohibit_ipc) {
                        જો (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                જો (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        પાછા r;
                                }
                        }

                        જો (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                જો (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        પાછા r;
                                }
                        }
                }

                જો (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        જો (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                પાછા r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        પાછા log_open_console();
}

શૂન્ય log_set_target(LogTarget target) {
        પ્રતિજ્ઞા(target >= 0);
        પ્રતિજ્ઞા(target < _LOG_TARGET_MAX);

        જો (upgrade_syslog_to_journal) {
                જો (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                નહીંતર જો (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

શૂન્ય log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

શૂન્ય log_close(શૂન્ય) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

શૂન્ય log_forget_fds(શૂન્ય) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

પૂર્ણાંક log_set_max_level(પૂર્ણાંક level) {
        પ્રતિજ્ઞા(level == LOG_NULL || log_level_is_valid(level));

        પૂર્ણાંક old = log_max_level;
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

        પાછા old;
}

શૂન્ય log_set_facility(પૂર્ણાંક facility) {
        log_facility = facility;
}

સ્થિર bool check_console_fd_is_tty(શૂન્ય) {
        જો (console_fd < 0)
                પાછા false;

        જો (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        પાછા console_fd_is_tty;
}

સ્થિર પૂર્ણાંક write_to_console(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *buffer) {

        સ્થિર પૂર્ણાંક dumb = -1;

        અક્ષર location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(પૂર્ણાંક) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        સંરચના iovec iovec[13];
        અચલ અક્ષર *on = ખાલી, *off = ખાલી;
        size_t n = 0;

        જો (console_fd < 0)
                પાછા 0;

        જો (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        જો (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                પાછા 0;

        જો (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        જો (show_time &&
            format_timestamp(header_time, કદ(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        જો (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        જો (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        જો (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ખાલી);

        જો (show_location) {
                અચલ અક્ષર *lon = "", *loff = "";
                જો (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (શૂન્ય) snprintf(location, કદ location, "%s%s:%i%s: ", lon, file, લીટી, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        જો (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        જો (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        જો (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        જો (writev(console_fd, iovec, n) < 0) {

                જો (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (શૂન્ય) log_open_console();
                        જો (console_fd < 0)
                                પાછા 0;

                        જો (writev(console_fd, iovec, n) < 0)
                                પાછા -errno;
                } નહીંતર
                        પાછા -errno;
        }

        પાછા 1;
}

સ્થિર પૂર્ણાંક write_to_syslog(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *buffer) {

        અક્ષર header_priority[2 + DECIMAL_STR_MAX(પૂર્ણાંક) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        સંરચના tm tm;
        પૂર્ણાંક r;

        જો (syslog_fd < 0)
                પાછા 0;

        જો (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                પાછા 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        જો (r < 0)
                પાછા r;

        જો (strftime(header_time, કદ(header_time), "%h %e %T ", &tm) <= 0)
                પાછા -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        સંરચના iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        અચલ સંરચના msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        જો (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        માટે (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                જો (n < 0)
                        પાછા -errno;

                જો (!syslog_is_stream)
                        તોડો;

                જો (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        તોડો;
        }

        પાછા 1;
}

સ્થિર પૂર્ણાંક write_to_kmsg(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        સ્થિર thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        અક્ષર header_priority[2 + DECIMAL_STR_MAX(પૂર્ણાંક) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        જો (kmsg_fd < 0)
                પાછા 0;

        જો (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                પાછા 0;

        જો (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                જો (ratelimit_અંક_dropped(&ratelimit) > 1)
                        પાછા 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        અચલ સંરચના iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        જો (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                પાછા -errno;

        પાછા 1;
}

સ્થિર પૂર્ણાંક log_do_header(
                અક્ષર *header,
                size_t size,
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file, પૂર્ણાંક લીટી, અચલ અક્ષર *func,
                અચલ અક્ષર *object_field, અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field, અચલ અક્ષર *extra) {
        પૂર્ણાંક r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_કિંમત(error);

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
                     લીટી ? "CODE_LINE=" : "",
                     લીટી ? 1 : 0, લીટી, /* %.0d means no output too, special case for 0 */
                     લીટી ? "\n" : "",
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

        પાછા 0;
}

સ્થિર શૂન્ય log_do_context(સંરચના iovec *iovec, size_t iovec_len, size_t *n) {
        પ્રતિજ્ઞા(iovec);
        પ્રતિજ્ઞા(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        જો (*n + 2 >= iovec_len)
                                પાછા;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                માટે (size_t i = 0; i < c->n_આગત_iovec; i++) {
                        જો (*n + 2 >= iovec_len)
                                પાછા;

                        iovec[(*n)++] = c->આગત_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                જો (c->ચાવી && c->કિંમત) {
                        જો (*n + 3 >= iovec_len)
                                પાછા;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ચાવી);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->કિંમત);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

સ્થિર પૂર્ણાંક write_to_journal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અચલ અક્ષર *buffer) {

        અક્ષર header[LINE_MAX];
        size_t n = 0, iovec_len;
        સંરચના iovec *iovec;

        જો (journal_fd < 0)
                પાછા 0;

        જો (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                પાછા 0;

        જો (!MUL_SAFE(&iovec_len, log_context_અંક_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        નહીંતર
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(સંરચના iovec, iovec_len);

        log_do_header(header, કદ(header), level, error, file, લીટી, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        જો (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        અચલ સંરચના msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        જો (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                પાછા -errno;

        પાછા 1;
}

પૂર્ણાંક log_dispatch_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અક્ષર *buffer) {

        assert_raw(buffer);

        જો (log_target == LOG_TARGET_NULL)
                પાછા -ERRNO_કિંમત(error);

        /* Patch in LOG_DAEMON facility if necessary */
        જો (LOG_FAC(level) == 0)
                level |= log_facility;

        જો (open_when_needed)
                (શૂન્ય) log_open();

        કરો {
                અક્ષર *e;
                પૂર્ણાંક k = 0;

                buffer += strspn(buffer, NEWLINE);

                જો (buffer[0] == 0)
                        તોડો;

                જો ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                જો (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, લીટી, func, object_field, object, extra_field, extra, buffer);
                        જો (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                જો (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, લીટી, func, buffer);
                        જો (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                જો (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        જો (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, લીટી, func, buffer);
                        જો (k < 0) {
                                log_close_kmsg();
                                (શૂન્ય) log_open_console();
                        }
                }

                જો (k <= 0)
                        (શૂન્ય) write_to_console(level, error, file, લીટી, func, buffer);

                buffer = e;
        } જ્યારે (buffer);

        જો (open_when_needed)
                log_close();

        પાછા -ERRNO_કિંમત(error);
}

પૂર્ણાંક log_dump_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અક્ષર *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        જો (_likely_(LOG_PRI(level) > log_max_level))
                પાછા -ERRNO_કિંમત(error);

        પાછા log_dispatch_internal(level, error, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી, buffer);
}

પૂર્ણાંક log_internalv(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format,
                va_list ap) {

        જો (_likely_(LOG_PRI(level) > log_max_level))
                પાછા -ERRNO_કિંમત(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        અક્ષર buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_કિંમત(error));

        (શૂન્ય) vsnprintf(buffer, કદ buffer, format, ap);

        પાછા log_dispatch_internal(level, error, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી, buffer);
}

પૂર્ણાંક log_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format, ...) {

        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = log_internalv(level, error, file, લીટી, func, format, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક log_object_internalv(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અચલ અક્ષર *format,
                va_list ap) {

        અક્ષર *buffer, *b;

        જો (_likely_(LOG_PRI(level) > log_max_level))
                પાછા -ERRNO_કિંમત(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_કિંમત(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(અક્ષર, LINE_MAX);
        (શૂન્ય) vsnprintf(b, LINE_MAX, format, ap);

        પાછા log_dispatch_internal(level, error, file, લીટી, func,
                                     object_field, object, extra_field, extra, buffer);
}

પૂર્ણાંક log_object_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અચલ અક્ષર *format, ...) {

        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, લીટી, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક log_oom_internal(પૂર્ણાંક level, અચલ અક્ષર *file, પૂર્ણાંક લીટી, અચલ અક્ષર *func) {
        પાછા log_internal(level, ENOMEM, file, લીટી, func, "Out of memory.");
}

પૂર્ણાંક log_format_iovec(
                સંરચના iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                પૂર્ણાંક error,
                અચલ અક્ષર *format,
                va_list ap) {

        પ્રતિજ્ઞા(iovec);
        પ્રતિજ્ઞા(n);

        જ્યારે (format && *n + 1 < iovec_len) {
                va_list aq;
                અક્ષર *m;
                પૂર્ણાંક r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_કિંમત(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                જો (r < 0)
                        પાછા -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                જો (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, અક્ષર *);
        }
        પાછા 0;
}

પૂર્ણાંક log_struct_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format, ...) {

        અક્ષર buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        જો (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                પાછા -ERRNO_કિંમત(error);

        જો (LOG_FAC(level) == 0)
                level |= log_facility;

        જો (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                જો (open_when_needed)
                        log_open_journal();

                જો (journal_fd >= 0) {
                        અક્ષર header[LINE_MAX];
                        સંરચના iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        પૂર્ણાંક r;
                        bool fallback = false;

                        જો (!MUL_SAFE(&iovec_len, log_context_અંક_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        નહીંતર
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(સંરચના iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, કદ(header), level, error, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        જો (r < 0)
                                fallback = true;
                        નહીંતર {
                                log_do_context(iovec, iovec_len, &n);

                                અચલ સંરચના msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (શૂન્ય) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        માટે (size_t i = 1; i < m; i += 2)
                                મુક્ત(iovec[i].iov_base);

                        જો (!fallback) {
                                જો (open_when_needed)
                                        log_close();

                                પાછા -ERRNO_કિંમત(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        જ્યારે (format) {
                va_list aq;

                errno = ERRNO_કિંમત(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (શૂન્ય) vsnprintf(buf, કદ buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                જો (startswith(buf, "MESSAGE=")) {
                        found = true;
                        તોડો;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, અક્ષર *);
        }
        va_end(ap);

        જો (!found) {
                જો (open_when_needed)
                        log_close();

                પાછા -ERRNO_કિંમત(error);
        }

        પાછા log_dispatch_internal(level, error, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી, buf + 8);
}

પૂર્ણાંક log_struct_iovec_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ સંરચના iovec આગત_iovec[],
                size_t n_આગત_iovec) {

        PROTECT_ERRNO;

        જો (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                પાછા -ERRNO_કિંમત(error);

        જો (LOG_FAC(level) == 0)
                level |= log_facility;

        જો (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                અક્ષર header[LINE_MAX];
                સંરચના iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_આગત_iovec_expanded;

                જો (!MUL_SAFE(&n_આગત_iovec_expanded, n_આગત_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_અંક_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_આગત_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                નહીંતર
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(સંરચના iovec, iovec_len);

                log_do_header(header, કદ(header), level, error, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                માટે (size_t i = 0; i < n_આગત_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = આગત_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                અચલ સંરચના msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                જો (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        પાછા -ERRNO_કિંમત(error);
        }

        માટે (size_t i = 0; i < n_આગત_iovec; i++)
                જો (memory_startswith(આગત_iovec[i].iov_base, આગત_iovec[i].iov_len, "MESSAGE=")) {
                        અક્ષર *m;

                        m = strndupa_safe((અક્ષર*) આગત_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          આગત_iovec[i].iov_len - STRLEN("MESSAGE="));

                        પાછા log_dispatch_internal(level, error, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી, m);
                }

        /* Couldn't find MESSAGE=. */
        પાછા -ERRNO_કિંમત(error);
}

પૂર્ણાંક log_set_target_from_string(અચલ અક્ષર *e) {
        LogTarget t;

        t = log_target_from_string(e);
        જો (t < 0)
                પાછા t;

        log_set_target(t);
        પાછા 0;
}

પૂર્ણાંક log_set_max_level_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *word = ખાલી, *prefix = ખાલી;
                LogTarget target;
                અચલ અક્ષર *colon;

                r = extract_first_word(&e, &word, ",", 0);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                colon = strchr(word, ':');
                જો (!colon) {
                        r = log_level_from_string(word);
                        જો (r < 0)
                                પાછા r;

                        log_set_max_level(r);
                        ચાલુ;
                }

                prefix = strndup(word, colon - word);
                જો (!prefix)
                        પાછા -ENOMEM;

                target = log_target_from_string(prefix);
                જો (target < 0)
                        પાછા target;

                જો (target >= _LOG_TARGET_SINGLE_MAX)
                        પાછા -EINVAL;

                r = log_level_from_string(colon + 1);
                જો (r < 0)
                        પાછા r;

                log_target_max_level[target] = r;
        }

        પાછા 0;
}

પૂર્ણાંક log_max_levels_to_string(પૂર્ણાંક level, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = log_level_to_string_alloc(level, &s);
        જો (r < 0)
                પાછા r;

        માટે (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ અક્ષર *l = ખાલી;

                જો (log_target_max_level[target] == INT_MAX)
                        ચાલુ;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                જો (r < 0)
                        પાછા r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(s);
        પાછા 0;
}

સ્થિર પૂર્ણાંક log_set_ratelimit_kmsg_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        r = parse_boolean(e);
        જો (r < 0)
                પાછા r;

        ratelimit_kmsg = r;
        પાછા 0;
}

સ્થિર પૂર્ણાંક parse_proc_cmdline_item(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત, શૂન્ય *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        જો (streq(ચાવી, "debug") && !કિંમત)
                log_set_max_level(LOG_DEBUG);

        નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_target")) {

                જો (proc_cmdline_કિંમત_missing(ચાવી, કિંમત))
                        પાછા 0;

                જો (log_set_target_from_string(કિંમત) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", કિંમત);

        } નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_level")) {

                જો (proc_cmdline_કિંમત_missing(ચાવી, કિંમત))
                        પાછા 0;

                જો (log_set_max_level_from_string(કિંમત) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", કિંમત);

        } નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_color")) {

                જો (log_show_color_from_string(કિંમત ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", કિંમત);

        } નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_location")) {

                જો (log_show_location_from_string(કિંમત ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", કિંમત);

        } નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_tid")) {

                જો (log_show_tid_from_string(કિંમત ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", કિંમત);

        } નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_time")) {

                જો (log_show_time_from_string(કિંમત ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", કિંમત);

        } નહીંતર જો (proc_cmdline_ચાવી_streq(ચાવી, "systemd.log_ratelimit_kmsg")) {

                જો (log_set_ratelimit_kmsg_from_string(કિંમત ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", કિંમત);
        }

        પાછા 0;
}

સ્થિર bool should_parse_proc_cmdline(શૂન્ય) {
        /* PID1 always reads the kernel command line. */
        જો (getpid_cached() == 1)
                પાછા true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        પાછા invoked_by_systemd();
}

શૂન્ય log_parse_environment_variables(શૂન્ય) {
        અચલ અક્ષર *e;
        પૂર્ણાંક r;

        e = getenv("SYSTEMD_LOG_TARGET");
        જો (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        જો (e) {
                r = log_set_max_level_from_string(e);
                જો (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } નહીંતર {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                જો (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                નહીંતર જો (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        જો (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        જો (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        જો (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        જો (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        જો (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        જો (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

શૂન્ય log_parse_environment(શૂન્ય) {
        /* Do not call from library code. */

        જો (should_parse_proc_cmdline())
                (શૂન્ય) proc_cmdline_parse(parse_proc_cmdline_item, ખાલી, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(શૂન્ય) {
        પાછા log_target;
}

શૂન્ય log_settle_target(શૂન્ય) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        જો (t != LOG_TARGET_AUTO)
                પાછા;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

પૂર્ણાંક log_get_max_level(શૂન્ય) {
        પાછા log_max_level;
}

પૂર્ણાંક log_get_target_max_level(LogTarget target) {
        પ્રતિજ્ઞા(target >= 0);
        પ્રતિજ્ઞા(target < _LOG_TARGET_SINGLE_MAX);
        પાછા log_target_max_level[target];
}

શૂન્ય log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(શૂન્ય) {
        પાછા show_color > 0; /* Defaults to false. */
}

શૂન્ય log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(શૂન્ય) {
        પાછા show_location;
}

શૂન્ય log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(શૂન્ય) {
        પાછા show_time;
}

શૂન્ય log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(શૂન્ય) {
        પાછા show_tid;
}

શૂન્ય log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(શૂન્ય) {
        પાછા show_executable;
}

પૂર્ણાંક log_show_color_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        r = parse_boolean(e);
        જો (r < 0)
                પાછા r;

        log_show_color(r);
        પાછા 0;
}

પૂર્ણાંક log_show_location_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        r = parse_boolean(e);
        જો (r < 0)
                પાછા r;

        log_show_location(r);
        પાછા 0;
}

પૂર્ણાંક log_show_time_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        r = parse_boolean(e);
        જો (r < 0)
                પાછા r;

        log_show_time(r);
        પાછા 0;
}

પૂર્ણાંક log_show_tid_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        r = parse_boolean(e);
        જો (r < 0)
                પાછા r;

        log_show_tid(r);
        પાછા 0;
}

પૂર્ણાંક log_show_executable_from_string(અચલ અક્ષર *e) {
        પૂર્ણાંક r;

        r = parse_boolean(e);
        જો (r < 0)
                પાછા r;

        log_show_executable(r);
        પાછા 0;
}

bool log_on_console(શૂન્ય) {
        જો (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                પાછા true;

        પાછા syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

સ્થિર અચલ અક્ષર *અચલ log_target_table[_LOG_TARGET_MAX] = {
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

શૂન્ય log_received_signal(પૂર્ણાંક level, અચલ સંરચના signalfd_siginfo *si) {
        પ્રતિજ્ઞા(si);

        જો (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ અક્ષર *p = ખાલી;

                (શૂન્ય) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } નહીંતર
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

શૂન્ય set_log_syntax_callback(log_syntax_callback_t cb, શૂન્ય *userdata) {
        પ્રતિજ્ઞા(!log_syntax_callback || !cb);
        પ્રતિજ્ઞા(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

પૂર્ણાંક log_syntax_internal(
                અચલ અક્ષર *unit,
                પૂર્ણાંક level,
                અચલ અક્ષર *config_file,
                unsigned config_લીટી,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format, ...) {

        PROTECT_ERRNO;

        જો (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        જો (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                પાછા -ERRNO_કિંમત(error);

        અક્ષર buffer[LINE_MAX];
        va_list ap;
        અચલ અક્ષર *unit_fmt = ખાલી;

        errno = ERRNO_કિંમત(error);

        va_start(ap, format);
        (શૂન્ય) vsnprintf(buffer, કદ buffer, format, ap);
        va_end(ap);

        જો (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        જો (config_file) {
                જો (config_લીટી > 0)
                        પાછા log_struct_internal(
                                        level,
                                        error,
                                        file, લીટી, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_લીટી),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_લીટી, buffer),
                                        unit_fmt, unit,
                                        ખાલી);
                નહીંતર
                        પાછા log_struct_internal(
                                        level,
                                        error,
                                        file, લીટી, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        ખાલી);
        } નહીંતર જો (unit)
                પાછા log_struct_internal(
                                level,
                                error,
                                file, લીટી, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                ખાલી);
        નહીંતર
                પાછા log_struct_internal(
                                level,
                                error,
                                file, લીટી, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                ખાલી);
}

પૂર્ણાંક log_syntax_invalid_utf8_internal(
                અચલ અક્ષર *unit,
                પૂર્ણાંક level,
                અચલ અક્ષર *config_file,
                unsigned config_લીટી,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ અક્ષર *p = ખાલી;

        જો (rvalue)
                p = utf8_escape_invalid(rvalue);

        પાછા log_syntax_internal(unit, level, config_file, config_લીટી,
                                   SYNTHETIC_ERRNO(EINVAL), file, લીટી, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

પૂર્ણાંક log_syntax_parse_error_internal(
                અચલ અક્ષર *unit,
                અચલ અક્ષર *config_file,
                unsigned config_લીટી,
                પૂર્ણાંક error,
                bool critical,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *lvalue,
                અચલ અક્ષર *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ અક્ષર *escaped = ખાલી;

        /* OOM is always handled as critical. */
        જો (ERRNO_કિંમત(error) == ENOMEM)
                પાછા log_oom_internal(LOG_ERR, file, લીટી, func);

        જો (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                જો (!escaped)
                        rvalue = "(oom)";
                નહીંતર
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_લીટી, error,
                            file, લીટી, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        પાછા critical ? -ERRNO_કિંમત(error) : 0;
}

શૂન્ય log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        જો (b) {
                જો (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                નહીંતર જો (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

શૂન્ય log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

શૂન્ય log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

શૂન્ય log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

પૂર્ણાંક log_emergency_level(શૂન્ય) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        પાછા getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

પૂર્ણાંક log_dup_console(શૂન્ય) {
        પૂર્ણાંક copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        જો (console_fd < 0 || console_fd >= 3)
                પાછા 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        જો (copy < 0)
                પાછા -errno;

        console_fd = copy;
        પાછા 0;
}

શૂન્ય log_setup(શૂન્ય) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (શૂન્ય) log_open();
        જો (log_on_console() && show_color < 0)
                log_show_color(true);
}

અચલ અક્ષર* _log_set_prefix(અચલ અક્ષર *prefix, bool force) {
        અચલ અક્ષર *old = log_prefix;

        જો (prefix || force)
                log_prefix = prefix;

        પાછા old;
}
