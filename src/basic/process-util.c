/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/oom.h>
#അന്തര്ഭാവ <pthread.h>
#അന്തര്ഭാവ <spawn.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <sys/mman.h>
#അന്തര്ഭാവ <sys/mount.h>
#അന്തര്ഭാവ <sys/personality.h>
#അന്തര്ഭാവ <sys/prctl.h>
#അന്തര്ഭാവ <sys/wait.h>
#അന്തര്ഭാവ <syslog.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>
#എങ്കിൽ HAVE_VALGRIND_VALGRIND_H
#അന്തര്ഭാവ <valgrind/valgrind.h>
#endif

#അന്തര്ഭാവ "sd-messages.h"

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "architecture.h"
#അന്തര്ഭാവ "argv-util.h"
#അന്തര്ഭാവ "bitfield.h"
#അന്തര്ഭാവ "capability-util.h"
#അന്തര്ഭാവ "cgroup-util.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "dlfcn-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "locale-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "mountpoint-util.h"
#അന്തര്ഭാവ "namespace-util.h"
#അന്തര്ഭാവ "nulstr-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidfd-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "raw-clone.h"
#അന്തര്ഭാവ "rlimit-util.h"
#അന്തര്ഭാവ "signal-util.h"
#അന്തര്ഭാവ "socket-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

സ്ഥിതം പൂർണ്ണസംഖ്യ get_process_state(pid_t pid) {
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *p;
        അക്ഷരം state;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        എങ്കിൽ (pid == 0 || pid == getpid_cached())
                തിരികെ (unsigned അക്ഷരം) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_വരി_file(p, &വരി);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        p = strrchr(വരി, ')');
        എങ്കിൽ (!p)
                തിരികെ -EIO;

        p++;

        എങ്കിൽ (sscanf(p, " %c", &state) != 1)
                തിരികെ -EIO;

        തിരികെ (unsigned അക്ഷരം) state;
}

