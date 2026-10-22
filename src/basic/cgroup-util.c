/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/fs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/magic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/xattr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capsule-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cgroup-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "login-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "set.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "special.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unit-name.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xattr-util.h"

ਪੂਰਨ cg_is_ਉਪਲਬਧ(ਖਾਲੀ) {
        ਬਣਤਰ statfs fs;

        ਜੇ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ਜੇ (errno == ENOENT) /* sysfs not mounted? */
                        ਵਾਪਸ false;

                ਵਾਪਸ log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ਵਾਪਸ is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

ਪੂਰਨ cg_ਰਸਤਾ_open(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

ਪੂਰਨ cg_cgroupid_open(ਪੂਰਨ cgroupfs_fd, uint64_t id) {
        _cleanup_close_ ਪੂਰਨ fsfd = -EBADF;

        ਜੇ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ਜੇ (fsfd < 0)
                        ਵਾਪਸ -errno;

                cgroupfs_fd = fsfd;
        }

        ਸੰਘ {
                ਬਣਤਰ file_handle file_handle;
                uint8_t space[offsetof(ਬਣਤਰ file_handle, f_handle) + ਆਕਾਰ(uint64_t)];
        } fh = {
                .file_handle.handle_ਬਾਈਟਾਂ = ਆਕਾਰ(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ਵਾਪਸ RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

ਪੂਰਨ cg_ਰਸਤਾ_from_cgroupid(ਪੂਰਨ cgroupfs_fd, uint64_t id, ਅੱਖਰ **ret) {
        _cleanup_close_ ਪੂਰਨ cgfd = -EBADF;
        ਪੂਰਨ r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ਜੇ (cgfd < 0)
                ਵਾਪਸ cgfd;

        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
        r = fd_get_ਰਸਤਾ(cgfd, &ਰਸਤਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!ਰਸਤਾ_startswith(ਰਸਤਾ, "/sys/fs/cgroup/"))
                ਵਾਪਸ -EXDEV; /* recognizable error */

        ਜੇ (ret)
                *ret = TAKE_PTR(ਰਸਤਾ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_enumerate_processes(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        FILE *f;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, "cgroup.procs", &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        f = fopen(fs, "re");
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        *ret = f;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        ਲਈ (;;) {
                errno = 0;
                ਜੇ (fscanf(f, "%lu", &ul) != 1) {

                        ਜੇ (feof(f)) {
                                *ret = 0;
                                ਵਾਪਸ 0;
                        }

                        ਵਾਪਸ errno_or_else(EIO);
                }

                ਜੇ (ul > PID_T_MAX)
                        ਵਾਪਸ -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ਜੇ (ul == 0 && !FLAGS_ਸੈੱਟ(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        ਜਾਰੀ;

                *ret = (pid_t) ul;
                ਵਾਪਸ 1;
        }
}

ਪੂਰਨ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ਜੇ (r == 0) {
                        *ret = PIDREF_NULL;
                        ਵਾਪਸ 0;
                }

                ਜੇ (pid == 0)
                        ਵਾਪਸ -EREMOTE;

                r = pidref_ਸੈੱਟ_pid(ret, pid);
                ਜੇ (r >= 0)
                        ਵਾਪਸ 1;
                ਜੇ (r != -ESRCH)
                        ਵਾਪਸ r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(ਖਾਲੀ) {
        ਸਥਿਰ thread_local ਪੂਰਨ supported = -1;

        ਜੇ (supported >= 0)
                ਵਾਪਸ supported;

        ਜੇ (cg_is_ਉਪਲਬਧ() <= 0)
                ਵਾਪਸ (supported = false);

        ਜੇ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ਵਾਪਸ (supported = true);
        ਜੇ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ਵਾਪਸ (supported = false);
}

ਪੂਰਨ cg_enumerate_subgroups(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, DIR **ret) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        DIR *d;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This is not recursive! */

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        d = opendir(fs);
        ਜੇ (!d)
                ਵਾਪਸ -errno;

        *ret = d;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_read_subgroup(DIR *d, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(d);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        FOREACH_DIRENT_ALL(de, d, ਵਾਪਸ -errno) {
                ਜੇ (de->d_type != DT_DIR)
                        ਜਾਰੀ;

                ਜੇ (dot_or_dot_dot(de->d_name))
                        ਜਾਰੀ;

                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret, de->d_name);
        }

        *ret = ਨਿਰਾ;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_kill(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ sig,
                CGroupFlags flags,
                ਸੈੱਟ_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                ਖਾਲੀ *userdata) {

        cleanup_ਸੈੱਟ_free ਸੈੱਟ_2 *allocated_ਸੈੱਟ = ਨਿਰਾ;
        ਪੂਰਨ r, ret = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ਜੇ (!killed_pids) {
                killed_pids = allocated_ਸੈੱਟ = ਸੈੱਟ_new(ਨਿਰਾ);
                ਜੇ (!killed_pids)
                        ਵਾਪਸ -ENOMEM;
        }

        bool ਪੂਰਾ;
        ਕਰੋ {
                _cleanup_fclose_ FILE *f = ਨਿਰਾ;
                ਪੂਰਨ ret_log_kill;

                ਪੂਰਾ = true;

                r = cg_enumerate_processes(ਰਸਤਾ, &f);
                ਜੇ (r == -ENOENT)
                        ਰੋਕੋ;
                ਜੇ (r < 0)
                        ਵਾਪਸ RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                ਲਈ (;;) {
                        _cleanup_(pidref_ਪੂਰਾ) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ਜੇ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                ਪੂਰਾ = true;
                                ਰੋਕੋ;
                        }
                        ਜੇ (r < 0)
                                ਵਾਪਸ RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", ਰਸਤਾ));
                        ਜੇ (r == 0)
                                ਰੋਕੋ;

                        ਜੇ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                ਜਾਰੀ;

                        ਜੇ (ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(killed_pids, PID_ਮੰਜ਼ਿਲ_PTR(pidref.pid)))
                                ਜਾਰੀ;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ਜੇ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, ਰਸਤਾ);
                                ਜਾਰੀ;
                        }

                        ਜੇ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ਜੇ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, ਰਸਤਾ));
                        ਜੇ (r >= 0) {
                                ਜੇ (flags & CGROUP_SIGCONT)
                                        (ਖਾਲੀ) pidref_kill(&pidref, SIGCONT);

                                ਜੇ (ret == 0) {
                                        ਜੇ (log_kill)
                                                ret = ret_log_kill;
                                        ਨਹੀਂਤਾਂ
                                                ret = 1;
                                }
                        }

                        ਪੂਰਾ = false;

                        r = ਸੈੱਟ_put(killed_pids, PID_ਮੰਜ਼ਿਲ_PTR(pidref.pid));
                        ਜੇ (r < 0)
                                ਵਾਪਸ RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ਜਦੋਂ (!ਪੂਰਾ);

        ਵਾਪਸ ret;
}

ਪੂਰਨ cg_kill_recursive(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ sig,
                CGroupFlags flags,
                ਸੈੱਟ_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                ਖਾਲੀ *userdata) {

        cleanup_ਸੈੱਟ_free ਸੈੱਟ_2 *allocated_ਸੈੱਟ = ਨਿਰਾ;
        _cleanup_closedir_ DIR *d = ਨਿਰਾ;
        ਪੂਰਨ r, ret;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(sig >= 0);

        ਜੇ (!killed_pids) {
                killed_pids = allocated_ਸੈੱਟ = ਸੈੱਟ_new(ਨਿਰਾ);
                ਜੇ (!killed_pids)
                        ਵਾਪਸ -ENOMEM;
        }

        ret = cg_kill(ਰਸਤਾ, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(ਰਸਤਾ, &d);
        ਜੇ (r < 0) {
                ਜੇ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", ਰਸਤਾ));

                ਵਾਪਸ ret;
        }

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *fn = ਨਿਰਾ, *p = ਨਿਰਾ;

                r = cg_read_subgroup(d, &fn);
                ਜੇ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", ਰਸਤਾ));
                        ਰੋਕੋ;
                }
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                p = ਰਸਤਾ_join(ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਰਸਤਾ), fn);
                ਜੇ (!p)
                        ਵਾਪਸ -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ਜੇ (r != 0 && ret >= 0)
                        ret = r;
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ cg_kill_kernel_sigkill(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ ਅੱਖਰ *killfile = ਨਿਰਾ;
        uint64_t n_pids = UINT64_MAX;
        ਪੂਰਨ r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        ਜੇ (!cg_kill_supported())
                ਵਾਪਸ -EOPNOTSUPP;

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, "cgroup.kill", &killfile);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(ਰਸਤਾ, "pids.current", &n_pids);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", ਰਸਤਾ);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_ਬਫਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", ਰਸਤਾ);

        ਜੇ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_get_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret) {
        ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (isempty(ਰਸਤਾ))
                ਰਸਤਾ = TAKE_PTR(suffix);

        t = ਰਸਤਾ_join("/sys/fs/cgroup", ਰਸਤਾ, suffix);
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        *ret = ਰਸਤਾ_simplify(t);
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਸੈੱਟ_xattr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਖਾਲੀ *ਮੁੱਲ, size_t ਮਾਪ, ਪੂਰਨ flags) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ || ਮਾਪ <= 0);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ RET_NERRNO(setxattr(fs, name, ਮੁੱਲ, ਮਾਪ, flags));
}

ਪੂਰਨ cg_get_xattr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ lgetxattr_malloc(fs, name, ret, ret_ਮਾਪ);
}

