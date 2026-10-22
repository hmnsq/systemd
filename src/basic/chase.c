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
#ವ್ಯಾಖ್ಯಾನಿಸು CHASE_NO_SHORTCUT_ಮುಖವಾಡ                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_ಸುರಕ್ಷಿತ |                                   \
         CHASE_ಹೆಜ್ಜೆ |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_ಪೋಷಕ |                                 \
         CHASE_MKDIR_0755)

#ವ್ಯಾಖ್ಯಾನಿಸು CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ chase_statx(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ statx *ret) {
        ಹಿಂತಿರುಗಿ xstatx_ತುಂಬಿದ(fd,
                        /* path= */ ಖಾಲಿ,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_ಉತ್ತಮ,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ chase_openat2(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ಸ್ಥಿರ bool can_openat2 = true;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        ಆದರೆ (!can_openat2)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        ಆದರೆ ((chase_flags & (CHASE_NO_SHORTCUT_ಮುಖವಾಡ & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_TRIGGER_AUTOFS))
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        ಆದರೆ (ಬೇರು_fd != XAT_FDROOT && ಬೇರು_fd != dir_fd)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ dir_fd_local = -EBADF;
        ಆದರೆ (dir_fd == XAT_FDROOT) {
                ಆದರೆ (ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                        dir_fd = AT_FDCWD;
                ಇಲ್ಲದಿದ್ದರೆ {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_ಮಾರ್ಗ);
                        ಆದರೆ (dir_fd_local < 0)
                                ಹಿಂತಿರುಗಿ -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ರಚನೆ open_how how = {
                .flags = O_ಮಾರ್ಗ|O_CLOEXEC,
        };
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        ಆದರೆ (ಬೇರು_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_ಒಳ_ಬೇರು;
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = openat2(dir_fd, ಮಾರ್ಗ, &how, ಗಾತ್ರ(how));
        ಆದರೆ (fd < 0) {
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
                ಆದರೆ (errno == ENOSYS)
                        can_openat2 = false;
                ಆದರೆ (ಒಳ_ಗಣ(errno, ENOSYS, EPERM, EAGAIN))
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
                ಹಿಂತಿರುಗಿ -errno;
        }

        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ chase_xopenat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags, ಪೂರ್ಣಾಂಕ open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_ಗಣ(open_flags, O_ಮಾರ್ಗ))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        ಹಿಂತಿರುಗಿ xopenat_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ, open_flags, xopen_flags, ಬಹುಲಕ_INVALID);
}

ಸ್ಥಿರ bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        ಆದರೆ (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ a != b; /* Otherwise we need to stay within the same UID */
}

ಪೂರ್ಣಾಂಕ statx_unsafe_transition(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *a, ಸ್ಥಿರಾಂಕ ರಚನೆ statx *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಆದರೆ (!FLAGS_ಗಣ(a->stx_ಮುಖವಾಡ, STATX_UID) || !FLAGS_ಗಣ(b->stx_ಮುಖವಾಡ, STATX_UID))
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಹಿಂತಿರುಗಿ uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ uid_unsafe_transition(a->st_uid, b->st_uid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_unsafe_transition(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags flags) {
        _cleanup_free_ ಅಕ್ಷರ *n1 = ಖಾಲಿ, *n2 = ಖಾಲಿ, *user_a = ಖಾಲಿ, *user_b = ಖಾಲಿ;
        ರಚನೆ stat st;

        ಆದರೆ (!FLAGS_ಗಣ(flags, CHASE_WARN))
                ಹಿಂತಿರುಗಿ -ENOLINK;

        (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(a, &n1);
        (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(b, &n2);

        ಆದರೆ (fstat(a, &st) == 0)
                user_a = uid_ಗಮ್ಯ_name(st.st_uid);
        ಆದರೆ (fstat(b, &st) == 0)
                user_b = uid_ಗಮ್ಯ_name(st.st_uid);

        ಹಿಂತಿರುಗಿ log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_ಬಲ), strna(n2), strna(user_b), ಮಾರ್ಗ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_autofs_mount_ಬಿಂದು(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags flags) {
        _cleanup_free_ ಅಕ್ಷರ *n1 = ಖಾಲಿ;

        ಆದರೆ (!FLAGS_ಗಣ(flags, CHASE_WARN))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(fd, &n1);

        ಹಿಂತಿರುಗಿ log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), ಮಾರ್ಗ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ log_prohibited_symlink(ಪೂರ್ಣಾಂಕ fd, ChaseFlags flags) {
        _cleanup_free_ ಅಕ್ಷರ *n1 = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (!FLAGS_ಗಣ(flags, CHASE_WARN))
                ಹಿಂತಿರುಗಿ -ELOOP;

        (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(fd, &n1);

        ಹಿಂತಿರುಗಿ log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

ಪೂರ್ಣಾಂಕ chaseat(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, CHASE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ|CHASE_ಹೊರತೆಗೆ_FILENAME));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(ಬೇರು_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        ಪ್ರತಿಜ್ಞಾ(dir_fd != AT_FDCWD || ಒಳ_ಗಣ(ಬೇರು_fd, AT_FDCWD, XAT_FDROOT));

        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ))
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
        ಆದರೆ (ಬೇರು_fd == AT_FDCWD)
                ಬೇರು_fd = XAT_FDROOT;
        ಇಲ್ಲದಿದ್ದರೆ {
                r = dir_fd_is_ಬೇರು(ಬೇರು_fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        ಬೇರು_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_ಬೇರು(dir_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r > 0 && ಬೇರು_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        ಆದರೆ (dir_fd == XAT_FDROOT || ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                dir_fd = ಬೇರು_fd;

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಮಾರ್ಗ = ".";

        bool append_trail_slash = false;
        ಆದರೆ (ENDSWITH_ಗಣ(ಮಾರ್ಗ, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dot_or_dot_dot(ಮಾರ್ಗ) || endswith(ಮಾರ್ಗ, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಪೋಷಕ))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_ಗಣ(flags, CHASE_MUST_BE_REGULAR) + FLAGS_ಗಣ(flags, CHASE_MUST_BE_SOCKET) > 1)
                ಹಿಂತಿರುಗಿ -EBADSLT;

        ಆದರೆ (!ret_ಮಾರ್ಗ) {
                r = chase_openat2(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, flags);
                ಆದರೆ (r >= 0) {
                        ಆದರೆ (ret_fd)
                                *ret_fd = r;
                        ಇಲ್ಲದಿದ್ದರೆ
                                ಸುರಕ್ಷಿತ_close(r);

                        ಹಿಂತಿರುಗಿ 1;
                }
                ಆದರೆ (r != -EOPNOTSUPP)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ಬೇರು_fd == XAT_FDROOT && !ret_ಮಾರ್ಗ && (flags & CHASE_NO_SHORTCUT_ಮುಖವಾಡ) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, ಮಾರ್ಗ, flags, O_ಮಾರ್ಗ|O_CLOEXEC, /* xopen_flags= */ 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ret_fd)
                        *ret_fd = r;
                ಇಲ್ಲದಿದ್ದರೆ
                        ಸುರಕ್ಷಿತ_close(r);

                ಹಿಂತಿರುಗಿ 1;
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
        bool need_absolute = (ಬೇರು_fd == XAT_FDROOT || dir_fd != ಬೇರು_fd) && (dir_fd == XAT_FDROOT || ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ));

        _cleanup_free_ ಅಕ್ಷರ *ಮುಗಿದ = ಖಾಲಿ;
        ಆದರೆ (need_absolute) {
                ಮುಗಿದ = strdup("/");
                ಆದರೆ (!ಮುಗಿದ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = xopenat(dir_fd, ಖಾಲಿ, O_CLOEXEC|O_DIRECTORY|O_ಮಾರ್ಗ);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ರಚನೆ statx stx;
        r = chase_statx(fd, &stx);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ರಚನೆ statx ಬೇರು_stx;
        ಆದರೆ (ಬೇರು_fd != XAT_FDROOT) {
                ಆದರೆ (ಬೇರು_fd == dir_fd)
                        ಬೇರು_stx = stx;
                ಇಲ್ಲದಿದ್ದರೆ {
                        r = chase_statx(ಬೇರು_fd, &ಬೇರು_stx);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }
        }

        _cleanup_free_ ಅಕ್ಷರ *ಬಫರ್ = strdup(ಮಾರ್ಗ);
        ಆದರೆ (!ಬಫರ್)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *todo = ಬಫರ್;
        bool exists = true;
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n_ಹೆಜ್ಜೆಗಳು = 0;; n_ಹೆಜ್ಜೆಗಳು++) {
                _cleanup_free_ ಅಕ್ಷರ *ಮೊದಲ = ಖಾಲಿ;
                _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಗು = -EBADF;
                ರಚನೆ statx stx_ಮಗು;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                ಆದರೆ (n_ಹೆಜ್ಜೆಗಳು > CHASE_MAX)
                        ಹಿಂತಿರುಗಿ -ELOOP;

                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&todo, /* accept_dot_dot= */ true, &e);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0) /* We reached the end. */
                        ನಿಲ್ಲಿಸು;

                ಮೊದಲ = strndup(e, r);
                ಆದರೆ (!ಮೊದಲ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                ಆದರೆ (streq(ಮೊದಲ, "..")) {
                        _cleanup_free_ ಅಕ್ಷರ *ಪೋಷಕ = ಖಾಲಿ;
                        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_ಪೋಷಕ = -EBADF;
                        ರಚನೆ statx stx_ಪೋಷಕ;

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
                        ಆದರೆ (ಬೇರು_fd != XAT_FDROOT) {
                                bool is_ಬೇರು = ಬೇರು_fd == dir_fd && ಖಾಲಿಗಳು_or_ಬೇರು(ಮುಗಿದ);
                                ಆದರೆ (!is_ಬೇರು && statx_inode_same(&stx, &ಬೇರು_stx)) {
                                        r = statx_mount_same(&stx, &ಬೇರು_stx);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ r;

                                        is_ಬೇರು = r > 0;
                                }
                                ಆದರೆ (is_ಬೇರು) {
                                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ))
                                                ಹೋಗು chased_one;
                                        ಮುಂದುವರಿ;
                                }
                        }

                        fd_ಪೋಷಕ = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_ಮಾರ್ಗ|O_DIRECTORY);
                        ಆದರೆ (fd_ಪೋಷಕ < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        r = chase_statx(fd_ಪೋಷಕ, &stx_ಪೋಷಕ);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        ಆದರೆ (statx_inode_same(&stx_ಪೋಷಕ, &stx)) {
                                r = dir_fd_is_ಬೇರು(fd);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                                ಆದರೆ (r > 0) {
                                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ))
                                                ಹೋಗು chased_one;
                                        ಮುಂದುವರಿ;
                                }
                        }

                        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಮುಗಿದ, &ಪೋಷಕ);
                        ಆದರೆ (r >= 0) {
                                ಪ್ರತಿಜ್ಞಾ(!need_absolute || ಮಾರ್ಗ_is_absolute(ಪೋಷಕ));
                                free_and_replace(ಮುಗಿದ, ಪೋಷಕ);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                ಪ್ರತಿಜ್ಞಾ(!need_absolute);
                                ಮುಗಿದ = mfree(ಮುಗಿದ);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                ಪ್ರತಿಜ್ಞಾ(!need_absolute);

                                ಆದರೆ (!isempty(ಮುಗಿದ) && !ಮಾರ್ಗ_is_valid(ಮುಗಿದ))
                                        ಹಿಂತಿರುಗಿ -EINVAL;

                                ಆದರೆ (!ಮಾರ್ಗ_extend(&ಮುಗಿದ, ".."))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                        } ಇಲ್ಲದಿದ್ದರೆ
                                ಹಿಂತಿರುಗಿ r;

                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ))
                                ಹೋಗು chased_one;

                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಸುರಕ್ಷಿತ)) {
                                r = statx_unsafe_transition(&stx, &stx_ಪೋಷಕ);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                                ಆದರೆ (r > 0)
                                        ಹಿಂತಿರುಗಿ log_unsafe_transition(fd, fd_ಪೋಷಕ, ಮಾರ್ಗ, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಪೋಷಕ) && isempty(todo)) {
                                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_grandparent = -EBADF;
                                ರಚನೆ statx stx_grandparent;

                                fd_grandparent = openat(fd_ಪೋಷಕ, "..", O_CLOEXEC|O_NOFOLLOW|O_ಮಾರ್ಗ|O_DIRECTORY);
                                ಆದರೆ (fd_grandparent < 0)
                                        ಹಿಂತಿರುಗಿ -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಸುರಕ್ಷಿತ)) {
                                        r = statx_unsafe_transition(&stx_ಪೋಷಕ, &stx_grandparent);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ r;
                                        ಆದರೆ (r > 0)
                                                ಹಿಂತಿರುಗಿ log_unsafe_transition(fd_ಪೋಷಕ, fd_grandparent, ಮಾರ್ಗ, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                ನಿಲ್ಲಿಸು;
                        }

                        /* update fd and stat */
                        stx = stx_ಪೋಷಕ;
                        close_and_replace(fd, fd_ಪೋಷಕ);
                        ಮುಂದುವರಿ;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                ಮಗು = r = xopenat_ತುಂಬಿದ(fd, ಮೊದಲ,
                                         O_ಮಾರ್ಗ|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_ಗಣ(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         ಬಹುಲಕ_INVALID);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (r != -ENOENT)
                                ಹಿಂತಿರುಗಿ r;

                        ಆದರೆ (!isempty(todo) && !ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                ಹಿಂತಿರುಗಿ r;

                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_ಪೋಷಕ|CHASE_NONEXISTENT)))) {
                                ಮಗು = xopenat(fd,
                                                ಮೊದಲ,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_ಮಾರ್ಗ|O_CLOEXEC);
                                ಆದರೆ (ಮಗು < 0)
                                        ಹಿಂತಿರುಗಿ ಮಗು;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಪೋಷಕ) && isempty(todo)) {
                                ಆದರೆ (!ಮಾರ್ಗ_extend(&ಮುಗಿದ, ಮೊದಲ))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ನಿಲ್ಲಿಸು;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_NONEXISTENT)) {
                                ಆದರೆ (!ಮಾರ್ಗ_extend(&ಮುಗಿದ, ಮೊದಲ, todo))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                exists = false;
                                ನಿಲ್ಲಿಸು;
                        } ಇಲ್ಲದಿದ್ದರೆ
                                ಹಿಂತಿರುಗಿ r;
                }

                r = chase_statx(ಮಗು, &stx_ಮಗು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಸುರಕ್ಷಿತ)) {
                        r = statx_unsafe_transition(&stx, &stx_ಮಗು);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r > 0)
                                ಹಿಂತಿರುಗಿ log_unsafe_transition(fd, ಮಗು, ಮಾರ್ಗ, flags);
                }

                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(ಮಗು, AUTOFS_SUPER_MAGIC) > 0)
                        ಹಿಂತಿರುಗಿ log_autofs_mount_ಬಿಂದು(ಮಗು, ಮಾರ್ಗ, flags);

                ಆದರೆ (S_ISLNK(stx_ಮಗು.stx_ಬಹುಲಕ) && !(FLAGS_ಗಣ(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ ಅಕ್ಷರ *destination = ಖಾಲಿ;

                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_PROHIBIT_SYMLINKS))
                                ಹಿಂತಿರುಗಿ log_prohibited_symlink(ಮಗು, flags);

                        r = readlinkat_malloc(fd, ಮೊದಲ, &destination);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (isempty(destination))
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಆದರೆ (ಮಾರ್ಗ_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                ಸುರಕ್ಷಿತ_close(fd);
                                fd = fd_reopen(ಬೇರು_fd, O_CLOEXEC|O_ಮಾರ್ಗ|O_DIRECTORY);
                                ಆದರೆ (fd < 0)
                                        ಹಿಂತಿರುಗಿ fd;

                                r = chase_statx(fd, &stx);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಸುರಕ್ಷಿತ)) {
                                        r = statx_unsafe_transition(&stx_ಮಗು, &stx);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ r;
                                        ಆದರೆ (r > 0)
                                                ಹಿಂತಿರುಗಿ log_unsafe_transition(ಮಗು, fd, ಮಾರ್ಗ, flags);
                                }

                                ಆದರೆ (dir_fd != ಬೇರು_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&ಮುಗಿದ, need_absolute ? "/" : ಖಾಲಿ);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        ಆದರೆ (!ಮಾರ್ಗ_extend(&destination, todo))
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        free_and_replace(ಬಫರ್, destination);
                        todo = ಬಫರ್;

                        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ))
                                ಹೋಗು chased_one;

                        ಮುಂದುವರಿ;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                ಆದರೆ (!ಮಾರ್ಗ_extend(&ಮುಗಿದ, ಮೊದಲ))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಪೋಷಕ) && isempty(todo))
                        ನಿಲ್ಲಿಸು;

                /* And iterate again, but go one directory further down. */
                stx = stx_ಮಗು;
                close_and_replace(fd, ಮಗು);
        }

        ಆದರೆ (exists) {
                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }
        }

        ಆದರೆ (ret_ಮಾರ್ಗ) {
                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೊರತೆಗೆ_FILENAME) && ಮುಗಿದ) {
                        _cleanup_free_ ಅಕ್ಷರ *f = ಖಾಲಿ;

                        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮುಗಿದ, &f);
                        ಆದರೆ (r < 0 && r != -EADDRNOTAVAIL)
                                ಹಿಂತಿರುಗಿ r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(ಮುಗಿದ, f);
                }

                ಆದರೆ (!ಮುಗಿದ) {
                        ಪ್ರತಿಜ್ಞಾ(!need_absolute || FLAGS_ಗಣ(flags, CHASE_ಹೊರತೆಗೆ_FILENAME));
                        ಮುಗಿದ = strdup(".");
                        ಆದರೆ (!ಮುಗಿದ)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                ಆದರೆ (append_trail_slash)
                        ಆದರೆ (!strextend(&ಮುಗಿದ, "/"))
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                *ret_ಮಾರ್ಗ = TAKE_PTR(ಮುಗಿದ);
        }

        ಆದರೆ (ret_fd) {
                ಆದರೆ (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } ಇಲ್ಲದಿದ್ದರೆ
                        *ret_fd = -EBADF;
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಹೆಜ್ಜೆ))
                ಹಿಂತಿರುಗಿ 1;

        ಹಿಂತಿರುಗಿ exists;

