/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/oom.h>
#अन्तर्भाव <pthread.h>
#अन्तर्भाव <spawn.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/mman.h>
#अन्तर्भाव <sys/mount.h>
#अन्तर्भाव <sys/personality.h>
#अन्तर्भाव <sys/prctl.h>
#अन्तर्भाव <sys/wait.h>
#अन्तर्भाव <syslog.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>
#सचे HAVE_VALGRIND_VALGRIND_H
#अन्तर्भाव <valgrind/valgrind.h>
#endif

#अन्तर्भाव "sd-messages.h"

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "architecture.h"
#अन्तर्भाव "argv-util.h"
#अन्तर्भाव "bitfield.h"
#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "cgroup-util.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "dlfcn-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "locale-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "mountpoint-util.h"
#अन्तर्भाव "namespace-util.h"
#अन्तर्भाव "nulstr-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidfd-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "raw-clone.h"
#अन्तर्भाव "rlimit-util.h"
#अन्तर्भाव "signal-util.h"
#अन्तर्भाव "socket-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

स्थिर पूर्णाङ्क get_process_state(pid_t pid) {
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        ध्रुव वर्ण *p;
        वर्ण state;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        सचे (pid == 0 || pid == getpid_cached())
                पटिनिवत्त (unsigned वर्ण) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_पङ्क्तिः_file(p, &पङ्क्तिः);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        p = strrchr(पङ्क्तिः, ')');
        सचे (!p)
                पटिनिवत्त -EIO;

        p++;

        सचे (sscanf(p, " %c", &state) != 1)
                पटिनिवत्त -EIO;

        पटिनिवत्त (unsigned वर्ण) state;
}