ਪੂਰਨ cg_get_xattr_bool(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

ਪੂਰਨ cg_remove_xattr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ RET_NERRNO(removexattr(fs, name));
}

ਪੂਰਨ cg_pid_get_ਰਸਤਾ(pid_t pid, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *fs;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਰਸਤਾ);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ESRCH;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                ਅੱਖਰ *e;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ -ENODATA;

                e = startswith(ਸਤਰ, "0:");
                ਜੇ (!e)
                        ਜਾਰੀ;

                e = strchr(e, ':');
                ਜੇ (!e)
                        ਜਾਰੀ;

                _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = strdup(e + 1);
                ਜੇ (!ਰਸਤਾ)
                        ਵਾਪਸ -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ਜੇ (startswith(ਰਸਤਾ, "/../"))
                        ਵਾਪਸ -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(ਰਸਤਾ, " (deleted)");
                ਜੇ (e)
                        *e = 0;

                *ret_ਰਸਤਾ = TAKE_PTR(ਰਸਤਾ);
                ਵਾਪਸ 0;
        }
}

ਪੂਰਨ cg_pidref_get_ਰਸਤਾ(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਰਸਤਾ);

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_ਰਸਤਾ(pidref->pid, &ਰਸਤਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret_ਰਸਤਾ = TAKE_PTR(ਰਸਤਾ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* The root cgroup is always populated */
        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਰਸਤਾ))
                ਵਾਪਸ false;

        r = cg_get_keyed_attribute(ਰਸਤਾ, "cgroup.events", STRV_MAKE("populated"), &t);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ true;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ streq(t, "0");
}

