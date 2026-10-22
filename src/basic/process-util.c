/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/oom.h>
#ಅನ್ತರ್ಭಾವ <pthread.h>
#ಅನ್ತರ್ಭಾವ <spawn.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h>
#ಅನ್ತರ್ಭಾವ <sys/mount.h>
#ಅನ್ತರ್ಭಾವ <sys/personality.h>
#ಅನ್ತರ್ಭಾವ <sys/prctl.h>
#ಅನ್ತರ್ಭಾವ <sys/wait.h>
#ಅನ್ತರ್ಭಾವ <syslog.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಯದಿ HAVE_VALGRIND_VALGRIND_H
#ಅನ್ತರ್ಭಾವ <valgrind/valgrind.h>
#endif

#ಅನ್ತರ್ಭಾವ "sd-messages.h"

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "architecture.h"
#ಅನ್ತರ್ಭಾವ "argv-util.h"
#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "capability-util.h"
#ಅನ್ತರ್ಭಾವ "cgroup-util.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "dlfcn-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "locale-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "mountpoint-util.h"
#ಅನ್ತರ್ಭಾವ "namespace-util.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidfd-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "raw-clone.h"
#ಅನ್ತರ್ಭಾವ "rlimit-util.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_process_state(pid_t pid) {
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *p;
        ವರ್ಣ state;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        ಯದಿ (pid == 0 || pid == getpid_cached())
                ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        p = strrchr(ಸಾಲು, ')');
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        p++;

        ಯದಿ (sscanf(p, " %c", &state) != 1)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ) state;
}

