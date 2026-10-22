/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/magic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glyph-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHASE_NO_SHORTCUT_ਨਕਾਬ                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_ਸੁਰੱਖਿਅਤ |                                   \
         CHASE_ਕਦਮ |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_ਮਾਪਾ |                                 \
         CHASE_MKDIR_0755)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

ਸਥਿਰ ਪੂਰਨ chase_statx(ਪੂਰਨ fd, ਬਣਤਰ statx *ret) {
        ਵਾਪਸ xstatx_ਭਰਿਆ(fd,
                        /* path= */ ਨਿਰਾ,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_ਉੱਤਮ,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ਸਥਿਰ ਪੂਰਨ chase_openat2(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ਸਥਿਰ bool can_openat2 = true;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));

        ਜੇ (!can_openat2)
                ਵਾਪਸ -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        ਜੇ ((chase_flags & (CHASE_NO_SHORTCUT_ਨਕਾਬ & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                ਵਾਪਸ -EOPNOTSUPP;
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_TRIGGER_AUTOFS))
                ਵਾਪਸ -EOPNOTSUPP;
        ਜੇ (ਜੜ੍ਹ_fd != XAT_FDROOT && ਜੜ੍ਹ_fd != dir_fd)
                ਵਾਪਸ -EOPNOTSUPP;

        _cleanup_close_ ਪੂਰਨ dir_fd_local = -EBADF;
        ਜੇ (dir_fd == XAT_FDROOT) {
                ਜੇ (ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                        dir_fd = AT_FDCWD;
                ਨਹੀਂਤਾਂ {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_ਰਸਤਾ);
                        ਜੇ (dir_fd_local < 0)
                                ਵਾਪਸ -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ਬਣਤਰ open_how how = {
                .flags = O_ਰਸਤਾ|O_CLOEXEC,
        };
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        ਜੇ (ਜੜ੍ਹ_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_ਵਿੱਚ_ਜੜ੍ਹ;
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ ਪੂਰਨ fd = openat2(dir_fd, ਰਸਤਾ, &how, ਆਕਾਰ(how));
        ਜੇ (fd < 0) {
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
                ਜੇ (errno == ENOSYS)
                        can_openat2 = false;
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(errno, ENOSYS, EPERM, EAGAIN))
                        ਵਾਪਸ -EOPNOTSUPP;
                ਵਾਪਸ -errno;
        }

        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ TAKE_FD(fd);
}

ਸਥਿਰ ਪੂਰਨ chase_xopenat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਪੂਰਨ open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_ਸੈੱਟ(open_flags, O_ਰਸਤਾ))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        ਵਾਪਸ xopenat_ਭਰਿਆ(dir_fd, ਰਸਤਾ, open_flags, xopen_flags, ਬਹੁਲਕ_INVALID);
}

ਸਥਿਰ bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        ਜੇ (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                ਵਾਪਸ false;

        ਵਾਪਸ a != b; /* Otherwise we need to stay within the same UID */
}

ਪੂਰਨ statx_unsafe_transition(ਅਚਲ ਬਣਤਰ statx *a, ਅਚਲ ਬਣਤਰ statx *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਜੇ (!FLAGS_ਸੈੱਟ(a->stx_ਨਕਾਬ, STATX_UID) || !FLAGS_ਸੈੱਟ(b->stx_ਨਕਾਬ, STATX_UID))
                ਵਾਪਸ -ENODATA;

        ਵਾਪਸ uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ uid_unsafe_transition(a->st_uid, b->st_uid);
}

ਸਥਿਰ ਪੂਰਨ log_unsafe_transition(ਪੂਰਨ a, ਪੂਰਨ b, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags flags) {
        _cleanup_free_ ਅੱਖਰ *n1 = ਨਿਰਾ, *n2 = ਨਿਰਾ, *user_a = ਨਿਰਾ, *user_b = ਨਿਰਾ;
        ਬਣਤਰ stat st;

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, CHASE_WARN))
                ਵਾਪਸ -ENOLINK;

        (ਖਾਲੀ) fd_get_ਰਸਤਾ(a, &n1);
        (ਖਾਲੀ) fd_get_ਰਸਤਾ(b, &n2);

        ਜੇ (fstat(a, &st) == 0)
                user_a = uid_ਮੰਜ਼ਿਲ_name(st.st_uid);
        ਜੇ (fstat(b, &st) == 0)
                user_b = uid_ਮੰਜ਼ਿਲ_name(st.st_uid);

        ਵਾਪਸ log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_ਸੱਜਾ), strna(n2), strna(user_b), ਰਸਤਾ);
}

