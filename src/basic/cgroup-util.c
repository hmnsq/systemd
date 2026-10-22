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

صحیح cg_is_دستیاب(خالی) {
        ساخت statfs fs;

        اگر (statfs("/sys/fs/cgroup/", &fs) < 0) {
                اگر (errno == ENOENT) /* sysfs not mounted? */
                        واپس false;

                واپس log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        واپس is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

صحیح cg_راستہ_open(ثابت حرف *راستہ) {
        _cleanup_free_ حرف *fs = تہی;
        صحیح r;

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &fs);
        اگر (r < 0)
                واپس r;

        واپس RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

صحیح cg_cgroupid_open(صحیح cgroupfs_fd, uint64_t id) {
        _cleanup_close_ صحیح fsfd = -EBADF;

        اگر (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                اگر (fsfd < 0)
                        واپس -errno;

                cgroupfs_fd = fsfd;
        }

        اتحاد {
                ساخت file_handle file_handle;
                uint8_t space[offsetof(ساخت file_handle, f_handle) + حجم(uint64_t)];
        } fh = {
                .file_handle.handle_بائٹیں = حجم(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        واپس RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

صحیح cg_راستہ_from_cgroupid(صحیح cgroupfs_fd, uint64_t id, حرف **ret) {
        _cleanup_close_ صحیح cgfd = -EBADF;
        صحیح r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        اگر (cgfd < 0)
                واپس cgfd;

        _cleanup_free_ حرف *راستہ = تہی;
        r = fd_get_راستہ(cgfd, &راستہ);
        اگر (r < 0)
                واپس r;

        اگر (!راستہ_startswith(راستہ, "/sys/fs/cgroup/"))
                واپس -EXDEV; /* recognizable error */

        اگر (ret)
                *ret = TAKE_PTR(راستہ);
        واپس 0;
}

صحیح cg_enumerate_processes(ثابت حرف *راستہ, FILE **ret) {
        _cleanup_free_ حرف *fs = تہی;
        FILE *f;
        صحیح r;

        أكد(ret);

        r = cg_get_راستہ(راستہ, "cgroup.procs", &fs);
        اگر (r < 0)
                واپس r;

        f = fopen(fs, "re");
        اگر (!f)
                واپس -errno;

        *ret = f;
        واپس 0;
}

صحیح cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        غیرعلامتی طویل ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        أكد(f);
        أكد(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        برائے (;;) {
                errno = 0;
                اگر (fscanf(f, "%lu", &ul) != 1) {

                        اگر (feof(f)) {
                                *ret = 0;
                                واپس 0;
                        }

                        واپس errno_or_else(EIO);
                }

                اگر (ul > PID_T_MAX)
                        واپس -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                اگر (ul == 0 && !FLAGS_سیٹ(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        جاری;

                *ret = (pid_t) ul;
                واپس 1;
        }
}

صحیح cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        صحیح r;

        أكد(f);
        أكد(ret);

        برائے (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                اگر (r < 0)
                        واپس log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                اگر (r == 0) {
                        *ret = PIDREF_NULL;
                        واپس 0;
                }

                اگر (pid == 0)
                        واپس -EREMOTE;

                r = pidref_سیٹ_pid(ret, pid);
                اگر (r >= 0)
                        واپس 1;
                اگر (r != -ESRCH)
                        واپس r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(خالی) {
        ساکن thread_local صحیح supported = -1;

        اگر (supported >= 0)
                واپس supported;

        اگر (cg_is_دستیاب() <= 0)
                واپس (supported = false);

        اگر (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                واپس (supported = true);
        اگر (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        واپس (supported = false);
}

صحیح cg_enumerate_subgroups(ثابت حرف *راستہ, DIR **ret) {
        _cleanup_free_ حرف *fs = تہی;
        DIR *d;
        صحیح r;

        أكد(ret);

        /* This is not recursive! */

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &fs);
        اگر (r < 0)
                واپس r;

        d = opendir(fs);
        اگر (!d)
                واپس -errno;

        *ret = d;
        واپس 0;
}

صحیح cg_read_subgroup(DIR *d, حرف **ret) {
        أكد(d);
        أكد(ret);

        FOREACH_DIRENT_ALL(de, d, واپس -errno) {
                اگر (de->d_type != DT_DIR)
                        جاری;

                اگر (dot_or_dot_dot(de->d_name))
                        جاری;

                واپس strdup_مقصد_بھرا(ret, de->d_name);
        }

        *ret = تہی;
        واپس 0;
}

صحیح cg_kill(
                ثابت حرف *راستہ,
                صحیح sig,
                CGroupFlags flags,
                سیٹ_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                خالی *userdata) {

        cleanup_سیٹ_free سیٹ_2 *allocated_سیٹ = تہی;
        صحیح r, ret = 0;

        أكد(راستہ);
        أكد(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        اگر (اندر_سیٹ(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        اگر (!killed_pids) {
                killed_pids = allocated_سیٹ = سیٹ_new(تہی);
                اگر (!killed_pids)
                        واپس -ENOMEM;
        }

        bool مکمل;
        کرو {
                _cleanup_fclose_ FILE *f = تہی;
                صحیح ret_log_kill;

                مکمل = true;

                r = cg_enumerate_processes(راستہ, &f);
                اگر (r == -ENOENT)
                        توڑو;
                اگر (r < 0)
                        واپس RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                برائے (;;) {
                        _cleanup_(pidref_مکمل) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        اگر (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                مکمل = true;
                                توڑو;
                        }
                        اگر (r < 0)
                                واپس RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", راستہ));
                        اگر (r == 0)
                                توڑو;

                        اگر ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                جاری;

                        اگر (سیٹ_شامل_ہے(killed_pids, PID_مقصد_PTR(pidref.pid)))
                                جاری;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        اگر (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, راستہ);
                                جاری;
                        }

                        اگر (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        اگر (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, راستہ));
                        اگر (r >= 0) {
                                اگر (flags & CGROUP_SIGCONT)
                                        (خالی) pidref_kill(&pidref, SIGCONT);

                                اگر (ret == 0) {
                                        اگر (log_kill)
                                                ret = ret_log_kill;
                                        ورنہ
                                                ret = 1;
                                }
                        }

                        مکمل = false;

                        r = سیٹ_put(killed_pids, PID_مقصد_PTR(pidref.pid));
                        اگر (r < 0)
                                واپس RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } جب (!مکمل);

        واپس ret;
}

صحیح cg_kill_recursive(
                ثابت حرف *راستہ,
                صحیح sig,
                CGroupFlags flags,
                سیٹ_2 *killed_pids,
                cg_kill_log_func_t log_kill,
                خالی *userdata) {

        cleanup_سیٹ_free سیٹ_2 *allocated_سیٹ = تہی;
        _cleanup_closedir_ DIR *d = تہی;
        صحیح r, ret;

        أكد(راستہ);
        أكد(sig >= 0);

        اگر (!killed_pids) {
                killed_pids = allocated_سیٹ = سیٹ_new(تہی);
                اگر (!killed_pids)
                        واپس -ENOMEM;
        }

        ret = cg_kill(راستہ, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(راستہ, &d);
        اگر (r < 0) {
                اگر (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", راستہ));

                واپس ret;
        }

        برائے (;;) {
                _cleanup_free_ حرف *fn = تہی, *p = تہی;

                r = cg_read_subgroup(d, &fn);
                اگر (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", راستہ));
                        توڑو;
                }
                اگر (r == 0)
                        توڑو;

                p = راستہ_join(خالی_خانے_مقصد_جڑ(راستہ), fn);
                اگر (!p)
                        واپس -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                اگر (r != 0 && ret >= 0)
                        ret = r;
        }

        واپس ret;
}

صحیح cg_kill_kernel_sigkill(ثابت حرف *راستہ, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ حرف *killfile = تہی;
        uint64_t n_pids = UINT64_MAX;
        صحیح r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        أكد(راستہ);

        اگر (!cg_kill_supported())
                واپس -EOPNOTSUPP;

        r = cg_get_راستہ(راستہ, "cgroup.kill", &killfile);
        اگر (r < 0)
                واپس r;

        اگر (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(راستہ, "pids.current", &n_pids);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", راستہ);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_بفر);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", راستہ);

        اگر (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        واپس 0;
}

صحیح cg_get_راستہ(ثابت حرف *راستہ, ثابت حرف *suffix, حرف **ret) {
        حرف *t;

        أكد(ret);

        اگر (isempty(راستہ))
                راستہ = TAKE_PTR(suffix);

        t = راستہ_join("/sys/fs/cgroup", راستہ, suffix);
        اگر (!t)
                واپس -ENOMEM;

        *ret = راستہ_simplify(t);
        واپس 0;
}

صحیح cg_سیٹ_xattr(ثابت حرف *راستہ, ثابت حرف *name, ثابت خالی *قدر, size_t سائز, صحیح flags) {
        _cleanup_free_ حرف *fs = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(name);
        أكد(قدر || سائز <= 0);

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &fs);
        اگر (r < 0)
                واپس r;

        واپس RET_NERRNO(setxattr(fs, name, قدر, سائز, flags));
}

صحیح cg_get_xattr(ثابت حرف *راستہ, ثابت حرف *name, حرف **ret, size_t *ret_سائز) {
        _cleanup_free_ حرف *fs = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(name);

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &fs);
        اگر (r < 0)
                واپس r;

        واپس lgetxattr_malloc(fs, name, ret, ret_سائز);
}

صحیح cg_get_xattr_bool(ثابت حرف *راستہ, ثابت حرف *name) {
        _cleanup_free_ حرف *fs = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(name);

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &fs);
        اگر (r < 0)
                واپس r;

        واپس getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

صحیح cg_remove_xattr(ثابت حرف *راستہ, ثابت حرف *name) {
        _cleanup_free_ حرف *fs = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(name);

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &fs);
        اگر (r < 0)
                واپس r;

        واپس RET_NERRNO(removexattr(fs, name));
}

صحیح cg_pid_get_راستہ(pid_t pid, حرف **ret_راستہ) {
        _cleanup_fclose_ FILE *f = تہی;
        ثابت حرف *fs;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret_راستہ);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        اگر (r == -ENOENT)
                واپس -ESRCH;
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;
                حرف *e;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس -ENODATA;

                e = startswith(سطر, "0:");
                اگر (!e)
                        جاری;

                e = strchr(e, ':');
                اگر (!e)
                        جاری;

                _cleanup_free_ حرف *راستہ = strdup(e + 1);
                اگر (!راستہ)
                        واپس -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                اگر (startswith(راستہ, "/../"))
                        واپس -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(راستہ, " (deleted)");
                اگر (e)
                        *e = 0;

                *ret_راستہ = TAKE_PTR(راستہ);
                واپس 0;
        }
}

صحیح cg_pidref_get_راستہ(ثابت PidRef *pidref, حرف **ret_راستہ) {
        _cleanup_free_ حرف *راستہ = تہی;
        صحیح r;

        أكد(ret_راستہ);

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_راستہ(pidref->pid, &راستہ);
        اگر (r < 0)
                واپس r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        *ret_راستہ = TAKE_PTR(راستہ);
        واپس 0;
}

صحیح cg_is_خالی_خانے(ثابت حرف *راستہ) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        أكد(راستہ);

        /* The root cgroup is always populated */
        اگر (خالی_خانے_or_جڑ(راستہ))
                واپس false;

        r = cg_get_keyed_attribute(راستہ, "cgroup.events", STRV_MAKE("populated"), &t);
        اگر (r == -ENOENT)
                واپس true;
        اگر (r < 0)
                واپس r;

        واپس streq(t, "0");
}

صحیح cg_بانٹو_spec(ثابت حرف *spec, حرف **ret_controller, حرف **ret_راستہ) {
        _cleanup_free_ حرف *controller = تہی;
        ثابت حرف *راستہ;
        صحیح r;

        أكد(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        اگر (isempty(spec) || راستہ_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                راستہ = spec;
                جاؤ finalize;
        }

        ثابت حرف *e = strchr(spec, ':');
        اگر (!e) {
                /* Controller only. */
                اگر (ret_controller) {
                        controller = strdup(spec);
                        اگر (!controller)
                                واپس -ENOMEM;
                }

                راستہ = تہی;
        } ورنہ {
                /* Both controller and path. */
                اگر (ret_controller) {
                        controller = strndup(spec, e - spec);
                        اگر (!controller)
                                واپس -ENOMEM;
                }

                راستہ = e + 1;
        }

finalize:
        راستہ = خالی_خانے_مقصد_null(راستہ);

        اگر (راستہ) {
                /* Non-empty path must be absolute. */
                اگر (!راستہ_is_absolute(راستہ))
                        واپس -EINVAL;

                /* Path must not contain dot-dot. */
                اگر (!راستہ_is_محفوظ(راستہ))
                        واپس -EINVAL;
        }

        اگر (ret_راستہ) {
                r = راستہ_simplify_alloc(راستہ, ret_راستہ);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        واپس 0;
}

صحیح cg_get_جڑ_راستہ(حرف **ret_راستہ) {
        حرف *p, *e;
        صحیح r;

        أكد(ret_راستہ);

        r = cg_pid_get_راستہ(1, &p);
        اگر (r < 0)
                واپس r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        اگر (e)
                *e = 0;

        *ret_راستہ = p;
        واپس 0;
}

صحیح cg_سرکاؤ_راستہ(ثابت حرف *cgroup, ثابت حرف *جڑ, ثابت حرف **ret_shifted) {
        صحیح r;

        أكد(cgroup);
        أكد(ret_shifted);

        _cleanup_free_ حرف *rt = تہی;
        اگر (!جڑ) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_جڑ_راستہ(&rt);
                اگر (r < 0)
                        واپس r;

                جڑ = rt;
        }

        *ret_shifted = راستہ_startswith_بھرا(cgroup, جڑ, راستہ_STARTSWITH_RETURN_LEADING_SLASH|راستہ_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        واپس 0;
}

صحیح cg_pid_get_راستہ_shifted(pid_t pid, ثابت حرف *جڑ, حرف **ret_cgroup) {
        _cleanup_free_ حرف *raw = تہی;
        ثابت حرف *c;
        صحیح r;

        أكد(pid >= 0);
        أكد(ret_cgroup);

        r = cg_pid_get_راستہ(pid, &raw);
        اگر (r < 0)
                واپس r;

        r = cg_سرکاؤ_راستہ(raw, جڑ, &c);
        اگر (r < 0)
                واپس r;

        اگر (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                واپس 0;
        }

        واپس strdup_مقصد(ret_cgroup, c);
}

صحیح cg_pidref_get_راستہ_shifted(ثابت PidRef *pidref, ثابت حرف *جڑ, حرف **ret_cgroup) {
        _cleanup_free_ حرف *raw = تہی;
        ثابت حرف *c;
        صحیح r;

        أكد(ret_cgroup);

        r = cg_pidref_get_راستہ(pidref, &raw);
        اگر (r < 0)
                واپس r;

        r = cg_سرکاؤ_راستہ(raw, جڑ, &c);
        اگر (r < 0)
                واپس r;

        اگر (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                واپس 0;
        }

        واپس strdup_مقصد(ret_cgroup, c);
}

صحیح cg_راستہ_decode_unit(ثابت حرف *cgroup, حرف **ret_unit) {
        أكد(cgroup);

        size_t n = strcspn(cgroup, "/");
        اگر (n < 3)
                واپس -ENXIO;

        حرف *c = strndupa_محفوظ(cgroup, n);
        c = cg_unescape(c);

        اگر (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                واپس -ENXIO;

        اگر (ret_unit)
                واپس strdup_مقصد(ret_unit, c);

        واپس 0;
}

ساکن bool valid_قاش_name(ثابت حرف *p, size_t n) {
        أكد(p || n == 0);

        اگر (n < STRLEN("x.slice"))
                واپس false;

        حرف *c = strndupa_محفوظ(p, n);
        اگر (!endswith(c, ".slice"))
                واپس false;

        واپس unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

ساکن ثابت حرف* skip_slices(ثابت حرف *p) {
        أكد(p);

        /* Skips over all slice assignments */

        برائے (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                اگر (!valid_قاش_name(p, n))
                        واپس p;

                p += n;
        }
}

صحیح cg_راستہ_get_unit_بھرا(ثابت حرف *راستہ, حرف **ret_unit, حرف **ret_subgroup) {
        صحیح r;

        أكد(راستہ);

        ثابت حرف *e = skip_slices(راستہ);

        _cleanup_free_ حرف *unit = تہی;
        r = cg_راستہ_decode_unit(e, &unit);
        اگر (r < 0)
                واپس r;

        /* We skipped over the slices, don't accept any now */
        اگر (endswith(unit, ".slice"))
                واپس -ENXIO;

        اگر (ret_subgroup) {
                _cleanup_free_ حرف *subgroup = تہی;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                اگر (isempty(e))
                        subgroup = تہی;
                ورنہ {
                        subgroup = strdup(e);
                        اگر (!subgroup)
                                واپس -ENOMEM;
                }

                راستہ_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        اگر (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        واپس 0;
}

صحیح cg_راستہ_get_unit_راستہ(ثابت حرف *راستہ, حرف **ret) {
        _cleanup_free_ حرف *راستہ_copy = تہی;
        حرف *unit_name;

        أكد(راستہ);
        أكد(ret);

        راستہ_copy = strdup(راستہ);
        اگر (!راستہ_copy)
                واپس -ENOMEM;

        unit_name = (حرف*) skip_slices(راستہ_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        اگر (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                واپس -ENXIO;

        *ret = TAKE_PTR(راستہ_copy);

        واپس 0;
}

صحیح cg_pid_get_unit_بھرا(pid_t pid, حرف **ret_unit, حرف **ret_subgroup) {
        صحیح r;

        _cleanup_free_ حرف *cgroup = تہی;
        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_unit_بھرا(cgroup, ret_unit, ret_subgroup);
}

صحیح cg_pidref_get_unit_بھرا(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        _cleanup_free_ حرف *unit = تہی, *subgroup = تہی;
        r = cg_pid_get_unit_بھرا(pidref->pid, &unit, &subgroup);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        اگر (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        اگر (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        واپس 0;
}

ساکن ثابت حرف* skip_session(ثابت حرف *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        اگر (isempty(p))
                واپس تہی;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        اگر (n < STRLEN("session-x.scope"))
                واپس تہی;

        ثابت حرف *s = startswith(p, "session-");
        اگر (!s)
                واپس تہی;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        حرف *f = strndupa_محفوظ(s, p + n - s),
             *e = endswith(f, ".scope");
        اگر (!e)
                واپس تہی;
        *e = '\0';

        اگر (!session_id_valid(f))
                واپس تہی;

        واپس skip_leading_slash(p + n);
}

ساکن ثابت حرف* skip_user_manager(ثابت حرف *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        اگر (isempty(p))
                واپس تہی;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        اگر (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                واپس تہی;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ حرف *unit_name = strndup(p, n);
        اگر (!unit_name)
                واپس تہی;

        _cleanup_free_ حرف *i = تہی;
        UnitNameFlags type = unit_name_مقصد_instance(unit_name, &i);

        اگر (type != UNIT_NAME_INSTANCE)
                واپس تہی;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        اگر (!(startswith(unit_name, "user@") && parse_uid(i, تہی) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                واپس تہی;

        واپس skip_leading_slash(p + n);
}

ساکن ثابت حرف* skip_user_سابقہ(ثابت حرف *راستہ) {
        ثابت حرف *e, *t;

        أكد(راستہ);

        /* Skip slices, if there are any */
        e = skip_slices(راستہ);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        اگر (t)
                واپس t;

        /* Alternatively skip the user session if it is in the path... */
        واپس skip_session(e);
}

صحیح cg_راستہ_get_user_unit_بھرا(ثابت حرف *راستہ, حرف **ret_unit, حرف **ret_subgroup) {
        ثابت حرف *t;

        أكد(راستہ);

        t = skip_user_سابقہ(راستہ);
        اگر (!t)
                واپس -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        واپس cg_راستہ_get_unit_بھرا(t, ret_unit, ret_subgroup);
}

صحیح cg_pid_get_user_unit_بھرا(pid_t pid, حرف **ret_unit, حرف **ret_subgroup) {
        صحیح r;

        _cleanup_free_ حرف *cgroup = تہی;
        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_user_unit_بھرا(cgroup, ret_unit, ret_subgroup);
}

صحیح cg_pidref_get_user_unit_بھرا(ثابت PidRef *pidref, حرف **ret_unit, حرف **ret_subgroup) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        _cleanup_free_ حرف *unit = تہی, *subgroup = تہی;
        r = cg_pid_get_user_unit_بھرا(pidref->pid, &unit, &subgroup);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        اگر (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        اگر (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        واپس 0;
}

صحیح cg_راستہ_get_machine_name(ثابت حرف *راستہ, حرف **ret_machine) {
        _cleanup_free_ حرف *u = تہی;
        ثابت حرف *sl;
        صحیح r;

        r = cg_راستہ_get_unit(راستہ, &u);
        اگر (r < 0)
                واپس r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        واپس readlink_malloc(sl, ret_machine);
}

صحیح cg_pid_get_machine_name(pid_t pid, حرف **ret_machine) {
        _cleanup_free_ حرف *cgroup = تہی;
        صحیح r;

        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_machine_name(cgroup, ret_machine);
}

صحیح cg_راستہ_get_session(ثابت حرف *راستہ, حرف **ret_session) {
        _cleanup_free_ حرف *unit = تہی;
        حرف *آغاز, *انتہا;
        صحیح r;

        أكد(راستہ);

        r = cg_راستہ_get_unit(راستہ, &unit);
        اگر (r < 0)
                واپس r;

        آغاز = startswith(unit, "session-");
        اگر (!آغاز)
                واپس -ENXIO;
        انتہا = endswith(آغاز, ".scope");
        اگر (!انتہا)
                واپس -ENXIO;

        *انتہا = 0;
        اگر (!session_id_valid(آغاز))
                واپس -ENXIO;

        اگر (!ret_session)
                واپس 0;

        واپس strdup_مقصد(ret_session, آغاز);
}

صحیح cg_pid_get_session(pid_t pid, حرف **ret_session) {
        _cleanup_free_ حرف *cgroup = تہی;
        صحیح r;

        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_session(cgroup, ret_session);
}

صحیح cg_pidref_get_session(ثابت PidRef *pidref, حرف **ret) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        _cleanup_free_ حرف *session = تہی;
        r = cg_pid_get_session(pidref->pid, &session);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = TAKE_PTR(session);
        واپس 0;
}

صحیح cg_راستہ_get_owner_uid(ثابت حرف *راستہ, uid_t *ret_uid) {
        _cleanup_free_ حرف *قاش = تہی;
        حرف *آغاز, *انتہا;
        صحیح r;

        أكد(راستہ);

        r = cg_راستہ_get_قاش(راستہ, &قاش);
        اگر (r < 0)
                واپس r;

        آغاز = startswith(قاش, "user-");
        اگر (!آغاز)
                واپس -ENXIO;

        انتہا = endswith(آغاز, ".slice");
        اگر (!انتہا)
                واپس -ENXIO;

        *انتہا = 0;
        اگر (parse_uid(آغاز, ret_uid) < 0)
                واپس -ENXIO;

        واپس 0;
}

صحیح cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ حرف *cgroup = تہی;
        صحیح r;

        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_owner_uid(cgroup, ret_uid);
}

صحیح cg_pidref_get_owner_uid(ثابت PidRef *pidref, uid_t *ret) {
        صحیح r;

        اگر (!pidref_is_سیٹ(pidref))
                واپس -ESRCH;
        اگر (pidref_is_remote(pidref))
                واپس -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        اگر (r < 0)
                واپس r;

        r = pidref_verify(pidref);
        اگر (r < 0)
                واپس r;

        اگر (ret)
                *ret = uid;

        واپس 0;
}

صحیح cg_راستہ_get_قاش(ثابت حرف *p, حرف **ret_قاش) {
        ثابت حرف *e = تہی;

        أكد(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        برائے (;;) {
                ثابت حرف *s;
                صحیح n;

                n = راستہ_ڈھونڈو_پہلا_component(&p, /* accept_dot_dot= */ false, &s);
                اگر (n < 0)
                        واپس n;
                اگر (!valid_قاش_name(s, n))
                        توڑو;

                e = s;
        }

        اگر (e)
                واپس cg_راستہ_decode_unit(e, ret_قاش);

        اگر (ret_قاش)
                واپس strdup_مقصد(ret_قاش, SPECIAL_جڑ_قاش);

        واپس 0;
}

صحیح cg_pid_get_قاش(pid_t pid, حرف **ret_قاش) {
        _cleanup_free_ حرف *cgroup = تہی;
        صحیح r;

        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_قاش(cgroup, ret_قاش);
}

صحیح cg_راستہ_get_user_قاش(ثابت حرف *p, حرف **ret_قاش) {
        ثابت حرف *t;
        أكد(p);

        t = skip_user_سابقہ(p);
        اگر (!t)
                واپس -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        واپس cg_راستہ_get_قاش(t, ret_قاش);
}

صحیح cg_pid_get_user_قاش(pid_t pid, حرف **ret_قاش) {
        _cleanup_free_ حرف *cgroup = تہی;
        صحیح r;

        r = cg_pid_get_راستہ_shifted(pid, تہی, &cgroup);
        اگر (r < 0)
                واپس r;

        واپس cg_راستہ_get_user_قاش(cgroup, ret_قاش);
}

bool cg_needs_escape(ثابت حرف *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        اگر (!filename_is_valid(p))
                واپس true;

        اگر (اندر_سیٹ(p[0], '_', '.'))
                واپس true;

        اگر (STR_اندر_سیٹ(p, "notify_on_release", "release_agent", "tasks"))
                واپس true;

        اگر (startswith(p, "cgroup."))
                واپس true;

        برائے (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ثابت حرف *q;

                q = startswith(p, cgroup_controller_مقصد_string(c));
                اگر (!q)
                        جاری;

                اگر (q[0] == '.')
                        واپس true;
        }

        واپس false;
}

صحیح cg_escape(ثابت حرف *p, حرف **ret) {
        _cleanup_free_ حرف *n = تہی;

        أكد(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        اگر (cg_needs_escape(p)) {
                n = strjoin("_", p);
                اگر (!n)
                        واپس -ENOMEM;

                اگر (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        واپس -EINVAL;
        } ورنہ {
                n = strdup(p);
                اگر (!n)
                        واپس -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        واپس 0;
}

حرف* cg_unescape(ثابت حرف *p) {
        أكد(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        اگر (p[0] == '_')
                واپس (حرف*) p+1;

        واپس (حرف*) p;
}

صحیح cg_قاش_مقصد_راستہ(ثابت حرف *unit, حرف **ret) {
        _cleanup_free_ حرف *p = تہی, *s = تہی, *e = تہی;
        ثابت حرف *dash;
        صحیح r;

        أكد(unit);
        أكد(ret);

        اگر (streq(unit, SPECIAL_جڑ_قاش))
                واپس strdup_مقصد(ret, "");

        اگر (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                واپس -EINVAL;

        اگر (!endswith(unit, ".slice"))
                واپس -EINVAL;

        r = unit_name_مقصد_سابقہ(unit, &p);
        اگر (r < 0)
                واپس r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        اگر (dash == p)
                واپس -EINVAL;

        جب (dash) {
                _cleanup_free_ حرف *escaped = تہی;
                حرف n[dash - p + حجم(".slice")];

#اگر HAS_FEATURE_حافظہ_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#اگر_ختم

                /* Don't allow trailing or double dashes */
                اگر (اندر_سیٹ(dash[1], 0, '-'))
                        واپس -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                اگر (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        واپس -EINVAL;

                r = cg_escape(n, &escaped);
                اگر (r < 0)
                        واپس r;

                اگر (!strextend(&s, escaped, "/"))
                        واپس -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        اگر (r < 0)
                واپس r;

        اگر (!strextend(&s, e))
                واپس -ENOMEM;

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح cg_is_threaded(ثابت حرف *راستہ) {
        _cleanup_free_ حرف *fs = تہی, *contents = تہی;
        _cleanup_strv_free_ حرف **v = تہی;
        صحیح r;

        r = cg_get_راستہ(راستہ, "cgroup.type", &fs);
        اگر (r < 0)
                واپس r;

        r = read_بھرا_virtual_file(fs, &contents, تہی);
        اگر (r == -ENOENT)
                واپس false; /* Assume no. */
        اگر (r < 0)
                واپس r;

        v = strv_بانٹو(contents, تہی);
        اگر (!v)
                واپس -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        واپس strv_شامل_ہے(v, "threaded") || strv_شامل_ہے(v, "invalid");
}

صحیح cg_سیٹ_attribute(ثابت حرف *راستہ, ثابت حرف *attribute, ثابت حرف *قدر) {
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(attribute);

        r = cg_get_راستہ(راستہ, attribute, &p);
        اگر (r < 0)
                واپس r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        واپس write_string_file(p, قدر, WRITE_STRING_FILE_DISABLE_بفر|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

صحیح cg_get_attribute(ثابت حرف *راستہ, ثابت حرف *attribute, حرف **ret) {
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(attribute);

        r = cg_get_راستہ(راستہ, attribute, &p);
        اگر (r < 0)
                واپس r;

        واپس read_one_سطر_file(p, ret);
}

صحیح cg_get_attribute_as_uint64(ثابت حرف *راستہ, ثابت حرف *attribute, uint64_t *ret) {
        _cleanup_free_ حرف *قدر = تہی;
        uint64_t v;
        صحیح r;

        أكد(ret);

        r = cg_get_attribute(راستہ, attribute, &قدر);
        اگر (r == -ENOENT)
                واپس -ENODATA;
        اگر (r < 0)
                واپس r;

        اگر (streq(قدر, "max")) {
                *ret = CGROUP_حد_بندی_MAX;
                واپس 0;
        }

        r = محفوظ_atou64(قدر, &v);
        اگر (r < 0)
                واپس r;

        *ret = v;
        واپس 0;
}

صحیح cg_get_attribute_as_bool(ثابت حرف *راستہ, ثابت حرف *attribute) {
        _cleanup_free_ حرف *قدر = تہی;
        صحیح r;

        r = cg_get_attribute(راستہ, attribute, &قدر);
        اگر (r == -ENOENT)
                واپس -ENODATA;
        اگر (r < 0)
                واپس r;

        واپس parse_boolean(قدر);
}

صحیح cg_get_owner(ثابت حرف *راستہ, uid_t *ret_uid) {
        _cleanup_free_ حرف *f = تہی;
        ساخت stat stats;
        صحیح r;

        أكد(ret_uid);

        r = cg_get_راستہ(راستہ, /* suffix= */ تہی, &f);
        اگر (r < 0)
                واپس r;

        اگر (stat(f, &stats) < 0)
                واپس -errno;

        r = stat_verify_directory(&stats);
        اگر (r < 0)
                واپس r;

        *ret_uid = stats.st_uid;
        واپس 0;
}

صحیح cg_get_keyed_attribute(
                ثابت حرف *راستہ,
                ثابت حرف *attribute,
                حرف * ثابت *کلیدیں,
                حرف **values) {

        _cleanup_free_ حرف *filename = تہی, *contents = تہی;
        size_t n;
        صحیح r;

        أكد(راستہ);
        أكد(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_راستہ(راستہ, attribute, &filename);
        اگر (r < 0)
                واپس r;

        r = read_بھرا_file(filename, &contents, /* ret_size= */ تہی);
        اگر (r < 0)
                واپس r;

        n = strv_لمبائی(کلیدیں);
        اگر (n == 0) /* No keys to retrieve? That's easy, we are done then */
                واپس 0;
        أكد(strv_is_uniq(کلیدیں));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        حرف **v = newa0(حرف*, n);
        size_t n_مکمل = 0;

        برائے (ثابت حرف *p = contents; *p;) {
                ثابت حرف *w;
                size_t i;

                برائے (i = 0; i < n; i++) {
                        w = پہلا_لفظ(p, کلیدیں[i]);
                        اگر (w)
                                توڑو;
                }

                اگر (w) {
                        اگر (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                جاؤ fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        اگر (!v[i]) {
                                r = -ENOMEM;
                                جاؤ fail;
                        }

                        n_مکمل++;
                        اگر (n_مکمل >= n)
                                توڑو;

                        p = w + l;
                } ورنہ
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        اگر (n_مکمل < n) {
                r = -ENXIO;
                جاؤ fail;
        }

        حافظہ_نقل(values, v, حجم(حرف*) * n);
        واپس 0;

fail:
        free_many_charp(v, n);
        واپس r;
}

صحیح cg_get_keyed_attribute_uint64(ثابت حرف *راستہ, ثابت حرف *attribute, ثابت حرف *کلید, uint64_t *ret) {
        _cleanup_free_ حرف *val = تہی;
        صحیح r;

        أكد(کلید);
        أكد(ret);

        r = cg_get_keyed_attribute(راستہ, attribute, STRV_MAKE(کلید), &val);
        اگر (r < 0)
                واپس r;

        r = محفوظ_atou64(val, ret);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, کلید, attribute);

        واپس 0;
}

صحیح cg_نقاب_مقصد_string(CGroup_نقاب نقاب_3, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        أكد(ret);

        اگر (نقاب_3 == 0) {
                *ret = تہی;
                واپس 0;
        }

        برائے (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ثابت حرف *k;
                size_t l;

                اگر (!FLAGS_سیٹ(نقاب_3, CGROUP_CONTROLLER_مقصد_نقاب(c)))
                        جاری;

                k = cgroup_controller_مقصد_string(c);
                l = لڑی_لمبائی(k);

                اگر (!GREEDY_REALLOC(s, n + space + l + 1))
                        واپس -ENOMEM;

                اگر (space)
                        s[n] = ' ';
                حافظہ_نقل(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        أكد(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        واپس 0;
}

صحیح cg_نقاب_from_string(ثابت حرف *s, CGroup_نقاب *ret) {
        CGroup_نقاب m = 0;

        أكد(ret);
        أكد(s);

        برائے (;;) {
                _cleanup_free_ حرف *n = تہی;
                CGroupController v;
                صحیح r;

                r = نکالو_پہلا_لفظ(&s, &n, تہی, 0);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                v = cgroup_controller_from_string(n);
                اگر (v < 0)
                        جاری;

                m |= CGROUP_CONTROLLER_مقصد_نقاب(v);
        }

        *ret = m;
        واپس 0;
}

صحیح cg_نقاب_supported_subtree(ثابت حرف *جڑ, CGroup_نقاب *ret) {
        CGroup_نقاب نقاب_3;
        صحیح r;

        أكد(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ حرف *controllers = تہی, *راستہ = تہی;
        r = cg_get_راستہ(جڑ, "cgroup.controllers", &راستہ);
        اگر (r < 0)
                واپس r;

        r = read_one_سطر_file(راستہ, &controllers);
        اگر (r < 0)
                واپس r;

        r = cg_نقاب_from_string(controllers, &نقاب_3);
        اگر (r < 0)
                واپس r;

        /* Mask controllers that are not supported in cgroup v2. */
        نقاب_3 &= CGROUP_نقاب_V2;

        *ret = نقاب_3;
        واپس 0;
}

صحیح cg_نقاب_supported(CGroup_نقاب *ret) {
        _cleanup_free_ حرف *جڑ = تہی;
        صحیح r;

        r = cg_get_جڑ_راستہ(&جڑ);
        اگر (r < 0)
                واپس r;

        واپس cg_نقاب_supported_subtree(جڑ, ret);
}

صحیح cg_is_delegated(ثابت حرف *راستہ) {
        صحیح r;

        أكد(راستہ);

        r = cg_get_xattr_bool(راستہ, "trusted.delegate");
        اگر (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                واپس r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(راستہ, "user.delegate");
        واپس ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

صحیح cg_is_delegated_fd(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ تہی, "trusted.delegate", /* at_flags= */ 0);
        اگر (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                واپس r;

        r = getxattr_at_bool(fd, /* path= */ تہی, "user.delegate", /* at_flags= */ 0);
        واپس ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

صحیح cg_has_coredump_receive(ثابت حرف *راستہ) {
        صحیح r;

        أكد(راستہ);

        r = cg_get_xattr_bool(راستہ, "user.coredump_receive");
        اگر (ERRNO_IS_NEG_XATTR_ABSENT(r))
                واپس false;

        واپس r;
}

ثابت uint64_t cgroup_io_حد_بندی_defaults[CGROUP_IO_حد_بندی_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_حد_بندی_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_حد_بندی_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_حد_بندی_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_حد_بندی_MAX,
};

ساکن ثابت حرف* ثابت cgroup_io_حد_بندی_type_جدول[CGROUP_IO_حد_بندی_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_جدول_LOOKUP(cgroup_io_حد_بندی_type, CGroupIOLimitType);

خالی cgroup_io_limits_list(خالی) {
        DUMP_STRING_جدول(cgroup_io_حد_بندی_type, CGroupIOLimitType, CGROUP_IO_حد_بندی_TYPE_MAX);
}

ساکن ثابت حرف *ثابت cgroup_controller_جدول[_CGROUP_CONTROLLER_MAX] = {
        [CGROUP_CONTROLLER_CPU]                             = "cpu",
        [CGROUP_CONTROLLER_CPUACCT]                         = "cpuacct",
        [CGROUP_CONTROLLER_CPUSET]                          = "cpuset",
        [CGROUP_CONTROLLER_IO]                              = "io",
        [CGROUP_CONTROLLER_BLKIO]                           = "blkio",
        [CGROUP_CONTROLLER_حافظہ]                          = "memory",
        [CGROUP_CONTROLLER_DEVICES]                         = "devices",
        [CGROUP_CONTROLLER_PIDS]                            = "pids",
        [CGROUP_CONTROLLER_BPF_FIREWALL]                    = "bpf-firewall",
        [CGROUP_CONTROLLER_BPF_DEVICES]                     = "bpf-devices",
        [CGROUP_CONTROLLER_BPF_FOREIGN]                     = "bpf-foreign",
        [CGROUP_CONTROLLER_BPF_SOCKET_BIND]                 = "bpf-socket-bind",
        [CGROUP_CONTROLLER_BPF_RESTRICT_جال_INTERFACES] = "bpf-restrict-network-interfaces",
        [CGROUP_CONTROLLER_BPF_BIND_جال_INTERFACE]      = "bpf-bind-network-interface",
};

DEFINE_STRING_جدول_LOOKUP(cgroup_controller, CGroupController);

ساکن ثابت حرف* ثابت managed_oom_منوال_جدول[MANAGED_OOM_منوال_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_جدول_LOOKUP(managed_oom_منوال, ManagedOOMMode);

ساکن ثابت حرف* ثابت managed_oom_preference_جدول[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_جدول_LOOKUP(managed_oom_preference, ManagedOOMPreference);
