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
#जर HAVE_VALGRIND_VALGRIND_H
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

स्थिर पूर्णांक get_process_state(pid_t pid) {
        _cleanup_free_ अक्षर *ओळ = रिक्त;
        स्थिरांक अक्षर *p;
        अक्षर state;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        जर (pid == 0 || pid == getpid_cached())
                परत (अचिन्हित अक्षर) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_ओळ_file(p, &ओळ);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        p = म_खोजप(ओळ, ')');
        जर (!p)
                परत -EIO;

        p++;

        जर (माला_पूछो(p, " %c", &state) != 1)
                परत -EIO;

        परत (अचिन्हित अक्षर) state;
}

पूर्णांक pid_get_comm(pid_t pid, अक्षर **ret) {
        _cleanup_free_ अक्षर *escaped = रिक्त, *comm = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        जर (pid == 0 || pid == getpid_cached()) {
                comm = new0(अक्षर, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                जर (!comm)
                        परत -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (अचिन्हित दीर्घ) comm, 0, 0, 0);
                जर (r < 0)
                        परत r;
        } अन्यथा {
                स्थिरांक अक्षर *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_ओळ_file(p, &comm);
                जर (r == -ENOENT)
                        परत -ESRCH;
                जर (r < 0)
                        परत r;
        }

        escaped = new(अक्षर, COMM_MAX_LEN);
        जर (!escaped)
                परत -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        परत 0;
}

पूर्णांक pidref_get_comm(स्थिरांक PidRef *pid, अक्षर **ret) {
        _cleanup_free_ अक्षर *comm = रिक्त;
        पूर्णांक r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        जर (r < 0)
                परत r;

        r = pidref_verify(pid);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = TAKE_PTR(comm);
        परत 0;
}

