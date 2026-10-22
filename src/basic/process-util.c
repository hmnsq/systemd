/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/oom.h>
#அந்தர்பாவ <pthread.h>
#அந்தர்பாவ <spawn.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <sys/mman.h>
#அந்தர்பாவ <sys/mount.h>
#அந்தர்பாவ <sys/personality.h>
#அந்தர்பாவ <sys/prctl.h>
#அந்தர்பாவ <sys/wait.h>
#அந்தர்பாவ <syslog.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>
#எனில் HAVE_VALGRIND_VALGRIND_H
#அந்தர்பாவ <valgrind/valgrind.h>
#endif

#அந்தர்பாவ "sd-messages.h"

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "architecture.h"
#அந்தர்பாவ "argv-util.h"
#அந்தர்பாவ "bitfield.h"
#அந்தர்பாவ "capability-util.h"
#அந்தர்பாவ "cgroup-util.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "dlfcn-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "locale-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "mountpoint-util.h"
#அந்தர்பாவ "namespace-util.h"
#அந்தர்பாவ "nulstr-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidfd-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "raw-clone.h"
#அந்தர்பாவ "rlimit-util.h"
#அந்தர்பாவ "signal-util.h"
#அந்தர்பாவ "socket-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

நிலையான முழு get_process_state(pid_t pid) {
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        மாறிலி எழுத்து *p;
        எழுத்து state;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        எனில் (pid == 0 || pid == getpid_cached())
                திரும்பு (unsigned எழுத்து) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_வரி_file(p, &வரி);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        p = strrchr(வரி, ')');
        எனில் (!p)
                திரும்பு -EIO;

        p++;

        எனில் (sscanf(p, " %c", &state) != 1)
                திரும்பு -EIO;

        திரும்பு (unsigned எழுத்து) state;
}

