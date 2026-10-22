/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/magic.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "glyph-util.h"
#شامل "log.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "user-util.h"

/* Flags that prevent us from taking any of the early shortcuts: either they change the path resolution
 * semantics (e.g. CHASE_NONEXISTENT, CHASE_PARENT, CHASE_STEP) or ask for per-component validation that a
 * single open() cannot provide (e.g. CHASE_SAFE, CHASE_NO_AUTOFS, CHASE_PROHIBIT_SYMLINKS).
 *
 * Notably, the following are *not* listed here:
 *   - CHASE_TRIGGER_AUTOFS: plain open() already triggers automounts, and O_PATH shortcuts can use
 *     XO_TRIGGER_AUTOMOUNT to tell xopenat_full() to use open_tree() instead.
 *   - CHASE_MUST_BE_{DIRECTORY,REGULAR,SOCKET}: xopenat_full() can enforce these via O_DIRECTORY,
 *     XO_REGULAR and XO_SOCKET. Shortcut callers that don't go through xopenat_full() (stat/access
 *     paths) must include CHASE_MUST_BE_ANY in their local mask to still bail on these. */
#define CHASE_NO_SHORTCUT_MASK                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_SAFE |                                   \
         CHASE_STEP |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_PARENT |                                 \
         CHASE_MKDIR_0755)

#define CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