പൂർണ്ണസംഖ്യ pid_get_comm(pid_t pid, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *escaped = ശൂന്യ, *comm = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pid == 0 || pid == getpid_cached()) {
                comm = new0(അക്ഷരം, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                എങ്കിൽ (!comm)
                        തിരികെ -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        } അല്ലെങ്കിൽ {
                സ്ഥിരം അക്ഷരം *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_വരി_file(p, &comm);
                എങ്കിൽ (r == -ENOENT)
                        തിരികെ -ESRCH;
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        escaped = new(അക്ഷരം, COMM_MAX_LEN);
        എങ്കിൽ (!escaped)
                തിരികെ -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_get_comm(സ്ഥിരം PidRef *pid, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *comm = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(comm);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                അക്ഷരം **ret,
                size_t *ret_size) {

        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *p;
        size_t k;
        പൂർണ്ണസംഖ്യ r;

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
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (k == 0) {
                എങ്കിൽ (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        തിരികെ -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ അക്ഷരം *comm = ശൂന്യ;

                r = pid_get_comm(pid, &comm);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                സ്വതന്ത്രം(t);
                t = strjoin("[", comm, "]");
                എങ്കിൽ (!t)
                        തിരികെ -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                എങ്കിൽ (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(t);
        എങ്കിൽ (ret_size)
                *ret_size = k;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        size_t k;
        അക്ഷരം *ans;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

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

        പൂർണ്ണസംഖ്യ full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        എങ്കിൽ (full < 0)
                തിരികെ full;

        എങ്കിൽ (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                പ്രതിജ്ഞാ(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ അക്ഷരം **args = ശൂന്യ;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                എങ്കിൽ (!args)
                        തിരികെ -ENOMEM;

                ans = quote_command_വരി(args, shflags);
                എങ്കിൽ (!ans)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                വേണ്ടി (size_t i = 0; i < k - 1; i++)
                        എങ്കിൽ (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                എങ്കിൽ (!ans)
                        തിരികെ -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_get_cmdline(സ്ഥിരം PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, അക്ഷരം ***ret) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        അക്ഷരം **args;
        size_t k;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        പ്രതിജ്ഞാ(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        എങ്കിൽ (!args)
                തിരികെ -ENOMEM;

        *ret = args;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_get_cmdline_strv(സ്ഥിരം PidRef *pid, ProcessCmdlineFlags flags, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **args = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(args);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ procfs_get_stat_flags(സ്ഥിരം അക്ഷരം *path, unsigned long long *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        r = read_one_വരി_file(path, &വരി);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Skip past the comm field */
        അക്ഷരം *q = strrchr(വരി, ')');
        എങ്കിൽ (!q)
                തിരികെ -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        വേണ്ടി (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                എങ്കിൽ (l < 1)
                        തിരികെ -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                എങ്കിൽ (l < 1)
                        തിരികെ -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        എങ്കിൽ (l < 1)
                തിരികെ -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        എങ്കിൽ (l < 1)
                തിരികെ -EINVAL;
        q[l] = 0;

        തിരികെ safe_atollu(q, ret);
}

പൂർണ്ണസംഖ്യ pid_is_kernel_thread(pid_t pid) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                തിരികെ 0;
        എങ്കിൽ (!pid_is_valid(pid))
                തിരികെ -EINVAL;

        സ്ഥിരം അക്ഷരം *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ !!(flags & PF_KTHREAD);
}

പൂർണ്ണസംഖ്യ pidref_is_kernel_thread(സ്ഥിരം PidRef *pid) {
        പൂർണ്ണസംഖ്യ result, r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        എങ്കിൽ (result < 0)
                തിരികെ result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_process_link_contents(pid_t pid, സ്ഥിരം അക്ഷരം *proc_file, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        തിരികെ (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

പൂർണ്ണസംഖ്യ get_process_exe(pid_t pid, അക്ഷരം **ret) {
        അക്ഷരം *d;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret) {
                d = endswith(*ret, " (deleted)");
                എങ്കിൽ (d)
                        *d = '\0';
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pid_get_uid(pid_t pid, uid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                തിരികെ 0;
        }

        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_uid(v, ret);
}

പൂർണ്ണസംഖ്യ pidref_get_uid(സ്ഥിരം PidRef *pid, uid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                എങ്കിൽ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        തിരികെ r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = uid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_process_gid(pid_t pid, gid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                തിരികെ 0;
        }

        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_gid(v, ret);
}

പൂർണ്ണസംഖ്യ get_process_cwd(pid_t pid, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(pid >= 0);

        എങ്കിൽ (pid == 0 || pid == getpid_cached())
                തിരികെ safe_getcwd(ret);

        തിരികെ get_process_link_contents(pid, "cwd", ret);
}

പൂർണ്ണസംഖ്യ get_process_വേര്(pid_t pid, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(pid >= 0);
        തിരികെ get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

പൂർണ്ണസംഖ്യ get_process_environ(pid_t pid, അക്ഷരം **ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *outcome = ശൂന്യ;
        size_t sz = 0;
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                അക്ഷരം c;

                എങ്കിൽ (sz >= ENVIRONMENT_BLOCK_MAX)
                        തിരികെ -ENOBUFS;

                എങ്കിൽ (!GREEDY_REALLOC(outcome, sz + 5))
                        തിരികെ -ENOMEM;

                r = safe_fgetc(f, &c);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ (c == '\0')
                        outcome[sz++] = '\n';
                അല്ലെങ്കിൽ
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        unsigned long ppid;
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);

        എങ്കിൽ (pid == 0)
                pid = getpid_cached();
        എങ്കിൽ (pid == 1) /* PID 1 has no parent, shortcut this case */
                തിരികെ -EADDRNOTAVAIL;

        എങ്കിൽ (pid == getpid_cached()) {
                എങ്കിൽ (ret)
                        *ret = getppid();
                തിരികെ 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_വരി_file(p, &വരി);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(വരി, ')');
        എങ്കിൽ (!p)
                തിരികെ -EIO;
        p++;

        എങ്കിൽ (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                തിരികെ -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        എങ്കിൽ (ppid == 0)
                തിരികെ -EADDRNOTAVAIL;

        എങ്കിൽ ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                തിരികെ -ERANGE;

        എങ്കിൽ (ret)
                *ret = (pid_t) ppid;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_get_ppid(സ്ഥിരം PidRef *pidref, pid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                എങ്കിൽ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        തിരികെ r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = ppid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_get_ppid_as_pidref(സ്ഥിരം PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        പൂർണ്ണസംഖ്യ r;

        POINTER_MAY_BE_NULL(pidref);
        പ്രതിജ്ഞാ(ret);

        r = pidref_get_ppid(pidref, &ppid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                എങ്കിൽ (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        എങ്കിൽ (parent.pid != ppid)
                                തുടരുക;
                }

                *ret = TAKE_PIDREF(parent);
                തിരികെ 0;
        }

        /* Give up after 16 tries */
        തിരികെ -ENOTRECOVERABLE;
}

പൂർണ്ണസംഖ്യ pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_വരി_file(p, &വരി);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(വരി, ')');
        എങ്കിൽ (!p)
                തിരികെ -EIO;
        p++;

        unsigned long llu;

        എങ്കിൽ (sscanf(p, " "
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
                തിരികെ -EIO;

        എങ്കിൽ (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_get_start_time(സ്ഥിരം PidRef *pid, usec_t *ret) {
        usec_t t;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = t;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ അക്ഷരം *m = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_mode(m, ret);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pid_get_sigmask(pid_t pid, സ്ഥിരം അക്ഷരം *field_name, uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *field = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(field_name);
        പ്രതിജ്ഞാ(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ safe_atou64_full(field, 16, ret);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pidref_has_sigmask(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig, സ്ഥിരം അക്ഷരം *field_name) {
        uint64_t mask;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;
        എങ്കിൽ (!SIGNAL_VALID(sig))
                തിരികെ -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ BIT_SET(mask, sig - 1);
}

പൂർണ്ണസംഖ്യ pidref_has_sigcgt(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig) {
        തിരികെ pidref_has_sigmask(pidref, sig, "SigCgt");
}

പൂർണ്ണസംഖ്യ pidref_has_sigblk(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig) {
        തിരികെ pidref_has_sigmask(pidref, sig, "SigBlk");
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
പൂർണ്ണസംഖ്യ pidref_wait_for_terminate_and_check(സ്ഥിരം അക്ഷരം *name, PidRef *pidref, WaitFlags flags) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;
        എങ്കിൽ (pidref->pid == 1 || pidref_is_self(pidref))
                തിരികെ -ECHILD;

        _cleanup_free_ അക്ഷരം *buffer = ശൂന്യ;
        എങ്കിൽ (!name) {
                r = pidref_get_comm(pidref, &buffer);
                എങ്കിൽ (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                അല്ലെങ്കിൽ
                        name = buffer;
        }

        പൂർണ്ണസംഖ്യ prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        എങ്കിൽ (status.si_code == CLD_EXITED) {
                എങ്കിൽ (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                അല്ലെങ്കിൽ
                        log_debug("'%s' succeeded.", name);

                തിരികെ status.si_status;

        } അല്ലെങ്കിൽ എങ്കിൽ (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                തിരികെ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        തിരികെ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

പൂർണ്ണസംഖ്യ kill_and_sigcont(pid_t pid, പൂർണ്ണസംഖ്യ sig) {
        പൂർണ്ണസംഖ്യ r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        എങ്കിൽ (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (ശൂന്യം) kill(pid, SIGCONT);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ getenv_for_pid(pid_t pid, സ്ഥിരം അക്ഷരം *field, അക്ഷരം **ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *path;
        size_t sum = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(field);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pid == 0 || pid == getpid_cached())
                തിരികെ strdup_to_full(ret, getenv(field));

        എങ്കിൽ (!pid_is_valid(pid))
                തിരികെ -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
                സ്ഥിരം അക്ഷരം *match;

                എങ്കിൽ (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        തിരികെ -ENOBUFS;

                r = read_nul_string(f, LONG_വരി_MAX, &വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)  /* EOF */
                        നിർത്തുക;

                sum += r;

                match = startswith(വരി, field);
                എങ്കിൽ (match && *match == '=')
                        തിരികെ strdup_to_full(ret, match + 1);
        }

        *ret = ശൂന്യ;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidref_is_my_child(PidRef *pid) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pid->pid == 1 || pidref_is_self(pid))
                തിരികെ false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        എങ്കിൽ (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ ppid == getpid_cached();
}

പൂർണ്ണസംഖ്യ pid_is_my_child(pid_t pid) {

        എങ്കിൽ (pid == 0)
                തിരികെ false;

        തിരികെ pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

പൂർണ്ണസംഖ്യ pidref_is_unwaited(PidRef *pid) {
        പൂർണ്ണസംഖ്യ r;

        /* Checks whether a PID is still valid at all, including a zombie */

        എങ്കിൽ (!pidref_is_set(pid))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pid))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pid->pid == 1 || pidref_is_self(pid))
                തിരികെ true;

        r = pidref_kill(pid, 0);
        എങ്കിൽ (r == -ESRCH)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ pid_is_unwaited(pid_t pid) {

        എങ്കിൽ (pid == 0)
                തിരികെ true;

        തിരികെ pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

പൂർണ്ണസംഖ്യ pid_is_alive(pid_t pid) {
        പൂർണ്ണസംഖ്യ r;

        /* Checks whether a PID is still valid and not a zombie */

        എങ്കിൽ (pid < 0)
                തിരികെ -ESRCH;

        എങ്കിൽ (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                തിരികെ true;

        എങ്കിൽ (pid == getpid_cached())
                തിരികെ true;

        r = get_process_state(pid);
        എങ്കിൽ (r == -ESRCH)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ r != 'Z';
}

പൂർണ്ണസംഖ്യ pidref_is_alive(സ്ഥിരം PidRef *pidref) {
        പൂർണ്ണസംഖ്യ r, result;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        result = pid_is_alive(pidref->pid);
        എങ്കിൽ (result < 0) {
                പ്രതിജ്ഞാ(result != -ESRCH);
                തിരികെ result;
        }

        r = pidref_verify(pidref);
        എങ്കിൽ (r == -ESRCH)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ result;
}

പൂർണ്ണസംഖ്യ pidref_from_same_വേര്_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        എങ്കിൽ (!a || !b) {
                r = pidref_set_self(&self);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (!a)
                        a = &self;
                എങ്കിൽ (!b)
                        b = &self;
        }

        എങ്കിൽ (!pidref_is_set(a) || !pidref_is_set(b))
                തിരികെ -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        എങ്കിൽ (pidref_is_remote(a) && pidref_is_remote(b))
                തിരികെ -EREMOTE;
        എങ്കിൽ (pidref_is_remote(a) || pidref_is_remote(b))
                തിരികെ false;

        എങ്കിൽ (pidref_equal(a, b))
                തിരികെ true;

        സ്ഥിരം അക്ഷരം *roota = procfs_file_alloca(a->pid, "root");
        സ്ഥിരം അക്ഷരം *rootb = procfs_file_alloca(b->pid, "root");

        പൂർണ്ണസംഖ്യ result = inode_same(roota, rootb, 0);
        എങ്കിൽ (result == -ENOENT)
                തിരികെ proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        എങ്കിൽ (result < 0)
                തിരികെ result;

        r = pidref_verify(a);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        r = pidref_verify(b);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ result;
}

bool is_main_thread(ശൂന്യം) {
        സ്ഥിതം thread_local പൂർണ്ണസംഖ്യ cached = -1;

        എങ്കിൽ (cached < 0)
                cached = getpid_cached() == gettid();

        തിരികെ cached;
}

unsigned long personality_from_string(സ്ഥിരം അക്ഷരം *s) {
        Architecture architecture;

        എങ്കിൽ (!s)
                തിരികെ PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        എങ്കിൽ (architecture < 0)
                തിരികെ PERSONALITY_INVALID;

        എങ്കിൽ (architecture == native_architecture())
                തിരികെ PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        എങ്കിൽ (architecture == ARCHITECTURE_SECONDARY)
                തിരികെ PER_LINUX32;
#endif

        തിരികെ PERSONALITY_INVALID;
}

സ്ഥിരം അക്ഷരം* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        എങ്കിൽ (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        അല്ലെങ്കിൽ എങ്കിൽ (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        എങ്കിൽ (architecture < 0)
                തിരികെ ശൂന്യ;

        തിരികെ architecture_to_string(architecture);
}

പൂർണ്ണസംഖ്യ safe_personality(unsigned long p) {
        പൂർണ്ണസംഖ്യ ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        എങ്കിൽ (ret < 0) {
                എങ്കിൽ (errno != 0)
                        തിരികെ -errno;

                errno = -ret;
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ opinionated_personality(unsigned long *ret) {
        പൂർണ്ണസംഖ്യ current;

        പ്രതിജ്ഞാ(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        എങ്കിൽ (current < 0)
                തിരികെ current;

        എങ്കിൽ (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        അല്ലെങ്കിൽ
                *ret = PER_LINUX;

        തിരികെ 0;
}

ശൂന്യം valgrind_summary_hack(ശൂന്യം) {
#എങ്കിൽ HAVE_VALGRIND_VALGRIND_H
        എങ്കിൽ (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                എങ്കിൽ (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                അല്ലെങ്കിൽ എങ്കിൽ (pid == 0)
                        exit(EXIT_SUCCESS);
                അല്ലെങ്കിൽ {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (ശൂന്യം) pidref_set_pid(&pidref, pid);
                        (ശൂന്യം) pidref_wait_for_terminate(&pidref, ശൂന്യ);
                }
        }
#endif
}

പൂർണ്ണസംഖ്യ pid_compare_func(സ്ഥിരം pid_t *a, സ്ഥിരം pid_t *b) {
        /* Suitable for usage in qsort() */
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ CMP(*a, *b);
}

bool nice_is_valid(പൂർണ്ണസംഖ്യ n) {
        തിരികെ n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(പൂർണ്ണസംഖ്യ policy) {
        തിരികെ IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(പൂർണ്ണസംഖ്യ policy) {
        തിരികെ sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
പൂർണ്ണസംഖ്യ sched_get_priority_min_safe(പൂർണ്ണസംഖ്യ policy) {
        പൂർണ്ണസംഖ്യ r;

        r = sched_get_priority_min(policy);
        എങ്കിൽ (r >= 0)
                തിരികെ r;

        /* Fallback priority */
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sched_get_priority_max_safe(പൂർണ്ണസംഖ്യ policy) {
        പൂർണ്ണസംഖ്യ r;

        r = sched_get_priority_max(policy);
        എങ്കിൽ (r >= 0)
                തിരികെ r;

        തിരികെ 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

സ്ഥിതം pid_t cached_pid = CACHED_PID_UNSET;

ശൂന്യം reset_cached_pid(ശൂന്യം) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(ശൂന്യം) {
        സ്ഥിതം bool installed = false;
        pid_t current_മൂല്യം;

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
        current_മൂല്യം = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        എങ്കിൽ (current_മൂല്യം > 0)
                തിരികെ current_മൂല്യം;

        current_മൂല്യം = CACHED_PID_UNSET;
        (ശൂന്യം) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_മൂല്യം,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        തിരഞ്ഞെടുപ്പ് (current_മൂല്യം) {

        അവസ്ഥ CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                എങ്കിൽ (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        എങ്കിൽ (pthread_atfork(ശൂന്യ, ശൂന്യ, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                തിരികെ new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                തിരികെ new_pid;
        }

        അവസ്ഥ CACHED_PID_BUSY: /* Somebody else is currently initializing */
                തിരികെ getpid();

        സ്ഥിരസ്ഥിതി: /* Properly initialized */
                തിരികെ current_മൂല്യം;
        }
}

പൂർണ്ണസംഖ്യ must_be_വേര്(ശൂന്യം) {

        എങ്കിൽ (geteuid() == 0)
                തിരികെ 0;

        തിരികെ log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_അട്ടി(പൂർണ്ണസംഖ്യ (*fn)(ശൂന്യം *), പൂർണ്ണസംഖ്യ flags, ശൂന്യം *userdata) {
        size_t ps;
        pid_t pid;
        ശൂന്യം *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        പ്രതിജ്ഞാ((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (ശൂന്യം*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#എങ്കിൽ HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#അല്ലെങ്കിൽ
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        എങ്കിൽ (pid < 0)
                തിരികെ -errno;

        തിരികെ pid;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fork_flags_to_signal(ForkFlags flags) {
        തിരികെ (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

പൂർണ്ണസംഖ്യ pidref_safe_fork_full(
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം പൂർണ്ണസംഖ്യ stdio_fds[3],
                പൂർണ്ണസംഖ്യ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = ശൂന്യ;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ പൂർണ്ണസംഖ്യ pidref_transport_fds[2] = EBADF_PAIR;
        പൂർണ്ണസംഖ്യ prio, r;

        പ്രതിജ്ഞാ(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        എങ്കിൽ (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        എങ്കിൽ (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } അല്ലെങ്കിൽ എങ്കിൽ (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        എങ്കിൽ (block_signals) {
                എങ്കിൽ (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        തിരികെ log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        എങ്കിൽ (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                എങ്കിൽ (r < 0)
                        തിരികെ log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                എങ്കിൽ (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        എങ്കിൽ (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                തിരികെ log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        എങ്കിൽ (pid < 0)
                                തിരികെ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        എങ്കിൽ (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                എങ്കിൽ (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        എങ്കിൽ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                തിരികെ -EPROTO;

                                        പൂർണ്ണസംഖ്യ pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, വലുപ്പം(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        എങ്കിൽ (n < 0)
                                                തിരികെ log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                തിരികെ 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        എങ്കിൽ ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        അല്ലെങ്കിൽ
                pid = fork();
        എങ്കിൽ (pid < 0)
                തിരികെ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        എങ്കിൽ (pid > 0) {

                /* If we are in the intermediary process, exit now */
                എങ്കിൽ (intermediary) {
                        എങ്കിൽ (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                എങ്കിൽ (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, വലുപ്പം(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                എങ്കിൽ (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                എങ്കിൽ (flags & FORK_WAIT) {
                        എങ്കിൽ (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (ശൂന്യം) sigprocmask(SIG_SETMASK, &ss, ശൂന്യ);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                തിരികെ -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        എങ്കിൽ (ret)
                                *ret = PIDREF_NULL;

                        തിരികെ 1;
                }

                എങ്കിൽ (ret) {
                        r = pidref_set_pid(ret, pid);
                        എങ്കിൽ (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                തിരികെ 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ശൂന്യ;

        എങ്കിൽ (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        എങ്കിൽ (name) {
                r = rename_process(name);
                എങ്കിൽ (r < 0)
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
        എങ്കിൽ (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        എങ്കിൽ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } അല്ലെങ്കിൽ എങ്കിൽ (block_signals) { /* undo what we did above */
                എങ്കിൽ (sigprocmask(SIG_SETMASK, &saved_ss, ശൂന്യ) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                എങ്കിൽ (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                അല്ലെങ്കിൽ എങ്കിൽ (ppid != original_pid) {
                        പൂർണ്ണസംഖ്യ sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (ശൂന്യം) ഉയർത്തുക(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                എങ്കിൽ (mount(ശൂന്യ, "/", ശൂന്യ, MS_SLAVE | MS_REC, ശൂന്യ) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                പ്രതിജ്ഞാ(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (flags & FORK_REARRANGE_STDIO) {
                എങ്കിൽ (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        എങ്കിൽ (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } അല്ലെങ്കിൽ {
                        r = make_null_stdio();
                        എങ്കിൽ (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } അല്ലെങ്കിൽ എങ്കിൽ (flags & FORK_STDOUT_TO_STDERR) {
                എങ്കിൽ (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                പ്രതിജ്ഞാ(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        എങ്കിൽ (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        എങ്കിൽ (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        എങ്കിൽ (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        എങ്കിൽ (ret) {
                r = pidref_set_self(ret);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ namespace_fork_full(
                സ്ഥിരം അക്ഷരം *outer_name,
                സ്ഥിരം അക്ഷരം *inner_name,
                പൂർണ്ണസംഖ്യ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                പൂർണ്ണസംഖ്യ pidns_fd,
                പൂർണ്ണസംഖ്യ mntns_fd,
                പൂർണ്ണസംഖ്യ netns_fd,
                പൂർണ്ണസംഖ്യ userns_fd,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ പൂർണ്ണസംഖ്യ errno_pipe_fd[2] = EBADF_PAIR;
        പൂർണ്ണസംഖ്യ r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        പ്രതിജ്ഞാ((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        പ്രതിജ്ഞാ((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(വലുപ്പം(പൂർണ്ണസംഖ്യ) <= PIPE_BUF);
        എങ്കിൽ (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                തിരികെ log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ ശൂന്യ, /* except_fds= */ ശൂന്യ, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        എങ്കിൽ (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                പൂർണ്ണസംഖ്യ k = read_errno(errno_pipe_fd[0]);
                എങ്കിൽ (k < 0 && k != -EIO)
                        തിരികെ k;
        }
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, വേര്_fd, namespace_flags);
                എങ്കിൽ (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                ശൂന്യ,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                എങ്കിൽ (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                എങ്കിൽ (r == 0) {
                        /* Child */

                        എങ്കിൽ (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } അല്ലെങ്കിൽ {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        എങ്കിൽ (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        തിരികെ 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (ശൂന്യം) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                എങ്കിൽ (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        എങ്കിൽ (r < 0)
                തിരികെ r; /* the child logs about failures on its own, no need to duplicate here */

        എങ്കിൽ (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        അല്ലെങ്കിൽ
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ namespace_fork(
                സ്ഥിരം അക്ഷരം *outer_name,
                സ്ഥിരം അക്ഷരം *inner_name,
                ForkFlags flags,
                പൂർണ്ണസംഖ്യ pidns_fd,
                പൂർണ്ണസംഖ്യ mntns_fd,
                പൂർണ്ണസംഖ്യ netns_fd,
                പൂർണ്ണസംഖ്യ userns_fd,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                PidRef *ret) {

        തിരികെ namespace_fork_full(outer_name, inner_name, /* except_fds= */ ശൂന്യ, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, വേര്_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(പൂർണ്ണസംഖ്യ oa) {
        തിരികെ oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

പൂർണ്ണസംഖ്യ set_oom_score_adjust(പൂർണ്ണസംഖ്യ മൂല്യം) {
        അക്ഷരം t[DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ)];

        എങ്കിൽ (!oom_score_adjust_is_valid(മൂല്യം))
                തിരികെ -EINVAL;

        xsprintf(t, "%i", മൂല്യം);

        തിരികെ write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

പൂർണ്ണസംഖ്യ get_oom_score_adjust(പൂർണ്ണസംഖ്യ *ret) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!oom_score_adjust_is_valid(a))
                തിരികെ -ENODATA;

        എങ്കിൽ (ret)
                *ret = a;

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rlimit_to_nice(rlim_t limit) {
        എങ്കിൽ (limit <= 1)
                തിരികെ PRIO_MAX-1; /* i.e. 19 */

        എങ്കിൽ (limit >= -PRIO_MIN + PRIO_MAX)
                തിരികെ PRIO_MIN; /* i.e. -20 */

        തിരികെ PRIO_MAX - (പൂർണ്ണസംഖ്യ) limit;
}

പൂർണ്ണസംഖ്യ setpriority_closest(പൂർണ്ണസംഖ്യ priority) {
        ഘടന rlimit highest;
        പൂർണ്ണസംഖ്യ r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        എങ്കിൽ (r >= 0)
                തിരികെ 1;
        എങ്കിൽ (!ERRNO_IS_NEG_PRIVILEGE(r))
                തിരികെ r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        എങ്കിൽ (errno != 0)
                തിരികെ -errno;

        എങ്കിൽ (priority == current)
                തിരികെ 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        എങ്കിൽ (priority > current)
                തിരികെ r;

        എങ്കിൽ (getrlimit(RLIMIT_NICE, &highest) < 0)
                തിരികെ -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        എങ്കിൽ (current > limit)
                എങ്കിൽ (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        തിരികെ -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        തിരികെ 0;
}

_noreturn_ ശൂന്യം freeze(ശൂന്യം) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (ശൂന്യം) close_all_fds_without_malloc(ശൂന്യ, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        വേണ്ടി (;;) {
                siginfo_t si = {};

                എങ്കിൽ (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        നിർത്തുക;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        വേണ്ടി (;;)
                pause();
}

പൂർണ്ണസംഖ്യ get_process_threads(pid_t pid) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ n, r;

        എങ്കിൽ (pid < 0)
                തിരികെ -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = safe_atoi(t, &n);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (n < 0)
                തിരികെ -EINVAL;

        തിരികെ n;
}

പൂർണ്ണസംഖ്യ is_reaper_process(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        എങ്കിൽ (getpid_cached() == 1)
                തിരികെ true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ b != 0;
}

പൂർണ്ണസംഖ്യ make_reaper_process(bool b) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (getpid_cached() == 1) {

                എങ്കിൽ (!b)
                        തിരികെ -EINVAL;

                തിരികെ 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, ശൂന്യ);

പൂർണ്ണസംഖ്യ posix_spawn_wrapper(
                സ്ഥിരം അക്ഷരം *path,
                അക്ഷരം * സ്ഥിരം *argv,
                അക്ഷരം * സ്ഥിരം *envp,
                സ്ഥിരം അക്ഷരം *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        പൂർണ്ണസംഖ്യ r;

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

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(argv);
        പ്രതിജ്ഞാ(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        എങ്കിൽ (r != 0)
                തിരികെ -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        സ്ഥിതം bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ പൂർണ്ണസംഖ്യ cgroup_fd = -EBADF;

        എങ്കിൽ (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ അക്ഷരം *resolved_cgroup = ശൂന്യ;

                r = cg_get_path(cgroup, /* suffix= */ ശൂന്യ, &resolved_cgroup);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                എങ്കിൽ (cgroup_fd < 0)
                        തിരികെ -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                എങ്കിൽ (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                അല്ലെങ്കിൽ എങ്കിൽ (r != ENOSYS)
                        തിരികെ -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        എങ്കിൽ (r != 0)
                തിരികെ -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        എങ്കിൽ (r != 0)
                തിരികെ -r;

        _cleanup_close_ പൂർണ്ണസംഖ്യ pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, ശൂന്യ, &attr, argv, envp);
        എങ്കിൽ (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                തിരികെ -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        എങ്കിൽ ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                എങ്കിൽ (r != 0)
                        തിരികെ -r;

                r = pidfd_spawn(&pidfd, path, ശൂന്യ, &attr, argv, envp);
        }
        എങ്കിൽ (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(r))
                തിരികെ -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, ശൂന്യ, &attr, argv, envp);
        എങ്കിൽ (r != 0)
                തിരികെ -r;

        r = pidref_set_pid(ret_pidref, pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

പൂർണ്ണസംഖ്യ proc_dir_open(DIR **ret) {
        DIR *d;

        പ്രതിജ്ഞാ(ret);

        d = opendir("/proc");
        എങ്കിൽ (!d)
                തിരികെ -errno;

        *ret = d;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ proc_dir_read(DIR *d, pid_t *ret) {
        പ്രതിജ്ഞാ(d);

        വേണ്ടി (;;) {
                ഘടന dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                എങ്കിൽ (!de) {
                        എങ്കിൽ (errno != 0)
                                തിരികെ -errno;

                        നിർത്തുക;
                }

                എങ്കിൽ (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        തുടരുക;

                എങ്കിൽ (parse_pid(de->d_name, ret) >= 0)
                        തിരികെ 1;
        }

        എങ്കിൽ (ret)
                *ret = 0;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ proc_dir_read_pidref(DIR *d, PidRef *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(d);

        വേണ്ടി (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                r = pidref_set_pid(ret, pid);
                എങ്കിൽ (r == -ESRCH) /* gone by now? skip it */
                        തുടരുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ 1;
        }

        എങ്കിൽ (ret)
                *ret = PIDREF_NULL;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_mlockall(പൂർണ്ണസംഖ്യ flags) {
        പൂർണ്ണസംഖ്യ r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        എങ്കിൽ (r == 0)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        എങ്കിൽ (mlockall(flags) < 0)
                തിരികെ log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, പൂർണ്ണസംഖ്യ, INT_MAX);

_noreturn_ ശൂന്യം report_errno_and_exit(പൂർണ്ണസംഖ്യ errno_fd, പൂർണ്ണസംഖ്യ error) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (error >= 0)
                _exit(EXIT_SUCCESS);

        പ്രതിജ്ഞാ(errno_fd >= 0);

        r = loop_write(errno_fd, &error, വലുപ്പം(error));
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

പൂർണ്ണസംഖ്യ read_errno(പൂർണ്ണസംഖ്യ errno_fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, വലുപ്പം(r), /* do_poll= */ false);
        എങ്കിൽ (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                തിരികെ -EIO;
        }
        എങ്കിൽ (n == 0) /* the process exited without reporting an error, assuming success */
                തിരികെ 0;
        എങ്കിൽ (n != വലുപ്പം(r))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        എങ്കിൽ (r == 0)
                തിരികെ 0;
        എങ്കിൽ (r < 0) /* child process reported an error, return it */
                തിരികെ log_debug_errno(r, "Child process failed with errno: %m");

        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

പൂർണ്ണസംഖ്യ prctl_safe(പൂർണ്ണസംഖ്യ op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        തിരികെ RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

പൂർണ്ണസംഖ്യ proc_set_comm(സ്ഥിരം അക്ഷരം *comm) {
        തിരികെ prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

പൂർണ്ണസംഖ്യ proc_set_nnp(ശൂന്യം) {
        തിരികെ prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        എങ്കിൽ (r < 0)
                തിരികെ NSEC_INFINITY;

        തിരികെ r;
}
