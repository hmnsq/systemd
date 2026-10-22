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

整数 cg_is_available(void) {
        struct statfs fs;

        如果 (statfs("/sys/fs/cgroup/", &fs) < 0) {
                如果 (errno == ENOENT) /* sysfs not mounted? */
                        返回 false;

                返回 log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        返回 is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

整数 cg_path_open(const char *path) {
        _cleanup_free_ char *fs = NULL;
        整数 r;

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        如果 (r < 0)
                返回 r;

        返回 RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

整数 cg_cgroupid_open(整数 cgroupfs_fd, uint64_t id) {
        _cleanup_close_ 整数 fsfd = -EBADF;

        如果 (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                如果 (fsfd < 0)
                        返回 -errno;

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

        返回 RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

整数 cg_path_from_cgroupid(整数 cgroupfs_fd, uint64_t id, char **ret) {
        _cleanup_close_ 整数 cgfd = -EBADF;
        整数 r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        如果 (cgfd < 0)
                返回 cgfd;

        _cleanup_free_ char *path = NULL;
        r = fd_get_path(cgfd, &path);
        如果 (r < 0)
                返回 r;

        如果 (!path_startswith(path, "/sys/fs/cgroup/"))
                返回 -EXDEV; /* recognizable error */

        如果 (ret)
                *ret = TAKE_PTR(path);
        返回 0;
}

整数 cg_enumerate_processes(const char *path, FILE **ret) {
        _cleanup_free_ char *fs = NULL;
        FILE *f;
        整数 r;

        assert(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        如果 (r < 0)
                返回 r;

        f = fopen(fs, "re");
        如果 (!f)
                返回 -errno;

        *ret = f;
        返回 0;
}

整数 cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        assert(f);
        assert(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        为 (;;) {
                errno = 0;
                如果 (fscanf(f, "%lu", &ul) != 1) {

                        如果 (feof(f)) {
                                *ret = 0;
                                返回 0;
                        }

                        返回 errno_or_else(EIO);
                }

                如果 (ul > PID_T_MAX)
                        返回 -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                如果 (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        continue;

                *ret = (pid_t) ul;
                返回 1;
        }
}

整数 cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        整数 r;

        assert(f);
        assert(ret);

        为 (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                如果 (r < 0)
                        返回 log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                如果 (r == 0) {
                        *ret = PIDREF_NULL;
                        返回 0;
                }

                如果 (pid == 0)
                        返回 -EREMOTE;

                r = pidref_set_pid(ret, pid);
                如果 (r >= 0)
                        返回 1;
                如果 (r != -ESRCH)
                        返回 r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(void) {
        static thread_local 整数 supported = -1;

        如果 (supported >= 0)
                返回 supported;

        如果 (cg_is_available() <= 0)
                返回 (supported = false);

        如果 (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                返回 (supported = true);
        如果 (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        返回 (supported = false);
}

整数 cg_enumerate_subgroups(const char *path, DIR **ret) {
        _cleanup_free_ char *fs = NULL;
        DIR *d;
        整数 r;

        assert(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        如果 (r < 0)
                返回 r;

        d = opendir(fs);
        如果 (!d)
                返回 -errno;

        *ret = d;
        返回 0;
}

整数 cg_read_subgroup(DIR *d, char **ret) {
        assert(d);
        assert(ret);

        FOREACH_DIRENT_ALL(de, d, 返回 -errno) {
                如果 (de->d_type != DT_DIR)
                        continue;

                如果 (dot_or_dot_dot(de->d_name))
                        continue;

                返回 strdup_to_full(ret, de->d_name);
        }

        *ret = NULL;
        返回 0;
}

整数 cg_kill(
                const char *path,
                整数 sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        整数 r, ret = 0;

        assert(path);
        assert(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        如果 (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        如果 (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                如果 (!killed_pids)
                        返回 -ENOMEM;
        }

        bool done;
        do {
                _cleanup_fclose_ FILE *f = NULL;
                整数 ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                如果 (r == -ENOENT)
                        break;
                如果 (r < 0)
                        返回 RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                为 (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        如果 (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                break;
                        }
                        如果 (r < 0)
                                返回 RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        如果 (r == 0)
                                break;

                        如果 ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                continue;

                        如果 (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                continue;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        如果 (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                continue;
                        }

                        如果 (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        如果 (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        如果 (r >= 0) {
                                如果 (flags & CGROUP_SIGCONT)
                                        (void) pidref_kill(&pidref, SIGCONT);

                                如果 (ret == 0) {
                                        如果 (log_kill)
                                                ret = ret_log_kill;
                                        否则
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        如果 (r < 0)
                                返回 RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } 当 (!done);

        返回 ret;
}

整数 cg_kill_recursive(
                const char *path,
                整数 sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        整数 r, ret;

        assert(path);
        assert(sig >= 0);

        如果 (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                如果 (!killed_pids)
                        返回 -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        如果 (r < 0) {
                如果 (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                返回 ret;
        }

        为 (;;) {
                _cleanup_free_ char *fn = NULL, *p = NULL;

                r = cg_read_subgroup(d, &fn);
                如果 (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        break;
                }
                如果 (r == 0)
                        break;

                p = path_join(empty_to_root(path), fn);
                如果 (!p)
                        返回 -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                如果 (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                如果 (r != 0 && ret >= 0)
                        ret = r;
        }

        返回 ret;
}

整数 cg_kill_kernel_sigkill(const char *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ char *killfile = NULL;
        uint64_t n_pids = UINT64_MAX;
        整数 r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        assert(path);

        如果 (!cg_kill_supported())
                返回 -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        如果 (r < 0)
                返回 r;

        如果 (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                如果 (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        如果 (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        返回 0;
}

整数 cg_get_path(const char *path, const char *suffix, char **ret) {
        char *t;

        assert(ret);

        如果 (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        如果 (!t)
                返回 -ENOMEM;

        *ret = path_simplify(t);
        返回 0;
}

整数 cg_set_xattr(const char *path, const char *name, const void *value, size_t size, 整数 flags) {
        _cleanup_free_ char *fs = NULL;
        整数 r;

        assert(path);
        assert(name);
        assert(value || size <= 0);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        如果 (r < 0)
                返回 r;

        返回 RET_NERRNO(setxattr(fs, name, value, size, flags));
}

整数 cg_get_xattr(const char *path, const char *name, char **ret, size_t *ret_size) {
        _cleanup_free_ char *fs = NULL;
        整数 r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        如果 (r < 0)
                返回 r;

        返回 lgetxattr_malloc(fs, name, ret, ret_size);
}

整数 cg_get_xattr_bool(const char *path, const char *name) {
        _cleanup_free_ char *fs = NULL;
        整数 r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        如果 (r < 0)
                返回 r;

        返回 getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

整数 cg_remove_xattr(const char *path, const char *name) {
        _cleanup_free_ char *fs = NULL;
        整数 r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        如果 (r < 0)
                返回 r;

        返回 RET_NERRNO(removexattr(fs, name));
}

整数 cg_pid_get_path(pid_t pid, char **ret_path) {
        _cleanup_fclose_ FILE *f = NULL;
        const char *fs;
        整数 r;

        assert(pid >= 0);
        assert(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        如果 (r == -ENOENT)
                返回 -ESRCH;
        如果 (r < 0)
                返回 r;

        为 (;;) {
                _cleanup_free_ char *line = NULL;
                char *e;

                r = read_line(f, LONG_LINE_MAX, &line);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        返回 -ENODATA;

                e = startswith(line, "0:");
                如果 (!e)
                        continue;

                e = strchr(e, ':');
                如果 (!e)
                        continue;

                _cleanup_free_ char *path = strdup(e + 1);
                如果 (!path)
                        返回 -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                如果 (startswith(path, "/../"))
                        返回 -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                如果 (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                返回 0;
        }
}

整数 cg_pidref_get_path(const PidRef *pidref, char **ret_path) {
        _cleanup_free_ char *path = NULL;
        整数 r;

        assert(ret_path);

        如果 (!pidref_is_set(pidref))
                返回 -ESRCH;
        如果 (pidref_is_remote(pidref))
                返回 -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        如果 (r < 0)
                返回 r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        如果 (r < 0)
                返回 r;

        *ret_path = TAKE_PTR(path);
        返回 0;
}

整数 cg_is_empty(const char *path) {
        _cleanup_free_ char *t = NULL;
        整数 r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        assert(path);

        /* The root cgroup is always populated */
        如果 (empty_or_root(path))
                返回 false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        如果 (r == -ENOENT)
                返回 true;
        如果 (r < 0)
                返回 r;

        返回 streq(t, "0");
}

整数 cg_split_spec(const char *spec, char **ret_controller, char **ret_path) {
        _cleanup_free_ char *controller = NULL;
        const char *path;
        整数 r;

        assert(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        如果 (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                goto finalize;
        }

        const char *e = strchr(spec, ':');
        如果 (!e) {
                /* Controller only. */
                如果 (ret_controller) {
                        controller = strdup(spec);
                        如果 (!controller)
                                返回 -ENOMEM;
                }

                path = NULL;
        } 否则 {
                /* Both controller and path. */
                如果 (ret_controller) {
                        controller = strndup(spec, e - spec);
                        如果 (!controller)
                                返回 -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        如果 (path) {
                /* Non-empty path must be absolute. */
                如果 (!path_is_absolute(path))
                        返回 -EINVAL;

                /* Path must not contain dot-dot. */
                如果 (!path_is_safe(path))
                        返回 -EINVAL;
        }

        如果 (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                如果 (r < 0)
                        返回 r;
        }

        如果 (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        返回 0;
}

整数 cg_get_root_path(char **ret_path) {
        char *p, *e;
        整数 r;

        assert(ret_path);

        r = cg_pid_get_path(1, &p);
        如果 (r < 0)
                返回 r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        如果 (e)
                *e = 0;

        *ret_path = p;
        返回 0;
}

整数 cg_shift_path(const char *cgroup, const char *root, const char **ret_shifted) {
        整数 r;

        assert(cgroup);
        assert(ret_shifted);

        _cleanup_free_ char *rt = NULL;
        如果 (!root) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_root_path(&rt);
                如果 (r < 0)
                        返回 r;

                root = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, root, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        返回 0;
}

整数 cg_pid_get_path_shifted(pid_t pid, const char *root, char **ret_cgroup) {
        _cleanup_free_ char *raw = NULL;
        const char *c;
        整数 r;

        assert(pid >= 0);
        assert(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        如果 (r < 0)
                返回 r;

        r = cg_shift_path(raw, root, &c);
        如果 (r < 0)
                返回 r;

        如果 (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                返回 0;
        }

        返回 strdup_to(ret_cgroup, c);
}

整数 cg_pidref_get_path_shifted(const PidRef *pidref, const char *root, char **ret_cgroup) {
        _cleanup_free_ char *raw = NULL;
        const char *c;
        整数 r;

        assert(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        如果 (r < 0)
                返回 r;

        r = cg_shift_path(raw, root, &c);
        如果 (r < 0)
                返回 r;

        如果 (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                返回 0;
        }

        返回 strdup_to(ret_cgroup, c);
}

整数 cg_path_decode_unit(const char *cgroup, char **ret_unit) {
        assert(cgroup);

        size_t n = strcspn(cgroup, "/");
        如果 (n < 3)
                返回 -ENXIO;

        char *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        如果 (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                返回 -ENXIO;

        如果 (ret_unit)
                返回 strdup_to(ret_unit, c);

        返回 0;
}

static bool valid_slice_name(const char *p, size_t n) {
        assert(p || n == 0);

        如果 (n < STRLEN("x.slice"))
                返回 false;

        char *c = strndupa_safe(p, n);
        如果 (!endswith(c, ".slice"))
                返回 false;

        返回 unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

static const char* skip_slices(const char *p) {
        assert(p);

        /* Skips over all slice assignments */

        为 (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                如果 (!valid_slice_name(p, n))
                        返回 p;

                p += n;
        }
}

整数 cg_path_get_unit_full(const char *path, char **ret_unit, char **ret_subgroup) {
        整数 r;

        assert(path);

        const char *e = skip_slices(path);

        _cleanup_free_ char *unit = NULL;
        r = cg_path_decode_unit(e, &unit);
        如果 (r < 0)
                返回 r;

        /* We skipped over the slices, don't accept any now */
        如果 (endswith(unit, ".slice"))
                返回 -ENXIO;

        如果 (ret_subgroup) {
                _cleanup_free_ char *subgroup = NULL;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                如果 (isempty(e))
                        subgroup = NULL;
                否则 {
                        subgroup = strdup(e);
                        如果 (!subgroup)
                                返回 -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        如果 (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        返回 0;
}

整数 cg_path_get_unit_path(const char *path, char **ret) {
        _cleanup_free_ char *path_copy = NULL;
        char *unit_name;

        assert(path);
        assert(ret);

        path_copy = strdup(path);
        如果 (!path_copy)
                返回 -ENOMEM;

        unit_name = (char*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        如果 (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                返回 -ENXIO;

        *ret = TAKE_PTR(path_copy);

        返回 0;
}

整数 cg_pid_get_unit_full(pid_t pid, char **ret_unit, char **ret_subgroup) {
        整数 r;

        _cleanup_free_ char *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

整数 cg_pidref_get_unit_full(const PidRef *pidref, char **ret_unit, char **ret_subgroup) {
        整数 r;

        如果 (!pidref_is_set(pidref))
                返回 -ESRCH;
        如果 (pidref_is_remote(pidref))
                返回 -EREMOTE;

        _cleanup_free_ char *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        如果 (r < 0)
                返回 r;

        r = pidref_verify(pidref);
        如果 (r < 0)
                返回 r;

        如果 (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        如果 (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        返回 0;
}

static const char* skip_session(const char *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        如果 (isempty(p))
                返回 NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        如果 (n < STRLEN("session-x.scope"))
                返回 NULL;

        const char *s = startswith(p, "session-");
        如果 (!s)
                返回 NULL;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        char *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        如果 (!e)
                返回 NULL;
        *e = '\0';

        如果 (!session_id_valid(f))
                返回 NULL;

        返回 skip_leading_slash(p + n);
}

static const char* skip_user_manager(const char *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        如果 (isempty(p))
                返回 NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        如果 (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                返回 NULL;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ char *unit_name = strndup(p, n);
        如果 (!unit_name)
                返回 NULL;

        _cleanup_free_ char *i = NULL;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        如果 (type != UNIT_NAME_INSTANCE)
                返回 NULL;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        如果 (!(startswith(unit_name, "user@") && parse_uid(i, NULL) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                返回 NULL;

        返回 skip_leading_slash(p + n);
}

static const char* skip_user_prefix(const char *path) {
        const char *e, *t;

        assert(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        如果 (t)
                返回 t;

        /* Alternatively skip the user session if it is in the path... */
        返回 skip_session(e);
}

整数 cg_path_get_user_unit_full(const char *path, char **ret_unit, char **ret_subgroup) {
        const char *t;

        assert(path);

        t = skip_user_prefix(path);
        如果 (!t)
                返回 -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        返回 cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

整数 cg_pid_get_user_unit_full(pid_t pid, char **ret_unit, char **ret_subgroup) {
        整数 r;

        _cleanup_free_ char *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

整数 cg_pidref_get_user_unit_full(const PidRef *pidref, char **ret_unit, char **ret_subgroup) {
        整数 r;

        如果 (!pidref_is_set(pidref))
                返回 -ESRCH;
        如果 (pidref_is_remote(pidref))
                返回 -EREMOTE;

        _cleanup_free_ char *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        如果 (r < 0)
                返回 r;

        r = pidref_verify(pidref);
        如果 (r < 0)
                返回 r;

        如果 (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        如果 (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        返回 0;
}

整数 cg_path_get_machine_name(const char *path, char **ret_machine) {
        _cleanup_free_ char *u = NULL;
        const char *sl;
        整数 r;

        r = cg_path_get_unit(path, &u);
        如果 (r < 0)
                返回 r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        返回 readlink_malloc(sl, ret_machine);
}

整数 cg_pid_get_machine_name(pid_t pid, char **ret_machine) {
        _cleanup_free_ char *cgroup = NULL;
        整数 r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_machine_name(cgroup, ret_machine);
}

整数 cg_path_get_session(const char *path, char **ret_session) {
        _cleanup_free_ char *unit = NULL;
        char *start, *end;
        整数 r;

        assert(path);

        r = cg_path_get_unit(path, &unit);
        如果 (r < 0)
                返回 r;

        start = startswith(unit, "session-");
        如果 (!start)
                返回 -ENXIO;
        end = endswith(start, ".scope");
        如果 (!end)
                返回 -ENXIO;

        *end = 0;
        如果 (!session_id_valid(start))
                返回 -ENXIO;

        如果 (!ret_session)
                返回 0;

        返回 strdup_to(ret_session, start);
}

整数 cg_pid_get_session(pid_t pid, char **ret_session) {
        _cleanup_free_ char *cgroup = NULL;
        整数 r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_session(cgroup, ret_session);
}

整数 cg_pidref_get_session(const PidRef *pidref, char **ret) {
        整数 r;

        如果 (!pidref_is_set(pidref))
                返回 -ESRCH;
        如果 (pidref_is_remote(pidref))
                返回 -EREMOTE;

        _cleanup_free_ char *session = NULL;
        r = cg_pid_get_session(pidref->pid, &session);
        如果 (r < 0)
                返回 r;

        r = pidref_verify(pidref);
        如果 (r < 0)
                返回 r;

        如果 (ret)
                *ret = TAKE_PTR(session);
        返回 0;
}

整数 cg_path_get_owner_uid(const char *path, uid_t *ret_uid) {
        _cleanup_free_ char *slice = NULL;
        char *start, *end;
        整数 r;

        assert(path);

        r = cg_path_get_slice(path, &slice);
        如果 (r < 0)
                返回 r;

        start = startswith(slice, "user-");
        如果 (!start)
                返回 -ENXIO;

        end = endswith(start, ".slice");
        如果 (!end)
                返回 -ENXIO;

        *end = 0;
        如果 (parse_uid(start, ret_uid) < 0)
                返回 -ENXIO;

        返回 0;
}

整数 cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ char *cgroup = NULL;
        整数 r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_owner_uid(cgroup, ret_uid);
}

整数 cg_pidref_get_owner_uid(const PidRef *pidref, uid_t *ret) {
        整数 r;

        如果 (!pidref_is_set(pidref))
                返回 -ESRCH;
        如果 (pidref_is_remote(pidref))
                返回 -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        如果 (r < 0)
                返回 r;

        r = pidref_verify(pidref);
        如果 (r < 0)
                返回 r;

        如果 (ret)
                *ret = uid;

        返回 0;
}

整数 cg_path_get_slice(const char *p, char **ret_slice) {
        const char *e = NULL;

        assert(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        为 (;;) {
                const char *s;
                整数 n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                如果 (n < 0)
                        返回 n;
                如果 (!valid_slice_name(s, n))
                        break;

                e = s;
        }

        如果 (e)
                返回 cg_path_decode_unit(e, ret_slice);

        如果 (ret_slice)
                返回 strdup_to(ret_slice, SPECIAL_ROOT_SLICE);

        返回 0;
}

整数 cg_pid_get_slice(pid_t pid, char **ret_slice) {
        _cleanup_free_ char *cgroup = NULL;
        整数 r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_slice(cgroup, ret_slice);
}

整数 cg_path_get_user_slice(const char *p, char **ret_slice) {
        const char *t;
        assert(p);

        t = skip_user_prefix(p);
        如果 (!t)
                返回 -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        返回 cg_path_get_slice(t, ret_slice);
}

整数 cg_pid_get_user_slice(pid_t pid, char **ret_slice) {
        _cleanup_free_ char *cgroup = NULL;
        整数 r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        如果 (r < 0)
                返回 r;

        返回 cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(const char *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        如果 (!filename_is_valid(p))
                返回 true;

        如果 (IN_SET(p[0], '_', '.'))
                返回 true;

        如果 (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                返回 true;

        如果 (startswith(p, "cgroup."))
                返回 true;

        为 (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const char *q;

                q = startswith(p, cgroup_controller_to_string(c));
                如果 (!q)
                        continue;

                如果 (q[0] == '.')
                        返回 true;
        }

        返回 false;
}

整数 cg_escape(const char *p, char **ret) {
        _cleanup_free_ char *n = NULL;

        assert(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        如果 (cg_needs_escape(p)) {
                n = strjoin("_", p);
                如果 (!n)
                        返回 -ENOMEM;

                如果 (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        返回 -EINVAL;
        } 否则 {
                n = strdup(p);
                如果 (!n)
                        返回 -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        返回 0;
}

char* cg_unescape(const char *p) {
        assert(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        如果 (p[0] == '_')
                返回 (char*) p+1;

        返回 (char*) p;
}

整数 cg_slice_to_path(const char *unit, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL, *e = NULL;
        const char *dash;
        整数 r;

        assert(unit);
        assert(ret);

        如果 (streq(unit, SPECIAL_ROOT_SLICE))
                返回 strdup_to(ret, "");

        如果 (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                返回 -EINVAL;

        如果 (!endswith(unit, ".slice"))
                返回 -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        如果 (r < 0)
                返回 r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        如果 (dash == p)
                返回 -EINVAL;

        当 (dash) {
                _cleanup_free_ char *escaped = NULL;
                char n[dash - p + sizeof(".slice")];

#如果 HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                如果 (IN_SET(dash[1], 0, '-'))
                        返回 -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                如果 (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        返回 -EINVAL;

                r = cg_escape(n, &escaped);
                如果 (r < 0)
                        返回 r;

                如果 (!strextend(&s, escaped, "/"))
                        返回 -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        如果 (r < 0)
                返回 r;

        如果 (!strextend(&s, e))
                返回 -ENOMEM;

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 cg_is_threaded(const char *path) {
        _cleanup_free_ char *fs = NULL, *contents = NULL;
        _cleanup_strv_free_ char **v = NULL;
        整数 r;

        r = cg_get_path(path, "cgroup.type", &fs);
        如果 (r < 0)
                返回 r;

        r = read_full_virtual_file(fs, &contents, NULL);
        如果 (r == -ENOENT)
                返回 false; /* Assume no. */
        如果 (r < 0)
                返回 r;

        v = strv_split(contents, NULL);
        如果 (!v)
                返回 -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        返回 strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

整数 cg_set_attribute(const char *path, const char *attribute, const char *value) {
        _cleanup_free_ char *p = NULL;
        整数 r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        如果 (r < 0)
                返回 r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        返回 write_string_file(p, value, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

整数 cg_get_attribute(const char *path, const char *attribute, char **ret) {
        _cleanup_free_ char *p = NULL;
        整数 r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        如果 (r < 0)
                返回 r;

        返回 read_one_line_file(p, ret);
}

整数 cg_get_attribute_as_uint64(const char *path, const char *attribute, uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        uint64_t v;
        整数 r;

        assert(ret);

        r = cg_get_attribute(path, attribute, &value);
        如果 (r == -ENOENT)
                返回 -ENODATA;
        如果 (r < 0)
                返回 r;

        如果 (streq(value, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                返回 0;
        }

        r = safe_atou64(value, &v);
        如果 (r < 0)
                返回 r;

        *ret = v;
        返回 0;
}

整数 cg_get_attribute_as_bool(const char *path, const char *attribute) {
        _cleanup_free_ char *value = NULL;
        整数 r;

        r = cg_get_attribute(path, attribute, &value);
        如果 (r == -ENOENT)
                返回 -ENODATA;
        如果 (r < 0)
                返回 r;

        返回 parse_boolean(value);
}

整数 cg_get_owner(const char *path, uid_t *ret_uid) {
        _cleanup_free_ char *f = NULL;
        struct stat stats;
        整数 r;

        assert(ret_uid);

        r = cg_get_path(path, /* suffix= */ NULL, &f);
        如果 (r < 0)
                返回 r;

        如果 (stat(f, &stats) < 0)
                返回 -errno;

        r = stat_verify_directory(&stats);
        如果 (r < 0)
                返回 r;

        *ret_uid = stats.st_uid;
        返回 0;
}

整数 cg_get_keyed_attribute(
                const char *path,
                const char *attribute,
                char * const *keys,
                char **values) {

        _cleanup_free_ char *filename = NULL, *contents = NULL;
        size_t n;
        整数 r;

        assert(path);
        assert(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        如果 (r < 0)
                返回 r;

        r = read_full_file(filename, &contents, /* ret_size= */ NULL);
        如果 (r < 0)
                返回 r;

        n = strv_length(keys);
        如果 (n == 0) /* No keys to retrieve? That's easy, we are done then */
                返回 0;
        assert(strv_is_uniq(keys));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        char **v = newa0(char*, n);
        size_t n_done = 0;

        为 (const char *p = contents; *p;) {
                const char *w;
                size_t i;

                为 (i = 0; i < n; i++) {
                        w = first_word(p, keys[i]);
                        如果 (w)
                                break;
                }

                如果 (w) {
                        如果 (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                goto fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        如果 (!v[i]) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        n_done++;
                        如果 (n_done >= n)
                                break;

                        p = w + l;
                } 否则
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        如果 (n_done < n) {
                r = -ENXIO;
                goto fail;
        }

        memcpy(values, v, sizeof(char*) * n);
        返回 0;

fail:
        free_many_charp(v, n);
        返回 r;
}

整数 cg_get_keyed_attribute_uint64(const char *path, const char *attribute, const char *key, uint64_t *ret) {
        _cleanup_free_ char *val = NULL;
        整数 r;

        assert(key);
        assert(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(key), &val);
        如果 (r < 0)
                返回 r;

        r = safe_atou64(val, ret);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, key, attribute);

        返回 0;
}

整数 cg_mask_to_string(CGroupMask mask, char **ret) {
        _cleanup_free_ char *s = NULL;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        assert(ret);

        如果 (mask == 0) {
                *ret = NULL;
                返回 0;
        }

        为 (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const char *k;
                size_t l;

                如果 (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        continue;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                如果 (!GREEDY_REALLOC(s, n + space + l + 1))
                        返回 -ENOMEM;

                如果 (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        assert(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        返回 0;
}

整数 cg_mask_from_string(const char *s, CGroupMask *ret) {
        CGroupMask m = 0;

        assert(ret);
        assert(s);

        为 (;;) {
                _cleanup_free_ char *n = NULL;
                CGroupController v;
                整数 r;

                r = extract_first_word(&s, &n, NULL, 0);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                v = cgroup_controller_from_string(n);
                如果 (v < 0)
                        continue;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        返回 0;
}

整数 cg_mask_supported_subtree(const char *root, CGroupMask *ret) {
        CGroupMask mask;
        整数 r;

        assert(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ char *controllers = NULL, *path = NULL;
        r = cg_get_path(root, "cgroup.controllers", &path);
        如果 (r < 0)
                返回 r;

        r = read_one_line_file(path, &controllers);
        如果 (r < 0)
                返回 r;

        r = cg_mask_from_string(controllers, &mask);
        如果 (r < 0)
                返回 r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        返回 0;
}

整数 cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ char *root = NULL;
        整数 r;

        r = cg_get_root_path(&root);
        如果 (r < 0)
                返回 r;

        返回 cg_mask_supported_subtree(root, ret);
}

整数 cg_is_delegated(const char *path) {
        整数 r;

        assert(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        如果 (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                返回 r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        返回 ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

整数 cg_is_delegated_fd(整数 fd) {
        整数 r;

        assert(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ NULL, "trusted.delegate", /* at_flags= */ 0);
        如果 (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                返回 r;

        r = getxattr_at_bool(fd, /* path= */ NULL, "user.delegate", /* at_flags= */ 0);
        返回 ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

整数 cg_has_coredump_receive(const char *path) {
        整数 r;

        assert(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        如果 (ERRNO_IS_NEG_XATTR_ABSENT(r))
                返回 false;

        返回 r;
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
