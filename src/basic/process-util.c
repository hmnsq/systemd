/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/oom.h>
#అన్తర్భావ <pthread.h>
#అన్తర్భావ <spawn.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <sys/mman.h>
#అన్తర్భావ <sys/mount.h>
#అన్తర్భావ <sys/personality.h>
#అన్తర్భావ <sys/prctl.h>
#అన్తర్భావ <sys/wait.h>
#అన్తర్భావ <syslog.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>
#ఒకవేళ HAVE_VALGRIND_VALGRIND_H
#అన్తర్భావ <valgrind/valgrind.h>
#అయితే_ముగింపు

#అన్తర్భావ "sd-messages.h"

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "architecture.h"
#అన్తర్భావ "argv-util.h"
#అన్తర్భావ "bitfield.h"
#అన్తర్భావ "capability-util.h"
#అన్తర్భావ "cgroup-util.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "dlfcn-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "locale-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "mountpoint-util.h"
#అన్తర్భావ "namespace-util.h"
#అన్తర్భావ "nulstr-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidfd-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "raw-clone.h"
#అన్తర్భావ "rlimit-util.h"
#అన్తర్భావ "signal-util.h"
#అన్తర్భావ "socket-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#నిర్వచించు COMM_MAX_LEN 128

స్థిర పూర్ణాంకం get_process_state(pid_t pid) {
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        స్థిరరాశి అక్షరం *p;
        అక్షరం state;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        ఒకవేళ (pid == 0 || pid == getpid_cached())
                తిరిగి (గుర్తులేని అక్షరం) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_పంక్తి_file(p, &పంక్తి);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        p = strrchr(పంక్తి, ')');
        ఒకవేళ (!p)
                తిరిగి -EIO;

        p++;

        ఒకవేళ (అక్షరశ్రేణి_చదువు(p, " %c", &state) != 1)
                తిరిగి -EIO;

        తిరిగి (గుర్తులేని అక్షరం) state;
}

