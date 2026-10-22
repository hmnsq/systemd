/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/oom.h>
#અન્તર્ભાવ <pthread.h>
#અન્તર્ભાવ <spawn.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <sys/mman.h>
#અન્તર્ભાવ <sys/mount.h>
#અન્તર્ભાવ <sys/personality.h>
#અન્તર્ભાવ <sys/prctl.h>
#અન્તર્ભાવ <sys/wait.h>
#અન્તર્ભાવ <syslog.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>
#જો HAVE_VALGRIND_VALGRIND_H
#અન્તર્ભાવ <valgrind/valgrind.h>
#endif

#અન્તર્ભાવ "sd-messages.h"

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "architecture.h"
#અન્તર્ભાવ "argv-util.h"
#અન્તર્ભાવ "bitfield.h"
#અન્તર્ભાવ "capability-util.h"
#અન્તર્ભાવ "cgroup-util.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "dlfcn-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "locale-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "mountpoint-util.h"
#અન્તર્ભાવ "namespace-util.h"
#અન્તર્ભાવ "nulstr-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidfd-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "raw-clone.h"
#અન્તર્ભાવ "rlimit-util.h"
#અન્તર્ભાવ "signal-util.h"
#અન્તર્ભાવ "socket-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

સ્થિર પૂર્ણાંક get_process_state(pid_t pid) {
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        અચલ અક્ષર *p;
        અક્ષર state;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        જો (pid == 0 || pid == getpid_cached())
                પાછા (unsigned અક્ષર) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_લીટી_file(p, &લીટી);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        p = strrchr(લીટી, ')');
        જો (!p)
                પાછા -EIO;

        p++;

        જો (sscanf(p, " %c", &state) != 1)
                પાછા -EIO;

        પાછા (unsigned અક્ષર) state;
}

પૂર્ણાંક pid_get_comm(pid_t pid, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *escaped = ખાલી, *comm = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

        જો (pid == 0 || pid == getpid_cached()) {
                comm = new0(અક્ષર, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                જો (!comm)
                        પાછા -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                જો (r < 0)
                        પાછા r;
        } નહીંતર {
                અચલ અક્ષર *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_લીટી_file(p, &comm);
                જો (r == -ENOENT)
                        પાછા -ESRCH;
                જો (r < 0)
                        પાછા r;
        }

        escaped = new(અક્ષર, COMM_MAX_LEN);
        જો (!escaped)
                પાછા -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        પાછા 0;
}

પૂર્ણાંક pidref_get_comm(અચલ PidRef *pid, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *comm = ખાલી;
        પૂર્ણાંક r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pid);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = TAKE_PTR(comm);
        પાછા 0;
}