पूर्णाङ्क pid_get_comm(pid_t pid, वर्ण **ret) {
        _cleanup_free_ वर्ण *escaped = रिक्त, *comm = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        सचे (pid == 0 || pid == getpid_cached()) {
                comm = new0(वर्ण, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                सचे (!comm)
                        पटिनिवत्त -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                सचे (r < 0)
                        पटिनिवत्त r;
        } अञ्ञथा {
                ध्रुव वर्ण *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_पङ्क्तिः_file(p, &comm);
                सचे (r == -ENOENT)
                        पटिनिवत्त -ESRCH;
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        escaped = new(वर्ण, COMM_MAX_LEN);
        सचे (!escaped)
                पटिनिवत्त -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_get_comm(ध्रुव PidRef *pid, वर्ण **ret) {
        _cleanup_free_ वर्ण *comm = रिक्त;
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret)
                *ret = TAKE_PTR(comm);
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                वर्ण **ret,
                size_t *ret_size) {

        _cleanup_free_ वर्ण *t = रिक्त;
        ध्रुव वर्ण *p;
        size_t k;
        पूर्णाङ्क r;

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
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (k == 0) {
                सचे (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        पटिनिवत्त -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ वर्ण *comm = रिक्त;

                r = pid_get_comm(pid, &comm);
                सचे (r < 0)
                        पटिनिवत्त r;

                मुक्त(t);
                t = strjoin("[", comm, "]");
                सचे (!t)
                        पटिनिवत्त -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                सचे (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        सचे (ret)
                *ret = TAKE_PTR(t);
        सचे (ret_size)
                *ret_size = k;

        पटिनिवत्त r;
}

पूर्णाङ्क pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *t = रिक्त;
        size_t k;
        वर्ण *ans;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

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

        पूर्णाङ्क full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        सचे (full < 0)
                पटिनिवत्त full;

        सचे (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                प्रतिज्ञा(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ वर्ण **args = रिक्त;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                सचे (!args)
                        पटिनिवत्त -ENOMEM;

                ans = quote_command_पङ्क्तिः(args, shflags);
                सचे (!ans)
                        पटिनिवत्त -ENOMEM;
        } अञ्ञथा {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                अत्थाय (size_t i = 0; i < k - 1; i++)
                        सचे (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                सचे (!ans)
                        पटिनिवत्त -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_get_cmdline(ध्रुव PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret)
                *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, वर्ण ***ret) {
        _cleanup_free_ वर्ण *t = रिक्त;
        वर्ण **args;
        size_t k;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        प्रतिज्ञा(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        सचे (r < 0)
                पटिनिवत्त r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        सचे (!args)
                पटिनिवत्त -ENOMEM;

        *ret = args;
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_get_cmdline_strv(ध्रुव PidRef *pid, ProcessCmdlineFlags flags, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **args = रिक्त;
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret)
                *ret = TAKE_PTR(args);

        पटिनिवत्त 0;
}

पूर्णाङ्क procfs_get_stat_flags(ध्रुव वर्ण *path, unsigned long long *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        r = read_one_पङ्क्तिः_file(path, &पङ्क्तिः);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        /* Skip past the comm field */
        वर्ण *q = strrchr(पङ्क्तिः, ')');
        सचे (!q)
                पटिनिवत्त -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        अत्थाय (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                सचे (l < 1)
                        पटिनिवत्त -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                सचे (l < 1)
                        पटिनिवत्त -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        सचे (l < 1)
                पटिनिवत्त -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        सचे (l < 1)
                पटिनिवत्त -EINVAL;
        q[l] = 0;

        पटिनिवत्त safe_atollu(q, ret);
}

पूर्णाङ्क pid_is_kernel_thread(pid_t pid) {
        पूर्णाङ्क r;

        सचे (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                पटिनिवत्त 0;
        सचे (!pid_is_valid(pid))
                पटिनिवत्त -EINVAL;

        ध्रुव वर्ण *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त !!(flags & PF_KTHREAD);
}

पूर्णाङ्क pidref_is_kernel_thread(ध्रुव PidRef *pid) {
        पूर्णाङ्क result, r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        सचे (result < 0)
                पटिनिवत्त result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त result;
}

स्थिर पूर्णाङ्क get_process_link_contents(pid_t pid, ध्रुव वर्ण *proc_file, वर्ण **ret) {
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        पटिनिवत्त (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

पूर्णाङ्क get_process_exe(pid_t pid, वर्ण **ret) {
        वर्ण *d;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret) {
                d = endswith(*ret, " (deleted)");
                सचे (d)
                        *d = '\0';
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क pid_get_uid(pid_t pid, uid_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        सचे (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                पटिनिवत्त 0;
        }

        _cleanup_free_ वर्ण *v = रिक्त;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त parse_uid(v, ret);
}

पूर्णाङ्क pidref_get_uid(ध्रुव PidRef *pid, uid_t *ret) {
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        सचे (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                सचे (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        पटिनिवत्त r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret)
                *ret = uid;
        पटिनिवत्त 0;
}

पूर्णाङ्क get_process_gid(pid_t pid, gid_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        सचे (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                पटिनिवत्त 0;
        }

        _cleanup_free_ वर्ण *v = रिक्त;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त parse_gid(v, ret);
}

पूर्णाङ्क get_process_cwd(pid_t pid, वर्ण **ret) {
        प्रतिज्ञा(pid >= 0);

        सचे (pid == 0 || pid == getpid_cached())
                पटिनिवत्त safe_getcwd(ret);

        पटिनिवत्त get_process_link_contents(pid, "cwd", ret);
}

पूर्णाङ्क get_process_मूलम्(pid_t pid, वर्ण **ret) {
        प्रतिज्ञा(pid >= 0);
        पटिनिवत्त get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

पूर्णाङ्क get_process_environ(pid_t pid, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_free_ वर्ण *outcome = रिक्त;
        size_t sz = 0;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        अत्थाय (;;) {
                वर्ण c;

                सचे (sz >= ENVIRONMENT_BLOCK_MAX)
                        पटिनिवत्त -ENOBUFS;

                सचे (!GREEDY_REALLOC(outcome, sz + 5))
                        पटिनिवत्त -ENOMEM;

                r = safe_fgetc(f, &c);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        भङ्ग;

                सचे (c == '\0')
                        outcome[sz++] = '\n';
                अञ्ञथा
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        पटिनिवत्त 0;
}

पूर्णाङ्क pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        unsigned long ppid;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);

        सचे (pid == 0)
                pid = getpid_cached();
        सचे (pid == 1) /* PID 1 has no parent, shortcut this case */
                पटिनिवत्त -EADDRNOTAVAIL;

        सचे (pid == getpid_cached()) {
                सचे (ret)
                        *ret = getppid();
                पटिनिवत्त 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पङ्क्तिः_file(p, &पङ्क्तिः);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(पङ्क्तिः, ')');
        सचे (!p)
                पटिनिवत्त -EIO;
        p++;

        सचे (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                पटिनिवत्त -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        सचे (ppid == 0)
                पटिनिवत्त -EADDRNOTAVAIL;

        सचे ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                पटिनिवत्त -ERANGE;

        सचे (ret)
                *ret = (pid_t) ppid;

        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_get_ppid(ध्रुव PidRef *pidref, pid_t *ret) {
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        सचे (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                सचे (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        पटिनिवत्त r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pidref);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret)
                *ret = ppid;
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_get_ppid_as_pidref(ध्रुव PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        पूर्णाङ्क r;

        POINTER_MAY_BE_NULL(pidref);
        प्रतिज्ञा(ret);

        r = pidref_get_ppid(pidref, &ppid);
        सचे (r < 0)
                पटिनिवत्त r;

        अत्थाय (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                सचे (r < 0)
                        पटिनिवत्त r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                सचे (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        सचे (parent.pid != ppid)
                                अनुवर्त;
                }

                *ret = TAKE_PIDREF(parent);
                पटिनिवत्त 0;
        }

        /* Give up after 16 tries */
        पटिनिवत्त -ENOTRECOVERABLE;
}

पूर्णाङ्क pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पङ्क्तिः_file(p, &पङ्क्तिः);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(पङ्क्तिः, ')');
        सचे (!p)
                पटिनिवत्त -EIO;
        p++;

        unsigned long llu;

        सचे (sscanf(p, " "
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
                पटिनिवत्त -EIO;

        सचे (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_get_start_time(ध्रुव PidRef *pid, usec_t *ret) {
        usec_t t;
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pid);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret)
                *ret = t;

        पटिनिवत्त 0;
}

पूर्णाङ्क get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ वर्ण *m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त parse_mode(m, ret);
}

स्थिर पूर्णाङ्क pid_get_sigmask(pid_t pid, ध्रुव वर्ण *field_name, uint64_t *ret) {
        _cleanup_free_ वर्ण *field = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(field_name);
        प्रतिज्ञा(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त safe_atou64_full(field, 16, ret);
}

स्थिर पूर्णाङ्क pidref_has_sigmask(ध्रुव PidRef *pidref, पूर्णाङ्क sig, ध्रुव वर्ण *field_name) {
        uint64_t mask;
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;
        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;
        सचे (!SIGNAL_VALID(sig))
                पटिनिवत्त -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        सचे (r < 0)
                पटिनिवत्त r;

        r = pidref_verify(pidref);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त BIT_SET(mask, sig - 1);
}

पूर्णाङ्क pidref_has_sigcgt(ध्रुव PidRef *pidref, पूर्णाङ्क sig) {
        पटिनिवत्त pidref_has_sigmask(pidref, sig, "SigCgt");
}

पूर्णाङ्क pidref_has_sigblk(ध्रुव PidRef *pidref, पूर्णाङ्क sig) {
        पटिनिवत्त pidref_has_sigmask(pidref, sig, "SigBlk");
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
पूर्णाङ्क pidref_wait_for_terminate_and_check(ध्रुव वर्ण *name, PidRef *pidref, WaitFlags flags) {
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;
        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;
        सचे (pidref->pid == 1 || pidref_is_self(pidref))
                पटिनिवत्त -ECHILD;

        _cleanup_free_ वर्ण *buffer = रिक्त;
        सचे (!name) {
                r = pidref_get_comm(pidref, &buffer);
                सचे (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                अञ्ञथा
                        name = buffer;
        }

        पूर्णाङ्क prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        सचे (r < 0)
                पटिनिवत्त log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        सचे (status.si_code == CLD_EXITED) {
                सचे (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                अञ्ञथा
                        log_debug("'%s' succeeded.", name);

                पटिनिवत्त status.si_status;

        } अञ्ञथा सचे (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                पटिनिवत्त log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        पटिनिवत्त log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

पूर्णाङ्क kill_and_sigcont(pid_t pid, पूर्णाङ्क sig) {
        पूर्णाङ्क r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        सचे (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (शून्य) kill(pid, SIGCONT);

        पटिनिवत्त r;
}

पूर्णाङ्क getenv_for_pid(pid_t pid, ध्रुव वर्ण *field, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        ध्रुव वर्ण *path;
        size_t sum = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(field);
        प्रतिज्ञा(ret);

        सचे (pid == 0 || pid == getpid_cached())
                पटिनिवत्त strdup_to_full(ret, getenv(field));

        सचे (!pid_is_valid(pid))
                पटिनिवत्त -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        अत्थाय (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
                ध्रुव वर्ण *match;

                सचे (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        पटिनिवत्त -ENOBUFS;

                r = read_nul_string(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)  /* EOF */
                        भङ्ग;

                sum += r;

                match = startswith(पङ्क्तिः, field);
                सचे (match && *match == '=')
                        पटिनिवत्त strdup_to_full(ret, match + 1);
        }

        *ret = रिक्त;
        पटिनिवत्त 0;
}

पूर्णाङ्क pidref_is_my_child(PidRef *pid) {
        पूर्णाङ्क r;

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        सचे (pid->pid == 1 || pidref_is_self(pid))
                पटिनिवत्त false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        सचे (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                पटिनिवत्त false;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त ppid == getpid_cached();
}

पूर्णाङ्क pid_is_my_child(pid_t pid) {

        सचे (pid == 0)
                पटिनिवत्त false;

        पटिनिवत्त pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

पूर्णाङ्क pidref_is_unwaited(PidRef *pid) {
        पूर्णाङ्क r;

        /* Checks whether a PID is still valid at all, including a zombie */

        सचे (!pidref_is_set(pid))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pid))
                पटिनिवत्त -EREMOTE;

        सचे (pid->pid == 1 || pidref_is_self(pid))
                पटिनिवत्त true;

        r = pidref_kill(pid, 0);
        सचे (r == -ESRCH)
                पटिनिवत्त false;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त true;
}

पूर्णाङ्क pid_is_unwaited(pid_t pid) {

        सचे (pid == 0)
                पटिनिवत्त true;

        पटिनिवत्त pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

पूर्णाङ्क pid_is_alive(pid_t pid) {
        पूर्णाङ्क r;

        /* Checks whether a PID is still valid and not a zombie */

        सचे (pid < 0)
                पटिनिवत्त -ESRCH;

        सचे (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                पटिनिवत्त true;

        सचे (pid == getpid_cached())
                पटिनिवत्त true;

        r = get_process_state(pid);
        सचे (r == -ESRCH)
                पटिनिवत्त false;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त r != 'Z';
}

पूर्णाङ्क pidref_is_alive(ध्रुव PidRef *pidref) {
        पूर्णाङ्क r, result;

        सचे (!pidref_is_set(pidref))
                पटिनिवत्त -ESRCH;

        सचे (pidref_is_remote(pidref))
                पटिनिवत्त -EREMOTE;

        result = pid_is_alive(pidref->pid);
        सचे (result < 0) {
                प्रतिज्ञा(result != -ESRCH);
                पटिनिवत्त result;
        }

        r = pidref_verify(pidref);
        सचे (r == -ESRCH)
                पटिनिवत्त false;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त result;
}

पूर्णाङ्क pidref_from_same_मूलम्_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        पूर्णाङ्क r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        सचे (!a || !b) {
                r = pidref_set_self(&self);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (!a)
                        a = &self;
                सचे (!b)
                        b = &self;
        }

        सचे (!pidref_is_set(a) || !pidref_is_set(b))
                पटिनिवत्त -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        सचे (pidref_is_remote(a) && pidref_is_remote(b))
                पटिनिवत्त -EREMOTE;
        सचे (pidref_is_remote(a) || pidref_is_remote(b))
                पटिनिवत्त false;

        सचे (pidref_equal(a, b))
                पटिनिवत्त true;

        ध्रुव वर्ण *roota = procfs_file_alloca(a->pid, "root");
        ध्रुव वर्ण *rootb = procfs_file_alloca(b->pid, "root");

        पूर्णाङ्क result = inode_same(roota, rootb, 0);
        सचे (result == -ENOENT)
                पटिनिवत्त proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        सचे (result < 0)
                पटिनिवत्त result;

        r = pidref_verify(a);
        सचे (r < 0)
                पटिनिवत्त r;
        r = pidref_verify(b);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त result;
}

bool is_main_thread(शून्य) {
        स्थिर thread_local पूर्णाङ्क cached = -1;

        सचे (cached < 0)
                cached = getpid_cached() == gettid();

        पटिनिवत्त cached;
}

unsigned long personality_from_string(ध्रुव वर्ण *s) {
        Architecture architecture;

        सचे (!s)
                पटिनिवत्त PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        सचे (architecture < 0)
                पटिनिवत्त PERSONALITY_INVALID;

        सचे (architecture == native_architecture())
                पटिनिवत्त PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        सचे (architecture == ARCHITECTURE_SECONDARY)
                पटिनिवत्त PER_LINUX32;
#endif

        पटिनिवत्त PERSONALITY_INVALID;
}

ध्रुव वर्ण* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        सचे (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        अञ्ञथा सचे (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        सचे (architecture < 0)
                पटिनिवत्त रिक्त;

        पटिनिवत्त architecture_to_string(architecture);
}

पूर्णाङ्क safe_personality(unsigned long p) {
        पूर्णाङ्क ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        सचे (ret < 0) {
                सचे (errno != 0)
                        पटिनिवत्त -errno;

                errno = -ret;
        }

        पटिनिवत्त ret;
}

पूर्णाङ्क opinionated_personality(unsigned long *ret) {
        पूर्णाङ्क current;

        प्रतिज्ञा(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        सचे (current < 0)
                पटिनिवत्त current;

        सचे (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        अञ्ञथा
                *ret = PER_LINUX;

        पटिनिवत्त 0;
}

शून्य valgrind_summary_hack(शून्य) {
#सचे HAVE_VALGRIND_VALGRIND_H
        सचे (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                सचे (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                अञ्ञथा सचे (pid == 0)
                        exit(EXIT_SUCCESS);
                अञ्ञथा {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (शून्य) pidref_set_pid(&pidref, pid);
                        (शून्य) pidref_wait_for_terminate(&pidref, रिक्त);
                }
        }
#endif
}

पूर्णाङ्क pid_compare_func(ध्रुव pid_t *a, ध्रुव pid_t *b) {
        /* Suitable for usage in qsort() */
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त CMP(*a, *b);
}

bool nice_is_valid(पूर्णाङ्क n) {
        पटिनिवत्त n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(पूर्णाङ्क policy) {
        पटिनिवत्त IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(पूर्णाङ्क policy) {
        पटिनिवत्त sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
पूर्णाङ्क sched_get_priority_min_safe(पूर्णाङ्क policy) {
        पूर्णाङ्क r;

        r = sched_get_priority_min(policy);
        सचे (r >= 0)
                पटिनिवत्त r;

        /* Fallback priority */
        पटिनिवत्त 0;
}

पूर्णाङ्क sched_get_priority_max_safe(पूर्णाङ्क policy) {
        पूर्णाङ्क r;

        r = sched_get_priority_max(policy);
        सचे (r >= 0)
                पटिनिवत्त r;

        पटिनिवत्त 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

स्थिर pid_t cached_pid = CACHED_PID_UNSET;

शून्य reset_cached_pid(शून्य) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(शून्य) {
        स्थिर bool installed = false;
        pid_t current_मानम्;

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
        current_मानम् = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        सचे (current_मानम् > 0)
                पटिनिवत्त current_मानम्;

        current_मानम् = CACHED_PID_UNSET;
        (शून्य) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_मानम्,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        विकल्प (current_मानम्) {

        पक्ष CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                सचे (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        सचे (pthread_atfork(रिक्त, रिक्त, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                पटिनिवत्त new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                पटिनिवत्त new_pid;
        }

        पक्ष CACHED_PID_BUSY: /* Somebody else is currently initializing */
                पटिनिवत्त getpid();

        स्वभाव: /* Properly initialized */
                पटिनिवत्त current_मानम्;
        }
}

पूर्णाङ्क must_be_मूलम्(शून्य) {

        सचे (geteuid() == 0)
                पटिनिवत्त 0;

        पटिनिवत्त log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_राशिः(पूर्णाङ्क (*fn)(शून्य *), पूर्णाङ्क flags, शून्य *userdata) {
        size_t ps;
        pid_t pid;
        शून्य *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        प्रतिज्ञा((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (शून्य*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#सचे HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#अञ्ञथा
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        सचे (pid < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त pid;
}

स्थिर पूर्णाङ्क fork_flags_to_signal(ForkFlags flags) {
        पटिनिवत्त (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

पूर्णाङ्क pidref_safe_fork_full(
                ध्रुव वर्ण *name,
                ध्रुव पूर्णाङ्क stdio_fds[3],
                पूर्णाङ्क except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = रिक्त;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ पूर्णाङ्क pidref_transport_fds[2] = EBADF_PAIR;
        पूर्णाङ्क prio, r;

        प्रतिज्ञा(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        प्रतिज्ञा(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        सचे (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        सचे (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } अञ्ञथा सचे (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        सचे (block_signals) {
                सचे (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        पटिनिवत्त log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        सचे (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                सचे (r < 0)
                        पटिनिवत्त log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                सचे (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        सचे (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                पटिनिवत्त log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        सचे (pid < 0)
                                पटिनिवत्त log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        सचे (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                सचे (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        सचे (r < 0)
                                                पटिनिवत्त log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        सचे (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                पटिनिवत्त -EPROTO;

                                        पूर्णाङ्क pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, परिमाण(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        सचे (n < 0)
                                                पटिनिवत्त log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                पटिनिवत्त 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        सचे ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        अञ्ञथा
                pid = fork();
        सचे (pid < 0)
                पटिनिवत्त log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        सचे (pid > 0) {

                /* If we are in the intermediary process, exit now */
                सचे (intermediary) {
                        सचे (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                सचे (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, परिमाण(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                सचे (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                सचे (flags & FORK_WAIT) {
                        सचे (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (शून्य) sigprocmask(SIG_SETMASK, &ss, रिक्त);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                पटिनिवत्त -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        सचे (ret)
                                *ret = PIDREF_NULL;

                        पटिनिवत्त 1;
                }

                सचे (ret) {
                        r = pidref_set_pid(ret, pid);
                        सचे (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                पटिनिवत्त 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = रिक्त;

        सचे (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        सचे (name) {
                r = rename_process(name);
                सचे (r < 0)
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
        सचे (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        सचे (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } अञ्ञथा सचे (block_signals) { /* undo what we did above */
                सचे (sigprocmask(SIG_SETMASK, &saved_ss, रिक्त) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                सचे (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                अञ्ञथा सचे (ppid != original_pid) {
                        पूर्णाङ्क sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (शून्य) उत्थापय(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                सचे (mount(रिक्त, "/", रिक्त, MS_SLAVE | MS_REC, रिक्त) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                प्रतिज्ञा(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (flags & FORK_REARRANGE_STDIO) {
                सचे (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        सचे (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } अञ्ञथा {
                        r = make_null_stdio();
                        सचे (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } अञ्ञथा सचे (flags & FORK_STDOUT_TO_STDERR) {
                सचे (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                प्रतिज्ञा(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        सचे (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        सचे (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        सचे (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        सचे (ret) {
                r = pidref_set_self(ret);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क namespace_fork_full(
                ध्रुव वर्ण *outer_name,
                ध्रुव वर्ण *inner_name,
                पूर्णाङ्क except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                पूर्णाङ्क pidns_fd,
                पूर्णाङ्क mntns_fd,
                पूर्णाङ्क netns_fd,
                पूर्णाङ्क userns_fd,
                पूर्णाङ्क मूलम्_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ पूर्णाङ्क errno_pipe_fd[2] = EBADF_PAIR;
        पूर्णाङ्क r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        प्रतिज्ञा((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        प्रतिज्ञा((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        प्रतिज्ञा(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(परिमाण(पूर्णाङ्क) <= PIPE_BUF);
        सचे (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                पटिनिवत्त log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ रिक्त, /* except_fds= */ रिक्त, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        सचे (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                पूर्णाङ्क k = read_errno(errno_pipe_fd[0]);
                सचे (k < 0 && k != -EIO)
                        पटिनिवत्त k;
        }
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, मूलम्_fd, namespace_flags);
                सचे (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                रिक्त,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                सचे (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                सचे (r == 0) {
                        /* Child */

                        सचे (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } अञ्ञथा {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        सचे (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        पटिनिवत्त 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (शून्य) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                सचे (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        सचे (r < 0)
                पटिनिवत्त r; /* the child logs about failures on its own, no need to duplicate here */

        सचे (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        अञ्ञथा
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        पटिनिवत्त 1;
}

पूर्णाङ्क namespace_fork(
                ध्रुव वर्ण *outer_name,
                ध्रुव वर्ण *inner_name,
                ForkFlags flags,
                पूर्णाङ्क pidns_fd,
                पूर्णाङ्क mntns_fd,
                पूर्णाङ्क netns_fd,
                पूर्णाङ्क userns_fd,
                पूर्णाङ्क मूलम्_fd,
                PidRef *ret) {

        पटिनिवत्त namespace_fork_full(outer_name, inner_name, /* except_fds= */ रिक्त, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, मूलम्_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(पूर्णाङ्क oa) {
        पटिनिवत्त oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

पूर्णाङ्क set_oom_score_adjust(पूर्णाङ्क मानम्) {
        वर्ण t[DECIMAL_STR_MAX(पूर्णाङ्क)];

        सचे (!oom_score_adjust_is_valid(मानम्))
                पटिनिवत्त -EINVAL;

        xsprintf(t, "%i", मानम्);

        पटिनिवत्त write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

पूर्णाङ्क get_oom_score_adjust(पूर्णाङ्क *ret) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!oom_score_adjust_is_valid(a))
                पटिनिवत्त -ENODATA;

        सचे (ret)
                *ret = a;

        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क rlimit_to_nice(rlim_t limit) {
        सचे (limit <= 1)
                पटिनिवत्त PRIO_MAX-1; /* i.e. 19 */

        सचे (limit >= -PRIO_MIN + PRIO_MAX)
                पटिनिवत्त PRIO_MIN; /* i.e. -20 */

        पटिनिवत्त PRIO_MAX - (पूर्णाङ्क) limit;
}

पूर्णाङ्क setpriority_closest(पूर्णाङ्क priority) {
        संरचना rlimit highest;
        पूर्णाङ्क r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        सचे (r >= 0)
                पटिनिवत्त 1;
        सचे (!ERRNO_IS_NEG_PRIVILEGE(r))
                पटिनिवत्त r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        सचे (errno != 0)
                पटिनिवत्त -errno;

        सचे (priority == current)
                पटिनिवत्त 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        सचे (priority > current)
                पटिनिवत्त r;

        सचे (getrlimit(RLIMIT_NICE, &highest) < 0)
                पटिनिवत्त -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        सचे (current > limit)
                सचे (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        पटिनिवत्त -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        पटिनिवत्त 0;
}

_noreturn_ शून्य freeze(शून्य) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (शून्य) close_all_fds_without_malloc(रिक्त, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        अत्थाय (;;) {
                siginfo_t si = {};

                सचे (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        भङ्ग;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        अत्थाय (;;)
                pause();
}

पूर्णाङ्क get_process_threads(pid_t pid) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क n, r;

        सचे (pid < 0)
                पटिनिवत्त -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        सचे (r == -ENOENT)
                पटिनिवत्त -ESRCH;
        सचे (r < 0)
                पटिनिवत्त r;

        r = safe_atoi(t, &n);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (n < 0)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त n;
}

पूर्णाङ्क is_reaper_process(शून्य) {
        पूर्णाङ्क b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        सचे (getpid_cached() == 1)
                पटिनिवत्त true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त b != 0;
}

पूर्णाङ्क make_reaper_process(bool b) {
        पूर्णाङ्क r;

        सचे (getpid_cached() == 1) {

                सचे (!b)
                        पटिनिवत्त -EINVAL;

                पटिनिवत्त 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, रिक्त);

पूर्णाङ्क posix_spawn_wrapper(
                ध्रुव वर्ण *path,
                वर्ण * ध्रुव *argv,
                वर्ण * ध्रुव *envp,
                ध्रुव वर्ण *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        पूर्णाङ्क r;

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

        प्रतिज्ञा(path);
        प्रतिज्ञा(argv);
        प्रतिज्ञा(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        सचे (r != 0)
                पटिनिवत्त -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        स्थिर bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ पूर्णाङ्क cgroup_fd = -EBADF;

        सचे (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ वर्ण *resolved_cgroup = रिक्त;

                r = cg_get_path(cgroup, /* suffix= */ रिक्त, &resolved_cgroup);
                सचे (r < 0)
                        पटिनिवत्त r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                सचे (cgroup_fd < 0)
                        पटिनिवत्त -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                सचे (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                अञ्ञथा सचे (r != ENOSYS)
                        पटिनिवत्त -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        सचे (r != 0)
                पटिनिवत्त -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        सचे (r != 0)
                पटिनिवत्त -r;

        _cleanup_close_ पूर्णाङ्क pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, रिक्त, &attr, argv, envp);
        सचे (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                पटिनिवत्त -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        सचे ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                सचे (r != 0)
                        पटिनिवत्त -r;

                r = pidfd_spawn(&pidfd, path, रिक्त, &attr, argv, envp);
        }
        सचे (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                सचे (r < 0)
                        पटिनिवत्त r;

                पटिनिवत्त FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        सचे (!ERRNO_IS_NOT_SUPPORTED(r))
                पटिनिवत्त -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, रिक्त, &attr, argv, envp);
        सचे (r != 0)
                पटिनिवत्त -r;

        r = pidref_set_pid(ret_pidref, pid);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

पूर्णाङ्क proc_dir_open(DIR **ret) {
        DIR *d;

        प्रतिज्ञा(ret);

        d = opendir("/proc");
        सचे (!d)
                पटिनिवत्त -errno;

        *ret = d;
        पटिनिवत्त 0;
}

पूर्णाङ्क proc_dir_read(DIR *d, pid_t *ret) {
        प्रतिज्ञा(d);

        अत्थाय (;;) {
                संरचना dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                सचे (!de) {
                        सचे (errno != 0)
                                पटिनिवत्त -errno;

                        भङ्ग;
                }

                सचे (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        अनुवर्त;

                सचे (parse_pid(de->d_name, ret) >= 0)
                        पटिनिवत्त 1;
        }

        सचे (ret)
                *ret = 0;
        पटिनिवत्त 0;
}

पूर्णाङ्क proc_dir_read_pidref(DIR *d, PidRef *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(d);

        अत्थाय (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        भङ्ग;

                r = pidref_set_pid(ret, pid);
                सचे (r == -ESRCH) /* gone by now? skip it */
                        अनुवर्त;
                सचे (r < 0)
                        पटिनिवत्त r;

                पटिनिवत्त 1;
        }

        सचे (ret)
                *ret = PIDREF_NULL;
        पटिनिवत्त 0;
}

पूर्णाङ्क safe_mlockall(पूर्णाङ्क flags) {
        पूर्णाङ्क r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        सचे (r == 0)
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        सचे (mlockall(flags) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        पटिनिवत्त 0;
}

स्थिर ध्रुव वर्ण *ध्रुव sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण* ध्रुव sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, पूर्णाङ्क, INT_MAX);

_noreturn_ शून्य report_errno_and_exit(पूर्णाङ्क errno_fd, पूर्णाङ्क error) {
        पूर्णाङ्क r;

        सचे (error >= 0)
                _exit(EXIT_SUCCESS);

        प्रतिज्ञा(errno_fd >= 0);

        r = loop_write(errno_fd, &error, परिमाण(error));
        सचे (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

पूर्णाङ्क read_errno(पूर्णाङ्क errno_fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, परिमाण(r), /* do_poll= */ false);
        सचे (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                पटिनिवत्त -EIO;
        }
        सचे (n == 0) /* the process exited without reporting an error, assuming success */
                पटिनिवत्त 0;
        सचे (n != परिमाण(r))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        सचे (r == 0)
                पटिनिवत्त 0;
        सचे (r < 0) /* child process reported an error, return it */
                पटिनिवत्त log_debug_errno(r, "Child process failed with errno: %m");

        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

पूर्णाङ्क prctl_safe(पूर्णाङ्क op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        पटिनिवत्त RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

पूर्णाङ्क proc_set_comm(ध्रुव वर्ण *comm) {
        पटिनिवत्त prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

पूर्णाङ्क proc_set_nnp(शून्य) {
        पटिनिवत्त prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(शून्य) {
        पूर्णाङ्क r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        सचे (r < 0)
                पटिनिवत्त NSEC_INFINITY;

        पटिनिवत्त r;
}