పూర్ణాంకం pid_get_comm(pid_t pid, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *escaped = ఖాళీ, *comm = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pid == 0 || pid == getpid_cached()) {
                comm = new0(అక్షరం, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                ఒకవేళ (!comm)
                        తిరిగి -ENOMEM;

                r = prctl_సురక్షిత(PR_GET_NAME, (గుర్తులేని దీర్ఘ) comm, 0, 0, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        } లేకపోతే {
                స్థిరరాశి అక్షరం *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_పంక్తి_file(p, &comm);
                ఒకవేళ (r == -ENOENT)
                        తిరిగి -ESRCH;
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        escaped = new(అక్షరం, COMM_MAX_LEN);
        ఒకవేళ (!escaped)
                తిరిగి -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        తిరిగి 0;
}

పూర్ణాంకం pidref_get_comm(స్థిరరాశి PidRef *pid, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *comm = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(comm);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_కొలత,
                ProcessCmdlineFlags flags,
                అక్షరం **ret,
                size_t *ret_కొలత) {

        _cleanup_free_ అక్షరం *t = ఖాళీ;
        స్థిరరాశి అక్షరం *p;
        size_t k;
        పూర్ణాంకం r;

        /* Retrieves a process' command line as a "sized nulstr", i.e. possibly without the last NUL, but
         * with a specified size.
         *
         * If PROCESS_CMDLINE_COMM_FALLBACK is specified in flags and the process has no command line set
         * (the case for kernel threads), or has a command line that resolves to the empty string, will
         * return the "comm" name of the process instead. This will use at most _SC_ARG_MAX bytes of input
         * data.
         *
         * Returns an error, 0 if output was read but is truncated, 1 otherwise.
         */

        p = procfs_file_alloca(pid, "cmdline");
        r = read_virtual_file(p, max_కొలత, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (k == 0) {
                ఒకవేళ (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        తిరిగి -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ అక్షరం *comm = ఖాళీ;

                r = pid_get_comm(pid, &comm);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                విడుదల(t);
                t = strjoin("[", comm, "]");
                ఒకవేళ (!t)
                        తిరిగి -ENOMEM;

                k = అక్షరశ్రేణి_పొడవు(t);
                r = k <= max_కొలత;
                ఒకవేళ (r == 0) /* truncation */
                        t[max_కొలత] = '\0';
        }

        ఒకవేళ (ret)
                *ret = TAKE_PTR(t);
        ఒకవేళ (ret_కొలత)
                *ret_కొలత = k;

        తిరిగి r;
}

పూర్ణాంకం pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        size_t k;
        అక్షరం *ans;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        /* Retrieve and format a command line. See above for discussion of retrieval options.
         *
         * There are two main formatting modes:
         *
         * - when PROCESS_CMDLINE_QUOTE is specified, output is quoted in C/Python style. If no shell special
         *   characters are present, this output can be copy-pasted into the terminal to execute. UTF-8
         *   output is assumed.
         *
         * - otherwise, a compact non-roundtrippable form is returned. Non-UTF8 bytes are replaced by �. The
         *   returned string is of the specified console width at most, abbreviated with an ellipsis.
         *
         * Returns -ESRCH if the process doesn't exist, and -ENOENT if the process has no command line (and
         * PROCESS_CMDLINE_COMM_FALLBACK is not specified). Returns 0 and sets *line otherwise. */

        పూర్ణాంకం నిండిన = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        ఒకవేళ (నిండిన < 0)
                తిరిగి నిండిన;

        ఒకవేళ (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_ఖాళీలు |
                        FLAGS_సమితి(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                ప్రతిజ్ఞా(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ అక్షరం **వాదనలు = ఖాళీ;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                వాదనలు = strv_parse_nulstr_నిండిన(t, k, /* drop_trailing_nuls= */ true);
                ఒకవేళ (!వాదనలు)
                        తిరిగి -ENOMEM;

                ans = quote_command_పంక్తి(వాదనలు, shflags);
                ఒకవేళ (!ans)
                        తిరిగి -ENOMEM;
        } లేకపోతే {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                కొరకు (size_t i = 0; i < k - 1; i++)
                        ఒకవేళ (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_బిట్టు = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_నిండిన(t, max_columns,
                                                eight_బిట్టు * XESCAPE_8_బిట్టు | !నిండిన * XESCAPE_FORCE_ELLIPSIS);
                ఒకవేళ (!ans)
                        తిరిగి -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        తిరిగి 0;
}

పూర్ణాంకం pidref_get_cmdline(స్థిరరాశి PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, అక్షరం ***ret) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        అక్షరం **వాదనలు;
        size_t k;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        ప్రతిజ్ఞా(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        ఒకవేళ (r < 0)
                తిరిగి r;

        వాదనలు = strv_parse_nulstr_నిండిన(t, k, /* drop_trailing_nuls= */ true);
        ఒకవేళ (!వాదనలు)
                తిరిగి -ENOMEM;

        *ret = వాదనలు;
        తిరిగి 0;
}

పూర్ణాంకం pidref_get_cmdline_strv(స్థిరరాశి PidRef *pid, ProcessCmdlineFlags flags, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **వాదనలు = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &వాదనలు);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(వాదనలు);

        తిరిగి 0;
}

పూర్ణాంకం procfs_get_stat_flags(స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని దీర్ఘ దీర్ఘ *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        r = read_one_పంక్తి_file(మార్గం, &పంక్తి);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Skip past the comm field */
        అక్షరం *q = strrchr(పంక్తి, ')');
        ఒకవేళ (!q)
                తిరిగి -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        కొరకు (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                ఒకవేళ (l < 1)
                        తిరిగి -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                ఒకవేళ (l < 1)
                        తిరిగి -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        ఒకవేళ (l < 1)
                తిరిగి -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        ఒకవేళ (l < 1)
                తిరిగి -EINVAL;
        q[l] = 0;

        తిరిగి సురక్షిత_atollu(q, ret);
}

పూర్ణాంకం pid_is_kernel_thread(pid_t pid) {
        పూర్ణాంకం r;

        ఒకవేళ (లోపల_సమితి(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                తిరిగి 0;
        ఒకవేళ (!pid_is_valid(pid))
                తిరిగి -EINVAL;

        స్థిరరాశి అక్షరం *p = procfs_file_alloca(pid, "stat");

        గుర్తులేని దీర్ఘ దీర్ఘ flags;
        r = procfs_get_stat_flags(p, &flags);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి !!(flags & PF_KTHREAD);
}

పూర్ణాంకం pidref_is_kernel_thread(స్థిరరాశి PidRef *pid) {
        పూర్ణాంకం ఫలితం, r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        ఫలితం = pid_is_kernel_thread(pid->pid);
        ఒకవేళ (ఫలితం < 0)
                తిరిగి ఫలితం;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం get_process_link_contents(pid_t pid, స్థిరరాశి అక్షరం *proc_file, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        తిరిగి (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

పూర్ణాంకం get_process_exe(pid_t pid, అక్షరం **ret) {
        అక్షరం *d;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret) {
                d = endswith(*ret, " (deleted)");
                ఒకవేళ (d)
                        *d = '\0';
        }

        తిరిగి 0;
}

పూర్ణాంకం pid_get_uid(pid_t pid, uid_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                తిరిగి 0;
        }

        _cleanup_free_ అక్షరం *v = ఖాళీ;
        r = procfs_file_get_క్షేత్రం(pid, "status", "Uid", &v);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_uid(v, ret);
}

పూర్ణాంకం pidref_get_uid(స్థిరరాశి PidRef *pid, uid_t *ret) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        ఒకవేళ (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                ఒకవేళ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        తిరిగి r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = uid;
        తిరిగి 0;
}

పూర్ణాంకం get_process_gid(pid_t pid, gid_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                తిరిగి 0;
        }

        _cleanup_free_ అక్షరం *v = ఖాళీ;
        r = procfs_file_get_క్షేత్రం(pid, "status", "Gid", &v);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_gid(v, ret);
}

పూర్ణాంకం get_process_cwd(pid_t pid, అక్షరం **ret) {
        ప్రతిజ్ఞా(pid >= 0);

        ఒకవేళ (pid == 0 || pid == getpid_cached())
                తిరిగి సురక్షిత_getcwd(ret);

        తిరిగి get_process_link_contents(pid, "cwd", ret);
}

పూర్ణాంకం get_process_మూలం(pid_t pid, అక్షరం **ret) {
        ప్రతిజ్ఞా(pid >= 0);
        తిరిగి get_process_link_contents(pid, "root", ret);
}

#నిర్వచించు ENVIRONMENT_ఖండం_MAX (5U*1024U*1024U)

పూర్ణాంకం get_process_environ(pid_t pid, అక్షరం **ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_free_ అక్షరం *outcome = ఖాళీ;
        size_t sz = 0;
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                అక్షరం c;

                ఒకవేళ (sz >= ENVIRONMENT_ఖండం_MAX)
                        తిరిగి -ENOBUFS;

                ఒకవేళ (!GREEDY_REALLOC(outcome, sz + 5))
                        తిరిగి -ENOMEM;

                r = సురక్షిత_fgetc(f, &c);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (c == '\0')
                        outcome[sz++] = '\n';
                లేకపోతే
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        తిరిగి 0;
}

పూర్ణాంకం pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        గుర్తులేని దీర్ఘ ppid;
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);

        ఒకవేళ (pid == 0)
                pid = getpid_cached();
        ఒకవేళ (pid == 1) /* PID 1 has no parent, shortcut this case */
                తిరిగి -EADDRNOTAVAIL;

        ఒకవేళ (pid == getpid_cached()) {
                ఒకవేళ (ret)
                        *ret = getppid();
                తిరిగి 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_పంక్తి_file(p, &పంక్తి);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(పంక్తి, ')');
        ఒకవేళ (!p)
                తిరిగి -EIO;
        p++;

        ఒకవేళ (అక్షరశ్రేణి_చదువు(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                తిరిగి -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        ఒకవేళ (ppid == 0)
                తిరిగి -EADDRNOTAVAIL;

        ఒకవేళ ((pid_t) ppid < 0 || (గుర్తులేని దీర్ఘ) (pid_t) ppid != ppid)
                తిరిగి -ERANGE;

        ఒకవేళ (ret)
                *ret = (pid_t) ppid;

        తిరిగి 0;
}

పూర్ణాంకం pidref_get_ppid(స్థిరరాశి PidRef *pidref, pid_t *ret) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                ఒకవేళ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        తిరిగి r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = ppid;
        తిరిగి 0;
}

పూర్ణాంకం pidref_get_ppid_as_pidref(స్థిరరాశి PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        పూర్ణాంకం r;

        సూచిక_MAY_BE_NULL(pidref);
        ప్రతిజ్ఞా(ret);

        r = pidref_get_ppid(pidref, &ppid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (గుర్తులేని attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_పూర్తి) PidRef మాతృ = PIDREF_NULL;

                r = pidref_సమితి_pid(&మాతృ, ppid);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                ఒకవేళ (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        ఒకవేళ (మాతృ.pid != ppid)
                                కొనసాగు;
                }

                *ret = TAKE_PIDREF(మాతృ);
                తిరిగి 0;
        }

        /* Give up after 16 tries */
        తిరిగి -ENOTRECOVERABLE;
}

పూర్ణాంకం pid_get_ప్రారంభం_సమయం(pid_t pid, usec_t *ret) {
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_పంక్తి_file(p, &పంక్తి);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(పంక్తి, ')');
        ఒకవేళ (!p)
                తిరిగి -EIO;
        p++;

        గుర్తులేని దీర్ఘ llu;

        ఒకవేళ (అక్షరశ్రేణి_చదువు(p, " "
                   "%*c " /* state */
                   "%*u " /* ppid */
                   "%*u " /* pgrp */
                   "%*u " /* session */
                   "%*u " /* tty_nr */
                   "%*u " /* tpgid */
                   "%*u " /* flags */
                   "%*u " /* minflt */
                   "%*u " /* cminflt */
                   "%*u " /* majflt */
                   "%*u " /* cmajflt */
                   "%*u " /* utime */
                   "%*u " /* stime */
                   "%*u " /* cutime */
                   "%*u " /* cstime */
                   "%*i " /*ప్రాధాన్యత*/
                   "%*i " /* nice */
                   "%*u " /* num_threads */
                   "%*u " /* itrealvalue */
                   "%lu ", /* starttime */
                   &llu) != 1)
                తిరిగి -EIO;

        ఒకవేళ (ret)
                *ret = jiffies_గమ్యం_usec(llu); /* CLOCK_BOOTTIME */

        తిరిగి 0;
}

పూర్ణాంకం pidref_get_ప్రారంభం_సమయం(స్థిరరాశి PidRef *pid, usec_t *ret) {
        usec_t t;
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        r = pid_get_ప్రారంభం_సమయం(pid->pid, ret ? &t : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = t;

        తిరిగి 0;
}

పూర్ణాంకం get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ అక్షరం *m = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        r = procfs_file_get_క్షేత్రం(pid, "status", "Umask", &m);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_బహుళకం(m, ret);
}

స్థిర పూర్ణాంకం pid_get_sigmask(pid_t pid, స్థిరరాశి అక్షరం *క్షేత్రం_name, uint64_t *ret) {
        _cleanup_free_ అక్షరం *క్షేత్రం = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(క్షేత్రం_name);
        ప్రతిజ్ఞా(ret);

        r = procfs_file_get_క్షేత్రం(pid, "status", క్షేత్రం_name, &క్షేత్రం);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి సురక్షిత_atou64_నిండిన(క్షేత్రం, 16, ret);
}

స్థిర పూర్ణాంకం pidref_has_sigmask(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig, స్థిరరాశి అక్షరం *క్షేత్రం_name) {
        uint64_t ముసుగు_3;
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;
        ఒకవేళ (!SIGNAL_VALID(sig))
                తిరిగి -EINVAL;

        r = pid_get_sigmask(pidref->pid, క్షేత్రం_name, &ముసుగు_3);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి బిట్టు_సమితి(ముసుగు_3, sig - 1);
}

పూర్ణాంకం pidref_has_sigcgt(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig) {
        తిరిగి pidref_has_sigmask(pidref, sig, "SigCgt");
}

పూర్ణాంకం pidref_has_sigblk(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig) {
        తిరిగి pidref_has_sigmask(pidref, sig, "SigBlk");
}

/*
 * Return values:
 * < 0 : pidref_wait_for_terminate() failed to get the state of the
 *       process, the process was terminated by a signal, or
 *       failed for an unknown reason.
 * >=0 : The process terminated normally, and its exit code is
 *       returned.
 *
 * That is, success is indicated by a return value of zero, and an
 * error is indicated by a non-zero value.
 *
 * A warning is emitted if the process terminates abnormally,
 * and also if it returns non-zero unless check_exit_code is true.
 */
పూర్ణాంకం pidref_wait_for_terminate_and_check(స్థిరరాశి అక్షరం *name, PidRef *pidref, WaitFlags flags) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;
        ఒకవేళ (pidref->pid == 1 || pidref_is_self(pidref))
                తిరిగి -ECHILD;

        _cleanup_free_ అక్షరం *బఫరు = ఖాళీ;
        ఒకవేళ (!name) {
                r = pidref_get_comm(pidref, &బఫరు);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                లేకపోతే
                        name = బఫరు;
        }

        పూర్ణాంకం prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        ఒకవేళ (status.si_code == CLD_EXITED) {
                ఒకవేళ (status.si_status != EXIT_SUCCESS)
                        log_నిండిన(flags & WAIT_LOG_NON_ZERO_EXIT_స్థితి ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                లేకపోతే
                        log_debug("'%s' succeeded.", name);

                తిరిగి status.si_status;

        } లేకపోతే ఒకవేళ (లోపల_సమితి(status.si_code, CLD_KILLED, CLD_DUMPED))
                తిరిగి log_నిండిన_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_గమ్యం_string(status.si_status));

        తిరిగి log_నిండిన_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

పూర్ణాంకం kill_and_sigcont(pid_t pid, పూర్ణాంకం sig) {
        పూర్ణాంకం r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        ఒకవేళ (r >= 0 && !లోపల_సమితి(sig, SIGCONT, SIGKILL))
                (శూన్య) kill(pid, SIGCONT);

        తిరిగి r;
}

పూర్ణాంకం getenv_for_pid(pid_t pid, స్థిరరాశి అక్షరం *క్షేత్రం, అక్షరం **ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        స్థిరరాశి అక్షరం *మార్గం;
        size_t కూడిక = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(క్షేత్రం);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pid == 0 || pid == getpid_cached())
                తిరిగి strdup_గమ్యం_నిండిన(ret, getenv(క్షేత్రం));

        ఒకవేళ (!pid_is_valid(pid))
                తిరిగి -EINVAL;

        మార్గం = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(మార్గం, "re", &f);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                స్థిరరాశి అక్షరం *సరిపోలిక;

                ఒకవేళ (కూడిక > ENVIRONMENT_ఖండం_MAX) /* Give up searching eventually */
                        తిరిగి -ENOBUFS;

                r = read_nul_string(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)  /* EOF */
                        ఆపు;

                కూడిక += r;

                సరిపోలిక = startswith(పంక్తి, క్షేత్రం);
                ఒకవేళ (సరిపోలిక && *సరిపోలిక == '=')
                        తిరిగి strdup_గమ్యం_నిండిన(ret, సరిపోలిక + 1);
        }

        *ret = ఖాళీ;
        తిరిగి 0;
}

పూర్ణాంకం pidref_is_my_బిడ్డ(PidRef *pid) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        ఒకవేళ (pid->pid == 1 || pidref_is_self(pid))
                తిరిగి false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        ఒకవేళ (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ppid == getpid_cached();
}

పూర్ణాంకం pid_is_my_బిడ్డ(pid_t pid) {

        ఒకవేళ (pid == 0)
                తిరిగి false;

        తిరిగి pidref_is_my_బిడ్డ(&PIDREF_MAKE_FROM_PID(pid));
}

పూర్ణాంకం pidref_is_unwaited(PidRef *pid) {
        పూర్ణాంకం r;

        /* Checks whether a PID is still valid at all, including a zombie */

        ఒకవేళ (!pidref_is_సమితి(pid))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pid))
                తిరిగి -EREMOTE;

        ఒకవేళ (pid->pid == 1 || pidref_is_self(pid))
                తిరిగి true;

        r = pidref_kill(pid, 0);
        ఒకవేళ (r == -ESRCH)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి true;
}

పూర్ణాంకం pid_is_unwaited(pid_t pid) {

        ఒకవేళ (pid == 0)
                తిరిగి true;

        తిరిగి pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

పూర్ణాంకం pid_is_alive(pid_t pid) {
        పూర్ణాంకం r;

        /* Checks whether a PID is still valid and not a zombie */

        ఒకవేళ (pid < 0)
                తిరిగి -ESRCH;

        ఒకవేళ (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                తిరిగి true;

        ఒకవేళ (pid == getpid_cached())
                తిరిగి true;

        r = get_process_state(pid);
        ఒకవేళ (r == -ESRCH)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి r != 'Z';
}

పూర్ణాంకం pidref_is_alive(స్థిరరాశి PidRef *pidref) {
        పూర్ణాంకం r, ఫలితం;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఫలితం = pid_is_alive(pidref->pid);
        ఒకవేళ (ఫలితం < 0) {
                ప్రతిజ్ఞా(ఫలితం != -ESRCH);
                తిరిగి ఫలితం;
        }

        r = pidref_verify(pidref);
        ఒకవేళ (r == -ESRCH)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ఫలితం;
}

పూర్ణాంకం pidref_from_same_మూలం_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_పూర్తి) PidRef self = PIDREF_NULL;
        పూర్ణాంకం r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        ఒకవేళ (!a || !b) {
                r = pidref_సమితి_self(&self);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (!a)
                        a = &self;
                ఒకవేళ (!b)
                        b = &self;
        }

        ఒకవేళ (!pidref_is_సమితి(a) || !pidref_is_సమితి(b))
                తిరిగి -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        ఒకవేళ (pidref_is_remote(a) && pidref_is_remote(b))
                తిరిగి -EREMOTE;
        ఒకవేళ (pidref_is_remote(a) || pidref_is_remote(b))
                తిరిగి false;

        ఒకవేళ (pidref_equal(a, b))
                తిరిగి true;

        స్థిరరాశి అక్షరం *roota = procfs_file_alloca(a->pid, "root");
        స్థిరరాశి అక్షరం *rootb = procfs_file_alloca(b->pid, "root");

        పూర్ణాంకం ఫలితం = inode_same(roota, rootb, 0);
        ఒకవేళ (ఫలితం == -ENOENT)
                తిరిగి proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        ఒకవేళ (ఫలితం < 0)
                తిరిగి ఫలితం;

        r = pidref_verify(a);
        ఒకవేళ (r < 0)
                తిరిగి r;
        r = pidref_verify(b);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ఫలితం;
}

bool is_main_thread(శూన్య) {
        స్థిర thread_local పూర్ణాంకం cached = -1;

        ఒకవేళ (cached < 0)
                cached = getpid_cached() == gettid();

        తిరిగి cached;
}

గుర్తులేని దీర్ఘ personality_from_string(స్థిరరాశి అక్షరం *s) {
        Architecture architecture;

        ఒకవేళ (!s)
                తిరిగి PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        ఒకవేళ (architecture < 0)
                తిరిగి PERSONALITY_INVALID;

        ఒకవేళ (architecture == native_architecture())
                తిరిగి PER_LINUX;
#నిర్వచితమైతే ARCHITECTURE_SECONDARY
        ఒకవేళ (architecture == ARCHITECTURE_SECONDARY)
                తిరిగి PER_LINUX32;
#అయితే_ముగింపు

        తిరిగి PERSONALITY_INVALID;
}

స్థిరరాశి అక్షరం* personality_గమ్యం_string(గుర్తులేని దీర్ఘ p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        ఒకవేళ (p == PER_LINUX)
                architecture = native_architecture();
#నిర్వచితమైతే ARCHITECTURE_SECONDARY
        లేకపోతే ఒకవేళ (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#అయితే_ముగింపు

        ఒకవేళ (architecture < 0)
                తిరిగి ఖాళీ;

        తిరిగి architecture_గమ్యం_string(architecture);
}

పూర్ణాంకం సురక్షిత_personality(గుర్తులేని దీర్ఘ p) {
        పూర్ణాంకం ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        ఒకవేళ (ret < 0) {
                ఒకవేళ (errno != 0)
                        తిరిగి -errno;

                errno = -ret;
        }

        తిరిగి ret;
}

పూర్ణాంకం opinionated_personality(గుర్తులేని దీర్ఘ *ret) {
        పూర్ణాంకం current;

        ప్రతిజ్ఞా(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = సురక్షిత_personality(PERSONALITY_INVALID);
        ఒకవేళ (current < 0)
                తిరిగి current;

        ఒకవేళ (((గుర్తులేని దీర్ఘ) current & OPINIONATED_PERSONALITY_ముసుగు) == PER_LINUX32)
                *ret = PER_LINUX32;
        లేకపోతే
                *ret = PER_LINUX;

        తిరిగి 0;
}

శూన్య valgrind_summary_hack(శూన్య) {
#ఒకవేళ HAVE_VALGRIND_VALGRIND_H
        ఒకవేళ (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                ఒకవేళ (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_సందేశం_ID(SD_సందేశం_VALGRIND_HELPER_FORK_STR),
                                LOG_సందేశం("Failed to fork off valgrind helper: %m"));
                లేకపోతే ఒకవేళ (pid == 0)
                        నిష్క్రమించు(EXIT_SUCCESS);
                లేకపోతే {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_పూర్తి) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (శూన్య) pidref_సమితి_pid(&pidref, pid);
                        (శూన్య) pidref_wait_for_terminate(&pidref, ఖాళీ);
                }
        }
#అయితే_ముగింపు
}

పూర్ణాంకం pid_compare_func(స్థిరరాశి pid_t *a, స్థిరరాశి pid_t *b) {
        /* Suitable for usage in qsort() */
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి CMP(*a, *b);
}

bool nice_is_valid(పూర్ణాంకం n) {
        తిరిగి n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(పూర్ణాంకం policy) {
        తిరిగి లోపల_సమితి(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(పూర్ణాంకం policy) {
        తిరిగి sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
పూర్ణాంకం sched_get_ప్రాధాన్యత_min_సురక్షిత(పూర్ణాంకం policy) {
        పూర్ణాంకం r;

        r = sched_get_priority_min(policy);
        ఒకవేళ (r >= 0)
                తిరిగి r;

        /* Fallback priority */
        తిరిగి 0;
}

పూర్ణాంకం sched_get_ప్రాధాన్యత_max_సురక్షిత(పూర్ణాంకం policy) {
        పూర్ణాంకం r;

        r = sched_get_priority_max(policy);
        ఒకవేళ (r >= 0)
                తిరిగి r;

        తిరిగి 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#నిర్వచించు CACHED_PID_UNSET ((pid_t) 0)
#నిర్వచించు CACHED_PID_BUSY ((pid_t) -1)

స్థిర pid_t cached_pid = CACHED_PID_UNSET;

శూన్య reset_cached_pid(శూన్య) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(శూన్య) {
        స్థిర bool installed = false;
        pid_t current_విలువ;

        /* getpid_cached() is much like getpid(), but caches the value in local memory, to avoid having to invoke a
         * system call each time. This restores glibc behaviour from before 2.24, when getpid() was unconditionally
         * cached. Starting with 2.24 getpid() started to become prohibitively expensive when used for detecting when
         * objects were used across fork()s. With this caching the old behaviour is somewhat restored.
         *
         * https://bugzilla.redhat.com/show_bug.cgi?id=1443976
         * https://sourceware.org/git/gitweb.cgi?p=glibc.git;h=c579f48edba88380635ab98cb612030e3ed8691e
         */

        /* The compare-and-exchange below is a locked read-modify-write even when it fails. Almost all calls
         * find the PID cached, hence check for it with a plain load first. */
        current_విలువ = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        ఒకవేళ (current_విలువ > 0)
                తిరిగి current_విలువ;

        current_విలువ = CACHED_PID_UNSET;
        (శూన్య) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_విలువ,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        ఎంపిక (current_విలువ) {

        స్థితి CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                ఒకవేళ (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        ఒకవేళ (pthread_atfork(ఖాళీ, ఖాళీ, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                తిరిగి new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                తిరిగి new_pid;
        }

        స్థితి CACHED_PID_BUSY: /* Somebody else is currently initializing */
                తిరిగి getpid();

        అప్రమేయ: /* Properly initialized */
                తిరిగి current_విలువ;
        }
}

పూర్ణాంకం must_be_మూలం(శూన్య) {

        ఒకవేళ (geteuid() == 0)
                తిరిగి 0;

        తిరిగి log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_దొంతర(పూర్ణాంకం (*fn)(శూన్య *), పూర్ణాంకం flags, శూన్య *userdata) {
        size_t ps;
        pid_t pid;
        శూన్య *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        ప్రతిజ్ఞా((flags & (CLONE_VM|CLONE_మాతృ_SETTID|CLONE_బిడ్డ_SETTID|
                         CLONE_బిడ్డ_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = పుట_కొలత();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (శూన్య*) ALIGN_గమ్యం((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#ఒకవేళ HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#లేకపోతే
        pid = __clone2(fn, mystack, ps, flags, userdata);
#అయితే_ముగింపు
        ఒకవేళ (pid < 0)
                తిరిగి -errno;

        తిరిగి pid;
}

స్థిర పూర్ణాంకం fork_flags_గమ్యం_signal(ForkFlags flags) {
        తిరిగి (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

పూర్ణాంకం pidref_సురక్షిత_fork_నిండిన(
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి పూర్ణాంకం stdio_fds[3],
                పూర్ణాంకం except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        వాడని _cleanup_(ఖండం_సంకేతాలు_reset) sigset_t *saved_ssp = ఖాళీ;
        bool ఖండం_సంకేతాలు = false, ఖండం_all = false, intermediary = false;
        _cleanup_close_pair_ పూర్ణాంకం pidref_transport_fds[2] = EBADF_PAIR;
        పూర్ణాంకం prio, r;

        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, FORK_WAIT|FORK_FREEZE));
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        ఒకవేళ (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        ఒకవేళ (flags & (FORK_RESET_సంకేతాలు|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                ఖండం_సంకేతాలు = ఖండం_all = true;

        } లేకపోతే ఒకవేళ (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                ఖండం_సంకేతాలు = true;
        }

        ఒకవేళ (ఖండం_సంకేతాలు) {
                ఒకవేళ (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        తిరిగి log_నిండిన_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        ఒకవేళ (FLAGS_సమితి(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                ఒకవేళ (r < 0)
                        తిరిగి log_నిండిన_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                ఒకవేళ (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        ఒకవేళ (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                తిరిగి log_నిండిన_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        ఒకవేళ (pid < 0)
                                తిరిగి log_నిండిన_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        ఒకవేళ (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = సురక్షిత_close(pidref_transport_fds[1]);

                                ఒకవేళ (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_సమితి(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        ఒకవేళ (r < 0)
                                                తిరిగి log_నిండిన_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        ఒకవేళ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                తిరిగి -EPROTO;

                                        పూర్ణాంకం pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, పరిమాణం(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        ఒకవేళ (n < 0)
                                                తిరిగి log_నిండిన_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                తిరిగి 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = సురక్షిత_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        ఒకవేళ ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_సమితి(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_సమితి(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_సమితి(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_సమితి(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        లేకపోతే
                pid = fork();
        ఒకవేళ (pid < 0)
                తిరిగి log_నిండిన_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        ఒకవేళ (pid > 0) {

                /* If we are in the intermediary process, exit now */
                ఒకవేళ (intermediary) {
                        ఒకవేళ (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_పూర్తి) PidRef pidref = PIDREF_NULL;

                                r = pidref_సమితి_pid(&pidref, pid);
                                ఒకవేళ (r < 0) {
                                        log_నిండిన_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, పరిమాణం(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                ఒకవేళ (r < 0) {
                                        log_నిండిన_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                ఒకవేళ (flags & FORK_WAIT) {
                        ఒకవేళ (ఖండం_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (శూన్య) sigprocmask(SIG_SETMASK, &ss, ఖాళీ);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_సమితి(flags, FORK_LOG) ? WAIT_LOG : 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                తిరిగి -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        ఒకవేళ (ret)
                                *ret = PIDREF_NULL;

                        తిరిగి 1;
                }

                ఒకవేళ (ret) {
                        r = pidref_సమితి_pid(ret, pid);
                        ఒకవేళ (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                తిరిగి 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = సురక్షిత_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ఖాళీ;

        ఒకవేళ (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_సమితి_open_when_needed(true);
                log_settle_లక్ష్యం();
        }

        ఒకవేళ (name) {
                r = rename_process(name);
                ఒకవేళ (r < 0)
                        log_నిండిన_errno(flags & FORK_LOG ? LOG_WARNING : LOG_DEBUG,
                                       r, "Failed to rename process, ignoring: %m");
        }

        /* let's disable dlopen() in the child, as a paranoia safety precaution: children should not live for
         * long and only do minimal work before exiting or exec()ing. Doing dlopen() is not either. If people
         * want dlopen() they should do it before forking. This is a safety precaution in particular for
         * cases where the child does namespace shenanigans: we should never end up loading a module from a
         * foreign environment. Note that this has no effect on NSS! (i.e. it only has effect on uses of our
         * dlopen_safe(), which we use comprehensively in our codebase, but glibc NSS doesn't bother, of
         * course.) */
        ఒకవేళ (!FLAGS_సమితి(flags, FORK_ALLOW_DLOPEN))
                ఖండం_dlopen();

        ఒకవేళ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_సురక్షిత(PR_SET_PDEATHSIG, fork_flags_గమ్యం_signal(flags), 0, 0, 0);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (flags & FORK_RESET_సంకేతాలు) {
                r = reset_all_signal_handlers();
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_ముసుగు();
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } లేకపోతే ఒకవేళ (ఖండం_సంకేతాలు) { /* undo what we did above */
                ఒకవేళ (sigprocmask(SIG_SETMASK, &saved_ss, ఖాళీ) < 0) {
                        log_నిండిన_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                ఒకవేళ (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                లేకపోతే ఒకవేళ (ppid != original_pid) {
                        పూర్ణాంకం sig = fork_flags_గమ్యం_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_గమ్యం_string(sig));
                        (శూన్య) లేపు(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (FLAGS_సమితి(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                ఒకవేళ (mount(ఖాళీ, "/", ఖాళీ, MS_SLAVE | MS_REC, ఖాళీ) < 0) {
                        log_నిండిన_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (FLAGS_సమితి(flags, FORK_వ్యక్తిగత_TMP)) {
                ప్రతిజ్ఞా(FLAGS_సమితి(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_నిరంతరం);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (flags & FORK_REARRANGE_STDIO) {
                ఒకవేళ (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        ఒకవేళ (r < 0) {
                                log_నిండిన_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } లేకపోతే {
                        r = make_null_stdio();
                        ఒకవేళ (r < 0) {
                                log_నిండిన_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } లేకపోతే ఒకవేళ (flags & FORK_STDOUT_గమ్యం_STDERR) {
                ఒకవేళ (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_నిండిన_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                ప్రతిజ్ఞా(FLAGS_సమితి(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        ఒకవేళ (flags & FORK_REOPEN_LOG) {
                log_open();
                log_సమితి_open_when_needed(false);
        }

        ఒకవేళ (flags & FORK_RLIMIT_NOFILE_సురక్షిత) {
                r = rlimit_nofile_సురక్షిత();
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (!FLAGS_సమితి(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ఒకవేళ (FLAGS_సమితి(flags, FORK_FREEZE))
                freeze();

        ఒకవేళ (ret) {
                r = pidref_సమితి_self(ret);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        తిరిగి 0;
}

పూర్ణాంకం namespace_fork_నిండిన(
                స్థిరరాశి అక్షరం *outer_name,
                స్థిరరాశి అక్షరం *inner_name,
                పూర్ణాంకం except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                పూర్ణాంకం pidns_fd,
                పూర్ణాంకం mntns_fd,
                పూర్ణాంకం netns_fd,
                పూర్ణాంకం userns_fd,
                పూర్ణాంకం మూలం_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ పూర్ణాంకం errno_pipe_fd[2] = EBADF_PAIR;
        పూర్ణాంకం r, prio = FLAGS_సమితి(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        ప్రతిజ్ఞా((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        ప్రతిజ్ఞా((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(పరిమాణం(పూర్ణాంకం) <= PIPE_BUF);
        ఒకవేళ (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                తిరిగి log_నిండిన_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_సురక్షిత_fork_నిండిన(
                        outer_name,
                        /* stdio_fds= */ ఖాళీ, /* except_fds= */ ఖాళీ, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_సురక్షిత),
                        &pidref_outer);
        ఒకవేళ (r == -EPROTO && FLAGS_సమితి(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = సురక్షిత_close(errno_pipe_fd[1]);

                పూర్ణాంకం k = read_errno(errno_pipe_fd[0]);
                ఒకవేళ (k < 0 && k != -EIO)
                        తిరిగి k;
        }
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0) {
                _cleanup_(pidref_పూర్తి) PidRef pidref_inner = PIDREF_NULL;

                /*బిడ్డ*/

                errno_pipe_fd[0] = సురక్షిత_close(errno_pipe_fd[0]);

                r = namespace_enter_నిండిన(pidns_fd, mntns_fd, netns_fd, userns_fd, మూలం_fd, namespace_flags);
                ఒకవేళ (r < 0) {
                        log_నిండిన_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_సురక్షిత_fork_నిండిన(
                                inner_name,
                                ఖాళీ,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_సంకేతాలు|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_గమ్యం_STDERR),
                                &pidref_inner);
                ఒకవేళ (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                ఒకవేళ (r == 0) {
                        /*బిడ్డ*/

                        ఒకవేళ (!FLAGS_సమితి(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = సురక్షిత_close(errno_pipe_fd[1]);
                                pidref_పూర్తి(&pidref_outer);
                        } లేకపోతే {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        ఒకవేళ (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        తిరిగి 0;
                }

                log_forget_fds();
                log_సమితి_open_when_needed(true);

                (శూన్య) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_సమితి(flags, FORK_LOG) ? WAIT_LOG : 0);
                ఒకవేళ (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = సురక్షిత_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        ఒకవేళ (r < 0)
                తిరిగి r; /* the child logs about failures on its own, no need to duplicate here */

        ఒకవేళ (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        లేకపోతే
                pidref_పూర్తి(&pidref_outer); /* disarm sigkill_wait */

        తిరిగి 1;
}

పూర్ణాంకం namespace_fork(
                స్థిరరాశి అక్షరం *outer_name,
                స్థిరరాశి అక్షరం *inner_name,
                ForkFlags flags,
                పూర్ణాంకం pidns_fd,
                పూర్ణాంకం mntns_fd,
                పూర్ణాంకం netns_fd,
                పూర్ణాంకం userns_fd,
                పూర్ణాంకం మూలం_fd,
                PidRef *ret) {

        తిరిగి namespace_fork_నిండిన(outer_name, inner_name, /* except_fds= */ ఖాళీ, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, మూలం_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(పూర్ణాంకం oa) {
        తిరిగి oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

పూర్ణాంకం సమితి_oom_score_adjust(పూర్ణాంకం విలువ) {
        అక్షరం t[DECIMAL_STR_MAX(పూర్ణాంకం)];

        ఒకవేళ (!oom_score_adjust_is_valid(విలువ))
                తిరిగి -EINVAL;

        xsprintf(t, "%i", విలువ);

        తిరిగి write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_బఫరు);
}

పూర్ణాంకం get_oom_score_adjust(పూర్ణాంకం *ret) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        delete_trailing_chars(t, WHITESPACE);

        r = సురక్షిత_atoi(t, &a);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!oom_score_adjust_is_valid(a))
                తిరిగి -ENODATA;

        ఒకవేళ (ret)
                *ret = a;

        తిరిగి 0;
}

స్థిర పూర్ణాంకం rlimit_గమ్యం_nice(rlim_t limit) {
        ఒకవేళ (limit <= 1)
                తిరిగి PRIO_MAX-1; /* i.e. 19 */

        ఒకవేళ (limit >= -PRIO_MIN + PRIO_MAX)
                తిరిగి PRIO_MIN; /* i.e. -20 */

        తిరిగి PRIO_MAX - (పూర్ణాంకం) limit;
}

పూర్ణాంకం setpriority_closest(పూర్ణాంకం priority) {
        నిర్మాణం rlimit highest;
        పూర్ణాంకం r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        ఒకవేళ (r >= 0)
                తిరిగి 1;
        ఒకవేళ (!ERRNO_IS_NEG_PRIVILEGE(r))
                తిరిగి r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        ఒకవేళ (errno != 0)
                తిరిగి -errno;

        ఒకవేళ (priority == current)
                తిరిగి 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        ఒకవేళ (priority > current)
                తిరిగి r;

        ఒకవేళ (getrlimit(RLIMIT_NICE, &highest) < 0)
                తిరిగి -errno;

        limit = rlimit_గమ్యం_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        ఒకవేళ (current > limit)
                ఒకవేళ (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        తిరిగి -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        తిరిగి 0;
}

_noreturn_ శూన్య freeze(శూన్య) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (శూన్య) close_all_fds_without_malloc(ఖాళీ, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        కొరకు (;;) {
                siginfo_t si = {};

                ఒకవేళ (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        ఆపు;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        కొరకు (;;)
                pause();
}

పూర్ణాంకం get_process_దారాలు(pid_t pid) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం n, r;

        ఒకవేళ (pid < 0)
                తిరిగి -EINVAL;

        r = procfs_file_get_క్షేత్రం(pid, "status", "Threads", &t);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = సురక్షిత_atoi(t, &n);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (n < 0)
                తిరిగి -EINVAL;

        తిరిగి n;
}

పూర్ణాంకం is_reaper_process(శూన్య) {
        పూర్ణాంకం b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        ఒకవేళ (getpid_cached() == 1)
                తిరిగి true;

        r = prctl_సురక్షిత(PR_GET_CHILD_SUBREAPER, (గుర్తులేని దీర్ఘ) &b, 0, 0, 0);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి b != 0;
}

పూర్ణాంకం make_reaper_process(bool b) {
        పూర్ణాంకం r;

        ఒకవేళ (getpid_cached() == 1) {

                ఒకవేళ (!b)
                        తిరిగి -EINVAL;

                తిరిగి 0;
        }

        r = prctl_సురక్షిత(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_నిండిన(posix_spawnattr_t*, posix_spawnattr_destroy, ఖాళీ);

పూర్ణాంకం posix_spawn_wrapper(
                స్థిరరాశి అక్షరం *మార్గం,
                అక్షరం * స్థిరరాశి *argv,
                అక్షరం * స్థిరరాశి *envp,
                స్థిరరాశి అక్షరం *cgroup,
                PidRef *ret_pidref) {

        పొట్టి flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t ముసుగు_3;
        పూర్ణాంకం r;

        /* Forks and invokes 'path' with 'argv' and 'envp' using CLONE_VM and CLONE_VFORK, which means the
         * caller will be blocked until the child either exits or exec's. The memory of the child will be
         * fully shared with the memory of the parent, so that there are no copy-on-write or memory.max
         * issues.
         *
         * Also, move the newly-created process into 'cgroup' through POSIX_SPAWN_SETCGROUP (clone3())
         * if available.
         * returns 1: We're already in the right cgroup
         *         0: 'cgroup' not specified or POSIX_SPAWN_SETCGROUP is not supported. The caller
         *            needs to call 'cg_attach' on their own */

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(argv);
        ప్రతిజ్ఞా(ret_pidref);

        assert_se(sigfillset(&ముసుగు_3) >= 0);

        r = posix_spawnattr_init(&attr);
        ఒకవేళ (r != 0)
                తిరిగి -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        వాడని _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        స్థిర bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ పూర్ణాంకం cgroup_fd = -EBADF;

        ఒకవేళ (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ అక్షరం *resolved_cgroup = ఖాళీ;

                r = cg_get_మార్గం(cgroup, /* suffix= */ ఖాళీ, &resolved_cgroup);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                cgroup_fd = open(resolved_cgroup, O_మార్గం|O_DIRECTORY|O_CLOEXEC);
                ఒకవేళ (cgroup_fd < 0)
                        తిరిగి -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                ఒకవేళ (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                లేకపోతే ఒకవేళ (r != ENOSYS)
                        తిరిగి -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        ఒకవేళ (r != 0)
                తిరిగి -r;
        r = posix_spawnattr_setsigmask(&attr, &ముసుగు_3);
        ఒకవేళ (r != 0)
                తిరిగి -r;

        _cleanup_close_ పూర్ణాంకం pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, మార్గం, ఖాళీ, &attr, argv, envp);
        ఒకవేళ (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_సమితి(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                తిరిగి -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        ఒకవేళ ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
            FLAGS_సమితి(flags, POSIX_SPAWN_SETCGROUP)) {
                /* Compiled on a newer host, or seccomp&friends blocking clone3()? Fallback, but
                 * need to disable POSIX_SPAWN_SETCGROUP, which is what redirects to clone3().
                 * CLONE_INTO_CGROUP definitely won't work, hence remember the fact so that we don't
                 * retry every time.
                 * Note, CLONE_INTO_CGROUP is supported since kernel v5.7, but some architectures still
                 * do not support clone3(). Hence, we need to keep the fallback logic for a while. */
                have_clone_into_cgroup = false;

                flags &= ~POSIX_SPAWN_SETCGROUP;
                r = posix_spawnattr_setflags(&attr, flags);
                ఒకవేళ (r != 0)
                        తిరిగి -r;

                r = pidfd_spawn(&pidfd, మార్గం, ఖాళీ, &attr, argv, envp);
        }
        ఒకవేళ (r == 0) {
                r = pidref_సమితి_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి FLAGS_సమితి(flags, POSIX_SPAWN_SETCGROUP);
        }
        ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(r))
                తిరిగి -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, మార్గం, ఖాళీ, &attr, argv, envp);
        ఒకవేళ (r != 0)
                తిరిగి -r;

        r = pidref_సమితి_pid(ret_pidref, pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

పూర్ణాంకం proc_dir_open(DIR **ret) {
        DIR *d;

        ప్రతిజ్ఞా(ret);

        d = opendir("/proc");
        ఒకవేళ (!d)
                తిరిగి -errno;

        *ret = d;
        తిరిగి 0;
}

పూర్ణాంకం proc_dir_read(DIR *d, pid_t *ret) {
        ప్రతిజ్ఞా(d);

        కొరకు (;;) {
                నిర్మాణం dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                ఒకవేళ (!de) {
                        ఒకవేళ (errno != 0)
                                తిరిగి -errno;

                        ఆపు;
                }

                ఒకవేళ (!లోపల_సమితి(de->d_type, DT_DIR, DT_UNKNOWN))
                        కొనసాగు;

                ఒకవేళ (parse_pid(de->d_name, ret) >= 0)
                        తిరిగి 1;
        }

        ఒకవేళ (ret)
                *ret = 0;
        తిరిగి 0;
}

పూర్ణాంకం proc_dir_read_pidref(DIR *d, PidRef *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(d);

        కొరకు (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                r = pidref_సమితి_pid(ret, pid);
                ఒకవేళ (r == -ESRCH) /* gone by now? skip it */
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి 1;
        }

        ఒకవేళ (ret)
                *ret = PIDREF_NULL;
        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_mlockall(పూర్ణాంకం flags) {
        పూర్ణాంకం r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        ఒకవేళ (r == 0)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        ఒకవేళ (mlockall(flags) < 0)
                తిరిగి log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        తిరిగి 0;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigchld_సంకేతం_పట్టిక[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_పట్టిక_LOOKUP(sigchld_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి sched_policy_పట్టిక[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(sched_policy, పూర్ణాంకం, గరిష్ఠ_పూర్ణాంకం);

_noreturn_ శూన్య report_errno_and_exit(పూర్ణాంకం errno_fd, పూర్ణాంకం error) {
        పూర్ణాంకం r;

        ఒకవేళ (error >= 0)
                _exit(EXIT_SUCCESS);

        ప్రతిజ్ఞా(errno_fd >= 0);

        r = loop_write(errno_fd, &error, పరిమాణం(error));
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

పూర్ణాంకం read_errno(పూర్ణాంకం errno_fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, పరిమాణం(r), /* do_poll= */ false);
        ఒకవేళ (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                తిరిగి -EIO;
        }
        ఒకవేళ (n == 0) /* the process exited without reporting an error, assuming success */
                తిరిగి 0;
        ఒకవేళ (n != పరిమాణం(r))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        ఒకవేళ (r == 0)
                తిరిగి 0;
        ఒకవేళ (r < 0) /* child process reported an error, return it */
                తిరిగి log_debug_errno(r, "Child process failed with errno: %m");

        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

పూర్ణాంకం prctl_సురక్షిత(పూర్ణాంకం op, గుర్తులేని దీర్ఘ arg2, గుర్తులేని దీర్ఘ arg3, గుర్తులేని దీర్ఘ arg4, గుర్తులేని దీర్ఘ arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        తిరిగి RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

పూర్ణాంకం proc_సమితి_comm(స్థిరరాశి అక్షరం *comm) {
        తిరిగి prctl_సురక్షిత(PR_SET_NAME, (గుర్తులేని దీర్ఘ) comm, 0, 0, 0);
}

పూర్ణాంకం proc_సమితి_nnp(శూన్య) {
        తిరిగి prctl_సురక్షిత(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(శూన్య) {
        పూర్ణాంకం r;

        r = prctl_సురక్షిత(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        ఒకవేళ (r < 0)
                తిరిగి NSEC_INFINITY;

        తిరిగి r;
}
