/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sys/signalfd.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <sys/uio.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "sd-messages.h"

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "ansi-color.h"
#అన్తర్భావ "argv-util.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "list.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "log-context.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "proc-cmdline.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "ratelimit.h"
#అన్తర్భావ "signal-util.h"
#అన్తర్భావ "socket-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "syslog-util.h"
#అన్తర్భావ "terminal-util.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "utf8.h"

#నిర్వచించు SNDBUF_కొలత (8*1024*1024)
#నిర్వచించు IOVEC_MAX 256U

స్థిర log_syntax_callback_t log_syntax_callback = ఖాళీ;
స్థిర శూన్య *log_syntax_callback_userdata = ఖాళీ;

స్థిర Log_లక్ష్యం log_లక్ష్యం = LOG_లక్ష్యం_CONSOLE;
స్థిర పూర్ణాంకం log_max_level = LOG_INFO;
స్థిర పూర్ణాంకం log_లక్ష్యం_max_level[LOG_లక్ష్యం_SINGLE_MAX] = {
        [LOG_లక్ష్యం_CONSOLE] = గరిష్ఠ_పూర్ణాంకం,
        [LOG_లక్ష్యం_KMSG]    = గరిష్ఠ_పూర్ణాంకం,
        [LOG_లక్ష్యం_SYSLOG]  = గరిష్ఠ_పూర్ణాంకం,
        [LOG_లక్ష్యం_JOURNAL] = గరిష్ఠ_పూర్ణాంకం,
};
స్థిర పూర్ణాంకం log_facility = LOG_DAEMON;
స్థిర bool ratelimit_kmsg = true;

స్థిర పూర్ణాంకం console_fd = STDERR_FILENO;
స్థిర పూర్ణాంకం console_fd_is_tty = -1; /* tri-state: -1 means don't know */
స్థిర పూర్ణాంకం syslog_fd = -EBADF;
స్థిర పూర్ణాంకం kmsg_fd = -EBADF;
స్థిర పూర్ణాంకం journal_fd = -EBADF;

స్థిర bool syslog_is_ప్రవాహం = false;

స్థిర పూర్ణాంకం show_color = -1; /* tristate */
స్థిర bool show_location = false;
స్థిర bool show_సమయం = false;
స్థిర bool show_tid = false;
స్థిర bool show_executable = false;

స్థిర bool upgrade_syslog_గమ్యం_journal = false;
స్థిర bool always_reopen_console = false;
స్థిర bool open_when_needed = false;
స్థిర bool prohibit_ipc = false;

స్థిర thread_local స్థిరరాశి అక్షరం *log_ఉపసర్గ = ఖాళీ;

#ఒకవేళ LOG_సందేశం_VERIFICATION || defined(__COVERITY__)
bool log_సందేశం_dummy = false; /* Always false */
#అయితే_ముగింపు

/* An assert to use in logging functions that does not call recursively
 * into our logging functions (since that might lead to a loop). */
#నిర్వచించు assert_raw(సమాసం)                                                \
        చేయి {                                                            \
                ఒకవేళ (_unlikely_(!(సమాసం))) {                              \
                        fputs(#expr "\n", stderr);                      \
                        abort();                                        \
                }                                                       \
        } అయితే (false)

శూన్య log_ఉపసర్గ_మార్పిడి(స్థిరరాశి అక్షరం **ఉపసర్గ) {
        ప్రతిజ్ఞా(ఉపసర్గ);

        మార్పిడి_TWO(log_ఉపసర్గ, *ఉపసర్గ);
}

స్థిర శూన్య log_close_console(శూన్య) {
        /* See comment in log_close_journal() */
        (శూన్య) సురక్షిత_close_above_stdio(TAKE_FD(console_fd));
        console_fd_is_tty = -1;
}

స్థిర పూర్ణాంకం log_open_console(శూన్య) {

        ఒకవేళ (!always_reopen_console) {
                console_fd = STDERR_FILENO;
                console_fd_is_tty = -1;
                తిరిగి 0;
        }

        ఒకవేళ (console_fd < 3) {
                పూర్ణాంకం fd;

                fd = open_terminal("/dev/console", O_WRONLY|O_NOCTTY|O_CLOEXEC);
                ఒకవేళ (fd < 0)
                        తిరిగి fd;

                console_fd = fd_move_above_stdio(fd);
                console_fd_is_tty = true;
        }

        తిరిగి 0;
}

స్థిర శూన్య log_close_kmsg(శూన్య) {
        /* See comment in log_close_journal() */
        (శూన్య) సురక్షిత_close(TAKE_FD(kmsg_fd));
}

స్థిర పూర్ణాంకం log_open_kmsg(శూన్య) {

        ఒకవేళ (kmsg_fd >= 0)
                తిరిగి 0;

        kmsg_fd = open("/dev/kmsg", O_WRONLY|O_NOCTTY|O_CLOEXEC);
        ఒకవేళ (kmsg_fd < 0)
                తిరిగి -errno;

        kmsg_fd = fd_move_above_stdio(kmsg_fd);
        తిరిగి 0;
}

స్థిర శూన్య log_close_syslog(శూన్య) {
        /* See comment in log_close_journal() */
        (శూన్య) సురక్షిత_close(TAKE_FD(syslog_fd));
}

స్థిర పూర్ణాంకం create_log_socket(పూర్ణాంకం type) {
        నిర్మాణం timeval tv;
        పూర్ణాంకం fd;

        fd = socket(AF_UNIX, type|SOCK_CLOEXEC, 0);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        fd = fd_move_above_stdio(fd);
        (శూన్య) fd_inc_sndbuf(fd, SNDBUF_కొలత);

        /* We need a blocking fd here since we'd otherwise lose messages way too early. However, let's not hang forever
         * in the unlikely case of a deadlock. */
        ఒకవేళ (getpid_cached() == 1)
                timeval_store(&tv, 10 * USEC_చొప్పున_MSEC);
        లేకపోతే
                timeval_store(&tv, 10 * USEC_చొప్పున_SEC);
        (శూన్య) setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, పరిమాణం(tv));

        తిరిగి fd;
}

స్థిర పూర్ణాంకం log_open_syslog(శూన్య) {
        పూర్ణాంకం r;

        ఒకవేళ (syslog_fd >= 0)
                తిరిగి 0;

        syslog_fd = create_log_socket(SOCK_DGRAM);
        ఒకవేళ (syslog_fd < 0) {
                r = syslog_fd;
                వెళ్ళు fail;
        }

        r = connect_unix_మార్గం(syslog_fd, AT_FDCWD, "/dev/log");
        ఒకవేళ (r < 0) {
                సురక్షిత_close(syslog_fd);

                /* Some legacy syslog systems still use stream sockets. They really shouldn't. But what can
                 * we do... */
                syslog_fd = create_log_socket(SOCK_STREAM);
                ఒకవేళ (syslog_fd < 0) {
                        r = syslog_fd;
                        వెళ్ళు fail;
                }

                r = connect_unix_మార్గం(syslog_fd, AT_FDCWD, "/dev/log");
                ఒకవేళ (r < 0)
                        వెళ్ళు fail;

                syslog_is_ప్రవాహం = true;
        } లేకపోతే
                syslog_is_ప్రవాహం = false;

        తిరిగి 0;

fail:
        log_close_syslog();
        తిరిగి r;
}

స్థిర శూన్య log_close_journal(శూన్య) {
        /* If the journal FD is bad, safe_close will fail, and will try to log, which will fail, so we'll
         * try to close the journal FD, which is bad, so safe_close will fail... Whether we can close it
         * or not, invalidate it immediately so that we don't get in a recursive loop until we run out of
         * stack. */
        (శూన్య) సురక్షిత_close(TAKE_FD(journal_fd));
}

