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
#ಆದರೆ HAVE_VALGRIND_VALGRIND_H
#ಅನ್ತರ್ಭಾವ <valgrind/valgrind.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿಸು COMM_MAX_LEN 128

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_process_state(pid_t pid) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಅಕ್ಷರ state;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        ಆದರೆ (pid == 0 || pid == getpid_cached())
                ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        p = strrchr(ಸಾಲು, ')');
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EIO;

        p++;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(p, " %c", &state) != 1)
                ಹಿಂತಿರುಗಿ -EIO;

        ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) state;
}

ಪೂರ್ಣಾಂಕ pid_get_comm(pid_t pid, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *escaped = ಖಾಲಿ, *comm = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pid == 0 || pid == getpid_cached()) {
                comm = new0(ಅಕ್ಷರ, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                ಆದರೆ (!comm)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = prctl_ಸುರಕ್ಷಿತ(PR_GET_NAME, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) comm, 0, 0, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_ಸಾಲು_file(p, &comm);
                ಆದರೆ (r == -ENOENT)
                        ಹಿಂತಿರುಗಿ -ESRCH;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        escaped = new(ಅಕ್ಷರ, COMM_MAX_LEN);
        ಆದರೆ (!escaped)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_get_comm(ಸ್ಥಿರಾಂಕ PidRef *pid, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *comm = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(comm);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_ಗಾತ್ರ,
                ProcessCmdlineFlags flags,
                ಅಕ್ಷರ **ret,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        size_t k;
        ಪೂರ್ಣಾಂಕ r;

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
        r = read_virtual_file(p, max_ಗಾತ್ರ, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (k == 0) {
                ಆದರೆ (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        ಹಿಂತಿರುಗಿ -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ ಅಕ್ಷರ *comm = ಖಾಲಿ;

                r = pid_get_comm(pid, &comm);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಬಿಡುಗಡೆ(t);
                t = strjoin("[", comm, "]");
                ಆದರೆ (!t)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                k = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(t);
                r = k <= max_ಗಾತ್ರ;
                ಆದರೆ (r == 0) /* truncation */
                        t[max_ಗಾತ್ರ] = '\0';
        }

        ಆದರೆ (ret)
                *ret = TAKE_PTR(t);
        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = k;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        size_t k;
        ಅಕ್ಷರ *ans;

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

        ಪೂರ್ಣಾಂಕ ತುಂಬಿದ = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        ಆದರೆ (ತುಂಬಿದ < 0)
                ಹಿಂತಿರುಗಿ ತುಂಬಿದ;

        ಆದರೆ (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_ಖಾಲಿಗಳು |
                        FLAGS_ಗಣ(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                ಪ್ರತಿಜ್ಞಾ(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ ಅಕ್ಷರ **ವಾದಗಳು = ಖಾಲಿ;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                ವಾದಗಳು = strv_parse_nulstr_ತುಂಬಿದ(t, k, /* drop_trailing_nuls= */ true);
                ಆದರೆ (!ವಾದಗಳು)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ans = quote_command_ಸಾಲು(ವಾದಗಳು, shflags);
                ಆದರೆ (!ans)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                ಗಾಗಿ (size_t i = 0; i < k - 1; i++)
                        ಆದರೆ (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_ಬಿಟ್ = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_ತುಂಬಿದ(t, max_columns,
                                                eight_ಬಿಟ್ * XESCAPE_8_ಬಿಟ್ | !ತುಂಬಿದ * XESCAPE_FORCE_ELLIPSIS);
                ಆದರೆ (!ans)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_get_cmdline(ಸ್ಥಿರಾಂಕ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ಅಕ್ಷರ ***ret) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಅಕ್ಷರ **ವಾದಗಳು;
        size_t k;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ವಾದಗಳು = strv_parse_nulstr_ತುಂಬಿದ(t, k, /* drop_trailing_nuls= */ true);
        ಆದರೆ (!ವಾದಗಳು)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = ವಾದಗಳು;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_get_cmdline_strv(ಸ್ಥಿರಾಂಕ PidRef *pid, ProcessCmdlineFlags flags, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **ವಾದಗಳು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &ವಾದಗಳು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(ವಾದಗಳು);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ procfs_get_stat_flags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        r = read_one_ಸಾಲು_file(ಮಾರ್ಗ, &ಸಾಲು);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Skip past the comm field */
        ಅಕ್ಷರ *q = strrchr(ಸಾಲು, ')');
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        ಗಾಗಿ (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                ಆದರೆ (l < 1)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                ಆದರೆ (l < 1)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        ಆದರೆ (l < 1)
                ಹಿಂತಿರುಗಿ -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        ಆದರೆ (l < 1)
                ಹಿಂತಿರುಗಿ -EINVAL;
        q[l] = 0;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atollu(q, ret);
}

ಪೂರ್ಣಾಂಕ pid_is_kernel_thread(pid_t pid) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (ಒಳ_ಗಣ(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (!pid_is_valid(pid))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = procfs_file_alloca(pid, "stat");

        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ flags;
        r = procfs_get_stat_flags(p, &flags);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ !!(flags & PF_KTHREAD);
}

ಪೂರ್ಣಾಂಕ pidref_is_kernel_thread(ಸ್ಥಿರಾಂಕ PidRef *pid) {
        ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ, r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಫಲಿತಾಂಶ = pid_is_kernel_thread(pid->pid);
        ಆದರೆ (ಫಲಿತಾಂಶ < 0)
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_process_link_contents(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proc_file, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        ಹಿಂತಿರುಗಿ (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

ಪೂರ್ಣಾಂಕ get_process_exe(pid_t pid, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *d;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret) {
                d = endswith(*ret, " (deleted)");
                ಆದರೆ (d)
                        *d = '\0';
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pid_get_uid(pid_t pid, uid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        r = procfs_file_get_ಕ್ಷೇತ್ರ(pid, "status", "Uid", &v);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_uid(v, ret);
}

ಪೂರ್ಣಾಂಕ pidref_get_uid(ಸ್ಥಿರಾಂಕ PidRef *pid, uid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                ಆದರೆ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಹಿಂತಿರುಗಿ r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = uid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_process_gid(pid_t pid, gid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        r = procfs_file_get_ಕ್ಷೇತ್ರ(pid, "status", "Gid", &v);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_gid(v, ret);
}

ಪೂರ್ಣಾಂಕ get_process_cwd(pid_t pid, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        ಆದರೆ (pid == 0 || pid == getpid_cached())
                ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_getcwd(ret);

        ಹಿಂತಿರುಗಿ get_process_link_contents(pid, "cwd", ret);
}

ಪೂರ್ಣಾಂಕ get_process_ಬೇರು(pid_t pid, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಹಿಂತಿರುಗಿ get_process_link_contents(pid, "root", ret);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ENVIRONMENT_ಖಂಡ_MAX (5U*1024U*1024U)

ಪೂರ್ಣಾಂಕ get_process_environ(pid_t pid, ಅಕ್ಷರ **ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *outcome = ಖಾಲಿ;
        size_t sz = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ c;

                ಆದರೆ (sz >= ENVIRONMENT_ಖಂಡ_MAX)
                        ಹಿಂತಿರುಗಿ -ENOBUFS;

                ಆದರೆ (!GREEDY_REALLOC(outcome, sz + 5))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = ಸುರಕ್ಷಿತ_fgetc(f, &c);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (c == '\0')
                        outcome[sz++] = '\n';
                ಇಲ್ಲದಿದ್ದರೆ
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ppid;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        ಆದರೆ (pid == 0)
                pid = getpid_cached();
        ಆದರೆ (pid == 1) /* PID 1 has no parent, shortcut this case */
                ಹಿಂತಿರುಗಿ -EADDRNOTAVAIL;

        ಆದರೆ (pid == getpid_cached()) {
                ಆದರೆ (ret)
                        *ret = getppid();
                ಹಿಂತಿರುಗಿ 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ಸಾಲು, ')');
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EIO;
        p++;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                ಹಿಂತಿರುಗಿ -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        ಆದರೆ (ppid == 0)
                ಹಿಂತಿರುಗಿ -EADDRNOTAVAIL;

        ಆದರೆ ((pid_t) ppid < 0 || (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) (pid_t) ppid != ppid)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret)
                *ret = (pid_t) ppid;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_get_ppid(ಸ್ಥಿರಾಂಕ PidRef *pidref, pid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                ಆದರೆ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಹಿಂತಿರುಗಿ r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = ppid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_get_ppid_as_pidref(ಸ್ಥಿರಾಂಕ PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        ಪೂರ್ಣಾಂಕ r;

        ಸೂಚಕ_MAY_BE_NULL(pidref);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = pidref_get_ppid(pidref, &ppid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_ಮುಗಿದ) PidRef ಪೋಷಕ = PIDREF_NULL;

                r = pidref_ಗಣ_pid(&ಪೋಷಕ, ppid);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                ಆದರೆ (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        ಆದರೆ (ಪೋಷಕ.pid != ppid)
                                ಮುಂದುವರಿ;
                }

                *ret = TAKE_PIDREF(ಪೋಷಕ);
                ಹಿಂತಿರುಗಿ 0;
        }

        /* Give up after 16 tries */
        ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;
}

ಪೂರ್ಣಾಂಕ pid_get_ಆರಂಭ_ಸಮಯ(pid_t pid, usec_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ಸಾಲು, ')');
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EIO;
        p++;

        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ llu;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(p, " "
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
                   "%*i " /*ಆದ್ಯತೆ*/
                   "%*i " /* nice */
                   "%*u " /* num_threads */
                   "%*u " /* itrealvalue */
                   "%lu ", /* starttime */
                   &llu) != 1)
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (ret)
                *ret = jiffies_ಗಮ್ಯ_usec(llu); /* CLOCK_BOOTTIME */

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_get_ಆರಂಭ_ಸಮಯ(ಸ್ಥಿರಾಂಕ PidRef *pid, usec_t *ret) {
        usec_t t;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        r = pid_get_ಆರಂಭ_ಸಮಯ(pid->pid, ret ? &t : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = t;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *m = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = procfs_file_get_ಕ್ಷೇತ್ರ(pid, "status", "Umask", &m);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_ಬಹುಲಕ(m, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pid_get_sigmask(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ_name, uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಕ್ಷೇತ್ರ_name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = procfs_file_get_ಕ್ಷೇತ್ರ(pid, "status", ಕ್ಷೇತ್ರ_name, &ಕ್ಷೇತ್ರ);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou64_ತುಂಬಿದ(ಕ್ಷೇತ್ರ, 16, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pidref_has_sigmask(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ_name) {
        uint64_t ಮುಖವಾಡ_3;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;
        ಆದರೆ (!SIGNAL_VALID(sig))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = pid_get_sigmask(pidref->pid, ಕ್ಷೇತ್ರ_name, &ಮುಖವಾಡ_3);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಬಿಟ್_ಗಣ(ಮುಖವಾಡ_3, sig - 1);
}

ಪೂರ್ಣಾಂಕ pidref_has_sigcgt(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig) {
        ಹಿಂತಿರುಗಿ pidref_has_sigmask(pidref, sig, "SigCgt");
}

ಪೂರ್ಣಾಂಕ pidref_has_sigblk(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig) {
        ಹಿಂತಿರುಗಿ pidref_has_sigmask(pidref, sig, "SigBlk");
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
ಪೂರ್ಣಾಂಕ pidref_wait_for_terminate_and_check(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, PidRef *pidref, WaitFlags flags) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;
        ಆದರೆ (pidref->pid == 1 || pidref_is_self(pidref))
                ಹಿಂತಿರುಗಿ -ECHILD;

        _cleanup_free_ ಅಕ್ಷರ *ಬಫರ್ = ಖಾಲಿ;
        ಆದರೆ (!name) {
                r = pidref_get_comm(pidref, &ಬಫರ್);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                ಇಲ್ಲದಿದ್ದರೆ
                        name = ಬಫರ್;
        }

        ಪೂರ್ಣಾಂಕ prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        ಆದರೆ (status.si_code == CLD_EXITED) {
                ಆದರೆ (status.si_status != EXIT_SUCCESS)
                        log_ತುಂಬಿದ(flags & WAIT_LOG_NON_ZERO_EXIT_ಸ್ಥಿತಿ ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                ಇಲ್ಲದಿದ್ದರೆ
                        log_debug("'%s' succeeded.", name);

                ಹಿಂತಿರುಗಿ status.si_status;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(status.si_code, CLD_KILLED, CLD_DUMPED))
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_ಗಮ್ಯ_string(status.si_status));

        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

ಪೂರ್ಣಾಂಕ kill_and_sigcont(pid_t pid, ಪೂರ್ಣಾಂಕ sig) {
        ಪೂರ್ಣಾಂಕ r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        ಆದರೆ (r >= 0 && !ಒಳ_ಗಣ(sig, SIGCONT, SIGKILL))
                (ಶೂನ್ಯ) kill(pid, SIGCONT);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ getenv_for_pid(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ **ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;
        size_t ಮೊತ್ತ = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಕ್ಷೇತ್ರ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pid == 0 || pid == getpid_cached())
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret, getenv(ಕ್ಷೇತ್ರ));

        ಆದರೆ (!pid_is_valid(pid))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಮಾರ್ಗ = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(ಮಾರ್ಗ, "re", &f);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಹೊಂದಿಕೆ;

                ಆದರೆ (ಮೊತ್ತ > ENVIRONMENT_ಖಂಡ_MAX) /* Give up searching eventually */
                        ಹಿಂತಿರುಗಿ -ENOBUFS;

                r = read_nul_string(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)  /* EOF */
                        ನಿಲ್ಲಿಸು;

                ಮೊತ್ತ += r;

                ಹೊಂದಿಕೆ = startswith(ಸಾಲು, ಕ್ಷೇತ್ರ);
                ಆದರೆ (ಹೊಂದಿಕೆ && *ಹೊಂದಿಕೆ == '=')
                        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret, ಹೊಂದಿಕೆ + 1);
        }

        *ret = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidref_is_my_ಮಗು(PidRef *pid) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pid->pid == 1 || pidref_is_self(pid))
                ಹಿಂತಿರುಗಿ false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        ಆದರೆ (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ppid == getpid_cached();
}

ಪೂರ್ಣಾಂಕ pid_is_my_ಮಗು(pid_t pid) {

        ಆದರೆ (pid == 0)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ pidref_is_my_ಮಗು(&PIDREF_MAKE_FROM_PID(pid));
}

ಪೂರ್ಣಾಂಕ pidref_is_unwaited(PidRef *pid) {
        ಪೂರ್ಣಾಂಕ r;

        /* Checks whether a PID is still valid at all, including a zombie */

        ಆದರೆ (!pidref_is_ಗಣ(pid))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pid))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pid->pid == 1 || pidref_is_self(pid))
                ಹಿಂತಿರುಗಿ true;

        r = pidref_kill(pid, 0);
        ಆದರೆ (r == -ESRCH)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ pid_is_unwaited(pid_t pid) {

        ಆದರೆ (pid == 0)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

ಪೂರ್ಣಾಂಕ pid_is_alive(pid_t pid) {
        ಪೂರ್ಣಾಂಕ r;

        /* Checks whether a PID is still valid and not a zombie */

        ಆದರೆ (pid < 0)
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (pid == getpid_cached())
                ಹಿಂತಿರುಗಿ true;

        r = get_process_state(pid);
        ಆದರೆ (r == -ESRCH)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ r != 'Z';
}

ಪೂರ್ಣಾಂಕ pidref_is_alive(ಸ್ಥಿರಾಂಕ PidRef *pidref) {
        ಪೂರ್ಣಾಂಕ r, ಫಲಿತಾಂಶ;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಫಲಿತಾಂಶ = pid_is_alive(pidref->pid);
        ಆದರೆ (ಫಲಿತಾಂಶ < 0) {
                ಪ್ರತಿಜ್ಞಾ(ಫಲಿತಾಂಶ != -ESRCH);
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }

        r = pidref_verify(pidref);
        ಆದರೆ (r == -ESRCH)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ pidref_from_same_ಬೇರು_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_ಮುಗಿದ) PidRef self = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        ಆದರೆ (!a || !b) {
                r = pidref_ಗಣ_self(&self);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (!a)
                        a = &self;
                ಆದರೆ (!b)
                        b = &self;
        }

        ಆದರೆ (!pidref_is_ಗಣ(a) || !pidref_is_ಗಣ(b))
                ಹಿಂತಿರುಗಿ -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        ಆದರೆ (pidref_is_remote(a) && pidref_is_remote(b))
                ಹಿಂತಿರುಗಿ -EREMOTE;
        ಆದರೆ (pidref_is_remote(a) || pidref_is_remote(b))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (pidref_equal(a, b))
                ಹಿಂತಿರುಗಿ true;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *roota = procfs_file_alloca(a->pid, "root");
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rootb = procfs_file_alloca(b->pid, "root");

        ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = inode_same(roota, rootb, 0);
        ಆದರೆ (ಫಲಿತಾಂಶ == -ENOENT)
                ಹಿಂತಿರುಗಿ proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        ಆದರೆ (ಫಲಿತಾಂಶ < 0)
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        r = pidref_verify(a);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        r = pidref_verify(b);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool is_main_thread(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಂಕ cached = -1;

        ಆದರೆ (cached < 0)
                cached = getpid_cached() == gettid();

        ಹಿಂತಿರುಗಿ cached;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ personality_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        Architecture architecture;

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        ಆದರೆ (architecture < 0)
                ಹಿಂತಿರುಗಿ PERSONALITY_INVALID;

        ಆದರೆ (architecture == native_architecture())
                ಹಿಂತಿರುಗಿ PER_LINUX;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARCHITECTURE_SECONDARY
        ಆದರೆ (architecture == ARCHITECTURE_SECONDARY)
                ಹಿಂತಿರುಗಿ PER_LINUX32;
#ಆದರೆ_ಅಂತ್ಯ

        ಹಿಂತಿರುಗಿ PERSONALITY_INVALID;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* personality_ಗಮ್ಯ_string(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        ಆದರೆ (p == PER_LINUX)
                architecture = native_architecture();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARCHITECTURE_SECONDARY
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (architecture < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ architecture_ಗಮ್ಯ_string(architecture);
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_personality(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ p) {
        ಪೂರ್ಣಾಂಕ ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        ಆದರೆ (ret < 0) {
                ಆದರೆ (errno != 0)
                        ಹಿಂತಿರುಗಿ -errno;

                errno = -ret;
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ opinionated_personality(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ret) {
        ಪೂರ್ಣಾಂಕ current;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = ಸುರಕ್ಷಿತ_personality(PERSONALITY_INVALID);
        ಆದರೆ (current < 0)
                ಹಿಂತಿರುಗಿ current;

        ಆದರೆ (((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) current & OPINIONATED_PERSONALITY_ಮುಖವಾಡ) == PER_LINUX32)
                *ret = PER_LINUX32;
        ಇಲ್ಲದಿದ್ದರೆ
                *ret = PER_LINUX;

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ valgrind_summary_hack(ಶೂನ್ಯ) {
#ಆದರೆ HAVE_VALGRIND_VALGRIND_H
        ಆದರೆ (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                ಆದರೆ (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_ಸಂದೇಶ_ID(SD_ಸಂದೇಶ_VALGRIND_HELPER_FORK_STR),
                                LOG_ಸಂದೇಶ("Failed to fork off valgrind helper: %m"));
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid == 0)
                        ನಿರ್ಗಮಿಸು(EXIT_SUCCESS);
                ಇಲ್ಲದಿದ್ದರೆ {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_ಮುಗಿದ) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (ಶೂನ್ಯ) pidref_ಗಣ_pid(&pidref, pid);
                        (ಶೂನ್ಯ) pidref_wait_for_terminate(&pidref, ಖಾಲಿ);
                }
        }
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ pid_compare_func(ಸ್ಥಿರಾಂಕ pid_t *a, ಸ್ಥಿರಾಂಕ pid_t *b) {
        /* Suitable for usage in qsort() */
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ CMP(*a, *b);
}

bool nice_is_valid(ಪೂರ್ಣಾಂಕ n) {
        ಹಿಂತಿರುಗಿ n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(ಪೂರ್ಣಾಂಕ policy) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(ಪೂರ್ಣಾಂಕ policy) {
        ಹಿಂತಿರುಗಿ sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
ಪೂರ್ಣಾಂಕ sched_get_ಆದ್ಯತೆ_min_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ policy) {
        ಪೂರ್ಣಾಂಕ r;

        r = sched_get_priority_min(policy);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ r;

        /* Fallback priority */
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sched_get_ಆದ್ಯತೆ_max_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ policy) {
        ಪೂರ್ಣಾಂಕ r;

        r = sched_get_priority_max(policy);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#ವ್ಯಾಖ್ಯಾನಿಸು CACHED_PID_UNSET ((pid_t) 0)
#ವ್ಯಾಖ್ಯಾನಿಸು CACHED_PID_BUSY ((pid_t) -1)

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
        ಆದರೆ (current_ಮೌಲ್ಯ > 0)
                ಹಿಂತಿರುಗಿ current_ಮೌಲ್ಯ;

        current_ಮೌಲ್ಯ = CACHED_PID_UNSET;
        (ಶೂನ್ಯ) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_ಮೌಲ್ಯ,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        ಆಯ್ಕೆ (current_ಮೌಲ್ಯ) {

        ಸ್ಥಿತಿ CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                ಆದರೆ (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        ಆದರೆ (pthread_atfork(ಖಾಲಿ, ಖಾಲಿ, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                ಹಿಂತಿರುಗಿ new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                ಹಿಂತಿರುಗಿ new_pid;
        }

        ಸ್ಥಿತಿ CACHED_PID_BUSY: /* Somebody else is currently initializing */
                ಹಿಂತಿರುಗಿ getpid();

        ಪೂರ್ವನಿಯೋಜಿತ: /* Properly initialized */
                ಹಿಂತಿರುಗಿ current_ಮೌಲ್ಯ;
        }
}

ಪೂರ್ಣಾಂಕ must_be_ಬೇರು(ಶೂನ್ಯ) {

        ಆದರೆ (geteuid() == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ರಾಶಿ(ಪೂರ್ಣಾಂಕ (*fn)(ಶೂನ್ಯ *), ಪೂರ್ಣಾಂಕ flags, ಶೂನ್ಯ *userdata) {
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

        ಪ್ರತಿಜ್ಞಾ((flags & (CLONE_VM|CLONE_ಪೋಷಕ_SETTID|CLONE_ಮಗು_SETTID|
                         CLONE_ಮಗು_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = ಪುಟ_ಗಾತ್ರ();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (ಶೂನ್ಯ*) ALIGN_ಗಮ್ಯ((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#ಆದರೆ HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#ಇಲ್ಲದಿದ್ದರೆ
        pid = __clone2(fn, mystack, ps, flags, userdata);
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (pid < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ pid;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fork_flags_ಗಮ್ಯ_signal(ForkFlags flags) {
        ಹಿಂತಿರುಗಿ (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

ಪೂರ್ಣಾಂಕ pidref_ಸುರಕ್ಷಿತ_fork_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ stdio_fds[3],
                ಪೂರ್ಣಾಂಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        ಬಳಸದ _cleanup_(ಖಂಡ_ಸಂಕೇತಗಳು_reset) sigset_t *saved_ssp = ಖಾಲಿ;
        bool ಖಂಡ_ಸಂಕೇತಗಳು = false, ಖಂಡ_all = false, intermediary = false;
        _cleanup_close_pair_ ಪೂರ್ಣಾಂಕ pidref_transport_fds[2] = EBADF_PAIR;
        ಪೂರ್ಣಾಂಕ prio, r;

        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, FORK_WAIT|FORK_FREEZE));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        ಆದರೆ (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        ಆದರೆ (flags & (FORK_RESET_ಸಂಕೇತಗಳು|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                ಖಂಡ_ಸಂಕೇತಗಳು = ಖಂಡ_all = true;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                ಖಂಡ_ಸಂಕೇತಗಳು = true;
        }

        ಆದರೆ (ಖಂಡ_ಸಂಕೇತಗಳು) {
                ಆದರೆ (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                ಆದರೆ (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        ಆದರೆ (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        ಆದರೆ (pid < 0)
                                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        ಆದರೆ (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = ಸುರಕ್ಷಿತ_close(pidref_transport_fds[1]);

                                ಆದರೆ (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_ಗಣ(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        ಆದರೆ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                ಹಿಂತಿರುಗಿ -EPROTO;

                                        ಪೂರ್ಣಾಂಕ pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, ಗಾತ್ರ(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        ಆದರೆ (n < 0)
                                                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                ಹಿಂತಿರುಗಿ 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = ಸುರಕ್ಷಿತ_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        ಆದರೆ ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_ಗಣ(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_ಗಣ(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_ಗಣ(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_ಗಣ(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        ಇಲ್ಲದಿದ್ದರೆ
                pid = fork();
        ಆದರೆ (pid < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        ಆದರೆ (pid > 0) {

                /* If we are in the intermediary process, exit now */
                ಆದರೆ (intermediary) {
                        ಆದರೆ (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_ಮುಗಿದ) PidRef pidref = PIDREF_NULL;

                                r = pidref_ಗಣ_pid(&pidref, pid);
                                ಆದರೆ (r < 0) {
                                        log_ತುಂಬಿದ_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, ಗಾತ್ರ(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                ಆದರೆ (r < 0) {
                                        log_ತುಂಬಿದ_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                ಆದರೆ (flags & FORK_WAIT) {
                        ಆದರೆ (ಖಂಡ_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (ಶೂನ್ಯ) sigprocmask(SIG_SETMASK, &ss, ಖಾಲಿ);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_ಗಣ(flags, FORK_LOG) ? WAIT_LOG : 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                ಹಿಂತಿರುಗಿ -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        ಆದರೆ (ret)
                                *ret = PIDREF_NULL;

                        ಹಿಂತಿರುಗಿ 1;
                }

                ಆದರೆ (ret) {
                        r = pidref_ಗಣ_pid(ret, pid);
                        ಆದರೆ (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                ಹಿಂತಿರುಗಿ 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = ಸುರಕ್ಷಿತ_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ಖಾಲಿ;

        ಆದರೆ (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_ಗಣ_open_when_needed(true);
                log_settle_ಗುರಿ();
        }

        ಆದರೆ (name) {
                r = rename_process(name);
                ಆದರೆ (r < 0)
                        log_ತುಂಬಿದ_errno(flags & FORK_LOG ? LOG_WARNING : LOG_DEBUG,
                                       r, "Failed to rename process, ignoring: %m");
        }

        /* let's disable dlopen() in the child, as a paranoia safety precaution: children should not live for
         * long and only do minimal work before exiting or exec()ing. Doing dlopen() is not either. If people
         * want dlopen() they should do it before forking. This is a safety precaution in particular for
         * cases where the child does namespace shenanigans: we should never end up loading a module from a
         * foreign environment. Note that this has no effect on NSS! (i.e. it only has effect on uses of our
         * dlopen_safe(), which we use comprehensively in our codebase, but glibc NSS doesn't bother, of
         * course.) */
        ಆದರೆ (!FLAGS_ಗಣ(flags, FORK_ALLOW_DLOPEN))
                ಖಂಡ_dlopen();

        ಆದರೆ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_ಸುರಕ್ಷಿತ(PR_SET_PDEATHSIG, fork_flags_ಗಮ್ಯ_signal(flags), 0, 0, 0);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (flags & FORK_RESET_ಸಂಕೇತಗಳು) {
                r = reset_all_signal_handlers();
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_ಮುಖವಾಡ();
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಖಂಡ_ಸಂಕೇತಗಳು) { /* undo what we did above */
                ಆದರೆ (sigprocmask(SIG_SETMASK, &saved_ss, ಖಾಲಿ) < 0) {
                        log_ತುಂಬಿದ_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                ಆದರೆ (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ppid != original_pid) {
                        ಪೂರ್ಣಾಂಕ sig = fork_flags_ಗಮ್ಯ_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_ಗಮ್ಯ_string(sig));
                        (ಶೂನ್ಯ) ಎತ್ತು(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                ಆದರೆ (mount(ಖಾಲಿ, "/", ಖಾಲಿ, MS_SLAVE | MS_REC, ಖಾಲಿ) < 0) {
                        log_ತುಂಬಿದ_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, FORK_ಖಾಸಗಿ_TMP)) {
                ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_ಸತತ);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (flags & FORK_REARRANGE_STDIO) {
                ಆದರೆ (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        ಆದರೆ (r < 0) {
                                log_ತುಂಬಿದ_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } ಇಲ್ಲದಿದ್ದರೆ {
                        r = make_null_stdio();
                        ಆದರೆ (r < 0) {
                                log_ತುಂಬಿದ_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & FORK_STDOUT_ಗಮ್ಯ_STDERR) {
                ಆದರೆ (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_ತುಂಬಿದ_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        ಆದರೆ (flags & FORK_REOPEN_LOG) {
                log_open();
                log_ಗಣ_open_when_needed(false);
        }

        ಆದರೆ (flags & FORK_RLIMIT_NOFILE_ಸುರಕ್ಷಿತ) {
                r = rlimit_nofile_ಸುರಕ್ಷಿತ();
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (!FLAGS_ಗಣ(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, FORK_FREEZE))
                freeze();

        ಆದರೆ (ret) {
                r = pidref_ಗಣ_self(ret);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ namespace_fork_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *outer_name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *inner_name,
                ಪೂರ್ಣಾಂಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ಪೂರ್ಣಾಂಕ pidns_fd,
                ಪೂರ್ಣಾಂಕ mntns_fd,
                ಪೂರ್ಣಾಂಕ netns_fd,
                ಪೂರ್ಣಾಂಕ userns_fd,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ ಪೂರ್ಣಾಂಕ errno_pipe_fd[2] = EBADF_PAIR;
        ಪೂರ್ಣಾಂಕ r, prio = FLAGS_ಗಣ(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

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
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) <= PIPE_BUF);
        ಆದರೆ (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_ಸುರಕ್ಷಿತ_fork_ತುಂಬಿದ(
                        outer_name,
                        /* stdio_fds= */ ಖಾಲಿ, /* except_fds= */ ಖಾಲಿ, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_ಸುರಕ್ಷಿತ),
                        &pidref_outer);
        ಆದರೆ (r == -EPROTO && FLAGS_ಗಣ(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = ಸುರಕ್ಷಿತ_close(errno_pipe_fd[1]);

                ಪೂರ್ಣಾಂಕ k = read_errno(errno_pipe_fd[0]);
                ಆದರೆ (k < 0 && k != -EIO)
                        ಹಿಂತಿರುಗಿ k;
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0) {
                _cleanup_(pidref_ಮುಗಿದ) PidRef pidref_inner = PIDREF_NULL;

                /*ಮಗು*/

                errno_pipe_fd[0] = ಸುರಕ್ಷಿತ_close(errno_pipe_fd[0]);

                r = namespace_enter_ತುಂಬಿದ(pidns_fd, mntns_fd, netns_fd, userns_fd, ಬೇರು_fd, namespace_flags);
                ಆದರೆ (r < 0) {
                        log_ತುಂಬಿದ_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_ಸುರಕ್ಷಿತ_fork_ತುಂಬಿದ(
                                inner_name,
                                ಖಾಲಿ,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_ಸಂಕೇತಗಳು|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_ಗಮ್ಯ_STDERR),
                                &pidref_inner);
                ಆದರೆ (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                ಆದರೆ (r == 0) {
                        /*ಮಗು*/

                        ಆದರೆ (!FLAGS_ಗಣ(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = ಸುರಕ್ಷಿತ_close(errno_pipe_fd[1]);
                                pidref_ಮುಗಿದ(&pidref_outer);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        ಆದರೆ (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        ಹಿಂತಿರುಗಿ 0;
                }

                log_forget_fds();
                log_ಗಣ_open_when_needed(true);

                (ಶೂನ್ಯ) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_ಗಣ(flags, FORK_LOG) ? WAIT_LOG : 0);
                ಆದರೆ (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = ಸುರಕ್ಷಿತ_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r; /* the child logs about failures on its own, no need to duplicate here */

        ಆದರೆ (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        ಇಲ್ಲದಿದ್ದರೆ
                pidref_ಮುಗಿದ(&pidref_outer); /* disarm sigkill_wait */

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ namespace_fork(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *outer_name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *inner_name,
                ForkFlags flags,
                ಪೂರ್ಣಾಂಕ pidns_fd,
                ಪೂರ್ಣಾಂಕ mntns_fd,
                ಪೂರ್ಣಾಂಕ netns_fd,
                ಪೂರ್ಣಾಂಕ userns_fd,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                PidRef *ret) {

        ಹಿಂತಿರುಗಿ namespace_fork_ತುಂಬಿದ(outer_name, inner_name, /* except_fds= */ ಖಾಲಿ, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, ಬೇರು_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(ಪೂರ್ಣಾಂಕ oa) {
        ಹಿಂತಿರುಗಿ oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

ಪೂರ್ಣಾಂಕ ಗಣ_oom_score_adjust(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ) {
        ಅಕ್ಷರ t[DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ)];

        ಆದರೆ (!oom_score_adjust_is_valid(ಮೌಲ್ಯ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        xsprintf(t, "%i", ಮೌಲ್ಯ);

        ಹಿಂತಿರುಗಿ write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_ಬಫರ್);
}

ಪೂರ್ಣಾಂಕ get_oom_score_adjust(ಪೂರ್ಣಾಂಕ *ret) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        delete_trailing_chars(t, WHITESPACE);

        r = ಸುರಕ್ಷಿತ_atoi(t, &a);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!oom_score_adjust_is_valid(a))
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಆದರೆ (ret)
                *ret = a;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rlimit_ಗಮ್ಯ_nice(rlim_t limit) {
        ಆದರೆ (limit <= 1)
                ಹಿಂತಿರುಗಿ PRIO_MAX-1; /* i.e. 19 */

        ಆದರೆ (limit >= -PRIO_MIN + PRIO_MAX)
                ಹಿಂತಿರುಗಿ PRIO_MIN; /* i.e. -20 */

        ಹಿಂತಿರುಗಿ PRIO_MAX - (ಪೂರ್ಣಾಂಕ) limit;
}

ಪೂರ್ಣಾಂಕ setpriority_closest(ಪೂರ್ಣಾಂಕ priority) {
        ರಚನೆ rlimit highest;
        ಪೂರ್ಣಾಂಕ r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ 1;
        ಆದರೆ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ಹಿಂತಿರುಗಿ r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        ಆದರೆ (errno != 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (priority == current)
                ಹಿಂತಿರುಗಿ 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        ಆದರೆ (priority > current)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (getrlimit(RLIMIT_NICE, &highest) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        limit = rlimit_ಗಮ್ಯ_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        ಆದರೆ (current > limit)
                ಆದರೆ (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        ಹಿಂತಿರುಗಿ 0;
}

_noreturn_ ಶೂನ್ಯ freeze(ಶೂನ್ಯ) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (ಶೂನ್ಯ) close_all_fds_without_malloc(ಖಾಲಿ, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        ಗಾಗಿ (;;) {
                siginfo_t si = {};

                ಆದರೆ (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        ನಿಲ್ಲಿಸು;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        ಗಾಗಿ (;;)
                pause();
}

ಪೂರ್ಣಾಂಕ get_process_ಎಳೆಗಳು(pid_t pid) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ n, r;

        ಆದರೆ (pid < 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = procfs_file_get_ಕ್ಷೇತ್ರ(pid, "status", "Threads", &t);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಸುರಕ್ಷಿತ_atoi(t, &n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ is_reaper_process(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        ಆದರೆ (getpid_cached() == 1)
                ಹಿಂತಿರುಗಿ true;

        r = prctl_ಸುರಕ್ಷಿತ(PR_GET_CHILD_SUBREAPER, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) &b, 0, 0, 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ b != 0;
}

ಪೂರ್ಣಾಂಕ make_reaper_process(bool b) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (getpid_cached() == 1) {

                ಆದರೆ (!b)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ 0;
        }

        r = prctl_ಸುರಕ್ಷಿತ(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_ತುಂಬಿದ(posix_spawnattr_t*, posix_spawnattr_destroy, ಖಾಲಿ);

ಪೂರ್ಣಾಂಕ posix_spawn_wrapper(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *argv,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *envp,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cgroup,
                PidRef *ret_pidref) {

        ಗಿಡ್ಡ flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t ಮುಖವಾಡ_3;
        ಪೂರ್ಣಾಂಕ r;

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

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(argv);
        ಪ್ರತಿಜ್ಞಾ(ret_pidref);

        assert_se(sigfillset(&ಮುಖವಾಡ_3) >= 0);

        r = posix_spawnattr_init(&attr);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        ಬಳಸದ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        ಸ್ಥಿರ bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ ಪೂರ್ಣಾಂಕ cgroup_fd = -EBADF;

        ಆದರೆ (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ ಅಕ್ಷರ *resolved_cgroup = ಖಾಲಿ;

                r = cg_get_ಮಾರ್ಗ(cgroup, /* suffix= */ ಖಾಲಿ, &resolved_cgroup);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                cgroup_fd = open(resolved_cgroup, O_ಮಾರ್ಗ|O_DIRECTORY|O_CLOEXEC);
                ಆದರೆ (cgroup_fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                ಆದರೆ (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r != ENOSYS)
                        ಹಿಂತಿರುಗಿ -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ -r;
        r = posix_spawnattr_setsigmask(&attr, &ಮುಖವಾಡ_3);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ -r;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, ಮಾರ್ಗ, ಖಾಲಿ, &attr, argv, envp);
        ಆದರೆ (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_ಗಣ(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                ಹಿಂತಿರುಗಿ -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        ಆದರೆ ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
            FLAGS_ಗಣ(flags, POSIX_SPAWN_SETCGROUP)) {
                /* Compiled on a newer host, or seccomp&friends blocking clone3()? Fallback, but
                 * need to disable POSIX_SPAWN_SETCGROUP, which is what redirects to clone3().
                 * CLONE_INTO_CGROUP definitely won't work, hence remember the fact so that we don't
                 * retry every time.
                 * Note, CLONE_INTO_CGROUP is supported since kernel v5.7, but some architectures still
                 * do not support clone3(). Hence, we need to keep the fallback logic for a while. */
                have_clone_into_cgroup = false;

                flags &= ~POSIX_SPAWN_SETCGROUP;
                r = posix_spawnattr_setflags(&attr, flags);
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ -r;

                r = pidfd_spawn(&pidfd, ಮಾರ್ಗ, ಖಾಲಿ, &attr, argv, envp);
        }
        ಆದರೆ (r == 0) {
                r = pidref_ಗಣ_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(flags, POSIX_SPAWN_SETCGROUP);
        }
        ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(r))
                ಹಿಂತಿರುಗಿ -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, ಮಾರ್ಗ, ಖಾಲಿ, &attr, argv, envp);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ -r;

        r = pidref_ಗಣ_pid(ret_pidref, pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

ಪೂರ್ಣಾಂಕ proc_dir_open(DIR **ret) {
        DIR *d;

        ಪ್ರತಿಜ್ಞಾ(ret);

        d = opendir("/proc");
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = d;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ proc_dir_read(DIR *d, pid_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(d);

        ಗಾಗಿ (;;) {
                ರಚನೆ dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                ಆದರೆ (!de) {
                        ಆದರೆ (errno != 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (!ಒಳ_ಗಣ(de->d_type, DT_DIR, DT_UNKNOWN))
                        ಮುಂದುವರಿ;

                ಆದರೆ (parse_pid(de->d_name, ret) >= 0)
                        ಹಿಂತಿರುಗಿ 1;
        }

        ಆದರೆ (ret)
                *ret = 0;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ proc_dir_read_pidref(DIR *d, PidRef *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(d);

        ಗಾಗಿ (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                r = pidref_ಗಣ_pid(ret, pid);
                ಆದರೆ (r == -ESRCH) /* gone by now? skip it */
                        ಮುಂದುವರಿ;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ 1;
        }

        ಆದರೆ (ret)
                *ret = PIDREF_NULL;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_mlockall(ಪೂರ್ಣಾಂಕ flags) {
        ಪೂರ್ಣಾಂಕ r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        ಆದರೆ (mlockall(flags) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigchld_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(sigchld_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ sched_policy_ಕೋಷ್ಟಕ[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(sched_policy, ಪೂರ್ಣಾಂಕ, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);

_noreturn_ ಶೂನ್ಯ report_errno_and_exit(ಪೂರ್ಣಾಂಕ errno_fd, ಪೂರ್ಣಾಂಕ error) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (error >= 0)
                _exit(EXIT_SUCCESS);

        ಪ್ರತಿಜ್ಞಾ(errno_fd >= 0);

        r = loop_write(errno_fd, &error, ಗಾತ್ರ(error));
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

ಪೂರ್ಣಾಂಕ read_errno(ಪೂರ್ಣಾಂಕ errno_fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, ಗಾತ್ರ(r), /* do_poll= */ false);
        ಆದರೆ (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                ಹಿಂತಿರುಗಿ -EIO;
        }
        ಆದರೆ (n == 0) /* the process exited without reporting an error, assuming success */
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (n != ಗಾತ್ರ(r))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (r < 0) /* child process reported an error, return it */
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Child process failed with errno: %m");

        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

ಪೂರ್ಣಾಂಕ prctl_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ op, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg2, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg3, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg4, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        ಹಿಂತಿರುಗಿ RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

ಪೂರ್ಣಾಂಕ proc_ಗಣ_comm(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *comm) {
        ಹಿಂತಿರುಗಿ prctl_ಸುರಕ್ಷಿತ(PR_SET_NAME, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) comm, 0, 0, 0);
}

ಪೂರ್ಣಾಂಕ proc_ಗಣ_nnp(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ prctl_ಸುರಕ್ಷಿತ(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        r = prctl_ಸುರಕ್ಷಿತ(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ NSEC_INFINITY;

        ಹಿಂತಿರುಗಿ r;
}
