/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/oom.h>
#ଅନ୍ତର୍ଭାଵ <pthread.h>
#ଅନ୍ତର୍ଭାଵ <spawn.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <sys/mman.h>
#ଅନ୍ତର୍ଭାଵ <sys/mount.h>
#ଅନ୍ତର୍ଭାଵ <sys/personality.h>
#ଅନ୍ତର୍ଭାଵ <sys/prctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/wait.h>
#ଅନ୍ତର୍ଭାଵ <syslog.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଯଦି HAVE_VALGRIND_VALGRIND_H
#ଅନ୍ତର୍ଭାଵ <valgrind/valgrind.h>
#endif

#ଅନ୍ତର୍ଭାଵ "sd-messages.h"

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "architecture.h"
#ଅନ୍ତର୍ଭାଵ "argv-util.h"
#ଅନ୍ତର୍ଭାଵ "bitfield.h"
#ଅନ୍ତର୍ଭାଵ "capability-util.h"
#ଅନ୍ତର୍ଭାଵ "cgroup-util.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "dlfcn-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "locale-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "mountpoint-util.h"
#ଅନ୍ତର୍ଭାଵ "namespace-util.h"
#ଅନ୍ତର୍ଭାଵ "nulstr-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidfd-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "raw-clone.h"
#ଅନ୍ତର୍ଭାଵ "rlimit-util.h"
#ଅନ୍ତର୍ଭାଵ "signal-util.h"
#ଅନ୍ତର୍ଭାଵ "socket-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_state(pid_t pid) {
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *p;
        ଅକ୍ଷର state;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        ଯଦି (pid == 0 || pid == getpid_cached())
                ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_ଧାଡ଼ି_file(p, &ଧାଡ଼ି);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        p = strrchr(ଧାଡ଼ି, ')');
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EIO;

        p++;

        ଯଦି (sscanf(p, " %c", &state) != 1)
                ଫେରନ୍ତୁ -EIO;

        ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର) state;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_comm(pid_t pid, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *escaped = ଖାଲି, *comm = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pid == 0 || pid == getpid_cached()) {
                comm = new0(ଅକ୍ଷର, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                ଯଦି (!comm)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        } ଅନ୍ୟଥା {
                ଅଚଳ ଅକ୍ଷର *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_ଧାଡ଼ି_file(p, &comm);
                ଯଦି (r == -ENOENT)
                        ଫେରନ୍ତୁ -ESRCH;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        escaped = new(ଅକ୍ଷର, COMM_MAX_LEN);
        ଯଦି (!escaped)
                ଫେରନ୍ତୁ -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_comm(ଅଚଳ PidRef *pid, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *comm = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = TAKE_PTR(comm);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                ଅକ୍ଷର **ret,
                size_t *ret_size) {

        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *p;
        size_t k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (k == 0) {
                ଯଦି (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        ଫେରନ୍ତୁ -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ ଅକ୍ଷର *comm = ଖାଲି;

                r = pid_get_comm(pid, &comm);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ମୁକ୍ତ(t);
                t = strjoin("[", comm, "]");
                ଯଦି (!t)
                        ଫେରନ୍ତୁ -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                ଯଦି (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        ଯଦି (ret)
                *ret = TAKE_PTR(t);
        ଯଦି (ret_size)
                *ret_size = k;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        size_t k;
        ଅକ୍ଷର *ans;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

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

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        ଯଦି (full < 0)
                ଫେରନ୍ତୁ full;

        ଯଦି (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                ପ୍ରତିଜ୍ଞା(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ ଅକ୍ଷର **args = ଖାଲି;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                ଯଦି (!args)
                        ଫେରନ୍ତୁ -ENOMEM;

                ans = quote_command_ଧାଡ଼ି(args, shflags);
                ଯଦି (!ans)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                ପାଇଁ (size_t i = 0; i < k - 1; i++)
                        ଯଦି (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                ଯଦି (!ans)
                        ଫେରନ୍ତୁ -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_cmdline(ଅଚଳ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ଅକ୍ଷର ***ret) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ଅକ୍ଷର **args;
        size_t k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        ପ୍ରତିଜ୍ଞା(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        ଯଦି (!args)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = args;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_cmdline_strv(ଅଚଳ PidRef *pid, ProcessCmdlineFlags flags, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **args = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = TAKE_PTR(args);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_get_stat_flags(ଅଚଳ ଅକ୍ଷର *path, unsigned long long *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        r = read_one_ଧାଡ଼ି_file(path, &ଧାଡ଼ି);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Skip past the comm field */
        ଅକ୍ଷର *q = strrchr(ଧାଡ଼ି, ')');
        ଯଦି (!q)
                ଫେରନ୍ତୁ -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        ପାଇଁ (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                ଯଦି (l < 1)
                        ଫେରନ୍ତୁ -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                ଯଦି (l < 1)
                        ଫେରନ୍ତୁ -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        ଯଦି (l < 1)
                ଫେରନ୍ତୁ -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        ଯଦି (l < 1)
                ଫେରନ୍ତୁ -EINVAL;
        q[l] = 0;

        ଫେରନ୍ତୁ safe_atollu(q, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_kernel_thread(pid_t pid) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                ଫେରନ୍ତୁ 0;
        ଯଦି (!pid_is_valid(pid))
                ଫେରନ୍ତୁ -EINVAL;

        ଅଚଳ ଅକ୍ଷର *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ !!(flags & PF_KTHREAD);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_kernel_thread(ଅଚଳ PidRef *pid) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା result, r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        ଯଦି (result < 0)
                ଫେରନ୍ତୁ result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_link_contents(pid_t pid, ଅଚଳ ଅକ୍ଷର *proc_file, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        ଫେରନ୍ତୁ (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_exe(pid_t pid, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *d;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret) {
                d = endswith(*ret, " (deleted)");
                ଯଦି (d)
                        *d = '\0';
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_uid(pid_t pid, uid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_uid(v, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_uid(ଅଚଳ PidRef *pid, uid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                ଯଦି (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ଫେରନ୍ତୁ r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = uid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_gid(pid_t pid, gid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_gid(v, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_cwd(pid_t pid, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(pid >= 0);

        ଯଦି (pid == 0 || pid == getpid_cached())
                ଫେରନ୍ତୁ safe_getcwd(ret);

        ଫେରନ୍ତୁ get_process_link_contents(pid, "cwd", ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_ମୂଳ(pid_t pid, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ଫେରନ୍ତୁ get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_environ(pid_t pid, ଅକ୍ଷର **ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *outcome = ଖାଲି;
        size_t sz = 0;
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                ଅକ୍ଷର c;

                ଯଦି (sz >= ENVIRONMENT_BLOCK_MAX)
                        ଫେରନ୍ତୁ -ENOBUFS;

                ଯଦି (!GREEDY_REALLOC(outcome, sz + 5))
                        ଫେରନ୍ତୁ -ENOMEM;

                r = safe_fgetc(f, &c);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (c == '\0')
                        outcome[sz++] = '\n';
                ଅନ୍ୟଥା
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        unsigned long ppid;
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);

        ଯଦି (pid == 0)
                pid = getpid_cached();
        ଯଦି (pid == 1) /* PID 1 has no parent, shortcut this case */
                ଫେରନ୍ତୁ -EADDRNOTAVAIL;

        ଯଦି (pid == getpid_cached()) {
                ଯଦି (ret)
                        *ret = getppid();
                ଫେରନ୍ତୁ 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ଧାଡ଼ି_file(p, &ଧାଡ଼ି);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ଧାଡ଼ି, ')');
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EIO;
        p++;

        ଯଦି (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                ଫେରନ୍ତୁ -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        ଯଦି (ppid == 0)
                ଫେରନ୍ତୁ -EADDRNOTAVAIL;

        ଯଦି ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret)
                *ret = (pid_t) ppid;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_ppid(ଅଚଳ PidRef *pidref, pid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                ଯଦି (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ଫେରନ୍ତୁ r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = ppid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_ppid_as_pidref(ଅଚଳ PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        POINTER_MAY_BE_NULL(pidref);
        ପ୍ରତିଜ୍ଞା(ret);

        r = pidref_get_ppid(pidref, &ppid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                ଯଦି (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        ଯଦି (parent.pid != ppid)
                                ଜାରି;
                }

                *ret = TAKE_PIDREF(parent);
                ଫେରନ୍ତୁ 0;
        }

        /* Give up after 16 tries */
        ଫେରନ୍ତୁ -ENOTRECOVERABLE;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ଧାଡ଼ି_file(p, &ଧାଡ଼ି);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(ଧାଡ଼ି, ')');
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EIO;
        p++;

        unsigned long llu;

        ଯଦି (sscanf(p, " "
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
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_start_time(ଅଚଳ PidRef *pid, usec_t *ret) {
        usec_t t;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = t;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *m = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_mode(m, ret);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_sigmask(pid_t pid, ଅଚଳ ଅକ୍ଷର *field_name, uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *field = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(field_name);
        ପ୍ରତିଜ୍ଞା(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ safe_atou64_full(field, 16, ret);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_has_sigmask(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, ଅଚଳ ଅକ୍ଷର *field_name) {
        uint64_t mask;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;
        ଯଦି (!SIGNAL_VALID(sig))
                ଫେରନ୍ତୁ -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ BIT_SET(mask, sig - 1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_has_sigcgt(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {
        ଫେରନ୍ତୁ pidref_has_sigmask(pidref, sig, "SigCgt");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_has_sigblk(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {
        ଫେରନ୍ତୁ pidref_has_sigmask(pidref, sig, "SigBlk");
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_wait_for_terminate_and_check(ଅଚଳ ଅକ୍ଷର *name, PidRef *pidref, WaitFlags flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;
        ଯଦି (pidref->pid == 1 || pidref_is_self(pidref))
                ଫେରନ୍ତୁ -ECHILD;

        _cleanup_free_ ଅକ୍ଷର *buffer = ଖାଲି;
        ଯଦି (!name) {
                r = pidref_get_comm(pidref, &buffer);
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                ଅନ୍ୟଥା
                        name = buffer;
        }

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        ଯଦି (status.si_code == CLD_EXITED) {
                ଯଦି (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                ଅନ୍ୟଥା
                        log_debug("'%s' succeeded.", name);

                ଫେରନ୍ତୁ status.si_status;

        } ଅନ୍ୟଥା ଯଦି (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                ଫେରନ୍ତୁ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        ଫେରନ୍ତୁ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା kill_and_sigcont(pid_t pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        ଯଦି (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (ଶୂନ୍ୟ) kill(pid, SIGCONT);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_for_pid(pid_t pid, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର **ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *path;
        size_t sum = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(field);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pid == 0 || pid == getpid_cached())
                ଫେରନ୍ତୁ strdup_to_full(ret, getenv(field));

        ଯଦି (!pid_is_valid(pid))
                ଫେରନ୍ତୁ -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
                ଅଚଳ ଅକ୍ଷର *match;

                ଯଦି (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        ଫେରନ୍ତୁ -ENOBUFS;

                r = read_nul_string(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)  /* EOF */
                        ଭାଙ୍ଗ;

                sum += r;

                match = startswith(ଧାଡ଼ି, field);
                ଯଦି (match && *match == '=')
                        ଫେରନ୍ତୁ strdup_to_full(ret, match + 1);
        }

        *ret = ଖାଲି;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_my_child(PidRef *pid) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pid->pid == 1 || pidref_is_self(pid))
                ଫେରନ୍ତୁ false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        ଯଦି (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ ppid == getpid_cached();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_my_child(pid_t pid) {

        ଯଦି (pid == 0)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_unwaited(PidRef *pid) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks whether a PID is still valid at all, including a zombie */

        ଯଦି (!pidref_is_set(pid))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pid))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pid->pid == 1 || pidref_is_self(pid))
                ଫେରନ୍ତୁ true;

        r = pidref_kill(pid, 0);
        ଯଦି (r == -ESRCH)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_unwaited(pid_t pid) {

        ଯଦି (pid == 0)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_alive(pid_t pid) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks whether a PID is still valid and not a zombie */

        ଯଦି (pid < 0)
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                ଫେରନ୍ତୁ true;

        ଯଦି (pid == getpid_cached())
                ଫେରନ୍ତୁ true;

        r = get_process_state(pid);
        ଯଦି (r == -ESRCH)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ r != 'Z';
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_alive(ଅଚଳ PidRef *pidref) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, result;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        result = pid_is_alive(pidref->pid);
        ଯଦି (result < 0) {
                ପ୍ରତିଜ୍ଞା(result != -ESRCH);
                ଫେରନ୍ତୁ result;
        }

        r = pidref_verify(pidref);
        ଯଦି (r == -ESRCH)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_from_same_ମୂଳ_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        ଯଦି (!a || !b) {
                r = pidref_set_self(&self);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (!a)
                        a = &self;
                ଯଦି (!b)
                        b = &self;
        }

        ଯଦି (!pidref_is_set(a) || !pidref_is_set(b))
                ଫେରନ୍ତୁ -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        ଯଦି (pidref_is_remote(a) && pidref_is_remote(b))
                ଫେରନ୍ତୁ -EREMOTE;
        ଯଦି (pidref_is_remote(a) || pidref_is_remote(b))
                ଫେରନ୍ତୁ false;

        ଯଦି (pidref_equal(a, b))
                ଫେରନ୍ତୁ true;

        ଅଚଳ ଅକ୍ଷର *roota = procfs_file_alloca(a->pid, "root");
        ଅଚଳ ଅକ୍ଷର *rootb = procfs_file_alloca(b->pid, "root");

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = inode_same(roota, rootb, 0);
        ଯଦି (result == -ENOENT)
                ଫେରନ୍ତୁ proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        ଯଦି (result < 0)
                ଫେରନ୍ତୁ result;

        r = pidref_verify(a);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        r = pidref_verify(b);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ result;
}

bool is_main_thread(ଶୂନ୍ୟ) {
        ସ୍ଥିର thread_local ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached = -1;

        ଯଦି (cached < 0)
                cached = getpid_cached() == gettid();

        ଫେରନ୍ତୁ cached;
}

unsigned long personality_from_string(ଅଚଳ ଅକ୍ଷର *s) {
        Architecture architecture;

        ଯଦି (!s)
                ଫେରନ୍ତୁ PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        ଯଦି (architecture < 0)
                ଫେରନ୍ତୁ PERSONALITY_INVALID;

        ଯଦି (architecture == native_architecture())
                ଫେରନ୍ତୁ PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        ଯଦି (architecture == ARCHITECTURE_SECONDARY)
                ଫେରନ୍ତୁ PER_LINUX32;
#endif

        ଫେରନ୍ତୁ PERSONALITY_INVALID;
}

ଅଚଳ ଅକ୍ଷର* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        ଯଦି (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        ଅନ୍ୟଥା ଯଦି (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        ଯଦି (architecture < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ architecture_to_string(architecture);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_personality(unsigned long p) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        ଯଦି (ret < 0) {
                ଯଦି (errno != 0)
                        ଫେରନ୍ତୁ -errno;

                errno = -ret;
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା opinionated_personality(unsigned long *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା current;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        ଯଦି (current < 0)
                ଫେରନ୍ତୁ current;

        ଯଦି (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        ଅନ୍ୟଥା
                *ret = PER_LINUX;

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ valgrind_summary_hack(ଶୂନ୍ୟ) {
#ଯଦି HAVE_VALGRIND_VALGRIND_H
        ଯଦି (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                ଯଦି (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                ଅନ୍ୟଥା ଯଦି (pid == 0)
                        exit(EXIT_SUCCESS);
                ଅନ୍ୟଥା {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (ଶୂନ୍ୟ) pidref_set_pid(&pidref, pid);
                        (ଶୂନ୍ୟ) pidref_wait_for_terminate(&pidref, ଖାଲି);
                }
        }
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_compare_func(ଅଚଳ pid_t *a, ଅଚଳ pid_t *b) {
        /* Suitable for usage in qsort() */
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ CMP(*a, *b);
}

bool nice_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) {
        ଫେରନ୍ତୁ n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy) {
        ଫେରନ୍ତୁ IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy) {
        ଫେରନ୍ତୁ sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sched_get_priority_min_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = sched_get_priority_min(policy);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ r;

        /* Fallback priority */
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sched_get_priority_max_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = sched_get_priority_max(policy);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

ସ୍ଥିର pid_t cached_pid = CACHED_PID_UNSET;

ଶୂନ୍ୟ reset_cached_pid(ଶୂନ୍ୟ) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(ଶୂନ୍ୟ) {
        ସ୍ଥିର bool installed = false;
        pid_t current_ମୂଲ୍ୟ;

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
        current_ମୂଲ୍ୟ = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        ଯଦି (current_ମୂଲ୍ୟ > 0)
                ଫେରନ୍ତୁ current_ମୂଲ୍ୟ;

        current_ମୂଲ୍ୟ = CACHED_PID_UNSET;
        (ଶୂନ୍ୟ) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_ମୂଲ୍ୟ,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        ବାଛ (current_ମୂଲ୍ୟ) {

        ସ୍ଥିତି CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                ଯଦି (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        ଯଦି (pthread_atfork(ଖାଲି, ଖାଲି, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                ଫେରନ୍ତୁ new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                ଫେରନ୍ତୁ new_pid;
        }

        ସ୍ଥିତି CACHED_PID_BUSY: /* Somebody else is currently initializing */
                ଫେରନ୍ତୁ getpid();

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* Properly initialized */
                ଫେରନ୍ତୁ current_ମୂଲ୍ୟ;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା must_be_ମୂଳ(ଶୂନ୍ୟ) {

        ଯଦି (geteuid() == 0)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_ଗଦା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fn)(ଶୂନ୍ୟ *), ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଶୂନ୍ୟ *userdata) {
        size_t ps;
        pid_t pid;
        ଶୂନ୍ୟ *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        ପ୍ରତିଜ୍ଞା((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (ଶୂନ୍ୟ*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#ଯଦି HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#ଅନ୍ୟଥା
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        ଯଦି (pid < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ pid;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fork_flags_to_signal(ForkFlags flags) {
        ଫେରନ୍ତୁ (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_safe_fork_full(
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା stdio_fds[3],
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = ଖାଲି;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_transport_fds[2] = EBADF_PAIR;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio, r;

        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        ଯଦି (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        ଯଦି (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } ଅନ୍ୟଥା ଯଦି (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        ଯଦି (block_signals) {
                ଯଦି (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        ଫେରନ୍ତୁ log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        ଯଦି (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                ଯଦି (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        ଯଦି (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                ଫେରନ୍ତୁ log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        ଯଦି (pid < 0)
                                ଫେରନ୍ତୁ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        ଯଦି (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                ଯଦି (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        ଯଦି (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                ଫେରନ୍ତୁ -EPROTO;

                                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, ଆକାର(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        ଯଦି (n < 0)
                                                ଫେରନ୍ତୁ log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                ଫେରନ୍ତୁ 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        ଯଦି ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        ଅନ୍ୟଥା
                pid = fork();
        ଯଦି (pid < 0)
                ଫେରନ୍ତୁ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        ଯଦି (pid > 0) {

                /* If we are in the intermediary process, exit now */
                ଯଦି (intermediary) {
                        ଯଦି (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                ଯଦି (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, ଆକାର(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                ଯଦି (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                ଯଦି (flags & FORK_WAIT) {
                        ଯଦି (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (ଶୂନ୍ୟ) sigprocmask(SIG_SETMASK, &ss, ଖାଲି);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                ଫେରନ୍ତୁ -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        ଯଦି (ret)
                                *ret = PIDREF_NULL;

                        ଫେରନ୍ତୁ 1;
                }

                ଯଦି (ret) {
                        r = pidref_set_pid(ret, pid);
                        ଯଦି (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                ଫେରନ୍ତୁ 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = ଖାଲି;

        ଯଦି (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        ଯଦି (name) {
                r = rename_process(name);
                ଯଦି (r < 0)
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
        ଯଦି (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        ଯଦି (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } ଅନ୍ୟଥା ଯଦି (block_signals) { /* undo what we did above */
                ଯଦି (sigprocmask(SIG_SETMASK, &saved_ss, ଖାଲି) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                ଯଦି (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                ଅନ୍ୟଥା ଯଦି (ppid != original_pid) {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (ଶୂନ୍ୟ) ଉଠାଅ(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                ଯଦି (mount(ଖାଲି, "/", ଖାଲି, MS_SLAVE | MS_REC, ଖାଲି) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                ପ୍ରତିଜ୍ଞା(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (flags & FORK_REARRANGE_STDIO) {
                ଯଦି (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        ଯଦି (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } ଅନ୍ୟଥା {
                        r = make_null_stdio();
                        ଯଦି (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } ଅନ୍ୟଥା ଯଦି (flags & FORK_STDOUT_TO_STDERR) {
                ଯଦି (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                ପ୍ରତିଜ୍ଞା(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        ଯଦି (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        ଯଦି (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଯଦି (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        ଯଦି (ret) {
                r = pidref_set_self(ret);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_fork_full(
                ଅଚଳ ଅକ୍ଷର *outer_name,
                ଅଚଳ ଅକ୍ଷର *inner_name,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_pipe_fd[2] = EBADF_PAIR;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        ପ୍ରତିଜ୍ଞା((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        ପ୍ରତିଜ୍ଞା((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) <= PIPE_BUF);
        ଯଦି (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                ଫେରନ୍ତୁ log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ ଖାଲି, /* except_fds= */ ଖାଲି, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        ଯଦି (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = read_errno(errno_pipe_fd[0]);
                ଯଦି (k < 0 && k != -EIO)
                        ଫେରନ୍ତୁ k;
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, ମୂଳ_fd, namespace_flags);
                ଯଦି (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                ଖାଲି,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                ଯଦି (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                ଯଦି (r == 0) {
                        /* Child */

                        ଯଦି (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } ଅନ୍ୟଥା {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        ଯଦି (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        ଫେରନ୍ତୁ 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (ଶୂନ୍ୟ) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                ଯଦି (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r; /* the child logs about failures on its own, no need to duplicate here */

        ଯଦି (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        ଅନ୍ୟଥା
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_fork(
                ଅଚଳ ଅକ୍ଷର *outer_name,
                ଅଚଳ ଅକ୍ଷର *inner_name,
                ForkFlags flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                PidRef *ret) {

        ଫେରନ୍ତୁ namespace_fork_full(outer_name, inner_name, /* except_fds= */ ଖାଲି, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, ମୂଳ_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା oa) {
        ଫେରନ୍ତୁ oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_oom_score_adjust(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ) {
        ଅକ୍ଷର t[DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)];

        ଯଦି (!oom_score_adjust_is_valid(ମୂଲ୍ୟ))
                ଫେରନ୍ତୁ -EINVAL;

        xsprintf(t, "%i", ମୂଲ୍ୟ);

        ଫେରନ୍ତୁ write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_oom_score_adjust(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!oom_score_adjust_is_valid(a))
                ଫେରନ୍ତୁ -ENODATA;

        ଯଦି (ret)
                *ret = a;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_to_nice(rlim_t limit) {
        ଯଦି (limit <= 1)
                ଫେରନ୍ତୁ PRIO_MAX-1; /* i.e. 19 */

        ଯଦି (limit >= -PRIO_MIN + PRIO_MAX)
                ଫେରନ୍ତୁ PRIO_MIN; /* i.e. -20 */

        ଫେରନ୍ତୁ PRIO_MAX - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) limit;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setpriority_closest(ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority) {
        ସଂରଚନା rlimit highest;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ 1;
        ଯଦି (!ERRNO_IS_NEG_PRIVILEGE(r))
                ଫେରନ୍ତୁ r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        ଯଦି (errno != 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (priority == current)
                ଫେରନ୍ତୁ 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        ଯଦି (priority > current)
                ଫେରନ୍ତୁ r;

        ଯଦି (getrlimit(RLIMIT_NICE, &highest) < 0)
                ଫେରନ୍ତୁ -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        ଯଦି (current > limit)
                ଯଦି (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        ଫେରନ୍ତୁ -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        ଫେରନ୍ତୁ 0;
}

_noreturn_ ଶୂନ୍ୟ freeze(ଶୂନ୍ୟ) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (ଶୂନ୍ୟ) close_all_fds_without_malloc(ଖାଲି, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        ପାଇଁ (;;) {
                siginfo_t si = {};

                ଯଦି (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        ଭାଙ୍ଗ;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        ପାଇଁ (;;)
                pause();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_threads(pid_t pid) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, r;

        ଯଦି (pid < 0)
                ଫେରନ୍ତୁ -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = safe_atoi(t, &n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_reaper_process(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        ଯଦି (getpid_cached() == 1)
                ଫେରନ୍ତୁ true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ b != 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_reaper_process(bool b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (getpid_cached() == 1) {

                ଯଦି (!b)
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, ଖାଲି);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_spawn_wrapper(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅକ୍ଷର * ଅଚଳ *argv,
                ଅକ୍ଷର * ଅଚଳ *envp,
                ଅଚଳ ଅକ୍ଷର *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(argv);
        ପ୍ରତିଜ୍ଞା(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        ସ୍ଥିର bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cgroup_fd = -EBADF;

        ଯଦି (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ ଅକ୍ଷର *resolved_cgroup = ଖାଲି;

                r = cg_get_path(cgroup, /* suffix= */ ଖାଲି, &resolved_cgroup);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ଯଦି (cgroup_fd < 0)
                        ଫେରନ୍ତୁ -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                ଯଦି (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                ଅନ୍ୟଥା ଯଦି (r != ENOSYS)
                        ଫେରନ୍ତୁ -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ -r;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, ଖାଲି, &attr, argv, envp);
        ଯଦି (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                ଫେରନ୍ତୁ -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        ଯଦି ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ -r;

                r = pidfd_spawn(&pidfd, path, ଖାଲି, &attr, argv, envp);
        }
        ଯଦି (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        ଯଦି (!ERRNO_IS_NOT_SUPPORTED(r))
                ଫେରନ୍ତୁ -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, ଖାଲି, &attr, argv, envp);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ -r;

        r = pidref_set_pid(ret_pidref, pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_dir_open(DIR **ret) {
        DIR *d;

        ପ୍ରତିଜ୍ଞା(ret);

        d = opendir("/proc");
        ଯଦି (!d)
                ଫେରନ୍ତୁ -errno;

        *ret = d;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_dir_read(DIR *d, pid_t *ret) {
        ପ୍ରତିଜ୍ଞା(d);

        ପାଇଁ (;;) {
                ସଂରଚନା dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                ଯଦି (!de) {
                        ଯଦି (errno != 0)
                                ଫେରନ୍ତୁ -errno;

                        ଭାଙ୍ଗ;
                }

                ଯଦି (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        ଜାରି;

                ଯଦି (parse_pid(de->d_name, ret) >= 0)
                        ଫେରନ୍ତୁ 1;
        }

        ଯଦି (ret)
                *ret = 0;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_dir_read_pidref(DIR *d, PidRef *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(d);

        ପାଇଁ (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                r = pidref_set_pid(ret, pid);
                ଯଦି (r == -ESRCH) /* gone by now? skip it */
                        ଜାରି;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ 1;
        }

        ଯଦି (ret)
                *ret = PIDREF_NULL;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_mlockall(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        ଯଦି (mlockall(flags) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, INT_MAX);

_noreturn_ ଶୂନ୍ୟ report_errno_and_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା error) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (error >= 0)
                _exit(EXIT_SUCCESS);

        ପ୍ରତିଜ୍ଞା(errno_fd >= 0);

        r = loop_write(errno_fd, &error, ଆକାର(error));
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_errno(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, ଆକାର(r), /* do_poll= */ false);
        ଯଦି (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                ଫେରନ୍ତୁ -EIO;
        }
        ଯଦି (n == 0) /* the process exited without reporting an error, assuming success */
                ଫେରନ୍ତୁ 0;
        ଯଦି (n != ଆକାର(r))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        ଯଦି (r == 0)
                ଫେରନ୍ତୁ 0;
        ଯଦି (r < 0) /* child process reported an error, return it */
                ଫେରନ୍ତୁ log_debug_errno(r, "Child process failed with errno: %m");

        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prctl_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        ଫେରନ୍ତୁ RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_set_comm(ଅଚଳ ଅକ୍ଷର *comm) {
        ଫେରନ୍ତୁ prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_set_nnp(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ NSEC_INFINITY;

        ଫେରନ୍ତୁ r;
}