స్థిర పూర్ణాంకం log_open_journal(శూన్య) {
        పూర్ణాంకం r;

        ఒకవేళ (journal_fd >= 0)
                తిరిగి 0;

        journal_fd = create_log_socket(SOCK_DGRAM);
        ఒకవేళ (journal_fd < 0) {
                r = journal_fd;
                వెళ్ళు fail;
        }

        r = connect_unix_మార్గం(journal_fd, AT_FDCWD, "/run/systemd/journal/socket");
        ఒకవేళ (r < 0)
                వెళ్ళు fail;

        తిరిగి 0;

fail:
        log_close_journal();
        తిరిగి r;
}

bool stderr_is_journal(శూన్య) {
        _cleanup_free_ అక్షరం *w = ఖాళీ;
        స్థిరరాశి అక్షరం *e;
        uint64_t dev, ino;
        నిర్మాణం stat st;

        e = getenv("JOURNAL_STREAM");
        ఒకవేళ (!e)
                తిరిగి false;

        ఒకవేళ (వెలికితీయి_మొదటి_మాట(&e, &w, ":", వెలికితీయి_DONT_COALESCE_SEPARATORS) <= 0)
                తిరిగి false;
        ఒకవేళ (!e)
                తిరిగి false;

        ఒకవేళ (సురక్షిత_atou64(w, &dev) < 0)
                తిరిగి false;
        ఒకవేళ (సురక్షిత_atou64(e, &ino) < 0)
                తిరిగి false;

        ఒకవేళ (fstat(STDERR_FILENO, &st) < 0)
                తిరిగి false;

        తిరిగి st.st_dev == dev && st.st_ino == ino;
}

పూర్ణాంకం log_open(శూన్య) {
        పూర్ణాంకం r;

        /* Do not call from library code. */

        /* This function is often called in preparation for logging. Let's make sure we don't clobber errno,
         * so that a call to a logging function immediately following a log_open() call can still easily
         * reference an error that happened immediately before the log_open() call. */
        PROTECT_ERRNO;

        /* If we don't use the console, we close it here to not get killed by SAK. If we don't use syslog, we
         * close it here too, so that we are not confused by somebody deleting the socket in the fs, and to
         * make sure we don't use it if prohibit_ipc is set. If we don't use /dev/kmsg we still keep it open,
         * because there is no reason to close it. */

        ఒకవేళ (log_లక్ష్యం == LOG_లక్ష్యం_NULL) {
                log_close_journal();
                log_close_syslog();
                log_close_console();
                తిరిగి 0;
        }

        ఒకవేళ (getpid_cached() == 1 ||
            stderr_is_journal() ||
            లోపల_సమితి(log_లక్ష్యం,
                   LOG_లక్ష్యం_KMSG,
                   LOG_లక్ష్యం_JOURNAL,
                   LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                   LOG_లక్ష్యం_SYSLOG,
                   LOG_లక్ష్యం_SYSLOG_OR_KMSG)) {

                ఒకవేళ (!prohibit_ipc) {
                        ఒకవేళ (లోపల_సమితి(log_లక్ష్యం,
                                   LOG_లక్ష్యం_AUTO,
                                   LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                                   LOG_లక్ష్యం_JOURNAL)) {

                                r = log_open_journal();
                                ఒకవేళ (r >= 0) {
                                        log_close_syslog();
                                        log_close_console();
                                        తిరిగి r;
                                }
                        }

                        ఒకవేళ (లోపల_సమితి(log_లక్ష్యం,
                                   LOG_లక్ష్యం_SYSLOG_OR_KMSG,
                                   LOG_లక్ష్యం_SYSLOG)) {

                                r = log_open_syslog();
                                ఒకవేళ (r >= 0) {
                                        log_close_journal();
                                        log_close_console();
                                        తిరిగి r;
                                }
                        }
                }

                ఒకవేళ (లోపల_సమితి(log_లక్ష్యం, LOG_లక్ష్యం_AUTO,
                                       LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                                       LOG_లక్ష్యం_SYSLOG_OR_KMSG,
                                       LOG_లక్ష్యం_KMSG)) {
                        r = log_open_kmsg();
                        ఒకవేళ (r >= 0) {
                                log_close_journal();
                                log_close_syslog();
                                log_close_console();
                                తిరిగి r;
                        }
                }
        }

        log_close_journal();
        log_close_syslog();

        తిరిగి log_open_console();
}

శూన్య log_సమితి_లక్ష్యం(Log_లక్ష్యం లక్ష్యం) {
        ప్రతిజ్ఞా(లక్ష్యం >= 0);
        ప్రతిజ్ఞా(లక్ష్యం < LOG_లక్ష్యం_MAX);

        ఒకవేళ (upgrade_syslog_గమ్యం_journal) {
                ఒకవేళ (లక్ష్యం == LOG_లక్ష్యం_SYSLOG)
                        లక్ష్యం = LOG_లక్ష్యం_JOURNAL;
                లేకపోతే ఒకవేళ (లక్ష్యం == LOG_లక్ష్యం_SYSLOG_OR_KMSG)
                        లక్ష్యం = LOG_లక్ష్యం_JOURNAL_OR_KMSG;
        }

        log_లక్ష్యం = లక్ష్యం;
}

శూన్య log_సమితి_లక్ష్యం_and_open(Log_లక్ష్యం లక్ష్యం) {
        log_సమితి_లక్ష్యం(లక్ష్యం);
        log_open();
}

శూన్య log_close(శూన్య) {
        /* Do not call from library code. */

        log_close_journal();
        log_close_syslog();
        log_close_kmsg();
        log_close_console();
}

శూన్య log_forget_fds(శూన్య) {
        /* Do not call from library code. */

        console_fd = kmsg_fd = syslog_fd = journal_fd = -EBADF;
        console_fd_is_tty = -1;
}

పూర్ణాంకం log_సమితి_max_level(పూర్ణాంకం level) {
        ప్రతిజ్ఞా(level == LOG_NULL || log_level_is_valid(level));

        పూర్ణాంకం old = log_max_level;
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

        తిరిగి old;
}

శూన్య log_సమితి_facility(పూర్ణాంకం facility) {
        log_facility = facility;
}

స్థిర bool check_console_fd_is_tty(శూన్య) {
        ఒకవేళ (console_fd < 0)
                తిరిగి false;

        ఒకవేళ (console_fd_is_tty < 0)
                console_fd_is_tty = isatty_సురక్షిత(console_fd);

        తిరిగి console_fd_is_tty;
}

