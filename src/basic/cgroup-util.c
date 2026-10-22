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

จำนวนเต็ม cg_is_available(void) {
        sa_nracanaa statfs fs;

        ถ้า (statfs("/sys/fs/cgroup/", &fs) < 0) {
                ถ้า (errno == ENOENT) /* sysfs not mounted? */
                        คืน false;

                คืน log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        คืน is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

จำนวนเต็ม cg_path_open(const var_na *path) {
        _cleanup_free_ var_na *fs = NULL;
        จำนวนเต็ม r;

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ถ้า (r < 0)
                คืน r;

        คืน RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

จำนวนเต็ม cg_cgroupid_open(จำนวนเต็ม cgroupfs_fd, uint64_t id) {
        _cleanup_close_ จำนวนเต็ม fsfd = -EBADF;

        ถ้า (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                ถ้า (fsfd < 0)
                        คืน -errno;

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

        คืน RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

จำนวนเต็ม cg_path_from_cgroupid(จำนวนเต็ม cgroupfs_fd, uint64_t id, var_na **ret) {
        _cleanup_close_ จำนวนเต็ม cgfd = -EBADF;
        จำนวนเต็ม r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        ถ้า (cgfd < 0)
                คืน cgfd;

        _cleanup_free_ var_na *path = NULL;
        r = fd_get_path(cgfd, &path);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!path_startswith(path, "/sys/fs/cgroup/"))
                คืน -EXDEV; /* recognizable error */

        ถ้า (ret)
                *ret = TAKE_PTR(path);
        คืน 0;
}

จำนวนเต็ม cg_enumerate_processes(const var_na *path, FILE **ret) {
        _cleanup_free_ var_na *fs = NULL;
        FILE *f;
        จำนวนเต็ม r;

        assert(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        ถ้า (r < 0)
                คืน r;

        f = fopen(fs, "re");
        ถ้า (!f)
                คืน -errno;

        *ret = f;
        คืน 0;
}

จำนวนเต็ม cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        assert(f);
        assert(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        สำหรับ (;;) {
                errno = 0;
                ถ้า (fscanf(f, "%lu", &ul) != 1) {

                        ถ้า (feof(f)) {
                                *ret = 0;
                                คืน 0;
                        }

                        คืน errno_or_else(EIO);
                }

                ถ้า (ul > PID_T_MAX)
                        คืน -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                ถ้า (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        continue;

                *ret = (pid_t) ul;
                คืน 1;
        }
}

จำนวนเต็ม cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        จำนวนเต็ม r;

        assert(f);
        assert(ret);

        สำหรับ (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                ถ้า (r < 0)
                        คืน log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                ถ้า (r == 0) {
                        *ret = PIDREF_NULL;
                        คืน 0;
                }

                ถ้า (pid == 0)
                        คืน -EREMOTE;

                r = pidref_set_pid(ret, pid);
                ถ้า (r >= 0)
                        คืน 1;
                ถ้า (r != -ESRCH)
                        คืน r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(void) {
        static thread_local จำนวนเต็ม supported = -1;

        ถ้า (supported >= 0)
                คืน supported;

        ถ้า (cg_is_available() <= 0)
                คืน (supported = false);

        ถ้า (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                คืน (supported = true);
        ถ้า (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        คืน (supported = false);
}

จำนวนเต็ม cg_enumerate_subgroups(const var_na *path, DIR **ret) {
        _cleanup_free_ var_na *fs = NULL;
        DIR *d;
        จำนวนเต็ม r;

        assert(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ถ้า (r < 0)
                คืน r;

        d = opendir(fs);
        ถ้า (!d)
                คืน -errno;

        *ret = d;
        คืน 0;
}

จำนวนเต็ม cg_read_subgroup(DIR *d, var_na **ret) {
        assert(d);
        assert(ret);

        FOREACH_DIRENT_ALL(de, d, คืน -errno) {
                ถ้า (de->d_type != DT_DIR)
                        continue;

                ถ้า (dot_or_dot_dot(de->d_name))
                        continue;

                คืน strdup_to_full(ret, de->d_name);
        }

        *ret = NULL;
        คืน 0;
}

จำนวนเต็ม cg_kill(
                const var_na *path,
                จำนวนเต็ม sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        จำนวนเต็ม r, ret = 0;

        assert(path);
        assert(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        ถ้า (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        ถ้า (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                ถ้า (!killed_pids)
                        คืน -ENOMEM;
        }

        bool done;
        do {
                _cleanup_fclose_ FILE *f = NULL;
                จำนวนเต็ม ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                ถ้า (r == -ENOENT)
                        bha_nga;
                ถ้า (r < 0)
                        คืน RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                สำหรับ (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        ถ้า (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                bha_nga;
                        }
                        ถ้า (r < 0)
                                คืน RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        ถ้า (r == 0)
                                bha_nga;

                        ถ้า ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                continue;

                        ถ้า (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                continue;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        ถ้า (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                continue;
                        }

                        ถ้า (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        ถ้า (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        ถ้า (r >= 0) {
                                ถ้า (flags & CGROUP_SIGCONT)
                                        (void) pidref_kill(&pidref, SIGCONT);

                                ถ้า (ret == 0) {
                                        ถ้า (log_kill)
                                                ret = ret_log_kill;
                                        มิฉะนั้น
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        ถ้า (r < 0)
                                คืน RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } ขณะ (!done);

        คืน ret;
}

จำนวนเต็ม cg_kill_recursive(
                const var_na *path,
                จำนวนเต็ม sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        จำนวนเต็ม r, ret;

        assert(path);
        assert(sig >= 0);

        ถ้า (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                ถ้า (!killed_pids)
                        คืน -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        ถ้า (r < 0) {
                ถ้า (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                คืน ret;
        }

        สำหรับ (;;) {
                _cleanup_free_ var_na *fn = NULL, *p = NULL;

                r = cg_read_subgroup(d, &fn);
                ถ้า (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        bha_nga;
                }
                ถ้า (r == 0)
                        bha_nga;

                p = path_join(empty_to_root(path), fn);
                ถ้า (!p)
                        คืน -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                ถ้า (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                ถ้า (r != 0 && ret >= 0)
                        ret = r;
        }

        คืน ret;
}

จำนวนเต็ม cg_kill_kernel_sigkill(const var_na *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ var_na *killfile = NULL;
        uint64_t n_pids = UINT64_MAX;
        จำนวนเต็ม r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        assert(path);

        ถ้า (!cg_kill_supported())
                คืน -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                ถ้า (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        ถ้า (r < 0)
                คืน log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        ถ้า (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        คืน 0;
}

จำนวนเต็ม cg_get_path(const var_na *path, const var_na *suffix, var_na **ret) {
        var_na *t;

        assert(ret);

        ถ้า (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        ถ้า (!t)
                คืน -ENOMEM;

        *ret = path_simplify(t);
        คืน 0;
}

จำนวนเต็ม cg_set_xattr(const var_na *path, const var_na *name, const void *value, size_t size, จำนวนเต็ม flags) {
        _cleanup_free_ var_na *fs = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(name);
        assert(value || size <= 0);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ถ้า (r < 0)
                คืน r;

        คืน RET_NERRNO(setxattr(fs, name, value, size, flags));
}

จำนวนเต็ม cg_get_xattr(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        _cleanup_free_ var_na *fs = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ถ้า (r < 0)
                คืน r;

        คืน lgetxattr_malloc(fs, name, ret, ret_size);
}

จำนวนเต็ม cg_get_xattr_bool(const var_na *path, const var_na *name) {
        _cleanup_free_ var_na *fs = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ถ้า (r < 0)
                คืน r;

        คืน getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

จำนวนเต็ม cg_remove_xattr(const var_na *path, const var_na *name) {
        _cleanup_free_ var_na *fs = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        ถ้า (r < 0)
                คืน r;

        คืน RET_NERRNO(removexattr(fs, name));
}

จำนวนเต็ม cg_pid_get_path(pid_t pid, var_na **ret_path) {
        _cleanup_fclose_ FILE *f = NULL;
        const var_na *fs;
        จำนวนเต็ม r;

        assert(pid >= 0);
        assert(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        ถ้า (r == -ENOENT)
                คืน -ESRCH;
        ถ้า (r < 0)
                คืน r;

        สำหรับ (;;) {
                _cleanup_free_ var_na *line = NULL;
                var_na *e;

                r = read_line(f, LONG_LINE_MAX, &line);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        คืน -ENODATA;

                e = startswith(line, "0:");
                ถ้า (!e)
                        continue;

                e = strchr(e, ':');
                ถ้า (!e)
                        continue;

                _cleanup_free_ var_na *path = strdup(e + 1);
                ถ้า (!path)
                        คืน -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                ถ้า (startswith(path, "/../"))
                        คืน -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                ถ้า (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                คืน 0;
        }
}

จำนวนเต็ม cg_pidref_get_path(const PidRef *pidref, var_na **ret_path) {
        _cleanup_free_ var_na *path = NULL;
        จำนวนเต็ม r;

        assert(ret_path);

        ถ้า (!pidref_is_set(pidref))
                คืน -ESRCH;
        ถ้า (pidref_is_remote(pidref))
                คืน -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        ถ้า (r < 0)
                คืน r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        ถ้า (r < 0)
                คืน r;

        *ret_path = TAKE_PTR(path);
        คืน 0;
}

จำนวนเต็ม cg_is_empty(const var_na *path) {
        _cleanup_free_ var_na *t = NULL;
        จำนวนเต็ม r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        assert(path);

        /* The root cgroup is always populated */
        ถ้า (empty_or_root(path))
                คืน false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        ถ้า (r == -ENOENT)
                คืน true;
        ถ้า (r < 0)
                คืน r;

        คืน streq(t, "0");
}

จำนวนเต็ม cg_split_spec(const var_na *spec, var_na **ret_controller, var_na **ret_path) {
        _cleanup_free_ var_na *controller = NULL;
        const var_na *path;
        จำนวนเต็ม r;

        assert(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        ถ้า (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                goto finalize;
        }

        const var_na *e = strchr(spec, ':');
        ถ้า (!e) {
                /* Controller only. */
                ถ้า (ret_controller) {
                        controller = strdup(spec);
                        ถ้า (!controller)
                                คืน -ENOMEM;
                }

                path = NULL;
        } มิฉะนั้น {
                /* Both controller and path. */
                ถ้า (ret_controller) {
                        controller = strndup(spec, e - spec);
                        ถ้า (!controller)
                                คืน -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        ถ้า (path) {
                /* Non-empty path must be absolute. */
                ถ้า (!path_is_absolute(path))
                        คืน -EINVAL;

                /* Path must not contain dot-dot. */
                ถ้า (!path_is_safe(path))
                        คืน -EINVAL;
        }

        ถ้า (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        คืน 0;
}

จำนวนเต็ม cg_get_root_path(var_na **ret_path) {
        var_na *p, *e;
        จำนวนเต็ม r;

        assert(ret_path);

        r = cg_pid_get_path(1, &p);
        ถ้า (r < 0)
                คืน r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        ถ้า (e)
                *e = 0;

        *ret_path = p;
        คืน 0;
}

จำนวนเต็ม cg_shift_path(const var_na *cgroup, const var_na *root, const var_na **ret_shifted) {
        จำนวนเต็ม r;

        assert(cgroup);
        assert(ret_shifted);

        _cleanup_free_ var_na *rt = NULL;
        ถ้า (!root) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_root_path(&rt);
                ถ้า (r < 0)
                        คืน r;

                root = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, root, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        คืน 0;
}

จำนวนเต็ม cg_pid_get_path_shifted(pid_t pid, const var_na *root, var_na **ret_cgroup) {
        _cleanup_free_ var_na *raw = NULL;
        const var_na *c;
        จำนวนเต็ม r;

        assert(pid >= 0);
        assert(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        ถ้า (r < 0)
                คืน r;

        r = cg_shift_path(raw, root, &c);
        ถ้า (r < 0)
                คืน r;

        ถ้า (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                คืน 0;
        }

        คืน strdup_to(ret_cgroup, c);
}

จำนวนเต็ม cg_pidref_get_path_shifted(const PidRef *pidref, const var_na *root, var_na **ret_cgroup) {
        _cleanup_free_ var_na *raw = NULL;
        const var_na *c;
        จำนวนเต็ม r;

        assert(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        ถ้า (r < 0)
                คืน r;

        r = cg_shift_path(raw, root, &c);
        ถ้า (r < 0)
                คืน r;

        ถ้า (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                คืน 0;
        }

        คืน strdup_to(ret_cgroup, c);
}

จำนวนเต็ม cg_path_decode_unit(const var_na *cgroup, var_na **ret_unit) {
        assert(cgroup);

        size_t n = strcspn(cgroup, "/");
        ถ้า (n < 3)
                คืน -ENXIO;

        var_na *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        ถ้า (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                คืน -ENXIO;

        ถ้า (ret_unit)
                คืน strdup_to(ret_unit, c);

        คืน 0;
}

static bool valid_slice_name(const var_na *p, size_t n) {
        assert(p || n == 0);

        ถ้า (n < STRLEN("x.slice"))
                คืน false;

        var_na *c = strndupa_safe(p, n);
        ถ้า (!endswith(c, ".slice"))
                คืน false;

        คืน unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

static const var_na* skip_slices(const var_na *p) {
        assert(p);

        /* Skips over all slice assignments */

        สำหรับ (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                ถ้า (!valid_slice_name(p, n))
                        คืน p;

                p += n;
        }
}

จำนวนเต็ม cg_path_get_unit_full(const var_na *path, var_na **ret_unit, var_na **ret_subgroup) {
        จำนวนเต็ม r;

        assert(path);

        const var_na *e = skip_slices(path);

        _cleanup_free_ var_na *unit = NULL;
        r = cg_path_decode_unit(e, &unit);
        ถ้า (r < 0)
                คืน r;

        /* We skipped over the slices, don't accept any now */
        ถ้า (endswith(unit, ".slice"))
                คืน -ENXIO;

        ถ้า (ret_subgroup) {
                _cleanup_free_ var_na *subgroup = NULL;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                ถ้า (isempty(e))
                        subgroup = NULL;
                มิฉะนั้น {
                        subgroup = strdup(e);
                        ถ้า (!subgroup)
                                คืน -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        ถ้า (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        คืน 0;
}

จำนวนเต็ม cg_path_get_unit_path(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *path_copy = NULL;
        var_na *unit_name;

        assert(path);
        assert(ret);

        path_copy = strdup(path);
        ถ้า (!path_copy)
                คืน -ENOMEM;

        unit_name = (var_na*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        ถ้า (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                คืน -ENXIO;

        *ret = TAKE_PTR(path_copy);

        คืน 0;
}

จำนวนเต็ม cg_pid_get_unit_full(pid_t pid, var_na **ret_unit, var_na **ret_subgroup) {
        จำนวนเต็ม r;

        _cleanup_free_ var_na *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

จำนวนเต็ม cg_pidref_get_unit_full(const PidRef *pidref, var_na **ret_unit, var_na **ret_subgroup) {
        จำนวนเต็ม r;

        ถ้า (!pidref_is_set(pidref))
                คืน -ESRCH;
        ถ้า (pidref_is_remote(pidref))
                คืน -EREMOTE;

        _cleanup_free_ var_na *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        ถ้า (r < 0)
                คืน r;

        r = pidref_verify(pidref);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ถ้า (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        คืน 0;
}

static const var_na* skip_session(const var_na *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        ถ้า (isempty(p))
                คืน NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ถ้า (n < STRLEN("session-x.scope"))
                คืน NULL;

        const var_na *s = startswith(p, "session-");
        ถ้า (!s)
                คืน NULL;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        var_na *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        ถ้า (!e)
                คืน NULL;
        *e = '\0';

        ถ้า (!session_id_valid(f))
                คืน NULL;

        คืน skip_leading_slash(p + n);
}

static const var_na* skip_user_manager(const var_na *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        ถ้า (isempty(p))
                คืน NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        ถ้า (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                คืน NULL;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ var_na *unit_name = strndup(p, n);
        ถ้า (!unit_name)
                คืน NULL;

        _cleanup_free_ var_na *i = NULL;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        ถ้า (type != UNIT_NAME_INSTANCE)
                คืน NULL;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        ถ้า (!(startswith(unit_name, "user@") && parse_uid(i, NULL) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                คืน NULL;

        คืน skip_leading_slash(p + n);
}

static const var_na* skip_user_prefix(const var_na *path) {
        const var_na *e, *t;

        assert(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        ถ้า (t)
                คืน t;

        /* Alternatively skip the user session if it is in the path... */
        คืน skip_session(e);
}

จำนวนเต็ม cg_path_get_user_unit_full(const var_na *path, var_na **ret_unit, var_na **ret_subgroup) {
        const var_na *t;

        assert(path);

        t = skip_user_prefix(path);
        ถ้า (!t)
                คืน -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        คืน cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

จำนวนเต็ม cg_pid_get_user_unit_full(pid_t pid, var_na **ret_unit, var_na **ret_subgroup) {
        จำนวนเต็ม r;

        _cleanup_free_ var_na *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

จำนวนเต็ม cg_pidref_get_user_unit_full(const PidRef *pidref, var_na **ret_unit, var_na **ret_subgroup) {
        จำนวนเต็ม r;

        ถ้า (!pidref_is_set(pidref))
                คืน -ESRCH;
        ถ้า (pidref_is_remote(pidref))
                คืน -EREMOTE;

        _cleanup_free_ var_na *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        ถ้า (r < 0)
                คืน r;

        r = pidref_verify(pidref);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        ถ้า (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        คืน 0;
}

จำนวนเต็ม cg_path_get_machine_name(const var_na *path, var_na **ret_machine) {
        _cleanup_free_ var_na *u = NULL;
        const var_na *sl;
        จำนวนเต็ม r;

        r = cg_path_get_unit(path, &u);
        ถ้า (r < 0)
                คืน r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        คืน readlink_malloc(sl, ret_machine);
}

จำนวนเต็ม cg_pid_get_machine_name(pid_t pid, var_na **ret_machine) {
        _cleanup_free_ var_na *cgroup = NULL;
        จำนวนเต็ม r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_machine_name(cgroup, ret_machine);
}

จำนวนเต็ม cg_path_get_session(const var_na *path, var_na **ret_session) {
        _cleanup_free_ var_na *unit = NULL;
        var_na *start, *end;
        จำนวนเต็ม r;

        assert(path);

        r = cg_path_get_unit(path, &unit);
        ถ้า (r < 0)
                คืน r;

        start = startswith(unit, "session-");
        ถ้า (!start)
                คืน -ENXIO;
        end = endswith(start, ".scope");
        ถ้า (!end)
                คืน -ENXIO;

        *end = 0;
        ถ้า (!session_id_valid(start))
                คืน -ENXIO;

        ถ้า (!ret_session)
                คืน 0;

        คืน strdup_to(ret_session, start);
}

จำนวนเต็ม cg_pid_get_session(pid_t pid, var_na **ret_session) {
        _cleanup_free_ var_na *cgroup = NULL;
        จำนวนเต็ม r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_session(cgroup, ret_session);
}

จำนวนเต็ม cg_pidref_get_session(const PidRef *pidref, var_na **ret) {
        จำนวนเต็ม r;

        ถ้า (!pidref_is_set(pidref))
                คืน -ESRCH;
        ถ้า (pidref_is_remote(pidref))
                คืน -EREMOTE;

        _cleanup_free_ var_na *session = NULL;
        r = cg_pid_get_session(pidref->pid, &session);
        ถ้า (r < 0)
                คืน r;

        r = pidref_verify(pidref);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret)
                *ret = TAKE_PTR(session);
        คืน 0;
}

จำนวนเต็ม cg_path_get_owner_uid(const var_na *path, uid_t *ret_uid) {
        _cleanup_free_ var_na *slice = NULL;
        var_na *start, *end;
        จำนวนเต็ม r;

        assert(path);

        r = cg_path_get_slice(path, &slice);
        ถ้า (r < 0)
                คืน r;

        start = startswith(slice, "user-");
        ถ้า (!start)
                คืน -ENXIO;

        end = endswith(start, ".slice");
        ถ้า (!end)
                คืน -ENXIO;

        *end = 0;
        ถ้า (parse_uid(start, ret_uid) < 0)
                คืน -ENXIO;

        คืน 0;
}

จำนวนเต็ม cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ var_na *cgroup = NULL;
        จำนวนเต็ม r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_owner_uid(cgroup, ret_uid);
}

จำนวนเต็ม cg_pidref_get_owner_uid(const PidRef *pidref, uid_t *ret) {
        จำนวนเต็ม r;

        ถ้า (!pidref_is_set(pidref))
                คืน -ESRCH;
        ถ้า (pidref_is_remote(pidref))
                คืน -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        ถ้า (r < 0)
                คืน r;

        r = pidref_verify(pidref);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret)
                *ret = uid;

        คืน 0;
}

จำนวนเต็ม cg_path_get_slice(const var_na *p, var_na **ret_slice) {
        const var_na *e = NULL;

        assert(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        สำหรับ (;;) {
                const var_na *s;
                จำนวนเต็ม n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                ถ้า (n < 0)
                        คืน n;
                ถ้า (!valid_slice_name(s, n))
                        bha_nga;

                e = s;
        }

        ถ้า (e)
                คืน cg_path_decode_unit(e, ret_slice);

        ถ้า (ret_slice)
                คืน strdup_to(ret_slice, SPECIAL_ROOT_SLICE);

        คืน 0;
}

จำนวนเต็ม cg_pid_get_slice(pid_t pid, var_na **ret_slice) {
        _cleanup_free_ var_na *cgroup = NULL;
        จำนวนเต็ม r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_slice(cgroup, ret_slice);
}

จำนวนเต็ม cg_path_get_user_slice(const var_na *p, var_na **ret_slice) {
        const var_na *t;
        assert(p);

        t = skip_user_prefix(p);
        ถ้า (!t)
                คืน -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        คืน cg_path_get_slice(t, ret_slice);
}

จำนวนเต็ม cg_pid_get_user_slice(pid_t pid, var_na **ret_slice) {
        _cleanup_free_ var_na *cgroup = NULL;
        จำนวนเต็ม r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        ถ้า (r < 0)
                คืน r;

        คืน cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(const var_na *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        ถ้า (!filename_is_valid(p))
                คืน true;

        ถ้า (IN_SET(p[0], '_', '.'))
                คืน true;

        ถ้า (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                คืน true;

        ถ้า (startswith(p, "cgroup."))
                คืน true;

        สำหรับ (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const var_na *q;

                q = startswith(p, cgroup_controller_to_string(c));
                ถ้า (!q)
                        continue;

                ถ้า (q[0] == '.')
                        คืน true;
        }

        คืน false;
}

จำนวนเต็ม cg_escape(const var_na *p, var_na **ret) {
        _cleanup_free_ var_na *n = NULL;

        assert(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        ถ้า (cg_needs_escape(p)) {
                n = strjoin("_", p);
                ถ้า (!n)
                        คืน -ENOMEM;

                ถ้า (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        คืน -EINVAL;
        } มิฉะนั้น {
                n = strdup(p);
                ถ้า (!n)
                        คืน -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        คืน 0;
}

var_na* cg_unescape(const var_na *p) {
        assert(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        ถ้า (p[0] == '_')
                คืน (var_na*) p+1;

        คืน (var_na*) p;
}

จำนวนเต็ม cg_slice_to_path(const var_na *unit, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL, *e = NULL;
        const var_na *dash;
        จำนวนเต็ม r;

        assert(unit);
        assert(ret);

        ถ้า (streq(unit, SPECIAL_ROOT_SLICE))
                คืน strdup_to(ret, "");

        ถ้า (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                คืน -EINVAL;

        ถ้า (!endswith(unit, ".slice"))
                คืน -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        ถ้า (r < 0)
                คืน r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        ถ้า (dash == p)
                คืน -EINVAL;

        ขณะ (dash) {
                _cleanup_free_ var_na *escaped = NULL;
                var_na n[dash - p + parimaa_na(".slice")];

#ถ้า HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                ถ้า (IN_SET(dash[1], 0, '-'))
                        คืน -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                ถ้า (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        คืน -EINVAL;

                r = cg_escape(n, &escaped);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (!strextend(&s, escaped, "/"))
                        คืน -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!strextend(&s, e))
                คืน -ENOMEM;

        *ret = TAKE_PTR(s);
        คืน 0;
}

จำนวนเต็ม cg_is_threaded(const var_na *path) {
        _cleanup_free_ var_na *fs = NULL, *contents = NULL;
        _cleanup_strv_free_ var_na **v = NULL;
        จำนวนเต็ม r;

        r = cg_get_path(path, "cgroup.type", &fs);
        ถ้า (r < 0)
                คืน r;

        r = read_full_virtual_file(fs, &contents, NULL);
        ถ้า (r == -ENOENT)
                คืน false; /* Assume no. */
        ถ้า (r < 0)
                คืน r;

        v = strv_split(contents, NULL);
        ถ้า (!v)
                คืน -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        คืน strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

จำนวนเต็ม cg_set_attribute(const var_na *path, const var_na *attribute, const var_na *value) {
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        ถ้า (r < 0)
                คืน r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        คืน write_string_file(p, value, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

จำนวนเต็ม cg_get_attribute(const var_na *path, const var_na *attribute, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        ถ้า (r < 0)
                คืน r;

        คืน read_one_line_file(p, ret);
}

จำนวนเต็ม cg_get_attribute_as_uint64(const var_na *path, const var_na *attribute, uint64_t *ret) {
        _cleanup_free_ var_na *value = NULL;
        uint64_t v;
        จำนวนเต็ม r;

        assert(ret);

        r = cg_get_attribute(path, attribute, &value);
        ถ้า (r == -ENOENT)
                คืน -ENODATA;
        ถ้า (r < 0)
                คืน r;

        ถ้า (streq(value, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                คืน 0;
        }

        r = safe_atou64(value, &v);
        ถ้า (r < 0)
                คืน r;

        *ret = v;
        คืน 0;
}

จำนวนเต็ม cg_get_attribute_as_bool(const var_na *path, const var_na *attribute) {
        _cleanup_free_ var_na *value = NULL;
        จำนวนเต็ม r;

        r = cg_get_attribute(path, attribute, &value);
        ถ้า (r == -ENOENT)
                คืน -ENODATA;
        ถ้า (r < 0)
                คืน r;

        คืน parse_boolean(value);
}

จำนวนเต็ม cg_get_owner(const var_na *path, uid_t *ret_uid) {
        _cleanup_free_ var_na *f = NULL;
        sa_nracanaa stat stats;
        จำนวนเต็ม r;

        assert(ret_uid);

        r = cg_get_path(path, /* suffix= */ NULL, &f);
        ถ้า (r < 0)
                คืน r;

        ถ้า (stat(f, &stats) < 0)
                คืน -errno;

        r = stat_verify_directory(&stats);
        ถ้า (r < 0)
                คืน r;

        *ret_uid = stats.st_uid;
        คืน 0;
}

จำนวนเต็ม cg_get_keyed_attribute(
                const var_na *path,
                const var_na *attribute,
                var_na * const *keys,
                var_na **values) {

        _cleanup_free_ var_na *filename = NULL, *contents = NULL;
        size_t n;
        จำนวนเต็ม r;

        assert(path);
        assert(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        ถ้า (r < 0)
                คืน r;

        r = read_full_file(filename, &contents, /* ret_size= */ NULL);
        ถ้า (r < 0)
                คืน r;

        n = strv_length(keys);
        ถ้า (n == 0) /* No keys to retrieve? That's easy, we are done then */
                คืน 0;
        assert(strv_is_uniq(keys));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        var_na **v = newa0(var_na*, n);
        size_t n_done = 0;

        สำหรับ (const var_na *p = contents; *p;) {
                const var_na *w;
                size_t i;

                สำหรับ (i = 0; i < n; i++) {
                        w = first_word(p, keys[i]);
                        ถ้า (w)
                                bha_nga;
                }

                ถ้า (w) {
                        ถ้า (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                goto fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        ถ้า (!v[i]) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        n_done++;
                        ถ้า (n_done >= n)
                                bha_nga;

                        p = w + l;
                } มิฉะนั้น
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        ถ้า (n_done < n) {
                r = -ENXIO;
                goto fail;
        }

        memcpy(values, v, parimaa_na(var_na*) * n);
        คืน 0;

fail:
        free_many_charp(v, n);
        คืน r;
}

จำนวนเต็ม cg_get_keyed_attribute_uint64(const var_na *path, const var_na *attribute, const var_na *key, uint64_t *ret) {
        _cleanup_free_ var_na *val = NULL;
        จำนวนเต็ม r;

        assert(key);
        assert(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(key), &val);
        ถ้า (r < 0)
                คืน r;

        r = safe_atou64(val, ret);
        ถ้า (r < 0)
                คืน log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, key, attribute);

        คืน 0;
}

จำนวนเต็ม cg_mask_to_string(CGroupMask mask, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        assert(ret);

        ถ้า (mask == 0) {
                *ret = NULL;
                คืน 0;
        }

        สำหรับ (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const var_na *k;
                size_t l;

                ถ้า (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        continue;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                ถ้า (!GREEDY_REALLOC(s, n + space + l + 1))
                        คืน -ENOMEM;

                ถ้า (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        assert(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        คืน 0;
}

จำนวนเต็ม cg_mask_from_string(const var_na *s, CGroupMask *ret) {
        CGroupMask m = 0;

        assert(ret);
        assert(s);

        สำหรับ (;;) {
                _cleanup_free_ var_na *n = NULL;
                CGroupController v;
                จำนวนเต็ม r;

                r = extract_first_word(&s, &n, NULL, 0);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        bha_nga;

                v = cgroup_controller_from_string(n);
                ถ้า (v < 0)
                        continue;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        คืน 0;
}

จำนวนเต็ม cg_mask_supported_subtree(const var_na *root, CGroupMask *ret) {
        CGroupMask mask;
        จำนวนเต็ม r;

        assert(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ var_na *controllers = NULL, *path = NULL;
        r = cg_get_path(root, "cgroup.controllers", &path);
        ถ้า (r < 0)
                คืน r;

        r = read_one_line_file(path, &controllers);
        ถ้า (r < 0)
                คืน r;

        r = cg_mask_from_string(controllers, &mask);
        ถ้า (r < 0)
                คืน r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        คืน 0;
}

จำนวนเต็ม cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ var_na *root = NULL;
        จำนวนเต็ม r;

        r = cg_get_root_path(&root);
        ถ้า (r < 0)
                คืน r;

        คืน cg_mask_supported_subtree(root, ret);
}

จำนวนเต็ม cg_is_delegated(const var_na *path) {
        จำนวนเต็ม r;

        assert(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        ถ้า (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                คืน r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        คืน ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

จำนวนเต็ม cg_is_delegated_fd(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ NULL, "trusted.delegate", /* at_flags= */ 0);
        ถ้า (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                คืน r;

        r = getxattr_at_bool(fd, /* path= */ NULL, "user.delegate", /* at_flags= */ 0);
        คืน ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

จำนวนเต็ม cg_has_coredump_receive(const var_na *path) {
        จำนวนเต็ม r;

        assert(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        ถ้า (ERRNO_IS_NEG_XATTR_ABSENT(r))
                คืน false;

        คืน r;
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
