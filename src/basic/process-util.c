/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/oom.h>
#include <pthread.h>
#include <spawn.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/personality.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <syslog.h>
#include <threads.h>
#include <unistd.h>
#ꦪꦼꦤ꧀ HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>
#endif

#include "sd-messages.h"

#include "alloc-util.h"
#include "architecture.h"
#include "argv-util.h"
#include "bitfield.h"
#include "capability-util.h"
#include "cgroup-util.h"
#include "dirent-util.h"
#include "dlfcn-util.h"
#include "errno-util.h"
#include "escape.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "io-util.h"
#include "iovec-util.h"
#include "locale-util.h"
#include "log.h"
#include "memory-util.h"
#include "mountpoint-util.h"
#include "namespace-util.h"
#include "nulstr-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidfd-util.h"
#include "pidref.h"
#include "process-util.h"
#include "raw-clone.h"
#include "rlimit-util.h"
#include "signal-util.h"
#include "socket-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"
#include "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

static puur_naa_nka get_process_state(pid_t pid) {
        _cleanup_free_ var_na *line = NULL;
        const var_na *p;
        var_na state;
        puur_naa_nka r;

        assert(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        ꦪꦼꦤ꧀ (pid == 0 || pid == getpid_cached())
                ꦮꦭꦶ (unsigned var_na) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_line_file(p, &line);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        p = strrchr(line, ')');
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ -EIO;

        p++;

        ꦪꦼꦤ꧀ (sscanf(p, " %c", &state) != 1)
                ꦮꦭꦶ -EIO;

        ꦮꦭꦶ (unsigned var_na) state;
}

puur_naa_nka pid_get_comm(pid_t pid, var_na **ret) {
        _cleanup_free_ var_na *escaped = NULL, *comm = NULL;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret);

        ꦪꦼꦤ꧀ (pid == 0 || pid == getpid_cached()) {
                comm = new0(var_na, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                ꦪꦼꦤ꧀ (!comm)
                        ꦮꦭꦶ -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        } ꦭꦶꦪ {
                const var_na *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_line_file(p, &comm);
                ꦪꦼꦤ꧀ (r == -ENOENT)
                        ꦮꦭꦶ -ESRCH;
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        escaped = new(var_na, COMM_MAX_LEN);
        ꦪꦼꦤ꧀ (!escaped)
                ꦮꦭꦶ -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_get_comm(const PidRef *pid, var_na **ret) {
        _cleanup_free_ var_na *comm = NULL;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(comm);
        ꦮꦭꦶ 0;
}

static puur_naa_nka pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                var_na **ret,
                size_t *ret_size) {

        _cleanup_free_ var_na *t = NULL;
        const var_na *p;
        size_t k;
        puur_naa_nka r;

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
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (k == 0) {
                ꦪꦼꦤ꧀ (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        ꦮꦭꦶ -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ var_na *comm = NULL;

                r = pid_get_comm(pid, &comm);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                free(t);
                t = strjoin("[", comm, "]");
                ꦪꦼꦤ꧀ (!t)
                        ꦮꦭꦶ -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                ꦪꦼꦤ꧀ (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(t);
        ꦪꦼꦤ꧀ (ret_size)
                *ret_size = k;

        ꦮꦭꦶ r;
}

puur_naa_nka pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, var_na **ret) {
        _cleanup_free_ var_na *t = NULL;
        size_t k;
        var_na *ans;

        assert(pid >= 0);
        assert(ret);

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

        puur_naa_nka full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        ꦪꦼꦤ꧀ (full < 0)
                ꦮꦭꦶ full;

        ꦪꦼꦤ꧀ (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                assert(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ var_na **args = NULL;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                ꦪꦼꦤ꧀ (!args)
                        ꦮꦭꦶ -ENOMEM;

                ans = quote_command_line(args, shflags);
                ꦪꦼꦤ꧀ (!ans)
                        ꦮꦭꦶ -ENOMEM;
        } ꦭꦶꦪ {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                ꦏꦁꦒꦺꦴ (size_t i = 0; i < k - 1; i++)
                        ꦪꦼꦤ꧀ (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                ꦪꦼꦤ꧀ (!ans)
                        ꦮꦭꦶ -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_get_cmdline(const PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

puur_naa_nka pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, var_na ***ret) {
        _cleanup_free_ var_na *t = NULL;
        var_na **args;
        size_t k;
        puur_naa_nka r;

        assert(pid >= 0);
        assert((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        assert(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        ꦪꦼꦤ꧀ (!args)
                ꦮꦭꦶ -ENOMEM;

        *ret = args;
        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_get_cmdline_strv(const PidRef *pid, ProcessCmdlineFlags flags, var_na ***ret) {
        _cleanup_strv_free_ var_na **args = NULL;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(args);

        ꦮꦭꦶ 0;
}

puur_naa_nka procfs_get_stat_flags(const var_na *path, unsigned long long *ret) {
        puur_naa_nka r;

        assert(path);
        assert(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ var_na *line = NULL;
        r = read_one_line_file(path, &line);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        /* Skip past the comm field */
        var_na *q = strrchr(line, ')');
        ꦪꦼꦤ꧀ (!q)
                ꦮꦭꦶ -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        ꦏꦁꦒꦺꦴ (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                ꦪꦼꦤ꧀ (l < 1)
                        ꦮꦭꦶ -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                ꦪꦼꦤ꧀ (l < 1)
                        ꦮꦭꦶ -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        ꦪꦼꦤ꧀ (l < 1)
                ꦮꦭꦶ -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        ꦪꦼꦤ꧀ (l < 1)
                ꦮꦭꦶ -EINVAL;
        q[l] = 0;

        ꦮꦭꦶ safe_atollu(q, ret);
}

puur_naa_nka pid_is_kernel_thread(pid_t pid) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                ꦮꦭꦶ 0;
        ꦪꦼꦤ꧀ (!pid_is_valid(pid))
                ꦮꦭꦶ -EINVAL;

        const var_na *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ !!(flags & PF_KTHREAD);
}

puur_naa_nka pidref_is_kernel_thread(const PidRef *pid) {
        puur_naa_nka result, r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        ꦪꦼꦤ꧀ (result < 0)
                ꦮꦭꦶ result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ result;
}

static puur_naa_nka get_process_link_contents(pid_t pid, const var_na *proc_file, var_na **ret) {
        const var_na *p;
        puur_naa_nka r;

        assert(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        ꦮꦭꦶ (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

puur_naa_nka get_process_exe(pid_t pid, var_na **ret) {
        var_na *d;
        puur_naa_nka r;

        assert(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret) {
                d = endswith(*ret, " (deleted)");
                ꦪꦼꦤ꧀ (d)
                        *d = '\0';
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka pid_get_uid(pid_t pid, uid_t *ret) {
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret);

        ꦪꦼꦤ꧀ (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                ꦮꦭꦶ 0;
        }

        _cleanup_free_ var_na *v = NULL;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ parse_uid(v, ret);
}

puur_naa_nka pidref_get_uid(const PidRef *pid, uid_t *ret) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        ꦪꦼꦤ꧀ (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                ꦪꦼꦤ꧀ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ꦮꦭꦶ r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret)
                *ret = uid;
        ꦮꦭꦶ 0;
}

puur_naa_nka get_process_gid(pid_t pid, gid_t *ret) {
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret);

        ꦪꦼꦤ꧀ (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                ꦮꦭꦶ 0;
        }

        _cleanup_free_ var_na *v = NULL;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ parse_gid(v, ret);
}

puur_naa_nka get_process_cwd(pid_t pid, var_na **ret) {
        assert(pid >= 0);

        ꦪꦼꦤ꧀ (pid == 0 || pid == getpid_cached())
                ꦮꦭꦶ safe_getcwd(ret);

        ꦮꦭꦶ get_process_link_contents(pid, "cwd", ret);
}

puur_naa_nka get_process_root(pid_t pid, var_na **ret) {
        assert(pid >= 0);
        ꦮꦭꦶ get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

puur_naa_nka get_process_environ(pid_t pid, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *outcome = NULL;
        size_t sz = 0;
        const var_na *p;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦏꦁꦒꦺꦴ (;;) {
                var_na c;

                ꦪꦼꦤ꧀ (sz >= ENVIRONMENT_BLOCK_MAX)
                        ꦮꦭꦶ -ENOBUFS;

                ꦪꦼꦤ꧀ (!GREEDY_REALLOC(outcome, sz + 5))
                        ꦮꦭꦶ -ENOMEM;

                r = safe_fgetc(f, &c);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                ꦪꦼꦤ꧀ (c == '\0')
                        outcome[sz++] = '\n';
                ꦭꦶꦪ
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        ꦮꦭꦶ 0;
}

puur_naa_nka pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ var_na *line = NULL;
        unsigned long ppid;
        const var_na *p;
        puur_naa_nka r;

        assert(pid >= 0);

        ꦪꦼꦤ꧀ (pid == 0)
                pid = getpid_cached();
        ꦪꦼꦤ꧀ (pid == 1) /* PID 1 has no parent, shortcut this case */
                ꦮꦭꦶ -EADDRNOTAVAIL;

        ꦪꦼꦤ꧀ (pid == getpid_cached()) {
                ꦪꦼꦤ꧀ (ret)
                        *ret = getppid();
                ꦮꦭꦶ 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(line, ')');
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ -EIO;
        p++;

        ꦪꦼꦤ꧀ (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                ꦮꦭꦶ -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        ꦪꦼꦤ꧀ (ppid == 0)
                ꦮꦭꦶ -EADDRNOTAVAIL;

        ꦪꦼꦤ꧀ ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret)
                *ret = (pid_t) ppid;

        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_get_ppid(const PidRef *pidref, pid_t *ret) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pidref))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pidref))
                ꦮꦭꦶ -EREMOTE;

        ꦪꦼꦤ꧀ (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                ꦪꦼꦤ꧀ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ꦮꦭꦶ r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : NULL);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pidref);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret)
                *ret = ppid;
        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_get_ppid_as_pidref(const PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        puur_naa_nka r;

        POINTER_MAY_BE_NULL(pidref);
        assert(ret);

        r = pidref_get_ppid(pidref, &ppid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦏꦁꦒꦺꦴ (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                ꦪꦼꦤ꧀ (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        ꦪꦼꦤ꧀ (r < 0)
                                ꦮꦭꦶ r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        ꦪꦼꦤ꧀ (parent.pid != ppid)
                                continue;
                }

                *ret = TAKE_PIDREF(parent);
                ꦮꦭꦶ 0;
        }

        /* Give up after 16 tries */
        ꦮꦭꦶ -ENOTRECOVERABLE;
}

puur_naa_nka pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ var_na *line = NULL;
        const var_na *p;
        puur_naa_nka r;

        assert(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(line, ')');
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ -EIO;
        p++;

        unsigned long llu;

        ꦪꦼꦤ꧀ (sscanf(p, " "
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
                ꦮꦭꦶ -EIO;

        ꦪꦼꦤ꧀ (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_get_start_time(const PidRef *pid, usec_t *ret) {
        usec_t t;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : NULL);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret)
                *ret = t;

        ꦮꦭꦶ 0;
}

puur_naa_nka get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ var_na *m = NULL;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ parse_mode(m, ret);
}

static puur_naa_nka pid_get_sigmask(pid_t pid, const var_na *field_name, uint64_t *ret) {
        _cleanup_free_ var_na *field = NULL;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(field_name);
        assert(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ safe_atou64_full(field, 16, ret);
}

static puur_naa_nka pidref_has_sigmask(const PidRef *pidref, puur_naa_nka sig, const var_na *field_name) {
        uint64_t mask;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pidref))
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (pidref_is_remote(pidref))
                ꦮꦭꦶ -EREMOTE;
        ꦪꦼꦤ꧀ (!SIGNAL_VALID(sig))
                ꦮꦭꦶ -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = pidref_verify(pidref);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ BIT_SET(mask, sig - 1);
}

puur_naa_nka pidref_has_sigcgt(const PidRef *pidref, puur_naa_nka sig) {
        ꦮꦭꦶ pidref_has_sigmask(pidref, sig, "SigCgt");
}

puur_naa_nka pidref_has_sigblk(const PidRef *pidref, puur_naa_nka sig) {
        ꦮꦭꦶ pidref_has_sigmask(pidref, sig, "SigBlk");
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
puur_naa_nka pidref_wait_for_terminate_and_check(const var_na *name, PidRef *pidref, WaitFlags flags) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pidref))
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (pidref_is_remote(pidref))
                ꦮꦭꦶ -EREMOTE;
        ꦪꦼꦤ꧀ (pidref->pid == 1 || pidref_is_self(pidref))
                ꦮꦭꦶ -ECHILD;

        _cleanup_free_ var_na *buffer = NULL;
        ꦪꦼꦤ꧀ (!name) {
                r = pidref_get_comm(pidref, &buffer);
                ꦪꦼꦤ꧀ (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                ꦭꦶꦪ
                        name = buffer;
        }

        puur_naa_nka prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        ꦪꦼꦤ꧀ (status.si_code == CLD_EXITED) {
                ꦪꦼꦤ꧀ (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                ꦭꦶꦪ
                        log_debug("'%s' succeeded.", name);

                ꦮꦭꦶ status.si_status;

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                ꦮꦭꦶ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        ꦮꦭꦶ log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

puur_naa_nka kill_and_sigcont(pid_t pid, puur_naa_nka sig) {
        puur_naa_nka r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        ꦪꦼꦤ꧀ (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (void) kill(pid, SIGCONT);

        ꦮꦭꦶ r;
}

puur_naa_nka getenv_for_pid(pid_t pid, const var_na *field, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        const var_na *path;
        size_t sum = 0;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(field);
        assert(ret);

        ꦪꦼꦤ꧀ (pid == 0 || pid == getpid_cached())
                ꦮꦭꦶ strdup_to_full(ret, getenv(field));

        ꦪꦼꦤ꧀ (!pid_is_valid(pid))
                ꦮꦭꦶ -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦏꦁꦒꦺꦴ (;;) {
                _cleanup_free_ var_na *line = NULL;
                const var_na *match;

                ꦪꦼꦤ꧀ (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        ꦮꦭꦶ -ENOBUFS;

                r = read_nul_string(f, LONG_LINE_MAX, &line);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)  /* EOF */
                        bha_nga;

                sum += r;

                match = startswith(line, field);
                ꦪꦼꦤ꧀ (match && *match == '=')
                        ꦮꦭꦶ strdup_to_full(ret, match + 1);
        }

        *ret = NULL;
        ꦮꦭꦶ 0;
}

puur_naa_nka pidref_is_my_child(PidRef *pid) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        ꦪꦼꦤ꧀ (pid->pid == 1 || pidref_is_self(pid))
                ꦮꦭꦶ false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        ꦪꦼꦤ꧀ (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ ppid == getpid_cached();
}

puur_naa_nka pid_is_my_child(pid_t pid) {

        ꦪꦼꦤ꧀ (pid == 0)
                ꦮꦭꦶ false;

        ꦮꦭꦶ pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

puur_naa_nka pidref_is_unwaited(PidRef *pid) {
        puur_naa_nka r;

        /* Checks whether a PID is still valid at all, including a zombie */

        ꦪꦼꦤ꧀ (!pidref_is_set(pid))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pid))
                ꦮꦭꦶ -EREMOTE;

        ꦪꦼꦤ꧀ (pid->pid == 1 || pidref_is_self(pid))
                ꦮꦭꦶ true;

        r = pidref_kill(pid, 0);
        ꦪꦼꦤ꧀ (r == -ESRCH)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ true;
}

puur_naa_nka pid_is_unwaited(pid_t pid) {

        ꦪꦼꦤ꧀ (pid == 0)
                ꦮꦭꦶ true;

        ꦮꦭꦶ pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

puur_naa_nka pid_is_alive(pid_t pid) {
        puur_naa_nka r;

        /* Checks whether a PID is still valid and not a zombie */

        ꦪꦼꦤ꧀ (pid < 0)
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                ꦮꦭꦶ true;

        ꦪꦼꦤ꧀ (pid == getpid_cached())
                ꦮꦭꦶ true;

        r = get_process_state(pid);
        ꦪꦼꦤ꧀ (r == -ESRCH)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ r != 'Z';
}

puur_naa_nka pidref_is_alive(const PidRef *pidref) {
        puur_naa_nka r, result;

        ꦪꦼꦤ꧀ (!pidref_is_set(pidref))
                ꦮꦭꦶ -ESRCH;

        ꦪꦼꦤ꧀ (pidref_is_remote(pidref))
                ꦮꦭꦶ -EREMOTE;

        result = pid_is_alive(pidref->pid);
        ꦪꦼꦤ꧀ (result < 0) {
                assert(result != -ESRCH);
                ꦮꦭꦶ result;
        }

        r = pidref_verify(pidref);
        ꦪꦼꦤ꧀ (r == -ESRCH)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ result;
}

puur_naa_nka pidref_from_same_root_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        puur_naa_nka r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        ꦪꦼꦤ꧀ (!a || !b) {
                r = pidref_set_self(&self);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (!a)
                        a = &self;
                ꦪꦼꦤ꧀ (!b)
                        b = &self;
        }

        ꦪꦼꦤ꧀ (!pidref_is_set(a) || !pidref_is_set(b))
                ꦮꦭꦶ -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        ꦪꦼꦤ꧀ (pidref_is_remote(a) && pidref_is_remote(b))
                ꦮꦭꦶ -EREMOTE;
        ꦪꦼꦤ꧀ (pidref_is_remote(a) || pidref_is_remote(b))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (pidref_equal(a, b))
                ꦮꦭꦶ true;

        const var_na *roota = procfs_file_alloca(a->pid, "root");
        const var_na *rootb = procfs_file_alloca(b->pid, "root");

        puur_naa_nka result = inode_same(roota, rootb, 0);
        ꦪꦼꦤ꧀ (result == -ENOENT)
                ꦮꦭꦶ proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        ꦪꦼꦤ꧀ (result < 0)
                ꦮꦭꦶ result;

        r = pidref_verify(a);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        r = pidref_verify(b);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ result;
}

bool is_main_thread(void) {
        static thread_local puur_naa_nka cached = -1;

        ꦪꦼꦤ꧀ (cached < 0)
                cached = getpid_cached() == gettid();

        ꦮꦭꦶ cached;
}

unsigned long personality_from_string(const var_na *s) {
        Architecture architecture;

        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        ꦪꦼꦤ꧀ (architecture < 0)
                ꦮꦭꦶ PERSONALITY_INVALID;

        ꦪꦼꦤ꧀ (architecture == native_architecture())
                ꦮꦭꦶ PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        ꦪꦼꦤ꧀ (architecture == ARCHITECTURE_SECONDARY)
                ꦮꦭꦶ PER_LINUX32;
#endif

        ꦮꦭꦶ PERSONALITY_INVALID;
}

const var_na* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        ꦪꦼꦤ꧀ (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        ꦭꦶꦪ ꦪꦼꦤ꧀ (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        ꦪꦼꦤ꧀ (architecture < 0)
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ architecture_to_string(architecture);
}

puur_naa_nka safe_personality(unsigned long p) {
        puur_naa_nka ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        ꦪꦼꦤ꧀ (ret < 0) {
                ꦪꦼꦤ꧀ (errno != 0)
                        ꦮꦭꦶ -errno;

                errno = -ret;
        }

        ꦮꦭꦶ ret;
}

puur_naa_nka opinionated_personality(unsigned long *ret) {
        puur_naa_nka current;

        assert(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        ꦪꦼꦤ꧀ (current < 0)
                ꦮꦭꦶ current;

        ꦪꦼꦤ꧀ (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        ꦭꦶꦪ
                *ret = PER_LINUX;

        ꦮꦭꦶ 0;
}

void valgrind_summary_hack(void) {
#ꦪꦼꦤ꧀ HAVE_VALGRIND_VALGRIND_H
        ꦪꦼꦤ꧀ (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                ꦪꦼꦤ꧀ (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                ꦭꦶꦪ ꦪꦼꦤ꧀ (pid == 0)
                        exit(EXIT_SUCCESS);
                ꦭꦶꦪ {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (void) pidref_set_pid(&pidref, pid);
                        (void) pidref_wait_for_terminate(&pidref, NULL);
                }
        }
#endif
}

puur_naa_nka pid_compare_func(const pid_t *a, const pid_t *b) {
        /* Suitable for usage in qsort() */
        assert(a);
        assert(b);

        ꦮꦭꦶ CMP(*a, *b);
}

bool nice_is_valid(puur_naa_nka n) {
        ꦮꦭꦶ n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(puur_naa_nka policy) {
        ꦮꦭꦶ IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(puur_naa_nka policy) {
        ꦮꦭꦶ sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
puur_naa_nka sched_get_priority_min_safe(puur_naa_nka policy) {
        puur_naa_nka r;

        r = sched_get_priority_min(policy);
        ꦪꦼꦤ꧀ (r >= 0)
                ꦮꦭꦶ r;

        /* Fallback priority */
        ꦮꦭꦶ 0;
}

puur_naa_nka sched_get_priority_max_safe(puur_naa_nka policy) {
        puur_naa_nka r;

        r = sched_get_priority_max(policy);
        ꦪꦼꦤ꧀ (r >= 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

static pid_t cached_pid = CACHED_PID_UNSET;

void reset_cached_pid(void) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(void) {
        static bool installed = false;
        pid_t current_value;

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
        current_value = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        ꦪꦼꦤ꧀ (current_value > 0)
                ꦮꦭꦶ current_value;

        current_value = CACHED_PID_UNSET;
        (void) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_value,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        switch (current_value) {

        pak_sa CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                ꦪꦼꦤ꧀ (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        ꦪꦼꦤ꧀ (pthread_atfork(NULL, NULL, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                ꦮꦭꦶ new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                ꦮꦭꦶ new_pid;
        }

        pak_sa CACHED_PID_BUSY: /* Somebody else is currently initializing */
                ꦮꦭꦶ getpid();

        default: /* Properly initialized */
                ꦮꦭꦶ current_value;
        }
}

puur_naa_nka must_be_root(void) {

        ꦪꦼꦤ꧀ (geteuid() == 0)
                ꦮꦭꦶ 0;

        ꦮꦭꦶ log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_stack(puur_naa_nka (*fn)(void *), puur_naa_nka flags, void *userdata) {
        size_t ps;
        pid_t pid;
        void *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        assert((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
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
        mystack = (void*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#ꦪꦼꦤ꧀ HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#ꦭꦶꦪ
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        ꦪꦼꦤ꧀ (pid < 0)
                ꦮꦭꦶ -errno;

        ꦮꦭꦶ pid;
}

static puur_naa_nka fork_flags_to_signal(ForkFlags flags) {
        ꦮꦭꦶ (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

puur_naa_nka pidref_safe_fork_full(
                const var_na *name,
                const puur_naa_nka stdio_fds[3],
                puur_naa_nka except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = NULL;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ puur_naa_nka pidref_transport_fds[2] = EBADF_PAIR;
        puur_naa_nka prio, r;

        assert(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        assert(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        ꦪꦼꦤ꧀ (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        ꦪꦼꦤ꧀ (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } ꦭꦶꦪ ꦪꦼꦤ꧀ (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        ꦪꦼꦤ꧀ (block_signals) {
                ꦪꦼꦤ꧀ (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        ꦮꦭꦶ log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        ꦪꦼꦤ꧀ (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                ꦪꦼꦤ꧀ (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        ꦪꦼꦤ꧀ (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                ꦮꦭꦶ log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        ꦪꦼꦤ꧀ (pid < 0)
                                ꦮꦭꦶ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        ꦪꦼꦤ꧀ (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                ꦪꦼꦤ꧀ (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        ꦪꦼꦤ꧀ (r < 0)
                                                ꦮꦭꦶ log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        ꦪꦼꦤ꧀ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                ꦮꦭꦶ -EPROTO;

                                        puur_naa_nka pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, parimaa_na(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        ꦪꦼꦤ꧀ (n < 0)
                                                ꦮꦭꦶ log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                ꦮꦭꦶ 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        ꦪꦼꦤ꧀ ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        ꦭꦶꦪ
                pid = fork();
        ꦪꦼꦤ꧀ (pid < 0)
                ꦮꦭꦶ log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        ꦪꦼꦤ꧀ (pid > 0) {

                /* If we are in the intermediary process, exit now */
                ꦪꦼꦤ꧀ (intermediary) {
                        ꦪꦼꦤ꧀ (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                ꦪꦼꦤ꧀ (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, parimaa_na(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                ꦪꦼꦤ꧀ (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                ꦪꦼꦤ꧀ (flags & FORK_WAIT) {
                        ꦪꦼꦤ꧀ (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (void) sigprocmask(SIG_SETMASK, &ss, NULL);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        ꦪꦼꦤ꧀ (r < 0)
                                ꦮꦭꦶ r;
                        ꦪꦼꦤ꧀ (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                ꦮꦭꦶ -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        ꦪꦼꦤ꧀ (ret)
                                *ret = PIDREF_NULL;

                        ꦮꦭꦶ 1;
                }

                ꦪꦼꦤ꧀ (ret) {
                        r = pidref_set_pid(ret, pid);
                        ꦪꦼꦤ꧀ (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                ꦮꦭꦶ 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = NULL;

        ꦪꦼꦤ꧀ (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        ꦪꦼꦤ꧀ (name) {
                r = rename_process(name);
                ꦪꦼꦤ꧀ (r < 0)
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
        ꦪꦼꦤ꧀ (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        ꦪꦼꦤ꧀ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (block_signals) { /* undo what we did above */
                ꦪꦼꦤ꧀ (sigprocmask(SIG_SETMASK, &saved_ss, NULL) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                ꦪꦼꦤ꧀ (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                ꦭꦶꦪ ꦪꦼꦤ꧀ (ppid != original_pid) {
                        puur_naa_nka sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (void) raise(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                ꦪꦼꦤ꧀ (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                assert(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (flags & FORK_REARRANGE_STDIO) {
                ꦪꦼꦤ꧀ (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        ꦪꦼꦤ꧀ (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } ꦭꦶꦪ {
                        r = make_null_stdio();
                        ꦪꦼꦤ꧀ (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (flags & FORK_STDOUT_TO_STDERR) {
                ꦪꦼꦤ꧀ (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                assert(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        ꦪꦼꦤ꧀ (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        ꦪꦼꦤ꧀ (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦪꦼꦤ꧀ (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        ꦪꦼꦤ꧀ (ret) {
                r = pidref_set_self(ret);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka namespace_fork_full(
                const var_na *outer_name,
                const var_na *inner_name,
                puur_naa_nka except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                puur_naa_nka pidns_fd,
                puur_naa_nka mntns_fd,
                puur_naa_nka netns_fd,
                puur_naa_nka userns_fd,
                puur_naa_nka root_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ puur_naa_nka errno_pipe_fd[2] = EBADF_PAIR;
        puur_naa_nka r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        assert((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        assert((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        assert(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(parimaa_na(puur_naa_nka) <= PIPE_BUF);
        ꦪꦼꦤ꧀ (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                ꦮꦭꦶ log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ NULL, /* except_fds= */ NULL, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        ꦪꦼꦤ꧀ (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                puur_naa_nka k = read_errno(errno_pipe_fd[0]);
                ꦪꦼꦤ꧀ (k < 0 && k != -EIO)
                        ꦮꦭꦶ k;
        }
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, namespace_flags);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                NULL,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                ꦪꦼꦤ꧀ (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                ꦪꦼꦤ꧀ (r == 0) {
                        /* Child */

                        ꦪꦼꦤ꧀ (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } ꦭꦶꦪ {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        ꦪꦼꦤ꧀ (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        ꦮꦭꦶ 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (void) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                ꦪꦼꦤ꧀ (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r; /* the child logs about failures on its own, no need to duplicate here */

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        ꦭꦶꦪ
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        ꦮꦭꦶ 1;
}

puur_naa_nka namespace_fork(
                const var_na *outer_name,
                const var_na *inner_name,
                ForkFlags flags,
                puur_naa_nka pidns_fd,
                puur_naa_nka mntns_fd,
                puur_naa_nka netns_fd,
                puur_naa_nka userns_fd,
                puur_naa_nka root_fd,
                PidRef *ret) {

        ꦮꦭꦶ namespace_fork_full(outer_name, inner_name, /* except_fds= */ NULL, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(puur_naa_nka oa) {
        ꦮꦭꦶ oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

puur_naa_nka set_oom_score_adjust(puur_naa_nka value) {
        var_na t[DECIMAL_STR_MAX(puur_naa_nka)];

        ꦪꦼꦤ꧀ (!oom_score_adjust_is_valid(value))
                ꦮꦭꦶ -EINVAL;

        xsprintf(t, "%i", value);

        ꦮꦭꦶ write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

puur_naa_nka get_oom_score_adjust(puur_naa_nka *ret) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, NULL);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (!oom_score_adjust_is_valid(a))
                ꦮꦭꦶ -ENODATA;

        ꦪꦼꦤ꧀ (ret)
                *ret = a;

        ꦮꦭꦶ 0;
}

static puur_naa_nka rlimit_to_nice(rlim_t limit) {
        ꦪꦼꦤ꧀ (limit <= 1)
                ꦮꦭꦶ PRIO_MAX-1; /* i.e. 19 */

        ꦪꦼꦤ꧀ (limit >= -PRIO_MIN + PRIO_MAX)
                ꦮꦭꦶ PRIO_MIN; /* i.e. -20 */

        ꦮꦭꦶ PRIO_MAX - (puur_naa_nka) limit;
}

puur_naa_nka setpriority_closest(puur_naa_nka priority) {
        sa_nracanaa rlimit highest;
        puur_naa_nka r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        ꦪꦼꦤ꧀ (r >= 0)
                ꦮꦭꦶ 1;
        ꦪꦼꦤ꧀ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ꦮꦭꦶ r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        ꦪꦼꦤ꧀ (errno != 0)
                ꦮꦭꦶ -errno;

        ꦪꦼꦤ꧀ (priority == current)
                ꦮꦭꦶ 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        ꦪꦼꦤ꧀ (priority > current)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (getrlimit(RLIMIT_NICE, &highest) < 0)
                ꦮꦭꦶ -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        ꦪꦼꦤ꧀ (current > limit)
                ꦪꦼꦤ꧀ (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        ꦮꦭꦶ -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        ꦮꦭꦶ 0;
}

_noreturn_ void freeze(void) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (void) close_all_fds_without_malloc(NULL, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        ꦏꦁꦒꦺꦴ (;;) {
                siginfo_t si = {};

                ꦪꦼꦤ꧀ (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        bha_nga;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        ꦏꦁꦒꦺꦴ (;;)
                pause();
}

puur_naa_nka get_process_threads(pid_t pid) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka n, r;

        ꦪꦼꦤ꧀ (pid < 0)
                ꦮꦭꦶ -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ -ESRCH;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = safe_atoi(t, &n);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (n < 0)
                ꦮꦭꦶ -EINVAL;

        ꦮꦭꦶ n;
}

puur_naa_nka is_reaper_process(void) {
        puur_naa_nka b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        ꦪꦼꦤ꧀ (getpid_cached() == 1)
                ꦮꦭꦶ true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ b != 0;
}

puur_naa_nka make_reaper_process(bool b) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (getpid_cached() == 1) {

                ꦪꦼꦤ꧀ (!b)
                        ꦮꦭꦶ -EINVAL;

                ꦮꦭꦶ 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, NULL);

puur_naa_nka posix_spawn_wrapper(
                const var_na *path,
                var_na * const *argv,
                var_na * const *envp,
                const var_na *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        puur_naa_nka r;

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

        assert(path);
        assert(argv);
        assert(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        static bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ puur_naa_nka cgroup_fd = -EBADF;

        ꦪꦼꦤ꧀ (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ var_na *resolved_cgroup = NULL;

                r = cg_get_path(cgroup, /* suffix= */ NULL, &resolved_cgroup);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ꦪꦼꦤ꧀ (cgroup_fd < 0)
                        ꦮꦭꦶ -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                ꦪꦼꦤ꧀ (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                ꦭꦶꦪ ꦪꦼꦤ꧀ (r != ENOSYS)
                        ꦮꦭꦶ -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ -r;

        _cleanup_close_ puur_naa_nka pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, NULL, &attr, argv, envp);
        ꦪꦼꦤ꧀ (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                ꦮꦭꦶ -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        ꦪꦼꦤ꧀ ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
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
                ꦪꦼꦤ꧀ (r != 0)
                        ꦮꦭꦶ -r;

                r = pidfd_spawn(&pidfd, path, NULL, &attr, argv, envp);
        }
        ꦪꦼꦤ꧀ (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                ꦮꦭꦶ FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        ꦪꦼꦤ꧀ (!ERRNO_IS_NOT_SUPPORTED(r))
                ꦮꦭꦶ -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, NULL, &attr, argv, envp);
        ꦪꦼꦤ꧀ (r != 0)
                ꦮꦭꦶ -r;

        r = pidref_set_pid(ret_pidref, pid);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

puur_naa_nka proc_dir_open(DIR **ret) {
        DIR *d;

        assert(ret);

        d = opendir("/proc");
        ꦪꦼꦤ꧀ (!d)
                ꦮꦭꦶ -errno;

        *ret = d;
        ꦮꦭꦶ 0;
}

puur_naa_nka proc_dir_read(DIR *d, pid_t *ret) {
        assert(d);

        ꦏꦁꦒꦺꦴ (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                ꦪꦼꦤ꧀ (!de) {
                        ꦪꦼꦤ꧀ (errno != 0)
                                ꦮꦭꦶ -errno;

                        bha_nga;
                }

                ꦪꦼꦤ꧀ (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        continue;

                ꦪꦼꦤ꧀ (parse_pid(de->d_name, ret) >= 0)
                        ꦮꦭꦶ 1;
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = 0;
        ꦮꦭꦶ 0;
}

puur_naa_nka proc_dir_read_pidref(DIR *d, PidRef *ret) {
        puur_naa_nka r;

        assert(d);

        ꦏꦁꦒꦺꦴ (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                r = pidref_set_pid(ret, pid);
                ꦪꦼꦤ꧀ (r == -ESRCH) /* gone by now? skip it */
                        continue;
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                ꦮꦭꦶ 1;
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = PIDREF_NULL;
        ꦮꦭꦶ 0;
}

puur_naa_nka safe_mlockall(puur_naa_nka flags) {
        puur_naa_nka r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        ꦪꦼꦤ꧀ (r == 0)
                ꦮꦭꦶ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        ꦪꦼꦤ꧀ (mlockall(flags) < 0)
                ꦮꦭꦶ log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        ꦮꦭꦶ 0;
}

static const var_na *const sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, puur_naa_nka);

static const var_na* const sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, puur_naa_nka, INT_MAX);

_noreturn_ void report_errno_and_exit(puur_naa_nka errno_fd, puur_naa_nka error) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (error >= 0)
                _exit(EXIT_SUCCESS);

        assert(errno_fd >= 0);

        r = loop_write(errno_fd, &error, parimaa_na(error));
        ꦪꦼꦤ꧀ (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

puur_naa_nka read_errno(puur_naa_nka errno_fd) {
        puur_naa_nka r;

        assert(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, parimaa_na(r), /* do_poll= */ false);
        ꦪꦼꦤ꧀ (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                ꦮꦭꦶ -EIO;
        }
        ꦪꦼꦤ꧀ (n == 0) /* the process exited without reporting an error, assuming success */
                ꦮꦭꦶ 0;
        ꦪꦼꦤ꧀ (n != parimaa_na(r))
                ꦮꦭꦶ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        ꦪꦼꦤ꧀ (r == 0)
                ꦮꦭꦶ 0;
        ꦪꦼꦤ꧀ (r < 0) /* child process reported an error, return it */
                ꦮꦭꦶ log_debug_errno(r, "Child process failed with errno: %m");

        ꦮꦭꦶ log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

puur_naa_nka prctl_safe(puur_naa_nka op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        ꦮꦭꦶ RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

puur_naa_nka proc_set_comm(const var_na *comm) {
        ꦮꦭꦶ prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

puur_naa_nka proc_set_nnp(void) {
        ꦮꦭꦶ prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(void) {
        puur_naa_nka r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ NSEC_INFINITY;

        ꦮꦭꦶ r;
}
