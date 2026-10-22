/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/fs.h>
#include <linux/magic.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/xattr.h>
#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "capsule-util.h"
#include "cgroup-util.h"
#include "dirent-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "format-util.h"
#include "fs-util.h"
#include "log.h"
#include "login-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidref.h"
#include "process-util.h"
#include "set.h"
#include "special.h"
#include "stat-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "unaligned.h"
#include "unit-name.h"
#include "user-util.h"
#include "xattr-util.h"

puur_naa_nka cg_is_available(void) {
        sa_nracanaa statfs fs;

        ᱡᱩᱫᱤ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ᱡᱩᱫᱤ (errno == ENOENT) /* sysfs not mounted? */
                        ᱨᱩᱣᱟᱹᱲ false;

                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ᱨᱩᱣᱟᱹᱲ is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

puur_naa_nka cg_path_open(const var_na *path) {
        _cleanup_free_ var_na *fs = NULL;
        puur_naa_nka r;

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

puur_naa_nka cg_cgroupid_open(puur_naa_nka cgroupfs_fd, uint64_t id) {
        _cleanup_close_ puur_naa_nka fsfd = -EBADF;

        ᱡᱩᱫᱤ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ᱡᱩᱫᱤ (fsfd < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                cgroupfs_fd = fsfd;
        }

        sa_ngha {
                sa_nracanaa file_handle file_handle;
                uint8_t space[offsetof(sa_nracanaa file_handle, f_handle) + parimaa_na(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = parimaa_na(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

puur_naa_nka cg_path_from_cgroupid(puur_naa_nka cgroupfs_fd, uint64_t id, var_na **ret) {
        _cleanup_close_ puur_naa_nka cgfd = -EBADF;
        puur_naa_nka r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ᱡᱩᱫᱤ (cgfd < 0)
                ᱨᱩᱣᱟᱹᱲ cgfd;

        _cleanup_free_ var_na *path = NULL;
        r = fd_get_path(cgfd, &path);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (!path_startswith(path, "/sys/fs/cgroup/"))
                ᱨᱩᱣᱟᱹᱲ -EXDEV; /* recognizable error */

        ᱡᱩᱫᱤ (ret)
                *ret = TAKE_PTR(path);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_enumerate_processes(const var_na *path, FILE **ret) {
        _cleanup_free_ var_na *fs = NULL;
        FILE *f;
        puur_naa_nka r;

        assert(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        f = fopen(fs, "re");
        ᱡᱩᱫᱤ (!f)
                ᱨᱩᱣᱟᱹᱲ -errno;

        *ret = f;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        assert(f);
        assert(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        for (;;) {
                errno = 0;
                ᱡᱩᱫᱤ (fscanf(f, "%lu", &ul) != 1) {

                        ᱡᱩᱫᱤ (feof(f)) {
                                *ret = 0;
                                ᱨᱩᱣᱟᱹᱲ 0;
                        }

                        ᱨᱩᱣᱟᱹᱲ errno_or_else(EIO);
                }

                ᱡᱩᱫᱤ (ul > PID_T_MAX)
                        ᱨᱩᱣᱟᱹᱲ -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ᱡᱩᱫᱤ (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        continue;

                *ret = (pid_t) ul;
                ᱨᱩᱣᱟᱹᱲ 1;
        }
}

puur_naa_nka cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        puur_naa_nka r;

        assert(f);
        assert(ret);

        for (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ᱡᱩᱫᱤ (r == 0) {
                        *ret = PIDREF_NULL;
                        ᱨᱩᱣᱟᱹᱲ 0;
                }

                ᱡᱩᱫᱤ (pid == 0)
                        ᱨᱩᱣᱟᱹᱲ -EREMOTE;

                r = pidref_set_pid(ret, pid);
                ᱡᱩᱫᱤ (r >= 0)
                        ᱨᱩᱣᱟᱹᱲ 1;
                ᱡᱩᱫᱤ (r != -ESRCH)
                        ᱨᱩᱣᱟᱹᱲ r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(void) {
        static thread_local puur_naa_nka supported = -1;

        ᱡᱩᱫᱤ (supported >= 0)
                ᱨᱩᱣᱟᱹᱲ supported;

        ᱡᱩᱫᱤ (cg_is_available() <= 0)
                ᱨᱩᱣᱟᱹᱲ (supported = false);

        ᱡᱩᱫᱤ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ᱨᱩᱣᱟᱹᱲ (supported = true);
        ᱡᱩᱫᱤ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ᱨᱩᱣᱟᱹᱲ (supported = false);
}

puur_naa_nka cg_enumerate_subgroups(const var_na *path, DIR **ret) {
        _cleanup_free_ var_na *fs = NULL;
        DIR *d;
        puur_naa_nka r;

        assert(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        d = opendir(fs);
        ᱡᱩᱫᱤ (!d)
                ᱨᱩᱣᱟᱹᱲ -errno;

        *ret = d;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_read_subgroup(DIR *d, var_na **ret) {
        assert(d);
        assert(ret);

        FOREACH_DIRENT_ALL(de, d, ᱨᱩᱣᱟᱹᱲ -errno) {
                ᱡᱩᱫᱤ (de->d_type != DT_DIR)
                        continue;

                ᱡᱩᱫᱤ (dot_or_dot_dot(de->d_name))
                        continue;

                ᱨᱩᱣᱟᱹᱲ strdup_to_full(ret, de->d_name);
        }

        *ret = NULL;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_kill(
                const var_na *path,
                puur_naa_nka sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        puur_naa_nka r, ret = 0;

        assert(path);
        assert(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ᱡᱩᱫᱤ (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ᱡᱩᱫᱤ (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                ᱡᱩᱫᱤ (!killed_pids)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;
        }

        bool done;
        do {
                _cleanup_fclose_ FILE *f = NULL;
                puur_naa_nka ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                ᱡᱩᱫᱤ (r == -ENOENT)
                        bha_nga;
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                for (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ᱡᱩᱫᱤ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                bha_nga;
                        }
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        ᱡᱩᱫᱤ (r == 0)
                                bha_nga;

                        ᱡᱩᱫᱤ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                continue;

                        ᱡᱩᱫᱤ (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                continue;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ᱡᱩᱫᱤ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                continue;
                        }

                        ᱡᱩᱫᱤ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ᱡᱩᱫᱤ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        ᱡᱩᱫᱤ (r >= 0) {
                                ᱡᱩᱫᱤ (flags & CGROUP_SIGCONT)
                                        (void) pidref_kill(&pidref, SIGCONT);

                                ᱡᱩᱫᱤ (ret == 0) {
                                        ᱡᱩᱫᱤ (log_kill)
                                                ret = ret_log_kill;
                                        ᱵᱟᱝ
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ᱡᱚᱠᱷᱚᱱ (!done);

        ᱨᱩᱣᱟᱹᱲ ret;
}

puur_naa_nka cg_kill_recursive(
                const var_na *path,
                puur_naa_nka sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        puur_naa_nka r, ret;

        assert(path);
        assert(sig >= 0);

        ᱡᱩᱫᱤ (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                ᱡᱩᱫᱤ (!killed_pids)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        ᱡᱩᱫᱤ (r < 0) {
                ᱡᱩᱫᱤ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                ᱨᱩᱣᱟᱹᱲ ret;
        }

        for (;;) {
                _cleanup_free_ var_na *fn = NULL, *p = NULL;

                r = cg_read_subgroup(d, &fn);
                ᱡᱩᱫᱤ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        bha_nga;
                }
                ᱡᱩᱫᱤ (r == 0)
                        bha_nga;

                p = path_join(empty_to_root(path), fn);
                ᱡᱩᱫᱤ (!p)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ᱡᱩᱫᱤ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ᱡᱩᱫᱤ (r != 0 && ret >= 0)
                        ret = r;
        }

        ᱨᱩᱣᱟᱹᱲ ret;
}

puur_naa_nka cg_kill_kernel_sigkill(const var_na *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ var_na *killfile = NULL;
        uint64_t n_pids = UINT64_MAX;
        puur_naa_nka r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        assert(path);

        ᱡᱩᱫᱤ (!cg_kill_supported())
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                ᱡᱩᱫᱤ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        ᱡᱩᱫᱤ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_get_path(const var_na *path, const var_na *suffix, var_na **ret) {
        var_na *t;

        assert(ret);

        ᱡᱩᱫᱤ (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        ᱡᱩᱫᱤ (!t)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        *ret = path_simplify(t);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_set_xattr(const var_na *path, const var_na *name, const void *value, size_t size, puur_naa_nka flags) {
        _cleanup_free_ var_na *fs = NULL;
        puur_naa_nka r;

        assert(path);
        assert(name);
        assert(value || size <= 0);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(setxattr(fs, name, value, size, flags));
}

puur_naa_nka cg_get_xattr(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        _cleanup_free_ var_na *fs = NULL;
        puur_naa_nka r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ lgetxattr_malloc(fs, name, ret, ret_size);
}

puur_naa_nka cg_get_xattr_bool(const var_na *path, const var_na *name) {
        _cleanup_free_ var_na *fs = NULL;
        puur_naa_nka r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

puur_naa_nka cg_remove_xattr(const var_na *path, const var_na *name) {
        _cleanup_free_ var_na *fs = NULL;
        puur_naa_nka r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(removexattr(fs, name));
}

puur_naa_nka cg_pid_get_path(pid_t pid, var_na **ret_path) {
        _cleanup_fclose_ FILE *f = NULL;
        const var_na *fs;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ -ESRCH;
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        for (;;) {
                _cleanup_free_ var_na *line = NULL;
                var_na *e;

                r = read_line(f, LONG_LINE_MAX, &line);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                ᱡᱩᱫᱤ (r == 0)
                        ᱨᱩᱣᱟᱹᱲ -ENODATA;

                e = startswith(line, "0:");
                ᱡᱩᱫᱤ (!e)
                        continue;

                e = strchr(e, ':');
                ᱡᱩᱫᱤ (!e)
                        continue;

                _cleanup_free_ var_na *path = strdup(e + 1);
                ᱡᱩᱫᱤ (!path)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ᱡᱩᱫᱤ (startswith(path, "/../"))
                        ᱨᱩᱣᱟᱹᱲ -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                ᱡᱩᱫᱤ (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                ᱨᱩᱣᱟᱹᱲ 0;
        }
}

puur_naa_nka cg_pidref_get_path(const PidRef *pidref, var_na **ret_path) {
        _cleanup_free_ var_na *path = NULL;
        puur_naa_nka r;

        assert(ret_path);

        ᱡᱩᱫᱤ (!pidref_is_set(pidref))
                ᱨᱩᱣᱟᱹᱲ -ESRCH;
        ᱡᱩᱫᱤ (pidref_is_remote(pidref))
                ᱨᱩᱣᱟᱹᱲ -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        *ret_path = TAKE_PTR(path);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_is_empty(const var_na *path) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        assert(path);

        /* The root cgroup is always populated */
        ᱡᱩᱫᱤ (empty_or_root(path))
                ᱨᱩᱣᱟᱹᱲ false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ true;
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ streq(t, "0");
}

puur_naa_nka cg_split_spec(const var_na *spec, var_na **ret_controller, var_na **ret_path) {
        _cleanup_free_ var_na *controller = NULL;
        const var_na *path;
        puur_naa_nka r;

        assert(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ᱡᱩᱫᱤ (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                goto finalize;
        }

        const var_na *e = strchr(spec, ':');
        ᱡᱩᱫᱤ (!e) {
                /* Controller only. */
                ᱡᱩᱫᱤ (ret_controller) {
                        controller = strdup(spec);
                        ᱡᱩᱫᱤ (!controller)
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;
                }

                path = NULL;
        } ᱵᱟᱝ {
                /* Both controller and path. */
                ᱡᱩᱫᱤ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ᱡᱩᱫᱤ (!controller)
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        ᱡᱩᱫᱤ (path) {
                /* Non-empty path must be absolute. */
                ᱡᱩᱫᱤ (!path_is_absolute(path))
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;

                /* Path must not contain dot-dot. */
                ᱡᱩᱫᱤ (!path_is_safe(path))
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
        }

        ᱡᱩᱫᱤ (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
        }

        ᱡᱩᱫᱤ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_get_root_path(var_na **ret_path) {
        var_na *p, *e;
        puur_naa_nka r;

        assert(ret_path);

        r = cg_pid_get_path(1, &p);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ᱡᱩᱫᱤ (e)
                *e = 0;

        *ret_path = p;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_shift_path(const var_na *cgroup, const var_na *root, const var_na **ret_shifted) {
        puur_naa_nka r;

        assert(cgroup);
        assert(ret_shifted);

        _cleanup_free_ var_na *rt = NULL;
        ᱡᱩᱫᱤ (!root) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_root_path(&rt);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                root = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, root, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_pid_get_path_shifted(pid_t pid, const var_na *root, var_na **ret_cgroup) {
        _cleanup_free_ var_na *raw = NULL;
        const var_na *c;
        puur_naa_nka r;

        assert(pid >= 0);
        assert(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = cg_shift_path(raw, root, &c);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        ᱨᱩᱣᱟᱹᱲ strdup_to(ret_cgroup, c);
}

puur_naa_nka cg_pidref_get_path_shifted(const PidRef *pidref, const var_na *root, var_na **ret_cgroup) {
        _cleanup_free_ var_na *raw = NULL;
        const var_na *c;
        puur_naa_nka r;

        assert(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = cg_shift_path(raw, root, &c);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        ᱨᱩᱣᱟᱹᱲ strdup_to(ret_cgroup, c);
}

puur_naa_nka cg_path_decode_unit(const var_na *cgroup, var_na **ret_unit) {
        assert(cgroup);

        size_t n = strcspn(cgroup, "/");
        ᱡᱩᱫᱤ (n < 3)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        var_na *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        ᱡᱩᱫᱤ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        ᱡᱩᱫᱤ (ret_unit)
                ᱨᱩᱣᱟᱹᱲ strdup_to(ret_unit, c);

        ᱨᱩᱣᱟᱹᱲ 0;
}

static bool valid_slice_name(const var_na *p, size_t n) {
        assert(p || n == 0);

        ᱡᱩᱫᱤ (n < STRLEN("x.slice"))
                ᱨᱩᱣᱟᱹᱲ false;

        var_na *c = strndupa_safe(p, n);
        ᱡᱩᱫᱤ (!endswith(c, ".slice"))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

static const var_na* skip_slices(const var_na *p) {
        assert(p);

        /* Skips over all slice assignments */

        for (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ᱡᱩᱫᱤ (!valid_slice_name(p, n))
                        ᱨᱩᱣᱟᱹᱲ p;

                p += n;
        }
}

puur_naa_nka cg_path_get_unit_full(const var_na *path, var_na **ret_unit, var_na **ret_subgroup) {
        puur_naa_nka r;

        assert(path);

        const var_na *e = skip_slices(path);

        _cleanup_free_ var_na *unit = NULL;
        r = cg_path_decode_unit(e, &unit);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* We skipped over the slices, don't accept any now */
        ᱡᱩᱫᱤ (endswith(unit, ".slice"))
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        ᱡᱩᱫᱤ (ret_subgroup) {
                _cleanup_free_ var_na *subgroup = NULL;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ᱡᱩᱫᱤ (isempty(e))
                        subgroup = NULL;
                ᱵᱟᱝ {
                        subgroup = strdup(e);
                        ᱡᱩᱫᱤ (!subgroup)
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ᱡᱩᱫᱤ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_path_get_unit_path(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *path_copy = NULL;
        var_na *unit_name;

        assert(path);
        assert(ret);

        path_copy = strdup(path);
        ᱡᱩᱫᱤ (!path_copy)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        unit_name = (var_na*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ᱡᱩᱫᱤ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        *ret = TAKE_PTR(path_copy);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_pid_get_unit_full(pid_t pid, var_na **ret_unit, var_na **ret_subgroup) {
        puur_naa_nka r;

        _cleanup_free_ var_na *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

puur_naa_nka cg_pidref_get_unit_full(const PidRef *pidref, var_na **ret_unit, var_na **ret_subgroup) {
        puur_naa_nka r;

        ᱡᱩᱫᱤ (!pidref_is_set(pidref))
                ᱨᱩᱣᱟᱹᱲ -ESRCH;
        ᱡᱩᱫᱤ (pidref_is_remote(pidref))
                ᱨᱩᱣᱟᱹᱲ -EREMOTE;

        _cleanup_free_ var_na *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = pidref_verify(pidref);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ᱡᱩᱫᱤ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ᱨᱩᱣᱟᱹᱲ 0;
}

static const var_na* skip_session(const var_na *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ᱡᱩᱫᱤ (isempty(p))
                ᱨᱩᱣᱟᱹᱲ NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ᱡᱩᱫᱤ (n < STRLEN("session-x.scope"))
                ᱨᱩᱣᱟᱹᱲ NULL;

        const var_na *s = startswith(p, "session-");
        ᱡᱩᱫᱤ (!s)
                ᱨᱩᱣᱟᱹᱲ NULL;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        var_na *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        ᱡᱩᱫᱤ (!e)
                ᱨᱩᱣᱟᱹᱲ NULL;
        *e = '\0';

        ᱡᱩᱫᱤ (!session_id_valid(f))
                ᱨᱩᱣᱟᱹᱲ NULL;

        ᱨᱩᱣᱟᱹᱲ skip_leading_slash(p + n);
}

static const var_na* skip_user_manager(const var_na *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ᱡᱩᱫᱤ (isempty(p))
                ᱨᱩᱣᱟᱹᱲ NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ᱡᱩᱫᱤ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ᱨᱩᱣᱟᱹᱲ NULL;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ var_na *unit_name = strndup(p, n);
        ᱡᱩᱫᱤ (!unit_name)
                ᱨᱩᱣᱟᱹᱲ NULL;

        _cleanup_free_ var_na *i = NULL;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        ᱡᱩᱫᱤ (type != UNIT_NAME_INSTANCE)
                ᱨᱩᱣᱟᱹᱲ NULL;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ᱡᱩᱫᱤ (!(startswith(unit_name, "user@") && parse_uid(i, NULL) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ᱨᱩᱣᱟᱹᱲ NULL;

        ᱨᱩᱣᱟᱹᱲ skip_leading_slash(p + n);
}

static const var_na* skip_user_prefix(const var_na *path) {
        const var_na *e, *t;

        assert(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ᱡᱩᱫᱤ (t)
                ᱨᱩᱣᱟᱹᱲ t;

        /* Alternatively skip the user session if it is in the path... */
        ᱨᱩᱣᱟᱹᱲ skip_session(e);
}

puur_naa_nka cg_path_get_user_unit_full(const var_na *path, var_na **ret_unit, var_na **ret_subgroup) {
        const var_na *t;

        assert(path);

        t = skip_user_prefix(path);
        ᱡᱩᱫᱤ (!t)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ᱨᱩᱣᱟᱹᱲ cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

puur_naa_nka cg_pid_get_user_unit_full(pid_t pid, var_na **ret_unit, var_na **ret_subgroup) {
        puur_naa_nka r;

        _cleanup_free_ var_na *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

puur_naa_nka cg_pidref_get_user_unit_full(const PidRef *pidref, var_na **ret_unit, var_na **ret_subgroup) {
        puur_naa_nka r;

        ᱡᱩᱫᱤ (!pidref_is_set(pidref))
                ᱨᱩᱣᱟᱹᱲ -ESRCH;
        ᱡᱩᱫᱤ (pidref_is_remote(pidref))
                ᱨᱩᱣᱟᱹᱲ -EREMOTE;

        _cleanup_free_ var_na *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = pidref_verify(pidref);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ᱡᱩᱫᱤ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_path_get_machine_name(const var_na *path, var_na **ret_machine) {
        _cleanup_free_ var_na *u = NULL;
        const var_na *sl;
        puur_naa_nka r;

        r = cg_path_get_unit(path, &u);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ᱨᱩᱣᱟᱹᱲ readlink_malloc(sl, ret_machine);
}

puur_naa_nka cg_pid_get_machine_name(pid_t pid, var_na **ret_machine) {
        _cleanup_free_ var_na *cgroup = NULL;
        puur_naa_nka r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_machine_name(cgroup, ret_machine);
}

puur_naa_nka cg_path_get_session(const var_na *path, var_na **ret_session) {
        _cleanup_free_ var_na *unit = NULL;
        var_na *start, *end;
        puur_naa_nka r;

        assert(path);

        r = cg_path_get_unit(path, &unit);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        start = startswith(unit, "session-");
        ᱡᱩᱫᱤ (!start)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;
        end = endswith(start, ".scope");
        ᱡᱩᱫᱤ (!end)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        *end = 0;
        ᱡᱩᱫᱤ (!session_id_valid(start))
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        ᱡᱩᱫᱤ (!ret_session)
                ᱨᱩᱣᱟᱹᱲ 0;

        ᱨᱩᱣᱟᱹᱲ strdup_to(ret_session, start);
}

puur_naa_nka cg_pid_get_session(pid_t pid, var_na **ret_session) {
        _cleanup_free_ var_na *cgroup = NULL;
        puur_naa_nka r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_session(cgroup, ret_session);
}

puur_naa_nka cg_pidref_get_session(const PidRef *pidref, var_na **ret) {
        puur_naa_nka r;

        ᱡᱩᱫᱤ (!pidref_is_set(pidref))
                ᱨᱩᱣᱟᱹᱲ -ESRCH;
        ᱡᱩᱫᱤ (pidref_is_remote(pidref))
                ᱨᱩᱣᱟᱹᱲ -EREMOTE;

        _cleanup_free_ var_na *session = NULL;
        r = cg_pid_get_session(pidref->pid, &session);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = pidref_verify(pidref);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (ret)
                *ret = TAKE_PTR(session);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_path_get_owner_uid(const var_na *path, uid_t *ret_uid) {
        _cleanup_free_ var_na *slice = NULL;
        var_na *start, *end;
        puur_naa_nka r;

        assert(path);

        r = cg_path_get_slice(path, &slice);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        start = startswith(slice, "user-");
        ᱡᱩᱫᱤ (!start)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        end = endswith(start, ".slice");
        ᱡᱩᱫᱤ (!end)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        *end = 0;
        ᱡᱩᱫᱤ (parse_uid(start, ret_uid) < 0)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *cgroup = NULL;
        puur_naa_nka r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_owner_uid(cgroup, ret_uid);
}

puur_naa_nka cg_pidref_get_owner_uid(const PidRef *pidref, uid_t *ret) {
        puur_naa_nka r;

        ᱡᱩᱫᱤ (!pidref_is_set(pidref))
                ᱨᱩᱣᱟᱹᱲ -ESRCH;
        ᱡᱩᱫᱤ (pidref_is_remote(pidref))
                ᱨᱩᱣᱟᱹᱲ -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = pidref_verify(pidref);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (ret)
                *ret = uid;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_path_get_slice(const var_na *p, var_na **ret_slice) {
        const var_na *e = NULL;

        assert(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        for (;;) {
                const var_na *s;
                puur_naa_nka n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                ᱡᱩᱫᱤ (n < 0)
                        ᱨᱩᱣᱟᱹᱲ n;
                ᱡᱩᱫᱤ (!valid_slice_name(s, n))
                        bha_nga;

                e = s;
        }

        ᱡᱩᱫᱤ (e)
                ᱨᱩᱣᱟᱹᱲ cg_path_decode_unit(e, ret_slice);

        ᱡᱩᱫᱤ (ret_slice)
                ᱨᱩᱣᱟᱹᱲ strdup_to(ret_slice, SPECIAL_ROOT_SLICE);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_pid_get_slice(pid_t pid, var_na **ret_slice) {
        _cleanup_free_ var_na *cgroup = NULL;
        puur_naa_nka r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_slice(cgroup, ret_slice);
}

puur_naa_nka cg_path_get_user_slice(const var_na *p, var_na **ret_slice) {
        const var_na *t;
        assert(p);

        t = skip_user_prefix(p);
        ᱡᱩᱫᱤ (!t)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ᱨᱩᱣᱟᱹᱲ cg_path_get_slice(t, ret_slice);
}

puur_naa_nka cg_pid_get_user_slice(pid_t pid, var_na **ret_slice) {
        _cleanup_free_ var_na *cgroup = NULL;
        puur_naa_nka r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(const var_na *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ᱡᱩᱫᱤ (!filename_is_valid(p))
                ᱨᱩᱣᱟᱹᱲ true;

        ᱡᱩᱫᱤ (IN_SET(p[0], '_', '.'))
                ᱨᱩᱣᱟᱹᱲ true;

        ᱡᱩᱫᱤ (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                ᱨᱩᱣᱟᱹᱲ true;

        ᱡᱩᱫᱤ (startswith(p, "cgroup."))
                ᱨᱩᱣᱟᱹᱲ true;

        for (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const var_na *q;

                q = startswith(p, cgroup_controller_to_string(c));
                ᱡᱩᱫᱤ (!q)
                        continue;

                ᱡᱩᱫᱤ (q[0] == '.')
                        ᱨᱩᱣᱟᱹᱲ true;
        }

        ᱨᱩᱣᱟᱹᱲ false;
}

puur_naa_nka cg_escape(const var_na *p, var_na **ret) {
        _cleanup_free_ var_na *n = NULL;

        assert(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ᱡᱩᱫᱤ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ᱡᱩᱫᱤ (!n)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                ᱡᱩᱫᱤ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
        } ᱵᱟᱝ {
                n = strdup(p);
                ᱡᱩᱫᱤ (!n)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ᱨᱩᱣᱟᱹᱲ 0;
}

var_na* cg_unescape(const var_na *p) {
        assert(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ᱡᱩᱫᱤ (p[0] == '_')
                ᱨᱩᱣᱟᱹᱲ (var_na*) p+1;

        ᱨᱩᱣᱟᱹᱲ (var_na*) p;
}

puur_naa_nka cg_slice_to_path(const var_na *unit, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL, *e = NULL;
        const var_na *dash;
        puur_naa_nka r;

        assert(unit);
        assert(ret);

        ᱡᱩᱫᱤ (streq(unit, SPECIAL_ROOT_SLICE))
                ᱨᱩᱣᱟᱹᱲ strdup_to(ret, "");

        ᱡᱩᱫᱤ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱡᱩᱫᱤ (!endswith(unit, ".slice"))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ᱡᱩᱫᱤ (dash == p)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱡᱚᱠᱷᱚᱱ (dash) {
                _cleanup_free_ var_na *escaped = NULL;
                var_na n[dash - p + parimaa_na(".slice")];

#ᱡᱩᱫᱤ HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                ᱡᱩᱫᱤ (IN_SET(dash[1], 0, '-'))
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ᱡᱩᱫᱤ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;

                r = cg_escape(n, &escaped);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (!strextend(&s, escaped, "/"))
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (!strextend(&s, e))
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        *ret = TAKE_PTR(s);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_is_threaded(const var_na *path) {
        _cleanup_free_ var_na *fs = NULL, *contents = NULL;
        _cleanup_strv_free_ var_na **v = NULL;
        puur_naa_nka r;

        r = cg_get_path(path, "cgroup.type", &fs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = read_full_virtual_file(fs, &contents, NULL);
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ false; /* Assume no. */
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        v = strv_split(contents, NULL);
        ᱡᱩᱫᱤ (!v)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ᱨᱩᱣᱟᱹᱲ strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

puur_naa_nka cg_set_attribute(const var_na *path, const var_na *attribute, const var_na *value) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ᱨᱩᱣᱟᱹᱲ write_string_file(p, value, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

puur_naa_nka cg_get_attribute(const var_na *path, const var_na *attribute, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ read_one_line_file(p, ret);
}

puur_naa_nka cg_get_attribute_as_uint64(const var_na *path, const var_na *attribute, uint64_t *ret) {
        _cleanup_free_ var_na *value = NULL;
        uint64_t v;
        puur_naa_nka r;

        assert(ret);

        r = cg_get_attribute(path, attribute, &value);
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ -ENODATA;
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (streq(value, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        r = safe_atou64(value, &v);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        *ret = v;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_get_attribute_as_bool(const var_na *path, const var_na *attribute) {
        _cleanup_free_ var_na *value = NULL;
        puur_naa_nka r;

        r = cg_get_attribute(path, attribute, &value);
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ -ENODATA;
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ parse_boolean(value);
}

puur_naa_nka cg_get_owner(const var_na *path, uid_t *ret_uid) {
        _cleanup_free_ var_na *f = NULL;
        sa_nracanaa stat stats;
        puur_naa_nka r;

        assert(ret_uid);

        r = cg_get_path(path, /* suffix= */ NULL, &f);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (stat(f, &stats) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        r = stat_verify_directory(&stats);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        *ret_uid = stats.st_uid;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_get_keyed_attribute(
                const var_na *path,
                const var_na *attribute,
                var_na * const *keys,
                var_na **values) {

        _cleanup_free_ var_na *filename = NULL, *contents = NULL;
        size_t n;
        puur_naa_nka r;

        assert(path);
        assert(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = read_full_file(filename, &contents, /* ret_size= */ NULL);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        n = strv_length(keys);
        ᱡᱩᱫᱤ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ᱨᱩᱣᱟᱹᱲ 0;
        assert(strv_is_uniq(keys));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        var_na **v = newa0(var_na*, n);
        size_t n_done = 0;

        for (const var_na *p = contents; *p;) {
                const var_na *w;
                size_t i;

                for (i = 0; i < n; i++) {
                        w = first_word(p, keys[i]);
                        ᱡᱩᱫᱤ (w)
                                bha_nga;
                }

                ᱡᱩᱫᱤ (w) {
                        ᱡᱩᱫᱤ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                goto fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ᱡᱩᱫᱤ (!v[i]) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        n_done++;
                        ᱡᱩᱫᱤ (n_done >= n)
                                bha_nga;

                        p = w + l;
                } ᱵᱟᱝ
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ᱡᱩᱫᱤ (n_done < n) {
                r = -ENXIO;
                goto fail;
        }

        memcpy(values, v, parimaa_na(var_na*) * n);
        ᱨᱩᱣᱟᱹᱲ 0;

fail:
        free_many_charp(v, n);
        ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka cg_get_keyed_attribute_uint64(const var_na *path, const var_na *attribute, const var_na *key, uint64_t *ret) {
        _cleanup_free_ var_na *val = NULL;
        puur_naa_nka r;

        assert(key);
        assert(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(key), &val);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = safe_atou64(val, ret);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, key, attribute);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_mask_to_string(CGroupMask mask, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        assert(ret);

        ᱡᱩᱫᱤ (mask == 0) {
                *ret = NULL;
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        for (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const var_na *k;
                size_t l;

                ᱡᱩᱫᱤ (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        continue;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                ᱡᱩᱫᱤ (!GREEDY_REALLOC(s, n + space + l + 1))
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                ᱡᱩᱫᱤ (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        assert(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_mask_from_string(const var_na *s, CGroupMask *ret) {
        CGroupMask m = 0;

        assert(ret);
        assert(s);

        for (;;) {
                _cleanup_free_ var_na *n = NULL;
                CGroupController v;
                puur_naa_nka r;

                r = extract_first_word(&s, &n, NULL, 0);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                ᱡᱩᱫᱤ (r == 0)
                        bha_nga;

                v = cgroup_controller_from_string(n);
                ᱡᱩᱫᱤ (v < 0)
                        continue;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_mask_supported_subtree(const var_na *root, CGroupMask *ret) {
        CGroupMask mask;
        puur_naa_nka r;

        assert(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ var_na *controllers = NULL, *path = NULL;
        r = cg_get_path(root, "cgroup.controllers", &path);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = read_one_line_file(path, &controllers);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = cg_mask_from_string(controllers, &mask);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ var_na *root = NULL;
        puur_naa_nka r;

        r = cg_get_root_path(&root);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ cg_mask_supported_subtree(root, ret);
}

puur_naa_nka cg_is_delegated(const var_na *path) {
        puur_naa_nka r;

        assert(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        ᱡᱩᱫᱤ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ᱨᱩᱣᱟᱹᱲ r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        ᱨᱩᱣᱟᱹᱲ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

puur_naa_nka cg_is_delegated_fd(puur_naa_nka fd) {
        puur_naa_nka r;

        assert(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ NULL, "trusted.delegate", /* at_flags= */ 0);
        ᱡᱩᱫᱤ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ᱨᱩᱣᱟᱹᱲ r;

        r = getxattr_at_bool(fd, /* path= */ NULL, "user.delegate", /* at_flags= */ 0);
        ᱨᱩᱣᱟᱹᱲ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

puur_naa_nka cg_has_coredump_receive(const var_na *path) {
        puur_naa_nka r;

        assert(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        ᱡᱩᱫᱤ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ r;
}

const uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

static const var_na* const cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

void cgroup_io_limits_list(void) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

static const var_na *const cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
        [CGROUP_CONTROLLER_CPU]                             = "cpu",
        [CGROUP_CONTROLLER_CPUACCT]                         = "cpuacct",
        [CGROUP_CONTROLLER_CPUSET]                          = "cpuset",
        [CGROUP_CONTROLLER_IO]                              = "io",
        [CGROUP_CONTROLLER_BLKIO]                           = "blkio",
        [CGROUP_CONTROLLER_MEMORY]                          = "memory",
        [CGROUP_CONTROLLER_DEVICES]                         = "devices",
        [CGROUP_CONTROLLER_PIDS]                            = "pids",
        [CGROUP_CONTROLLER_BPF_FIREWALL]                    = "bpf-firewall",
        [CGROUP_CONTROLLER_BPF_DEVICES]                     = "bpf-devices",
        [CGROUP_CONTROLLER_BPF_FOREIGN]                     = "bpf-foreign",
        [CGROUP_CONTROLLER_BPF_SOCKET_BIND]                 = "bpf-socket-bind",
        [CGROUP_CONTROLLER_BPF_RESTRICT_NETWORK_INTERFACES] = "bpf-restrict-network-interfaces",
        [CGROUP_CONTROLLER_BPF_BIND_NETWORK_INTERFACE]      = "bpf-bind-network-interface",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_controller, CGroupController);

static const var_na* const managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

static const var_na* const managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
