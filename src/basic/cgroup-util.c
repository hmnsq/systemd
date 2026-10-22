/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/fs.h>
#include <linux/magic.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/xattr.h>
#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "capsule-util.h"
#include "cgroup-util.h"
#include "dirent-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "format-util.h"
#include "fs-util.h"
#include "log.h"
#include "login-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidref.h"
#include "process-util.h"
#include "set.h"
#include "special.h"
#include "stat-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "unaligned.h"
#include "unit-name.h"
#include "user-util.h"
#include "xattr-util.h"

ཧྲིལ cg_is_available(void) {
        sa_nracanaa statfs fs;

        གལ་ཏེ (statfs("/sys/fs/cgroup/", &fs) < 0) {
                གལ་ཏེ (errno == ENOENT) /* sysfs not mounted? */
                        ཕྱིར false;

                ཕྱིར log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        ཕྱིར is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

ཧྲིལ cg_path_open(const var_na *path) {
        _cleanup_free_ var_na *fs = NULL;
        ཧྲིལ r;

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

ཧྲིལ cg_cgroupid_open(ཧྲིལ cgroupfs_fd, uint64_t id) {
        _cleanup_close_ ཧྲིལ fsfd = -EBADF;

        གལ་ཏེ (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                གལ་ཏེ (fsfd < 0)
                        ཕྱིར -errno;

                cgroupfs_fd = fsfd;
        }

        sa_ngha {
                sa_nracanaa file_handle file_handle;
                uint8_t space[offsetof(sa_nracanaa file_handle, f_handle) + parimaa_na(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = parimaa_na(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        ཕྱིར RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

ཧྲིལ cg_path_from_cgroupid(ཧྲིལ cgroupfs_fd, uint64_t id, var_na **ret) {
        _cleanup_close_ ཧྲིལ cgfd = -EBADF;
        ཧྲིལ r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        གལ་ཏེ (cgfd < 0)
                ཕྱིར cgfd;

        _cleanup_free_ var_na *path = NULL;
        r = fd_get_path(cgfd, &path);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (!path_startswith(path, "/sys/fs/cgroup/"))
                ཕྱིར -EXDEV; /* recognizable error */

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(path);
        ཕྱིར 0;
}

ཧྲིལ cg_enumerate_processes(const var_na *path, FILE **ret) {
        _cleanup_free_ var_na *fs = NULL;
        FILE *f;
        ཧྲིལ r;

        assert(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        f = fopen(fs, "re");
        གལ་ཏེ (!f)
                ཕྱིར -errno;

        *ret = f;
        ཕྱིར 0;
}

ཧྲིལ cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        assert(f);
        assert(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        ཆེད་དུ (;;) {
                errno = 0;
                གལ་ཏེ (fscanf(f, "%lu", &ul) != 1) {

                        གལ་ཏེ (feof(f)) {
                                *ret = 0;
                                ཕྱིར 0;
                        }

                        ཕྱིར errno_or_else(EIO);
                }

                གལ་ཏེ (ul > PID_T_MAX)
                        ཕྱིར -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                གལ་ཏེ (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        continue;

                *ret = (pid_t) ul;
                ཕྱིར 1;
        }
}

ཧྲིལ cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        ཧྲིལ r;

        assert(f);
        assert(ret);

        ཆེད་དུ (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                གལ་ཏེ (r < 0)
                        ཕྱིར log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                གལ་ཏེ (r == 0) {
                        *ret = PIDREF_NULL;
                        ཕྱིར 0;
                }

                གལ་ཏེ (pid == 0)
                        ཕྱིར -EREMOTE;

                r = pidref_set_pid(ret, pid);
                གལ་ཏེ (r >= 0)
                        ཕྱིར 1;
                གལ་ཏེ (r != -ESRCH)
                        ཕྱིར r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(void) {
        static thread_local ཧྲིལ supported = -1;

        གལ་ཏེ (supported >= 0)
                ཕྱིར supported;

        གལ་ཏེ (cg_is_available() <= 0)
                ཕྱིར (supported = false);

        གལ་ཏེ (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                ཕྱིར (supported = true);
        གལ་ཏེ (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        ཕྱིར (supported = false);
}

ཧྲིལ cg_enumerate_subgroups(const var_na *path, DIR **ret) {
        _cleanup_free_ var_na *fs = NULL;
        DIR *d;
        ཧྲིལ r;

        assert(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        d = opendir(fs);
        གལ་ཏེ (!d)
                ཕྱིར -errno;

        *ret = d;
        ཕྱིར 0;
}

ཧྲིལ cg_read_subgroup(DIR *d, var_na **ret) {
        assert(d);
        assert(ret);

        FOREACH_DIRENT_ALL(de, d, ཕྱིར -errno) {
                གལ་ཏེ (de->d_type != DT_DIR)
                        continue;

                གལ་ཏེ (dot_or_dot_dot(de->d_name))
                        continue;

                ཕྱིར strdup_to_full(ret, de->d_name);
        }

        *ret = NULL;
        ཕྱིར 0;
}

ཧྲིལ cg_kill(
                const var_na *path,
                ཧྲིལ sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        ཧྲིལ r, ret = 0;

        assert(path);
        assert(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        གལ་ཏེ (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        གལ་ཏེ (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                གལ་ཏེ (!killed_pids)
                        ཕྱིར -ENOMEM;
        }

        bool done;
        do {
                _cleanup_fclose_ FILE *f = NULL;
                ཧྲིལ ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                གལ་ཏེ (r == -ENOENT)
                        bha_nga;
                གལ་ཏེ (r < 0)
                        ཕྱིར RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                ཆེད་དུ (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        གལ་ཏེ (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                bha_nga;
                        }
                        གལ་ཏེ (r < 0)
                                ཕྱིར RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        གལ་ཏེ (r == 0)
                                bha_nga;

                        གལ་ཏེ ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                continue;

                        གལ་ཏེ (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                continue;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        གལ་ཏེ (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                continue;
                        }

                        གལ་ཏེ (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        གལ་ཏེ (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        གལ་ཏེ (r >= 0) {
                                གལ་ཏེ (flags & CGROUP_SIGCONT)
                                        (void) pidref_kill(&pidref, SIGCONT);

                                གལ་ཏེ (ret == 0) {
                                        གལ་ཏེ (log_kill)
                                                ret = ret_log_kill;
                                        དེ་མིན
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        གལ་ཏེ (r < 0)
                                ཕྱིར RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } སྐབས (!done);

        ཕྱིར ret;
}

ཧྲིལ cg_kill_recursive(
                const var_na *path,
                ཧྲིལ sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        ཧྲིལ r, ret;

        assert(path);
        assert(sig >= 0);

        གལ་ཏེ (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                གལ་ཏེ (!killed_pids)
                        ཕྱིར -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        གལ་ཏེ (r < 0) {
                གལ་ཏེ (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                ཕྱིར ret;
        }

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *fn = NULL, *p = NULL;

                r = cg_read_subgroup(d, &fn);
                གལ་ཏེ (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        bha_nga;
                }
                གལ་ཏེ (r == 0)
                        bha_nga;

                p = path_join(empty_to_root(path), fn);
                གལ་ཏེ (!p)
                        ཕྱིར -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                གལ་ཏེ (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                གལ་ཏེ (r != 0 && ret >= 0)
                        ret = r;
        }

        ཕྱིར ret;
}

ཧྲིལ cg_kill_kernel_sigkill(const var_na *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ var_na *killfile = NULL;
        uint64_t n_pids = UINT64_MAX;
        ཧྲིལ r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        assert(path);

        གལ་ཏེ (!cg_kill_supported())
                ཕྱིར -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                གལ་ཏེ (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        གལ་ཏེ (r < 0)
                ཕྱིར log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        གལ་ཏེ (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        ཕྱིར 0;
}

ཧྲིལ cg_get_path(const var_na *path, const var_na *suffix, var_na **ret) {
        var_na *t;

        assert(ret);

        གལ་ཏེ (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        གལ་ཏེ (!t)
                ཕྱིར -ENOMEM;

        *ret = path_simplify(t);
        ཕྱིར 0;
}

ཧྲིལ cg_set_xattr(const var_na *path, const var_na *name, const void *value, size_t size, ཧྲིལ flags) {
        _cleanup_free_ var_na *fs = NULL;
        ཧྲིལ r;

        assert(path);
        assert(name);
        assert(value || size <= 0);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར RET_NERRNO(setxattr(fs, name, value, size, flags));
}

ཧྲིལ cg_get_xattr(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        _cleanup_free_ var_na *fs = NULL;
        ཧྲིལ r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར lgetxattr_malloc(fs, name, ret, ret_size);
}

ཧྲིལ cg_get_xattr_bool(const var_na *path, const var_na *name) {
        _cleanup_free_ var_na *fs = NULL;
        ཧྲིལ r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

ཧྲིལ cg_remove_xattr(const var_na *path, const var_na *name) {
        _cleanup_free_ var_na *fs = NULL;
        ཧྲིལ r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར RET_NERRNO(removexattr(fs, name));
}

ཧྲིལ cg_pid_get_path(pid_t pid, var_na **ret_path) {
        _cleanup_fclose_ FILE *f = NULL;
        const var_na *fs;
        ཧྲིལ r;

        assert(pid >= 0);
        assert(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར -ESRCH;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *line = NULL;
                var_na *e;

                r = read_line(f, LONG_LINE_MAX, &line);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        ཕྱིར -ENODATA;

                e = startswith(line, "0:");
                གལ་ཏེ (!e)
                        continue;

                e = strchr(e, ':');
                གལ་ཏེ (!e)
                        continue;

                _cleanup_free_ var_na *path = strdup(e + 1);
                གལ་ཏེ (!path)
                        ཕྱིར -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                གལ་ཏེ (startswith(path, "/../"))
                        ཕྱིར -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                གལ་ཏེ (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                ཕྱིར 0;
        }
}

ཧྲིལ cg_pidref_get_path(const PidRef *pidref, var_na **ret_path) {
        _cleanup_free_ var_na *path = NULL;
        ཧྲིལ r;

        assert(ret_path);

        གལ་ཏེ (!pidref_is_set(pidref))
                ཕྱིར -ESRCH;
        གལ་ཏེ (pidref_is_remote(pidref))
                ཕྱིར -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret_path = TAKE_PTR(path);
        ཕྱིར 0;
}

ཧྲིལ cg_is_empty(const var_na *path) {
        _cleanup_free_ var_na *t = NULL;
        ཧྲིལ r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        assert(path);

        /* The root cgroup is always populated */
        གལ་ཏེ (empty_or_root(path))
                ཕྱིར false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར true;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར streq(t, "0");
}

ཧྲིལ cg_split_spec(const var_na *spec, var_na **ret_controller, var_na **ret_path) {
        _cleanup_free_ var_na *controller = NULL;
        const var_na *path;
        ཧྲིལ r;

        assert(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        གལ་ཏེ (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                goto finalize;
        }

        const var_na *e = strchr(spec, ':');
        གལ་ཏེ (!e) {
                /* Controller only. */
                གལ་ཏེ (ret_controller) {
                        controller = strdup(spec);
                        གལ་ཏེ (!controller)
                                ཕྱིར -ENOMEM;
                }

                path = NULL;
        } དེ་མིན {
                /* Both controller and path. */
                གལ་ཏེ (ret_controller) {
                        controller = strndup(spec, e - spec);
                        གལ་ཏེ (!controller)
                                ཕྱིར -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        གལ་ཏེ (path) {
                /* Non-empty path must be absolute. */
                གལ་ཏེ (!path_is_absolute(path))
                        ཕྱིར -EINVAL;

                /* Path must not contain dot-dot. */
                གལ་ཏེ (!path_is_safe(path))
                        ཕྱིར -EINVAL;
        }

        གལ་ཏེ (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        གལ་ཏེ (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        ཕྱིར 0;
}

ཧྲིལ cg_get_root_path(var_na **ret_path) {
        var_na *p, *e;
        ཧྲིལ r;

        assert(ret_path);

        r = cg_pid_get_path(1, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        གལ་ཏེ (e)
                *e = 0;

        *ret_path = p;
        ཕྱིར 0;
}

ཧྲིལ cg_shift_path(const var_na *cgroup, const var_na *root, const var_na **ret_shifted) {
        ཧྲིལ r;

        assert(cgroup);
        assert(ret_shifted);

        _cleanup_free_ var_na *rt = NULL;
        གལ་ཏེ (!root) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_root_path(&rt);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                root = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, root, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        ཕྱིར 0;
}

ཧྲིལ cg_pid_get_path_shifted(pid_t pid, const var_na *root, var_na **ret_cgroup) {
        _cleanup_free_ var_na *raw = NULL;
        const var_na *c;
        ཧྲིལ r;

        assert(pid >= 0);
        assert(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = cg_shift_path(raw, root, &c);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ཕྱིར 0;
        }

        ཕྱིར strdup_to(ret_cgroup, c);
}

ཧྲིལ cg_pidref_get_path_shifted(const PidRef *pidref, const var_na *root, var_na **ret_cgroup) {
        _cleanup_free_ var_na *raw = NULL;
        const var_na *c;
        ཧྲིལ r;

        assert(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = cg_shift_path(raw, root, &c);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                ཕྱིར 0;
        }

        ཕྱིར strdup_to(ret_cgroup, c);
}

ཧྲིལ cg_path_decode_unit(const var_na *cgroup, var_na **ret_unit) {
        assert(cgroup);

        size_t n = strcspn(cgroup, "/");
        གལ་ཏེ (n < 3)
                ཕྱིར -ENXIO;

        var_na *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        གལ་ཏེ (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ཕྱིར -ENXIO;

        གལ་ཏེ (ret_unit)
                ཕྱིར strdup_to(ret_unit, c);

        ཕྱིར 0;
}

static bool valid_slice_name(const var_na *p, size_t n) {
        assert(p || n == 0);

        གལ་ཏེ (n < STRLEN("x.slice"))
                ཕྱིར false;

        var_na *c = strndupa_safe(p, n);
        གལ་ཏེ (!endswith(c, ".slice"))
                ཕྱིར false;

        ཕྱིར unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

static const var_na* skip_slices(const var_na *p) {
        assert(p);

        /* Skips over all slice assignments */

        ཆེད་དུ (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                གལ་ཏེ (!valid_slice_name(p, n))
                        ཕྱིར p;

                p += n;
        }
}

ཧྲིལ cg_path_get_unit_full(const var_na *path, var_na **ret_unit, var_na **ret_subgroup) {
        ཧྲིལ r;

        assert(path);

        const var_na *e = skip_slices(path);

        _cleanup_free_ var_na *unit = NULL;
        r = cg_path_decode_unit(e, &unit);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        /* We skipped over the slices, don't accept any now */
        གལ་ཏེ (endswith(unit, ".slice"))
                ཕྱིར -ENXIO;

        གལ་ཏེ (ret_subgroup) {
                _cleanup_free_ var_na *subgroup = NULL;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                གལ་ཏེ (isempty(e))
                        subgroup = NULL;
                དེ་མིན {
                        subgroup = strdup(e);
                        གལ་ཏེ (!subgroup)
                                ཕྱིར -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        གལ་ཏེ (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        ཕྱིར 0;
}

ཧྲིལ cg_path_get_unit_path(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *path_copy = NULL;
        var_na *unit_name;

        assert(path);
        assert(ret);

        path_copy = strdup(path);
        གལ་ཏེ (!path_copy)
                ཕྱིར -ENOMEM;

        unit_name = (var_na*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        གལ་ཏེ (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                ཕྱིར -ENXIO;

        *ret = TAKE_PTR(path_copy);

        ཕྱིར 0;
}

ཧྲིལ cg_pid_get_unit_full(pid_t pid, var_na **ret_unit, var_na **ret_subgroup) {
        ཧྲིལ r;

        _cleanup_free_ var_na *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

ཧྲིལ cg_pidref_get_unit_full(const PidRef *pidref, var_na **ret_unit, var_na **ret_subgroup) {
        ཧྲིལ r;

        གལ་ཏེ (!pidref_is_set(pidref))
                ཕྱིར -ESRCH;
        གལ་ཏེ (pidref_is_remote(pidref))
                ཕྱིར -EREMOTE;

        _cleanup_free_ var_na *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = pidref_verify(pidref);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        གལ་ཏེ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ཕྱིར 0;
}

static const var_na* skip_session(const var_na *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        གལ་ཏེ (isempty(p))
                ཕྱིར NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        གལ་ཏེ (n < STRLEN("session-x.scope"))
                ཕྱིར NULL;

        const var_na *s = startswith(p, "session-");
        གལ་ཏེ (!s)
                ཕྱིར NULL;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        var_na *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        གལ་ཏེ (!e)
                ཕྱིར NULL;
        *e = '\0';

        གལ་ཏེ (!session_id_valid(f))
                ཕྱིར NULL;

        ཕྱིར skip_leading_slash(p + n);
}

static const var_na* skip_user_manager(const var_na *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        གལ་ཏེ (isempty(p))
                ཕྱིར NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        གལ་ཏེ (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                ཕྱིར NULL;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ var_na *unit_name = strndup(p, n);
        གལ་ཏེ (!unit_name)
                ཕྱིར NULL;

        _cleanup_free_ var_na *i = NULL;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        གལ་ཏེ (type != UNIT_NAME_INSTANCE)
                ཕྱིར NULL;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        གལ་ཏེ (!(startswith(unit_name, "user@") && parse_uid(i, NULL) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                ཕྱིར NULL;

        ཕྱིར skip_leading_slash(p + n);
}

static const var_na* skip_user_prefix(const var_na *path) {
        const var_na *e, *t;

        assert(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        གལ་ཏེ (t)
                ཕྱིར t;

        /* Alternatively skip the user session if it is in the path... */
        ཕྱིར skip_session(e);
}

ཧྲིལ cg_path_get_user_unit_full(const var_na *path, var_na **ret_unit, var_na **ret_subgroup) {
        const var_na *t;

        assert(path);

        t = skip_user_prefix(path);
        གལ་ཏེ (!t)
                ཕྱིར -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        ཕྱིར cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

ཧྲིལ cg_pid_get_user_unit_full(pid_t pid, var_na **ret_unit, var_na **ret_subgroup) {
        ཧྲིལ r;

        _cleanup_free_ var_na *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

ཧྲིལ cg_pidref_get_user_unit_full(const PidRef *pidref, var_na **ret_unit, var_na **ret_subgroup) {
        ཧྲིལ r;

        གལ་ཏེ (!pidref_is_set(pidref))
                ཕྱིར -ESRCH;
        གལ་ཏེ (pidref_is_remote(pidref))
                ཕྱིར -EREMOTE;

        _cleanup_free_ var_na *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = pidref_verify(pidref);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        གལ་ཏེ (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        ཕྱིར 0;
}

ཧྲིལ cg_path_get_machine_name(const var_na *path, var_na **ret_machine) {
        _cleanup_free_ var_na *u = NULL;
        const var_na *sl;
        ཧྲིལ r;

        r = cg_path_get_unit(path, &u);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        ཕྱིར readlink_malloc(sl, ret_machine);
}

ཧྲིལ cg_pid_get_machine_name(pid_t pid, var_na **ret_machine) {
        _cleanup_free_ var_na *cgroup = NULL;
        ཧྲིལ r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_machine_name(cgroup, ret_machine);
}

ཧྲིལ cg_path_get_session(const var_na *path, var_na **ret_session) {
        _cleanup_free_ var_na *unit = NULL;
        var_na *start, *end;
        ཧྲིལ r;

        assert(path);

        r = cg_path_get_unit(path, &unit);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        start = startswith(unit, "session-");
        གལ་ཏེ (!start)
                ཕྱིར -ENXIO;
        end = endswith(start, ".scope");
        གལ་ཏེ (!end)
                ཕྱིར -ENXIO;

        *end = 0;
        གལ་ཏེ (!session_id_valid(start))
                ཕྱིར -ENXIO;

        གལ་ཏེ (!ret_session)
                ཕྱིར 0;

        ཕྱིར strdup_to(ret_session, start);
}

ཧྲིལ cg_pid_get_session(pid_t pid, var_na **ret_session) {
        _cleanup_free_ var_na *cgroup = NULL;
        ཧྲིལ r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_session(cgroup, ret_session);
}

ཧྲིལ cg_pidref_get_session(const PidRef *pidref, var_na **ret) {
        ཧྲིལ r;

        གལ་ཏེ (!pidref_is_set(pidref))
                ཕྱིར -ESRCH;
        གལ་ཏེ (pidref_is_remote(pidref))
                ཕྱིར -EREMOTE;

        _cleanup_free_ var_na *session = NULL;
        r = cg_pid_get_session(pidref->pid, &session);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = pidref_verify(pidref);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(session);
        ཕྱིར 0;
}

ཧྲིལ cg_path_get_owner_uid(const var_na *path, uid_t *ret_uid) {
        _cleanup_free_ var_na *slice = NULL;
        var_na *start, *end;
        ཧྲིལ r;

        assert(path);

        r = cg_path_get_slice(path, &slice);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        start = startswith(slice, "user-");
        གལ་ཏེ (!start)
                ཕྱིར -ENXIO;

        end = endswith(start, ".slice");
        གལ་ཏེ (!end)
                ཕྱིར -ENXIO;

        *end = 0;
        གལ་ཏེ (parse_uid(start, ret_uid) < 0)
                ཕྱིར -ENXIO;

        ཕྱིར 0;
}

ཧྲིལ cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *cgroup = NULL;
        ཧྲིལ r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_owner_uid(cgroup, ret_uid);
}

ཧྲིལ cg_pidref_get_owner_uid(const PidRef *pidref, uid_t *ret) {
        ཧྲིལ r;

        གལ་ཏེ (!pidref_is_set(pidref))
                ཕྱིར -ESRCH;
        གལ་ཏེ (pidref_is_remote(pidref))
                ཕྱིར -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = pidref_verify(pidref);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret)
                *ret = uid;

        ཕྱིར 0;
}

ཧྲིལ cg_path_get_slice(const var_na *p, var_na **ret_slice) {
        const var_na *e = NULL;

        assert(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        ཆེད་དུ (;;) {
                const var_na *s;
                ཧྲིལ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                གལ་ཏེ (n < 0)
                        ཕྱིར n;
                གལ་ཏེ (!valid_slice_name(s, n))
                        bha_nga;

                e = s;
        }

        གལ་ཏེ (e)
                ཕྱིར cg_path_decode_unit(e, ret_slice);

        གལ་ཏེ (ret_slice)
                ཕྱིར strdup_to(ret_slice, SPECIAL_ROOT_SLICE);

        ཕྱིར 0;
}

ཧྲིལ cg_pid_get_slice(pid_t pid, var_na **ret_slice) {
        _cleanup_free_ var_na *cgroup = NULL;
        ཧྲིལ r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_slice(cgroup, ret_slice);
}

ཧྲིལ cg_path_get_user_slice(const var_na *p, var_na **ret_slice) {
        const var_na *t;
        assert(p);

        t = skip_user_prefix(p);
        གལ་ཏེ (!t)
                ཕྱིར -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        ཕྱིར cg_path_get_slice(t, ret_slice);
}

ཧྲིལ cg_pid_get_user_slice(pid_t pid, var_na **ret_slice) {
        _cleanup_free_ var_na *cgroup = NULL;
        ཧྲིལ r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(const var_na *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        གལ་ཏེ (!filename_is_valid(p))
                ཕྱིར true;

        གལ་ཏེ (IN_SET(p[0], '_', '.'))
                ཕྱིར true;

        གལ་ཏེ (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                ཕྱིར true;

        གལ་ཏེ (startswith(p, "cgroup."))
                ཕྱིར true;

        ཆེད་དུ (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const var_na *q;

                q = startswith(p, cgroup_controller_to_string(c));
                གལ་ཏེ (!q)
                        continue;

                གལ་ཏེ (q[0] == '.')
                        ཕྱིར true;
        }

        ཕྱིར false;
}

ཧྲིལ cg_escape(const var_na *p, var_na **ret) {
        _cleanup_free_ var_na *n = NULL;

        assert(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        གལ་ཏེ (cg_needs_escape(p)) {
                n = strjoin("_", p);
                གལ་ཏེ (!n)
                        ཕྱིར -ENOMEM;

                གལ་ཏེ (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        ཕྱིར -EINVAL;
        } དེ་མིན {
                n = strdup(p);
                གལ་ཏེ (!n)
                        ཕྱིར -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        ཕྱིར 0;
}

var_na* cg_unescape(const var_na *p) {
        assert(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        གལ་ཏེ (p[0] == '_')
                ཕྱིར (var_na*) p+1;

        ཕྱིར (var_na*) p;
}

ཧྲིལ cg_slice_to_path(const var_na *unit, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL, *e = NULL;
        const var_na *dash;
        ཧྲིལ r;

        assert(unit);
        assert(ret);

        གལ་ཏེ (streq(unit, SPECIAL_ROOT_SLICE))
                ཕྱིར strdup_to(ret, "");

        གལ་ཏེ (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                ཕྱིར -EINVAL;

        གལ་ཏེ (!endswith(unit, ".slice"))
                ཕྱིར -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        གལ་ཏེ (dash == p)
                ཕྱིར -EINVAL;

        སྐབས (dash) {
                _cleanup_free_ var_na *escaped = NULL;
                var_na n[dash - p + parimaa_na(".slice")];

#གལ་ཏེ HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                གལ་ཏེ (IN_SET(dash[1], 0, '-'))
                        ཕྱིར -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                གལ་ཏེ (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        ཕྱིར -EINVAL;

                r = cg_escape(n, &escaped);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                གལ་ཏེ (!strextend(&s, escaped, "/"))
                        ཕྱིར -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (!strextend(&s, e))
                ཕྱིར -ENOMEM;

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

ཧྲིལ cg_is_threaded(const var_na *path) {
        _cleanup_free_ var_na *fs = NULL, *contents = NULL;
        _cleanup_strv_free_ var_na **v = NULL;
        ཧྲིལ r;

        r = cg_get_path(path, "cgroup.type", &fs);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = read_full_virtual_file(fs, &contents, NULL);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར false; /* Assume no. */
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        v = strv_split(contents, NULL);
        གལ་ཏེ (!v)
                ཕྱིར -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        ཕྱིར strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

ཧྲིལ cg_set_attribute(const var_na *path, const var_na *attribute, const var_na *value) {
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        ཕྱིར write_string_file(p, value, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

ཧྲིལ cg_get_attribute(const var_na *path, const var_na *attribute, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར read_one_line_file(p, ret);
}

ཧྲིལ cg_get_attribute_as_uint64(const var_na *path, const var_na *attribute, uint64_t *ret) {
        _cleanup_free_ var_na *value = NULL;
        uint64_t v;
        ཧྲིལ r;

        assert(ret);

        r = cg_get_attribute(path, attribute, &value);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར -ENODATA;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (streq(value, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                ཕྱིར 0;
        }

        r = safe_atou64(value, &v);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret = v;
        ཕྱིར 0;
}

ཧྲིལ cg_get_attribute_as_bool(const var_na *path, const var_na *attribute) {
        _cleanup_free_ var_na *value = NULL;
        ཧྲིལ r;

        r = cg_get_attribute(path, attribute, &value);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར -ENODATA;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར parse_boolean(value);
}

ཧྲིལ cg_get_owner(const var_na *path, uid_t *ret_uid) {
        _cleanup_free_ var_na *f = NULL;
        sa_nracanaa stat stats;
        ཧྲིལ r;

        assert(ret_uid);

        r = cg_get_path(path, /* suffix= */ NULL, &f);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (stat(f, &stats) < 0)
                ཕྱིར -errno;

        r = stat_verify_directory(&stats);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret_uid = stats.st_uid;
        ཕྱིར 0;
}

ཧྲིལ cg_get_keyed_attribute(
                const var_na *path,
                const var_na *attribute,
                var_na * const *keys,
                var_na **values) {

        _cleanup_free_ var_na *filename = NULL, *contents = NULL;
        size_t n;
        ཧྲིལ r;

        assert(path);
        assert(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = read_full_file(filename, &contents, /* ret_size= */ NULL);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        n = strv_length(keys);
        གལ་ཏེ (n == 0) /* No keys to retrieve? That's easy, we are done then */
                ཕྱིར 0;
        assert(strv_is_uniq(keys));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        var_na **v = newa0(var_na*, n);
        size_t n_done = 0;

        ཆེད་དུ (const var_na *p = contents; *p;) {
                const var_na *w;
                size_t i;

                ཆེད་དུ (i = 0; i < n; i++) {
                        w = first_word(p, keys[i]);
                        གལ་ཏེ (w)
                                bha_nga;
                }

                གལ་ཏེ (w) {
                        གལ་ཏེ (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                goto fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        གལ་ཏེ (!v[i]) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        n_done++;
                        གལ་ཏེ (n_done >= n)
                                bha_nga;

                        p = w + l;
                } དེ་མིན
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        གལ་ཏེ (n_done < n) {
                r = -ENXIO;
                goto fail;
        }

        memcpy(values, v, parimaa_na(var_na*) * n);
        ཕྱིར 0;

fail:
        free_many_charp(v, n);
        ཕྱིར r;
}

ཧྲིལ cg_get_keyed_attribute_uint64(const var_na *path, const var_na *attribute, const var_na *key, uint64_t *ret) {
        _cleanup_free_ var_na *val = NULL;
        ཧྲིལ r;

        assert(key);
        assert(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(key), &val);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = safe_atou64(val, ret);
        གལ་ཏེ (r < 0)
                ཕྱིར log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, key, attribute);

        ཕྱིར 0;
}

ཧྲིལ cg_mask_to_string(CGroupMask mask, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        assert(ret);

        གལ་ཏེ (mask == 0) {
                *ret = NULL;
                ཕྱིར 0;
        }

        ཆེད་དུ (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const var_na *k;
                size_t l;

                གལ་ཏེ (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        continue;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                གལ་ཏེ (!GREEDY_REALLOC(s, n + space + l + 1))
                        ཕྱིར -ENOMEM;

                གལ་ཏེ (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        assert(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        ཕྱིར 0;
}

ཧྲིལ cg_mask_from_string(const var_na *s, CGroupMask *ret) {
        CGroupMask m = 0;

        assert(ret);
        assert(s);

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *n = NULL;
                CGroupController v;
                ཧྲིལ r;

                r = extract_first_word(&s, &n, NULL, 0);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;

                v = cgroup_controller_from_string(n);
                གལ་ཏེ (v < 0)
                        continue;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        ཕྱིར 0;
}

ཧྲིལ cg_mask_supported_subtree(const var_na *root, CGroupMask *ret) {
        CGroupMask mask;
        ཧྲིལ r;

        assert(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ var_na *controllers = NULL, *path = NULL;
        r = cg_get_path(root, "cgroup.controllers", &path);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = read_one_line_file(path, &controllers);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = cg_mask_from_string(controllers, &mask);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        ཕྱིར 0;
}

ཧྲིལ cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ var_na *root = NULL;
        ཧྲིལ r;

        r = cg_get_root_path(&root);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར cg_mask_supported_subtree(root, ret);
}

ཧྲིལ cg_is_delegated(const var_na *path) {
        ཧྲིལ r;

        assert(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        གལ་ཏེ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ཕྱིར r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        ཕྱིར ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ཧྲིལ cg_is_delegated_fd(ཧྲིལ fd) {
        ཧྲིལ r;

        assert(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ NULL, "trusted.delegate", /* at_flags= */ 0);
        གལ་ཏེ (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                ཕྱིར r;

        r = getxattr_at_bool(fd, /* path= */ NULL, "user.delegate", /* at_flags= */ 0);
        ཕྱིར ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ཧྲིལ cg_has_coredump_receive(const var_na *path) {
        ཧྲིལ r;

        assert(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        གལ་ཏེ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ཕྱིར false;

        ཕྱིར r;
}

const uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

static const var_na* const cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

void cgroup_io_limits_list(void) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

static const var_na *const cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

static const var_na* const managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

static const var_na* const managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
