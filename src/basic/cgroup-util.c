/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/fs.h>
#অন্তর্ভাব <linux/magic.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/xattr.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "capsule-util.h"
#অন্তর্ভাব "cgroup-util.h"
#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "login-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "set.h"
#অন্তর্ভাব "special.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "unaligned.h"
#অন্তর্ভাব "unit-name.h"
#অন্তর্ভাব "user-util.h"
#অন্তর্ভাব "xattr-util.h"

পূর্ণসংখ্যা cg_is_available(শূন্য) {
        কাঠামো statfs fs;

        যদি (statfs("/sys/fs/cgroup/", &fs) < 0) {
                যদি (errno == ENOENT) /* sysfs not mounted? */
                        ফেরত false;

                ফেরত log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ফেরত is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

পূর্ণসংখ্যা cg_path_open(ধ্রুবক অক্ষর *path) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &fs);
        যদি (r < 0)
                ফেরত r;

        ফেরত RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

পূর্ণসংখ্যা cg_cgroupid_open(পূর্ণসংখ্যা cgroupfs_fd, uint64_t id) {
        _cleanup_close_ পূর্ণসংখ্যা fsfd = -EBADF;

        যদি (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                যদি (fsfd < 0)
                        ফেরত -errno;

                cgroupfs_fd = fsfd;
        }

        সংঘ {
                কাঠামো file_handle file_handle;
                uint8_t space[offsetof(কাঠামো file_handle, f_handle) + আকার(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = আকার(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ফেরত RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

পূর্ণসংখ্যা cg_path_from_cgroupid(পূর্ণসংখ্যা cgroupfs_fd, uint64_t id, অক্ষর **ret) {
        _cleanup_close_ পূর্ণসংখ্যা cgfd = -EBADF;
        পূর্ণসংখ্যা r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        যদি (cgfd < 0)
                ফেরত cgfd;

        _cleanup_free_ অক্ষর *path = ফাঁকা;
        r = fd_get_path(cgfd, &path);
        যদি (r < 0)
                ফেরত r;

        যদি (!path_startswith(path, "/sys/fs/cgroup/"))
                ফেরত -EXDEV; /* recognizable error */

        যদি (ret)
                *ret = TAKE_PTR(path);
        ফেরত 0;
}

পূর্ণসংখ্যা cg_enumerate_processes(ধ্রুবক অক্ষর *path, FILE **ret) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        FILE *f;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        যদি (r < 0)
                ফেরত r;

        f = fopen(fs, "re");
        যদি (!f)
                ফেরত -errno;

        *ret = f;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        জন্য (;;) {
                errno = 0;
                যদি (fscanf(f, "%lu", &ul) != 1) {

                        যদি (feof(f)) {
                                *ret = 0;
                                ফেরত 0;
                        }

                        ফেরত errno_or_else(EIO);
                }

                যদি (ul > PID_T_MAX)
                        ফেরত -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                যদি (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        চালু;

                *ret = (pid_t) ul;
                ফেরত 1;
        }
}

পূর্ণসংখ্যা cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret);

        জন্য (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                যদি (r < 0)
                        ফেরত log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                যদি (r == 0) {
                        *ret = PIDREF_NULL;
                        ফেরত 0;
                }

                যদি (pid == 0)
                        ফেরত -EREMOTE;

                r = pidref_set_pid(ret, pid);
                যদি (r >= 0)
                        ফেরত 1;
                যদি (r != -ESRCH)
                        ফেরত r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(শূন্য) {
        স্থির thread_local পূর্ণসংখ্যা supported = -1;

        যদি (supported >= 0)
                ফেরত supported;

        যদি (cg_is_available() <= 0)
                ফেরত (supported = false);

        যদি (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ফেরত (supported = true);
        যদি (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ফেরত (supported = false);
}

পূর্ণসংখ্যা cg_enumerate_subgroups(ধ্রুবক অক্ষর *path, DIR **ret) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        DIR *d;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &fs);
        যদি (r < 0)
                ফেরত r;

        d = opendir(fs);
        যদি (!d)
                ফেরত -errno;

        *ret = d;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_read_subgroup(DIR *d, অক্ষর **ret) {
        প্রতিজ্ঞা(d);
        প্রতিজ্ঞা(ret);

        FOREACH_DIRENT_ALL(de, d, ফেরত -errno) {
                যদি (de->d_type != DT_DIR)
                        চালু;

                যদি (dot_or_dot_dot(de->d_name))
                        চালু;

                ফেরত strdup_to_full(ret, de->d_name);
        }

        *ret = ফাঁকা;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_kill(
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                শূন্য *userdata) {

        _cleanup_set_free_ Set *allocated_set = ফাঁকা;
        পূর্ণসংখ্যা r, ret = 0;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        যদি (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        যদি (!killed_pids) {
                killed_pids = allocated_set = set_new(ফাঁকা);
                যদি (!killed_pids)
                        ফেরত -ENOMEM;
        }

        bool done;
        করো {
                _cleanup_fclose_ FILE *f = ফাঁকা;
                পূর্ণসংখ্যা ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                যদি (r == -ENOENT)
                        বিরতি;
                যদি (r < 0)
                        ফেরত RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                জন্য (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        যদি (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                বিরতি;
                        }
                        যদি (r < 0)
                                ফেরত RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        যদি (r == 0)
                                বিরতি;

                        যদি ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                চালু;

                        যদি (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                চালু;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        যদি (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                চালু;
                        }

                        যদি (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        যদি (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        যদি (r >= 0) {
                                যদি (flags & CGROUP_SIGCONT)
                                        (শূন্য) pidref_kill(&pidref, SIGCONT);

                                যদি (ret == 0) {
                                        যদি (log_kill)
                                                ret = ret_log_kill;
                                        অন্যথা
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        যদি (r < 0)
                                ফেরত RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } যখন (!done);

        ফেরত ret;
}

পূর্ণসংখ্যা cg_kill_recursive(
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                শূন্য *userdata) {

        _cleanup_set_free_ Set *allocated_set = ফাঁকা;
        _cleanup_closedir_ DIR *d = ফাঁকা;
        পূর্ণসংখ্যা r, ret;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(sig >= 0);

        যদি (!killed_pids) {
                killed_pids = allocated_set = set_new(ফাঁকা);
                যদি (!killed_pids)
                        ফেরত -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        যদি (r < 0) {
                যদি (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                ফেরত ret;
        }

        জন্য (;;) {
                _cleanup_free_ অক্ষর *fn = ফাঁকা, *p = ফাঁকা;

                r = cg_read_subgroup(d, &fn);
                যদি (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        বিরতি;
                }
                যদি (r == 0)
                        বিরতি;

                p = path_join(empty_to_মূল(path), fn);
                যদি (!p)
                        ফেরত -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                যদি (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                যদি (r != 0 && ret >= 0)
                        ret = r;
        }

        ফেরত ret;
}

পূর্ণসংখ্যা cg_kill_kernel_sigkill(ধ্রুবক অক্ষর *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ অক্ষর *killfile = ফাঁকা;
        uint64_t n_pids = UINT64_MAX;
        পূর্ণসংখ্যা r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        প্রতিজ্ঞা(path);

        যদি (!cg_kill_supported())
                ফেরত -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                যদি (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_বাফার);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        যদি (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ফেরত 0;
}

পূর্ণসংখ্যা cg_get_path(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *suffix, অক্ষর **ret) {
        অক্ষর *t;

        প্রতিজ্ঞা(ret);

        যদি (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        যদি (!t)
                ফেরত -ENOMEM;

        *ret = path_simplify(t);
        ফেরত 0;
}

পূর্ণসংখ্যা cg_set_xattr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, ধ্রুবক শূন্য *মান, size_t আকার_2, পূর্ণসংখ্যা flags) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(মান || আকার_2 <= 0);

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &fs);
        যদি (r < 0)
                ফেরত r;

        ফেরত RET_NERRNO(setxattr(fs, name, মান, আকার_2, flags));
}

পূর্ণসংখ্যা cg_get_xattr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, অক্ষর **ret, size_t *ret_আকার) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(name);

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &fs);
        যদি (r < 0)
                ফেরত r;

        ফেরত lgetxattr_malloc(fs, name, ret, ret_আকার);
}

পূর্ণসংখ্যা cg_get_xattr_bool(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(name);

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &fs);
        যদি (r < 0)
                ফেরত r;

        ফেরত getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

পূর্ণসংখ্যা cg_remove_xattr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(name);

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &fs);
        যদি (r < 0)
                ফেরত r;

        ফেরত RET_NERRNO(removexattr(fs, name));
}

পূর্ণসংখ্যা cg_pid_get_path(pid_t pid, অক্ষর **ret_path) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        ধ্রুবক অক্ষর *fs;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pid >= 0);
        প্রতিজ্ঞা(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        যদি (r == -ENOENT)
                ফেরত -ESRCH;
        যদি (r < 0)
                ফেরত r;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *সারি = ফাঁকা;
                অক্ষর *e;

                r = read_সারি(f, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত -ENODATA;

                e = startswith(সারি, "0:");
                যদি (!e)
                        চালু;

                e = strchr(e, ':');
                যদি (!e)
                        চালু;

                _cleanup_free_ অক্ষর *path = strdup(e + 1);
                যদি (!path)
                        ফেরত -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                যদি (startswith(path, "/../"))
                        ফেরত -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                যদি (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                ফেরত 0;
        }
}

পূর্ণসংখ্যা cg_pidref_get_path(ধ্রুবক PidRef *pidref, অক্ষর **ret_path) {
        _cleanup_free_ অক্ষর *path = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_path);

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;
        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        যদি (r < 0)
                ফেরত r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        যদি (r < 0)
                ফেরত r;

        *ret_path = TAKE_PTR(path);
        ফেরত 0;
}

পূর্ণসংখ্যা cg_is_empty(ধ্রুবক অক্ষর *path) {
        _cleanup_free_ অক্ষর *t = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        প্রতিজ্ঞা(path);

        /* The root cgroup is always populated */
        যদি (empty_or_মূল(path))
                ফেরত false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        যদি (r == -ENOENT)
                ফেরত true;
        যদি (r < 0)
                ফেরত r;

        ফেরত streq(t, "0");
}

পূর্ণসংখ্যা cg_split_spec(ধ্রুবক অক্ষর *spec, অক্ষর **ret_controller, অক্ষর **ret_path) {
        _cleanup_free_ অক্ষর *controller = ফাঁকা;
        ধ্রুবক অক্ষর *path;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        যদি (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                যাও finalize;
        }

        ধ্রুবক অক্ষর *e = strchr(spec, ':');
        যদি (!e) {
                /* Controller only. */
                যদি (ret_controller) {
                        controller = strdup(spec);
                        যদি (!controller)
                                ফেরত -ENOMEM;
                }

                path = ফাঁকা;
        } অন্যথা {
                /* Both controller and path. */
                যদি (ret_controller) {
                        controller = strndup(spec, e - spec);
                        যদি (!controller)
                                ফেরত -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        যদি (path) {
                /* Non-empty path must be absolute. */
                যদি (!path_is_absolute(path))
                        ফেরত -EINVAL;

                /* Path must not contain dot-dot. */
                যদি (!path_is_safe(path))
                        ফেরত -EINVAL;
        }

        যদি (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ফেরত 0;
}

পূর্ণসংখ্যা cg_get_মূল_path(অক্ষর **ret_path) {
        অক্ষর *p, *e;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_path);

        r = cg_pid_get_path(1, &p);
        যদি (r < 0)
                ফেরত r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        যদি (e)
                *e = 0;

        *ret_path = p;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_shift_path(ধ্রুবক অক্ষর *cgroup, ধ্রুবক অক্ষর *মূল, ধ্রুবক অক্ষর **ret_shifted) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(cgroup);
        প্রতিজ্ঞা(ret_shifted);

        _cleanup_free_ অক্ষর *rt = ফাঁকা;
        যদি (!মূল) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_মূল_path(&rt);
                যদি (r < 0)
                        ফেরত r;

                মূল = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, মূল, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_pid_get_path_shifted(pid_t pid, ধ্রুবক অক্ষর *মূল, অক্ষর **ret_cgroup) {
        _cleanup_free_ অক্ষর *raw = ফাঁকা;
        ধ্রুবক অক্ষর *c;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pid >= 0);
        প্রতিজ্ঞা(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        যদি (r < 0)
                ফেরত r;

        r = cg_shift_path(raw, মূল, &c);
        যদি (r < 0)
                ফেরত r;

        যদি (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ফেরত 0;
        }

        ফেরত strdup_to(ret_cgroup, c);
}

পূর্ণসংখ্যা cg_pidref_get_path_shifted(ধ্রুবক PidRef *pidref, ধ্রুবক অক্ষর *মূল, অক্ষর **ret_cgroup) {
        _cleanup_free_ অক্ষর *raw = ফাঁকা;
        ধ্রুবক অক্ষর *c;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        যদি (r < 0)
                ফেরত r;

        r = cg_shift_path(raw, মূল, &c);
        যদি (r < 0)
                ফেরত r;

        যদি (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ফেরত 0;
        }

        ফেরত strdup_to(ret_cgroup, c);
}

পূর্ণসংখ্যা cg_path_decode_unit(ধ্রুবক অক্ষর *cgroup, অক্ষর **ret_unit) {
        প্রতিজ্ঞা(cgroup);

        size_t n = strcspn(cgroup, "/");
        যদি (n < 3)
                ফেরত -ENXIO;

        অক্ষর *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        যদি (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ফেরত -ENXIO;

        যদি (ret_unit)
                ফেরত strdup_to(ret_unit, c);

        ফেরত 0;
}

স্থির bool valid_slice_name(ধ্রুবক অক্ষর *p, size_t n) {
        প্রতিজ্ঞা(p || n == 0);

        যদি (n < STRLEN("x.slice"))
                ফেরত false;

        অক্ষর *c = strndupa_safe(p, n);
        যদি (!endswith(c, ".slice"))
                ফেরত false;

        ফেরত unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

স্থির ধ্রুবক অক্ষর* skip_slices(ধ্রুবক অক্ষর *p) {
        প্রতিজ্ঞা(p);

        /* Skips over all slice assignments */

        জন্য (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                যদি (!valid_slice_name(p, n))
                        ফেরত p;

                p += n;
        }
}

পূর্ণসংখ্যা cg_path_get_unit_full(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit, অক্ষর **ret_subgroup) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        ধ্রুবক অক্ষর *e = skip_slices(path);

        _cleanup_free_ অক্ষর *unit = ফাঁকা;
        r = cg_path_decode_unit(e, &unit);
        যদি (r < 0)
                ফেরত r;

        /* We skipped over the slices, don't accept any now */
        যদি (endswith(unit, ".slice"))
                ফেরত -ENXIO;

        যদি (ret_subgroup) {
                _cleanup_free_ অক্ষর *subgroup = ফাঁকা;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                যদি (isempty(e))
                        subgroup = ফাঁকা;
                অন্যথা {
                        subgroup = strdup(e);
                        যদি (!subgroup)
                                ফেরত -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        যদি (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ফেরত 0;
}

পূর্ণসংখ্যা cg_path_get_unit_path(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *path_copy = ফাঁকা;
        অক্ষর *unit_name;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        path_copy = strdup(path);
        যদি (!path_copy)
                ফেরত -ENOMEM;

        unit_name = (অক্ষর*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        যদি (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ফেরত -ENXIO;

        *ret = TAKE_PTR(path_copy);

        ফেরত 0;
}

পূর্ণসংখ্যা cg_pid_get_unit_full(pid_t pid, অক্ষর **ret_unit, অক্ষর **ret_subgroup) {
        পূর্ণসংখ্যা r;

        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

পূর্ণসংখ্যা cg_pidref_get_unit_full(ধ্রুবক PidRef *pidref, অক্ষর **ret_unit, অক্ষর **ret_subgroup) {
        পূর্ণসংখ্যা r;

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;
        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        _cleanup_free_ অক্ষর *unit = ফাঁকা, *subgroup = ফাঁকা;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        যদি (r < 0)
                ফেরত r;

        r = pidref_verify(pidref);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        যদি (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ফেরত 0;
}

স্থির ধ্রুবক অক্ষর* skip_session(ধ্রুবক অক্ষর *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        যদি (isempty(p))
                ফেরত ফাঁকা;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        যদি (n < STRLEN("session-x.scope"))
                ফেরত ফাঁকা;

        ধ্রুবক অক্ষর *s = startswith(p, "session-");
        যদি (!s)
                ফেরত ফাঁকা;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        অক্ষর *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        যদি (!e)
                ফেরত ফাঁকা;
        *e = '\0';

        যদি (!session_id_valid(f))
                ফেরত ফাঁকা;

        ফেরত skip_leading_slash(p + n);
}

স্থির ধ্রুবক অক্ষর* skip_user_manager(ধ্রুবক অক্ষর *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        যদি (isempty(p))
                ফেরত ফাঁকা;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        যদি (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ফেরত ফাঁকা;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ অক্ষর *unit_name = strndup(p, n);
        যদি (!unit_name)
                ফেরত ফাঁকা;

        _cleanup_free_ অক্ষর *i = ফাঁকা;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        যদি (type != UNIT_NAME_INSTANCE)
                ফেরত ফাঁকা;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        যদি (!(startswith(unit_name, "user@") && parse_uid(i, ফাঁকা) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ফেরত ফাঁকা;

        ফেরত skip_leading_slash(p + n);
}

স্থির ধ্রুবক অক্ষর* skip_user_prefix(ধ্রুবক অক্ষর *path) {
        ধ্রুবক অক্ষর *e, *t;

        প্রতিজ্ঞা(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        যদি (t)
                ফেরত t;

        /* Alternatively skip the user session if it is in the path... */
        ফেরত skip_session(e);
}

পূর্ণসংখ্যা cg_path_get_user_unit_full(ধ্রুবক অক্ষর *path, অক্ষর **ret_unit, অক্ষর **ret_subgroup) {
        ধ্রুবক অক্ষর *t;

        প্রতিজ্ঞা(path);

        t = skip_user_prefix(path);
        যদি (!t)
                ফেরত -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ফেরত cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

পূর্ণসংখ্যা cg_pid_get_user_unit_full(pid_t pid, অক্ষর **ret_unit, অক্ষর **ret_subgroup) {
        পূর্ণসংখ্যা r;

        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

পূর্ণসংখ্যা cg_pidref_get_user_unit_full(ধ্রুবক PidRef *pidref, অক্ষর **ret_unit, অক্ষর **ret_subgroup) {
        পূর্ণসংখ্যা r;

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;
        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        _cleanup_free_ অক্ষর *unit = ফাঁকা, *subgroup = ফাঁকা;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        যদি (r < 0)
                ফেরত r;

        r = pidref_verify(pidref);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        যদি (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ফেরত 0;
}

পূর্ণসংখ্যা cg_path_get_machine_name(ধ্রুবক অক্ষর *path, অক্ষর **ret_machine) {
        _cleanup_free_ অক্ষর *u = ফাঁকা;
        ধ্রুবক অক্ষর *sl;
        পূর্ণসংখ্যা r;

        r = cg_path_get_unit(path, &u);
        যদি (r < 0)
                ফেরত r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ফেরত readlink_malloc(sl, ret_machine);
}

পূর্ণসংখ্যা cg_pid_get_machine_name(pid_t pid, অক্ষর **ret_machine) {
        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_machine_name(cgroup, ret_machine);
}

পূর্ণসংখ্যা cg_path_get_session(ধ্রুবক অক্ষর *path, অক্ষর **ret_session) {
        _cleanup_free_ অক্ষর *unit = ফাঁকা;
        অক্ষর *start, *end;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        r = cg_path_get_unit(path, &unit);
        যদি (r < 0)
                ফেরত r;

        start = startswith(unit, "session-");
        যদি (!start)
                ফেরত -ENXIO;
        end = endswith(start, ".scope");
        যদি (!end)
                ফেরত -ENXIO;

        *end = 0;
        যদি (!session_id_valid(start))
                ফেরত -ENXIO;

        যদি (!ret_session)
                ফেরত 0;

        ফেরত strdup_to(ret_session, start);
}

পূর্ণসংখ্যা cg_pid_get_session(pid_t pid, অক্ষর **ret_session) {
        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_session(cgroup, ret_session);
}

পূর্ণসংখ্যা cg_pidref_get_session(ধ্রুবক PidRef *pidref, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;
        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        _cleanup_free_ অক্ষর *session = ফাঁকা;
        r = cg_pid_get_session(pidref->pid, &session);
        যদি (r < 0)
                ফেরত r;

        r = pidref_verify(pidref);
        যদি (r < 0)
                ফেরত r;

        যদি (ret)
                *ret = TAKE_PTR(session);
        ফেরত 0;
}

পূর্ণসংখ্যা cg_path_get_owner_uid(ধ্রুবক অক্ষর *path, uid_t *ret_uid) {
        _cleanup_free_ অক্ষর *slice = ফাঁকা;
        অক্ষর *start, *end;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        r = cg_path_get_slice(path, &slice);
        যদি (r < 0)
                ফেরত r;

        start = startswith(slice, "user-");
        যদি (!start)
                ফেরত -ENXIO;

        end = endswith(start, ".slice");
        যদি (!end)
                ফেরত -ENXIO;

        *end = 0;
        যদি (parse_uid(start, ret_uid) < 0)
                ফেরত -ENXIO;

        ফেরত 0;
}

পূর্ণসংখ্যা cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_owner_uid(cgroup, ret_uid);
}

পূর্ণসংখ্যা cg_pidref_get_owner_uid(ধ্রুবক PidRef *pidref, uid_t *ret) {
        পূর্ণসংখ্যা r;

        যদি (!pidref_is_set(pidref))
                ফেরত -ESRCH;
        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        যদি (r < 0)
                ফেরত r;

        r = pidref_verify(pidref);
        যদি (r < 0)
                ফেরত r;

        যদি (ret)
                *ret = uid;

        ফেরত 0;
}

পূর্ণসংখ্যা cg_path_get_slice(ধ্রুবক অক্ষর *p, অক্ষর **ret_slice) {
        ধ্রুবক অক্ষর *e = ফাঁকা;

        প্রতিজ্ঞা(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        জন্য (;;) {
                ধ্রুবক অক্ষর *s;
                পূর্ণসংখ্যা n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                যদি (n < 0)
                        ফেরত n;
                যদি (!valid_slice_name(s, n))
                        বিরতি;

                e = s;
        }

        যদি (e)
                ফেরত cg_path_decode_unit(e, ret_slice);

        যদি (ret_slice)
                ফেরত strdup_to(ret_slice, SPECIAL_মূল_SLICE);

        ফেরত 0;
}

পূর্ণসংখ্যা cg_pid_get_slice(pid_t pid, অক্ষর **ret_slice) {
        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_slice(cgroup, ret_slice);
}

পূর্ণসংখ্যা cg_path_get_user_slice(ধ্রুবক অক্ষর *p, অক্ষর **ret_slice) {
        ধ্রুবক অক্ষর *t;
        প্রতিজ্ঞা(p);

        t = skip_user_prefix(p);
        যদি (!t)
                ফেরত -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ফেরত cg_path_get_slice(t, ret_slice);
}

পূর্ণসংখ্যা cg_pid_get_user_slice(pid_t pid, অক্ষর **ret_slice) {
        _cleanup_free_ অক্ষর *cgroup = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_pid_get_path_shifted(pid, ফাঁকা, &cgroup);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(ধ্রুবক অক্ষর *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        যদি (!filename_is_valid(p))
                ফেরত true;

        যদি (IN_SET(p[0], '_', '.'))
                ফেরত true;

        যদি (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                ফেরত true;

        যদি (startswith(p, "cgroup."))
                ফেরত true;

        জন্য (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ধ্রুবক অক্ষর *q;

                q = startswith(p, cgroup_controller_to_string(c));
                যদি (!q)
                        চালু;

                যদি (q[0] == '.')
                        ফেরত true;
        }

        ফেরত false;
}

পূর্ণসংখ্যা cg_escape(ধ্রুবক অক্ষর *p, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *n = ফাঁকা;

        প্রতিজ্ঞা(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        যদি (cg_needs_escape(p)) {
                n = strjoin("_", p);
                যদি (!n)
                        ফেরত -ENOMEM;

                যদি (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ফেরত -EINVAL;
        } অন্যথা {
                n = strdup(p);
                যদি (!n)
                        ফেরত -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ফেরত 0;
}

অক্ষর* cg_unescape(ধ্রুবক অক্ষর *p) {
        প্রতিজ্ঞা(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        যদি (p[0] == '_')
                ফেরত (অক্ষর*) p+1;

        ফেরত (অক্ষর*) p;
}

পূর্ণসংখ্যা cg_slice_to_path(ধ্রুবক অক্ষর *unit, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা, *s = ফাঁকা, *e = ফাঁকা;
        ধ্রুবক অক্ষর *dash;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(unit);
        প্রতিজ্ঞা(ret);

        যদি (streq(unit, SPECIAL_মূল_SLICE))
                ফেরত strdup_to(ret, "");

        যদি (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ফেরত -EINVAL;

        যদি (!endswith(unit, ".slice"))
                ফেরত -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        যদি (r < 0)
                ফেরত r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        যদি (dash == p)
                ফেরত -EINVAL;

        যখন (dash) {
                _cleanup_free_ অক্ষর *escaped = ফাঁকা;
                অক্ষর n[dash - p + আকার(".slice")];

#যদি HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                যদি (IN_SET(dash[1], 0, '-'))
                        ফেরত -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                যদি (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ফেরত -EINVAL;

                r = cg_escape(n, &escaped);
                যদি (r < 0)
                        ফেরত r;

                যদি (!strextend(&s, escaped, "/"))
                        ফেরত -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        যদি (r < 0)
                ফেরত r;

        যদি (!strextend(&s, e))
                ফেরত -ENOMEM;

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা cg_is_threaded(ধ্রুবক অক্ষর *path) {
        _cleanup_free_ অক্ষর *fs = ফাঁকা, *contents = ফাঁকা;
        _cleanup_strv_free_ অক্ষর **v = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_get_path(path, "cgroup.type", &fs);
        যদি (r < 0)
                ফেরত r;

        r = read_full_virtual_file(fs, &contents, ফাঁকা);
        যদি (r == -ENOENT)
                ফেরত false; /* Assume no. */
        যদি (r < 0)
                ফেরত r;

        v = strv_split(contents, ফাঁকা);
        যদি (!v)
                ফেরত -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ফেরত strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

পূর্ণসংখ্যা cg_set_attribute(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, ধ্রুবক অক্ষর *মান) {
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(attribute);

        r = cg_get_path(path, attribute, &p);
        যদি (r < 0)
                ফেরত r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ফেরত write_string_file(p, মান, WRITE_STRING_FILE_DISABLE_বাফার|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

পূর্ণসংখ্যা cg_get_attribute(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(attribute);

        r = cg_get_path(path, attribute, &p);
        যদি (r < 0)
                ফেরত r;

        ফেরত read_one_সারি_file(p, ret);
}

পূর্ণসংখ্যা cg_get_attribute_as_uint64(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, uint64_t *ret) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        uint64_t v;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = cg_get_attribute(path, attribute, &মান);
        যদি (r == -ENOENT)
                ফেরত -ENODATA;
        যদি (r < 0)
                ফেরত r;

        যদি (streq(মান, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                ফেরত 0;
        }

        r = safe_atou64(মান, &v);
        যদি (r < 0)
                ফেরত r;

        *ret = v;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_get_attribute_as_bool(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_get_attribute(path, attribute, &মান);
        যদি (r == -ENOENT)
                ফেরত -ENODATA;
        যদি (r < 0)
                ফেরত r;

        ফেরত parse_boolean(মান);
}

পূর্ণসংখ্যা cg_get_owner(ধ্রুবক অক্ষর *path, uid_t *ret_uid) {
        _cleanup_free_ অক্ষর *f = ফাঁকা;
        কাঠামো stat stats;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_uid);

        r = cg_get_path(path, /* suffix= */ ফাঁকা, &f);
        যদি (r < 0)
                ফেরত r;

        যদি (stat(f, &stats) < 0)
                ফেরত -errno;

        r = stat_verify_directory(&stats);
        যদি (r < 0)
                ফেরত r;

        *ret_uid = stats.st_uid;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_get_keyed_attribute(
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *attribute,
                অক্ষর * ধ্রুবক *চাবিগুলি,
                অক্ষর **values) {

        _cleanup_free_ অক্ষর *filename = ফাঁকা, *contents = ফাঁকা;
        size_t n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        যদি (r < 0)
                ফেরত r;

        r = read_full_file(filename, &contents, /* ret_size= */ ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        n = strv_দৈর্ঘ্য(চাবিগুলি);
        যদি (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ফেরত 0;
        প্রতিজ্ঞা(strv_is_uniq(চাবিগুলি));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        অক্ষর **v = newa0(অক্ষর*, n);
        size_t n_done = 0;

        জন্য (ধ্রুবক অক্ষর *p = contents; *p;) {
                ধ্রুবক অক্ষর *w;
                size_t i;

                জন্য (i = 0; i < n; i++) {
                        w = first_word(p, চাবিগুলি[i]);
                        যদি (w)
                                বিরতি;
                }

                যদি (w) {
                        যদি (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                যাও fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        যদি (!v[i]) {
                                r = -ENOMEM;
                                যাও fail;
                        }

                        n_done++;
                        যদি (n_done >= n)
                                বিরতি;

                        p = w + l;
                } অন্যথা
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        যদি (n_done < n) {
                r = -ENXIO;
                যাও fail;
        }

        memcpy(values, v, আকার(অক্ষর*) * n);
        ফেরত 0;

fail:
        free_many_charp(v, n);
        ফেরত r;
}

পূর্ণসংখ্যা cg_get_keyed_attribute_uint64(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *attribute, ধ্রুবক অক্ষর *চাবি, uint64_t *ret) {
        _cleanup_free_ অক্ষর *val = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(চাবি), &val);
        যদি (r < 0)
                ফেরত r;

        r = safe_atou64(val, ret);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, চাবি, attribute);

        ফেরত 0;
}

পূর্ণসংখ্যা cg_mask_to_string(CGroupMask mask, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        প্রতিজ্ঞা(ret);

        যদি (mask == 0) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        জন্য (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ধ্রুবক অক্ষর *k;
                size_t l;

                যদি (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        চালু;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                যদি (!GREEDY_REALLOC(s, n + space + l + 1))
                        ফেরত -ENOMEM;

                যদি (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        প্রতিজ্ঞা(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ফেরত 0;
}

পূর্ণসংখ্যা cg_mask_from_string(ধ্রুবক অক্ষর *s, CGroupMask *ret) {
        CGroupMask m = 0;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(s);

        জন্য (;;) {
                _cleanup_free_ অক্ষর *n = ফাঁকা;
                CGroupController v;
                পূর্ণসংখ্যা r;

                r = extract_first_word(&s, &n, ফাঁকা, 0);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                v = cgroup_controller_from_string(n);
                যদি (v < 0)
                        চালু;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_mask_supported_subtree(ধ্রুবক অক্ষর *মূল, CGroupMask *ret) {
        CGroupMask mask;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ অক্ষর *controllers = ফাঁকা, *path = ফাঁকা;
        r = cg_get_path(মূল, "cgroup.controllers", &path);
        যদি (r < 0)
                ফেরত r;

        r = read_one_সারি_file(path, &controllers);
        যদি (r < 0)
                ফেরত r;

        r = cg_mask_from_string(controllers, &mask);
        যদি (r < 0)
                ফেরত r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        ফেরত 0;
}

পূর্ণসংখ্যা cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ অক্ষর *মূল = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = cg_get_মূল_path(&মূল);
        যদি (r < 0)
                ফেরত r;

        ফেরত cg_mask_supported_subtree(মূল, ret);
}

পূর্ণসংখ্যা cg_is_delegated(ধ্রুবক অক্ষর *path) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        যদি (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ফেরত r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        ফেরত ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

পূর্ণসংখ্যা cg_is_delegated_fd(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ফাঁকা, "trusted.delegate", /* at_flags= */ 0);
        যদি (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ফেরত r;

        r = getxattr_at_bool(fd, /* path= */ ফাঁকা, "user.delegate", /* at_flags= */ 0);
        ফেরত ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

পূর্ণসংখ্যা cg_has_coredump_receive(ধ্রুবক অক্ষর *path) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        যদি (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ফেরত false;

        ফেরত r;
}

ধ্রুবক uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

স্থির ধ্রুবক অক্ষর* ধ্রুবক cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

শূন্য cgroup_io_limits_list(শূন্য) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

স্থির ধ্রুবক অক্ষর* ধ্রুবক managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

স্থির ধ্রুবক অক্ষর* ধ্রুবক managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
