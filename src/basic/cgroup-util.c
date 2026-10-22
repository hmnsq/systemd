/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/fs.h>
#समावेश <linux/magic.h>
#समावेश <signal.h>
#समावेश <stdlib.h>
#समावेश <sys/xattr.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "capsule-util.h"
#समावेश "cgroup-util.h"
#समावेश "dirent-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "format-util.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "login-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "set.h"
#समावेश "special.h"
#समावेश "stat-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "unaligned.h"
#समावेश "unit-name.h"
#समावेश "user-util.h"
#समावेश "xattr-util.h"

पूर्णांक cg_is_available(व्योम) {
        काष्ठा statfs fs;

        अगर (statfs("/sys/fs/cgroup/", &fs) < 0) {
                अगर (त्रुटि_सं == ENOENT) /* sysfs not mounted? */
                        वापस false;

                वापस log_debug_errno(त्रुटि_सं, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        वापस is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

पूर्णांक cg_path_open(स्थिर अक्षर *path) {
        _cleanup_free_ अक्षर *fs = शून्य;
        पूर्णांक r;

        r = cg_get_path(path, /* suffix= */ शून्य, &fs);
        अगर (r < 0)
                वापस r;

        वापस RET_NERRNO(खोलो(fs, O_DIRECTORY|O_CLOEXEC));
}

पूर्णांक cg_cgroupid_open(पूर्णांक cgroupfs_fd, uint64_t id) {
        _cleanup_close_ पूर्णांक fsfd = -EBADF;

        अगर (cgroupfs_fd < 0) {
                fsfd = खोलो("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                अगर (fsfd < 0)
                        वापस -त्रुटि_सं;

                cgroupfs_fd = fsfd;
        }

        जोड़ {
                काष्ठा file_handle file_handle;
                uint8_t space[दुरत्व(काष्ठा file_handle, f_handle) + माप(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = माप(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        वापस RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

पूर्णांक cg_path_from_cgroupid(पूर्णांक cgroupfs_fd, uint64_t id, अक्षर **ret) {
        _cleanup_close_ पूर्णांक cgfd = -EBADF;
        पूर्णांक r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        अगर (cgfd < 0)
                वापस cgfd;

        _cleanup_free_ अक्षर *path = शून्य;
        r = fd_get_path(cgfd, &path);
        अगर (r < 0)
                वापस r;

        अगर (!path_startswith(path, "/sys/fs/cgroup/"))
                वापस -EXDEV; /* recognizable error */

        अगर (ret)
                *ret = TAKE_PTR(path);
        वापस 0;
}

पूर्णांक cg_enumerate_processes(स्थिर अक्षर *path, खाता **ret) {
        _cleanup_free_ अक्षर *fs = शून्य;
        खाता *f;
        पूर्णांक r;

        निश्चित(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        अगर (r < 0)
                वापस r;

        f = ख_खोलो(fs, "re");
        अगर (!f)
                वापस -त्रुटि_सं;

        *ret = f;
        वापस 0;
}

पूर्णांक cg_read_pid(खाता *f, pid_t *ret, CGroupFlags flags) {
        अचिन्हित दीर्घ ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        निश्चित(f);
        निश्चित(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        क्रम (;;) {
                त्रुटि_सं = 0;
                अगर (ख_पूछो(f, "%lu", &ul) != 1) {

                        अगर (ख_पूर्ण(f)) {
                                *ret = 0;
                                वापस 0;
                        }

                        वापस errno_or_else(EIO);
                }

                अगर (ul > PID_T_MAX)
                        वापस -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                अगर (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        जारी;

                *ret = (pid_t) ul;
                वापस 1;
        }
}

पूर्णांक cg_read_pidref(खाता *f, PidRef *ret, CGroupFlags flags) {
        पूर्णांक r;

        निश्चित(f);
        निश्चित(ret);

        क्रम (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                अगर (r < 0)
                        वापस log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                अगर (r == 0) {
                        *ret = PIDREF_NULL;
                        वापस 0;
                }

                अगर (pid == 0)
                        वापस -EREMOTE;

                r = pidref_set_pid(ret, pid);
                अगर (r >= 0)
                        वापस 1;
                अगर (r != -ESRCH)
                        वापस r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(व्योम) {
        अटल thread_local पूर्णांक supported = -1;

        अगर (supported >= 0)
                वापस supported;

        अगर (cg_is_available() <= 0)
                वापस (supported = false);

        अगर (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                वापस (supported = true);
        अगर (त्रुटि_सं != ENOENT)
                log_debug_errno(त्रुटि_सं, "Failed to check whether cgroup.kill is available, assuming not: %m");
        वापस (supported = false);
}

पूर्णांक cg_enumerate_subgroups(स्थिर अक्षर *path, सूची **ret) {
        _cleanup_free_ अक्षर *fs = शून्य;
        सूची *d;
        पूर्णांक r;

        निश्चित(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ शून्य, &fs);
        अगर (r < 0)
                वापस r;

        d = सूची_खोलो(fs);
        अगर (!d)
                वापस -त्रुटि_सं;

        *ret = d;
        वापस 0;
}

पूर्णांक cg_read_subgroup(सूची *d, अक्षर **ret) {
        निश्चित(d);
        निश्चित(ret);

        FOREACH_DIRENT_ALL(de, d, वापस -त्रुटि_सं) {
                अगर (de->d_type != DT_DIR)
                        जारी;

                अगर (dot_or_dot_dot(de->d_name))
                        जारी;

                वापस strdup_to_full(ret, de->d_name);
        }

        *ret = शून्य;
        वापस 0;
}

पूर्णांक cg_kill(
                स्थिर अक्षर *path,
                पूर्णांक sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                व्योम *userdata) {

        _cleanup_set_free_ Set *allocated_set = शून्य;
        पूर्णांक r, ret = 0;

        निश्चित(path);
        निश्चित(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        अगर (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        अगर (!killed_pids) {
                killed_pids = allocated_set = set_new(शून्य);
                अगर (!killed_pids)
                        वापस -ENOMEM;
        }

        bool done;
        करो {
                _cleanup_fclose_ खाता *f = शून्य;
                पूर्णांक ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                अगर (r == -ENOENT)
                        अवरोध;
                अगर (r < 0)
                        वापस RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                क्रम (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        अगर (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                अवरोध;
                        }
                        अगर (r < 0)
                                वापस RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        अगर (r == 0)
                                अवरोध;

                        अगर ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                जारी;

                        अगर (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                जारी;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        अगर (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                जारी;
                        }

                        अगर (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        अगर (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        अगर (r >= 0) {
                                अगर (flags & CGROUP_SIGCONT)
                                        (व्योम) pidref_kill(&pidref, SIGCONT);

                                अगर (ret == 0) {
                                        अगर (log_kill)
                                                ret = ret_log_kill;
                                        अन्यथा
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        अगर (r < 0)
                                वापस RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } जबतक (!done);

        वापस ret;
}

पूर्णांक cg_kill_recursive(
                स्थिर अक्षर *path,
                पूर्णांक sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                व्योम *userdata) {

        _cleanup_set_free_ Set *allocated_set = शून्य;
        _cleanup_closedir_ सूची *d = शून्य;
        पूर्णांक r, ret;

        निश्चित(path);
        निश्चित(sig >= 0);

        अगर (!killed_pids) {
                killed_pids = allocated_set = set_new(शून्य);
                अगर (!killed_pids)
                        वापस -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        अगर (r < 0) {
                अगर (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                वापस ret;
        }

        क्रम (;;) {
                _cleanup_free_ अक्षर *fn = शून्य, *p = शून्य;

                r = cg_read_subgroup(d, &fn);
                अगर (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        अवरोध;
                }
                अगर (r == 0)
                        अवरोध;

                p = path_join(empty_to_मूल(path), fn);
                अगर (!p)
                        वापस -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                अगर (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                अगर (r != 0 && ret >= 0)
                        ret = r;
        }

        वापस ret;
}

पूर्णांक cg_kill_kernel_sigkill(स्थिर अक्षर *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ अक्षर *killfile = शून्य;
        uint64_t n_pids = UINT64_MAX;
        पूर्णांक r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        निश्चित(path);

        अगर (!cg_kill_supported())
                वापस -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        अगर (r < 0)
                वापस r;

        अगर (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                अगर (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_बफर);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        अगर (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        वापस 0;
}

पूर्णांक cg_get_path(स्थिर अक्षर *path, स्थिर अक्षर *suffix, अक्षर **ret) {
        अक्षर *t;

        निश्चित(ret);

        अगर (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        अगर (!t)
                वापस -ENOMEM;

        *ret = path_simplify(t);
        वापस 0;
}

पूर्णांक cg_set_xattr(स्थिर अक्षर *path, स्थिर अक्षर *name, स्थिर व्योम *मूल्य, माप_प्रकार आकार, पूर्णांक flags) {
        _cleanup_free_ अक्षर *fs = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(name);
        निश्चित(मूल्य || आकार <= 0);

        r = cg_get_path(path, /* suffix= */ शून्य, &fs);
        अगर (r < 0)
                वापस r;

        वापस RET_NERRNO(setxattr(fs, name, मूल्य, आकार, flags));
}

पूर्णांक cg_get_xattr(स्थिर अक्षर *path, स्थिर अक्षर *name, अक्षर **ret, माप_प्रकार *ret_आकार) {
        _cleanup_free_ अक्षर *fs = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(name);

        r = cg_get_path(path, /* suffix= */ शून्य, &fs);
        अगर (r < 0)
                वापस r;

        वापस lgetxattr_malloc(fs, name, ret, ret_आकार);
}

पूर्णांक cg_get_xattr_bool(स्थिर अक्षर *path, स्थिर अक्षर *name) {
        _cleanup_free_ अक्षर *fs = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(name);

        r = cg_get_path(path, /* suffix= */ शून्य, &fs);
        अगर (r < 0)
                वापस r;

        वापस getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

पूर्णांक cg_remove_xattr(स्थिर अक्षर *path, स्थिर अक्षर *name) {
        _cleanup_free_ अक्षर *fs = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(name);

        r = cg_get_path(path, /* suffix= */ शून्य, &fs);
        अगर (r < 0)
                वापस r;

        वापस RET_NERRNO(removexattr(fs, name));
}

पूर्णांक cg_pid_get_path(pid_t pid, अक्षर **ret_path) {
        _cleanup_fclose_ खाता *f = शून्य;
        स्थिर अक्षर *fs;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        अगर (r == -ENOENT)
                वापस -ESRCH;
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;
                अक्षर *e;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस -ENODATA;

                e = startswith(पंक्ति, "0:");
                अगर (!e)
                        जारी;

                e = म_अक्षर(e, ':');
                अगर (!e)
                        जारी;

                _cleanup_free_ अक्षर *path = strdup(e + 1);
                अगर (!path)
                        वापस -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                अगर (startswith(path, "/../"))
                        वापस -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                अगर (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                वापस 0;
        }
}

पूर्णांक cg_pidref_get_path(स्थिर PidRef *pidref, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *path = शून्य;
        पूर्णांक r;

        निश्चित(ret_path);

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        अगर (r < 0)
                वापस r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        *ret_path = TAKE_PTR(path);
        वापस 0;
}

पूर्णांक cg_is_empty(स्थिर अक्षर *path) {
        _cleanup_free_ अक्षर *t = शून्य;
        पूर्णांक r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        निश्चित(path);

        /* The root cgroup is always populated */
        अगर (empty_or_मूल(path))
                वापस false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        अगर (r == -ENOENT)
                वापस true;
        अगर (r < 0)
                वापस r;

        वापस streq(t, "0");
}

पूर्णांक cg_split_spec(स्थिर अक्षर *spec, अक्षर **ret_controller, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *controller = शून्य;
        स्थिर अक्षर *path;
        पूर्णांक r;

        निश्चित(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        अगर (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                जाओ finalize;
        }

        स्थिर अक्षर *e = म_अक्षर(spec, ':');
        अगर (!e) {
                /* Controller only. */
                अगर (ret_controller) {
                        controller = strdup(spec);
                        अगर (!controller)
                                वापस -ENOMEM;
                }

                path = शून्य;
        } अन्यथा {
                /* Both controller and path. */
                अगर (ret_controller) {
                        controller = strndup(spec, e - spec);
                        अगर (!controller)
                                वापस -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        अगर (path) {
                /* Non-empty path must be absolute. */
                अगर (!path_is_absolute(path))
                        वापस -EINVAL;

                /* Path must not contain dot-dot. */
                अगर (!path_is_safe(path))
                        वापस -EINVAL;
        }

        अगर (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        वापस 0;
}

पूर्णांक cg_get_मूल_path(अक्षर **ret_path) {
        अक्षर *p, *e;
        पूर्णांक r;

        निश्चित(ret_path);

        r = cg_pid_get_path(1, &p);
        अगर (r < 0)
                वापस r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        अगर (e)
                *e = 0;

        *ret_path = p;
        वापस 0;
}

पूर्णांक cg_shift_path(स्थिर अक्षर *cgroup, स्थिर अक्षर *मूल, स्थिर अक्षर **ret_shifted) {
        पूर्णांक r;

        निश्चित(cgroup);
        निश्चित(ret_shifted);

        _cleanup_free_ अक्षर *rt = शून्य;
        अगर (!मूल) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_मूल_path(&rt);
                अगर (r < 0)
                        वापस r;

                मूल = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, मूल, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        वापस 0;
}

पूर्णांक cg_pid_get_path_shifted(pid_t pid, स्थिर अक्षर *मूल, अक्षर **ret_cgroup) {
        _cleanup_free_ अक्षर *raw = शून्य;
        स्थिर अक्षर *c;
        पूर्णांक r;

        निश्चित(pid >= 0);
        निश्चित(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        अगर (r < 0)
                वापस r;

        r = cg_shift_path(raw, मूल, &c);
        अगर (r < 0)
                वापस r;

        अगर (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                वापस 0;
        }

        वापस strdup_to(ret_cgroup, c);
}

पूर्णांक cg_pidref_get_path_shifted(स्थिर PidRef *pidref, स्थिर अक्षर *मूल, अक्षर **ret_cgroup) {
        _cleanup_free_ अक्षर *raw = शून्य;
        स्थिर अक्षर *c;
        पूर्णांक r;

        निश्चित(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        अगर (r < 0)
                वापस r;

        r = cg_shift_path(raw, मूल, &c);
        अगर (r < 0)
                वापस r;

        अगर (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                वापस 0;
        }

        वापस strdup_to(ret_cgroup, c);
}

पूर्णांक cg_path_decode_unit(स्थिर अक्षर *cgroup, अक्षर **ret_unit) {
        निश्चित(cgroup);

        माप_प्रकार n = म_खोज(cgroup, "/");
        अगर (n < 3)
                वापस -ENXIO;

        अक्षर *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        अगर (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                वापस -ENXIO;

        अगर (ret_unit)
                वापस strdup_to(ret_unit, c);

        वापस 0;
}

अटल bool valid_slice_name(स्थिर अक्षर *p, माप_प्रकार n) {
        निश्चित(p || n == 0);

        अगर (n < STRLEN("x.slice"))
                वापस false;

        अक्षर *c = strndupa_safe(p, n);
        अगर (!endswith(c, ".slice"))
                वापस false;

        वापस unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

अटल स्थिर अक्षर* skip_slices(स्थिर अक्षर *p) {
        निश्चित(p);

        /* Skips over all slice assignments */

        क्रम (;;) {
                माप_प्रकार n;

                p += म_अखोज(p, "/");

                n = म_खोज(p, "/");
                अगर (!valid_slice_name(p, n))
                        वापस p;

                p += n;
        }
}

पूर्णांक cg_path_get_unit_full(स्थिर अक्षर *path, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        निश्चित(path);

        स्थिर अक्षर *e = skip_slices(path);

        _cleanup_free_ अक्षर *unit = शून्य;
        r = cg_path_decode_unit(e, &unit);
        अगर (r < 0)
                वापस r;

        /* We skipped over the slices, don't accept any now */
        अगर (endswith(unit, ".slice"))
                वापस -ENXIO;

        अगर (ret_subgroup) {
                _cleanup_free_ अक्षर *subgroup = शून्य;
                e += म_खोज(e, "/");
                e += म_अखोज(e, "/");

                अगर (isempty(e))
                        subgroup = शून्य;
                अन्यथा {
                        subgroup = strdup(e);
                        अगर (!subgroup)
                                वापस -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        अगर (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        वापस 0;
}

पूर्णांक cg_path_get_unit_path(स्थिर अक्षर *path, अक्षर **ret) {
        _cleanup_free_ अक्षर *path_copy = शून्य;
        अक्षर *unit_name;

        निश्चित(path);
        निश्चित(ret);

        path_copy = strdup(path);
        अगर (!path_copy)
                वापस -ENOMEM;

        unit_name = (अक्षर*) skip_slices(path_copy);
        unit_name[म_खोज(unit_name, "/")] = 0;

        अगर (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                वापस -ENXIO;

        *ret = TAKE_PTR(path_copy);

        वापस 0;
}

पूर्णांक cg_pid_get_unit_full(pid_t pid, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        _cleanup_free_ अक्षर *cgroup = शून्य;
        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

पूर्णांक cg_pidref_get_unit_full(स्थिर PidRef *pidref, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        _cleanup_free_ अक्षर *unit = शून्य, *subgroup = शून्य;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        अगर (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        अगर (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        वापस 0;
}

अटल स्थिर अक्षर* skip_session(स्थिर अक्षर *p) {
        माप_प्रकार n;

        /* Skip session-*.scope, but require it to be there. */

        अगर (isempty(p))
                वापस शून्य;

        p += म_अखोज(p, "/");

        n = म_खोज(p, "/");
        अगर (n < STRLEN("session-x.scope"))
                वापस शून्य;

        स्थिर अक्षर *s = startswith(p, "session-");
        अगर (!s)
                वापस शून्य;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        अक्षर *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        अगर (!e)
                वापस शून्य;
        *e = '\0';

        अगर (!session_id_valid(f))
                वापस शून्य;

        वापस skip_leading_slash(p + n);
}

अटल स्थिर अक्षर* skip_user_manager(स्थिर अक्षर *p) {
        माप_प्रकार n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        अगर (isempty(p))
                वापस शून्य;

        p += म_अखोज(p, "/");

        n = म_खोज(p, "/");
        अगर (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                वापस शून्य;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ अक्षर *unit_name = strndup(p, n);
        अगर (!unit_name)
                वापस शून्य;

        _cleanup_free_ अक्षर *i = शून्य;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        अगर (type != UNIT_NAME_INSTANCE)
                वापस शून्य;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        अगर (!(startswith(unit_name, "user@") && parse_uid(i, शून्य) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                वापस शून्य;

        वापस skip_leading_slash(p + n);
}

अटल स्थिर अक्षर* skip_user_prefix(स्थिर अक्षर *path) {
        स्थिर अक्षर *e, *t;

        निश्चित(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        अगर (t)
                वापस t;

        /* Alternatively skip the user session if it is in the path... */
        वापस skip_session(e);
}

पूर्णांक cg_path_get_user_unit_full(स्थिर अक्षर *path, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        स्थिर अक्षर *t;

        निश्चित(path);

        t = skip_user_prefix(path);
        अगर (!t)
                वापस -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        वापस cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

पूर्णांक cg_pid_get_user_unit_full(pid_t pid, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        _cleanup_free_ अक्षर *cgroup = शून्य;
        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

पूर्णांक cg_pidref_get_user_unit_full(स्थिर PidRef *pidref, अक्षर **ret_unit, अक्षर **ret_subgroup) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        _cleanup_free_ अक्षर *unit = शून्य, *subgroup = शून्य;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        अगर (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        अगर (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        वापस 0;
}

पूर्णांक cg_path_get_machine_name(स्थिर अक्षर *path, अक्षर **ret_machine) {
        _cleanup_free_ अक्षर *u = शून्य;
        स्थिर अक्षर *sl;
        पूर्णांक r;

        r = cg_path_get_unit(path, &u);
        अगर (r < 0)
                वापस r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        वापस readlink_malloc(sl, ret_machine);
}

पूर्णांक cg_pid_get_machine_name(pid_t pid, अक्षर **ret_machine) {
        _cleanup_free_ अक्षर *cgroup = शून्य;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_machine_name(cgroup, ret_machine);
}

पूर्णांक cg_path_get_session(स्थिर अक्षर *path, अक्षर **ret_session) {
        _cleanup_free_ अक्षर *unit = शून्य;
        अक्षर *start, *end;
        पूर्णांक r;

        निश्चित(path);

        r = cg_path_get_unit(path, &unit);
        अगर (r < 0)
                वापस r;

        start = startswith(unit, "session-");
        अगर (!start)
                वापस -ENXIO;
        end = endswith(start, ".scope");
        अगर (!end)
                वापस -ENXIO;

        *end = 0;
        अगर (!session_id_valid(start))
                वापस -ENXIO;

        अगर (!ret_session)
                वापस 0;

        वापस strdup_to(ret_session, start);
}

पूर्णांक cg_pid_get_session(pid_t pid, अक्षर **ret_session) {
        _cleanup_free_ अक्षर *cgroup = शून्य;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_session(cgroup, ret_session);
}

पूर्णांक cg_pidref_get_session(स्थिर PidRef *pidref, अक्षर **ret) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        _cleanup_free_ अक्षर *session = शून्य;
        r = cg_pid_get_session(pidref->pid, &session);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = TAKE_PTR(session);
        वापस 0;
}

पूर्णांक cg_path_get_owner_uid(स्थिर अक्षर *path, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *slice = शून्य;
        अक्षर *start, *end;
        पूर्णांक r;

        निश्चित(path);

        r = cg_path_get_slice(path, &slice);
        अगर (r < 0)
                वापस r;

        start = startswith(slice, "user-");
        अगर (!start)
                वापस -ENXIO;

        end = endswith(start, ".slice");
        अगर (!end)
                वापस -ENXIO;

        *end = 0;
        अगर (parse_uid(start, ret_uid) < 0)
                वापस -ENXIO;

        वापस 0;
}

पूर्णांक cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *cgroup = शून्य;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_owner_uid(cgroup, ret_uid);
}

पूर्णांक cg_pidref_get_owner_uid(स्थिर PidRef *pidref, uid_t *ret) {
        पूर्णांक r;

        अगर (!pidref_is_set(pidref))
                वापस -ESRCH;
        अगर (pidref_is_remote(pidref))
                वापस -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        अगर (r < 0)
                वापस r;

        r = pidref_verify(pidref);
        अगर (r < 0)
                वापस r;

        अगर (ret)
                *ret = uid;

        वापस 0;
}

पूर्णांक cg_path_get_slice(स्थिर अक्षर *p, अक्षर **ret_slice) {
        स्थिर अक्षर *e = शून्य;

        निश्चित(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        क्रम (;;) {
                स्थिर अक्षर *s;
                पूर्णांक n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                अगर (n < 0)
                        वापस n;
                अगर (!valid_slice_name(s, n))
                        अवरोध;

                e = s;
        }

        अगर (e)
                वापस cg_path_decode_unit(e, ret_slice);

        अगर (ret_slice)
                वापस strdup_to(ret_slice, SPECIAL_मूल_SLICE);

        वापस 0;
}

पूर्णांक cg_pid_get_slice(pid_t pid, अक्षर **ret_slice) {
        _cleanup_free_ अक्षर *cgroup = शून्य;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_slice(cgroup, ret_slice);
}

पूर्णांक cg_path_get_user_slice(स्थिर अक्षर *p, अक्षर **ret_slice) {
        स्थिर अक्षर *t;
        निश्चित(p);

        t = skip_user_prefix(p);
        अगर (!t)
                वापस -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        वापस cg_path_get_slice(t, ret_slice);
}

पूर्णांक cg_pid_get_user_slice(pid_t pid, अक्षर **ret_slice) {
        _cleanup_free_ अक्षर *cgroup = शून्य;
        पूर्णांक r;

        r = cg_pid_get_path_shifted(pid, शून्य, &cgroup);
        अगर (r < 0)
                वापस r;

        वापस cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(स्थिर अक्षर *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        अगर (!filename_is_valid(p))
                वापस true;

        अगर (IN_SET(p[0], '_', '.'))
                वापस true;

        अगर (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                वापस true;

        अगर (startswith(p, "cgroup."))
                वापस true;

        क्रम (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                स्थिर अक्षर *q;

                q = startswith(p, cgroup_controller_to_string(c));
                अगर (!q)
                        जारी;

                अगर (q[0] == '.')
                        वापस true;
        }

        वापस false;
}

पूर्णांक cg_escape(स्थिर अक्षर *p, अक्षर **ret) {
        _cleanup_free_ अक्षर *n = शून्य;

        निश्चित(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        अगर (cg_needs_escape(p)) {
                n = strjoin("_", p);
                अगर (!n)
                        वापस -ENOMEM;

                अगर (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        वापस -EINVAL;
        } अन्यथा {
                n = strdup(p);
                अगर (!n)
                        वापस -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        वापस 0;
}

अक्षर* cg_unescape(स्थिर अक्षर *p) {
        निश्चित(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        अगर (p[0] == '_')
                वापस (अक्षर*) p+1;

        वापस (अक्षर*) p;
}

पूर्णांक cg_slice_to_path(स्थिर अक्षर *unit, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य, *s = शून्य, *e = शून्य;
        स्थिर अक्षर *dash;
        पूर्णांक r;

        निश्चित(unit);
        निश्चित(ret);

        अगर (streq(unit, SPECIAL_मूल_SLICE))
                वापस strdup_to(ret, "");

        अगर (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                वापस -EINVAL;

        अगर (!endswith(unit, ".slice"))
                वापस -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        अगर (r < 0)
                वापस r;

        dash = म_अक्षर(p, '-');

        /* Don't allow initial dashes */
        अगर (dash == p)
                वापस -EINVAL;

        जबतक (dash) {
                _cleanup_free_ अक्षर *escaped = शून्य;
                अक्षर n[dash - p + माप(".slice")];

#अगर HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#पूर्ण_अगर

                /* Don't allow trailing or double dashes */
                अगर (IN_SET(dash[1], 0, '-'))
                        वापस -EINVAL;

                म_नकल(stpncpy(n, p, dash - p), ".slice");
                अगर (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        वापस -EINVAL;

                r = cg_escape(n, &escaped);
                अगर (r < 0)
                        वापस r;

                अगर (!strextend(&s, escaped, "/"))
                        वापस -ENOMEM;

                dash = म_अक्षर(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        अगर (r < 0)
                वापस r;

        अगर (!strextend(&s, e))
                वापस -ENOMEM;

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक cg_is_threaded(स्थिर अक्षर *path) {
        _cleanup_free_ अक्षर *fs = शून्य, *contents = शून्य;
        _cleanup_strv_free_ अक्षर **v = शून्य;
        पूर्णांक r;

        r = cg_get_path(path, "cgroup.type", &fs);
        अगर (r < 0)
                वापस r;

        r = read_full_virtual_file(fs, &contents, शून्य);
        अगर (r == -ENOENT)
                वापस false; /* Assume no. */
        अगर (r < 0)
                वापस r;

        v = strv_split(contents, शून्य);
        अगर (!v)
                वापस -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        वापस strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

पूर्णांक cg_set_attribute(स्थिर अक्षर *path, स्थिर अक्षर *attribute, स्थिर अक्षर *मूल्य) {
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(attribute);

        r = cg_get_path(path, attribute, &p);
        अगर (r < 0)
                वापस r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        वापस write_string_file(p, मूल्य, WRITE_STRING_FILE_DISABLE_बफर|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

पूर्णांक cg_get_attribute(स्थिर अक्षर *path, स्थिर अक्षर *attribute, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(attribute);

        r = cg_get_path(path, attribute, &p);
        अगर (r < 0)
                वापस r;

        वापस read_one_पंक्ति_file(p, ret);
}

पूर्णांक cg_get_attribute_as_uint64(स्थिर अक्षर *path, स्थिर अक्षर *attribute, uint64_t *ret) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        uint64_t v;
        पूर्णांक r;

        निश्चित(ret);

        r = cg_get_attribute(path, attribute, &मूल्य);
        अगर (r == -ENOENT)
                वापस -ENODATA;
        अगर (r < 0)
                वापस r;

        अगर (streq(मूल्य, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                वापस 0;
        }

        r = safe_atou64(मूल्य, &v);
        अगर (r < 0)
                वापस r;

        *ret = v;
        वापस 0;
}

पूर्णांक cg_get_attribute_as_bool(स्थिर अक्षर *path, स्थिर अक्षर *attribute) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        पूर्णांक r;

        r = cg_get_attribute(path, attribute, &मूल्य);
        अगर (r == -ENOENT)
                वापस -ENODATA;
        अगर (r < 0)
                वापस r;

        वापस parse_boolean(मूल्य);
}

पूर्णांक cg_get_owner(स्थिर अक्षर *path, uid_t *ret_uid) {
        _cleanup_free_ अक्षर *f = शून्य;
        काष्ठा stat stats;
        पूर्णांक r;

        निश्चित(ret_uid);

        r = cg_get_path(path, /* suffix= */ शून्य, &f);
        अगर (r < 0)
                वापस r;

        अगर (stat(f, &stats) < 0)
                वापस -त्रुटि_सं;

        r = stat_verify_directory(&stats);
        अगर (r < 0)
                वापस r;

        *ret_uid = stats.st_uid;
        वापस 0;
}

पूर्णांक cg_get_keyed_attribute(
                स्थिर अक्षर *path,
                स्थिर अक्षर *attribute,
                अक्षर * स्थिर *कुंजियाँ,
                अक्षर **values) {

        _cleanup_free_ अक्षर *filename = शून्य, *contents = शून्य;
        माप_प्रकार n;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        अगर (r < 0)
                वापस r;

        r = read_full_file(filename, &contents, /* ret_size= */ शून्य);
        अगर (r < 0)
                वापस r;

        n = strv_लंबाई(कुंजियाँ);
        अगर (n == 0) /* No keys to retrieve? That's easy, we are done then */
                वापस 0;
        निश्चित(strv_is_uniq(कुंजियाँ));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        अक्षर **v = newa0(अक्षर*, n);
        माप_प्रकार n_done = 0;

        क्रम (स्थिर अक्षर *p = contents; *p;) {
                स्थिर अक्षर *w;
                माप_प्रकार i;

                क्रम (i = 0; i < n; i++) {
                        w = first_word(p, कुंजियाँ[i]);
                        अगर (w)
                                अवरोध;
                }

                अगर (w) {
                        अगर (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                जाओ fail;
                        }

                        माप_प्रकार l = म_खोज(w, NEWLINE);

                        v[i] = strndup(w, l);
                        अगर (!v[i]) {
                                r = -ENOMEM;
                                जाओ fail;
                        }

                        n_done++;
                        अगर (n_done >= n)
                                अवरोध;

                        p = w + l;
                } अन्यथा
                        p += म_खोज(p, NEWLINE);

                p += म_अखोज(p, NEWLINE);
        }

        अगर (n_done < n) {
                r = -ENXIO;
                जाओ fail;
        }

        स_नकल(values, v, माप(अक्षर*) * n);
        वापस 0;

fail:
        free_many_charp(v, n);
        वापस r;
}

पूर्णांक cg_get_keyed_attribute_uint64(स्थिर अक्षर *path, स्थिर अक्षर *attribute, स्थिर अक्षर *कुंजी, uint64_t *ret) {
        _cleanup_free_ अक्षर *val = शून्य;
        पूर्णांक r;

        निश्चित(कुंजी);
        निश्चित(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(कुंजी), &val);
        अगर (r < 0)
                वापस r;

        r = safe_atou64(val, ret);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, कुंजी, attribute);

        वापस 0;
}

पूर्णांक cg_mask_to_string(CGroupMask mask, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        bool space = false;
        CGroupController c;
        माप_प्रकार n = 0;

        निश्चित(ret);

        अगर (mask == 0) {
                *ret = शून्य;
                वापस 0;
        }

        क्रम (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                स्थिर अक्षर *k;
                माप_प्रकार l;

                अगर (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        जारी;

                k = cgroup_controller_to_string(c);
                l = म_माप(k);

                अगर (!GREEDY_REALLOC(s, n + space + l + 1))
                        वापस -ENOMEM;

                अगर (space)
                        s[n] = ' ';
                स_नकल(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        निश्चित(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        वापस 0;
}

पूर्णांक cg_mask_from_string(स्थिर अक्षर *s, CGroupMask *ret) {
        CGroupMask m = 0;

        निश्चित(ret);
        निश्चित(s);

        क्रम (;;) {
                _cleanup_free_ अक्षर *n = शून्य;
                CGroupController v;
                पूर्णांक r;

                r = extract_first_word(&s, &n, शून्य, 0);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                v = cgroup_controller_from_string(n);
                अगर (v < 0)
                        जारी;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        वापस 0;
}

पूर्णांक cg_mask_supported_subtree(स्थिर अक्षर *मूल, CGroupMask *ret) {
        CGroupMask mask;
        पूर्णांक r;

        निश्चित(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ अक्षर *controllers = शून्य, *path = शून्य;
        r = cg_get_path(मूल, "cgroup.controllers", &path);
        अगर (r < 0)
                वापस r;

        r = read_one_पंक्ति_file(path, &controllers);
        अगर (r < 0)
                वापस r;

        r = cg_mask_from_string(controllers, &mask);
        अगर (r < 0)
                वापस r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        वापस 0;
}

पूर्णांक cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ अक्षर *मूल = शून्य;
        पूर्णांक r;

        r = cg_get_मूल_path(&मूल);
        अगर (r < 0)
                वापस r;

        वापस cg_mask_supported_subtree(मूल, ret);
}

पूर्णांक cg_is_delegated(स्थिर अक्षर *path) {
        पूर्णांक r;

        निश्चित(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        अगर (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                वापस r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        वापस ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

पूर्णांक cg_is_delegated_fd(पूर्णांक fd) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ शून्य, "trusted.delegate", /* at_flags= */ 0);
        अगर (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                वापस r;

        r = getxattr_at_bool(fd, /* path= */ शून्य, "user.delegate", /* at_flags= */ 0);
        वापस ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

पूर्णांक cg_has_coredump_receive(स्थिर अक्षर *path) {
        पूर्णांक r;

        निश्चित(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        अगर (ERRNO_IS_NEG_XATTR_ABSENT(r))
                वापस false;

        वापस r;
}

स्थिर uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

अटल स्थिर अक्षर* स्थिर cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

व्योम cgroup_io_limits_list(व्योम) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

अटल स्थिर अक्षर *स्थिर cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

अटल स्थिर अक्षर* स्थिर managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

अटल स्थिर अक्षर* स्थिर managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
