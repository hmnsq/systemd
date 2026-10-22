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

पूर्णांक cg_is_available(शून्य) {
        संरचना statfs fs;

        जर (statfs("/sys/fs/cgroup/", &fs) < 0) {
                जर (त्रुटि_सं == ENOENT) /* sysfs not mounted? */
                        परत false;

                परत log_debug_errno(त्रुटि_सं, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        परत is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

पूर्णांक cg_path_open(स्थिरांक अक्षर *path) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        पूर्णांक r;

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        जर (r < 0)
                परत r;

        परत RET_NERRNO(खोलो(fs, O_DIRECTORY|O_CLOEXEC));
}

पूर्णांक cg_cgroupid_open(पूर्णांक cgroupfs_fd, uint64_t id) {
        _cleanup_close_ पूर्णांक fsfd = -EBADF;

        जर (cgroupfs_fd < 0) {
                fsfd = खोलो("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                जर (fsfd < 0)
                        परत -त्रुटि_सं;

                cgroupfs_fd = fsfd;
        }

        संघ {
                संरचना file_handle file_handle;
                uint8_t space[दुरत्व(संरचना file_handle, f_handle) + आकार(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = आकार(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        परत RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

पूर्णांक cg_path_from_cgroupid(पूर्णांक cgroupfs_fd, uint64_t id, अक्षर **ret) {
        _cleanup_close_ पूर्णांक cgfd = -EBADF;
        पूर्णांक r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        जर (cgfd < 0)
                परत cgfd;

        _cleanup_free_ अक्षर *path = रिक्त;
        r = fd_get_path(cgfd, &path);
        जर (r < 0)
                परत r;

        जर (!path_startswith(path, "/sys/fs/cgroup/"))
                परत -EXDEV; /* recognizable error */

        जर (ret)
                *ret = TAKE_PTR(path);
        परत 0;
}

पूर्णांक cg_enumerate_processes(स्थिरांक अक्षर *path, खाता **ret) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        खाता *f;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        जर (r < 0)
                परत r;

        f = ख_खोलो(fs, "re");
        जर (!f)
                परत -त्रुटि_सं;

        *ret = f;
        परत 0;
}

पूर्णांक cg_read_pid(खाता *f, pid_t *ret, CGroupFlags flags) {
        अचिन्हित दीर्घ ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        साठी (;;) {
                त्रुटि_सं = 0;
                जर (ख_पूछो(f, "%lu", &ul) != 1) {

                        जर (ख_पूर्ण(f)) {
                                *ret = 0;
                                परत 0;
                        }

                        परत errno_or_else(EIO);
                }

                जर (ul > PID_T_MAX)
                        परत -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                जर (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        सुरूठेवा;

                *ret = (pid_t) ul;
                परत 1;
        }
}

पूर्णांक cg_read_pidref(खाता *f, PidRef *ret, CGroupFlags flags) {
        पूर्णांक r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        साठी (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                जर (r < 0)
                        परत log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                जर (r == 0) {
                        *ret = PIDREF_NULL;
                        परत 0;
                }

                जर (pid == 0)
                        परत -EREMOTE;

                r = pidref_set_pid(ret, pid);
                जर (r >= 0)
                        परत 1;
                जर (r != -ESRCH)
                        परत r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(शून्य) {
        स्थिर thread_local पूर्णांक supported = -1;

        जर (supported >= 0)
                परत supported;

        जर (cg_is_available() <= 0)
                परत (supported = false);

        जर (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                परत (supported = true);
        जर (त्रुटि_सं != ENOENT)
                log_debug_errno(त्रुटि_सं, "Failed to check whether cgroup.kill is available, assuming not: %m");
        परत (supported = false);
}

पूर्णांक cg_enumerate_subgroups(स्थिरांक अक्षर *path, सूची **ret) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        सूची *d;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        जर (r < 0)
                परत r;

        d = सूची_खोलो(fs);
        जर (!d)
                परत -त्रुटि_सं;

        *ret = d;
        परत 0;
}

पूर्णांक cg_read_subgroup(सूची *d, अक्षर **ret) {
        प्रतिज्ञा(d);
        प्रतिज्ञा(ret);

        FOREACH_DIRENT_ALL(de, d, परत -त्रुटि_सं) {
                जर (de->d_type != DT_DIR)
                        सुरूठेवा;

                जर (dot_or_dot_dot(de->d_name))
                        सुरूठेवा;

                परत strdup_to_full(ret, de->d_name);
        }

        *ret = रिक्त;
        परत 0;
}

पूर्णांक cg_kill(
                स्थिरांक अक्षर *path,
                पूर्णांक sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                शून्य *userdata) {

        _cleanup_set_free_ Set *allocated_set = रिक्त;
        पूर्णांक r, ret = 0;

        प्रतिज्ञा(path);
        प्रतिज्ञा(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        जर (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        जर (!killed_pids) {
                killed_pids = allocated_set = set_new(रिक्त);
                जर (!killed_pids)
                        परत -ENOMEM;
        }

        bool done;
        करा {
                _cleanup_fclose_ खाता *f = रिक्त;
                पूर्णांक ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                जर (r == -ENOENT)
                        थांबा;
                जर (r < 0)
                        परत RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                साठी (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        जर (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                थांबा;
                        }
                        जर (r < 0)
                                परत RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        जर (r == 0)
                                थांबा;

                        जर ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                सुरूठेवा;

                        जर (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                सुरूठेवा;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        जर (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                सुरूठेवा;
                        }

                        जर (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        जर (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        जर (r >= 0) {
                                जर (flags & CGROUP_SIGCONT)
                                        (शून्य) pidref_kill(&pidref, SIGCONT);

                                जर (ret == 0) {
                                        जर (log_kill)
                                                ret = ret_log_kill;
                                        अन्यथा
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        जर (r < 0)
                                परत RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } पर्यंत (!done);

        परत ret;
}

पूर्णांक cg_kill_recursive(
                स्थिरांक अक्षर *path,
                पूर्णांक sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                शून्य *userdata) {

        _cleanup_set_free_ Set *allocated_set = रिक्त;
        _cleanup_closedir_ सूची *d = रिक्त;
        पूर्णांक r, ret;

        प्रतिज्ञा(path);
        प्रतिज्ञा(sig >= 0);

        जर (!killed_pids) {
                killed_pids = allocated_set = set_new(रिक्त);
                जर (!killed_pids)
                        परत -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        जर (r < 0) {
                जर (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                परत ret;
        }

        साठी (;;) {
                _cleanup_free_ अक्षर *fn = रिक्त, *p = रिक्त;

                r = cg_read_subgroup(d, &fn);
                जर (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        थांबा;
                }
                जर (r == 0)
                        थांबा;

                p = path_join(empty_to_मूळ(path), fn);
                जर (!p)
                        परत -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                जर (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                जर (r != 0 && ret >= 0)
                        ret = r;
        }

        परत ret;
}

पूर्णांक cg_kill_kernel_sigkill(स्थिरांक अक्षर *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ अक्षर *killfile = रिक्त;
        uint64_t n_pids = UINT64_MAX;
        पूर्णांक r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        प्रतिज्ञा(path);

        जर (!cg_kill_supported())
                परत -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        जर (r < 0)
                परत r;

        जर (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                जर (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        जर (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        परत 0;
}

पूर्णांक cg_get_path(स्थिरांक अक्षर *path, स्थिरांक अक्षर *suffix, अक्षर **ret) {
        अक्षर *t;

        प्रतिज्ञा(ret);

        जर (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        जर (!t)
                परत -ENOMEM;

        *ret = path_simplify(t);
        परत 0;
}

पूर्णांक cg_set_xattr(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, स्थिरांक शून्य *मूल्य, माप_प्रकार size, पूर्णांक flags) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);
        प्रतिज्ञा(मूल्य || size <= 0);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        जर (r < 0)
                परत r;

        परत RET_NERRNO(setxattr(fs, name, मूल्य, size, flags));
}

पूर्णांक cg_get_xattr(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, अक्षर **ret, माप_प्रकार *ret_size) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        जर (r < 0)
                परत r;

        परत lgetxattr_malloc(fs, name, ret, ret_size);
}

पूर्णांक cg_get_xattr_bool(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        जर (r < 0)
                परत r;

        परत getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

पूर्णांक cg_remove_xattr(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name) {
        _cleanup_free_ अक्षर *fs = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(name);

        r = cg_get_path(path, /* suffix= */ रिक्त, &fs);
        जर (r < 0)
                परत r;

        परत RET_NERRNO(removexattr(fs, name));
}

पूर्णांक cg_pid_get_path(pid_t pid, अक्षर **ret_path) {
        _cleanup_fclose_ खाता *f = रिक्त;
        स्थिरांक अक्षर *fs;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        जर (r == -ENOENT)
                परत -ESRCH;
        जर (r < 0)
                परत r;

        साठी (;;) {
                _cleanup_free_ अक्षर *ओळ = रिक्त;
                अक्षर *e;

                r = read_ओळ(f, LONG_ओळ_MAX, &ओळ);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        परत -ENODATA;

                e = startswith(ओळ, "0:");
                जर (!e)
                        सुरूठेवा;

                e = म_अक्षर(e, ':');
                जर (!e)
                        सुरूठेवा;

                _cleanup_free_ अक्षर *path = strdup(e + 1);
                जर (!path)
                        परत -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                जर (startswith(path, "/../"))
                        परत -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                जर (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                परत 0;
        }
}

पूर्णांक cg_pidref_get_path(स्थिरांक PidRef *pidref, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *path = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret_path);

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        जर (r < 0)
                परत r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        *ret_path = TAKE_PTR(path);
        परत 0;
}

पूर्णांक cg_is_empty(स्थिरांक अक्षर *path) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        प्रतिज्ञा(path);

        /* The root cgroup is always populated */
        जर (empty_or_मूळ(path))
                परत false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        जर (r == -ENOENT)
                परत true;
        जर (r < 0)
                परत r;

        परत streq(t, "0");
}

पूर्णांक cg_split_spec(स्थिरांक अक्षर *spec, अक्षर **ret_controller, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *controller = रिक्त;
        स्थिरांक अक्षर *path;
        पूर्णांक r;

        प्रतिज्ञा(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        जर (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                जा finalize;
        }

        स्थिरांक अक्षर *e = म_अक्षर(spec, ':');
        जर (!e) {
                /* Controller only. */
                जर (ret_controller) {
                        controller = strdup(spec);
                        जर (!controller)
                                परत -ENOMEM;
                }

                path = रिक्त;
        } अन्यथा {
                /* Both controller and path. */
                जर (ret_controller) {
                        controller = strndup(spec, e - spec);
                        जर (!controller)
                                परत -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        जर (path) {
                /* Non-empty path must be absolute. */
                जर (!path_is_absolute(path))
                        परत -EINVAL;

                /* Path must not contain dot-dot. */
                जर (!path_is_safe(path))
                        परत -EINVAL;
        }

        जर (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                जर (r < 0)
                        परत r;
        }

        जर (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        परत 0;
}

पूर्णांक cg_get_मूळ_path(अक्षर **ret_path) {
        अक्षर *p, *e;
        पूर्णांक r;

        प्रतिज्ञा(ret_path);

        r = cg_pid_get_path(1, &p);
        जर (r < 0)
                परत r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        जर (e)
                *e = 0;

        *ret_path = p;
        परत 0;
}

पूर्णांक cg_shift_path(स्थिरांक अक्षर *cgroup, स्थिरांक अक्षर *मूळ, स्थिरांक अक्षर **ret_shifted) {
        पूर्णांक r;

        प्रतिज्ञा(cgroup);
        प्रतिज्ञा(ret_shifted);

        _cleanup_free_ अक्षर *rt = रिक्त;
        जर (!मूळ) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_मूळ_path(&rt);
                जर (r < 0)
                        परत r;

                मूळ = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, मूळ, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        परत 0;
}

पूर्णांक cg_pid_get_path_shifted(pid_t pid, स्थिरांक अक्षर *मूळ, अक्षर **ret_cgroup) {
        _cleanup_free_ अक्षर *raw = रिक्त;
        स्थिरांक अक्षर *c;
        पूर्णांक r;

        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        जर (r < 0)
                परत r;

        r = cg_shift_path(raw, मूळ, &c);
        जर (r < 0)
                परत r;

        जर (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                परत 0;
        }

        परत strdup_to(ret_cgroup, c);
}

पूर्णांक cg_pidref_get_path_shifted(स्थिरांक PidRef *pidref, स्थिरांक अक्षर *मूळ, अक्षर **ret_cgroup) {
        _cleanup_free_ अक्षर *raw = रिक्त;
        स्थिरांक अक्षर *c;
        पूर्णांक r;

        प्रतिज्ञा(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        जर (r < 0)
                परत r;

        r = cg_shift_path(raw, मूळ, &c);
        जर (r < 0)
                परत r;

        जर (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                परत 0;
        }

        परत strdup_to(ret_cgroup, c);
}

पूर्णांक cg_path_decode_unit(स्थिरांक अक्षर *cgroup, अक्षर **ret_unit) {
        प्रतिज्ञा(cgroup);

        माप_प्रकार n = म_खोज(cgroup, "/");
        जर (n < 3)
                परत -ENXIO;

        अक्षर *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        जर (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                परत -ENXIO;

        जर (ret_unit)
                परत strdup_to(ret_unit, c);

        परत 0;
}

स्थिर bool valid_slice_name(स्थिरांक अक्षर *p, माप_प्रकार n) {
        प्रतिज्ञा(p || n == 0);

        जर (n < STRLEN("x.slice"))
                परत false;

        अक्षर *c = strndupa_safe(p, n);
        जर (!endswith(c, ".slice"))
                परत false;

        परत unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

स्थिर स्थिरांक अक्षर* skip_slices(स्थिरांक अक्षर *p) {
        प्रतिज्ञा(p);

        /* Skips over all slice assignments */

        साठी (;;) {
                माप_प्रकार n;

                p += म_अखोज(p, "/");

                n = म_खोज(p, "/");
                जर (!valid_slice_name(p, n))
                        परत p;

                p += n;
        }
}

पूर्णांक cg_path_get_unit_full(स्थिरांक अक्षर *path, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        प्रतिज्ञा(path);

        स्थिरांक अक्षर *e = skip_slices(path);

        _cleanup_free_ अक्षर *unit = रिक्त;
        r = cg_path_decode_unit(e, &unit);
        जर (r < 0)
                परत r;

        /* We skipped over the slices, don't accept any now */
        जर (endswith(unit, ".slice"))
                परत -ENXIO;

        जर (ret_subgroup) {
                _cleanup_free_ अक्षर *subgroup = रिक्त;
                e += म_खोज(e, "/");
                e += म_अखोज(e, "/");

                जर (isempty(e))
                        subgroup = रिक्त;
                अन्यथा {
                        subgroup = strdup(e);
                        जर (!subgroup)
                                परत -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        जर (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        परत 0;
}

पूर्णांक cg_path_get_unit_path(स्थिरांक अक्षर *path, अक्षर **ret) {
        _cleanup_free_ अक्षर *path_copy = रिक्त;
        अक्षर *unit_name;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        path_copy = strdup(path);
        जर (!path_copy)
                परत -ENOMEM;

        unit_name = (अक्षर*) skip_slices(path_copy);
        unit_name[म_खोज(unit_name, "/")] = 0;

        जर (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                परत -ENXIO;

        *ret = TAKE_PTR(path_copy);

        परत 0;
}

पूर्णांक cg_pid_get_unit_full(pid_t pid, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        _cleanup_free_ अक्षर *cgroup = रिक्त;
        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

पूर्णांक cg_pidref_get_unit_full(स्थिरांक PidRef *pidref, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        _cleanup_free_ अक्षर *unit = रिक्त, *subgroup = रिक्त;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        जर (r < 0)
                परत r;

        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        जर (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        जर (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        परत 0;
}

स्थिर स्थिरांक अक्षर* skip_session(स्थिरांक अक्षर *p) {
        माप_प्रकार n;

        /* Skip session-*.scope, but require it to be there. */

        जर (isempty(p))
                परत रिक्त;

        p += म_अखोज(p, "/");

        n = म_खोज(p, "/");
        जर (n < STRLEN("session-x.scope"))
                परत रिक्त;

        स्थिरांक अक्षर *s = startswith(p, "session-");
        जर (!s)
                परत रिक्त;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        अक्षर *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        जर (!e)
                परत रिक्त;
        *e = '\0';

        जर (!session_id_valid(f))
                परत रिक्त;

        परत skip_leading_slash(p + n);
}

स्थिर स्थिरांक अक्षर* skip_user_manager(स्थिरांक अक्षर *p) {
        माप_प्रकार n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        जर (isempty(p))
                परत रिक्त;

        p += म_अखोज(p, "/");

        n = म_खोज(p, "/");
        जर (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                परत रिक्त;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ अक्षर *unit_name = strndup(p, n);
        जर (!unit_name)
                परत रिक्त;

        _cleanup_free_ अक्षर *i = रिक्त;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        जर (type != UNIT_NAME_INSTANCE)
                परत रिक्त;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        जर (!(startswith(unit_name, "user@") && parse_uid(i, रिक्त) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                परत रिक्त;

        परत skip_leading_slash(p + n);
}

स्थिर स्थिरांक अक्षर* skip_user_prefix(स्थिरांक अक्षर *path) {
        स्थिरांक अक्षर *e, *t;

        प्रतिज्ञा(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        जर (t)
                परत t;

        /* Alternatively skip the user session if it is in the path... */
        परत skip_session(e);
}

पूर्णांक cg_path_get_user_unit_full(स्थिरांक अक्षर *path, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        स्थिरांक अक्षर *t;

        प्रतिज्ञा(path);

        t = skip_user_prefix(path);
        जर (!t)
                परत -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        परत cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

पूर्णांक cg_pid_get_user_unit_full(pid_t pid, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        _cleanup_free_ अक्षर *cgroup = रिक्त;
        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

पूर्णांक cg_pidref_get_user_unit_full(स्थिरांक PidRef *pidref, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        _cleanup_free_ अक्षर *unit = रिक्त, *subgroup = रिक्त;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        जर (r < 0)
                परत r;

        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        जर (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        जर (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        परत 0;
}

पूर्णांक cg_path_get_machine_name(स्थिरांक अक्षर *path, अक्षर **ret_machine) {
        _cleanup_free_ अक्षर *u = रिक्त;
        स्थिरांक अक्षर *sl;
        पूर्णांक r;

        r = cg_path_get_unit(path, &u);
        जर (r < 0)
                परत r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        परत readlink_malloc(sl, ret_machine);
}

पूर्णांक cg_pid_get_machine_name(pid_t pid, अक्षर **ret_machine) {
        _cleanup_free_ अक्षर *cgroup = रिक्त;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_machine_name(cgroup, ret_machine);
}

पूर्णांक cg_path_get_session(स्थिरांक अक्षर *path, अक्षर **ret_session) {
        _cleanup_free_ अक्षर *unit = रिक्त;
        अक्षर *start, *end;
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = cg_path_get_unit(path, &unit);
        जर (r < 0)
                परत r;

        start = startswith(unit, "session-");
        जर (!start)
                परत -ENXIO;
        end = endswith(start, ".scope");
        जर (!end)
                परत -ENXIO;

        *end = 0;
        जर (!session_id_valid(start))
                परत -ENXIO;

        जर (!ret_session)
                परत 0;

        परत strdup_to(ret_session, start);
}

पूर्णांक cg_pid_get_session(pid_t pid, अक्षर **ret_session) {
        _cleanup_free_ अक्षर *cgroup = रिक्त;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_session(cgroup, ret_session);
}

पूर्णांक cg_pidref_get_session(स्थिरांक PidRef *pidref, अक्षर **ret) {
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        _cleanup_free_ अक्षर *session = रिक्त;
        r = cg_pid_get_session(pidref->pid, &session);
        जर (r < 0)
                परत r;

        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = TAKE_PTR(session);
        परत 0;
}

पूर्णांक cg_path_get_owner_uid(स्थिरांक अक्षर *path, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *slice = रिक्त;
        अक्षर *start, *end;
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = cg_path_get_slice(path, &slice);
        जर (r < 0)
                परत r;

        start = startswith(slice, "user-");
        जर (!start)
                परत -ENXIO;

        end = endswith(start, ".slice");
        जर (!end)
                परत -ENXIO;

        *end = 0;
        जर (parse_uid(start, ret_uid) < 0)
                परत -ENXIO;

        परत 0;
}

पूर्णांक cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *cgroup = रिक्त;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_owner_uid(cgroup, ret_uid);
}

पूर्णांक cg_pidref_get_owner_uid(स्थिरांक PidRef *pidref, uid_t *ret) {
        पूर्णांक r;

        जर (!pidref_is_set(pidref))
                परत -ESRCH;
        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        जर (r < 0)
                परत r;

        r = pidref_verify(pidref);
        जर (r < 0)
                परत r;

        जर (ret)
                *ret = uid;

        परत 0;
}

पूर्णांक cg_path_get_slice(स्थिरांक अक्षर *p, अक्षर **ret_slice) {
        स्थिरांक अक्षर *e = रिक्त;

        प्रतिज्ञा(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        साठी (;;) {
                स्थिरांक अक्षर *s;
                पूर्णांक n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                जर (n < 0)
                        परत n;
                जर (!valid_slice_name(s, n))
                        थांबा;

                e = s;
        }

        जर (e)
                परत cg_path_decode_unit(e, ret_slice);

        जर (ret_slice)
                परत strdup_to(ret_slice, SPECIAL_मूळ_SLICE);

        परत 0;
}

पूर्णांक cg_pid_get_slice(pid_t pid, अक्षर **ret_slice) {
        _cleanup_free_ अक्षर *cgroup = रिक्त;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_slice(cgroup, ret_slice);
}

पूर्णांक cg_path_get_user_slice(स्थिरांक अक्षर *p, अक्षर **ret_slice) {
        स्थिरांक अक्षर *t;
        प्रतिज्ञा(p);

        t = skip_user_prefix(p);
        जर (!t)
                परत -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        परत cg_path_get_slice(t, ret_slice);
}

पूर्णांक cg_pid_get_user_slice(pid_t pid, अक्षर **ret_slice) {
        _cleanup_free_ अक्षर *cgroup = रिक्त;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, रिक्त, &cgroup);
        जर (r < 0)
                परत r;

        परत cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(स्थिरांक अक्षर *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        जर (!filename_is_valid(p))
                परत true;

        जर (IN_SET(p[0], '_', '.'))
                परत true;

        जर (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                परत true;

        जर (startswith(p, "cgroup."))
                परत true;

        साठी (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                स्थिरांक अक्षर *q;

                q = startswith(p, cgroup_controller_to_string(c));
                जर (!q)
                        सुरूठेवा;

                जर (q[0] == '.')
                        परत true;
        }

        परत false;
}

पूर्णांक cg_escape(स्थिरांक अक्षर *p, अक्षर **ret) {
        _cleanup_free_ अक्षर *n = रिक्त;

        प्रतिज्ञा(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        जर (cg_needs_escape(p)) {
                n = strjoin("_", p);
                जर (!n)
                        परत -ENOMEM;

                जर (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        परत -EINVAL;
        } अन्यथा {
                n = strdup(p);
                जर (!n)
                        परत -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        परत 0;
}

अक्षर* cg_unescape(स्थिरांक अक्षर *p) {
        प्रतिज्ञा(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        जर (p[0] == '_')
                परत (अक्षर*) p+1;

        परत (अक्षर*) p;
}

पूर्णांक cg_slice_to_path(स्थिरांक अक्षर *unit, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त, *s = रिक्त, *e = रिक्त;
        स्थिरांक अक्षर *dash;
        पूर्णांक r;

        प्रतिज्ञा(unit);
        प्रतिज्ञा(ret);

        जर (streq(unit, SPECIAL_मूळ_SLICE))
                परत strdup_to(ret, "");

        जर (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                परत -EINVAL;

        जर (!endswith(unit, ".slice"))
                परत -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        जर (r < 0)
                परत r;

        dash = म_अक्षर(p, '-');

        /* Don't allow initial dashes */
        जर (dash == p)
                परत -EINVAL;

        पर्यंत (dash) {
                _cleanup_free_ अक्षर *escaped = रिक्त;
                अक्षर n[dash - p + आकार(".slice")];

#जर HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                जर (IN_SET(dash[1], 0, '-'))
                        परत -EINVAL;

                म_नकल(stpncpy(n, p, dash - p), ".slice");
                जर (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        परत -EINVAL;

                r = cg_escape(n, &escaped);
                जर (r < 0)
                        परत r;

                जर (!strextend(&s, escaped, "/"))
                        परत -ENOMEM;

                dash = म_अक्षर(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        जर (r < 0)
                परत r;

        जर (!strextend(&s, e))
                परत -ENOMEM;

        *ret = TAKE_PTR(s);
        परत 0;
}

पूर्णांक cg_is_threaded(स्थिरांक अक्षर *path) {
        _cleanup_free_ अक्षर *fs = रिक्त, *contents = रिक्त;
        _cleanup_strv_free_ अक्षर **v = रिक्त;
        पूर्णांक r;

        r = cg_get_path(path, "cgroup.type", &fs);
        जर (r < 0)
                परत r;

        r = read_full_virtual_file(fs, &contents, रिक्त);
        जर (r == -ENOENT)
                परत false; /* Assume no. */
        जर (r < 0)
                परत r;

        v = strv_split(contents, रिक्त);
        जर (!v)
                परत -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        परत strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

पूर्णांक cg_set_attribute(स्थिरांक अक्षर *path, स्थिरांक अक्षर *attribute, स्थिरांक अक्षर *मूल्य) {
        _cleanup_free_ अक्षर *p = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(attribute);

        r = cg_get_path(path, attribute, &p);
        जर (r < 0)
                परत r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        परत write_string_file(p, मूल्य, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

पूर्णांक cg_get_attribute(स्थिरांक अक्षर *path, स्थिरांक अक्षर *attribute, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(attribute);

        r = cg_get_path(path, attribute, &p);
        जर (r < 0)
                परत r;

        परत read_one_ओळ_file(p, ret);
}

पूर्णांक cg_get_attribute_as_uint64(स्थिरांक अक्षर *path, स्थिरांक अक्षर *attribute, uint64_t *ret) {
        _cleanup_free_ अक्षर *मूल्य = रिक्त;
        uint64_t v;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = cg_get_attribute(path, attribute, &मूल्य);
        जर (r == -ENOENT)
                परत -ENODATA;
        जर (r < 0)
                परत r;

        जर (streq(मूल्य, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                परत 0;
        }

        r = safe_atou64(मूल्य, &v);
        जर (r < 0)
                परत r;

        *ret = v;
        परत 0;
}

पूर्णांक cg_get_attribute_as_bool(स्थिरांक अक्षर *path, स्थिरांक अक्षर *attribute) {
        _cleanup_free_ अक्षर *मूल्य = रिक्त;
        पूर्णांक r;

        r = cg_get_attribute(path, attribute, &मूल्य);
        जर (r == -ENOENT)
                परत -ENODATA;
        जर (r < 0)
                परत r;

        परत parse_boolean(मूल्य);
}

पूर्णांक cg_get_owner(स्थिरांक अक्षर *path, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *f = रिक्त;
        संरचना stat stats;
        पूर्णांक r;

        प्रतिज्ञा(ret_uid);

        r = cg_get_path(path, /* suffix= */ रिक्त, &f);
        जर (r < 0)
                परत r;

        जर (stat(f, &stats) < 0)
                परत -त्रुटि_सं;

        r = stat_verify_directory(&stats);
        जर (r < 0)
                परत r;

        *ret_uid = stats.st_uid;
        परत 0;
}

पूर्णांक cg_get_keyed_attribute(
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *attribute,
                अक्षर * स्थिरांक *किल्ल्या,
                अक्षर **values) {

        _cleanup_free_ अक्षर *filename = रिक्त, *contents = रिक्त;
        माप_प्रकार n;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        जर (r < 0)
                परत r;

        r = read_full_file(filename, &contents, /* ret_size= */ रिक्त);
        जर (r < 0)
                परत r;

        n = strv_length(किल्ल्या);
        जर (n == 0) /* No keys to retrieve? That's easy, we are done then */
                परत 0;
        प्रतिज्ञा(strv_is_uniq(किल्ल्या));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        अक्षर **v = newa0(अक्षर*, n);
        माप_प्रकार n_done = 0;

        साठी (स्थिरांक अक्षर *p = contents; *p;) {
                स्थिरांक अक्षर *w;
                माप_प्रकार i;

                साठी (i = 0; i < n; i++) {
                        w = first_word(p, किल्ल्या[i]);
                        जर (w)
                                थांबा;
                }

                जर (w) {
                        जर (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                जा fail;
                        }

                        माप_प्रकार l = म_खोज(w, NEWLINE);

                        v[i] = strndup(w, l);
                        जर (!v[i]) {
                                r = -ENOMEM;
                                जा fail;
                        }

                        n_done++;
                        जर (n_done >= n)
                                थांबा;

                        p = w + l;
                } अन्यथा
                        p += म_खोज(p, NEWLINE);

                p += म_अखोज(p, NEWLINE);
        }

        जर (n_done < n) {
                r = -ENXIO;
                जा fail;
        }

        स_नकल(values, v, आकार(अक्षर*) * n);
        परत 0;

fail:
        free_many_charp(v, n);
        परत r;
}

पूर्णांक cg_get_keyed_attribute_uint64(स्थिरांक अक्षर *path, स्थिरांक अक्षर *attribute, स्थिरांक अक्षर *किल्ली, uint64_t *ret) {
        _cleanup_free_ अक्षर *val = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(किल्ली);
        प्रतिज्ञा(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(किल्ली), &val);
        जर (r < 0)
                परत r;

        r = safe_atou64(val, ret);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, किल्ली, attribute);

        परत 0;
}

पूर्णांक cg_mask_to_string(CGroupMask mask, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        bool space = false;
        CGroupController c;
        माप_प्रकार n = 0;

        प्रतिज्ञा(ret);

        जर (mask == 0) {
                *ret = रिक्त;
                परत 0;
        }

        साठी (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                स्थिरांक अक्षर *k;
                माप_प्रकार l;

                जर (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        सुरूठेवा;

                k = cgroup_controller_to_string(c);
                l = म_माप(k);

                जर (!GREEDY_REALLOC(s, n + space + l + 1))
                        परत -ENOMEM;

                जर (space)
                        s[n] = ' ';
                स_नकल(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        प्रतिज्ञा(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        परत 0;
}

पूर्णांक cg_mask_from_string(स्थिरांक अक्षर *s, CGroupMask *ret) {
        CGroupMask m = 0;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(s);

        साठी (;;) {
                _cleanup_free_ अक्षर *n = रिक्त;
                CGroupController v;
                पूर्णांक r;

                r = extract_first_word(&s, &n, रिक्त, 0);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                v = cgroup_controller_from_string(n);
                जर (v < 0)
                        सुरूठेवा;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        परत 0;
}

पूर्णांक cg_mask_supported_subtree(स्थिरांक अक्षर *मूळ, CGroupMask *ret) {
        CGroupMask mask;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ अक्षर *controllers = रिक्त, *path = रिक्त;
        r = cg_get_path(मूळ, "cgroup.controllers", &path);
        जर (r < 0)
                परत r;

        r = read_one_ओळ_file(path, &controllers);
        जर (r < 0)
                परत r;

        r = cg_mask_from_string(controllers, &mask);
        जर (r < 0)
                परत r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        परत 0;
}

पूर्णांक cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ अक्षर *मूळ = रिक्त;
        पूर्णांक r;

        r = cg_get_मूळ_path(&मूळ);
        जर (r < 0)
                परत r;

        परत cg_mask_supported_subtree(मूळ, ret);
}

पूर्णांक cg_is_delegated(स्थिरांक अक्षर *path) {
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        जर (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                परत r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        परत ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

पूर्णांक cg_is_delegated_fd(पूर्णांक fd) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ रिक्त, "trusted.delegate", /* at_flags= */ 0);
        जर (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                परत r;

        r = getxattr_at_bool(fd, /* path= */ रिक्त, "user.delegate", /* at_flags= */ 0);
        परत ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

पूर्णांक cg_has_coredump_receive(स्थिरांक अक्षर *path) {
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        जर (ERRNO_IS_NEG_XATTR_ABSENT(r))
                परत false;

        परत r;
}

स्थिरांक uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

स्थिर स्थिरांक अक्षर* स्थिरांक cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

शून्य cgroup_io_limits_list(शून्य) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

स्थिर स्थिरांक अक्षर *स्थिरांक cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

स्थिर स्थिरांक अक्षर* स्थिरांक managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

स्थिर स्थिरांक अक्षर* स्थिरांक managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
