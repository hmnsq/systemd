/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/magic.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "glyph-util.h"
#ضمن "log.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "user-util.h"

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

ساكن صحيح chase_statx(صحيح fd, بنية statx *ret) {
        أرجع xstatx_full(fd,
                        /* path= */ عدم,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ساكن صحيح chase_openat2(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ساكن bool can_openat2 = true;
        صحيح r;

        أكد(path);
        أكد(wildcard_fd_is_valid(dir_fd));

        إذا (!can_openat2)
                أرجع -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        إذا ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                أرجع -EOPNOTSUPP;
        إذا (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                أرجع -EOPNOTSUPP;
        إذا (اکر_fd != XAT_FDROOT && اکر_fd != dir_fd)
                أرجع -EOPNOTSUPP;

        _cleanup_close_ صحيح dir_fd_local = -EBADF;
        إذا (dir_fd == XAT_FDROOT) {
                إذا (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                وإلا {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        إذا (dir_fd_local < 0)
                                أرجع -errno;
                        dir_fd = dir_fd_local;
                }
        }

        بنية open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        إذا (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        إذا (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        إذا (اکر_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_اکر;
        إذا (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ صحيح fd = openat2(dir_fd, path, &how, حجم(how));
        إذا (fd < 0) {
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
                إذا (errno == ENOSYS)
                        can_openat2 = false;
                إذا (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        أرجع -EOPNOTSUPP;
                أرجع -errno;
        }

        إذا (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                إذا (r < 0)
                        أرجع r;
        }
        إذا (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع TAKE_FD(fd);
}

ساكن صحيح chase_xopenat(صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags, صحيح open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        إذا (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        إذا (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        إذا (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        إذا (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        إذا (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        أرجع xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

ساكن bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        إذا (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                أرجع false;

        أرجع a != b; /* Otherwise we need to stay within the same UID */
}

صحيح statx_unsafe_transition(ثابت بنية statx *a, ثابت بنية statx *b) {
        أكد(a);
        أكد(b);

        إذا (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                أرجع -ENODATA;

        أرجع uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ثابت بنية stat *a, ثابت بنية stat *b) {
        أكد(a);
        أكد(b);

        أرجع uid_unsafe_transition(a->st_uid, b->st_uid);
}

ساكن صحيح log_unsafe_transition(صحيح a, صحيح b, ثابت حرف *path, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = عدم, *n2 = عدم, *user_a = عدم, *user_b = عدم;
        بنية stat st;

        إذا (!FLAGS_SET(flags, CHASE_WARN))
                أرجع -ENOLINK;

        (فراغ) fd_get_path(a, &n1);
        (فراغ) fd_get_path(b, &n2);

        إذا (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        إذا (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        أرجع log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_کانن), strna(n2), strna(user_b), path);
}

ساكن صحيح log_autofs_mount_point(صحيح fd, ثابت حرف *path, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = عدم;

        إذا (!FLAGS_SET(flags, CHASE_WARN))
                أرجع -EREMOTE;

        (فراغ) fd_get_path(fd, &n1);

        أرجع log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

ساكن صحيح log_prohibited_symlink(صحيح fd, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = عدم;

        أكد(fd >= 0);

        إذا (!FLAGS_SET(flags, CHASE_WARN))
                أرجع -ELOOP;

        (فراغ) fd_get_path(fd, &n1);

        أرجع log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

صحيح chaseat(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags flags, حرف **ret_path, صحيح *ret_fd) {
        صحيح r;

        أكد(!FLAGS_SET(flags, CHASE_PREFIX_اکر));
        أكد(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        أكد(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(wildcard_fd_is_valid(اکر_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        أكد(dir_fd != AT_FDCWD || IN_SET(اکر_fd, AT_FDCWD, XAT_FDROOT));

        إذا (FLAGS_SET(flags, CHASE_STEP))
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
        إذا (اکر_fd == AT_FDCWD)
                اکر_fd = XAT_FDROOT;
        وإلا {
                r = dir_fd_is_اکر(اکر_fd);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        اکر_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_اکر(dir_fd);
        إذا (r < 0)
                أرجع r;
        إذا (r > 0 && اکر_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        إذا (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = اکر_fd;

        إذا (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        إذا (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                إذا (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } وإلا إذا (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        إذا (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        إذا (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                أرجع -EBADSLT;

        إذا (!ret_path) {
                r = chase_openat2(اکر_fd, dir_fd, path, flags);
                إذا (r >= 0) {
                        إذا (ret_fd)
                                *ret_fd = r;
                        وإلا
                                safe_close(r);

                        أرجع 1;
                }
                إذا (r != -EOPNOTSUPP)
                        أرجع r;
        }

        إذا (اکر_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                إذا (r < 0)
                        أرجع r;

                إذا (ret_fd)
                        *ret_fd = r;
                وإلا
                        safe_close(r);

                أرجع 1;
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
        bool need_absolute = (اکر_fd == XAT_FDROOT || dir_fd != اکر_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ حرف *done = عدم;
        إذا (need_absolute) {
                done = strdup("/");
                إذا (!done)
                        أرجع -ENOMEM;
        }

        _cleanup_close_ صحيح fd = xopenat(dir_fd, عدم, O_CLOEXEC|O_DIRECTORY|O_PATH);
        إذا (fd < 0)
                أرجع fd;

        بنية statx stx;
        r = chase_statx(fd, &stx);
        إذا (r < 0)
                أرجع r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        بنية statx اکر_stx;
        إذا (اکر_fd != XAT_FDROOT) {
                إذا (اکر_fd == dir_fd)
                        اکر_stx = stx;
                وإلا {
                        r = chase_statx(اکر_fd, &اکر_stx);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        _cleanup_free_ حرف *buffer = strdup(path);
        إذا (!buffer)
                أرجع -ENOMEM;

        ثابت حرف *todo = buffer;
        bool exists = true;
        لكل (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ حرف *first = عدم;
                _cleanup_close_ صحيح child = -EBADF;
                بنية statx stx_child;
                ثابت حرف *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                إذا (n_steps > CHASE_MAX)
                        أرجع -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0) /* We reached the end. */
                        اكسر;

                first = strndup(e, r);
                إذا (!first)
                        أرجع -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                إذا (streq(first, "..")) {
                        _cleanup_free_ حرف *parent = عدم;
                        _cleanup_close_ صحيح fd_parent = -EBADF;
                        بنية statx stx_parent;

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
                        إذا (اکر_fd != XAT_FDROOT) {
                                bool is_اکر = اکر_fd == dir_fd && empty_or_اکر(done);
                                إذا (!is_اکر && statx_inode_same(&stx, &اکر_stx)) {
                                        r = statx_mount_same(&stx, &اکر_stx);
                                        إذا (r < 0)
                                                أرجع r;

                                        is_اکر = r > 0;
                                }
                                إذا (is_اکر) {
                                        إذا (FLAGS_SET(flags, CHASE_STEP))
                                                اذهب chased_one;
                                        استمر;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        إذا (fd_parent < 0)
                                أرجع -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        إذا (r < 0)
                                أرجع r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        إذا (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_اکر(fd);
                                إذا (r < 0)
                                        أرجع r;
                                إذا (r > 0) {
                                        إذا (FLAGS_SET(flags, CHASE_STEP))
                                                اذهب chased_one;
                                        استمر;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        إذا (r >= 0) {
                                أكد(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } وإلا إذا (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                أكد(!need_absolute);
                                done = mfree(done);
                        } وإلا إذا (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } وإلا إذا (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                أكد(!need_absolute);

                                إذا (!isempty(done) && !path_is_valid(done))
                                        أرجع -EINVAL;

                                إذا (!path_extend(&done, ".."))
                                        أرجع -ENOMEM;
                        } وإلا
                                أرجع r;

                        إذا (FLAGS_SET(flags, CHASE_STEP))
                                اذهب chased_one;

                        إذا (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                إذا (r < 0)
                                        أرجع r;
                                إذا (r > 0)
                                        أرجع log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        إذا (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ صحيح fd_grandparent = -EBADF;
                                بنية statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                إذا (fd_grandparent < 0)
                                        أرجع -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        إذا (r < 0)
                                                أرجع r;
                                        إذا (r > 0)
                                                أرجع log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                اكسر;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        استمر;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                إذا (r < 0) {
                        إذا (r != -ENOENT)
                                أرجع r;

                        إذا (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                أرجع r;

                        إذا (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                إذا (child < 0)
                                        أرجع child;
                        } وإلا إذا (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                إذا (!path_extend(&done, first))
                                        أرجع -ENOMEM;

                                اكسر;
                        } وإلا إذا (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                إذا (!path_extend(&done, first, todo))
                                        أرجع -ENOMEM;

                                exists = false;
                                اكسر;
                        } وإلا
                                أرجع r;
                }

                r = chase_statx(child, &stx_child);
                إذا (r < 0)
                        أرجع r;

                إذا (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r > 0)
                                أرجع log_unsafe_transition(fd, child, path, flags);
                }

                إذا (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        أرجع log_autofs_mount_point(child, path, flags);

                إذا (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ حرف *destination = عدم;

                        إذا (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                أرجع log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        إذا (r < 0)
                                أرجع r;
                        إذا (isempty(destination))
                                أرجع -EINVAL;

                        إذا (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(اکر_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                إذا (fd < 0)
                                        أرجع fd;

                                r = chase_statx(fd, &stx);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        إذا (r < 0)
                                                أرجع r;
                                        إذا (r > 0)
                                                أرجع log_unsafe_transition(child, fd, path, flags);
                                }

                                إذا (dir_fd != اکر_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : عدم);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        إذا (!path_extend(&destination, todo))
                                أرجع -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        إذا (FLAGS_SET(flags, CHASE_STEP))
                                اذهب chased_one;

                        استمر;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                إذا (!path_extend(&done, first))
                        أرجع -ENOMEM;

                إذا (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        اكسر;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        إذا (exists) {
                إذا (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        إذا (r < 0)
                                أرجع r;
                }

                إذا (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        إذا (r < 0)
                                أرجع r;
                }

                إذا (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        إذا (ret_path) {
                إذا (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ حرف *f = عدم;

                        r = path_extract_filename(done, &f);
                        إذا (r < 0 && r != -EADDRNOTAVAIL)
                                أرجع r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                إذا (!done) {
                        أكد(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        إذا (!done)
                                أرجع -ENOMEM;
                }

                إذا (append_trail_slash)
                        إذا (!strextend(&done, "/"))
                                أرجع -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        إذا (ret_fd) {
                إذا (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        أكد(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } وإلا
                        *ret_fd = -EBADF;
        }

        إذا (FLAGS_SET(flags, CHASE_STEP))
                أرجع 1;

        أرجع exists;

chased_one:
        إذا (ret_path) {
                ثابت حرف *e;

                إذا (!done) {
                        أكد(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        إذا (!done)
                                أرجع -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        *ret_path = TAKE_PTR(done);
                وإلا {
                        حرف *c;

                        c = path_join(done, e);
                        إذا (!c)
                                أرجع -ENOMEM;

                        *ret_path = c;
                }
        }

        أرجع 0;
}

صحيح chase(ثابت حرف *path, ثابت حرف *اکر, ChaseFlags flags, حرف **ret_path, صحيح *ret_fd) {
        _cleanup_free_ حرف *اکر_abs = عدم, *absolute = عدم, *p = عدم;
        _cleanup_close_ صحيح fd = -EBADF, pfd = -EBADF;
        صحيح r;

        أكد(path);

        إذا (isempty(path))
                أرجع -EINVAL;

        r = empty_or_اکر_harder_to_null(&اکر);
        إذا (r < 0)
                أرجع r;

        /* A root directory of "/" or "" is identical to "/". */
        إذا (empty_or_اکر(اکر))
                اکر = "/";
        وإلا {
                r = path_make_absolute_cwd(اکر, &اکر_abs);
                إذا (r < 0)
                        أرجع r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                اکر = path_simplify(اکر_abs);

                أكد(path_is_absolute(اکر));
                أكد(!empty_or_اکر(اکر));

                إذا (FLAGS_SET(flags, CHASE_PREFIX_اکر)) {
                        absolute = path_join(اکر, path);
                        إذا (!absolute)
                                أرجع -ENOMEM;
                }
        }

        إذا (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                إذا (r < 0)
                        أرجع r;
        }

        path = path_startswith(absolute, اکر);
        إذا (!path)
                أرجع log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, اکر);

        إذا (empty_or_اکر(اکر))
                fd = XAT_FDROOT;
        وإلا {
                fd = open(اکر, O_CLOEXEC|O_DIRECTORY|O_PATH);
                إذا (fd < 0)
                        أرجع -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_اکر, ret_path ? &p : عدم, ret_fd ? &pfd : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path) {
                إذا (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        إذا (empty_or_اکر(اکر))
                                أكد(path_is_absolute(p));
                        وإلا {
                                حرف *q;

                                أكد(!path_is_absolute(p));

                                q = path_join(اکر, p + STR_IN_SET(p, ".", "./"));
                                إذا (!q)
                                        أرجع -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        أرجع r;
}

صحيح chaseat_prefix_اکر(ثابت حرف *path, ثابت حرف *اکر, حرف **ret) {
        حرف *q;
        صحيح r;

        أكد(path);
        أكد(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        إذا (!path_is_absolute(path)) {
                _cleanup_free_ حرف *اکر_abs = عدم;

                r = empty_or_اکر_harder_to_null(&اکر);
                إذا (r < 0 && r != -ENOENT)
                        أرجع r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                إذا (empty_or_اکر(اکر))
                        أرجع -EINVAL;

                r = path_make_absolute_cwd(اکر, &اکر_abs);
                إذا (r < 0)
                        أرجع r;

                اکر = path_simplify(اکر_abs);

                q = path_join(اکر, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } وإلا
                q = strdup(path);
        إذا (!q)
                أرجع -ENOMEM;

        *ret = q;
        أرجع 0;
}

صحيح chase_extract_filename(ثابت حرف *path, ثابت حرف *اکر, حرف **ret) {
        صحيح r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        أكد(path);
        أكد(ret);

        إذا (isempty(path))
                أرجع -EINVAL;

        إذا (!path_is_absolute(path))
                أرجع -EINVAL;

        r = empty_or_اکر_harder_to_null(&اکر);
        إذا (r < 0 && r != -ENOENT)
                أرجع r;

        إذا (!empty_or_اکر(اکر)) {
                _cleanup_free_ حرف *اکر_abs = عدم;

                r = path_make_absolute_cwd(اکر, &اکر_abs);
                إذا (r < 0)
                        أرجع r;

                path = path_startswith(path, اکر_abs);
                إذا (!path)
                        أرجع -EINVAL;
        }

        إذا (!isempty(path)) {
                r = path_extract_filename(path, ret);
                إذا (r != -EADDRNOTAVAIL)
                        أرجع r;
        }

        أرجع strdup_to(ret, ".");
}

صحيح chase_and_open(
                ثابت حرف *path,
                ثابت حرف *اکر,
                ChaseFlags chase_flags,
                صحيح open_flags,
                حرف **ret_path) {

        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        ثابت حرف *open_name = عدم;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        إذا (empty_or_اکر(اکر) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                أرجع chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, اکر, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        إذا (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                إذا (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                وإلا {
                        r = chase_extract_filename(p, اکر, &fname);
                        إذا (r < 0)
                                أرجع r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع r;
}

صحيح chase_and_opendir(ثابت حرف *path, ثابت حرف *اکر, ChaseFlags chase_flags, حرف **ret_path, DIR **ret_dir) {
        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        DIR *d;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        إذا (empty_or_اکر(اکر) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                إذا (!d)
                        أرجع -errno;

                *ret_dir = d;
                أرجع 0;
        }

        r = chase(path, اکر, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : عدم, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ عدم, /* flags= */ 0);
        إذا (!d)
                أرجع -errno;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        أرجع 0;
}

صحيح chase_and_stat(ثابت حرف *path, ثابت حرف *اکر, ChaseFlags chase_flags, حرف **ret_path, بنية stat *ret_stat) {
        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        أكد(ret_stat);

        إذا (empty_or_اکر(اکر) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                أرجع RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, اکر, chase_flags, ret_path ? &p : عدم, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        إذا (fstat(path_fd, ret_stat) < 0)
                أرجع -errno;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_access(ثابت حرف *path, ثابت حرف *اکر, ChaseFlags chase_flags, صحيح access_mode, حرف **ret_path) {
        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        إذا (empty_or_اکر(اکر) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                أرجع RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, اکر, chase_flags, ret_path ? &p : عدم, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_fopen_unlocked(
                ثابت حرف *path,
                ثابت حرف *اکر,
                ChaseFlags chase_flags,
                ثابت حرف *open_flags,
                حرف **ret_path,
                FILE **ret_file) {

        _cleanup_free_ حرف *final_path = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح mode_flags, r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        أكد(open_flags);
        أكد(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        إذا (mode_flags < 0)
                أرجع mode_flags;

        fd = chase_and_open(path, اکر, chase_flags, mode_flags, ret_path ? &final_path : عدم);
        إذا (fd < 0)
                أرجع fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path)
                *ret_path = TAKE_PTR(final_path);

        أرجع 0;
}

صحيح chase_and_unlink(ثابت حرف *path, ثابت حرف *اکر, ChaseFlags chase_flags, صحيح unlink_flags, حرف **ret_path) {
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, اکر, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        إذا (fd < 0)
                أرجع fd;

        r = path_extract_filename(p, &fname);
        إذا (r < 0)
                أرجع r;

        إذا (unlinkat(fd, fname, unlink_flags) < 0)
                أرجع -errno;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_open_parent(ثابت حرف *path, ثابت حرف *اکر, ChaseFlags chase_flags, حرف **ret_filename) {
        صحيح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, اکر, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        إذا (r < 0)
                أرجع r;

        أرجع pfd;
}

صحيح chase_and_openat(
                صحيح اکر_fd,
                صحيح dir_fd,
                ثابت حرف *path,
                ChaseFlags chase_flags,
                صحيح open_flags,
                حرف **ret_path) {

        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        ثابت حرف *open_name = عدم;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        إذا (اکر_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                أرجع chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(اکر_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        إذا (r < 0)
                أرجع r;

        إذا (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                إذا (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                وإلا {
                        r = path_extract_filename(p, &fname);
                        إذا (r < 0 && r != -EADDRNOTAVAIL)
                                أرجع r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع r;
}

صحيح chase_and_opendirat(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags, حرف **ret_path, DIR **ret_dir) {
        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        DIR *d;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        إذا (اکر_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                إذا (!d)
                        أرجع -errno;

                *ret_dir = d;
                أرجع 0;
        }

        r = chaseat(اکر_fd, dir_fd, path, chase_flags, ret_path ? &p : عدم, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ عدم, /* flags= */ 0);
        إذا (!d)
                أرجع -errno;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        أرجع 0;
}

صحيح chase_and_statat(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags, حرف **ret_path, بنية stat *ret_stat) {
        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        أكد(ret_stat);

        إذا (اکر_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                أرجع RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(اکر_fd, dir_fd, path, chase_flags, ret_path ? &p : عدم, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        إذا (fstat(path_fd, ret_stat) < 0)
                أرجع -errno;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_accessat(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags, صحيح access_mode, حرف **ret_path) {
        _cleanup_close_ صحيح path_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        إذا (اکر_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                أرجع RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(اکر_fd, dir_fd, path, chase_flags, ret_path ? &p : عدم, &path_fd);
        إذا (r < 0)
                أرجع r;
        أكد(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_fopenat_unlocked(
                صحيح اکر_fd,
                صحيح dir_fd,
                ثابت حرف *path,
                ChaseFlags chase_flags,
                ثابت حرف *open_flags,
                حرف **ret_path,
                FILE **ret_file) {

        _cleanup_free_ حرف *final_path = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح mode_flags, r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        أكد(open_flags);
        أكد(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        إذا (mode_flags < 0)
                أرجع mode_flags;

        fd = chase_and_openat(اکر_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : عدم);
        إذا (fd < 0)
                أرجع fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        إذا (r < 0)
                أرجع r;

        إذا (ret_path)
                *ret_path = TAKE_PTR(final_path);

        أرجع 0;
}

صحيح chase_and_unlinkat(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags, صحيح unlink_flags, حرف **ret_path) {
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(path);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(اکر_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        إذا (fd < 0)
                أرجع fd;

        r = path_extract_filename(p, &fname);
        إذا (r < 0)
                أرجع r;

        إذا (unlinkat(fd, fname, unlink_flags) < 0)
                أرجع -errno;

        إذا (ret_path)
                *ret_path = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_open_parent_at(صحيح اکر_fd, صحيح dir_fd, ثابت حرف *path, ChaseFlags chase_flags, حرف **ret_filename) {
        صحيح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(اکر_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        إذا (r < 0)
                أرجع r;

        أرجع pfd;
}
