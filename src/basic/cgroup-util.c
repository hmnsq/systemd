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

ಪೂರ್ಣಾಂಕ cg_is_ಲಭ್ಯ(ಶೂನ್ಯ) {
        ರಚನೆ statfs fs;

        ಆದರೆ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ಆದರೆ (errno == ENOENT) /* sysfs not mounted? */
                        ಹಿಂತಿರುಗಿ false;

                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ಹಿಂತಿರುಗಿ is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

ಪೂರ್ಣಾಂಕ cg_cgroupid_open(ಪೂರ್ಣಾಂಕ cgroupfs_fd, uint64_t id) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fsfd = -EBADF;

        ಆದರೆ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ಆದರೆ (fsfd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                cgroupfs_fd = fsfd;
        }

        ಒಕ್ಕೂಟ {
                ರಚನೆ file_handle file_handle;
                uint8_t space[offsetof(ರಚನೆ file_handle, f_handle) + ಗಾತ್ರ(uint64_t)];
        } fh = {
                .file_handle.handle_ಬೈಟುಗಳು = ಗಾತ್ರ(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ಹಿಂತಿರುಗಿ RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_from_cgroupid(ಪೂರ್ಣಾಂಕ cgroupfs_fd, uint64_t id, ಅಕ್ಷರ **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ cgfd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ಆದರೆ (cgfd < 0)
                ಹಿಂತಿರುಗಿ cgfd;

        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
        r = fd_get_ಮಾರ್ಗ(cgfd, &ಮಾರ್ಗ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!ಮಾರ್ಗ_startswith(ಮಾರ್ಗ, "/sys/fs/cgroup/"))
                ಹಿಂತಿರುಗಿ -EXDEV; /* recognizable error */

        ಆದರೆ (ret)
                *ret = TAKE_PTR(ಮಾರ್ಗ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_enumerate_processes(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        FILE *f;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, "cgroup.procs", &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        f = fopen(fs, "re");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = f;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        ಗಾಗಿ (;;) {
                errno = 0;
                ಆದರೆ (fscanf(f, "%lu", &ul) != 1) {

                        ಆದರೆ (feof(f)) {
                                *ret = 0;
                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಹಿಂತಿರುಗಿ errno_or_else(EIO);
                }

                ಆದರೆ (ul > PID_T_MAX)
                        ಹಿಂತಿರುಗಿ -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ಆದರೆ (ul == 0 && !FLAGS_ಗಣ(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        ಮುಂದುವರಿ;

                *ret = (pid_t) ul;
                ಹಿಂತಿರುಗಿ 1;
        }
}

ಪೂರ್ಣಾಂಕ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ಆದರೆ (r == 0) {
                        *ret = PIDREF_NULL;
                        ಹಿಂತಿರುಗಿ 0;
                }

                ಆದರೆ (pid == 0)
                        ಹಿಂತಿರುಗಿ -EREMOTE;

                r = pidref_ಗಣ_pid(ret, pid);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ 1;
                ಆದರೆ (r != -ESRCH)
                        ಹಿಂತಿರುಗಿ r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಂಕ supported = -1;

        ಆದರೆ (supported >= 0)
                ಹಿಂತಿರುಗಿ supported;

        ಆದರೆ (cg_is_ಲಭ್ಯ() <= 0)
                ಹಿಂತಿರುಗಿ (supported = false);

        ಆದರೆ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ಹಿಂತಿರುಗಿ (supported = true);
        ಆದರೆ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ಹಿಂತಿರುಗಿ (supported = false);
}

ಪೂರ್ಣಾಂಕ cg_enumerate_subgroups(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, DIR **ret) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        DIR *d;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is not recursive! */

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        d = opendir(fs);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = d;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_read_subgroup(DIR *d, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(d);
        ಪ್ರತಿಜ್ಞಾ(ret);

        FOREACH_DIRENT_ALL(de, d, ಹಿಂತಿರುಗಿ -errno) {
                ಆದರೆ (de->d_type != DT_DIR)
                        ಮುಂದುವರಿ;

                ಆದರೆ (dot_or_dot_dot(de->d_name))
                        ಮುಂದುವರಿ;

                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret, de->d_name);
        }

        *ret = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_kill(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ sig,
                CGroupFlags flags,
                ಗಣ_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                ಶೂನ್ಯ *userdata) {

        cleanup_ಗಣ_free ಗಣ_2 *allocated_ಗಣ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ಆದರೆ (ಒಳ_ಗಣ(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ಆದರೆ (!killed_pids) {
                killed_pids = allocated_ಗಣ = ಗಣ_new(ಖಾಲಿ);
                ಆದರೆ (!killed_pids)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        bool ಮುಗಿದ;
        ಮಾಡು {
                _cleanup_fclose_ FILE *f = ಖಾಲಿ;
                ಪೂರ್ಣಾಂಕ ret_log_kill;

                ಮುಗಿದ = true;

                r = cg_enumerate_processes(ಮಾರ್ಗ, &f);
                ಆದರೆ (r == -ENOENT)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                ಗಾಗಿ (;;) {
                        _cleanup_(pidref_ಮುಗಿದ) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ಆದರೆ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                ಮುಗಿದ = true;
                                ನಿಲ್ಲಿಸು;
                        }
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", ಮಾರ್ಗ));
                        ಆದರೆ (r == 0)
                                ನಿಲ್ಲಿಸು;

                        ಆದರೆ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                ಮುಂದುವರಿ;

                        ಆದರೆ (ಗಣ_ಒಳಗೊಂಡಿದೆ(killed_pids, PID_ಗಮ್ಯ_PTR(pidref.pid)))
                                ಮುಂದುವರಿ;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ಆದರೆ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, ಮಾರ್ಗ);
                                ಮುಂದುವರಿ;
                        }

                        ಆದರೆ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ಆದರೆ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, ಮಾರ್ಗ));
                        ಆದರೆ (r >= 0) {
                                ಆದರೆ (flags & CGROUP_SIGCONT)
                                        (ಶೂನ್ಯ) pidref_kill(&pidref, SIGCONT);

                                ಆದರೆ (ret == 0) {
                                        ಆದರೆ (log_kill)
                                                ret = ret_log_kill;
                                        ಇಲ್ಲದಿದ್ದರೆ
                                                ret = 1;
                                }
                        }

                        ಮುಗಿದ = false;

                        r = ಗಣ_put(killed_pids, PID_ಗಮ್ಯ_PTR(pidref.pid));
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ಆಗ (!ಮುಗಿದ);

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ cg_kill_recursive(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ sig,
                CGroupFlags flags,
                ಗಣ_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                ಶೂನ್ಯ *userdata) {

        cleanup_ಗಣ_free ಗಣ_2 *allocated_ಗಣ = ಖಾಲಿ;
        _cleanup_closedir_ DIR *d = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r, ret;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(sig >= 0);

        ಆದರೆ (!killed_pids) {
                killed_pids = allocated_ಗಣ = ಗಣ_new(ಖಾಲಿ);
                ಆದರೆ (!killed_pids)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        ret = cg_kill(ಮಾರ್ಗ, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(ಮಾರ್ಗ, &d);
        ಆದರೆ (r < 0) {
                ಆದರೆ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", ಮಾರ್ಗ));

                ಹಿಂತಿರುಗಿ ret;
        }

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *fn = ಖಾಲಿ, *p = ಖಾಲಿ;

                r = cg_read_subgroup(d, &fn);
                ಆದರೆ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", ಮಾರ್ಗ));
                        ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                p = ಮಾರ್ಗ_join(ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಮಾರ್ಗ), fn);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ಆದರೆ (r != 0 && ret >= 0)
                        ret = r;
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ cg_kill_kernel_sigkill(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ ಅಕ್ಷರ *killfile = ಖಾಲಿ;
        uint64_t n_pids = UINT64_MAX;
        ಪೂರ್ಣಾಂಕ r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        ಆದರೆ (!cg_kill_supported())
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, "cgroup.kill", &killfile);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(ಮಾರ್ಗ, "pids.current", &n_pids);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", ಮಾರ್ಗ);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_ಬಫರ್);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", ಮಾರ್ಗ);

        ಆದರೆ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಮಾರ್ಗ = TAKE_PTR(suffix);

        t = ಮಾರ್ಗ_join("/sys/fs/cgroup", ಮಾರ್ಗ, suffix);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = ಮಾರ್ಗ_simplify(t);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಗಣ_xattr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t ಗಾತ್ರ_2, ಪೂರ್ಣಾಂಕ flags) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || ಗಾತ್ರ_2 <= 0);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ RET_NERRNO(setxattr(fs, name, ಮೌಲ್ಯ, ಗಾತ್ರ_2, flags));
}

ಪೂರ್ಣಾಂಕ cg_get_xattr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(name);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ lgetxattr_malloc(fs, name, ret, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ cg_get_xattr_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(name);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

ಪೂರ್ಣಾಂಕ cg_remove_xattr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(name);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ RET_NERRNO(removexattr(fs, name));
}

ಪೂರ್ಣಾಂಕ cg_pid_get_ಮಾರ್ಗ(pid_t pid, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fs;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_ಮಾರ್ಗ);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
                ಅಕ್ಷರ *e;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ -ENODATA;

                e = startswith(ಸಾಲು, "0:");
                ಆದರೆ (!e)
                        ಮುಂದುವರಿ;

                e = strchr(e, ':');
                ಆದರೆ (!e)
                        ಮುಂದುವರಿ;

                _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = strdup(e + 1);
                ಆದರೆ (!ಮಾರ್ಗ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ಆದರೆ (startswith(ಮಾರ್ಗ, "/../"))
                        ಹಿಂತಿರುಗಿ -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(ಮಾರ್ಗ, " (deleted)");
                ಆದರೆ (e)
                        *e = 0;

                *ret_ಮಾರ್ಗ = TAKE_PTR(ಮಾರ್ಗ);
                ಹಿಂತಿರುಗಿ 0;
        }
}

ಪೂರ್ಣಾಂಕ cg_pidref_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_ಮಾರ್ಗ);

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_ಮಾರ್ಗ(pidref->pid, &ಮಾರ್ಗ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret_ಮಾರ್ಗ = TAKE_PTR(ಮಾರ್ಗ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* The root cgroup is always populated */
        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ false;

        r = cg_get_keyed_attribute(ಮಾರ್ಗ, "cgroup.events", STRV_MAKE("populated"), &t);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ true;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ streq(t, "0");
}

ಪೂರ್ಣಾಂಕ cg_ವಿಭಜಿಸು_spec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *spec, ಅಕ್ಷರ **ret_controller, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *controller = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ಆದರೆ (isempty(spec) || ಮಾರ್ಗ_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                ಮಾರ್ಗ = spec;
                ಹೋಗು finalize;
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = strchr(spec, ':');
        ಆದರೆ (!e) {
                /* Controller only. */
                ಆದರೆ (ret_controller) {
                        controller = strdup(spec);
                        ಆದರೆ (!controller)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                ಮಾರ್ಗ = ಖಾಲಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* Both controller and path. */
                ಆದರೆ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ಆದರೆ (!controller)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                ಮಾರ್ಗ = e + 1;
        }

finalize:
        ಮಾರ್ಗ = ಖಾಲಿಗಳು_ಗಮ್ಯ_null(ಮಾರ್ಗ);

        ಆದರೆ (ಮಾರ್ಗ) {
                /* Non-empty path must be absolute. */
                ಆದರೆ (!ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Path must not contain dot-dot. */
                ಆದರೆ (!ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (ret_ಮಾರ್ಗ) {
                r = ಮಾರ್ಗ_simplify_alloc(ಮಾರ್ಗ, ret_ಮಾರ್ಗ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_get_ಬೇರು_ಮಾರ್ಗ(ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಅಕ್ಷರ *p, *e;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_ಮಾರ್ಗ);

        r = cg_pid_get_ಮಾರ್ಗ(1, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ಆದರೆ (e)
                *e = 0;

        *ret_ಮಾರ್ಗ = p;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಸರಿಸು_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cgroup, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret_shifted) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(cgroup);
        ಪ್ರತಿಜ್ಞಾ(ret_shifted);

        _cleanup_free_ ಅಕ್ಷರ *rt = ಖಾಲಿ;
        ಆದರೆ (!ಬೇರು) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_ಬೇರು_ಮಾರ್ಗ(&rt);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಬೇರು = rt;
        }

        *ret_shifted = ಮಾರ್ಗ_startswith_ತುಂಬಿದ(cgroup, ಬೇರು, ಮಾರ್ಗ_STARTSWITH_RETURN_LEADING_SLASH|ಮಾರ್ಗ_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_pid_get_ಮಾರ್ಗ_shifted(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret_cgroup) {
        _cleanup_free_ ಅಕ್ಷರ *raw = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_cgroup);

        r = cg_pid_get_ಮಾರ್ಗ(pid, &raw);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = cg_ಸರಿಸು_ಮಾರ್ಗ(raw, ಬೇರು, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret_cgroup, c);
}

ಪೂರ್ಣಾಂಕ cg_pidref_get_ಮಾರ್ಗ_shifted(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret_cgroup) {
        _cleanup_free_ ಅಕ್ಷರ *raw = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_cgroup);

        r = cg_pidref_get_ಮಾರ್ಗ(pidref, &raw);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = cg_ಸರಿಸು_ಮಾರ್ಗ(raw, ಬೇರು, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret_cgroup, c);
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_decode_unit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cgroup, ಅಕ್ಷರ **ret_unit) {
        ಪ್ರತಿಜ್ಞಾ(cgroup);

        size_t n = strcspn(cgroup, "/");
        ಆದರೆ (n < 3)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಅಕ್ಷರ *c = strndupa_ಸುರಕ್ಷಿತ(cgroup, n);
        c = cg_unescape(c);

        ಆದರೆ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಆದರೆ (ret_unit)
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret_unit, c);

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ bool valid_ತುಂಡು_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        ಆದರೆ (n < STRLEN("x.slice"))
                ಹಿಂತಿರುಗಿ false;

        ಅಕ್ಷರ *c = strndupa_ಸುರಕ್ಷಿತ(p, n);
        ಆದರೆ (!endswith(c, ".slice"))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_slices(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* Skips over all slice assignments */

        ಗಾಗಿ (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ಆದರೆ (!valid_ತುಂಡು_name(p, n))
                        ಹಿಂತಿರುಗಿ p;

                p += n;
        }
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = skip_slices(ಮಾರ್ಗ);

        _cleanup_free_ ಅಕ್ಷರ *unit = ಖಾಲಿ;
        r = cg_ಮಾರ್ಗ_decode_unit(e, &unit);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* We skipped over the slices, don't accept any now */
        ಆದರೆ (endswith(unit, ".slice"))
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಆದರೆ (ret_subgroup) {
                _cleanup_free_ ಅಕ್ಷರ *subgroup = ಖಾಲಿ;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ಆದರೆ (isempty(e))
                        subgroup = ಖಾಲಿ;
                ಇಲ್ಲದಿದ್ದರೆ {
                        subgroup = strdup(e);
                        ಆದರೆ (!subgroup)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                ಮಾರ್ಗ_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ಆದರೆ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_unit_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ_copy = ಖಾಲಿ;
        ಅಕ್ಷರ *unit_name;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಮಾರ್ಗ_copy = strdup(ಮಾರ್ಗ);
        ಆದರೆ (!ಮಾರ್ಗ_copy)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        unit_name = (ಅಕ್ಷರ*) skip_slices(ಮಾರ್ಗ_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ಆದರೆ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ಹಿಂತಿರುಗಿ -ENXIO;

        *ret = TAKE_PTR(ಮಾರ್ಗ_copy);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_pid_get_unit_ತುಂಬಿದ(pid_t pid, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup) {
        ಪೂರ್ಣಾಂಕ r;

        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_unit_ತುಂಬಿದ(cgroup, ret_unit, ret_subgroup);
}

ಪೂರ್ಣಾಂಕ cg_pidref_get_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        _cleanup_free_ ಅಕ್ಷರ *unit = ಖಾಲಿ, *subgroup = ಖಾಲಿ;
        r = cg_pid_get_unit_ತುಂಬಿದ(pidref->pid, &unit, &subgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ಆದರೆ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_session(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ಆದರೆ (n < STRLEN("session-x.scope"))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = startswith(p, "session-");
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        ಅಕ್ಷರ *f = strndupa_ಸುರಕ್ಷಿತ(s, p + n - s),
             *e = endswith(f, ".scope");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        *e = '\0';

        ಆದರೆ (!session_id_valid(f))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ skip_leading_slash(p + n);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_user_manager(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ಆದರೆ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ ಅಕ್ಷರ *unit_name = strndup(p, n);
        ಆದರೆ (!unit_name)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        _cleanup_free_ ಅಕ್ಷರ *i = ಖಾಲಿ;
        UnitNameFlags type = unit_name_ಗಮ್ಯ_instance(unit_name, &i);

        ಆದರೆ (type != UNIT_NAME_INSTANCE)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ಆದರೆ (!(startswith(unit_name, "user@") && parse_uid(i, ಖಾಲಿ) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ skip_leading_slash(p + n);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_user_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *t;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Skip slices, if there are any */
        e = skip_slices(ಮಾರ್ಗ);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ಆದರೆ (t)
                ಹಿಂತಿರುಗಿ t;

        /* Alternatively skip the user session if it is in the path... */
        ಹಿಂತಿರುಗಿ skip_session(e);
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_user_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        t = skip_user_ಪೂರ್ವಪ್ರತ್ಯಯ(ಮಾರ್ಗ);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_unit_ತುಂಬಿದ(t, ret_unit, ret_subgroup);
}

ಪೂರ್ಣಾಂಕ cg_pid_get_user_unit_ತುಂಬಿದ(pid_t pid, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup) {
        ಪೂರ್ಣಾಂಕ r;

        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_user_unit_ತುಂಬಿದ(cgroup, ret_unit, ret_subgroup);
}

ಪೂರ್ಣಾಂಕ cg_pidref_get_user_unit_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret_unit, ಅಕ್ಷರ **ret_subgroup) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        _cleanup_free_ ಅಕ್ಷರ *unit = ಖಾಲಿ, *subgroup = ಖಾಲಿ;
        r = cg_pid_get_user_unit_ತುಂಬಿದ(pidref->pid, &unit, &subgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ಆದರೆ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_machine_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_machine) {
        _cleanup_free_ ಅಕ್ಷರ *u = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sl;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_ಮಾರ್ಗ_get_unit(ಮಾರ್ಗ, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ಹಿಂತಿರುಗಿ readlink_malloc(sl, ret_machine);
}

ಪೂರ್ಣಾಂಕ cg_pid_get_machine_name(pid_t pid, ಅಕ್ಷರ **ret_machine) {
        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_machine_name(cgroup, ret_machine);
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_session(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_session) {
        _cleanup_free_ ಅಕ್ಷರ *unit = ಖಾಲಿ;
        ಅಕ್ಷರ *ಆರಂಭ, *ಕೊನೆ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = cg_ಮಾರ್ಗ_get_unit(ಮಾರ್ಗ, &unit);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆರಂಭ = startswith(unit, "session-");
        ಆದರೆ (!ಆರಂಭ)
                ಹಿಂತಿರುಗಿ -ENXIO;
        ಕೊನೆ = endswith(ಆರಂಭ, ".scope");
        ಆದರೆ (!ಕೊನೆ)
                ಹಿಂತಿರುಗಿ -ENXIO;

        *ಕೊನೆ = 0;
        ಆದರೆ (!session_id_valid(ಆರಂಭ))
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಆದರೆ (!ret_session)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret_session, ಆರಂಭ);
}

ಪೂರ್ಣಾಂಕ cg_pid_get_session(pid_t pid, ಅಕ್ಷರ **ret_session) {
        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_session(cgroup, ret_session);
}

ಪೂರ್ಣಾಂಕ cg_pidref_get_session(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        _cleanup_free_ ಅಕ್ಷರ *session = ಖಾಲಿ;
        r = cg_pid_get_session(pidref->pid, &session);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(session);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_owner_uid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uid_t *ret_uid) {
        _cleanup_free_ ಅಕ್ಷರ *ತುಂಡು = ಖಾಲಿ;
        ಅಕ್ಷರ *ಆರಂಭ, *ಕೊನೆ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = cg_ಮಾರ್ಗ_get_ತುಂಡು(ಮಾರ್ಗ, &ತುಂಡು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆರಂಭ = startswith(ತುಂಡು, "user-");
        ಆದರೆ (!ಆರಂಭ)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಕೊನೆ = endswith(ಆರಂಭ, ".slice");
        ಆದರೆ (!ಕೊನೆ)
                ಹಿಂತಿರುಗಿ -ENXIO;

        *ಕೊನೆ = 0;
        ಆದರೆ (parse_uid(ಆರಂಭ, ret_uid) < 0)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_owner_uid(cgroup, ret_uid);
}

ಪೂರ್ಣಾಂಕ cg_pidref_get_owner_uid(ಸ್ಥಿರಾಂಕ PidRef *pidref, uid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!pidref_is_ಗಣ(pidref))
                ಹಿಂತಿರುಗಿ -ESRCH;
        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = pidref_verify(pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret)
                *ret = uid;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_ತುಂಡು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret_ತುಂಡು) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
                ಪೂರ್ಣಾಂಕ n;

                n = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&p, /* accept_dot_dot= */ false, &s);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ n;
                ಆದರೆ (!valid_ತುಂಡು_name(s, n))
                        ನಿಲ್ಲಿಸು;

                e = s;
        }

        ಆದರೆ (e)
                ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_decode_unit(e, ret_ತುಂಡು);

        ಆದರೆ (ret_ತುಂಡು)
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret_ತುಂಡು, SPECIAL_ಬೇರು_ತುಂಡು);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_pid_get_ತುಂಡು(pid_t pid, ಅಕ್ಷರ **ret_ತುಂಡು) {
        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_ತುಂಡು(cgroup, ret_ತುಂಡು);
}

ಪೂರ್ಣಾಂಕ cg_ಮಾರ್ಗ_get_user_ತುಂಡು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret_ತುಂಡು) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;
        ಪ್ರತಿಜ್ಞಾ(p);

        t = skip_user_ಪೂರ್ವಪ್ರತ್ಯಯ(p);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_ತುಂಡು(t, ret_ತುಂಡು);
}

ಪೂರ್ಣಾಂಕ cg_pid_get_user_ತುಂಡು(pid_t pid, ಅಕ್ಷರ **ret_ತುಂಡು) {
        _cleanup_free_ ಅಕ್ಷರ *cgroup = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_pid_get_ಮಾರ್ಗ_shifted(pid, ಖಾಲಿ, &cgroup);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮಾರ್ಗ_get_user_ತುಂಡು(cgroup, ret_ತುಂಡು);
}

bool cg_needs_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ಆದರೆ (!filename_is_valid(p))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಒಳ_ಗಣ(p[0], '_', '.'))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (STR_ಒಳ_ಗಣ(p, "notify_on_release", "release_agent", "tasks"))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (startswith(p, "cgroup."))
                ಹಿಂತಿರುಗಿ true;

        ಗಾಗಿ (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q;

                q = startswith(p, cgroup_controller_ಗಮ್ಯ_string(c));
                ಆದರೆ (!q)
                        ಮುಂದುವರಿ;

                ಆದರೆ (q[0] == '.')
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ cg_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ಆದರೆ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ಆದರೆ (!n)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ಹಿಂತಿರುಗಿ -EINVAL;
        } ಇಲ್ಲದಿದ್ದರೆ {
                n = strdup(p);
                ಆದರೆ (!n)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* cg_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ಆದರೆ (p[0] == '_')
                ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) p+1;

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) p;
}

ಪೂರ್ಣಾಂಕ cg_ತುಂಡು_ಗಮ್ಯ_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *s = ಖಾಲಿ, *e = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dash;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(unit);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (streq(unit, SPECIAL_ಬೇರು_ತುಂಡು))
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, "");

        ಆದರೆ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!endswith(unit, ".slice"))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = unit_name_ಗಮ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ(unit, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ಆದರೆ (dash == p)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆಗ (dash) {
                _cleanup_free_ ಅಕ್ಷರ *escaped = ಖಾಲಿ;
                ಅಕ್ಷರ n[dash - p + ಗಾತ್ರ(".slice")];

#ಆದರೆ HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#ಆದರೆ_ಅಂತ್ಯ

                /* Don't allow trailing or double dashes */
                ಆದರೆ (ಒಳ_ಗಣ(dash[1], 0, '-'))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ಆದರೆ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                r = cg_escape(n, &escaped);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (!strextend(&s, escaped, "/"))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!strextend(&s, e))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_is_threaded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *fs = ಖಾಲಿ, *contents = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **v = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, "cgroup.type", &fs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = read_ತುಂಬಿದ_virtual_file(fs, &contents, ಖಾಲಿ);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ false; /* Assume no. */
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        v = strv_ವಿಭಜಿಸು(contents, ಖಾಲಿ);
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ಹಿಂತಿರುಗಿ strv_ಒಳಗೊಂಡಿದೆ(v, "threaded") || strv_ಒಳಗೊಂಡಿದೆ(v, "invalid");
}

ಪೂರ್ಣಾಂಕ cg_ಗಣ_attribute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(attribute);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, attribute, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ಹಿಂತಿರುಗಿ write_string_file(p, ಮೌಲ್ಯ, WRITE_STRING_FILE_DISABLE_ಬಫರ್|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

ಪೂರ್ಣಾಂಕ cg_get_attribute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(attribute);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, attribute, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ read_one_ಸಾಲು_file(p, ret);
}

ಪೂರ್ಣಾಂಕ cg_get_attribute_as_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        uint64_t v;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = cg_get_attribute(ಮಾರ್ಗ, attribute, &ಮೌಲ್ಯ);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ENODATA;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (streq(ಮೌಲ್ಯ, "max")) {
                *ret = CGROUP_ಎಲ್ಲೆ_MAX;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = ಸುರಕ್ಷಿತ_atou64(ಮೌಲ್ಯ, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = v;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_get_attribute_as_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_get_attribute(ಮಾರ್ಗ, attribute, &ಮೌಲ್ಯ);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -ENODATA;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_boolean(ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ cg_get_owner(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uid_t *ret_uid) {
        _cleanup_free_ ಅಕ್ಷರ *f = ಖಾಲಿ;
        ರಚನೆ stat stats;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_uid);

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, /* suffix= */ ಖಾಲಿ, &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (stat(f, &stats) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = stat_verify_directory(&stats);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret_uid = stats.st_uid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_get_keyed_attribute(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೀಲಿಗಳು,
                ಅಕ್ಷರ **values) {

        _cleanup_free_ ಅಕ್ಷರ *filename = ಖಾಲಿ, *contents = ಖಾಲಿ;
        size_t n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_ಮಾರ್ಗ(ಮಾರ್ಗ, attribute, &filename);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = read_ತುಂಬಿದ_file(filename, &contents, /* ret_size= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        n = strv_ಉದ್ದಳತೆ(ಕೀಲಿಗಳು);
        ಆದರೆ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ಹಿಂತಿರುಗಿ 0;
        ಪ್ರತಿಜ್ಞಾ(strv_is_uniq(ಕೀಲಿಗಳು));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        ಅಕ್ಷರ **v = newa0(ಅಕ್ಷರ*, n);
        size_t n_ಮುಗಿದ = 0;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = contents; *p;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *w;
                size_t i;

                ಗಾಗಿ (i = 0; i < n; i++) {
                        w = ಮೊದಲ_ಶಬ್ದ(p, ಕೀಲಿಗಳು[i]);
                        ಆದರೆ (w)
                                ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (w) {
                        ಆದರೆ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                ಹೋಗು fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ಆದರೆ (!v[i]) {
                                r = -ENOMEM;
                                ಹೋಗು fail;
                        }

                        n_ಮುಗಿದ++;
                        ಆದರೆ (n_ಮುಗಿದ >= n)
                                ನಿಲ್ಲಿಸು;

                        p = w + l;
                } ಇಲ್ಲದಿದ್ದರೆ
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ಆದರೆ (n_ಮುಗಿದ < n) {
                r = -ENXIO;
                ಹೋಗು fail;
        }

        ಸ್ಮರಣೆ_ನಕಲು(values, v, ಗಾತ್ರ(ಅಕ್ಷರ*) * n);
        ಹಿಂತಿರುಗಿ 0;

fail:
        free_many_charp(v, n);
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ cg_get_keyed_attribute_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *attribute, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, uint64_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *val = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = cg_get_keyed_attribute(ಮಾರ್ಗ, attribute, STRV_MAKE(ಕೀಲಿ), &val);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಸುರಕ್ಷಿತ_atou64(val, ret);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, ಕೀಲಿ, attribute);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_ಗಮ್ಯ_string(CGroup_ಮುಖವಾಡ ಮುಖವಾಡ_3, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (ಮುಖವಾಡ_3 == 0) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಗಾಗಿ (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k;
                size_t l;

                ಆದರೆ (!FLAGS_ಗಣ(ಮುಖವಾಡ_3, CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(c)))
                        ಮುಂದುವರಿ;

                k = cgroup_controller_ಗಮ್ಯ_string(c);
                l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(k);

                ಆದರೆ (!GREEDY_REALLOC(s, n + space + l + 1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (space)
                        s[n] = ' ';
                ಸ್ಮರಣೆ_ನಕಲು(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        ಪ್ರತಿಜ್ಞಾ(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, CGroup_ಮುಖವಾಡ *ret) {
        CGroup_ಮುಖವಾಡ m = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ;
                CGroupController v;
                ಪೂರ್ಣಾಂಕ r;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&s, &n, ಖಾಲಿ, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                v = cgroup_controller_from_string(n);
                ಆದರೆ (v < 0)
                        ಮುಂದುವರಿ;

                m |= CGROUP_CONTROLLER_ಗಮ್ಯ_ಮುಖವಾಡ(v);
        }

        *ret = m;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_supported_subtree(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, CGroup_ಮುಖವಾಡ *ret) {
        CGroup_ಮುಖವಾಡ ಮುಖವಾಡ_3;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ ಅಕ್ಷರ *controllers = ಖಾಲಿ, *ಮಾರ್ಗ = ಖಾಲಿ;
        r = cg_get_ಮಾರ್ಗ(ಬೇರು, "cgroup.controllers", &ಮಾರ್ಗ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = read_one_ಸಾಲು_file(ಮಾರ್ಗ, &controllers);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = cg_ಮುಖವಾಡ_from_string(controllers, &ಮುಖವಾಡ_3);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Mask controllers that are not supported in cgroup v2. */
        ಮುಖವಾಡ_3 &= CGROUP_ಮುಖವಾಡ_V2;

        *ret = ಮುಖವಾಡ_3;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ cg_ಮುಖವಾಡ_supported(CGroup_ಮುಖವಾಡ *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = cg_get_ಬೇರು_ಮಾರ್ಗ(&ಬೇರು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cg_ಮುಖವಾಡ_supported_subtree(ಬೇರು, ret);
}

ಪೂರ್ಣಾಂಕ cg_is_delegated(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = cg_get_xattr_bool(ಮಾರ್ಗ, "trusted.delegate");
        ಆದರೆ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಹಿಂತಿರುಗಿ r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(ಮಾರ್ಗ, "user.delegate");
        ಹಿಂತಿರುಗಿ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ಪೂರ್ಣಾಂಕ cg_is_delegated_fd(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ಖಾಲಿ, "trusted.delegate", /* at_flags= */ 0);
        ಆದರೆ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಹಿಂತಿರುಗಿ r;

        r = getxattr_at_bool(fd, /* path= */ ಖಾಲಿ, "user.delegate", /* at_flags= */ 0);
        ಹಿಂತಿರುಗಿ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ಪೂರ್ಣಾಂಕ cg_has_coredump_receive(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = cg_get_xattr_bool(ಮಾರ್ಗ, "user.coredump_receive");
        ಆದರೆ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರಾಂಕ uint64_t cgroup_io_ಎಲ್ಲೆ_defaults[CGROUP_IO_ಎಲ್ಲೆ_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_ಎಲ್ಲೆ_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_ಎಲ್ಲೆ_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_ಎಲ್ಲೆ_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_ಎಲ್ಲೆ_MAX,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ cgroup_io_ಎಲ್ಲೆ_type_ಕೋಷ್ಟಕ[CGROUP_IO_ಎಲ್ಲೆ_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(cgroup_io_ಎಲ್ಲೆ_type, CGroupIOLimitType);

ಶೂನ್ಯ cgroup_io_limits_list(ಶೂನ್ಯ) {
        DUMP_STRING_ಕೋಷ್ಟಕ(cgroup_io_ಎಲ್ಲೆ_type, CGroupIOLimitType, CGROUP_IO_ಎಲ್ಲೆ_TYPE_MAX);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ cgroup_controller_ಕೋಷ್ಟಕ[_CGROUP_CONTROLLER_MAX] = {
        [CGROUP_CONTROLLER_CPU]                             = "cpu",
        [CGROUP_CONTROLLER_CPUACCT]                         = "cpuacct",
        [CGROUP_CONTROLLER_CPUSET]                          = "cpuset",
        [CGROUP_CONTROLLER_IO]                              = "io",
        [CGROUP_CONTROLLER_BLKIO]                           = "blkio",
        [CGROUP_CONTROLLER_ಸ್ಮರಣೆ]                          = "memory",
        [CGROUP_CONTROLLER_DEVICES]                         = "devices",
        [CGROUP_CONTROLLER_PIDS]                            = "pids",
        [CGROUP_CONTROLLER_BPF_FIREWALL]                    = "bpf-firewall",
        [CGROUP_CONTROLLER_BPF_DEVICES]                     = "bpf-devices",
        [CGROUP_CONTROLLER_BPF_FOREIGN]                     = "bpf-foreign",
        [CGROUP_CONTROLLER_BPF_SOCKET_BIND]                 = "bpf-socket-bind",
        [CGROUP_CONTROLLER_BPF_RESTRICT_ಜಾಲ_INTERFACES] = "bpf-restrict-network-interfaces",
        [CGROUP_CONTROLLER_BPF_BIND_ಜಾಲ_INTERFACE]      = "bpf-bind-network-interface",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(cgroup_controller, CGroupController);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ managed_oom_ಬಹುಲಕ_ಕೋಷ್ಟಕ[MANAGED_OOM_ಬಹುಲಕ_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(managed_oom_ಬಹುಲಕ, ManagedOOMMode);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ managed_oom_preference_ಕೋಷ್ಟಕ[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(managed_oom_preference, ManagedOOMPreference);
