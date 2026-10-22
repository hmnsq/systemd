/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/oom.h>
#ضمن <pthread.h>
#ضمن <spawn.h>
#ضمن <stdio.h>
#ضمن <sys/mman.h>
#ضمن <sys/mount.h>
#ضمن <sys/personality.h>
#ضمن <sys/prctl.h>
#ضمن <sys/wait.h>
#ضمن <syslog.h>
#ضمن <threads.h>
#ضمن <unistd.h>
#إذا HAVE_VALGRIND_VALGRIND_H
#ضمن <valgrind/valgrind.h>
#endif

#ضمن "sd-messages.h"

#ضمن "alloc-util.h"
#ضمن "architecture.h"
#ضمن "argv-util.h"
#ضمن "bitfield.h"
#ضمن "capability-util.h"
#ضمن "cgroup-util.h"
#ضمن "dirent-util.h"
#ضمن "dlfcn-util.h"
#ضمن "errno-util.h"
#ضمن "escape.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "io-util.h"
#ضمن "iovec-util.h"
#ضمن "locale-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "mountpoint-util.h"
#ضمن "namespace-util.h"
#ضمن "nulstr-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "pidfd-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "raw-clone.h"
#ضمن "rlimit-util.h"
#ضمن "signal-util.h"
#ضمن "socket-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "time-util.h"
#ضمن "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