ਪੂਰਨ cg_ਵੰਡੋ_spec(ਅਚਲ ਅੱਖਰ *spec, ਅੱਖਰ **ret_controller, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *controller = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ਜੇ (isempty(spec) || ਰਸਤਾ_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                ਰਸਤਾ = spec;
                ਜਾਓ finalize;
        }

        ਅਚਲ ਅੱਖਰ *e = strchr(spec, ':');
        ਜੇ (!e) {
                /* Controller only. */
                ਜੇ (ret_controller) {
                        controller = strdup(spec);
                        ਜੇ (!controller)
                                ਵਾਪਸ -ENOMEM;
                }

                ਰਸਤਾ = ਨਿਰਾ;
        } ਨਹੀਂਤਾਂ {
                /* Both controller and path. */
                ਜੇ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ਜੇ (!controller)
                                ਵਾਪਸ -ENOMEM;
                }

                ਰਸਤਾ = e + 1;
        }

finalize:
        ਰਸਤਾ = ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(ਰਸਤਾ);

        ਜੇ (ਰਸਤਾ) {
                /* Non-empty path must be absolute. */
                ਜੇ (!ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                        ਵਾਪਸ -EINVAL;

                /* Path must not contain dot-dot. */
                ਜੇ (!ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(ਰਸਤਾ))
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (ret_ਰਸਤਾ) {
                r = ਰਸਤਾ_simplify_alloc(ਰਸਤਾ, ret_ਰਸਤਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_get_ਜੜ੍ਹ_ਰਸਤਾ(ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਅੱਖਰ *p, *e;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਰਸਤਾ);

        r = cg_pid_get_ਰਸਤਾ(1, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ਜੇ (e)
                *e = 0;

        *ret_ਰਸਤਾ = p;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਖਿਸਕਾਓ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *cgroup, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ **ret_shifted) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(cgroup);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_shifted);

        _cleanup_free_ ਅੱਖਰ *rt = ਨਿਰਾ;
        ਜੇ (!ਜੜ੍ਹ) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_ਜੜ੍ਹ_ਰਸਤਾ(&rt);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੜ੍ਹ = rt;
        }

        *ret_shifted = ਰਸਤਾ_startswith_ਭਰਿਆ(cgroup, ਜੜ੍ਹ, ਰਸਤਾ_STARTSWITH_RETURN_LEADING_SLASH|ਰਸਤਾ_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_pid_get_ਰਸਤਾ_shifted(pid_t pid, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret_cgroup) {
        _cleanup_free_ ਅੱਖਰ *raw = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_cgroup);

        r = cg_pid_get_ਰਸਤਾ(pid, &raw);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = cg_ਖਿਸਕਾਓ_ਰਸਤਾ(raw, ਜੜ੍ਹ, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret_cgroup, c);
}

ਪੂਰਨ cg_pidref_get_ਰਸਤਾ_shifted(ਅਚਲ PidRef *pidref, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret_cgroup) {
        _cleanup_free_ ਅੱਖਰ *raw = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_cgroup);

        r = cg_pidref_get_ਰਸਤਾ(pidref, &raw);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = cg_ਖਿਸਕਾਓ_ਰਸਤਾ(raw, ਜੜ੍ਹ, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret_cgroup, c);
}

ਪੂਰਨ cg_ਰਸਤਾ_decode_unit(ਅਚਲ ਅੱਖਰ *cgroup, ਅੱਖਰ **ret_unit) {
        ਪ੍ਰਤਿਜ੍ਞਾ(cgroup);

        size_t n = strcspn(cgroup, "/");
        ਜੇ (n < 3)
                ਵਾਪਸ -ENXIO;

        ਅੱਖਰ *c = strndupa_ਸੁਰੱਖਿਅਤ(cgroup, n);
        c = cg_unescape(c);

        ਜੇ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ਵਾਪਸ -ENXIO;

        ਜੇ (ret_unit)
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret_unit, c);

        ਵਾਪਸ 0;
}

