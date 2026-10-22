/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "glyph-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ chase_statx(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ statx *ret) {
        ಪ್ರತ್ಯಾಗಮ xstatx_full(fd,
                        /* path= */ ರಿಕ್ತ,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ chase_openat2(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ಸ್ಥಿರ bool can_openat2 = true;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        ಯದಿ (!can_openat2)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        ಯದಿ ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        ಯದಿ (ಬೇರು_fd != XAT_FDROOT && ಬೇರು_fd != dir_fd)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ dir_fd_local = -EBADF;
        ಯದಿ (dir_fd == XAT_FDROOT) {
                ಯದಿ (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                ಅನ್ಯಥಾ {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        ಯದಿ (dir_fd_local < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ಸಂರಚನಾ open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        ಯದಿ (ಬೇರು_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_ಬೇರು;
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = openat2(dir_fd, path, &how, ಪರಿಮಾಣ(how));
        ಯದಿ (fd < 0) {
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
                ಯದಿ (errno == ENOSYS)
                        can_openat2 = false;
                ಯದಿ (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
                ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಯದಿ (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ chase_xopenat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        ಯದಿ (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        ಯದಿ (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        ಪ್ರತ್ಯಾಗಮ xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

ಸ್ಥಿರ bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        ಯದಿ (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ a != b; /* Otherwise we need to stay within the same UID */
}

ಪೂರ್ಣಾಙ್ಕ statx_unsafe_transition(ಧ್ರುವ ಸಂರಚನಾ statx *a, ಧ್ರುವ ಸಂರಚನಾ statx *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಯದಿ (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಪ್ರತ್ಯಾಗಮ uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ uid_unsafe_transition(a->st_uid, b->st_uid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_unsafe_transition(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags flags) {
        _cleanup_free_ ವರ್ಣ *n1 = ರಿಕ್ತ, *n2 = ರಿಕ್ತ, *user_a = ರಿಕ್ತ, *user_b = ರಿಕ್ತ;
        ಸಂರಚನಾ stat st;

        ಯದಿ (!FLAGS_SET(flags, CHASE_WARN))
                ಪ್ರತ್ಯಾಗಮ -ENOLINK;

        (ಶೂನ್ಯ) fd_get_path(a, &n1);
        (ಶೂನ್ಯ) fd_get_path(b, &n2);

        ಯದಿ (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        ಯದಿ (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        ಪ್ರತ್ಯಾಗಮ log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_ಬಲ), strna(n2), strna(user_b), path);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_autofs_mount_point(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags flags) {
        _cleanup_free_ ವರ್ಣ *n1 = ರಿಕ್ತ;

        ಯದಿ (!FLAGS_SET(flags, CHASE_WARN))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE;

        (ಶೂನ್ಯ) fd_get_path(fd, &n1);

        ಪ್ರತ್ಯಾಗಮ log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ log_prohibited_symlink(ಪೂರ್ಣಾಙ್ಕ fd, ChaseFlags flags) {
        _cleanup_free_ ವರ್ಣ *n1 = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (!FLAGS_SET(flags, CHASE_WARN))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        (ಶೂನ್ಯ) fd_get_path(fd, &n1);

        ಪ್ರತ್ಯಾಗಮ log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

ಪೂರ್ಣಾಙ್ಕ chaseat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags flags, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, CHASE_PREFIX_ಬೇರು));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(ಬೇರು_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        ಪ್ರತಿಜ್ಞಾ(dir_fd != AT_FDCWD || IN_SET(ಬೇರು_fd, AT_FDCWD, XAT_FDROOT));

        ಯದಿ (FLAGS_SET(flags, CHASE_STEP))
                ಪ್ರತಿಜ್ಞಾ(!ret_fd);

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
        ಯದಿ (ಬೇರು_fd == AT_FDCWD)
                ಬೇರು_fd = XAT_FDROOT;
        ಅನ್ಯಥಾ {
                r = dir_fd_is_ಬೇರು(ಬೇರು_fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r > 0)
                        ಬೇರು_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_ಬೇರು(dir_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r > 0 && ಬೇರು_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        ಯದಿ (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = ಬೇರು_fd;

        ಯದಿ (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        ಯದಿ (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                ಯದಿ (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } ಅನ್ಯಥಾ ಯದಿ (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        ಯದಿ (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        ಯದಿ (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                ಪ್ರತ್ಯಾಗಮ -EBADSLT;

        ಯದಿ (!ret_path) {
                r = chase_openat2(ಬೇರು_fd, dir_fd, path, flags);
                ಯದಿ (r >= 0) {
                        ಯದಿ (ret_fd)
                                *ret_fd = r;
                        ಅನ್ಯಥಾ
                                safe_close(r);

                        ಪ್ರತ್ಯಾಗಮ 1;
                }
                ಯದಿ (r != -EOPNOTSUPP)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ಬೇರು_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (ret_fd)
                        *ret_fd = r;
                ಅನ್ಯಥಾ
                        safe_close(r);

                ಪ್ರತ್ಯಾಗಮ 1;
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
        bool need_absolute = (ಬೇರು_fd == XAT_FDROOT || dir_fd != ಬೇರು_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ ವರ್ಣ *done = ರಿಕ್ತ;
        ಯದಿ (need_absolute) {
                done = strdup("/");
                ಯದಿ (!done)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = xopenat(dir_fd, ರಿಕ್ತ, O_CLOEXEC|O_DIRECTORY|O_PATH);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಸಂರಚನಾ statx stx;
        r = chase_statx(fd, &stx);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ಸಂರಚನಾ statx ಬೇರು_stx;
        ಯದಿ (ಬೇರು_fd != XAT_FDROOT) {
                ಯದಿ (ಬೇರು_fd == dir_fd)
                        ಬೇರು_stx = stx;
                ಅನ್ಯಥಾ {
                        r = chase_statx(ಬೇರು_fd, &ಬೇರು_stx);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }
        }

        _cleanup_free_ ವರ್ಣ *buffer = strdup(path);
        ಯದಿ (!buffer)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಧ್ರುವ ವರ್ಣ *todo = buffer;
        bool exists = true;
        ಪ್ರತಿ (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ ವರ್ಣ *first = ರಿಕ್ತ;
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ child = -EBADF;
                ಸಂರಚನಾ statx stx_child;
                ಧ್ರುವ ವರ್ಣ *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                ಯದಿ (n_steps > CHASE_MAX)
                        ಪ್ರತ್ಯಾಗಮ -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0) /* We reached the end. */
                        ಭಙ್ಗ;

                first = strndup(e, r);
                ಯದಿ (!first)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                ಯದಿ (streq(first, "..")) {
                        _cleanup_free_ ವರ್ಣ *parent = ರಿಕ್ತ;
                        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_parent = -EBADF;
                        ಸಂರಚನಾ statx stx_parent;

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
                        ಯದಿ (ಬೇರು_fd != XAT_FDROOT) {
                                bool is_ಬೇರು = ಬೇರು_fd == dir_fd && empty_or_ಬೇರು(done);
                                ಯದಿ (!is_ಬೇರು && statx_inode_same(&stx, &ಬೇರು_stx)) {
                                        r = statx_mount_same(&stx, &ಬೇರು_stx);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        is_ಬೇರು = r > 0;
                                }
                                ಯದಿ (is_ಬೇರು) {
                                        ಯದಿ (FLAGS_SET(flags, CHASE_STEP))
                                                ಗಚ್ಛ chased_one;
                                        ಅನುವರ್ತ;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        ಯದಿ (fd_parent < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        ಯದಿ (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_ಬೇರು(fd);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                                ಯದಿ (r > 0) {
                                        ಯದಿ (FLAGS_SET(flags, CHASE_STEP))
                                                ಗಚ್ಛ chased_one;
                                        ಅನುವರ್ತ;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        ಯದಿ (r >= 0) {
                                ಪ್ರತಿಜ್ಞಾ(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } ಅನ್ಯಥಾ ಯದಿ (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                ಪ್ರತಿಜ್ಞಾ(!need_absolute);
                                done = mfree(done);
                        } ಅನ್ಯಥಾ ಯದಿ (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } ಅನ್ಯಥಾ ಯದಿ (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                ಪ್ರತಿಜ್ಞಾ(!need_absolute);

                                ಯದಿ (!isempty(done) && !path_is_valid(done))
                                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                                ಯದಿ (!path_extend(&done, ".."))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        } ಅನ್ಯಥಾ
                                ಪ್ರತ್ಯಾಗಮ r;

                        ಯದಿ (FLAGS_SET(flags, CHASE_STEP))
                                ಗಚ್ಛ chased_one;

                        ಯದಿ (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                                ಯದಿ (r > 0)
                                        ಪ್ರತ್ಯಾಗಮ log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        ಯದಿ (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_grandparent = -EBADF;
                                ಸಂರಚನಾ statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                ಯದಿ (fd_grandparent < 0)
                                        ಪ್ರತ್ಯಾಗಮ -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ r;
                                        ಯದಿ (r > 0)
                                                ಪ್ರತ್ಯಾಗಮ log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                ಭಙ್ಗ;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        ಅನುವರ್ತ;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                ಯದಿ (r < 0) {
                        ಯದಿ (r != -ENOENT)
                                ಪ್ರತ್ಯಾಗಮ r;

                        ಯದಿ (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                ಪ್ರತ್ಯಾಗಮ r;

                        ಯದಿ (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                ಯದಿ (child < 0)
                                        ಪ್ರತ್ಯಾಗಮ child;
                        } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                ಯದಿ (!path_extend(&done, first))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಭಙ್ಗ;
                        } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                ಯದಿ (!path_extend(&done, first, todo))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                exists = false;
                                ಭಙ್ಗ;
                        } ಅನ್ಯಥಾ
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                r = chase_statx(child, &stx_child);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r > 0)
                                ಪ್ರತ್ಯಾಗಮ log_unsafe_transition(fd, child, path, flags);
                }

                ಯದಿ (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        ಪ್ರತ್ಯಾಗಮ log_autofs_mount_point(child, path, flags);

                ಯದಿ (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ ವರ್ಣ *destination = ರಿಕ್ತ;

                        ಯದಿ (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                ಪ್ರತ್ಯಾಗಮ log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (isempty(destination))
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        ಯದಿ (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(ಬೇರು_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                ಯದಿ (fd < 0)
                                        ಪ್ರತ್ಯಾಗಮ fd;

                                r = chase_statx(fd, &stx);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ r;
                                        ಯದಿ (r > 0)
                                                ಪ್ರತ್ಯಾಗಮ log_unsafe_transition(child, fd, path, flags);
                                }

                                ಯದಿ (dir_fd != ಬೇರು_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : ರಿಕ್ತ);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        ಯದಿ (!path_extend(&destination, todo))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        ಯದಿ (FLAGS_SET(flags, CHASE_STEP))
                                ಗಚ್ಛ chased_one;

                        ಅನುವರ್ತ;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                ಯದಿ (!path_extend(&done, first))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        ಭಙ್ಗ;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        ಯದಿ (exists) {
                ಯದಿ (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }
        }

        ಯದಿ (ret_path) {
                ಯದಿ (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ ವರ್ಣ *f = ರಿಕ್ತ;

                        r = path_extract_filename(done, &f);
                        ಯದಿ (r < 0 && r != -EADDRNOTAVAIL)
                                ಪ್ರತ್ಯಾಗಮ r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                ಯದಿ (!done) {
                        ಪ್ರತಿಜ್ಞಾ(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        ಯದಿ (!done)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                ಯದಿ (append_trail_slash)
                        ಯದಿ (!strextend(&done, "/"))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        ಯದಿ (ret_fd) {
                ಯದಿ (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } ಅನ್ಯಥಾ
                        *ret_fd = -EBADF;
        }

        ಯದಿ (FLAGS_SET(flags, CHASE_STEP))
                ಪ್ರತ್ಯಾಗಮ 1;

        ಪ್ರತ್ಯಾಗಮ exists;

chased_one:
        ಯದಿ (ret_path) {
                ಧ್ರುವ ವರ್ಣ *e;

                ಯದಿ (!done) {
                        ಪ್ರತಿಜ್ಞಾ(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        ಯದಿ (!done)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        *ret_path = TAKE_PTR(done);
                ಅನ್ಯಥಾ {
                        ವರ್ಣ *c;

                        c = path_join(done, e);
                        ಯದಿ (!c)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        *ret_path = c;
                }
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags flags, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd) {
        _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ, *absolute = ರಿಕ್ತ, *p = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF, pfd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = empty_or_ಬೇರು_harder_to_null(&ಬೇರು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* A root directory of "/" or "" is identical to "/". */
        ಯದಿ (empty_or_ಬೇರು(ಬೇರು))
                ಬೇರು = "/";
        ಅನ್ಯಥಾ {
                r = path_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                ಬೇರು = path_simplify(ಬೇರು_abs);

                ಪ್ರತಿಜ್ಞಾ(path_is_absolute(ಬೇರು));
                ಪ್ರತಿಜ್ಞಾ(!empty_or_ಬೇರು(ಬೇರು));

                ಯದಿ (FLAGS_SET(flags, CHASE_PREFIX_ಬೇರು)) {
                        absolute = path_join(ಬೇರು, path);
                        ಯದಿ (!absolute)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }
        }

        ಯದಿ (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        path = path_startswith(absolute, ಬೇರು);
        ಯದಿ (!path)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, ಬೇರು);

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು))
                fd = XAT_FDROOT;
        ಅನ್ಯಥಾ {
                fd = open(ಬೇರು, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_ಬೇರು, ret_path ? &p : ರಿಕ್ತ, ret_fd ? &pfd : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path) {
                ಯದಿ (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        ಯದಿ (empty_or_ಬೇರು(ಬೇರು))
                                ಪ್ರತಿಜ್ಞಾ(path_is_absolute(p));
                        ಅನ್ಯಥಾ {
                                ವರ್ಣ *q;

                                ಪ್ರತಿಜ್ಞಾ(!path_is_absolute(p));

                                q = path_join(ಬೇರು, p + STR_IN_SET(p, ".", "./"));
                                ಯದಿ (!q)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        ಯದಿ (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ chaseat_prefix_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret) {
        ವರ್ಣ *q;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        ಯದಿ (!path_is_absolute(path)) {
                _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;

                r = empty_or_ಬೇರು_harder_to_null(&ಬೇರು);
                ಯದಿ (r < 0 && r != -ENOENT)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                ಯದಿ (empty_or_ಬೇರು(ಬೇರು))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                r = path_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಬೇರು = path_simplify(ಬೇರು_abs);

                q = path_join(ಬೇರು, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } ಅನ್ಯಥಾ
                q = strdup(path);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = q;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_extract_filename(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!path_is_absolute(path))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = empty_or_ಬೇರು_harder_to_null(&ಬೇರು);
        ಯದಿ (r < 0 && r != -ENOENT)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!empty_or_ಬೇರು(ಬೇರು)) {
                _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;

                r = path_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                path = path_startswith(path, ಬೇರು_abs);
                ಯದಿ (!path)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಯದಿ (!isempty(path)) {
                r = path_extract_filename(path, ret);
                ಯದಿ (r != -EADDRNOTAVAIL)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, ".");
}

ಪೂರ್ಣಾಙ್ಕ chase_and_open(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ChaseFlags chase_flags,
                ಪೂರ್ಣಾಙ್ಕ open_flags,
                ವರ್ಣ **ret_path) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *fname = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *open_name = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ಪ್ರತ್ಯಾಗಮ chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, ಬೇರು, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        ಯದಿ (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                ಯದಿ (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ಅನ್ಯಥಾ {
                        r = chase_extract_filename(p, ಬೇರು, &fname);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_opendir(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ವರ್ಣ **ret_path, DIR **ret_dir) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        DIR *d;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ಪ್ರತಿಜ್ಞಾ(ret_dir);

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                ಯದಿ (!d)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                *ret_dir = d;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = chase(path, ಬೇರು, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : ರಿಕ್ತ, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ರಿಕ್ತ, /* flags= */ 0);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_stat(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ವರ್ಣ **ret_path, ಸಂರಚನಾ stat *ret_stat) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        ಪ್ರತಿಜ್ಞಾ(ret_stat);

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, ಬೇರು, chase_flags, ret_path ? &p : ರಿಕ್ತ, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        ಯದಿ (fstat(path_fd, ret_stat) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_access(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ access_mode, ವರ್ಣ **ret_path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, ಬೇರು, chase_flags, ret_path ? &p : ರಿಕ್ತ, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_fopen_unlocked(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ChaseFlags chase_flags,
                ಧ್ರುವ ವರ್ಣ *open_flags,
                ವರ್ಣ **ret_path,
                FILE **ret_file) {

        _cleanup_free_ ವರ್ಣ *final_path = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ mode_flags, r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        ಪ್ರತಿಜ್ಞಾ(open_flags);
        ಪ್ರತಿಜ್ಞಾ(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        ಯದಿ (mode_flags < 0)
                ಪ್ರತ್ಯಾಗಮ mode_flags;

        fd = chase_and_open(path, ಬೇರು, chase_flags, mode_flags, ret_path ? &final_path : ರಿಕ್ತ);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_unlink(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ unlink_flags, ವರ್ಣ **ret_path) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *fname = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, ಬೇರು, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        r = path_extract_filename(p, &fname);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (unlinkat(fd, fname, unlink_flags) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_open_parent(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ವರ್ಣ **ret_filename) {
        ಪೂರ್ಣಾಙ್ಕ pfd, r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, ಬೇರು, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ pfd;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_openat(
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ChaseFlags chase_flags,
                ಪೂರ್ಣಾಙ್ಕ open_flags,
                ವರ್ಣ **ret_path) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *fname = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *open_name = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        ಯದಿ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ಪ್ರತ್ಯಾಗಮ chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(ಬೇರು_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                ಯದಿ (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ಅನ್ಯಥಾ {
                        r = path_extract_filename(p, &fname);
                        ಯದಿ (r < 0 && r != -EADDRNOTAVAIL)
                                ಪ್ರತ್ಯಾಗಮ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_opendirat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ವರ್ಣ **ret_path, DIR **ret_dir) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        DIR *d;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ಪ್ರತಿಜ್ಞಾ(ret_dir);

        ಯದಿ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                ಯದಿ (!d)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                *ret_dir = d;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = chaseat(ಬೇರು_fd, dir_fd, path, chase_flags, ret_path ? &p : ರಿಕ್ತ, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ರಿಕ್ತ, /* flags= */ 0);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_statat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ವರ್ಣ **ret_path, ಸಂರಚನಾ stat *ret_stat) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        ಪ್ರತಿಜ್ಞಾ(ret_stat);

        ಯದಿ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ಬೇರು_fd, dir_fd, path, chase_flags, ret_path ? &p : ರಿಕ್ತ, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        ಯದಿ (fstat(path_fd, ret_stat) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_accessat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ access_mode, ವರ್ಣ **ret_path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ path_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        ಯದಿ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ಬೇರು_fd, dir_fd, path, chase_flags, ret_path ? &p : ರಿಕ್ತ, &path_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_fopenat_unlocked(
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ChaseFlags chase_flags,
                ಧ್ರುವ ವರ್ಣ *open_flags,
                ವರ್ಣ **ret_path,
                FILE **ret_file) {

        _cleanup_free_ ವರ್ಣ *final_path = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ mode_flags, r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        ಪ್ರತಿಜ್ಞಾ(open_flags);
        ಪ್ರತಿಜ್ಞಾ(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        ಯದಿ (mode_flags < 0)
                ಪ್ರತ್ಯಾಗಮ mode_flags;

        fd = chase_and_openat(ಬೇರು_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : ರಿಕ್ತ);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_unlinkat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ unlink_flags, ವರ್ಣ **ret_path) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ, *fname = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(ಬೇರು_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        r = path_extract_filename(p, &fname);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (unlinkat(fd, fname, unlink_flags) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ chase_and_open_parent_at(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ವರ್ಣ **ret_filename) {
        ಪೂರ್ಣಾಙ್ಕ pfd, r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(ಬೇರು_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ pfd;
}