స్థిర పూర్ణాంకం write_గమ్యం_console(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *బఫరు) {

        స్థిర పూర్ణాంకం dumb = -1;

        అక్షరం location[256],
             శీర్షిక_సమయం[FORMAT_TIMESTAMP_MAX],
             ఉపసర్గ[1 + DECIMAL_STR_MAX(పూర్ణాంకం) + 2],
             tid_string[3 + DECIMAL_STR_MAX(pid_t) + 1];
        నిర్మాణం iovec iovec[13];
        స్థిరరాశి అక్షరం *on = ఖాళీ, *off = ఖాళీ;
        size_t n = 0;

        ఒకవేళ (console_fd < 0)
                తిరిగి 0;

        ఒకవేళ (dumb < 0)
                dumb = getenv_terminal_is_dumb();

        ఒకవేళ (LOG_PRI(level) > log_లక్ష్యం_max_level[LOG_లక్ష్యం_CONSOLE])
                తిరిగి 0;

        ఒకవేళ (log_లక్ష్యం == LOG_లక్ష్యం_CONSOLE_PREFIXED) {
                xsprintf(ఉపసర్గ, "<%i>", level);
                iovec[n++] = IOVEC_MAKE_STRING(ఉపసర్గ);
        }

        ఒకవేళ (show_సమయం &&
            format_timestamp(శీర్షిక_సమయం, పరిమాణం(శీర్షిక_సమయం), now(CLOCK_REALTIME))) {
                iovec[n++] = IOVEC_MAKE_STRING(శీర్షిక_సమయం);
                iovec[n++] = IOVEC_MAKE_STRING(" ");
        }

        ఒకవేళ (show_executable) {
                iovec[n++] = IOVEC_MAKE_STRING(కార్యక్రమం_invocation_short_name);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }

        ఒకవేళ (show_tid) {
                xsprintf(tid_string, "(" PID_FMT ") ", gettid());
                iovec[n++] = IOVEC_MAKE_STRING(tid_string);
        }

        ఒకవేళ (log_get_show_color())
                get_log_colors(LOG_PRI(level), &on, &off, ఖాళీ);

        ఒకవేళ (show_location) {
                స్థిరరాశి అక్షరం *lon = "", *loff = "";
                ఒకవేళ (log_get_show_color()) {
                        lon = ansi_highlight_yellow4();
                        loff = ansi_సాధారణ();
                }

                (శూన్య) snprintf(location, పరిమాణం location, "%s%s:%i%s: ", lon, file, పంక్తి, loff);
                iovec[n++] = IOVEC_MAKE_STRING(location);
        }

        ఒకవేళ (on)
                iovec[n++] = IOVEC_MAKE_STRING(on);
        ఒకవేళ (log_ఉపసర్గ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_ఉపసర్గ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(బఫరు);
        ఒకవేళ (off)
                iovec[n++] = IOVEC_MAKE_STRING(off);

        /* When writing to a TTY we output an extra '\r' (i.e. CR) first, to generate CRNL rather than just
         * NL. This is a robustness thing in case the TTY is currently in raw mode (specifically: has the
         * ONLCR flag off). We want that subsequent output definitely starts at the beginning of the line
         * again, after all. If the TTY is not in raw mode the extra CR should not hurt. If we're writing to
         * a dumb terminal, only write NL as CRNL might be interpreted as a double newline. */
        iovec[n++] = IOVEC_MAKE_STRING(check_console_fd_is_tty() && !dumb ? "\r\n" : "\n");

        ఒకవేళ (writev(console_fd, iovec, n) < 0) {

                ఒకవేళ (errno == EIO && getpid_cached() == 1) {

                        /* If somebody tried to kick us from our console tty (via vhangup() or suchlike), try
                         * to reconnect. */

                        log_close_console();
                        (శూన్య) log_open_console();
                        ఒకవేళ (console_fd < 0)
                                తిరిగి 0;

                        ఒకవేళ (writev(console_fd, iovec, n) < 0)
                                తిరిగి -errno;
                } లేకపోతే
                        తిరిగి -errno;
        }

        తిరిగి 1;
}

స్థిర పూర్ణాంకం write_గమ్యం_syslog(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *బఫరు) {

        అక్షరం శీర్షిక_ప్రాధాన్యత[2 + DECIMAL_STR_MAX(పూర్ణాంకం) + 1],
             శీర్షిక_సమయం[64],
             శీర్షిక_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];
        నిర్మాణం tm tm;
        పూర్ణాంకం r;

        ఒకవేళ (syslog_fd < 0)
                తిరిగి 0;

        ఒకవేళ (LOG_PRI(level) > log_లక్ష్యం_max_level[LOG_లక్ష్యం_SYSLOG])
                తిరిగి 0;

        xsprintf(శీర్షిక_ప్రాధాన్యత, "<%i>", level);

        r = localtime_or_gmtime_usec(now(CLOCK_REALTIME), /* utc= */ false, &tm);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (strftime(శీర్షిక_సమయం, పరిమాణం(శీర్షిక_సమయం), "%h %e %T ", &tm) <= 0)
                తిరిగి -EINVAL;

        xsprintf(శీర్షిక_pid, "["PID_FMT"]: ", getpid_cached());

        నిర్మాణం iovec iovec[] = {
                IOVEC_MAKE_STRING(శీర్షిక_ప్రాధాన్యత),
                IOVEC_MAKE_STRING(శీర్షిక_సమయం),
                IOVEC_MAKE_STRING(కార్యక్రమం_invocation_short_name),
                IOVEC_MAKE_STRING(శీర్షిక_pid),
                IOVEC_MAKE_STRING(strempty(log_ఉపసర్గ)),
                IOVEC_MAKE_STRING(log_ఉపసర్గ ? ": " : ""),
                IOVEC_MAKE_STRING(బఫరు),
        };
        స్థిరరాశి నిర్మాణం msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = ELEMENTSOF(iovec),
        };

        /* When using syslog via SOCK_STREAM separate the messages by NUL chars */
        ఒకవేళ (syslog_is_ప్రవాహం)
                iovec[ELEMENTSOF(iovec) - 1].iov_len++;

        కొరకు (;;) {
                ssize_t n;

                n = sendmsg(syslog_fd, &msghdr, MSG_NOSIGNAL);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;

                ఒకవేళ (!syslog_is_ప్రవాహం)
                        ఆపు;

                ఒకవేళ (iovec_inc_many(iovec, ELEMENTSOF(iovec), n))
                        ఆపు;
        }

        తిరిగి 1;
}

స్థిర పూర్ణాంకం write_గమ్యం_kmsg(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *బఫరు) {

        /* Set a ratelimit on the amount of messages logged to /dev/kmsg. This is mostly supposed to be a
         * safety catch for the case where start indiscriminately logging in a loop. It will not catch cases
         * where we log excessively, but not in a tight loop.
         *
         * Note that this ratelimit is per-emitter, so we might still overwhelm /dev/kmsg with multiple
         * loggers.
         */
        స్థిర thread_local రేటు_హద్దు ratelimit = { 5 * USEC_చొప్పున_SEC, 200 };

        అక్షరం శీర్షిక_ప్రాధాన్యత[2 + DECIMAL_STR_MAX(పూర్ణాంకం) + 1],
             శీర్షిక_pid[4 + DECIMAL_STR_MAX(pid_t) + 1];

        ఒకవేళ (kmsg_fd < 0)
                తిరిగి 0;

        ఒకవేళ (LOG_PRI(level) > log_లక్ష్యం_max_level[LOG_లక్ష్యం_KMSG])
                తిరిగి 0;

        ఒకవేళ (ratelimit_kmsg && !ratelimit_క్రింద(&ratelimit)) {
                ఒకవేళ (ratelimit_అంకె_dropped(&ratelimit) > 1)
                        తిరిగి 0;

                బఫరు = "Too many messages being logged to kmsg, ignoring";
        }

        xsprintf(శీర్షిక_ప్రాధాన్యత, "<%i>", level);
        xsprintf(శీర్షిక_pid, "["PID_FMT"]: ", getpid_cached());

        స్థిరరాశి నిర్మాణం iovec iovec[] = {
                IOVEC_MAKE_STRING(శీర్షిక_ప్రాధాన్యత),
                IOVEC_MAKE_STRING(కార్యక్రమం_invocation_short_name),
                IOVEC_MAKE_STRING(శీర్షిక_pid),
                IOVEC_MAKE_STRING(strempty(log_ఉపసర్గ)),
                IOVEC_MAKE_STRING(log_ఉపసర్గ ? ": " : ""),
                IOVEC_MAKE_STRING(బఫరు),
                IOVEC_MAKE_STRING("\n"),
        };

        ఒకవేళ (writev(kmsg_fd, iovec, ELEMENTSOF(iovec)) < 0)
                తిరిగి -errno;

        తిరిగి 1;
}

