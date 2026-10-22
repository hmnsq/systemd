/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/fs.h>
#అన్తర్భావ <linux/magic.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/xattr.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "capsule-util.h"
#అన్తర్భావ "cgroup-util.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "login-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "set.h"
#అన్తర్భావ "special.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "unaligned.h"
#అన్తర్భావ "unit-name.h"
#అన్తర్భావ "user-util.h"
#అన్తర్భావ "xattr-util.h"

పూర్ణాంకం cg_is_అందుబాటు(శూన్య) {
        నిర్మాణం statfs fs;

        ఒకవేళ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ఒకవేళ (errno == ENOENT) /* sysfs not mounted? */
                        తిరిగి false;

                తిరిగి log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        తిరిగి is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

పూర్ణాంకం cg_మార్గం_open(స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

పూర్ణాంకం cg_cgroupid_open(పూర్ణాంకం cgroupfs_fd, uint64_t id) {
        _cleanup_close_ పూర్ణాంకం fsfd = -EBADF;

        ఒకవేళ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ఒకవేళ (fsfd < 0)
                        తిరిగి -errno;

                cgroupfs_fd = fsfd;
        }

        సంఘం {
                నిర్మాణం file_handle file_handle;
                uint8_t space[offsetof(నిర్మాణం file_handle, f_handle) + పరిమాణం(uint64_t)];
        } fh = {
                .file_handle.handle_బైట్లు = పరిమాణం(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        తిరిగి RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

పూర్ణాంకం cg_మార్గం_from_cgroupid(పూర్ణాంకం cgroupfs_fd, uint64_t id, అక్షరం **ret) {
        _cleanup_close_ పూర్ణాంకం cgfd = -EBADF;
        పూర్ణాంకం r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ఒకవేళ (cgfd < 0)
                తిరిగి cgfd;

        _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;
        r = fd_get_మార్గం(cgfd, &మార్గం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!మార్గం_startswith(మార్గం, "/sys/fs/cgroup/"))
                తిరిగి -EXDEV; /* recognizable error */

        ఒకవేళ (ret)
                *ret = TAKE_PTR(మార్గం);
        తిరిగి 0;
}

పూర్ణాంకం cg_enumerate_processes(స్థిరరాశి అక్షరం *మార్గం, FILE **ret) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        FILE *f;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = cg_get_మార్గం(మార్గం, "cgroup.procs", &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        f = fopen(fs, "re");
        ఒకవేళ (!f)
                తిరిగి -errno;

        *ret = f;
        తిరిగి 0;
}

పూర్ణాంకం cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        గుర్తులేని దీర్ఘ ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        కొరకు (;;) {
                errno = 0;
                ఒకవేళ (fscanf(f, "%lu", &ul) != 1) {

                        ఒకవేళ (feof(f)) {
                                *ret = 0;
                                తిరిగి 0;
                        }

                        తిరిగి errno_or_else(EIO);
                }

                ఒకవేళ (ul > PID_T_MAX)
                        తిరిగి -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ఒకవేళ (ul == 0 && !FLAGS_సమితి(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        కొనసాగు;

                *ret = (pid_t) ul;
                తిరిగి 1;
        }
}

పూర్ణాంకం cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(ret);

        కొరకు (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ఒకవేళ (r == 0) {
                        *ret = PIDREF_NULL;
                        తిరిగి 0;
                }

                ఒకవేళ (pid == 0)
                        తిరిగి -EREMOTE;

                r = pidref_సమితి_pid(ret, pid);
                ఒకవేళ (r >= 0)
                        తిరిగి 1;
                ఒకవేళ (r != -ESRCH)
                        తిరిగి r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(శూన్య) {
        స్థిర thread_local పూర్ణాంకం supported = -1;

        ఒకవేళ (supported >= 0)
                తిరిగి supported;

        ఒకవేళ (cg_is_అందుబాటు() <= 0)
                తిరిగి (supported = false);

        ఒకవేళ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                తిరిగి (supported = true);
        ఒకవేళ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        తిరిగి (supported = false);
}

పూర్ణాంకం cg_enumerate_subgroups(స్థిరరాశి అక్షరం *మార్గం, DIR **ret) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        DIR *d;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* This is not recursive! */

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        d = opendir(fs);
        ఒకవేళ (!d)
                తిరిగి -errno;

        *ret = d;
        తిరిగి 0;
}

పూర్ణాంకం cg_read_subgroup(DIR *d, అక్షరం **ret) {
        ప్రతిజ్ఞా(d);
        ప్రతిజ్ఞా(ret);

        FOREACH_DIRENT_ALL(de, d, తిరిగి -errno) {
                ఒకవేళ (de->d_type != DT_DIR)
                        కొనసాగు;

                ఒకవేళ (dot_or_dot_dot(de->d_name))
                        కొనసాగు;

                తిరిగి strdup_గమ్యం_నిండిన(ret, de->d_name);
        }

        *ret = ఖాళీ;
        తిరిగి 0;
}

పూర్ణాంకం cg_kill(
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం sig,
                CGroupFlags flags,
                సమితి_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                శూన్య *userdata) {

        cleanup_సమితి_free సమితి_2 *allocated_సమితి = ఖాళీ;
        పూర్ణాంకం r, ret = 0;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ఒకవేళ (లోపల_సమితి(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ఒకవేళ (!killed_pids) {
                killed_pids = allocated_సమితి = సమితి_new(ఖాళీ);
                ఒకవేళ (!killed_pids)
                        తిరిగి -ENOMEM;
        }

        bool పూర్తి;
        చేయి {
                _cleanup_fclose_ FILE *f = ఖాళీ;
                పూర్ణాంకం ret_log_kill;

                పూర్తి = true;

                r = cg_enumerate_processes(మార్గం, &f);
                ఒకవేళ (r == -ENOENT)
                        ఆపు;
                ఒకవేళ (r < 0)
                        తిరిగి RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                కొరకు (;;) {
                        _cleanup_(pidref_పూర్తి) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ఒకవేళ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                పూర్తి = true;
                                ఆపు;
                        }
                        ఒకవేళ (r < 0)
                                తిరిగి RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", మార్గం));
                        ఒకవేళ (r == 0)
                                ఆపు;

                        ఒకవేళ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                కొనసాగు;

                        ఒకవేళ (సమితి_కలిగియుంది(killed_pids, PID_గమ్యం_PTR(pidref.pid)))
                                కొనసాగు;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ఒకవేళ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, మార్గం);
                                కొనసాగు;
                        }

                        ఒకవేళ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ఒకవేళ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, మార్గం));
                        ఒకవేళ (r >= 0) {
                                ఒకవేళ (flags & CGROUP_SIGCONT)
                                        (శూన్య) pidref_kill(&pidref, SIGCONT);

                                ఒకవేళ (ret == 0) {
                                        ఒకవేళ (log_kill)
                                                ret = ret_log_kill;
                                        లేకపోతే
                                                ret = 1;
                                }
                        }

                        పూర్తి = false;

                        r = సమితి_put(killed_pids, PID_గమ్యం_PTR(pidref.pid));
                        ఒకవేళ (r < 0)
                                తిరిగి RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } అయితే (!పూర్తి);

        తిరిగి ret;
}

పూర్ణాంకం cg_kill_recursive(
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం sig,
                CGroupFlags flags,
                సమితి_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                శూన్య *userdata) {

        cleanup_సమితి_free సమితి_2 *allocated_సమితి = ఖాళీ;
        _cleanup_closedir_ DIR *d = ఖాళీ;
        పూర్ణాంకం r, ret;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(sig >= 0);

        ఒకవేళ (!killed_pids) {
                killed_pids = allocated_సమితి = సమితి_new(ఖాళీ);
                ఒకవేళ (!killed_pids)
                        తిరిగి -ENOMEM;
        }

        ret = cg_kill(మార్గం, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(మార్గం, &d);
        ఒకవేళ (r < 0) {
                ఒకవేళ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", మార్గం));

                తిరిగి ret;
        }

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *fn = ఖాళీ, *p = ఖాళీ;

                r = cg_read_subgroup(d, &fn);
                ఒకవేళ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", మార్గం));
                        ఆపు;
                }
                ఒకవేళ (r == 0)
                        ఆపు;

                p = మార్గం_join(ఖాళీలు_గమ్యం_మూలం(మార్గం), fn);
                ఒకవేళ (!p)
                        తిరిగి -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ఒకవేళ (r != 0 && ret >= 0)
                        ret = r;
        }

        తిరిగి ret;
}

పూర్ణాంకం cg_kill_kernel_sigkill(స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ అక్షరం *killfile = ఖాళీ;
        uint64_t n_pids = UINT64_MAX;
        పూర్ణాంకం r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        ప్రతిజ్ఞా(మార్గం);

        ఒకవేళ (!cg_kill_supported())
                తిరిగి -EOPNOTSUPP;

        r = cg_get_మార్గం(మార్గం, "cgroup.kill", &killfile);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(మార్గం, "pids.current", &n_pids);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", మార్గం);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_బఫరు);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", మార్గం);

        ఒకవేళ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        తిరిగి 0;
}

పూర్ణాంకం cg_get_మార్గం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret) {
        అక్షరం *t;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (isempty(మార్గం))
                మార్గం = TAKE_PTR(suffix);

        t = మార్గం_join("/sys/fs/cgroup", మార్గం, suffix);
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        *ret = మార్గం_simplify(t);
        తిరిగి 0;
}

