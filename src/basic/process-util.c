/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/oom.h>
#समावेश <pthread.h>
#समावेश <spawn.h>
#समावेश <stdio.h>
#समावेश <sys/mman.h>
#समावेश <sys/mount.h>
#समावेश <sys/personality.h>
#समावेश <sys/prctl.h>
#समावेश <sys/wait.h>
#समावेश <syslog.h>
#समावेश <threads.h>
#समावेश <unistd.h>
#अगर HAVE_VALGRIND_VALGRIND_H
#समावेश <valgrind/valgrind.h>
#पूर्ण_अगर

#समावेश "sd-messages.h"

#समावेश "alloc-util.h"
#समावेश "architecture.h"
#समावेश "argv-util.h"
#समावेश "bitfield.h"
#समावेश "capability-util.h"
#समावेश "cgroup-util.h"
#समावेश "dirent-util.h"
#समावेश "dlfcn-util.h"
#समावेश "errno-util.h"
#समावेश "escape.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "io-util.h"
#समावेश "iovec-util.h"
#समावेश "locale-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "mountpoint-util.h"
#समावेश "namespace-util.h"
#समावेश "nulstr-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "pidfd-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "raw-clone.h"
#समावेश "rlimit-util.h"
#समावेश "signal-util.h"
#समावेश "socket-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "time-util.h"
#समावेश "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#घोषणा COMM_MAX_LEN 128

अटल पूर्णांक get_process_state(pid_t pid) {
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        स्थिर अक्षर *p;
        अक्षर state;
        पूर्णांक r;

        निश्चित(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        अगर (pid == 0 || pid == getpid_cached())
                वापस (अचिन्हित अक्षर) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_पंक्ति_file(p, &पंक्ति);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        p = म_खोजप(पंक्ति, ')');
        अगर (!p)
                वापस -EIO;

        p++;

        अगर (माला_पूछो(p, " %c", &state) != 1)
                वापस -EIO;

        वापस (अचिन्हित अक्षर) state;
}

पूर्णांक pid_get_comm(pid_t pid, अक्षर **ret) {
        _cleanup_free_ अक्षर *escaped = शून्य, *comm = शून्य;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret);

        अगर (pid == 0 || pid == getpid_cached()) {
                comm = new0(अक्षर, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                अगर (!comm)
                        वापस -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (अचिन्हित दीर्घ) comm, 0, 0, 0);
                अगर (r < 0)
                        वापस r;
        } अन्यथा {
                स्थिर अक्षर *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_पंक्ति_file(p, &comm);
                अगर (r == -ENOENT)
                        वापस -ESRCH;
                अगर (r < 0)
                        वापस r;
        }

        escaped = new(अक्षर, COMM_MAX_LEN);
        अगर (!escaped)
                वापस -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        वापस 0;
}

पूर्णांक pidref_get_comm(स्थिर PidRef *pid, अक्षर **ret) {
        _cleanup_free_ अक्षर *comm = शून्य;
        पूर्णांक r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pid);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = TAKE_PTR(comm);
        वापस 0;
}