స్థిర పూర్ణాంకం log_do_శీర్షిక(
                అక్షరం *శీర్షిక,
                size_t కొలత,
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం, స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం, స్థిరరాశి అక్షరం *extra) {
        పూర్ణాంకం r;

        error = IS_SYNTHETIC_ERRNO(error) ? 0 : ERRNO_విలువ(error);

        r = snprintf(శీర్షిక, కొలత,
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
                     పంక్తి ? "CODE_LINE=" : "",
                     పంక్తి ? 1 : 0, పంక్తి, /* %.0d means no output too, special case for 0 */
                     పంక్తి ? "\n" : "",
                     isempty(func) ? "" : "CODE_FUNC=",
                     isempty(func) ? "" : func,
                     isempty(func) ? "" : "\n",
                     error ? "ERRNO=" : "",
                     error ? 1 : 0, error,
                     error ? "\n" : "",
                     isempty(object) ? "" : ASSERT_PTR(object_క్షేత్రం),
                     isempty(object) ? "" : object,
                     isempty(object) ? "" : "\n",
                     isempty(extra) ? "" : ASSERT_PTR(extra_క్షేత్రం),
                     isempty(extra) ? "" : extra,
                     isempty(extra) ? "" : "\n",
                     కార్యక్రమం_invocation_short_name);
        assert_raw((size_t) r < కొలత);

        తిరిగి 0;
}

స్థిర శూన్య log_do_context(నిర్మాణం iovec *iovec, size_t iovec_len, size_t *n) {
        ప్రతిజ్ఞా(iovec);
        ప్రతిజ్ఞా(n);

        LIST_FOREACH(ll, c, log_context_తల()) {
                STRV_FOREACH(s, c->fields) {
                        ఒకవేళ (*n + 2 >= iovec_len)
                                తిరిగి;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(*s);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                కొరకు (size_t i = 0; i < c->n_ఆదానం_iovec; i++) {
                        ఒకవేళ (*n + 2 >= iovec_len)
                                తిరిగి;

                        iovec[(*n)++] = c->ఆదానం_iovec[i];
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }

                ఒకవేళ (c->కీలకం && c->విలువ) {
                        ఒకవేళ (*n + 3 >= iovec_len)
                                తిరిగి;

                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->కీలకం);
                        iovec[(*n)++] = IOVEC_MAKE_STRING(c->విలువ);
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");
                }
        }
}

స్థిర పూర్ణాంకం write_గమ్యం_journal(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                స్థిరరాశి అక్షరం *బఫరు) {

        అక్షరం శీర్షిక[LINE_MAX];
        size_t n = 0, iovec_len;
        నిర్మాణం iovec *iovec;

        ఒకవేళ (journal_fd < 0)
                తిరిగి 0;

        ఒకవేళ (LOG_PRI(level) > log_లక్ష్యం_max_level[LOG_లక్ష్యం_JOURNAL])
                తిరిగి 0;

        ఒకవేళ (!MUL_సురక్షిత(&iovec_len, log_context_అంకె_fields_2(), 3) ||
            !కూడు_సురక్షిత(&iovec_len, iovec_len, 6))
                iovec_len = IOVEC_MAX;
        లేకపోతే
                iovec_len = MIN(iovec_len, IOVEC_MAX);
        iovec = newa(నిర్మాణం iovec, iovec_len);

        log_do_శీర్షిక(శీర్షిక, పరిమాణం(శీర్షిక), level, error, file, పంక్తి, func, object_క్షేత్రం, object, extra_క్షేత్రం, extra);

        iovec[n++] = IOVEC_MAKE_STRING(శీర్షిక);
        iovec[n++] = IOVEC_MAKE_STRING("MESSAGE=");
        ఒకవేళ (log_ఉపసర్గ) {
                iovec[n++] = IOVEC_MAKE_STRING(log_ఉపసర్గ);
                iovec[n++] = IOVEC_MAKE_STRING(": ");
        }
        iovec[n++] = IOVEC_MAKE_STRING(బఫరు);
        iovec[n++] = IOVEC_MAKE_STRING("\n");

        log_do_context(iovec, iovec_len, &n);

        స్థిరరాశి నిర్మాణం msghdr msghdr = {
                .msg_iov = iovec,
                .msg_iovlen = n,
        };

        ఒకవేళ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) < 0)
                తిరిగి -errno;

        తిరిగి 1;
}

పూర్ణాంకం log_dispatch_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                అక్షరం *బఫరు) {

        assert_raw(బఫరు);

        ఒకవేళ (log_లక్ష్యం == LOG_లక్ష్యం_NULL)
                తిరిగి -ERRNO_విలువ(error);

        /* Patch in LOG_DAEMON facility if necessary */
        ఒకవేళ (LOG_FAC(level) == 0)
                level |= log_facility;

        ఒకవేళ (open_when_needed)
                (శూన్య) log_open();

        చేయి {
                అక్షరం *e;
                పూర్ణాంకం k = 0;

                బఫరు += strspn(బఫరు, NEWLINE);

                ఒకవేళ (బఫరు[0] == 0)
                        ఆపు;

                ఒకవేళ ((e = strpbrk(బఫరు, NEWLINE)))
                        *(e++) = 0;

                ఒకవేళ (లోపల_సమితి(log_లక్ష్యం, LOG_లక్ష్యం_AUTO,
                                       LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                                       LOG_లక్ష్యం_JOURNAL)) {

                        k = write_గమ్యం_journal(level, error, file, పంక్తి, func, object_క్షేత్రం, object, extra_క్షేత్రం, extra, బఫరు);
                        ఒకవేళ (k < 0 && k != -EAGAIN)
                                log_close_journal();
                }

                ఒకవేళ (లోపల_సమితి(log_లక్ష్యం, LOG_లక్ష్యం_SYSLOG_OR_KMSG,
                                       LOG_లక్ష్యం_SYSLOG)) {

                        k = write_గమ్యం_syslog(level, error, file, పంక్తి, func, బఫరు);
                        ఒకవేళ (k < 0 && k != -EAGAIN)
                                log_close_syslog();
                }

                ఒకవేళ (k <= 0 &&
                    లోపల_సమితి(log_లక్ష్యం, LOG_లక్ష్యం_AUTO,
                                       LOG_లక్ష్యం_SYSLOG_OR_KMSG,
                                       LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                                       LOG_లక్ష్యం_KMSG)) {

                        ఒకవేళ (k < 0)
                                log_open_kmsg();

                        k = write_గమ్యం_kmsg(level, error, file, పంక్తి, func, బఫరు);
                        ఒకవేళ (k < 0) {
                                log_close_kmsg();
                                (శూన్య) log_open_console();
                        }
                }

                ఒకవేళ (k <= 0)
                        (శూన్య) write_గమ్యం_console(level, error, file, పంక్తి, func, బఫరు);

                బఫరు = e;
        } అయితే (బఫరు);

        ఒకవేళ (open_when_needed)
                log_close();

        తిరిగి -ERRNO_విలువ(error);
}

పూర్ణాంకం log_dump_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                అక్షరం *బఫరు) {

        PROTECT_ERRNO;

        /* This modifies the buffer... */

        ఒకవేళ (_likely_(LOG_PRI(level) > log_max_level))
                తిరిగి -ERRNO_విలువ(error);

        తిరిగి log_dispatch_అంతర్గత(level, error, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, బఫరు);
}