chased_one:
        ಆದರೆ (ret_ಮಾರ್ಗ) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                ಆದರೆ (!ಮುಗಿದ) {
                        ಪ್ರತಿಜ್ಞಾ(!need_absolute);
                        ಮುಗಿದ = strdup(append_trail_slash ? "./" : ".");
                        ಆದರೆ (!ಮುಗಿದ)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&todo, /* accept_dot_dot= */ true, &e);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        *ret_ಮಾರ್ಗ = TAKE_PTR(ಮುಗಿದ);
                ಇಲ್ಲದಿದ್ದರೆ {
                        ಅಕ್ಷರ *c;

                        c = ಮಾರ್ಗ_join(ಮುಗಿದ, e);
                        ಆದರೆ (!c)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        *ret_ಮಾರ್ಗ = c;
                }
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ChaseFlags flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd) {
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ, *absolute = ಖಾಲಿ, *p = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF, pfd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = ಖಾಲಿಗಳು_or_ಬೇರು_harder_ಗಮ್ಯ_null(&ಬೇರು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* A root directory of "/" or "" is identical to "/". */
        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು))
                ಬೇರು = "/";
        ಇಲ್ಲದಿದ್ದರೆ {
                r = ಮಾರ್ಗ_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                ಬೇರು = ಮಾರ್ಗ_simplify(ಬೇರು_abs);

                ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_is_absolute(ಬೇರು));
                ಪ್ರತಿಜ್ಞಾ(!ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು));

                ಆದರೆ (FLAGS_ಗಣ(flags, CHASE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು)) {
                        absolute = ಮಾರ್ಗ_join(ಬೇರು, ಮಾರ್ಗ);
                        ಆದರೆ (!absolute)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }
        }

        ಆದರೆ (!absolute) {
                r = ಮಾರ್ಗ_make_absolute_cwd(ಮಾರ್ಗ, &absolute);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಮಾರ್ಗ = ಮಾರ್ಗ_startswith(absolute, ಬೇರು);
        ಆದರೆ (!ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(FLAGS_ಗಣ(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, ಬೇರು);

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು))
                fd = XAT_FDROOT;
        ಇಲ್ಲದಿದ್ದರೆ {
                fd = open(ಬೇರು, O_CLOEXEC|O_DIRECTORY|O_ಮಾರ್ಗ);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        r = chaseat(fd, fd, ಮಾರ್ಗ, flags & ~CHASE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, ret_fd ? &pfd : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ) {
                ಆದರೆ (!FLAGS_ಗಣ(flags, CHASE_ಹೊರತೆಗೆ_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು))
                                ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_is_absolute(p));
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಅಕ್ಷರ *q;

                                ಪ್ರತಿಜ್ಞಾ(!ಮಾರ್ಗ_is_absolute(p));

                                q = ಮಾರ್ಗ_join(ಬೇರು, p + STR_ಒಳ_ಗಣ(p, ".", "./"));
                                ಆದರೆ (!q)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_ಮಾರ್ಗ = TAKE_PTR(p);
        }

        ಆದರೆ (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *q;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ)) {
                _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;

                r = ಖಾಲಿಗಳು_or_ಬೇರು_harder_ಗಮ್ಯ_null(&ಬೇರು);
                ಆದರೆ (r < 0 && r != -ENOENT)
                        ಹಿಂತಿರುಗಿ r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                r = ಮಾರ್ಗ_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಬೇರು = ಮಾರ್ಗ_simplify(ಬೇರು_abs);

                q = ಮಾರ್ಗ_join(ಬೇರು, ಮಾರ್ಗ + (ಮಾರ್ಗ[0] == '.' && ಒಳ_ಗಣ(ಮಾರ್ಗ[1], '/', '\0')));
        } ಇಲ್ಲದಿದ್ದರೆ
                q = strdup(ಮಾರ್ಗ);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = q;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_ಹೊರತೆಗೆ_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = ಖಾಲಿಗಳು_or_ಬೇರು_harder_ಗಮ್ಯ_null(&ಬೇರು);
        ಆದರೆ (r < 0 && r != -ENOENT)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು)) {
                _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;

                r = ಮಾರ್ಗ_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಮಾರ್ಗ = ಮಾರ್ಗ_startswith(ಮಾರ್ಗ, ಬೇರು_abs);
                ಆದರೆ (!ಮಾರ್ಗ)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (!isempty(ಮಾರ್ಗ)) {
                r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, ret);
                ಆದರೆ (r != -EADDRNOTAVAIL)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, ".");
}

