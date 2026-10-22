/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/fs.h>
#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/xattr.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "capsule-util.h"
#ಅನ್ತರ್ಭಾವ "cgroup-util.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "login-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "set.h"
#ಅನ್ತರ್ಭಾವ "special.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"
#ಅನ್ತರ್ಭಾವ "unit-name.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"
#ಅನ್ತರ್ಭಾವ "xattr-util.h"

ಪೂರ್ಣಾಙ್ಕ cg_is_available(ಶೂನ್ಯ) {
        ಸಂರಚನಾ statfs fs;

        ಯದಿ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ಯದಿ (errno == ENOENT) /* sysfs not mounted? */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ಪ್ರತ್ಯಾಗಮ is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

ಪೂರ್ಣಾಙ್ಕ cg_path_open(ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

ಪೂರ್ಣಾಙ್ಕ cg_cgroupid_open(ಪೂರ್ಣಾಙ್ಕ cgroupfs_fd, uint64_t id) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fsfd = -EBADF;

        ಯದಿ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ಯದಿ (fsfd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                cgroupfs_fd = fsfd;
        }

        ಸಙ್ಘ {
                ಸಂರಚನಾ file_handle file_handle;
                uint8_t space[offsetof(ಸಂರಚನಾ file_handle, f_handle) + ಪರಿಮಾಣ(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = ಪರಿಮಾಣ(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

ಪೂರ್ಣಾಙ್ಕ cg_path_from_cgroupid(ಪೂರ್ಣಾಙ್ಕ cgroupfs_fd, uint64_t id, ವರ್ಣ **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ cgfd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ಯದಿ (cgfd < 0)
                ಪ್ರತ್ಯಾಗಮ cgfd;

        _cleanup_free_ ವರ್ಣ *path = ರಿಕ್ತ;
        r = fd_get_path(cgfd, &path);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!path_startswith(path, "/sys/fs/cgroup/"))
                ಪ್ರತ್ಯಾಗಮ -EXDEV; /* recognizable error */

        ಯದಿ (ret)
                *ret = TAKE_PTR(path);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_enumerate_processes(ಧ್ರುವ ವರ್ಣ *path, FILE **ret) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        FILE *f;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        f = fopen(fs, "re");
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = f;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        ಪ್ರತಿ (;;) {
                errno = 0;
                ಯದಿ (fscanf(f, "%lu", &ul) != 1) {

                        ಯದಿ (feof(f)) {
                                *ret = 0;
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);
                }

                ಯದಿ (ul > PID_T_MAX)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ಯದಿ (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        ಅನುವರ್ತ;

                *ret = (pid_t) ul;
                ಪ್ರತ್ಯಾಗಮ 1;
        }
}

ಪೂರ್ಣಾಙ್ಕ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ಯದಿ (r == 0) {
                        *ret = PIDREF_NULL;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                ಯದಿ (pid == 0)
                        ಪ್ರತ್ಯಾಗಮ -EREMOTE;

                r = pidref_set_pid(ret, pid);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ 1;
                ಯದಿ (r != -ESRCH)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಙ್ಕ supported = -1;

        ಯದಿ (supported >= 0)
                ಪ್ರತ್ಯಾಗಮ supported;

        ಯದಿ (cg_is_available() <= 0)
                ಪ್ರತ್ಯಾಗಮ (supported = false);

        ಯದಿ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ಪ್ರತ್ಯಾಗಮ (supported = true);
        ಯದಿ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ಪ್ರತ್ಯಾಗಮ (supported = false);
}

ಪೂರ್ಣಾಙ್ಕ cg_enumerate_subgroups(ಧ್ರುವ ವರ್ಣ *path, DIR **ret) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        DIR *d;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        d = opendir(fs);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = d;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_read_subgroup(DIR *d, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(d);
        ಪ್ರತಿಜ್ಞಾ(ret);

        FOREACH_DIRENT_ALL(de, d, ಪ್ರತ್ಯಾಗಮ -errno) {
                ಯದಿ (de->d_type != DT_DIR)
                        ಅನುವರ್ತ;

                ಯದಿ (dot_or_dot_dot(de->d_name))
                        ಅನುವರ್ತ;

                ಪ್ರತ್ಯಾಗಮ strdup_to_full(ret, de->d_name);
        }

        *ret = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_kill(
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                ಶೂನ್ಯ *userdata) {

        _cleanup_set_free_ Set *allocated_set = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ಯದಿ (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ಯದಿ (!killed_pids) {
                killed_pids = allocated_set = set_new(ರಿಕ್ತ);
                ಯದಿ (!killed_pids)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        bool done;
        ಕುರು {
                _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
                ಪೂರ್ಣಾಙ್ಕ ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                ಯದಿ (r == -ENOENT)
                        ಭಙ್ಗ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                ಪ್ರತಿ (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ಯದಿ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                ಭಙ್ಗ;
                        }
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        ಯದಿ (r == 0)
                                ಭಙ್ಗ;

                        ಯದಿ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                ಅನುವರ್ತ;

                        ಯದಿ (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                ಅನುವರ್ತ;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ಯದಿ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                ಅನುವರ್ತ;
                        }

                        ಯದಿ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ಯದಿ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        ಯದಿ (r >= 0) {
                                ಯದಿ (flags & CGROUP_SIGCONT)
                                        (ಶೂನ್ಯ) pidref_kill(&pidref, SIGCONT);

                                ಯದಿ (ret == 0) {
                                        ಯದಿ (log_kill)
                                                ret = ret_log_kill;
                                        ಅನ್ಯಥಾ
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ಯಾವತ್ (!done);

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ cg_kill_recursive(
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                ಶೂನ್ಯ *userdata) {

        _cleanup_set_free_ Set *allocated_set = ರಿಕ್ತ;
        _cleanup_closedir_ DIR *d = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r, ret;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(sig >= 0);

        ಯದಿ (!killed_pids) {
                killed_pids = allocated_set = set_new(ರಿಕ್ತ);
                ಯದಿ (!killed_pids)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        ಯದಿ (r < 0) {
                ಯದಿ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                ಪ್ರತ್ಯಾಗಮ ret;
        }

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *fn = ರಿಕ್ತ, *p = ರಿಕ್ತ;

                r = cg_read_subgroup(d, &fn);
                ಯದಿ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        ಭಙ್ಗ;
                }
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                p = path_join(empty_to_ಬೇರು(path), fn);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ಯದಿ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ಯದಿ (r != 0 && ret >= 0)
                        ret = r;
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ cg_kill_kernel_sigkill(ಧ್ರುವ ವರ್ಣ *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ ವರ್ಣ *killfile = ರಿಕ್ತ;
        uint64_t n_pids = UINT64_MAX;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        ಪ್ರತಿಜ್ಞಾ(path);

        ಯದಿ (!cg_kill_supported())
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                ಯದಿ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        ಯದಿ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_get_path(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret) {
        ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = path_simplify(t);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_set_xattr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t size, ಪೂರ್ಣಾಙ್ಕ flags) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || size <= 0);

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setxattr(fs, name, ಮೌಲ್ಯ, size, flags));
}

ಪೂರ್ಣಾಙ್ಕ cg_get_xattr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret, size_t *ret_size) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(name);

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ lgetxattr_malloc(fs, name, ret, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ cg_get_xattr_bool(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(name);

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

ಪೂರ್ಣಾಙ್ಕ cg_remove_xattr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(name);

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(removexattr(fs, name));
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_path(pid_t pid, ವರ್ಣ **ret_path) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *fs;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
                ವರ್ಣ *e;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ -ENODATA;

                e = startswith(ಸಾಲು, "0:");
                ಯದಿ (!e)
                        ಅನುವರ್ತ;

                e = strchr(e, ':');
                ಯದಿ (!e)
                        ಅನುವರ್ತ;

                _cleanup_free_ ವರ್ಣ *path = strdup(e + 1);
                ಯದಿ (!path)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ಯದಿ (startswith(path, "/../"))
                        ಪ್ರತ್ಯಾಗಮ -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                ಯದಿ (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                ಪ್ರತ್ಯಾಗಮ 0;
        }
}

ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_path(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_path) {
        _cleanup_free_ ವರ್ಣ *path = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_path);

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret_path = TAKE_PTR(path);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_is_empty(ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        ಪ್ರತಿಜ್ಞಾ(path);

        /* The root cgroup is always populated */
        ಯದಿ (empty_or_ಬೇರು(path))
                ಪ್ರತ್ಯಾಗಮ false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ true;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ streq(t, "0");
}

ಪೂರ್ಣಾಙ್ಕ cg_split_spec(ಧ್ರುವ ವರ್ಣ *spec, ವರ್ಣ **ret_controller, ವರ್ಣ **ret_path) {
        _cleanup_free_ ವರ್ಣ *controller = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *path;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ಯದಿ (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                ಗಚ್ಛ finalize;
        }

        ಧ್ರುವ ವರ್ಣ *e = strchr(spec, ':');
        ಯದಿ (!e) {
                /* Controller only. */
                ಯದಿ (ret_controller) {
                        controller = strdup(spec);
                        ಯದಿ (!controller)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                path = ರಿಕ್ತ;
        } ಅನ್ಯಥಾ {
                /* Both controller and path. */
                ಯದಿ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ಯದಿ (!controller)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        ಯದಿ (path) {
                /* Non-empty path must be absolute. */
                ಯದಿ (!path_is_absolute(path))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Path must not contain dot-dot. */
                ಯದಿ (!path_is_safe(path))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಯದಿ (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_get_ಬೇರು_path(ವರ್ಣ **ret_path) {
        ವರ್ಣ *p, *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_path);

        r = cg_pid_get_path(1, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ಯದಿ (e)
                *e = 0;

        *ret_path = p;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_shift_path(ಧ್ರುವ ವರ್ಣ *cgroup, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ **ret_shifted) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(cgroup);
        ಪ್ರತಿಜ್ಞಾ(ret_shifted);

        _cleanup_free_ ವರ್ಣ *rt = ರಿಕ್ತ;
        ಯದಿ (!ಬೇರು) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_ಬೇರು_path(&rt);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಬೇರು = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, ಬೇರು, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_path_shifted(pid_t pid, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret_cgroup) {
        _cleanup_free_ ವರ್ಣ *raw = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = cg_shift_path(raw, ಬೇರು, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret_cgroup, c);
}

ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_path_shifted(ಧ್ರುವ PidRef *pidref, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret_cgroup) {
        _cleanup_free_ ವರ್ಣ *raw = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = cg_shift_path(raw, ಬೇರು, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret_cgroup, c);
}

ಪೂರ್ಣಾಙ್ಕ cg_path_decode_unit(ಧ್ರುವ ವರ್ಣ *cgroup, ವರ್ಣ **ret_unit) {
        ಪ್ರತಿಜ್ಞಾ(cgroup);

        size_t n = strcspn(cgroup, "/");
        ಯದಿ (n < 3)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ವರ್ಣ *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        ಯದಿ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಯದಿ (ret_unit)
                ಪ್ರತ್ಯಾಗಮ strdup_to(ret_unit, c);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ bool valid_slice_name(ಧ್ರುವ ವರ್ಣ *p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        ಯದಿ (n < STRLEN("x.slice"))
                ಪ್ರತ್ಯಾಗಮ false;

        ವರ್ಣ *c = strndupa_safe(p, n);
        ಯದಿ (!endswith(c, ".slice"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* skip_slices(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* Skips over all slice assignments */

        ಪ್ರತಿ (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ಯದಿ (!valid_slice_name(p, n))
                        ಪ್ರತ್ಯಾಗಮ p;

                p += n;
        }
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_unit_full(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        ಧ್ರುವ ವರ್ಣ *e = skip_slices(path);

        _cleanup_free_ ವರ್ಣ *unit = ರಿಕ್ತ;
        r = cg_path_decode_unit(e, &unit);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* We skipped over the slices, don't accept any now */
        ಯದಿ (endswith(unit, ".slice"))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಯದಿ (ret_subgroup) {
                _cleanup_free_ ವರ್ಣ *subgroup = ರಿಕ್ತ;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ಯದಿ (isempty(e))
                        subgroup = ರಿಕ್ತ;
                ಅನ್ಯಥಾ {
                        subgroup = strdup(e);
                        ಯದಿ (!subgroup)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ಯದಿ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_unit_path(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *path_copy = ರಿಕ್ತ;
        ವರ್ಣ *unit_name;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        path_copy = strdup(path);
        ಯದಿ (!path_copy)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        unit_name = (ವರ್ಣ*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ಯದಿ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        *ret = TAKE_PTR(path_copy);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_unit_full(pid_t pid, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup) {
        ಪೂರ್ಣಾಙ್ಕ r;

        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_unit_full(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        _cleanup_free_ ವರ್ಣ *unit = ರಿಕ್ತ, *subgroup = ರಿಕ್ತ;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ಯದಿ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* skip_session(ಧ್ರುವ ವರ್ಣ *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ಯದಿ (n < STRLEN("session-x.scope"))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಧ್ರುವ ವರ್ಣ *s = startswith(p, "session-");
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        ವರ್ಣ *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        *e = '\0';

        ಯದಿ (!session_id_valid(f))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ skip_leading_slash(p + n);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* skip_user_manager(ಧ್ರುವ ವರ್ಣ *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ಯದಿ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ ವರ್ಣ *unit_name = strndup(p, n);
        ಯದಿ (!unit_name)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        _cleanup_free_ ವರ್ಣ *i = ರಿಕ್ತ;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        ಯದಿ (type != UNIT_NAME_INSTANCE)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ಯದಿ (!(startswith(unit_name, "user@") && parse_uid(i, ರಿಕ್ತ) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ skip_leading_slash(p + n);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* skip_user_prefix(ಧ್ರುವ ವರ್ಣ *path) {
        ಧ್ರುವ ವರ್ಣ *e, *t;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ಯದಿ (t)
                ಪ್ರತ್ಯಾಗಮ t;

        /* Alternatively skip the user session if it is in the path... */
        ಪ್ರತ್ಯಾಗಮ skip_session(e);
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_user_unit_full(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup) {
        ಧ್ರುವ ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(path);

        t = skip_user_prefix(path);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ಪ್ರತ್ಯಾಗಮ cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_user_unit_full(pid_t pid, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup) {
        ಪೂರ್ಣಾಙ್ಕ r;

        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_user_unit_full(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret_unit, ವರ್ಣ **ret_subgroup) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        _cleanup_free_ ವರ್ಣ *unit = ರಿಕ್ತ, *subgroup = ರಿಕ್ತ;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ಯದಿ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_machine_name(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_machine) {
        _cleanup_free_ ವರ್ಣ *u = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *sl;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_path_get_unit(path, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ಪ್ರತ್ಯಾಗಮ readlink_malloc(sl, ret_machine);
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_machine_name(pid_t pid, ವರ್ಣ **ret_machine) {
        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_machine_name(cgroup, ret_machine);
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_session(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_session) {
        _cleanup_free_ ವರ್ಣ *unit = ರಿಕ್ತ;
        ವರ್ಣ *start, *end;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = cg_path_get_unit(path, &unit);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        start = startswith(unit, "session-");
        ಯದಿ (!start)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;
        end = endswith(start, ".scope");
        ಯದಿ (!end)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        *end = 0;
        ಯದಿ (!session_id_valid(start))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಯದಿ (!ret_session)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret_session, start);
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_session(pid_t pid, ವರ್ಣ **ret_session) {
        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_session(cgroup, ret_session);
}

ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_session(ಧ್ರುವ PidRef *pidref, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        _cleanup_free_ ವರ್ಣ *session = ರಿಕ್ತ;
        r = cg_pid_get_session(pidref->pid, &session);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = TAKE_PTR(session);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_owner_uid(ಧ್ರುವ ವರ್ಣ *path, uid_t *ret_uid) {
        _cleanup_free_ ವರ್ಣ *slice = ರಿಕ್ತ;
        ವರ್ಣ *start, *end;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = cg_path_get_slice(path, &slice);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        start = startswith(slice, "user-");
        ಯದಿ (!start)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        end = endswith(start, ".slice");
        ಯದಿ (!end)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        *end = 0;
        ಯದಿ (parse_uid(start, ret_uid) < 0)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_owner_uid(cgroup, ret_uid);
}

ಪೂರ್ಣಾಙ್ಕ cg_pidref_get_owner_uid(ಧ್ರುವ PidRef *pidref, uid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!pidref_is_set(pidref))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;
        ಯದಿ (pidref_is_remote(pidref))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = pidref_verify(pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret)
                *ret = uid;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_slice(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret_slice) {
        ಧ್ರುವ ವರ್ಣ *e = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *s;
                ಪೂರ್ಣಾಙ್ಕ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ n;
                ಯದಿ (!valid_slice_name(s, n))
                        ಭಙ್ಗ;

                e = s;
        }

        ಯದಿ (e)
                ಪ್ರತ್ಯಾಗಮ cg_path_decode_unit(e, ret_slice);

        ಯದಿ (ret_slice)
                ಪ್ರತ್ಯಾಗಮ strdup_to(ret_slice, SPECIAL_ಬೇರು_SLICE);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_slice(pid_t pid, ವರ್ಣ **ret_slice) {
        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_slice(cgroup, ret_slice);
}

ಪೂರ್ಣಾಙ್ಕ cg_path_get_user_slice(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret_slice) {
        ಧ್ರುವ ವರ್ಣ *t;
        ಪ್ರತಿಜ್ಞಾ(p);

        t = skip_user_prefix(p);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ಪ್ರತ್ಯಾಗಮ cg_path_get_slice(t, ret_slice);
}

ಪೂರ್ಣಾಙ್ಕ cg_pid_get_user_slice(pid_t pid, ವರ್ಣ **ret_slice) {
        _cleanup_free_ ವರ್ಣ *cgroup = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_pid_get_path_shifted(pid, ರಿಕ್ತ, &cgroup);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(ಧ್ರುವ ವರ್ಣ *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ಯದಿ (!filename_is_valid(p))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (IN_SET(p[0], '_', '.'))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (startswith(p, "cgroup."))
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತಿ (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ಧ್ರುವ ವರ್ಣ *q;

                q = startswith(p, cgroup_controller_to_string(c));
                ಯದಿ (!q)
                        ಅನುವರ್ತ;

                ಯದಿ (q[0] == '.')
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ cg_escape(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ಯದಿ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ಯದಿ (!n)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        } ಅನ್ಯಥಾ {
                n = strdup(p);
                ಯದಿ (!n)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* cg_unescape(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ಯದಿ (p[0] == '_')
                ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) p+1;

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) p;
}

ಪೂರ್ಣಾಙ್ಕ cg_slice_to_path(ಧ್ರುವ ವರ್ಣ *unit, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *s = ರಿಕ್ತ, *e = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *dash;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(unit);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (streq(unit, SPECIAL_ಬೇರು_SLICE))
                ಪ್ರತ್ಯಾಗಮ strdup_to(ret, "");

        ಯದಿ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!endswith(unit, ".slice"))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ಯದಿ (dash == p)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯಾವತ್ (dash) {
                _cleanup_free_ ವರ್ಣ *escaped = ರಿಕ್ತ;
                ವರ್ಣ n[dash - p + ಪರಿಮಾಣ(".slice")];

#ಯದಿ HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                ಯದಿ (IN_SET(dash[1], 0, '-'))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ಯದಿ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                r = cg_escape(n, &escaped);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (!strextend(&s, escaped, "/"))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!strextend(&s, e))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_is_threaded(ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_free_ ವರ್ಣ *fs = ರಿಕ್ತ, *contents = ರಿಕ್ತ;
        _cleanup_strv_free_ ವರ್ಣ **v = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_get_path(path, "cgroup.type", &fs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = read_full_virtual_file(fs, &contents, ರಿಕ್ತ);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ false; /* Assume no. */
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        v = strv_split(contents, ರಿಕ್ತ);
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ಪ್ರತ್ಯಾಗಮ strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

ಪೂರ್ಣಾಙ್ಕ cg_set_attribute(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(attribute);

        r = cg_get_path(path, attribute, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ಪ್ರತ್ಯಾಗಮ write_string_file(p, ಮೌಲ್ಯ, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

ಪೂರ್ಣಾಙ್ಕ cg_get_attribute(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(attribute);

        r = cg_get_path(path, attribute, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ read_one_ಸಾಲು_file(p, ret);
}

ಪೂರ್ಣಾಙ್ಕ cg_get_attribute_as_uint64(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        uint64_t v;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = cg_get_attribute(path, attribute, &ಮೌಲ್ಯ);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ENODATA;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (streq(ಮೌಲ್ಯ, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = safe_atou64(ಮೌಲ್ಯ, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = v;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_get_attribute_as_bool(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_get_attribute(path, attribute, &ಮೌಲ್ಯ);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -ENODATA;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_boolean(ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ cg_get_owner(ಧ್ರುವ ವರ್ಣ *path, uid_t *ret_uid) {
        _cleanup_free_ ವರ್ಣ *f = ರಿಕ್ತ;
        ಸಂರಚನಾ stat stats;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_uid);

        r = cg_get_path(path, /* suffix= */ ರಿಕ್ತ, &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (stat(f, &stats) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = stat_verify_directory(&stats);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret_uid = stats.st_uid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_get_keyed_attribute(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *attribute,
                ವರ್ಣ * ಧ್ರುವ *ಕೀಲಿಗಳು,
                ವರ್ಣ **values) {

        _cleanup_free_ ವರ್ಣ *filename = ರಿಕ್ತ, *contents = ರಿಕ್ತ;
        size_t n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = read_full_file(filename, &contents, /* ret_size= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        n = strv_length(ಕೀಲಿಗಳು);
        ಯದಿ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪ್ರತಿಜ್ಞಾ(strv_is_uniq(ಕೀಲಿಗಳು));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        ವರ್ಣ **v = newa0(ವರ್ಣ*, n);
        size_t n_done = 0;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = contents; *p;) {
                ಧ್ರುವ ವರ್ಣ *w;
                size_t i;

                ಪ್ರತಿ (i = 0; i < n; i++) {
                        w = first_word(p, ಕೀಲಿಗಳು[i]);
                        ಯದಿ (w)
                                ಭಙ್ಗ;
                }

                ಯದಿ (w) {
                        ಯದಿ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                ಗಚ್ಛ fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ಯದಿ (!v[i]) {
                                r = -ENOMEM;
                                ಗಚ್ಛ fail;
                        }

                        n_done++;
                        ಯದಿ (n_done >= n)
                                ಭಙ್ಗ;

                        p = w + l;
                } ಅನ್ಯಥಾ
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ಯದಿ (n_done < n) {
                r = -ENXIO;
                ಗಚ್ಛ fail;
        }

        memcpy(values, v, ಪರಿಮಾಣ(ವರ್ಣ*) * n);
        ಪ್ರತ್ಯಾಗಮ 0;

fail:
        free_many_charp(v, n);
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ cg_get_keyed_attribute_uint64(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *attribute, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, uint64_t *ret) {
        _cleanup_free_ ವರ್ಣ *val = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(ಕೀಲಿ), &val);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = safe_atou64(val, ret);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, ಕೀಲಿ, attribute);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_mask_to_string(CGroupMask mask, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (mask == 0) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತಿ (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ಧ್ರುವ ವರ್ಣ *k;
                size_t l;

                ಯದಿ (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        ಅನುವರ್ತ;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                ಯದಿ (!GREEDY_REALLOC(s, n + space + l + 1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        ಪ್ರತಿಜ್ಞಾ(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_mask_from_string(ಧ್ರುವ ವರ್ಣ *s, CGroupMask *ret) {
        CGroupMask m = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ;
                CGroupController v;
                ಪೂರ್ಣಾಙ್ಕ r;

                r = extract_first_word(&s, &n, ರಿಕ್ತ, 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                v = cgroup_controller_from_string(n);
                ಯದಿ (v < 0)
                        ಅನುವರ್ತ;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_mask_supported_subtree(ಧ್ರುವ ವರ್ಣ *ಬೇರು, CGroupMask *ret) {
        CGroupMask mask;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ ವರ್ಣ *controllers = ರಿಕ್ತ, *path = ರಿಕ್ತ;
        r = cg_get_path(ಬೇರು, "cgroup.controllers", &path);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = read_one_ಸಾಲು_file(path, &controllers);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = cg_mask_from_string(controllers, &mask);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ ವರ್ಣ *ಬೇರು = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = cg_get_ಬೇರು_path(&ಬೇರು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cg_mask_supported_subtree(ಬೇರು, ret);
}

ಪೂರ್ಣಾಙ್ಕ cg_is_delegated(ಧ್ರುವ ವರ್ಣ *path) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        ಯದಿ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಪ್ರತ್ಯಾಗಮ r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        ಪ್ರತ್ಯಾಗಮ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ಪೂರ್ಣಾಙ್ಕ cg_is_delegated_fd(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ರಿಕ್ತ, "trusted.delegate", /* at_flags= */ 0);
        ಯದಿ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಪ್ರತ್ಯಾಗಮ r;

        r = getxattr_at_bool(fd, /* path= */ ರಿಕ್ತ, "user.delegate", /* at_flags= */ 0);
        ಪ್ರತ್ಯಾಗಮ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ಪೂರ್ಣಾಙ್ಕ cg_has_coredump_receive(ಧ್ರುವ ವರ್ಣ *path) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        ಯದಿ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಧ್ರುವ uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

ಶೂನ್ಯ cgroup_io_limits_list(ಶೂನ್ಯ) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