పూర్ణాంకం log_internalv(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format,
                va_list ap) {

        ఒకవేళ (_likely_(LOG_PRI(level) > log_max_level))
                తిరిగి -ERRNO_విలువ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        అక్షరం బఫరు[LINE_MAX];
        LOCAL_ERRNO(ERRNO_విలువ(error));

        (శూన్య) vsnprintf(బఫరు, పరిమాణం బఫరు, format, ap);

        తిరిగి log_dispatch_అంతర్గత(level, error, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, బఫరు);
}

పూర్ణాంకం log_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format, ...) {

        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = log_internalv(level, error, file, పంక్తి, func, format, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం log_object_internalv(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                స్థిరరాశి అక్షరం *format,
                va_list ap) {

        అక్షరం *బఫరు, *b;

        ఒకవేళ (_likely_(LOG_PRI(level) > log_max_level))
                తిరిగి -ERRNO_విలువ(error);

        /* Make sure that %m maps to the specified error (or "Success"). */
        LOCAL_ERRNO(ERRNO_విలువ(error));

        LOG_సమితి_ఉపసర్గ(object);

        b = బఫరు = newa(అక్షరం, LINE_MAX);
        (శూన్య) vsnprintf(b, LINE_MAX, format, ap);

        తిరిగి log_dispatch_అంతర్గత(level, error, file, పంక్తి, func,
                                     object_క్షేత్రం, object, extra_క్షేత్రం, extra, బఫరు);
}

పూర్ణాంకం log_object_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                స్థిరరాశి అక్షరం *format, ...) {

        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = log_object_internalv(level, error, file, పంక్తి, func, object_క్షేత్రం, object, extra_క్షేత్రం, extra, format, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం log_oom_అంతర్గత(పూర్ణాంకం level, స్థిరరాశి అక్షరం *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *func) {
        తిరిగి log_అంతర్గత(level, ENOMEM, file, పంక్తి, func, "Out of memory.");
}

పూర్ణాంకం log_format_iovec(
                నిర్మాణం iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *format,
                va_list ap) {

        ప్రతిజ్ఞా(iovec);
        ప్రతిజ్ఞా(n);

        అయితే (format && *n + 1 < iovec_len) {
                va_list aq;
                అక్షరం *m;
                పూర్ణాంకం r;

                /* We need to copy the va_list structure,
                 * since vasprintf() leaves it afterwards at
                 * an undefined location */

                errno = ERRNO_విలువ(error);

                va_copy(aq, ap);
                r = vasprintf(&m, format, aq);
                va_end(aq);
                ఒకవేళ (r < 0)
                        తిరిగి -EINVAL;

                /* Now, jump enough ahead, so that we point to
                 * the next format string */
                VA_FORMAT_ADVANCE(format, ap);

                iovec[(*n)++] = IOVEC_MAKE_STRING(m);
                ఒకవేళ (newline_separator)
                        iovec[(*n)++] = IOVEC_MAKE_STRING("\n");

                format = va_arg(ap, అక్షరం *);
        }
        తిరిగి 0;
}

పూర్ణాంకం log_struct_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format, ...) {

        అక్షరం buf[LINE_MAX];
        bool దొరికింది = false;
        PROTECT_ERRNO;
        va_list ap;

        ఒకవేళ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_లక్ష్యం == LOG_లక్ష్యం_NULL)
                తిరిగి -ERRNO_విలువ(error);

        ఒకవేళ (LOG_FAC(level) == 0)
                level |= log_facility;

        ఒకవేళ (లోపల_సమితి(log_లక్ష్యం,
                   LOG_లక్ష్యం_AUTO,
                   LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                   LOG_లక్ష్యం_JOURNAL)) {

                ఒకవేళ (open_when_needed)
                        log_open_journal();

                ఒకవేళ (journal_fd >= 0) {
                        అక్షరం శీర్షిక[LINE_MAX];
                        నిర్మాణం iovec *iovec;
                        size_t n = 0, m, iovec_len;
                        పూర్ణాంకం r;
                        bool fallback = false;

                        ఒకవేళ (!MUL_సురక్షిత(&iovec_len, log_context_అంకె_fields_2(), 3) ||
                            !కూడు_సురక్షిత(&iovec_len, iovec_len, 17))
                                iovec_len = IOVEC_MAX;
                        లేకపోతే
                                iovec_len = MIN(iovec_len, IOVEC_MAX);
                        iovec = newa(నిర్మాణం iovec, iovec_len);

                        /* If the journal is available do structured logging.
                         * Do not report the errno if it is synthetic. */
                        log_do_శీర్షిక(శీర్షిక, పరిమాణం(శీర్షిక), level, error, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
                        iovec[n++] = IOVEC_MAKE_STRING(శీర్షిక);

                        va_start(ap, format);
                        DISABLE_WARNING_FORMAT_NONLITERAL;
                        r = log_format_iovec(iovec, iovec_len, &n, true, error, format, ap);
                        REENABLE_WARNING;
                        m = n;
                        ఒకవేళ (r < 0)
                                fallback = true;
                        లేకపోతే {
                                log_do_context(iovec, iovec_len, &n);

                                స్థిరరాశి నిర్మాణం msghdr msghdr = {
                                        .msg_iov = iovec,
                                        .msg_iovlen = n,
                                };

                                (శూన్య) sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL);
                        }

                        va_end(ap);
                        కొరకు (size_t i = 1; i < m; i += 2)
                                విడుదల(iovec[i].iov_base);

                        ఒకవేళ (!fallback) {
                                ఒకవేళ (open_when_needed)
                                        log_close();

                                తిరిగి -ERRNO_విలువ(error);
                        }
                }
        }

        /* Fallback if journal logging is not available or didn't work. */

        va_start(ap, format);
        అయితే (format) {
                va_list aq;

                errno = ERRNO_విలువ(error);

                va_copy(aq, ap);
                DISABLE_WARNING_FORMAT_NONLITERAL;
                (శూన్య) vsnprintf(buf, పరిమాణం buf, format, aq);
                REENABLE_WARNING;
                va_end(aq);

                ఒకవేళ (startswith(buf, "MESSAGE=")) {
                        దొరికింది = true;
                        ఆపు;
                }

                VA_FORMAT_ADVANCE(format, ap);

                format = va_arg(ap, అక్షరం *);
        }
        va_end(ap);

        ఒకవేళ (!దొరికింది) {
                ఒకవేళ (open_when_needed)
                        log_close();

                తిరిగి -ERRNO_విలువ(error);
        }

        తిరిగి log_dispatch_అంతర్గత(level, error, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, buf + 8);
}

పూర్ణాంకం log_struct_iovec_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి నిర్మాణం iovec ఆదానం_iovec[],
                size_t n_ఆదానం_iovec) {

        PROTECT_ERRNO;

        ఒకవేళ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_లక్ష్యం == LOG_లక్ష్యం_NULL)
                తిరిగి -ERRNO_విలువ(error);

        ఒకవేళ (LOG_FAC(level) == 0)
                level |= log_facility;

        ఒకవేళ (లోపల_సమితి(log_లక్ష్యం, LOG_లక్ష్యం_AUTO,
                               LOG_లక్ష్యం_JOURNAL_OR_KMSG,
                               LOG_లక్ష్యం_JOURNAL) &&
            journal_fd >= 0) {

                అక్షరం శీర్షిక[LINE_MAX];
                నిర్మాణం iovec *iovec;
                size_t n = 0, iovec_len, n_context_iovec, n_ఆదానం_iovec_expanded;

                ఒకవేళ (!MUL_సురక్షిత(&n_ఆదానం_iovec_expanded, n_ఆదానం_iovec, 2) ||
                    !MUL_సురక్షిత(&n_context_iovec, log_context_అంకె_fields_2(), 3) ||
                    !కూడు_సురక్షిత(&iovec_len, 1, n_ఆదానం_iovec_expanded) ||
                    !కూడు_సురక్షిత(&iovec_len, iovec_len, n_context_iovec))
                        iovec_len = IOVEC_MAX;
                లేకపోతే
                        iovec_len = MIN(iovec_len, IOVEC_MAX);
                iovec = newa(నిర్మాణం iovec, iovec_len);

                log_do_శీర్షిక(శీర్షిక, పరిమాణం(శీర్షిక), level, error, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);

                iovec[n++] = IOVEC_MAKE_STRING(శీర్షిక);
                కొరకు (size_t i = 0; i < n_ఆదానం_iovec && n + 2 <= iovec_len; i++) {
                        iovec[n++] = ఆదానం_iovec[i];
                        iovec[n++] = IOVEC_MAKE_STRING("\n");
                }

                log_do_context(iovec, iovec_len, &n);

                స్థిరరాశి నిర్మాణం msghdr msghdr = {
                        .msg_iov = iovec,
                        .msg_iovlen = n,
                };

                ఒకవేళ (sendmsg(journal_fd, &msghdr, MSG_NOSIGNAL) >= 0)
                        తిరిగి -ERRNO_విలువ(error);
        }

        కొరకు (size_t i = 0; i < n_ఆదానం_iovec; i++)
                ఒకవేళ (జ్ఞాపకం_startswith(ఆదానం_iovec[i].iov_base, ఆదానం_iovec[i].iov_len, "MESSAGE=")) {
                        అక్షరం *m;

                        m = strndupa_సురక్షిత((అక్షరం*) ఆదానం_iovec[i].iov_base + STRLEN("MESSAGE="),
                                          ఆదానం_iovec[i].iov_len - STRLEN("MESSAGE="));

                        తిరిగి log_dispatch_అంతర్గత(level, error, file, పంక్తి, func, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, m);
                }

        /* Couldn't find MESSAGE=. */
        తిరిగి -ERRNO_విలువ(error);
}