अटल पूर्णांक pid_get_cmdline_nulstr(
                pid_t pid,
                माप_प्रकार max_आकार,
                ProcessCmdlineFlags flags,
                अक्षर **ret,
                माप_प्रकार *ret_आकार) {

        _cleanup_free_ अक्षर *t = शून्य;
        स्थिर अक्षर *p;
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
        r = read_virtual_file(p, max_आकार, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        अगर (k == 0) {
                अगर (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        वापस -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ अक्षर *comm = शून्य;

                r = pid_get_comm(pid, &comm);
                अगर (r < 0)
                        वापस r;

                मुक्त(t);
                t = strjoin("[", comm, "]");
                अगर (!t)
                        वापस -ENOMEM;

                k = म_माप(t);
                r = k <= max_आकार;
                अगर (r == 0) /* truncation */
                        t[max_आकार] = '\0';
        }

        अगर (ret)
                *ret = TAKE_PTR(t);
        अगर (ret_आकार)
                *ret_आकार = k;

        वापस r;
}

पूर्णांक pid_get_cmdline(pid_t pid, माप_प्रकार max_columns, ProcessCmdlineFlags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *t = शून्य;
        माप_प्रकार k;
        अक्षर *ans;

        निश्चित(pid >= 0);
        निश्चित(ret);

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
        अगर (full < 0)
                वापस full;

        अगर (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                निश्चित(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ अक्षर **args = शून्य;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                अगर (!args)
                        वापस -ENOMEM;

                ans = quote_command_पंक्ति(args, shflags);
                अगर (!ans)
                        वापस -ENOMEM;
        } अन्यथा {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                क्रम (माप_प्रकार i = 0; i < k - 1; i++)
                        अगर (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                अगर (!ans)
                        वापस -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        वापस 0;
}

पूर्णांक pidref_get_cmdline(स्थिर PidRef *pid, माप_प्रकार max_columns, ProcessCmdlineFlags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pid);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, अक्षर ***ret) {
        _cleanup_free_ अक्षर *t = शून्य;
        अक्षर **args;
        माप_प्रकार k;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        निश्चित(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        अगर (r < 0)
                वापस r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        अगर (!args)
                वापस -ENOMEM;

        *ret = args;
        वापस 0;
}

पूर्णांक pidref_get_cmdline_strv(स्थिर PidRef *pid, ProcessCmdlineFlags flags, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **args = शून्य;
        पूर्णांक r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pid);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = TAKE_PTR(args);

        वापस 0;
}

पूर्णांक procfs_get_stat_flags(स्थिर अक्षर *path, अचिन्हित दीर्घ दीर्घ *ret) {
        पूर्णांक r;

        निश्चित(path);
        निश्चित(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        r = read_one_पंक्ति_file(path, &पंक्ति);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        /* Skip past the comm field */
        अक्षर *q = म_खोजप(पंक्ति, ')');
        अगर (!q)
                वापस -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        क्रम (माप_प्रकार i = 0; i < 6; i++) {
                माप_प्रकार l = म_अखोज(q, WHITESPACE);
                अगर (l < 1)
                        वापस -EINVAL;
                q += l;

                l = म_खोज(q, WHITESPACE);
                अगर (l < 1)
                        वापस -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        माप_प्रकार l = म_अखोज(q, WHITESPACE);
        अगर (l < 1)
                वापस -EINVAL;
        q += l;

        /* Truncate the rest */
        l = म_खोज(q, WHITESPACE);
        अगर (l < 1)
                वापस -EINVAL;
        q[l] = 0;

        वापस safe_atollu(q, ret);
}

पूर्णांक pid_is_kernel_thread(pid_t pid) {
        पूर्णांक r;

        अगर (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                वापस 0;
        अगर (!pid_is_valid(pid))
                वापस -EINVAL;

        स्थिर अक्षर *p = procfs_file_alloca(pid, "stat");

        अचिन्हित दीर्घ दीर्घ flags;
        r = procfs_get_stat_flags(p, &flags);
        अगर (r < 0)
                वापस r;

        वापस !!(flags & PF_KTHREAD);
}

पूर्णांक pidref_is_kernel_thread(स्थिर PidRef *pid) {
        पूर्णांक परिणाम, r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        परिणाम = pid_is_kernel_thread(pid->pid);
        अगर (परिणाम < 0)
                वापस परिणाम;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        अगर (r < 0)
                वापस r;

        वापस परिणाम;
}

अटल पूर्णांक get_process_link_contents(pid_t pid, स्थिर अक्षर *proc_file, अक्षर **ret) {
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        वापस (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

पूर्णांक get_process_exe(pid_t pid, अक्षर **ret) {
        अक्षर *d;
        पूर्णांक r;

        निश्चित(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        अगर (r < 0)
                वापस r;

        अगर (ret) {
                d = endswith(*ret, " (deleted)");
                अगर (d)
                        *d = '\0';
        }

        वापस 0;
}

पूर्णांक pid_get_uid(pid_t pid, uid_t *ret) {
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret);

        अगर (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                वापस 0;
        }

        _cleanup_free_ अक्षर *v = शून्य;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        वापस parse_uid(v, ret);
}

पूर्णांक pidref_get_uid(स्थिर PidRef *pid, uid_t *ret) {
        पूर्णांक r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        अगर (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                अगर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        वापस r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pid);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = uid;
        वापस 0;
}

पूर्णांक get_process_gid(pid_t pid, gid_t *ret) {
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret);

        अगर (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                वापस 0;
        }

        _cleanup_free_ अक्षर *v = शून्य;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        वापस parse_gid(v, ret);
}

पूर्णांक get_process_cwd(pid_t pid, अक्षर **ret) {
        निश्चित(pid >= 0);

        अगर (pid == 0 || pid == getpid_cached())
                वापस safe_getcwd(ret);

        वापस get_process_link_contents(pid, "cwd", ret);
}

पूर्णांक get_process_मूल(pid_t pid, अक्षर **ret) {
        निश्चित(pid >= 0);
        वापस get_process_link_contents(pid, "root", ret);
}

#घोषणा ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

पूर्णांक get_process_environ(pid_t pid, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_free_ अक्षर *outcome = शून्य;
        माप_प्रकार sz = 0;
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                अक्षर c;

                अगर (sz >= ENVIRONMENT_BLOCK_MAX)
                        वापस -ENOBUFS;

                अगर (!GREEDY_REALLOC(outcome, sz + 5))
                        वापस -ENOMEM;

                r = safe_fgetc(f, &c);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                अगर (c == '\0')
                        outcome[sz++] = '\n';
                अन्यथा
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        वापस 0;
}

पूर्णांक pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        अचिन्हित दीर्घ ppid;
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(pid >= 0);

        अगर (pid == 0)
                pid = getpid_cached();
        अगर (pid == 1) /* PID 1 has no parent, shortcut this case */
                वापस -EADDRNOTAVAIL;

        अगर (pid == getpid_cached()) {
                अगर (ret)
                        *ret = getppid();
                वापस 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पंक्ति_file(p, &पंक्ति);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = म_खोजप(पंक्ति, ')');
        अगर (!p)
                वापस -EIO;
        p++;

        अगर (माला_पूछो(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                वापस -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        अगर (ppid == 0)
                वापस -EADDRNOTAVAIL;

        अगर ((pid_t) ppid < 0 || (अचिन्हित दीर्घ) (pid_t) ppid != ppid)
                वापस -दुस्फल;

        अगर (ret)
                *ret = (pid_t) ppid;

        वापस 0;
}

पूर्णांक pidref_get_ppid(स्थिर PidRef *pidref, pid_t *ret) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        अगर (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                अगर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        वापस r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : शून्य);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = ppid;
        वापस 0;
}

पूर्णांक pidref_get_ppid_as_pidref(स्थिर PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        पूर्णांक r;

        POINTER_MAY_BE_NULL(pidref);
        निश्चित(ret);

        r = pidref_get_ppid(pidref, &ppid);
        अगर (r < 0)
                वापस r;

        क्रम (अचिन्हित attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                अगर (r < 0)
                        वापस r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                अगर (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        अगर (r < 0)
                                वापस r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        अगर (parent.pid != ppid)
                                जारी;
                }

                *ret = TAKE_PIDREF(parent);
                वापस 0;
        }

        /* Give up after 16 tries */
        वापस -ENOTRECOVERABLE;
}

पूर्णांक pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पंक्ति_file(p, &पंक्ति);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = म_खोजप(पंक्ति, ')');
        अगर (!p)
                वापस -EIO;
        p++;

        अचिन्हित दीर्घ llu;

        अगर (माला_पूछो(p, " "
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
                वापस -EIO;

        अगर (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        वापस 0;
}

पूर्णांक pidref_get_start_time(स्थिर PidRef *pid, usec_t *ret) {
        usec_t t;
        पूर्णांक r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : शून्य);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pid);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = t;

        वापस 0;
}

पूर्णांक get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ अक्षर *m = शून्य;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        वापस parse_mode(m, ret);
}

अटल पूर्णांक pid_get_sigmask(pid_t pid, स्थिर अक्षर *field_name, uint64_t *ret) {
        _cleanup_free_ अक्षर *field = शून्य;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(field_name);
        निश्चित(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        वापस safe_atou64_full(field, 16, ret);
}

अटल पूर्णांक pidref_has_sigmask(स्थिर PidRef *pidref, पूर्णांक sig, स्थिर अक्षर *field_name) {
        uint64_t mask;
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;
        अगर (!SIGNAL_VALID(sig))
                वापस -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        वापस BIT_SET(mask, sig - 1);
}

पूर्णांक pidref_has_sigcgt(स्थिर PidRef *pidref, पूर्णांक sig) {
        वापस pidref_has_sigmask(pidref, sig, "SigCgt");
}

पूर्णांक pidref_has_sigblk(स्थिर PidRef *pidref, पूर्णांक sig) {
        वापस pidref_has_sigmask(pidref, sig, "SigBlk");
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
पूर्णांक pidref_wait_for_terminate_and_check(स्थिर अक्षर *name, PidRef *pidref, WaitFlags flags) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;
        अगर (pidref->pid == 1 || pidref_is_self(pidref))
                वापस -ECHILD;

        _cleanup_free_ अक्षर *बफर = शून्य;
        अगर (!name) {
                r = pidref_get_comm(pidref, &बफर);
                अगर (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                अन्यथा
                        name = बफर;
        }

        पूर्णांक prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        अगर (r < 0)
                वापस log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        अगर (status.si_code == CLD_EXITED) {
                अगर (status.si_status != निकास_सफल)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                अन्यथा
                        log_debug("'%s' succeeded.", name);

                वापस status.si_status;

        } अन्यथा अगर (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                वापस log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        वापस log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

पूर्णांक kill_and_sigcont(pid_t pid, पूर्णांक sig) {
        पूर्णांक r;

        r = RET_NERRNO(समाप्त(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        अगर (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (व्योम) समाप्त(pid, SIGCONT);

        वापस r;
}

पूर्णांक getenv_for_pid(pid_t pid, स्थिर अक्षर *field, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        स्थिर अक्षर *path;
        माप_प्रकार योग = 0;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(field);
        निश्चित(ret);

        अगर (pid == 0 || pid == getpid_cached())
                वापस strdup_to_full(ret, दो_पर्या(field));

        अगर (!pid_is_valid(pid))
                वापस -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;
                स्थिर अक्षर *match;

                अगर (योग > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        वापस -ENOBUFS;

                r = read_nul_string(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)  /* EOF */
                        अवरोध;

                योग += r;

                match = startswith(पंक्ति, field);
                अगर (match && *match == '=')
                        वापस strdup_to_full(ret, match + 1);
        }

        *ret = शून्य;
        वापस 0;
}

पूर्णांक pidref_is_my_child(PidRef *pid) {
        पूर्णांक r;

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        अगर (pid->pid == 1 || pidref_is_self(pid))
                वापस false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        अगर (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                वापस false;
        अगर (r < 0)
                वापस r;

        वापस ppid == getpid_cached();
}

पूर्णांक pid_is_my_child(pid_t pid) {

        अगर (pid == 0)
                वापस false;

        वापस pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

पूर्णांक pidref_is_unwaited(PidRef *pid) {
        पूर्णांक r;

        /* Checks whether a PID is still valid at all, including a zombie */

        अगर (!pidref_is_set(pid))
                वापस -ESRCH;

        अगर (pidref_is_remote(pid))
                वापस -EREMOTE;

        अगर (pid->pid == 1 || pidref_is_self(pid))
                वापस true;

        r = pidref_kill(pid, 0);
        अगर (r == -ESRCH)
                वापस false;
        अगर (r < 0)
                वापस r;

        वापस true;
}

पूर्णांक pid_is_unwaited(pid_t pid) {

        अगर (pid == 0)
                वापस true;

        वापस pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

पूर्णांक pid_is_alive(pid_t pid) {
        पूर्णांक r;

        /* Checks whether a PID is still valid and not a zombie */

        अगर (pid < 0)
                वापस -ESRCH;

        अगर (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                वापस true;

        अगर (pid == getpid_cached())
                वापस true;

        r = get_process_state(pid);
        अगर (r == -ESRCH)
                वापस false;
        अगर (r < 0)
                वापस r;

        वापस r != 'Z';
}

पूर्णांक pidref_is_alive(स्थिर PidRef *pidref) {
        पूर्णांक r, परिणाम;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;

        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        परिणाम = pid_is_alive(pidref->pid);
        अगर (परिणाम < 0) {
                निश्चित(परिणाम != -ESRCH);
                वापस परिणाम;
        }

        r = pidref_verify(pidref);
        अगर (r == -ESRCH)
                वापस false;
        अगर (r < 0)
                वापस r;

        वापस परिणाम;
}

पूर्णांक pidref_from_same_मूल_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        पूर्णांक r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        अगर (!a || !b) {
                r = pidref_set_self(&self);
                अगर (r < 0)
                        वापस r;
                अगर (!a)
                        a = &self;
                अगर (!b)
                        b = &self;
        }

        अगर (!pidref_is_set(a) || !pidref_is_set(b))
                वापस -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        अगर (pidref_is_remote(a) && pidref_is_remote(b))
                वापस -EREMOTE;
        अगर (pidref_is_remote(a) || pidref_is_remote(b))
                वापस false;

        अगर (pidref_equal(a, b))
                वापस true;

        स्थिर अक्षर *roota = procfs_file_alloca(a->pid, "root");
        स्थिर अक्षर *rootb = procfs_file_alloca(b->pid, "root");

        पूर्णांक परिणाम = inode_same(roota, rootb, 0);
        अगर (परिणाम == -ENOENT)
                वापस proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        अगर (परिणाम < 0)
                वापस परिणाम;

        r = pidref_verify(a);
        अगर (r < 0)
                वापस r;
        r = pidref_verify(b);
        अगर (r < 0)
                वापस r;

        वापस परिणाम;
}

bool is_main_thread(व्योम) {
        अटल thread_local पूर्णांक cached = -1;

        अगर (cached < 0)
                cached = getpid_cached() == gettid();

        वापस cached;
}

अचिन्हित दीर्घ personality_from_string(स्थिर अक्षर *s) {
        Architecture architecture;

        अगर (!s)
                वापस PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        अगर (architecture < 0)
                वापस PERSONALITY_INVALID;

        अगर (architecture == native_architecture())
                वापस PER_LINUX;
#अगर_घोषित ARCHITECTURE_SECONDARY
        अगर (architecture == ARCHITECTURE_SECONDARY)
                वापस PER_LINUX32;
#पूर्ण_अगर

        वापस PERSONALITY_INVALID;
}

स्थिर अक्षर* personality_to_string(अचिन्हित दीर्घ p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        अगर (p == PER_LINUX)
                architecture = native_architecture();
#अगर_घोषित ARCHITECTURE_SECONDARY
        अन्यथा अगर (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#पूर्ण_अगर

        अगर (architecture < 0)
                वापस शून्य;

        वापस architecture_to_string(architecture);
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
        अगर (ret < 0) {
                अगर (त्रुटि_सं != 0)
                        वापस -त्रुटि_सं;

                त्रुटि_सं = -ret;
        }

        वापस ret;
}

पूर्णांक opinionated_personality(अचिन्हित दीर्घ *ret) {
        पूर्णांक current;

        निश्चित(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        अगर (current < 0)
                वापस current;

        अगर (((अचिन्हित दीर्घ) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        अन्यथा
                *ret = PER_LINUX;

        वापस 0;
}

व्योम valgrind_summary_hack(व्योम) {
#अगर HAVE_VALGRIND_VALGRIND_H
        अगर (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                अगर (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, त्रुटि_सं,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                अन्यथा अगर (pid == 0)
                        निकास(निकास_सफल);
                अन्यथा {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (व्योम) pidref_set_pid(&pidref, pid);
                        (व्योम) pidref_wait_for_terminate(&pidref, शून्य);
                }
        }
#पूर्ण_अगर
}

पूर्णांक pid_compare_func(स्थिर pid_t *a, स्थिर pid_t *b) {
        /* Suitable for usage in qsort() */
        निश्चित(a);
        निश्चित(b);

        वापस CMP(*a, *b);
}

bool nice_is_valid(पूर्णांक n) {
        वापस n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(पूर्णांक policy) {
        वापस IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(पूर्णांक policy) {
        वापस sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
पूर्णांक sched_get_priority_min_safe(पूर्णांक policy) {
        पूर्णांक r;

        r = sched_get_priority_min(policy);
        अगर (r >= 0)
                वापस r;

        /* Fallback priority */
        वापस 0;
}

पूर्णांक sched_get_priority_max_safe(पूर्णांक policy) {
        पूर्णांक r;

        r = sched_get_priority_max(policy);
        अगर (r >= 0)
                वापस r;

        वापस 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#घोषणा CACHED_PID_UNSET ((pid_t) 0)
#घोषणा CACHED_PID_BUSY ((pid_t) -1)

अटल pid_t cached_pid = CACHED_PID_UNSET;

व्योम reset_cached_pid(व्योम) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(व्योम) {
        अटल bool installed = false;
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
        अगर (current_मूल्य > 0)
                वापस current_मूल्य;

        current_मूल्य = CACHED_PID_UNSET;
        (व्योम) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_मूल्य,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        चयन (current_मूल्य) {

        हाल CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                अगर (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        अगर (pthread_atfork(शून्य, शून्य, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                वापस new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                वापस new_pid;
        }

        हाल CACHED_PID_BUSY: /* Somebody else is currently initializing */
                वापस getpid();

        शेष: /* Properly initialized */
                वापस current_मूल्य;
        }
}

पूर्णांक must_be_मूल(व्योम) {

        अगर (geteuid() == 0)
                वापस 0;

        वापस log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ढेर(पूर्णांक (*fn)(व्योम *), पूर्णांक flags, व्योम *userdata) {
        माप_प्रकार ps;
        pid_t pid;
        व्योम *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        निश्चित((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
                         CLONE_CHILD_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = page_आकार();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (व्योम*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#अगर HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#अन्यथा
        pid = __clone2(fn, mystack, ps, flags, userdata);
#पूर्ण_अगर
        अगर (pid < 0)
                वापस -त्रुटि_सं;

        वापस pid;
}

अटल पूर्णांक fork_flags_to_signal(ForkFlags flags) {
        वापस (flags & FORK_DEATHSIG_SIGTERM) ? संक_इति :
                (flags & FORK_DEATHSIG_SIGINT) ? संक_विघ्न :
                                                 SIGKILL;
}

पूर्णांक pidref_safe_fork_full(
                स्थिर अक्षर *name,
                स्थिर पूर्णांक stdio_fds[3],
                पूर्णांक except_fds[],
                माप_प्रकार n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = शून्य;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ पूर्णांक pidref_transport_fds[2] = EBADF_PAIR;
        पूर्णांक prio, r;

        निश्चित(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        निश्चित(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        अगर (flags & FORK_FLUSH_STDIO) {
                ख_साफ(मानक_निकास);
                ख_साफ(मानक_त्रुटि); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        अगर (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } अन्यथा अगर (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        अगर (block_signals) {
                अगर (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        वापस log_full_errno(prio, त्रुटि_सं, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        अगर (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                अगर (r < 0)
                        वापस log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                अगर (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        अगर (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                वापस log_full_errno(prio, त्रुटि_सं, "Failed to allocate pidref socket: %m");

                        pid = विभाजन();
                        अगर (pid < 0)
                                वापस log_full_errno(prio, त्रुटि_सं, "Failed to fork off '%s': %m", strna(name));
                        अगर (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                अगर (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        अगर (r < 0)
                                                वापस log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        अगर (r != निकास_सफल) /* exit status > 0 should be treated as failure, too */
                                                वापस -EPROTO;

                                        पूर्णांक pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, माप(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        अगर (n < 0)
                                                वापस log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                वापस 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        अगर ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        अन्यथा
                pid = विभाजन();
        अगर (pid < 0)
                वापस log_full_errno(prio, त्रुटि_सं, "Failed to fork off '%s': %m", strna(name));
        अगर (pid > 0) {

                /* If we are in the intermediary process, exit now */
                अगर (intermediary) {
                        अगर (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                अगर (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _निकास(निकास_त्रुटि);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, माप(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                अगर (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _निकास(निकास_त्रुटि);
                                }
                        }

                        _निकास(निकास_सफल);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                अगर (flags & FORK_WAIT) {
                        अगर (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (व्योम) sigprocmask(SIG_SETMASK, &ss, शून्य);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        अगर (r < 0)
                                वापस r;
                        अगर (r != निकास_सफल) /* exit status > 0 should be treated as failure, too */
                                वापस -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        अगर (ret)
                                *ret = PIDREF_NULL;

                        वापस 1;
                }

                अगर (ret) {
                        r = pidref_set_pid(ret, pid);
                        अगर (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                वापस 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = शून्य;

        अगर (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        अगर (name) {
                r = rename_process(name);
                अगर (r < 0)
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
        अगर (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        अगर (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _निकास(निकास_त्रुटि);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _निकास(निकास_त्रुटि);
                }
        } अन्यथा अगर (block_signals) { /* undo what we did above */
                अगर (sigprocmask(SIG_SETMASK, &saved_ss, शून्य) < 0) {
                        log_full_errno(prio, त्रुटि_सं, "Failed to restore signal mask: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                अगर (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                अन्यथा अगर (ppid != original_pid) {
                        पूर्णांक sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (व्योम) उठाओ(sig);
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                अगर (mount(शून्य, "/", शून्य, MS_SLAVE | MS_REC, शून्य) < 0) {
                        log_full_errno(prio, त्रुटि_सं, "Failed to remount root directory as MS_SLAVE: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                निश्चित(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (flags & FORK_REARRANGE_STDIO) {
                अगर (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        अगर (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _निकास(निकास_त्रुटि);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } अन्यथा {
                        r = make_null_stdio();
                        अगर (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _निकास(निकास_त्रुटि);
                        }
                }
        } अन्यथा अगर (flags & FORK_STDOUT_TO_STDERR) {
                अगर (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, त्रुटि_सं, "Failed to connect stdout to stderr: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                निश्चित(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        अगर (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        अगर (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        अगर (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        अगर (ret) {
                r = pidref_set_self(ret);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _निकास(निकास_त्रुटि);
                }
        }

        वापस 0;
}

पूर्णांक namespace_fork_full(
                स्थिर अक्षर *outer_name,
                स्थिर अक्षर *inner_name,
                पूर्णांक except_fds[],
                माप_प्रकार n_except_fds,
                ForkFlags flags,
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूल_fd,
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
        निश्चित((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        निश्चित((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        निश्चित(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(माप(पूर्णांक) <= PIPE_BUF);
        अगर (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                वापस log_full_errno(prio, त्रुटि_सं, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ शून्य, /* except_fds= */ शून्य, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        अगर (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                पूर्णांक k = read_errno(errno_pipe_fd[0]);
                अगर (k < 0 && k != -EIO)
                        वापस k;
        }
        अगर (r < 0)
                वापस r;
        अगर (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, मूल_fd, namespace_flags);
                अगर (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                शून्य,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                अगर (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                अगर (r == 0) {
                        /* Child */

                        अगर (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } अन्यथा {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        अगर (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        वापस 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (व्योम) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                अगर (r < 0)
                        _निकास(निकास_त्रुटि);

                _निकास(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        अगर (r < 0)
                वापस r; /* the child logs about failures on its own, no need to duplicate here */

        अगर (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        अन्यथा
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        वापस 1;
}

पूर्णांक namespace_fork(
                स्थिर अक्षर *outer_name,
                स्थिर अक्षर *inner_name,
                ForkFlags flags,
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूल_fd,
                PidRef *ret) {

        वापस namespace_fork_full(outer_name, inner_name, /* except_fds= */ शून्य, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, मूल_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(पूर्णांक oa) {
        वापस oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

पूर्णांक set_oom_score_adjust(पूर्णांक मूल्य) {
        अक्षर t[DECIMAL_STR_MAX(पूर्णांक)];

        अगर (!oom_score_adjust_is_valid(मूल्य))
                वापस -EINVAL;

        xsprintf(t, "%i", मूल्य);

        वापस write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_बफर);
}

पूर्णांक get_oom_score_adjust(पूर्णांक *ret) {
        _cleanup_free_ अक्षर *t = शून्य;
        पूर्णांक r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, शून्य);
        अगर (r < 0)
                वापस r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        अगर (r < 0)
                वापस r;

        अगर (!oom_score_adjust_is_valid(a))
                वापस -ENODATA;

        अगर (ret)
                *ret = a;

        वापस 0;
}

अटल पूर्णांक rlimit_to_nice(rlim_t limit) {
        अगर (limit <= 1)
                वापस PRIO_MAX-1; /* i.e. 19 */

        अगर (limit >= -PRIO_MIN + PRIO_MAX)
                वापस PRIO_MIN; /* i.e. -20 */

        वापस PRIO_MAX - (पूर्णांक) limit;
}

पूर्णांक setpriority_closest(पूर्णांक priority) {
        काष्ठा rlimit highest;
        पूर्णांक r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        अगर (r >= 0)
                वापस 1;
        अगर (!ERRNO_IS_NEG_PRIVILEGE(r))
                वापस r;

        त्रुटि_सं = 0;
        current = getpriority(PRIO_PROCESS, 0);
        अगर (त्रुटि_सं != 0)
                वापस -त्रुटि_सं;

        अगर (priority == current)
                वापस 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        अगर (priority > current)
                वापस r;

        अगर (getrlimit(RLIMIT_NICE, &highest) < 0)
                वापस -त्रुटि_सं;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        अगर (current > limit)
                अगर (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        वापस -त्रुटि_सं;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        वापस 0;
}

_noreturn_ व्योम freeze(व्योम) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (व्योम) close_all_fds_without_malloc(शून्य, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        क्रम (;;) {
                siginfo_t si = {};

                अगर (waitid(P_ALL, 0, &si, WEXITED) < 0 && त्रुटि_सं != EINTR)
                        अवरोध;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        क्रम (;;)
                छोड़ो();
}

पूर्णांक get_process_threads(pid_t pid) {
        _cleanup_free_ अक्षर *t = शून्य;
        पूर्णांक n, r;

        अगर (pid < 0)
                वापस -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        r = safe_atoi(t, &n);
        अगर (r < 0)
                वापस r;
        अगर (n < 0)
                वापस -EINVAL;

        वापस n;
}

पूर्णांक is_reaper_process(व्योम) {
        पूर्णांक b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        अगर (getpid_cached() == 1)
                वापस true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (अचिन्हित दीर्घ) &b, 0, 0, 0);
        अगर (r < 0)
                वापस r;

        वापस b != 0;
}

पूर्णांक make_reaper_process(bool b) {
        पूर्णांक r;

        अगर (getpid_cached() == 1) {

                अगर (!b)
                        वापस -EINVAL;

                वापस 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        अगर (r < 0)
                वापस r;

        वापस 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, शून्य);

पूर्णांक posix_spawn_wrapper(
                स्थिर अक्षर *path,
                अक्षर * स्थिर *argv,
                अक्षर * स्थिर *envp,
                स्थिर अक्षर *cgroup,
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

        निश्चित(path);
        निश्चित(argv);
        निश्चित(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        अगर (r != 0)
                वापस -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        अटल bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ पूर्णांक cgroup_fd = -EBADF;

        अगर (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ अक्षर *resolved_cgroup = शून्य;

                r = cg_get_path(cgroup, /* suffix= */ शून्य, &resolved_cgroup);
                अगर (r < 0)
                        वापस r;

                cgroup_fd = खोलो(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                अगर (cgroup_fd < 0)
                        वापस -त्रुटि_सं;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                अगर (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                अन्यथा अगर (r != ENOSYS)
                        वापस -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        अगर (r != 0)
                वापस -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        अगर (r != 0)
                वापस -r;

        _cleanup_close_ पूर्णांक pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, शून्य, &attr, argv, envp);
        अगर (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                वापस -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        अगर ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                अगर (r != 0)
                        वापस -r;

                r = pidfd_spawn(&pidfd, path, शून्य, &attr, argv, envp);
        }
        अगर (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                अगर (r < 0)
                        वापस r;

                वापस FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        अगर (!ERRNO_IS_NOT_SUPPORTED(r))
                वापस -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, शून्य, &attr, argv, envp);
        अगर (r != 0)
                वापस -r;

        r = pidref_set_pid(ret_pidref, pid);
        अगर (r < 0)
                वापस r;

        वापस 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

पूर्णांक proc_dir_open(सूची **ret) {
        सूची *d;

        निश्चित(ret);

        d = सूची_खोलो("/proc");
        अगर (!d)
                वापस -त्रुटि_सं;

        *ret = d;
        वापस 0;
}

पूर्णांक proc_dir_read(सूची *d, pid_t *ret) {
        निश्चित(d);

        क्रम (;;) {
                काष्ठा dirent *de;

                त्रुटि_सं = 0;
                de = readdir_no_dot(d);
                अगर (!de) {
                        अगर (त्रुटि_सं != 0)
                                वापस -त्रुटि_सं;

                        अवरोध;
                }

                अगर (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        जारी;

                अगर (parse_pid(de->d_name, ret) >= 0)
                        वापस 1;
        }

        अगर (ret)
                *ret = 0;
        वापस 0;
}

पूर्णांक proc_dir_read_pidref(सूची *d, PidRef *ret) {
        पूर्णांक r;

        निश्चित(d);

        क्रम (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                r = pidref_set_pid(ret, pid);
                अगर (r == -ESRCH) /* gone by now? skip it */
                        जारी;
                अगर (r < 0)
                        वापस r;

                वापस 1;
        }

        अगर (ret)
                *ret = PIDREF_NULL;
        वापस 0;
}

पूर्णांक safe_mlockall(पूर्णांक flags) {
        पूर्णांक r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        अगर (r == 0)
                वापस log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        अगर (mlockall(flags) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        वापस 0;
}

अटल स्थिर अक्षर *स्थिर sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, पूर्णांक);

अटल स्थिर अक्षर* स्थिर sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, पूर्णांक, पूर्णांक_उच्च);

_noreturn_ व्योम report_errno_and_exit(पूर्णांक errno_fd, पूर्णांक error) {
        पूर्णांक r;

        अगर (error >= 0)
                _निकास(निकास_सफल);

        निश्चित(errno_fd >= 0);

        r = loop_write(errno_fd, &error, माप(error));
        अगर (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _निकास(निकास_त्रुटि);
}

पूर्णांक read_errno(पूर्णांक errno_fd) {
        पूर्णांक r;

        निश्चित(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, माप(r), /* do_poll= */ false);
        अगर (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                वापस -EIO;
        }
        अगर (n == 0) /* the process exited without reporting an error, assuming success */
                वापस 0;
        अगर (n != माप(r))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        अगर (r == 0)
                वापस 0;
        अगर (r < 0) /* child process reported an error, return it */
                वापस log_debug_errno(r, "Child process failed with errno: %m");

        वापस log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

पूर्णांक prctl_safe(पूर्णांक op, अचिन्हित दीर्घ arg2, अचिन्हित दीर्घ arg3, अचिन्हित दीर्घ arg4, अचिन्हित दीर्घ arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        वापस RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

पूर्णांक proc_set_comm(स्थिर अक्षर *comm) {
        वापस prctl_safe(PR_SET_NAME, (अचिन्हित दीर्घ) comm, 0, 0, 0);
}

पूर्णांक proc_set_nnp(व्योम) {
        वापस prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(व्योम) {
        पूर्णांक r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        अगर (r < 0)
                वापस NSEC_INFINITY;

        वापस r;
}