ਸਥਿਰ ਪੂਰਨ log_autofs_mount_ਬਿੰਦੂ(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags flags) {
        _cleanup_free_ ਅੱਖਰ *n1 = ਨਿਰਾ;

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, CHASE_WARN))
                ਵਾਪਸ -EREMOTE;

        (ਖਾਲੀ) fd_get_ਰਸਤਾ(fd, &n1);

        ਵਾਪਸ log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), ਰਸਤਾ);
}

ਸਥਿਰ ਪੂਰਨ log_prohibited_symlink(ਪੂਰਨ fd, ChaseFlags flags) {
        _cleanup_free_ ਅੱਖਰ *n1 = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, CHASE_WARN))
                ਵਾਪਸ -ELOOP;

        (ਖਾਲੀ) fd_get_ਰਸਤਾ(fd, &n1);

        ਵਾਪਸ log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

ਪੂਰਨ chaseat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ));
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ|CHASE_ਕੱਢੋ_FILENAME));
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(ਜੜ੍ਹ_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd != AT_FDCWD || ਵਿੱਚ_ਸੈੱਟ(ਜੜ੍ਹ_fd, AT_FDCWD, XAT_FDROOT));

        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ))
                ਪ੍ਰਤਿਜ੍ਞਾ(!ret_fd);

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
        ਜੇ (ਜੜ੍ਹ_fd == AT_FDCWD)
                ਜੜ੍ਹ_fd = XAT_FDROOT;
        ਨਹੀਂਤਾਂ {
                r = dir_fd_is_ਜੜ੍ਹ(ਜੜ੍ਹ_fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        ਜੜ੍ਹ_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_ਜੜ੍ਹ(dir_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r > 0 && ਜੜ੍ਹ_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        ਜੇ (dir_fd == XAT_FDROOT || ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                dir_fd = ਜੜ੍ਹ_fd;

        ਜੇ (isempty(ਰਸਤਾ))
                ਰਸਤਾ = ".";

        bool append_trail_slash = false;
        ਜੇ (ENDSWITH_ਸੈੱਟ(ਰਸਤਾ, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } ਨਹੀਂਤਾਂ ਜੇ (dot_or_dot_dot(ਰਸਤਾ) || endswith(ਰਸਤਾ, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਮਾਪਾ))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_ਸੈੱਟ(flags, CHASE_MUST_BE_REGULAR) + FLAGS_ਸੈੱਟ(flags, CHASE_MUST_BE_SOCKET) > 1)
                ਵਾਪਸ -EBADSLT;

        ਜੇ (!ret_ਰਸਤਾ) {
                r = chase_openat2(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, flags);
                ਜੇ (r >= 0) {
                        ਜੇ (ret_fd)
                                *ret_fd = r;
                        ਨਹੀਂਤਾਂ
                                ਸੁਰੱਖਿਅਤ_close(r);

                        ਵਾਪਸ 1;
                }
                ਜੇ (r != -EOPNOTSUPP)
                        ਵਾਪਸ r;
        }

        ਜੇ (ਜੜ੍ਹ_fd == XAT_FDROOT && !ret_ਰਸਤਾ && (flags & CHASE_NO_SHORTCUT_ਨਕਾਬ) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, ਰਸਤਾ, flags, O_ਰਸਤਾ|O_CLOEXEC, /* xopen_flags= */ 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (ret_fd)
                        *ret_fd = r;
                ਨਹੀਂਤਾਂ
                        ਸੁਰੱਖਿਅਤ_close(r);

                ਵਾਪਸ 1;
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
        bool need_absolute = (ਜੜ੍ਹ_fd == XAT_FDROOT || dir_fd != ਜੜ੍ਹ_fd) && (dir_fd == XAT_FDROOT || ਰਸਤਾ_is_absolute(ਰਸਤਾ));

        _cleanup_free_ ਅੱਖਰ *ਪੂਰਾ = ਨਿਰਾ;
        ਜੇ (need_absolute) {
                ਪੂਰਾ = strdup("/");
                ਜੇ (!ਪੂਰਾ)
                        ਵਾਪਸ -ENOMEM;
        }

        _cleanup_close_ ਪੂਰਨ fd = xopenat(dir_fd, ਨਿਰਾ, O_CLOEXEC|O_DIRECTORY|O_ਰਸਤਾ);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਬਣਤਰ statx stx;
        r = chase_statx(fd, &stx);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ਬਣਤਰ statx ਜੜ੍ਹ_stx;
        ਜੇ (ਜੜ੍ਹ_fd != XAT_FDROOT) {
                ਜੇ (ਜੜ੍ਹ_fd == dir_fd)
                        ਜੜ੍ਹ_stx = stx;
                ਨਹੀਂਤਾਂ {
                        r = chase_statx(ਜੜ੍ਹ_fd, &ਜੜ੍ਹ_stx);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }
        }

        _cleanup_free_ ਅੱਖਰ *ਬਫਰ = strdup(ਰਸਤਾ);
        ਜੇ (!ਬਫਰ)
                ਵਾਪਸ -ENOMEM;

        ਅਚਲ ਅੱਖਰ *todo = ਬਫਰ;
        bool exists = true;
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n_ਕਦਮਾਂ = 0;; n_ਕਦਮਾਂ++) {
                _cleanup_free_ ਅੱਖਰ *ਪਹਿਲਾ = ਨਿਰਾ;
                _cleanup_close_ ਪੂਰਨ ਬੱਚਾ = -EBADF;
                ਬਣਤਰ statx stx_ਬੱਚਾ;
                ਅਚਲ ਅੱਖਰ *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                ਜੇ (n_ਕਦਮਾਂ > CHASE_MAX)
                        ਵਾਪਸ -ELOOP;

                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&todo, /* accept_dot_dot= */ true, &e);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0) /* We reached the end. */
                        ਰੋਕੋ;

                ਪਹਿਲਾ = strndup(e, r);
                ਜੇ (!ਪਹਿਲਾ)
                        ਵਾਪਸ -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                ਜੇ (streq(ਪਹਿਲਾ, "..")) {
                        _cleanup_free_ ਅੱਖਰ *ਮਾਪਾ = ਨਿਰਾ;
                        _cleanup_close_ ਪੂਰਨ fd_ਮਾਪਾ = -EBADF;
                        ਬਣਤਰ statx stx_ਮਾਪਾ;

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
                        ਜੇ (ਜੜ੍ਹ_fd != XAT_FDROOT) {
                                bool is_ਜੜ੍ਹ = ਜੜ੍ਹ_fd == dir_fd && ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਪੂਰਾ);
                                ਜੇ (!is_ਜੜ੍ਹ && statx_inode_same(&stx, &ਜੜ੍ਹ_stx)) {
                                        r = statx_mount_same(&stx, &ਜੜ੍ਹ_stx);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ r;

                                        is_ਜੜ੍ਹ = r > 0;
                                }
                                ਜੇ (is_ਜੜ੍ਹ) {
                                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ))
                                                ਜਾਓ chased_one;
                                        ਜਾਰੀ;
                                }
                        }

                        fd_ਮਾਪਾ = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_ਰਸਤਾ|O_DIRECTORY);
                        ਜੇ (fd_ਮਾਪਾ < 0)
                                ਵਾਪਸ -errno;

                        r = chase_statx(fd_ਮਾਪਾ, &stx_ਮਾਪਾ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        ਜੇ (statx_inode_same(&stx_ਮਾਪਾ, &stx)) {
                                r = dir_fd_is_ਜੜ੍ਹ(fd);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                                ਜੇ (r > 0) {
                                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ))
                                                ਜਾਓ chased_one;
                                        ਜਾਰੀ;
                                }
                        }

                        r = ਰਸਤਾ_ਕੱਢੋ_directory(ਪੂਰਾ, &ਮਾਪਾ);
                        ਜੇ (r >= 0) {
                                ਪ੍ਰਤਿਜ੍ਞਾ(!need_absolute || ਰਸਤਾ_is_absolute(ਮਾਪਾ));
                                free_and_replace(ਪੂਰਾ, ਮਾਪਾ);
                        } ਨਹੀਂਤਾਂ ਜੇ (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                ਪ੍ਰਤਿਜ੍ਞਾ(!need_absolute);
                                ਪੂਰਾ = mfree(ਪੂਰਾ);
                        } ਨਹੀਂਤਾਂ ਜੇ (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } ਨਹੀਂਤਾਂ ਜੇ (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                ਪ੍ਰਤਿਜ੍ਞਾ(!need_absolute);

                                ਜੇ (!isempty(ਪੂਰਾ) && !ਰਸਤਾ_is_valid(ਪੂਰਾ))
                                        ਵਾਪਸ -EINVAL;

                                ਜੇ (!ਰਸਤਾ_extend(&ਪੂਰਾ, ".."))
                                        ਵਾਪਸ -ENOMEM;
                        } ਨਹੀਂਤਾਂ
                                ਵਾਪਸ r;

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ))
                                ਜਾਓ chased_one;

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਸੁਰੱਖਿਅਤ)) {
                                r = statx_unsafe_transition(&stx, &stx_ਮਾਪਾ);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                                ਜੇ (r > 0)
                                        ਵਾਪਸ log_unsafe_transition(fd, fd_ਮਾਪਾ, ਰਸਤਾ, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਮਾਪਾ) && isempty(todo)) {
                                _cleanup_close_ ਪੂਰਨ fd_grandparent = -EBADF;
                                ਬਣਤਰ statx stx_grandparent;

                                fd_grandparent = openat(fd_ਮਾਪਾ, "..", O_CLOEXEC|O_NOFOLLOW|O_ਰਸਤਾ|O_DIRECTORY);
                                ਜੇ (fd_grandparent < 0)
                                        ਵਾਪਸ -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਸੁਰੱਖਿਅਤ)) {
                                        r = statx_unsafe_transition(&stx_ਮਾਪਾ, &stx_grandparent);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ r;
                                        ਜੇ (r > 0)
                                                ਵਾਪਸ log_unsafe_transition(fd_ਮਾਪਾ, fd_grandparent, ਰਸਤਾ, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                ਰੋਕੋ;
                        }

                        /* update fd and stat */
                        stx = stx_ਮਾਪਾ;
                        close_and_replace(fd, fd_ਮਾਪਾ);
                        ਜਾਰੀ;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                ਬੱਚਾ = r = xopenat_ਭਰਿਆ(fd, ਪਹਿਲਾ,
                                         O_ਰਸਤਾ|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_ਸੈੱਟ(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         ਬਹੁਲਕ_INVALID);
                ਜੇ (r < 0) {
                        ਜੇ (r != -ENOENT)
                                ਵਾਪਸ r;

                        ਜੇ (!isempty(todo) && !ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                ਵਾਪਸ r;

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_ਮਾਪਾ|CHASE_NONEXISTENT)))) {
                                ਬੱਚਾ = xopenat(fd,
                                                ਪਹਿਲਾ,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_ਰਸਤਾ|O_CLOEXEC);
                                ਜੇ (ਬੱਚਾ < 0)
                                        ਵਾਪਸ ਬੱਚਾ;
                        } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਮਾਪਾ) && isempty(todo)) {
                                ਜੇ (!ਰਸਤਾ_extend(&ਪੂਰਾ, ਪਹਿਲਾ))
                                        ਵਾਪਸ -ENOMEM;

                                ਰੋਕੋ;
                        } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_NONEXISTENT)) {
                                ਜੇ (!ਰਸਤਾ_extend(&ਪੂਰਾ, ਪਹਿਲਾ, todo))
                                        ਵਾਪਸ -ENOMEM;

                                exists = false;
                                ਰੋਕੋ;
                        } ਨਹੀਂਤਾਂ
                                ਵਾਪਸ r;
                }

                r = chase_statx(ਬੱਚਾ, &stx_ਬੱਚਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਸੁਰੱਖਿਅਤ)) {
                        r = statx_unsafe_transition(&stx, &stx_ਬੱਚਾ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r > 0)
                                ਵਾਪਸ log_unsafe_transition(fd, ਬੱਚਾ, ਰਸਤਾ, flags);
                }

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(ਬੱਚਾ, AUTOFS_SUPER_MAGIC) > 0)
                        ਵਾਪਸ log_autofs_mount_ਬਿੰਦੂ(ਬੱਚਾ, ਰਸਤਾ, flags);

                ਜੇ (S_ISLNK(stx_ਬੱਚਾ.stx_ਬਹੁਲਕ) && !(FLAGS_ਸੈੱਟ(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ ਅੱਖਰ *destination = ਨਿਰਾ;

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_PROHIBIT_SYMLINKS))
                                ਵਾਪਸ log_prohibited_symlink(ਬੱਚਾ, flags);

                        r = readlinkat_malloc(fd, ਪਹਿਲਾ, &destination);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (isempty(destination))
                                ਵਾਪਸ -EINVAL;

                        ਜੇ (ਰਸਤਾ_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                ਸੁਰੱਖਿਅਤ_close(fd);
                                fd = fd_reopen(ਜੜ੍ਹ_fd, O_CLOEXEC|O_ਰਸਤਾ|O_DIRECTORY);
                                ਜੇ (fd < 0)
                                        ਵਾਪਸ fd;

                                r = chase_statx(fd, &stx);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਸੁਰੱਖਿਅਤ)) {
                                        r = statx_unsafe_transition(&stx_ਬੱਚਾ, &stx);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ r;
                                        ਜੇ (r > 0)
                                                ਵਾਪਸ log_unsafe_transition(ਬੱਚਾ, fd, ਰਸਤਾ, flags);
                                }

                                ਜੇ (dir_fd != ਜੜ੍ਹ_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&ਪੂਰਾ, need_absolute ? "/" : ਨਿਰਾ);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        ਜੇ (!ਰਸਤਾ_extend(&destination, todo))
                                ਵਾਪਸ -ENOMEM;

                        free_and_replace(ਬਫਰ, destination);
                        todo = ਬਫਰ;

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ))
                                ਜਾਓ chased_one;

                        ਜਾਰੀ;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                ਜੇ (!ਰਸਤਾ_extend(&ਪੂਰਾ, ਪਹਿਲਾ))
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਮਾਪਾ) && isempty(todo))
                        ਰੋਕੋ;

                /* And iterate again, but go one directory further down. */
                stx = stx_ਬੱਚਾ;
                close_and_replace(fd, ਬੱਚਾ);
        }

        ਜੇ (exists) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }
        }

        ਜੇ (ret_ਰਸਤਾ) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕੱਢੋ_FILENAME) && ਪੂਰਾ) {
                        _cleanup_free_ ਅੱਖਰ *f = ਨਿਰਾ;

                        r = ਰਸਤਾ_ਕੱਢੋ_filename(ਪੂਰਾ, &f);
                        ਜੇ (r < 0 && r != -EADDRNOTAVAIL)
                                ਵਾਪਸ r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(ਪੂਰਾ, f);
                }

                ਜੇ (!ਪੂਰਾ) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(!need_absolute || FLAGS_ਸੈੱਟ(flags, CHASE_ਕੱਢੋ_FILENAME));
                        ਪੂਰਾ = strdup(".");
                        ਜੇ (!ਪੂਰਾ)
                                ਵਾਪਸ -ENOMEM;
                }

                ਜੇ (append_trail_slash)
                        ਜੇ (!strextend(&ਪੂਰਾ, "/"))
                                ਵਾਪਸ -ENOMEM;

                *ret_ਰਸਤਾ = TAKE_PTR(ਪੂਰਾ);
        }

        ਜੇ (ret_fd) {
                ਜੇ (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } ਨਹੀਂਤਾਂ
                        *ret_fd = -EBADF;
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਕਦਮ))
                ਵਾਪਸ 1;

        ਵਾਪਸ exists;