పూర్ణాంకం cg_సమితి_xattr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, స్థిరరాశి శూన్య *విలువ, size_t కొలత, పూర్ణాంకం flags) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(విలువ || కొలత <= 0);

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి RET_NERRNO(setxattr(fs, name, విలువ, కొలత, flags));
}

పూర్ణాంకం cg_get_xattr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, అక్షరం **ret, size_t *ret_కొలత) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(name);

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి lgetxattr_malloc(fs, name, ret, ret_కొలత);
}

పూర్ణాంకం cg_get_xattr_bool(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(name);

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

పూర్ణాంకం cg_remove_xattr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(name);

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి RET_NERRNO(removexattr(fs, name));
}

పూర్ణాంకం cg_pid_get_మార్గం(pid_t pid, అక్షరం **ret_మార్గం) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        స్థిరరాశి అక్షరం *fs;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret_మార్గం);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ESRCH;
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                అక్షరం *e;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి -ENODATA;

                e = startswith(పంక్తి, "0:");
                ఒకవేళ (!e)
                        కొనసాగు;

                e = strchr(e, ':');
                ఒకవేళ (!e)
                        కొనసాగు;

                _cleanup_free_ అక్షరం *మార్గం = strdup(e + 1);
                ఒకవేళ (!మార్గం)
                        తిరిగి -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ఒకవేళ (startswith(మార్గం, "/../"))
                        తిరిగి -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(మార్గం, " (deleted)");
                ఒకవేళ (e)
                        *e = 0;

                *ret_మార్గం = TAKE_PTR(మార్గం);
                తిరిగి 0;
        }
}