పూర్ణాంకం log_సమితి_లక్ష్యం_from_string(స్థిరరాశి అక్షరం *e) {
        Log_లక్ష్యం t;

        t = log_లక్ష్యం_from_string(e);
        ఒకవేళ (t < 0)
                తిరిగి t;

        log_సమితి_లక్ష్యం(t);
        తిరిగి 0;
}

పూర్ణాంకం log_సమితి_max_level_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *మాట = ఖాళీ, *ఉపసర్గ = ఖాళీ;
                Log_లక్ష్యం లక్ష్యం;
                స్థిరరాశి అక్షరం *colon;

                r = వెలికితీయి_మొదటి_మాట(&e, &మాట, ",", 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                colon = strchr(మాట, ':');
                ఒకవేళ (!colon) {
                        r = log_level_from_string(మాట);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        log_సమితి_max_level(r);
                        కొనసాగు;
                }

                ఉపసర్గ = strndup(మాట, colon - మాట);
                ఒకవేళ (!ఉపసర్గ)
                        తిరిగి -ENOMEM;

                లక్ష్యం = log_లక్ష్యం_from_string(ఉపసర్గ);
                ఒకవేళ (లక్ష్యం < 0)
                        తిరిగి లక్ష్యం;

                ఒకవేళ (లక్ష్యం >= LOG_లక్ష్యం_SINGLE_MAX)
                        తిరిగి -EINVAL;

                r = log_level_from_string(colon + 1);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                log_లక్ష్యం_max_level[లక్ష్యం] = r;
        }

        తిరిగి 0;
}

పూర్ణాంకం log_max_స్థాయిలు_గమ్యం_string(పూర్ణాంకం level, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = log_level_గమ్యం_string_alloc(level, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (Log_లక్ష్యం లక్ష్యం = 0; లక్ష్యం < LOG_లక్ష్యం_SINGLE_MAX; లక్ష్యం++) {
                _cleanup_free_ అక్షరం *l = ఖాళీ;

                ఒకవేళ (log_లక్ష్యం_max_level[లక్ష్యం] == గరిష్ఠ_పూర్ణాంకం)
                        కొనసాగు;

                r = log_level_గమ్యం_string_alloc(log_లక్ష్యం_max_level[లక్ష్యం], &l);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                r = strextendf_with_separator(&s, ",", "%s:%s", log_లక్ష్యం_గమ్యం_string(లక్ష్యం), l);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం log_సమితి_ratelimit_kmsg_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        r = parse_boolean(e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ratelimit_kmsg = r;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం parse_proc_cmdline_అంశం(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ, శూన్య *దత్తాంశం) {

        /*
         * The systemd.log_xyz= settings are parsed by all tools, and
         * so is "debug".
         *
         * However, "quiet" is only parsed by PID 1, and only turns of
         * status output to /dev/console, but does not alter the log
         * level.
         */

        ఒకవేళ (streq(కీలకం, "debug") && !విలువ)
                log_సమితి_max_level(LOG_DEBUG);

        లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_target")) {

                ఒకవేళ (proc_cmdline_విలువ_missing(కీలకం, విలువ))
                        తిరిగి 0;

                ఒకవేళ (log_సమితి_లక్ష్యం_from_string(విలువ) < 0)
                        log_warning("Failed to parse log target '%s', ignoring.", విలువ);

        } లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_level")) {

                ఒకవేళ (proc_cmdline_విలువ_missing(కీలకం, విలువ))
                        తిరిగి 0;

                ఒకవేళ (log_సమితి_max_level_from_string(విలువ) < 0)
                        log_warning("Failed to parse log level setting '%s', ignoring.", విలువ);

        } లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_color")) {

                ఒకవేళ (log_show_color_from_string(విలువ ?: "1") < 0)
                        log_warning("Failed to parse log color setting '%s', ignoring.", విలువ);

        } లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_location")) {

                ఒకవేళ (log_show_location_from_string(విలువ ?: "1") < 0)
                        log_warning("Failed to parse log location setting '%s', ignoring.", విలువ);

        } లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_tid")) {

                ఒకవేళ (log_show_tid_from_string(విలువ ?: "1") < 0)
                        log_warning("Failed to parse log tid setting '%s', ignoring.", విలువ);

        } లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_time")) {

                ఒకవేళ (log_show_సమయం_from_string(విలువ ?: "1") < 0)
                        log_warning("Failed to parse log time setting '%s', ignoring.", విలువ);

        } లేకపోతే ఒకవేళ (proc_cmdline_కీలకం_streq(కీలకం, "systemd.log_ratelimit_kmsg")) {

                ఒకవేళ (log_సమితి_ratelimit_kmsg_from_string(విలువ ?: "1") < 0)
                        log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", విలువ);
        }

        తిరిగి 0;
}

స్థిర bool should_parse_proc_cmdline(శూన్య) {
        /* PID1 always reads the kernel command line. */
        ఒకవేళ (getpid_cached() == 1)
                తిరిగి true;

        /* Otherwise, parse the command line if invoked directly by systemd. */
        తిరిగి invoked_by_systemd();
}