स्थिर पूर्णांक pid_get_cmdline_nulstr(
                pid_t pid,
                माप_प्रकार max_size,
                ProcessCmdlineFlags flags,
                अक्षर **ret,
                माप_प्रकार *ret_size) {

        _cleanup_free_ अक्षर *t = रिक्त;
        स्थिरांक अक्षर *p;
        माप_प्रकार k;
        पूर्णांक r;

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
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        जर (k == 0) {
                जर (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        परत -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ अक्षर *comm = रिक्त;

                r = pid_get_comm(pid, &comm);
                जर (r < 0)
                        परत r;

                मुक्त(t);
                t = strjoin("[", comm, "]");
                जर (!t)
                        परत -ENOMEM;

                k = म_माप(t);
                r = k <= max_size;
                जर (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        जर (ret)
                *ret = TAKE_PTR(t);
        जर (ret_size)
                *ret_size = k;

        परत r;
}

पूर्णांक pid_get_cmdline(pid_t pid, माप_प्रकार max_columns, ProcessCmdlineFlags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *t = रिक्त;
        माप_प्रकार k;
        अक्षर *ans;

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

        पूर्णांक full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        जर (full < 0)
                परत full;

        जर (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                प्रतिज्ञा(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ अक्षर **args = रिक्त;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                जर (!args)
                        परत -ENOMEM;

                ans = quote_command_ओळ(args, shflags);
                जर (!ans)
                        परत -ENOMEM;
        } अन्यथा {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                साठी (माप_प्रकार i = 0; i < k - 1; i++)
                        जर (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                जर (!ans)
                        परत -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        परत 0;
}

पूर्णांक pidref_get_cmdline(स्थिरांक PidRef *pid, माप_प्रकार max_columns, ProcessCmdlineFlags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        जर (r < 0)
                परत r;

        r = pidref_verify(pid);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, अक्षर ***ret) {
        _cleanup_free_ अक्षर *t = रिक्त;
        अक्षर **args;
        माप_प्रकार k;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        प्रतिज्ञा(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        जर (r < 0)
                परत r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        जर (!args)
                परत -ENOMEM;

        *ret = args;
        परत 0;
}

पूर्णांक pidref_get_cmdline_strv(स्थिरांक PidRef *pid, ProcessCmdlineFlags flags, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **args = रिक्त;
        पूर्णांक r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        जर (r < 0)
                परत r;

        r = pidref_verify(pid);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = TAKE_PTR(args);

        परत 0;
}

पूर्णांक procfs_get_stat_flags(स्थिरांक अक्षर *path, अचिन्हित दीर्घ दीर्घ *ret) {
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ अक्षर *ओळ = रिक्त;
        r = read_one_ओळ_file(path, &ओळ);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        /* Skip past the comm field */
        अक्षर *q = म_खोजप(ओळ, ')');
        जर (!q)
                परत -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        साठी (माप_प्रकार i = 0; i < 6; i++) {
                माप_प्रकार l = म_अखोज(q, WHITESPACE);
                जर (l < 1)
                        परत -EINVAL;
                q += l;

                l = म_खोज(q, WHITESPACE);
                जर (l < 1)
                        परत -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        माप_प्रकार l = म_अखोज(q, WHITESPACE);
        जर (l < 1)
                परत -EINVAL;
        q += l;

        /* Truncate the rest */
        l = म_खोज(q, WHITESPACE);
        जर (l < 1)
                परत -EINVAL;
        q[l] = 0;

        परत safe_atollu(q, ret);
}

पूर्णांक pid_is_kernel_thread(pid_t pid) {
        पूर्णांक r;

        जर (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                परत 0;
        जर (!pid_is_valid(pid))
                परत -EINVAL;

        स्थिरांक अक्षर *p = procfs_file_alloca(pid, "stat");

        अचिन्हित दीर्घ दीर्घ flags;
        r = procfs_get_stat_flags(p, &flags);
        जर (r < 0)
                परत r;

        परत !!(flags & PF_KTHREAD);
}

पूर्णांक pidref_is_kernel_thread(स्थिरांक PidRef *pid) {
        पूर्णांक result, r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        जर (result < 0)
                परत result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        जर (r < 0)
                परत r;

        परत result;
}

स्थिर पूर्णांक get_process_link_contents(pid_t pid, स्थिरांक अक्षर *proc_file, अक्षर **ret) {
        स्थिरांक अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        परत (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

पूर्णांक get_process_exe(pid_t pid, अक्षर **ret) {
        अक्षर *d;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        जर (r < 0)
                परत r;

        जर (ret) {
                d = endswith(*ret, " (deleted)");
                जर (d)
                        *d = '\0';
        }

        परत 0;
}

पूर्णांक pid_get_uid(pid_t pid, uid_t *ret) {
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        जर (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                परत 0;
        }

        _cleanup_free_ अक्षर *v = रिक्त;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        परत parse_uid(v, ret);
}

पूर्णांक pidref_get_uid(स्थिरांक PidRef *pid, uid_t *ret) {
        पूर्णांक r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        जर (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                जर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        परत r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        जर (r < 0)
                परत r;

        r = pidref_verify(pid);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = uid;
        परत 0;
}

पूर्णांक get_process_gid(pid_t pid, gid_t *ret) {
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        जर (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                परत 0;
        }

        _cleanup_free_ अक्षर *v = रिक्त;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        परत parse_gid(v, ret);
}

पूर्णांक get_process_cwd(pid_t pid, अक्षर **ret) {
        प्रतिज्ञा(pid >= 0);

        जर (pid == 0 || pid == getpid_cached())
                परत safe_getcwd(ret);

        परत get_process_link_contents(pid, "cwd", ret);
}

पूर्णांक get_process_मूळ(pid_t pid, अक्षर **ret) {
        प्रतिज्ञा(pid >= 0);
        परत get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

पूर्णांक get_process_environ(pid_t pid, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_free_ अक्षर *outcome = रिक्त;
        माप_प्रकार sz = 0;
        स्थिरांक अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        साठी (;;) {
                अक्षर c;

                जर (sz >= ENVIRONMENT_BLOCK_MAX)
                        परत -ENOBUFS;

                जर (!GREEDY_REALLOC(outcome, sz + 5))
                        परत -ENOMEM;

                r = safe_fgetc(f, &c);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                जर (c == '\0')
                        outcome[sz++] = '\n';
                अन्यथा
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        परत 0;
}

पूर्णांक pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ अक्षर *ओळ = रिक्त;
        अचिन्हित दीर्घ ppid;
        स्थिरांक अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);

        जर (pid == 0)
                pid = getpid_cached();
        जर (pid == 1) /* PID 1 has no parent, shortcut this case */
                परत -EADDRNOTAVAIL;

        जर (pid == getpid_cached()) {
                जर (ret)
                        *ret = getppid();
                परत 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ओळ_file(p, &ओळ);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = म_खोजप(ओळ, ')');
        जर (!p)
                परत -EIO;
        p++;

        जर (माला_पूछो(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                परत -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        जर (ppid == 0)
                परत -EADDRNOTAVAIL;

        जर ((pid_t) ppid < 0 || (अचिन्हित दीर्घ) (pid_t) ppid != ppid)
                परत -दुस्फल;

        जर (ret)
                *ret = (pid_t) ppid;

        परत 0;
}

पूर्णांक pidref_get_ppid(स्थिरांक PidRef *pidref, pid_t *ret) {
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                जर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        परत r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : रिक्त);
        जर (r < 0)
                परत r;

        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = ppid;
        परत 0;
}

पूर्णांक pidref_get_ppid_as_pidref(स्थिरांक PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        पूर्णांक r;

        POINTER_MAY_BE_NULL(pidref);
        प्रतिज्ञा(ret);

        r = pidref_get_ppid(pidref, &ppid);
        जर (r < 0)
                परत r;

        साठी (अचिन्हित attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                जर (r < 0)
                        परत r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                जर (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        जर (r < 0)
                                परत r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        जर (parent.pid != ppid)
                                सुरूठेवा;
                }

                *ret = TAKE_PIDREF(parent);
                परत 0;
        }

        /* Give up after 16 tries */
        परत -ENOTRECOVERABLE;
}

पूर्णांक pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ अक्षर *ओळ = रिक्त;
        स्थिरांक अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ओळ_file(p, &ओळ);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = म_खोजप(ओळ, ')');
        जर (!p)
                परत -EIO;
        p++;

        अचिन्हित दीर्घ llu;

        जर (माला_पूछो(p, " "
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
                परत -EIO;

        जर (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        परत 0;
}

पूर्णांक pidref_get_start_time(स्थिरांक PidRef *pid, usec_t *ret) {
        usec_t t;
        पूर्णांक r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : रिक्त);
        जर (r < 0)
                परत r;

        r = pidref_verify(pid);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = t;

        परत 0;
}

पूर्णांक get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ अक्षर *m = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        परत parse_mode(m, ret);
}

स्थिर पूर्णांक pid_get_sigmask(pid_t pid, स्थिरांक अक्षर *field_name, uint64_t *ret) {
        _cleanup_free_ अक्षर *field = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(field_name);
        प्रतिज्ञा(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        परत safe_atou64_full(field, 16, ret);
}

स्थिर पूर्णांक pidref_has_sigmask(स्थिरांक PidRef *pidref, पूर्णांक sig, स्थिरांक अक्षर *field_name) {
        uint64_t mask;
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;
        जर (!SIGNAL_VALID(sig))
                परत -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        जर (r < 0)
                परत r;

        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        परत BIT_SET(mask, sig - 1);
}

पूर्णांक pidref_has_sigcgt(स्थिरांक PidRef *pidref, पूर्णांक sig) {
        परत pidref_has_sigmask(pidref, sig, "SigCgt");
}

पूर्णांक pidref_has_sigblk(स्थिरांक PidRef *pidref, पूर्णांक sig) {
        परत pidref_has_sigmask(pidref, sig, "SigBlk");
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
पूर्णांक pidref_wait_for_terminate_and_check(स्थिरांक अक्षर *name, PidRef *pidref, WaitFlags flags) {
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;
        जर (pidref->pid == 1 || pidref_is_self(pidref))
                परत -ECHILD;

        _cleanup_free_ अक्षर *buffer = रिक्त;
        जर (!name) {
                r = pidref_get_comm(pidref, &buffer);
                जर (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                अन्यथा
                        name = buffer;
        }

        पूर्णांक prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        जर (r < 0)
                परत log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        जर (status.si_code == CLD_EXITED) {
                जर (status.si_status != निकास_सफल)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                अन्यथा
                        log_debug("'%s' succeeded.", name);

                परत status.si_status;

        } अन्यथा जर (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                परत log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        परत log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

पूर्णांक kill_and_sigcont(pid_t pid, पूर्णांक sig) {
        पूर्णांक r;

        r = RET_NERRNO(समाप्त(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        जर (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (शून्य) समाप्त(pid, SIGCONT);

        परत r;
}

पूर्णांक getenv_for_pid(pid_t pid, स्थिरांक अक्षर *field, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        स्थिरांक अक्षर *path;
        माप_प्रकार sum = 0;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(field);
        प्रतिज्ञा(ret);

        जर (pid == 0 || pid == getpid_cached())
                परत strdup_to_full(ret, दो_पर्या(field));

        जर (!pid_is_valid(pid))
                परत -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        साठी (;;) {
                _cleanup_free_ अक्षर *ओळ = रिक्त;
                स्थिरांक अक्षर *match;

                जर (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        परत -ENOBUFS;

                r = read_nul_string(f, LONG_ओळ_MAX, &ओळ);
                जर (r < 0)
                        परत r;
                जर (r == 0)  /* EOF */
                        थांबा;

                sum += r;

                match = startswith(ओळ, field);
                जर (match && *match == '=')
                        परत strdup_to_full(ret, match + 1);
        }

        *ret = रिक्त;
        परत 0;
}

पूर्णांक pidref_is_my_child(PidRef *pid) {
        पूर्णांक r;

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        जर (pid->pid == 1 || pidref_is_self(pid))
                परत false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        जर (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                परत false;
        जर (r < 0)
                परत r;

        परत ppid == getpid_cached();
}

पूर्णांक pid_is_my_child(pid_t pid) {

        जर (pid == 0)
                परत false;

        परत pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

पूर्णांक pidref_is_unwaited(PidRef *pid) {
        पूर्णांक r;

        /* Checks whether a PID is still valid at all, including a zombie */

        जर (!pidref_is_set(pid))
                परत -ESRCH;

        जर (pidref_is_remote(pid))
                परत -EREMOTE;

        जर (pid->pid == 1 || pidref_is_self(pid))
                परत true;

        r = pidref_kill(pid, 0);
        जर (r == -ESRCH)
                परत false;
        जर (r < 0)
                परत r;

        परत true;
}

पूर्णांक pid_is_unwaited(pid_t pid) {

        जर (pid == 0)
                परत true;

        परत pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

पूर्णांक pid_is_alive(pid_t pid) {
        पूर्णांक r;

        /* Checks whether a PID is still valid and not a zombie */

        जर (pid < 0)
                परत -ESRCH;

        जर (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                परत true;

        जर (pid == getpid_cached())
                परत true;

        r = get_process_state(pid);
        जर (r == -ESRCH)
                परत false;
        जर (r < 0)
                परत r;

        परत r != 'Z';
}

पूर्णांक pidref_is_alive(स्थिरांक PidRef *pidref) {
        पूर्णांक r, result;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        result = pid_is_alive(pidref->pid);
        जर (result < 0) {
                प्रतिज्ञा(result != -ESRCH);
                परत result;
        }

        r = pidref_verify(pidref);
        जर (r == -ESRCH)
                परत false;
        जर (r < 0)
                परत r;

        परत result;
}

पूर्णांक pidref_from_same_मूळ_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        पूर्णांक r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        जर (!a || !b) {
                r = pidref_set_self(&self);
                जर (r < 0)
                        परत r;
                जर (!a)
                        a = &self;
                जर (!b)
                        b = &self;
        }

        जर (!pidref_is_set(a) || !pidref_is_set(b))
                परत -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        जर (pidref_is_remote(a) && pidref_is_remote(b))
                परत -EREMOTE;
        जर (pidref_is_remote(a) || pidref_is_remote(b))
                परत false;

        जर (pidref_equal(a, b))
                परत true;

        स्थिरांक अक्षर *roota = procfs_file_alloca(a->pid, "root");
        स्थिरांक अक्षर *rootb = procfs_file_alloca(b->pid, "root");

        पूर्णांक result = inode_same(roota, rootb, 0);
        जर (result == -ENOENT)
                परत proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        जर (result < 0)
                परत result;

        r = pidref_verify(a);
        जर (r < 0)
                परत r;
        r = pidref_verify(b);
        जर (r < 0)
                परत r;

        परत result;
}

bool is_main_thread(शून्य) {
        स्थिर thread_local पूर्णांक cached = -1;

        जर (cached < 0)
                cached = getpid_cached() == gettid();

        परत cached;
}

अचिन्हित दीर्घ personality_from_string(स्थिरांक अक्षर *s) {
        Architecture architecture;

        जर (!s)
                परत PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        जर (architecture < 0)
                परत PERSONALITY_INVALID;

        जर (architecture == native_architecture())
                परत PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        जर (architecture == ARCHITECTURE_SECONDARY)
                परत PER_LINUX32;
#endif

        परत PERSONALITY_INVALID;
}

स्थिरांक अक्षर* personality_to_string(अचिन्हित दीर्घ p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        जर (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        अन्यथा जर (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        जर (architecture < 0)
                परत रिक्त;

        परत architecture_to_string(architecture);
}

पूर्णांक safe_personality(अचिन्हित दीर्घ p) {
        पूर्णांक ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        त्रुटि_सं = 0;
        ret = personality(p);
        जर (ret < 0) {
                जर (त्रुटि_सं != 0)
                        परत -त्रुटि_सं;

                त्रुटि_सं = -ret;
        }

        परत ret;
}

पूर्णांक opinionated_personality(अचिन्हित दीर्घ *ret) {
        पूर्णांक current;

        प्रतिज्ञा(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        जर (current < 0)
                परत current;

        जर (((अचिन्हित दीर्घ) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        अन्यथा
                *ret = PER_LINUX;

        परत 0;
}

शून्य valgrind_summary_hack(शून्य) {
#जर HAVE_VALGRIND_VALGRIND_H
        जर (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                जर (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, त्रुटि_सं,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                अन्यथा जर (pid == 0)
                        निकास(निकास_सफल);
                अन्यथा {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (शून्य) pidref_set_pid(&pidref, pid);
                        (शून्य) pidref_wait_for_terminate(&pidref, रिक्त);
                }
        }
#endif
}

पूर्णांक pid_compare_func(स्थिरांक pid_t *a, स्थिरांक pid_t *b) {
        /* Suitable for usage in qsort() */
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        परत CMP(*a, *b);
}

bool nice_is_valid(पूर्णांक n) {
        परत n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(पूर्णांक policy) {
        परत IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(पूर्णांक policy) {
        परत sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
पूर्णांक sched_get_priority_min_safe(पूर्णांक policy) {
        पूर्णांक r;

        r = sched_get_priority_min(policy);
        जर (r >= 0)
                परत r;

        /* Fallback priority */
        परत 0;
}

पूर्णांक sched_get_priority_max_safe(पूर्णांक policy) {
        पूर्णांक r;

        r = sched_get_priority_max(policy);
        जर (r >= 0)
                परत r;

        परत 0;
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
        pid_t current_मूल्य;

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
        current_मूल्य = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        जर (current_मूल्य > 0)
                परत current_मूल्य;

        current_मूल्य = CACHED_PID_UNSET;
        (शून्य) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_मूल्य,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        निवड (current_मूल्य) {

        स्थिती CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                जर (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        जर (pthread_atfork(रिक्त, रिक्त, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                परत new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                परत new_pid;
        }

        स्थिती CACHED_PID_BUSY: /* Somebody else is currently initializing */
                परत getpid();

        पूर्वनिर्धारित: /* Properly initialized */
                परत current_मूल्य;
        }
}

पूर्णांक must_be_मूळ(शून्य) {

        जर (geteuid() == 0)
                परत 0;

        परत log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_चळत(पूर्णांक (*fn)(शून्य *), पूर्णांक flags, शून्य *userdata) {
        माप_प्रकार ps;
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

#जर HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#अन्यथा
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        जर (pid < 0)
                परत -त्रुटि_सं;

        परत pid;
}

स्थिर पूर्णांक fork_flags_to_signal(ForkFlags flags) {
        परत (flags & FORK_DEATHSIG_SIGTERM) ? संक_इति :
                (flags & FORK_DEATHSIG_SIGINT) ? संक_विघ्न :
                                                 SIGKILL;
}

पूर्णांक pidref_safe_fork_full(
                स्थिरांक अक्षर *name,
                स्थिरांक पूर्णांक stdio_fds[3],
                पूर्णांक except_fds[],
                माप_प्रकार n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = रिक्त;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ पूर्णांक pidref_transport_fds[2] = EBADF_PAIR;
        पूर्णांक prio, r;

        प्रतिज्ञा(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        प्रतिज्ञा(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        जर (flags & FORK_FLUSH_STDIO) {
                ख_साफ(मानक_निकास);
                ख_साफ(मानक_त्रुटि); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        जर (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } अन्यथा जर (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        जर (block_signals) {
                जर (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        परत log_full_errno(prio, त्रुटि_सं, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        जर (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                जर (r < 0)
                        परत log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                जर (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        जर (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                परत log_full_errno(prio, त्रुटि_सं, "Failed to allocate pidref socket: %m");

                        pid = विभाजन();
                        जर (pid < 0)
                                परत log_full_errno(prio, त्रुटि_सं, "Failed to fork off '%s': %m", strna(name));
                        जर (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                जर (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        जर (r < 0)
                                                परत log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        जर (r != निकास_सफल) /* exit status > 0 should be treated as failure, too */
                                                परत -EPROTO;

                                        पूर्णांक pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, आकार(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        जर (n < 0)
                                                परत log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                परत 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        जर ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        अन्यथा
                pid = विभाजन();
        जर (pid < 0)
                परत log_full_errno(prio, त्रुटि_सं, "Failed to fork off '%s': %m", strna(name));
        जर (pid > 0) {

                /* If we are in the intermediary process, exit now */
                जर (intermediary) {
                        जर (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                जर (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(निकास_त्रुटि);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, आकार(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                जर (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(निकास_त्रुटि);
                                }
                        }

                        _exit(निकास_सफल);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                जर (flags & FORK_WAIT) {
                        जर (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (शून्य) sigprocmask(SIG_SETMASK, &ss, रिक्त);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        जर (r < 0)
                                परत r;
                        जर (r != निकास_सफल) /* exit status > 0 should be treated as failure, too */
                                परत -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        जर (ret)
                                *ret = PIDREF_NULL;

                        परत 1;
                }

                जर (ret) {
                        r = pidref_set_pid(ret, pid);
                        जर (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                परत 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = रिक्त;

        जर (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        जर (name) {
                r = rename_process(name);
                जर (r < 0)
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
        जर (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        जर (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(निकास_त्रुटि);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(निकास_त्रुटि);
                }
        } अन्यथा जर (block_signals) { /* undo what we did above */
                जर (sigprocmask(SIG_SETMASK, &saved_ss, रिक्त) < 0) {
                        log_full_errno(prio, त्रुटि_सं, "Failed to restore signal mask: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                जर (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                अन्यथा जर (ppid != original_pid) {
                        पूर्णांक sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (शून्य) उठवा(sig);
                        _exit(निकास_त्रुटि);
                }
        }

        जर (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                जर (mount(रिक्त, "/", रिक्त, MS_SLAVE | MS_REC, रिक्त) < 0) {
                        log_full_errno(prio, त्रुटि_सं, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                प्रतिज्ञा(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (flags & FORK_REARRANGE_STDIO) {
                जर (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        जर (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(निकास_त्रुटि);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } अन्यथा {
                        r = make_null_stdio();
                        जर (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(निकास_त्रुटि);
                        }
                }
        } अन्यथा जर (flags & FORK_STDOUT_TO_STDERR) {
                जर (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, त्रुटि_सं, "Failed to connect stdout to stderr: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                प्रतिज्ञा(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        जर (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        जर (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        जर (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        जर (ret) {
                r = pidref_set_self(ret);
                जर (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(निकास_त्रुटि);
                }
        }

        परत 0;
}

पूर्णांक namespace_fork_full(
                स्थिरांक अक्षर *outer_name,
                स्थिरांक अक्षर *inner_name,
                पूर्णांक except_fds[],
                माप_प्रकार n_except_fds,
                ForkFlags flags,
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूळ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ पूर्णांक errno_pipe_fd[2] = EBADF_PAIR;
        पूर्णांक r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

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
        assert_cc(आकार(पूर्णांक) <= PIPE_BUF);
        जर (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                परत log_full_errno(prio, त्रुटि_सं, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ रिक्त, /* except_fds= */ रिक्त, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        जर (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                पूर्णांक k = read_errno(errno_pipe_fd[0]);
                जर (k < 0 && k != -EIO)
                        परत k;
        }
        जर (r < 0)
                परत r;
        जर (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, मूळ_fd, namespace_flags);
                जर (r < 0) {
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
                जर (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                जर (r == 0) {
                        /* Child */

                        जर (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } अन्यथा {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        जर (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        परत 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (शून्य) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                जर (r < 0)
                        _exit(निकास_त्रुटि);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        जर (r < 0)
                परत r; /* the child logs about failures on its own, no need to duplicate here */

        जर (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        अन्यथा
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        परत 1;
}

पूर्णांक namespace_fork(
                स्थिरांक अक्षर *outer_name,
                स्थिरांक अक्षर *inner_name,
                ForkFlags flags,
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूळ_fd,
                PidRef *ret) {

        परत namespace_fork_full(outer_name, inner_name, /* except_fds= */ रिक्त, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, मूळ_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(पूर्णांक oa) {
        परत oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

पूर्णांक set_oom_score_adjust(पूर्णांक मूल्य) {
        अक्षर t[DECIMAL_STR_MAX(पूर्णांक)];

        जर (!oom_score_adjust_is_valid(मूल्य))
                परत -EINVAL;

        xsprintf(t, "%i", मूल्य);

        परत write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

पूर्णांक get_oom_score_adjust(पूर्णांक *ret) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, रिक्त);
        जर (r < 0)
                परत r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        जर (r < 0)
                परत r;

        जर (!oom_score_adjust_is_valid(a))
                परत -ENODATA;

        जर (ret)
                *ret = a;

        परत 0;
}

स्थिर पूर्णांक rlimit_to_nice(rlim_t limit) {
        जर (limit <= 1)
                परत PRIO_MAX-1; /* i.e. 19 */

        जर (limit >= -PRIO_MIN + PRIO_MAX)
                परत PRIO_MIN; /* i.e. -20 */

        परत PRIO_MAX - (पूर्णांक) limit;
}

पूर्णांक setpriority_closest(पूर्णांक priority) {
        संरचना rlimit highest;
        पूर्णांक r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        जर (r >= 0)
                परत 1;
        जर (!ERRNO_IS_NEG_PRIVILEGE(r))
                परत r;

        त्रुटि_सं = 0;
        current = getpriority(PRIO_PROCESS, 0);
        जर (त्रुटि_सं != 0)
                परत -त्रुटि_सं;

        जर (priority == current)
                परत 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        जर (priority > current)
                परत r;

        जर (getrlimit(RLIMIT_NICE, &highest) < 0)
                परत -त्रुटि_सं;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        जर (current > limit)
                जर (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        परत -त्रुटि_सं;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        परत 0;
}

_noreturn_ शून्य freeze(शून्य) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (शून्य) close_all_fds_without_malloc(रिक्त, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        साठी (;;) {
                siginfo_t si = {};

                जर (waitid(P_ALL, 0, &si, WEXITED) < 0 && त्रुटि_सं != EINTR)
                        थांबा;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        साठी (;;)
                छोड़ो();
}

पूर्णांक get_process_threads(pid_t pid) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक n, r;

        जर (pid < 0)
                परत -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        r = safe_atoi(t, &n);
        जर (r < 0)
                परत r;
        जर (n < 0)
                परत -EINVAL;

        परत n;
}

पूर्णांक is_reaper_process(शून्य) {
        पूर्णांक b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        जर (getpid_cached() == 1)
                परत true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (अचिन्हित दीर्घ) &b, 0, 0, 0);
        जर (r < 0)
                परत r;

        परत b != 0;
}

पूर्णांक make_reaper_process(bool b) {
        पूर्णांक r;

        जर (getpid_cached() == 1) {

                जर (!b)
                        परत -EINVAL;

                परत 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        जर (r < 0)
                परत r;

        परत 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, रिक्त);

पूर्णांक posix_spawn_wrapper(
                स्थिरांक अक्षर *path,
                अक्षर * स्थिरांक *argv,
                अक्षर * स्थिरांक *envp,
                स्थिरांक अक्षर *cgroup,
                PidRef *ret_pidref) {

        लघु flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        पूर्णांक r;

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
        जर (r != 0)
                परत -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        स्थिर bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ पूर्णांक cgroup_fd = -EBADF;

        जर (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ अक्षर *resolved_cgroup = रिक्त;

                r = cg_get_path(cgroup, /* suffix= */ रिक्त, &resolved_cgroup);
                जर (r < 0)
                        परत r;

                cgroup_fd = खोलो(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                जर (cgroup_fd < 0)
                        परत -त्रुटि_सं;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                जर (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                अन्यथा जर (r != ENOSYS)
                        परत -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        जर (r != 0)
                परत -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        जर (r != 0)
                परत -r;

        _cleanup_close_ पूर्णांक pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, रिक्त, &attr, argv, envp);
        जर (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                परत -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        जर ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                जर (r != 0)
                        परत -r;

                r = pidfd_spawn(&pidfd, path, रिक्त, &attr, argv, envp);
        }
        जर (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                जर (r < 0)
                        परत r;

                परत FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        जर (!ERRNO_IS_NOT_SUPPORTED(r))
                परत -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, रिक्त, &attr, argv, envp);
        जर (r != 0)
                परत -r;

        r = pidref_set_pid(ret_pidref, pid);
        जर (r < 0)
                परत r;

        परत 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

पूर्णांक proc_dir_open(सूची **ret) {
        सूची *d;

        प्रतिज्ञा(ret);

        d = सूची_खोलो("/proc");
        जर (!d)
                परत -त्रुटि_सं;

        *ret = d;
        परत 0;
}

पूर्णांक proc_dir_read(सूची *d, pid_t *ret) {
        प्रतिज्ञा(d);

        साठी (;;) {
                संरचना dirent *de;

                त्रुटि_सं = 0;
                de = readdir_no_dot(d);
                जर (!de) {
                        जर (त्रुटि_सं != 0)
                                परत -त्रुटि_सं;

                        थांबा;
                }

                जर (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        सुरूठेवा;

                जर (parse_pid(de->d_name, ret) >= 0)
                        परत 1;
        }

        जर (ret)
                *ret = 0;
        परत 0;
}

पूर्णांक proc_dir_read_pidref(सूची *d, PidRef *ret) {
        पूर्णांक r;

        प्रतिज्ञा(d);

        साठी (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                r = pidref_set_pid(ret, pid);
                जर (r == -ESRCH) /* gone by now? skip it */
                        सुरूठेवा;
                जर (r < 0)
                        परत r;

                परत 1;
        }

        जर (ret)
                *ret = PIDREF_NULL;
        परत 0;
}

पूर्णांक safe_mlockall(पूर्णांक flags) {
        पूर्णांक r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        जर (r == 0)
                परत log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        जर (mlockall(flags) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        परत 0;
}

स्थिर स्थिरांक अक्षर *स्थिरांक sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, पूर्णांक);

स्थिर स्थिरांक अक्षर* स्थिरांक sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, पूर्णांक, पूर्णांक_उच्च);

_noreturn_ शून्य report_errno_and_exit(पूर्णांक errno_fd, पूर्णांक error) {
        पूर्णांक r;

        जर (error >= 0)
                _exit(निकास_सफल);

        प्रतिज्ञा(errno_fd >= 0);

        r = loop_write(errno_fd, &error, आकार(error));
        जर (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(निकास_त्रुटि);
}

पूर्णांक read_errno(पूर्णांक errno_fd) {
        पूर्णांक r;

        प्रतिज्ञा(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, आकार(r), /* do_poll= */ false);
        जर (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                परत -EIO;
        }
        जर (n == 0) /* the process exited without reporting an error, assuming success */
                परत 0;
        जर (n != आकार(r))
                परत log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        जर (r == 0)
                परत 0;
        जर (r < 0) /* child process reported an error, return it */
                परत log_debug_errno(r, "Child process failed with errno: %m");

        परत log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

पूर्णांक prctl_safe(पूर्णांक op, अचिन्हित दीर्घ arg2, अचिन्हित दीर्घ arg3, अचिन्हित दीर्घ arg4, अचिन्हित दीर्घ arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        परत RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

पूर्णांक proc_set_comm(स्थिरांक अक्षर *comm) {
        परत prctl_safe(PR_SET_NAME, (अचिन्हित दीर्घ) comm, 0, 0, 0);
}

पूर्णांक proc_set_nnp(शून्य) {
        परत prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(शून्य) {
        पूर्णांक r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        जर (r < 0)
                परत NSEC_INFINITY;

        परत r;
}
