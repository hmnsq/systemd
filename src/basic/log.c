/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sys/signalfd.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <sys/uio.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "sd-messages.h"

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "ansi-color.h"
#ଅନ୍ତର୍ଭାଵ "argv-util.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "list.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "log-context.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "proc-cmdline.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "ratelimit.h"
#ଅନ୍ତର୍ଭାଵ "signal-util.h"
#ଅନ୍ତର୍ଭାଵ "socket-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "syslog-util.h"
#ଅନ୍ତର୍ଭାଵ "terminal-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

#define SNDBUF_SIZE (8*1024*1024)
#define IOVEC_MAX 256U

ସ୍ଥିର log_syntax_callback_t log_syntax_callback = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ *log_syntax_callback_userdata = ଖାଲି;

ସ୍ଥିର LogTarget log_target = LOG_TARGET_CONSOLE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_max_level = LOG_INFO;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_target_max_level[_LOG_TARGET_SINGLE_MAX] = {
        [LOG_TARGET_CONSOLE] = INT_MAX,
        [LOG_TARGET_KMSG]    = INT_MAX,
        [LOG_TARGET_SYSLOG]  = INT_MAX,
        [LOG_TARGET_JOURNAL] = INT_MAX,
};
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_facility = LOG_DAEMON;
ସ୍ଥିର bool ratelimit_kmsg = true;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା console_fd = STDERR_FILENO;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା console_fd_is_tty = -1; /* tri-state: -1 means don't know */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା syslog_fd = -EBADF;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା kmsg_fd = -EBADF;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା journal_fd = -EBADF;

ସ୍ଥିର bool syslog_is_stream = false;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_color = -1; /* tristate */
ସ୍ଥିର bool show_location = false;
ସ୍ଥିର bool show_time = false;
ସ୍ଥିର bool show_tid = false;
ସ୍ଥିର bool show_executable = false;

ସ୍ଥିର bool upgrade_syslog_to_journal = false;
ସ୍ଥିର bool always_reopen_console = false;
ସ୍ଥିର bool open_when_needed = false;
ସ୍ଥିର bool prohibit_ipc = false;

ସ୍ଥିର thread_local ଅଚଳ ଅକ୍ଷର *log_prefix = ଖାଲି;