ਸਥਿਰ bool valid_ਟੁਕੜਾ_name(ਅਚਲ ਅੱਖਰ *p, size_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p || n == 0);

        ਜੇ (n < STRLEN("x.slice"))
                ਵਾਪਸ false;

        ਅੱਖਰ *c = strndupa_ਸੁਰੱਖਿਅਤ(p, n);
        ਜੇ (!endswith(c, ".slice"))
                ਵਾਪਸ false;

        ਵਾਪਸ unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* skip_slices(ਅਚਲ ਅੱਖਰ *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Skips over all slice assignments */

        ਲਈ (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ਜੇ (!valid_ਟੁਕੜਾ_name(p, n))
                        ਵਾਪਸ p;

                p += n;
        }
}

ਪੂਰਨ cg_ਰਸਤਾ_get_unit_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        ਅਚਲ ਅੱਖਰ *e = skip_slices(ਰਸਤਾ);

        _cleanup_free_ ਅੱਖਰ *unit = ਨਿਰਾ;
        r = cg_ਰਸਤਾ_decode_unit(e, &unit);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* We skipped over the slices, don't accept any now */
        ਜੇ (endswith(unit, ".slice"))
                ਵਾਪਸ -ENXIO;

        ਜੇ (ret_subgroup) {
                _cleanup_free_ ਅੱਖਰ *subgroup = ਨਿਰਾ;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ਜੇ (isempty(e))
                        subgroup = ਨਿਰਾ;
                ਨਹੀਂਤਾਂ {
                        subgroup = strdup(e);
                        ਜੇ (!subgroup)
                                ਵਾਪਸ -ENOMEM;
                }

                ਰਸਤਾ_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ਜੇ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਰਸਤਾ_get_unit_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ_copy = ਨਿਰਾ;
        ਅੱਖਰ *unit_name;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਰਸਤਾ_copy = strdup(ਰਸਤਾ);
        ਜੇ (!ਰਸਤਾ_copy)
                ਵਾਪਸ -ENOMEM;

        unit_name = (ਅੱਖਰ*) skip_slices(ਰਸਤਾ_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ਜੇ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ਵਾਪਸ -ENXIO;

        *ret = TAKE_PTR(ਰਸਤਾ_copy);

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_pid_get_unit_ਭਰਿਆ(pid_t pid, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup) {
        ਪੂਰਨ r;

        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_unit_ਭਰਿਆ(cgroup, ret_unit, ret_subgroup);
}

ਪੂਰਨ cg_pidref_get_unit_ਭਰਿਆ(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        _cleanup_free_ ਅੱਖਰ *unit = ਨਿਰਾ, *subgroup = ਨਿਰਾ;
        r = cg_pid_get_unit_ਭਰਿਆ(pidref->pid, &unit, &subgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ਜੇ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* skip_session(ਅਚਲ ਅੱਖਰ *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ਜੇ (isempty(p))
                ਵਾਪਸ ਨਿਰਾ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ਜੇ (n < STRLEN("session-x.scope"))
                ਵਾਪਸ ਨਿਰਾ;

        ਅਚਲ ਅੱਖਰ *s = startswith(p, "session-");
        ਜੇ (!s)
                ਵਾਪਸ ਨਿਰਾ;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        ਅੱਖਰ *f = strndupa_ਸੁਰੱਖਿਅਤ(s, p + n - s),
             *e = endswith(f, ".scope");
        ਜੇ (!e)
                ਵਾਪਸ ਨਿਰਾ;
        *e = '\0';

        ਜੇ (!session_id_valid(f))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ skip_leading_slash(p + n);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* skip_user_manager(ਅਚਲ ਅੱਖਰ *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ਜੇ (isempty(p))
                ਵਾਪਸ ਨਿਰਾ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ਜੇ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ਵਾਪਸ ਨਿਰਾ;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ ਅੱਖਰ *unit_name = strndup(p, n);
        ਜੇ (!unit_name)
                ਵਾਪਸ ਨਿਰਾ;

        _cleanup_free_ ਅੱਖਰ *i = ਨਿਰਾ;
        UnitNameFlags type = unit_name_ਮੰਜ਼ਿਲ_instance(unit_name, &i);

        ਜੇ (type != UNIT_NAME_INSTANCE)
                ਵਾਪਸ ਨਿਰਾ;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ਜੇ (!(startswith(unit_name, "user@") && parse_uid(i, ਨਿਰਾ) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ skip_leading_slash(p + n);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* skip_user_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਅਚਲ ਅੱਖਰ *e, *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Skip slices, if there are any */
        e = skip_slices(ਰਸਤਾ);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ਜੇ (t)
                ਵਾਪਸ t;

        /* Alternatively skip the user session if it is in the path... */
        ਵਾਪਸ skip_session(e);
}

ਪੂਰਨ cg_ਰਸਤਾ_get_user_unit_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup) {
        ਅਚਲ ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        t = skip_user_ਅਗੇਤਰ(ਰਸਤਾ);
        ਜੇ (!t)
                ਵਾਪਸ -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ਵਾਪਸ cg_ਰਸਤਾ_get_unit_ਭਰਿਆ(t, ret_unit, ret_subgroup);
}

ਪੂਰਨ cg_pid_get_user_unit_ਭਰਿਆ(pid_t pid, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup) {
        ਪੂਰਨ r;

        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_user_unit_ਭਰਿਆ(cgroup, ret_unit, ret_subgroup);
}

ਪੂਰਨ cg_pidref_get_user_unit_ਭਰਿਆ(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret_unit, ਅੱਖਰ **ret_subgroup) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        _cleanup_free_ ਅੱਖਰ *unit = ਨਿਰਾ, *subgroup = ਨਿਰਾ;
        r = cg_pid_get_user_unit_ਭਰਿਆ(pidref->pid, &unit, &subgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ਜੇ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਰਸਤਾ_get_machine_name(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_machine) {
        _cleanup_free_ ਅੱਖਰ *u = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *sl;
        ਪੂਰਨ r;

        r = cg_ਰਸਤਾ_get_unit(ਰਸਤਾ, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ਵਾਪਸ readlink_malloc(sl, ret_machine);
}

ਪੂਰਨ cg_pid_get_machine_name(pid_t pid, ਅੱਖਰ **ret_machine) {
        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_machine_name(cgroup, ret_machine);
}

ਪੂਰਨ cg_ਰਸਤਾ_get_session(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_session) {
        _cleanup_free_ ਅੱਖਰ *unit = ਨਿਰਾ;
        ਅੱਖਰ *ਸ਼ੁਰੂ, *ਸਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = cg_ਰਸਤਾ_get_unit(ਰਸਤਾ, &unit);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਸ਼ੁਰੂ = startswith(unit, "session-");
        ਜੇ (!ਸ਼ੁਰੂ)
                ਵਾਪਸ -ENXIO;
        ਸਿਰਾ = endswith(ਸ਼ੁਰੂ, ".scope");
        ਜੇ (!ਸਿਰਾ)
                ਵਾਪਸ -ENXIO;

        *ਸਿਰਾ = 0;
        ਜੇ (!session_id_valid(ਸ਼ੁਰੂ))
                ਵਾਪਸ -ENXIO;

        ਜੇ (!ret_session)
                ਵਾਪਸ 0;

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret_session, ਸ਼ੁਰੂ);
}

ਪੂਰਨ cg_pid_get_session(pid_t pid, ਅੱਖਰ **ret_session) {
        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_session(cgroup, ret_session);
}

ਪੂਰਨ cg_pidref_get_session(ਅਚਲ PidRef *pidref, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        _cleanup_free_ ਅੱਖਰ *session = ਨਿਰਾ;
        r = cg_pid_get_session(pidref->pid, &session);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = TAKE_PTR(session);
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਰਸਤਾ_get_owner_uid(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uid_t *ret_uid) {
        _cleanup_free_ ਅੱਖਰ *ਟੁਕੜਾ = ਨਿਰਾ;
        ਅੱਖਰ *ਸ਼ੁਰੂ, *ਸਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = cg_ਰਸਤਾ_get_ਟੁਕੜਾ(ਰਸਤਾ, &ਟੁਕੜਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਸ਼ੁਰੂ = startswith(ਟੁਕੜਾ, "user-");
        ਜੇ (!ਸ਼ੁਰੂ)
                ਵਾਪਸ -ENXIO;

        ਸਿਰਾ = endswith(ਸ਼ੁਰੂ, ".slice");
        ਜੇ (!ਸਿਰਾ)
                ਵਾਪਸ -ENXIO;

        *ਸਿਰਾ = 0;
        ਜੇ (parse_uid(ਸ਼ੁਰੂ, ret_uid) < 0)
                ਵਾਪਸ -ENXIO;

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_owner_uid(cgroup, ret_uid);
}

ਪੂਰਨ cg_pidref_get_owner_uid(ਅਚਲ PidRef *pidref, uid_t *ret) {
        ਪੂਰਨ r;

        ਜੇ (!pidref_is_ਸੈੱਟ(pidref))
                ਵਾਪਸ -ESRCH;
        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = pidref_verify(pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret)
                *ret = uid;

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਰਸਤਾ_get_ਟੁਕੜਾ(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret_ਟੁਕੜਾ) {
        ਅਚਲ ਅੱਖਰ *e = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *s;
                ਪੂਰਨ n;

                n = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&p, /* accept_dot_dot= */ false, &s);
                ਜੇ (n < 0)
                        ਵਾਪਸ n;
                ਜੇ (!valid_ਟੁਕੜਾ_name(s, n))
                        ਰੋਕੋ;

                e = s;
        }

        ਜੇ (e)
                ਵਾਪਸ cg_ਰਸਤਾ_decode_unit(e, ret_ਟੁਕੜਾ);

        ਜੇ (ret_ਟੁਕੜਾ)
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret_ਟੁਕੜਾ, SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_pid_get_ਟੁਕੜਾ(pid_t pid, ਅੱਖਰ **ret_ਟੁਕੜਾ) {
        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_ਟੁਕੜਾ(cgroup, ret_ਟੁਕੜਾ);
}

ਪੂਰਨ cg_ਰਸਤਾ_get_user_ਟੁਕੜਾ(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret_ਟੁਕੜਾ) {
        ਅਚਲ ਅੱਖਰ *t;
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        t = skip_user_ਅਗੇਤਰ(p);
        ਜੇ (!t)
                ਵਾਪਸ -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ਵਾਪਸ cg_ਰਸਤਾ_get_ਟੁਕੜਾ(t, ret_ਟੁਕੜਾ);
}

ਪੂਰਨ cg_pid_get_user_ਟੁਕੜਾ(pid_t pid, ਅੱਖਰ **ret_ਟੁਕੜਾ) {
        _cleanup_free_ ਅੱਖਰ *cgroup = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_pid_get_ਰਸਤਾ_shifted(pid, ਨਿਰਾ, &cgroup);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਰਸਤਾ_get_user_ਟੁਕੜਾ(cgroup, ret_ਟੁਕੜਾ);
}

bool cg_needs_escape(ਅਚਲ ਅੱਖਰ *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ਜੇ (!filename_is_valid(p))
                ਵਾਪਸ true;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(p[0], '_', '.'))
                ਵਾਪਸ true;

        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(p, "notify_on_release", "release_agent", "tasks"))
                ਵਾਪਸ true;

        ਜੇ (startswith(p, "cgroup."))
                ਵਾਪਸ true;

        ਲਈ (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ਅਚਲ ਅੱਖਰ *q;

                q = startswith(p, cgroup_controller_ਮੰਜ਼ਿਲ_string(c));
                ਜੇ (!q)
                        ਜਾਰੀ;

                ਜੇ (q[0] == '.')
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਪੂਰਨ cg_escape(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ਜੇ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ਜੇ (!n)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ਵਾਪਸ -EINVAL;
        } ਨਹੀਂਤਾਂ {
                n = strdup(p);
                ਜੇ (!n)
                        ਵਾਪਸ -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ਵਾਪਸ 0;
}

ਅੱਖਰ* cg_unescape(ਅਚਲ ਅੱਖਰ *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ਜੇ (p[0] == '_')
                ਵਾਪਸ (ਅੱਖਰ*) p+1;

        ਵਾਪਸ (ਅੱਖਰ*) p;
}

ਪੂਰਨ cg_ਟੁਕੜਾ_ਮੰਜ਼ਿਲ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *unit, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *s = ਨਿਰਾ, *e = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *dash;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(unit);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (streq(unit, SPECIAL_ਜੜ੍ਹ_ਟੁਕੜਾ))
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, "");

        ਜੇ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!endswith(unit, ".slice"))
                ਵਾਪਸ -EINVAL;

        r = unit_name_ਮੰਜ਼ਿਲ_ਅਗੇਤਰ(unit, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ਜੇ (dash == p)
                ਵਾਪਸ -EINVAL;

        ਜਦੋਂ (dash) {
                _cleanup_free_ ਅੱਖਰ *escaped = ਨਿਰਾ;
                ਅੱਖਰ n[dash - p + ਆਕਾਰ(".slice")];

#ਜੇ HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#ਜੇ_ਅੰਤ

                /* Don't allow trailing or double dashes */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(dash[1], 0, '-'))
                        ਵਾਪਸ -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ਜੇ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ਵਾਪਸ -EINVAL;

                r = cg_escape(n, &escaped);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (!strextend(&s, escaped, "/"))
                        ਵਾਪਸ -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!strextend(&s, e))
                ਵਾਪਸ -ENOMEM;

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_is_threaded(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *fs = ਨਿਰਾ, *contents = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **v = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, "cgroup.type", &fs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = read_ਭਰਿਆ_virtual_file(fs, &contents, ਨਿਰਾ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ false; /* Assume no. */
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        v = strv_ਵੰਡੋ(contents, ਨਿਰਾ);
        ਜੇ (!v)
                ਵਾਪਸ -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ਵਾਪਸ strv_ਸ਼ਾਮਲ_ਹੈ(v, "threaded") || strv_ਸ਼ਾਮਲ_ਹੈ(v, "invalid");
}

ਪੂਰਨ cg_ਸੈੱਟ_attribute(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(attribute);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, attribute, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ਵਾਪਸ write_string_file(p, ਮੁੱਲ, WRITE_STRING_FILE_DISABLE_ਬਫਰ|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

ਪੂਰਨ cg_get_attribute(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(attribute);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, attribute, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ read_one_ਸਤਰ_file(p, ret);
}

ਪੂਰਨ cg_get_attribute_as_uint64(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        uint64_t v;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = cg_get_attribute(ਰਸਤਾ, attribute, &ਮੁੱਲ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ENODATA;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (streq(ਮੁੱਲ, "max")) {
                *ret = CGROUP_ਹੱਦਬੰਦੀ_MAX;
                ਵਾਪਸ 0;
        }

        r = ਸੁਰੱਖਿਅਤ_atou64(ਮੁੱਲ, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = v;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_get_attribute_as_bool(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_get_attribute(ਰਸਤਾ, attribute, &ਮੁੱਲ);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -ENODATA;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_boolean(ਮੁੱਲ);
}

ਪੂਰਨ cg_get_owner(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uid_t *ret_uid) {
        _cleanup_free_ ਅੱਖਰ *f = ਨਿਰਾ;
        ਬਣਤਰ stat stats;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_uid);

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, /* suffix= */ ਨਿਰਾ, &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (stat(f, &stats) < 0)
                ਵਾਪਸ -errno;

        r = stat_verify_directory(&stats);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret_uid = stats.st_uid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_get_keyed_attribute(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *attribute,
                ਅੱਖਰ * ਅਚਲ *ਕੁੰਜੀਆਂ,
                ਅੱਖਰ **values) {

        _cleanup_free_ ਅੱਖਰ *filename = ਨਿਰਾ, *contents = ਨਿਰਾ;
        size_t n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_ਰਸਤਾ(ਰਸਤਾ, attribute, &filename);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = read_ਭਰਿਆ_file(filename, &contents, /* ret_size= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        n = strv_ਲੰਮਾਈ(ਕੁੰਜੀਆਂ);
        ਜੇ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ਵਾਪਸ 0;
        ਪ੍ਰਤਿਜ੍ਞਾ(strv_is_uniq(ਕੁੰਜੀਆਂ));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        ਅੱਖਰ **v = newa0(ਅੱਖਰ*, n);
        size_t n_ਪੂਰਾ = 0;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = contents; *p;) {
                ਅਚਲ ਅੱਖਰ *w;
                size_t i;

                ਲਈ (i = 0; i < n; i++) {
                        w = ਪਹਿਲਾ_ਸ਼ਬਦ(p, ਕੁੰਜੀਆਂ[i]);
                        ਜੇ (w)
                                ਰੋਕੋ;
                }

                ਜੇ (w) {
                        ਜੇ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                ਜਾਓ fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ਜੇ (!v[i]) {
                                r = -ENOMEM;
                                ਜਾਓ fail;
                        }

                        n_ਪੂਰਾ++;
                        ਜੇ (n_ਪੂਰਾ >= n)
                                ਰੋਕੋ;

                        p = w + l;
                } ਨਹੀਂਤਾਂ
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ਜੇ (n_ਪੂਰਾ < n) {
                r = -ENXIO;
                ਜਾਓ fail;
        }

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(values, v, ਆਕਾਰ(ਅੱਖਰ*) * n);
        ਵਾਪਸ 0;

fail:
        free_many_charp(v, n);
        ਵਾਪਸ r;
}

ਪੂਰਨ cg_get_keyed_attribute_uint64(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *attribute, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, uint64_t *ret) {
        _cleanup_free_ ਅੱਖਰ *val = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = cg_get_keyed_attribute(ਰਸਤਾ, attribute, STRV_MAKE(ਕੁੰਜੀ), &val);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਸੁਰੱਖਿਅਤ_atou64(val, ret);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, ਕੁੰਜੀ, attribute);

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਨਕਾਬ_ਮੰਜ਼ਿਲ_string(CGroup_ਨਕਾਬ ਨਕਾਬ_3, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (ਨਕਾਬ_3 == 0) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਲਈ (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ਅਚਲ ਅੱਖਰ *k;
                size_t l;

                ਜੇ (!FLAGS_ਸੈੱਟ(ਨਕਾਬ_3, CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(c)))
                        ਜਾਰੀ;

                k = cgroup_controller_ਮੰਜ਼ਿਲ_string(c);
                l = ਲੜੀ_ਲੰਬਾਈ(k);

                ਜੇ (!GREEDY_REALLOC(s, n + space + l + 1))
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (space)
                        s[n] = ' ';
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਨਕਾਬ_from_string(ਅਚਲ ਅੱਖਰ *s, CGroup_ਨਕਾਬ *ret) {
        CGroup_ਨਕਾਬ m = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ;
                CGroupController v;
                ਪੂਰਨ r;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&s, &n, ਨਿਰਾ, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                v = cgroup_controller_from_string(n);
                ਜੇ (v < 0)
                        ਜਾਰੀ;

                m |= CGROUP_CONTROLLER_ਮੰਜ਼ਿਲ_ਨਕਾਬ(v);
        }

        *ret = m;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਨਕਾਬ_supported_subtree(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, CGroup_ਨਕਾਬ *ret) {
        CGroup_ਨਕਾਬ ਨਕਾਬ_3;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ ਅੱਖਰ *controllers = ਨਿਰਾ, *ਰਸਤਾ = ਨਿਰਾ;
        r = cg_get_ਰਸਤਾ(ਜੜ੍ਹ, "cgroup.controllers", &ਰਸਤਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = read_one_ਸਤਰ_file(ਰਸਤਾ, &controllers);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = cg_ਨਕਾਬ_from_string(controllers, &ਨਕਾਬ_3);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Mask controllers that are not supported in cgroup v2. */
        ਨਕਾਬ_3 &= CGROUP_ਨਕਾਬ_V2;

        *ret = ਨਕਾਬ_3;
        ਵਾਪਸ 0;
}

ਪੂਰਨ cg_ਨਕਾਬ_supported(CGroup_ਨਕਾਬ *ret) {
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = cg_get_ਜੜ੍ਹ_ਰਸਤਾ(&ਜੜ੍ਹ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cg_ਨਕਾਬ_supported_subtree(ਜੜ੍ਹ, ret);
}

ਪੂਰਨ cg_is_delegated(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = cg_get_xattr_bool(ਰਸਤਾ, "trusted.delegate");
        ਜੇ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ਵਾਪਸ r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(ਰਸਤਾ, "user.delegate");
        ਵਾਪਸ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ਪੂਰਨ cg_is_delegated_fd(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ਨਿਰਾ, "trusted.delegate", /* at_flags= */ 0);
        ਜੇ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ਵਾਪਸ r;

        r = getxattr_at_bool(fd, /* path= */ ਨਿਰਾ, "user.delegate", /* at_flags= */ 0);
        ਵਾਪਸ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ਪੂਰਨ cg_has_coredump_receive(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = cg_get_xattr_bool(ਰਸਤਾ, "user.coredump_receive");
        ਜੇ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ਵਾਪਸ false;

        ਵਾਪਸ r;
}

ਅਚਲ uint64_t cgroup_io_ਹੱਦਬੰਦੀ_defaults[CGROUP_IO_ਹੱਦਬੰਦੀ_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_ਹੱਦਬੰਦੀ_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_ਹੱਦਬੰਦੀ_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_ਹੱਦਬੰਦੀ_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_ਹੱਦਬੰਦੀ_MAX,
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ cgroup_io_ਹੱਦਬੰਦੀ_type_ਸਾਰਣੀ[CGROUP_IO_ਹੱਦਬੰਦੀ_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(cgroup_io_ਹੱਦਬੰਦੀ_type, CGroupIOLimitType);

ਖਾਲੀ cgroup_io_limits_list(ਖਾਲੀ) {
        DUMP_STRING_ਸਾਰਣੀ(cgroup_io_ਹੱਦਬੰਦੀ_type, CGroupIOLimitType, CGROUP_IO_ਹੱਦਬੰਦੀ_TYPE_MAX);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ cgroup_controller_ਸਾਰਣੀ[_CGROUP_CONTROLLER_MAX] = {
        [CGROUP_CONTROLLER_CPU]                             = "cpu",
        [CGROUP_CONTROLLER_CPUACCT]                         = "cpuacct",
        [CGROUP_CONTROLLER_CPUSET]                          = "cpuset",
        [CGROUP_CONTROLLER_IO]                              = "io",
        [CGROUP_CONTROLLER_BLKIO]                           = "blkio",
        [CGROUP_CONTROLLER_ਸਮ੍ਰਿਤੀ]                          = "memory",
        [CGROUP_CONTROLLER_DEVICES]                         = "devices",
        [CGROUP_CONTROLLER_PIDS]                            = "pids",
        [CGROUP_CONTROLLER_BPF_FIREWALL]                    = "bpf-firewall",
        [CGROUP_CONTROLLER_BPF_DEVICES]                     = "bpf-devices",
        [CGROUP_CONTROLLER_BPF_FOREIGN]                     = "bpf-foreign",
        [CGROUP_CONTROLLER_BPF_SOCKET_BIND]                 = "bpf-socket-bind",
        [CGROUP_CONTROLLER_BPF_RESTRICT_ਜਾਲ_INTERFACES] = "bpf-restrict-network-interfaces",
        [CGROUP_CONTROLLER_BPF_BIND_ਜਾਲ_INTERFACE]      = "bpf-bind-network-interface",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(cgroup_controller, CGroupController);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ managed_oom_ਬਹੁਲਕ_ਸਾਰਣੀ[MANAGED_OOM_ਬਹੁਲਕ_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(managed_oom_ਬਹੁਲਕ, ManagedOOMMode);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ managed_oom_preference_ਸਾਰਣੀ[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(managed_oom_preference, ManagedOOMPreference);
