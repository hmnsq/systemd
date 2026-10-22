/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/fs.h>
#ଅନ୍ତର୍ଭାଵ <linux/magic.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/xattr.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "capsule-util.h"
#ଅନ୍ତର୍ଭାଵ "cgroup-util.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "login-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "set.h"
#ଅନ୍ତର୍ଭାଵ "special.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"
#ଅନ୍ତର୍ଭାଵ "unit-name.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"
#ଅନ୍ତର୍ଭାଵ "xattr-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_available(ଶୂନ୍ୟ) {
        ସଂରଚନା statfs fs;

        ଯଦି (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ଯଦି (errno == ENOENT) /* sysfs not mounted? */
                        ଫେରନ୍ତୁ false;

                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ଫେରନ୍ତୁ is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_open(ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_cgroupid_open(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cgroupfs_fd, uint64_t id) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsfd = -EBADF;

        ଯଦି (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ଯଦି (fsfd < 0)
                        ଫେରନ୍ତୁ -errno;

                cgroupfs_fd = fsfd;
        }

        ସଂଘ {
                ସଂରଚନା file_handle file_handle;
                uint8_t space[offsetof(ସଂରଚନା file_handle, f_handle) + ଆକାର(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = ଆକାର(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ଫେରନ୍ତୁ RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_from_cgroupid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cgroupfs_fd, uint64_t id, ଅକ୍ଷର **ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cgfd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ଯଦି (cgfd < 0)
                ଫେରନ୍ତୁ cgfd;

        _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;
        r = fd_get_path(cgfd, &path);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!path_startswith(path, "/sys/fs/cgroup/"))
                ଫେରନ୍ତୁ -EXDEV; /* recognizable error */

        ଯଦି (ret)
                *ret = TAKE_PTR(path);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_enumerate_processes(ଅଚଳ ଅକ୍ଷର *path, FILE **ret) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        FILE *f;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        f = fopen(fs, "re");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        *ret = f;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        ପାଇଁ (;;) {
                errno = 0;
                ଯଦି (fscanf(f, "%lu", &ul) != 1) {

                        ଯଦି (feof(f)) {
                                *ret = 0;
                                ଫେରନ୍ତୁ 0;
                        }

                        ଫେରନ୍ତୁ errno_or_else(EIO);
                }

                ଯଦି (ul > PID_T_MAX)
                        ଫେରନ୍ତୁ -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ଯଦି (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        ଜାରି;

                *ret = (pid_t) ul;
                ଫେରନ୍ତୁ 1;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ଯଦି (r == 0) {
                        *ret = PIDREF_NULL;
                        ଫେରନ୍ତୁ 0;
                }

                ଯଦି (pid == 0)
                        ଫେରନ୍ତୁ -EREMOTE;

                r = pidref_set_pid(ret, pid);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ 1;
                ଯଦି (r != -ESRCH)
                        ଫେରନ୍ତୁ r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(ଶୂନ୍ୟ) {
        ସ୍ଥିର thread_local ପୂର୍ଣ୍ଣସଂଖ୍ୟା supported = -1;

        ଯଦି (supported >= 0)
                ଫେରନ୍ତୁ supported;

        ଯଦି (cg_is_available() <= 0)
                ଫେରନ୍ତୁ (supported = false);

        ଯଦି (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ଫେରନ୍ତୁ (supported = true);
        ଯଦି (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ଫେରନ୍ତୁ (supported = false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_enumerate_subgroups(ଅଚଳ ଅକ୍ଷର *path, DIR **ret) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        DIR *d;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        d = opendir(fs);
        ଯଦି (!d)
                ଫେରନ୍ତୁ -errno;

        *ret = d;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_read_subgroup(DIR *d, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(d);
        ପ୍ରତିଜ୍ଞା(ret);

        FOREACH_DIRENT_ALL(de, d, ଫେରନ୍ତୁ -errno) {
                ଯଦି (de->d_type != DT_DIR)
                        ଜାରି;

                ଯଦି (dot_or_dot_dot(de->d_name))
                        ଜାରି;

                ଫେରନ୍ତୁ strdup_to_full(ret, de->d_name);
        }

        *ret = ଖାଲି;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_kill(
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                ଶୂନ୍ୟ *userdata) {

        _cleanup_set_free_ Set *allocated_set = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ଯଦି (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ଯଦି (!killed_pids) {
                killed_pids = allocated_set = set_new(ଖାଲି);
                ଯଦି (!killed_pids)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        bool done;
        କର {
                _cleanup_fclose_ FILE *f = ଖାଲି;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                ଯଦି (r == -ENOENT)
                        ଭାଙ୍ଗ;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                ପାଇଁ (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ଯଦି (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                ଭାଙ୍ଗ;
                        }
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        ଯଦି (r == 0)
                                ଭାଙ୍ଗ;

                        ଯଦି ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                ଜାରି;

                        ଯଦି (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                ଜାରି;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ଯଦି (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                ଜାରି;
                        }

                        ଯଦି (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ଯଦି (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        ଯଦି (r >= 0) {
                                ଯଦି (flags & CGROUP_SIGCONT)
                                        (ଶୂନ୍ୟ) pidref_kill(&pidref, SIGCONT);

                                ଯଦି (ret == 0) {
                                        ଯଦି (log_kill)
                                                ret = ret_log_kill;
                                        ଅନ୍ୟଥା
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ଯେତେବେଳେ (!done);

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_kill_recursive(
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                ଶୂନ୍ୟ *userdata) {

        _cleanup_set_free_ Set *allocated_set = ଖାଲି;
        _cleanup_closedir_ DIR *d = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(sig >= 0);

        ଯଦି (!killed_pids) {
                killed_pids = allocated_set = set_new(ଖାଲି);
                ଯଦି (!killed_pids)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        ଯଦି (r < 0) {
                ଯଦି (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                ଫେରନ୍ତୁ ret;
        }

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *fn = ଖାଲି, *p = ଖାଲି;

                r = cg_read_subgroup(d, &fn);
                ଯଦି (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        ଭାଙ୍ଗ;
                }
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                p = path_join(empty_to_ମୂଳ(path), fn);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ଯଦି (r != 0 && ret >= 0)
                        ret = r;
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_kill_kernel_sigkill(ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ ଅକ୍ଷର *killfile = ଖାଲି;
        uint64_t n_pids = UINT64_MAX;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        ପ୍ରତିଜ୍ଞା(path);

        ଯଦି (!cg_kill_supported())
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        ଯଦି (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_path(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = path_simplify(t);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_set_xattr(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଶୂନ୍ୟ *ମୂଲ୍ୟ, size_t size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ || size <= 0);

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ RET_NERRNO(setxattr(fs, name, ମୂଲ୍ୟ, size, flags));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_xattr(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret, size_t *ret_size) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(name);

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ lgetxattr_malloc(fs, name, ret, ret_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_xattr_bool(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(name);

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_remove_xattr(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(name);

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ RET_NERRNO(removexattr(fs, name));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_path(pid_t pid, ଅକ୍ଷର **ret_path) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *fs;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
                ଅକ୍ଷର *e;

                r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ -ENODATA;

                e = startswith(ଧାଡ଼ି, "0:");
                ଯଦି (!e)
                        ଜାରି;

                e = strchr(e, ':');
                ଯଦି (!e)
                        ଜାରି;

                _cleanup_free_ ଅକ୍ଷର *path = strdup(e + 1);
                ଯଦି (!path)
                        ଫେରନ୍ତୁ -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ଯଦି (startswith(path, "/../"))
                        ଫେରନ୍ତୁ -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                ଯଦି (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                ଫେରନ୍ତୁ 0;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_path(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_path) {
        _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_path);

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret_path = TAKE_PTR(path);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_empty(ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        ପ୍ରତିଜ୍ଞା(path);

        /* The root cgroup is always populated */
        ଯଦି (empty_or_ମୂଳ(path))
                ଫେରନ୍ତୁ false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ true;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ streq(t, "0");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_split_spec(ଅଚଳ ଅକ୍ଷର *spec, ଅକ୍ଷର **ret_controller, ଅକ୍ଷର **ret_path) {
        _cleanup_free_ ଅକ୍ଷର *controller = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *path;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ଯଦି (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                ଯାଅ finalize;
        }

        ଅଚଳ ଅକ୍ଷର *e = strchr(spec, ':');
        ଯଦି (!e) {
                /* Controller only. */
                ଯଦି (ret_controller) {
                        controller = strdup(spec);
                        ଯଦି (!controller)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                path = ଖାଲି;
        } ଅନ୍ୟଥା {
                /* Both controller and path. */
                ଯଦି (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ଯଦି (!controller)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        ଯଦି (path) {
                /* Non-empty path must be absolute. */
                ଯଦି (!path_is_absolute(path))
                        ଫେରନ୍ତୁ -EINVAL;

                /* Path must not contain dot-dot. */
                ଯଦି (!path_is_safe(path))
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_ମୂଳ_path(ଅକ୍ଷର **ret_path) {
        ଅକ୍ଷର *p, *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_path);

        r = cg_pid_get_path(1, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ଯଦି (e)
                *e = 0;

        *ret_path = p;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_shift_path(ଅଚଳ ଅକ୍ଷର *cgroup, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅଚଳ ଅକ୍ଷର **ret_shifted) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(cgroup);
        ପ୍ରତିଜ୍ଞା(ret_shifted);

        _cleanup_free_ ଅକ୍ଷର *rt = ଖାଲି;
        ଯଦି (!ମୂଳ) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_ମୂଳ_path(&rt);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ମୂଳ = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, ମୂଳ, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_path_shifted(pid_t pid, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret_cgroup) {
        _cleanup_free_ ଅକ୍ଷର *raw = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = cg_shift_path(raw, ମୂଳ, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ strdup_to(ret_cgroup, c);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_path_shifted(ଅଚଳ PidRef *pidref, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret_cgroup) {
        _cleanup_free_ ଅକ୍ଷର *raw = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = cg_shift_path(raw, ମୂଳ, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ strdup_to(ret_cgroup, c);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_decode_unit(ଅଚଳ ଅକ୍ଷର *cgroup, ଅକ୍ଷର **ret_unit) {
        ପ୍ରତିଜ୍ଞା(cgroup);

        size_t n = strcspn(cgroup, "/");
        ଯଦି (n < 3)
                ଫେରନ୍ତୁ -ENXIO;

        ଅକ୍ଷର *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        ଯଦି (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ଫେରନ୍ତୁ -ENXIO;

        ଯଦି (ret_unit)
                ଫେରନ୍ତୁ strdup_to(ret_unit, c);

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର bool valid_slice_name(ଅଚଳ ଅକ୍ଷର *p, size_t n) {
        ପ୍ରତିଜ୍ଞା(p || n == 0);

        ଯଦି (n < STRLEN("x.slice"))
                ଫେରନ୍ତୁ false;

        ଅକ୍ଷର *c = strndupa_safe(p, n);
        ଯଦି (!endswith(c, ".slice"))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* skip_slices(ଅଚଳ ଅକ୍ଷର *p) {
        ପ୍ରତିଜ୍ଞା(p);

        /* Skips over all slice assignments */

        ପାଇଁ (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ଯଦି (!valid_slice_name(p, n))
                        ଫେରନ୍ତୁ p;

                p += n;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_unit_full(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        ଅଚଳ ଅକ୍ଷର *e = skip_slices(path);

        _cleanup_free_ ଅକ୍ଷର *unit = ଖାଲି;
        r = cg_path_decode_unit(e, &unit);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* We skipped over the slices, don't accept any now */
        ଯଦି (endswith(unit, ".slice"))
                ଫେରନ୍ତୁ -ENXIO;

        ଯଦି (ret_subgroup) {
                _cleanup_free_ ଅକ୍ଷର *subgroup = ଖାଲି;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ଯଦି (isempty(e))
                        subgroup = ଖାଲି;
                ଅନ୍ୟଥା {
                        subgroup = strdup(e);
                        ଯଦି (!subgroup)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ଯଦି (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_unit_path(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *path_copy = ଖାଲି;
        ଅକ୍ଷର *unit_name;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        path_copy = strdup(path);
        ଯଦି (!path_copy)
                ଫେରନ୍ତୁ -ENOMEM;

        unit_name = (ଅକ୍ଷର*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ଯଦି (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ଫେରନ୍ତୁ -ENXIO;

        *ret = TAKE_PTR(path_copy);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_unit_full(pid_t pid, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_unit_full(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        _cleanup_free_ ଅକ୍ଷର *unit = ଖାଲି, *subgroup = ଖାଲି;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ଯଦି (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* skip_session(ଅଚଳ ଅକ୍ଷର *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ ଖାଲି;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ଯଦି (n < STRLEN("session-x.scope"))
                ଫେରନ୍ତୁ ଖାଲି;

        ଅଚଳ ଅକ୍ଷର *s = startswith(p, "session-");
        ଯଦି (!s)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        ଅକ୍ଷର *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        ଯଦି (!e)
                ଫେରନ୍ତୁ ଖାଲି;
        *e = '\0';

        ଯଦି (!session_id_valid(f))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ skip_leading_slash(p + n);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* skip_user_manager(ଅଚଳ ଅକ୍ଷର *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ ଖାଲି;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ଯଦି (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ଫେରନ୍ତୁ ଖାଲି;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ ଅକ୍ଷର *unit_name = strndup(p, n);
        ଯଦି (!unit_name)
                ଫେରନ୍ତୁ ଖାଲି;

        _cleanup_free_ ଅକ୍ଷର *i = ଖାଲି;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        ଯଦି (type != UNIT_NAME_INSTANCE)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ଯଦି (!(startswith(unit_name, "user@") && parse_uid(i, ଖାଲି) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ skip_leading_slash(p + n);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* skip_user_prefix(ଅଚଳ ଅକ୍ଷର *path) {
        ଅଚଳ ଅକ୍ଷର *e, *t;

        ପ୍ରତିଜ୍ଞା(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ଯଦି (t)
                ଫେରନ୍ତୁ t;

        /* Alternatively skip the user session if it is in the path... */
        ଫେରନ୍ତୁ skip_session(e);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_user_unit_full(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup) {
        ଅଚଳ ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(path);

        t = skip_user_prefix(path);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ଫେରନ୍ତୁ cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_user_unit_full(pid_t pid, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_user_unit_full(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret_unit, ଅକ୍ଷର **ret_subgroup) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        _cleanup_free_ ଅକ୍ଷର *unit = ଖାଲି, *subgroup = ଖାଲି;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ଯଦି (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_machine_name(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_machine) {
        _cleanup_free_ ଅକ୍ଷର *u = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *sl;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_path_get_unit(path, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ଫେରନ୍ତୁ readlink_malloc(sl, ret_machine);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_machine_name(pid_t pid, ଅକ୍ଷର **ret_machine) {
        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_machine_name(cgroup, ret_machine);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_session(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_session) {
        _cleanup_free_ ଅକ୍ଷର *unit = ଖାଲି;
        ଅକ୍ଷର *start, *end;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = cg_path_get_unit(path, &unit);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        start = startswith(unit, "session-");
        ଯଦି (!start)
                ଫେରନ୍ତୁ -ENXIO;
        end = endswith(start, ".scope");
        ଯଦି (!end)
                ଫେରନ୍ତୁ -ENXIO;

        *end = 0;
        ଯଦି (!session_id_valid(start))
                ଫେରନ୍ତୁ -ENXIO;

        ଯଦି (!ret_session)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ strdup_to(ret_session, start);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_session(pid_t pid, ଅକ୍ଷର **ret_session) {
        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_session(cgroup, ret_session);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_session(ଅଚଳ PidRef *pidref, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        _cleanup_free_ ଅକ୍ଷର *session = ଖାଲି;
        r = cg_pid_get_session(pidref->pid, &session);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = TAKE_PTR(session);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_owner_uid(ଅଚଳ ଅକ୍ଷର *path, uid_t *ret_uid) {
        _cleanup_free_ ଅକ୍ଷର *slice = ଖାଲି;
        ଅକ୍ଷର *start, *end;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = cg_path_get_slice(path, &slice);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        start = startswith(slice, "user-");
        ଯଦି (!start)
                ଫେରନ୍ତୁ -ENXIO;

        end = endswith(start, ".slice");
        ଯଦି (!end)
                ଫେରନ୍ତୁ -ENXIO;

        *end = 0;
        ଯଦି (parse_uid(start, ret_uid) < 0)
                ଫେରନ୍ତୁ -ENXIO;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_owner_uid(cgroup, ret_uid);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pidref_get_owner_uid(ଅଚଳ PidRef *pidref, uid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!pidref_is_set(pidref))
                ଫେରନ୍ତୁ -ESRCH;
        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = pidref_verify(pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret)
                *ret = uid;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_slice(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret_slice) {
        ଅଚଳ ଅକ୍ଷର *e = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *s;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ n;
                ଯଦି (!valid_slice_name(s, n))
                        ଭାଙ୍ଗ;

                e = s;
        }

        ଯଦି (e)
                ଫେରନ୍ତୁ cg_path_decode_unit(e, ret_slice);

        ଯଦି (ret_slice)
                ଫେରନ୍ତୁ strdup_to(ret_slice, SPECIAL_ମୂଳ_SLICE);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_slice(pid_t pid, ଅକ୍ଷର **ret_slice) {
        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_slice(cgroup, ret_slice);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_path_get_user_slice(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret_slice) {
        ଅଚଳ ଅକ୍ଷର *t;
        ପ୍ରତିଜ୍ଞା(p);

        t = skip_user_prefix(p);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ଫେରନ୍ତୁ cg_path_get_slice(t, ret_slice);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_pid_get_user_slice(pid_t pid, ଅକ୍ଷର **ret_slice) {
        _cleanup_free_ ଅକ୍ଷର *cgroup = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_pid_get_path_shifted(pid, ଖାଲି, &cgroup);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(ଅଚଳ ଅକ୍ଷର *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ଯଦି (!filename_is_valid(p))
                ଫେରନ୍ତୁ true;

        ଯଦି (IN_SET(p[0], '_', '.'))
                ଫେରନ୍ତୁ true;

        ଯଦି (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                ଫେରନ୍ତୁ true;

        ଯଦି (startswith(p, "cgroup."))
                ଫେରନ୍ତୁ true;

        ପାଇଁ (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ଅଚଳ ଅକ୍ଷର *q;

                q = startswith(p, cgroup_controller_to_string(c));
                ଯଦି (!q)
                        ଜାରି;

                ଯଦି (q[0] == '.')
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_escape(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ଯଦି (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ଯଦି (!n)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ଫେରନ୍ତୁ -EINVAL;
        } ଅନ୍ୟଥା {
                n = strdup(p);
                ଯଦି (!n)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* cg_unescape(ଅଚଳ ଅକ୍ଷର *p) {
        ପ୍ରତିଜ୍ଞା(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ଯଦି (p[0] == '_')
                ଫେରନ୍ତୁ (ଅକ୍ଷର*) p+1;

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) p;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_slice_to_path(ଅଚଳ ଅକ୍ଷର *unit, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *s = ଖାଲି, *e = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *dash;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(unit);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (streq(unit, SPECIAL_ମୂଳ_SLICE))
                ଫେରନ୍ତୁ strdup_to(ret, "");

        ଯଦି (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!endswith(unit, ".slice"))
                ଫେରନ୍ତୁ -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ଯଦି (dash == p)
                ଫେରନ୍ତୁ -EINVAL;

        ଯେତେବେଳେ (dash) {
                _cleanup_free_ ଅକ୍ଷର *escaped = ଖାଲି;
                ଅକ୍ଷର n[dash - p + ଆକାର(".slice")];

#ଯଦି HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                ଯଦି (IN_SET(dash[1], 0, '-'))
                        ଫେରନ୍ତୁ -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ଯଦି (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ଫେରନ୍ତୁ -EINVAL;

                r = cg_escape(n, &escaped);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (!strextend(&s, escaped, "/"))
                        ଫେରନ୍ତୁ -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!strextend(&s, e))
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_threaded(ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_free_ ଅକ୍ଷର *fs = ଖାଲି, *contents = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **v = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_get_path(path, "cgroup.type", &fs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = read_full_virtual_file(fs, &contents, ଖାଲି);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ false; /* Assume no. */
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        v = strv_split(contents, ଖାଲି);
        ଯଦି (!v)
                ଫେରନ୍ତୁ -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ଫେରନ୍ତୁ strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_set_attribute(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(attribute);

        r = cg_get_path(path, attribute, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ଫେରନ୍ତୁ write_string_file(p, ମୂଲ୍ୟ, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_attribute(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(attribute);

        r = cg_get_path(path, attribute, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ read_one_ଧାଡ଼ି_file(p, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_attribute_as_uint64(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        uint64_t v;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = cg_get_attribute(path, attribute, &ମୂଲ୍ୟ);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ENODATA;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (streq(ମୂଲ୍ୟ, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                ଫେରନ୍ତୁ 0;
        }

        r = safe_atou64(ମୂଲ୍ୟ, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = v;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_attribute_as_bool(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_get_attribute(path, attribute, &ମୂଲ୍ୟ);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -ENODATA;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_boolean(ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_owner(ଅଚଳ ଅକ୍ଷର *path, uid_t *ret_uid) {
        _cleanup_free_ ଅକ୍ଷର *f = ଖାଲି;
        ସଂରଚନା stat stats;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_uid);

        r = cg_get_path(path, /* suffix= */ ଖାଲି, &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (stat(f, &stats) < 0)
                ଫେରନ୍ତୁ -errno;

        r = stat_verify_directory(&stats);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret_uid = stats.st_uid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_keyed_attribute(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *attribute,
                ଅକ୍ଷର * ଅଚଳ *ଚାବିଗୁଡ଼ିକ,
                ଅକ୍ଷର **values) {

        _cleanup_free_ ଅକ୍ଷର *filename = ଖାଲି, *contents = ଖାଲି;
        size_t n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = read_full_file(filename, &contents, /* ret_size= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        n = strv_length(ଚାବିଗୁଡ଼ିକ);
        ଯଦି (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ଫେରନ୍ତୁ 0;
        ପ୍ରତିଜ୍ଞା(strv_is_uniq(ଚାବିଗୁଡ଼ିକ));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        ଅକ୍ଷର **v = newa0(ଅକ୍ଷର*, n);
        size_t n_done = 0;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = contents; *p;) {
                ଅଚଳ ଅକ୍ଷର *w;
                size_t i;

                ପାଇଁ (i = 0; i < n; i++) {
                        w = first_word(p, ଚାବିଗୁଡ଼ିକ[i]);
                        ଯଦି (w)
                                ଭାଙ୍ଗ;
                }

                ଯଦି (w) {
                        ଯଦି (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                ଯାଅ fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ଯଦି (!v[i]) {
                                r = -ENOMEM;
                                ଯାଅ fail;
                        }

                        n_done++;
                        ଯଦି (n_done >= n)
                                ଭାଙ୍ଗ;

                        p = w + l;
                } ଅନ୍ୟଥା
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ଯଦି (n_done < n) {
                r = -ENXIO;
                ଯାଅ fail;
        }

        memcpy(values, v, ଆକାର(ଅକ୍ଷର*) * n);
        ଫେରନ୍ତୁ 0;

fail:
        free_many_charp(v, n);
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_get_keyed_attribute_uint64(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *attribute, ଅଚଳ ଅକ୍ଷର *ଚାବି, uint64_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *val = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ଚାବି);
        ପ୍ରତିଜ୍ଞା(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(ଚାବି), &val);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = safe_atou64(val, ret);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, ଚାବି, attribute);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_to_string(CGroupMask mask, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (mask == 0) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ପାଇଁ (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ଅଚଳ ଅକ୍ଷର *k;
                size_t l;

                ଯଦି (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        ଜାରି;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                ଯଦି (!GREEDY_REALLOC(s, n + space + l + 1))
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        ପ୍ରତିଜ୍ଞା(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_from_string(ଅଚଳ ଅକ୍ଷର *s, CGroupMask *ret) {
        CGroupMask m = 0;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି;
                CGroupController v;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = extract_first_word(&s, &n, ଖାଲି, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                v = cgroup_controller_from_string(n);
                ଯଦି (v < 0)
                        ଜାରି;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_supported_subtree(ଅଚଳ ଅକ୍ଷର *ମୂଳ, CGroupMask *ret) {
        CGroupMask mask;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ ଅକ୍ଷର *controllers = ଖାଲି, *path = ଖାଲି;
        r = cg_get_path(ମୂଳ, "cgroup.controllers", &path);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = read_one_ଧାଡ଼ି_file(path, &controllers);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = cg_mask_from_string(controllers, &mask);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = cg_get_ମୂଳ_path(&ମୂଳ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cg_mask_supported_subtree(ମୂଳ, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_delegated(ଅଚଳ ଅକ୍ଷର *path) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        ଯଦି (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ଫେରନ୍ତୁ r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        ଫେରନ୍ତୁ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_is_delegated_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ଖାଲି, "trusted.delegate", /* at_flags= */ 0);
        ଯଦି (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ଫେରନ୍ତୁ r;

        r = getxattr_at_bool(fd, /* path= */ ଖାଲି, "user.delegate", /* at_flags= */ 0);
        ଫେରନ୍ତୁ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା cg_has_coredump_receive(ଅଚଳ ଅକ୍ଷର *path) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        ଯଦି (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ r;
}

ଅଚଳ uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

ଶୂନ୍ୟ cgroup_io_limits_list(ଶୂନ୍ୟ) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
