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

số_nguyên cg_is_available(void) {
        struct statfs fs;

        nếu (statfs("/sys/fs/cgroup/", &fs) < 0) {
                nếu (errno == ENOENT) /* sysfs not mounted? */
                        trả_về false;

                trả_về log_debug_errno(errno, "Failed to statfs /sys/fs/cgroup/: %m");
        }

        trả_về is_fs_type(&fs, CGROUP2_SUPER_MAGIC);
}

số_nguyên cg_path_open(const char *path) {
        _cleanup_free_ char *fs = NULL;
        số_nguyên r;

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        nếu (r < 0)
                trả_về r;

        trả_về RET_NERRNO(open(fs, O_DIRECTORY|O_CLOEXEC));
}

số_nguyên cg_cgroupid_open(số_nguyên cgroupfs_fd, uint64_t id) {
        _cleanup_close_ số_nguyên fsfd = -EBADF;

        nếu (cgroupfs_fd < 0) {
                fsfd = open("/sys/fs/cgroup", O_CLOEXEC|O_DIRECTORY);
                nếu (fsfd < 0)
                        trả_về -errno;

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

        trả_về RET_NERRNO(open_by_handle_at(cgroupfs_fd, &fh.file_handle, O_DIRECTORY|O_CLOEXEC));
}

số_nguyên cg_path_from_cgroupid(số_nguyên cgroupfs_fd, uint64_t id, char **ret) {
        _cleanup_close_ số_nguyên cgfd = -EBADF;
        số_nguyên r;

        cgfd = cg_cgroupid_open(cgroupfs_fd, id);
        nếu (cgfd < 0)
                trả_về cgfd;

        _cleanup_free_ char *path = NULL;
        r = fd_get_path(cgfd, &path);
        nếu (r < 0)
                trả_về r;

        nếu (!path_startswith(path, "/sys/fs/cgroup/"))
                trả_về -EXDEV; /* recognizable error */

        nếu (ret)
                *ret = TAKE_PTR(path);
        trả_về 0;
}

số_nguyên cg_enumerate_processes(const char *path, FILE **ret) {
        _cleanup_free_ char *fs = NULL;
        FILE *f;
        số_nguyên r;

        assert(ret);

        r = cg_get_path(path, "cgroup.procs", &fs);
        nếu (r < 0)
                trả_về r;

        f = fopen(fs, "re");
        nếu (!f)
                trả_về -errno;

        *ret = f;
        trả_về 0;
}

số_nguyên cg_read_pid(FILE *f, pid_t *ret, CGroupFlags flags) {
        unsigned long ul;

        /* Note that the cgroup.procs might contain duplicates! See cgroups.txt for details. */

        assert(f);
        assert(ret);

        /* NB: The kernel returns ENODEV if we tried to read from cgroup.procs of a cgroup that has been
         * removed already. Callers should handle that! */

        cho (;;) {
                errno = 0;
                nếu (fscanf(f, "%lu", &ul) != 1) {

                        nếu (feof(f)) {
                                *ret = 0;
                                trả_về 0;
                        }

                        trả_về errno_or_else(EIO);
                }

                nếu (ul > PID_T_MAX)
                        trả_về -EIO;

                /* In some circumstances (e.g. WSL), cgroups might contain unmappable PIDs from other
                 * contexts. These show up as zeros, and depending on the caller, can either be plain
                 * skipped over, or returned as-is. */
                nếu (ul == 0 && !FLAGS_SET(flags, CGROUP_DONT_SKIP_UNMAPPED))
                        continue;

                *ret = (pid_t) ul;
                trả_về 1;
        }
}

số_nguyên cg_read_pidref(FILE *f, PidRef *ret, CGroupFlags flags) {
        số_nguyên r;

        assert(f);
        assert(ret);

        cho (;;) {
                pid_t pid;

                r = cg_read_pid(f, &pid, flags);
                nếu (r < 0)
                        trả_về log_debug_errno(r, "Failed to read pid from cgroup item: %m");
                nếu (r == 0) {
                        *ret = PIDREF_NULL;
                        trả_về 0;
                }

                nếu (pid == 0)
                        trả_về -EREMOTE;

                r = pidref_set_pid(ret, pid);
                nếu (r >= 0)
                        trả_về 1;
                nếu (r != -ESRCH)
                        trả_về r;

                /* ESRCH → gone by now? just skip over it, read the next */
        }
}

bool cg_kill_supported(void) {
        static thread_local số_nguyên supported = -1;

        nếu (supported >= 0)
                trả_về supported;

        nếu (cg_is_available() <= 0)
                trả_về (supported = false);

        nếu (access("/sys/fs/cgroup/init.scope/cgroup.kill", F_OK) >= 0)
                trả_về (supported = true);
        nếu (errno != ENOENT)
                log_debug_errno(errno, "Failed to check whether cgroup.kill is available, assuming not: %m");
        trả_về (supported = false);
}

số_nguyên cg_enumerate_subgroups(const char *path, DIR **ret) {
        _cleanup_free_ char *fs = NULL;
        DIR *d;
        số_nguyên r;

        assert(ret);

        /* This is not recursive! */

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        nếu (r < 0)
                trả_về r;

        d = opendir(fs);
        nếu (!d)
                trả_về -errno;

        *ret = d;
        trả_về 0;
}

số_nguyên cg_read_subgroup(DIR *d, char **ret) {
        assert(d);
        assert(ret);

        FOREACH_DIRENT_ALL(de, d, trả_về -errno) {
                nếu (de->d_type != DT_DIR)
                        continue;

                nếu (dot_or_dot_dot(de->d_name))
                        continue;

                trả_về strdup_to_full(ret, de->d_name);
        }

        *ret = NULL;
        trả_về 0;
}

số_nguyên cg_kill(
                const char *path,
                số_nguyên sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        số_nguyên r, ret = 0;

        assert(path);
        assert(sig >= 0);

         /* Don't send SIGCONT twice. Also, SIGKILL always works even when process is suspended, hence
          * don't send SIGCONT on SIGKILL. */
        nếu (IN_SET(sig, SIGCONT, SIGKILL))
                flags &= ~CGROUP_SIGCONT;

        /* This goes through the tasks list and kills them all. This is repeated until no further processes
         * are added to the tasks list, to properly handle forking processes.
         *
         * When sending SIGKILL, prefer cg_kill_kernel_sigkill(), which is fully atomic. */

        nếu (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                nếu (!killed_pids)
                        trả_về -ENOMEM;
        }

        bool done;
        do {
                _cleanup_fclose_ FILE *f = NULL;
                số_nguyên ret_log_kill;

                done = true;

                r = cg_enumerate_processes(path, &f);
                nếu (r == -ENOENT)
                        break;
                nếu (r < 0)
                        trả_về RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup items: %m"));

                cho (;;) {
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                        r = cg_read_pidref(f, &pidref, flags);
                        nếu (r == -ENODEV) {
                                /* reading from cgroup.pids will result in ENODEV if the cgroup is
                                 * concurrently removed. Just leave in that case, because a removed cgroup
                                 * contains no processes anymore. */
                                done = true;
                                break;
                        }
                        nếu (r < 0)
                                trả_về RET_GATHER(ret, log_debug_errno(r, "Failed to read pidref from cgroup '%s': %m", path));
                        nếu (r == 0)
                                break;

                        nếu ((flags & CGROUP_IGNORE_SELF) && pidref_is_self(&pidref))
                                continue;

                        nếu (set_contains(killed_pids, PID_TO_PTR(pidref.pid)))
                                continue;

                        /* Ignore kernel threads to mimic the behavior of cgroup.kill. */
                        nếu (pidref_is_kernel_thread(&pidref) > 0) {
                                log_debug("Ignoring kernel thread with pid " PID_FMT " in cgroup '%s'", pidref.pid, path);
                                continue;
                        }

                        nếu (log_kill)
                                ret_log_kill = log_kill(&pidref, sig, userdata);

                        /* If we haven't killed this process yet, kill it */
                        r = pidref_kill(&pidref, sig);
                        nếu (r < 0 && r != -ESRCH)
                                RET_GATHER(ret, log_debug_errno(r, "Failed to kill process with pid " PID_FMT " from cgroup '%s': %m", pidref.pid, path));
                        nếu (r >= 0) {
                                nếu (flags & CGROUP_SIGCONT)
                                        (void) pidref_kill(&pidref, SIGCONT);

                                nếu (ret == 0) {
                                        nếu (log_kill)
                                                ret = ret_log_kill;
                                        không_thì
                                                ret = 1;
                                }
                        }

                        done = false;

                        r = set_put(killed_pids, PID_TO_PTR(pidref.pid));
                        nếu (r < 0)
                                trả_về RET_GATHER(ret, r);
                }

                /* To avoid racing against processes which fork quicker than we can kill them, we repeat this
                 * until no new pids need to be killed. */

        } trong_khi (!done);

        trả_về ret;
}

số_nguyên cg_kill_recursive(
                const char *path,
                số_nguyên sig,
                CGroupFlags flags,
                Set *killed_pids,
                cg_kill_log_func_t log_kill,
                void *userdata) {

        _cleanup_set_free_ Set *allocated_set = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        số_nguyên r, ret;

        assert(path);
        assert(sig >= 0);

        nếu (!killed_pids) {
                killed_pids = allocated_set = set_new(NULL);
                nếu (!killed_pids)
                        trả_về -ENOMEM;
        }

        ret = cg_kill(path, sig, flags, killed_pids, log_kill, userdata);

        r = cg_enumerate_subgroups(path, &d);
        nếu (r < 0) {
                nếu (r != -ENOENT)
                        RET_GATHER(ret, log_debug_errno(r, "Failed to enumerate cgroup '%s' subgroups: %m", path));

                trả_về ret;
        }

        cho (;;) {
                _cleanup_free_ char *fn = NULL, *p = NULL;

                r = cg_read_subgroup(d, &fn);
                nếu (r < 0) {
                        RET_GATHER(ret, log_debug_errno(r, "Failed to read subgroup from cgroup '%s': %m", path));
                        break;
                }
                nếu (r == 0)
                        break;

                p = path_join(empty_to_root(path), fn);
                nếu (!p)
                        trả_về -ENOMEM;

                r = cg_kill_recursive(p, sig, flags, killed_pids, log_kill, userdata);
                nếu (r < 0)
                        log_debug_errno(r, "Failed to recursively kill processes in cgroup '%s': %m", p);
                nếu (r != 0 && ret >= 0)
                        ret = r;
        }

        trả_về ret;
}

số_nguyên cg_kill_kernel_sigkill(const char *path, uint64_t *ret_n_pids_killed) {
        _cleanup_free_ char *killfile = NULL;
        uint64_t n_pids = UINT64_MAX;
        số_nguyên r;

        /* Kills the cgroup at `path` directly by writing to its cgroup.kill file.  This sends SIGKILL to all
         * processes in the cgroup and has the advantage of being completely atomic, unlike cg_kill_items(). */

        assert(path);

        nếu (!cg_kill_supported())
                trả_về -EOPNOTSUPP;

        r = cg_get_path(path, "cgroup.kill", &killfile);
        nếu (r < 0)
                trả_về r;

        nếu (ret_n_pids_killed) {
                /* This is not and cannot be atomic so there is a chance the counter might not be accurate
                 * if a process starts/stops between this read and the next write, but this is used for
                 * informational purposes so that's ok. */
                r = cg_get_attribute_as_uint64(path, "pids.current", &n_pids);
                nếu (r < 0)
                        log_debug_errno(r, "Failed to read pids.current from cgroup '%s', ignoring: %m", path);
        }

        r = write_string_file(killfile, "1", WRITE_STRING_FILE_DISABLE_BUFFER);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to write to cgroup.kill for cgroup '%s': %m", path);

        nếu (ret_n_pids_killed)
                *ret_n_pids_killed = n_pids;

        trả_về 0;
}

số_nguyên cg_get_path(const char *path, const char *suffix, char **ret) {
        char *t;

        assert(ret);

        nếu (isempty(path))
                path = TAKE_PTR(suffix);

        t = path_join("/sys/fs/cgroup", path, suffix);
        nếu (!t)
                trả_về -ENOMEM;

        *ret = path_simplify(t);
        trả_về 0;
}

số_nguyên cg_set_xattr(const char *path, const char *name, const void *value, size_t size, số_nguyên flags) {
        _cleanup_free_ char *fs = NULL;
        số_nguyên r;

        assert(path);
        assert(name);
        assert(value || size <= 0);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        nếu (r < 0)
                trả_về r;

        trả_về RET_NERRNO(setxattr(fs, name, value, size, flags));
}

số_nguyên cg_get_xattr(const char *path, const char *name, char **ret, size_t *ret_size) {
        _cleanup_free_ char *fs = NULL;
        số_nguyên r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        nếu (r < 0)
                trả_về r;

        trả_về lgetxattr_malloc(fs, name, ret, ret_size);
}

số_nguyên cg_get_xattr_bool(const char *path, const char *name) {
        _cleanup_free_ char *fs = NULL;
        số_nguyên r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        nếu (r < 0)
                trả_về r;

        trả_về getxattr_at_bool(AT_FDCWD, fs, name, /* at_flags= */ 0);
}

số_nguyên cg_remove_xattr(const char *path, const char *name) {
        _cleanup_free_ char *fs = NULL;
        số_nguyên r;

        assert(path);
        assert(name);

        r = cg_get_path(path, /* suffix= */ NULL, &fs);
        nếu (r < 0)
                trả_về r;

        trả_về RET_NERRNO(removexattr(fs, name));
}

số_nguyên cg_pid_get_path(pid_t pid, char **ret_path) {
        _cleanup_fclose_ FILE *f = NULL;
        const char *fs;
        số_nguyên r;

        assert(pid >= 0);
        assert(ret_path);

        fs = procfs_file_alloca(pid, "cgroup");
        r = fopen_unlocked(fs, "re", &f);
        nếu (r == -ENOENT)
                trả_về -ESRCH;
        nếu (r < 0)
                trả_về r;

        cho (;;) {
                _cleanup_free_ char *line = NULL;
                char *e;

                r = read_line(f, LONG_LINE_MAX, &line);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        trả_về -ENODATA;

                e = startswith(line, "0:");
                nếu (!e)
                        continue;

                e = strchr(e, ':');
                nếu (!e)
                        continue;

                _cleanup_free_ char *path = strdup(e + 1);
                nếu (!path)
                        trả_về -ENOMEM;

                /* Refuse cgroup paths from outside our cgroup namespace */
                nếu (startswith(path, "/../"))
                        trả_về -EUNATCH;

                /* Truncate suffix indicating the process is a zombie */
                e = endswith(path, " (deleted)");
                nếu (e)
                        *e = 0;

                *ret_path = TAKE_PTR(path);
                trả_về 0;
        }
}

số_nguyên cg_pidref_get_path(const PidRef *pidref, char **ret_path) {
        _cleanup_free_ char *path = NULL;
        số_nguyên r;

        assert(ret_path);

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;
        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        // XXX: Ideally we'd use pidfd_get_cgroupid() + cg_path_from_cgroupid() here, to extract this
        // bit of information from pidfd directly. However, the latter requires privilege and it's
        // not entirely clear how to handle cgroups from outer namespace.

        r = cg_pid_get_path(pidref->pid, &path);
        nếu (r < 0)
                trả_về r;

        /* Before we return the path, make sure the procfs entry for this pid still matches the pidref */
        r = pidref_verify(pidref);
        nếu (r < 0)
                trả_về r;

        *ret_path = TAKE_PTR(path);
        trả_về 0;
}

số_nguyên cg_is_empty(const char *path) {
        _cleanup_free_ char *t = NULL;
        số_nguyên r;

        /* Check if the cgroup hierarchy under 'path' is empty. On cgroup v2 it's exposed via the "populated"
         * attribute of "cgroup.events". */

        assert(path);

        /* The root cgroup is always populated */
        nếu (empty_or_root(path))
                trả_về false;

        r = cg_get_keyed_attribute(path, "cgroup.events", STRV_MAKE("populated"), &t);
        nếu (r == -ENOENT)
                trả_về true;
        nếu (r < 0)
                trả_về r;

        trả_về streq(t, "0");
}

số_nguyên cg_split_spec(const char *spec, char **ret_controller, char **ret_path) {
        _cleanup_free_ char *controller = NULL;
        const char *path;
        số_nguyên r;

        assert(spec);

        /* This extracts the path part from the deprecated controller:path spec. The path must be absolute or
         * an empty string. No validation is done for the controller part. */

        nếu (isempty(spec) || path_is_absolute(spec)) {
                /* Assume this does not contain controller. */
                path = spec;
                goto finalize;
        }

        const char *e = strchr(spec, ':');
        nếu (!e) {
                /* Controller only. */
                nếu (ret_controller) {
                        controller = strdup(spec);
                        nếu (!controller)
                                trả_về -ENOMEM;
                }

                path = NULL;
        } không_thì {
                /* Both controller and path. */
                nếu (ret_controller) {
                        controller = strndup(spec, e - spec);
                        nếu (!controller)
                                trả_về -ENOMEM;
                }

                path = e + 1;
        }

finalize:
        path = empty_to_null(path);

        nếu (path) {
                /* Non-empty path must be absolute. */
                nếu (!path_is_absolute(path))
                        trả_về -EINVAL;

                /* Path must not contain dot-dot. */
                nếu (!path_is_safe(path))
                        trả_về -EINVAL;
        }

        nếu (ret_path) {
                r = path_simplify_alloc(path, ret_path);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_controller)
                *ret_controller = TAKE_PTR(controller);

        trả_về 0;
}

số_nguyên cg_get_root_path(char **ret_path) {
        char *p, *e;
        số_nguyên r;

        assert(ret_path);

        r = cg_pid_get_path(1, &p);
        nếu (r < 0)
                trả_về r;

        e = endswith(p, "/" SPECIAL_INIT_SCOPE);
        nếu (e)
                *e = 0;

        *ret_path = p;
        trả_về 0;
}

số_nguyên cg_shift_path(const char *cgroup, const char *root, const char **ret_shifted) {
        số_nguyên r;

        assert(cgroup);
        assert(ret_shifted);

        _cleanup_free_ char *rt = NULL;
        nếu (!root) {
                /* If the root was specified let's use that, otherwise
                 * let's determine it from PID 1 */

                r = cg_get_root_path(&rt);
                nếu (r < 0)
                        trả_về r;

                root = rt;
        }

        *ret_shifted = path_startswith_full(cgroup, root, PATH_STARTSWITH_RETURN_LEADING_SLASH|PATH_STARTSWITH_REFUSE_DOT_DOT) ?: cgroup;
        trả_về 0;
}

số_nguyên cg_pid_get_path_shifted(pid_t pid, const char *root, char **ret_cgroup) {
        _cleanup_free_ char *raw = NULL;
        const char *c;
        số_nguyên r;

        assert(pid >= 0);
        assert(ret_cgroup);

        r = cg_pid_get_path(pid, &raw);
        nếu (r < 0)
                trả_về r;

        r = cg_shift_path(raw, root, &c);
        nếu (r < 0)
                trả_về r;

        nếu (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                trả_về 0;
        }

        trả_về strdup_to(ret_cgroup, c);
}

số_nguyên cg_pidref_get_path_shifted(const PidRef *pidref, const char *root, char **ret_cgroup) {
        _cleanup_free_ char *raw = NULL;
        const char *c;
        số_nguyên r;

        assert(ret_cgroup);

        r = cg_pidref_get_path(pidref, &raw);
        nếu (r < 0)
                trả_về r;

        r = cg_shift_path(raw, root, &c);
        nếu (r < 0)
                trả_về r;

        nếu (c == raw) {
                *ret_cgroup = TAKE_PTR(raw);
                trả_về 0;
        }

        trả_về strdup_to(ret_cgroup, c);
}

số_nguyên cg_path_decode_unit(const char *cgroup, char **ret_unit) {
        assert(cgroup);

        size_t n = strcspn(cgroup, "/");
        nếu (n < 3)
                trả_về -ENXIO;

        char *c = strndupa_safe(cgroup, n);
        c = cg_unescape(c);

        nếu (!unit_name_is_valid(c, UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                trả_về -ENXIO;

        nếu (ret_unit)
                trả_về strdup_to(ret_unit, c);

        trả_về 0;
}

static bool valid_slice_name(const char *p, size_t n) {
        assert(p || n == 0);

        nếu (n < STRLEN("x.slice"))
                trả_về false;

        char *c = strndupa_safe(p, n);
        nếu (!endswith(c, ".slice"))
                trả_về false;

        trả_về unit_name_is_valid(cg_unescape(c), UNIT_NAME_PLAIN);
}

static const char* skip_slices(const char *p) {
        assert(p);

        /* Skips over all slice assignments */

        cho (;;) {
                size_t n;

                p += strspn(p, "/");

                n = strcspn(p, "/");
                nếu (!valid_slice_name(p, n))
                        trả_về p;

                p += n;
        }
}

số_nguyên cg_path_get_unit_full(const char *path, char **ret_unit, char **ret_subgroup) {
        số_nguyên r;

        assert(path);

        const char *e = skip_slices(path);

        _cleanup_free_ char *unit = NULL;
        r = cg_path_decode_unit(e, &unit);
        nếu (r < 0)
                trả_về r;

        /* We skipped over the slices, don't accept any now */
        nếu (endswith(unit, ".slice"))
                trả_về -ENXIO;

        nếu (ret_subgroup) {
                _cleanup_free_ char *subgroup = NULL;
                e += strcspn(e, "/");
                e += strspn(e, "/");

                nếu (isempty(e))
                        subgroup = NULL;
                không_thì {
                        subgroup = strdup(e);
                        nếu (!subgroup)
                                trả_về -ENOMEM;
                }

                path_simplify(subgroup);

                *ret_subgroup = TAKE_PTR(subgroup);
        }

        nếu (ret_unit)
                *ret_unit = TAKE_PTR(unit);

        trả_về 0;
}

số_nguyên cg_path_get_unit_path(const char *path, char **ret) {
        _cleanup_free_ char *path_copy = NULL;
        char *unit_name;

        assert(path);
        assert(ret);

        path_copy = strdup(path);
        nếu (!path_copy)
                trả_về -ENOMEM;

        unit_name = (char*) skip_slices(path_copy);
        unit_name[strcspn(unit_name, "/")] = 0;

        nếu (!unit_name_is_valid(cg_unescape(unit_name), UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE))
                trả_về -ENXIO;

        *ret = TAKE_PTR(path_copy);

        trả_về 0;
}

số_nguyên cg_pid_get_unit_full(pid_t pid, char **ret_unit, char **ret_subgroup) {
        số_nguyên r;

        _cleanup_free_ char *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_unit_full(cgroup, ret_unit, ret_subgroup);
}

số_nguyên cg_pidref_get_unit_full(const PidRef *pidref, char **ret_unit, char **ret_subgroup) {
        số_nguyên r;

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;
        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        _cleanup_free_ char *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_unit_full(pidref->pid, &unit, &subgroup);
        nếu (r < 0)
                trả_về r;

        r = pidref_verify(pidref);
        nếu (r < 0)
                trả_về r;

        nếu (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        nếu (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        trả_về 0;
}

static const char* skip_session(const char *p) {
        size_t n;

        /* Skip session-*.scope, but require it to be there. */

        nếu (isempty(p))
                trả_về NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        nếu (n < STRLEN("session-x.scope"))
                trả_về NULL;

        const char *s = startswith(p, "session-");
        nếu (!s)
                trả_về NULL;

        /* Note that session scopes never need unescaping, since they cannot conflict with the kernel's
         * own names, hence we don't need to call cg_unescape() here. */
        char *f = strndupa_safe(s, p + n - s),
             *e = endswith(f, ".scope");
        nếu (!e)
                trả_về NULL;
        *e = '\0';

        nếu (!session_id_valid(f))
                trả_về NULL;

        trả_về skip_leading_slash(p + n);
}

static const char* skip_user_manager(const char *p) {
        size_t n;

        /* Skip user@*.service or capsule@*.service, but require either of them to be there. */

        nếu (isempty(p))
                trả_về NULL;

        p += strspn(p, "/");

        n = strcspn(p, "/");
        nếu (n < CONST_MIN(STRLEN("user@x.service"), STRLEN("capsule@x.service")))
                trả_về NULL;

        /* Any possible errors from functions called below are converted to NULL return, so our callers won't
         * resolve user/capsule name. */
        _cleanup_free_ char *unit_name = strndup(p, n);
        nếu (!unit_name)
                trả_về NULL;

        _cleanup_free_ char *i = NULL;
        UnitNameFlags type = unit_name_to_instance(unit_name, &i);

        nếu (type != UNIT_NAME_INSTANCE)
                trả_về NULL;

        /* Note that user manager services never need unescaping, since they cannot conflict with the
         * kernel's own names, hence we don't need to call cg_unescape() here.  Prudently check validity of
         * instance names, they should be always valid as we validate them upon unit start. */
        nếu (!(startswith(unit_name, "user@") && parse_uid(i, NULL) >= 0) &&
            !(startswith(unit_name, "capsule@") && capsule_name_is_valid(i) > 0))
                trả_về NULL;

        trả_về skip_leading_slash(p + n);
}

static const char* skip_user_prefix(const char *path) {
        const char *e, *t;

        assert(path);

        /* Skip slices, if there are any */
        e = skip_slices(path);

        /* Skip the user manager, if it's in the path now... */
        t = skip_user_manager(e);
        nếu (t)
                trả_về t;

        /* Alternatively skip the user session if it is in the path... */
        trả_về skip_session(e);
}

số_nguyên cg_path_get_user_unit_full(const char *path, char **ret_unit, char **ret_subgroup) {
        const char *t;

        assert(path);

        t = skip_user_prefix(path);
        nếu (!t)
                trả_về -ENXIO;

        /* And from here on it looks pretty much the same as for a system unit, hence let's use the same
         * parser. */
        trả_về cg_path_get_unit_full(t, ret_unit, ret_subgroup);
}

số_nguyên cg_pid_get_user_unit_full(pid_t pid, char **ret_unit, char **ret_subgroup) {
        số_nguyên r;

        _cleanup_free_ char *cgroup = NULL;
        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_user_unit_full(cgroup, ret_unit, ret_subgroup);
}

số_nguyên cg_pidref_get_user_unit_full(const PidRef *pidref, char **ret_unit, char **ret_subgroup) {
        số_nguyên r;

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;
        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        _cleanup_free_ char *unit = NULL, *subgroup = NULL;
        r = cg_pid_get_user_unit_full(pidref->pid, &unit, &subgroup);
        nếu (r < 0)
                trả_về r;

        r = pidref_verify(pidref);
        nếu (r < 0)
                trả_về r;

        nếu (ret_unit)
                *ret_unit = TAKE_PTR(unit);
        nếu (ret_subgroup)
                *ret_subgroup = TAKE_PTR(subgroup);
        trả_về 0;
}

số_nguyên cg_path_get_machine_name(const char *path, char **ret_machine) {
        _cleanup_free_ char *u = NULL;
        const char *sl;
        số_nguyên r;

        r = cg_path_get_unit(path, &u);
        nếu (r < 0)
                trả_về r;

        sl = strjoina("/run/systemd/machines/unit:", u);
        trả_về readlink_malloc(sl, ret_machine);
}

số_nguyên cg_pid_get_machine_name(pid_t pid, char **ret_machine) {
        _cleanup_free_ char *cgroup = NULL;
        số_nguyên r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_machine_name(cgroup, ret_machine);
}

số_nguyên cg_path_get_session(const char *path, char **ret_session) {
        _cleanup_free_ char *unit = NULL;
        char *start, *end;
        số_nguyên r;

        assert(path);

        r = cg_path_get_unit(path, &unit);
        nếu (r < 0)
                trả_về r;

        start = startswith(unit, "session-");
        nếu (!start)
                trả_về -ENXIO;
        end = endswith(start, ".scope");
        nếu (!end)
                trả_về -ENXIO;

        *end = 0;
        nếu (!session_id_valid(start))
                trả_về -ENXIO;

        nếu (!ret_session)
                trả_về 0;

        trả_về strdup_to(ret_session, start);
}

số_nguyên cg_pid_get_session(pid_t pid, char **ret_session) {
        _cleanup_free_ char *cgroup = NULL;
        số_nguyên r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_session(cgroup, ret_session);
}

số_nguyên cg_pidref_get_session(const PidRef *pidref, char **ret) {
        số_nguyên r;

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;
        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        _cleanup_free_ char *session = NULL;
        r = cg_pid_get_session(pidref->pid, &session);
        nếu (r < 0)
                trả_về r;

        r = pidref_verify(pidref);
        nếu (r < 0)
                trả_về r;

        nếu (ret)
                *ret = TAKE_PTR(session);
        trả_về 0;
}

số_nguyên cg_path_get_owner_uid(const char *path, uid_t *ret_uid) {
        _cleanup_free_ char *slice = NULL;
        char *start, *end;
        số_nguyên r;

        assert(path);

        r = cg_path_get_slice(path, &slice);
        nếu (r < 0)
                trả_về r;

        start = startswith(slice, "user-");
        nếu (!start)
                trả_về -ENXIO;

        end = endswith(start, ".slice");
        nếu (!end)
                trả_về -ENXIO;

        *end = 0;
        nếu (parse_uid(start, ret_uid) < 0)
                trả_về -ENXIO;

        trả_về 0;
}

số_nguyên cg_pid_get_owner_uid(pid_t pid, uid_t *ret_uid) {
        _cleanup_free_ char *cgroup = NULL;
        số_nguyên r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_owner_uid(cgroup, ret_uid);
}

số_nguyên cg_pidref_get_owner_uid(const PidRef *pidref, uid_t *ret) {
        số_nguyên r;

        nếu (!pidref_is_set(pidref))
                trả_về -ESRCH;
        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        uid_t uid;
        r = cg_pid_get_owner_uid(pidref->pid, &uid);
        nếu (r < 0)
                trả_về r;

        r = pidref_verify(pidref);
        nếu (r < 0)
                trả_về r;

        nếu (ret)
                *ret = uid;

        trả_về 0;
}

số_nguyên cg_path_get_slice(const char *p, char **ret_slice) {
        const char *e = NULL;

        assert(p);

        /* Finds the right-most slice unit from the beginning, but stops before we come to
         * the first non-slice unit. */

        cho (;;) {
                const char *s;
                số_nguyên n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, &s);
                nếu (n < 0)
                        trả_về n;
                nếu (!valid_slice_name(s, n))
                        break;

                e = s;
        }

        nếu (e)
                trả_về cg_path_decode_unit(e, ret_slice);

        nếu (ret_slice)
                trả_về strdup_to(ret_slice, SPECIAL_ROOT_SLICE);

        trả_về 0;
}

số_nguyên cg_pid_get_slice(pid_t pid, char **ret_slice) {
        _cleanup_free_ char *cgroup = NULL;
        số_nguyên r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_slice(cgroup, ret_slice);
}

số_nguyên cg_path_get_user_slice(const char *p, char **ret_slice) {
        const char *t;
        assert(p);

        t = skip_user_prefix(p);
        nếu (!t)
                trả_về -ENXIO;

        /* And now it looks pretty much the same as for a system slice, so let's just use the same parser
         * from here on. */
        trả_về cg_path_get_slice(t, ret_slice);
}

số_nguyên cg_pid_get_user_slice(pid_t pid, char **ret_slice) {
        _cleanup_free_ char *cgroup = NULL;
        số_nguyên r;

        r = cg_pid_get_path_shifted(pid, NULL, &cgroup);
        nếu (r < 0)
                trả_về r;

        trả_về cg_path_get_user_slice(cgroup, ret_slice);
}

bool cg_needs_escape(const char *p) {

        /* Checks if the specified path is a valid cgroup name by our rules, or if it must be escaped. Note
         * that we consider escaped cgroup names invalid here, as they need to be escaped a second time if
         * they shall be used. Also note that various names cannot be made valid by escaping even if we
         * return true here (because too long, or contain the forbidden character "/"). */

        nếu (!filename_is_valid(p))
                trả_về true;

        nếu (IN_SET(p[0], '_', '.'))
                trả_về true;

        nếu (STR_IN_SET(p, "notify_on_release", "release_agent", "tasks"))
                trả_về true;

        nếu (startswith(p, "cgroup."))
                trả_về true;

        cho (CGroupController c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const char *q;

                q = startswith(p, cgroup_controller_to_string(c));
                nếu (!q)
                        continue;

                nếu (q[0] == '.')
                        trả_về true;
        }

        trả_về false;
}

số_nguyên cg_escape(const char *p, char **ret) {
        _cleanup_free_ char *n = NULL;

        assert(ret);

        /* This implements very minimal escaping for names to be used as file names in the cgroup tree: any
         * name which might conflict with a kernel name or is prefixed with '_' is prefixed with a '_'. That
         * way, when reading cgroup names it is sufficient to remove a single prefixing underscore if there
         * is one. */

        /* The return value of this function (unlike cg_unescape()) needs free()! */

        nếu (cg_needs_escape(p)) {
                n = strjoin("_", p);
                nếu (!n)
                        trả_về -ENOMEM;

                nếu (!filename_is_valid(n)) /* became invalid due to the prefixing? Or contained things like a slash that cannot be fixed by prefixing? */
                        trả_về -EINVAL;
        } không_thì {
                n = strdup(p);
                nếu (!n)
                        trả_về -ENOMEM;
        }

        *ret = TAKE_PTR(n);
        trả_về 0;
}

char* cg_unescape(const char *p) {
        assert(p);

        /* The return value of this function (unlike cg_escape())
         * doesn't need free()! */

        nếu (p[0] == '_')
                trả_về (char*) p+1;

        trả_về (char*) p;
}

số_nguyên cg_slice_to_path(const char *unit, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL, *e = NULL;
        const char *dash;
        số_nguyên r;

        assert(unit);
        assert(ret);

        nếu (streq(unit, SPECIAL_ROOT_SLICE))
                trả_về strdup_to(ret, "");

        nếu (!unit_name_is_valid(unit, UNIT_NAME_PLAIN))
                trả_về -EINVAL;

        nếu (!endswith(unit, ".slice"))
                trả_về -EINVAL;

        r = unit_name_to_prefix(unit, &p);
        nếu (r < 0)
                trả_về r;

        dash = strchr(p, '-');

        /* Don't allow initial dashes */
        nếu (dash == p)
                trả_về -EINVAL;

        trong_khi (dash) {
                _cleanup_free_ char *escaped = NULL;
                char n[dash - p + sizeof(".slice")];

#nếu HAS_FEATURE_MEMORY_SANITIZER
                /* msan doesn't instrument stpncpy, so it thinks
                 * n is later used uninitialized:
                 * https://github.com/google/sanitizers/issues/926
                 */
                zero(n);
#endif

                /* Don't allow trailing or double dashes */
                nếu (IN_SET(dash[1], 0, '-'))
                        trả_về -EINVAL;

                strcpy(stpncpy(n, p, dash - p), ".slice");
                nếu (!unit_name_is_valid(n, UNIT_NAME_PLAIN))
                        trả_về -EINVAL;

                r = cg_escape(n, &escaped);
                nếu (r < 0)
                        trả_về r;

                nếu (!strextend(&s, escaped, "/"))
                        trả_về -ENOMEM;

                dash = strchr(dash+1, '-');
        }

        r = cg_escape(unit, &e);
        nếu (r < 0)
                trả_về r;

        nếu (!strextend(&s, e))
                trả_về -ENOMEM;

        *ret = TAKE_PTR(s);
        trả_về 0;
}

số_nguyên cg_is_threaded(const char *path) {
        _cleanup_free_ char *fs = NULL, *contents = NULL;
        _cleanup_strv_free_ char **v = NULL;
        số_nguyên r;

        r = cg_get_path(path, "cgroup.type", &fs);
        nếu (r < 0)
                trả_về r;

        r = read_full_virtual_file(fs, &contents, NULL);
        nếu (r == -ENOENT)
                trả_về false; /* Assume no. */
        nếu (r < 0)
                trả_về r;

        v = strv_split(contents, NULL);
        nếu (!v)
                trả_về -ENOMEM;

        /* If the cgroup is in the threaded mode, it contains "threaded".
         * If one of the parents or siblings is in the threaded mode, it may contain "invalid". */
        trả_về strv_contains(v, "threaded") || strv_contains(v, "invalid");
}

số_nguyên cg_set_attribute(const char *path, const char *attribute, const char *value) {
        _cleanup_free_ char *p = NULL;
        số_nguyên r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        nếu (r < 0)
                trả_về r;

        /* https://lore.kernel.org/all/20250419183545.1982187-1-shakeel.butt@linux.dev/ adds O_NONBLOCK
         * semantics to memory.max and memory.high to skip synchronous memory reclaim when O_NONBLOCK is
         * enabled. Let's always open cgroupv2 attribute files in nonblocking mode to immediately take
         * advantage of this and any other asynchronous resource reclaim that's added to the cgroupv2 API in
         * the future. */
        trả_về write_string_file(p, value, WRITE_STRING_FILE_DISABLE_BUFFER|WRITE_STRING_FILE_OPEN_NONBLOCKING);
}

số_nguyên cg_get_attribute(const char *path, const char *attribute, char **ret) {
        _cleanup_free_ char *p = NULL;
        số_nguyên r;

        assert(attribute);

        r = cg_get_path(path, attribute, &p);
        nếu (r < 0)
                trả_về r;

        trả_về read_one_line_file(p, ret);
}

số_nguyên cg_get_attribute_as_uint64(const char *path, const char *attribute, uint64_t *ret) {
        _cleanup_free_ char *value = NULL;
        uint64_t v;
        số_nguyên r;

        assert(ret);

        r = cg_get_attribute(path, attribute, &value);
        nếu (r == -ENOENT)
                trả_về -ENODATA;
        nếu (r < 0)
                trả_về r;

        nếu (streq(value, "max")) {
                *ret = CGROUP_LIMIT_MAX;
                trả_về 0;
        }

        r = safe_atou64(value, &v);
        nếu (r < 0)
                trả_về r;

        *ret = v;
        trả_về 0;
}

số_nguyên cg_get_attribute_as_bool(const char *path, const char *attribute) {
        _cleanup_free_ char *value = NULL;
        số_nguyên r;

        r = cg_get_attribute(path, attribute, &value);
        nếu (r == -ENOENT)
                trả_về -ENODATA;
        nếu (r < 0)
                trả_về r;

        trả_về parse_boolean(value);
}

số_nguyên cg_get_owner(const char *path, uid_t *ret_uid) {
        _cleanup_free_ char *f = NULL;
        struct stat stats;
        số_nguyên r;

        assert(ret_uid);

        r = cg_get_path(path, /* suffix= */ NULL, &f);
        nếu (r < 0)
                trả_về r;

        nếu (stat(f, &stats) < 0)
                trả_về -errno;

        r = stat_verify_directory(&stats);
        nếu (r < 0)
                trả_về r;

        *ret_uid = stats.st_uid;
        trả_về 0;
}

số_nguyên cg_get_keyed_attribute(
                const char *path,
                const char *attribute,
                char * const *keys,
                char **values) {

        _cleanup_free_ char *filename = NULL, *contents = NULL;
        size_t n;
        số_nguyên r;

        assert(path);
        assert(attribute);

        /* Reads one or more fields of a cgroup v2 keyed attribute file. The 'keys' parameter should be an strv with
         * all keys to retrieve. The 'values' parameter should be passed as string size with the same number of
         * entries as 'keys'. On success each entry will be set to the value of the matching key.
         *
         * If the attribute file doesn't exist at all returns ENOENT, if any key is not found returns ENXIO. */

        r = cg_get_path(path, attribute, &filename);
        nếu (r < 0)
                trả_về r;

        r = read_full_file(filename, &contents, /* ret_size= */ NULL);
        nếu (r < 0)
                trả_về r;

        n = strv_length(keys);
        nếu (n == 0) /* No keys to retrieve? That's easy, we are done then */
                trả_về 0;
        assert(strv_is_uniq(keys));

        /* Let's build this up in a temporary array for now in order not to clobber the return parameter on failure */
        char **v = newa0(char*, n);
        size_t n_done = 0;

        cho (const char *p = contents; *p;) {
                const char *w;
                size_t i;

                cho (i = 0; i < n; i++) {
                        w = first_word(p, keys[i]);
                        nếu (w)
                                break;
                }

                nếu (w) {
                        nếu (v[i]) { /* duplicate entry? */
                                r = -EBADMSG;
                                goto fail;
                        }

                        size_t l = strcspn(w, NEWLINE);

                        v[i] = strndup(w, l);
                        nếu (!v[i]) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        n_done++;
                        nếu (n_done >= n)
                                break;

                        p = w + l;
                } không_thì
                        p += strcspn(p, NEWLINE);

                p += strspn(p, NEWLINE);
        }

        nếu (n_done < n) {
                r = -ENXIO;
                goto fail;
        }

        memcpy(values, v, sizeof(char*) * n);
        trả_về 0;

fail:
        free_many_charp(v, n);
        trả_về r;
}

số_nguyên cg_get_keyed_attribute_uint64(const char *path, const char *attribute, const char *key, uint64_t *ret) {
        _cleanup_free_ char *val = NULL;
        số_nguyên r;

        assert(key);
        assert(ret);

        r = cg_get_keyed_attribute(path, attribute, STRV_MAKE(key), &val);
        nếu (r < 0)
                trả_về r;

        r = safe_atou64(val, ret);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to parse value '%s' of key '%s' in cgroup attribute '%s': %m", val, key, attribute);

        trả_về 0;
}

số_nguyên cg_mask_to_string(CGroupMask mask, char **ret) {
        _cleanup_free_ char *s = NULL;
        bool space = false;
        CGroupController c;
        size_t n = 0;

        assert(ret);

        nếu (mask == 0) {
                *ret = NULL;
                trả_về 0;
        }

        cho (c = 0; c < _CGROUP_CONTROLLER_MAX; c++) {
                const char *k;
                size_t l;

                nếu (!FLAGS_SET(mask, CGROUP_CONTROLLER_TO_MASK(c)))
                        continue;

                k = cgroup_controller_to_string(c);
                l = strlen(k);

                nếu (!GREEDY_REALLOC(s, n + space + l + 1))
                        trả_về -ENOMEM;

                nếu (space)
                        s[n] = ' ';
                memcpy(s + n + space, k, l);
                n += space + l;

                space = true;
        }

        assert(s);

        s[n] = 0;
        *ret = TAKE_PTR(s);

        trả_về 0;
}

số_nguyên cg_mask_from_string(const char *s, CGroupMask *ret) {
        CGroupMask m = 0;

        assert(ret);
        assert(s);

        cho (;;) {
                _cleanup_free_ char *n = NULL;
                CGroupController v;
                số_nguyên r;

                r = extract_first_word(&s, &n, NULL, 0);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;

                v = cgroup_controller_from_string(n);
                nếu (v < 0)
                        continue;

                m |= CGROUP_CONTROLLER_TO_MASK(v);
        }

        *ret = m;
        trả_về 0;
}

số_nguyên cg_mask_supported_subtree(const char *root, CGroupMask *ret) {
        CGroupMask mask;
        số_nguyên r;

        assert(ret);

        /* Determines the mask of supported cgroup controllers. Only includes controllers we can make sense of and that
         * are actually accessible. Only covers real controllers, i.e. not the CGROUP_CONTROLLER_BPF_xyz
         * pseudo-controllers. */

        /* We can read the supported and accessible controllers from the top-level cgroup attribute */
        _cleanup_free_ char *controllers = NULL, *path = NULL;
        r = cg_get_path(root, "cgroup.controllers", &path);
        nếu (r < 0)
                trả_về r;

        r = read_one_line_file(path, &controllers);
        nếu (r < 0)
                trả_về r;

        r = cg_mask_from_string(controllers, &mask);
        nếu (r < 0)
                trả_về r;

        /* Mask controllers that are not supported in cgroup v2. */
        mask &= CGROUP_MASK_V2;

        *ret = mask;
        trả_về 0;
}

số_nguyên cg_mask_supported(CGroupMask *ret) {
        _cleanup_free_ char *root = NULL;
        số_nguyên r;

        r = cg_get_root_path(&root);
        nếu (r < 0)
                trả_về r;

        trả_về cg_mask_supported_subtree(root, ret);
}

số_nguyên cg_is_delegated(const char *path) {
        số_nguyên r;

        assert(path);

        r = cg_get_xattr_bool(path, "trusted.delegate");
        nếu (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                trả_về r;

        /* If the trusted xattr isn't set (preferred), then check the untrusted one. Under the assumption
         * that whoever is trusted enough to own the cgroup, is also trusted enough to decide if it is
         * delegated or not this should be safe. */
        r = cg_get_xattr_bool(path, "user.delegate");
        trả_về ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

số_nguyên cg_is_delegated_fd(số_nguyên fd) {
        số_nguyên r;

        assert(fd >= 0);

        r = getxattr_at_bool(fd, /* path= */ NULL, "trusted.delegate", /* at_flags= */ 0);
        nếu (!ERRNO_IS_NEG_XATTR_ABSENT(r))
                trả_về r;

        r = getxattr_at_bool(fd, /* path= */ NULL, "user.delegate", /* at_flags= */ 0);
        trả_về ERRNO_IS_NEG_XATTR_ABSENT(r) ? false : r;
}

số_nguyên cg_has_coredump_receive(const char *path) {
        số_nguyên r;

        assert(path);

        r = cg_get_xattr_bool(path, "user.coredump_receive");
        nếu (ERRNO_IS_NEG_XATTR_ABSENT(r))
                trả_về false;

        trả_về r;
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