#ଯଦି LOG_MESSAGE_VERIFICATION || defined(__COVERITY__)
bool _log_message_dummy = false; /* Always false */
#endif

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#define assert_raw(ପଦସମଷ୍ଟି)                                                \
        କର {                                                            \
                ଯଦି (_unlikely_(!(ପଦସମଷ୍ଟି))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } ଯେତେବେଳେ (false)

ଶୂନ୍ୟ log_prefix_swap(ଅଚଳ ଅକ୍ଷର **prefix) {
        ପ୍ରତିଜ୍ଞା(prefix);

        SWAP_TWO(log_prefix, *prefix);
}

ସ୍ଥିର ଶୂନ୍ୟ log_close_console(ଶୂନ୍ୟ) {
        /* See comment in log_close_journal() */
        (ଶୂନ୍ୟ) safe_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_open_console(ଶୂନ୍ୟ) {

        ଯଦି (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (console_fd < 3) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ log_close_kmsg(ଶୂନ୍ୟ) {
        /* See comment in log_close_journal() */
        (ଶୂନ୍ୟ) safe_close(TAKE_FD(kmsg_fd));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_open_kmsg(ଶୂନ୍ୟ) {

        ଯଦି (kmsg_fd >= 0)
                ଫେରନ୍ତୁ 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ଯଦି (kmsg_fd < 0)
                ଫେରନ୍ତୁ -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ log_close_syslog(ଶୂନ୍ୟ) {
        /* See comment in log_close_journal() */
        (ଶୂନ୍ୟ) safe_close(TAKE_FD(syslog_fd));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା create_log_socket(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type) {
        ସଂରଚନା timeval tv;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        fd = fd_move_above_stdio(fd);
        (ଶୂନ୍ୟ) fd_inc_sndbuf(fd, SNDBUF_SIZE);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ଯଦି (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_PER_MSEC);
        ଅନ୍ୟଥା
                timeval_store(&tv, 10 * USEC_PER_SEC);
        (ଶୂନ୍ୟ) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, ଆକାର(tv));

        ଫେରନ୍ତୁ fd;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_open_syslog(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (syslog_fd >= 0)
                ଫେରନ୍ତୁ 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ଯଦି (syslog_fd < 0) {
                r = syslog_fd;
                ଯାଅ fail;
        }

        r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
        ଯଦି (r < 0) {
                safe_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ଯଦି (syslog_fd < 0) {
                        r = syslog_fd;
                        ଯାଅ fail;
                }

                r = connect_unix_path(syslog_fd, AT_FDCWD, "/dev/log");
                ଯଦି (r < 0)
                        ଯାଅ fail;

                syslog_is_stream = true;
        } ଅନ୍ୟଥା
                syslog_is_stream = false;

        ଫେରନ୍ତୁ 0;

fail:
        log_close_syslog();
        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ଶୂନ୍ୟ log_close_journal(ଶୂନ୍ୟ) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (ଶୂନ୍ୟ) safe_close(TAKE_FD(journal_fd));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_open_journal(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (journal_fd >= 0)
                ଫେରନ୍ତୁ 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ଯଦି (journal_fd < 0) {
                r = journal_fd;
                ଯାଅ fail;
        }

        r = connect_unix_path(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ଯଦି (r < 0)
                ଯାଅ fail;

        ଫେରନ୍ତୁ 0;

fail:
        log_close_journal();
        ଫେରନ୍ତୁ r;
}

bool stderr_is_journal(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *w = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *e;
        uint64_t dev, ino;
        ସଂରଚନା stat st;

        e = getenv("JOURNAL_STREAM");
        ଯଦି (!e)
                ଫେରନ୍ତୁ false;

        ଯଦି (extract_first_word(&e, &w, ":", EXTRACT_DONT_COALESCE_SEPARATORS) <= 0)
                ଫେରନ୍ତୁ false;
        ଯଦି (!e)
                ଫେରନ୍ତୁ false;

        ଯଦି (safe_atou64(w, &dev) < 0)
                ଫେରନ୍ତୁ false;
        ଯଦି (safe_atou64(e, &ino) < 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (fstat(STDERR_FILENO, &st) < 0)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ st.st_dev == dev && st.st_ino == ino;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_open(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ଯଦି (log_target == LOG_TARGET_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (getpid_cached() == 1 ||
            stderr_is_journal() ||
            IN_SET(log_target,
                   LOG_TARGET_KMSG,
                   LOG_TARGET_JOURNAL,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_SYSLOG,
                   LOG_TARGET_SYSLOG_OR_KMSG)) {

                ଯଦି (!prohibit_ipc) {
                        ଯଦି (IN_SET(log_target,
                                   LOG_TARGET_AUTO,
                                   LOG_TARGET_JOURNAL_OR_KMSG,
                                   LOG_TARGET_JOURNAL)) {

                                r = log_open_journal();
                                ଯଦି (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        ଫେରନ୍ତୁ r;
                                }
                        }

                        ଯଦି (IN_SET(log_target,
                                   LOG_TARGET_SYSLOG_OR_KMSG,
                                   LOG_TARGET_SYSLOG)) {

                                r = log_open_syslog();
                                ଯଦି (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        ଫେରନ୍ତୁ r;
                                }
                        }
                }

                ଯଦି (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_KMSG)) {
                        r = log_open_kmsg();
                        ଯଦି (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                ଫେରନ୍ତୁ r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        ଫେରନ୍ତୁ log_open_console();
}

ଶୂନ୍ୟ log_set_target(LogTarget target) {
        ପ୍ରତିଜ୍ଞା(target >= 0);
        ପ୍ରତିଜ୍ଞା(target < _LOG_TARGET_MAX);

        ଯଦି (upgrade_syslog_to_journal) {
                ଯଦି (target == LOG_TARGET_SYSLOG)
                        target = LOG_TARGET_JOURNAL;
                ଅନ୍ୟଥା ଯଦି (target == LOG_TARGET_SYSLOG_OR_KMSG)
                        target = LOG_TARGET_JOURNAL_OR_KMSG;
        }

        log_target = target;
}

ଶୂନ୍ୟ log_set_target_and_open(LogTarget target) {
        log_set_target(target);
        log_open();
}

ଶୂନ୍ୟ log_close(ଶୂନ୍ୟ) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

ଶୂନ୍ୟ log_forget_fds(ଶୂନ୍ୟ) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_max_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {
        ପ୍ରତିଜ୍ଞା(level == LOG_NULL || log_level_is_valid(level));

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା old = log_max_level;
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

        ଫେରନ୍ତୁ old;
}

ଶୂନ୍ୟ log_set_facility(ପୂର୍ଣ୍ଣସଂଖ୍ୟା facility) {
        log_facility = facility;
}

ସ୍ଥିର bool check_console_fd_is_tty(ଶୂନ୍ୟ) {
        ଯଦି (console_fd < 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_safe(console_fd);

        ଫେରନ୍ତୁ console_fd_is_tty;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_to_console(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *buffer) {

        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dumb = -1;

        ଅକ୍ଷର location[256],
             header_time[FORMAT_TIMESTAMP_MAX],
             prefix[1 + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        ସଂରଚନା iovec iovec[13];
        ଅଚଳ ଅକ୍ଷର *on = ଖାଲି, *off = ଖାଲି;
        size_t n = 0;

        ଯଦି (console_fd < 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ଯଦି (LOG_PRI(level) > log_target_max_level[LOG_TARGET_CONSOLE])
                ଫେରନ୍ତୁ 0;

        ଯଦି (log_target == LOG_TARGET_CONSOLE_PREFIXED) {
                xsprintf(prefix, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(prefix);
        }

        ଯଦି (show_time &&
            format_timestamp(header_time, ଆକାର(header_time), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(header_time);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ଯଦି (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(program_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ଯଦି (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ଯଦି (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ଖାଲି);

        ଯଦି (show_location) {
                ଅଚଳ ଅକ୍ଷର *lon = "", *loff = "";
                ଯଦି (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_normal();
                }

                (ଶୂନ୍ୟ) snprintf(location, ଆକାର location, "%s%s:%i%s: ", lon, file, ଧାଡ଼ି, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ଯଦି (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ଯଦି (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        ଯଦି (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ଯଦି (writev(console_fd, iovec, n) < 0) {

                ଯଦି (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (ଶୂନ୍ୟ) log_open_console();
                        ଯଦି (console_fd < 0)
                                ଫେରନ୍ତୁ 0;

                        ଯଦି (writev(console_fd, iovec, n) < 0)
                                ଫେରନ୍ତୁ -errno;
                } ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_to_syslog(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *buffer) {

        ଅକ୍ଷର header_priority[2 + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) + 1],
             header_time[64],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        ସଂରଚନା tm tm;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (syslog_fd < 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (LOG_PRI(level) > log_target_max_level[LOG_TARGET_SYSLOG])
                ଫେରନ୍ତୁ 0;

        xsprintf(header_priority, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (strftime(header_time, ଆକାର(header_time), "%h %e %T ", &tm) <= 0)
                ଫେରନ୍ତୁ -EINVAL;

        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ସଂରଚନା iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(header_time),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
        };
        ଅଚଳ ସଂରଚନା msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        ଯଦି (syslog_is_stream)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        ପାଇଁ (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (!syslog_is_stream)
                        ଭାଙ୍ଗ;

                ଯଦି (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        ଭାଙ୍ଗ;
        }

        ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_to_kmsg(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *buffer) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        ସ୍ଥିର thread_local RateLimit ratelimit = { 5 * USEC_PER_SEC, 200 };

        ଅକ୍ଷର header_priority[2 + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) + 1],
             header_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ଯଦି (kmsg_fd < 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (LOG_PRI(level) > log_target_max_level[LOG_TARGET_KMSG])
                ଫେରନ୍ତୁ 0;

        ଯଦି (ratelimit_kmsg && !ratelimit_below(&ratelimit)) {
                ଯଦି (ratelimit_ଅଙ୍କ_dropped(&ratelimit) > 1)
                        ଫେରନ୍ତୁ 0;

                buffer = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(header_priority, "<%i>", level);
        xsprintf(header_pid, "["PID_FMT"]: ", getpid_cached());

        ଅଚଳ ସଂରଚନା iovec iovec[] = {
                IOVEC_MAKE_STRING(header_priority),
                IOVEC_MAKE_STRING(program_invocation_short_name),
                IOVEC_MAKE_STRING(header_pid),
                IOVEC_MAKE_STRING(strempty(log_prefix)),
                IOVEC_MAKE_STRING(log_prefix ? ": " : ""),
                IOVEC_MAKE_STRING(buffer),
                IOVEC_MAKE_STRING("\n"),
        };

        ଯଦି (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_do_header(
                ଅକ୍ଷର *header,
                size_t size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field, ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field, ଅଚଳ ଅକ୍ଷର *extra) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_ମୂଲ୍ୟ(error);

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
                     ଧାଡ଼ି ? "CODE_LINE=" : "",
                     ଧାଡ଼ି ? 1 : 0, ଧାଡ଼ି, /* %.0d means no output too, special case for 0 */
                     ଧାଡ଼ି ? "\n" : "",
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

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ log_do_context(ସଂରଚନା iovec *iovec, size_t iovec_len, size_t *n) {
        ପ୍ରତିଜ୍ଞା(iovec);
        ପ୍ରତିଜ୍ଞା(n);

        LIST_FOREACH(ll, c, log_context_head()) {
                STRV_FOREACH(s, c->fields) {
                        ଯଦି (*n + 2 >= iovec_len)
                                ଫେରନ୍ତୁ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ପାଇଁ (size_t i = 0; i < c->n_ନିବେଶ_iovec; i++) {
                        ଯଦି (*n + 2 >= iovec_len)
                                ଫେରନ୍ତୁ;

                        iovec[(*n)++] = c->ନିବେଶ_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ଯଦି (c->ଚାବି && c->ମୂଲ୍ୟ) {
                        ଯଦି (*n + 3 >= iovec_len)
                                ଫେରନ୍ତୁ;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ଚାବି);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->ମୂଲ୍ୟ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_to_journal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅଚଳ ଅକ୍ଷର *buffer) {

        ଅକ୍ଷର header[LINE_MAX];
        size_t n = 0, iovec_len;
        ସଂରଚନା iovec *iovec;

        ଯଦି (journal_fd < 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (LOG_PRI(level) > log_target_max_level[LOG_TARGET_JOURNAL])
                ଫେରନ୍ତୁ 0;

        ଯଦି (!MUL_SAFE(&iovec_len, log_context_ଅଙ୍କ_fields_2(), 3) ||
            !ADD_SAFE(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        ଅନ୍ୟଥା
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(ସଂରଚନା iovec, iovec_len);

        log_do_header(header, ଆକାର(header), level, error, file, ଧାଡ଼ି, func, object_field, object, extra_field, extra);

        iovec[n++] = IOVEC_MAKE_STRING(header);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ଯଦି (log_prefix) {
                iovec[n++] = IOVEC_MAKE_STRING(log_prefix);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(buffer);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        ଅଚଳ ସଂରଚନା msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        ଯଦି (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dispatch_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅକ୍ଷର *buffer) {

        assert_raw(buffer);

        ଯଦି (log_target == LOG_TARGET_NULL)
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ଯଦି (LOG_FAC(level) == 0)
                level |= log_facility;

        ଯଦି (open_when_needed)
                (ଶୂନ୍ୟ) log_open();

        କର {
                ଅକ୍ଷର *e;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0;

                buffer += strspn(buffer, NEWLINE);

                ଯଦି (buffer[0] == 0)
                        ଭାଙ୍ଗ;

                ଯଦି ((e = strpbrk(buffer, NEWLINE)))
                        *(e++) = 0;

                ଯଦି (IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_JOURNAL)) {

                        k = write_to_journal(level, error, file, ଧାଡ଼ି, func, object_field, object, extra_field, extra, buffer);
                        ଯଦି (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ଯଦି (IN_SET(log_target, LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_SYSLOG)) {

                        k = write_to_syslog(level, error, file, ଧାଡ଼ି, func, buffer);
                        ଯଦି (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ଯଦି (k <= 0 &&
                    IN_SET(log_target, LOG_TARGET_AUTO,
                                       LOG_TARGET_SYSLOG_OR_KMSG,
                                       LOG_TARGET_JOURNAL_OR_KMSG,
                                       LOG_TARGET_KMSG)) {

                        ଯଦି (k < 0)
                                log_open_kmsg();

                        k = write_to_kmsg(level, error, file, ଧାଡ଼ି, func, buffer);
                        ଯଦି (k < 0) {
                                log_close_kmsg();
                                (ଶୂନ୍ୟ) log_open_console();
                        }
                }

                ଯଦି (k <= 0)
                        (ଶୂନ୍ୟ) write_to_console(level, error, file, ଧାଡ଼ି, func, buffer);

                buffer = e;
        } ଯେତେବେଳେ (buffer);

        ଯଦି (open_when_needed)
                log_close();

        ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dump_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅକ୍ଷର *buffer) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ଯଦି (_likely_(LOG_PRI(level) > log_max_level))
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        ଫେରନ୍ତୁ log_dispatch_internal(level, error, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, buffer);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_internalv(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format,
                va_list ap) {

        ଯଦି (_likely_(LOG_PRI(level) > log_max_level))
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        ଅକ୍ଷର buffer[LINE_MAX];
        LOCAL_ERRNO(ERRNO_ମୂଲ୍ୟ(error));

        (ଶୂନ୍ୟ) vsnprintf(buffer, ଆକାର buffer, format, ap);

        ଫେରନ୍ତୁ log_dispatch_internal(level, error, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, buffer);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format, ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = log_internalv(level, error, file, ଧାଡ଼ି, func, format, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_object_internalv(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅଚଳ ଅକ୍ଷର *format,
                va_list ap) {

        ଅକ୍ଷର *buffer, *b;

        ଯଦି (_likely_(LOG_PRI(level) > log_max_level))
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_ମୂଲ୍ୟ(error));

        LOG_SET_PREFIX(object);

        b = buffer = newa(ଅକ୍ଷର, LINE_MAX);
        (ଶୂନ୍ୟ) vsnprintf(b, LINE_MAX, format, ap);

        ଫେରନ୍ତୁ log_dispatch_internal(level, error, file, ଧାଡ଼ି, func,
                                     object_field, object, extra_field, extra, buffer);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_object_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅଚଳ ଅକ୍ଷର *format, ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, ଧାଡ଼ି, func, object_field, object, extra_field, extra, format, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_oom_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ଅକ୍ଷର *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *func) {
        ଫେରନ୍ତୁ log_internal(level, ENOMEM, file, ଧାଡ଼ି, func, "Out of memory.");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_format_iovec(
                ସଂରଚନା iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *format,
                va_list ap) {

        ପ୍ରତିଜ୍ଞା(iovec);
        ପ୍ରତିଜ୍ଞା(n);

        ଯେତେବେଳେ (format && *n + 1 < iovec_len) {
                va_list aq;
                ଅକ୍ଷର *m;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_ମୂଲ୍ୟ(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ଯଦି (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, ଅକ୍ଷର *);
        }
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_struct_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format, ...) {

        ଅକ୍ଷର buf[LINE_MAX];
        bool found = false;
        PROTECT_ERRNO;
        va_list ap;

        ଯଦି (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        ଯଦି (LOG_FAC(level) == 0)
                level |= log_facility;

        ଯଦି (IN_SET(log_target,
                   LOG_TARGET_AUTO,
                   LOG_TARGET_JOURNAL_OR_KMSG,
                   LOG_TARGET_JOURNAL)) {

                ଯଦି (open_when_needed)
                        log_open_journal();

                ଯଦି (journal_fd >= 0) {
                        ଅକ୍ଷର header[LINE_MAX];
                        ସଂରଚନା iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
                        bool fallback = false;

                        ଯଦି (!MUL_SAFE(&iovec_len, log_context_ଅଙ୍କ_fields_2(), 3) ||
                            !ADD_SAFE(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        ଅନ୍ୟଥା
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(ସଂରଚନା iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_header(header, ଆକାର(header), level, error, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);
                        iovec[n++] = IOVEC_MAKE_STRING(header);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        ଯଦି (r < 0)
                                fallback = true;
                        ଅନ୍ୟଥା {
                                log_do_context(iovec, iovec_len, &n);

                                ଅଚଳ ସଂରଚନା msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (ଶୂନ୍ୟ) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        ପାଇଁ (size_t i = 1; i < m; i += 2)
                                ମୁକ୍ତ(iovec[i].iov_base);

                        ଯଦି (!fallback) {
                                ଯଦି (open_when_needed)
                                        log_close();

                                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        ଯେତେବେଳେ (format) {
                va_list aq;

                errno = ERRNO_ମୂଲ୍ୟ(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (ଶୂନ୍ୟ) vsnprintf(buf, ଆକାର buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ଯଦି (startswith(buf, "MESSAGE=")) {
                        found = true;
                        ଭାଙ୍ଗ;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, ଅକ୍ଷର *);
        }
        va_end(ap);

        ଯଦି (!found) {
                ଯଦି (open_when_needed)
                        log_close();

                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);
        }

        ଫେରନ୍ତୁ log_dispatch_internal(level, error, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, buf + 8);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_struct_iovec_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ସଂରଚନା iovec ନିବେଶ_iovec[],
                size_t n_ନିବେଶ_iovec) {

        PROTECT_ERRNO;

        ଯଦି (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        ଯଦି (LOG_FAC(level) == 0)
                level |= log_facility;

        ଯଦି (IN_SET(log_target, LOG_TARGET_AUTO,
                               LOG_TARGET_JOURNAL_OR_KMSG,
                               LOG_TARGET_JOURNAL) &&
            journal_fd >= 0) {

                ଅକ୍ଷର header[LINE_MAX];
                ସଂରଚନା iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_ନିବେଶ_iovec_expanded;

                ଯଦି (!MUL_SAFE(&n_ନିବେଶ_iovec_expanded, n_ନିବେଶ_iovec, 2) ||
                    !MUL_SAFE(&n_context_iovec, log_context_ଅଙ୍କ_fields_2(), 3) ||
                    !ADD_SAFE(&iovec_len, 1, n_ନିବେଶ_iovec_expanded) ||
                    !ADD_SAFE(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                ଅନ୍ୟଥା
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(ସଂରଚନା iovec, iovec_len);

                log_do_header(header, ଆକାର(header), level, error, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);

                iovec[n++] = IOVEC_MAKE_STRING(header);
                ପାଇଁ (size_t i = 0; i < n_ନିବେଶ_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = ନିବେଶ_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                ଅଚଳ ସଂରଚନା msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ଯଦି (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);
        }

        ପାଇଁ (size_t i = 0; i < n_ନିବେଶ_iovec; i++)
                ଯଦି (memory_startswith(ନିବେଶ_iovec[i].iov_base, ନିବେଶ_iovec[i].iov_len, "MESSAGE=")) {
                        ଅକ୍ଷର *m;

                        m = strndupa_safe((ଅକ୍ଷର*) ନିବେଶ_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          ନିବେଶ_iovec[i].iov_len - STRLEN("MESSAGE="));

                        ଫେରନ୍ତୁ log_dispatch_internal(level, error, file, ଧାଡ଼ି, func, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, m);
                }

        /* Couldn't find MESSAGE=. */
        ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_target_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        LogTarget t;

        t = log_target_from_string(e);
        ଯଦି (t < 0)
                ଫେରନ୍ତୁ t;

        log_set_target(t);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_max_level_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *word = ଖାଲି, *prefix = ଖାଲି;
                LogTarget target;
                ଅଚଳ ଅକ୍ଷର *colon;

                r = extract_first_word(&e, &word, ",", 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                colon = strchr(word, ':');
                ଯଦି (!colon) {
                        r = log_level_from_string(word);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        log_set_max_level(r);
                        ଜାରି;
                }

                prefix = strndup(word, colon - word);
                ଯଦି (!prefix)
                        ଫେରନ୍ତୁ -ENOMEM;

                target = log_target_from_string(prefix);
                ଯଦି (target < 0)
                        ଫେରନ୍ତୁ target;

                ଯଦି (target >= _LOG_TARGET_SINGLE_MAX)
                        ଫେରନ୍ତୁ -EINVAL;

                r = log_level_from_string(colon + 1);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                log_target_max_level[target] = r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_max_levels_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = log_level_to_string_alloc(level, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (LogTarget target = 0; target < _LOG_TARGET_SINGLE_MAX; target++) {
                _cleanup_free_ ଅକ୍ଷର *l = ଖାଲି;

                ଯଦି (log_target_max_level[target] == INT_MAX)
                        ଜାରି;

                r = log_level_to_string_alloc(log_target_max_level[target], &l);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_target_to_string(target), l);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_ratelimit_kmsg_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = parse_boolean(e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ratelimit_kmsg = r;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_proc_cmdline_item(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, ଶୂନ୍ୟ *data) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ଯଦି (streq(ଚାବି, "debug") && !ମୂଲ୍ୟ)
                log_set_max_level(LOG_DEBUG);

        ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_target")) {

                ଯଦି (proc_cmdline_ମୂଲ୍ୟ_missing(ଚାବି, ମୂଲ୍ୟ))
                        ଫେରନ୍ତୁ 0;

                ଯଦି (log_set_target_from_string(ମୂଲ୍ୟ) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", ମୂଲ୍ୟ);

        } ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_level")) {

                ଯଦି (proc_cmdline_ମୂଲ୍ୟ_missing(ଚାବି, ମୂଲ୍ୟ))
                        ଫେରନ୍ତୁ 0;

                ଯଦି (log_set_max_level_from_string(ମୂଲ୍ୟ) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", ମୂଲ୍ୟ);

        } ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_color")) {

                ଯଦି (log_show_color_from_string(ମୂଲ୍ୟ ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", ମୂଲ୍ୟ);

        } ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_location")) {

                ଯଦି (log_show_location_from_string(ମୂଲ୍ୟ ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", ମୂଲ୍ୟ);

        } ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_tid")) {

                ଯଦି (log_show_tid_from_string(ମୂଲ୍ୟ ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", ମୂଲ୍ୟ);

        } ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_time")) {

                ଯଦି (log_show_time_from_string(ମୂଲ୍ୟ ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", ମୂଲ୍ୟ);

        } ଅନ୍ୟଥା ଯଦି (proc_cmdline_ଚାବି_streq(ଚାବି, "systemd.log_ratelimit_kmsg")) {

                ଯଦି (log_set_ratelimit_kmsg_from_string(ମୂଲ୍ୟ ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", ମୂଲ୍ୟ);
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର bool should_parse_proc_cmdline(ଶୂନ୍ୟ) {
        /* PID1 always reads the kernel command line. */
        ଯଦି (getpid_cached() == 1)
                ଫେରନ୍ତୁ true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        ଫେରନ୍ତୁ invoked_by_systemd();
}

ଶୂନ୍ୟ log_parse_environment_variables(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ଯଦି (e && log_set_target_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ଯଦି (e) {
                r = log_set_max_level_from_string(e);
                ଯଦି (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } ଅନ୍ୟଥା {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ଯଦି (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                ଅନ୍ୟଥା ଯଦି (r > 0)
                        log_set_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ଯଦି (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ଯଦି (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ଯଦି (e && log_show_time_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ଯଦି (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ଯଦି (e && log_set_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ଯଦି (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

ଶୂନ୍ୟ log_parse_environment(ଶୂନ୍ୟ) {
        /* Do not call from library code. */

        ଯଦି (should_parse_proc_cmdline())
                (ଶୂନ୍ୟ) proc_cmdline_parse(parse_proc_cmdline_item, ଖାଲି, PROC_CMDLINE_STRIP_RD_PREFIX);

        log_parse_environment_variables();
}

LogTarget log_get_target(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ log_target;
}

ଶୂନ୍ୟ log_settle_target(ଶୂନ୍ୟ) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        LogTarget t = log_get_target();

        ଯଦି (t != LOG_TARGET_AUTO)
                ଫେରନ୍ତୁ;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_TARGET_KMSG : LOG_TARGET_JOURNAL_OR_KMSG)
                                                        : LOG_TARGET_CONSOLE;
        log_set_target(t);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_get_max_level(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ log_max_level;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_get_target_max_level(LogTarget target) {
        ପ୍ରତିଜ୍ଞା(target >= 0);
        ପ୍ରତିଜ୍ଞା(target < _LOG_TARGET_SINGLE_MAX);
        ଫେରନ୍ତୁ log_target_max_level[target];
}

ଶୂନ୍ୟ log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ show_color > 0; /* Defaults to false. */
}

ଶୂନ୍ୟ log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ show_location;
}

ଶୂନ୍ୟ log_show_time(bool b) {
        show_time = b;
}

bool log_get_show_time(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ show_time;
}

ଶୂନ୍ୟ log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ show_tid;
}

ଶୂନ୍ୟ log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ show_executable;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_color_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = parse_boolean(e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        log_show_color(r);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_location_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = parse_boolean(e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        log_show_location(r);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_time_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = parse_boolean(e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        log_show_time(r);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_tid_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = parse_boolean(e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        log_show_tid(r);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_executable_from_string(ଅଚଳ ଅକ୍ଷର *e) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = parse_boolean(e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        log_show_executable(r);
        ଫେରନ୍ତୁ 0;
}

bool log_on_console(ଶୂନ୍ୟ) {
        ଯଦି (IN_SET(log_target, LOG_TARGET_CONSOLE,
                               LOG_TARGET_CONSOLE_PREFIXED))
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ log_target_table[_LOG_TARGET_MAX] = {
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

ଶୂନ୍ୟ log_received_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ସଂରଚନା signalfd_siginfo *si) {
        ପ୍ରତିଜ୍ଞା(si);

        ଯଦି (si_code_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                (ଶୂନ୍ୟ) pid_get_comm(si->ssi_pid, &p);

                log_full(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_to_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } ଅନ୍ୟଥା
                log_full(level,
                         "Received SIG%s.",
                         signal_to_string(si->ssi_signo));
}

ଶୂନ୍ୟ set_log_syntax_callback(log_syntax_callback_t cb, ଶୂନ୍ୟ *userdata) {
        ପ୍ରତିଜ୍ଞା(!log_syntax_callback || !cb);
        ପ୍ରତିଜ୍ଞା(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_syntax_internal(
                ଅଚଳ ଅକ୍ଷର *unit,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ଅଚଳ ଅକ୍ଷର *config_file,
                unsigned config_ଧାଡ଼ି,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format, ...) {

        PROTECT_ERRNO;

        ଯଦି (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ଯଦି (_likely_(LOG_PRI(level) > log_max_level) ||
            log_target == LOG_TARGET_NULL)
                ଫେରନ୍ତୁ -ERRNO_ମୂଲ୍ୟ(error);

        ଅକ୍ଷର buffer[LINE_MAX];
        va_list ap;
        ଅଚଳ ଅକ୍ଷର *unit_fmt = ଖାଲି;

        errno = ERRNO_ମୂଲ୍ୟ(error);

        va_start(ap, format);
        (ଶୂନ୍ୟ) vsnprintf(buffer, ଆକାର buffer, format, ap);
        va_end(ap);

        ଯଦି (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ଯଦି (config_file) {
                ଯଦି (config_ଧାଡ଼ି > 0)
                        ଫେରନ୍ତୁ log_struct_internal(
                                        level,
                                        error,
                                        file, ଧାଡ଼ି, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_ITEM("CONFIG_LINE=%u", config_ଧାଡ଼ି),
                                        LOG_MESSAGE("%s:%u: %s", config_file, config_ଧାଡ଼ି, buffer),
                                        unit_fmt, unit,
                                        ଖାଲି);
                ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ log_struct_internal(
                                        level,
                                        error,
                                        file, ଧାଡ଼ି, func,
                                        LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                        LOG_ITEM("CONFIG_FILE=%s", config_file),
                                        LOG_MESSAGE("%s: %s", config_file, buffer),
                                        unit_fmt, unit,
                                        ଖାଲି);
        } ଅନ୍ୟଥା ଯଦି (unit)
                ଫେରନ୍ତୁ log_struct_internal(
                                level,
                                error,
                                file, ଧାଡ଼ି, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s: %s", unit, buffer),
                                unit_fmt, unit,
                                ଖାଲି);
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ log_struct_internal(
                                level,
                                error,
                                file, ଧାଡ଼ି, func,
                                LOG_MESSAGE_ID(SD_MESSAGE_INVALID_CONFIGURATION_STR),
                                LOG_MESSAGE("%s", buffer),
                                ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_syntax_invalid_utf8_internal(
                ଅଚଳ ଅକ୍ଷର *unit,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ଅଚଳ ଅକ୍ଷର *config_file,
                unsigned config_ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

        ଯଦି (rvalue)
                p = utf8_escape_invalid(rvalue);

        ଫେରନ୍ତୁ log_syntax_internal(unit, level, config_file, config_ଧାଡ଼ି,
                                   SYNTHETIC_ERRNO(EINVAL), file, ଧାଡ଼ି, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_syntax_parse_error_internal(
                ଅଚଳ ଅକ୍ଷର *unit,
                ଅଚଳ ଅକ୍ଷର *config_file,
                unsigned config_ଧାଡ଼ି,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                bool critical,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *lvalue,
                ଅଚଳ ଅକ୍ଷର *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ ଅକ୍ଷର *escaped = ଖାଲି;

        /* OOM is always handled as critical. */
        ଯଦି (ERRNO_ମୂଲ୍ୟ(error) == ENOMEM)
                ଫେରନ୍ତୁ log_oom_internal(LOG_ERR, file, ଧାଡ଼ି, func);

        ଯଦି (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ଯଦି (!escaped)
                        rvalue = "(oom)";
                ଅନ୍ୟଥା
                        rvalue = " (escaped)";
        }

        log_syntax_internal(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_ଧାଡ଼ି, error,
                            file, ଧାଡ଼ି, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        ଫେରନ୍ତୁ critical ? -ERRNO_ମୂଲ୍ୟ(error) : 0;
}

ଶୂନ୍ୟ log_set_upgrade_syslog_to_journal(bool b) {
        upgrade_syslog_to_journal = b;

        /* Make the change effective immediately */
        ଯଦି (b) {
                ଯଦି (log_target == LOG_TARGET_SYSLOG)
                        log_target = LOG_TARGET_JOURNAL;
                ଅନ୍ୟଥା ଯଦି (log_target == LOG_TARGET_SYSLOG_OR_KMSG)
                        log_target = LOG_TARGET_JOURNAL_OR_KMSG;
        }
}

ଶୂନ୍ୟ log_set_always_reopen_console(bool b) {
        always_reopen_console = b;
}

ଶୂନ୍ୟ log_set_open_when_needed(bool b) {
        open_when_needed = b;
}

ଶୂନ୍ୟ log_set_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_emergency_level(ଶୂନ୍ୟ) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        ଫେରନ୍ତୁ getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dup_console(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ଯଦି (console_fd < 0 || console_fd >= 3)
                ଫେରନ୍ତୁ 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ଯଦି (copy < 0)
                ଫେରନ୍ତୁ -errno;

        console_fd = copy;
        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ log_setup(ଶୂନ୍ୟ) {
        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        (ଶୂନ୍ୟ) log_open();
        ଯଦି (log_on_console() && show_color < 0)
                log_show_color(true);
}

ଅଚଳ ଅକ୍ଷର* _log_set_prefix(ଅଚଳ ଅକ୍ଷର *prefix, bool force) {
        ଅଚଳ ଅକ୍ଷର *old = log_prefix;

        ଯଦି (prefix || force)
                log_prefix = prefix;

        ଫେରନ୍ତୁ old;
}
