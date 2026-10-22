/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/fs.h>
#شامل <linux/magic.h>
#شامل <signal.h>
#شامل <stdlib.h>
#شامل <sys/xattr.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "capsule-util.h"
#شامل "cgroup-util.h"
#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "format-util.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "login-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "set.h"
#شامل "special.h"
#شامل "stat-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "unaligned.h"
#شامل "unit-name.h"
#شامل "user-util.h"
#شامل "xattr-util.h"

صحیح cg_is_available(تهی) {
        ساختار statfs fs;

        اگر (statfs("/sys/fs/cgroup/", &fs) < 0) {
                اگر (errno == ENOENT) /* sysfs not mounted? */
                        بازگشت false;

                بازگشت log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        بازگشت is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

صحیح cg_path_open(ثابت نویسه *path) {
        _cleanup_free_ نویسه *fs = پوچ;
        صحیح r;

        r = cg_get_path(path, /* suffix= */ پوچ, &fs);
        اگر (r < 0)
                بازگشت r;

        بازگشت RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

صحیح cg_cgroupid_open(صحیح cgroupfs_fd, uint64_t id) {
        _cleanup_close_ صحیح fsfd = -EBADF;

        اگر (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                اگر (fsfd < 0)
                        بازگشت -errno;

                cgroupfs_fd = fsfd;
        }

        اجتماع {
                ساختار file_handle file_handle;
                uint8_t space[offsetof(ساختار file_handle, f_handle) + اندازه(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = اندازه(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        بازگشت RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

صحیح cg_path_from_cgroupid(صحیح cgroupfs_fd, uint64_t id, نویسه **ret) {
        _cleanup_close_ صحیح cgfd = -EBADF;
        صحیح r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        اگر (cgfd < 0)
                بازگشت cgfd;

        _cleanup_free_ نویسه *path = پوچ;
        r = fd_get_path(cgfd, &path);
        اگر (r < 0)
                بازگشت r;

        اگر (!path_startswith(path, "/sys/fs/cgroup/"))
                بازگشت -EXDEV; /* recognizable error */

        اگر (ret)
                *ret = TAKE_PTR(path);
        بازگشت 0;
}

صحیح cg_enumerate_processes(ثابت نویسه *path, FILE **ret) {
        _cleanup_free_ نویسه *fs = پوچ;
        FILE *f;
        صحیح r;

        أكد(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        اگر (r < 0)
                بازگشت r;

        f = fopen(fs, "re");
        اگر (!f)
                بازگشت -errno;

        *ret = f;
        بازگشت 0;
}

صحیح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        أكد(f);
        أكد(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        برای (;;) {
                errno = 0;
                اگر (fscanf(f, "%lu", &ul) != 1) {

                        اگر (feof(f)) {
                                *ret = 0;
                                بازگشت 0;
                        }

                        بازگشت errno_or_else(EIO);
                }

                اگر (ul > PID_T_MAX)
                        بازگشت -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                اگر (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        ادامه;

                *ret = (pid_t) ul;
                بازگشت 1;
        }
}

صحیح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        صحیح r;

        أكد(f);
        أكد(ret);

        برای (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                اگر (r == 0) {
                        *ret = PIDREF_NULL;
                        بازگشت 0;
                }

                اگر (pid == 0)
                        بازگشت -EREMOTE;

                r = pidref_set_pid(ret, pid);
                اگر (r >= 0)
                        بازگشت 1;
                اگر (r != -ESRCH)
                        بازگشت r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(تهی) {
        ایستا thread_local صحیح supported = -1;

        اگر (supported >= 0)
                بازگشت supported;

        اگر (cg_is_available() <= 0)
                بازگشت (supported = false);

        اگر (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                بازگشت (supported = true);
        اگر (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        بازگشت (supported = false);
}

صحیح cg_enumerate_subgroups(ثابت نویسه *path, DIR **ret) {
        _cleanup_free_ نویسه *fs = پوچ;
        DIR *d;
        صحیح r;

        أكد(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ پوچ, &fs);
        اگر (r < 0)
                بازگشت r;

        d = opendir(fs);
        اگر (!d)
                بازگشت -errno;

        *ret = d;
        بازگشت 0;
}

صحیح cg_read_subgroup(DIR *d, نویسه **ret) {
        أكد(d);
        أكد(ret);

        FOREACH_DIRENT_ALL(de, d, بازگشت -errno) {
                اگر (de->d_type != DT_DIR)
                        ادامه;

                اگر (dot_or_dot_dot(de->d_name))
                        ادامه;

                بازگشت strdup_to_full(ret, de->d_name);
        }

        *ret = پوچ;
        بازگشت 0;
}

صحیح cg_kill(
                ثابت نویسه *path,
                صحیح sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                تهی *userdata) {

        _cleanup_set_free_ Set *allocated_set = پوچ;
        صحیح r, ret = 0;

        أكد(path);
        أكد(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        اگر (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        اگر (!killed_pids) {
                killed_pids = allocated_set = set_new(پوچ);
                اگر (!killed_pids)
                        بازگشت -ENOMEM;
        }

        bool done;
        انجامبده {
                _cleanup_fclose_ FILE *f = پوچ;
                صحیح ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                اگر (r == -ENOENT)
                        بشکن;
                اگر (r < 0)
                        بازگشت RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                برای (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        اگر (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                بشکن;
                        }
                        اگر (r < 0)
                                بازگشت RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        اگر (r == 0)
                                بشکن;

                        اگر ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                ادامه;

                        اگر (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                ادامه;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        اگر (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                ادامه;
                        }

                        اگر (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        اگر (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        اگر (r >= 0) {
                                اگر (flags & CGROUP_SIGCONT)
                                        (تهی) pidref_kill(&pidref, SIGCONT);

                                اگر (ret == 0) {
                                        اگر (log_kill)
                                                ret = ret_log_kill;
                                        وگرنه
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        اگر (r < 0)
                                بازگشت RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } وقتی (!done);

        بازگشت ret;
}

صحیح cg_kill_recursive(
                ثابت نویسه *path,
                صحیح sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                تهی *userdata) {

        _cleanup_set_free_ Set *allocated_set = پوچ;
        _cleanup_closedir_ DIR *d = پوچ;
        صحیح r, ret;

        أكد(path);
        أكد(sig >= 0);

        اگر (!killed_pids) {
                killed_pids = allocated_set = set_new(پوچ);
                اگر (!killed_pids)
                        بازگشت -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        اگر (r < 0) {
                اگر (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                بازگشت ret;
        }

        برای (;;) {
                _cleanup_free_ نویسه *fn = پوچ, *p = پوچ;

                r = cg_read_subgroup(d, &fn);
                اگر (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        بشکن;
                }
                اگر (r == 0)
                        بشکن;

                p = path_join(empty_to_ریشه(path), fn);
                اگر (!p)
                        بازگشت -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                اگر (r != 0 && ret >= 0)
                        ret = r;
        }

        بازگشت ret;
}

صحیح cg_kill_kernel_sigkill(ثابت نویسه *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ نویسه *killfile = پوچ;
        uint64_t n_pids = UINT64_MAX;
        صحیح r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        أكد(path);

        اگر (!cg_kill_supported())
                بازگشت -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        اگر (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        بازگشت 0;
}

صحیح cg_get_path(ثابت نویسه *path, ثابت نویسه *suffix, نویسه **ret) {
        نویسه *t;

        أكد(ret);

        اگر (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        اگر (!t)
                بازگشت -ENOMEM;

        *ret = path_simplify(t);
        بازگشت 0;
}

صحیح cg_set_xattr(ثابت نویسه *path, ثابت نویسه *name, ثابت تهی *مقدار, size_t size, صحیح flags) {
        _cleanup_free_ نویسه *fs = پوچ;
        صحیح r;

        أكد(path);
        أكد(name);
        أكد(مقدار || size <= 0);

        r = cg_get_path(path, /* suffix= */ پوچ, &fs);
        اگر (r < 0)
                بازگشت r;

        بازگشت RET_NERRNO(setxattr(fs, name, مقدار, size, flags));
}

صحیح cg_get_xattr(ثابت نویسه *path, ثابت نویسه *name, نویسه **ret, size_t *ret_size) {
        _cleanup_free_ نویسه *fs = پوچ;
        صحیح r;

        أكد(path);
        أكد(name);

        r = cg_get_path(path, /* suffix= */ پوچ, &fs);
        اگر (r < 0)
                بازگشت r;

        بازگشت lgetxattr_malloc(fs, name, ret, ret_size);
}

صحیح cg_get_xattr_bool(ثابت نویسه *path, ثابت نویسه *name) {
        _cleanup_free_ نویسه *fs = پوچ;
        صحیح r;

        أكد(path);
        أكد(name);

        r = cg_get_path(path, /* suffix= */ پوچ, &fs);
        اگر (r < 0)
                بازگشت r;

        بازگشت getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

صحیح cg_remove_xattr(ثابت نویسه *path, ثابت نویسه *name) {
        _cleanup_free_ نویسه *fs = پوچ;
        صحیح r;

        أكد(path);
        أكد(name);

        r = cg_get_path(path, /* suffix= */ پوچ, &fs);
        اگر (r < 0)
                بازگشت r;

        بازگشت RET_NERRNO(removexattr(fs, name));
}

صحیح cg_pid_get_path(pid_t pid, نویسه **ret_path) {
        _cleanup_fclose_ FILE *f = پوچ;
        ثابت نویسه *fs;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        اگر (r == -ENOENT)
                بازگشت -ESRCH;
        اگر (r < 0)
                بازگشت r;

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ;
                نویسه *e;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت -ENODATA;

                e = startswith(سطر, "0:");
                اگر (!e)
                        ادامه;

                e = strchr(e, ':');
                اگر (!e)
                        ادامه;

                _cleanup_free_ نویسه *path = strdup(e + 1);
                اگر (!path)
                        بازگشت -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                اگر (startswith(path, "/../"))
                        بازگشت -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                اگر (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                بازگشت 0;
        }
}

صحیح cg_pidref_get_path(ثابت PidRef *pidref, نویسه **ret_path) {
        _cleanup_free_ نویسه *path = پوچ;
        صحیح r;

        أكد(ret_path);

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;
        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        اگر (r < 0)
                بازگشت r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        اگر (r < 0)
                بازگشت r;

        *ret_path = TAKE_PTR(path);
        بازگشت 0;
}

صحیح cg_is_empty(ثابت نویسه *path) {
        _cleanup_free_ نویسه *t = پوچ;
        صحیح r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        أكد(path);

        /* The root cgroup is always populated */
        اگر (empty_or_ریشه(path))
                بازگشت false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        اگر (r == -ENOENT)
                بازگشت true;
        اگر (r < 0)
                بازگشت r;

        بازگشت streq(t, "0");
}

صحیح cg_split_spec(ثابت نویسه *spec, نویسه **ret_controller, نویسه **ret_path) {
        _cleanup_free_ نویسه *controller = پوچ;
        ثابت نویسه *path;
        صحیح r;

        أكد(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        اگر (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                برو finalize;
        }

        ثابت نویسه *e = strchr(spec, ':');
        اگر (!e) {
                /* Controller only. */
                اگر (ret_controller) {
                        controller = strdup(spec);
                        اگر (!controller)
                                بازگشت -ENOMEM;
                }

                path = پوچ;
        } وگرنه {
                /* Both controller and path. */
                اگر (ret_controller) {
                        controller = strndup(spec, e - spec);
                        اگر (!controller)
                                بازگشت -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        اگر (path) {
                /* Non-empty path must be absolute. */
                اگر (!path_is_absolute(path))
                        بازگشت -EINVAL;

                /* Path must not contain dot-dot. */
                اگر (!path_is_safe(path))
                        بازگشت -EINVAL;
        }

        اگر (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        بازگشت 0;
}

صحیح cg_get_ریشه_path(نویسه **ret_path) {
        نویسه *p, *e;
        صحیح r;

        أكد(ret_path);

        r = cg_pid_get_path(1, &p);
        اگر (r < 0)
                بازگشت r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        اگر (e)
                *e = 0;

        *ret_path = p;
        بازگشت 0;
}

صحیح cg_shift_path(ثابت نویسه *cgroup, ثابت نویسه *ریشه, ثابت نویسه **ret_shifted) {
        صحیح r;

        أكد(cgroup);
        أكد(ret_shifted);

        _cleanup_free_ نویسه *rt = پوچ;
        اگر (!ریشه) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_ریشه_path(&rt);
                اگر (r < 0)
                        بازگشت r;

                ریشه = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, ریشه, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        بازگشت 0;
}

صحیح cg_pid_get_path_shifted(pid_t pid, ثابت نویسه *ریشه, نویسه **ret_cgroup) {
        _cleanup_free_ نویسه *raw = پوچ;
        ثابت نویسه *c;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        اگر (r < 0)
                بازگشت r;

        r = cg_shift_path(raw, ریشه, &c);
        اگر (r < 0)
                بازگشت r;

        اگر (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                بازگشت 0;
        }

        بازگشت strdup_to(ret_cgroup, c);
}

صحیح cg_pidref_get_path_shifted(ثابت PidRef *pidref, ثابت نویسه *ریشه, نویسه **ret_cgroup) {
        _cleanup_free_ نویسه *raw = پوچ;
        ثابت نویسه *c;
        صحیح r;

        أكد(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        اگر (r < 0)
                بازگشت r;

        r = cg_shift_path(raw, ریشه, &c);
        اگر (r < 0)
                بازگشت r;

        اگر (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                بازگشت 0;
        }

        بازگشت strdup_to(ret_cgroup, c);
}

صحیح cg_path_decode_unit(ثابت نویسه *cgroup, نویسه **ret_unit) {
        أكد(cgroup);

        size_t n = strcspn(cgroup, "/");
        اگر (n < 3)
                بازگشت -ENXIO;

        نویسه *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        اگر (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                بازگشت -ENXIO;

        اگر (ret_unit)
                بازگشت strdup_to(ret_unit, c);

        بازگشت 0;
}

ایستا bool valid_slice_name(ثابت نویسه *p, size_t n) {
        أكد(p || n == 0);

        اگر (n < STRLEN("x.slice"))
                بازگشت false;

        نویسه *c = strndupa_safe(p, n);
        اگر (!endswith(c, ".slice"))
                بازگشت false;

        بازگشت unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ایستا ثابت نویسه* skip_slices(ثابت نویسه *p) {
        أكد(p);

        /* Skips over all slice assignments */

        برای (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                اگر (!valid_slice_name(p, n))
                        بازگشت p;

                p += n;
        }
}

صحیح cg_path_get_unit_full(ثابت نویسه *path, نویسه **ret_unit, نویسه **ret_subgroup) {
        صحیح r;

        أكد(path);

        ثابت نویسه *e = skip_slices(path);

        _cleanup_free_ نویسه *unit = پوچ;
        r = cg_path_decode_unit(e, &unit);
        اگر (r < 0)
                بازگشت r;

        /* We skipped over the slices, don't accept any now */
        اگر (endswith(unit, ".slice"))
                بازگشت -ENXIO;

        اگر (ret_subgroup) {
                _cleanup_free_ نویسه *subgroup = پوچ;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                اگر (isempty(e))
                        subgroup = پوچ;
                وگرنه {
                        subgroup = strdup(e);
                        اگر (!subgroup)
                                بازگشت -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        اگر (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        بازگشت 0;
}

صحیح cg_path_get_unit_path(ثابت نویسه *path, نویسه **ret) {
        _cleanup_free_ نویسه *path_copy = پوچ;
        نویسه *unit_name;

        أكد(path);
        أكد(ret);

        path_copy = strdup(path);
        اگر (!path_copy)
                بازگشت -ENOMEM;

        unit_name = (نویسه*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        اگر (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                بازگشت -ENXIO;

        *ret = TAKE_PTR(path_copy);

        بازگشت 0;
}

صحیح cg_pid_get_unit_full(pid_t pid, نویسه **ret_unit, نویسه **ret_subgroup) {
        صحیح r;

        _cleanup_free_ نویسه *cgroup = پوچ;
        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

صحیح cg_pidref_get_unit_full(ثابت PidRef *pidref, نویسه **ret_unit, نویسه **ret_subgroup) {
        صحیح r;

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;
        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        _cleanup_free_ نویسه *unit = پوچ, *subgroup = پوچ;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        اگر (r < 0)
                بازگشت r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        اگر (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        بازگشت 0;
}

ایستا ثابت نویسه* skip_session(ثابت نویسه *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        اگر (isempty(p))
                بازگشت پوچ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        اگر (n < STRLEN("session-x.scope"))
                بازگشت پوچ;

        ثابت نویسه *s = startswith(p, "session-");
        اگر (!s)
                بازگشت پوچ;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        نویسه *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        اگر (!e)
                بازگشت پوچ;
        *e = '\0';

        اگر (!session_id_valid(f))
                بازگشت پوچ;

        بازگشت skip_leading_slash(p + n);
}

ایستا ثابت نویسه* skip_user_manager(ثابت نویسه *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        اگر (isempty(p))
                بازگشت پوچ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        اگر (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                بازگشت پوچ;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ نویسه *unit_name = strndup(p, n);
        اگر (!unit_name)
                بازگشت پوچ;

        _cleanup_free_ نویسه *i = پوچ;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        اگر (type != UNIT_NAME_INSTANCE)
                بازگشت پوچ;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        اگر (!(startswith(unit_name, "user@") && parse_uid(i, پوچ) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                بازگشت پوچ;

        بازگشت skip_leading_slash(p + n);
}

ایستا ثابت نویسه* skip_user_prefix(ثابت نویسه *path) {
        ثابت نویسه *e, *t;

        أكد(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        اگر (t)
                بازگشت t;

        /* Alternatively skip the user session if it is in the path... */
        بازگشت skip_session(e);
}

صحیح cg_path_get_user_unit_full(ثابت نویسه *path, نویسه **ret_unit, نویسه **ret_subgroup) {
        ثابت نویسه *t;

        أكد(path);

        t = skip_user_prefix(path);
        اگر (!t)
                بازگشت -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        بازگشت cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

صحیح cg_pid_get_user_unit_full(pid_t pid, نویسه **ret_unit, نویسه **ret_subgroup) {
        صحیح r;

        _cleanup_free_ نویسه *cgroup = پوچ;
        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

صحیح cg_pidref_get_user_unit_full(ثابت PidRef *pidref, نویسه **ret_unit, نویسه **ret_subgroup) {
        صحیح r;

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;
        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        _cleanup_free_ نویسه *unit = پوچ, *subgroup = پوچ;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        اگر (r < 0)
                بازگشت r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        اگر (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        بازگشت 0;
}

صحیح cg_path_get_machine_name(ثابت نویسه *path, نویسه **ret_machine) {
        _cleanup_free_ نویسه *u = پوچ;
        ثابت نویسه *sl;
        صحیح r;

        r = cg_path_get_unit(path, &u);
        اگر (r < 0)
                بازگشت r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        بازگشت readlink_malloc(sl, ret_machine);
}

صحیح cg_pid_get_machine_name(pid_t pid, نویسه **ret_machine) {
        _cleanup_free_ نویسه *cgroup = پوچ;
        صحیح r;

        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_machine_name(cgroup, ret_machine);
}

صحیح cg_path_get_session(ثابت نویسه *path, نویسه **ret_session) {
        _cleanup_free_ نویسه *unit = پوچ;
        نویسه *start, *end;
        صحیح r;

        أكد(path);

        r = cg_path_get_unit(path, &unit);
        اگر (r < 0)
                بازگشت r;

        start = startswith(unit, "session-");
        اگر (!start)
                بازگشت -ENXIO;
        end = endswith(start, ".scope");
        اگر (!end)
                بازگشت -ENXIO;

        *end = 0;
        اگر (!session_id_valid(start))
                بازگشت -ENXIO;

        اگر (!ret_session)
                بازگشت 0;

        بازگشت strdup_to(ret_session, start);
}

صحیح cg_pid_get_session(pid_t pid, نویسه **ret_session) {
        _cleanup_free_ نویسه *cgroup = پوچ;
        صحیح r;

        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_session(cgroup, ret_session);
}

صحیح cg_pidref_get_session(ثابت PidRef *pidref, نویسه **ret) {
        صحیح r;

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;
        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        _cleanup_free_ نویسه *session = پوچ;
        r = cg_pid_get_session(pidref->pid, &session);
        اگر (r < 0)
                بازگشت r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                بازگشت r;

        اگر (ret)
                *ret = TAKE_PTR(session);
        بازگشت 0;
}

صحیح cg_path_get_owner_uid(ثابت نویسه *path, uid_t *ret_uid) {
        _cleanup_free_ نویسه *slice = پوچ;
        نویسه *start, *end;
        صحیح r;

        أكد(path);

        r = cg_path_get_slice(path, &slice);
        اگر (r < 0)
                بازگشت r;

        start = startswith(slice, "user-");
        اگر (!start)
                بازگشت -ENXIO;

        end = endswith(start, ".slice");
        اگر (!end)
                بازگشت -ENXIO;

        *end = 0;
        اگر (parse_uid(start, ret_uid) < 0)
                بازگشت -ENXIO;

        بازگشت 0;
}

صحیح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ نویسه *cgroup = پوچ;
        صحیح r;

        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_owner_uid(cgroup, ret_uid);
}

صحیح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret) {
        صحیح r;

        اگر (!pidref_is_set(pidref))
                بازگشت -ESRCH;
        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        اگر (r < 0)
                بازگشت r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                بازگشت r;

        اگر (ret)
                *ret = uid;

        بازگشت 0;
}

صحیح cg_path_get_slice(ثابت نویسه *p, نویسه **ret_slice) {
        ثابت نویسه *e = پوچ;

        أكد(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        برای (;;) {
                ثابت نویسه *s;
                صحیح n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                اگر (n < 0)
                        بازگشت n;
                اگر (!valid_slice_name(s, n))
                        بشکن;

                e = s;
        }

        اگر (e)
                بازگشت cg_path_decode_unit(e, ret_slice);

        اگر (ret_slice)
                بازگشت strdup_to(ret_slice, SPECIAL_ریشه_SLICE);

        بازگشت 0;
}

صحیح cg_pid_get_slice(pid_t pid, نویسه **ret_slice) {
        _cleanup_free_ نویسه *cgroup = پوچ;
        صحیح r;

        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_slice(cgroup, ret_slice);
}

صحیح cg_path_get_user_slice(ثابت نویسه *p, نویسه **ret_slice) {
        ثابت نویسه *t;
        أكد(p);

        t = skip_user_prefix(p);
        اگر (!t)
                بازگشت -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        بازگشت cg_path_get_slice(t, ret_slice);
}

صحیح cg_pid_get_user_slice(pid_t pid, نویسه **ret_slice) {
        _cleanup_free_ نویسه *cgroup = پوچ;
        صحیح r;

        r = cg_pid_get_path_shifted(pid, پوچ, &cgroup);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(ثابت نویسه *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        اگر (!filename_is_valid(p))
                بازگشت true;

        اگر (IN_SET(p[0], '_', '.'))
                بازگشت true;

        اگر (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                بازگشت true;

        اگر (startswith(p, "cgroup."))
                بازگشت true;

        برای (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ثابت نویسه *q;

                q = startswith(p, cgroup_controller_to_string(c));
                اگر (!q)
                        ادامه;

                اگر (q[0] == '.')
                        بازگشت true;
        }

        بازگشت false;
}

صحیح cg_escape(ثابت نویسه *p, نویسه **ret) {
        _cleanup_free_ نویسه *n = پوچ;

        أكد(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        اگر (cg_needs_escape(p)) {
                n = strjoin("_", p);
                اگر (!n)
                        بازگشت -ENOMEM;

                اگر (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        بازگشت -EINVAL;
        } وگرنه {
                n = strdup(p);
                اگر (!n)
                        بازگشت -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        بازگشت 0;
}

نویسه* cg_unescape(ثابت نویسه *p) {
        أكد(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        اگر (p[0] == '_')
                بازگشت (نویسه*) p+1;

        بازگشت (نویسه*) p;
}

صحیح cg_slice_to_path(ثابت نویسه *unit, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ, *s = پوچ, *e = پوچ;
        ثابت نویسه *dash;
        صحیح r;

        أكد(unit);
        أكد(ret);

        اگر (streq(unit, SPECIAL_ریشه_SLICE))
                بازگشت strdup_to(ret, "");

        اگر (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                بازگشت -EINVAL;

        اگر (!endswith(unit, ".slice"))
                بازگشت -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        اگر (r < 0)
                بازگشت r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        اگر (dash == p)
                بازگشت -EINVAL;

        وقتی (dash) {
                _cleanup_free_ نویسه *escaped = پوچ;
                نویسه n[dash - p + اندازه(".slice")];

#اگر HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                اگر (IN_SET(dash[1], 0, '-'))
                        بازگشت -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                اگر (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        بازگشت -EINVAL;

                r = cg_escape(n, &escaped);
                اگر (r < 0)
                        بازگشت r;

                اگر (!strextend(&s, escaped, "/"))
                        بازگشت -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        اگر (r < 0)
                بازگشت r;

        اگر (!strextend(&s, e))
                بازگشت -ENOMEM;

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح cg_is_threaded(ثابت نویسه *path) {
        _cleanup_free_ نویسه *fs = پوچ, *contents = پوچ;
        _cleanup_strv_free_ نویسه **v = پوچ;
        صحیح r;

        r = cg_get_path(path, "cgroup.type", &fs);
        اگر (r < 0)
                بازگشت r;

        r = read_full_virtual_file(fs, &contents, پوچ);
        اگر (r == -ENOENT)
                بازگشت false; /* Assume no. */
        اگر (r < 0)
                بازگشت r;

        v = strv_split(contents, پوچ);
        اگر (!v)
                بازگشت -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        بازگشت strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

صحیح cg_set_attribute(ثابت نویسه *path, ثابت نویسه *attribute, ثابت نویسه *مقدار) {
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(attribute);

        r = cg_get_path(path, attribute, &p);
        اگر (r < 0)
                بازگشت r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        بازگشت write_string_file(p, مقدار, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

صحیح cg_get_attribute(ثابت نویسه *path, ثابت نویسه *attribute, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(attribute);

        r = cg_get_path(path, attribute, &p);
        اگر (r < 0)
                بازگشت r;

        بازگشت read_one_سطر_file(p, ret);
}

صحیح cg_get_attribute_as_uint64(ثابت نویسه *path, ثابت نویسه *attribute, uint64_t *ret) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        uint64_t v;
        صحیح r;

        أكد(ret);

        r = cg_get_attribute(path, attribute, &مقدار);
        اگر (r == -ENOENT)
                بازگشت -ENODATA;
        اگر (r < 0)
                بازگشت r;

        اگر (streq(مقدار, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                بازگشت 0;
        }

        r = safe_atou64(مقدار, &v);
        اگر (r < 0)
                بازگشت r;

        *ret = v;
        بازگشت 0;
}

صحیح cg_get_attribute_as_bool(ثابت نویسه *path, ثابت نویسه *attribute) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        صحیح r;

        r = cg_get_attribute(path, attribute, &مقدار);
        اگر (r == -ENOENT)
                بازگشت -ENODATA;
        اگر (r < 0)
                بازگشت r;

        بازگشت parse_boolean(مقدار);
}

صحیح cg_get_owner(ثابت نویسه *path, uid_t *ret_uid) {
        _cleanup_free_ نویسه *f = پوچ;
        ساختار stat stats;
        صحیح r;

        أكد(ret_uid);

        r = cg_get_path(path, /* suffix= */ پوچ, &f);
        اگر (r < 0)
                بازگشت r;

        اگر (stat(f, &stats) < 0)
                بازگشت -errno;

        r = stat_verify_directory(&stats);
        اگر (r < 0)
                بازگشت r;

        *ret_uid = stats.st_uid;
        بازگشت 0;
}

صحیح cg_get_keyed_attribute(
                ثابت نویسه *path,
                ثابت نویسه *attribute,
                نویسه * ثابت *کلیدها,
                نویسه **values) {

        _cleanup_free_ نویسه *filename = پوچ, *contents = پوچ;
        size_t n;
        صحیح r;

        أكد(path);
        أكد(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        اگر (r < 0)
                بازگشت r;

        r = read_full_file(filename, &contents, /* ret_size= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        n = strv_length(کلیدها);
        اگر (n == 0) /* No keys to retrieve? That's easy, we are done then */
                بازگشت 0;
        أكد(strv_is_uniq(کلیدها));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        نویسه **v = newa0(نویسه*, n);
        size_t n_done = 0;

        برای (ثابت نویسه *p = contents; *p;) {
                ثابت نویسه *w;
                size_t i;

                برای (i = 0; i < n; i++) {
                        w = first_word(p, کلیدها[i]);
                        اگر (w)
                                بشکن;
                }

                اگر (w) {
                        اگر (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                برو fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        اگر (!v[i]) {
                                r = -ENOMEM;
                                برو fail;
                        }

                        n_done++;
                        اگر (n_done >= n)
                                بشکن;

                        p = w + l;
                } وگرنه
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        اگر (n_done < n) {
                r = -ENXIO;
                برو fail;
        }

        memcpy(values, v, اندازه(نویسه*) * n);
        بازگشت 0;

fail:
        free_many_charp(v, n);
        بازگشت r;
}

صحیح cg_get_keyed_attribute_uint64(ثابت نویسه *path, ثابت نویسه *attribute, ثابت نویسه *کلید, uint64_t *ret) {
        _cleanup_free_ نویسه *val = پوچ;
        صحیح r;

        أكد(کلید);
        أكد(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(کلید), &val);
        اگر (r < 0)
                بازگشت r;

        r = safe_atou64(val, ret);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, کلید, attribute);

        بازگشت 0;
}

صحیح cg_mask_to_string(CGroupMask mask, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        أكد(ret);

        اگر (mask == 0) {
                *ret = پوچ;
                بازگشت 0;
        }

        برای (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ثابت نویسه *k;
                size_t l;

                اگر (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        ادامه;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                اگر (!GREEDY_REALLOC(s, n + space + l + 1))
                        بازگشت -ENOMEM;

                اگر (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        أكد(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        بازگشت 0;
}

صحیح cg_mask_from_string(ثابت نویسه *s, CGroupMask *ret) {
        CGroupMask m = 0;

        أكد(ret);
        أكد(s);

        برای (;;) {
                _cleanup_free_ نویسه *n = پوچ;
                CGroupController v;
                صحیح r;

                r = extract_first_word(&s, &n, پوچ, 0);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                v = cgroup_controller_from_string(n);
                اگر (v < 0)
                        ادامه;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        بازگشت 0;
}

صحیح cg_mask_supported_subtree(ثابت نویسه *ریشه, CGroupMask *ret) {
        CGroupMask mask;
        صحیح r;

        أكد(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ نویسه *controllers = پوچ, *path = پوچ;
        r = cg_get_path(ریشه, "cgroup.controllers", &path);
        اگر (r < 0)
                بازگشت r;

        r = read_one_سطر_file(path, &controllers);
        اگر (r < 0)
                بازگشت r;

        r = cg_mask_from_string(controllers, &mask);
        اگر (r < 0)
                بازگشت r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        بازگشت 0;
}

صحیح cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ نویسه *ریشه = پوچ;
        صحیح r;

        r = cg_get_ریشه_path(&ریشه);
        اگر (r < 0)
                بازگشت r;

        بازگشت cg_mask_supported_subtree(ریشه, ret);
}

صحیح cg_is_delegated(ثابت نویسه *path) {
        صحیح r;

        أكد(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        اگر (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                بازگشت r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        بازگشت ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

صحیح cg_is_delegated_fd(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ پوچ, "trusted.delegate", /* at_flags= */ 0);
        اگر (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                بازگشت r;

        r = getxattr_at_bool(fd, /* path= */ پوچ, "user.delegate", /* at_flags= */ 0);
        بازگشت ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

صحیح cg_has_coredump_receive(ثابت نویسه *path) {
        صحیح r;

        أكد(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        اگر (ERRNO_IS_NEG_XATTR_ABSENT(r))
                بازگشت false;

        بازگشت r;
}

ثابت uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

ایستا ثابت نویسه* ثابت cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

تهی cgroup_io_limits_list(تهی) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

ایستا ثابت نویسه *ثابت cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

ایستا ثابت نویسه* ثابت managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

ایستا ثابت نویسه* ثابت managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
