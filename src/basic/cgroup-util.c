/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/fs.h>
#अन्तर्भाव <linux/magic.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/xattr.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "capsule-util.h"
#अन्तर्भाव "cgroup-util.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "login-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "set.h"
#अन्तर्भाव "special.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "unaligned.h"
#अन्तर्भाव "unit-name.h"
#अन्तर्भाव "user-util.h"
#अन्तर्भाव "xattr-util.h"

पूर्णाङ्क cg_is_available(शून्य) {
        संरचना statfs fs;

        यदि (statfs("/sys/fs/cgroup/", &fs) < 0) {
                यदि (त्रुटि_सं == ENOENT) /* sysfs not mounted? */
                        प्रत्यागम false;

                प्रत्यागम log_debug_errno(त्रुटि_सं, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        प्रत्यागम is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

पूर्णाङ्क cg_path_open(ध्रुव वर्ण *path) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        पूर्णाङ्क r;

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम RET_NERRNO(खोलो(fs, O_DIRECTORY|O_CLOEXEC));
}

पूर्णाङ्क cg_cgroupid_open(पूर्णाङ्क cgroupfs_fd, uint64_t id) {
        _cleanup_close_ पूर्णाङ्क fsfd = -EBADF;

        यदि (cgroupfs_fd < 0) {
                fsfd = खोलो("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                यदि (fsfd < 0)
                        प्रत्यागम -त्रुटि_सं;

                cgroupfs_fd = fsfd;
        }

        सङ्घ {
                संरचना file_handle file_handle;
                uint8_t space[दुरत्व(संरचना file_handle, f_handle) + परिमाण(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = परिमाण(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        प्रत्यागम RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

पूर्णाङ्क cg_path_from_cgroupid(पूर्णाङ्क cgroupfs_fd, uint64_t id, वर्ण **ret) {
        _cleanup_close_ पूर्णाङ्क cgfd = -EBADF;
        पूर्णाङ्क r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        यदि (cgfd < 0)
                प्रत्यागम cgfd;

        _cleanup_free_ वर्ण *path = रिक्त;
        r = fd_get_path(cgfd, &path);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!path_startswith(path, "/sys/fs/cgroup/"))
                प्रत्यागम -EXDEV; /* recognizable error */

        यदि (ret)
                *ret = TAKE_PTR(path);
        प्रत्यागम 0;
}

पूर्णाङ्क cg_enumerate_processes(ध्रुव वर्ण *path, खाता **ret) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        खाता *f;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        यदि (r < 0)
                प्रत्यागम r;

        f = ख_खोलो(fs, "re");
        यदि (!f)
                प्रत्यागम -त्रुटि_सं;

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_read_pid(खाता *f, pid_t *ret, CGroupFlags flags) {
        अचिन्हित दीर्घ ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        प्रति (;;) {
                त्रुटि_सं = 0;
                यदि (ख_पूछो(f, "%lu", &ul) != 1) {

                        यदि (ख_पूर्ण(f)) {
                                *ret = 0;
                                प्रत्यागम 0;
                        }

                        प्रत्यागम errno_or_else(EIO);
                }

                यदि (ul > PID_T_MAX)
                        प्रत्यागम -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                यदि (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        अनुवर्त;

                *ret = (pid_t) ul;
                प्रत्यागम 1;
        }
}

पूर्णाङ्क cg_read_pidref(खाता *f, PidRef *ret, CGroupFlags flags) {
        पूर्णाङ्क r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        प्रति (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                यदि (r < 0)
                        प्रत्यागम log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                यदि (r == 0) {
                        *ret = PIDREF_NULL;
                        प्रत्यागम 0;
                }

                यदि (pid == 0)
                        प्रत्यागम -EREMOTE;

                r = pidref_set_pid(ret, pid);
                यदि (r >= 0)
                        प्रत्यागम 1;
                यदि (r != -ESRCH)
                        प्रत्यागम r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(शून्य) {
        स्थिर thread_local पूर्णाङ्क supported = -1;

        यदि (supported >= 0)
                प्रत्यागम supported;

        यदि (cg_is_available() <= 0)
                प्रत्यागम (supported = false);

        यदि (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                प्रत्यागम (supported = true);
        यदि (त्रुटि_सं != ENOENT)
                log_debug_errno(त्रुटि_सं, "Failed to check whether cgroup.kill is available, assuming not: %m");
        प्रत्यागम (supported = false);
}

पूर्णाङ्क cg_enumerate_subgroups(ध्रुव वर्ण *path, सूची **ret) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        सूची *d;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        यदि (r < 0)
                प्रत्यागम r;

        d = सूची_खोलो(fs);
        यदि (!d)
                प्रत्यागम -त्रुटि_सं;

        *ret = d;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_read_subgroup(सूची *d, वर्ण **ret) {
        प्रतिज्ञा(d);
        प्रतिज्ञा(ret);

        FOREACH_DIRENT_ALL(de, d, प्रत्यागम -त्रुटि_सं) {
                यदि (de->d_type != DT_DIR)
                        अनुवर्त;

                यदि (dot_or_dot_dot(de->d_name))
                        अनुवर्त;

                प्रत्यागम strdup_to_full(ret, de->d_name);
        }

        *ret = रिक्त;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_kill(
                ध्रुव वर्ण *path,
                पूर्णाङ्क sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                शून्य *userdata) {

        _cleanup_set_free_ Set *allocated_set = रिक्त;
        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(path);
        प्रतिज्ञा(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        यदि (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        यदि (!killed_pids) {
                killed_pids = allocated_set = set_new(रिक्त);
                यदि (!killed_pids)
                        प्रत्यागम -ENOMEM;
        }

        bool done;
        कुरु {
                _cleanup_fclose_ खाता *f = रिक्त;
                पूर्णाङ्क ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                यदि (r == -ENOENT)
                        भङ्ग;
                यदि (r < 0)
                        प्रत्यागम RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                प्रति (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        यदि (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                भङ्ग;
                        }
                        यदि (r < 0)
                                प्रत्यागम RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        यदि (r == 0)
                                भङ्ग;

                        यदि ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                अनुवर्त;

                        यदि (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                अनुवर्त;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        यदि (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                अनुवर्त;
                        }

                        यदि (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        यदि (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        यदि (r >= 0) {
                                यदि (flags & CGROUP_SIGCONT)
                                        (शून्य) pidref_kill(&pidref, SIGCONT);

                                यदि (ret == 0) {
                                        यदि (log_kill)
                                                ret = ret_log_kill;
                                        अन्यथा
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        यदि (r < 0)
                                प्रत्यागम RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } यावत् (!done);

        प्रत्यागम ret;
}

पूर्णाङ्क cg_kill_recursive(
                ध्रुव वर्ण *path,
                पूर्णाङ्क sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                शून्य *userdata) {

        _cleanup_set_free_ Set *allocated_set = रिक्त;
        _cleanup_closedir_ सूची *d = रिक्त;
        पूर्णाङ्क r, ret;

        प्रतिज्ञा(path);
        प्रतिज्ञा(sig >= 0);

        यदि (!killed_pids) {
                killed_pids = allocated_set = set_new(रिक्त);
                यदि (!killed_pids)
                        प्रत्यागम -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        यदि (r < 0) {
                यदि (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                प्रत्यागम ret;
        }

        प्रति (;;) {
                _cleanup_free_ वर्ण *fn = रिक्त, *p = रिक्त;

                r = cg_read_subgroup(d, &fn);
                यदि (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        भङ्ग;
                }
                यदि (r == 0)
                        भङ्ग;

                p = path_join(empty_to_मूल(path), fn);
                यदि (!p)
                        प्रत्यागम -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                यदि (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                यदि (r != 0 && ret >= 0)
                        ret = r;
        }

        प्रत्यागम ret;
}

पूर्णाङ्क cg_kill_kernel_sigkill(ध्रुव वर्ण *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ वर्ण *killfile = रिक्त;
        uint64_t n_pids = UINT64_MAX;
        पूर्णाङ्क r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        प्रतिज्ञा(path);

        यदि (!cg_kill_supported())
                प्रत्यागम -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                यदि (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        यदि (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        प्रत्यागम 0;
}

पूर्णाङ्क cg_get_path(ध्रुव वर्ण *path, ध्रुव वर्ण *suffix, वर्ण **ret) {
        वर्ण *t;

        प्रतिज्ञा(ret);

        यदि (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        यदि (!t)
                प्रत्यागम -ENOMEM;

        *ret = path_simplify(t);
        प्रत्यागम 0;
}

पूर्णाङ्क cg_set_xattr(ध्रुव वर्ण *path, ध्रुव वर्ण *name, ध्रुव शून्य *मूल्य, माप_प्रकार size, पूर्णाङ्क flags) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);
        प्रतिज्ञा(मूल्य || size <= 0);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम RET_NERRNO(setxattr(fs, name, मूल्य, size, flags));
}

पूर्णाङ्क cg_get_xattr(ध्रुव वर्ण *path, ध्रुव वर्ण *name, वर्ण **ret, माप_प्रकार *ret_size) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम lgetxattr_malloc(fs, name, ret, ret_size);
}

पूर्णाङ्क cg_get_xattr_bool(ध्रुव वर्ण *path, ध्रुव वर्ण *name) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

पूर्णाङ्क cg_remove_xattr(ध्रुव वर्ण *path, ध्रुव वर्ण *name) {
        _cleanup_free_ वर्ण *fs = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम RET_NERRNO(removexattr(fs, name));
}

पूर्णाङ्क cg_pid_get_path(pid_t pid, वर्ण **ret_path) {
        _cleanup_fclose_ खाता *f = रिक्त;
        ध्रुव वर्ण *fs;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        यदि (r == -ENOENT)
                प्रत्यागम -ESRCH;
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पंक्ति = रिक्त;
                वर्ण *e;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम -ENODATA;

                e = startswith(पंक्ति, "0:");
                यदि (!e)
                        अनुवर्त;

                e = म_अक्षर(e, ':');
                यदि (!e)
                        अनुवर्त;

                _cleanup_free_ वर्ण *path = strdup(e + 1);
                यदि (!path)
                        प्रत्यागम -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                यदि (startswith(path, "/../"))
                        प्रत्यागम -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                यदि (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                प्रत्यागम 0;
        }
}

पूर्णाङ्क cg_pidref_get_path(ध्रुव PidRef *pidref, वर्ण **ret_path) {
        _cleanup_free_ वर्ण *path = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_path);

        यदि (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;
        यदि (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        यदि (r < 0)
                प्रत्यागम r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        यदि (r < 0)
                प्रत्यागम r;

        *ret_path = TAKE_PTR(path);
        प्रत्यागम 0;
}

पूर्णाङ्क cg_is_empty(ध्रुव वर्ण *path) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        प्रतिज्ञा(path);

        /* The root cgroup is always populated */
        यदि (empty_or_मूल(path))
                प्रत्यागम false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        यदि (r == -ENOENT)
                प्रत्यागम true;
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम streq(t, "0");
}

पूर्णाङ्क cg_split_spec(ध्रुव वर्ण *spec, वर्ण **ret_controller, वर्ण **ret_path) {
        _cleanup_free_ वर्ण *controller = रिक्त;
        ध्रुव वर्ण *path;
        पूर्णाङ्क r;

        प्रतिज्ञा(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        यदि (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                गच्छ finalize;
        }

        ध्रुव वर्ण *e = म_अक्षर(spec, ':');
        यदि (!e) {
                /* Controller only. */
                यदि (ret_controller) {
                        controller = strdup(spec);
                        यदि (!controller)
                                प्रत्यागम -ENOMEM;
                }

                path = रिक्त;
        } अन्यथा {
                /* Both controller and path. */
                यदि (ret_controller) {
                        controller = strndup(spec, e - spec);
                        यदि (!controller)
                                प्रत्यागम -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        यदि (path) {
                /* Non-empty path must be absolute. */
                यदि (!path_is_absolute(path))
                        प्रत्यागम -EINVAL;

                /* Path must not contain dot-dot. */
                यदि (!path_is_safe(path))
                        प्रत्यागम -EINVAL;
        }

        यदि (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        प्रत्यागम 0;
}

पूर्णाङ्क cg_get_मूल_path(वर्ण **ret_path) {
        वर्ण *p, *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_path);

        r = cg_pid_get_path(1, &p);
        यदि (r < 0)
                प्रत्यागम r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        यदि (e)
                *e = 0;

        *ret_path = p;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_shift_path(ध्रुव वर्ण *cgroup, ध्रुव वर्ण *मूल, ध्रुव वर्ण **ret_shifted) {
        पूर्णाङ्क r;

        प्रतिज्ञा(cgroup);
        प्रतिज्ञा(ret_shifted);

        _cleanup_free_ वर्ण *rt = रिक्त;
        यदि (!मूल) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_मूल_path(&rt);
                यदि (r < 0)
                        प्रत्यागम r;

                मूल = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, मूल, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_pid_get_path_shifted(pid_t pid, ध्रुव वर्ण *मूल, वर्ण **ret_cgroup) {
        _cleanup_free_ वर्ण *raw = रिक्त;
        ध्रुव वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        यदि (r < 0)
                प्रत्यागम r;

        r = cg_shift_path(raw, मूल, &c);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                प्रत्यागम 0;
        }

        प्रत्यागम strdup_to(ret_cgroup, c);
}

पूर्णाङ्क cg_pidref_get_path_shifted(ध्रुव PidRef *pidref, ध्रुव वर्ण *मूल, वर्ण **ret_cgroup) {
        _cleanup_free_ वर्ण *raw = रिक्त;
        ध्रुव वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        यदि (r < 0)
                प्रत्यागम r;

        r = cg_shift_path(raw, मूल, &c);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                प्रत्यागम 0;
        }

        प्रत्यागम strdup_to(ret_cgroup, c);
}

पूर्णाङ्क cg_path_decode_unit(ध्रुव वर्ण *cgroup, वर्ण **ret_unit) {
        प्रतिज्ञा(cgroup);

        माप_प्रकार n = म_खोज(cgroup, "/");
        यदि (n < 3)
                प्रत्यागम -ENXIO;

        वर्ण *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        यदि (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                प्रत्यागम -ENXIO;

        यदि (ret_unit)
                प्रत्यागम strdup_to(ret_unit, c);

        प्रत्यागम 0;
}

स्थिर bool valid_slice_name(ध्रुव वर्ण *p, माप_प्रकार n) {
        प्रतिज्ञा(p || n == 0);

        यदि (n < STRLEN("x.slice"))
                प्रत्यागम false;

        वर्ण *c = strndupa_safe(p, n);
        यदि (!endswith(c, ".slice"))
                प्रत्यागम false;

        प्रत्यागम unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

स्थिर ध्रुव वर्ण* skip_slices(ध्रुव वर्ण *p) {
        प्रतिज्ञा(p);

        /* Skips over all slice assignments */

        प्रति (;;) {
                माप_प्रकार n;

                p += म_अखोज(p, "/");

                n = म_खोज(p, "/");
                यदि (!valid_slice_name(p, n))
                        प्रत्यागम p;

                p += n;
        }
}

पूर्णाङ्क cg_path_get_unit_full(ध्रुव वर्ण *path, वर्ण **ret_unit, वर्ण **ret_subgroup) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        ध्रुव वर्ण *e = skip_slices(path);

        _cleanup_free_ वर्ण *unit = रिक्त;
        r = cg_path_decode_unit(e, &unit);
        यदि (r < 0)
                प्रत्यागम r;

        /* We skipped over the slices, don't accept any now */
        यदि (endswith(unit, ".slice"))
                प्रत्यागम -ENXIO;

        यदि (ret_subgroup) {
                _cleanup_free_ वर्ण *subgroup = रिक्त;
                e += म_खोज(e, "/");
                e += म_अखोज(e, "/");

                यदि (isempty(e))
                        subgroup = रिक्त;
                अन्यथा {
                        subgroup = strdup(e);
                        यदि (!subgroup)
                                प्रत्यागम -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        यदि (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        प्रत्यागम 0;
}

पूर्णाङ्क cg_path_get_unit_path(ध्रुव वर्ण *path, वर्ण **ret) {
        _cleanup_free_ वर्ण *path_copy = रिक्त;
        वर्ण *unit_name;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        path_copy = strdup(path);
        यदि (!path_copy)
                प्रत्यागम -ENOMEM;

        unit_name = (वर्ण*) skip_slices(path_copy);
        unit_name[म_खोज(unit_name, "/")] = 0;

        यदि (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                प्रत्यागम -ENXIO;

        *ret = TAKE_PTR(path_copy);

        प्रत्यागम 0;
}

पूर्णाङ्क cg_pid_get_unit_full(pid_t pid, वर्ण **ret_unit, वर्ण **ret_subgroup) {
        पूर्णाङ्क r;

        _cleanup_free_ वर्ण *cgroup = रिक्त;
        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

पूर्णाङ्क cg_pidref_get_unit_full(ध्रुव PidRef *pidref, वर्ण **ret_unit, वर्ण **ret_subgroup) {
        पूर्णाङ्क r;

        यदि (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;
        यदि (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        _cleanup_free_ वर्ण *unit = रिक्त, *subgroup = रिक्त;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        यदि (r < 0)
                प्रत्यागम r;

        r = pidref_verify(pidref);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        यदि (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        प्रत्यागम 0;
}

स्थिर ध्रुव वर्ण* skip_session(ध्रुव वर्ण *p) {
        माप_प्रकार n;

        /* Skip session-*.scope, but require it to be there. */

        यदि (isempty(p))
                प्रत्यागम रिक्त;

        p += म_अखोज(p, "/");

        n = म_खोज(p, "/");
        यदि (n < STRLEN("session-x.scope"))
                प्रत्यागम रिक्त;

        ध्रुव वर्ण *s = startswith(p, "session-");
        यदि (!s)
                प्रत्यागम रिक्त;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        वर्ण *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        यदि (!e)
                प्रत्यागम रिक्त;
        *e = '\0';

        यदि (!session_id_valid(f))
                प्रत्यागम रिक्त;

        प्रत्यागम skip_leading_slash(p + n);
}

स्थिर ध्रुव वर्ण* skip_user_manager(ध्रुव वर्ण *p) {
        माप_प्रकार n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        यदि (isempty(p))
                प्रत्यागम रिक्त;

        p += म_अखोज(p, "/");

        n = म_खोज(p, "/");
        यदि (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                प्रत्यागम रिक्त;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ वर्ण *unit_name = strndup(p, n);
        यदि (!unit_name)
                प्रत्यागम रिक्त;

        _cleanup_free_ वर्ण *i = रिक्त;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        यदि (type != UNIT_NAME_INSTANCE)
                प्रत्यागम रिक्त;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        यदि (!(startswith(unit_name, "user@") && parse_uid(i, रिक्त) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                प्रत्यागम रिक्त;

        प्रत्यागम skip_leading_slash(p + n);
}

स्थिर ध्रुव वर्ण* skip_user_prefix(ध्रुव वर्ण *path) {
        ध्रुव वर्ण *e, *t;

        प्रतिज्ञा(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        यदि (t)
                प्रत्यागम t;

        /* Alternatively skip the user session if it is in the path... */
        प्रत्यागम skip_session(e);
}

पूर्णाङ्क cg_path_get_user_unit_full(ध्रुव वर्ण *path, वर्ण **ret_unit, वर्ण **ret_subgroup) {
        ध्रुव वर्ण *t;

        प्रतिज्ञा(path);

        t = skip_user_prefix(path);
        यदि (!t)
                प्रत्यागम -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        प्रत्यागम cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

पूर्णाङ्क cg_pid_get_user_unit_full(pid_t pid, वर्ण **ret_unit, वर्ण **ret_subgroup) {
        पूर्णाङ्क r;

        _cleanup_free_ वर्ण *cgroup = रिक्त;
        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

पूर्णाङ्क cg_pidref_get_user_unit_full(ध्रुव PidRef *pidref, वर्ण **ret_unit, वर्ण **ret_subgroup) {
        पूर्णाङ्क r;

        यदि (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;
        यदि (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        _cleanup_free_ वर्ण *unit = रिक्त, *subgroup = रिक्त;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        यदि (r < 0)
                प्रत्यागम r;

        r = pidref_verify(pidref);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        यदि (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        प्रत्यागम 0;
}

पूर्णाङ्क cg_path_get_machine_name(ध्रुव वर्ण *path, वर्ण **ret_machine) {
        _cleanup_free_ वर्ण *u = रिक्त;
        ध्रुव वर्ण *sl;
        पूर्णाङ्क r;

        r = cg_path_get_unit(path, &u);
        यदि (r < 0)
                प्रत्यागम r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        प्रत्यागम readlink_malloc(sl, ret_machine);
}

पूर्णाङ्क cg_pid_get_machine_name(pid_t pid, वर्ण **ret_machine) {
        _cleanup_free_ वर्ण *cgroup = रिक्त;
        पूर्णाङ्क r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_machine_name(cgroup, ret_machine);
}

पूर्णाङ्क cg_path_get_session(ध्रुव वर्ण *path, वर्ण **ret_session) {
        _cleanup_free_ वर्ण *unit = रिक्त;
        वर्ण *start, *end;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = cg_path_get_unit(path, &unit);
        यदि (r < 0)
                प्रत्यागम r;

        start = startswith(unit, "session-");
        यदि (!start)
                प्रत्यागम -ENXIO;
        end = endswith(start, ".scope");
        यदि (!end)
                प्रत्यागम -ENXIO;

        *end = 0;
        यदि (!session_id_valid(start))
                प्रत्यागम -ENXIO;

        यदि (!ret_session)
                प्रत्यागम 0;

        प्रत्यागम strdup_to(ret_session, start);
}

पूर्णाङ्क cg_pid_get_session(pid_t pid, वर्ण **ret_session) {
        _cleanup_free_ वर्ण *cgroup = रिक्त;
        पूर्णाङ्क r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_session(cgroup, ret_session);
}

पूर्णाङ्क cg_pidref_get_session(ध्रुव PidRef *pidref, वर्ण **ret) {
        पूर्णाङ्क r;

        यदि (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;
        यदि (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        _cleanup_free_ वर्ण *session = रिक्त;
        r = cg_pid_get_session(pidref->pid, &session);
        यदि (r < 0)
                प्रत्यागम r;

        r = pidref_verify(pidref);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret)
                *ret = TAKE_PTR(session);
        प्रत्यागम 0;
}

पूर्णाङ्क cg_path_get_owner_uid(ध्रुव वर्ण *path, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *slice = रिक्त;
        वर्ण *start, *end;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = cg_path_get_slice(path, &slice);
        यदि (r < 0)
                प्रत्यागम r;

        start = startswith(slice, "user-");
        यदि (!start)
                प्रत्यागम -ENXIO;

        end = endswith(start, ".slice");
        यदि (!end)
                प्रत्यागम -ENXIO;

        *end = 0;
        यदि (parse_uid(start, ret_uid) < 0)
                प्रत्यागम -ENXIO;

        प्रत्यागम 0;
}

पूर्णाङ्क cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *cgroup = रिक्त;
        पूर्णाङ्क r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_owner_uid(cgroup, ret_uid);
}

पूर्णाङ्क cg_pidref_get_owner_uid(ध्रुव PidRef *pidref, uid_t *ret) {
        पूर्णाङ्क r;

        यदि (!pidref_is_set(pidref))
                प्रत्यागम -ESRCH;
        यदि (pidref_is_remote(pidref))
                प्रत्यागम -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        यदि (r < 0)
                प्रत्यागम r;

        r = pidref_verify(pidref);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret)
                *ret = uid;

        प्रत्यागम 0;
}

पूर्णाङ्क cg_path_get_slice(ध्रुव वर्ण *p, वर्ण **ret_slice) {
        ध्रुव वर्ण *e = रिक्त;

        प्रतिज्ञा(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        प्रति (;;) {
                ध्रुव वर्ण *s;
                पूर्णाङ्क n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                यदि (n < 0)
                        प्रत्यागम n;
                यदि (!valid_slice_name(s, n))
                        भङ्ग;

                e = s;
        }

        यदि (e)
                प्रत्यागम cg_path_decode_unit(e, ret_slice);

        यदि (ret_slice)
                प्रत्यागम strdup_to(ret_slice, SPECIAL_मूल_SLICE);

        प्रत्यागम 0;
}

पूर्णाङ्क cg_pid_get_slice(pid_t pid, वर्ण **ret_slice) {
        _cleanup_free_ वर्ण *cgroup = रिक्त;
        पूर्णाङ्क r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_slice(cgroup, ret_slice);
}

पूर्णाङ्क cg_path_get_user_slice(ध्रुव वर्ण *p, वर्ण **ret_slice) {
        ध्रुव वर्ण *t;
        प्रतिज्ञा(p);

        t = skip_user_prefix(p);
        यदि (!t)
                प्रत्यागम -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        प्रत्यागम cg_path_get_slice(t, ret_slice);
}

पूर्णाङ्क cg_pid_get_user_slice(pid_t pid, वर्ण **ret_slice) {
        _cleanup_free_ वर्ण *cgroup = रिक्त;
        पूर्णाङ्क r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(ध्रुव वर्ण *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        यदि (!filename_is_valid(p))
                प्रत्यागम true;

        यदि (IN_SET(p[0], '_', '.'))
                प्रत्यागम true;

        यदि (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                प्रत्यागम true;

        यदि (startswith(p, "cgroup."))
                प्रत्यागम true;

        प्रति (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ध्रुव वर्ण *q;

                q = startswith(p, cgroup_controller_to_string(c));
                यदि (!q)
                        अनुवर्त;

                यदि (q[0] == '.')
                        प्रत्यागम true;
        }

        प्रत्यागम false;
}

पूर्णाङ्क cg_escape(ध्रुव वर्ण *p, वर्ण **ret) {
        _cleanup_free_ वर्ण *n = रिक्त;

        प्रतिज्ञा(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        यदि (cg_needs_escape(p)) {
                n = strjoin("_", p);
                यदि (!n)
                        प्रत्यागम -ENOMEM;

                यदि (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        प्रत्यागम -EINVAL;
        } अन्यथा {
                n = strdup(p);
                यदि (!n)
                        प्रत्यागम -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        प्रत्यागम 0;
}

वर्ण* cg_unescape(ध्रुव वर्ण *p) {
        प्रतिज्ञा(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        यदि (p[0] == '_')
                प्रत्यागम (वर्ण*) p+1;

        प्रत्यागम (वर्ण*) p;
}

पूर्णाङ्क cg_slice_to_path(ध्रुव वर्ण *unit, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त, *s = रिक्त, *e = रिक्त;
        ध्रुव वर्ण *dash;
        पूर्णाङ्क r;

        प्रतिज्ञा(unit);
        प्रतिज्ञा(ret);

        यदि (streq(unit, SPECIAL_मूल_SLICE))
                प्रत्यागम strdup_to(ret, "");

        यदि (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                प्रत्यागम -EINVAL;

        यदि (!endswith(unit, ".slice"))
                प्रत्यागम -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        यदि (r < 0)
                प्रत्यागम r;

        dash = म_अक्षर(p, '-');

        /* Don't allow initial dashes */
        यदि (dash == p)
                प्रत्यागम -EINVAL;

        यावत् (dash) {
                _cleanup_free_ वर्ण *escaped = रिक्त;
                वर्ण n[dash - p + परिमाण(".slice")];

#यदि HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                यदि (IN_SET(dash[1], 0, '-'))
                        प्रत्यागम -EINVAL;

                म_नकल(stpncpy(n, p, dash - p), ".slice");
                यदि (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        प्रत्यागम -EINVAL;

                r = cg_escape(n, &escaped);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (!strextend(&s, escaped, "/"))
                        प्रत्यागम -ENOMEM;

                dash = म_अक्षर(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!strextend(&s, e))
                प्रत्यागम -ENOMEM;

        *ret = TAKE_PTR(s);
        प्रत्यागम 0;
}

पूर्णाङ्क cg_is_threaded(ध्रुव वर्ण *path) {
        _cleanup_free_ वर्ण *fs = रिक्त, *contents = रिक्त;
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        पूर्णाङ्क r;

        r = cg_get_path(path, "cgroup.type", &fs);
        यदि (r < 0)
                प्रत्यागम r;

        r = read_full_virtual_file(fs, &contents, रिक्त);
        यदि (r == -ENOENT)
                प्रत्यागम false; /* Assume no. */
        यदि (r < 0)
                प्रत्यागम r;

        v = strv_split(contents, रिक्त);
        यदि (!v)
                प्रत्यागम -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        प्रत्यागम strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

पूर्णाङ्क cg_set_attribute(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, ध्रुव वर्ण *मूल्य) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(attribute);

        r = cg_get_path(path, attribute, &p);
        यदि (r < 0)
                प्रत्यागम r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        प्रत्यागम write_string_file(p, मूल्य, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

पूर्णाङ्क cg_get_attribute(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(attribute);

        r = cg_get_path(path, attribute, &p);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम read_one_पंक्ति_file(p, ret);
}

पूर्णाङ्क cg_get_attribute_as_uint64(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, uint64_t *ret) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        uint64_t v;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = cg_get_attribute(path, attribute, &मूल्य);
        यदि (r == -ENOENT)
                प्रत्यागम -ENODATA;
        यदि (r < 0)
                प्रत्यागम r;

        यदि (streq(मूल्य, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                प्रत्यागम 0;
        }

        r = safe_atou64(मूल्य, &v);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = v;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_get_attribute_as_bool(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        पूर्णाङ्क r;

        r = cg_get_attribute(path, attribute, &मूल्य);
        यदि (r == -ENOENT)
                प्रत्यागम -ENODATA;
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम parse_boolean(मूल्य);
}

पूर्णाङ्क cg_get_owner(ध्रुव वर्ण *path, uid_t *ret_uid) {
        _cleanup_free_ वर्ण *f = रिक्त;
        संरचना stat stats;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_uid);

        r = cg_get_path(path, /* suffix= */ रिक्त, &f);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (stat(f, &stats) < 0)
                प्रत्यागम -त्रुटि_सं;

        r = stat_verify_directory(&stats);
        यदि (r < 0)
                प्रत्यागम r;

        *ret_uid = stats.st_uid;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_get_keyed_attribute(
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *attribute,
                वर्ण * ध्रुव *कुंजियाँ,
                वर्ण **values) {

        _cleanup_free_ वर्ण *filename = रिक्त, *contents = रिक्त;
        माप_प्रकार n;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        यदि (r < 0)
                प्रत्यागम r;

        r = read_full_file(filename, &contents, /* ret_size= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        n = strv_length(कुंजियाँ);
        यदि (n == 0) /* No keys to retrieve? That's easy, we are done then */
                प्रत्यागम 0;
        प्रतिज्ञा(strv_is_uniq(कुंजियाँ));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        वर्ण **v = newa0(वर्ण*, n);
        माप_प्रकार n_done = 0;

        प्रति (ध्रुव वर्ण *p = contents; *p;) {
                ध्रुव वर्ण *w;
                माप_प्रकार i;

                प्रति (i = 0; i < n; i++) {
                        w = first_word(p, कुंजियाँ[i]);
                        यदि (w)
                                भङ्ग;
                }

                यदि (w) {
                        यदि (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                गच्छ fail;
                        }

                        माप_प्रकार l = म_खोज(w, NEWLINE);

                        v[i] = strndup(w, l);
                        यदि (!v[i]) {
                                r = -ENOMEM;
                                गच्छ fail;
                        }

                        n_done++;
                        यदि (n_done >= n)
                                भङ्ग;

                        p = w + l;
                } अन्यथा
                        p += म_खोज(p, NEWLINE);

                p += म_अखोज(p, NEWLINE);
        }

        यदि (n_done < n) {
                r = -ENXIO;
                गच्छ fail;
        }

        स_नकल(values, v, परिमाण(वर्ण*) * n);
        प्रत्यागम 0;

fail:
        free_many_charp(v, n);
        प्रत्यागम r;
}

पूर्णाङ्क cg_get_keyed_attribute_uint64(ध्रुव वर्ण *path, ध्रुव वर्ण *attribute, ध्रुव वर्ण *कुंजी, uint64_t *ret) {
        _cleanup_free_ वर्ण *val = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुंजी);
        प्रतिज्ञा(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(कुंजी), &val);
        यदि (r < 0)
                प्रत्यागम r;

        r = safe_atou64(val, ret);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, कुंजी, attribute);

        प्रत्यागम 0;
}

पूर्णाङ्क cg_mask_to_string(CGroupMask mask, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        bool space = false;
        CGroupController c;
        माप_प्रकार n = 0;

        प्रतिज्ञा(ret);

        यदि (mask == 0) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        प्रति (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                ध्रुव वर्ण *k;
                माप_प्रकार l;

                यदि (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        अनुवर्त;

                k = cgroup_controller_to_string(c);
                l = म_माप(k);

                यदि (!GREEDY_REALLOC(s, n + space + l + 1))
                        प्रत्यागम -ENOMEM;

                यदि (space)
                        s[n] = ' ';
                स_नकल(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        प्रतिज्ञा(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        प्रत्यागम 0;
}

पूर्णाङ्क cg_mask_from_string(ध्रुव वर्ण *s, CGroupMask *ret) {
        CGroupMask m = 0;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(s);

        प्रति (;;) {
                _cleanup_free_ वर्ण *n = रिक्त;
                CGroupController v;
                पूर्णाङ्क r;

                r = extract_first_word(&s, &n, रिक्त, 0);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                v = cgroup_controller_from_string(n);
                यदि (v < 0)
                        अनुवर्त;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_mask_supported_subtree(ध्रुव वर्ण *मूल, CGroupMask *ret) {
        CGroupMask mask;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ वर्ण *controllers = रिक्त, *path = रिक्त;
        r = cg_get_path(मूल, "cgroup.controllers", &path);
        यदि (r < 0)
                प्रत्यागम r;

        r = read_one_पंक्ति_file(path, &controllers);
        यदि (r < 0)
                प्रत्यागम r;

        r = cg_mask_from_string(controllers, &mask);
        यदि (r < 0)
                प्रत्यागम r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        प्रत्यागम 0;
}

पूर्णाङ्क cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ वर्ण *मूल = रिक्त;
        पूर्णाङ्क r;

        r = cg_get_मूल_path(&मूल);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cg_mask_supported_subtree(मूल, ret);
}

पूर्णाङ्क cg_is_delegated(ध्रुव वर्ण *path) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        यदि (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                प्रत्यागम r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        प्रत्यागम ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

पूर्णाङ्क cg_is_delegated_fd(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ रिक्त, "trusted.delegate", /* at_flags= */ 0);
        यदि (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                प्रत्यागम r;

        r = getxattr_at_bool(fd, /* path= */ रिक्त, "user.delegate", /* at_flags= */ 0);
        प्रत्यागम ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

पूर्णाङ्क cg_has_coredump_receive(ध्रुव वर्ण *path) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        यदि (ERRNO_IS_NEG_XATTR_ABSENT(r))
                प्रत्यागम false;

        प्रत्यागम r;
}

ध्रुव uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

स्थिर ध्रुव वर्ण* ध्रुव cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

शून्य cgroup_io_limits_list(शून्य) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

स्थिर ध्रुव वर्ण *ध्रुव cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

स्थिर ध्रुव वर्ण* ध्रुव managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

स्थिर ध्रुव वर्ण* ध्रुव managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