chased_one:
        ਜੇ (ret_ਰਸਤਾ) {
                ਅਚਲ ਅੱਖਰ *e;

                ਜੇ (!ਪੂਰਾ) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(!need_absolute);
                        ਪੂਰਾ = strdup(append_trail_slash ? "./" : ".");
                        ਜੇ (!ਪੂਰਾ)
                                ਵਾਪਸ -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&todo, /* accept_dot_dot= */ true, &e);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        *ret_ਰਸਤਾ = TAKE_PTR(ਪੂਰਾ);
                ਨਹੀਂਤਾਂ {
                        ਅੱਖਰ *c;

                        c = ਰਸਤਾ_join(ਪੂਰਾ, e);
                        ਜੇ (!c)
                                ਵਾਪਸ -ENOMEM;

                        *ret_ਰਸਤਾ = c;
                }
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd) {
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ, *absolute = ਨਿਰਾ, *p = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF, pfd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        r = ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ_harder_ਮੰਜ਼ਿਲ_null(&ਜੜ੍ਹ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* A root directory of "/" or "" is identical to "/". */
        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ))
                ਜੜ੍ਹ = "/";
        ਨਹੀਂਤਾਂ {
                r = ਰਸਤਾ_make_absolute_cwd(ਜੜ੍ਹ, &ਜੜ੍ਹ_abs);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                ਜੜ੍ਹ = ਰਸਤਾ_simplify(ਜੜ੍ਹ_abs);

                ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_is_absolute(ਜੜ੍ਹ));
                ਪ੍ਰਤਿਜ੍ਞਾ(!ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ));

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ)) {
                        absolute = ਰਸਤਾ_join(ਜੜ੍ਹ, ਰਸਤਾ);
                        ਜੇ (!absolute)
                                ਵਾਪਸ -ENOMEM;
                }
        }

        ਜੇ (!absolute) {
                r = ਰਸਤਾ_make_absolute_cwd(ਰਸਤਾ, &absolute);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਰਸਤਾ = ਰਸਤਾ_startswith(absolute, ਜੜ੍ਹ);
        ਜੇ (!ਰਸਤਾ)
                ਵਾਪਸ log_ਭਰਿਆ_errno(FLAGS_ਸੈੱਟ(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, ਜੜ੍ਹ);

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ))
                fd = XAT_FDROOT;
        ਨਹੀਂਤਾਂ {
                fd = open(ਜੜ੍ਹ, O_CLOEXEC|O_DIRECTORY|O_ਰਸਤਾ);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;
        }

        r = chaseat(fd, fd, ਰਸਤਾ, flags & ~CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, ret_fd ? &pfd : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ) {
                ਜੇ (!FLAGS_ਸੈੱਟ(flags, CHASE_ਕੱਢੋ_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ))
                                ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_is_absolute(p));
                        ਨਹੀਂਤਾਂ {
                                ਅੱਖਰ *q;

                                ਪ੍ਰਤਿਜ੍ਞਾ(!ਰਸਤਾ_is_absolute(p));

                                q = ਰਸਤਾ_join(ਜੜ੍ਹ, p + STR_ਵਿੱਚ_ਸੈੱਟ(p, ".", "./"));
                                ਜੇ (!q)
                                        ਵਾਪਸ -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_ਰਸਤਾ = TAKE_PTR(p);
        }

        ਜੇ (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        ਵਾਪਸ r;
}