ایستا صحیح chase_statx(صحیح fd, ساختار statx *ret) {
        بازگشت xstatx_full(fd,
                        /* path= */ پوچ,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ایستا صحیح chase_openat2(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ایستا bool can_openat2 = true;
        صحیح r;

        أكد(path);
        أكد(wildcard_fd_is_valid(dir_fd));

        اگر (!can_openat2)
                بازگشت -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        اگر ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                بازگشت -EOPNOTSUPP;
        اگر (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                بازگشت -EOPNOTSUPP;
        اگر (ریشه_fd != XAT_FDROOT && ریشه_fd != dir_fd)
                بازگشت -EOPNOTSUPP;

        _cleanup_close_ صحیح dir_fd_local = -EBADF;
        اگر (dir_fd == XAT_FDROOT) {
                اگر (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                وگرنه {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        اگر (dir_fd_local < 0)
                                بازگشت -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ساختار open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        اگر (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        اگر (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        اگر (ریشه_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_ریشه;
        اگر (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ صحیح fd = openat2(dir_fd, path, &how, اندازه(how));
        اگر (fd < 0) {
                /* ENOSYS: kernel too old or seccomp filter (systemd's filter returns ENOSYS).
                 * EPERM: Some seccomp profiles of container runtimes use EPERM rather than ENOSYS.
                 * But EPERM might also be returned because we can't access some component of the path. So
                 * we can't cache the result and skip using openat2() if it is blocked with EPERM. Instead
                 * we fall back to userspace chase() if we get EPERM.
                 * EAGAIN: with RESOLVE_IN_ROOT the kernel returns this when a ".." component
                 * (typically from following a symlink like /etc/os-release → ../usr/lib/os-release)
                 * is processed and the global mount_lock or rename_lock seqcount changed during
                 * the walk. Any mount activity anywhere in the system bumps mount_lock, so this
                 * fires reliably while we're still setting up a mount tree. Fall back to the
                 * regular chase loop, which handles root boundaries without openat2(). Don't
                 * cache this — the condition is per-call, not a kernel/sandbox capability. */
                اگر (errno == ENOSYS)
                        can_openat2 = false;
                اگر (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        بازگشت -EOPNOTSUPP;
                بازگشت -errno;
        }

        اگر (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                اگر (r < 0)
                        بازگشت r;
        }
        اگر (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت TAKE_FD(fd);
}

ایستا صحیح chase_xopenat(صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, صحیح open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        اگر (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        اگر (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        اگر (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        اگر (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        اگر (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        بازگشت xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

ایستا bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        اگر (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                بازگشت false;

        بازگشت a != b; /* Otherwise we need to stay within the same UID */
}

صحیح statx_unsafe_transition(ثابت ساختار statx *a, ثابت ساختار statx *b) {
        أكد(a);
        أكد(b);

        اگر (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                بازگشت -ENODATA;

        بازگشت uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ثابت ساختار stat *a, ثابت ساختار stat *b) {
        أكد(a);
        أكد(b);

        بازگشت uid_unsafe_transition(a->st_uid, b->st_uid);
}

ایستا صحیح log_unsafe_transition(صحیح a, صحیح b, ثابت نویسه *path, ChaseFlags flags) {
        _cleanup_free_ نویسه *n1 = پوچ, *n2 = پوچ, *user_a = پوچ, *user_b = پوچ;
        ساختار stat st;

        اگر (!FLAGS_SET(flags, CHASE_WARN))
                بازگشت -ENOLINK;

        (تهی) fd_get_path(a, &n1);
        (تهی) fd_get_path(b, &n2);

        اگر (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        اگر (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        بازگشت log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_راست), strna(n2), strna(user_b), path);
}

ایستا صحیح log_autofs_mount_point(صحیح fd, ثابت نویسه *path, ChaseFlags flags) {
        _cleanup_free_ نویسه *n1 = پوچ;

        اگر (!FLAGS_SET(flags, CHASE_WARN))
                بازگشت -EREMOTE;

        (تهی) fd_get_path(fd, &n1);

        بازگشت log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

ایستا صحیح log_prohibited_symlink(صحیح fd, ChaseFlags flags) {
        _cleanup_free_ نویسه *n1 = پوچ;

        أكد(fd >= 0);

        اگر (!FLAGS_SET(flags, CHASE_WARN))
                بازگشت -ELOOP;

        (تهی) fd_get_path(fd, &n1);

        بازگشت log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

صحیح chaseat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags flags, نویسه **ret_path, صحیح *ret_fd) {
        صحیح r;

        أكد(!FLAGS_SET(flags, CHASE_PREFIX_ریشه));
        أكد(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        أكد(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(wildcard_fd_is_valid(ریشه_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        أكد(dir_fd != AT_FDCWD || IN_SET(ریشه_fd, AT_FDCWD, XAT_FDROOT));

        اگر (FLAGS_SET(flags, CHASE_STEP))
                أكد(!ret_fd);

        /* This function resolves symlinks of the path relative to the given directory file descriptor.
         * The root directory file descriptor sets the chroot boundary: symlinks may not escape it, and
         * absolute symlinks encountered during resolution are resolved relative to it. When the root fd is
         * XAT_FDROOT, symlinks are resolved relative to the host's root directory with no containment.
         *
         * The given path is always resolved starting at dir_fd, regardless of whether it is absolute or
         * relative. The leading slashes of an absolute path are ignored. The only exceptions are
         * dir_fd == XAT_FDROOT (which starts resolution at root_fd) and dir_fd == AT_FDCWD with an absolute
         * path (which starts resolution at "/" rather than the current working directory).
         *
         * Note that we do not verify that dir_fd actually points to a descendant of root_fd. If dir_fd
         * lies outside the root_fd subtree, ".." traversal and absolute symlinks may still be clamped to
         * root_fd, leading to surprising results. Callers must ensure the relationship themselves.
         *
         * Absolute paths returned by this function are relative to the given root file descriptor. Relative
         * paths returned by this function are relative to the given directory file descriptor. The result is
         * absolute when root_fd is XAT_FDROOT (i.e. there is no chroot boundary, so openat()-like callers
         * need an absolute path to reach the host inode), or when an absolute symlink made us jump to a
         * different subtree than the one dir_fd points into. Otherwise the result is relative.
         *
         * Algorithmically this operates on two path buffers: "done" are the components of the path we
         * already processed and resolved symlinks, "." and ".." of. "todo" are the components of the path we
         * still need to process. On each iteration, we move one component from "todo" to "done", processing
         * its special meaning each time. We always keep an O_PATH fd to the component we are currently
         * processing, thus keeping lookup races to a minimum.
         *
         * There are five ways to invoke this function:
         *
         * 1. Without CHASE_STEP or ret_fd: in this case the path is resolved and the normalized path is
         *    returned in `ret_path`. The return value is < 0 on error. If CHASE_NONEXISTENT is also set, 0
         *    is returned if the file doesn't exist, > 0 otherwise. If CHASE_NONEXISTENT is not set, >= 0 is
         *    returned if the destination was found, -ENOENT if it wasn't.
         *
         * 2. With ret_fd: in this case the destination is opened after chasing it as O_PATH and this file
         *    descriptor is returned as return value. This is useful to open files relative to some root
         *    directory. Note that the returned O_PATH file descriptors must be converted into a regular one
         *    (using fd_reopen() or such) before it can be used for reading/writing. If CHASE_NONEXISTENT
         *    is also set and the destination doesn't exist, 0 is returned and ret_fd is set to -EBADF.
         *
         * 3. With CHASE_STEP: in this case only a single step of the normalization is executed, i.e. only
         *    the first symlink or ".." component of the path is resolved, and the resulting path is
         *    returned. This is useful if a caller wants to trace the path through the file system verbosely.
         *    Returns < 0 on error, > 0 if the path is fully normalized, and == 0 for each normalization
         *    step. This may be combined with CHASE_NONEXISTENT, in which case 1 is returned when a component
         *    is not found.
         *
         * 4. With CHASE_SAFE: in this case the path must not contain unsafe transitions, i.e. transitions
         *    from unprivileged to privileged files or directories. In such cases the return value is
         *    -ENOLINK. If CHASE_WARN is also set, a warning describing the unsafe transition is emitted.
         *    CHASE_WARN cannot be used in PID 1.
         *
         * 5. With CHASE_NO_AUTOFS: in this case if an autofs mount point is encountered, path normalization
         *    is aborted and -EREMOTE is returned. If CHASE_WARN is also set, a warning showing the path of
         *    the mount point is emitted. CHASE_WARN cannot be used in PID 1.
         */

        /* We treat AT_FDCWD as XAT_FDROOT for a more seamless migration for all callers of chaseat() before
         * it was reworked to support separate root_fd and dir_fd arguments. */
        اگر (ریشه_fd == AT_FDCWD)
                ریشه_fd = XAT_FDROOT;
        وگرنه {
                r = dir_fd_is_ریشه(ریشه_fd);
                اگر (r < 0)
                        بازگشت r;
                اگر (r > 0)
                        ریشه_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_ریشه(dir_fd);
        اگر (r < 0)
                بازگشت r;
        اگر (r > 0 && ریشه_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        اگر (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = ریشه_fd;

        اگر (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        اگر (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                اگر (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } وگرنه اگر (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        اگر (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        اگر (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                بازگشت -EBADSLT;

        اگر (!ret_path) {
                r = chase_openat2(ریشه_fd, dir_fd, path, flags);
                اگر (r >= 0) {
                        اگر (ret_fd)
                                *ret_fd = r;
                        وگرنه
                                safe_close(r);

                        بازگشت 1;
                }
                اگر (r != -EOPNOTSUPP)
                        بازگشت r;
        }

        اگر (ریشه_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                اگر (r < 0)
                        بازگشت r;

                اگر (ret_fd)
                        *ret_fd = r;
                وگرنه
                        safe_close(r);

                بازگشت 1;
        }

        /* Decide whether to return an absolute or relative path.
         *
         * We return an absolute path only when there is no chroot boundary (root_fd == XAT_FDROOT)
         * and resolution starts from root — i.e. either dir_fd was XAT_FDROOT or path is absolute,
         * both of which caused dir_fd = root_fd above. In every other case we return a relative
         * path so the result keeps working when fed to an openat()-style call against dir_fd,
         * which would ignore dir_fd if handed an absolute path.
         *
         * When root_fd != XAT_FDROOT and an absolute symlink later causes resolution to escape
         * dir_fd, the loop below rebases onto root_fd and switches to an absolute result at that
         * point — it is not handled here.
         */
        bool need_absolute = (ریشه_fd == XAT_FDROOT || dir_fd != ریشه_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ نویسه *done = پوچ;
        اگر (need_absolute) {
                done = strdup("/");
                اگر (!done)
                        بازگشت -ENOMEM;
        }

        _cleanup_close_ صحیح fd = xopenat(dir_fd, پوچ, O_CLOEXEC|O_DIRECTORY|O_PATH);
        اگر (fd < 0)
                بازگشت fd;

        ساختار statx stx;
        r = chase_statx(fd, &stx);
        اگر (r < 0)
                بازگشت r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ساختار statx ریشه_stx;
        اگر (ریشه_fd != XAT_FDROOT) {
                اگر (ریشه_fd == dir_fd)
                        ریشه_stx = stx;
                وگرنه {
                        r = chase_statx(ریشه_fd, &ریشه_stx);
                        اگر (r < 0)
                                بازگشت r;
                }
        }

        _cleanup_free_ نویسه *buffer = strdup(path);
        اگر (!buffer)
                بازگشت -ENOMEM;

        ثابت نویسه *todo = buffer;
        bool exists = true;
        برای (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ نویسه *first = پوچ;
                _cleanup_close_ صحیح child = -EBADF;
                ساختار statx stx_child;
                ثابت نویسه *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                اگر (n_steps > CHASE_MAX)
                        بازگشت -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0) /* We reached the end. */
                        بشکن;

                first = strndup(e, r);
                اگر (!first)
                        بازگشت -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                اگر (streq(first, "..")) {
                        _cleanup_free_ نویسه *parent = پوچ;
                        _cleanup_close_ صحیح fd_parent = -EBADF;
                        ساختار statx stx_parent;

                        /* If we already are at the top, then going up will not change anything. This is
                         * in-line with how the kernel handles this. We check this both by path and by
                         * inode/mount identity check. The latter is load-bearing if concurrent access of the
                         * root tree we operate in is allowed, where an inode is moved up the tree while we
                         * look at it, and thus get the current path wrong and think we are deeper down than
                         * we actually are.
                         *
                         * The path-based fast path is only valid when the caller started at the root fd:
                         * otherwise 'done' being empty just means we haven't descended past the starting
                         * dir_fd, not that we're at the chroot boundary. */
                        اگر (ریشه_fd != XAT_FDROOT) {
                                bool is_ریشه = ریشه_fd == dir_fd && empty_or_ریشه(done);
                                اگر (!is_ریشه && statx_inode_same(&stx, &ریشه_stx)) {
                                        r = statx_mount_same(&stx, &ریشه_stx);
                                        اگر (r < 0)
                                                بازگشت r;

                                        is_ریشه = r > 0;
                                }
                                اگر (is_ریشه) {
                                        اگر (FLAGS_SET(flags, CHASE_STEP))
                                                برو chased_one;
                                        ادامه;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        اگر (fd_parent < 0)
                                بازگشت -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        اگر (r < 0)
                                بازگشت r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        اگر (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_ریشه(fd);
                                اگر (r < 0)
                                        بازگشت r;
                                اگر (r > 0) {
                                        اگر (FLAGS_SET(flags, CHASE_STEP))
                                                برو chased_one;
                                        ادامه;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        اگر (r >= 0) {
                                أكد(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } وگرنه اگر (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                أكد(!need_absolute);
                                done = mfree(done);
                        } وگرنه اگر (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } وگرنه اگر (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                أكد(!need_absolute);

                                اگر (!isempty(done) && !path_is_valid(done))
                                        بازگشت -EINVAL;

                                اگر (!path_extend(&done, ".."))
                                        بازگشت -ENOMEM;
                        } وگرنه
                                بازگشت r;

                        اگر (FLAGS_SET(flags, CHASE_STEP))
                                برو chased_one;

                        اگر (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                اگر (r < 0)
                                        بازگشت r;
                                اگر (r > 0)
                                        بازگشت log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        اگر (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ صحیح fd_grandparent = -EBADF;
                                ساختار statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                اگر (fd_grandparent < 0)
                                        بازگشت -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        اگر (r < 0)
                                                بازگشت r;
                                        اگر (r > 0)
                                                بازگشت log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                بشکن;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        ادامه;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                اگر (r < 0) {
                        اگر (r != -ENOENT)
                                بازگشت r;

                        اگر (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                بازگشت r;

                        اگر (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                اگر (child < 0)
                                        بازگشت child;
                        } وگرنه اگر (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                اگر (!path_extend(&done, first))
                                        بازگشت -ENOMEM;

                                بشکن;
                        } وگرنه اگر (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                اگر (!path_extend(&done, first, todo))
                                        بازگشت -ENOMEM;

                                exists = false;
                                بشکن;
                        } وگرنه
                                بازگشت r;
                }

                r = chase_statx(child, &stx_child);
                اگر (r < 0)
                        بازگشت r;

                اگر (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r > 0)
                                بازگشت log_unsafe_transition(fd, child, path, flags);
                }

                اگر (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        بازگشت log_autofs_mount_point(child, path, flags);

                اگر (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ نویسه *destination = پوچ;

                        اگر (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                بازگشت log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (isempty(destination))
                                بازگشت -EINVAL;

                        اگر (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(ریشه_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                اگر (fd < 0)
                                        بازگشت fd;

                                r = chase_statx(fd, &stx);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        اگر (r < 0)
                                                بازگشت r;
                                        اگر (r > 0)
                                                بازگشت log_unsafe_transition(child, fd, path, flags);
                                }

                                اگر (dir_fd != ریشه_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : پوچ);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        اگر (!path_extend(&destination, todo))
                                بازگشت -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        اگر (FLAGS_SET(flags, CHASE_STEP))
                                برو chased_one;

                        ادامه;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                اگر (!path_extend(&done, first))
                        بازگشت -ENOMEM;

                اگر (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        بشکن;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        اگر (exists) {
                اگر (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        اگر (r < 0)
                                بازگشت r;
                }
        }

        اگر (ret_path) {
                اگر (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ نویسه *f = پوچ;

                        r = path_extract_filename(done, &f);
                        اگر (r < 0 && r != -EADDRNOTAVAIL)
                                بازگشت r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                اگر (!done) {
                        أكد(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        اگر (!done)
                                بازگشت -ENOMEM;
                }

                اگر (append_trail_slash)
                        اگر (!strextend(&done, "/"))
                                بازگشت -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        اگر (ret_fd) {
                اگر (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        أكد(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } وگرنه
                        *ret_fd = -EBADF;
        }

        اگر (FLAGS_SET(flags, CHASE_STEP))
                بازگشت 1;

        بازگشت exists;

chased_one:
        اگر (ret_path) {
                ثابت نویسه *e;

                اگر (!done) {
                        أكد(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        اگر (!done)
                                بازگشت -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        *ret_path = TAKE_PTR(done);
                وگرنه {
                        نویسه *c;

                        c = path_join(done, e);
                        اگر (!c)
                                بازگشت -ENOMEM;

                        *ret_path = c;
                }
        }

        بازگشت 0;
}

صحیح chase(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags flags, نویسه **ret_path, صحیح *ret_fd) {
        _cleanup_free_ نویسه *ریشه_abs = پوچ, *absolute = پوچ, *p = پوچ;
        _cleanup_close_ صحیح fd = -EBADF, pfd = -EBADF;
        صحیح r;

        أكد(path);

        اگر (isempty(path))
                بازگشت -EINVAL;

        r = empty_or_ریشه_harder_to_null(&ریشه);
        اگر (r < 0)
                بازگشت r;

        /* A root directory of "/" or "" is identical to "/". */
        اگر (empty_or_ریشه(ریشه))
                ریشه = "/";
        وگرنه {
                r = path_make_absolute_cwd(ریشه, &ریشه_abs);
                اگر (r < 0)
                        بازگشت r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                ریشه = path_simplify(ریشه_abs);

                أكد(path_is_absolute(ریشه));
                أكد(!empty_or_ریشه(ریشه));

                اگر (FLAGS_SET(flags, CHASE_PREFIX_ریشه)) {
                        absolute = path_join(ریشه, path);
                        اگر (!absolute)
                                بازگشت -ENOMEM;
                }
        }

        اگر (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                اگر (r < 0)
                        بازگشت r;
        }

        path = path_startswith(absolute, ریشه);
        اگر (!path)
                بازگشت log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, ریشه);

        اگر (empty_or_ریشه(ریشه))
                fd = XAT_FDROOT;
        وگرنه {
                fd = open(ریشه, O_CLOEXEC|O_DIRECTORY|O_PATH);
                اگر (fd < 0)
                        بازگشت -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_ریشه, ret_path ? &p : پوچ, ret_fd ? &pfd : پوچ);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path) {
                اگر (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        اگر (empty_or_ریشه(ریشه))
                                أكد(path_is_absolute(p));
                        وگرنه {
                                نویسه *q;

                                أكد(!path_is_absolute(p));

                                q = path_join(ریشه, p + STR_IN_SET(p, ".", "./"));
                                اگر (!q)
                                        بازگشت -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        بازگشت r;
}

صحیح chaseat_prefix_ریشه(ثابت نویسه *path, ثابت نویسه *ریشه, نویسه **ret) {
        نویسه *q;
        صحیح r;

        أكد(path);
        أكد(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        اگر (!path_is_absolute(path)) {
                _cleanup_free_ نویسه *ریشه_abs = پوچ;

                r = empty_or_ریشه_harder_to_null(&ریشه);
                اگر (r < 0 && r != -ENOENT)
                        بازگشت r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                اگر (empty_or_ریشه(ریشه))
                        بازگشت -EINVAL;

                r = path_make_absolute_cwd(ریشه, &ریشه_abs);
                اگر (r < 0)
                        بازگشت r;

                ریشه = path_simplify(ریشه_abs);

                q = path_join(ریشه, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } وگرنه
                q = strdup(path);
        اگر (!q)
                بازگشت -ENOMEM;

        *ret = q;
        بازگشت 0;
}

صحیح chase_extract_filename(ثابت نویسه *path, ثابت نویسه *ریشه, نویسه **ret) {
        صحیح r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        أكد(path);
        أكد(ret);

        اگر (isempty(path))
                بازگشت -EINVAL;

        اگر (!path_is_absolute(path))
                بازگشت -EINVAL;

        r = empty_or_ریشه_harder_to_null(&ریشه);
        اگر (r < 0 && r != -ENOENT)
                بازگشت r;

        اگر (!empty_or_ریشه(ریشه)) {
                _cleanup_free_ نویسه *ریشه_abs = پوچ;

                r = path_make_absolute_cwd(ریشه, &ریشه_abs);
                اگر (r < 0)
                        بازگشت r;

                path = path_startswith(path, ریشه_abs);
                اگر (!path)
                        بازگشت -EINVAL;
        }

        اگر (!isempty(path)) {
                r = path_extract_filename(path, ret);
                اگر (r != -EADDRNOTAVAIL)
                        بازگشت r;
        }

        بازگشت strdup_to(ret, ".");
}

صحیح chase_and_open(
                ثابت نویسه *path,
                ثابت نویسه *ریشه,
                ChaseFlags chase_flags,
                صحیح open_flags,
                نویسه **ret_path) {

        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ, *fname = پوچ;
        ثابت نویسه *open_name = پوچ;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        اگر (empty_or_ریشه(ریشه) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                بازگشت chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, ریشه, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        اگر (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                اگر (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                وگرنه {
                        r = chase_extract_filename(p, ریشه, &fname);
                        اگر (r < 0)
                                بازگشت r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت r;
}

صحیح chase_and_opendir(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, نویسه **ret_path, DIR **ret_dir) {
        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        DIR *d;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        اگر (empty_or_ریشه(ریشه) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                اگر (!d)
                        بازگشت -errno;

                *ret_dir = d;
                بازگشت 0;
        }

        r = chase(path, ریشه, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : پوچ, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ پوچ, /* flags= */ 0);
        اگر (!d)
                بازگشت -errno;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        بازگشت 0;
}

صحیح chase_and_stat(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, نویسه **ret_path, ساختار stat *ret_stat) {
        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        أكد(ret_stat);

        اگر (empty_or_ریشه(ریشه) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                بازگشت RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, ریشه, chase_flags, ret_path ? &p : پوچ, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        اگر (fstat(path_fd, ret_stat) < 0)
                بازگشت -errno;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت 0;
}

صحیح chase_and_access(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, صحیح access_mode, نویسه **ret_path) {
        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        اگر (empty_or_ریشه(ریشه) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                بازگشت RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, ریشه, chase_flags, ret_path ? &p : پوچ, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت 0;
}

صحیح chase_and_fopen_unlocked(
                ثابت نویسه *path,
                ثابت نویسه *ریشه,
                ChaseFlags chase_flags,
                ثابت نویسه *open_flags,
                نویسه **ret_path,
                FILE **ret_file) {

        _cleanup_free_ نویسه *final_path = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح mode_flags, r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        أكد(open_flags);
        أكد(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        اگر (mode_flags < 0)
                بازگشت mode_flags;

        fd = chase_and_open(path, ریشه, chase_flags, mode_flags, ret_path ? &final_path : پوچ);
        اگر (fd < 0)
                بازگشت fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(final_path);

        بازگشت 0;
}

صحیح chase_and_unlink(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, صحیح unlink_flags, نویسه **ret_path) {
        _cleanup_free_ نویسه *p = پوچ, *fname = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, ریشه, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        اگر (fd < 0)
                بازگشت fd;

        r = path_extract_filename(p, &fname);
        اگر (r < 0)
                بازگشت r;

        اگر (unlinkat(fd, fname, unlink_flags) < 0)
                بازگشت -errno;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت 0;
}

صحیح chase_and_open_parent(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, نویسه **ret_filename) {
        صحیح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, ریشه, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        اگر (r < 0)
                بازگشت r;

        بازگشت pfd;
}

صحیح chase_and_openat(
                صحیح ریشه_fd,
                صحیح dir_fd,
                ثابت نویسه *path,
                ChaseFlags chase_flags,
                صحیح open_flags,
                نویسه **ret_path) {

        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ, *fname = پوچ;
        ثابت نویسه *open_name = پوچ;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        اگر (ریشه_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                بازگشت chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(ریشه_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        اگر (r < 0)
                بازگشت r;

        اگر (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                اگر (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                وگرنه {
                        r = path_extract_filename(p, &fname);
                        اگر (r < 0 && r != -EADDRNOTAVAIL)
                                بازگشت r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت r;
}

صحیح chase_and_opendirat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, نویسه **ret_path, DIR **ret_dir) {
        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        DIR *d;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        اگر (ریشه_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                اگر (!d)
                        بازگشت -errno;

                *ret_dir = d;
                بازگشت 0;
        }

        r = chaseat(ریشه_fd, dir_fd, path, chase_flags, ret_path ? &p : پوچ, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ پوچ, /* flags= */ 0);
        اگر (!d)
                بازگشت -errno;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        بازگشت 0;
}

صحیح chase_and_statat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, نویسه **ret_path, ساختار stat *ret_stat) {
        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        أكد(ret_stat);

        اگر (ریشه_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                بازگشت RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ریشه_fd, dir_fd, path, chase_flags, ret_path ? &p : پوچ, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        اگر (fstat(path_fd, ret_stat) < 0)
                بازگشت -errno;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت 0;
}

صحیح chase_and_accessat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, صحیح access_mode, نویسه **ret_path) {
        _cleanup_close_ صحیح path_fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        اگر (ریشه_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                بازگشت RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ریشه_fd, dir_fd, path, chase_flags, ret_path ? &p : پوچ, &path_fd);
        اگر (r < 0)
                بازگشت r;
        أكد(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت 0;
}

صحیح chase_and_fopenat_unlocked(
                صحیح ریشه_fd,
                صحیح dir_fd,
                ثابت نویسه *path,
                ChaseFlags chase_flags,
                ثابت نویسه *open_flags,
                نویسه **ret_path,
                FILE **ret_file) {

        _cleanup_free_ نویسه *final_path = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح mode_flags, r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        أكد(open_flags);
        أكد(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        اگر (mode_flags < 0)
                بازگشت mode_flags;

        fd = chase_and_openat(ریشه_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : پوچ);
        اگر (fd < 0)
                بازگشت fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(final_path);

        بازگشت 0;
}

صحیح chase_and_unlinkat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, صحیح unlink_flags, نویسه **ret_path) {
        _cleanup_free_ نویسه *p = پوچ, *fname = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(ریشه_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        اگر (fd < 0)
                بازگشت fd;

        r = path_extract_filename(p, &fname);
        اگر (r < 0)
                بازگشت r;

        اگر (unlinkat(fd, fname, unlink_flags) < 0)
                بازگشت -errno;

        اگر (ret_path)
                *ret_path = TAKE_PTR(p);

        بازگشت 0;
}

صحیح chase_and_open_parent_at(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, نویسه **ret_filename) {
        صحیح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(ریشه_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        اگر (r < 0)
                بازگشت r;

        بازگشت pfd;
}