સ્થિર પૂર્ણાંક pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                અક્ષર **ret,
                size_t *ret_size) {

        _cleanup_free_ અક્ષર *t = ખાલી;
        અચલ અક્ષર *p;
        size_t k;
        પૂર્ણાંક r;

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
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        જો (k == 0) {
                જો (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        પાછા -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ અક્ષર *comm = ખાલી;

                r = pid_get_comm(pid, &comm);
                જો (r < 0)
                        પાછા r;

                મુક્ત(t);
                t = strjoin("[", comm, "]");
                જો (!t)
                        પાછા -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                જો (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        જો (ret)
                *ret = TAKE_PTR(t);
        જો (ret_size)
                *ret_size = k;

        પાછા r;
}

પૂર્ણાંક pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        size_t k;
        અક્ષર *ans;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

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

        પૂર્ણાંક full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        જો (full < 0)
                પાછા full;

        જો (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                પ્રતિજ્ઞા(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ અક્ષર **args = ખાલી;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                જો (!args)
                        પાછા -ENOMEM;

                ans = quote_command_લીટી(args, shflags);
                જો (!ans)
                        પાછા -ENOMEM;
        } નહીંતર {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                માટે (size_t i = 0; i < k - 1; i++)
                        જો (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                જો (!ans)
                        પાછા -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        પાછા 0;
}

પૂર્ણાંક pidref_get_cmdline(અચલ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pid);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, અક્ષર ***ret) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        અક્ષર **args;
        size_t k;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        પ્રતિજ્ઞા(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        જો (r < 0)
                પાછા r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        જો (!args)
                પાછા -ENOMEM;

        *ret = args;
        પાછા 0;
}

પૂર્ણાંક pidref_get_cmdline_strv(અચલ PidRef *pid, ProcessCmdlineFlags flags, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **args = ખાલી;
        પૂર્ણાંક r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pid);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = TAKE_PTR(args);

        પાછા 0;
}

પૂર્ણાંક procfs_get_stat_flags(અચલ અક્ષર *path, unsigned long long *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        r = read_one_લીટી_file(path, &લીટી);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        /* Skip past the comm field */
        અક્ષર *q = strrchr(લીટી, ')');
        જો (!q)
                પાછા -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        માટે (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                જો (l < 1)
                        પાછા -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                જો (l < 1)
                        પાછા -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        જો (l < 1)
                પાછા -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        જો (l < 1)
                પાછા -EINVAL;
        q[l] = 0;

        પાછા safe_atollu(q, ret);
}

પૂર્ણાંક pid_is_kernel_thread(pid_t pid) {
        પૂર્ણાંક r;

        જો (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                પાછા 0;
        જો (!pid_is_valid(pid))
                પાછા -EINVAL;

        અચલ અક્ષર *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        જો (r < 0)
                પાછા r;

        પાછા !!(flags & PF_KTHREAD);
}

પૂર્ણાંક pidref_is_kernel_thread(અચલ PidRef *pid) {
        પૂર્ણાંક result, r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        જો (result < 0)
                પાછા result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        જો (r < 0)
                પાછા r;

        પાછા result;
}

સ્થિર પૂર્ણાંક get_process_link_contents(pid_t pid, અચલ અક્ષર *proc_file, અક્ષર **ret) {
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        પાછા (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

પૂર્ણાંક get_process_exe(pid_t pid, અક્ષર **ret) {
        અક્ષર *d;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        જો (r < 0)
                પાછા r;

        જો (ret) {
                d = endswith(*ret, " (deleted)");
                જો (d)
                        *d = '\0';
        }

        પાછા 0;
}

પૂર્ણાંક pid_get_uid(pid_t pid, uid_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

        જો (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                પાછા 0;
        }

        _cleanup_free_ અક્ષર *v = ખાલી;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        પાછા parse_uid(v, ret);
}

પૂર્ણાંક pidref_get_uid(અચલ PidRef *pid, uid_t *ret) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        જો (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                જો (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        પાછા r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pid);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = uid;
        પાછા 0;
}

પૂર્ણાંક get_process_gid(pid_t pid, gid_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

        જો (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                પાછા 0;
        }

        _cleanup_free_ અક્ષર *v = ખાલી;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        પાછા parse_gid(v, ret);
}

પૂર્ણાંક get_process_cwd(pid_t pid, અક્ષર **ret) {
        પ્રતિજ્ઞા(pid >= 0);

        જો (pid == 0 || pid == getpid_cached())
                પાછા safe_getcwd(ret);

        પાછા get_process_link_contents(pid, "cwd", ret);
}

પૂર્ણાંક get_process_મૂળ(pid_t pid, અક્ષર **ret) {
        પ્રતિજ્ઞા(pid >= 0);
        પાછા get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

પૂર્ણાંક get_process_environ(pid_t pid, અક્ષર **ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_free_ અક્ષર *outcome = ખાલી;
        size_t sz = 0;
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                અક્ષર c;

                જો (sz >= ENVIRONMENT_BLOCK_MAX)
                        પાછા -ENOBUFS;

                જો (!GREEDY_REALLOC(outcome, sz + 5))
                        પાછા -ENOMEM;

                r = safe_fgetc(f, &c);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                જો (c == '\0')
                        outcome[sz++] = '\n';
                નહીંતર
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        પાછા 0;
}

પૂર્ણાંક pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        unsigned long ppid;
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);

        જો (pid == 0)
                pid = getpid_cached();
        જો (pid == 1) /* PID 1 has no parent, shortcut this case */
                પાછા -EADDRNOTAVAIL;

        જો (pid == getpid_cached()) {
                જો (ret)
                        *ret = getppid();
                પાછા 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_લીટી_file(p, &લીટી);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(લીટી, ')');
        જો (!p)
                પાછા -EIO;
        p++;

        જો (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                પાછા -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        જો (ppid == 0)
                પાછા -EADDRNOTAVAIL;

        જો ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                પાછા -ERANGE;

        જો (ret)
                *ret = (pid_t) ppid;

        પાછા 0;
}

પૂર્ણાંક pidref_get_ppid(અચલ PidRef *pidref, pid_t *ret) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                જો (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        પાછા r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ખાલી);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = ppid;
        પાછા 0;
}

પૂર્ણાંક pidref_get_ppid_as_pidref(અચલ PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        પૂર્ણાંક r;

        POINTER_MAY_BE_NULL(pidref);
        પ્રતિજ્ઞા(ret);

        r = pidref_get_ppid(pidref, &ppid);
        જો (r < 0)
                પાછા r;

        માટે (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                જો (r < 0)
                        પાછા r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                જો (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        જો (r < 0)
                                પાછા r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        જો (parent.pid != ppid)
                                ચાલુ;
                }

                *ret = TAKE_PIDREF(parent);
                પાછા 0;
        }

        /* Give up after 16 tries */
        પાછા -ENOTRECOVERABLE;
}

પૂર્ણાંક pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_લીટી_file(p, &લીટી);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(લીટી, ')');
        જો (!p)
                પાછા -EIO;
        p++;

        unsigned long llu;

        જો (sscanf(p, " "
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
                પાછા -EIO;

        જો (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        પાછા 0;
}

પૂર્ણાંક pidref_get_start_time(અચલ PidRef *pid, usec_t *ret) {
        usec_t t;
        પૂર્ણાંક r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : ખાલી);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pid);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = t;

        પાછા 0;
}

પૂર્ણાંક get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ અક્ષર *m = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        પાછા parse_mode(m, ret);
}

સ્થિર પૂર્ણાંક pid_get_sigmask(pid_t pid, અચલ અક્ષર *field_name, uint64_t *ret) {
        _cleanup_free_ અક્ષર *field = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(field_name);
        પ્રતિજ્ઞા(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        પાછા safe_atou64_full(field, 16, ret);
}

સ્થિર પૂર્ણાંક pidref_has_sigmask(અચલ PidRef *pidref, પૂર્ણાંક sig, અચલ અક્ષર *field_name) {
        uint64_t mask;
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;
        જો (!SIGNAL_VALID(sig))
                પાછા -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        પાછા BIT_SET(mask, sig - 1);
}

પૂર્ણાંક pidref_has_sigcgt(અચલ PidRef *pidref, પૂર્ણાંક sig) {
        પાછા pidref_has_sigmask(pidref, sig, "SigCgt");
}

પૂર્ણાંક pidref_has_sigblk(અચલ PidRef *pidref, પૂર્ણાંક sig) {
        પાછા pidref_has_sigmask(pidref, sig, "SigBlk");
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
પૂર્ણાંક pidref_wait_for_terminate_and_check(અચલ અક્ષર *name, PidRef *pidref, WaitFlags flags) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;
        જો (pidref->pid == 1 || pidref_is_self(pidref))
                પાછા -ECHILD;

        _cleanup_free_ અક્ષર *buffer = ખાલી;
        જો (!name) {
                r = pidref_get_comm(pidref, &buffer);
                જો (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                નહીંતર
                        name = buffer;
        }

        પૂર્ણાંક prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        જો (r < 0)
                પાછા log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        જો (status.si_code == CLD_EXITED) {
                જો (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                નહીંતર
                        log_debug("'%s' succeeded.", name);

                પાછા status.si_status;

        } નહીંતર જો (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                પાછા log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        પાછા log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

પૂર્ણાંક kill_and_sigcont(pid_t pid, પૂર્ણાંક sig) {
        પૂર્ણાંક r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        જો (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (શૂન્ય) kill(pid, SIGCONT);

        પાછા r;
}

પૂર્ણાંક getenv_for_pid(pid_t pid, અચલ અક્ષર *field, અક્ષર **ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        અચલ અક્ષર *path;
        size_t sum = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(field);
        પ્રતિજ્ઞા(ret);

        જો (pid == 0 || pid == getpid_cached())
                પાછા strdup_to_full(ret, getenv(field));

        જો (!pid_is_valid(pid))
                પાછા -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;
                અચલ અક્ષર *match;

                જો (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        પાછા -ENOBUFS;

                r = read_nul_string(f, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)  /* EOF */
                        તોડો;

                sum += r;

                match = startswith(લીટી, field);
                જો (match && *match == '=')
                        પાછા strdup_to_full(ret, match + 1);
        }

        *ret = ખાલી;
        પાછા 0;
}

પૂર્ણાંક pidref_is_my_child(PidRef *pid) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        જો (pid->pid == 1 || pidref_is_self(pid))
                પાછા false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        જો (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                પાછા false;
        જો (r < 0)
                પાછા r;

        પાછા ppid == getpid_cached();
}

પૂર્ણાંક pid_is_my_child(pid_t pid) {

        જો (pid == 0)
                પાછા false;

        પાછા pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

પૂર્ણાંક pidref_is_unwaited(PidRef *pid) {
        પૂર્ણાંક r;

        /* Checks whether a PID is still valid at all, including a zombie */

        જો (!pidref_is_set(pid))
                પાછા -ESRCH;

        જો (pidref_is_remote(pid))
                પાછા -EREMOTE;

        જો (pid->pid == 1 || pidref_is_self(pid))
                પાછા true;

        r = pidref_kill(pid, 0);
        જો (r == -ESRCH)
                પાછા false;
        જો (r < 0)
                પાછા r;

        પાછા true;
}

પૂર્ણાંક pid_is_unwaited(pid_t pid) {

        જો (pid == 0)
                પાછા true;

        પાછા pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

પૂર્ણાંક pid_is_alive(pid_t pid) {
        પૂર્ણાંક r;

        /* Checks whether a PID is still valid and not a zombie */

        જો (pid < 0)
                પાછા -ESRCH;

        જો (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                પાછા true;

        જો (pid == getpid_cached())
                પાછા true;

        r = get_process_state(pid);
        જો (r == -ESRCH)
                પાછા false;
        જો (r < 0)
                પાછા r;

        પાછા r != 'Z';
}

પૂર્ણાંક pidref_is_alive(અચલ PidRef *pidref) {
        પૂર્ણાંક r, result;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        result = pid_is_alive(pidref->pid);
        જો (result < 0) {
                પ્રતિજ્ઞા(result != -ESRCH);
                પાછા result;
        }

        r = pidref_verify(pidref);
        જો (r == -ESRCH)
                પાછા false;
        જો (r < 0)
                પાછા r;

        પાછા result;
}

પૂર્ણાંક pidref_from_same_મૂળ_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        પૂર્ણાંક r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        જો (!a || !b) {
                r = pidref_set_self(&self);
                જો (r < 0)
                        પાછા r;
                જો (!a)
                        a = &self;
                જો (!b)
                        b = &self;
        }

        જો (!pidref_is_set(a) || !pidref_is_set(b))
                પાછા -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        જો (pidref_is_remote(a) && pidref_is_remote(b))
                પાછા -EREMOTE;
        જો (pidref_is_remote(a) || pidref_is_remote(b))
                પાછા false;

        જો (pidref_equal(a, b))
                પાછા true;

        અચલ અક્ષર *roota = procfs_file_alloca(a->pid, "root");
        અચલ અક્ષર *rootb = procfs_file_alloca(b->pid, "root");

        પૂર્ણાંક result = inode_same(roota, rootb, 0);
        જો (result == -ENOENT)
                પાછા proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        જો (result < 0)
                પાછા result;

        r = pidref_verify(a);
        જો (r < 0)
                પાછા r;
        r = pidref_verify(b);
        જો (r < 0)
                પાછા r;

        પાછા result;
}

bool is_main_thread(શૂન્ય) {
        સ્થિર thread_local પૂર્ણાંક cached = -1;

        જો (cached < 0)
                cached = getpid_cached() == gettid();

        પાછા cached;
}

unsigned long personality_from_string(અચલ અક્ષર *s) {
        Architecture architecture;

        જો (!s)
                પાછા PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        જો (architecture < 0)
                પાછા PERSONALITY_INVALID;

        જો (architecture == native_architecture())
                પાછા PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        જો (architecture == ARCHITECTURE_SECONDARY)
                પાછા PER_LINUX32;
#endif

        પાછા PERSONALITY_INVALID;
}

અચલ અક્ષર* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        જો (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        નહીંતર જો (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        જો (architecture < 0)
                પાછા ખાલી;

        પાછા architecture_to_string(architecture);
}

પૂર્ણાંક safe_personality(unsigned long p) {
        પૂર્ણાંક ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        જો (ret < 0) {
                જો (errno != 0)
                        પાછા -errno;

                errno = -ret;
        }

        પાછા ret;
}

પૂર્ણાંક opinionated_personality(unsigned long *ret) {
        પૂર્ણાંક current;

        પ્રતિજ્ઞા(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        જો (current < 0)
                પાછા current;

        જો (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        નહીંતર
                *ret = PER_LINUX;

        પાછા 0;
}

શૂન્ય valgrind_summary_hack(શૂન્ય) {
#જો HAVE_VALGRIND_VALGRIND_H
        જો (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                જો (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                નહીંતર જો (pid == 0)
                        exit(EXIT_SUCCESS);
                નહીંતર {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (શૂન્ય) pidref_set_pid(&pidref, pid);
                        (શૂન્ય) pidref_wait_for_terminate(&pidref, ખાલી);
                }
        }
#endif
}

પૂર્ણાંક pid_compare_func(અચલ pid_t *a, અચલ pid_t *b) {
        /* Suitable for usage in qsort() */
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા CMP(*a, *b);
}

bool nice_is_valid(પૂર્ણાંક n) {
        પાછા n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(પૂર્ણાંક policy) {
        પાછા IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(પૂર્ણાંક policy) {
        પાછા sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
પૂર્ણાંક sched_get_priority_min_safe(પૂર્ણાંક policy) {
        પૂર્ણાંક r;

        r = sched_get_priority_min(policy);
        જો (r >= 0)
                પાછા r;

        /* Fallback priority */
        પાછા 0;
}

પૂર્ણાંક sched_get_priority_max_safe(પૂર્ણાંક policy) {
        પૂર્ણાંક r;

        r = sched_get_priority_max(policy);
        જો (r >= 0)
                પાછા r;

        પાછા 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

સ્થિર pid_t cached_pid = CACHED_PID_UNSET;

શૂન્ય reset_cached_pid(શૂન્ય) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(શૂન્ય) {
        સ્થિર bool installed = false;
        pid_t current_કિંમત;

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
        current_કિંમત = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        જો (current_કિંમત > 0)
                પાછા current_કિંમત;

        current_કિંમત = CACHED_PID_UNSET;
        (શૂન્ય) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_કિંમત,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        પસંદગી (current_કિંમત) {

        સ્થિતિ CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                જો (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        જો (pthread_atfork(ખાલી, ખાલી, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                પાછા new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                પાછા new_pid;
        }

        સ્થિતિ CACHED_PID_BUSY: /* Somebody else is currently initializing */
                પાછા getpid();

        મૂળભૂત: /* Properly initialized */
                પાછા current_કિંમત;
        }
}

પૂર્ણાંક must_be_મૂળ(શૂન્ય) {

        જો (geteuid() == 0)
                પાછા 0;

        પાછા log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ઢગલો(પૂર્ણાંક (*fn)(શૂન્ય *), પૂર્ણાંક flags, શૂન્ય *userdata) {
        size_t ps;
        pid_t pid;
        શૂન્ય *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        પ્રતિજ્ઞા((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (શૂન્ય*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#જો HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#નહીંતર
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        જો (pid < 0)
                પાછા -errno;

        પાછા pid;
}

સ્થિર પૂર્ણાંક fork_flags_to_signal(ForkFlags flags) {
        પાછા (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

પૂર્ણાંક pidref_safe_fork_full(
                અચલ અક્ષર *name,
                અચલ પૂર્ણાંક stdio_fds[3],
                પૂર્ણાંક except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = ખાલી;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ પૂર્ણાંક pidref_transport_fds[2] = EBADF_PAIR;
        પૂર્ણાંક prio, r;

        પ્રતિજ્ઞા(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        પ્રતિજ્ઞા(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        જો (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        જો (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } નહીંતર જો (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        જો (block_signals) {
                જો (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        પાછા log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        જો (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                જો (r < 0)
                        પાછા log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                જો (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        જો (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                પાછા log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        જો (pid < 0)
                                પાછા log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        જો (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                જો (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        જો (r < 0)
                                                પાછા log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        જો (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                પાછા -EPROTO;

                                        પૂર્ણાંક pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, કદ(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        જો (n < 0)
                                                પાછા log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                પાછા 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        જો ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        નહીંતર
                pid = fork();
        જો (pid < 0)
                પાછા log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        જો (pid > 0) {

                /* If we are in the intermediary process, exit now */
                જો (intermediary) {
                        જો (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                જો (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, કદ(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                જો (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                જો (flags & FORK_WAIT) {
                        જો (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (શૂન્ય) sigprocmask(SIG_SETMASK, &ss, ખાલી);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        જો (r < 0)
                                પાછા r;
                        જો (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                પાછા -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        જો (ret)
                                *ret = PIDREF_NULL;

                        પાછા 1;
                }

                જો (ret) {
                        r = pidref_set_pid(ret, pid);
                        જો (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                પાછા 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ખાલી;

        જો (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        જો (name) {
                r = rename_process(name);
                જો (r < 0)
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
        જો (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        જો (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } નહીંતર જો (block_signals) { /* undo what we did above */
                જો (sigprocmask(SIG_SETMASK, &saved_ss, ખાલી) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                જો (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                નહીંતર જો (ppid != original_pid) {
                        પૂર્ણાંક sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (શૂન્ય) ઉઠાવો(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        જો (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                જો (mount(ખાલી, "/", ખાલી, MS_SLAVE | MS_REC, ખાલી) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                પ્રતિજ્ઞા(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (flags & FORK_REARRANGE_STDIO) {
                જો (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        જો (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } નહીંતર {
                        r = make_null_stdio();
                        જો (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } નહીંતર જો (flags & FORK_STDOUT_TO_STDERR) {
                જો (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                પ્રતિજ્ઞા(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        જો (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        જો (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        જો (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        જો (ret) {
                r = pidref_set_self(ret);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        પાછા 0;
}

પૂર્ણાંક namespace_fork_full(
                અચલ અક્ષર *outer_name,
                અચલ અક્ષર *inner_name,
                પૂર્ણાંક except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                પૂર્ણાંક pidns_fd,
                પૂર્ણાંક mntns_fd,
                પૂર્ણાંક netns_fd,
                પૂર્ણાંક userns_fd,
                પૂર્ણાંક મૂળ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ પૂર્ણાંક errno_pipe_fd[2] = EBADF_PAIR;
        પૂર્ણાંક r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        પ્રતિજ્ઞા((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        પ્રતિજ્ઞા((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        પ્રતિજ્ઞા(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(કદ(પૂર્ણાંક) <= PIPE_BUF);
        જો (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                પાછા log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ ખાલી, /* except_fds= */ ખાલી, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        જો (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                પૂર્ણાંક k = read_errno(errno_pipe_fd[0]);
                જો (k < 0 && k != -EIO)
                        પાછા k;
        }
        જો (r < 0)
                પાછા r;
        જો (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, મૂળ_fd, namespace_flags);
                જો (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                ખાલી,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                જો (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                જો (r == 0) {
                        /* Child */

                        જો (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } નહીંતર {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        જો (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        પાછા 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (શૂન્ય) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                જો (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        જો (r < 0)
                પાછા r; /* the child logs about failures on its own, no need to duplicate here */

        જો (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        નહીંતર
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        પાછા 1;
}

પૂર્ણાંક namespace_fork(
                અચલ અક્ષર *outer_name,
                અચલ અક્ષર *inner_name,
                ForkFlags flags,
                પૂર્ણાંક pidns_fd,
                પૂર્ણાંક mntns_fd,
                પૂર્ણાંક netns_fd,
                પૂર્ણાંક userns_fd,
                પૂર્ણાંક મૂળ_fd,
                PidRef *ret) {

        પાછા namespace_fork_full(outer_name, inner_name, /* except_fds= */ ખાલી, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, મૂળ_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(પૂર્ણાંક oa) {
        પાછા oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

પૂર્ણાંક set_oom_score_adjust(પૂર્ણાંક કિંમત) {
        અક્ષર t[DECIMAL_STR_MAX(પૂર્ણાંક)];

        જો (!oom_score_adjust_is_valid(કિંમત))
                પાછા -EINVAL;

        xsprintf(t, "%i", કિંમત);

        પાછા write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

પૂર્ણાંક get_oom_score_adjust(પૂર્ણાંક *ret) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        પૂર્ણાંક r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ખાલી);
        જો (r < 0)
                પાછા r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        જો (r < 0)
                પાછા r;

        જો (!oom_score_adjust_is_valid(a))
                પાછા -ENODATA;

        જો (ret)
                *ret = a;

        પાછા 0;
}

સ્થિર પૂર્ણાંક rlimit_to_nice(rlim_t limit) {
        જો (limit <= 1)
                પાછા PRIO_MAX-1; /* i.e. 19 */

        જો (limit >= -PRIO_MIN + PRIO_MAX)
                પાછા PRIO_MIN; /* i.e. -20 */

        પાછા PRIO_MAX - (પૂર્ણાંક) limit;
}

પૂર્ણાંક setpriority_closest(પૂર્ણાંક priority) {
        સંરચના rlimit highest;
        પૂર્ણાંક r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        જો (r >= 0)
                પાછા 1;
        જો (!ERRNO_IS_NEG_PRIVILEGE(r))
                પાછા r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        જો (errno != 0)
                પાછા -errno;

        જો (priority == current)
                પાછા 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        જો (priority > current)
                પાછા r;

        જો (getrlimit(RLIMIT_NICE, &highest) < 0)
                પાછા -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        જો (current > limit)
                જો (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        પાછા -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        પાછા 0;
}

_noreturn_ શૂન્ય freeze(શૂન્ય) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (શૂન્ય) close_all_fds_without_malloc(ખાલી, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        માટે (;;) {
                siginfo_t si = {};

                જો (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        તોડો;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        માટે (;;)
                pause();
}

પૂર્ણાંક get_process_threads(pid_t pid) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        પૂર્ણાંક n, r;

        જો (pid < 0)
                પાછા -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        r = safe_atoi(t, &n);
        જો (r < 0)
                પાછા r;
        જો (n < 0)
                પાછા -EINVAL;

        પાછા n;
}

પૂર્ણાંક is_reaper_process(શૂન્ય) {
        પૂર્ણાંક b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        જો (getpid_cached() == 1)
                પાછા true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        જો (r < 0)
                પાછા r;

        પાછા b != 0;
}

પૂર્ણાંક make_reaper_process(bool b) {
        પૂર્ણાંક r;

        જો (getpid_cached() == 1) {

                જો (!b)
                        પાછા -EINVAL;

                પાછા 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        જો (r < 0)
                પાછા r;

        પાછા 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, ખાલી);

પૂર્ણાંક posix_spawn_wrapper(
                અચલ અક્ષર *path,
                અક્ષર * અચલ *argv,
                અક્ષર * અચલ *envp,
                અચલ અક્ષર *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        પૂર્ણાંક r;

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

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(argv);
        પ્રતિજ્ઞા(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        જો (r != 0)
                પાછા -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        સ્થિર bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ પૂર્ણાંક cgroup_fd = -EBADF;

        જો (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ અક્ષર *resolved_cgroup = ખાલી;

                r = cg_get_path(cgroup, /* suffix= */ ખાલી, &resolved_cgroup);
                જો (r < 0)
                        પાછા r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                જો (cgroup_fd < 0)
                        પાછા -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                જો (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                નહીંતર જો (r != ENOSYS)
                        પાછા -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        જો (r != 0)
                પાછા -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        જો (r != 0)
                પાછા -r;

        _cleanup_close_ પૂર્ણાંક pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, ખાલી, &attr, argv, envp);
        જો (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                પાછા -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        જો ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                જો (r != 0)
                        પાછા -r;

                r = pidfd_spawn(&pidfd, path, ખાલી, &attr, argv, envp);
        }
        જો (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                જો (r < 0)
                        પાછા r;

                પાછા FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        જો (!ERRNO_IS_NOT_SUPPORTED(r))
                પાછા -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, ખાલી, &attr, argv, envp);
        જો (r != 0)
                પાછા -r;

        r = pidref_set_pid(ret_pidref, pid);
        જો (r < 0)
                પાછા r;

        પાછા 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

પૂર્ણાંક proc_dir_open(DIR **ret) {
        DIR *d;

        પ્રતિજ્ઞા(ret);

        d = opendir("/proc");
        જો (!d)
                પાછા -errno;

        *ret = d;
        પાછા 0;
}

પૂર્ણાંક proc_dir_read(DIR *d, pid_t *ret) {
        પ્રતિજ્ઞા(d);

        માટે (;;) {
                સંરચના dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                જો (!de) {
                        જો (errno != 0)
                                પાછા -errno;

                        તોડો;
                }

                જો (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        ચાલુ;

                જો (parse_pid(de->d_name, ret) >= 0)
                        પાછા 1;
        }

        જો (ret)
                *ret = 0;
        પાછા 0;
}

પૂર્ણાંક proc_dir_read_pidref(DIR *d, PidRef *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(d);

        માટે (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                r = pidref_set_pid(ret, pid);
                જો (r == -ESRCH) /* gone by now? skip it */
                        ચાલુ;
                જો (r < 0)
                        પાછા r;

                પાછા 1;
        }

        જો (ret)
                *ret = PIDREF_NULL;
        પાછા 0;
}

પૂર્ણાંક safe_mlockall(પૂર્ણાંક flags) {
        પૂર્ણાંક r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        જો (r == 0)
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        જો (mlockall(flags) < 0)
                પાછા log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        પાછા 0;
}

સ્થિર અચલ અક્ષર *અચલ sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર* અચલ sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, પૂર્ણાંક, INT_MAX);

_noreturn_ શૂન્ય report_errno_and_exit(પૂર્ણાંક errno_fd, પૂર્ણાંક error) {
        પૂર્ણાંક r;

        જો (error >= 0)
                _exit(EXIT_SUCCESS);

        પ્રતિજ્ઞા(errno_fd >= 0);

        r = loop_write(errno_fd, &error, કદ(error));
        જો (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

પૂર્ણાંક read_errno(પૂર્ણાંક errno_fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, કદ(r), /* do_poll= */ false);
        જો (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                પાછા -EIO;
        }
        જો (n == 0) /* the process exited without reporting an error, assuming success */
                પાછા 0;
        જો (n != કદ(r))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        જો (r == 0)
                પાછા 0;
        જો (r < 0) /* child process reported an error, return it */
                પાછા log_debug_errno(r, "Child process failed with errno: %m");

        પાછા log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

પૂર્ણાંક prctl_safe(પૂર્ણાંક op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        પાછા RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

પૂર્ણાંક proc_set_comm(અચલ અક્ષર *comm) {
        પાછા prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

પૂર્ણાંક proc_set_nnp(શૂન્ય) {
        પાછા prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(શૂન્ય) {
        પૂર્ણાંક r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        જો (r < 0)
                પાછા NSEC_INFINITY;

        પાછા r;
}