ਪੂਰਨ chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret) {
        ਅੱਖਰ *q;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        ਜੇ (!ਰਸਤਾ_is_absolute(ਰਸਤਾ)) {
                _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;

                r = ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ_harder_ਮੰਜ਼ਿਲ_null(&ਜੜ੍ਹ);
                ਜੇ (r < 0 && r != -ENOENT)
                        ਵਾਪਸ r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ))
                        ਵਾਪਸ -EINVAL;

                r = ਰਸਤਾ_make_absolute_cwd(ਜੜ੍ਹ, &ਜੜ੍ਹ_abs);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੜ੍ਹ = ਰਸਤਾ_simplify(ਜੜ੍ਹ_abs);

                q = ਰਸਤਾ_join(ਜੜ੍ਹ, ਰਸਤਾ + (ਰਸਤਾ[0] == '.' && ਵਿੱਚ_ਸੈੱਟ(ਰਸਤਾ[1], '/', '\0')));
        } ਨਹੀਂਤਾਂ
                q = strdup(ਰਸਤਾ);
        ਜੇ (!q)
                ਵਾਪਸ -ENOMEM;

        *ret = q;
        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_ਕੱਢੋ_filename(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        r = ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ_harder_ਮੰਜ਼ਿਲ_null(&ਜੜ੍ਹ);
        ਜੇ (r < 0 && r != -ENOENT)
                ਵਾਪਸ r;

        ਜੇ (!ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ)) {
                _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;

                r = ਰਸਤਾ_make_absolute_cwd(ਜੜ੍ਹ, &ਜੜ੍ਹ_abs);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਰਸਤਾ = ਰਸਤਾ_startswith(ਰਸਤਾ, ਜੜ੍ਹ_abs);
                ਜੇ (!ਰਸਤਾ)
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (!isempty(ਰਸਤਾ)) {
                r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, ret);
                ਜੇ (r != -EADDRNOTAVAIL)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, ".");
}