శూన్య log_parse_environment_variables(శూన్య) {
        స్థిరరాశి అక్షరం *e;
        పూర్ణాంకం r;

        e = getenv("SYSTEMD_LOG_TARGET");
        ఒకవేళ (e && log_సమితి_లక్ష్యం_from_string(e) < 0)
                log_warning("Failed to parse log target '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LEVEL");
        ఒకవేళ (e) {
                r = log_సమితి_max_level_from_string(e);
                ఒకవేళ (r < 0)
                        log_warning_errno(r, "Failed to parse log level '%s', ignoring: %m", e);
        } లేకపోతే {
                /* If no explicit log level is specified then let's see if this is a debug invocation, and if
                 * so raise the log level to debug too. Note that this is not symmetric: just because
                 * DEBUG_INVOCATION is explicitly set to 0 we won't lower the log level below debug. This
                 * follows the logic that debug logging is an opt-in thing anyway, and if there's any reason
                 * to enable it we should not disable it here automatically. */
                r = getenv_bool("DEBUG_INVOCATION");
                ఒకవేళ (r < 0 && r != -ENXIO)
                        log_warning_errno(r, "Failed to parse $DEBUG_INVOCATION value, ignoring: %m");
                లేకపోతే ఒకవేళ (r > 0)
                        log_సమితి_max_level(LOG_DEBUG);
        }

        e = getenv("SYSTEMD_LOG_COLOR");
        ఒకవేళ (e && log_show_color_from_string(e) < 0)
                log_warning("Failed to parse log color '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_LOCATION");
        ఒకవేళ (e && log_show_location_from_string(e) < 0)
                log_warning("Failed to parse log location '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TIME");
        ఒకవేళ (e && log_show_సమయం_from_string(e) < 0)
                log_warning("Failed to parse log time '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_TID");
        ఒకవేళ (e && log_show_tid_from_string(e) < 0)
                log_warning("Failed to parse log tid '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_RATELIMIT_KMSG");
        ఒకవేళ (e && log_సమితి_ratelimit_kmsg_from_string(e) < 0)
                log_warning("Failed to parse log ratelimit kmsg boolean '%s', ignoring.", e);

        e = getenv("SYSTEMD_LOG_EXECUTABLE");
        ఒకవేళ (e && log_show_executable_from_string(e) < 0)
                log_warning("Failed to parse log executable '%s', ignoring.", e);
}

శూన్య log_parse_environment(శూన్య) {
        /* Do not call from library code. */

        ఒకవేళ (should_parse_proc_cmdline())
                (శూన్య) proc_cmdline_parse(parse_proc_cmdline_అంశం, ఖాళీ, PROC_CMDLINE_STRIP_RD_ఉపసర్గ);

        log_parse_environment_variables();
}

Log_లక్ష్యం log_get_లక్ష్యం(శూన్య) {
        తిరిగి log_లక్ష్యం;
}

శూన్య log_settle_లక్ష్యం(శూన్య) {

        /* If we're using LOG_TARGET_AUTO and opening the log again on every single log call, we'll check if
         * stderr is attached to the journal every single log call. However, if we then close all file
         * descriptors later, that will stop working because stderr will be closed as well. To avoid that
         * problem, this function is used to permanently change the log target depending on whether stderr is
         * connected to the journal or not. */

        Log_లక్ష్యం t = log_get_లక్ష్యం();

        ఒకవేళ (t != LOG_లక్ష్యం_AUTO)
                తిరిగి;

        t = getpid_cached() == 1 || stderr_is_journal() ? (prohibit_ipc ? LOG_లక్ష్యం_KMSG : LOG_లక్ష్యం_JOURNAL_OR_KMSG)
                                                        : LOG_లక్ష్యం_CONSOLE;
        log_సమితి_లక్ష్యం(t);
}

పూర్ణాంకం log_get_max_level(శూన్య) {
        తిరిగి log_max_level;
}

పూర్ణాంకం log_get_లక్ష్యం_max_level(Log_లక్ష్యం లక్ష్యం) {
        ప్రతిజ్ఞా(లక్ష్యం >= 0);
        ప్రతిజ్ఞా(లక్ష్యం < LOG_లక్ష్యం_SINGLE_MAX);
        తిరిగి log_లక్ష్యం_max_level[లక్ష్యం];
}

శూన్య log_show_color(bool b) {
        show_color = b;
}

bool log_get_show_color(శూన్య) {
        తిరిగి show_color > 0; /* Defaults to false. */
}

శూన్య log_show_location(bool b) {
        show_location = b;
}

bool log_get_show_location(శూన్య) {
        తిరిగి show_location;
}

శూన్య log_show_సమయం(bool b) {
        show_సమయం = b;
}

bool log_get_show_సమయం(శూన్య) {
        తిరిగి show_సమయం;
}

శూన్య log_show_tid(bool b) {
        show_tid = b;
}

bool log_get_show_tid(శూన్య) {
        తిరిగి show_tid;
}

శూన్య log_show_executable(bool b) {
        show_executable = b;
}

bool log_get_show_executable(శూన్య) {
        తిరిగి show_executable;
}

పూర్ణాంకం log_show_color_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        r = parse_boolean(e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_show_color(r);
        తిరిగి 0;
}

పూర్ణాంకం log_show_location_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        r = parse_boolean(e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_show_location(r);
        తిరిగి 0;
}

పూర్ణాంకం log_show_సమయం_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        r = parse_boolean(e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_show_సమయం(r);
        తిరిగి 0;
}

పూర్ణాంకం log_show_tid_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        r = parse_boolean(e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_show_tid(r);
        తిరిగి 0;
}

