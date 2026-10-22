/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/fs.h>
#அந்தர்பாவ <linux/magic.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/xattr.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "capsule-util.h"
#அந்தர்பாவ "cgroup-util.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "login-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "set.h"
#அந்தர்பாவ "special.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "unaligned.h"
#அந்தர்பாவ "unit-name.h"
#அந்தர்பாவ "user-util.h"
#அந்தர்பாவ "xattr-util.h"

முழு cg_is_available(வெறுமை) {
        கட்டமைப்பு statfs fs;

        எனில் (statfs("/sys/fs/cgroup/", &fs) < 0) {
                எனில் (errno == ENOENT) /* sysfs not mounted? */
                        திரும்பு false;

                திரும்பு log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        திரும்பு is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

முழு cg_path_open(மாறிலி எழுத்து *path) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        முழு r;

        r = cg_get_path(path, /* suffix= */ வெற்று, &fs);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

முழு cg_cgroupid_open(முழு cgroupfs_fd, uint64_t id) {
        _cleanup_close_ முழு fsfd = -EBADF;

        எனில் (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                எனில் (fsfd < 0)
                        திரும்பு -errno;

                cgroupfs_fd = fsfd;
        }

        ஒன்றிப்பு {
                கட்டமைப்பு file_handle file_handle;
                uint8_t space[offsetof(கட்டமைப்பு file_handle, f_handle) + அளவு(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = அளவு(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        திரும்பு RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

முழு cg_path_from_cgroupid(முழு cgroupfs_fd, uint64_t id, எழுத்து **ret) {
        _cleanup_close_ முழு cgfd = -EBADF;
        முழு r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        எனில் (cgfd < 0)
                திரும்பு cgfd;

        _cleanup_free_ எழுத்து *path = வெற்று;
        r = fd_get_path(cgfd, &path);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!path_startswith(path, "/sys/fs/cgroup/"))
                திரும்பு -EXDEV; /* recognizable error */

        எனில் (ret)
                *ret = TAKE_PTR(path);
        திரும்பு 0;
}

முழு cg_enumerate_processes(மாறிலி எழுத்து *path, FILE **ret) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        FILE *f;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        எனில் (r < 0)
                திரும்பு r;

        f = fopen(fs, "re");
        எனில் (!f)
                திரும்பு -errno;

        *ret = f;
        திரும்பு 0;
}

முழு cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        க்கு (;;) {
                errno = 0;
                எனில் (fscanf(f, "%lu", &ul) != 1) {

                        எனில் (feof(f)) {
                                *ret = 0;
                                திரும்பு 0;
                        }

                        திரும்பு errno_or_else(EIO);
                }

                எனில் (ul > PID_T_MAX)
                        திரும்பு -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                எனில் (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        தொடர்;

                *ret = (pid_t) ul;
                திரும்பு 1;
        }
}

முழு cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        முழு r;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(ret);

        க்கு (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                எனில் (r == 0) {
                        *ret = PIDREF_NULL;
                        திரும்பு 0;
                }

                எனில் (pid == 0)
                        திரும்பு -EREMOTE;

                r = pidref_set_pid(ret, pid);
                எனில் (r >= 0)
                        திரும்பு 1;
                எனில் (r != -ESRCH)
                        திரும்பு r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(வெறுமை) {
        நிலையான thread_local முழு supported = -1;

        எனில் (supported >= 0)
                திரும்பு supported;

        எனில் (cg_is_available() <= 0)
                திரும்பு (supported = false);

        எனில் (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                திரும்பு (supported = true);
        எனில் (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        திரும்பு (supported = false);
}

முழு cg_enumerate_subgroups(மாறிலி எழுத்து *path, DIR **ret) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        DIR *d;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ வெற்று, &fs);
        எனில் (r < 0)
                திரும்பு r;

        d = opendir(fs);
        எனில் (!d)
                திரும்பு -errno;

        *ret = d;
        திரும்பு 0;
}

முழு cg_read_subgroup(DIR *d, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(d);
        ப்ரதிஜ்ஞா(ret);

        FOREACH_DIRENT_ALL(de, d, திரும்பு -errno) {
                எனில் (de->d_type != DT_DIR)
                        தொடர்;

                எனில் (dot_or_dot_dot(de->d_name))
                        தொடர்;

                திரும்பு strdup_to_full(ret, de->d_name);
        }

        *ret = வெற்று;
        திரும்பு 0;
}

முழு cg_kill(
                மாறிலி எழுத்து *path,
                முழு sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                வெறுமை *userdata) {

        _cleanup_set_free_ Set *allocated_set = வெற்று;
        முழு r, ret = 0;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        எனில் (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        எனில் (!killed_pids) {
                killed_pids = allocated_set = set_new(வெற்று);
                எனில் (!killed_pids)
                        திரும்பு -ENOMEM;
        }

        bool done;
        செய் {
                _cleanup_fclose_ FILE *f = வெற்று;
                முழு ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                எனில் (r == -ENOENT)
                        நிறுத்து;
                எனில் (r < 0)
                        திரும்பு RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                க்கு (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        எனில் (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                நிறுத்து;
                        }
                        எனில் (r < 0)
                                திரும்பு RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        எனில் (r == 0)
                                நிறுத்து;

                        எனில் ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                தொடர்;

                        எனில் (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                தொடர்;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        எனில் (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                தொடர்;
                        }

                        எனில் (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        எனில் (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        எனில் (r >= 0) {
                                எனில் (flags & CGROUP_SIGCONT)
                                        (வெறுமை) pidref_kill(&pidref, SIGCONT);

                                எனில் (ret == 0) {
                                        எனில் (log_kill)
                                                ret = ret_log_kill;
                                        இல்லையெனில்
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        எனில் (r < 0)
                                திரும்பு RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } போது (!done);

        திரும்பு ret;
}

முழு cg_kill_recursive(
                மாறிலி எழுத்து *path,
                முழு sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                வெறுமை *userdata) {

        _cleanup_set_free_ Set *allocated_set = வெற்று;
        _cleanup_closedir_ DIR *d = வெற்று;
        முழு r, ret;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(sig >= 0);

        எனில் (!killed_pids) {
                killed_pids = allocated_set = set_new(வெற்று);
                எனில் (!killed_pids)
                        திரும்பு -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        எனில் (r < 0) {
                எனில் (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                திரும்பு ret;
        }

        க்கு (;;) {
                _cleanup_free_ எழுத்து *fn = வெற்று, *p = வெற்று;

                r = cg_read_subgroup(d, &fn);
                எனில் (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        நிறுத்து;
                }
                எனில் (r == 0)
                        நிறுத்து;

                p = path_join(empty_to_வேர்(path), fn);
                எனில் (!p)
                        திரும்பு -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                எனில் (r != 0 && ret >= 0)
                        ret = r;
        }

        திரும்பு ret;
}

முழு cg_kill_kernel_sigkill(மாறிலி எழுத்து *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ எழுத்து *killfile = வெற்று;
        uint64_t n_pids = UINT64_MAX;
        முழு r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        ப்ரதிஜ்ஞா(path);

        எனில் (!cg_kill_supported())
                திரும்பு -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        எனில் (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        திரும்பு 0;
}

முழு cg_get_path(மாறிலி எழுத்து *path, மாறிலி எழுத்து *suffix, எழுத்து **ret) {
        எழுத்து *t;

        ப்ரதிஜ்ஞா(ret);

        எனில் (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        எனில் (!t)
                திரும்பு -ENOMEM;

        *ret = path_simplify(t);
        திரும்பு 0;
}

முழு cg_set_xattr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, மாறிலி வெறுமை *மதிப்பு, size_t size, முழு flags) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(மதிப்பு || size <= 0);

        r = cg_get_path(path, /* suffix= */ வெற்று, &fs);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு RET_NERRNO(setxattr(fs, name, மதிப்பு, size, flags));
}

முழு cg_get_xattr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, எழுத்து **ret, size_t *ret_size) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(name);

        r = cg_get_path(path, /* suffix= */ வெற்று, &fs);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு lgetxattr_malloc(fs, name, ret, ret_size);
}

முழு cg_get_xattr_bool(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(name);

        r = cg_get_path(path, /* suffix= */ வெற்று, &fs);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

முழு cg_remove_xattr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name) {
        _cleanup_free_ எழுத்து *fs = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(name);

        r = cg_get_path(path, /* suffix= */ வெற்று, &fs);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு RET_NERRNO(removexattr(fs, name));
}

முழு cg_pid_get_path(pid_t pid, எழுத்து **ret_path) {
        _cleanup_fclose_ FILE *f = வெற்று;
        மாறிலி எழுத்து *fs;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        எனில் (r == -ENOENT)
                திரும்பு -ESRCH;
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;
                எழுத்து *e;

                r = read_வரி(f, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு -ENODATA;

                e = startswith(வரி, "0:");
                எனில் (!e)
                        தொடர்;

                e = strchr(e, ':');
                எனில் (!e)
                        தொடர்;

                _cleanup_free_ எழுத்து *path = strdup(e + 1);
                எனில் (!path)
                        திரும்பு -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                எனில் (startswith(path, "/../"))
                        திரும்பு -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                எனில் (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                திரும்பு 0;
        }
}

முழு cg_pidref_get_path(மாறிலி PidRef *pidref, எழுத்து **ret_path) {
        _cleanup_free_ எழுத்து *path = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret_path);

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        எனில் (r < 0)
                திரும்பு r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        *ret_path = TAKE_PTR(path);
        திரும்பு 0;
}

முழு cg_is_empty(மாறிலி எழுத்து *path) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        ப்ரதிஜ்ஞா(path);

        /* The root cgroup is always populated */
        எனில் (empty_or_வேர்(path))
                திரும்பு false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        எனில் (r == -ENOENT)
                திரும்பு true;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு streq(t, "0");
}

முழு cg_split_spec(மாறிலி எழுத்து *spec, எழுத்து **ret_controller, எழுத்து **ret_path) {
        _cleanup_free_ எழுத்து *controller = வெற்று;
        மாறிலி எழுத்து *path;
        முழு r;

        ப்ரதிஜ்ஞா(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        எனில் (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                செல் finalize;
        }

        மாறிலி எழுத்து *e = strchr(spec, ':');
        எனில் (!e) {
                /* Controller only. */
                எனில் (ret_controller) {
                        controller = strdup(spec);
                        எனில் (!controller)
                                திரும்பு -ENOMEM;
                }

                path = வெற்று;
        } இல்லையெனில் {
                /* Both controller and path. */
                எனில் (ret_controller) {
                        controller = strndup(spec, e - spec);
                        எனில் (!controller)
                                திரும்பு -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        எனில் (path) {
                /* Non-empty path must be absolute. */
                எனில் (!path_is_absolute(path))
                        திரும்பு -EINVAL;

                /* Path must not contain dot-dot. */
                எனில் (!path_is_safe(path))
                        திரும்பு -EINVAL;
        }

        எனில் (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        திரும்பு 0;
}

முழு cg_get_வேர்_path(எழுத்து **ret_path) {
        எழுத்து *p, *e;
        முழு r;

        ப்ரதிஜ்ஞா(ret_path);

        r = cg_pid_get_path(1, &p);
        எனில் (r < 0)
                திரும்பு r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        எனில் (e)
                *e = 0;

        *ret_path = p;
        திரும்பு 0;
}

முழு cg_shift_path(மாறிலி எழுத்து *cgroup, மாறிலி எழுத்து *வேர், மாறிலி எழுத்து **ret_shifted) {
        முழு r;

        ப்ரதிஜ்ஞா(cgroup);
        ப்ரதிஜ்ஞா(ret_shifted);

        _cleanup_free_ எழுத்து *rt = வெற்று;
        எனில் (!வேர்) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_வேர்_path(&rt);
                எனில் (r < 0)
                        திரும்பு r;

                வேர் = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, வேர், PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        திரும்பு 0;
}

முழு cg_pid_get_path_shifted(pid_t pid, மாறிலி எழுத்து *வேர், எழுத்து **ret_cgroup) {
        _cleanup_free_ எழுத்து *raw = வெற்று;
        மாறிலி எழுத்து *c;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        எனில் (r < 0)
                திரும்பு r;

        r = cg_shift_path(raw, வேர், &c);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                திரும்பு 0;
        }

        திரும்பு strdup_to(ret_cgroup, c);
}

முழு cg_pidref_get_path_shifted(மாறிலி PidRef *pidref, மாறிலி எழுத்து *வேர், எழுத்து **ret_cgroup) {
        _cleanup_free_ எழுத்து *raw = வெற்று;
        மாறிலி எழுத்து *c;
        முழு r;

        ப்ரதிஜ்ஞா(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        எனில் (r < 0)
                திரும்பு r;

        r = cg_shift_path(raw, வேர், &c);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                திரும்பு 0;
        }

        திரும்பு strdup_to(ret_cgroup, c);
}

முழு cg_path_decode_unit(மாறிலி எழுத்து *cgroup, எழுத்து **ret_unit) {
        ப்ரதிஜ்ஞா(cgroup);

        size_t n = strcspn(cgroup, "/");
        எனில் (n < 3)
                திரும்பு -ENXIO;

        எழுத்து *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        எனில் (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                திரும்பு -ENXIO;

        எனில் (ret_unit)
                திரும்பு strdup_to(ret_unit, c);

        திரும்பு 0;
}

நிலையான bool valid_slice_name(மாறிலி எழுத்து *p, size_t n) {
        ப்ரதிஜ்ஞா(p || n == 0);

        எனில் (n < STRLEN("x.slice"))
                திரும்பு false;

        எழுத்து *c = strndupa_safe(p, n);
        எனில் (!endswith(c, ".slice"))
                திரும்பு false;

        திரும்பு unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

நிலையான மாறிலி எழுத்து* skip_slices(மாறிலி எழுத்து *p) {
        ப்ரதிஜ்ஞா(p);

        /* Skips over all slice assignments */

        க்கு (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                எனில் (!valid_slice_name(p, n))
                        திரும்பு p;

                p += n;
        }
}

முழு cg_path_get_unit_full(மாறிலி எழுத்து *path, எழுத்து **ret_unit, எழுத்து **ret_subgroup) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        மாறிலி எழுத்து *e = skip_slices(path);

        _cleanup_free_ எழுத்து *unit = வெற்று;
        r = cg_path_decode_unit(e, &unit);
        எனில் (r < 0)
                திரும்பு r;

        /* We skipped over the slices, don't accept any now */
        எனில் (endswith(unit, ".slice"))
                திரும்பு -ENXIO;

        எனில் (ret_subgroup) {
                _cleanup_free_ எழுத்து *subgroup = வெற்று;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                எனில் (isempty(e))
                        subgroup = வெற்று;
                இல்லையெனில் {
                        subgroup = strdup(e);
                        எனில் (!subgroup)
                                திரும்பு -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        எனில் (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        திரும்பு 0;
}

முழு cg_path_get_unit_path(மாறிலி எழுத்து *path, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *path_copy = வெற்று;
        எழுத்து *unit_name;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        path_copy = strdup(path);
        எனில் (!path_copy)
                திரும்பு -ENOMEM;

        unit_name = (எழுத்து*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        எனில் (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                திரும்பு -ENXIO;

        *ret = TAKE_PTR(path_copy);

        திரும்பு 0;
}

முழு cg_pid_get_unit_full(pid_t pid, எழுத்து **ret_unit, எழுத்து **ret_subgroup) {
        முழு r;

        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

முழு cg_pidref_get_unit_full(மாறிலி PidRef *pidref, எழுத்து **ret_unit, எழுத்து **ret_subgroup) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        _cleanup_free_ எழுத்து *unit = வெற்று, *subgroup = வெற்று;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        எனில் (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        திரும்பு 0;
}

நிலையான மாறிலி எழுத்து* skip_session(மாறிலி எழுத்து *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        எனில் (isempty(p))
                திரும்பு வெற்று;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        எனில் (n < STRLEN("session-x.scope"))
                திரும்பு வெற்று;

        மாறிலி எழுத்து *s = startswith(p, "session-");
        எனில் (!s)
                திரும்பு வெற்று;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        எழுத்து *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        எனில் (!e)
                திரும்பு வெற்று;
        *e = '\0';

        எனில் (!session_id_valid(f))
                திரும்பு வெற்று;

        திரும்பு skip_leading_slash(p + n);
}

நிலையான மாறிலி எழுத்து* skip_user_manager(மாறிலி எழுத்து *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        எனில் (isempty(p))
                திரும்பு வெற்று;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        எனில் (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                திரும்பு வெற்று;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ எழுத்து *unit_name = strndup(p, n);
        எனில் (!unit_name)
                திரும்பு வெற்று;

        _cleanup_free_ எழுத்து *i = வெற்று;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        எனில் (type != UNIT_NAME_INSTANCE)
                திரும்பு வெற்று;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        எனில் (!(startswith(unit_name, "user@") && parse_uid(i, வெற்று) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                திரும்பு வெற்று;

        திரும்பு skip_leading_slash(p + n);
}

நிலையான மாறிலி எழுத்து* skip_user_prefix(மாறிலி எழுத்து *path) {
        மாறிலி எழுத்து *e, *t;

        ப்ரதிஜ்ஞா(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        எனில் (t)
                திரும்பு t;

        /* Alternatively skip the user session if it is in the path... */
        திரும்பு skip_session(e);
}

முழு cg_path_get_user_unit_full(மாறிலி எழுத்து *path, எழுத்து **ret_unit, எழுத்து **ret_subgroup) {
        மாறிலி எழுத்து *t;

        ப்ரதிஜ்ஞா(path);

        t = skip_user_prefix(path);
        எனில் (!t)
                திரும்பு -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        திரும்பு cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

முழு cg_pid_get_user_unit_full(pid_t pid, எழுத்து **ret_unit, எழுத்து **ret_subgroup) {
        முழு r;

        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

முழு cg_pidref_get_user_unit_full(மாறிலி PidRef *pidref, எழுத்து **ret_unit, எழுத்து **ret_subgroup) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        _cleanup_free_ எழுத்து *unit = வெற்று, *subgroup = வெற்று;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        எனில் (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        திரும்பு 0;
}

முழு cg_path_get_machine_name(மாறிலி எழுத்து *path, எழுத்து **ret_machine) {
        _cleanup_free_ எழுத்து *u = வெற்று;
        மாறிலி எழுத்து *sl;
        முழு r;

        r = cg_path_get_unit(path, &u);
        எனில் (r < 0)
                திரும்பு r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        திரும்பு readlink_malloc(sl, ret_machine);
}

முழு cg_pid_get_machine_name(pid_t pid, எழுத்து **ret_machine) {
        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        முழு r;

        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_machine_name(cgroup, ret_machine);
}

முழு cg_path_get_session(மாறிலி எழுத்து *path, எழுத்து **ret_session) {
        _cleanup_free_ எழுத்து *unit = வெற்று;
        எழுத்து *start, *end;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = cg_path_get_unit(path, &unit);
        எனில் (r < 0)
                திரும்பு r;

        start = startswith(unit, "session-");
        எனில் (!start)
                திரும்பு -ENXIO;
        end = endswith(start, ".scope");
        எனில் (!end)
                திரும்பு -ENXIO;

        *end = 0;
        எனில் (!session_id_valid(start))
                திரும்பு -ENXIO;

        எனில் (!ret_session)
                திரும்பு 0;

        திரும்பு strdup_to(ret_session, start);
}

முழு cg_pid_get_session(pid_t pid, எழுத்து **ret_session) {
        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        முழு r;

        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_session(cgroup, ret_session);
}

முழு cg_pidref_get_session(மாறிலி PidRef *pidref, எழுத்து **ret) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        _cleanup_free_ எழுத்து *session = வெற்று;
        r = cg_pid_get_session(pidref->pid, &session);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = TAKE_PTR(session);
        திரும்பு 0;
}

முழு cg_path_get_owner_uid(மாறிலி எழுத்து *path, uid_t *ret_uid) {
        _cleanup_free_ எழுத்து *slice = வெற்று;
        எழுத்து *start, *end;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = cg_path_get_slice(path, &slice);
        எனில் (r < 0)
                திரும்பு r;

        start = startswith(slice, "user-");
        எனில் (!start)
                திரும்பு -ENXIO;

        end = endswith(start, ".slice");
        எனில் (!end)
                திரும்பு -ENXIO;

        *end = 0;
        எனில் (parse_uid(start, ret_uid) < 0)
                திரும்பு -ENXIO;

        திரும்பு 0;
}

முழு cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        முழு r;

        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_owner_uid(cgroup, ret_uid);
}

முழு cg_pidref_get_owner_uid(மாறிலி PidRef *pidref, uid_t *ret) {
        முழு r;

        எனில் (!pidref_is_set(pidref))
                திரும்பு -ESRCH;
        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        எனில் (r < 0)
                திரும்பு r;

        r = pidref_verify(pidref);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret)
                *ret = uid;

        திரும்பு 0;
}

முழு cg_path_get_slice(மாறிலி எழுத்து *p, எழுத்து **ret_slice) {
        மாறிலி எழுத்து *e = வெற்று;

        ப்ரதிஜ்ஞா(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        க்கு (;;) {
                மாறிலி எழுத்து *s;
                முழு n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                எனில் (n < 0)
                        திரும்பு n;
                எனில் (!valid_slice_name(s, n))
                        நிறுத்து;

                e = s;
        }

        எனில் (e)
                திரும்பு cg_path_decode_unit(e, ret_slice);

        எனில் (ret_slice)
                திரும்பு strdup_to(ret_slice, SPECIAL_வேர்_SLICE);

        திரும்பு 0;
}

முழு cg_pid_get_slice(pid_t pid, எழுத்து **ret_slice) {
        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        முழு r;

        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_slice(cgroup, ret_slice);
}

முழு cg_path_get_user_slice(மாறிலி எழுத்து *p, எழுத்து **ret_slice) {
        மாறிலி எழுத்து *t;
        ப்ரதிஜ்ஞா(p);

        t = skip_user_prefix(p);
        எனில் (!t)
                திரும்பு -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        திரும்பு cg_path_get_slice(t, ret_slice);
}

முழு cg_pid_get_user_slice(pid_t pid, எழுத்து **ret_slice) {
        _cleanup_free_ எழுத்து *cgroup = வெற்று;
        முழு r;

        r = cg_pid_get_path_shifted(pid, வெற்று, &cgroup);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(மாறிலி எழுத்து *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        எனில் (!filename_is_valid(p))
                திரும்பு true;

        எனில் (IN_SET(p[0], '_', '.'))
                திரும்பு true;

        எனில் (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                திரும்பு true;

        எனில் (startswith(p, "cgroup."))
                திரும்பு true;

        க்கு (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                மாறிலி எழுத்து *q;

                q = startswith(p, cgroup_controller_to_string(c));
                எனில் (!q)
                        தொடர்;

                எனில் (q[0] == '.')
                        திரும்பு true;
        }

        திரும்பு false;
}

முழு cg_escape(மாறிலி எழுத்து *p, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *n = வெற்று;

        ப்ரதிஜ்ஞா(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        எனில் (cg_needs_escape(p)) {
                n = strjoin("_", p);
                எனில் (!n)
                        திரும்பு -ENOMEM;

                எனில் (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        திரும்பு -EINVAL;
        } இல்லையெனில் {
                n = strdup(p);
                எனில் (!n)
                        திரும்பு -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        திரும்பு 0;
}

எழுத்து* cg_unescape(மாறிலி எழுத்து *p) {
        ப்ரதிஜ்ஞா(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        எனில் (p[0] == '_')
                திரும்பு (எழுத்து*) p+1;

        திரும்பு (எழுத்து*) p;
}

முழு cg_slice_to_path(மாறிலி எழுத்து *unit, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று, *s = வெற்று, *e = வெற்று;
        மாறிலி எழுத்து *dash;
        முழு r;

        ப்ரதிஜ்ஞா(unit);
        ப்ரதிஜ்ஞா(ret);

        எனில் (streq(unit, SPECIAL_வேர்_SLICE))
                திரும்பு strdup_to(ret, "");

        எனில் (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                திரும்பு -EINVAL;

        எனில் (!endswith(unit, ".slice"))
                திரும்பு -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        எனில் (r < 0)
                திரும்பு r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        எனில் (dash == p)
                திரும்பு -EINVAL;

        போது (dash) {
                _cleanup_free_ எழுத்து *escaped = வெற்று;
                எழுத்து n[dash - p + அளவு(".slice")];

#எனில் HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                எனில் (IN_SET(dash[1], 0, '-'))
                        திரும்பு -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                எனில் (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        திரும்பு -EINVAL;

                r = cg_escape(n, &escaped);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (!strextend(&s, escaped, "/"))
                        திரும்பு -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!strextend(&s, e))
                திரும்பு -ENOMEM;

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு cg_is_threaded(மாறிலி எழுத்து *path) {
        _cleanup_free_ எழுத்து *fs = வெற்று, *contents = வெற்று;
        _cleanup_strv_free_ எழுத்து **v = வெற்று;
        முழு r;

        r = cg_get_path(path, "cgroup.type", &fs);
        எனில் (r < 0)
                திரும்பு r;

        r = read_full_virtual_file(fs, &contents, வெற்று);
        எனில் (r == -ENOENT)
                திரும்பு false; /* Assume no. */
        எனில் (r < 0)
                திரும்பு r;

        v = strv_split(contents, வெற்று);
        எனில் (!v)
                திரும்பு -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        திரும்பு strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

முழு cg_set_attribute(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, மாறிலி எழுத்து *மதிப்பு) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(attribute);

        r = cg_get_path(path, attribute, &p);
        எனில் (r < 0)
                திரும்பு r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        திரும்பு write_string_file(p, மதிப்பு, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

முழு cg_get_attribute(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(attribute);

        r = cg_get_path(path, attribute, &p);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு read_one_வரி_file(p, ret);
}

முழு cg_get_attribute_as_uint64(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, uint64_t *ret) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        uint64_t v;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = cg_get_attribute(path, attribute, &மதிப்பு);
        எனில் (r == -ENOENT)
                திரும்பு -ENODATA;
        எனில் (r < 0)
                திரும்பு r;

        எனில் (streq(மதிப்பு, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                திரும்பு 0;
        }

        r = safe_atou64(மதிப்பு, &v);
        எனில் (r < 0)
                திரும்பு r;

        *ret = v;
        திரும்பு 0;
}

முழு cg_get_attribute_as_bool(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        முழு r;

        r = cg_get_attribute(path, attribute, &மதிப்பு);
        எனில் (r == -ENOENT)
                திரும்பு -ENODATA;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_boolean(மதிப்பு);
}

முழு cg_get_owner(மாறிலி எழுத்து *path, uid_t *ret_uid) {
        _cleanup_free_ எழுத்து *f = வெற்று;
        கட்டமைப்பு stat stats;
        முழு r;

        ப்ரதிஜ்ஞா(ret_uid);

        r = cg_get_path(path, /* suffix= */ வெற்று, &f);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (stat(f, &stats) < 0)
                திரும்பு -errno;

        r = stat_verify_directory(&stats);
        எனில் (r < 0)
                திரும்பு r;

        *ret_uid = stats.st_uid;
        திரும்பு 0;
}

முழு cg_get_keyed_attribute(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *attribute,
                எழுத்து * மாறிலி *சாவிகள்,
                எழுத்து **values) {

        _cleanup_free_ எழுத்து *filename = வெற்று, *contents = வெற்று;
        size_t n;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        எனில் (r < 0)
                திரும்பு r;

        r = read_full_file(filename, &contents, /* ret_size= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        n = strv_length(சாவிகள்);
        எனில் (n == 0) /* No keys to retrieve? That's easy, we are done then */
                திரும்பு 0;
        ப்ரதிஜ்ஞா(strv_is_uniq(சாவிகள்));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        எழுத்து **v = newa0(எழுத்து*, n);
        size_t n_done = 0;

        க்கு (மாறிலி எழுத்து *p = contents; *p;) {
                மாறிலி எழுத்து *w;
                size_t i;

                க்கு (i = 0; i < n; i++) {
                        w = first_word(p, சாவிகள்[i]);
                        எனில் (w)
                                நிறுத்து;
                }

                எனில் (w) {
                        எனில் (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                செல் fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        எனில் (!v[i]) {
                                r = -ENOMEM;
                                செல் fail;
                        }

                        n_done++;
                        எனில் (n_done >= n)
                                நிறுத்து;

                        p = w + l;
                } இல்லையெனில்
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        எனில் (n_done < n) {
                r = -ENXIO;
                செல் fail;
        }

        memcpy(values, v, அளவு(எழுத்து*) * n);
        திரும்பு 0;

fail:
        free_many_charp(v, n);
        திரும்பு r;
}

முழு cg_get_keyed_attribute_uint64(மாறிலி எழுத்து *path, மாறிலி எழுத்து *attribute, மாறிலி எழுத்து *சாவி, uint64_t *ret) {
        _cleanup_free_ எழுத்து *val = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(சாவி);
        ப்ரதிஜ்ஞா(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(சாவி), &val);
        எனில் (r < 0)
                திரும்பு r;

        r = safe_atou64(val, ret);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, சாவி, attribute);

        திரும்பு 0;
}

முழு cg_mask_to_string(CGroupMask mask, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        ப்ரதிஜ்ஞா(ret);

        எனில் (mask == 0) {
                *ret = வெற்று;
                திரும்பு 0;
        }

        க்கு (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                மாறிலி எழுத்து *k;
                size_t l;

                எனில் (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        தொடர்;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                எனில் (!GREEDY_REALLOC(s, n + space + l + 1))
                        திரும்பு -ENOMEM;

                எனில் (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        ப்ரதிஜ்ஞா(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        திரும்பு 0;
}

முழு cg_mask_from_string(மாறிலி எழுத்து *s, CGroupMask *ret) {
        CGroupMask m = 0;

        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(s);

        க்கு (;;) {
                _cleanup_free_ எழுத்து *n = வெற்று;
                CGroupController v;
                முழு r;

                r = extract_first_word(&s, &n, வெற்று, 0);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                v = cgroup_controller_from_string(n);
                எனில் (v < 0)
                        தொடர்;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        திரும்பு 0;
}

முழு cg_mask_supported_subtree(மாறிலி எழுத்து *வேர், CGroupMask *ret) {
        CGroupMask mask;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ எழுத்து *controllers = வெற்று, *path = வெற்று;
        r = cg_get_path(வேர், "cgroup.controllers", &path);
        எனில் (r < 0)
                திரும்பு r;

        r = read_one_வரி_file(path, &controllers);
        எனில் (r < 0)
                திரும்பு r;

        r = cg_mask_from_string(controllers, &mask);
        எனில் (r < 0)
                திரும்பு r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        திரும்பு 0;
}

முழு cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ எழுத்து *வேர் = வெற்று;
        முழு r;

        r = cg_get_வேர்_path(&வேர்);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cg_mask_supported_subtree(வேர், ret);
}

முழு cg_is_delegated(மாறிலி எழுத்து *path) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        எனில் (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                திரும்பு r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        திரும்பு ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

முழு cg_is_delegated_fd(முழு fd) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ வெற்று, "trusted.delegate", /* at_flags= */ 0);
        எனில் (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                திரும்பு r;

        r = getxattr_at_bool(fd, /* path= */ வெற்று, "user.delegate", /* at_flags= */ 0);
        திரும்பு ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

முழு cg_has_coredump_receive(மாறிலி எழுத்து *path) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        எனில் (ERRNO_IS_NEG_XATTR_ABSENT(r))
                திரும்பு false;

        திரும்பு r;
}

மாறிலி uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

நிலையான மாறிலி எழுத்து* மாறிலி cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

வெறுமை cgroup_io_limits_list(வெறுமை) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

நிலையான மாறிலி எழுத்து *மாறிலி cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

நிலையான மாறிலி எழுத்து* மாறிலி managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

நிலையான மாறிலி எழுத்து* மாறிலி managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