ಪೂರ್ಣಾಂಕ chase_and_open(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ChaseFlags chase_flags,
                ಪೂರ್ಣಾಂಕ open_flags,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *fname = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *open_name = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು) && !ret_ಮಾರ್ಗ && (chase_flags & CHASE_NO_SHORTCUT_ಮುಖವಾಡ) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ಹಿಂತಿರುಗಿ chase_xopenat(AT_FDCWD, ಮಾರ್ಗ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(ಮಾರ್ಗ, ಬೇರು, (CHASE_ಪೋಷಕ|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        ಆದರೆ (!FLAGS_ಗಣ(chase_flags, CHASE_ಪೋಷಕ)) {
                ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_ಹೊರತೆಗೆ_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ಇಲ್ಲದಿದ್ದರೆ {
                        r = chase_ಹೊರತೆಗೆ_filename(p, ಬೇರು, &fname);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(ಮಾರ್ಗ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ chase_and_opendir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ChaseFlags chase_flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, DIR **ret_dir) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        DIR *d;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ಪ್ರತಿಜ್ಞಾ(ret_dir);

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು) && !ret_ಮಾರ್ಗ && (chase_flags & CHASE_NO_SHORTCUT_ಮುಖವಾಡ) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(ಮಾರ್ಗ);
                ಆದರೆ (!d)
                        ಹಿಂತಿರುಗಿ -errno;

                *ret_dir = d;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = chase(ಮಾರ್ಗ, ಬೇರು, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        d = xopendirat(ಮಾರ್ಗ_fd, /* path= */ ಖಾಲಿ, /* flags= */ 0);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        *ret_dir = d;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_stat(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ChaseFlags chase_flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ರಚನೆ stat *ret_stat) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));
        ಪ್ರತಿಜ್ಞಾ(ret_stat);

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು) && !ret_ಮಾರ್ಗ && (chase_flags & (CHASE_NO_SHORTCUT_ಮುಖವಾಡ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ಹಿಂತಿರುಗಿ RET_NERRNO(fstatat(AT_FDCWD, ಮಾರ್ಗ, ret_stat,
                                          FLAGS_ಗಣ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(ಮಾರ್ಗ, ಬೇರು, chase_flags, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        ಆದರೆ (fstat(ಮಾರ್ಗ_fd, ret_stat) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_access(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಂಕ access_ಬಹುಲಕ, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು) && !ret_ಮಾರ್ಗ && (chase_flags & (CHASE_NO_SHORTCUT_ಮುಖವಾಡ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ಹಿಂತಿರುಗಿ RET_NERRNO(faccessat(AT_FDCWD, ಮಾರ್ಗ, access_ಬಹುಲಕ,
                                            FLAGS_ಗಣ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(ಮಾರ್ಗ, ಬೇರು, chase_flags, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        r = access_fd(ಮಾರ್ಗ_fd, access_ಬಹುಲಕ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_fopen_unlocked(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ChaseFlags chase_flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *open_flags,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ,
                FILE **ret_file) {

        _cleanup_free_ ಅಕ್ಷರ *final_ಮಾರ್ಗ = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_flags, r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_ಪೋಷಕ|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        ಪ್ರತಿಜ್ಞಾ(open_flags);
        ಪ್ರತಿಜ್ಞಾ(ret_file);

        ಬಹುಲಕ_flags = fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(open_flags);
        ಆದರೆ (ಬಹುಲಕ_flags < 0)
                ಹಿಂತಿರುಗಿ ಬಹುಲಕ_flags;

        fd = chase_and_open(ಮಾರ್ಗ, ಬೇರು, chase_flags, ಬಹುಲಕ_flags, ret_ಮಾರ್ಗ ? &final_ಮಾರ್ಗ : ಖಾಲಿ);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(final_ಮಾರ್ಗ);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_unlink(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಂಕ unlink_flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *fname = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_ಪೋಷಕ|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_ಹೊರತೆಗೆ_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(ಮಾರ್ಗ, ಬೇರು, chase_flags|CHASE_ಪೋಷಕ|CHASE_NOFOLLOW, O_ಮಾರ್ಗ|O_DIRECTORY|O_CLOEXEC, &p);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(p, &fname);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (unlinkat(fd, fname, unlink_flags) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_open_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ChaseFlags chase_flags, ಅಕ್ಷರ **ret_filename) {
        ಪೂರ್ಣಾಂಕ pfd, r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));

        r = chase(ಮಾರ್ಗ, ಬೇರು, CHASE_ಪೋಷಕ|CHASE_ಹೊರತೆಗೆ_FILENAME|chase_flags, ret_filename, &pfd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pfd;
}

ಪೂರ್ಣಾಂಕ chase_and_openat(
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ChaseFlags chase_flags,
                ಪೂರ್ಣಾಂಕ open_flags,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *fname = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *open_name = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_MUST_BE_SOCKET)));

        ಆದರೆ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ಮಾರ್ಗ && (chase_flags & CHASE_NO_SHORTCUT_ಮುಖವಾಡ) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ಹಿಂತಿರುಗಿ chase_xopenat(dir_fd, ಮಾರ್ಗ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, (chase_flags|CHASE_ಪೋಷಕ)&~CHASE_MUST_BE_REGULAR, &p, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!FLAGS_ಗಣ(chase_flags, CHASE_ಪೋಷಕ)) {
                ಆದರೆ (FLAGS_ಗಣ(chase_flags, CHASE_ಹೊರತೆಗೆ_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ಇಲ್ಲದಿದ್ದರೆ {
                        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(p, &fname);
                        ಆದರೆ (r < 0 && r != -EADDRNOTAVAIL)
                                ಹಿಂತಿರುಗಿ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(ಮಾರ್ಗ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ chase_and_opendirat(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, DIR **ret_dir) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        DIR *d;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ಪ್ರತಿಜ್ಞಾ(ret_dir);

        ಆದರೆ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ಮಾರ್ಗ && (chase_flags & CHASE_NO_SHORTCUT_ಮುಖವಾಡ) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(ಮಾರ್ಗ);
                ಆದರೆ (!d)
                        ಹಿಂತಿರುಗಿ -errno;

                *ret_dir = d;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = chaseat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, chase_flags, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        d = xopendirat(ಮಾರ್ಗ_fd, /* path= */ ಖಾಲಿ, /* flags= */ 0);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        *ret_dir = d;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_statat(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ರಚನೆ stat *ret_stat) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));
        ಪ್ರತಿಜ್ಞಾ(ret_stat);

        ಆದರೆ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ಮಾರ್ಗ && (chase_flags & (CHASE_NO_SHORTCUT_ಮುಖವಾಡ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ಹಿಂತಿರುಗಿ RET_NERRNO(fstatat(AT_FDCWD, ಮಾರ್ಗ, ret_stat,
                                          FLAGS_ಗಣ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, chase_flags, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        ಆದರೆ (fstat(ಮಾರ್ಗ_fd, ret_stat) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_accessat(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags, ಪೂರ್ಣಾಂಕ access_ಬಹುಲಕ, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));

        ಆದರೆ (ಬೇರು_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ಮಾರ್ಗ && (chase_flags & (CHASE_NO_SHORTCUT_ಮುಖವಾಡ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ಹಿಂತಿರುಗಿ RET_NERRNO(faccessat(AT_FDCWD, ಮಾರ್ಗ, access_ಬಹುಲಕ,
                                            FLAGS_ಗಣ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, chase_flags, ret_ಮಾರ್ಗ ? &p : ಖಾಲಿ, &ಮಾರ್ಗ_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ_fd >= 0);

        r = access_fd(ಮಾರ್ಗ_fd, access_ಬಹುಲಕ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_fopenat_unlocked(
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ChaseFlags chase_flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *open_flags,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ,
                FILE **ret_file) {

        _cleanup_free_ ಅಕ್ಷರ *final_ಮಾರ್ಗ = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_flags, r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_ಪೋಷಕ)));
        ಪ್ರತಿಜ್ಞಾ(open_flags);
        ಪ್ರತಿಜ್ಞಾ(ret_file);

        ಬಹುಲಕ_flags = fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(open_flags);
        ಆದರೆ (ಬಹುಲಕ_flags < 0)
                ಹಿಂತಿರುಗಿ ಬಹುಲಕ_flags;

        fd = chase_and_openat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, chase_flags, ಬಹುಲಕ_flags, ret_ಮಾರ್ಗ ? &final_ಮಾರ್ಗ : ಖಾಲಿ);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(final_ಮಾರ್ಗ);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_unlinkat(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags, ಪೂರ್ಣಾಂಕ unlink_flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ, *fname = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ|CHASE_ಪೋಷಕ|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_ಹೊರತೆಗೆ_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, chase_flags|CHASE_ಪೋಷಕ|CHASE_NOFOLLOW, O_ಮಾರ್ಗ|O_DIRECTORY|O_CLOEXEC, &p);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(p, &fname);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (unlinkat(fd, fname, unlink_flags) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ chase_and_open_ಪೋಷಕ_at(ಪೂರ್ಣಾಂಕ ಬೇರು_fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ChaseFlags chase_flags, ಅಕ್ಷರ **ret_filename) {
        ಪೂರ್ಣಾಂಕ pfd, r;

        ಪ್ರತಿಜ್ಞಾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ಹೆಜ್ಜೆ)));

        r = chaseat(ಬೇರು_fd, dir_fd, ಮಾರ್ಗ, CHASE_ಪೋಷಕ|CHASE_ಹೊರತೆಗೆ_FILENAME|chase_flags, ret_filename, &pfd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pfd;
}
