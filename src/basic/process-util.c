/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/oom.h>
#شامل <pthread.h>
#شامل <spawn.h>
#شامل <stdio.h>
#شامل <sys/mman.h>
#شامل <sys/mount.h>
#شامل <sys/personality.h>
#شامل <sys/prctl.h>
#شامل <sys/wait.h>
#شامل <syslog.h>
#شامل <threads.h>
#شامل <unistd.h>
#اگر HAVE_VALGRIND_VALGRIND_H
#شامل <valgrind/valgrind.h>
#اگر_ختم

#شامل "sd-messages.h"

#شامل "alloc-util.h"
#شامل "architecture.h"
#شامل "argv-util.h"
#شامل "bitfield.h"
#شامل "capability-util.h"
#شامل "cgroup-util.h"
#شامل "dirent-util.h"
#شامل "dlfcn-util.h"
#شامل "errno-util.h"
#شامل "escape.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "io-util.h"
#شامل "iovec-util.h"
#شامل "locale-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "mountpoint-util.h"
#شامل "namespace-util.h"
#شامل "nulstr-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "pidfd-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "raw-clone.h"
#شامل "rlimit-util.h"
#شامل "signal-util.h"
#شامل "socket-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "time-util.h"
#شامل "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#تعریف_کرو COMM_MAX_LEN 128

ساکن صحیح get_process_state(pid_t pid) {
        _cleanup_free_ حرف *سطر = تہی;
        ثابت حرف *p;
        حرف state;
        صحیح r;

        أكد(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        اگر (pid == 0 || pid == getpid_cached())
                واپس (غیرعلامتی حرف) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_سطر_file(p, &سطر);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        p = strrchr(سطر, ')');
        اگر (!p)
                واپس -EIO;

        p++;

        اگر (لڑی_سے_پڑھو(p, " %c", &state) != 1)
                واپس -EIO;

        واپس (غیرعلامتی حرف) state;
}

صحیح pid_get_comm(pid_t pid, حرف **ret) {
        _cleanup_free_ حرف *escaped = تہی, *comm = تہی;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret);

        اگر (pid == 0 || pid == getpid_cached()) {
                comm = new0(حرف, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                اگر (!comm)
                        واپس -ENOMEM;

                r = prctl_محفوظ(PR_GET_NAME, (غیرعلامتی طویل) comm, 0, 0, 0);
                اگر (r < 0)
                        واپس r;
        } ورنہ {
                ثابت حرف *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_سطر_file(p, &comm);
                اگر (r == -ENOENT)
                        واپس -ESRCH;
                اگر (r < 0)
                        واپس r;
        }

        escaped = new(حرف, COMM_MAX_LEN);
        اگر (!escaped)
                واپس -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        واپس 0;
}

صحیح pidref_get_comm(ثابت PidRef *pid, حرف **ret) {
        _cleanup_free_ حرف *comm = تہی;
        صحیح r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pid);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = TAKE_PTR(comm);
        واپس 0;
}