ساكن صحيح get_process_state(pid_t pid) {
        _cleanup_free_ حرف *قۇر = عدم;
        ثابت حرف *p;
        حرف state;
        صحيح r;

        أكد(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        إذا (pid == 0 || pid == getpid_cached())
                أرجع (unsigned حرف) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_قۇر_file(p, &قۇر);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        p = strrchr(قۇر, ')');
        إذا (!p)
                أرجع -EIO;

        p++;

        إذا (sscanf(p, " %c", &state) != 1)
                أرجع -EIO;

        أرجع (unsigned حرف) state;
}

صحيح pid_get_comm(pid_t pid, حرف **ret) {
        _cleanup_free_ حرف *escaped = عدم, *comm = عدم;
        صحيح r;

        أكد(pid >= 0);
        أكد(ret);

        إذا (pid == 0 || pid == getpid_cached()) {
                comm = new0(حرف, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                إذا (!comm)
                        أرجع -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                إذا (r < 0)
                        أرجع r;
        } وإلا {
                ثابت حرف *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_قۇر_file(p, &comm);
                إذا (r == -ENOENT)
                        أرجع -ESRCH;
                إذا (r < 0)
                        أرجع r;
        }

        escaped = new(حرف, COMM_MAX_LEN);
        إذا (!escaped)
                أرجع -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        أرجع 0;
}

صحيح pidref_get_comm(ثابت PidRef *pid, حرف **ret) {
        _cleanup_free_ حرف *comm = عدم;
        صحيح r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pid);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = TAKE_PTR(comm);
        أرجع 0;
}

ساكن صحيح pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                حرف **ret,
                size_t *ret_size) {

        _cleanup_free_ حرف *t = عدم;
        ثابت حرف *p;
        size_t k;
        صحيح r;

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
        r = read_virtual_file(p, max_size, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        إذا (k == 0) {
                إذا (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        أرجع -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ حرف *comm = عدم;

                r = pid_get_comm(pid, &comm);
                إذا (r < 0)
                        أرجع r;

                حرر(t);
                t = strjoin("[", comm, "]");
                إذا (!t)
                        أرجع -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                إذا (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        إذا (ret)
                *ret = TAKE_PTR(t);
        إذا (ret_size)
                *ret_size = k;

        أرجع r;
}

صحيح pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret) {
        _cleanup_free_ حرف *t = عدم;
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

        صحيح full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        إذا (full < 0)
                أرجع full;

        إذا (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                أكد(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ حرف **args = عدم;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                إذا (!args)
                        أرجع -ENOMEM;

                ans = quote_command_قۇر(args, shflags);
                إذا (!ans)
                        أرجع -ENOMEM;
        } وإلا {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                لكل (size_t i = 0; i < k - 1; i++)
                        إذا (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                إذا (!ans)
                        أرجع -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        أرجع 0;
}

صحيح pidref_get_cmdline(ثابت PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pid);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, حرف ***ret) {
        _cleanup_free_ حرف *t = عدم;
        حرف **args;
        size_t k;
        صحيح r;

        أكد(pid >= 0);
        أكد((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        أكد(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        إذا (r < 0)
                أرجع r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        إذا (!args)
                أرجع -ENOMEM;

        *ret = args;
        أرجع 0;
}

صحيح pidref_get_cmdline_strv(ثابت PidRef *pid, ProcessCmdlineFlags flags, حرف ***ret) {
        _cleanup_strv_free_ حرف **args = عدم;
        صحيح r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pid);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = TAKE_PTR(args);

        أرجع 0;
}

صحيح procfs_get_stat_flags(ثابت حرف *path, unsigned long long *ret) {
        صحيح r;

        أكد(path);
        أكد(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ حرف *قۇر = عدم;
        r = read_one_قۇر_file(path, &قۇر);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        /* Skip past the comm field */
        حرف *q = strrchr(قۇر, ')');
        إذا (!q)
                أرجع -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        لكل (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                إذا (l < 1)
                        أرجع -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                إذا (l < 1)
                        أرجع -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        إذا (l < 1)
                أرجع -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        إذا (l < 1)
                أرجع -EINVAL;
        q[l] = 0;

        أرجع safe_atollu(q, ret);
}

صحيح pid_is_kernel_thread(pid_t pid) {
        صحيح r;

        إذا (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                أرجع 0;
        إذا (!pid_is_valid(pid))
                أرجع -EINVAL;

        ثابت حرف *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        إذا (r < 0)
                أرجع r;

        أرجع !!(flags & PF_KTHREAD);
}

صحيح pidref_is_kernel_thread(ثابت PidRef *pid) {
        صحيح result, r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        إذا (result < 0)
                أرجع result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        إذا (r < 0)
                أرجع r;

        أرجع result;
}

ساكن صحيح get_process_link_contents(pid_t pid, ثابت حرف *proc_file, حرف **ret) {
        ثابت حرف *p;
        صحيح r;

        أكد(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        أرجع (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

صحيح get_process_exe(pid_t pid, حرف **ret) {
        حرف *d;
        صحيح r;

        أكد(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        إذا (r < 0)
                أرجع r;

        إذا (ret) {
                d = endswith(*ret, " (deleted)");
                إذا (d)
                        *d = '\0';
        }

        أرجع 0;
}

صحيح pid_get_uid(pid_t pid, uid_t *ret) {
        صحيح r;

        أكد(pid >= 0);
        أكد(ret);

        إذا (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                أرجع 0;
        }

        _cleanup_free_ حرف *v = عدم;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        أرجع parse_uid(v, ret);
}

صحيح pidref_get_uid(ثابت PidRef *pid, uid_t *ret) {
        صحيح r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        إذا (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                إذا (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        أرجع r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pid);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = uid;
        أرجع 0;
}

صحيح get_process_gid(pid_t pid, gid_t *ret) {
        صحيح r;

        أكد(pid >= 0);
        أكد(ret);

        إذا (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                أرجع 0;
        }

        _cleanup_free_ حرف *v = عدم;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        أرجع parse_gid(v, ret);
}

صحيح get_process_cwd(pid_t pid, حرف **ret) {
        أكد(pid >= 0);

        إذا (pid == 0 || pid == getpid_cached())
                أرجع safe_getcwd(ret);

        أرجع get_process_link_contents(pid, "cwd", ret);
}

صحيح get_process_يىلتىز(pid_t pid, حرف **ret) {
        أكد(pid >= 0);
        أرجع get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

صحيح get_process_environ(pid_t pid, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *outcome = عدم;
        size_t sz = 0;
        ثابت حرف *p;
        صحيح r;

        أكد(pid >= 0);
        أكد(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        لكل (;;) {
                حرف c;

                إذا (sz >= ENVIRONMENT_BLOCK_MAX)
                        أرجع -ENOBUFS;

                إذا (!GREEDY_REALLOC(outcome, sz + 5))
                        أرجع -ENOMEM;

                r = safe_fgetc(f, &c);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                إذا (c == '\0')
                        outcome[sz++] = '\n';
                وإلا
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        أرجع 0;
}

صحيح pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ حرف *قۇر = عدم;
        unsigned long ppid;
        ثابت حرف *p;
        صحيح r;

        أكد(pid >= 0);

        إذا (pid == 0)
                pid = getpid_cached();
        إذا (pid == 1) /* PID 1 has no parent, shortcut this case */
                أرجع -EADDRNOTAVAIL;

        إذا (pid == getpid_cached()) {
                إذا (ret)
                        *ret = getppid();
                أرجع 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_قۇر_file(p, &قۇر);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(قۇر, ')');
        إذا (!p)
                أرجع -EIO;
        p++;

        إذا (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                أرجع -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        إذا (ppid == 0)
                أرجع -EADDRNOTAVAIL;

        إذا ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                أرجع -ERANGE;

        إذا (ret)
                *ret = (pid_t) ppid;

        أرجع 0;
}

صحيح pidref_get_ppid(ثابت PidRef *pidref, pid_t *ret) {
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                إذا (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        أرجع r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : عدم);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = ppid;
        أرجع 0;
}

صحيح pidref_get_ppid_as_pidref(ثابت PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        صحيح r;

        POINTER_MAY_BE_NULL(pidref);
        أكد(ret);

        r = pidref_get_ppid(pidref, &ppid);
        إذا (r < 0)
                أرجع r;

        لكل (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                إذا (r < 0)
                        أرجع r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                إذا (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        إذا (r < 0)
                                أرجع r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        إذا (parent.pid != ppid)
                                استمر;
                }

                *ret = TAKE_PIDREF(parent);
                أرجع 0;
        }

        /* Give up after 16 tries */
        أرجع -ENOTRECOVERABLE;
}

صحيح pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ حرف *قۇر = عدم;
        ثابت حرف *p;
        صحيح r;

        أكد(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_قۇر_file(p, &قۇر);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(قۇر, ')');
        إذا (!p)
                أرجع -EIO;
        p++;

        unsigned long llu;

        إذا (sscanf(p, " "
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
                   "%*i " /* priority */
                   "%*i " /* nice */
                   "%*u " /* num_threads */
                   "%*u " /* itrealvalue */
                   "%lu ", /* starttime */
                   &llu) != 1)
                أرجع -EIO;

        إذا (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        أرجع 0;
}

صحيح pidref_get_start_time(ثابت PidRef *pid, usec_t *ret) {
        usec_t t;
        صحيح r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : عدم);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pid);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = t;

        أرجع 0;
}

صحيح get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ حرف *m = عدم;
        صحيح r;

        أكد(pid >= 0);
        أكد(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        أرجع parse_mode(m, ret);
}

ساكن صحيح pid_get_sigmask(pid_t pid, ثابت حرف *field_name, uint64_t *ret) {
        _cleanup_free_ حرف *field = عدم;
        صحيح r;

        أكد(pid >= 0);
        أكد(field_name);
        أكد(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        أرجع safe_atou64_full(field, 16, ret);
}

ساكن صحيح pidref_has_sigmask(ثابت PidRef *pidref, صحيح sig, ثابت حرف *field_name) {
        uint64_t mask;
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;
        إذا (!SIGNAL_VALID(sig))
                أرجع -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        أرجع BIT_SET(mask, sig - 1);
}

صحيح pidref_has_sigcgt(ثابت PidRef *pidref, صحيح sig) {
        أرجع pidref_has_sigmask(pidref, sig, "SigCgt");
}

صحيح pidref_has_sigblk(ثابت PidRef *pidref, صحيح sig) {
        أرجع pidref_has_sigmask(pidref, sig, "SigBlk");
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
صحيح pidref_wait_for_terminate_and_check(ثابت حرف *name, PidRef *pidref, WaitFlags flags) {
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;
        إذا (pidref->pid == 1 || pidref_is_self(pidref))
                أرجع -ECHILD;

        _cleanup_free_ حرف *buffer = عدم;
        إذا (!name) {
                r = pidref_get_comm(pidref, &buffer);
                إذا (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                وإلا
                        name = buffer;
        }

        صحيح prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        إذا (r < 0)
                أرجع log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        إذا (status.si_code == CLD_EXITED) {
                إذا (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                وإلا
                        log_debug("'%s' succeeded.", name);

                أرجع status.si_status;

        } وإلا إذا (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                أرجع log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        أرجع log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

صحيح kill_and_sigcont(pid_t pid, صحيح sig) {
        صحيح r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        إذا (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (فراغ) kill(pid, SIGCONT);

        أرجع r;
}

صحيح getenv_for_pid(pid_t pid, ثابت حرف *field, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        ثابت حرف *path;
        size_t sum = 0;
        صحيح r;

        أكد(pid >= 0);
        أكد(field);
        أكد(ret);

        إذا (pid == 0 || pid == getpid_cached())
                أرجع strdup_to_full(ret, getenv(field));

        إذا (!pid_is_valid(pid))
                أرجع -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        لكل (;;) {
                _cleanup_free_ حرف *قۇر = عدم;
                ثابت حرف *match;

                إذا (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        أرجع -ENOBUFS;

                r = read_nul_string(f, LONG_قۇر_MAX, &قۇر);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)  /* EOF */
                        اكسر;

                sum += r;

                match = startswith(قۇر, field);
                إذا (match && *match == '=')
                        أرجع strdup_to_full(ret, match + 1);
        }

        *ret = عدم;
        أرجع 0;
}

صحيح pidref_is_my_child(PidRef *pid) {
        صحيح r;

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        إذا (pid->pid == 1 || pidref_is_self(pid))
                أرجع false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        إذا (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                أرجع false;
        إذا (r < 0)
                أرجع r;

        أرجع ppid == getpid_cached();
}

صحيح pid_is_my_child(pid_t pid) {

        إذا (pid == 0)
                أرجع false;

        أرجع pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

صحيح pidref_is_unwaited(PidRef *pid) {
        صحيح r;

        /* Checks whether a PID is still valid at all, including a zombie */

        إذا (!pidref_is_set(pid))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pid))
                أرجع -EREMOTE;

        إذا (pid->pid == 1 || pidref_is_self(pid))
                أرجع true;

        r = pidref_kill(pid, 0);
        إذا (r == -ESRCH)
                أرجع false;
        إذا (r < 0)
                أرجع r;

        أرجع true;
}

صحيح pid_is_unwaited(pid_t pid) {

        إذا (pid == 0)
                أرجع true;

        أرجع pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

صحيح pid_is_alive(pid_t pid) {
        صحيح r;

        /* Checks whether a PID is still valid and not a zombie */

        إذا (pid < 0)
                أرجع -ESRCH;

        إذا (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                أرجع true;

        إذا (pid == getpid_cached())
                أرجع true;

        r = get_process_state(pid);
        إذا (r == -ESRCH)
                أرجع false;
        إذا (r < 0)
                أرجع r;

        أرجع r != 'Z';
}

صحيح pidref_is_alive(ثابت PidRef *pidref) {
        صحيح r, result;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        result = pid_is_alive(pidref->pid);
        إذا (result < 0) {
                أكد(result != -ESRCH);
                أرجع result;
        }

        r = pidref_verify(pidref);
        إذا (r == -ESRCH)
                أرجع false;
        إذا (r < 0)
                أرجع r;

        أرجع result;
}

صحيح pidref_from_same_يىلتىز_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        صحيح r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        إذا (!a || !b) {
                r = pidref_set_self(&self);
                إذا (r < 0)
                        أرجع r;
                إذا (!a)
                        a = &self;
                إذا (!b)
                        b = &self;
        }

        إذا (!pidref_is_set(a) || !pidref_is_set(b))
                أرجع -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        إذا (pidref_is_remote(a) && pidref_is_remote(b))
                أرجع -EREMOTE;
        إذا (pidref_is_remote(a) || pidref_is_remote(b))
                أرجع false;

        إذا (pidref_equal(a, b))
                أرجع true;

        ثابت حرف *roota = procfs_file_alloca(a->pid, "root");
        ثابت حرف *rootb = procfs_file_alloca(b->pid, "root");

        صحيح result = inode_same(roota, rootb, 0);
        إذا (result == -ENOENT)
                أرجع proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        إذا (result < 0)
                أرجع result;

        r = pidref_verify(a);
        إذا (r < 0)
                أرجع r;
        r = pidref_verify(b);
        إذا (r < 0)
                أرجع r;

        أرجع result;
}

bool is_main_thread(فراغ) {
        ساكن thread_local صحيح cached = -1;

        إذا (cached < 0)
                cached = getpid_cached() == gettid();

        أرجع cached;
}

unsigned long personality_from_string(ثابت حرف *s) {
        Architecture architecture;

        إذا (!s)
                أرجع PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        إذا (architecture < 0)
                أرجع PERSONALITY_INVALID;

        إذا (architecture == native_architecture())
                أرجع PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        إذا (architecture == ARCHITECTURE_SECONDARY)
                أرجع PER_LINUX32;
#endif

        أرجع PERSONALITY_INVALID;
}

ثابت حرف* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        إذا (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        وإلا إذا (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        إذا (architecture < 0)
                أرجع عدم;

        أرجع architecture_to_string(architecture);
}

صحيح safe_personality(unsigned long p) {
        صحيح ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        إذا (ret < 0) {
                إذا (errno != 0)
                        أرجع -errno;

                errno = -ret;
        }

        أرجع ret;
}

صحيح opinionated_personality(unsigned long *ret) {
        صحيح current;

        أكد(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        إذا (current < 0)
                أرجع current;

        إذا (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        وإلا
                *ret = PER_LINUX;

        أرجع 0;
}

فراغ valgrind_summary_hack(فراغ) {
#إذا HAVE_VALGRIND_VALGRIND_H
        إذا (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                إذا (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                وإلا إذا (pid == 0)
                        exit(EXIT_SUCCESS);
                وإلا {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (فراغ) pidref_set_pid(&pidref, pid);
                        (فراغ) pidref_wait_for_terminate(&pidref, عدم);
                }
        }
#endif
}

صحيح pid_compare_func(ثابت pid_t *a, ثابت pid_t *b) {
        /* Suitable for usage in qsort() */
        أكد(a);
        أكد(b);

        أرجع CMP(*a, *b);
}

bool nice_is_valid(صحيح n) {
        أرجع n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(صحيح policy) {
        أرجع IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(صحيح policy) {
        أرجع sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
صحيح sched_get_priority_min_safe(صحيح policy) {
        صحيح r;

        r = sched_get_priority_min(policy);
        إذا (r >= 0)
                أرجع r;

        /* Fallback priority */
        أرجع 0;
}

صحيح sched_get_priority_max_safe(صحيح policy) {
        صحيح r;

        r = sched_get_priority_max(policy);
        إذا (r >= 0)
                أرجع r;

        أرجع 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

ساكن pid_t cached_pid = CACHED_PID_UNSET;

فراغ reset_cached_pid(فراغ) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(فراغ) {
        ساكن bool installed = false;
        pid_t current_قىممەت;

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
        current_قىممەت = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        إذا (current_قىممەت > 0)
                أرجع current_قىممەت;

        current_قىممەت = CACHED_PID_UNSET;
        (فراغ) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_قىممەت,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        بدل (current_قىممەت) {

        حالة CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                إذا (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        إذا (pthread_atfork(عدم, عدم, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                أرجع new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                أرجع new_pid;
        }

        حالة CACHED_PID_BUSY: /* Somebody else is currently initializing */
                أرجع getpid();

        افتراضي: /* Properly initialized */
                أرجع current_قىممەت;
        }
}

صحيح must_be_يىلتىز(فراغ) {

        إذا (geteuid() == 0)
                أرجع 0;

        أرجع log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_دۆۋە(صحيح (*fn)(فراغ *), صحيح flags, فراغ *userdata) {
        size_t ps;
        pid_t pid;
        فراغ *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        أكد((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
                         CLONE_CHILD_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = page_size();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (فراغ*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#إذا HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#وإلا
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        إذا (pid < 0)
                أرجع -errno;

        أرجع pid;
}

ساكن صحيح fork_flags_to_signal(ForkFlags flags) {
        أرجع (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

صحيح pidref_safe_fork_full(
                ثابت حرف *name,
                ثابت صحيح stdio_fds[3],
                صحيح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = عدم;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ صحيح pidref_transport_fds[2] = EBADF_PAIR;
        صحيح prio, r;

        أكد(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        أكد(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        إذا (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        إذا (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } وإلا إذا (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        إذا (block_signals) {
                إذا (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        أرجع log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        إذا (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                إذا (r < 0)
                        أرجع log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                إذا (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        إذا (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                أرجع log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        إذا (pid < 0)
                                أرجع log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        إذا (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                إذا (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        إذا (r < 0)
                                                أرجع log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        إذا (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                أرجع -EPROTO;

                                        صحيح pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, حجم(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        إذا (n < 0)
                                                أرجع log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                أرجع 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        إذا ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        وإلا
                pid = fork();
        إذا (pid < 0)
                أرجع log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        إذا (pid > 0) {

                /* If we are in the intermediary process, exit now */
                إذا (intermediary) {
                        إذا (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                إذا (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, حجم(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                إذا (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                إذا (flags & FORK_WAIT) {
                        إذا (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (فراغ) sigprocmask(SIG_SETMASK, &ss, عدم);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                أرجع -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        إذا (ret)
                                *ret = PIDREF_NULL;

                        أرجع 1;
                }

                إذا (ret) {
                        r = pidref_set_pid(ret, pid);
                        إذا (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                أرجع 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = عدم;

        إذا (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        إذا (name) {
                r = rename_process(name);
                إذا (r < 0)
                        log_full_errno(flags & FORK_LOG ? LOG_WARNING : LOG_DEBUG,
                                       r, "Failed to rename process, ignoring: %m");
        }

        /* let's disable dlopen() in the child, as a paranoia safety precaution: children should not live for
         * long and only do minimal work before exiting or exec()ing. Doing dlopen() is not either. If people
         * want dlopen() they should do it before forking. This is a safety precaution in particular for
         * cases where the child does namespace shenanigans: we should never end up loading a module from a
         * foreign environment. Note that this has no effect on NSS! (i.e. it only has effect on uses of our
         * dlopen_safe(), which we use comprehensively in our codebase, but glibc NSS doesn't bother, of
         * course.) */
        إذا (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        إذا (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } وإلا إذا (block_signals) { /* undo what we did above */
                إذا (sigprocmask(SIG_SETMASK, &saved_ss, عدم) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                إذا (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                وإلا إذا (ppid != original_pid) {
                        صحيح sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (فراغ) ارفع(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                إذا (mount(عدم, "/", عدم, MS_SLAVE | MS_REC, عدم) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                أكد(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (flags & FORK_REARRANGE_STDIO) {
                إذا (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        إذا (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } وإلا {
                        r = make_null_stdio();
                        إذا (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } وإلا إذا (flags & FORK_STDOUT_TO_STDERR) {
                إذا (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                أكد(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        إذا (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        إذا (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        إذا (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        إذا (ret) {
                r = pidref_set_self(ret);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        أرجع 0;
}

صحيح namespace_fork_full(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                صحيح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح يىلتىز_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ صحيح errno_pipe_fd[2] = EBADF_PAIR;
        صحيح r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

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
        أكد(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(حجم(صحيح) <= PIPE_BUF);
        إذا (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                أرجع log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ عدم, /* except_fds= */ عدم, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        إذا (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                صحيح k = read_errno(errno_pipe_fd[0]);
                إذا (k < 0 && k != -EIO)
                        أرجع k;
        }
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, يىلتىز_fd, namespace_flags);
                إذا (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                عدم,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                إذا (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                إذا (r == 0) {
                        /* Child */

                        إذا (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } وإلا {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        إذا (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        أرجع 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (فراغ) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                إذا (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        إذا (r < 0)
                أرجع r; /* the child logs about failures on its own, no need to duplicate here */

        إذا (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        وإلا
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        أرجع 1;
}

صحيح namespace_fork(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                ForkFlags flags,
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح يىلتىز_fd,
                PidRef *ret) {

        أرجع namespace_fork_full(outer_name, inner_name, /* except_fds= */ عدم, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, يىلتىز_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(صحيح oa) {
        أرجع oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

صحيح set_oom_score_adjust(صحيح قىممەت) {
        حرف t[DECIMAL_STR_MAX(صحيح)];

        إذا (!oom_score_adjust_is_valid(قىممەت))
                أرجع -EINVAL;

        xsprintf(t, "%i", قىممەت);

        أرجع write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

صحيح get_oom_score_adjust(صحيح *ret) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, عدم);
        إذا (r < 0)
                أرجع r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        إذا (r < 0)
                أرجع r;

        إذا (!oom_score_adjust_is_valid(a))
                أرجع -ENODATA;

        إذا (ret)
                *ret = a;

        أرجع 0;
}

ساكن صحيح rlimit_to_nice(rlim_t limit) {
        إذا (limit <= 1)
                أرجع PRIO_MAX-1; /* i.e. 19 */

        إذا (limit >= -PRIO_MIN + PRIO_MAX)
                أرجع PRIO_MIN; /* i.e. -20 */

        أرجع PRIO_MAX - (صحيح) limit;
}

صحيح setpriority_closest(صحيح priority) {
        بنية rlimit highest;
        صحيح r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        إذا (r >= 0)
                أرجع 1;
        إذا (!ERRNO_IS_NEG_PRIVILEGE(r))
                أرجع r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        إذا (errno != 0)
                أرجع -errno;

        إذا (priority == current)
                أرجع 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        إذا (priority > current)
                أرجع r;

        إذا (getrlimit(RLIMIT_NICE, &highest) < 0)
                أرجع -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        إذا (current > limit)
                إذا (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        أرجع -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        أرجع 0;
}

_noreturn_ فراغ freeze(فراغ) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (فراغ) close_all_fds_without_malloc(عدم, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        لكل (;;) {
                siginfo_t si = {};

                إذا (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        اكسر;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        لكل (;;)
                pause();
}

صحيح get_process_threads(pid_t pid) {
        _cleanup_free_ حرف *t = عدم;
        صحيح n, r;

        إذا (pid < 0)
                أرجع -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        r = safe_atoi(t, &n);
        إذا (r < 0)
                أرجع r;
        إذا (n < 0)
                أرجع -EINVAL;

        أرجع n;
}

صحيح is_reaper_process(فراغ) {
        صحيح b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        إذا (getpid_cached() == 1)
                أرجع true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        إذا (r < 0)
                أرجع r;

        أرجع b != 0;
}

صحيح make_reaper_process(bool b) {
        صحيح r;

        إذا (getpid_cached() == 1) {

                إذا (!b)
                        أرجع -EINVAL;

                أرجع 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        إذا (r < 0)
                أرجع r;

        أرجع 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, عدم);

صحيح posix_spawn_wrapper(
                ثابت حرف *path,
                حرف * ثابت *argv,
                حرف * ثابت *envp,
                ثابت حرف *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        صحيح r;

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

        أكد(path);
        أكد(argv);
        أكد(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        إذا (r != 0)
                أرجع -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        ساكن bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ صحيح cgroup_fd = -EBADF;

        إذا (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ حرف *resolved_cgroup = عدم;

                r = cg_get_path(cgroup, /* suffix= */ عدم, &resolved_cgroup);
                إذا (r < 0)
                        أرجع r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                إذا (cgroup_fd < 0)
                        أرجع -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                إذا (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                وإلا إذا (r != ENOSYS)
                        أرجع -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        إذا (r != 0)
                أرجع -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        إذا (r != 0)
                أرجع -r;

        _cleanup_close_ صحيح pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, عدم, &attr, argv, envp);
        إذا (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                أرجع -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        إذا ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
            FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP)) {
                /* Compiled on a newer host, or seccomp&friends blocking clone3()? Fallback, but
                 * need to disable POSIX_SPAWN_SETCGROUP, which is what redirects to clone3().
                 * CLONE_INTO_CGROUP definitely won't work, hence remember the fact so that we don't
                 * retry every time.
                 * Note, CLONE_INTO_CGROUP is supported since kernel v5.7, but some architectures still
                 * do not support clone3(). Hence, we need to keep the fallback logic for a while. */
                have_clone_into_cgroup = false;

                flags &= ~POSIX_SPAWN_SETCGROUP;
                r = posix_spawnattr_setflags(&attr, flags);
                إذا (r != 0)
                        أرجع -r;

                r = pidfd_spawn(&pidfd, path, عدم, &attr, argv, envp);
        }
        إذا (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                إذا (r < 0)
                        أرجع r;

                أرجع FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        إذا (!ERRNO_IS_NOT_SUPPORTED(r))
                أرجع -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, عدم, &attr, argv, envp);
        إذا (r != 0)
                أرجع -r;

        r = pidref_set_pid(ret_pidref, pid);
        إذا (r < 0)
                أرجع r;

        أرجع 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

صحيح proc_dir_open(DIR **ret) {
        DIR *d;

        أكد(ret);

        d = opendir("/proc");
        إذا (!d)
                أرجع -errno;

        *ret = d;
        أرجع 0;
}

صحيح proc_dir_read(DIR *d, pid_t *ret) {
        أكد(d);

        لكل (;;) {
                بنية dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                إذا (!de) {
                        إذا (errno != 0)
                                أرجع -errno;

                        اكسر;
                }

                إذا (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        استمر;

                إذا (parse_pid(de->d_name, ret) >= 0)
                        أرجع 1;
        }

        إذا (ret)
                *ret = 0;
        أرجع 0;
}

صحيح proc_dir_read_pidref(DIR *d, PidRef *ret) {
        صحيح r;

        أكد(d);

        لكل (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                r = pidref_set_pid(ret, pid);
                إذا (r == -ESRCH) /* gone by now? skip it */
                        استمر;
                إذا (r < 0)
                        أرجع r;

                أرجع 1;
        }

        إذا (ret)
                *ret = PIDREF_NULL;
        أرجع 0;
}

صحيح safe_mlockall(صحيح flags) {
        صحيح r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        إذا (r == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        إذا (mlockall(flags) < 0)
                أرجع log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        أرجع 0;
}

ساكن ثابت حرف *ثابت sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, صحيح);

ساكن ثابت حرف* ثابت sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, صحيح, INT_MAX);

_noreturn_ فراغ report_errno_and_exit(صحيح errno_fd, صحيح error) {
        صحيح r;

        إذا (error >= 0)
                _exit(EXIT_SUCCESS);

        أكد(errno_fd >= 0);

        r = loop_write(errno_fd, &error, حجم(error));
        إذا (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

صحيح read_errno(صحيح errno_fd) {
        صحيح r;

        أكد(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, حجم(r), /* do_poll= */ false);
        إذا (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                أرجع -EIO;
        }
        إذا (n == 0) /* the process exited without reporting an error, assuming success */
                أرجع 0;
        إذا (n != حجم(r))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        إذا (r == 0)
                أرجع 0;
        إذا (r < 0) /* child process reported an error, return it */
                أرجع log_debug_errno(r, "Child process failed with errno: %m");

        أرجع log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

صحيح prctl_safe(صحيح op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        أرجع RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

صحيح proc_set_comm(ثابت حرف *comm) {
        أرجع prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

صحيح proc_set_nnp(فراغ) {
        أرجع prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(فراغ) {
        صحيح r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        إذا (r < 0)
                أرجع NSEC_INFINITY;

        أرجع r;
}