పూర్ణాంకం log_show_executable_from_string(స్థిరరాశి అక్షరం *e) {
        పూర్ణాంకం r;

        r = parse_boolean(e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        log_show_executable(r);
        తిరిగి 0;
}

bool log_on_console(శూన్య) {
        ఒకవేళ (లోపల_సమితి(log_లక్ష్యం, LOG_లక్ష్యం_CONSOLE,
                               LOG_లక్ష్యం_CONSOLE_PREFIXED))
                తిరిగి true;

        తిరిగి syslog_fd < 0 && kmsg_fd < 0 && journal_fd < 0;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి log_లక్ష్యం_పట్టిక[LOG_లక్ష్యం_MAX] = {
        [LOG_లక్ష్యం_CONSOLE]          = "console",
        [LOG_లక్ష్యం_CONSOLE_PREFIXED] = "console-prefixed",
        [LOG_లక్ష్యం_KMSG]             = "kmsg",
        [LOG_లక్ష్యం_JOURNAL]          = "journal",
        [LOG_లక్ష్యం_JOURNAL_OR_KMSG]  = "journal-or-kmsg",
        [LOG_లక్ష్యం_SYSLOG]           = "syslog",
        [LOG_లక్ష్యం_SYSLOG_OR_KMSG]   = "syslog-or-kmsg",
        [LOG_లక్ష్యం_AUTO]             = "auto",
        [LOG_లక్ష్యం_NULL]             = "null",
};

DEFINE_STRING_పట్టిక_LOOKUP(log_లక్ష్యం, Log_లక్ష్యం);

శూన్య log_received_signal(పూర్ణాంకం level, స్థిరరాశి నిర్మాణం signalfd_siginfo *si) {
        ప్రతిజ్ఞా(si);

        ఒకవేళ (si_సంకేతం_from_process(si->ssi_code) && pid_is_valid(si->ssi_pid)) {
                _cleanup_free_ అక్షరం *p = ఖాళీ;

                (శూన్య) pid_get_comm(si->ssi_pid, &p);

                log_నిండిన(level,
                         "Received SIG%s from PID %"PRIu32" (%s).",
                         signal_గమ్యం_string(si->ssi_signo),
                         si->ssi_pid, strna(p));
        } లేకపోతే
                log_నిండిన(level,
                         "Received SIG%s.",
                         signal_గమ్యం_string(si->ssi_signo));
}

శూన్య సమితి_log_syntax_callback(log_syntax_callback_t cb, శూన్య *userdata) {
        ప్రతిజ్ఞా(!log_syntax_callback || !cb);
        ప్రతిజ్ఞా(!log_syntax_callback_userdata || !userdata);

        log_syntax_callback = cb;
        log_syntax_callback_userdata = userdata;
}

పూర్ణాంకం log_syntax_అంతర్గత(
                స్థిరరాశి అక్షరం *unit,
                పూర్ణాంకం level,
                స్థిరరాశి అక్షరం *config_file,
                గుర్తులేని config_పంక్తి,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format, ...) {

        PROTECT_ERRNO;

        ఒకవేళ (log_syntax_callback)
                log_syntax_callback(unit, level, log_syntax_callback_userdata);

        ఒకవేళ (_likely_(LOG_PRI(level) > log_max_level) ||
            log_లక్ష్యం == LOG_లక్ష్యం_NULL)
                తిరిగి -ERRNO_విలువ(error);

        అక్షరం బఫరు[LINE_MAX];
        va_list ap;
        స్థిరరాశి అక్షరం *unit_fmt = ఖాళీ;

        errno = ERRNO_విలువ(error);

        va_start(ap, format);
        (శూన్య) vsnprintf(బఫరు, పరిమాణం బఫరు, format, ap);
        va_end(ap);

        ఒకవేళ (unit)
                unit_fmt = getpid_cached() == 1 ? "UNIT=%s" : "USER_UNIT=%s";

        ఒకవేళ (config_file) {
                ఒకవేళ (config_పంక్తి > 0)
                        తిరిగి log_struct_అంతర్గత(
                                        level,
                                        error,
                                        file, పంక్తి, func,
                                        LOG_సందేశం_ID(SD_సందేశం_INVALID_CONFIGURATION_STR),
                                        LOG_అంశం("CONFIG_FILE=%s", config_file),
                                        LOG_అంశం("CONFIG_LINE=%u", config_పంక్తి),
                                        LOG_సందేశం("%s:%u: %s", config_file, config_పంక్తి, బఫరు),
                                        unit_fmt, unit,
                                        ఖాళీ);
                లేకపోతే
                        తిరిగి log_struct_అంతర్గత(
                                        level,
                                        error,
                                        file, పంక్తి, func,
                                        LOG_సందేశం_ID(SD_సందేశం_INVALID_CONFIGURATION_STR),
                                        LOG_అంశం("CONFIG_FILE=%s", config_file),
                                        LOG_సందేశం("%s: %s", config_file, బఫరు),
                                        unit_fmt, unit,
                                        ఖాళీ);
        } లేకపోతే ఒకవేళ (unit)
                తిరిగి log_struct_అంతర్గత(
                                level,
                                error,
                                file, పంక్తి, func,
                                LOG_సందేశం_ID(SD_సందేశం_INVALID_CONFIGURATION_STR),
                                LOG_సందేశం("%s: %s", unit, బఫరు),
                                unit_fmt, unit,
                                ఖాళీ);
        లేకపోతే
                తిరిగి log_struct_అంతర్గత(
                                level,
                                error,
                                file, పంక్తి, func,
                                LOG_సందేశం_ID(SD_సందేశం_INVALID_CONFIGURATION_STR),
                                LOG_సందేశం("%s", బఫరు),
                                ఖాళీ);
}

పూర్ణాంకం log_syntax_invalid_utf8_అంతర్గత(
                స్థిరరాశి అక్షరం *unit,
                పూర్ణాంకం level,
                స్థిరరాశి అక్షరం *config_file,
                గుర్తులేని config_పంక్తి,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ అక్షరం *p = ఖాళీ;

        ఒకవేళ (rvalue)
                p = utf8_escape_invalid(rvalue);

        తిరిగి log_syntax_అంతర్గత(unit, level, config_file, config_పంక్తి,
                                   SYNTHETIC_ERRNO(EINVAL), file, పంక్తి, func,
                                   "String is not UTF-8 clean, ignoring assignment: %s", strna(p));
}

పూర్ణాంకం log_syntax_parse_error_అంతర్గత(
                స్థిరరాశి అక్షరం *unit,
                స్థిరరాశి అక్షరం *config_file,
                గుర్తులేని config_పంక్తి,
                పూర్ణాంకం error,
                bool critical,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *lvalue,
                స్థిరరాశి అక్షరం *rvalue) {

        PROTECT_ERRNO;
        _cleanup_free_ అక్షరం *escaped = ఖాళీ;

        /* OOM is always handled as critical. */
        ఒకవేళ (ERRNO_విలువ(error) == ENOMEM)
                తిరిగి log_oom_అంతర్గత(LOG_ERR, file, పంక్తి, func);

        ఒకవేళ (rvalue && !utf8_is_valid(rvalue)) {
                escaped = utf8_escape_invalid(rvalue);
                ఒకవేళ (!escaped)
                        rvalue = "(oom)";
                లేకపోతే
                        rvalue = " (escaped)";
        }

        log_syntax_అంతర్గత(unit, critical ? LOG_ERR : LOG_WARNING, config_file, config_పంక్తి, error,
                            file, పంక్తి, func,
                            "Failed to parse %s=%s%s%s%s%s",
                            strna(lvalue), strempty(escaped), strempty(rvalue),
                            critical ? "" : ", ignoring",
                            error == 0 ? "." : ": ",
                            error == 0 ? "" : STRERROR(error));

        తిరిగి critical ? -ERRNO_విలువ(error) : 0;
}

శూన్య log_సమితి_upgrade_syslog_గమ్యం_journal(bool b) {
        upgrade_syslog_గమ్యం_journal = b;

        /* Make the change effective immediately */
        ఒకవేళ (b) {
                ఒకవేళ (log_లక్ష్యం == LOG_లక్ష్యం_SYSLOG)
                        log_లక్ష్యం = LOG_లక్ష్యం_JOURNAL;
                లేకపోతే ఒకవేళ (log_లక్ష్యం == LOG_లక్ష్యం_SYSLOG_OR_KMSG)
                        log_లక్ష్యం = LOG_లక్ష్యం_JOURNAL_OR_KMSG;
        }
}

శూన్య log_సమితి_always_reopen_console(bool b) {
        always_reopen_console = b;
}

శూన్య log_సమితి_open_when_needed(bool b) {
        open_when_needed = b;
}

శూన్య log_సమితి_prohibit_ipc(bool b) {
        prohibit_ipc = b;
}

పూర్ణాంకం log_emergency_level(శూన్య) {
        /* Returns the log level to use for log_emergency() logging. We use LOG_EMERG only when we are PID 1, as only
         * then the system of the whole system is obviously affected. */

        తిరిగి getpid_cached() == 1 ? LOG_EMERG : LOG_ERR;
}

పూర్ణాంకం log_dup_console(శూన్య) {
        పూర్ణాంకం copy;

        /* Duplicate the fd we use for fd logging if it's < 3 and use the copy from now on. This call is useful
         * whenever we want to continue logging through the original fd, but want to rearrange stderr. */

        ఒకవేళ (console_fd < 0 || console_fd >= 3)
                తిరిగి 0;

        copy = fcntl(console_fd, F_DUPFD_CLOEXEC, 3);
        ఒకవేళ (copy < 0)
                తిరిగి -errno;

        console_fd = copy;
        తిరిగి 0;
}

శూన్య log_అమరిక(శూన్య) {
        log_సమితి_లక్ష్యం(LOG_లక్ష్యం_AUTO);
        log_parse_environment();
        (శూన్య) log_open();
        ఒకవేళ (log_on_console() && show_color < 0)
                log_show_color(true);
}

స్థిరరాశి అక్షరం* log_సమితి_ఉపసర్గ(స్థిరరాశి అక్షరం *ఉపసర్గ, bool force) {
        స్థిరరాశి అక్షరం *old = log_ఉపసర్గ;

        ఒకవేళ (ఉపసర్గ || force)
                log_ఉపసర్గ = ఉపసర్గ;

        తిరిగి old;
}