ساکن صحیح pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_سائز,
                ProcessCmdlineFlags flags,
                حرف **ret,
                size_t *ret_سائز) {

        _cleanup_free_ حرف *t = تہی;
        ثابت حرف *p;
        size_t k;
        صحیح r;

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
        r = read_virtual_file(p, max_سائز, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        اگر (k == 0) {
                اگر (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        واپس -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ حرف *comm = تہی;

                r = pid_get_comm(pid, &comm);
                اگر (r < 0)
                        واپس r;

                آزاد(t);
                t = strjoin("[", comm, "]");
                اگر (!t)
                        واپس -ENOMEM;

                k = لڑی_لمبائی(t);
                r = k <= max_سائز;
                اگر (r == 0) /* truncation */
                        t[max_سائز] = '\0';
        }

        اگر (ret)
                *ret = TAKE_PTR(t);
        اگر (ret_سائز)
                *ret_سائز = k;

        واپس r;
}

صحیح pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret) {
        _cleanup_free_ حرف *t = تہی;
        size_t k;
        حرف *ans;

        أكد(pid >= 0);
        أكد(ret);

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

        صحیح بھرا = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        اگر (بھرا < 0)
                واپس بھرا;

        اگر (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_خالی_خانے |
                        FLAGS_سیٹ(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                أكد(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ حرف **دلائل = تہی;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                دلائل = strv_parse_nulstr_بھرا(t, k, /* drop_trailing_nuls= */ true);
                اگر (!دلائل)
                        واپس -ENOMEM;

                ans = quote_command_سطر(دلائل, shflags);
                اگر (!ans)
                        واپس -ENOMEM;
        } ورنہ {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                برائے (size_t i = 0; i < k - 1; i++)
                        اگر (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_ثنائی_ہندسہ = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_بھرا(t, max_columns,
                                                eight_ثنائی_ہندسہ * XESCAPE_8_ثنائی_ہندسہ | !بھرا * XESCAPE_FORCE_ELLIPSIS);
                اگر (!ans)
                        واپس -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        واپس 0;
}

صحیح pidref_get_cmdline(ثابت PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pid);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, حرف ***ret) {
        _cleanup_free_ حرف *t = تہی;
        حرف **دلائل;
        size_t k;
        صحیح r;

        أكد(pid >= 0);
        أكد((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        أكد(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        اگر (r < 0)
                واپس r;

        دلائل = strv_parse_nulstr_بھرا(t, k, /* drop_trailing_nuls= */ true);
        اگر (!دلائل)
                واپس -ENOMEM;

        *ret = دلائل;
        واپس 0;
}

صحیح pidref_get_cmdline_strv(ثابت PidRef *pid, ProcessCmdlineFlags flags, حرف ***ret) {
        _cleanup_strv_free_ حرف **دلائل = تہی;
        صحیح r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &دلائل);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pid);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = TAKE_PTR(دلائل);

        واپس 0;
}

صحیح procfs_get_stat_flags(ثابت حرف *راستہ, غیرعلامتی طویل طویل *ret) {
        صحیح r;

        أكد(راستہ);
        أكد(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ حرف *سطر = تہی;
        r = read_one_سطر_file(راستہ, &سطر);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        /* Skip past the comm field */
        حرف *q = strrchr(سطر, ')');
        اگر (!q)
                واپس -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        برائے (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                اگر (l < 1)
                        واپس -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                اگر (l < 1)
                        واپس -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        اگر (l < 1)
                واپس -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        اگر (l < 1)
                واپس -EINVAL;
        q[l] = 0;

        واپس محفوظ_atollu(q, ret);
}

صحیح pid_is_kernel_thread(pid_t pid) {
        صحیح r;

        اگر (اندر_سیٹ(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                واپس 0;
        اگر (!pid_is_valid(pid))
                واپس -EINVAL;

        ثابت حرف *p = procfs_file_alloca(pid, "stat");

        غیرعلامتی طویل طویل flags;
        r = procfs_get_stat_flags(p, &flags);
        اگر (r < 0)
                واپس r;

        واپس !!(flags & PF_KTHREAD);
}

صحیح pidref_is_kernel_thread(ثابت PidRef *pid) {
        صحیح نتیجہ, r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        نتیجہ = pid_is_kernel_thread(pid->pid);
        اگر (نتیجہ < 0)
                واپس نتیجہ;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        اگر (r < 0)
                واپس r;

        واپس نتیجہ;
}

ساکن صحیح get_process_link_contents(pid_t pid, ثابت حرف *proc_file, حرف **ret) {
        ثابت حرف *p;
        صحیح r;

        أكد(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        واپس (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

صحیح get_process_exe(pid_t pid, حرف **ret) {
        حرف *d;
        صحیح r;

        أكد(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        اگر (r < 0)
                واپس r;

        اگر (ret) {
                d = endswith(*ret, " (deleted)");
                اگر (d)
                        *d = '\0';
        }

        واپس 0;
}

صحیح pid_get_uid(pid_t pid, uid_t *ret) {
        صحیح r;

        أكد(pid >= 0);
        أكد(ret);

        اگر (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                واپس 0;
        }

        _cleanup_free_ حرف *v = تہی;
        r = procfs_file_get_میدان(pid, "status", "Uid", &v);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        واپس parse_uid(v, ret);
}

صحیح pidref_get_uid(ثابت PidRef *pid, uid_t *ret) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        اگر (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        واپس r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pid);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = uid;
        واپس 0;
}

صحیح get_process_gid(pid_t pid, gid_t *ret) {
        صحیح r;

        أكد(pid >= 0);
        أكد(ret);

        اگر (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                واپس 0;
        }

        _cleanup_free_ حرف *v = تہی;
        r = procfs_file_get_میدان(pid, "status", "Gid", &v);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        واپس parse_gid(v, ret);
}

صحیح get_process_cwd(pid_t pid, حرف **ret) {
        أكد(pid >= 0);

        اگر (pid == 0 || pid == getpid_cached())
                واپس محفوظ_getcwd(ret);

        واپس get_process_link_contents(pid, "cwd", ret);
}

صحیح get_process_جڑ(pid_t pid, حرف **ret) {
        أكد(pid >= 0);
        واپس get_process_link_contents(pid, "root", ret);
}

#تعریف_کرو ENVIRONMENT_بلاک_MAX (5U*1024U*1024U)

صحیح get_process_environ(pid_t pid, حرف **ret) {
        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_free_ حرف *outcome = تہی;
        size_t sz = 0;
        ثابت حرف *p;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                حرف c;

                اگر (sz >= ENVIRONMENT_بلاک_MAX)
                        واپس -ENOBUFS;

                اگر (!GREEDY_REALLOC(outcome, sz + 5))
                        واپس -ENOMEM;

                r = محفوظ_fgetc(f, &c);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                اگر (c == '\0')
                        outcome[sz++] = '\n';
                ورنہ
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        واپس 0;
}

صحیح pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ حرف *سطر = تہی;
        غیرعلامتی طویل ppid;
        ثابت حرف *p;
        صحیح r;

        أكد(pid >= 0);

        اگر (pid == 0)
                pid = getpid_cached();
        اگر (pid == 1) /* PID 1 has no parent, shortcut this case */
                واپس -EADDRNOTAVAIL;

        اگر (pid == getpid_cached()) {
                اگر (ret)
                        *ret = getppid();
                واپس 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_سطر_file(p, &سطر);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(سطر, ')');
        اگر (!p)
                واپس -EIO;
        p++;

        اگر (لڑی_سے_پڑھو(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                واپس -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        اگر (ppid == 0)
                واپس -EADDRNOTAVAIL;

        اگر ((pid_t) ppid < 0 || (غیرعلامتی طویل) (pid_t) ppid != ppid)
                واپس -ERANGE;

        اگر (ret)
                *ret = (pid_t) ppid;

        واپس 0;
}

صحیح pidref_get_ppid(ثابت PidRef *pidref, pid_t *ret) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;

        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        اگر (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        واپس r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : تہی);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = ppid;
        واپس 0;
}

صحیح pidref_get_ppid_as_pidref(ثابت PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        صحیح r;

        اشارہ_گر_MAY_BE_NULL(pidref);
        أكد(ret);

        r = pidref_get_ppid(pidref, &ppid);
        اگر (r < 0)
                واپس r;

        برائے (غیرعلامتی attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_مکمل) PidRef والد = PIDREF_NULL;

                r = pidref_سیٹ_pid(&والد, ppid);
                اگر (r < 0)
                        واپس r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                اگر (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        اگر (r < 0)
                                واپس r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        اگر (والد.pid != ppid)
                                جاری;
                }

                *ret = TAKE_PIDREF(والد);
                واپس 0;
        }

        /* Give up after 16 tries */
        واپس -ENOTRECOVERABLE;
}

صحیح pid_get_آغاز_وقت(pid_t pid, usec_t *ret) {
        _cleanup_free_ حرف *سطر = تہی;
        ثابت حرف *p;
        صحیح r;

        أكد(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_سطر_file(p, &سطر);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(سطر, ')');
        اگر (!p)
                واپس -EIO;
        p++;

        غیرعلامتی طویل llu;

        اگر (لڑی_سے_پڑھو(p, " "
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
                   "%*i " /*ترجیح*/
                   "%*i " /* nice */
                   "%*u " /* num_threads */
                   "%*u " /* itrealvalue */
                   "%lu ", /* starttime */
                   &llu) != 1)
                واپس -EIO;

        اگر (ret)
                *ret = jiffies_مقصد_usec(llu); /* CLOCK_BOOTTIME */

        واپس 0;
}

صحیح pidref_get_آغاز_وقت(ثابت PidRef *pid, usec_t *ret) {
        usec_t t;
        صحیح r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        r = pid_get_آغاز_وقت(pid->pid, ret ? &t : تہی);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pid);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = t;

        واپس 0;
}

صحیح get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ حرف *m = تہی;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret);

        r = procfs_file_get_میدان(pid, "status", "Umask", &m);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        واپس parse_منوال(m, ret);
}

ساکن صحیح pid_get_sigmask(pid_t pid, ثابت حرف *میدان_name, uint64_t *ret) {
        _cleanup_free_ حرف *میدان = تہی;
        صحیح r;

        أكد(pid >= 0);
        أكد(میدان_name);
        أكد(ret);

        r = procfs_file_get_میدان(pid, "status", میدان_name, &میدان);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        واپس محفوظ_atou64_بھرا(میدان, 16, ret);
}

ساکن صحیح pidref_has_sigmask(ثابت PidRef *pidref, صحیح sig, ثابت حرف *میدان_name) {
        uint64_t نقاب_3;
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;
        اگر (!SIGNAL_VALID(sig))
                واپس -EINVAL;

        r = pid_get_sigmask(pidref->pid, میدان_name, &نقاب_3);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        واپس ثنائی_ہندسہ_سیٹ(نقاب_3, sig - 1);
}

صحیح pidref_has_sigcgt(ثابت PidRef *pidref, صحیح sig) {
        واپس pidref_has_sigmask(pidref, sig, "SigCgt");
}

صحیح pidref_has_sigblk(ثابت PidRef *pidref, صحیح sig) {
        واپس pidref_has_sigmask(pidref, sig, "SigBlk");
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
صحیح pidref_wait_for_terminate_and_check(ثابت حرف *name, PidRef *pidref, WaitFlags flags) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;
        اگر (pidref->pid == 1 || pidref_is_self(pidref))
                واپس -ECHILD;

        _cleanup_free_ حرف *بفر = تہی;
        اگر (!name) {
                r = pidref_get_comm(pidref, &بفر);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                ورنہ
                        name = بفر;
        }

        صحیح prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        اگر (r < 0)
                واپس log_بھرا_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        اگر (status.si_code == CLD_EXITED) {
                اگر (status.si_status != EXIT_SUCCESS)
                        log_بھرا(flags & WAIT_LOG_NON_ZERO_EXIT_حالت ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                ورنہ
                        log_debug("'%s' succeeded.", name);

                واپس status.si_status;

        } ورنہ اگر (اندر_سیٹ(status.si_code, CLD_KILLED, CLD_DUMPED))
                واپس log_بھرا_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_مقصد_string(status.si_status));

        واپس log_بھرا_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

صحیح kill_and_sigcont(pid_t pid, صحیح sig) {
        صحیح r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        اگر (r >= 0 && !اندر_سیٹ(sig, SIGCONT, SIGKILL))
                (خالی) kill(pid, SIGCONT);

        واپس r;
}

صحیح getenv_for_pid(pid_t pid, ثابت حرف *میدان, حرف **ret) {
        _cleanup_fclose_ FILE *f = تہی;
        ثابت حرف *راستہ;
        size_t مجموع = 0;
        صحیح r;

        أكد(pid >= 0);
        أكد(میدان);
        أكد(ret);

        اگر (pid == 0 || pid == getpid_cached())
                واپس strdup_مقصد_بھرا(ret, getenv(میدان));

        اگر (!pid_is_valid(pid))
                واپس -EINVAL;

        راستہ = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(راستہ, "re", &f);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;
                ثابت حرف *مطابقت;

                اگر (مجموع > ENVIRONMENT_بلاک_MAX) /* Give up searching eventually */
                        واپس -ENOBUFS;

                r = read_nul_string(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)  /* EOF */
                        توڑو;

                مجموع += r;

                مطابقت = startswith(سطر, میدان);
                اگر (مطابقت && *مطابقت == '=')
                        واپس strdup_مقصد_بھرا(ret, مطابقت + 1);
        }

        *ret = تہی;
        واپس 0;
}

صحیح pidref_is_my_بچہ(PidRef *pid) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        اگر (pid->pid == 1 || pidref_is_self(pid))
                واپس false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        اگر (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                واپس false;
        اگر (r < 0)
                واپس r;

        واپس ppid == getpid_cached();
}

صحیح pid_is_my_بچہ(pid_t pid) {

        اگر (pid == 0)
                واپس false;

        واپس pidref_is_my_بچہ(&PIDREF_MAKE_FROM_PID(pid));
}

صحیح pidref_is_unwaited(PidRef *pid) {
        صحیح r;

        /* Checks whether a PID is still valid at all, including a zombie */

        اگر (!pidref_is_سیٹ(pid))
                واپس -ESRCH;

        اگر (pidref_is_remote(pid))
                واپس -EREMOTE;

        اگر (pid->pid == 1 || pidref_is_self(pid))
                واپس true;

        r = pidref_kill(pid, 0);
        اگر (r == -ESRCH)
                واپس false;
        اگر (r < 0)
                واپس r;

        واپس true;
}

صحیح pid_is_unwaited(pid_t pid) {

        اگر (pid == 0)
                واپس true;

        واپس pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

صحیح pid_is_alive(pid_t pid) {
        صحیح r;

        /* Checks whether a PID is still valid and not a zombie */

        اگر (pid < 0)
                واپس -ESRCH;

        اگر (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                واپس true;

        اگر (pid == getpid_cached())
                واپس true;

        r = get_process_state(pid);
        اگر (r == -ESRCH)
                واپس false;
        اگر (r < 0)
                واپس r;

        واپس r != 'Z';
}

صحیح pidref_is_alive(ثابت PidRef *pidref) {
        صحیح r, نتیجہ;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;

        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        نتیجہ = pid_is_alive(pidref->pid);
        اگر (نتیجہ < 0) {
                أكد(نتیجہ != -ESRCH);
                واپس نتیجہ;
        }

        r = pidref_verify(pidref);
        اگر (r == -ESRCH)
                واپس false;
        اگر (r < 0)
                واپس r;

        واپس نتیجہ;
}

صحیح pidref_from_same_جڑ_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_مکمل) PidRef self = PIDREF_NULL;
        صحیح r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        اگر (!a || !b) {
                r = pidref_سیٹ_self(&self);
                اگر (r < 0)
                        واپس r;
                اگر (!a)
                        a = &self;
                اگر (!b)
                        b = &self;
        }

        اگر (!pidref_is_سیٹ(a) || !pidref_is_سیٹ(b))
                واپس -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        اگر (pidref_is_remote(a) && pidref_is_remote(b))
                واپس -EREMOTE;
        اگر (pidref_is_remote(a) || pidref_is_remote(b))
                واپس false;

        اگر (pidref_equal(a, b))
                واپس true;

        ثابت حرف *roota = procfs_file_alloca(a->pid, "root");
        ثابت حرف *rootb = procfs_file_alloca(b->pid, "root");

        صحیح نتیجہ = inode_same(roota, rootb, 0);
        اگر (نتیجہ == -ENOENT)
                واپس proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        اگر (نتیجہ < 0)
                واپس نتیجہ;

        r = pidref_verify(a);
        اگر (r < 0)
                واپس r;
        r = pidref_verify(b);
        اگر (r < 0)
                واپس r;

        واپس نتیجہ;
}

bool is_main_thread(خالی) {
        ساکن thread_local صحیح cached = -1;

        اگر (cached < 0)
                cached = getpid_cached() == gettid();

        واپس cached;
}

غیرعلامتی طویل personality_from_string(ثابت حرف *s) {
        Architecture architecture;

        اگر (!s)
                واپس PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        اگر (architecture < 0)
                واپس PERSONALITY_INVALID;

        اگر (architecture == native_architecture())
                واپس PER_LINUX;
#اگر_تعریف_شدہ ARCHITECTURE_SECONDARY
        اگر (architecture == ARCHITECTURE_SECONDARY)
                واپس PER_LINUX32;
#اگر_ختم

        واپس PERSONALITY_INVALID;
}

ثابت حرف* personality_مقصد_string(غیرعلامتی طویل p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        اگر (p == PER_LINUX)
                architecture = native_architecture();
#اگر_تعریف_شدہ ARCHITECTURE_SECONDARY
        ورنہ اگر (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#اگر_ختم

        اگر (architecture < 0)
                واپس تہی;

        واپس architecture_مقصد_string(architecture);
}

صحیح محفوظ_personality(غیرعلامتی طویل p) {
        صحیح ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        اگر (ret < 0) {
                اگر (errno != 0)
                        واپس -errno;

                errno = -ret;
        }

        واپس ret;
}

صحیح opinionated_personality(غیرعلامتی طویل *ret) {
        صحیح current;

        أكد(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = محفوظ_personality(PERSONALITY_INVALID);
        اگر (current < 0)
                واپس current;

        اگر (((غیرعلامتی طویل) current & OPINIONATED_PERSONALITY_نقاب) == PER_LINUX32)
                *ret = PER_LINUX32;
        ورنہ
                *ret = PER_LINUX;

        واپس 0;
}

خالی valgrind_summary_hack(خالی) {
#اگر HAVE_VALGRIND_VALGRIND_H
        اگر (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                اگر (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_پیغام_ID(SD_پیغام_VALGRIND_HELPER_FORK_STR),
                                LOG_پیغام("Failed to fork off valgrind helper: %m"));
                ورنہ اگر (pid == 0)
                        باہر_نکلو(EXIT_SUCCESS);
                ورنہ {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_مکمل) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (خالی) pidref_سیٹ_pid(&pidref, pid);
                        (خالی) pidref_wait_for_terminate(&pidref, تہی);
                }
        }
#اگر_ختم
}

صحیح pid_compare_func(ثابت pid_t *a, ثابت pid_t *b) {
        /* Suitable for usage in qsort() */
        أكد(a);
        أكد(b);

        واپس CMP(*a, *b);
}

bool nice_is_valid(صحیح n) {
        واپس n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(صحیح policy) {
        واپس اندر_سیٹ(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(صحیح policy) {
        واپس sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
صحیح sched_get_ترجیح_min_محفوظ(صحیح policy) {
        صحیح r;

        r = sched_get_priority_min(policy);
        اگر (r >= 0)
                واپس r;

        /* Fallback priority */
        واپس 0;
}

صحیح sched_get_ترجیح_max_محفوظ(صحیح policy) {
        صحیح r;

        r = sched_get_priority_max(policy);
        اگر (r >= 0)
                واپس r;

        واپس 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#تعریف_کرو CACHED_PID_UNSET ((pid_t) 0)
#تعریف_کرو CACHED_PID_BUSY ((pid_t) -1)

ساکن pid_t cached_pid = CACHED_PID_UNSET;

خالی reset_cached_pid(خالی) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(خالی) {
        ساکن bool installed = false;
        pid_t current_قدر;

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
        current_قدر = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        اگر (current_قدر > 0)
                واپس current_قدر;

        current_قدر = CACHED_PID_UNSET;
        (خالی) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_قدر,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        انتخاب (current_قدر) {

        حالت CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                اگر (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        اگر (pthread_atfork(تہی, تہی, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                واپس new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                واپس new_pid;
        }

        حالت CACHED_PID_BUSY: /* Somebody else is currently initializing */
                واپس getpid();

        طےشدہ: /* Properly initialized */
                واپس current_قدر;
        }
}

صحیح must_be_جڑ(خالی) {

        اگر (geteuid() == 0)
                واپس 0;

        واپس log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ڈھیر(صحیح (*fn)(خالی *), صحیح flags, خالی *userdata) {
        size_t ps;
        pid_t pid;
        خالی *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        أكد((flags & (CLONE_VM|CLONE_والد_SETTID|CLONE_بچہ_SETTID|
                         CLONE_بچہ_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = صفحہ_سائز();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (خالی*) ALIGN_مقصد((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#اگر HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#ورنہ
        pid = __clone2(fn, mystack, ps, flags, userdata);
#اگر_ختم
        اگر (pid < 0)
                واپس -errno;

        واپس pid;
}

ساکن صحیح fork_flags_مقصد_signal(ForkFlags flags) {
        واپس (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

صحیح pidref_محفوظ_fork_بھرا(
                ثابت حرف *name,
                ثابت صحیح stdio_fds[3],
                صحیح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        غیر_مستعمل _cleanup_(بلاک_اشارات_reset) sigset_t *saved_ssp = تہی;
        bool بلاک_اشارات = false, بلاک_all = false, intermediary = false;
        _cleanup_close_pair_ صحیح pidref_transport_fds[2] = EBADF_PAIR;
        صحیح prio, r;

        أكد(!FLAGS_سیٹ(flags, FORK_WAIT|FORK_FREEZE));
        أكد(!FLAGS_سیٹ(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        اگر (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        اگر (flags & (FORK_RESET_اشارات|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                بلاک_اشارات = بلاک_all = true;

        } ورنہ اگر (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                بلاک_اشارات = true;
        }

        اگر (بلاک_اشارات) {
                اگر (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        واپس log_بھرا_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        اگر (FLAGS_سیٹ(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                اگر (r < 0)
                        واپس log_بھرا_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                اگر (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        اگر (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                واپس log_بھرا_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        اگر (pid < 0)
                                واپس log_بھرا_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        اگر (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = محفوظ_close(pidref_transport_fds[1]);

                                اگر (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_سیٹ(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        اگر (r < 0)
                                                واپس log_بھرا_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        اگر (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                واپس -EPROTO;

                                        صحیح pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, حجم(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        اگر (n < 0)
                                                واپس log_بھرا_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                واپس 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = محفوظ_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        اگر ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_سیٹ(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_سیٹ(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_سیٹ(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_سیٹ(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        ورنہ
                pid = fork();
        اگر (pid < 0)
                واپس log_بھرا_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        اگر (pid > 0) {

                /* If we are in the intermediary process, exit now */
                اگر (intermediary) {
                        اگر (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_مکمل) PidRef pidref = PIDREF_NULL;

                                r = pidref_سیٹ_pid(&pidref, pid);
                                اگر (r < 0) {
                                        log_بھرا_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, حجم(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                اگر (r < 0) {
                                        log_بھرا_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                اگر (flags & FORK_WAIT) {
                        اگر (بلاک_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (خالی) sigprocmask(SIG_SETMASK, &ss, تہی);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_سیٹ(flags, FORK_LOG) ? WAIT_LOG : 0);
                        اگر (r < 0)
                                واپس r;
                        اگر (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                واپس -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        اگر (ret)
                                *ret = PIDREF_NULL;

                        واپس 1;
                }

                اگر (ret) {
                        r = pidref_سیٹ_pid(ret, pid);
                        اگر (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                واپس 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = محفوظ_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = تہی;

        اگر (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_سیٹ_open_when_needed(true);
                log_settle_ہدف();
        }

        اگر (name) {
                r = rename_process(name);
                اگر (r < 0)
                        log_بھرا_errno(flags & FORK_LOG ? LOG_WARNING : LOG_DEBUG,
                                       r, "Failed to rename process, ignoring: %m");
        }

        /* let's disable dlopen() in the child, as a paranoia safety precaution: children should not live for
         * long and only do minimal work before exiting or exec()ing. Doing dlopen() is not either. If people
         * want dlopen() they should do it before forking. This is a safety precaution in particular for
         * cases where the child does namespace shenanigans: we should never end up loading a module from a
         * foreign environment. Note that this has no effect on NSS! (i.e. it only has effect on uses of our
         * dlopen_safe(), which we use comprehensively in our codebase, but glibc NSS doesn't bother, of
         * course.) */
        اگر (!FLAGS_سیٹ(flags, FORK_ALLOW_DLOPEN))
                بلاک_dlopen();

        اگر (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_محفوظ(PR_SET_PDEATHSIG, fork_flags_مقصد_signal(flags), 0, 0, 0);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (flags & FORK_RESET_اشارات) {
                r = reset_all_signal_handlers();
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_نقاب();
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } ورنہ اگر (بلاک_اشارات) { /* undo what we did above */
                اگر (sigprocmask(SIG_SETMASK, &saved_ss, تہی) < 0) {
                        log_بھرا_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                اگر (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                ورنہ اگر (ppid != original_pid) {
                        صحیح sig = fork_flags_مقصد_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_مقصد_string(sig));
                        (خالی) اٹھاؤ(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (FLAGS_سیٹ(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                اگر (mount(تہی, "/", تہی, MS_SLAVE | MS_REC, تہی) < 0) {
                        log_بھرا_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (FLAGS_سیٹ(flags, FORK_نجی_TMP)) {
                أكد(FLAGS_سیٹ(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_تسلسل);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (flags & FORK_REARRANGE_STDIO) {
                اگر (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        اگر (r < 0) {
                                log_بھرا_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } ورنہ {
                        r = make_null_stdio();
                        اگر (r < 0) {
                                log_بھرا_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } ورنہ اگر (flags & FORK_STDOUT_مقصد_STDERR) {
                اگر (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_بھرا_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                أكد(FLAGS_سیٹ(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        اگر (flags & FORK_REOPEN_LOG) {
                log_open();
                log_سیٹ_open_when_needed(false);
        }

        اگر (flags & FORK_RLIMIT_NOFILE_محفوظ) {
                r = rlimit_nofile_محفوظ();
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (!FLAGS_سیٹ(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        اگر (FLAGS_سیٹ(flags, FORK_FREEZE))
                freeze();

        اگر (ret) {
                r = pidref_سیٹ_self(ret);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        واپس 0;
}

صحیح namespace_fork_بھرا(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                صحیح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح جڑ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_مکمل_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ صحیح errno_pipe_fd[2] = EBADF_PAIR;
        صحیح r, prio = FLAGS_سیٹ(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        أكد((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        أكد((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        أكد(!FLAGS_سیٹ(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(حجم(صحیح) <= PIPE_BUF);
        اگر (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                واپس log_بھرا_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_محفوظ_fork_بھرا(
                        outer_name,
                        /* stdio_fds= */ تہی, /* except_fds= */ تہی, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_محفوظ),
                        &pidref_outer);
        اگر (r == -EPROTO && FLAGS_سیٹ(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = محفوظ_close(errno_pipe_fd[1]);

                صحیح k = read_errno(errno_pipe_fd[0]);
                اگر (k < 0 && k != -EIO)
                        واپس k;
        }
        اگر (r < 0)
                واپس r;
        اگر (r == 0) {
                _cleanup_(pidref_مکمل) PidRef pidref_inner = PIDREF_NULL;

                /*بچہ*/

                errno_pipe_fd[0] = محفوظ_close(errno_pipe_fd[0]);

                r = namespace_enter_بھرا(pidns_fd, mntns_fd, netns_fd, userns_fd, جڑ_fd, namespace_flags);
                اگر (r < 0) {
                        log_بھرا_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_محفوظ_fork_بھرا(
                                inner_name,
                                تہی,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_اشارات|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_مقصد_STDERR),
                                &pidref_inner);
                اگر (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                اگر (r == 0) {
                        /*بچہ*/

                        اگر (!FLAGS_سیٹ(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = محفوظ_close(errno_pipe_fd[1]);
                                pidref_مکمل(&pidref_outer);
                        } ورنہ {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        اگر (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        واپس 0;
                }

                log_forget_fds();
                log_سیٹ_open_when_needed(true);

                (خالی) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_سیٹ(flags, FORK_LOG) ? WAIT_LOG : 0);
                اگر (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = محفوظ_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        اگر (r < 0)
                واپس r; /* the child logs about failures on its own, no need to duplicate here */

        اگر (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        ورنہ
                pidref_مکمل(&pidref_outer); /* disarm sigkill_wait */

        واپس 1;
}

صحیح namespace_fork(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                ForkFlags flags,
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح جڑ_fd,
                PidRef *ret) {

        واپس namespace_fork_بھرا(outer_name, inner_name, /* except_fds= */ تہی, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, جڑ_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(صحیح oa) {
        واپس oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

صحیح سیٹ_oom_score_adjust(صحیح قدر) {
        حرف t[DECIMAL_STR_MAX(صحیح)];

        اگر (!oom_score_adjust_is_valid(قدر))
                واپس -EINVAL;

        xsprintf(t, "%i", قدر);

        واپس write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_بفر);
}

صحیح get_oom_score_adjust(صحیح *ret) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, تہی);
        اگر (r < 0)
                واپس r;

        delete_trailing_chars(t, WHITESPACE);

        r = محفوظ_atoi(t, &a);
        اگر (r < 0)
                واپس r;

        اگر (!oom_score_adjust_is_valid(a))
                واپس -ENODATA;

        اگر (ret)
                *ret = a;

        واپس 0;
}

ساکن صحیح rlimit_مقصد_nice(rlim_t limit) {
        اگر (limit <= 1)
                واپس PRIO_MAX-1; /* i.e. 19 */

        اگر (limit >= -PRIO_MIN + PRIO_MAX)
                واپس PRIO_MIN; /* i.e. -20 */

        واپس PRIO_MAX - (صحیح) limit;
}

صحیح setpriority_closest(صحیح priority) {
        ساخت rlimit highest;
        صحیح r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        اگر (r >= 0)
                واپس 1;
        اگر (!ERRNO_IS_NEG_PRIVILEGE(r))
                واپس r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        اگر (errno != 0)
                واپس -errno;

        اگر (priority == current)
                واپس 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        اگر (priority > current)
                واپس r;

        اگر (getrlimit(RLIMIT_NICE, &highest) < 0)
                واپس -errno;

        limit = rlimit_مقصد_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        اگر (current > limit)
                اگر (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        واپس -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        واپس 0;
}

_noreturn_ خالی freeze(خالی) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (خالی) close_all_fds_without_malloc(تہی, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        برائے (;;) {
                siginfo_t si = {};

                اگر (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        توڑو;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        برائے (;;)
                pause();
}

صحیح get_process_دھاگے(pid_t pid) {
        _cleanup_free_ حرف *t = تہی;
        صحیح n, r;

        اگر (pid < 0)
                واپس -EINVAL;

        r = procfs_file_get_میدان(pid, "status", "Threads", &t);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        r = محفوظ_atoi(t, &n);
        اگر (r < 0)
                واپس r;
        اگر (n < 0)
                واپس -EINVAL;

        واپس n;
}

صحیح is_reaper_process(خالی) {
        صحیح b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        اگر (getpid_cached() == 1)
                واپس true;

        r = prctl_محفوظ(PR_GET_CHILD_SUBREAPER, (غیرعلامتی طویل) &b, 0, 0, 0);
        اگر (r < 0)
                واپس r;

        واپس b != 0;
}

صحیح make_reaper_process(bool b) {
        صحیح r;

        اگر (getpid_cached() == 1) {

                اگر (!b)
                        واپس -EINVAL;

                واپس 0;
        }

        r = prctl_محفوظ(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        اگر (r < 0)
                واپس r;

        واپس 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_بھرا(posix_spawnattr_t*, posix_spawnattr_destroy, تہی);

صحیح posix_spawn_wrapper(
                ثابت حرف *راستہ,
                حرف * ثابت *argv,
                حرف * ثابت *envp,
                ثابت حرف *cgroup,
                PidRef *ret_pidref) {

        مختصر flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t نقاب_3;
        صحیح r;

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

        أكد(راستہ);
        أكد(argv);
        أكد(ret_pidref);

        assert_se(sigfillset(&نقاب_3) >= 0);

        r = posix_spawnattr_init(&attr);
        اگر (r != 0)
                واپس -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        غیر_مستعمل _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        ساکن bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ صحیح cgroup_fd = -EBADF;

        اگر (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ حرف *resolved_cgroup = تہی;

                r = cg_get_راستہ(cgroup, /* suffix= */ تہی, &resolved_cgroup);
                اگر (r < 0)
                        واپس r;

                cgroup_fd = open(resolved_cgroup, O_راستہ|O_DIRECTORY|O_CLOEXEC);
                اگر (cgroup_fd < 0)
                        واپس -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                اگر (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                ورنہ اگر (r != ENOSYS)
                        واپس -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        اگر (r != 0)
                واپس -r;
        r = posix_spawnattr_setsigmask(&attr, &نقاب_3);
        اگر (r != 0)
                واپس -r;

        _cleanup_close_ صحیح pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, راستہ, تہی, &attr, argv, envp);
        اگر (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_سیٹ(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                واپس -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        اگر ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
            FLAGS_سیٹ(flags, POSIX_SPAWN_SETCGROUP)) {
                /* Compiled on a newer host, or seccomp&friends blocking clone3()? Fallback, but
                 * need to disable POSIX_SPAWN_SETCGROUP, which is what redirects to clone3().
                 * CLONE_INTO_CGROUP definitely won't work, hence remember the fact so that we don't
                 * retry every time.
                 * Note, CLONE_INTO_CGROUP is supported since kernel v5.7, but some architectures still
                 * do not support clone3(). Hence, we need to keep the fallback logic for a while. */
                have_clone_into_cgroup = false;

                flags &= ~POSIX_SPAWN_SETCGROUP;
                r = posix_spawnattr_setflags(&attr, flags);
                اگر (r != 0)
                        واپس -r;

                r = pidfd_spawn(&pidfd, راستہ, تہی, &attr, argv, envp);
        }
        اگر (r == 0) {
                r = pidref_سیٹ_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                اگر (r < 0)
                        واپس r;

                واپس FLAGS_سیٹ(flags, POSIX_SPAWN_SETCGROUP);
        }
        اگر (!ERRNO_IS_NOT_SUPPORTED(r))
                واپس -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, راستہ, تہی, &attr, argv, envp);
        اگر (r != 0)
                واپس -r;

        r = pidref_سیٹ_pid(ret_pidref, pid);
        اگر (r < 0)
                واپس r;

        واپس 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

صحیح proc_dir_open(DIR **ret) {
        DIR *d;

        أكد(ret);

        d = opendir("/proc");
        اگر (!d)
                واپس -errno;

        *ret = d;
        واپس 0;
}

صحیح proc_dir_read(DIR *d, pid_t *ret) {
        أكد(d);

        برائے (;;) {
                ساخت dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                اگر (!de) {
                        اگر (errno != 0)
                                واپس -errno;

                        توڑو;
                }

                اگر (!اندر_سیٹ(de->d_type, DT_DIR, DT_UNKNOWN))
                        جاری;

                اگر (parse_pid(de->d_name, ret) >= 0)
                        واپس 1;
        }

        اگر (ret)
                *ret = 0;
        واپس 0;
}

صحیح proc_dir_read_pidref(DIR *d, PidRef *ret) {
        صحیح r;

        أكد(d);

        برائے (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                r = pidref_سیٹ_pid(ret, pid);
                اگر (r == -ESRCH) /* gone by now? skip it */
                        جاری;
                اگر (r < 0)
                        واپس r;

                واپس 1;
        }

        اگر (ret)
                *ret = PIDREF_NULL;
        واپس 0;
}

صحیح محفوظ_mlockall(صحیح flags) {
        صحیح r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        اگر (r == 0)
                واپس log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        اگر (mlockall(flags) < 0)
                واپس log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        واپس 0;
}

ساکن ثابت حرف *ثابت sigchld_رمز_جدول[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_جدول_LOOKUP(sigchld_رمز, صحیح);

ساکن ثابت حرف* ثابت sched_policy_جدول[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(sched_policy, صحیح, زیادہ_سے_زیادہ_عدد);

_noreturn_ خالی report_errno_and_exit(صحیح errno_fd, صحیح error) {
        صحیح r;

        اگر (error >= 0)
                _exit(EXIT_SUCCESS);

        أكد(errno_fd >= 0);

        r = loop_write(errno_fd, &error, حجم(error));
        اگر (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

صحیح read_errno(صحیح errno_fd) {
        صحیح r;

        أكد(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, حجم(r), /* do_poll= */ false);
        اگر (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                واپس -EIO;
        }
        اگر (n == 0) /* the process exited without reporting an error, assuming success */
                واپس 0;
        اگر (n != حجم(r))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        اگر (r == 0)
                واپس 0;
        اگر (r < 0) /* child process reported an error, return it */
                واپس log_debug_errno(r, "Child process failed with errno: %m");

        واپس log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

صحیح prctl_محفوظ(صحیح op, غیرعلامتی طویل arg2, غیرعلامتی طویل arg3, غیرعلامتی طویل arg4, غیرعلامتی طویل arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        واپس RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

صحیح proc_سیٹ_comm(ثابت حرف *comm) {
        واپس prctl_محفوظ(PR_SET_NAME, (غیرعلامتی طویل) comm, 0, 0, 0);
}

صحیح proc_سیٹ_nnp(خالی) {
        واپس prctl_محفوظ(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(خالی) {
        صحیح r;

        r = prctl_محفوظ(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        اگر (r < 0)
                واپس NSEC_INFINITY;

        واپس r;
}