ਪੂਰਨ chase_and_open(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ChaseFlags chase_flags,
                ਪੂਰਨ open_flags,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *fname = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *open_name = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ) && !ret_ਰਸਤਾ && (chase_flags & CHASE_NO_SHORTCUT_ਨਕਾਬ) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ਵਾਪਸ chase_xopenat(AT_FDCWD, ਰਸਤਾ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(ਰਸਤਾ, ਜੜ੍ਹ, (CHASE_ਮਾਪਾ|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        ਜੇ (!FLAGS_ਸੈੱਟ(chase_flags, CHASE_ਮਾਪਾ)) {
                ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_ਕੱਢੋ_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ਨਹੀਂਤਾਂ {
                        r = chase_ਕੱਢੋ_filename(p, ਜੜ੍ਹ, &fname);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(ਰਸਤਾ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ r;
}

ਪੂਰਨ chase_and_opendir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, DIR **ret_dir) {
        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        DIR *d;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_dir);

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ) && !ret_ਰਸਤਾ && (chase_flags & CHASE_NO_SHORTCUT_ਨਕਾਬ) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(ਰਸਤਾ);
                ਜੇ (!d)
                        ਵਾਪਸ -errno;

                *ret_dir = d;
                ਵਾਪਸ 0;
        }

        r = chase(ਰਸਤਾ, ਜੜ੍ਹ, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        d = xopendirat(ਰਸਤਾ_fd, /* path= */ ਨਿਰਾ, /* flags= */ 0);
        ਜੇ (!d)
                ਵਾਪਸ -errno;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        *ret_dir = d;
        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_stat(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਬਣਤਰ stat *ret_stat) {
        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_stat);

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ) && !ret_ਰਸਤਾ && (chase_flags & (CHASE_NO_SHORTCUT_ਨਕਾਬ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ਵਾਪਸ RET_NERRNO(fstatat(AT_FDCWD, ਰਸਤਾ, ret_stat,
                                          FLAGS_ਸੈੱਟ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(ਰਸਤਾ, ਜੜ੍ਹ, chase_flags, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        ਜੇ (fstat(ਰਸਤਾ_fd, ret_stat) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_access(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਪੂਰਨ access_ਬਹੁਲਕ, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ) && !ret_ਰਸਤਾ && (chase_flags & (CHASE_NO_SHORTCUT_ਨਕਾਬ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ਵਾਪਸ RET_NERRNO(faccessat(AT_FDCWD, ਰਸਤਾ, access_ਬਹੁਲਕ,
                                            FLAGS_ਸੈੱਟ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(ਰਸਤਾ, ਜੜ੍ਹ, chase_flags, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        r = access_fd(ਰਸਤਾ_fd, access_ਬਹੁਲਕ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_fopen_unlocked(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ChaseFlags chase_flags,
                ਅਚਲ ਅੱਖਰ *open_flags,
                ਅੱਖਰ **ret_ਰਸਤਾ,
                FILE **ret_file) {

        _cleanup_free_ ਅੱਖਰ *final_ਰਸਤਾ = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ ਬਹੁਲਕ_flags, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_ਮਾਪਾ|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        ਪ੍ਰਤਿਜ੍ਞਾ(open_flags);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_file);

        ਬਹੁਲਕ_flags = fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(open_flags);
        ਜੇ (ਬਹੁਲਕ_flags < 0)
                ਵਾਪਸ ਬਹੁਲਕ_flags;

        fd = chase_and_open(ਰਸਤਾ, ਜੜ੍ਹ, chase_flags, ਬਹੁਲਕ_flags, ret_ਰਸਤਾ ? &final_ਰਸਤਾ : ਨਿਰਾ);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(final_ਰਸਤਾ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_unlink(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਪੂਰਨ unlink_flags, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *fname = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_ਮਾਪਾ|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_ਕੱਢੋ_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(ਰਸਤਾ, ਜੜ੍ਹ, chase_flags|CHASE_ਮਾਪਾ|CHASE_NOFOLLOW, O_ਰਸਤਾ|O_DIRECTORY|O_CLOEXEC, &p);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        r = ਰਸਤਾ_ਕੱਢੋ_filename(p, &fname);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (unlinkat(fd, fname, unlink_flags) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_open_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅੱਖਰ **ret_filename) {
        ਪੂਰਨ pfd, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));

        r = chase(ਰਸਤਾ, ਜੜ੍ਹ, CHASE_ਮਾਪਾ|CHASE_ਕੱਢੋ_FILENAME|chase_flags, ret_filename, &pfd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ pfd;
}

ਪੂਰਨ chase_and_openat(
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ChaseFlags chase_flags,
                ਪੂਰਨ open_flags,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *fname = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *open_name = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_MUST_BE_SOCKET)));

        ਜੇ (ਜੜ੍ਹ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ਰਸਤਾ && (chase_flags & CHASE_NO_SHORTCUT_ਨਕਾਬ) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ਵਾਪਸ chase_xopenat(dir_fd, ਰਸਤਾ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, (chase_flags|CHASE_ਮਾਪਾ)&~CHASE_MUST_BE_REGULAR, &p, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!FLAGS_ਸੈੱਟ(chase_flags, CHASE_ਮਾਪਾ)) {
                ਜੇ (FLAGS_ਸੈੱਟ(chase_flags, CHASE_ਕੱਢੋ_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ਨਹੀਂਤਾਂ {
                        r = ਰਸਤਾ_ਕੱਢੋ_filename(p, &fname);
                        ਜੇ (r < 0 && r != -EADDRNOTAVAIL)
                                ਵਾਪਸ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(ਰਸਤਾ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ r;
}

ਪੂਰਨ chase_and_opendirat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, DIR **ret_dir) {
        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        DIR *d;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_dir);

        ਜੇ (ਜੜ੍ਹ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ਰਸਤਾ && (chase_flags & CHASE_NO_SHORTCUT_ਨਕਾਬ) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(ਰਸਤਾ);
                ਜੇ (!d)
                        ਵਾਪਸ -errno;

                *ret_dir = d;
                ਵਾਪਸ 0;
        }

        r = chaseat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, chase_flags, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        d = xopendirat(ਰਸਤਾ_fd, /* path= */ ਨਿਰਾ, /* flags= */ 0);
        ਜੇ (!d)
                ਵਾਪਸ -errno;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        *ret_dir = d;
        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_statat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਬਣਤਰ stat *ret_stat) {
        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_stat);

        ਜੇ (ਜੜ੍ਹ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ਰਸਤਾ && (chase_flags & (CHASE_NO_SHORTCUT_ਨਕਾਬ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ਵਾਪਸ RET_NERRNO(fstatat(AT_FDCWD, ਰਸਤਾ, ret_stat,
                                          FLAGS_ਸੈੱਟ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, chase_flags, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        ਜੇ (fstat(ਰਸਤਾ_fd, ret_stat) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_accessat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਪੂਰਨ access_ਬਹੁਲਕ, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ ਰਸਤਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));

        ਜੇ (ਜੜ੍ਹ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_ਰਸਤਾ && (chase_flags & (CHASE_NO_SHORTCUT_ਨਕਾਬ|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ਵਾਪਸ RET_NERRNO(faccessat(AT_FDCWD, ਰਸਤਾ, access_ਬਹੁਲਕ,
                                            FLAGS_ਸੈੱਟ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, chase_flags, ret_ਰਸਤਾ ? &p : ਨਿਰਾ, &ਰਸਤਾ_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ_fd >= 0);

        r = access_fd(ਰਸਤਾ_fd, access_ਬਹੁਲਕ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_fopenat_unlocked(
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ChaseFlags chase_flags,
                ਅਚਲ ਅੱਖਰ *open_flags,
                ਅੱਖਰ **ret_ਰਸਤਾ,
                FILE **ret_file) {

        _cleanup_free_ ਅੱਖਰ *final_ਰਸਤਾ = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ ਬਹੁਲਕ_flags, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_ਮਾਪਾ)));
        ਪ੍ਰਤਿਜ੍ਞਾ(open_flags);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_file);

        ਬਹੁਲਕ_flags = fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(open_flags);
        ਜੇ (ਬਹੁਲਕ_flags < 0)
                ਵਾਪਸ ਬਹੁਲਕ_flags;

        fd = chase_and_openat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, chase_flags, ਬਹੁਲਕ_flags, ret_ਰਸਤਾ ? &final_ਰਸਤਾ : ਨਿਰਾ);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(final_ਰਸਤਾ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_unlinkat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਪੂਰਨ unlink_flags, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *fname = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ|CHASE_ਮਾਪਾ|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_ਕੱਢੋ_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, chase_flags|CHASE_ਮਾਪਾ|CHASE_NOFOLLOW, O_ਰਸਤਾ|O_DIRECTORY|O_CLOEXEC, &p);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        r = ਰਸਤਾ_ਕੱਢੋ_filename(p, &fname);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (unlinkat(fd, fname, unlink_flags) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ 0;
}

ਪੂਰਨ chase_and_open_ਮਾਪਾ_at(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅੱਖਰ **ret_filename) {
        ਪੂਰਨ pfd, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_ਕਦਮ)));

        r = chaseat(ਜੜ੍ਹ_fd, dir_fd, ਰਸਤਾ, CHASE_ਮਾਪਾ|CHASE_ਕੱਢੋ_FILENAME|chase_flags, ret_filename, &pfd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ pfd;
}