పూర్ణాంకం cg_pidref_get_మార్గం(స్థిరరాశి PidRef *pidref, అక్షరం **ret_మార్గం) {
        _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_మార్గం);

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_మార్గం(pidref->pid, &మార్గం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret_మార్గం = TAKE_PTR(మార్గం);
        తిరిగి 0;
}

పూర్ణాంకం cg_is_ఖాళీలు(స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        ప్రతిజ్ఞా(మార్గం);

        /* The root cgroup is always populated */
        ఒకవేళ (ఖాళీలు_or_మూలం(మార్గం))
                తిరిగి false;

        r = cg_get_keyed_attribute(మార్గం, "cgroup.events", STRV_MAKE("populated"), &t);
        ఒకవేళ (r == -ENOENT)
                తిరిగి true;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి streq(t, "0");
}

పూర్ణాంకం cg_విభజించు_spec(స్థిరరాశి అక్షరం *spec, అక్షరం **ret_controller, అక్షరం **ret_మార్గం) {
        _cleanup_free_ అక్షరం *controller = ఖాళీ;
        స్థిరరాశి అక్షరం *మార్గం;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ఒకవేళ (isempty(spec) || మార్గం_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                మార్గం = spec;
                వెళ్ళు finalize;
        }

        స్థిరరాశి అక్షరం *e = strchr(spec, ':');
        ఒకవేళ (!e) {
                /* Controller only. */
                ఒకవేళ (ret_controller) {
                        controller = strdup(spec);
                        ఒకవేళ (!controller)
                                తిరిగి -ENOMEM;
                }

                మార్గం = ఖాళీ;
        } లేకపోతే {
                /* Both controller and path. */
                ఒకవేళ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ఒకవేళ (!controller)
                                తిరిగి -ENOMEM;
                }

                మార్గం = e + 1;
        }

finalize:
        మార్గం = ఖాళీలు_గమ్యం_null(మార్గం);

        ఒకవేళ (మార్గం) {
                /* Non-empty path must be absolute. */
                ఒకవేళ (!మార్గం_is_absolute(మార్గం))
                        తిరిగి -EINVAL;

                /* Path must not contain dot-dot. */
                ఒకవేళ (!మార్గం_is_సురక్షిత(మార్గం))
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (ret_మార్గం) {
                r = మార్గం_simplify_alloc(మార్గం, ret_మార్గం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        తిరిగి 0;
}

పూర్ణాంకం cg_get_మూలం_మార్గం(అక్షరం **ret_మార్గం) {
        అక్షరం *p, *e;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_మార్గం);

        r = cg_pid_get_మార్గం(1, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ఒకవేళ (e)
                *e = 0;

        *ret_మార్గం = p;
        తిరిగి 0;
}

పూర్ణాంకం cg_జరుపు_మార్గం(స్థిరరాశి అక్షరం *cgroup, స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం **ret_shifted) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(cgroup);
        ప్రతిజ్ఞా(ret_shifted);

        _cleanup_free_ అక్షరం *rt = ఖాళీ;
        ఒకవేళ (!మూలం) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_మూలం_మార్గం(&rt);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                మూలం = rt;
        }

        *ret_shifted = మార్గం_startswith_నిండిన(cgroup, మూలం, మార్గం_STARTSWITH_RETURN_LEADING_SLASH|మార్గం_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        తిరిగి 0;
}

పూర్ణాంకం cg_pid_get_మార్గం_shifted(pid_t pid, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret_cgroup) {
        _cleanup_free_ అక్షరం *raw = ఖాళీ;
        స్థిరరాశి అక్షరం *c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret_cgroup);

        r = cg_pid_get_మార్గం(pid, &raw);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = cg_జరుపు_మార్గం(raw, మూలం, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                తిరిగి 0;
        }

        తిరిగి strdup_గమ్యం(ret_cgroup, c);
}

పూర్ణాంకం cg_pidref_get_మార్గం_shifted(స్థిరరాశి PidRef *pidref, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret_cgroup) {
        _cleanup_free_ అక్షరం *raw = ఖాళీ;
        స్థిరరాశి అక్షరం *c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_cgroup);

        r = cg_pidref_get_మార్గం(pidref, &raw);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = cg_జరుపు_మార్గం(raw, మూలం, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                తిరిగి 0;
        }

        తిరిగి strdup_గమ్యం(ret_cgroup, c);
}

