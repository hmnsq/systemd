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

ціле cg_is_available(void) {
        struct statfs fs;

        якщо (statfs("/sys/fs/cgroup/", &fs) < 0) {
                якщо (errno == ENOENT) /* sysfs not mounted? */
                        повернути false;

                повернути log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        повернути is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

ціле cg_path_open(const char *path) {
        _cleanup_free_ char *fs = NULL;
        ціле r;

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        якщо (r < 0)
                повернути r;

        повернути RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

ціле cg_cgroupid_open(ціле cgroupfs_fd, uint64_t id) {
        _cleanup_close_ ціле fsfd = -EBADF;

        якщо (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                якщо (fsfd < 0)
                        повернути -errno;

                cgroupfs_fd = fsfd;
        }

        union {
                struct file_handle file_handle;
                uint8_t space[offsetof(struct file_handle, f_handle) + sizeof(uint64_t)];
        } fh = {
                .file_handle.handle_bytes = sizeof(uint64_t),
                .file_handle.handle_type = FILEID_KERNFS,
        };

        unaligned_write_ne64(fh.file_handle.f_handle, id);

        повернути RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

ціле cg_path_from_cgroupid(ціле cgroupfs_fd, uint64_t id, char **ret) {
        _cleanup_close_ ціле cgfd = -EBADF;
        ціле r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        якщо (cgfd < 0)
                повернути cgfd;

        _cleanup_free_ char *path = NULL;
        r = fd_get_path(cgfd, &path);
        якщо (r < 0)
                повернути r;

        якщо (!path_startswith(path, "/sys/fs/cgroup/"))
                повернути -EXDEV; /* recognizable error */

        якщо (ret)
                *ret = TAKE_PTR(path);
        повернути 0;
}

ціле cg_enumerate_processes(const char *path, FILE **ret) {
        _cleanup_free_ char *fs = NULL;
        FILE *f;
        ціле r;

        assert(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        якщо (r < 0)
                повернути r;

        f = fopen(fs, "re");
        якщо (!f)
                повернути -errno;

        *ret = f;
        повернути 0;
}

ціле cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        assert(f);
        assert(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        для (;;) {
                errno = 0;
                якщо (fscanf(f, "%lu", &ul) != 1) {

                        якщо (feof(f)) {
                                *ret = 0;
                                повернути 0;
                        }

                        повернути errno_or_else(EIO);
                }

                якщо (ul > PID_T_MAX)
                        повернути -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                якщо (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        continue;

                *ret = (pid_t) ul;
                повернути 1;
        }
}

ціле cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        ціле r;

        assert(f);
        assert(ret);

        для (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                якщо (r < 0)
                        повернути log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                якщо (r == 0) {
                        *ret = PIDREF_NULL;
                        повернути 0;
                }

                якщо (pid == 0)
                        повернути -EREMOTE;

                r = pidref_set_pid(ret, pid);
                якщо (r >= 0)
                        повернути 1;
                якщо (r != -ESRCH)
                        повернути r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(void) {
        static thread_local ціле supported = -1;

        якщо (supported >= 0)
                повернути supported;

        якщо (cg_is_available() <= 0)
                повернути (supported = false);

        якщо (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                повернути (supported = true);
        якщо (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        повернути (supported = false);
}

ціле cg_enumerate_subgroups(const char *path, DIR **ret) {
        _cleanup_free_ char *fs = NULL;
        DIR *d;
        ціле r;

        assert(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        якщо (r < 0)
                повернути r;

        d = opendir(fs);
        якщо (!d)
                повернути -errno;

        *ret = d;
        повернути 0;
}

ціле cg_read_subgroup(DIR *d, char **ret) {
        assert(d);
        assert(ret);

        FOREACH_DIRENT_ALL(de, d, повернути -errno) {
                якщо (de->d_type != DT_DIR)
                        continue;

                якщо (dot_or_dot_dot(de->d_name))
                        continue;

                повернути strdup_to_full(ret, de->d_name);
        }

        *ret = NULL;
        повернути 0;
}

ціле cg_kill(
                const char *path,
                ціле sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        ціле r, ret = 0;

        assert(path);
        assert(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        якщо (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        якщо (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                якщо (!killed_pids)
                        повернути -ENOMEM;
        }

        bool done;
        do {
                _cleanup_fclose_ FILE *f = NULL;
                ціле ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                якщо (r == -ENOENT)
                        break;
                якщо (r < 0)
                        повернути RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                для (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        якщо (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                break;
                        }
                        якщо (r < 0)
                                повернути RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        якщо (r == 0)
                                break;

                        якщо ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                continue;

                        якщо (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                continue;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        якщо (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                continue;
                        }

                        якщо (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        якщо (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        якщо (r >= 0) {
                                якщо (flags & CGROUP_SIGCONT)
                                        (void) pidref_kill(&pidref, SIGCONT);

                                якщо (ret == 0) {
                                        якщо (log_kill)
                                                ret = ret_log_kill;
                                        інакше
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        якщо (r < 0)
                                повернути RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } поки (!done);

        повернути ret;
}

ціле cg_kill_recursive(
                const char *path,
                ціле sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        ціле r, ret;

        assert(path);
        assert(sig >= 0);

        якщо (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                якщо (!killed_pids)
                        повернути -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        якщо (r < 0) {
                якщо (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                повернути ret;
        }

        для (;;) {
                _cleanup_free_ char *fn = NULL, *p = NULL;

                r = cg_read_subgroup(d, &fn);
                якщо (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        break;
                }
                якщо (r == 0)
                        break;

                p = path_join(empty_to_root(path), fn);
                якщо (!p)
                        повернути -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                якщо (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                якщо (r != 0 && ret >= 0)
                        ret = r;
        }

        повернути ret;
}

ціле cg_kill_kernel_sigkill(const char *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ char *killfile = NULL;
        uint64_t n_pids = UINT64_MAX;
        ціле r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        assert(path);

        якщо (!cg_kill_supported())
                повернути -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        якщо (r < 0)
                повернути r;

        якщо (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                якщо (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        якщо (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        повернути 0;
}

ціле cg_get_path(const char *path, const char *suffix, char **ret) {
        char *t;

        assert(ret);

        якщо (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        якщо (!t)
                повернути -ENOMEM;

        *ret = path_simplify(t);
        повернути 0;
}

ціле cg_set_xattr(const char *path, const char *name, const void *value, size_t size, ціле flags) {
        _cleanup_free_ char *fs = NULL;
        ціле r;

        assert(path);
        assert(name);
        assert(value || size <= 0);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        якщо (r < 0)
                повернути r;

        повернути RET_NERRNO(setxattr(fs, name, value, size, flags));
}

ціле cg_get_xattr(const char *path, const char *name, char **ret, size_t *ret_size) {
        _cleanup_free_ char *fs = NULL;
        ціле r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        якщо (r < 0)
                повернути r;

        повернути lgetxattr_malloc(fs, name, ret, ret_size);
}

ціле cg_get_xattr_bool(const char *path, const char *name) {
        _cleanup_free_ char *fs = NULL;
        ціле r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        якщо (r < 0)
                повернути r;

        повернути getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

ціле cg_remove_xattr(const char *path, const char *name) {
        _cleanup_free_ char *fs = NULL;
        ціле r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        якщо (r < 0)
                повернути r;

        повернути RET_NERRNO(removexattr(fs, name));
}

ціле cg_pid_get_path(pid_t pid, char **ret_path) {
        _cleanup_fclose_ FILE *f = NULL;
        const char *fs;
        ціле r;

        assert(pid >= 0);
        assert(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        якщо (r == -ENOENT)
                повернути -ESRCH;
        якщо (r < 0)
                повернути r;

        для (;;) {
                _cleanup_free_ char *line = NULL;
                char *e;

                r = read_line(f, LONG_LINE_MAX, &line);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        повернути -ENODATA;

                e = startswith(line, "0:");
                якщо (!e)
                        continue;

                e = strchr(e, ':');
                якщо (!e)
                        continue;

                _cleanup_free_ char *path = strdup(e + 1);
                якщо (!path)
                        повернути -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                якщо (startswith(path, "/../"))
                        повернути -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                якщо (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                повернути 0;
        }
}

ціле cg_pidref_get_path(const PidRef *pidref, char **ret_path) {
        _cleanup_free_ char *path = NULL;
        ціле r;

        assert(ret_path);

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;
        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        якщо (r < 0)
                повернути r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        якщо (r < 0)
                повернути r;

        *ret_path = TAKE_PTR(path);
        повернути 0;
}

ціле cg_is_empty(const char *path) {
        _cleanup_free_ char *t = NULL;
        ціле r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        assert(path);

        /* The root cgroup is always populated */
        якщо (empty_or_root(path))
                повернути false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        якщо (r == -ENOENT)
                повернути true;
        якщо (r < 0)
                повернути r;

        повернути streq(t, "0");
}

ціле cg_split_spec(const char *spec, char **ret_controller, char **ret_path) {
        _cleanup_free_ char *controller = NULL;
        const char *path;
        ціле r;

        assert(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        якщо (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                goto finalize;
        }

        const char *e = strchr(spec, ':');
        якщо (!e) {
                /* Controller only. */
                якщо (ret_controller) {
                        controller = strdup(spec);
                        якщо (!controller)
                                повернути -ENOMEM;
                }

                path = NULL;
        } інакше {
                /* Both controller and path. */
                якщо (ret_controller) {
                        controller = strndup(spec, e - spec);
                        якщо (!controller)
                                повернути -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        якщо (path) {
                /* Non-empty path must be absolute. */
                якщо (!path_is_absolute(path))
                        повернути -EINVAL;

                /* Path must not contain dot-dot. */
                якщо (!path_is_safe(path))
                        повернути -EINVAL;
        }

        якщо (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        повернути 0;
}

ціле cg_get_root_path(char **ret_path) {
        char *p, *e;
        ціле r;

        assert(ret_path);

        r = cg_pid_get_path(1, &p);
        якщо (r < 0)
                повернути r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        якщо (e)
                *e = 0;

        *ret_path = p;
        повернути 0;
}

ціле cg_shift_path(const char *cgroup, const char *root, const char **ret_shifted) {
        ціле r;

        assert(cgroup);
        assert(ret_shifted);

        _cleanup_free_ char *rt = NULL;
        якщо (!root) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_root_path(&rt);
                якщо (r < 0)
                        повернути r;

                root = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, root, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        повернути 0;
}

ціле cg_pid_get_path_shifted(pid_t pid, const char *root, char **ret_cgroup) {
        _cleanup_free_ char *raw = NULL;
        const char *c;
        ціле r;

        assert(pid >= 0);
        assert(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        якщо (r < 0)
                повернути r;

        r = cg_shift_path(raw, root, &c);
        якщо (r < 0)
                повернути r;

        якщо (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                повернути 0;
        }

        повернути strdup_to(ret_cgroup, c);
}

ціле cg_pidref_get_path_shifted(const PidRef *pidref, const char *root, char **ret_cgroup) {
        _cleanup_free_ char *raw = NULL;
        const char *c;
        ціле r;

        assert(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        якщо (r < 0)
                повернути r;

        r = cg_shift_path(raw, root, &c);
        якщо (r < 0)
                повернути r;

        якщо (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                повернути 0;
        }

        повернути strdup_to(ret_cgroup, c);
}

ціле cg_path_decode_unit(const char *cgroup, char **ret_unit) {
        assert(cgroup);

        size_t n = strcspn(cgroup, "/");
        якщо (n < 3)
                повернути -ENXIO;

        char *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        якщо (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                повернути -ENXIO;

        якщо (ret_unit)
                повернути strdup_to(ret_unit, c);

        повернути 0;
}

static bool valid_slice_name(const char *p, size_t n) {
        assert(p || n == 0);

        якщо (n < STRLEN("x.slice"))
                повернути false;

        char *c = strndupa_safe(p, n);
        якщо (!endswith(c, ".slice"))
                повернути false;

        повернути unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

static const char* skip_slices(const char *p) {
        assert(p);

        /* Skips over all slice assignments */

        для (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                якщо (!valid_slice_name(p, n))
                        повернути p;

                p += n;
        }
}

ціле cg_path_get_unit_full(const char *path, char **ret_unit, char **ret_subgroup) {
        ціле r;

        assert(path);

        const char *e = skip_slices(path);

        _cleanup_free_ char *unit = NULL;
        r = cg_path_decode_unit(e, &unit);
        якщо (r < 0)
                повернути r;

        /* We skipped over the slices, don't accept any now */
        якщо (endswith(unit, ".slice"))
                повернути -ENXIO;

        якщо (ret_subgroup) {
                _cleanup_free_ char *subgroup = NULL;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                якщо (isempty(e))
                        subgroup = NULL;
                інакше {
                        subgroup = strdup(e);
                        якщо (!subgroup)
                                повернути -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        якщо (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        повернути 0;
}

ціле cg_path_get_unit_path(const char *path, char **ret) {
        _cleanup_free_ char *path_copy = NULL;
        char *unit_name;

        assert(path);
        assert(ret);

        path_copy = strdup(path);
        якщо (!path_copy)
                повернути -ENOMEM;

        unit_name = (char*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        якщо (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                повернути -ENXIO;

        *ret = TAKE_PTR(path_copy);

        повернути 0;
}

ціле cg_pid_get_unit_full(pid_t pid, char **ret_unit, char **ret_subgroup) {
        ціле r;

        _cleanup_free_ char *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

ціле cg_pidref_get_unit_full(const PidRef *pidref, char **ret_unit, char **ret_subgroup) {
        ціле r;

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;
        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        _cleanup_free_ char *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        якщо (r < 0)
                повернути r;

        r = pidref_verify(pidref);
        якщо (r < 0)
                повернути r;

        якщо (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        якщо (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        повернути 0;
}

static const char* skip_session(const char *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        якщо (isempty(p))
                повернути NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        якщо (n < STRLEN("session-x.scope"))
                повернути NULL;

        const char *s = startswith(p, "session-");
        якщо (!s)
                повернути NULL;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        char *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        якщо (!e)
                повернути NULL;
        *e = '\0';

        якщо (!session_id_valid(f))
                повернути NULL;

        повернути skip_leading_slash(p + n);
}

static const char* skip_user_manager(const char *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        якщо (isempty(p))
                повернути NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        якщо (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                повернути NULL;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ char *unit_name = strndup(p, n);
        якщо (!unit_name)
                повернути NULL;

        _cleanup_free_ char *i = NULL;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        якщо (type != UNIT_NAME_INSTANCE)
                повернути NULL;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        якщо (!(startswith(unit_name, "user@") && parse_uid(i, NULL) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                повернути NULL;

        повернути skip_leading_slash(p + n);
}

static const char* skip_user_prefix(const char *path) {
        const char *e, *t;

        assert(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        якщо (t)
                повернути t;

        /* Alternatively skip the user session if it is in the path... */
        повернути skip_session(e);
}

ціле cg_path_get_user_unit_full(const char *path, char **ret_unit, char **ret_subgroup) {
        const char *t;

        assert(path);

        t = skip_user_prefix(path);
        якщо (!t)
                повернути -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        повернути cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

ціле cg_pid_get_user_unit_full(pid_t pid, char **ret_unit, char **ret_subgroup) {
        ціле r;

        _cleanup_free_ char *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

ціле cg_pidref_get_user_unit_full(const PidRef *pidref, char **ret_unit, char **ret_subgroup) {
        ціле r;

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;
        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        _cleanup_free_ char *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        якщо (r < 0)
                повернути r;

        r = pidref_verify(pidref);
        якщо (r < 0)
                повернути r;

        якщо (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        якщо (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        повернути 0;
}

ціле cg_path_get_machine_name(const char *path, char **ret_machine) {
        _cleanup_free_ char *u = NULL;
        const char *sl;
        ціле r;

        r = cg_path_get_unit(path, &u);
        якщо (r < 0)
                повернути r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        повернути readlink_malloc(sl, ret_machine);
}

ціле cg_pid_get_machine_name(pid_t pid, char **ret_machine) {
        _cleanup_free_ char *cgroup = NULL;
        ціле r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_machine_name(cgroup, ret_machine);
}

ціле cg_path_get_session(const char *path, char **ret_session) {
        _cleanup_free_ char *unit = NULL;
        char *start, *end;
        ціле r;

        assert(path);

        r = cg_path_get_unit(path, &unit);
        якщо (r < 0)
                повернути r;

        start = startswith(unit, "session-");
        якщо (!start)
                повернути -ENXIO;
        end = endswith(start, ".scope");
        якщо (!end)
                повернути -ENXIO;

        *end = 0;
        якщо (!session_id_valid(start))
                повернути -ENXIO;

        якщо (!ret_session)
                повернути 0;

        повернути strdup_to(ret_session, start);
}

ціле cg_pid_get_session(pid_t pid, char **ret_session) {
        _cleanup_free_ char *cgroup = NULL;
        ціле r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_session(cgroup, ret_session);
}

ціле cg_pidref_get_session(const PidRef *pidref, char **ret) {
        ціле r;

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;
        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        _cleanup_free_ char *session = NULL;
        r = cg_pid_get_session(pidref->pid, &session);
        якщо (r < 0)
                повернути r;

        r = pidref_verify(pidref);
        якщо (r < 0)
                повернути r;

        якщо (ret)
                *ret = TAKE_PTR(session);
        повернути 0;
}

ціле cg_path_get_owner_uid(const char *path, uid_t *ret_uid) {
        _cleanup_free_ char *slice = NULL;
        char *start, *end;
        ціле r;

        assert(path);

        r = cg_path_get_slice(path, &slice);
        якщо (r < 0)
                повернути r;

        start = startswith(slice, "user-");
        якщо (!start)
                повернути -ENXIO;

        end = endswith(start, ".slice");
        якщо (!end)
                повернути -ENXIO;

        *end = 0;
        якщо (parse_uid(start, ret_uid) < 0)
                повернути -ENXIO;

        повернути 0;
}

ціле cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ char *cgroup = NULL;
        ціле r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_owner_uid(cgroup, ret_uid);
}

ціле cg_pidref_get_owner_uid(const PidRef *pidref, uid_t *ret) {
        ціле r;

        якщо (!pidref_is_set(pidref))
                повернути -ESRCH;
        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        якщо (r < 0)
                повернути r;

        r = pidref_verify(pidref);
        якщо (r < 0)
                повернути r;

        якщо (ret)
                *ret = uid;

        повернути 0;
}

ціле cg_path_get_slice(const char *p, char **ret_slice) {
        const char *e = NULL;

        assert(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        для (;;) {
                const char *s;
                ціле n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                якщо (n < 0)
                        повернути n;
                якщо (!valid_slice_name(s, n))
                        break;

                e = s;
        }

        якщо (e)
                повернути cg_path_decode_unit(e, ret_slice);

        якщо (ret_slice)
                повернути strdup_to(ret_slice, SPECIAL_ROOT_SLICE);

        повернути 0;
}

ціле cg_pid_get_slice(pid_t pid, char **ret_slice) {
        _cleanup_free_ char *cgroup = NULL;
        ціле r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_slice(cgroup, ret_slice);
}

ціле cg_path_get_user_slice(const char *p, char **ret_slice) {
        const char *t;
        assert(p);

        t = skip_user_prefix(p);
        якщо (!t)
                повернути -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        повернути cg_path_get_slice(t, ret_slice);
}

ціле cg_pid_get_user_slice(pid_t pid, char **ret_slice) {
        _cleanup_free_ char *cgroup = NULL;
        ціле r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        якщо (r < 0)
                повернути r;

        повернути cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(const char *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        якщо (!filename_is_valid(p))
                повернути true;

        якщо (IN_SET(p[0], '_', '.'))
                повернути true;

        якщо (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                повернути true;

        якщо (startswith(p, "cgroup."))
                повернути true;

        для (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const char *q;

                q = startswith(p, cgroup_controller_to_string(c));
                якщо (!q)
                        continue;

                якщо (q[0] == '.')
                        повернути true;
        }

        повернути false;
}

ціле cg_escape(const char *p, char **ret) {
        _cleanup_free_ char *n = NULL;

        assert(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        якщо (cg_needs_escape(p)) {
                n = strjoin("_", p);
                якщо (!n)
                        повернути -ENOMEM;

                якщо (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        повернути -EINVAL;
        } інакше {
                n = strdup(p);
                якщо (!n)
                        повернути -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        повернути 0;
}

char* cg_unescape(const char *p) {
        assert(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        якщо (p[0] == '_')
                повернути (char*) p+1;

        повернути (char*) p;
}

ціле cg_slice_to_path(const char *unit, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL, *e = NULL;
        const char *dash;
        ціле r;

        assert(unit);
        assert(ret);

        якщо (streq(unit, SPECIAL_ROOT_SLICE))
                повернути strdup_to(ret, "");

        якщо (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                повернути -EINVAL;

        якщо (!endswith(unit, ".slice"))
                повернути -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        якщо (r < 0)
                повернути r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        якщо (dash == p)
                повернути -EINVAL;

        поки (dash) {
                _cleanup_free_ char *escaped = NULL;
                char n[dash - p + sizeof(".slice")];

#якщо HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                якщо (IN_SET(dash[1], 0, '-'))
                        повернути -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                якщо (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        повернути -EINVAL;

                r = cg_escape(n, &escaped);
                якщо (r < 0)
                        повернути r;

                якщо (!strextend(&s, escaped, "/"))
                        повернути -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        якщо (r < 0)
                повернути r;

        якщо (!strextend(&s, e))
                повернути -ENOMEM;

        *ret = TAKE_PTR(s);
        повернути 0;
}

ціле cg_is_threaded(const char *path) {
        _cleanup_free_ char *fs = NULL, *contents = NULL;
        _cleanup_strv_free_ char **v = NULL;
        ціле r;

        r = cg_get_path(path, "cgroup.type", &fs);
        якщо (r < 0)
                повернути r;

        r = read_full_virtual_file(fs, &contents, NULL);
        якщо (r == -ENOENT)
                повернути false; /* Assume no. */
        якщо (r < 0)
                повернути r;

        v = strv_split(contents, NULL);
        якщо (!v)
                повернути -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        повернути strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

ціле cg_set_attribute(const char *path, const char *attribute, const char *value) {
        _cleanup_free_ char *p = NULL;
        ціле r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        якщо (r < 0)
                повернути r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        повернути write_string_file(p, value, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

ціле cg_get_attribute(const char *path, const char *attribute, char **ret) {
        _cleanup_free_ char *p = NULL;
        ціле r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        якщо (r < 0)
                повернути r;

        повернути read_one_line_file(p, ret);
}

ціле cg_get_attribute_as_uint64(const char *path, const char *attribute, uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        uint64_t v;
        ціле r;

        assert(ret);

        r = cg_get_attribute(path, attribute, &value);
        якщо (r == -ENOENT)
                повернути -ENODATA;
        якщо (r < 0)
                повернути r;

        якщо (streq(value, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                повернути 0;
        }

        r = safe_atou64(value, &v);
        якщо (r < 0)
                повернути r;

        *ret = v;
        повернути 0;
}

ціле cg_get_attribute_as_bool(const char *path, const char *attribute) {
        _cleanup_free_ char *value = NULL;
        ціле r;

        r = cg_get_attribute(path, attribute, &value);
        якщо (r == -ENOENT)
                повернути -ENODATA;
        якщо (r < 0)
                повернути r;

        повернути parse_boolean(value);
}

ціле cg_get_owner(const char *path, uid_t *ret_uid) {
        _cleanup_free_ char *f = NULL;
        struct stat stats;
        ціле r;

        assert(ret_uid);

        r = cg_get_path(path, /* suffix= */ NULL, &f);
        якщо (r < 0)
                повернути r;

        якщо (stat(f, &stats) < 0)
                повернути -errno;

        r = stat_verify_directory(&stats);
        якщо (r < 0)
                повернути r;

        *ret_uid = stats.st_uid;
        повернути 0;
}

ціле cg_get_keyed_attribute(
                const char *path,
                const char *attribute,
                char * const *keys,
                char **values) {

        _cleanup_free_ char *filename = NULL, *contents = NULL;
        size_t n;
        ціле r;

        assert(path);
        assert(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        якщо (r < 0)
                повернути r;

        r = read_full_file(filename, &contents, /* ret_size= */ NULL);
        якщо (r < 0)
                повернути r;

        n = strv_length(keys);
        якщо (n == 0) /* No keys to retrieve? That's easy, we are done then */
                повернути 0;
        assert(strv_is_uniq(keys));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        char **v = newa0(char*, n);
        size_t n_done = 0;

        для (const char *p = contents; *p;) {
                const char *w;
                size_t i;

                для (i = 0; i < n; i++) {
                        w = first_word(p, keys[i]);
                        якщо (w)
                                break;
                }

                якщо (w) {
                        якщо (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                goto fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        якщо (!v[i]) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        n_done++;
                        якщо (n_done >= n)
                                break;

                        p = w + l;
                } інакше
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        якщо (n_done < n) {
                r = -ENXIO;
                goto fail;
        }

        memcpy(values, v, sizeof(char*) * n);
        повернути 0;

fail:
        free_many_charp(v, n);
        повернути r;
}

ціле cg_get_keyed_attribute_uint64(const char *path, const char *attribute, const char *key, uint64_t *ret) {
        _cleanup_free_ char *val = NULL;
        ціле r;

        assert(key);
        assert(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(key), &val);
        якщо (r < 0)
                повернути r;

        r = safe_atou64(val, ret);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, key, attribute);

        повернути 0;
}

ціле cg_mask_to_string(CGroupMask mask, char **ret) {
        _cleanup_free_ char *s = NULL;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        assert(ret);

        якщо (mask == 0) {
                *ret = NULL;
                повернути 0;
        }

        для (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const char *k;
                size_t l;

                якщо (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        continue;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                якщо (!GREEDY_REALLOC(s, n + space + l + 1))
                        повернути -ENOMEM;

                якщо (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        assert(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        повернути 0;
}

ціле cg_mask_from_string(const char *s, CGroupMask *ret) {
        CGroupMask m = 0;

        assert(ret);
        assert(s);

        для (;;) {
                _cleanup_free_ char *n = NULL;
                CGroupController v;
                ціле r;

                r = extract_first_word(&s, &n, NULL, 0);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        break;

                v = cgroup_controller_from_string(n);
                якщо (v < 0)
                        continue;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        повернути 0;
}

ціле cg_mask_supported_subtree(const char *root, CGroupMask *ret) {
        CGroupMask mask;
        ціле r;

        assert(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ char *controllers = NULL, *path = NULL;
        r = cg_get_path(root, "cgroup.controllers", &path);
        якщо (r < 0)
                повернути r;

        r = read_one_line_file(path, &controllers);
        якщо (r < 0)
                повернути r;

        r = cg_mask_from_string(controllers, &mask);
        якщо (r < 0)
                повернути r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        повернути 0;
}

ціле cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ char *root = NULL;
        ціле r;

        r = cg_get_root_path(&root);
        якщо (r < 0)
                повернути r;

        повернути cg_mask_supported_subtree(root, ret);
}

ціле cg_is_delegated(const char *path) {
        ціле r;

        assert(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        якщо (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                повернути r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        повернути ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ціле cg_is_delegated_fd(ціле fd) {
        ціле r;

        assert(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ NULL, "trusted.delegate", /* at_flags= */ 0);
        якщо (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                повернути r;

        r = getxattr_at_bool(fd, /* path= */ NULL, "user.delegate", /* at_flags= */ 0);
        повернути ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

ціле cg_has_coredump_receive(const char *path) {
        ціле r;

        assert(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        якщо (ERRNO_IS_NEG_XATTR_ABSENT(r))
                повернути false;

        повернути r;
}

const uint64_t cgroup_io_limit_defaults[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WBPS_MAX]  = CGROUP_LIMIT_MAX,
        [CGROUP_IO_RIOPS_MAX] = CGROUP_LIMIT_MAX,
        [CGROUP_IO_WIOPS_MAX] = CGROUP_LIMIT_MAX,
};

static const char* const cgroup_io_limit_type_table[_CGROUP_IO_LIMIT_TYPE_MAX] = {
        [CGROUP_IO_RBPS_MAX]  = "IOReadBandwidthMax",
        [CGROUP_IO_WBPS_MAX]  = "IOWriteBandwidthMax",
        [CGROUP_IO_RIOPS_MAX] = "IOReadIOPSMax",
        [CGROUP_IO_WIOPS_MAX] = "IOWriteIOPSMax",
};

DEFINE_STRING_TABLE_LOOKUP(cgroup_io_limit_type, CGroupIOLimitType);

void cgroup_io_limits_list(void) {
        DUMP_STRING_TABLE(cgroup_io_limit_type, CGroupIOLimitType, _CGROUP_IO_LIMIT_TYPE_MAX);
}

static const char *const cgroup_controller_table[_CGROUP_CONTROLLER_MAX] = {
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

static const char* const managed_oom_mode_table[_MANAGED_OOM_MODE_MAX] = {
        [MANAGED_OOM_AUTO] = "auto",
        [MANAGED_OOM_KILL] = "kill",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_mode, ManagedOOMMode);

static const char* const managed_oom_preference_table[_MANAGED_OOM_PREFERENCE_MAX] = {
        [MANAGED_OOM_PREFERENCE_NONE] = "none",
        [MANAGED_OOM_PREFERENCE_AVOID] = "avoid",
        [MANAGED_OOM_PREFERENCE_OMIT] = "omit",
};

DEFINE_STRING_TABLE_LOOKUP(managed_oom_preference, ManagedOOMPreference);