ಪೂರ್ಣಾಙ್ಕ pid_get_comm(pid_t pid, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *escaped = ರಿಕ್ತ, *comm = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pid == 0 || pid == getpid_cached()) {
                comm = new0(ವರ್ಣ, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                ಯದಿ (!comm)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        } ಅನ್ಯಥಾ {
                ಧ್ರುವ ವರ್ಣ *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_ಸಾಲು_file(p, &comm);
                ಯದಿ (r == -ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        escaped = new(ವರ್ಣ, COMM_MAX_LEN);
        ಯದಿ (!escaped)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_comm(ಧ್ರುವ PidRef *pid, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *comm = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = TAKE_PTR(comm);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                ವರ್ಣ **ret,
                size_t *ret_size) {

        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *p;
        size_t k;
        ಪೂರ್ಣಾಙ್ಕ r;

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
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (k == 0) {
                ಯದಿ (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        ಪ್ರತ್ಯಾಗಮ -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ ವರ್ಣ *comm = ರಿಕ್ತ;

                r = pid_get_comm(pid, &comm);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಮುಕ್ತ(t);
                t = strjoin("[", comm, "]");
                ಯದಿ (!t)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                ಯದಿ (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        ಯದಿ (ret)
                *ret = TAKE_PTR(t);
        ಯದಿ (ret_size)
                *ret_size = k;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        size_t k;
        ವರ್ಣ *ans;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

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

        ಪೂರ್ಣಾಙ್ಕ full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        ಯದಿ (full < 0)
                ಪ್ರತ್ಯಾಗಮ full;

        ಯದಿ (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                ಪ್ರತಿಜ್ಞಾ(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ ವರ್ಣ **args = ರಿಕ್ತ;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                ಯದಿ (!args)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ans = quote_command_ಸಾಲು(args, shflags);
                ಯದಿ (!ans)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                ಪ್ರತಿ (size_t i = 0; i < k - 1; i++)
                        ಯದಿ (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                ಯದಿ (!ans)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_cmdline(ಧ್ರುವ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ವರ್ಣ ***ret) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ವರ್ಣ **args;
        size_t k;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        ಯದಿ (!args)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = args;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_cmdline_strv(ಧ್ರುವ PidRef *pid, ProcessCmdlineFlags flags, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **args = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = TAKE_PTR(args);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ procfs_get_stat_flags(ಧ್ರುವ ವರ್ಣ *path, unsigned long long *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        r = read_one_ಸಾಲು_file(path, &ಸಾಲು);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Skip past the comm field */
        ವರ್ಣ *q = strrchr(ಸಾಲು, ')');
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        ಪ್ರತಿ (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                ಯದಿ (l < 1)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                ಯದಿ (l < 1)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        ಯದಿ (l < 1)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        ಯದಿ (l < 1)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        q[l] = 0;

        ಪ್ರತ್ಯಾಗಮ safe_atollu(q, ret);
}

ಪೂರ್ಣಾಙ್ಕ pid_is_kernel_thread(pid_t pid) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (!pid_is_valid(pid))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಧ್ರುವ ವರ್ಣ *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ !!(flags & PF_KTHREAD);
}

ಪೂರ್ಣಾಙ್ಕ pidref_is_kernel_thread(ಧ್ರುವ PidRef *pid) {
        ಪೂರ್ಣಾಙ್ಕ result, r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        ಯದಿ (result < 0)
                ಪ್ರತ್ಯಾಗಮ result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_process_link_contents(pid_t pid, ಧ್ರುವ ವರ್ಣ *proc_file, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        ಪ್ರತ್ಯಾಗಮ (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

ಪೂರ್ಣಾಙ್ಕ get_process_exe(pid_t pid, ವರ್ಣ **ret) {
        ವರ್ಣ *d;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret) {
                d = endswith(*ret, " (deleted)");
                ಯದಿ (d)
                        *d = '\0';
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pid_get_uid(pid_t pid, uid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_uid(v, ret);
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_uid(ಧ್ರುವ PidRef *pid, uid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                ಯದಿ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = uid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_process_gid(pid_t pid, gid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_gid(v, ret);
}

ಪೂರ್ಣಾಙ್ಕ get_process_cwd(pid_t pid, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        ಯದಿ (pid == 0 || pid == getpid_cached())
                ಪ್ರತ್ಯಾಗಮ safe_getcwd(ret);

        ಪ್ರತ್ಯಾಗಮ get_process_link_contents(pid, "cwd", ret);
}

ಪೂರ್ಣಾಙ್ಕ get_process_ಬೇರು(pid_t pid, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತ್ಯಾಗಮ get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

ಪೂರ್ಣಾಙ್ಕ get_process_environ(pid_t pid, ವರ್ಣ **ret) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *outcome = ರಿಕ್ತ;
        size_t sz = 0;
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                ವರ್ಣ c;

                ಯದಿ (sz >= ENVIRONMENT_BLOCK_MAX)
                        ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                ಯದಿ (!GREEDY_REALLOC(outcome, sz + 5))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = safe_fgetc(f, &c);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (c == '\0')
                        outcome[sz++] = '\n';
                ಅನ್ಯಥಾ
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        unsigned long ppid;
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        ಯದಿ (pid == 0)
                pid = getpid_cached();
        ಯದಿ (pid == 1) /* PID 1 has no parent, shortcut this case */
                ಪ್ರತ್ಯಾಗಮ -EADDRNOTAVAIL;

        ಯದಿ (pid == getpid_cached()) {
                ಯದಿ (ret)
                        *ret = getppid();
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ಸಾಲು, ')');
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EIO;
        p++;

        ಯದಿ (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        ಯದಿ (ppid == 0)
                ಪ್ರತ್ಯಾಗಮ -EADDRNOTAVAIL;

        ಯದಿ ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret)
                *ret = (pid_t) ppid;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_ppid(ಧ್ರುವ PidRef *pidref, pid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                ಯದಿ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = ppid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_ppid_as_pidref(ಧ್ರುವ PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        ಪೂರ್ಣಾಙ್ಕ r;

        POINTER_MAY_BE_NULL(pidref);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = pidref_get_ppid(pidref, &ppid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                ಯದಿ (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        ಯದಿ (parent.pid != ppid)
                                ಅನುವರ್ತ;
                }

                *ret = TAKE_PIDREF(parent);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* Give up after 16 tries */
        ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;
}

ಪೂರ್ಣಾಙ್ಕ pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ಸಾಲು, ')');
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EIO;
        p++;

        unsigned long llu;

        ಯದಿ (sscanf(p, " "
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
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_get_start_time(ಧ್ರುವ PidRef *pid, usec_t *ret) {
        usec_t t;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = t;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ ವರ್ಣ *m = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_mode(m, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pid_get_sigmask(pid_t pid, ಧ್ರುವ ವರ್ಣ *field_name, uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *field = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(field_name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ safe_atou64_full(field, 16, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pidref_has_sigmask(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig, ಧ್ರುವ ವರ್ಣ *field_name) {
        uint64_t mask;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;
        ಯದಿ (!SIGNAL_VALID(sig))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ BIT_SET(mask, sig - 1);
}

ಪೂರ್ಣಾಙ್ಕ pidref_has_sigcgt(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig) {
        ಪ್ರತ್ಯಾಗಮ pidref_has_sigmask(pidref, sig, "SigCgt");
}

ಪೂರ್ಣಾಙ್ಕ pidref_has_sigblk(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig) {
        ಪ್ರತ್ಯಾಗಮ pidref_has_sigmask(pidref, sig, "SigBlk");
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
ಪೂರ್ಣಾಙ್ಕ pidref_wait_for_terminate_and_check(ಧ್ರುವ ವರ್ಣ *name, PidRef *pidref, WaitFlags flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;
        ಯದಿ (pidref->pid == 1 || pidref_is_self(pidref))
                ಪ್ರತ್ಯಾಗಮ -ECHILD;

        _cleanup_free_ ವರ್ಣ *buffer = ರಿಕ್ತ;
        ಯದಿ (!name) {
                r = pidref_get_comm(pidref, &buffer);
                ಯದಿ (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                ಅನ್ಯಥಾ
                        name = buffer;
        }

        ಪೂರ್ಣಾಙ್ಕ prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        ಯದಿ (status.si_code == CLD_EXITED) {
                ಯದಿ (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                ಅನ್ಯಥಾ
                        log_debug("'%s' succeeded.", name);

                ಪ್ರತ್ಯಾಗಮ status.si_status;

        } ಅನ್ಯಥಾ ಯದಿ (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

ಪೂರ್ಣಾಙ್ಕ kill_and_sigcont(pid_t pid, ಪೂರ್ಣಾಙ್ಕ sig) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        ಯದಿ (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (ಶೂನ್ಯ) kill(pid, SIGCONT);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ getenv_for_pid(pid_t pid, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ **ret) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *path;
        size_t sum = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(field);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pid == 0 || pid == getpid_cached())
                ಪ್ರತ್ಯಾಗಮ strdup_to_full(ret, getenv(field));

        ಯದಿ (!pid_is_valid(pid))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
                ಧ್ರುವ ವರ್ಣ *match;

                ಯದಿ (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                r = read_nul_string(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)  /* EOF */
                        ಭಙ್ಗ;

                sum += r;

                match = startswith(ಸಾಲು, field);
                ಯದಿ (match && *match == '=')
                        ಪ್ರತ್ಯಾಗಮ strdup_to_full(ret, match + 1);
        }

        *ret = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidref_is_my_child(PidRef *pid) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pid->pid == 1 || pidref_is_self(pid))
                ಪ್ರತ್ಯಾಗಮ false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        ಯದಿ (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ ppid == getpid_cached();
}

ಪೂರ್ಣಾಙ್ಕ pid_is_my_child(pid_t pid) {

        ಯದಿ (pid == 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

ಪೂರ್ಣಾಙ್ಕ pidref_is_unwaited(PidRef *pid) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Checks whether a PID is still valid at all, including a zombie */

        ಯದಿ (!pidref_is_set(pid))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pid))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        ಯದಿ (pid->pid == 1 || pidref_is_self(pid))
                ಪ್ರತ್ಯಾಗಮ true;

        r = pidref_kill(pid, 0);
        ಯದಿ (r == -ESRCH)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ pid_is_unwaited(pid_t pid) {

        ಯದಿ (pid == 0)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

ಪೂರ್ಣಾಙ್ಕ pid_is_alive(pid_t pid) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Checks whether a PID is still valid and not a zombie */

        ಯದಿ (pid < 0)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (pid == getpid_cached())
                ಪ್ರತ್ಯಾಗಮ true;

        r = get_process_state(pid);
        ಯದಿ (r == -ESRCH)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ r != 'Z';
}

ಪೂರ್ಣಾಙ್ಕ pidref_is_alive(ಧ್ರುವ PidRef *pidref) {
        ಪೂರ್ಣಾಙ್ಕ r, result;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        result = pid_is_alive(pidref->pid);
        ಯದಿ (result < 0) {
                ಪ್ರತಿಜ್ಞಾ(result != -ESRCH);
                ಪ್ರತ್ಯಾಗಮ result;
        }

        r = pidref_verify(pidref);
        ಯದಿ (r == -ESRCH)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ pidref_from_same_ಬೇರು_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        ಯದಿ (!a || !b) {
                r = pidref_set_self(&self);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (!a)
                        a = &self;
                ಯದಿ (!b)
                        b = &self;
        }

        ಯದಿ (!pidref_is_set(a) || !pidref_is_set(b))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        ಯದಿ (pidref_is_remote(a) && pidref_is_remote(b))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;
        ಯದಿ (pidref_is_remote(a) || pidref_is_remote(b))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (pidref_equal(a, b))
                ಪ್ರತ್ಯಾಗಮ true;

        ಧ್ರುವ ವರ್ಣ *roota = procfs_file_alloca(a->pid, "root");
        ಧ್ರುವ ವರ್ಣ *rootb = procfs_file_alloca(b->pid, "root");

        ಪೂರ್ಣಾಙ್ಕ result = inode_same(roota, rootb, 0);
        ಯದಿ (result == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        ಯದಿ (result < 0)
                ಪ್ರತ್ಯಾಗಮ result;

        r = pidref_verify(a);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        r = pidref_verify(b);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ result;
}

bool is_main_thread(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಙ್ಕ cached = -1;

        ಯದಿ (cached < 0)
                cached = getpid_cached() == gettid();

        ಪ್ರತ್ಯಾಗಮ cached;
}

unsigned long personality_from_string(ಧ್ರುವ ವರ್ಣ *s) {
        Architecture architecture;

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        ಯದಿ (architecture < 0)
                ಪ್ರತ್ಯಾಗಮ PERSONALITY_INVALID;

        ಯದಿ (architecture == native_architecture())
                ಪ್ರತ್ಯಾಗಮ PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        ಯದಿ (architecture == ARCHITECTURE_SECONDARY)
                ಪ್ರತ್ಯಾಗಮ PER_LINUX32;
#endif

        ಪ್ರತ್ಯಾಗಮ PERSONALITY_INVALID;
}

ಧ್ರುವ ವರ್ಣ* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        ಯದಿ (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        ಅನ್ಯಥಾ ಯದಿ (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        ಯದಿ (architecture < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ architecture_to_string(architecture);
}

ಪೂರ್ಣಾಙ್ಕ safe_personality(unsigned long p) {
        ಪೂರ್ಣಾಙ್ಕ ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        ಯದಿ (ret < 0) {
                ಯದಿ (errno != 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                errno = -ret;
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ opinionated_personality(unsigned long *ret) {
        ಪೂರ್ಣಾಙ್ಕ current;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        ಯದಿ (current < 0)
                ಪ್ರತ್ಯಾಗಮ current;

        ಯದಿ (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        ಅನ್ಯಥಾ
                *ret = PER_LINUX;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ valgrind_summary_hack(ಶೂನ್ಯ) {
#ಯದಿ HAVE_VALGRIND_VALGRIND_H
        ಯದಿ (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                ಯದಿ (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                ಅನ್ಯಥಾ ಯದಿ (pid == 0)
                        exit(EXIT_SUCCESS);
                ಅನ್ಯಥಾ {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (ಶೂನ್ಯ) pidref_set_pid(&pidref, pid);
                        (ಶೂನ್ಯ) pidref_wait_for_terminate(&pidref, ರಿಕ್ತ);
                }
        }
#endif
}

ಪೂರ್ಣಾಙ್ಕ pid_compare_func(ಧ್ರುವ pid_t *a, ಧ್ರುವ pid_t *b) {
        /* Suitable for usage in qsort() */
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ CMP(*a, *b);
}

bool nice_is_valid(ಪೂರ್ಣಾಙ್ಕ n) {
        ಪ್ರತ್ಯಾಗಮ n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(ಪೂರ್ಣಾಙ್ಕ policy) {
        ಪ್ರತ್ಯಾಗಮ IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(ಪೂರ್ಣಾಙ್ಕ policy) {
        ಪ್ರತ್ಯಾಗಮ sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
ಪೂರ್ಣಾಙ್ಕ sched_get_priority_min_safe(ಪೂರ್ಣಾಙ್ಕ policy) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = sched_get_priority_min(policy);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Fallback priority */
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sched_get_priority_max_safe(ಪೂರ್ಣಾಙ್ಕ policy) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = sched_get_priority_max(policy);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

ಸ್ಥಿರ pid_t cached_pid = CACHED_PID_UNSET;

ಶೂನ್ಯ reset_cached_pid(ಶೂನ್ಯ) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(ಶೂನ್ಯ) {
        ಸ್ಥಿರ bool installed = false;
        pid_t current_ಮೌಲ್ಯ;

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
        current_ಮೌಲ್ಯ = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        ಯದಿ (current_ಮೌಲ್ಯ > 0)
                ಪ್ರತ್ಯಾಗಮ current_ಮೌಲ್ಯ;

        current_ಮೌಲ್ಯ = CACHED_PID_UNSET;
        (ಶೂನ್ಯ) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_ಮೌಲ್ಯ,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        ವಿಕಲ್ಪ (current_ಮೌಲ್ಯ) {

        ಪಕ್ಷ CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                ಯದಿ (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        ಯದಿ (pthread_atfork(ರಿಕ್ತ, ರಿಕ್ತ, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                ಪ್ರತ್ಯಾಗಮ new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                ಪ್ರತ್ಯಾಗಮ new_pid;
        }

        ಪಕ್ಷ CACHED_PID_BUSY: /* Somebody else is currently initializing */
                ಪ್ರತ್ಯಾಗಮ getpid();

        ಸ್ವಭಾವ: /* Properly initialized */
                ಪ್ರತ್ಯಾಗಮ current_ಮೌಲ್ಯ;
        }
}

ಪೂರ್ಣಾಙ್ಕ must_be_ಬೇರು(ಶೂನ್ಯ) {

        ಯದಿ (geteuid() == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ರಾಶಿ(ಪೂರ್ಣಾಙ್ಕ (*fn)(ಶೂನ್ಯ *), ಪೂರ್ಣಾಙ್ಕ flags, ಶೂನ್ಯ *userdata) {
        size_t ps;
        pid_t pid;
        ಶೂನ್ಯ *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        ಪ್ರತಿಜ್ಞಾ((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (ಶೂನ್ಯ*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#ಯದಿ HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#ಅನ್ಯಥಾ
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        ಯದಿ (pid < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ pid;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fork_flags_to_signal(ForkFlags flags) {
        ಪ್ರತ್ಯಾಗಮ (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

ಪೂರ್ಣಾಙ್ಕ pidref_safe_fork_full(
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ stdio_fds[3],
                ಪೂರ್ಣಾಙ್ಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = ರಿಕ್ತ;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ ಪೂರ್ಣಾಙ್ಕ pidref_transport_fds[2] = EBADF_PAIR;
        ಪೂರ್ಣಾಙ್ಕ prio, r;

        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        ಯದಿ (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        ಯದಿ (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } ಅನ್ಯಥಾ ಯದಿ (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        ಯದಿ (block_signals) {
                ಯದಿ (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        ಯದಿ (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                ಯದಿ (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        ಯದಿ (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        ಯದಿ (pid < 0)
                                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        ಯದಿ (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                ಯದಿ (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        ಯದಿ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                ಪ್ರತ್ಯಾಗಮ -EPROTO;

                                        ಪೂರ್ಣಾಙ್ಕ pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, ಪರಿಮಾಣ(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        ಯದಿ (n < 0)
                                                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                ಪ್ರತ್ಯಾಗಮ 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        ಯದಿ ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        ಅನ್ಯಥಾ
                pid = fork();
        ಯದಿ (pid < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        ಯದಿ (pid > 0) {

                /* If we are in the intermediary process, exit now */
                ಯದಿ (intermediary) {
                        ಯದಿ (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                ಯದಿ (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, ಪರಿಮಾಣ(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                ಯದಿ (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                ಯದಿ (flags & FORK_WAIT) {
                        ಯದಿ (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (ಶೂನ್ಯ) sigprocmask(SIG_SETMASK, &ss, ರಿಕ್ತ);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                ಪ್ರತ್ಯಾಗಮ -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        ಯದಿ (ret)
                                *ret = PIDREF_NULL;

                        ಪ್ರತ್ಯಾಗಮ 1;
                }

                ಯದಿ (ret) {
                        r = pidref_set_pid(ret, pid);
                        ಯದಿ (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                ಪ್ರತ್ಯಾಗಮ 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ರಿಕ್ತ;

        ಯದಿ (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        ಯದಿ (name) {
                r = rename_process(name);
                ಯದಿ (r < 0)
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
        ಯದಿ (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        ಯದಿ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } ಅನ್ಯಥಾ ಯದಿ (block_signals) { /* undo what we did above */
                ಯದಿ (sigprocmask(SIG_SETMASK, &saved_ss, ರಿಕ್ತ) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                ಯದಿ (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                ಅನ್ಯಥಾ ಯದಿ (ppid != original_pid) {
                        ಪೂರ್ಣಾಙ್ಕ sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (ಶೂನ್ಯ) ಉತ್ಥಾಪಯ(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                ಯದಿ (mount(ರಿಕ್ತ, "/", ರಿಕ್ತ, MS_SLAVE | MS_REC, ರಿಕ್ತ) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (flags & FORK_REARRANGE_STDIO) {
                ಯದಿ (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        ಯದಿ (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } ಅನ್ಯಥಾ {
                        r = make_null_stdio();
                        ಯದಿ (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } ಅನ್ಯಥಾ ಯದಿ (flags & FORK_STDOUT_TO_STDERR) {
                ಯದಿ (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        ಯದಿ (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        ಯದಿ (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಯದಿ (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        ಯದಿ (ret) {
                r = pidref_set_self(ret);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ namespace_fork_full(
                ಧ್ರುವ ವರ್ಣ *outer_name,
                ಧ್ರುವ ವರ್ಣ *inner_name,
                ಪೂರ್ಣಾಙ್ಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ಪೂರ್ಣಾಙ್ಕ pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ netns_fd,
                ಪೂರ್ಣಾಙ್ಕ userns_fd,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ ಪೂರ್ಣಾಙ್ಕ errno_pipe_fd[2] = EBADF_PAIR;
        ಪೂರ್ಣಾಙ್ಕ r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        ಪ್ರತಿಜ್ಞಾ((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        ಪ್ರತಿಜ್ಞಾ((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) <= PIPE_BUF);
        ಯದಿ (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ ರಿಕ್ತ, /* except_fds= */ ರಿಕ್ತ, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        ಯದಿ (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                ಪೂರ್ಣಾಙ್ಕ k = read_errno(errno_pipe_fd[0]);
                ಯದಿ (k < 0 && k != -EIO)
                        ಪ್ರತ್ಯಾಗಮ k;
        }
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, ಬೇರು_fd, namespace_flags);
                ಯದಿ (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                ರಿಕ್ತ,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                ಯದಿ (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                ಯದಿ (r == 0) {
                        /* Child */

                        ಯದಿ (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } ಅನ್ಯಥಾ {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        ಯದಿ (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (ಶೂನ್ಯ) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                ಯದಿ (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r; /* the child logs about failures on its own, no need to duplicate here */

        ಯದಿ (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        ಅನ್ಯಥಾ
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ namespace_fork(
                ಧ್ರುವ ವರ್ಣ *outer_name,
                ಧ್ರುವ ವರ್ಣ *inner_name,
                ForkFlags flags,
                ಪೂರ್ಣಾಙ್ಕ pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ netns_fd,
                ಪೂರ್ಣಾಙ್ಕ userns_fd,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                PidRef *ret) {

        ಪ್ರತ್ಯಾಗಮ namespace_fork_full(outer_name, inner_name, /* except_fds= */ ರಿಕ್ತ, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, ಬೇರು_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(ಪೂರ್ಣಾಙ್ಕ oa) {
        ಪ್ರತ್ಯಾಗಮ oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

ಪೂರ್ಣಾಙ್ಕ set_oom_score_adjust(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ) {
        ವರ್ಣ t[DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ)];

        ಯದಿ (!oom_score_adjust_is_valid(ಮೌಲ್ಯ))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        xsprintf(t, "%i", ಮೌಲ್ಯ);

        ಪ್ರತ್ಯಾಗಮ write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

ಪೂರ್ಣಾಙ್ಕ get_oom_score_adjust(ಪೂರ್ಣಾಙ್ಕ *ret) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!oom_score_adjust_is_valid(a))
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಯದಿ (ret)
                *ret = a;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rlimit_to_nice(rlim_t limit) {
        ಯದಿ (limit <= 1)
                ಪ್ರತ್ಯಾಗಮ PRIO_MAX-1; /* i.e. 19 */

        ಯದಿ (limit >= -PRIO_MIN + PRIO_MAX)
                ಪ್ರತ್ಯಾಗಮ PRIO_MIN; /* i.e. -20 */

        ಪ್ರತ್ಯಾಗಮ PRIO_MAX - (ಪೂರ್ಣಾಙ್ಕ) limit;
}

ಪೂರ್ಣಾಙ್ಕ setpriority_closest(ಪೂರ್ಣಾಙ್ಕ priority) {
        ಸಂರಚನಾ rlimit highest;
        ಪೂರ್ಣಾಙ್ಕ r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ 1;
        ಯದಿ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ಪ್ರತ್ಯಾಗಮ r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        ಯದಿ (errno != 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (priority == current)
                ಪ್ರತ್ಯಾಗಮ 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        ಯದಿ (priority > current)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (getrlimit(RLIMIT_NICE, &highest) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        ಯದಿ (current > limit)
                ಯದಿ (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        ಪ್ರತ್ಯಾಗಮ 0;
}

_noreturn_ ಶೂನ್ಯ freeze(ಶೂನ್ಯ) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (ಶೂನ್ಯ) close_all_fds_without_malloc(ರಿಕ್ತ, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        ಪ್ರತಿ (;;) {
                siginfo_t si = {};

                ಯದಿ (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        ಭಙ್ಗ;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        ಪ್ರತಿ (;;)
                pause();
}

ಪೂರ್ಣಾಙ್ಕ get_process_threads(pid_t pid) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ n, r;

        ಯದಿ (pid < 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = safe_atoi(t, &n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ is_reaper_process(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        ಯದಿ (getpid_cached() == 1)
                ಪ್ರತ್ಯಾಗಮ true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ b != 0;
}

ಪೂರ್ಣಾಙ್ಕ make_reaper_process(bool b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (getpid_cached() == 1) {

                ಯದಿ (!b)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, ರಿಕ್ತ);

ಪೂರ್ಣಾಙ್ಕ posix_spawn_wrapper(
                ಧ್ರುವ ವರ್ಣ *path,
                ವರ್ಣ * ಧ್ರುವ *argv,
                ವರ್ಣ * ಧ್ರುವ *envp,
                ಧ್ರುವ ವರ್ಣ *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        ಪೂರ್ಣಾಙ್ಕ r;

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

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(argv);
        ಪ್ರತಿಜ್ಞಾ(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        ಸ್ಥಿರ bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ cgroup_fd = -EBADF;

        ಯದಿ (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ ವರ್ಣ *resolved_cgroup = ರಿಕ್ತ;

                r = cg_get_path(cgroup, /* suffix= */ ರಿಕ್ತ, &resolved_cgroup);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ಯದಿ (cgroup_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                ಯದಿ (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                ಅನ್ಯಥಾ ಯದಿ (r != ENOSYS)
                        ಪ್ರತ್ಯಾಗಮ -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ -r;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, ರಿಕ್ತ, &attr, argv, envp);
        ಯದಿ (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                ಪ್ರತ್ಯಾಗಮ -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        ಯದಿ ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ -r;

                r = pidfd_spawn(&pidfd, path, ರಿಕ್ತ, &attr, argv, envp);
        }
        ಯದಿ (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(r))
                ಪ್ರತ್ಯಾಗಮ -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, ರಿಕ್ತ, &attr, argv, envp);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ -r;

        r = pidref_set_pid(ret_pidref, pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

ಪೂರ್ಣಾಙ್ಕ proc_dir_open(DIR **ret) {
        DIR *d;

        ಪ್ರತಿಜ್ಞಾ(ret);

        d = opendir("/proc");
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = d;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ proc_dir_read(DIR *d, pid_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(d);

        ಪ್ರತಿ (;;) {
                ಸಂರಚನಾ dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                ಯದಿ (!de) {
                        ಯದಿ (errno != 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        ಭಙ್ಗ;
                }

                ಯದಿ (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        ಅನುವರ್ತ;

                ಯದಿ (parse_pid(de->d_name, ret) >= 0)
                        ಪ್ರತ್ಯಾಗಮ 1;
        }

        ಯದಿ (ret)
                *ret = 0;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ proc_dir_read_pidref(DIR *d, PidRef *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(d);

        ಪ್ರತಿ (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                r = pidref_set_pid(ret, pid);
                ಯದಿ (r == -ESRCH) /* gone by now? skip it */
                        ಅನುವರ್ತ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ 1;
        }

        ಯದಿ (ret)
                *ret = PIDREF_NULL;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_mlockall(ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        ಯದಿ (mlockall(flags) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, ಪೂರ್ಣಾಙ್ಕ, INT_MAX);

_noreturn_ ಶೂನ್ಯ report_errno_and_exit(ಪೂರ್ಣಾಙ್ಕ errno_fd, ಪೂರ್ಣಾಙ್ಕ error) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (error >= 0)
                _exit(EXIT_SUCCESS);

        ಪ್ರತಿಜ್ಞಾ(errno_fd >= 0);

        r = loop_write(errno_fd, &error, ಪರಿಮಾಣ(error));
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

ಪೂರ್ಣಾಙ್ಕ read_errno(ಪೂರ್ಣಾಙ್ಕ errno_fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, ಪರಿಮಾಣ(r), /* do_poll= */ false);
        ಯದಿ (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                ಪ್ರತ್ಯಾಗಮ -EIO;
        }
        ಯದಿ (n == 0) /* the process exited without reporting an error, assuming success */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (n != ಪರಿಮಾಣ(r))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (r < 0) /* child process reported an error, return it */
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Child process failed with errno: %m");

        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

ಪೂರ್ಣಾಙ್ಕ prctl_safe(ಪೂರ್ಣಾಙ್ಕ op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

ಪೂರ್ಣಾಙ್ಕ proc_set_comm(ಧ್ರುವ ವರ್ಣ *comm) {
        ಪ್ರತ್ಯಾಗಮ prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

ಪೂರ್ಣಾಙ್ಕ proc_set_nnp(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ NSEC_INFINITY;

        ಪ್ರತ್ಯಾಗಮ r;
}
