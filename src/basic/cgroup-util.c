/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/fs.h>
#അന്തര്ഭാവ <linux/magic.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/xattr.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "capsule-util.h"
#അന്തര്ഭാവ "cgroup-util.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "login-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "set.h"
#അന്തര്ഭാവ "special.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "unaligned.h"
#അന്തര്ഭാവ "unit-name.h"
#അന്തര്ഭാവ "user-util.h"
#അന്തര്ഭാവ "xattr-util.h"

പൂർണ്ണസംഖ്യ cg_is_available(ശൂന്യം) {
        ഘടന statfs fs;

        എങ്കിൽ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                എങ്കിൽ (errno == ENOENT) /* sysfs not mounted? */
                        തിരികെ false;

                തിരികെ log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        തിരികെ is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

പൂർണ്ണസംഖ്യ cg_path_open(സ്ഥിരം അക്ഷരം *path) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

പൂർണ്ണസംഖ്യ cg_cgroupid_open(പൂർണ്ണസംഖ്യ cgroupfs_fd, uint64_t id) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fsfd = -EBADF;

        എങ്കിൽ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                എങ്കിൽ (fsfd < 0)
                        തിരികെ -errno;

                cgroupfs_fd = fsfd;
        }

        സംഘം {
                ഘടന file_handle file_handle;
                uint8_t space[offsetof(ഘടന file_handle, f_handle) + വലുപ്പം(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = വലുപ്പം(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        തിരികെ RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

പൂർണ്ണസംഖ്യ cg_path_from_cgroupid(പൂർണ്ണസംഖ്യ cgroupfs_fd, uint64_t id, അക്ഷരം **ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ cgfd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        എങ്കിൽ (cgfd < 0)
                തിരികെ cgfd;

        _cleanup_free_ അക്ഷരം *path = ശൂന്യ;
        r = fd_get_path(cgfd, &path);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!path_startswith(path, "/sys/fs/cgroup/"))
                തിരികെ -EXDEV; /* recognizable error */

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(path);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_enumerate_processes(സ്ഥിരം അക്ഷരം *path, FILE **ret) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        FILE *f;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        f = fopen(fs, "re");
        എങ്കിൽ (!f)
                തിരികെ -errno;

        *ret = f;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        വേണ്ടി (;;) {
                errno = 0;
                എങ്കിൽ (fscanf(f, "%lu", &ul) != 1) {

                        എങ്കിൽ (feof(f)) {
                                *ret = 0;
                                തിരികെ 0;
                        }

                        തിരികെ errno_or_else(EIO);
                }

                എങ്കിൽ (ul > PID_T_MAX)
                        തിരികെ -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                എങ്കിൽ (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        തുടരുക;

                *ret = (pid_t) ul;
                തിരികെ 1;
        }
}

പൂർണ്ണസംഖ്യ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                എങ്കിൽ (r == 0) {
                        *ret = PIDREF_NULL;
                        തിരികെ 0;
                }

                എങ്കിൽ (pid == 0)
                        തിരികെ -EREMOTE;

                r = pidref_set_pid(ret, pid);
                എങ്കിൽ (r >= 0)
                        തിരികെ 1;
                എങ്കിൽ (r != -ESRCH)
                        തിരികെ r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(ശൂന്യം) {
        സ്ഥിതം thread_local പൂർണ്ണസംഖ്യ supported = -1;

        എങ്കിൽ (supported >= 0)
                തിരികെ supported;

        എങ്കിൽ (cg_is_available() <= 0)
                തിരികെ (supported = false);

        എങ്കിൽ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                തിരികെ (supported = true);
        എങ്കിൽ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        തിരികെ (supported = false);
}

പൂർണ്ണസംഖ്യ cg_enumerate_subgroups(സ്ഥിരം അക്ഷരം *path, DIR **ret) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        DIR *d;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        d = opendir(fs);
        എങ്കിൽ (!d)
                തിരികെ -errno;

        *ret = d;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_read_subgroup(DIR *d, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(d);
        പ്രതിജ്ഞാ(ret);

        FOREACH_DIRENT_ALL(de, d, തിരികെ -errno) {
                എങ്കിൽ (de->d_type != DT_DIR)
                        തുടരുക;

                എങ്കിൽ (dot_or_dot_dot(de->d_name))
                        തുടരുക;

                തിരികെ strdup_to_full(ret, de->d_name);
        }

        *ret = ശൂന്യ;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_kill(
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                ശൂന്യം *userdata) {

        _cleanup_set_free_ Set *allocated_set = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r, ret = 0;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        എങ്കിൽ (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        എങ്കിൽ (!killed_pids) {
                killed_pids = allocated_set = set_new(ശൂന്യ);
                എങ്കിൽ (!killed_pids)
                        തിരികെ -ENOMEM;
        }

        bool done;
        ചെയ്യുക {
                _cleanup_fclose_ FILE *f = ശൂന്യ;
                പൂർണ്ണസംഖ്യ ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                എങ്കിൽ (r == -ENOENT)
                        നിർത്തുക;
                എങ്കിൽ (r < 0)
                        തിരികെ RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                വേണ്ടി (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        എങ്കിൽ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                നിർത്തുക;
                        }
                        എങ്കിൽ (r < 0)
                                തിരികെ RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        എങ്കിൽ (r == 0)
                                നിർത്തുക;

                        എങ്കിൽ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                തുടരുക;

                        എങ്കിൽ (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                തുടരുക;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        എങ്കിൽ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                തുടരുക;
                        }

                        എങ്കിൽ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        എങ്കിൽ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        എങ്കിൽ (r >= 0) {
                                എങ്കിൽ (flags & CGROUP_SIGCONT)
                                        (ശൂന്യം) pidref_kill(&pidref, SIGCONT);

                                എങ്കിൽ (ret == 0) {
                                        എങ്കിൽ (log_kill)
                                                ret = ret_log_kill;
                                        അല്ലെങ്കിൽ
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        എങ്കിൽ (r < 0)
                                തിരികെ RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ആകുമ്പോൾ (!done);

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ cg_kill_recursive(
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                ശൂന്യം *userdata) {

        _cleanup_set_free_ Set *allocated_set = ശൂന്യ;
        _cleanup_closedir_ DIR *d = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r, ret;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(sig >= 0);

        എങ്കിൽ (!killed_pids) {
                killed_pids = allocated_set = set_new(ശൂന്യ);
                എങ്കിൽ (!killed_pids)
                        തിരികെ -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                തിരികെ ret;
        }

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *fn = ശൂന്യ, *p = ശൂന്യ;

                r = cg_read_subgroup(d, &fn);
                എങ്കിൽ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        നിർത്തുക;
                }
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                p = path_join(empty_to_വേര്(path), fn);
                എങ്കിൽ (!p)
                        തിരികെ -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                എങ്കിൽ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                എങ്കിൽ (r != 0 && ret >= 0)
                        ret = r;
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ cg_kill_kernel_sigkill(സ്ഥിരം അക്ഷരം *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ അക്ഷരം *killfile = ശൂന്യ;
        uint64_t n_pids = UINT64_MAX;
        പൂർണ്ണസംഖ്യ r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        പ്രതിജ്ഞാ(path);

        എങ്കിൽ (!cg_kill_supported())
                തിരികെ -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                എങ്കിൽ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        എങ്കിൽ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_get_path(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret) {
        അക്ഷരം *t;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        *ret = path_simplify(t);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_set_xattr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം ശൂന്യം *മൂല്യം, size_t size, പൂർണ്ണസംഖ്യ flags) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(മൂല്യം || size <= 0);

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ RET_NERRNO(setxattr(fs, name, മൂല്യം, size, flags));
}

പൂർണ്ണസംഖ്യ cg_get_xattr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret, size_t *ret_size) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(name);

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ lgetxattr_malloc(fs, name, ret, ret_size);
}

പൂർണ്ണസംഖ്യ cg_get_xattr_bool(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(name);

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

പൂർണ്ണസംഖ്യ cg_remove_xattr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(name);

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ RET_NERRNO(removexattr(fs, name));
}

പൂർണ്ണസംഖ്യ cg_pid_get_path(pid_t pid, അക്ഷരം **ret_path) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *fs;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ESRCH;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
                അക്ഷരം *e;

                r = read_വരി(f, LONG_വരി_MAX, &വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ -ENODATA;

                e = startswith(വരി, "0:");
                എങ്കിൽ (!e)
                        തുടരുക;

                e = strchr(e, ':');
                എങ്കിൽ (!e)
                        തുടരുക;

                _cleanup_free_ അക്ഷരം *path = strdup(e + 1);
                എങ്കിൽ (!path)
                        തിരികെ -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                എങ്കിൽ (startswith(path, "/../"))
                        തിരികെ -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                എങ്കിൽ (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                തിരികെ 0;
        }
}

പൂർണ്ണസംഖ്യ cg_pidref_get_path(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_path) {
        _cleanup_free_ അക്ഷരം *path = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_path);

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret_path = TAKE_PTR(path);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_is_empty(സ്ഥിരം അക്ഷരം *path) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        പ്രതിജ്ഞാ(path);

        /* The root cgroup is always populated */
        എങ്കിൽ (empty_or_വേര്(path))
                തിരികെ false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ true;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ streq(t, "0");
}

പൂർണ്ണസംഖ്യ cg_split_spec(സ്ഥിരം അക്ഷരം *spec, അക്ഷരം **ret_controller, അക്ഷരം **ret_path) {
        _cleanup_free_ അക്ഷരം *controller = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *path;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        എങ്കിൽ (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                പോകുക finalize;
        }

        സ്ഥിരം അക്ഷരം *e = strchr(spec, ':');
        എങ്കിൽ (!e) {
                /* Controller only. */
                എങ്കിൽ (ret_controller) {
                        controller = strdup(spec);
                        എങ്കിൽ (!controller)
                                തിരികെ -ENOMEM;
                }

                path = ശൂന്യ;
        } അല്ലെങ്കിൽ {
                /* Both controller and path. */
                എങ്കിൽ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        എങ്കിൽ (!controller)
                                തിരികെ -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        എങ്കിൽ (path) {
                /* Non-empty path must be absolute. */
                എങ്കിൽ (!path_is_absolute(path))
                        തിരികെ -EINVAL;

                /* Path must not contain dot-dot. */
                എങ്കിൽ (!path_is_safe(path))
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_get_വേര്_path(അക്ഷരം **ret_path) {
        അക്ഷരം *p, *e;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_path);

        r = cg_pid_get_path(1, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        എങ്കിൽ (e)
                *e = 0;

        *ret_path = p;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_shift_path(സ്ഥിരം അക്ഷരം *cgroup, സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം **ret_shifted) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(cgroup);
        പ്രതിജ്ഞാ(ret_shifted);

        _cleanup_free_ അക്ഷരം *rt = ശൂന്യ;
        എങ്കിൽ (!വേര്) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_വേര്_path(&rt);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                വേര് = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, വേര്, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_pid_get_path_shifted(pid_t pid, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret_cgroup) {
        _cleanup_free_ അക്ഷരം *raw = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = cg_shift_path(raw, വേര്, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                തിരികെ 0;
        }

        തിരികെ strdup_to(ret_cgroup, c);
}

പൂർണ്ണസംഖ്യ cg_pidref_get_path_shifted(സ്ഥിരം PidRef *pidref, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret_cgroup) {
        _cleanup_free_ അക്ഷരം *raw = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = cg_shift_path(raw, വേര്, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                തിരികെ 0;
        }

        തിരികെ strdup_to(ret_cgroup, c);
}

പൂർണ്ണസംഖ്യ cg_path_decode_unit(സ്ഥിരം അക്ഷരം *cgroup, അക്ഷരം **ret_unit) {
        പ്രതിജ്ഞാ(cgroup);

        size_t n = strcspn(cgroup, "/");
        എങ്കിൽ (n < 3)
                തിരികെ -ENXIO;

        അക്ഷരം *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        എങ്കിൽ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                തിരികെ -ENXIO;

        എങ്കിൽ (ret_unit)
                തിരികെ strdup_to(ret_unit, c);

        തിരികെ 0;
}

സ്ഥിതം bool valid_slice_name(സ്ഥിരം അക്ഷരം *p, size_t n) {
        പ്രതിജ്ഞാ(p || n == 0);

        എങ്കിൽ (n < STRLEN("x.slice"))
                തിരികെ false;

        അക്ഷരം *c = strndupa_safe(p, n);
        എങ്കിൽ (!endswith(c, ".slice"))
                തിരികെ false;

        തിരികെ unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* skip_slices(സ്ഥിരം അക്ഷരം *p) {
        പ്രതിജ്ഞാ(p);

        /* Skips over all slice assignments */

        വേണ്ടി (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                എങ്കിൽ (!valid_slice_name(p, n))
                        തിരികെ p;

                p += n;
        }
}

പൂർണ്ണസംഖ്യ cg_path_get_unit_full(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        സ്ഥിരം അക്ഷരം *e = skip_slices(path);

        _cleanup_free_ അക്ഷരം *unit = ശൂന്യ;
        r = cg_path_decode_unit(e, &unit);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* We skipped over the slices, don't accept any now */
        എങ്കിൽ (endswith(unit, ".slice"))
                തിരികെ -ENXIO;

        എങ്കിൽ (ret_subgroup) {
                _cleanup_free_ അക്ഷരം *subgroup = ശൂന്യ;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                എങ്കിൽ (isempty(e))
                        subgroup = ശൂന്യ;
                അല്ലെങ്കിൽ {
                        subgroup = strdup(e);
                        എങ്കിൽ (!subgroup)
                                തിരികെ -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        എങ്കിൽ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_path_get_unit_path(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *path_copy = ശൂന്യ;
        അക്ഷരം *unit_name;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        path_copy = strdup(path);
        എങ്കിൽ (!path_copy)
                തിരികെ -ENOMEM;

        unit_name = (അക്ഷരം*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        എങ്കിൽ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                തിരികെ -ENXIO;

        *ret = TAKE_PTR(path_copy);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_pid_get_unit_full(pid_t pid, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup) {
        പൂർണ്ണസംഖ്യ r;

        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

പൂർണ്ണസംഖ്യ cg_pidref_get_unit_full(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        _cleanup_free_ അക്ഷരം *unit = ശൂന്യ, *subgroup = ശൂന്യ;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        എങ്കിൽ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* skip_session(സ്ഥിരം അക്ഷരം *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        എങ്കിൽ (isempty(p))
                തിരികെ ശൂന്യ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        എങ്കിൽ (n < STRLEN("session-x.scope"))
                തിരികെ ശൂന്യ;

        സ്ഥിരം അക്ഷരം *s = startswith(p, "session-");
        എങ്കിൽ (!s)
                തിരികെ ശൂന്യ;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        അക്ഷരം *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        എങ്കിൽ (!e)
                തിരികെ ശൂന്യ;
        *e = '\0';

        എങ്കിൽ (!session_id_valid(f))
                തിരികെ ശൂന്യ;

        തിരികെ skip_leading_slash(p + n);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* skip_user_manager(സ്ഥിരം അക്ഷരം *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        എങ്കിൽ (isempty(p))
                തിരികെ ശൂന്യ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        എങ്കിൽ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                തിരികെ ശൂന്യ;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ അക്ഷരം *unit_name = strndup(p, n);
        എങ്കിൽ (!unit_name)
                തിരികെ ശൂന്യ;

        _cleanup_free_ അക്ഷരം *i = ശൂന്യ;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        എങ്കിൽ (type != UNIT_NAME_INSTANCE)
                തിരികെ ശൂന്യ;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        എങ്കിൽ (!(startswith(unit_name, "user@") && parse_uid(i, ശൂന്യ) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                തിരികെ ശൂന്യ;

        തിരികെ skip_leading_slash(p + n);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* skip_user_prefix(സ്ഥിരം അക്ഷരം *path) {
        സ്ഥിരം അക്ഷരം *e, *t;

        പ്രതിജ്ഞാ(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        എങ്കിൽ (t)
                തിരികെ t;

        /* Alternatively skip the user session if it is in the path... */
        തിരികെ skip_session(e);
}

പൂർണ്ണസംഖ്യ cg_path_get_user_unit_full(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup) {
        സ്ഥിരം അക്ഷരം *t;

        പ്രതിജ്ഞാ(path);

        t = skip_user_prefix(path);
        എങ്കിൽ (!t)
                തിരികെ -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        തിരികെ cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

പൂർണ്ണസംഖ്യ cg_pid_get_user_unit_full(pid_t pid, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup) {
        പൂർണ്ണസംഖ്യ r;

        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

പൂർണ്ണസംഖ്യ cg_pidref_get_user_unit_full(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret_unit, അക്ഷരം **ret_subgroup) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        _cleanup_free_ അക്ഷരം *unit = ശൂന്യ, *subgroup = ശൂന്യ;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        എങ്കിൽ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_path_get_machine_name(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_machine) {
        _cleanup_free_ അക്ഷരം *u = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *sl;
        പൂർണ്ണസംഖ്യ r;

        r = cg_path_get_unit(path, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        തിരികെ readlink_malloc(sl, ret_machine);
}

പൂർണ്ണസംഖ്യ cg_pid_get_machine_name(pid_t pid, അക്ഷരം **ret_machine) {
        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_machine_name(cgroup, ret_machine);
}

പൂർണ്ണസംഖ്യ cg_path_get_session(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_session) {
        _cleanup_free_ അക്ഷരം *unit = ശൂന്യ;
        അക്ഷരം *start, *end;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = cg_path_get_unit(path, &unit);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        start = startswith(unit, "session-");
        എങ്കിൽ (!start)
                തിരികെ -ENXIO;
        end = endswith(start, ".scope");
        എങ്കിൽ (!end)
                തിരികെ -ENXIO;

        *end = 0;
        എങ്കിൽ (!session_id_valid(start))
                തിരികെ -ENXIO;

        എങ്കിൽ (!ret_session)
                തിരികെ 0;

        തിരികെ strdup_to(ret_session, start);
}

പൂർണ്ണസംഖ്യ cg_pid_get_session(pid_t pid, അക്ഷരം **ret_session) {
        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_session(cgroup, ret_session);
}

പൂർണ്ണസംഖ്യ cg_pidref_get_session(സ്ഥിരം PidRef *pidref, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        _cleanup_free_ അക്ഷരം *session = ശൂന്യ;
        r = cg_pid_get_session(pidref->pid, &session);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(session);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_path_get_owner_uid(സ്ഥിരം അക്ഷരം *path, uid_t *ret_uid) {
        _cleanup_free_ അക്ഷരം *slice = ശൂന്യ;
        അക്ഷരം *start, *end;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = cg_path_get_slice(path, &slice);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        start = startswith(slice, "user-");
        എങ്കിൽ (!start)
                തിരികെ -ENXIO;

        end = endswith(start, ".slice");
        എങ്കിൽ (!end)
                തിരികെ -ENXIO;

        *end = 0;
        എങ്കിൽ (parse_uid(start, ret_uid) < 0)
                തിരികെ -ENXIO;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_owner_uid(cgroup, ret_uid);
}

പൂർണ്ണസംഖ്യ cg_pidref_get_owner_uid(സ്ഥിരം PidRef *pidref, uid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!pidref_is_set(pidref))
                തിരികെ -ESRCH;
        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = pidref_verify(pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret)
                *ret = uid;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_path_get_slice(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret_slice) {
        സ്ഥിരം അക്ഷരം *e = ശൂന്യ;

        പ്രതിജ്ഞാ(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *s;
                പൂർണ്ണസംഖ്യ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                എങ്കിൽ (n < 0)
                        തിരികെ n;
                എങ്കിൽ (!valid_slice_name(s, n))
                        നിർത്തുക;

                e = s;
        }

        എങ്കിൽ (e)
                തിരികെ cg_path_decode_unit(e, ret_slice);

        എങ്കിൽ (ret_slice)
                തിരികെ strdup_to(ret_slice, SPECIAL_വേര്_SLICE);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_pid_get_slice(pid_t pid, അക്ഷരം **ret_slice) {
        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_slice(cgroup, ret_slice);
}

പൂർണ്ണസംഖ്യ cg_path_get_user_slice(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret_slice) {
        സ്ഥിരം അക്ഷരം *t;
        പ്രതിജ്ഞാ(p);

        t = skip_user_prefix(p);
        എങ്കിൽ (!t)
                തിരികെ -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        തിരികെ cg_path_get_slice(t, ret_slice);
}

പൂർണ്ണസംഖ്യ cg_pid_get_user_slice(pid_t pid, അക്ഷരം **ret_slice) {
        _cleanup_free_ അക്ഷരം *cgroup = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_pid_get_path_shifted(pid, ശൂന്യ, &cgroup);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(സ്ഥിരം അക്ഷരം *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        എങ്കിൽ (!filename_is_valid(p))
                തിരികെ true;

        എങ്കിൽ (IN_SET(p[0], '_', '.'))
                തിരികെ true;

        എങ്കിൽ (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                തിരികെ true;

        എങ്കിൽ (startswith(p, "cgroup."))
                തിരികെ true;

        വേണ്ടി (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                സ്ഥിരം അക്ഷരം *q;

                q = startswith(p, cgroup_controller_to_string(c));
                എങ്കിൽ (!q)
                        തുടരുക;

                എങ്കിൽ (q[0] == '.')
                        തിരികെ true;
        }

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ cg_escape(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *n = ശൂന്യ;

        പ്രതിജ്ഞാ(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        എങ്കിൽ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                എങ്കിൽ (!n)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        തിരികെ -EINVAL;
        } അല്ലെങ്കിൽ {
                n = strdup(p);
                എങ്കിൽ (!n)
                        തിരികെ -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        തിരികെ 0;
}

അക്ഷരം* cg_unescape(സ്ഥിരം അക്ഷരം *p) {
        പ്രതിജ്ഞാ(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        എങ്കിൽ (p[0] == '_')
                തിരികെ (അക്ഷരം*) p+1;

        തിരികെ (അക്ഷരം*) p;
}

പൂർണ്ണസംഖ്യ cg_slice_to_path(സ്ഥിരം അക്ഷരം *unit, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *s = ശൂന്യ, *e = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *dash;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(unit);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (streq(unit, SPECIAL_വേര്_SLICE))
                തിരികെ strdup_to(ret, "");

        എങ്കിൽ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                തിരികെ -EINVAL;

        എങ്കിൽ (!endswith(unit, ".slice"))
                തിരികെ -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        എങ്കിൽ (dash == p)
                തിരികെ -EINVAL;

        ആകുമ്പോൾ (dash) {
                _cleanup_free_ അക്ഷരം *escaped = ശൂന്യ;
                അക്ഷരം n[dash - p + വലുപ്പം(".slice")];

#എങ്കിൽ HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                എങ്കിൽ (IN_SET(dash[1], 0, '-'))
                        തിരികെ -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                എങ്കിൽ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        തിരികെ -EINVAL;

                r = cg_escape(n, &escaped);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (!strextend(&s, escaped, "/"))
                        തിരികെ -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!strextend(&s, e))
                തിരികെ -ENOMEM;

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_is_threaded(സ്ഥിരം അക്ഷരം *path) {
        _cleanup_free_ അക്ഷരം *fs = ശൂന്യ, *contents = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **v = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_get_path(path, "cgroup.type", &fs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = read_full_virtual_file(fs, &contents, ശൂന്യ);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ false; /* Assume no. */
        എങ്കിൽ (r < 0)
                തിരികെ r;

        v = strv_split(contents, ശൂന്യ);
        എങ്കിൽ (!v)
                തിരികെ -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        തിരികെ strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

പൂർണ്ണസംഖ്യ cg_set_attribute(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(attribute);

        r = cg_get_path(path, attribute, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        തിരികെ write_string_file(p, മൂല്യം, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

പൂർണ്ണസംഖ്യ cg_get_attribute(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(attribute);

        r = cg_get_path(path, attribute, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ read_one_വരി_file(p, ret);
}

പൂർണ്ണസംഖ്യ cg_get_attribute_as_uint64(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        uint64_t v;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = cg_get_attribute(path, attribute, &മൂല്യം);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ENODATA;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (streq(മൂല്യം, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                തിരികെ 0;
        }

        r = safe_atou64(മൂല്യം, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = v;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_get_attribute_as_bool(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_get_attribute(path, attribute, &മൂല്യം);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -ENODATA;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_boolean(മൂല്യം);
}

പൂർണ്ണസംഖ്യ cg_get_owner(സ്ഥിരം അക്ഷരം *path, uid_t *ret_uid) {
        _cleanup_free_ അക്ഷരം *f = ശൂന്യ;
        ഘടന stat stats;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_uid);

        r = cg_get_path(path, /* suffix= */ ശൂന്യ, &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (stat(f, &stats) < 0)
                തിരികെ -errno;

        r = stat_verify_directory(&stats);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret_uid = stats.st_uid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_get_keyed_attribute(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *attribute,
                അക്ഷരം * സ്ഥിരം *താക്കോലുകൾ,
                അക്ഷരം **values) {

        _cleanup_free_ അക്ഷരം *filename = ശൂന്യ, *contents = ശൂന്യ;
        size_t n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = read_full_file(filename, &contents, /* ret_size= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        n = strv_length(താക്കോലുകൾ);
        എങ്കിൽ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                തിരികെ 0;
        പ്രതിജ്ഞാ(strv_is_uniq(താക്കോലുകൾ));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        അക്ഷരം **v = newa0(അക്ഷരം*, n);
        size_t n_done = 0;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = contents; *p;) {
                സ്ഥിരം അക്ഷരം *w;
                size_t i;

                വേണ്ടി (i = 0; i < n; i++) {
                        w = first_word(p, താക്കോലുകൾ[i]);
                        എങ്കിൽ (w)
                                നിർത്തുക;
                }

                എങ്കിൽ (w) {
                        എങ്കിൽ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                പോകുക fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        എങ്കിൽ (!v[i]) {
                                r = -ENOMEM;
                                പോകുക fail;
                        }

                        n_done++;
                        എങ്കിൽ (n_done >= n)
                                നിർത്തുക;

                        p = w + l;
                } അല്ലെങ്കിൽ
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        എങ്കിൽ (n_done < n) {
                r = -ENXIO;
                പോകുക fail;
        }

        memcpy(values, v, വലുപ്പം(അക്ഷരം*) * n);
        തിരികെ 0;

fail:
        free_many_charp(v, n);
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ cg_get_keyed_attribute_uint64(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *attribute, സ്ഥിരം അക്ഷരം *താക്കോൽ, uint64_t *ret) {
        _cleanup_free_ അക്ഷരം *val = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(താക്കോൽ);
        പ്രതിജ്ഞാ(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(താക്കോൽ), &val);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = safe_atou64(val, ret);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, താക്കോൽ, attribute);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_mask_to_string(CGroupMask mask, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (mask == 0) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        വേണ്ടി (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                സ്ഥിരം അക്ഷരം *k;
                size_t l;

                എങ്കിൽ (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        തുടരുക;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                എങ്കിൽ (!GREEDY_REALLOC(s, n + space + l + 1))
                        തിരികെ -ENOMEM;

                എങ്കിൽ (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        പ്രതിജ്ഞാ(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_mask_from_string(സ്ഥിരം അക്ഷരം *s, CGroupMask *ret) {
        CGroupMask m = 0;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(s);

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *n = ശൂന്യ;
                CGroupController v;
                പൂർണ്ണസംഖ്യ r;

                r = extract_first_word(&s, &n, ശൂന്യ, 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                v = cgroup_controller_from_string(n);
                എങ്കിൽ (v < 0)
                        തുടരുക;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_mask_supported_subtree(സ്ഥിരം അക്ഷരം *വേര്, CGroupMask *ret) {
        CGroupMask mask;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ അക്ഷരം *controllers = ശൂന്യ, *path = ശൂന്യ;
        r = cg_get_path(വേര്, "cgroup.controllers", &path);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = read_one_വരി_file(path, &controllers);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = cg_mask_from_string(controllers, &mask);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ അക്ഷരം *വേര് = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = cg_get_വേര്_path(&വേര്);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cg_mask_supported_subtree(വേര്, ret);
}

പൂർണ്ണസംഖ്യ cg_is_delegated(സ്ഥിരം അക്ഷരം *path) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        എങ്കിൽ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                തിരികെ r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        തിരികെ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

പൂർണ്ണസംഖ്യ cg_is_delegated_fd(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ശൂന്യ, "trusted.delegate", /* at_flags= */ 0);
        എങ്കിൽ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                തിരികെ r;

        r = getxattr_at_bool(fd, /* path= */ ശൂന്യ, "user.delegate", /* at_flags= */ 0);
        തിരികെ ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

പൂർണ്ണസംഖ്യ cg_has_coredump_receive(സ്ഥിരം അക്ഷരം *path) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        എങ്കിൽ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                തിരികെ false;

        തിരികെ r;
}

സ്ഥിരം uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

ശൂന്യം cgroup_io_limits_list(ശൂന്യം) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