முழு pid_get_comm(pid_t pid, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *escaped = வெற்று, *comm = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (pid == 0 || pid == getpid_cached()) {
                comm = new0(எழுத்து, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                எனில் (!comm)
                        திரும்பு -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                எனில் (r < 0)
                        திரும்பு r;
        } இல்லையெனில் {
                மாறிலி எழுத்து *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_வரி_file(p, &comm);
                எனில் (r == -ENOENT)
                        திரும்பு -ESRCH;
                எனில் (r < 0)
                        திரும்பு r;
        }

        escaped = new(எழுத்து, COMM_MAX_LEN);
        எனில் (!escaped)
                திரும்பு -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        திரும்பு 0;
}

முழு pidref_get_comm(மாறிலி PidRef *pid, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *comm = வெற்று;
        முழு r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = TAKE_PTR(comm);
        திரும்பு 0;
}

நிலையான முழு pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                எழுத்து **ret,
                size_t *ret_size) {

        _cleanup_free_ எழுத்து *t = வெற்று;
        மாறிலி எழுத்து *p;
        size_t k;
        முழு r;

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
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        எனில் (k == 0) {
                எனில் (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        திரும்பு -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ எழுத்து *comm = வெற்று;

                r = pid_get_comm(pid, &comm);
                எனில் (r < 0)
                        திரும்பு r;

                விடுவி(t);
                t = strjoin("[", comm, "]");
                எனில் (!t)
                        திரும்பு -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                எனில் (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        எனில் (ret)
                *ret = TAKE_PTR(t);
        எனில் (ret_size)
                *ret_size = k;

        திரும்பு r;
}

முழு pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        size_t k;
        எழுத்து *ans;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

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

        முழு full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        எனில் (full < 0)
                திரும்பு full;

        எனில் (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                ப்ரதிஜ்ஞா(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ எழுத்து **args = வெற்று;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                எனில் (!args)
                        திரும்பு -ENOMEM;

                ans = quote_command_வரி(args, shflags);
                எனில் (!ans)
                        திரும்பு -ENOMEM;
        } இல்லையெனில் {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                க்கு (size_t i = 0; i < k - 1; i++)
                        எனில் (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                எனில் (!ans)
                        திரும்பு -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        திரும்பு 0;
}

முழு pidref_get_cmdline(மாறிலி PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, எழுத்து ***ret) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        எழுத்து **args;
        size_t k;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        ப்ரதிஜ்ஞா(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        எனில் (r < 0)
                திரும்பு r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        எனில் (!args)
                திரும்பு -ENOMEM;

        *ret = args;
        திரும்பு 0;
}

முழு pidref_get_cmdline_strv(மாறிலி PidRef *pid, ProcessCmdlineFlags flags, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **args = வெற்று;
        முழு r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = TAKE_PTR(args);

        திரும்பு 0;
}

முழு procfs_get_stat_flags(மாறிலி எழுத்து *path, unsigned long long *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ எழுத்து *வரி = வெற்று;
        r = read_one_வரி_file(path, &வரி);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        /* Skip past the comm field */
        எழுத்து *q = strrchr(வரி, ')');
        எனில் (!q)
                திரும்பு -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        க்கு (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                எனில் (l < 1)
                        திரும்பு -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                எனில் (l < 1)
                        திரும்பு -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        எனில் (l < 1)
                திரும்பு -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        எனில் (l < 1)
                திரும்பு -EINVAL;
        q[l] = 0;

        திரும்பு safe_atollu(q, ret);
}

முழு pid_is_kernel_thread(pid_t pid) {
        முழு r;

        எனில் (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                திரும்பு 0;
        எனில் (!pid_is_valid(pid))
                திரும்பு -EINVAL;

        மாறிலி எழுத்து *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு !!(flags & PF_KTHREAD);
}

முழு pidref_is_kernel_thread(மாறிலி PidRef *pid) {
        முழு result, r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        எனில் (result < 0)
                திரும்பு result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு result;
}

நிலையான முழு get_process_link_contents(pid_t pid, மாறிலி எழுத்து *proc_file, எழுத்து **ret) {
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        திரும்பு (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

முழு get_process_exe(pid_t pid, எழுத்து **ret) {
        எழுத்து *d;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret) {
                d = endswith(*ret, " (deleted)");
                எனில் (d)
                        *d = '\0';
        }

        திரும்பு 0;
}

முழு pid_get_uid(pid_t pid, uid_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                திரும்பு 0;
        }

        _cleanup_free_ எழுத்து *v = வெற்று;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_uid(v, ret);
}

முழு pidref_get_uid(மாறிலி PidRef *pid, uid_t *ret) {
        முழு r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        எனில் (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                எனில் (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        திரும்பு r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = uid;
        திரும்பு 0;
}

முழு get_process_gid(pid_t pid, gid_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                திரும்பு 0;
        }

        _cleanup_free_ எழுத்து *v = வெற்று;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_gid(v, ret);
}

முழு get_process_cwd(pid_t pid, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(pid >= 0);

        எனில் (pid == 0 || pid == getpid_cached())
                திரும்பு safe_getcwd(ret);

        திரும்பு get_process_link_contents(pid, "cwd", ret);
}

முழு get_process_வேர்(pid_t pid, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(pid >= 0);
        திரும்பு get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

முழு get_process_environ(pid_t pid, எழுத்து **ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_free_ எழுத்து *outcome = வெற்று;
        size_t sz = 0;
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                எழுத்து c;

                எனில் (sz >= ENVIRONMENT_BLOCK_MAX)
                        திரும்பு -ENOBUFS;

                எனில் (!GREEDY_REALLOC(outcome, sz + 5))
                        திரும்பு -ENOMEM;

                r = safe_fgetc(f, &c);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                எனில் (c == '\0')
                        outcome[sz++] = '\n';
                இல்லையெனில்
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        திரும்பு 0;
}

முழு pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        unsigned long ppid;
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);

        எனில் (pid == 0)
                pid = getpid_cached();
        எனில் (pid == 1) /* PID 1 has no parent, shortcut this case */
                திரும்பு -EADDRNOTAVAIL;

        எனில் (pid == getpid_cached()) {
                எனில் (ret)
                        *ret = getppid();
                திரும்பு 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_வரி_file(p, &வரி);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(வரி, ')');
        எனில் (!p)
                திரும்பு -EIO;
        p++;

        எனில் (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                திரும்பு -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        எனில் (ppid == 0)
                திரும்பு -EADDRNOTAVAIL;

        எனில் ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                திரும்பு -ERANGE;

        எனில் (ret)
                *ret = (pid_t) ppid;

        திரும்பு 0;
}

முழு pidref_get_ppid(மாறிலி PidRef *pidref, pid_t *ret) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                எனில் (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        திரும்பு r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = ppid;
        திரும்பு 0;
}

முழு pidref_get_ppid_as_pidref(மாறிலி PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        முழு r;

        POINTER_MAY_BE_NULL(pidref);
        ப்ரதிஜ்ஞா(ret);

        r = pidref_get_ppid(pidref, &ppid);
        எனில் (r < 0)
                திரும்பு r;

        க்கு (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                எனில் (r < 0)
                        திரும்பு r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                எனில் (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        எனில் (r < 0)
                                திரும்பு r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        எனில் (parent.pid != ppid)
                                தொடர்;
                }

                *ret = TAKE_PIDREF(parent);
                திரும்பு 0;
        }

        /* Give up after 16 tries */
        திரும்பு -ENOTRECOVERABLE;
}

முழு pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_வரி_file(p, &வரி);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(வரி, ')');
        எனில் (!p)
                திரும்பு -EIO;
        p++;

        unsigned long llu;

        எனில் (sscanf(p, " "
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
                திரும்பு -EIO;

        எனில் (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        திரும்பு 0;
}

முழு pidref_get_start_time(மாறிலி PidRef *pid, usec_t *ret) {
        usec_t t;
        முழு r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = t;

        திரும்பு 0;
}

முழு get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ எழுத்து *m = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_mode(m, ret);
}

நிலையான முழு pid_get_sigmask(pid_t pid, மாறிலி எழுத்து *field_name, uint64_t *ret) {
        _cleanup_free_ எழுத்து *field = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(field_name);
        ப்ரதிஜ்ஞா(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு safe_atou64_full(field, 16, ret);
}

நிலையான முழு pidref_has_sigmask(மாறிலி PidRef *pidref, முழு sig, மாறிலி எழுத்து *field_name) {
        uint64_t mask;
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;
        எனில் (!SIGNAL_VALID(sig))
                திரும்பு -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு BIT_SET(mask, sig - 1);
}

முழு pidref_has_sigcgt(மாறிலி PidRef *pidref, முழு sig) {
        திரும்பு pidref_has_sigmask(pidref, sig, "SigCgt");
}

முழு pidref_has_sigblk(மாறிலி PidRef *pidref, முழு sig) {
        திரும்பு pidref_has_sigmask(pidref, sig, "SigBlk");
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
முழு pidref_wait_for_terminate_and_check(மாறிலி எழுத்து *name, PidRef *pidref, WaitFlags flags) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;
        எனில் (pidref->pid == 1 || pidref_is_self(pidref))
                திரும்பு -ECHILD;

        _cleanup_free_ எழுத்து *buffer = வெற்று;
        எனில் (!name) {
                r = pidref_get_comm(pidref, &buffer);
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                இல்லையெனில்
                        name = buffer;
        }

        முழு prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        எனில் (r < 0)
                திரும்பு log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        எனில் (status.si_code == CLD_EXITED) {
                எனில் (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                இல்லையெனில்
                        log_debug("'%s' succeeded.", name);

                திரும்பு status.si_status;

        } இல்லையெனில் எனில் (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                திரும்பு log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        திரும்பு log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

முழு kill_and_sigcont(pid_t pid, முழு sig) {
        முழு r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        எனில் (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (வெறுமை) kill(pid, SIGCONT);

        திரும்பு r;
}

முழு getenv_for_pid(pid_t pid, மாறிலி எழுத்து *field, எழுத்து **ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        மாறிலி எழுத்து *path;
        size_t sum = 0;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(field);
        ப்ரதிஜ்ஞா(ret);

        எனில் (pid == 0 || pid == getpid_cached())
                திரும்பு strdup_to_full(ret, getenv(field));

        எனில் (!pid_is_valid(pid))
                திரும்பு -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;
                மாறிலி எழுத்து *match;

                எனில் (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        திரும்பு -ENOBUFS;

                r = read_nul_string(f, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)  /* EOF */
                        நிறுத்து;

                sum += r;

                match = startswith(வரி, field);
                எனில் (match && *match == '=')
                        திரும்பு strdup_to_full(ret, match + 1);
        }

        *ret = வெற்று;
        திரும்பு 0;
}

முழு pidref_is_my_child(PidRef *pid) {
        முழு r;

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        எனில் (pid->pid == 1 || pidref_is_self(pid))
                திரும்பு false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        எனில் (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு ppid == getpid_cached();
}

முழு pid_is_my_child(pid_t pid) {

        எனில் (pid == 0)
                திரும்பு false;

        திரும்பு pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

முழு pidref_is_unwaited(PidRef *pid) {
        முழு r;

        /* Checks whether a PID is still valid at all, including a zombie */

        எனில் (!pidref_is_set(pid))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pid))
                திரும்பு -EREMOTE;

        எனில் (pid->pid == 1 || pidref_is_self(pid))
                திரும்பு true;

        r = pidref_kill(pid, 0);
        எனில் (r == -ESRCH)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு true;
}

முழு pid_is_unwaited(pid_t pid) {

        எனில் (pid == 0)
                திரும்பு true;

        திரும்பு pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

முழு pid_is_alive(pid_t pid) {
        முழு r;

        /* Checks whether a PID is still valid and not a zombie */

        எனில் (pid < 0)
                திரும்பு -ESRCH;

        எனில் (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                திரும்பு true;

        எனில் (pid == getpid_cached())
                திரும்பு true;

        r = get_process_state(pid);
        எனில் (r == -ESRCH)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு r != 'Z';
}

முழு pidref_is_alive(மாறிலி PidRef *pidref) {
        முழு r, result;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        result = pid_is_alive(pidref->pid);
        எனில் (result < 0) {
                ப்ரதிஜ்ஞா(result != -ESRCH);
                திரும்பு result;
        }

        r = pidref_verify(pidref);
        எனில் (r == -ESRCH)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு result;
}

முழு pidref_from_same_வேர்_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        முழு r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        எனில் (!a || !b) {
                r = pidref_set_self(&self);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (!a)
                        a = &self;
                எனில் (!b)
                        b = &self;
        }

        எனில் (!pidref_is_set(a) || !pidref_is_set(b))
                திரும்பு -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        எனில் (pidref_is_remote(a) && pidref_is_remote(b))
                திரும்பு -EREMOTE;
        எனில் (pidref_is_remote(a) || pidref_is_remote(b))
                திரும்பு false;

        எனில் (pidref_equal(a, b))
                திரும்பு true;

        மாறிலி எழுத்து *roota = procfs_file_alloca(a->pid, "root");
        மாறிலி எழுத்து *rootb = procfs_file_alloca(b->pid, "root");

        முழு result = inode_same(roota, rootb, 0);
        எனில் (result == -ENOENT)
                திரும்பு proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        எனில் (result < 0)
                திரும்பு result;

        r = pidref_verify(a);
        எனில் (r < 0)
                திரும்பு r;
        r = pidref_verify(b);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு result;
}

bool is_main_thread(வெறுமை) {
        நிலையான thread_local முழு cached = -1;

        எனில் (cached < 0)
                cached = getpid_cached() == gettid();

        திரும்பு cached;
}

unsigned long personality_from_string(மாறிலி எழுத்து *s) {
        Architecture architecture;

        எனில் (!s)
                திரும்பு PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        எனில் (architecture < 0)
                திரும்பு PERSONALITY_INVALID;

        எனில் (architecture == native_architecture())
                திரும்பு PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        எனில் (architecture == ARCHITECTURE_SECONDARY)
                திரும்பு PER_LINUX32;
#endif

        திரும்பு PERSONALITY_INVALID;
}

மாறிலி எழுத்து* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        எனில் (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        இல்லையெனில் எனில் (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        எனில் (architecture < 0)
                திரும்பு வெற்று;

        திரும்பு architecture_to_string(architecture);
}

முழு safe_personality(unsigned long p) {
        முழு ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        எனில் (ret < 0) {
                எனில் (errno != 0)
                        திரும்பு -errno;

                errno = -ret;
        }

        திரும்பு ret;
}

முழு opinionated_personality(unsigned long *ret) {
        முழு current;

        ப்ரதிஜ்ஞா(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        எனில் (current < 0)
                திரும்பு current;

        எனில் (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        இல்லையெனில்
                *ret = PER_LINUX;

        திரும்பு 0;
}

வெறுமை valgrind_summary_hack(வெறுமை) {
#எனில் HAVE_VALGRIND_VALGRIND_H
        எனில் (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                எனில் (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                இல்லையெனில் எனில் (pid == 0)
                        exit(EXIT_SUCCESS);
                இல்லையெனில் {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (வெறுமை) pidref_set_pid(&pidref, pid);
                        (வெறுமை) pidref_wait_for_terminate(&pidref, வெற்று);
                }
        }
#endif
}

முழு pid_compare_func(மாறிலி pid_t *a, மாறிலி pid_t *b) {
        /* Suitable for usage in qsort() */
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு CMP(*a, *b);
}

bool nice_is_valid(முழு n) {
        திரும்பு n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(முழு policy) {
        திரும்பு IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(முழு policy) {
        திரும்பு sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
முழு sched_get_priority_min_safe(முழு policy) {
        முழு r;

        r = sched_get_priority_min(policy);
        எனில் (r >= 0)
                திரும்பு r;

        /* Fallback priority */
        திரும்பு 0;
}

முழு sched_get_priority_max_safe(முழு policy) {
        முழு r;

        r = sched_get_priority_max(policy);
        எனில் (r >= 0)
                திரும்பு r;

        திரும்பு 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

நிலையான pid_t cached_pid = CACHED_PID_UNSET;

வெறுமை reset_cached_pid(வெறுமை) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(வெறுமை) {
        நிலையான bool installed = false;
        pid_t current_மதிப்பு;

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
        current_மதிப்பு = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        எனில் (current_மதிப்பு > 0)
                திரும்பு current_மதிப்பு;

        current_மதிப்பு = CACHED_PID_UNSET;
        (வெறுமை) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_மதிப்பு,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        தேர்வு (current_மதிப்பு) {

        நிலை CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                எனில் (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        எனில் (pthread_atfork(வெற்று, வெற்று, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                திரும்பு new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                திரும்பு new_pid;
        }

        நிலை CACHED_PID_BUSY: /* Somebody else is currently initializing */
                திரும்பு getpid();

        இயல்பு: /* Properly initialized */
                திரும்பு current_மதிப்பு;
        }
}

முழு must_be_வேர்(வெறுமை) {

        எனில் (geteuid() == 0)
                திரும்பு 0;

        திரும்பு log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_அடுக்கு(முழு (*fn)(வெறுமை *), முழு flags, வெறுமை *userdata) {
        size_t ps;
        pid_t pid;
        வெறுமை *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        ப்ரதிஜ்ஞா((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (வெறுமை*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#எனில் HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#இல்லையெனில்
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        எனில் (pid < 0)
                திரும்பு -errno;

        திரும்பு pid;
}

நிலையான முழு fork_flags_to_signal(ForkFlags flags) {
        திரும்பு (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

முழு pidref_safe_fork_full(
                மாறிலி எழுத்து *name,
                மாறிலி முழு stdio_fds[3],
                முழு except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = வெற்று;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ முழு pidref_transport_fds[2] = EBADF_PAIR;
        முழு prio, r;

        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        எனில் (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        எனில் (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } இல்லையெனில் எனில் (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        எனில் (block_signals) {
                எனில் (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        திரும்பு log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        எனில் (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                எனில் (r < 0)
                        திரும்பு log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                எனில் (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        எனில் (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                திரும்பு log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        எனில் (pid < 0)
                                திரும்பு log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        எனில் (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                எனில் (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        எனில் (r < 0)
                                                திரும்பு log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        எனில் (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                திரும்பு -EPROTO;

                                        முழு pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, அளவு(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        எனில் (n < 0)
                                                திரும்பு log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                திரும்பு 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        எனில் ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        இல்லையெனில்
                pid = fork();
        எனில் (pid < 0)
                திரும்பு log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        எனில் (pid > 0) {

                /* If we are in the intermediary process, exit now */
                எனில் (intermediary) {
                        எனில் (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                எனில் (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, அளவு(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                எனில் (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                எனில் (flags & FORK_WAIT) {
                        எனில் (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (வெறுமை) sigprocmask(SIG_SETMASK, &ss, வெற்று);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                திரும்பு -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        எனில் (ret)
                                *ret = PIDREF_NULL;

                        திரும்பு 1;
                }

                எனில் (ret) {
                        r = pidref_set_pid(ret, pid);
                        எனில் (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                திரும்பு 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = வெற்று;

        எனில் (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        எனில் (name) {
                r = rename_process(name);
                எனில் (r < 0)
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
        எனில் (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        எனில் (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } இல்லையெனில் எனில் (block_signals) { /* undo what we did above */
                எனில் (sigprocmask(SIG_SETMASK, &saved_ss, வெற்று) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                எனில் (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                இல்லையெனில் எனில் (ppid != original_pid) {
                        முழு sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (வெறுமை) எழுப்பு(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                எனில் (mount(வெற்று, "/", வெற்று, MS_SLAVE | MS_REC, வெற்று) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                ப்ரதிஜ்ஞா(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (flags & FORK_REARRANGE_STDIO) {
                எனில் (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        எனில் (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } இல்லையெனில் {
                        r = make_null_stdio();
                        எனில் (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } இல்லையெனில் எனில் (flags & FORK_STDOUT_TO_STDERR) {
                எனில் (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                ப்ரதிஜ்ஞா(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        எனில் (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        எனில் (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        எனில் (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        எனில் (ret) {
                r = pidref_set_self(ret);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        திரும்பு 0;
}

முழு namespace_fork_full(
                மாறிலி எழுத்து *outer_name,
                மாறிலி எழுத்து *inner_name,
                முழு except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                முழு pidns_fd,
                முழு mntns_fd,
                முழு netns_fd,
                முழு userns_fd,
                முழு வேர்_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ முழு errno_pipe_fd[2] = EBADF_PAIR;
        முழு r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        ப்ரதிஜ்ஞா((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        ப்ரதிஜ்ஞா((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(அளவு(முழு) <= PIPE_BUF);
        எனில் (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                திரும்பு log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ வெற்று, /* except_fds= */ வெற்று, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        எனில் (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                முழு k = read_errno(errno_pipe_fd[0]);
                எனில் (k < 0 && k != -EIO)
                        திரும்பு k;
        }
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, வேர்_fd, namespace_flags);
                எனில் (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                வெற்று,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                எனில் (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                எனில் (r == 0) {
                        /* Child */

                        எனில் (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } இல்லையெனில் {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        எனில் (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        திரும்பு 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (வெறுமை) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                எனில் (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        எனில் (r < 0)
                திரும்பு r; /* the child logs about failures on its own, no need to duplicate here */

        எனில் (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        இல்லையெனில்
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        திரும்பு 1;
}

முழு namespace_fork(
                மாறிலி எழுத்து *outer_name,
                மாறிலி எழுத்து *inner_name,
                ForkFlags flags,
                முழு pidns_fd,
                முழு mntns_fd,
                முழு netns_fd,
                முழு userns_fd,
                முழு வேர்_fd,
                PidRef *ret) {

        திரும்பு namespace_fork_full(outer_name, inner_name, /* except_fds= */ வெற்று, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, வேர்_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(முழு oa) {
        திரும்பு oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

முழு set_oom_score_adjust(முழு மதிப்பு) {
        எழுத்து t[DECIMAL_STR_MAX(முழு)];

        எனில் (!oom_score_adjust_is_valid(மதிப்பு))
                திரும்பு -EINVAL;

        xsprintf(t, "%i", மதிப்பு);

        திரும்பு write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

முழு get_oom_score_adjust(முழு *ret) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!oom_score_adjust_is_valid(a))
                திரும்பு -ENODATA;

        எனில் (ret)
                *ret = a;

        திரும்பு 0;
}

நிலையான முழு rlimit_to_nice(rlim_t limit) {
        எனில் (limit <= 1)
                திரும்பு PRIO_MAX-1; /* i.e. 19 */

        எனில் (limit >= -PRIO_MIN + PRIO_MAX)
                திரும்பு PRIO_MIN; /* i.e. -20 */

        திரும்பு PRIO_MAX - (முழு) limit;
}

முழு setpriority_closest(முழு priority) {
        கட்டமைப்பு rlimit highest;
        முழு r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        எனில் (r >= 0)
                திரும்பு 1;
        எனில் (!ERRNO_IS_NEG_PRIVILEGE(r))
                திரும்பு r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        எனில் (errno != 0)
                திரும்பு -errno;

        எனில் (priority == current)
                திரும்பு 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        எனில் (priority > current)
                திரும்பு r;

        எனில் (getrlimit(RLIMIT_NICE, &highest) < 0)
                திரும்பு -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        எனில் (current > limit)
                எனில் (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        திரும்பு -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        திரும்பு 0;
}

_noreturn_ வெறுமை freeze(வெறுமை) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (வெறுமை) close_all_fds_without_malloc(வெற்று, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        க்கு (;;) {
                siginfo_t si = {};

                எனில் (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        நிறுத்து;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        க்கு (;;)
                pause();
}

முழு get_process_threads(pid_t pid) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு n, r;

        எனில் (pid < 0)
                திரும்பு -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        r = safe_atoi(t, &n);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (n < 0)
                திரும்பு -EINVAL;

        திரும்பு n;
}

முழு is_reaper_process(வெறுமை) {
        முழு b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        எனில் (getpid_cached() == 1)
                திரும்பு true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு b != 0;
}

முழு make_reaper_process(bool b) {
        முழு r;

        எனில் (getpid_cached() == 1) {

                எனில் (!b)
                        திரும்பு -EINVAL;

                திரும்பு 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, வெற்று);

முழு posix_spawn_wrapper(
                மாறிலி எழுத்து *path,
                எழுத்து * மாறிலி *argv,
                எழுத்து * மாறிலி *envp,
                மாறிலி எழுத்து *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        முழு r;

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

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(argv);
        ப்ரதிஜ்ஞா(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        எனில் (r != 0)
                திரும்பு -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        நிலையான bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ முழு cgroup_fd = -EBADF;

        எனில் (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ எழுத்து *resolved_cgroup = வெற்று;

                r = cg_get_path(cgroup, /* suffix= */ வெற்று, &resolved_cgroup);
                எனில் (r < 0)
                        திரும்பு r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                எனில் (cgroup_fd < 0)
                        திரும்பு -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                எனில் (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                இல்லையெனில் எனில் (r != ENOSYS)
                        திரும்பு -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        எனில் (r != 0)
                திரும்பு -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        எனில் (r != 0)
                திரும்பு -r;

        _cleanup_close_ முழு pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, வெற்று, &attr, argv, envp);
        எனில் (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                திரும்பு -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        எனில் ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                எனில் (r != 0)
                        திரும்பு -r;

                r = pidfd_spawn(&pidfd, path, வெற்று, &attr, argv, envp);
        }
        எனில் (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        எனில் (!ERRNO_IS_NOT_SUPPORTED(r))
                திரும்பு -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, வெற்று, &attr, argv, envp);
        எனில் (r != 0)
                திரும்பு -r;

        r = pidref_set_pid(ret_pidref, pid);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

முழு proc_dir_open(DIR **ret) {
        DIR *d;

        ப்ரதிஜ்ஞா(ret);

        d = opendir("/proc");
        எனில் (!d)
                திரும்பு -errno;

        *ret = d;
        திரும்பு 0;
}

முழு proc_dir_read(DIR *d, pid_t *ret) {
        ப்ரதிஜ்ஞா(d);

        க்கு (;;) {
                கட்டமைப்பு dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                எனில் (!de) {
                        எனில் (errno != 0)
                                திரும்பு -errno;

                        நிறுத்து;
                }

                எனில் (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        தொடர்;

                எனில் (parse_pid(de->d_name, ret) >= 0)
                        திரும்பு 1;
        }

        எனில் (ret)
                *ret = 0;
        திரும்பு 0;
}

முழு proc_dir_read_pidref(DIR *d, PidRef *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(d);

        க்கு (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                r = pidref_set_pid(ret, pid);
                எனில் (r == -ESRCH) /* gone by now? skip it */
                        தொடர்;
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு 1;
        }

        எனில் (ret)
                *ret = PIDREF_NULL;
        திரும்பு 0;
}

முழு safe_mlockall(முழு flags) {
        முழு r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        எனில் (r == 0)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        எனில் (mlockall(flags) < 0)
                திரும்பு log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        திரும்பு 0;
}

நிலையான மாறிலி எழுத்து *மாறிலி sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, முழு);

நிலையான மாறிலி எழுத்து* மாறிலி sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, முழு, INT_MAX);

_noreturn_ வெறுமை report_errno_and_exit(முழு errno_fd, முழு error) {
        முழு r;

        எனில் (error >= 0)
                _exit(EXIT_SUCCESS);

        ப்ரதிஜ்ஞா(errno_fd >= 0);

        r = loop_write(errno_fd, &error, அளவு(error));
        எனில் (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

முழு read_errno(முழு errno_fd) {
        முழு r;

        ப்ரதிஜ்ஞா(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, அளவு(r), /* do_poll= */ false);
        எனில் (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                திரும்பு -EIO;
        }
        எனில் (n == 0) /* the process exited without reporting an error, assuming success */
                திரும்பு 0;
        எனில் (n != அளவு(r))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        எனில் (r == 0)
                திரும்பு 0;
        எனில் (r < 0) /* child process reported an error, return it */
                திரும்பு log_debug_errno(r, "Child process failed with errno: %m");

        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

முழு prctl_safe(முழு op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        திரும்பு RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

முழு proc_set_comm(மாறிலி எழுத்து *comm) {
        திரும்பு prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

முழு proc_set_nnp(வெறுமை) {
        திரும்பு prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(வெறுமை) {
        முழு r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        எனில் (r < 0)
                திரும்பு NSEC_INFINITY;

        திரும்பு r;
}
