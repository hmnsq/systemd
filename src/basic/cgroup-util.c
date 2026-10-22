/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/fs.h>
#ضمن <linux/magic.h>
#ضمن <signal.h>
#ضمن <stdlib.h>
#ضمن <sys/xattr.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "capsule-util.h"
#ضمن "cgroup-util.h"
#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "format-util.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "login-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "set.h"
#ضمن "special.h"
#ضمن "stat-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "unaligned.h"
#ضمن "unit-name.h"
#ضمن "user-util.h"
#ضمن "xattr-util.h"

صحيح cg_is_available(فراغ) {
        بنية statfs fs;

        إذا (statfs("/sys/fs/cgroup/", &fs) < 0) {
                إذا (errno == ENOENT) /* sysfs not mounted? */
                        أرجع false;

                أرجع log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        أرجع is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

صحيح cg_path_open(ثابت حرف *path) {
        _cleanup_free_ حرف *fs = عدم;
        صحيح r;

        r = cg_get_path(path, /* suffix= */ عدم, &fs);
        إذا (r < 0)
                أرجع r;

        أرجع RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

صحيح cg_cgroupid_open(صحيح cgroupfs_fd, uint64_t id) {
        _cleanup_close_ صحيح fsfd = -EBADF;

        إذا (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                إذا (fsfd < 0)
                        أرجع -errno;

                cgroupfs_fd = fsfd;
        }

        اتحاد {
                بنية file_handle file_handle;
                uint8_t space[offsetof(بنية file_handle, f_handle) + حجم(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = حجم(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        أرجع RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

صحيح cg_path_from_cgroupid(صحيح cgroupfs_fd, uint64_t id, حرف **ret) {
        _cleanup_close_ صحيح cgfd = -EBADF;
        صحيح r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        إذا (cgfd < 0)
                أرجع cgfd;

        _cleanup_free_ حرف *path = عدم;
        r = fd_get_path(cgfd, &path);
        إذا (r < 0)
                أرجع r;

        إذا (!path_startswith(path, "/sys/fs/cgroup/"))
                أرجع -EXDEV; /* recognizable error */

        إذا (ret)
                *ret = TAKE_PTR(path);
        أرجع 0;
}

صحيح cg_enumerate_processes(ثابت حرف *path, FILE **ret) {
        _cleanup_free_ حرف *fs = عدم;
        FILE *f;
        صحيح r;

        أكد(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        إذا (r < 0)
                أرجع r;

        f = fopen(fs, "re");
        إذا (!f)
                أرجع -errno;

        *ret = f;
        أرجع 0;
}

صحيح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        أكد(f);
        أكد(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        لكل (;;) {
                errno = 0;
                إذا (fscanf(f, "%lu", &ul) != 1) {

                        إذا (feof(f)) {
                                *ret = 0;
                                أرجع 0;
                        }

                        أرجع errno_or_else(EIO);
                }

                إذا (ul > PID_T_MAX)
                        أرجع -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                إذا (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        استمر;

                *ret = (pid_t) ul;
                أرجع 1;
        }
}

صحيح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        صحيح r;

        أكد(f);
        أكد(ret);

        لكل (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                إذا (r < 0)
                        أرجع log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                إذا (r == 0) {
                        *ret = PIDREF_NULL;
                        أرجع 0;
                }

                إذا (pid == 0)
                        أرجع -EREMOTE;

                r = pidref_set_pid(ret, pid);
                إذا (r >= 0)
                        أرجع 1;
                إذا (r != -ESRCH)
                        أرجع r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(فراغ) {
        ساكن thread_local صحيح supported = -1;

        إذا (supported >= 0)
                أرجع supported;

        إذا (cg_is_available() <= 0)
                أرجع (supported = false);

        إذا (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                أرجع (supported = true);
        إذا (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        أرجع (supported = false);
}

صحيح cg_enumerate_subgroups(ثابت حرف *path, DIR **ret) {
        _cleanup_free_ حرف *fs = عدم;
        DIR *d;
        صحيح r;

        أكد(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ عدم, &fs);
        إذا (r < 0)
                أرجع r;

        d = opendir(fs);
        إذا (!d)
                أرجع -errno;

        *ret = d;
        أرجع 0;
}

صحيح cg_read_subgroup(DIR *d, حرف **ret) {
        أكد(d);
        أكد(ret);

        FOREACH_DIRENT_ALL(de, d, أرجع -errno) {
                إذا (de->d_type != DT_DIR)
                        استمر;

                إذا (dot_or_dot_dot(de->d_name))
                        استمر;

                أرجع strdup_to_full(ret, de->d_name);
        }

        *ret = عدم;
        أرجع 0;
}

صحيح cg_kill(
                ثابت حرف *path,
                صحيح sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                فراغ *userdata) {

        _cleanup_set_free_ Set *allocated_set = عدم;
        صحيح r, ret = 0;

        أكد(path);
        أكد(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        إذا (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        إذا (!killed_pids) {
                killed_pids = allocated_set = set_new(عدم);
                إذا (!killed_pids)
                        أرجع -ENOMEM;
        }

        bool done;
        افعل {
                _cleanup_fclose_ FILE *f = عدم;
                صحيح ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                إذا (r == -ENOENT)
                        اكسر;
                إذا (r < 0)
                        أرجع RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                لكل (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        إذا (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                اكسر;
                        }
                        إذا (r < 0)
                                أرجع RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        إذا (r == 0)
                                اكسر;

                        إذا ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                استمر;

                        إذا (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                استمر;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        إذا (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                استمر;
                        }

                        إذا (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        إذا (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        إذا (r >= 0) {
                                إذا (flags & CGROUP_SIGCONT)
                                        (فراغ) pidref_kill(&pidref, SIGCONT);

                                إذا (ret == 0) {
                                        إذا (log_kill)
                                                ret = ret_log_kill;
                                        وإلا
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        إذا (r < 0)
                                أرجع RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } طالما (!done);

        أرجع ret;
}

صحيح cg_kill_recursive(
                ثابت حرف *path,
                صحيح sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                فراغ *userdata) {

        _cleanup_set_free_ Set *allocated_set = عدم;
        _cleanup_closedir_ DIR *d = عدم;
        صحيح r, ret;

        أكد(path);
        أكد(sig >= 0);

        إذا (!killed_pids) {
                killed_pids = allocated_set = set_new(عدم);
                إذا (!killed_pids)
                        أرجع -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        إذا (r < 0) {
                إذا (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                أرجع ret;
        }

        لكل (;;) {
                _cleanup_free_ حرف *fn = عدم, *p = عدم;

                r = cg_read_subgroup(d, &fn);
                إذا (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        اكسر;
                }
                إذا (r == 0)
                        اكسر;

                p = path_join(empty_to_ڕەگ(path), fn);
                إذا (!p)
                        أرجع -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                إذا (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                إذا (r != 0 && ret >= 0)
                        ret = r;
        }

        أرجع ret;
}

صحيح cg_kill_kernel_sigkill(ثابت حرف *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ حرف *killfile = عدم;
        uint64_t n_pids = UINT64_MAX;
        صحيح r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        أكد(path);

        إذا (!cg_kill_supported())
                أرجع -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        إذا (r < 0)
                أرجع r;

        إذا (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                إذا (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        إذا (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        أرجع 0;
}

صحيح cg_get_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret) {
        حرف *t;

        أكد(ret);

        إذا (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        إذا (!t)
                أرجع -ENOMEM;

        *ret = path_simplify(t);
        أرجع 0;
}

صحيح cg_set_xattr(ثابت حرف *path, ثابت حرف *name, ثابت فراغ *بەها, size_t size, صحيح flags) {
        _cleanup_free_ حرف *fs = عدم;
        صحيح r;

        أكد(path);
        أكد(name);
        أكد(بەها || size <= 0);

        r = cg_get_path(path, /* suffix= */ عدم, &fs);
        إذا (r < 0)
                أرجع r;

        أرجع RET_NERRNO(setxattr(fs, name, بەها, size, flags));
}

صحيح cg_get_xattr(ثابت حرف *path, ثابت حرف *name, حرف **ret, size_t *ret_size) {
        _cleanup_free_ حرف *fs = عدم;
        صحيح r;

        أكد(path);
        أكد(name);

        r = cg_get_path(path, /* suffix= */ عدم, &fs);
        إذا (r < 0)
                أرجع r;

        أرجع lgetxattr_malloc(fs, name, ret, ret_size);
}

صحيح cg_get_xattr_bool(ثابت حرف *path, ثابت حرف *name) {
        _cleanup_free_ حرف *fs = عدم;
        صحيح r;

        أكد(path);
        أكد(name);

        r = cg_get_path(path, /* suffix= */ عدم, &fs);
        إذا (r < 0)
                أرجع r;

        أرجع getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

صحيح cg_remove_xattr(ثابت حرف *path, ثابت حرف *name) {
        _cleanup_free_ حرف *fs = عدم;
        صحيح r;

        أكد(path);
        أكد(name);

        r = cg_get_path(path, /* suffix= */ عدم, &fs);
        إذا (r < 0)
                أرجع r;

        أرجع RET_NERRNO(removexattr(fs, name));
}

صحيح cg_pid_get_path(pid_t pid, حرف **ret_path) {
        _cleanup_fclose_ FILE *f = عدم;
        ثابت حرف *fs;
        صحيح r;

        أكد(pid >= 0);
        أكد(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        إذا (r == -ENOENT)
                أرجع -ESRCH;
        إذا (r < 0)
                أرجع r;

        لكل (;;) {
                _cleanup_free_ حرف *هێڵ = عدم;
                حرف *e;

                r = read_هێڵ(f, LONG_هێڵ_MAX, &هێڵ);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع -ENODATA;

                e = startswith(هێڵ, "0:");
                إذا (!e)
                        استمر;

                e = strchr(e, ':');
                إذا (!e)
                        استمر;

                _cleanup_free_ حرف *path = strdup(e + 1);
                إذا (!path)
                        أرجع -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                إذا (startswith(path, "/../"))
                        أرجع -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                إذا (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                أرجع 0;
        }
}

صحيح cg_pidref_get_path(ثابت PidRef *pidref, حرف **ret_path) {
        _cleanup_free_ حرف *path = عدم;
        صحيح r;

        أكد(ret_path);

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        إذا (r < 0)
                أرجع r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        *ret_path = TAKE_PTR(path);
        أرجع 0;
}

صحيح cg_is_empty(ثابت حرف *path) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        أكد(path);

        /* The root cgroup is always populated */
        إذا (empty_or_ڕەگ(path))
                أرجع false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        إذا (r == -ENOENT)
                أرجع true;
        إذا (r < 0)
                أرجع r;

        أرجع streq(t, "0");
}

صحيح cg_split_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_path) {
        _cleanup_free_ حرف *controller = عدم;
        ثابت حرف *path;
        صحيح r;

        أكد(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        إذا (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                اذهب finalize;
        }

        ثابت حرف *e = strchr(spec, ':');
        إذا (!e) {
                /* Controller only. */
                إذا (ret_controller) {
                        controller = strdup(spec);
                        إذا (!controller)
                                أرجع -ENOMEM;
                }

                path = عدم;
        } وإلا {
                /* Both controller and path. */
                إذا (ret_controller) {
                        controller = strndup(spec, e - spec);
                        إذا (!controller)
                                أرجع -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        إذا (path) {
                /* Non-empty path must be absolute. */
                إذا (!path_is_absolute(path))
                        أرجع -EINVAL;

                /* Path must not contain dot-dot. */
                إذا (!path_is_safe(path))
                        أرجع -EINVAL;
        }

        إذا (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        أرجع 0;
}

صحيح cg_get_ڕەگ_path(حرف **ret_path) {
        حرف *p, *e;
        صحيح r;

        أكد(ret_path);

        r = cg_pid_get_path(1, &p);
        إذا (r < 0)
                أرجع r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        إذا (e)
                *e = 0;

        *ret_path = p;
        أرجع 0;
}

صحيح cg_shift_path(ثابت حرف *cgroup, ثابت حرف *ڕەگ, ثابت حرف **ret_shifted) {
        صحيح r;

        أكد(cgroup);
        أكد(ret_shifted);

        _cleanup_free_ حرف *rt = عدم;
        إذا (!ڕەگ) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_ڕەگ_path(&rt);
                إذا (r < 0)
                        أرجع r;

                ڕەگ = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, ڕەگ, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        أرجع 0;
}

صحيح cg_pid_get_path_shifted(pid_t pid, ثابت حرف *ڕەگ, حرف **ret_cgroup) {
        _cleanup_free_ حرف *raw = عدم;
        ثابت حرف *c;
        صحيح r;

        أكد(pid >= 0);
        أكد(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        إذا (r < 0)
                أرجع r;

        r = cg_shift_path(raw, ڕەگ, &c);
        إذا (r < 0)
                أرجع r;

        إذا (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                أرجع 0;
        }

        أرجع strdup_to(ret_cgroup, c);
}

صحيح cg_pidref_get_path_shifted(ثابت PidRef *pidref, ثابت حرف *ڕەگ, حرف **ret_cgroup) {
        _cleanup_free_ حرف *raw = عدم;
        ثابت حرف *c;
        صحيح r;

        أكد(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        إذا (r < 0)
                أرجع r;

        r = cg_shift_path(raw, ڕەگ, &c);
        إذا (r < 0)
                أرجع r;

        إذا (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                أرجع 0;
        }

        أرجع strdup_to(ret_cgroup, c);
}

صحيح cg_path_decode_unit(ثابت حرف *cgroup, حرف **ret_unit) {
        أكد(cgroup);

        size_t n = strcspn(cgroup, "/");
        إذا (n < 3)
                أرجع -ENXIO;

        حرف *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        إذا (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                أرجع -ENXIO;

        إذا (ret_unit)
                أرجع strdup_to(ret_unit, c);

        أرجع 0;
}

ساكن bool valid_slice_name(ثابت حرف *p, size_t n) {
        أكد(p || n == 0);

        إذا (n < STRLEN("x.slice"))
                أرجع false;

        حرف *c = strndupa_safe(p, n);
        إذا (!endswith(c, ".slice"))
                أرجع false;

        أرجع unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ساكن ثابت حرف* skip_slices(ثابت حرف *p) {
        أكد(p);

        /* Skips over all slice assignments */

        لكل (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                إذا (!valid_slice_name(p, n))
                        أرجع p;

                p += n;
        }
}

صحيح cg_path_get_unit_full(ثابت حرف *path, حرف **ret_unit, حرف **ret_subgroup) {
        صحيح r;

        أكد(path);

        ثابت حرف *e = skip_slices(path);

        _cleanup_free_ حرف *unit = عدم;
        r = cg_path_decode_unit(e, &unit);
        إذا (r < 0)
                أرجع r;

        /* We skipped over the slices, don't accept any now */
        إذا (endswith(unit, ".slice"))
                أرجع -ENXIO;

        إذا (ret_subgroup) {
                _cleanup_free_ حرف *subgroup = عدم;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                إذا (isempty(e))
                        subgroup = عدم;
                وإلا {
                        subgroup = strdup(e);
                        إذا (!subgroup)
                                أرجع -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        إذا (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        أرجع 0;
}

صحيح cg_path_get_unit_path(ثابت حرف *path, حرف **ret) {
        _cleanup_free_ حرف *path_copy = عدم;
        حرف *unit_name;

        أكد(path);
        أكد(ret);

        path_copy = strdup(path);
        إذا (!path_copy)
                أرجع -ENOMEM;

        unit_name = (حرف*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        إذا (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                أرجع -ENXIO;

        *ret = TAKE_PTR(path_copy);

        أرجع 0;
}

صحيح cg_pid_get_unit_full(pid_t pid, حرف **ret_unit, حرف **ret_subgroup) {
        صحيح r;

        _cleanup_free_ حرف *cgroup = عدم;
        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

صحيح cg_pidref_get_unit_full(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup) {
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        _cleanup_free_ حرف *unit = عدم, *subgroup = عدم;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        إذا (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        إذا (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        أرجع 0;
}

ساكن ثابت حرف* skip_session(ثابت حرف *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        إذا (isempty(p))
                أرجع عدم;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        إذا (n < STRLEN("session-x.scope"))
                أرجع عدم;

        ثابت حرف *s = startswith(p, "session-");
        إذا (!s)
                أرجع عدم;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        حرف *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        إذا (!e)
                أرجع عدم;
        *e = '\0';

        إذا (!session_id_valid(f))
                أرجع عدم;

        أرجع skip_leading_slash(p + n);
}

ساكن ثابت حرف* skip_user_manager(ثابت حرف *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        إذا (isempty(p))
                أرجع عدم;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        إذا (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                أرجع عدم;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ حرف *unit_name = strndup(p, n);
        إذا (!unit_name)
                أرجع عدم;

        _cleanup_free_ حرف *i = عدم;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        إذا (type != UNIT_NAME_INSTANCE)
                أرجع عدم;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        إذا (!(startswith(unit_name, "user@") && parse_uid(i, عدم) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                أرجع عدم;

        أرجع skip_leading_slash(p + n);
}

ساكن ثابت حرف* skip_user_prefix(ثابت حرف *path) {
        ثابت حرف *e, *t;

        أكد(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        إذا (t)
                أرجع t;

        /* Alternatively skip the user session if it is in the path... */
        أرجع skip_session(e);
}

صحيح cg_path_get_user_unit_full(ثابت حرف *path, حرف **ret_unit, حرف **ret_subgroup) {
        ثابت حرف *t;

        أكد(path);

        t = skip_user_prefix(path);
        إذا (!t)
                أرجع -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        أرجع cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

صحيح cg_pid_get_user_unit_full(pid_t pid, حرف **ret_unit, حرف **ret_subgroup) {
        صحيح r;

        _cleanup_free_ حرف *cgroup = عدم;
        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

صحيح cg_pidref_get_user_unit_full(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup) {
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        _cleanup_free_ حرف *unit = عدم, *subgroup = عدم;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        إذا (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        إذا (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        أرجع 0;
}

صحيح cg_path_get_machine_name(ثابت حرف *path, حرف **ret_machine) {
        _cleanup_free_ حرف *u = عدم;
        ثابت حرف *sl;
        صحيح r;

        r = cg_path_get_unit(path, &u);
        إذا (r < 0)
                أرجع r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        أرجع readlink_malloc(sl, ret_machine);
}

صحيح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine) {
        _cleanup_free_ حرف *cgroup = عدم;
        صحيح r;

        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_machine_name(cgroup, ret_machine);
}

صحيح cg_path_get_session(ثابت حرف *path, حرف **ret_session) {
        _cleanup_free_ حرف *unit = عدم;
        حرف *start, *end;
        صحيح r;

        أكد(path);

        r = cg_path_get_unit(path, &unit);
        إذا (r < 0)
                أرجع r;

        start = startswith(unit, "session-");
        إذا (!start)
                أرجع -ENXIO;
        end = endswith(start, ".scope");
        إذا (!end)
                أرجع -ENXIO;

        *end = 0;
        إذا (!session_id_valid(start))
                أرجع -ENXIO;

        إذا (!ret_session)
                أرجع 0;

        أرجع strdup_to(ret_session, start);
}

صحيح cg_pid_get_session(pid_t pid, حرف **ret_session) {
        _cleanup_free_ حرف *cgroup = عدم;
        صحيح r;

        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_session(cgroup, ret_session);
}

صحيح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret) {
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        _cleanup_free_ حرف *session = عدم;
        r = cg_pid_get_session(pidref->pid, &session);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = TAKE_PTR(session);
        أرجع 0;
}

صحيح cg_path_get_owner_uid(ثابت حرف *path, uid_t *ret_uid) {
        _cleanup_free_ حرف *slice = عدم;
        حرف *start, *end;
        صحيح r;

        أكد(path);

        r = cg_path_get_slice(path, &slice);
        إذا (r < 0)
                أرجع r;

        start = startswith(slice, "user-");
        إذا (!start)
                أرجع -ENXIO;

        end = endswith(start, ".slice");
        إذا (!end)
                أرجع -ENXIO;

        *end = 0;
        إذا (parse_uid(start, ret_uid) < 0)
                أرجع -ENXIO;

        أرجع 0;
}

صحيح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ حرف *cgroup = عدم;
        صحيح r;

        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_owner_uid(cgroup, ret_uid);
}

صحيح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret) {
        صحيح r;

        إذا (!pidref_is_set(pidref))
                أرجع -ESRCH;
        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        إذا (r < 0)
                أرجع r;

        r = pidref_verify(pidref);
        إذا (r < 0)
                أرجع r;

        إذا (ret)
                *ret = uid;

        أرجع 0;
}

صحيح cg_path_get_slice(ثابت حرف *p, حرف **ret_slice) {
        ثابت حرف *e = عدم;

        أكد(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        لكل (;;) {
                ثابت حرف *s;
                صحيح n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                إذا (n < 0)
                        أرجع n;
                إذا (!valid_slice_name(s, n))
                        اكسر;

                e = s;
        }

        إذا (e)
                أرجع cg_path_decode_unit(e, ret_slice);

        إذا (ret_slice)
                أرجع strdup_to(ret_slice, SPECIAL_ڕەگ_SLICE);

        أرجع 0;
}

صحيح cg_pid_get_slice(pid_t pid, حرف **ret_slice) {
        _cleanup_free_ حرف *cgroup = عدم;
        صحيح r;

        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_slice(cgroup, ret_slice);
}

صحيح cg_path_get_user_slice(ثابت حرف *p, حرف **ret_slice) {
        ثابت حرف *t;
        أكد(p);

        t = skip_user_prefix(p);
        إذا (!t)
                أرجع -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        أرجع cg_path_get_slice(t, ret_slice);
}

صحيح cg_pid_get_user_slice(pid_t pid, حرف **ret_slice) {
        _cleanup_free_ حرف *cgroup = عدم;
        صحيح r;

        r = cg_pid_get_path_shifted(pid, عدم, &cgroup);
        إذا (r < 0)
                أرجع r;

        أرجع cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(ثابت حرف *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        إذا (!filename_is_valid(p))
                أرجع true;

        إذا (IN_SET(p[0], '_', '.'))
                أرجع true;

        إذا (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                أرجع true;

        إذا (startswith(p, "cgroup."))
                أرجع true;

        لكل (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ثابت حرف *q;

                q = startswith(p, cgroup_controller_to_string(c));
                إذا (!q)
                        استمر;

                إذا (q[0] == '.')
                        أرجع true;
        }

        أرجع false;
}

صحيح cg_escape(ثابت حرف *p, حرف **ret) {
        _cleanup_free_ حرف *n = عدم;

        أكد(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        إذا (cg_needs_escape(p)) {
                n = strjoin("_", p);
                إذا (!n)
                        أرجع -ENOMEM;

                إذا (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        أرجع -EINVAL;
        } وإلا {
                n = strdup(p);
                إذا (!n)
                        أرجع -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        أرجع 0;
}

حرف* cg_unescape(ثابت حرف *p) {
        أكد(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        إذا (p[0] == '_')
                أرجع (حرف*) p+1;

        أرجع (حرف*) p;
}

صحيح cg_slice_to_path(ثابت حرف *unit, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم, *e = عدم;
        ثابت حرف *dash;
        صحيح r;

        أكد(unit);
        أكد(ret);

        إذا (streq(unit, SPECIAL_ڕەگ_SLICE))
                أرجع strdup_to(ret, "");

        إذا (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                أرجع -EINVAL;

        إذا (!endswith(unit, ".slice"))
                أرجع -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        إذا (r < 0)
                أرجع r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        إذا (dash == p)
                أرجع -EINVAL;

        طالما (dash) {
                _cleanup_free_ حرف *escaped = عدم;
                حرف n[dash - p + حجم(".slice")];

#إذا HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                إذا (IN_SET(dash[1], 0, '-'))
                        أرجع -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                إذا (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        أرجع -EINVAL;

                r = cg_escape(n, &escaped);
                إذا (r < 0)
                        أرجع r;

                إذا (!strextend(&s, escaped, "/"))
                        أرجع -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        إذا (r < 0)
                أرجع r;

        إذا (!strextend(&s, e))
                أرجع -ENOMEM;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح cg_is_threaded(ثابت حرف *path) {
        _cleanup_free_ حرف *fs = عدم, *contents = عدم;
        _cleanup_strv_free_ حرف **v = عدم;
        صحيح r;

        r = cg_get_path(path, "cgroup.type", &fs);
        إذا (r < 0)
                أرجع r;

        r = read_full_virtual_file(fs, &contents, عدم);
        إذا (r == -ENOENT)
                أرجع false; /* Assume no. */
        إذا (r < 0)
                أرجع r;

        v = strv_split(contents, عدم);
        إذا (!v)
                أرجع -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        أرجع strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

صحيح cg_set_attribute(ثابت حرف *path, ثابت حرف *attribute, ثابت حرف *بەها) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(attribute);

        r = cg_get_path(path, attribute, &p);
        إذا (r < 0)
                أرجع r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        أرجع write_string_file(p, بەها, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

صحيح cg_get_attribute(ثابت حرف *path, ثابت حرف *attribute, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(attribute);

        r = cg_get_path(path, attribute, &p);
        إذا (r < 0)
                أرجع r;

        أرجع read_one_هێڵ_file(p, ret);
}

صحيح cg_get_attribute_as_uint64(ثابت حرف *path, ثابت حرف *attribute, uint64_t *ret) {
        _cleanup_free_ حرف *بەها = عدم;
        uint64_t v;
        صحيح r;

        أكد(ret);

        r = cg_get_attribute(path, attribute, &بەها);
        إذا (r == -ENOENT)
                أرجع -ENODATA;
        إذا (r < 0)
                أرجع r;

        إذا (streq(بەها, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                أرجع 0;
        }

        r = safe_atou64(بەها, &v);
        إذا (r < 0)
                أرجع r;

        *ret = v;
        أرجع 0;
}

صحيح cg_get_attribute_as_bool(ثابت حرف *path, ثابت حرف *attribute) {
        _cleanup_free_ حرف *بەها = عدم;
        صحيح r;

        r = cg_get_attribute(path, attribute, &بەها);
        إذا (r == -ENOENT)
                أرجع -ENODATA;
        إذا (r < 0)
                أرجع r;

        أرجع parse_boolean(بەها);
}

صحيح cg_get_owner(ثابت حرف *path, uid_t *ret_uid) {
        _cleanup_free_ حرف *f = عدم;
        بنية stat stats;
        صحيح r;

        أكد(ret_uid);

        r = cg_get_path(path, /* suffix= */ عدم, &f);
        إذا (r < 0)
                أرجع r;

        إذا (stat(f, &stats) < 0)
                أرجع -errno;

        r = stat_verify_directory(&stats);
        إذا (r < 0)
                أرجع r;

        *ret_uid = stats.st_uid;
        أرجع 0;
}

صحيح cg_get_keyed_attribute(
                ثابت حرف *path,
                ثابت حرف *attribute,
                حرف * ثابت *کلیلەکان,
                حرف **values) {

        _cleanup_free_ حرف *filename = عدم, *contents = عدم;
        size_t n;
        صحيح r;

        أكد(path);
        أكد(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        إذا (r < 0)
                أرجع r;

        r = read_full_file(filename, &contents, /* ret_size= */ عدم);
        إذا (r < 0)
                أرجع r;

        n = strv_length(کلیلەکان);
        إذا (n == 0) /* No keys to retrieve? That's easy, we are done then */
                أرجع 0;
        أكد(strv_is_uniq(کلیلەکان));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        حرف **v = newa0(حرف*, n);
        size_t n_done = 0;

        لكل (ثابت حرف *p = contents; *p;) {
                ثابت حرف *w;
                size_t i;

                لكل (i = 0; i < n; i++) {
                        w = first_word(p, کلیلەکان[i]);
                        إذا (w)
                                اكسر;
                }

                إذا (w) {
                        إذا (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                اذهب fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        إذا (!v[i]) {
                                r = -ENOMEM;
                                اذهب fail;
                        }

                        n_done++;
                        إذا (n_done >= n)
                                اكسر;

                        p = w + l;
                } وإلا
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        إذا (n_done < n) {
                r = -ENXIO;
                اذهب fail;
        }

        memcpy(values, v, حجم(حرف*) * n);
        أرجع 0;

fail:
        free_many_charp(v, n);
        أرجع r;
}

صحيح cg_get_keyed_attribute_uint64(ثابت حرف *path, ثابت حرف *attribute, ثابت حرف *کلیل, uint64_t *ret) {
        _cleanup_free_ حرف *val = عدم;
        صحيح r;

        أكد(کلیل);
        أكد(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(کلیل), &val);
        إذا (r < 0)
                أرجع r;

        r = safe_atou64(val, ret);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, کلیل, attribute);

        أرجع 0;
}

صحيح cg_mask_to_string(CGroupMask mask, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        أكد(ret);

        إذا (mask == 0) {
                *ret = عدم;
                أرجع 0;
        }

        لكل (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ثابت حرف *k;
                size_t l;

                إذا (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        استمر;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                إذا (!GREEDY_REALLOC(s, n + space + l + 1))
                        أرجع -ENOMEM;

                إذا (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        أكد(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        أرجع 0;
}

صحيح cg_mask_from_string(ثابت حرف *s, CGroupMask *ret) {
        CGroupMask m = 0;

        أكد(ret);
        أكد(s);

        لكل (;;) {
                _cleanup_free_ حرف *n = عدم;
                CGroupController v;
                صحيح r;

                r = extract_first_word(&s, &n, عدم, 0);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                v = cgroup_controller_from_string(n);
                إذا (v < 0)
                        استمر;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        أرجع 0;
}

صحيح cg_mask_supported_subtree(ثابت حرف *ڕەگ, CGroupMask *ret) {
        CGroupMask mask;
        صحيح r;

        أكد(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ حرف *controllers = عدم, *path = عدم;
        r = cg_get_path(ڕەگ, "cgroup.controllers", &path);
        إذا (r < 0)
                أرجع r;

        r = read_one_هێڵ_file(path, &controllers);
        إذا (r < 0)
                أرجع r;

        r = cg_mask_from_string(controllers, &mask);
        إذا (r < 0)
                أرجع r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        أرجع 0;
}

صحيح cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ حرف *ڕەگ = عدم;
        صحيح r;

        r = cg_get_ڕەگ_path(&ڕەگ);
        إذا (r < 0)
                أرجع r;

        أرجع cg_mask_supported_subtree(ڕەگ, ret);
}

صحيح cg_is_delegated(ثابت حرف *path) {
        صحيح r;

        أكد(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        إذا (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                أرجع r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        أرجع ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

صحيح cg_is_delegated_fd(صحيح fd) {
        صحيح r;

        أكد(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ عدم, "trusted.delegate", /* at_flags= */ 0);
        إذا (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                أرجع r;

        r = getxattr_at_bool(fd, /* path= */ عدم, "user.delegate", /* at_flags= */ 0);
        أرجع ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

صحيح cg_has_coredump_receive(ثابت حرف *path) {
        صحيح r;

        أكد(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        إذا (ERRNO_IS_NEG_XATTR_ABSENT(r))
                أرجع false;

        أرجع r;
}

ثابت uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

ساكن ثابت حرف* ثابت cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

فراغ cgroup_io_limits_list(فراغ) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

ساكن ثابت حرف *ثابت cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

ساكن ثابت حرف* ثابت managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

ساكن ثابت حرف* ثابت managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
