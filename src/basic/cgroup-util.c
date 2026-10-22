/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/fs.h>
#અન્તર્ભાવ <linux/magic.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/xattr.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "capsule-util.h"
#અન્તર્ભાવ "cgroup-util.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "login-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "set.h"
#અન્તર્ભાવ "special.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "unaligned.h"
#અન્તર્ભાવ "unit-name.h"
#અન્તર્ભાવ "user-util.h"
#અન્તર્ભાવ "xattr-util.h"

પૂર્ણાંક cg_is_available(શૂન્ય) {
        સંરચના statfs fs;

        જો (statfs("/sys/fs/cgroup/", &fs) < 0) {
                જો (errno == ENOENT) /* sysfs not mounted? */
                        પાછા false;

                પાછા log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        પાછા is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

પૂર્ણાંક cg_path_open(અચલ અક્ષર *path) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        પૂર્ણાંક r;

        r = cg_get_path(path, /* suffix= */ ખાલી, &fs);
        જો (r < 0)
                પાછા r;

        પાછા RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

પૂર્ણાંક cg_cgroupid_open(પૂર્ણાંક cgroupfs_fd, uint64_t id) {
        _cleanup_close_ પૂર્ણાંક fsfd = -EBADF;

        જો (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                જો (fsfd < 0)
                        પાછા -errno;

                cgroupfs_fd = fsfd;
        }

        સંઘ {
                સંરચના file_handle file_handle;
                uint8_t space[offsetof(સંરચના file_handle, f_handle) + કદ(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = કદ(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        પાછા RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

પૂર્ણાંક cg_path_from_cgroupid(પૂર્ણાંક cgroupfs_fd, uint64_t id, અક્ષર **ret) {
        _cleanup_close_ પૂર્ણાંક cgfd = -EBADF;
        પૂર્ણાંક r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        જો (cgfd < 0)
                પાછા cgfd;

        _cleanup_free_ અક્ષર *path = ખાલી;
        r = fd_get_path(cgfd, &path);
        જો (r < 0)
                પાછા r;

        જો (!path_startswith(path, "/sys/fs/cgroup/"))
                પાછા -EXDEV; /* recognizable error */

        જો (ret)
                *ret = TAKE_PTR(path);
        પાછા 0;
}

પૂર્ણાંક cg_enumerate_processes(અચલ અક્ષર *path, FILE **ret) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        FILE *f;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        જો (r < 0)
                પાછા r;

        f = fopen(fs, "re");
        જો (!f)
                પાછા -errno;

        *ret = f;
        પાછા 0;
}

પૂર્ણાંક cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        માટે (;;) {
                errno = 0;
                જો (fscanf(f, "%lu", &ul) != 1) {

                        જો (feof(f)) {
                                *ret = 0;
                                પાછા 0;
                        }

                        પાછા errno_or_else(EIO);
                }

                જો (ul > PID_T_MAX)
                        પાછા -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                જો (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        ચાલુ;

                *ret = (pid_t) ul;
                પાછા 1;
        }
}

પૂર્ણાંક cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(ret);

        માટે (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                જો (r < 0)
                        પાછા log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                જો (r == 0) {
                        *ret = PIDREF_NULL;
                        પાછા 0;
                }

                જો (pid == 0)
                        પાછા -EREMOTE;

                r = pidref_set_pid(ret, pid);
                જો (r >= 0)
                        પાછા 1;
                જો (r != -ESRCH)
                        પાછા r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(શૂન્ય) {
        સ્થિર thread_local પૂર્ણાંક supported = -1;

        જો (supported >= 0)
                પાછા supported;

        જો (cg_is_available() <= 0)
                પાછા (supported = false);

        જો (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                પાછા (supported = true);
        જો (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        પાછા (supported = false);
}

પૂર્ણાંક cg_enumerate_subgroups(અચલ અક્ષર *path, DIR **ret) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        DIR *d;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ ખાલી, &fs);
        જો (r < 0)
                પાછા r;

        d = opendir(fs);
        જો (!d)
                પાછા -errno;

        *ret = d;
        પાછા 0;
}

પૂર્ણાંક cg_read_subgroup(DIR *d, અક્ષર **ret) {
        પ્રતિજ્ઞા(d);
        પ્રતિજ્ઞા(ret);

        FOREACH_DIRENT_ALL(de, d, પાછા -errno) {
                જો (de->d_type != DT_DIR)
                        ચાલુ;

                જો (dot_or_dot_dot(de->d_name))
                        ચાલુ;

                પાછા strdup_to_full(ret, de->d_name);
        }

        *ret = ખાલી;
        પાછા 0;
}

પૂર્ણાંક cg_kill(
                અચલ અક્ષર *path,
                પૂર્ણાંક sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                શૂન્ય *userdata) {

        _cleanup_set_free_ Set *allocated_set = ખાલી;
        પૂર્ણાંક r, ret = 0;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        જો (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        જો (!killed_pids) {
                killed_pids = allocated_set = set_new(ખાલી);
                જો (!killed_pids)
                        પાછા -ENOMEM;
        }

        bool done;
        કરો {
                _cleanup_fclose_ FILE *f = ખાલી;
                પૂર્ણાંક ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                જો (r == -ENOENT)
                        તોડો;
                જો (r < 0)
                        પાછા RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                માટે (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        જો (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                તોડો;
                        }
                        જો (r < 0)
                                પાછા RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        જો (r == 0)
                                તોડો;

                        જો ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                ચાલુ;

                        જો (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                ચાલુ;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        જો (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                ચાલુ;
                        }

                        જો (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        જો (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        જો (r >= 0) {
                                જો (flags & CGROUP_SIGCONT)
                                        (શૂન્ય) pidref_kill(&pidref, SIGCONT);

                                જો (ret == 0) {
                                        જો (log_kill)
                                                ret = ret_log_kill;
                                        નહીંતર
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        જો (r < 0)
                                પાછા RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } જ્યારે (!done);

        પાછા ret;
}

પૂર્ણાંક cg_kill_recursive(
                અચલ અક્ષર *path,
                પૂર્ણાંક sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                શૂન્ય *userdata) {

        _cleanup_set_free_ Set *allocated_set = ખાલી;
        _cleanup_closedir_ DIR *d = ખાલી;
        પૂર્ણાંક r, ret;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(sig >= 0);

        જો (!killed_pids) {
                killed_pids = allocated_set = set_new(ખાલી);
                જો (!killed_pids)
                        પાછા -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        જો (r < 0) {
                જો (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                પાછા ret;
        }

        માટે (;;) {
                _cleanup_free_ અક્ષર *fn = ખાલી, *p = ખાલી;

                r = cg_read_subgroup(d, &fn);
                જો (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        તોડો;
                }
                જો (r == 0)
                        તોડો;

                p = path_join(empty_to_મૂળ(path), fn);
                જો (!p)
                        પાછા -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                જો (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                જો (r != 0 && ret >= 0)
                        ret = r;
        }

        પાછા ret;
}

પૂર્ણાંક cg_kill_kernel_sigkill(અચલ અક્ષર *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ અક્ષર *killfile = ખાલી;
        uint64_t n_pids = UINT64_MAX;
        પૂર્ણાંક r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        પ્રતિજ્ઞા(path);

        જો (!cg_kill_supported())
                પાછા -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        જો (r < 0)
                પાછા r;

        જો (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                જો (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        જો (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        પાછા 0;
}

પૂર્ણાંક cg_get_path(અચલ અક્ષર *path, અચલ અક્ષર *suffix, અક્ષર **ret) {
        અક્ષર *t;

        પ્રતિજ્ઞા(ret);

        જો (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        જો (!t)
                પાછા -ENOMEM;

        *ret = path_simplify(t);
        પાછા 0;
}

પૂર્ણાંક cg_set_xattr(અચલ અક્ષર *path, અચલ અક્ષર *name, અચલ શૂન્ય *કિંમત, size_t size, પૂર્ણાંક flags) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(કિંમત || size <= 0);

        r = cg_get_path(path, /* suffix= */ ખાલી, &fs);
        જો (r < 0)
                પાછા r;

        પાછા RET_NERRNO(setxattr(fs, name, કિંમત, size, flags));
}

પૂર્ણાંક cg_get_xattr(અચલ અક્ષર *path, અચલ અક્ષર *name, અક્ષર **ret, size_t *ret_size) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(name);

        r = cg_get_path(path, /* suffix= */ ખાલી, &fs);
        જો (r < 0)
                પાછા r;

        પાછા lgetxattr_malloc(fs, name, ret, ret_size);
}

પૂર્ણાંક cg_get_xattr_bool(અચલ અક્ષર *path, અચલ અક્ષર *name) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(name);

        r = cg_get_path(path, /* suffix= */ ખાલી, &fs);
        જો (r < 0)
                પાછા r;

        પાછા getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

પૂર્ણાંક cg_remove_xattr(અચલ અક્ષર *path, અચલ અક્ષર *name) {
        _cleanup_free_ અક્ષર *fs = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(name);

        r = cg_get_path(path, /* suffix= */ ખાલી, &fs);
        જો (r < 0)
                પાછા r;

        પાછા RET_NERRNO(removexattr(fs, name));
}

પૂર્ણાંક cg_pid_get_path(pid_t pid, અક્ષર **ret_path) {
        _cleanup_fclose_ FILE *f = ખાલી;
        અચલ અક્ષર *fs;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        જો (r == -ENOENT)
                પાછા -ESRCH;
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;
                અક્ષર *e;

                r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા -ENODATA;

                e = startswith(લીટી, "0:");
                જો (!e)
                        ચાલુ;

                e = strchr(e, ':');
                જો (!e)
                        ચાલુ;

                _cleanup_free_ અક્ષર *path = strdup(e + 1);
                જો (!path)
                        પાછા -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                જો (startswith(path, "/../"))
                        પાછા -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                જો (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                પાછા 0;
        }
}

પૂર્ણાંક cg_pidref_get_path(અચલ PidRef *pidref, અક્ષર **ret_path) {
        _cleanup_free_ અક્ષર *path = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_path);

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        જો (r < 0)
                પાછા r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        *ret_path = TAKE_PTR(path);
        પાછા 0;
}

પૂર્ણાંક cg_is_empty(અચલ અક્ષર *path) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        પૂર્ણાંક r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        પ્રતિજ્ઞા(path);

        /* The root cgroup is always populated */
        જો (empty_or_મૂળ(path))
                પાછા false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        જો (r == -ENOENT)
                પાછા true;
        જો (r < 0)
                પાછા r;

        પાછા streq(t, "0");
}

પૂર્ણાંક cg_split_spec(અચલ અક્ષર *spec, અક્ષર **ret_controller, અક્ષર **ret_path) {
        _cleanup_free_ અક્ષર *controller = ખાલી;
        અચલ અક્ષર *path;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        જો (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                જાઓ finalize;
        }

        અચલ અક્ષર *e = strchr(spec, ':');
        જો (!e) {
                /* Controller only. */
                જો (ret_controller) {
                        controller = strdup(spec);
                        જો (!controller)
                                પાછા -ENOMEM;
                }

                path = ખાલી;
        } નહીંતર {
                /* Both controller and path. */
                જો (ret_controller) {
                        controller = strndup(spec, e - spec);
                        જો (!controller)
                                પાછા -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        જો (path) {
                /* Non-empty path must be absolute. */
                જો (!path_is_absolute(path))
                        પાછા -EINVAL;

                /* Path must not contain dot-dot. */
                જો (!path_is_safe(path))
                        પાછા -EINVAL;
        }

        જો (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        પાછા 0;
}

પૂર્ણાંક cg_get_મૂળ_path(અક્ષર **ret_path) {
        અક્ષર *p, *e;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_path);

        r = cg_pid_get_path(1, &p);
        જો (r < 0)
                પાછા r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        જો (e)
                *e = 0;

        *ret_path = p;
        પાછા 0;
}

પૂર્ણાંક cg_shift_path(અચલ અક્ષર *cgroup, અચલ અક્ષર *મૂળ, અચલ અક્ષર **ret_shifted) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(cgroup);
        પ્રતિજ્ઞા(ret_shifted);

        _cleanup_free_ અક્ષર *rt = ખાલી;
        જો (!મૂળ) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_મૂળ_path(&rt);
                જો (r < 0)
                        પાછા r;

                મૂળ = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, મૂળ, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        પાછા 0;
}

પૂર્ણાંક cg_pid_get_path_shifted(pid_t pid, અચલ અક્ષર *મૂળ, અક્ષર **ret_cgroup) {
        _cleanup_free_ અક્ષર *raw = ખાલી;
        અચલ અક્ષર *c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        જો (r < 0)
                પાછા r;

        r = cg_shift_path(raw, મૂળ, &c);
        જો (r < 0)
                પાછા r;

        જો (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                પાછા 0;
        }

        પાછા strdup_to(ret_cgroup, c);
}

પૂર્ણાંક cg_pidref_get_path_shifted(અચલ PidRef *pidref, અચલ અક્ષર *મૂળ, અક્ષર **ret_cgroup) {
        _cleanup_free_ અક્ષર *raw = ખાલી;
        અચલ અક્ષર *c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        જો (r < 0)
                પાછા r;

        r = cg_shift_path(raw, મૂળ, &c);
        જો (r < 0)
                પાછા r;

        જો (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                પાછા 0;
        }

        પાછા strdup_to(ret_cgroup, c);
}

પૂર્ણાંક cg_path_decode_unit(અચલ અક્ષર *cgroup, અક્ષર **ret_unit) {
        પ્રતિજ્ઞા(cgroup);

        size_t n = strcspn(cgroup, "/");
        જો (n < 3)
                પાછા -ENXIO;

        અક્ષર *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        જો (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                પાછા -ENXIO;

        જો (ret_unit)
                પાછા strdup_to(ret_unit, c);

        પાછા 0;
}

સ્થિર bool valid_slice_name(અચલ અક્ષર *p, size_t n) {
        પ્રતિજ્ઞા(p || n == 0);

        જો (n < STRLEN("x.slice"))
                પાછા false;

        અક્ષર *c = strndupa_safe(p, n);
        જો (!endswith(c, ".slice"))
                પાછા false;

        પાછા unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

સ્થિર અચલ અક્ષર* skip_slices(અચલ અક્ષર *p) {
        પ્રતિજ્ઞા(p);

        /* Skips over all slice assignments */

        માટે (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                જો (!valid_slice_name(p, n))
                        પાછા p;

                p += n;
        }
}

પૂર્ણાંક cg_path_get_unit_full(અચલ અક્ષર *path, અક્ષર **ret_unit, અક્ષર **ret_subgroup) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        અચલ અક્ષર *e = skip_slices(path);

        _cleanup_free_ અક્ષર *unit = ખાલી;
        r = cg_path_decode_unit(e, &unit);
        જો (r < 0)
                પાછા r;

        /* We skipped over the slices, don't accept any now */
        જો (endswith(unit, ".slice"))
                પાછા -ENXIO;

        જો (ret_subgroup) {
                _cleanup_free_ અક્ષર *subgroup = ખાલી;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                જો (isempty(e))
                        subgroup = ખાલી;
                નહીંતર {
                        subgroup = strdup(e);
                        જો (!subgroup)
                                પાછા -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        જો (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        પાછા 0;
}

પૂર્ણાંક cg_path_get_unit_path(અચલ અક્ષર *path, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *path_copy = ખાલી;
        અક્ષર *unit_name;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret);

        path_copy = strdup(path);
        જો (!path_copy)
                પાછા -ENOMEM;

        unit_name = (અક્ષર*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        જો (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                પાછા -ENXIO;

        *ret = TAKE_PTR(path_copy);

        પાછા 0;
}

પૂર્ણાંક cg_pid_get_unit_full(pid_t pid, અક્ષર **ret_unit, અક્ષર **ret_subgroup) {
        પૂર્ણાંક r;

        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

પૂર્ણાંક cg_pidref_get_unit_full(અચલ PidRef *pidref, અક્ષર **ret_unit, અક્ષર **ret_subgroup) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        _cleanup_free_ અક્ષર *unit = ખાલી, *subgroup = ખાલી;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        જો (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        જો (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        પાછા 0;
}

સ્થિર અચલ અક્ષર* skip_session(અચલ અક્ષર *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        જો (isempty(p))
                પાછા ખાલી;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        જો (n < STRLEN("session-x.scope"))
                પાછા ખાલી;

        અચલ અક્ષર *s = startswith(p, "session-");
        જો (!s)
                પાછા ખાલી;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        અક્ષર *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        જો (!e)
                પાછા ખાલી;
        *e = '\0';

        જો (!session_id_valid(f))
                પાછા ખાલી;

        પાછા skip_leading_slash(p + n);
}

સ્થિર અચલ અક્ષર* skip_user_manager(અચલ અક્ષર *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        જો (isempty(p))
                પાછા ખાલી;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        જો (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                પાછા ખાલી;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ અક્ષર *unit_name = strndup(p, n);
        જો (!unit_name)
                પાછા ખાલી;

        _cleanup_free_ અક્ષર *i = ખાલી;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        જો (type != UNIT_NAME_INSTANCE)
                પાછા ખાલી;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        જો (!(startswith(unit_name, "user@") && parse_uid(i, ખાલી) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                પાછા ખાલી;

        પાછા skip_leading_slash(p + n);
}

સ્થિર અચલ અક્ષર* skip_user_prefix(અચલ અક્ષર *path) {
        અચલ અક્ષર *e, *t;

        પ્રતિજ્ઞા(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        જો (t)
                પાછા t;

        /* Alternatively skip the user session if it is in the path... */
        પાછા skip_session(e);
}

પૂર્ણાંક cg_path_get_user_unit_full(અચલ અક્ષર *path, અક્ષર **ret_unit, અક્ષર **ret_subgroup) {
        અચલ અક્ષર *t;

        પ્રતિજ્ઞા(path);

        t = skip_user_prefix(path);
        જો (!t)
                પાછા -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        પાછા cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

પૂર્ણાંક cg_pid_get_user_unit_full(pid_t pid, અક્ષર **ret_unit, અક્ષર **ret_subgroup) {
        પૂર્ણાંક r;

        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

પૂર્ણાંક cg_pidref_get_user_unit_full(અચલ PidRef *pidref, અક્ષર **ret_unit, અક્ષર **ret_subgroup) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        _cleanup_free_ અક્ષર *unit = ખાલી, *subgroup = ખાલી;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        જો (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        જો (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        પાછા 0;
}

પૂર્ણાંક cg_path_get_machine_name(અચલ અક્ષર *path, અક્ષર **ret_machine) {
        _cleanup_free_ અક્ષર *u = ખાલી;
        અચલ અક્ષર *sl;
        પૂર્ણાંક r;

        r = cg_path_get_unit(path, &u);
        જો (r < 0)
                પાછા r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        પાછા readlink_malloc(sl, ret_machine);
}

પૂર્ણાંક cg_pid_get_machine_name(pid_t pid, અક્ષર **ret_machine) {
        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        પૂર્ણાંક r;

        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_machine_name(cgroup, ret_machine);
}

પૂર્ણાંક cg_path_get_session(અચલ અક્ષર *path, અક્ષર **ret_session) {
        _cleanup_free_ અક્ષર *unit = ખાલી;
        અક્ષર *start, *end;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        r = cg_path_get_unit(path, &unit);
        જો (r < 0)
                પાછા r;

        start = startswith(unit, "session-");
        જો (!start)
                પાછા -ENXIO;
        end = endswith(start, ".scope");
        જો (!end)
                પાછા -ENXIO;

        *end = 0;
        જો (!session_id_valid(start))
                પાછા -ENXIO;

        જો (!ret_session)
                પાછા 0;

        પાછા strdup_to(ret_session, start);
}

પૂર્ણાંક cg_pid_get_session(pid_t pid, અક્ષર **ret_session) {
        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        પૂર્ણાંક r;

        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_session(cgroup, ret_session);
}

પૂર્ણાંક cg_pidref_get_session(અચલ PidRef *pidref, અક્ષર **ret) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        _cleanup_free_ અક્ષર *session = ખાલી;
        r = cg_pid_get_session(pidref->pid, &session);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = TAKE_PTR(session);
        પાછા 0;
}

પૂર્ણાંક cg_path_get_owner_uid(અચલ અક્ષર *path, uid_t *ret_uid) {
        _cleanup_free_ અક્ષર *slice = ખાલી;
        અક્ષર *start, *end;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        r = cg_path_get_slice(path, &slice);
        જો (r < 0)
                પાછા r;

        start = startswith(slice, "user-");
        જો (!start)
                પાછા -ENXIO;

        end = endswith(start, ".slice");
        જો (!end)
                પાછા -ENXIO;

        *end = 0;
        જો (parse_uid(start, ret_uid) < 0)
                પાછા -ENXIO;

        પાછા 0;
}

પૂર્ણાંક cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        પૂર્ણાંક r;

        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_owner_uid(cgroup, ret_uid);
}

પૂર્ણાંક cg_pidref_get_owner_uid(અચલ PidRef *pidref, uid_t *ret) {
        પૂર્ણાંક r;

        જો (!pidref_is_set(pidref))
                પાછા -ESRCH;
        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        જો (r < 0)
                પાછા r;

        r = pidref_verify(pidref);
        જો (r < 0)
                પાછા r;

        જો (ret)
                *ret = uid;

        પાછા 0;
}

પૂર્ણાંક cg_path_get_slice(અચલ અક્ષર *p, અક્ષર **ret_slice) {
        અચલ અક્ષર *e = ખાલી;

        પ્રતિજ્ઞા(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        માટે (;;) {
                અચલ અક્ષર *s;
                પૂર્ણાંક n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                જો (n < 0)
                        પાછા n;
                જો (!valid_slice_name(s, n))
                        તોડો;

                e = s;
        }

        જો (e)
                પાછા cg_path_decode_unit(e, ret_slice);

        જો (ret_slice)
                પાછા strdup_to(ret_slice, SPECIAL_મૂળ_SLICE);

        પાછા 0;
}

પૂર્ણાંક cg_pid_get_slice(pid_t pid, અક્ષર **ret_slice) {
        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        પૂર્ણાંક r;

        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_slice(cgroup, ret_slice);
}

પૂર્ણાંક cg_path_get_user_slice(અચલ અક્ષર *p, અક્ષર **ret_slice) {
        અચલ અક્ષર *t;
        પ્રતિજ્ઞા(p);

        t = skip_user_prefix(p);
        જો (!t)
                પાછા -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        પાછા cg_path_get_slice(t, ret_slice);
}

પૂર્ણાંક cg_pid_get_user_slice(pid_t pid, અક્ષર **ret_slice) {
        _cleanup_free_ અક્ષર *cgroup = ખાલી;
        પૂર્ણાંક r;

        r = cg_pid_get_path_shifted(pid, ખાલી, &cgroup);
        જો (r < 0)
                પાછા r;

        પાછા cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(અચલ અક્ષર *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        જો (!filename_is_valid(p))
                પાછા true;

        જો (IN_SET(p[0], '_', '.'))
                પાછા true;

        જો (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                પાછા true;

        જો (startswith(p, "cgroup."))
                પાછા true;

        માટે (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                અચલ અક્ષર *q;

                q = startswith(p, cgroup_controller_to_string(c));
                જો (!q)
                        ચાલુ;

                જો (q[0] == '.')
                        પાછા true;
        }

        પાછા false;
}

પૂર્ણાંક cg_escape(અચલ અક્ષર *p, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *n = ખાલી;

        પ્રતિજ્ઞા(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        જો (cg_needs_escape(p)) {
                n = strjoin("_", p);
                જો (!n)
                        પાછા -ENOMEM;

                જો (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        પાછા -EINVAL;
        } નહીંતર {
                n = strdup(p);
                જો (!n)
                        પાછા -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        પાછા 0;
}

અક્ષર* cg_unescape(અચલ અક્ષર *p) {
        પ્રતિજ્ઞા(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        જો (p[0] == '_')
                પાછા (અક્ષર*) p+1;

        પાછા (અક્ષર*) p;
}

પૂર્ણાંક cg_slice_to_path(અચલ અક્ષર *unit, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી, *s = ખાલી, *e = ખાલી;
        અચલ અક્ષર *dash;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(unit);
        પ્રતિજ્ઞા(ret);

        જો (streq(unit, SPECIAL_મૂળ_SLICE))
                પાછા strdup_to(ret, "");

        જો (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                પાછા -EINVAL;

        જો (!endswith(unit, ".slice"))
                પાછા -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        જો (r < 0)
                પાછા r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        જો (dash == p)
                પાછા -EINVAL;

        જ્યારે (dash) {
                _cleanup_free_ અક્ષર *escaped = ખાલી;
                અક્ષર n[dash - p + કદ(".slice")];

#જો HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                જો (IN_SET(dash[1], 0, '-'))
                        પાછા -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                જો (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        પાછા -EINVAL;

                r = cg_escape(n, &escaped);
                જો (r < 0)
                        પાછા r;

                જો (!strextend(&s, escaped, "/"))
                        પાછા -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        જો (r < 0)
                પાછા r;

        જો (!strextend(&s, e))
                પાછા -ENOMEM;

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક cg_is_threaded(અચલ અક્ષર *path) {
        _cleanup_free_ અક્ષર *fs = ખાલી, *contents = ખાલી;
        _cleanup_strv_free_ અક્ષર **v = ખાલી;
        પૂર્ણાંક r;

        r = cg_get_path(path, "cgroup.type", &fs);
        જો (r < 0)
                પાછા r;

        r = read_full_virtual_file(fs, &contents, ખાલી);
        જો (r == -ENOENT)
                પાછા false; /* Assume no. */
        જો (r < 0)
                પાછા r;

        v = strv_split(contents, ખાલી);
        જો (!v)
                પાછા -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        પાછા strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

પૂર્ણાંક cg_set_attribute(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અચલ અક્ષર *કિંમત) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(attribute);

        r = cg_get_path(path, attribute, &p);
        જો (r < 0)
                પાછા r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        પાછા write_string_file(p, કિંમત, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

પૂર્ણાંક cg_get_attribute(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(attribute);

        r = cg_get_path(path, attribute, &p);
        જો (r < 0)
                પાછા r;

        પાછા read_one_લીટી_file(p, ret);
}

પૂર્ણાંક cg_get_attribute_as_uint64(અચલ અક્ષર *path, અચલ અક્ષર *attribute, uint64_t *ret) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        uint64_t v;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = cg_get_attribute(path, attribute, &કિંમત);
        જો (r == -ENOENT)
                પાછા -ENODATA;
        જો (r < 0)
                પાછા r;

        જો (streq(કિંમત, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                પાછા 0;
        }

        r = safe_atou64(કિંમત, &v);
        જો (r < 0)
                પાછા r;

        *ret = v;
        પાછા 0;
}

પૂર્ણાંક cg_get_attribute_as_bool(અચલ અક્ષર *path, અચલ અક્ષર *attribute) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        પૂર્ણાંક r;

        r = cg_get_attribute(path, attribute, &કિંમત);
        જો (r == -ENOENT)
                પાછા -ENODATA;
        જો (r < 0)
                પાછા r;

        પાછા parse_boolean(કિંમત);
}

પૂર્ણાંક cg_get_owner(અચલ અક્ષર *path, uid_t *ret_uid) {
        _cleanup_free_ અક્ષર *f = ખાલી;
        સંરચના stat stats;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_uid);

        r = cg_get_path(path, /* suffix= */ ખાલી, &f);
        જો (r < 0)
                પાછા r;

        જો (stat(f, &stats) < 0)
                પાછા -errno;

        r = stat_verify_directory(&stats);
        જો (r < 0)
                પાછા r;

        *ret_uid = stats.st_uid;
        પાછા 0;
}

પૂર્ણાંક cg_get_keyed_attribute(
                અચલ અક્ષર *path,
                અચલ અક્ષર *attribute,
                અક્ષર * અચલ *ચાવીઓ,
                અક્ષર **values) {

        _cleanup_free_ અક્ષર *filename = ખાલી, *contents = ખાલી;
        size_t n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        જો (r < 0)
                પાછા r;

        r = read_full_file(filename, &contents, /* ret_size= */ ખાલી);
        જો (r < 0)
                પાછા r;

        n = strv_length(ચાવીઓ);
        જો (n == 0) /* No keys to retrieve? That's easy, we are done then */
                પાછા 0;
        પ્રતિજ્ઞા(strv_is_uniq(ચાવીઓ));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        અક્ષર **v = newa0(અક્ષર*, n);
        size_t n_done = 0;

        માટે (અચલ અક્ષર *p = contents; *p;) {
                અચલ અક્ષર *w;
                size_t i;

                માટે (i = 0; i < n; i++) {
                        w = first_word(p, ચાવીઓ[i]);
                        જો (w)
                                તોડો;
                }

                જો (w) {
                        જો (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                જાઓ fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        જો (!v[i]) {
                                r = -ENOMEM;
                                જાઓ fail;
                        }

                        n_done++;
                        જો (n_done >= n)
                                તોડો;

                        p = w + l;
                } નહીંતર
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        જો (n_done < n) {
                r = -ENXIO;
                જાઓ fail;
        }

        memcpy(values, v, કદ(અક્ષર*) * n);
        પાછા 0;

fail:
        free_many_charp(v, n);
        પાછા r;
}

પૂર્ણાંક cg_get_keyed_attribute_uint64(અચલ અક્ષર *path, અચલ અક્ષર *attribute, અચલ અક્ષર *ચાવી, uint64_t *ret) {
        _cleanup_free_ અક્ષર *val = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ચાવી);
        પ્રતિજ્ઞા(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(ચાવી), &val);
        જો (r < 0)
                પાછા r;

        r = safe_atou64(val, ret);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, ચાવી, attribute);

        પાછા 0;
}

પૂર્ણાંક cg_mask_to_string(CGroupMask mask, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        પ્રતિજ્ઞા(ret);

        જો (mask == 0) {
                *ret = ખાલી;
                પાછા 0;
        }

        માટે (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                અચલ અક્ષર *k;
                size_t l;

                જો (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        ચાલુ;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                જો (!GREEDY_REALLOC(s, n + space + l + 1))
                        પાછા -ENOMEM;

                જો (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        પ્રતિજ્ઞા(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        પાછા 0;
}

પૂર્ણાંક cg_mask_from_string(અચલ અક્ષર *s, CGroupMask *ret) {
        CGroupMask m = 0;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(s);

        માટે (;;) {
                _cleanup_free_ અક્ષર *n = ખાલી;
                CGroupController v;
                પૂર્ણાંક r;

                r = extract_first_word(&s, &n, ખાલી, 0);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                v = cgroup_controller_from_string(n);
                જો (v < 0)
                        ચાલુ;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        પાછા 0;
}

પૂર્ણાંક cg_mask_supported_subtree(અચલ અક્ષર *મૂળ, CGroupMask *ret) {
        CGroupMask mask;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ અક્ષર *controllers = ખાલી, *path = ખાલી;
        r = cg_get_path(મૂળ, "cgroup.controllers", &path);
        જો (r < 0)
                પાછા r;

        r = read_one_લીટી_file(path, &controllers);
        જો (r < 0)
                પાછા r;

        r = cg_mask_from_string(controllers, &mask);
        જો (r < 0)
                પાછા r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        પાછા 0;
}

પૂર્ણાંક cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ અક્ષર *મૂળ = ખાલી;
        પૂર્ણાંક r;

        r = cg_get_મૂળ_path(&મૂળ);
        જો (r < 0)
                પાછા r;

        પાછા cg_mask_supported_subtree(મૂળ, ret);
}

પૂર્ણાંક cg_is_delegated(અચલ અક્ષર *path) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        જો (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                પાછા r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        પાછા ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

પૂર્ણાંક cg_is_delegated_fd(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ખાલી, "trusted.delegate", /* at_flags= */ 0);
        જો (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                પાછા r;

        r = getxattr_at_bool(fd, /* path= */ ખાલી, "user.delegate", /* at_flags= */ 0);
        પાછા ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

પૂર્ણાંક cg_has_coredump_receive(અચલ અક્ષર *path) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        જો (ERRNO_IS_NEG_XATTR_ABSENT(r))
                પાછા false;

        પાછા r;
}

અચલ uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

સ્થિર અચલ અક્ષર* અચલ cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

શૂન્ય cgroup_io_limits_list(શૂન્ય) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

સ્થિર અચલ અક્ષર *અચલ cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

સ્થિર અચલ અક્ષર* અચલ managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

સ્થિર અચલ અક્ષર* અચલ managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