పూర్ణాంకం cg_మార్గం_decode_unit(స్థిరరాశి అక్షరం *cgroup, అక్షరం **ret_unit) {
        ప్రతిజ్ఞా(cgroup);

        size_t n = strcspn(cgroup, "/");
        ఒకవేళ (n < 3)
                తిరిగి -ENXIO;

        అక్షరం *c = strndupa_సురక్షిత(cgroup, n);
        c = cg_unescape(c);

        ఒకవేళ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                తిరిగి -ENXIO;

        ఒకవేళ (ret_unit)
                తిరిగి strdup_గమ్యం(ret_unit, c);

        తిరిగి 0;
}

స్థిర bool valid_ముక్క_name(స్థిరరాశి అక్షరం *p, size_t n) {
        ప్రతిజ్ఞా(p || n == 0);

        ఒకవేళ (n < STRLEN("x.slice"))
                తిరిగి false;

        అక్షరం *c = strndupa_సురక్షిత(p, n);
        ఒకవేళ (!endswith(c, ".slice"))
                తిరిగి false;

        తిరిగి unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

స్థిర స్థిరరాశి అక్షరం* skip_slices(స్థిరరాశి అక్షరం *p) {
        ప్రతిజ్ఞా(p);

        /* Skips over all slice assignments */

        కొరకు (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ఒకవేళ (!valid_ముక్క_name(p, n))
                        తిరిగి p;

                p += n;
        }
}

పూర్ణాంకం cg_మార్గం_get_unit_నిండిన(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit, అక్షరం **ret_subgroup) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        స్థిరరాశి అక్షరం *e = skip_slices(మార్గం);

        _cleanup_free_ అక్షరం *unit = ఖాళీ;
        r = cg_మార్గం_decode_unit(e, &unit);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* We skipped over the slices, don't accept any now */
        ఒకవేళ (endswith(unit, ".slice"))
                తిరిగి -ENXIO;

        ఒకవేళ (ret_subgroup) {
                _cleanup_free_ అక్షరం *subgroup = ఖాళీ;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ఒకవేళ (isempty(e))
                        subgroup = ఖాళీ;
                లేకపోతే {
                        subgroup = strdup(e);
                        ఒకవేళ (!subgroup)
                                తిరిగి -ENOMEM;
                }

                మార్గం_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ఒకవేళ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        తిరిగి 0;
}

పూర్ణాంకం cg_మార్గం_get_unit_మార్గం(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *మార్గం_copy = ఖాళీ;
        అక్షరం *unit_name;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        మార్గం_copy = strdup(మార్గం);
        ఒకవేళ (!మార్గం_copy)
                తిరిగి -ENOMEM;

        unit_name = (అక్షరం*) skip_slices(మార్గం_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ఒకవేళ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                తిరిగి -ENXIO;

        *ret = TAKE_PTR(మార్గం_copy);

        తిరిగి 0;
}

పూర్ణాంకం cg_pid_get_unit_నిండిన(pid_t pid, అక్షరం **ret_unit, అక్షరం **ret_subgroup) {
        పూర్ణాంకం r;

        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_unit_నిండిన(cgroup, ret_unit, ret_subgroup);
}

పూర్ణాంకం cg_pidref_get_unit_నిండిన(స్థిరరాశి PidRef *pidref, అక్షరం **ret_unit, అక్షరం **ret_subgroup) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        _cleanup_free_ అక్షరం *unit = ఖాళీ, *subgroup = ఖాళీ;
        r = cg_pid_get_unit_నిండిన(pidref->pid, &unit, &subgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ఒకవేళ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        తిరిగి 0;
}

స్థిర స్థిరరాశి అక్షరం* skip_session(స్థిరరాశి అక్షరం *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ఒకవేళ (isempty(p))
                తిరిగి ఖాళీ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ఒకవేళ (n < STRLEN("session-x.scope"))
                తిరిగి ఖాళీ;

        స్థిరరాశి అక్షరం *s = startswith(p, "session-");
        ఒకవేళ (!s)
                తిరిగి ఖాళీ;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        అక్షరం *f = strndupa_సురక్షిత(s, p + n - s),
             *e = endswith(f, ".scope");
        ఒకవేళ (!e)
                తిరిగి ఖాళీ;
        *e = '\0';

        ఒకవేళ (!session_id_valid(f))
                తిరిగి ఖాళీ;

        తిరిగి skip_leading_slash(p + n);
}

స్థిర స్థిరరాశి అక్షరం* skip_user_manager(స్థిరరాశి అక్షరం *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ఒకవేళ (isempty(p))
                తిరిగి ఖాళీ;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ఒకవేళ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                తిరిగి ఖాళీ;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ అక్షరం *unit_name = strndup(p, n);
        ఒకవేళ (!unit_name)
                తిరిగి ఖాళీ;

        _cleanup_free_ అక్షరం *i = ఖాళీ;
        UnitNameFlags type = unit_name_గమ్యం_instance(unit_name, &i);

        ఒకవేళ (type != UNIT_NAME_INSTANCE)
                తిరిగి ఖాళీ;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ఒకవేళ (!(startswith(unit_name, "user@") && parse_uid(i, ఖాళీ) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                తిరిగి ఖాళీ;

        తిరిగి skip_leading_slash(p + n);
}

స్థిర స్థిరరాశి అక్షరం* skip_user_ఉపసర్గ(స్థిరరాశి అక్షరం *మార్గం) {
        స్థిరరాశి అక్షరం *e, *t;

        ప్రతిజ్ఞా(మార్గం);

        /* Skip slices, if there are any */
        e = skip_slices(మార్గం);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ఒకవేళ (t)
                తిరిగి t;

        /* Alternatively skip the user session if it is in the path... */
        తిరిగి skip_session(e);
}

పూర్ణాంకం cg_మార్గం_get_user_unit_నిండిన(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_unit, అక్షరం **ret_subgroup) {
        స్థిరరాశి అక్షరం *t;

        ప్రతిజ్ఞా(మార్గం);

        t = skip_user_ఉపసర్గ(మార్గం);
        ఒకవేళ (!t)
                తిరిగి -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        తిరిగి cg_మార్గం_get_unit_నిండిన(t, ret_unit, ret_subgroup);
}

పూర్ణాంకం cg_pid_get_user_unit_నిండిన(pid_t pid, అక్షరం **ret_unit, అక్షరం **ret_subgroup) {
        పూర్ణాంకం r;

        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_user_unit_నిండిన(cgroup, ret_unit, ret_subgroup);
}

పూర్ణాంకం cg_pidref_get_user_unit_నిండిన(స్థిరరాశి PidRef *pidref, అక్షరం **ret_unit, అక్షరం **ret_subgroup) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        _cleanup_free_ అక్షరం *unit = ఖాళీ, *subgroup = ఖాళీ;
        r = cg_pid_get_user_unit_నిండిన(pidref->pid, &unit, &subgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ఒకవేళ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        తిరిగి 0;
}

పూర్ణాంకం cg_మార్గం_get_machine_name(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_machine) {
        _cleanup_free_ అక్షరం *u = ఖాళీ;
        స్థిరరాశి అక్షరం *sl;
        పూర్ణాంకం r;

        r = cg_మార్గం_get_unit(మార్గం, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        తిరిగి readlink_malloc(sl, ret_machine);
}

పూర్ణాంకం cg_pid_get_machine_name(pid_t pid, అక్షరం **ret_machine) {
        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_machine_name(cgroup, ret_machine);
}

పూర్ణాంకం cg_మార్గం_get_session(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_session) {
        _cleanup_free_ అక్షరం *unit = ఖాళీ;
        అక్షరం *ప్రారంభం, *చివర;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = cg_మార్గం_get_unit(మార్గం, &unit);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రారంభం = startswith(unit, "session-");
        ఒకవేళ (!ప్రారంభం)
                తిరిగి -ENXIO;
        చివర = endswith(ప్రారంభం, ".scope");
        ఒకవేళ (!చివర)
                తిరిగి -ENXIO;

        *చివర = 0;
        ఒకవేళ (!session_id_valid(ప్రారంభం))
                తిరిగి -ENXIO;

        ఒకవేళ (!ret_session)
                తిరిగి 0;

        తిరిగి strdup_గమ్యం(ret_session, ప్రారంభం);
}

పూర్ణాంకం cg_pid_get_session(pid_t pid, అక్షరం **ret_session) {
        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_session(cgroup, ret_session);
}

పూర్ణాంకం cg_pidref_get_session(స్థిరరాశి PidRef *pidref, అక్షరం **ret) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        _cleanup_free_ అక్షరం *session = ఖాళీ;
        r = cg_pid_get_session(pidref->pid, &session);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(session);
        తిరిగి 0;
}

పూర్ణాంకం cg_మార్గం_get_owner_uid(స్థిరరాశి అక్షరం *మార్గం, uid_t *ret_uid) {
        _cleanup_free_ అక్షరం *ముక్క = ఖాళీ;
        అక్షరం *ప్రారంభం, *చివర;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = cg_మార్గం_get_ముక్క(మార్గం, &ముక్క);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రారంభం = startswith(ముక్క, "user-");
        ఒకవేళ (!ప్రారంభం)
                తిరిగి -ENXIO;

        చివర = endswith(ప్రారంభం, ".slice");
        ఒకవేళ (!చివర)
                తిరిగి -ENXIO;

        *చివర = 0;
        ఒకవేళ (parse_uid(ప్రారంభం, ret_uid) < 0)
                తిరిగి -ENXIO;

        తిరిగి 0;
}

పూర్ణాంకం cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_owner_uid(cgroup, ret_uid);
}

పూర్ణాంకం cg_pidref_get_owner_uid(స్థిరరాశి PidRef *pidref, uid_t *ret) {
        పూర్ణాంకం r;

        ఒకవేళ (!pidref_is_సమితి(pidref))
                తిరిగి -ESRCH;
        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = pidref_verify(pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret)
                *ret = uid;

        తిరిగి 0;
}

పూర్ణాంకం cg_మార్గం_get_ముక్క(స్థిరరాశి అక్షరం *p, అక్షరం **ret_ముక్క) {
        స్థిరరాశి అక్షరం *e = ఖాళీ;

        ప్రతిజ్ఞా(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        కొరకు (;;) {
                స్థిరరాశి అక్షరం *s;
                పూర్ణాంకం n;

                n = మార్గం_కనుగొను_మొదటి_component(&p, /* accept_dot_dot= */ false, &s);
                ఒకవేళ (n < 0)
                        తిరిగి n;
                ఒకవేళ (!valid_ముక్క_name(s, n))
                        ఆపు;

                e = s;
        }

        ఒకవేళ (e)
                తిరిగి cg_మార్గం_decode_unit(e, ret_ముక్క);

        ఒకవేళ (ret_ముక్క)
                తిరిగి strdup_గమ్యం(ret_ముక్క, SPECIAL_మూలం_ముక్క);

        తిరిగి 0;
}

పూర్ణాంకం cg_pid_get_ముక్క(pid_t pid, అక్షరం **ret_ముక్క) {
        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_ముక్క(cgroup, ret_ముక్క);
}

పూర్ణాంకం cg_మార్గం_get_user_ముక్క(స్థిరరాశి అక్షరం *p, అక్షరం **ret_ముక్క) {
        స్థిరరాశి అక్షరం *t;
        ప్రతిజ్ఞా(p);

        t = skip_user_ఉపసర్గ(p);
        ఒకవేళ (!t)
                తిరిగి -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        తిరిగి cg_మార్గం_get_ముక్క(t, ret_ముక్క);
}

పూర్ణాంకం cg_pid_get_user_ముక్క(pid_t pid, అక్షరం **ret_ముక్క) {
        _cleanup_free_ అక్షరం *cgroup = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_pid_get_మార్గం_shifted(pid, ఖాళీ, &cgroup);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_మార్గం_get_user_ముక్క(cgroup, ret_ముక్క);
}

bool cg_needs_escape(స్థిరరాశి అక్షరం *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ఒకవేళ (!filename_is_valid(p))
                తిరిగి true;

        ఒకవేళ (లోపల_సమితి(p[0], '_', '.'))
                తిరిగి true;

        ఒకవేళ (STR_లోపల_సమితి(p, "notify_on_release", "release_agent", "tasks"))
                తిరిగి true;

        ఒకవేళ (startswith(p, "cgroup."))
                తిరిగి true;

        కొరకు (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                స్థిరరాశి అక్షరం *q;

                q = startswith(p, cgroup_controller_గమ్యం_string(c));
                ఒకవేళ (!q)
                        కొనసాగు;

                ఒకవేళ (q[0] == '.')
                        తిరిగి true;
        }

        తిరిగి false;
}

పూర్ణాంకం cg_escape(స్థిరరాశి అక్షరం *p, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *n = ఖాళీ;

        ప్రతిజ్ఞా(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ఒకవేళ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ఒకవేళ (!n)
                        తిరిగి -ENOMEM;

                ఒకవేళ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        తిరిగి -EINVAL;
        } లేకపోతే {
                n = strdup(p);
                ఒకవేళ (!n)
                        తిరిగి -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        తిరిగి 0;
}

అక్షరం* cg_unescape(స్థిరరాశి అక్షరం *p) {
        ప్రతిజ్ఞా(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ఒకవేళ (p[0] == '_')
                తిరిగి (అక్షరం*) p+1;

        తిరిగి (అక్షరం*) p;
}

పూర్ణాంకం cg_ముక్క_గమ్యం_మార్గం(స్థిరరాశి అక్షరం *unit, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ, *s = ఖాళీ, *e = ఖాళీ;
        స్థిరరాశి అక్షరం *dash;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(unit);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (streq(unit, SPECIAL_మూలం_ముక్క))
                తిరిగి strdup_గమ్యం(ret, "");

        ఒకవేళ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                తిరిగి -EINVAL;

        ఒకవేళ (!endswith(unit, ".slice"))
                తిరిగి -EINVAL;

        r = unit_name_గమ్యం_ఉపసర్గ(unit, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ఒకవేళ (dash == p)
                తిరిగి -EINVAL;

        అయితే (dash) {
                _cleanup_free_ అక్షరం *escaped = ఖాళీ;
                అక్షరం n[dash - p + పరిమాణం(".slice")];

#ఒకవేళ HAS_FEATURE_జ్ఞాపకం_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#అయితే_ముగింపు

                /* Don't allow trailing or double dashes */
                ఒకవేళ (లోపల_సమితి(dash[1], 0, '-'))
                        తిరిగి -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ఒకవేళ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        తిరిగి -EINVAL;

                r = cg_escape(n, &escaped);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (!strextend(&s, escaped, "/"))
                        తిరిగి -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!strextend(&s, e))
                తిరిగి -ENOMEM;

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం cg_is_threaded(స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_free_ అక్షరం *fs = ఖాళీ, *contents = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **v = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_get_మార్గం(మార్గం, "cgroup.type", &fs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = read_నిండిన_virtual_file(fs, &contents, ఖాళీ);
        ఒకవేళ (r == -ENOENT)
                తిరిగి false; /* Assume no. */
        ఒకవేళ (r < 0)
                తిరిగి r;

        v = strv_విభజించు(contents, ఖాళీ);
        ఒకవేళ (!v)
                తిరిగి -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        తిరిగి strv_కలిగియుంది(v, "threaded") || strv_కలిగియుంది(v, "invalid");
}

పూర్ణాంకం cg_సమితి_attribute(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, స్థిరరాశి అక్షరం *విలువ) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(attribute);

        r = cg_get_మార్గం(మార్గం, attribute, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        తిరిగి write_string_file(p, విలువ, WRITE_STRING_FILE_DISABLE_బఫరు|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

పూర్ణాంకం cg_get_attribute(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(attribute);

        r = cg_get_మార్గం(మార్గం, attribute, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి read_one_పంక్తి_file(p, ret);
}

పూర్ణాంకం cg_get_attribute_as_uint64(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, uint64_t *ret) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        uint64_t v;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = cg_get_attribute(మార్గం, attribute, &విలువ);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ENODATA;
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (streq(విలువ, "max")) {
                *ret = CGROUP_హద్దు_MAX;
                తిరిగి 0;
        }

        r = సురక్షిత_atou64(విలువ, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = v;
        తిరిగి 0;
}

పూర్ణాంకం cg_get_attribute_as_bool(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_get_attribute(మార్గం, attribute, &విలువ);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -ENODATA;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_boolean(విలువ);
}

పూర్ణాంకం cg_get_owner(స్థిరరాశి అక్షరం *మార్గం, uid_t *ret_uid) {
        _cleanup_free_ అక్షరం *f = ఖాళీ;
        నిర్మాణం stat stats;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_uid);

        r = cg_get_మార్గం(మార్గం, /* suffix= */ ఖాళీ, &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (stat(f, &stats) < 0)
                తిరిగి -errno;

        r = stat_verify_directory(&stats);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret_uid = stats.st_uid;
        తిరిగి 0;
}

పూర్ణాంకం cg_get_keyed_attribute(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *attribute,
                అక్షరం * స్థిరరాశి *కీలకాలు,
                అక్షరం **values) {

        _cleanup_free_ అక్షరం *filename = ఖాళీ, *contents = ఖాళీ;
        size_t n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_మార్గం(మార్గం, attribute, &filename);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = read_నిండిన_file(filename, &contents, /* ret_size= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        n = strv_నిడివి(కీలకాలు);
        ఒకవేళ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                తిరిగి 0;
        ప్రతిజ్ఞా(strv_is_uniq(కీలకాలు));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        అక్షరం **v = newa0(అక్షరం*, n);
        size_t n_పూర్తి = 0;

        కొరకు (స్థిరరాశి అక్షరం *p = contents; *p;) {
                స్థిరరాశి అక్షరం *w;
                size_t i;

                కొరకు (i = 0; i < n; i++) {
                        w = మొదటి_మాట(p, కీలకాలు[i]);
                        ఒకవేళ (w)
                                ఆపు;
                }

                ఒకవేళ (w) {
                        ఒకవేళ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                వెళ్ళు fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ఒకవేళ (!v[i]) {
                                r = -ENOMEM;
                                వెళ్ళు fail;
                        }

                        n_పూర్తి++;
                        ఒకవేళ (n_పూర్తి >= n)
                                ఆపు;

                        p = w + l;
                } లేకపోతే
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ఒకవేళ (n_పూర్తి < n) {
                r = -ENXIO;
                వెళ్ళు fail;
        }

        జ్ఞాపకం_నకలు(values, v, పరిమాణం(అక్షరం*) * n);
        తిరిగి 0;

fail:
        free_many_charp(v, n);
        తిరిగి r;
}

పూర్ణాంకం cg_get_keyed_attribute_uint64(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *attribute, స్థిరరాశి అక్షరం *కీలకం, uint64_t *ret) {
        _cleanup_free_ అక్షరం *val = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(కీలకం);
        ప్రతిజ్ఞా(ret);

        r = cg_get_keyed_attribute(మార్గం, attribute, STRV_MAKE(కీలకం), &val);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = సురక్షిత_atou64(val, ret);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, కీలకం, attribute);

        తిరిగి 0;
}

పూర్ణాంకం cg_ముసుగు_గమ్యం_string(CGroup_ముసుగు ముసుగు_3, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (ముసుగు_3 == 0) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        కొరకు (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                స్థిరరాశి అక్షరం *k;
                size_t l;

                ఒకవేళ (!FLAGS_సమితి(ముసుగు_3, CGROUP_CONTROLLER_గమ్యం_ముసుగు(c)))
                        కొనసాగు;

                k = cgroup_controller_గమ్యం_string(c);
                l = అక్షరశ్రేణి_పొడవు(k);

                ఒకవేళ (!GREEDY_REALLOC(s, n + space + l + 1))
                        తిరిగి -ENOMEM;

                ఒకవేళ (space)
                        s[n] = ' ';
                జ్ఞాపకం_నకలు(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        ప్రతిజ్ఞా(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        తిరిగి 0;
}

పూర్ణాంకం cg_ముసుగు_from_string(స్థిరరాశి అక్షరం *s, CGroup_ముసుగు *ret) {
        CGroup_ముసుగు m = 0;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(s);

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *n = ఖాళీ;
                CGroupController v;
                పూర్ణాంకం r;

                r = వెలికితీయి_మొదటి_మాట(&s, &n, ఖాళీ, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                v = cgroup_controller_from_string(n);
                ఒకవేళ (v < 0)
                        కొనసాగు;

                m |= CGROUP_CONTROLLER_గమ్యం_ముసుగు(v);
        }

        *ret = m;
        తిరిగి 0;
}

పూర్ణాంకం cg_ముసుగు_supported_subtree(స్థిరరాశి అక్షరం *మూలం, CGroup_ముసుగు *ret) {
        CGroup_ముసుగు ముసుగు_3;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ అక్షరం *controllers = ఖాళీ, *మార్గం = ఖాళీ;
        r = cg_get_మార్గం(మూలం, "cgroup.controllers", &మార్గం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = read_one_పంక్తి_file(మార్గం, &controllers);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = cg_ముసుగు_from_string(controllers, &ముసుగు_3);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Mask controllers that are not supported in cgroup v2. */
        ముసుగు_3 &= CGROUP_ముసుగు_V2;

        *ret = ముసుగు_3;
        తిరిగి 0;
}

పూర్ణాంకం cg_ముసుగు_supported(CGroup_ముసుగు *ret) {
        _cleanup_free_ అక్షరం *మూలం = ఖాళీ;
        పూర్ణాంకం r;

        r = cg_get_మూలం_మార్గం(&మూలం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cg_ముసుగు_supported_subtree(మూలం, ret);
}

పూర్ణాంకం cg_is_delegated(స్థిరరాశి అక్షరం *మార్గం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = cg_get_xattr_bool(మార్గం, "trusted.delegate");
        ఒకవేళ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                తిరిగి r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(మార్గం, "user.delegate");
        తిరిగి ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

పూర్ణాంకం cg_is_delegated_fd(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ ఖాళీ, "trusted.delegate", /* at_flags= */ 0);
        ఒకవేళ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                తిరిగి r;

        r = getxattr_at_bool(fd, /* path= */ ఖాళీ, "user.delegate", /* at_flags= */ 0);
        తిరిగి ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

పూర్ణాంకం cg_has_coredump_receive(స్థిరరాశి అక్షరం *మార్గం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = cg_get_xattr_bool(మార్గం, "user.coredump_receive");
        ఒకవేళ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                తిరిగి false;

        తిరిగి r;
}

స్థిరరాశి uint64_t cgroup_io_హద్దు_defaults[CGROUP_IO_హద్దు_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_హద్దు_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_హద్దు_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_హద్దు_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_హద్దు_MAX,
};

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి cgroup_io_హద్దు_type_పట్టిక[CGROUP_IO_హద్దు_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_పట్టిక_LOOKUP(cgroup_io_హద్దు_type, CGroupIOLimitType);

శూన్య cgroup_io_limits_list(శూన్య) {
        DUMP_STRING_పట్టిక(cgroup_io_హద్దు_type, CGroupIOLimitType, CGROUP_IO_హద్దు_TYPE_MAX);
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి cgroup_controller_పట్టిక[_CGROUP_CONTROLLER_MAX] = {
        [CGROUP_CONTROLLER_CPU]                             = "cpu",
        [CGROUP_CONTROLLER_CPUACCT]                         = "cpuacct",
        [CGROUP_CONTROLLER_CPUSET]                          = "cpuset",
        [CGROUP_CONTROLLER_IO]                              = "io",
        [CGROUP_CONTROLLER_BLKIO]                           = "blkio",
        [CGROUP_CONTROLLER_జ్ఞాపకం]                          = "memory",
        [CGROUP_CONTROLLER_DEVICES]                         = "devices",
        [CGROUP_CONTROLLER_PIDS]                            = "pids",
        [CGROUP_CONTROLLER_BPF_FIREWALL]                    = "bpf-firewall",
        [CGROUP_CONTROLLER_BPF_DEVICES]                     = "bpf-devices",
        [CGROUP_CONTROLLER_BPF_FOREIGN]                     = "bpf-foreign",
        [CGROUP_CONTROLLER_BPF_SOCKET_BIND]                 = "bpf-socket-bind",
        [CGROUP_CONTROLLER_BPF_RESTRICT_జాలం_INTERFACES] = "bpf-restrict-network-interfaces",
        [CGROUP_CONTROLLER_BPF_BIND_జాలం_INTERFACE]      = "bpf-bind-network-interface",
};

DEFINE_STRING_పట్టిక_LOOKUP(cgroup_controller, CGroupController);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి managed_oom_బహుళకం_పట్టిక[MANAGED_OOM_బహుళకం_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_పట్టిక_LOOKUP(managed_oom_బహుళకం, ManagedOOMMode);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి managed_oom_preference_పట్టిక[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_పట్టిక_LOOKUP(managed_oom_preference, ManagedOOMPreference);
