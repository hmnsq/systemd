/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/oom.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pthread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <spawn.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mman.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mount.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/personality.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/prctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/wait.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ HAVE_VALGRIND_VALGRIND_H
#ਅਨ੍ਤਰ੍ਭਾਵ <valgrind/valgrind.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-messages.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "architecture.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "argv-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bitfield.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cgroup-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dlfcn-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locale-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mountpoint-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "namespace-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "nulstr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidfd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "raw-clone.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rlimit-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "signal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socket-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMM_MAX_LEN 128

ਸਥਿਰ ਪੂਰਨ get_process_state(pid_t pid) {
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *p;
        ਅੱਖਰ state;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        ਜੇ (pid == 0 || pid == getpid_cached())
                ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_ਸਤਰ_file(p, &ਸਤਰ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        p = strrchr(ਸਤਰ, ')');
        ਜੇ (!p)
                ਵਾਪਸ -EIO;

        p++;

        ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(p, " %c", &state) != 1)
                ਵਾਪਸ -EIO;

        ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) state;
}

ਪੂਰਨ pid_get_comm(pid_t pid, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *escaped = ਨਿਰਾ, *comm = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pid == 0 || pid == getpid_cached()) {
                comm = new0(ਅੱਖਰ, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                ਜੇ (!comm)
                        ਵਾਪਸ -ENOMEM;

                r = prctl_ਸੁਰੱਖਿਅਤ(PR_GET_NAME, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) comm, 0, 0, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        } ਨਹੀਂਤਾਂ {
                ਅਚਲ ਅੱਖਰ *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_ਸਤਰ_file(p, &comm);
                ਜੇ (r == -ENOENT)
                        ਵਾਪਸ -ESRCH;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        escaped = new(ਅੱਖਰ, COMM_MAX_LEN);
        ਜੇ (!escaped)
                ਵਾਪਸ -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_get_comm(ਅਚਲ PidRef *pid, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *comm = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = TAKE_PTR(comm);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_ਮਾਪ,
                ProcessCmdlineFlags flags,
                ਅੱਖਰ **ret,
                size_t *ret_ਮਾਪ) {

        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *p;
        size_t k;
        ਪੂਰਨ r;

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
        r = read_virtual_file(p, max_ਮਾਪ, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (k == 0) {
                ਜੇ (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        ਵਾਪਸ -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ ਅੱਖਰ *comm = ਨਿਰਾ;

                r = pid_get_comm(pid, &comm);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਮੁਕਤ(t);
                t = strjoin("[", comm, "]");
                ਜੇ (!t)
                        ਵਾਪਸ -ENOMEM;

                k = ਲੜੀ_ਲੰਬਾਈ(t);
                r = k <= max_ਮਾਪ;
                ਜੇ (r == 0) /* truncation */
                        t[max_ਮਾਪ] = '\0';
        }

        ਜੇ (ret)
                *ret = TAKE_PTR(t);
        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = k;

        ਵਾਪਸ r;
}

ਪੂਰਨ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        size_t k;
        ਅੱਖਰ *ans;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

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

        ਪੂਰਨ ਭਰਿਆ = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        ਜੇ (ਭਰਿਆ < 0)
                ਵਾਪਸ ਭਰਿਆ;

        ਜੇ (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_ਖਾਲੀ_ਥਾਵਾਂ |
                        FLAGS_ਸੈੱਟ(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                ਪ੍ਰਤਿਜ੍ਞਾ(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ ਅੱਖਰ **ਦਲੀਲਾਂ = ਨਿਰਾ;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                ਦਲੀਲਾਂ = strv_parse_nulstr_ਭਰਿਆ(t, k, /* drop_trailing_nuls= */ true);
                ਜੇ (!ਦਲੀਲਾਂ)
                        ਵਾਪਸ -ENOMEM;

                ans = quote_command_ਸਤਰ(ਦਲੀਲਾਂ, shflags);
                ਜੇ (!ans)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                ਲਈ (size_t i = 0; i < k - 1; i++)
                        ਜੇ (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_ਦੋਆਧਾਰੀ_ਅੰਕ = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_ਭਰਿਆ(t, max_columns,
                                                eight_ਦੋਆਧਾਰੀ_ਅੰਕ * XESCAPE_8_ਦੋਆਧਾਰੀ_ਅੰਕ | !ਭਰਿਆ * XESCAPE_FORCE_ELLIPSIS);
                ਜੇ (!ans)
                        ਵਾਪਸ -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_get_cmdline(ਅਚਲ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ਅੱਖਰ ***ret) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਅੱਖਰ **ਦਲੀਲਾਂ;
        size_t k;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਦਲੀਲਾਂ = strv_parse_nulstr_ਭਰਿਆ(t, k, /* drop_trailing_nuls= */ true);
        ਜੇ (!ਦਲੀਲਾਂ)
                ਵਾਪਸ -ENOMEM;

        *ret = ਦਲੀਲਾਂ;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_get_cmdline_strv(ਅਚਲ PidRef *pid, ProcessCmdlineFlags flags, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **ਦਲੀਲਾਂ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &ਦਲੀਲਾਂ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = TAKE_PTR(ਦਲੀਲਾਂ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ procfs_get_stat_flags(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        r = read_one_ਸਤਰ_file(ਰਸਤਾ, &ਸਤਰ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Skip past the comm field */
        ਅੱਖਰ *q = strrchr(ਸਤਰ, ')');
        ਜੇ (!q)
                ਵਾਪਸ -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        ਲਈ (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                ਜੇ (l < 1)
                        ਵਾਪਸ -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                ਜੇ (l < 1)
                        ਵਾਪਸ -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        ਜੇ (l < 1)
                ਵਾਪਸ -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        ਜੇ (l < 1)
                ਵਾਪਸ -EINVAL;
        q[l] = 0;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atollu(q, ret);
}

ਪੂਰਨ pid_is_kernel_thread(pid_t pid) {
        ਪੂਰਨ r;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                ਵਾਪਸ 0;
        ਜੇ (!pid_is_valid(pid))
                ਵਾਪਸ -EINVAL;

        ਅਚਲ ਅੱਖਰ *p = procfs_file_alloca(pid, "stat");

        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ flags;
        r = procfs_get_stat_flags(p, &flags);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ !!(flags & PF_KTHREAD);
}

ਪੂਰਨ pidref_is_kernel_thread(ਅਚਲ PidRef *pid) {
        ਪੂਰਨ ਨਤੀਜਾ, r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        ਨਤੀਜਾ = pid_is_kernel_thread(pid->pid);
        ਜੇ (ਨਤੀਜਾ < 0)
                ਵਾਪਸ ਨਤੀਜਾ;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ get_process_link_contents(pid_t pid, ਅਚਲ ਅੱਖਰ *proc_file, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        ਵਾਪਸ (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

ਪੂਰਨ get_process_exe(pid_t pid, ਅੱਖਰ **ret) {
        ਅੱਖਰ *d;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret) {
                d = endswith(*ret, " (deleted)");
                ਜੇ (d)
                        *d = '\0';
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ pid_get_uid(pid_t pid, uid_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                ਵਾਪਸ 0;
        }

        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        r = procfs_file_get_ਖੇਤਰ(pid, "status", "Uid", &v);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_uid(v, ret);
}

ਪੂਰਨ pidref_get_uid(ਅਚਲ PidRef *pid, uid_t *ret) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                ਜੇ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ਵਾਪਸ r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = uid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_process_gid(pid_t pid, gid_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                ਵਾਪਸ 0;
        }

        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        r = procfs_file_get_ਖੇਤਰ(pid, "status", "Gid", &v);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_gid(v, ret);
}

ਪੂਰਨ get_process_cwd(pid_t pid, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);

        ਜੇ (pid == 0 || pid == getpid_cached())
                ਵਾਪਸ ਸੁਰੱਖਿਅਤ_getcwd(ret);

        ਵਾਪਸ get_process_link_contents(pid, "cwd", ret);
}

ਪੂਰਨ get_process_ਜੜ੍ਹ(pid_t pid, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਵਾਪਸ get_process_link_contents(pid, "root", ret);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENVIRONMENT_ਖੰਡ_MAX (5U*1024U*1024U)

ਪੂਰਨ get_process_environ(pid_t pid, ਅੱਖਰ **ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *outcome = ਨਿਰਾ;
        size_t sz = 0;
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                ਅੱਖਰ c;

                ਜੇ (sz >= ENVIRONMENT_ਖੰਡ_MAX)
                        ਵਾਪਸ -ENOBUFS;

                ਜੇ (!GREEDY_REALLOC(outcome, sz + 5))
                        ਵਾਪਸ -ENOMEM;

                r = ਸੁਰੱਖਿਅਤ_fgetc(f, &c);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (c == '\0')
                        outcome[sz++] = '\n';
                ਨਹੀਂਤਾਂ
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        ਵਾਪਸ 0;
}

ਪੂਰਨ pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ppid;
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);

        ਜੇ (pid == 0)
                pid = getpid_cached();
        ਜੇ (pid == 1) /* PID 1 has no parent, shortcut this case */
                ਵਾਪਸ -EADDRNOTAVAIL;

        ਜੇ (pid == getpid_cached()) {
                ਜੇ (ret)
                        *ret = getppid();
                ਵਾਪਸ 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ਸਤਰ_file(p, &ਸਤਰ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ਸਤਰ, ')');
        ਜੇ (!p)
                ਵਾਪਸ -EIO;
        p++;

        ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                ਵਾਪਸ -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        ਜੇ (ppid == 0)
                ਵਾਪਸ -EADDRNOTAVAIL;

        ਜੇ ((pid_t) ppid < 0 || (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) (pid_t) ppid != ppid)
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret)
                *ret = (pid_t) ppid;

        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_get_ppid(ਅਚਲ PidRef *pidref, pid_t *ret) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                ਜੇ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ਵਾਪਸ r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = ppid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_get_ppid_as_pidref(ਅਚਲ PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        ਪੂਰਨ r;

        ਸੂਚਕ_MAY_BE_NULL(pidref);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = pidref_get_ppid(pidref, &ppid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_ਪੂਰਾ) PidRef ਮਾਪਾ = PIDREF_NULL;

                r = pidref_ਸੈੱਟ_pid(&ਮਾਪਾ, ppid);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                ਜੇ (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        ਜੇ (ਮਾਪਾ.pid != ppid)
                                ਜਾਰੀ;
                }

                *ret = TAKE_PIDREF(ਮਾਪਾ);
                ਵਾਪਸ 0;
        }

        /* Give up after 16 tries */
        ਵਾਪਸ -ENOTRECOVERABLE;
}

ਪੂਰਨ pid_get_ਸ਼ੁਰੂ_ਸਮਾਂ(pid_t pid, usec_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ਸਤਰ_file(p, &ਸਤਰ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ਸਤਰ, ')');
        ਜੇ (!p)
                ਵਾਪਸ -EIO;
        p++;

        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ llu;

        ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(p, " "
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
                   "%*i " /*ਤਰਜੀਹ*/
                   "%*i " /* nice */
                   "%*u " /* num_threads */
                   "%*u " /* itrealvalue */
                   "%lu ", /* starttime */
                   &llu) != 1)
                ਵਾਪਸ -EIO;

        ਜੇ (ret)
                *ret = jiffies_ਮੰਜ਼ਿਲ_usec(llu); /* CLOCK_BOOTTIME */

        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_get_ਸ਼ੁਰੂ_ਸਮਾਂ(ਅਚਲ PidRef *pid, usec_t *ret) {
        usec_t t;
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        r = pid_get_ਸ਼ੁਰੂ_ਸਮਾਂ(pid->pid, ret ? &t : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = t;

        ਵਾਪਸ 0;
}

ਪੂਰਨ get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ ਅੱਖਰ *m = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = procfs_file_get_ਖੇਤਰ(pid, "status", "Umask", &m);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_ਬਹੁਲਕ(m, ret);
}

ਸਥਿਰ ਪੂਰਨ pid_get_sigmask(pid_t pid, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ_name, uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਖੇਤਰ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਖੇਤਰ_name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = procfs_file_get_ਖੇਤਰ(pid, "status", ਖੇਤਰ_name, &ਖੇਤਰ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou64_ਭਰਿਆ(ਖੇਤਰ, 16, ret);
}

ਸਥਿਰ ਪੂਰਨ pidref_has_sigmask(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ_name) {
        uint64_t ਨਕਾਬ_3;
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;
        ਜੇ (!SIGNAL_VALID(sig))
                ਵਾਪਸ -EINVAL;

        r = pid_get_sigmask(pidref->pid, ਖੇਤਰ_name, &ਨਕਾਬ_3);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(ਨਕਾਬ_3, sig - 1);
}

ਪੂਰਨ pidref_has_sigcgt(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig) {
        ਵਾਪਸ pidref_has_sigmask(pidref, sig, "SigCgt");
}

ਪੂਰਨ pidref_has_sigblk(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig) {
        ਵਾਪਸ pidref_has_sigmask(pidref, sig, "SigBlk");
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
ਪੂਰਨ pidref_wait_for_terminate_and_check(ਅਚਲ ਅੱਖਰ *name, PidRef *pidref, WaitFlags flags) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;
        ਜੇ (pidref->pid == 1 || pidref_is_self(pidref))
                ਵਾਪਸ -ECHILD;

        _cleanup_free_ ਅੱਖਰ *ਬਫਰ = ਨਿਰਾ;
        ਜੇ (!name) {
                r = pidref_get_comm(pidref, &ਬਫਰ);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                ਨਹੀਂਤਾਂ
                        name = ਬਫਰ;
        }

        ਪੂਰਨ prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        ਜੇ (status.si_code == CLD_EXITED) {
                ਜੇ (status.si_status != EXIT_SUCCESS)
                        log_ਭਰਿਆ(flags & WAIT_LOG_NON_ZERO_EXIT_ਸਥਿਤੀ ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                ਨਹੀਂਤਾਂ
                        log_debug("'%s' succeeded.", name);

                ਵਾਪਸ status.si_status;

        } ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(status.si_code, CLD_KILLED, CLD_DUMPED))
                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_ਮੰਜ਼ਿਲ_string(status.si_status));

        ਵਾਪਸ log_ਭਰਿਆ_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

ਪੂਰਨ kill_and_sigcont(pid_t pid, ਪੂਰਨ sig) {
        ਪੂਰਨ r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        ਜੇ (r >= 0 && !ਵਿੱਚ_ਸੈੱਟ(sig, SIGCONT, SIGKILL))
                (ਖਾਲੀ) kill(pid, SIGCONT);

        ਵਾਪਸ r;
}

ਪੂਰਨ getenv_for_pid(pid_t pid, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ **ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;
        size_t ਜੋੜ = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਖੇਤਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pid == 0 || pid == getpid_cached())
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret, getenv(ਖੇਤਰ));

        ਜੇ (!pid_is_valid(pid))
                ਵਾਪਸ -EINVAL;

        ਰਸਤਾ = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(ਰਸਤਾ, "re", &f);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                ਅਚਲ ਅੱਖਰ *ਮੇਲ;

                ਜੇ (ਜੋੜ > ENVIRONMENT_ਖੰਡ_MAX) /* Give up searching eventually */
                        ਵਾਪਸ -ENOBUFS;

                r = read_nul_string(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)  /* EOF */
                        ਰੋਕੋ;

                ਜੋੜ += r;

                ਮੇਲ = startswith(ਸਤਰ, ਖੇਤਰ);
                ਜੇ (ਮੇਲ && *ਮੇਲ == '=')
                        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret, ਮੇਲ + 1);
        }

        *ret = ਨਿਰਾ;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidref_is_my_ਬੱਚਾ(PidRef *pid) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pid->pid == 1 || pidref_is_self(pid))
                ਵਾਪਸ false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        ਜੇ (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ppid == getpid_cached();
}

ਪੂਰਨ pid_is_my_ਬੱਚਾ(pid_t pid) {

        ਜੇ (pid == 0)
                ਵਾਪਸ false;

        ਵਾਪਸ pidref_is_my_ਬੱਚਾ(&PIDREF_MAKE_FROM_PID(pid));
}

ਪੂਰਨ pidref_is_unwaited(PidRef *pid) {
        ਪੂਰਨ r;

        /* Checks whether a PID is still valid at all, including a zombie */

        ਜੇ (!pidref_is_ਸੈੱਟ(pid))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pid))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pid->pid == 1 || pidref_is_self(pid))
                ਵਾਪਸ true;

        r = pidref_kill(pid, 0);
        ਜੇ (r == -ESRCH)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ true;
}

ਪੂਰਨ pid_is_unwaited(pid_t pid) {

        ਜੇ (pid == 0)
                ਵਾਪਸ true;

        ਵਾਪਸ pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

ਪੂਰਨ pid_is_alive(pid_t pid) {
        ਪੂਰਨ r;

        /* Checks whether a PID is still valid and not a zombie */

        ਜੇ (pid < 0)
                ਵਾਪਸ -ESRCH;

        ਜੇ (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                ਵਾਪਸ true;

        ਜੇ (pid == getpid_cached())
                ਵਾਪਸ true;

        r = get_process_state(pid);
        ਜੇ (r == -ESRCH)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ r != 'Z';
}

ਪੂਰਨ pidref_is_alive(ਅਚਲ PidRef *pidref) {
        ਪੂਰਨ r, ਨਤੀਜਾ;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਨਤੀਜਾ = pid_is_alive(pidref->pid);
        ਜੇ (ਨਤੀਜਾ < 0) {
                ਪ੍ਰਤਿਜ੍ਞਾ(ਨਤੀਜਾ != -ESRCH);
                ਵਾਪਸ ਨਤੀਜਾ;
        }

        r = pidref_verify(pidref);
        ਜੇ (r == -ESRCH)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ pidref_from_same_ਜੜ੍ਹ_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_ਪੂਰਾ) PidRef self = PIDREF_NULL;
        ਪੂਰਨ r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        ਜੇ (!a || !b) {
                r = pidref_ਸੈੱਟ_self(&self);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (!a)
                        a = &self;
                ਜੇ (!b)
                        b = &self;
        }

        ਜੇ (!pidref_is_ਸੈੱਟ(a) || !pidref_is_ਸੈੱਟ(b))
                ਵਾਪਸ -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        ਜੇ (pidref_is_remote(a) && pidref_is_remote(b))
                ਵਾਪਸ -EREMOTE;
        ਜੇ (pidref_is_remote(a) || pidref_is_remote(b))
                ਵਾਪਸ false;

        ਜੇ (pidref_equal(a, b))
                ਵਾਪਸ true;

        ਅਚਲ ਅੱਖਰ *roota = procfs_file_alloca(a->pid, "root");
        ਅਚਲ ਅੱਖਰ *rootb = procfs_file_alloca(b->pid, "root");

        ਪੂਰਨ ਨਤੀਜਾ = inode_same(roota, rootb, 0);
        ਜੇ (ਨਤੀਜਾ == -ENOENT)
                ਵਾਪਸ proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        ਜੇ (ਨਤੀਜਾ < 0)
                ਵਾਪਸ ਨਤੀਜਾ;

        r = pidref_verify(a);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        r = pidref_verify(b);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਨਤੀਜਾ;
}

bool is_main_thread(ਖਾਲੀ) {
        ਸਥਿਰ thread_local ਪੂਰਨ cached = -1;

        ਜੇ (cached < 0)
                cached = getpid_cached() == gettid();

        ਵਾਪਸ cached;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ personality_from_string(ਅਚਲ ਅੱਖਰ *s) {
        Architecture architecture;

        ਜੇ (!s)
                ਵਾਪਸ PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        ਜੇ (architecture < 0)
                ਵਾਪਸ PERSONALITY_INVALID;

        ਜੇ (architecture == native_architecture())
                ਵਾਪਸ PER_LINUX;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARCHITECTURE_SECONDARY
        ਜੇ (architecture == ARCHITECTURE_SECONDARY)
                ਵਾਪਸ PER_LINUX32;
#ਜੇ_ਅੰਤ

        ਵਾਪਸ PERSONALITY_INVALID;
}

ਅਚਲ ਅੱਖਰ* personality_ਮੰਜ਼ਿਲ_string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        ਜੇ (p == PER_LINUX)
                architecture = native_architecture();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARCHITECTURE_SECONDARY
        ਨਹੀਂਤਾਂ ਜੇ (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#ਜੇ_ਅੰਤ

        ਜੇ (architecture < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ architecture_ਮੰਜ਼ਿਲ_string(architecture);
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_personality(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ p) {
        ਪੂਰਨ ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        ਜੇ (ret < 0) {
                ਜੇ (errno != 0)
                        ਵਾਪਸ -errno;

                errno = -ret;
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ opinionated_personality(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret) {
        ਪੂਰਨ current;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = ਸੁਰੱਖਿਅਤ_personality(PERSONALITY_INVALID);
        ਜੇ (current < 0)
                ਵਾਪਸ current;

        ਜੇ (((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) current & OPINIONATED_PERSONALITY_ਨਕਾਬ) == PER_LINUX32)
                *ret = PER_LINUX32;
        ਨਹੀਂਤਾਂ
                *ret = PER_LINUX;

        ਵਾਪਸ 0;
}

ਖਾਲੀ valgrind_summary_hack(ਖਾਲੀ) {
#ਜੇ HAVE_VALGRIND_VALGRIND_H
        ਜੇ (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                ਜੇ (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_ਸੁਨੇਹਾ_ID(SD_ਸੁਨੇਹਾ_VALGRIND_HELPER_FORK_STR),
                                LOG_ਸੁਨੇਹਾ("Failed to fork off valgrind helper: %m"));
                ਨਹੀਂਤਾਂ ਜੇ (pid == 0)
                        ਬਾਹਰ_ਨਿਕਲੋ(EXIT_SUCCESS);
                ਨਹੀਂਤਾਂ {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_ਪੂਰਾ) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (ਖਾਲੀ) pidref_ਸੈੱਟ_pid(&pidref, pid);
                        (ਖਾਲੀ) pidref_wait_for_terminate(&pidref, ਨਿਰਾ);
                }
        }
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ pid_compare_func(ਅਚਲ pid_t *a, ਅਚਲ pid_t *b) {
        /* Suitable for usage in qsort() */
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ CMP(*a, *b);
}

bool nice_is_valid(ਪੂਰਨ n) {
        ਵਾਪਸ n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(ਪੂਰਨ policy) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(ਪੂਰਨ policy) {
        ਵਾਪਸ sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
ਪੂਰਨ sched_get_ਤਰਜੀਹ_min_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ policy) {
        ਪੂਰਨ r;

        r = sched_get_priority_min(policy);
        ਜੇ (r >= 0)
                ਵਾਪਸ r;

        /* Fallback priority */
        ਵਾਪਸ 0;
}

ਪੂਰਨ sched_get_ਤਰਜੀਹ_max_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ policy) {
        ਪੂਰਨ r;

        r = sched_get_priority_max(policy);
        ਜੇ (r >= 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CACHED_PID_UNSET ((pid_t) 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CACHED_PID_BUSY ((pid_t) -1)

ਸਥਿਰ pid_t cached_pid = CACHED_PID_UNSET;

ਖਾਲੀ reset_cached_pid(ਖਾਲੀ) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(ਖਾਲੀ) {
        ਸਥਿਰ bool installed = false;
        pid_t current_ਮੁੱਲ;

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
        current_ਮੁੱਲ = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        ਜੇ (current_ਮੁੱਲ > 0)
                ਵਾਪਸ current_ਮੁੱਲ;

        current_ਮੁੱਲ = CACHED_PID_UNSET;
        (ਖਾਲੀ) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_ਮੁੱਲ,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        ਚੋਣ (current_ਮੁੱਲ) {

        ਹਾਲਤ CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                ਜੇ (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        ਜੇ (pthread_atfork(ਨਿਰਾ, ਨਿਰਾ, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                ਵਾਪਸ new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                ਵਾਪਸ new_pid;
        }

        ਹਾਲਤ CACHED_PID_BUSY: /* Somebody else is currently initializing */
                ਵਾਪਸ getpid();

        ਮੂਲ: /* Properly initialized */
                ਵਾਪਸ current_ਮੁੱਲ;
        }
}

ਪੂਰਨ must_be_ਜੜ੍ਹ(ਖਾਲੀ) {

        ਜੇ (geteuid() == 0)
                ਵਾਪਸ 0;

        ਵਾਪਸ log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ਢੇਰ(ਪੂਰਨ (*fn)(ਖਾਲੀ *), ਪੂਰਨ flags, ਖਾਲੀ *userdata) {
        size_t ps;
        pid_t pid;
        ਖਾਲੀ *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        ਪ੍ਰਤਿਜ੍ਞਾ((flags & (CLONE_VM|CLONE_ਮਾਪਾ_SETTID|CLONE_ਬੱਚਾ_SETTID|
                         CLONE_ਬੱਚਾ_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = ਪੰਨਾ_ਮਾਪ();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (ਖਾਲੀ*) ALIGN_ਮੰਜ਼ਿਲ((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#ਜੇ HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#ਨਹੀਂਤਾਂ
        pid = __clone2(fn, mystack, ps, flags, userdata);
#ਜੇ_ਅੰਤ
        ਜੇ (pid < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ pid;
}

ਸਥਿਰ ਪੂਰਨ fork_flags_ਮੰਜ਼ਿਲ_signal(ForkFlags flags) {
        ਵਾਪਸ (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

ਪੂਰਨ pidref_ਸੁਰੱਖਿਅਤ_fork_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਪੂਰਨ stdio_fds[3],
                ਪੂਰਨ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        ਅਣਵਰਤਿਆ _cleanup_(ਖੰਡ_ਸੰਕੇਤ_reset) sigset_t *saved_ssp = ਨਿਰਾ;
        bool ਖੰਡ_ਸੰਕੇਤ = false, ਖੰਡ_all = false, intermediary = false;
        _cleanup_close_pair_ ਪੂਰਨ pidref_transport_fds[2] = EBADF_PAIR;
        ਪੂਰਨ prio, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, FORK_WAIT|FORK_FREEZE));
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        ਜੇ (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        ਜੇ (flags & (FORK_RESET_ਸੰਕੇਤ|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                ਖੰਡ_ਸੰਕੇਤ = ਖੰਡ_all = true;

        } ਨਹੀਂਤਾਂ ਜੇ (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                ਖੰਡ_ਸੰਕੇਤ = true;
        }

        ਜੇ (ਖੰਡ_ਸੰਕੇਤ) {
                ਜੇ (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                ਜੇ (r < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                ਜੇ (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        ਜੇ (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        ਜੇ (pid < 0)
                                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        ਜੇ (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = ਸੁਰੱਖਿਅਤ_close(pidref_transport_fds[1]);

                                ਜੇ (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_ਸੈੱਟ(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        ਜੇ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                ਵਾਪਸ -EPROTO;

                                        ਪੂਰਨ pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, ਆਕਾਰ(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        ਜੇ (n < 0)
                                                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                ਵਾਪਸ 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = ਸੁਰੱਖਿਅਤ_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        ਜੇ ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_ਸੈੱਟ(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_ਸੈੱਟ(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_ਸੈੱਟ(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_ਸੈੱਟ(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        ਨਹੀਂਤਾਂ
                pid = fork();
        ਜੇ (pid < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        ਜੇ (pid > 0) {

                /* If we are in the intermediary process, exit now */
                ਜੇ (intermediary) {
                        ਜੇ (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_ਪੂਰਾ) PidRef pidref = PIDREF_NULL;

                                r = pidref_ਸੈੱਟ_pid(&pidref, pid);
                                ਜੇ (r < 0) {
                                        log_ਭਰਿਆ_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, ਆਕਾਰ(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                ਜੇ (r < 0) {
                                        log_ਭਰਿਆ_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                ਜੇ (flags & FORK_WAIT) {
                        ਜੇ (ਖੰਡ_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (ਖਾਲੀ) sigprocmask(SIG_SETMASK, &ss, ਨਿਰਾ);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_ਸੈੱਟ(flags, FORK_LOG) ? WAIT_LOG : 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                ਵਾਪਸ -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        ਜੇ (ret)
                                *ret = PIDREF_NULL;

                        ਵਾਪਸ 1;
                }

                ਜੇ (ret) {
                        r = pidref_ਸੈੱਟ_pid(ret, pid);
                        ਜੇ (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                ਵਾਪਸ 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = ਸੁਰੱਖਿਅਤ_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ਨਿਰਾ;

        ਜੇ (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_ਸੈੱਟ_open_when_needed(true);
                log_settle_ਨਿਸ਼ਾਨਾ();
        }

        ਜੇ (name) {
                r = rename_process(name);
                ਜੇ (r < 0)
                        log_ਭਰਿਆ_errno(flags & FORK_LOG ? LOG_WARNING : LOG_DEBUG,
                                       r, "Failed to rename process, ignoring: %m");
        }

        /* let's disable dlopen() in the child, as a paranoia safety precaution: children should not live for
         * long and only do minimal work before exiting or exec()ing. Doing dlopen() is not either. If people
         * want dlopen() they should do it before forking. This is a safety precaution in particular for
         * cases where the child does namespace shenanigans: we should never end up loading a module from a
         * foreign environment. Note that this has no effect on NSS! (i.e. it only has effect on uses of our
         * dlopen_safe(), which we use comprehensively in our codebase, but glibc NSS doesn't bother, of
         * course.) */
        ਜੇ (!FLAGS_ਸੈੱਟ(flags, FORK_ALLOW_DLOPEN))
                ਖੰਡ_dlopen();

        ਜੇ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_PDEATHSIG, fork_flags_ਮੰਜ਼ਿਲ_signal(flags), 0, 0, 0);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (flags & FORK_RESET_ਸੰਕੇਤ) {
                r = reset_all_signal_handlers();
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_ਨਕਾਬ();
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } ਨਹੀਂਤਾਂ ਜੇ (ਖੰਡ_ਸੰਕੇਤ) { /* undo what we did above */
                ਜੇ (sigprocmask(SIG_SETMASK, &saved_ss, ਨਿਰਾ) < 0) {
                        log_ਭਰਿਆ_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                ਜੇ (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                ਨਹੀਂਤਾਂ ਜੇ (ppid != original_pid) {
                        ਪੂਰਨ sig = fork_flags_ਮੰਜ਼ਿਲ_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_ਮੰਜ਼ਿਲ_string(sig));
                        (ਖਾਲੀ) ਉਠਾਓ(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                ਜੇ (mount(ਨਿਰਾ, "/", ਨਿਰਾ, MS_SLAVE | MS_REC, ਨਿਰਾ) < 0) {
                        log_ਭਰਿਆ_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, FORK_ਨਿੱਜੀ_TMP)) {
                ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_ਲੜੀਵਾਰ);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (flags & FORK_REARRANGE_STDIO) {
                ਜੇ (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        ਜੇ (r < 0) {
                                log_ਭਰਿਆ_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } ਨਹੀਂਤਾਂ {
                        r = make_null_stdio();
                        ਜੇ (r < 0) {
                                log_ਭਰਿਆ_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } ਨਹੀਂਤਾਂ ਜੇ (flags & FORK_STDOUT_ਮੰਜ਼ਿਲ_STDERR) {
                ਜੇ (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_ਭਰਿਆ_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        ਜੇ (flags & FORK_REOPEN_LOG) {
                log_open();
                log_ਸੈੱਟ_open_when_needed(false);
        }

        ਜੇ (flags & FORK_RLIMIT_NOFILE_ਸੁਰੱਖਿਅਤ) {
                r = rlimit_nofile_ਸੁਰੱਖਿਅਤ();
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, FORK_FREEZE))
                freeze();

        ਜੇ (ret) {
                r = pidref_ਸੈੱਟ_self(ret);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ namespace_fork_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *outer_name,
                ਅਚਲ ਅੱਖਰ *inner_name,
                ਪੂਰਨ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ਪੂਰਨ pidns_fd,
                ਪੂਰਨ mntns_fd,
                ਪੂਰਨ netns_fd,
                ਪੂਰਨ userns_fd,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ ਪੂਰਨ errno_pipe_fd[2] = EBADF_PAIR;
        ਪੂਰਨ r, prio = FLAGS_ਸੈੱਟ(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(ਆਕਾਰ(ਪੂਰਨ) <= PIPE_BUF);
        ਜੇ (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_ਸੁਰੱਖਿਅਤ_fork_ਭਰਿਆ(
                        outer_name,
                        /* stdio_fds= */ ਨਿਰਾ, /* except_fds= */ ਨਿਰਾ, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_ਸੁਰੱਖਿਅਤ),
                        &pidref_outer);
        ਜੇ (r == -EPROTO && FLAGS_ਸੈੱਟ(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = ਸੁਰੱਖਿਅਤ_close(errno_pipe_fd[1]);

                ਪੂਰਨ k = read_errno(errno_pipe_fd[0]);
                ਜੇ (k < 0 && k != -EIO)
                        ਵਾਪਸ k;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0) {
                _cleanup_(pidref_ਪੂਰਾ) PidRef pidref_inner = PIDREF_NULL;

                /*ਬੱਚਾ*/

                errno_pipe_fd[0] = ਸੁਰੱਖਿਅਤ_close(errno_pipe_fd[0]);

                r = namespace_enter_ਭਰਿਆ(pidns_fd, mntns_fd, netns_fd, userns_fd, ਜੜ੍ਹ_fd, namespace_flags);
                ਜੇ (r < 0) {
                        log_ਭਰਿਆ_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_ਸੁਰੱਖਿਅਤ_fork_ਭਰਿਆ(
                                inner_name,
                                ਨਿਰਾ,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_ਸੰਕੇਤ|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_ਮੰਜ਼ਿਲ_STDERR),
                                &pidref_inner);
                ਜੇ (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                ਜੇ (r == 0) {
                        /*ਬੱਚਾ*/

                        ਜੇ (!FLAGS_ਸੈੱਟ(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = ਸੁਰੱਖਿਅਤ_close(errno_pipe_fd[1]);
                                pidref_ਪੂਰਾ(&pidref_outer);
                        } ਨਹੀਂਤਾਂ {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        ਜੇ (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        ਵਾਪਸ 0;
                }

                log_forget_fds();
                log_ਸੈੱਟ_open_when_needed(true);

                (ਖਾਲੀ) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_ਸੈੱਟ(flags, FORK_LOG) ? WAIT_LOG : 0);
                ਜੇ (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = ਸੁਰੱਖਿਅਤ_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        ਜੇ (r < 0)
                ਵਾਪਸ r; /* the child logs about failures on its own, no need to duplicate here */

        ਜੇ (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        ਨਹੀਂਤਾਂ
                pidref_ਪੂਰਾ(&pidref_outer); /* disarm sigkill_wait */

        ਵਾਪਸ 1;
}

ਪੂਰਨ namespace_fork(
                ਅਚਲ ਅੱਖਰ *outer_name,
                ਅਚਲ ਅੱਖਰ *inner_name,
                ForkFlags flags,
                ਪੂਰਨ pidns_fd,
                ਪੂਰਨ mntns_fd,
                ਪੂਰਨ netns_fd,
                ਪੂਰਨ userns_fd,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                PidRef *ret) {

        ਵਾਪਸ namespace_fork_ਭਰਿਆ(outer_name, inner_name, /* except_fds= */ ਨਿਰਾ, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, ਜੜ੍ਹ_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(ਪੂਰਨ oa) {
        ਵਾਪਸ oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

ਪੂਰਨ ਸੈੱਟ_oom_score_adjust(ਪੂਰਨ ਮੁੱਲ) {
        ਅੱਖਰ t[DECIMAL_STR_MAX(ਪੂਰਨ)];

        ਜੇ (!oom_score_adjust_is_valid(ਮੁੱਲ))
                ਵਾਪਸ -EINVAL;

        xsprintf(t, "%i", ਮੁੱਲ);

        ਵਾਪਸ write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_ਬਫਰ);
}

ਪੂਰਨ get_oom_score_adjust(ਪੂਰਨ *ret) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        delete_trailing_chars(t, WHITESPACE);

        r = ਸੁਰੱਖਿਅਤ_atoi(t, &a);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!oom_score_adjust_is_valid(a))
                ਵਾਪਸ -ENODATA;

        ਜੇ (ret)
                *ret = a;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rlimit_ਮੰਜ਼ਿਲ_nice(rlim_t limit) {
        ਜੇ (limit <= 1)
                ਵਾਪਸ PRIO_MAX-1; /* i.e. 19 */

        ਜੇ (limit >= -PRIO_MIN + PRIO_MAX)
                ਵਾਪਸ PRIO_MIN; /* i.e. -20 */

        ਵਾਪਸ PRIO_MAX - (ਪੂਰਨ) limit;
}

ਪੂਰਨ setpriority_closest(ਪੂਰਨ priority) {
        ਬਣਤਰ rlimit highest;
        ਪੂਰਨ r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        ਜੇ (r >= 0)
                ਵਾਪਸ 1;
        ਜੇ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ਵਾਪਸ r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        ਜੇ (errno != 0)
                ਵਾਪਸ -errno;

        ਜੇ (priority == current)
                ਵਾਪਸ 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        ਜੇ (priority > current)
                ਵਾਪਸ r;

        ਜੇ (getrlimit(RLIMIT_NICE, &highest) < 0)
                ਵਾਪਸ -errno;

        limit = rlimit_ਮੰਜ਼ਿਲ_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        ਜੇ (current > limit)
                ਜੇ (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        ਵਾਪਸ -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        ਵਾਪਸ 0;
}

_noreturn_ ਖਾਲੀ freeze(ਖਾਲੀ) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (ਖਾਲੀ) close_all_fds_without_malloc(ਨਿਰਾ, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        ਲਈ (;;) {
                siginfo_t si = {};

                ਜੇ (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        ਰੋਕੋ;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        ਲਈ (;;)
                pause();
}

ਪੂਰਨ get_process_ਧਾਗੇ(pid_t pid) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ n, r;

        ਜੇ (pid < 0)
                ਵਾਪਸ -EINVAL;

        r = procfs_file_get_ਖੇਤਰ(pid, "status", "Threads", &t);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਸੁਰੱਖਿਅਤ_atoi(t, &n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (n < 0)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ n;
}

ਪੂਰਨ is_reaper_process(ਖਾਲੀ) {
        ਪੂਰਨ b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        ਜੇ (getpid_cached() == 1)
                ਵਾਪਸ true;

        r = prctl_ਸੁਰੱਖਿਅਤ(PR_GET_CHILD_SUBREAPER, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) &b, 0, 0, 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ b != 0;
}

ਪੂਰਨ make_reaper_process(bool b) {
        ਪੂਰਨ r;

        ਜੇ (getpid_cached() == 1) {

                ਜੇ (!b)
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ 0;
        }

        r = prctl_ਸੁਰੱਖਿਅਤ(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_ਭਰਿਆ(posix_spawnattr_t*, posix_spawnattr_destroy, ਨਿਰਾ);

ਪੂਰਨ posix_spawn_wrapper(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅੱਖਰ * ਅਚਲ *argv,
                ਅੱਖਰ * ਅਚਲ *envp,
                ਅਚਲ ਅੱਖਰ *cgroup,
                PidRef *ret_pidref) {

        ਛੋਟਾ flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t ਨਕਾਬ_3;
        ਪੂਰਨ r;

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

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(argv);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_pidref);

        assert_se(sigfillset(&ਨਕਾਬ_3) >= 0);

        r = posix_spawnattr_init(&attr);
        ਜੇ (r != 0)
                ਵਾਪਸ -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        ਅਣਵਰਤਿਆ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        ਸਥਿਰ bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ ਪੂਰਨ cgroup_fd = -EBADF;

        ਜੇ (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ ਅੱਖਰ *resolved_cgroup = ਨਿਰਾ;

                r = cg_get_ਰਸਤਾ(cgroup, /* suffix= */ ਨਿਰਾ, &resolved_cgroup);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                cgroup_fd = open(resolved_cgroup, O_ਰਸਤਾ|O_DIRECTORY|O_CLOEXEC);
                ਜੇ (cgroup_fd < 0)
                        ਵਾਪਸ -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                ਜੇ (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                ਨਹੀਂਤਾਂ ਜੇ (r != ENOSYS)
                        ਵਾਪਸ -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        ਜੇ (r != 0)
                ਵਾਪਸ -r;
        r = posix_spawnattr_setsigmask(&attr, &ਨਕਾਬ_3);
        ਜੇ (r != 0)
                ਵਾਪਸ -r;

        _cleanup_close_ ਪੂਰਨ pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, ਰਸਤਾ, ਨਿਰਾ, &attr, argv, envp);
        ਜੇ (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_ਸੈੱਟ(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                ਵਾਪਸ -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        ਜੇ ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
            FLAGS_ਸੈੱਟ(flags, POSIX_SPAWN_SETCGROUP)) {
                /* Compiled on a newer host, or seccomp&friends blocking clone3()? Fallback, but
                 * need to disable POSIX_SPAWN_SETCGROUP, which is what redirects to clone3().
                 * CLONE_INTO_CGROUP definitely won't work, hence remember the fact so that we don't
                 * retry every time.
                 * Note, CLONE_INTO_CGROUP is supported since kernel v5.7, but some architectures still
                 * do not support clone3(). Hence, we need to keep the fallback logic for a while. */
                have_clone_into_cgroup = false;

                flags &= ~POSIX_SPAWN_SETCGROUP;
                r = posix_spawnattr_setflags(&attr, flags);
                ਜੇ (r != 0)
                        ਵਾਪਸ -r;

                r = pidfd_spawn(&pidfd, ਰਸਤਾ, ਨਿਰਾ, &attr, argv, envp);
        }
        ਜੇ (r == 0) {
                r = pidref_ਸੈੱਟ_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ FLAGS_ਸੈੱਟ(flags, POSIX_SPAWN_SETCGROUP);
        }
        ਜੇ (!ERRNO_IS_NOT_SUPPORTED(r))
                ਵਾਪਸ -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, ਰਸਤਾ, ਨਿਰਾ, &attr, argv, envp);
        ਜੇ (r != 0)
                ਵਾਪਸ -r;

        r = pidref_ਸੈੱਟ_pid(ret_pidref, pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

ਪੂਰਨ proc_dir_open(DIR **ret) {
        DIR *d;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        d = opendir("/proc");
        ਜੇ (!d)
                ਵਾਪਸ -errno;

        *ret = d;
        ਵਾਪਸ 0;
}

ਪੂਰਨ proc_dir_read(DIR *d, pid_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(d);

        ਲਈ (;;) {
                ਬਣਤਰ dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                ਜੇ (!de) {
                        ਜੇ (errno != 0)
                                ਵਾਪਸ -errno;

                        ਰੋਕੋ;
                }

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(de->d_type, DT_DIR, DT_UNKNOWN))
                        ਜਾਰੀ;

                ਜੇ (parse_pid(de->d_name, ret) >= 0)
                        ਵਾਪਸ 1;
        }

        ਜੇ (ret)
                *ret = 0;
        ਵਾਪਸ 0;
}

ਪੂਰਨ proc_dir_read_pidref(DIR *d, PidRef *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(d);

        ਲਈ (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                r = pidref_ਸੈੱਟ_pid(ret, pid);
                ਜੇ (r == -ESRCH) /* gone by now? skip it */
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ 1;
        }

        ਜੇ (ret)
                *ret = PIDREF_NULL;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_mlockall(ਪੂਰਨ flags) {
        ਪੂਰਨ r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        ਜੇ (r == 0)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        ਜੇ (mlockall(flags) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigchld_ਕੋਡ_ਸਾਰਣੀ[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(sigchld_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ sched_policy_ਸਾਰਣੀ[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(sched_policy, ਪੂਰਨ, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);

_noreturn_ ਖਾਲੀ report_errno_and_exit(ਪੂਰਨ errno_fd, ਪੂਰਨ error) {
        ਪੂਰਨ r;

        ਜੇ (error >= 0)
                _exit(EXIT_SUCCESS);

        ਪ੍ਰਤਿਜ੍ਞਾ(errno_fd >= 0);

        r = loop_write(errno_fd, &error, ਆਕਾਰ(error));
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

ਪੂਰਨ read_errno(ਪੂਰਨ errno_fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, ਆਕਾਰ(r), /* do_poll= */ false);
        ਜੇ (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                ਵਾਪਸ -EIO;
        }
        ਜੇ (n == 0) /* the process exited without reporting an error, assuming success */
                ਵਾਪਸ 0;
        ਜੇ (n != ਆਕਾਰ(r))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        ਜੇ (r == 0)
                ਵਾਪਸ 0;
        ਜੇ (r < 0) /* child process reported an error, return it */
                ਵਾਪਸ log_debug_errno(r, "Child process failed with errno: %m");

        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

ਪੂਰਨ prctl_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ op, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg2, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg3, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg4, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        ਵਾਪਸ RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

ਪੂਰਨ proc_ਸੈੱਟ_comm(ਅਚਲ ਅੱਖਰ *comm) {
        ਵਾਪਸ prctl_ਸੁਰੱਖਿਅਤ(PR_SET_NAME, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) comm, 0, 0, 0);
}

ਪੂਰਨ proc_ਸੈੱਟ_nnp(ਖਾਲੀ) {
        ਵਾਪਸ prctl_ਸੁਰੱਖਿਅਤ(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(ਖਾਲੀ) {
        ਪੂਰਨ r;

        r = prctl_ਸੁਰੱਖਿਅਤ(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        ਜੇ (r < 0)
                ਵਾਪਸ NSEC_INFINITY;

        ਵਾਪਸ r;
}
