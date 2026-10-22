/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <sys/signalfd.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <sys/uio.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "sd-messages.h"

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "ansi-color.h"
#അന്തര്ഭാവ "argv-util.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "list.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "log-context.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "proc-cmdline.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "ratelimit.h"
#അന്തര്ഭാവ "signal-util.h"
#അന്തര്ഭാവ "socket-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "syslog-util.h"
#അന്തര്ഭാവ "terminal-util.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

സ്ഥിതം log_syntax_callback_t log_syntax_callback = ശൂന്യ;
സ്ഥിതം ശൂന്യം *log_syntax_callback_userdata = ശൂന്യ;

സ്ഥിതം LogTarget log_target = LOG_TARGET_CONSOLE;
സ്ഥിതം പൂർണ്ണസംഖ്യ log_max_level = LOG_INFO;
സ്ഥിതം പൂർണ്ണസംഖ്യ log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
സ്ഥിതം പൂർണ്ണസംഖ്യ log_facility = LOG_DAEMON;
സ്ഥിതം bool ratelimit_kmsg = true;

സ്ഥിതം പൂർണ്ണസംഖ്യ console_fd = STDERR_FILENO;
സ്ഥിതം പൂർണ്ണസംഖ്യ console_fd_is_tty = -1; /* tri-state: -1 means don't know */
സ്ഥിതം പൂർണ്ണസംഖ്യ syslog_fd = -EBADF;
സ്ഥിതം പൂർണ്ണസംഖ്യ kmsg_fd = -EBADF;
സ്ഥിതം പൂർണ്ണസംഖ്യ journal_fd = -EBADF;

സ്ഥിതം bool syslog_is_stream = false;

സ്ഥിതം പൂർണ്ണസംഖ്യ show_color = -1; /* tristate */
സ്ഥിതം bool show_location = false;
സ്ഥിതം bool show_time = false;
സ്ഥിതം bool show_tid = false;
സ്ഥിതം bool show_executable = false;

സ്ഥിതം bool upgrade_syslog_to_journal = false;
സ്ഥിതം bool always_reopen_console = false;
സ്ഥിതം bool open_when_needed = false;
സ്ഥിതം bool prohibit_ipc = false;

സ്ഥിതം thread_local സ്ഥിരം അക്ഷരം *log_prefix = ശൂന്യ;

#എങ്കിൽ LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(വ്യഞ്ജകം)                                                \
        ചെയ്യുക {                                                            \
                എങ്കിൽ (_unlikely_(!(വ്യഞ്ജകം))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ആകുമ്പോൾ (false)

ശൂന്യം log_prefix_swap(സ്ഥിരം അക്ഷരം **prefix) {
        പ്രതിജ്ഞാ(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

സ്ഥിതം ശൂന്യം log_close_console(ശൂന്യം) {
        /* See comment in log_close_journal() */
        (ശൂന്യം) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_open_console(ശൂന്യം) {

        എങ്കിൽ (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                തിരികെ 0;
        }

        എങ്കിൽ (console_fd < 3) {
                പൂർണ്ണസംഖ്യ fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                എങ്കിൽ (fd < 0)
                        തിരികെ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        തിരികെ 0;
}

സ്ഥിതം ശൂന്യം log_close_kmsg(ശൂന്യം) {
        /* See comment in log_close_journal() */
        (ശൂന്യം) safe_close(TAKE_FD(kmsg_fd));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_open_kmsg(ശൂന്യം) {

        എങ്കിൽ (kmsg_fd >= 0)
                തിരികെ 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        എങ്കിൽ (kmsg_fd < 0)
                തിരികെ -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        തിരികെ 0;
}

സ്ഥിതം ശൂന്യം log_close_syslog(ശൂന്യം) {
        /* See comment in log_close_journal() */
        (ശൂന്യം) safe_close(TAKE_FD(syslog_fd));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ create_log_socket(പൂർണ്ണസംഖ്യ type) {
        ഘടന timeval tv;
        പൂർണ്ണസംഖ്യ fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        fd = fd_move_above_stdio(fd);
        (ശൂന്യം) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        എങ്കിൽ (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        അല്ലെങ്കിൽ
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (ശൂന്യം) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, വലുപ്പം(tv));

        തിരികെ fd;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_open_syslog(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (syslog_fd >= 0)
                തിരികെ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        എങ്കിൽ (syslog_fd < 0) {
                r = syslog_fd;
                പോകുക fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        എങ്കിൽ (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                എങ്കിൽ (syslog_fd < 0) {
                        r = syslog_fd;
                        പോകുക fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                എങ്കിൽ (r < 0)
                        പോകുക fail;

                syslog_is_stream = true;
        } അല്ലെങ്കിൽ
                syslog_is_stream = false;

        തിരികെ 0;

fail:
        log_close_syslog();
        തിരികെ r;
}

സ്ഥിതം ശൂന്യം log_close_journal(ശൂന്യം) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (ശൂന്യം) safe_close(TAKE_FD(journal_fd));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_open_journal(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (journal_fd >= 0)
                തിരികെ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        എങ്കിൽ (journal_fd < 0) {
                r = journal_fd;
                പോകുക fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        എങ്കിൽ (r < 0)
                പോകുക fail;

        തിരികെ 0;

fail:
        log_close_journal();
        തിരികെ r;
}

bool stderr_is_journal(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *w = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *e;
        uint64_t dev, ino;
        ഘടന stat st;

        e = getenv("JOURNAL_STREAM");
        എങ്കിൽ (!e)
                തിരികെ false;

        എങ്കിൽ (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                തിരികെ false;
        എങ്കിൽ (!e)
                തിരികെ false;

        എങ്കിൽ (safe_atou64(w, &dev) < 0)
                തിരികെ false;
        എങ്കിൽ (safe_atou64(e, &ino) < 0)
                തിരികെ false;

        എങ്കിൽ (fstat(STDERR_FILENO, &st) < 0)
                തിരികെ false;

        തിരികെ st.st_dev == dev && st.st_ino == ino;
}

പൂർണ്ണസംഖ്യ log_open(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        എങ്കിൽ (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                തിരികെ 0;
        }

        എങ്കിൽ (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                എങ്കിൽ (!prohibit_ipc) {
                        എങ്കിൽ (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                എങ്കിൽ (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        തിരികെ r;
                                }
                        }

                        എങ്കിൽ (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                എങ്കിൽ (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        തിരികെ r;
                                }
                        }
                }

                എങ്കിൽ (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        എങ്കിൽ (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                തിരികെ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        തിരികെ log_open_console();
}

ശൂന്യം log_set_target(LogTarget target) {
        പ്രതിജ്ഞാ(target >= 0);
        പ്രതിജ്ഞാ(target < _LOG_TARGET_MAX);

        എങ്കിൽ (upgrade_syslog_to_journal) {
                എങ്കിൽ (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                അല്ലെങ്കിൽ എങ്കിൽ (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

ശൂന്യം log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

ശൂന്യം log_close(ശൂന്യം) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

ശൂന്യം log_forget_fds(ശൂന്യം) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

പൂർണ്ണസംഖ്യ log_set_max_level(പൂർണ്ണസംഖ്യ level) {
        പ്രതിജ്ഞാ(level == LOG_NULL || log_level_is_valid(level));

        പൂർണ്ണസംഖ്യ old = log_max_level;
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

        തിരികെ old;
}

ശൂന്യം log_set_facility(പൂർണ്ണസംഖ്യ facility) {
        log_facility = facility;
}

സ്ഥിതം bool check_console_fd_is_tty(ശൂന്യം) {
        എങ്കിൽ (console_fd < 0)
                തിരികെ false;

        എങ്കിൽ (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        തിരികെ console_fd_is_tty;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_to_console(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *buffer) {

        സ്ഥിതം പൂർണ്ണസംഖ്യ dumb = -1;

        അക്ഷരം location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ഘടന iovec iovec[13];
        സ്ഥിരം അക്ഷരം *on = ശൂന്യ, *off = ശൂന്യ;
        size_t n = 0;

        എങ്കിൽ (console_fd < 0)
                തിരികെ 0;

        എങ്കിൽ (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        എങ്കിൽ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                തിരികെ 0;

        എങ്കിൽ (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        എങ്കിൽ (show_time &&
            format_timestamp(header_time, വലുപ്പം(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        എങ്കിൽ (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        എങ്കിൽ (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        എങ്കിൽ (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ശൂന്യ);

        എങ്കിൽ (show_location) {
                സ്ഥിരം അക്ഷരം *lon = "", *loff = "";
                എങ്കിൽ (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (ശൂന്യം) snprintf(location, വലുപ്പം location, "%s%s:%i%s: ", lon, file, വരി, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        എങ്കിൽ (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        എങ്കിൽ (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        എങ്കിൽ (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        എങ്കിൽ (writev(console_fd, iovec, n) < 0) {

                എങ്കിൽ (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (ശൂന്യം) log_open_console();
                        എങ്കിൽ (console_fd < 0)
                                തിരികെ 0;

                        എങ്കിൽ (writev(console_fd, iovec, n) < 0)
                                തിരികെ -errno;
                } അല്ലെങ്കിൽ
                        തിരികെ -errno;
        }

        തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_to_syslog(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *buffer) {

        അക്ഷരം header_priority[2 + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ഘടന tm tm;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (syslog_fd < 0)
                തിരികെ 0;

        എങ്കിൽ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                തിരികെ 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (strftime(header_time, വലുപ്പം(header_time), "%h %e %T ", &tm) <= 0)
                തിരികെ -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ഘടന iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        സ്ഥിരം ഘടന msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        എങ്കിൽ (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        വേണ്ടി (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;

                എങ്കിൽ (!syslog_is_stream)
                        നിർത്തുക;

                എങ്കിൽ (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        നിർത്തുക;
        }

        തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_to_kmsg(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        സ്ഥിതം thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        അക്ഷരം header_priority[2 + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        എങ്കിൽ (kmsg_fd < 0)
                തിരികെ 0;

        എങ്കിൽ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                തിരികെ 0;

        എങ്കിൽ (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                എങ്കിൽ (ratelimit_അക്കം_dropped(&ratelimit) > 1)
                        തിരികെ 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        സ്ഥിരം ഘടന iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        എങ്കിൽ (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                തിരികെ -errno;

        തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_do_header(
                അക്ഷരം *header,
                size_t size,
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field, സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field, സ്ഥിരം അക്ഷരം *extra) {
        പൂർണ്ണസംഖ്യ r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_മൂല്യം(error);

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
                     വരി ? "CODE_LINE=" : "",
                     വരി ? 1 : 0, വരി, /* %.0d means no output too, special case for 0 */
                     വരി ? "\n" : "",
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

        തിരികെ 0;
}

സ്ഥിതം ശൂന്യം log_do_context(ഘടന iovec *iovec, size_t iovec_len, size_t *n) {
        പ്രതിജ്ഞാ(iovec);
        പ്രതിജ്ഞാ(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        എങ്കിൽ (*n + 2 >= iovec_len)
                                തിരികെ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                വേണ്ടി (size_t i = 0; i < c->n_നിവേശം_iovec; i++) {
                        എങ്കിൽ (*n + 2 >= iovec_len)
                                തിരികെ;

                        iovec[(*n)++] = c->നിവേശം_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                എങ്കിൽ (c->താക്കോൽ && c->മൂല്യം) {
                        എങ്കിൽ (*n + 3 >= iovec_len)
                                തിരികെ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->താക്കോൽ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->മൂല്യം);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_to_journal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                സ്ഥിരം അക്ഷരം *buffer) {

        അക്ഷരം header[LINE_MAX];
        size_t n = 0, iovec_len;
        ഘടന iovec *iovec;

        എങ്കിൽ (journal_fd < 0)
                തിരികെ 0;

        എങ്കിൽ (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                തിരികെ 0;

        എങ്കിൽ (!MUL_SAFE(&iovec_len, log_context_അക്കം_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        അല്ലെങ്കിൽ
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ഘടന iovec, iovec_len);

        log_do_header(header, വലുപ്പം(header), level, error, file, വരി, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        എങ്കിൽ (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        സ്ഥിരം ഘടന msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        എങ്കിൽ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                തിരികെ -errno;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ log_dispatch_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                അക്ഷരം *buffer) {

        assert_raw(buffer);

        എങ്കിൽ (log_target == LOG_TARGET_NULL)
                തിരികെ -ERRNO_മൂല്യം(error);

        /* Patch in LOG_DAEMON facility if necessary */
        എങ്കിൽ (LOG_FAC(level) == 0)
                level |= log_facility;

        എങ്കിൽ (open_when_needed)
                (ശൂന്യം) log_open();

        ചെയ്യുക {
                അക്ഷരം *e;
                പൂർണ്ണസംഖ്യ k = 0;

                buffer += strspn(buffer, NEWLINE);

                എങ്കിൽ (buffer[0] == 0)
                        നിർത്തുക;

                എങ്കിൽ ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                എങ്കിൽ (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, വരി, func, object_field, object, extra_field, extra, buffer);
                        എങ്കിൽ (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                എങ്കിൽ (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, വരി, func, buffer);
                        എങ്കിൽ (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                എങ്കിൽ (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        എങ്കിൽ (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, വരി, func, buffer);
                        എങ്കിൽ (k < 0) {
                                log_close_kmsg();
                                (ശൂന്യം) log_open_console();
                        }
                }

                എങ്കിൽ (k <= 0)
                        (ശൂന്യം) write_to_console(level, error, file, വരി, func, buffer);

                buffer = e;
        } ആകുമ്പോൾ (buffer);

        എങ്കിൽ (open_when_needed)
                log_close();

        തിരികെ -ERRNO_മൂല്യം(error);
}

പൂർണ്ണസംഖ്യ log_dump_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                അക്ഷരം *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_max_level))
                തിരികെ -ERRNO_മൂല്യം(error);

        തിരികെ log_dispatch_internal(level, error, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, buffer);
}

പൂർണ്ണസംഖ്യ log_internalv(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format,
                va_list ap) {

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_max_level))
                തിരികെ -ERRNO_മൂല്യം(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        അക്ഷരം buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_മൂല്യം(error));

        (ശൂന്യം) vsnprintf(buffer, വലുപ്പം buffer, format, ap);

        തിരികെ log_dispatch_internal(level, error, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, buffer);
}

പൂർണ്ണസംഖ്യ log_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format, ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = log_internalv(level, error, file, വരി, func, format, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ log_object_internalv(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                സ്ഥിരം അക്ഷരം *format,
                va_list ap) {

        അക്ഷരം *buffer, *b;

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_max_level))
                തിരികെ -ERRNO_മൂല്യം(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_മൂല്യം(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(അക്ഷരം, LINE_MAX);
        (ശൂന്യം) vsnprintf(b, LINE_MAX, format, ap);

        തിരികെ log_dispatch_internal(level, error, file, വരി, func,
                                     object_field, object, extra_field, extra, buffer);
}

പൂർണ്ണസംഖ്യ log_object_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                സ്ഥിരം അക്ഷരം *format, ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, വരി, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ log_oom_internal(പൂർണ്ണസംഖ്യ level, സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *func) {
        തിരികെ log_internal(level, ENOMEM, file, വരി, func, "Out of memory.");
}

പൂർണ്ണസംഖ്യ log_format_iovec(
                ഘടന iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *format,
                va_list ap) {

        പ്രതിജ്ഞാ(iovec);
        പ്രതിജ്ഞാ(n);

        ആകുമ്പോൾ (format && *n + 1 < iovec_len) {
                va_list aq;
                അക്ഷരം *m;
                പൂർണ്ണസംഖ്യ r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_മൂല്യം(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                എങ്കിൽ (r < 0)
                        തിരികെ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                എങ്കിൽ (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, അക്ഷരം *);
        }
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_struct_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format, ...) {

        അക്ഷരം buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                തിരികെ -ERRNO_മൂല്യം(error);

        എങ്കിൽ (LOG_FAC(level) == 0)
                level |= log_facility;

        എങ്കിൽ (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                എങ്കിൽ (open_when_needed)
                        log_open_journal();

                എങ്കിൽ (journal_fd >= 0) {
                        അക്ഷരം header[LINE_MAX];
                        ഘടന iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        പൂർണ്ണസംഖ്യ r;
                        bool fallback = false;

                        എങ്കിൽ (!MUL_SAFE(&iovec_len, log_context_അക്കം_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        അല്ലെങ്കിൽ
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ഘടന iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, വലുപ്പം(header), level, error, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        എങ്കിൽ (r < 0)
                                fallback = true;
                        അല്ലെങ്കിൽ {
                                log_do_context(iovec, iovec_len, &n);

                                സ്ഥിരം ഘടന msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (ശൂന്യം) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        വേണ്ടി (size_t i = 1; i < m; i += 2)
                                സ്വതന്ത്രം(iovec[i].iov_base);

                        എങ്കിൽ (!fallback) {
                                എങ്കിൽ (open_when_needed)
                                        log_close();

                                തിരികെ -ERRNO_മൂല്യം(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ആകുമ്പോൾ (format) {
                va_list aq;

                errno = ERRNO_മൂല്യം(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (ശൂന്യം) vsnprintf(buf, വലുപ്പം buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                എങ്കിൽ (startswith(buf, "MESSAGE=")) {
                        found = true;
                        നിർത്തുക;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, അക്ഷരം *);
        }
        va_end(ap);

        എങ്കിൽ (!found) {
                എങ്കിൽ (open_when_needed)
                        log_close();

                തിരികെ -ERRNO_മൂല്യം(error);
        }

        തിരികെ log_dispatch_internal(level, error, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, buf + 8);
}

പൂർണ്ണസംഖ്യ log_struct_iovec_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം ഘടന iovec നിവേശം_iovec[],
                size_t n_നിവേശം_iovec) {

        PROTECT_ERRNO;

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                തിരികെ -ERRNO_മൂല്യം(error);

        എങ്കിൽ (LOG_FAC(level) == 0)
                level |= log_facility;

        എങ്കിൽ (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                അക്ഷരം header[LINE_MAX];
                ഘടന iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_നിവേശം_iovec_expanded;

                എങ്കിൽ (!MUL_SAFE(&n_നിവേശം_iovec_expanded, n_നിവേശം_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_അക്കം_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_നിവേശം_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                അല്ലെങ്കിൽ
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ഘടന iovec, iovec_len);

                log_do_header(header, വലുപ്പം(header), level, error, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                വേണ്ടി (size_t i = 0; i < n_നിവേശം_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = നിവേശം_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                സ്ഥിരം ഘടന msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                എങ്കിൽ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        തിരികെ -ERRNO_മൂല്യം(error);
        }

        വേണ്ടി (size_t i = 0; i < n_നിവേശം_iovec; i++)
                എങ്കിൽ (memory_startswith(നിവേശം_iovec[i].iov_base, നിവേശം_iovec[i].iov_len, "MESSAGE=")) {
                        അക്ഷരം *m;

                        m = strndupa_safe((അക്ഷരം*) നിവേശം_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          നിവേശം_iovec[i].iov_len - STRLEN("MESSAGE="));

                        തിരികെ log_dispatch_internal(level, error, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, m);
                }

        /* Couldn't find MESSAGE=. */
        തിരികെ -ERRNO_മൂല്യം(error);
}

പൂർണ്ണസംഖ്യ log_set_target_from_string(സ്ഥിരം അക്ഷരം *e) {
        LogTarget t;

        t = log_target_from_string(e);
        എങ്കിൽ (t < 0)
                തിരികെ t;

        log_set_target(t);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_set_max_level_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *word = ശൂന്യ, *prefix = ശൂന്യ;
                LogTarget target;
                സ്ഥിരം അക്ഷരം *colon;

                r = extract_first_word(&e, &word, ",", 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                colon = strchr(word, ':');
                എങ്കിൽ (!colon) {
                        r = log_level_from_string(word);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        log_set_max_level(r);
                        തുടരുക;
                }

                prefix = strndup(word, colon - word);
                എങ്കിൽ (!prefix)
                        തിരികെ -ENOMEM;

                target = log_target_from_string(prefix);
                എങ്കിൽ (target < 0)
                        തിരികെ target;

                എങ്കിൽ (target >= _LOG_TARGET_SINGLE_MAX)
                        തിരികെ -EINVAL;

                r = log_level_from_string(colon + 1);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                log_target_max_level[target] = r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_max_levels_to_string(പൂർണ്ണസംഖ്യ level, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = log_level_to_string_alloc(level, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ അക്ഷരം *l = ശൂന്യ;

                എങ്കിൽ (log_target_max_level[target] == INT_MAX)
                        തുടരുക;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_set_ratelimit_kmsg_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        r = parse_boolean(e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        ratelimit_kmsg = r;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_proc_cmdline_item(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം, ശൂന്യം *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        എങ്കിൽ (streq(താക്കോൽ, "debug") && !മൂല്യം)
                log_set_max_level(LOG_DEBUG);

        അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_target")) {

                എങ്കിൽ (proc_cmdline_മൂല്യം_missing(താക്കോൽ, മൂല്യം))
                        തിരികെ 0;

                എങ്കിൽ (log_set_target_from_string(മൂല്യം) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", മൂല്യം);

        } അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_level")) {

                എങ്കിൽ (proc_cmdline_മൂല്യം_missing(താക്കോൽ, മൂല്യം))
                        തിരികെ 0;

                എങ്കിൽ (log_set_max_level_from_string(മൂല്യം) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", മൂല്യം);

        } അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_color")) {

                എങ്കിൽ (log_show_color_from_string(മൂല്യം ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", മൂല്യം);

        } അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_location")) {

                എങ്കിൽ (log_show_location_from_string(മൂല്യം ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", മൂല്യം);

        } അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_tid")) {

                എങ്കിൽ (log_show_tid_from_string(മൂല്യം ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", മൂല്യം);

        } അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_time")) {

                എങ്കിൽ (log_show_time_from_string(മൂല്യം ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", മൂല്യം);

        } അല്ലെങ്കിൽ എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(താക്കോൽ, "systemd.log_ratelimit_kmsg")) {

                എങ്കിൽ (log_set_ratelimit_kmsg_from_string(മൂല്യം ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", മൂല്യം);
        }

        തിരികെ 0;
}

സ്ഥിതം bool should_parse_proc_cmdline(ശൂന്യം) {
        /* PID1 always reads the kernel command line. */
        എങ്കിൽ (getpid_cached() == 1)
                തിരികെ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        തിരികെ invoked_by_systemd();
}

ശൂന്യം log_parse_environment_variables(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ r;

        e = getenv("SYSTEMD_LOG_TARGET");
        എങ്കിൽ (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        എങ്കിൽ (e) {
                r = log_set_max_level_from_string(e);
                എങ്കിൽ (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } അല്ലെങ്കിൽ {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                എങ്കിൽ (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                അല്ലെങ്കിൽ എങ്കിൽ (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        എങ്കിൽ (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        എങ്കിൽ (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        എങ്കിൽ (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        എങ്കിൽ (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        എങ്കിൽ (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        എങ്കിൽ (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

ശൂന്യം log_parse_environment(ശൂന്യം) {
        /* Do not call from library code. */

        എങ്കിൽ (should_parse_proc_cmdline())
                (ശൂന്യം) proc_cmdline_parse(parse_proc_cmdline_item, ശൂന്യ, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(ശൂന്യം) {
        തിരികെ log_target;
}

ശൂന്യം log_settle_target(ശൂന്യം) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        എങ്കിൽ (t != LOG_TARGET_AUTO)
                തിരികെ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

പൂർണ്ണസംഖ്യ log_get_max_level(ശൂന്യം) {
        തിരികെ log_max_level;
}

പൂർണ്ണസംഖ്യ log_get_target_max_level(LogTarget target) {
        പ്രതിജ്ഞാ(target >= 0);
        പ്രതിജ്ഞാ(target < _LOG_TARGET_SINGLE_MAX);
        തിരികെ log_target_max_level[target];
}

ശൂന്യം log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(ശൂന്യം) {
        തിരികെ show_color > 0; /* Defaults to false. */
}

ശൂന്യം log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(ശൂന്യം) {
        തിരികെ show_location;
}

ശൂന്യം log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(ശൂന്യം) {
        തിരികെ show_time;
}

ശൂന്യം log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(ശൂന്യം) {
        തിരികെ show_tid;
}

ശൂന്യം log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(ശൂന്യം) {
        തിരികെ show_executable;
}

പൂർണ്ണസംഖ്യ log_show_color_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        r = parse_boolean(e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        log_show_color(r);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_show_location_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        r = parse_boolean(e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        log_show_location(r);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_show_time_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        r = parse_boolean(e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        log_show_time(r);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_show_tid_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        r = parse_boolean(e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        log_show_tid(r);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ log_show_executable_from_string(സ്ഥിരം അക്ഷരം *e) {
        പൂർണ്ണസംഖ്യ r;

        r = parse_boolean(e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        log_show_executable(r);
        തിരികെ 0;
}

bool log_on_console(ശൂന്യം) {
        എങ്കിൽ (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                തിരികെ true;

        തിരികെ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം log_target_table[_LOG_TARGET_MAX] = {
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

ശൂന്യം log_received_signal(പൂർണ്ണസംഖ്യ level, സ്ഥിരം ഘടന signalfd_siginfo *si) {
        പ്രതിജ്ഞാ(si);

        എങ്കിൽ (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                (ശൂന്യം) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } അല്ലെങ്കിൽ
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

ശൂന്യം set_log_syntax_callback(log_syntax_callback_t cb, ശൂന്യം *userdata) {
        പ്രതിജ്ഞാ(!log_syntax_callback || !cb);
        പ്രതിജ്ഞാ(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

പൂർണ്ണസംഖ്യ log_syntax_internal(
                സ്ഥിരം അക്ഷരം *unit,
                പൂർണ്ണസംഖ്യ level,
                സ്ഥിരം അക്ഷരം *config_file,
                unsigned config_വരി,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format, ...) {

        PROTECT_ERRNO;

        എങ്കിൽ (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                തിരികെ -ERRNO_മൂല്യം(error);

        അക്ഷരം buffer[LINE_MAX];
        va_list ap;
        സ്ഥിരം അക്ഷരം *unit_fmt = ശൂന്യ;

        errno = ERRNO_മൂല്യം(error);

        va_start(ap, format);
        (ശൂന്യം) vsnprintf(buffer, വലുപ്പം buffer, format, ap);
        va_end(ap);

        എങ്കിൽ (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        എങ്കിൽ (config_file) {
                എങ്കിൽ (config_വരി > 0)
                        തിരികെ log_struct_internal(
                                        level,
                                        error,
                                        file, വരി, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_വരി),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_വരി, buffer),
                                        unit_fmt, unit,
                                        ശൂന്യ);
                അല്ലെങ്കിൽ
                        തിരികെ log_struct_internal(
                                        level,
                                        error,
                                        file, വരി, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        ശൂന്യ);
        } അല്ലെങ്കിൽ എങ്കിൽ (unit)
                തിരികെ log_struct_internal(
                                level,
                                error,
                                file, വരി, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                ശൂന്യ);
        അല്ലെങ്കിൽ
                തിരികെ log_struct_internal(
                                level,
                                error,
                                file, വരി, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                ശൂന്യ);
}

പൂർണ്ണസംഖ്യ log_syntax_invalid_utf8_internal(
                സ്ഥിരം അക്ഷരം *unit,
                പൂർണ്ണസംഖ്യ level,
                സ്ഥിരം അക്ഷരം *config_file,
                unsigned config_വരി,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

        എങ്കിൽ (rvalue)
                p = utf8_escape_invalid(rvalue);

        തിരികെ log_syntax_internal(unit, level, config_file, config_വരി,
                                   SYNTHETIC_ERRNO(EINVAL), file, വരി, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

പൂർണ്ണസംഖ്യ log_syntax_parse_error_internal(
                സ്ഥിരം അക്ഷരം *unit,
                സ്ഥിരം അക്ഷരം *config_file,
                unsigned config_വരി,
                പൂർണ്ണസംഖ്യ error,
                bool critical,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *lvalue,
                സ്ഥിരം അക്ഷരം *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ അക്ഷരം *escaped = ശൂന്യ;

        /* OOM is always handled as critical. */
        എങ്കിൽ (ERRNO_മൂല്യം(error) == ENOMEM)
                തിരികെ log_oom_internal(LOG_ERR, file, വരി, func);

        എങ്കിൽ (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                എങ്കിൽ (!escaped)
                        rvalue = "(oom)";
                അല്ലെങ്കിൽ
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_വരി, error,
                            file, വരി, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        തിരികെ critical ? -ERRNO_മൂല്യം(error) : 0;
}

ശൂന്യം log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        എങ്കിൽ (b) {
                എങ്കിൽ (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                അല്ലെങ്കിൽ എങ്കിൽ (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

ശൂന്യം log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

ശൂന്യം log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

ശൂന്യം log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

പൂർണ്ണസംഖ്യ log_emergency_level(ശൂന്യം) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        തിരികെ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

പൂർണ്ണസംഖ്യ log_dup_console(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        എങ്കിൽ (console_fd < 0 || console_fd >= 3)
                തിരികെ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        എങ്കിൽ (copy < 0)
                തിരികെ -errno;

        console_fd = copy;
        തിരികെ 0;
}

ശൂന്യം log_setup(ശൂന്യം) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (ശൂന്യം) log_open();
        എങ്കിൽ (log_on_console() && show_color < 0)
                log_show_color(true);
}

സ്ഥിരം അക്ഷരം* _log_set_prefix(സ്ഥിരം അക്ഷരം *prefix, bool force) {
        സ്ഥിരം അക്ഷരം *old = log_prefix;

        എങ്കിൽ (prefix || force)
                log_prefix = prefix;

        തിരികെ old;
}
