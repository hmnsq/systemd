/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/magic.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "glyph-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "user-util.h"

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

நிலையான முழு chase_statx(முழு fd, கட்டமைப்பு statx *ret) {
        திரும்பு xstatx_full(fd,
                        /* path= */ வெற்று,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

நிலையான முழு chase_openat2(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        நிலையான bool can_openat2 = true;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));

        எனில் (!can_openat2)
                திரும்பு -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        எனில் ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                திரும்பு -EOPNOTSUPP;
        எனில் (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                திரும்பு -EOPNOTSUPP;
        எனில் (வேர்_fd != XAT_FDROOT && வேர்_fd != dir_fd)
                திரும்பு -EOPNOTSUPP;

        _cleanup_close_ முழு dir_fd_local = -EBADF;
        எனில் (dir_fd == XAT_FDROOT) {
                எனில் (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                இல்லையெனில் {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        எனில் (dir_fd_local < 0)
                                திரும்பு -errno;
                        dir_fd = dir_fd_local;
                }
        }

        கட்டமைப்பு open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        எனில் (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        எனில் (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        எனில் (வேர்_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_வேர்;
        எனில் (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ முழு fd = openat2(dir_fd, path, &how, அளவு(how));
        எனில் (fd < 0) {
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
                எனில் (errno == ENOSYS)
                        can_openat2 = false;
                எனில் (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        திரும்பு -EOPNOTSUPP;
                திரும்பு -errno;
        }

        எனில் (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                எனில் (r < 0)
                        திரும்பு r;
        }
        எனில் (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு TAKE_FD(fd);
}

நிலையான முழு chase_xopenat(முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, முழு open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        எனில் (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        எனில் (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        எனில் (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        எனில் (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        எனில் (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        திரும்பு xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

நிலையான bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        எனில் (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                திரும்பு false;

        திரும்பு a != b; /* Otherwise we need to stay within the same UID */
}

முழு statx_unsafe_transition(மாறிலி கட்டமைப்பு statx *a, மாறிலி கட்டமைப்பு statx *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        எனில் (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                திரும்பு -ENODATA;

        திரும்பு uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு uid_unsafe_transition(a->st_uid, b->st_uid);
}

நிலையான முழு log_unsafe_transition(முழு a, முழு b, மாறிலி எழுத்து *path, ChaseFlags flags) {
        _cleanup_free_ எழுத்து *n1 = வெற்று, *n2 = வெற்று, *user_a = வெற்று, *user_b = வெற்று;
        கட்டமைப்பு stat st;

        எனில் (!FLAGS_SET(flags, CHASE_WARN))
                திரும்பு -ENOLINK;

        (வெறுமை) fd_get_path(a, &n1);
        (வெறுமை) fd_get_path(b, &n2);

        எனில் (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        எனில் (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        திரும்பு log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_வலது), strna(n2), strna(user_b), path);
}

நிலையான முழு log_autofs_mount_point(முழு fd, மாறிலி எழுத்து *path, ChaseFlags flags) {
        _cleanup_free_ எழுத்து *n1 = வெற்று;

        எனில் (!FLAGS_SET(flags, CHASE_WARN))
                திரும்பு -EREMOTE;

        (வெறுமை) fd_get_path(fd, &n1);

        திரும்பு log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

நிலையான முழு log_prohibited_symlink(முழு fd, ChaseFlags flags) {
        _cleanup_free_ எழுத்து *n1 = வெற்று;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (!FLAGS_SET(flags, CHASE_WARN))
                திரும்பு -ELOOP;

        (வெறுமை) fd_get_path(fd, &n1);

        திரும்பு log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

முழு chaseat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags flags, எழுத்து **ret_path, முழு *ret_fd) {
        முழு r;

        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, CHASE_PREFIX_வேர்));
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(வேர்_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        ப்ரதிஜ்ஞா(dir_fd != AT_FDCWD || IN_SET(வேர்_fd, AT_FDCWD, XAT_FDROOT));

        எனில் (FLAGS_SET(flags, CHASE_STEP))
                ப்ரதிஜ்ஞா(!ret_fd);

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
        எனில் (வேர்_fd == AT_FDCWD)
                வேர்_fd = XAT_FDROOT;
        இல்லையெனில் {
                r = dir_fd_is_வேர்(வேர்_fd);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r > 0)
                        வேர்_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_வேர்(dir_fd);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r > 0 && வேர்_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        எனில் (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = வேர்_fd;

        எனில் (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        எனில் (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                எனில் (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } இல்லையெனில் எனில் (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        எனில் (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        எனில் (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                திரும்பு -EBADSLT;

        எனில் (!ret_path) {
                r = chase_openat2(வேர்_fd, dir_fd, path, flags);
                எனில் (r >= 0) {
                        எனில் (ret_fd)
                                *ret_fd = r;
                        இல்லையெனில்
                                safe_close(r);

                        திரும்பு 1;
                }
                எனில் (r != -EOPNOTSUPP)
                        திரும்பு r;
        }

        எனில் (வேர்_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (ret_fd)
                        *ret_fd = r;
                இல்லையெனில்
                        safe_close(r);

                திரும்பு 1;
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
        bool need_absolute = (வேர்_fd == XAT_FDROOT || dir_fd != வேர்_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ எழுத்து *done = வெற்று;
        எனில் (need_absolute) {
                done = strdup("/");
                எனில் (!done)
                        திரும்பு -ENOMEM;
        }

        _cleanup_close_ முழு fd = xopenat(dir_fd, வெற்று, O_CLOEXEC|O_DIRECTORY|O_PATH);
        எனில் (fd < 0)
                திரும்பு fd;

        கட்டமைப்பு statx stx;
        r = chase_statx(fd, &stx);
        எனில் (r < 0)
                திரும்பு r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        கட்டமைப்பு statx வேர்_stx;
        எனில் (வேர்_fd != XAT_FDROOT) {
                எனில் (வேர்_fd == dir_fd)
                        வேர்_stx = stx;
                இல்லையெனில் {
                        r = chase_statx(வேர்_fd, &வேர்_stx);
                        எனில் (r < 0)
                                திரும்பு r;
                }
        }

        _cleanup_free_ எழுத்து *buffer = strdup(path);
        எனில் (!buffer)
                திரும்பு -ENOMEM;

        மாறிலி எழுத்து *todo = buffer;
        bool exists = true;
        க்கு (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ எழுத்து *first = வெற்று;
                _cleanup_close_ முழு child = -EBADF;
                கட்டமைப்பு statx stx_child;
                மாறிலி எழுத்து *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                எனில் (n_steps > CHASE_MAX)
                        திரும்பு -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0) /* We reached the end. */
                        நிறுத்து;

                first = strndup(e, r);
                எனில் (!first)
                        திரும்பு -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                எனில் (streq(first, "..")) {
                        _cleanup_free_ எழுத்து *parent = வெற்று;
                        _cleanup_close_ முழு fd_parent = -EBADF;
                        கட்டமைப்பு statx stx_parent;

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
                        எனில் (வேர்_fd != XAT_FDROOT) {
                                bool is_வேர் = வேர்_fd == dir_fd && empty_or_வேர்(done);
                                எனில் (!is_வேர் && statx_inode_same(&stx, &வேர்_stx)) {
                                        r = statx_mount_same(&stx, &வேர்_stx);
                                        எனில் (r < 0)
                                                திரும்பு r;

                                        is_வேர் = r > 0;
                                }
                                எனில் (is_வேர்) {
                                        எனில் (FLAGS_SET(flags, CHASE_STEP))
                                                செல் chased_one;
                                        தொடர்;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        எனில் (fd_parent < 0)
                                திரும்பு -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        எனில் (r < 0)
                                திரும்பு r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        எனில் (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_வேர்(fd);
                                எனில் (r < 0)
                                        திரும்பு r;
                                எனில் (r > 0) {
                                        எனில் (FLAGS_SET(flags, CHASE_STEP))
                                                செல் chased_one;
                                        தொடர்;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        எனில் (r >= 0) {
                                ப்ரதிஜ்ஞா(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } இல்லையெனில் எனில் (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                ப்ரதிஜ்ஞா(!need_absolute);
                                done = mfree(done);
                        } இல்லையெனில் எனில் (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } இல்லையெனில் எனில் (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                ப்ரதிஜ்ஞா(!need_absolute);

                                எனில் (!isempty(done) && !path_is_valid(done))
                                        திரும்பு -EINVAL;

                                எனில் (!path_extend(&done, ".."))
                                        திரும்பு -ENOMEM;
                        } இல்லையெனில்
                                திரும்பு r;

                        எனில் (FLAGS_SET(flags, CHASE_STEP))
                                செல் chased_one;

                        எனில் (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                எனில் (r < 0)
                                        திரும்பு r;
                                எனில் (r > 0)
                                        திரும்பு log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        எனில் (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ முழு fd_grandparent = -EBADF;
                                கட்டமைப்பு statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                எனில் (fd_grandparent < 0)
                                        திரும்பு -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        எனில் (r < 0)
                                                திரும்பு r;
                                        எனில் (r > 0)
                                                திரும்பு log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                நிறுத்து;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        தொடர்;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                எனில் (r < 0) {
                        எனில் (r != -ENOENT)
                                திரும்பு r;

                        எனில் (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                திரும்பு r;

                        எனில் (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                எனில் (child < 0)
                                        திரும்பு child;
                        } இல்லையெனில் எனில் (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                எனில் (!path_extend(&done, first))
                                        திரும்பு -ENOMEM;

                                நிறுத்து;
                        } இல்லையெனில் எனில் (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                எனில் (!path_extend(&done, first, todo))
                                        திரும்பு -ENOMEM;

                                exists = false;
                                நிறுத்து;
                        } இல்லையெனில்
                                திரும்பு r;
                }

                r = chase_statx(child, &stx_child);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r > 0)
                                திரும்பு log_unsafe_transition(fd, child, path, flags);
                }

                எனில் (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        திரும்பு log_autofs_mount_point(child, path, flags);

                எனில் (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ எழுத்து *destination = வெற்று;

                        எனில் (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                திரும்பு log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (isempty(destination))
                                திரும்பு -EINVAL;

                        எனில் (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(வேர்_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                எனில் (fd < 0)
                                        திரும்பு fd;

                                r = chase_statx(fd, &stx);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        எனில் (r < 0)
                                                திரும்பு r;
                                        எனில் (r > 0)
                                                திரும்பு log_unsafe_transition(child, fd, path, flags);
                                }

                                எனில் (dir_fd != வேர்_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : வெற்று);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        எனில் (!path_extend(&destination, todo))
                                திரும்பு -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        எனில் (FLAGS_SET(flags, CHASE_STEP))
                                செல் chased_one;

                        தொடர்;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                எனில் (!path_extend(&done, first))
                        திரும்பு -ENOMEM;

                எனில் (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        நிறுத்து;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        எனில் (exists) {
                எனில் (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        எனில் (r < 0)
                                திரும்பு r;
                }
        }

        எனில் (ret_path) {
                எனில் (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ எழுத்து *f = வெற்று;

                        r = path_extract_filename(done, &f);
                        எனில் (r < 0 && r != -EADDRNOTAVAIL)
                                திரும்பு r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                எனில் (!done) {
                        ப்ரதிஜ்ஞா(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        எனில் (!done)
                                திரும்பு -ENOMEM;
                }

                எனில் (append_trail_slash)
                        எனில் (!strextend(&done, "/"))
                                திரும்பு -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        எனில் (ret_fd) {
                எனில் (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        ப்ரதிஜ்ஞா(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } இல்லையெனில்
                        *ret_fd = -EBADF;
        }

        எனில் (FLAGS_SET(flags, CHASE_STEP))
                திரும்பு 1;

        திரும்பு exists;

chased_one:
        எனில் (ret_path) {
                மாறிலி எழுத்து *e;

                எனில் (!done) {
                        ப்ரதிஜ்ஞா(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        எனில் (!done)
                                திரும்பு -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        *ret_path = TAKE_PTR(done);
                இல்லையெனில் {
                        எழுத்து *c;

                        c = path_join(done, e);
                        எனில் (!c)
                                திரும்பு -ENOMEM;

                        *ret_path = c;
                }
        }

        திரும்பு 0;
}

முழு chase(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags flags, எழுத்து **ret_path, முழு *ret_fd) {
        _cleanup_free_ எழுத்து *வேர்_abs = வெற்று, *absolute = வெற்று, *p = வெற்று;
        _cleanup_close_ முழு fd = -EBADF, pfd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        எனில் (isempty(path))
                திரும்பு -EINVAL;

        r = empty_or_வேர்_harder_to_null(&வேர்);
        எனில் (r < 0)
                திரும்பு r;

        /* A root directory of "/" or "" is identical to "/". */
        எனில் (empty_or_வேர்(வேர்))
                வேர் = "/";
        இல்லையெனில் {
                r = path_make_absolute_cwd(வேர், &வேர்_abs);
                எனில் (r < 0)
                        திரும்பு r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                வேர் = path_simplify(வேர்_abs);

                ப்ரதிஜ்ஞா(path_is_absolute(வேர்));
                ப்ரதிஜ்ஞா(!empty_or_வேர்(வேர்));

                எனில் (FLAGS_SET(flags, CHASE_PREFIX_வேர்)) {
                        absolute = path_join(வேர், path);
                        எனில் (!absolute)
                                திரும்பு -ENOMEM;
                }
        }

        எனில் (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                எனில் (r < 0)
                        திரும்பு r;
        }

        path = path_startswith(absolute, வேர்);
        எனில் (!path)
                திரும்பு log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, வேர்);

        எனில் (empty_or_வேர்(வேர்))
                fd = XAT_FDROOT;
        இல்லையெனில் {
                fd = open(வேர், O_CLOEXEC|O_DIRECTORY|O_PATH);
                எனில் (fd < 0)
                        திரும்பு -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_வேர், ret_path ? &p : வெற்று, ret_fd ? &pfd : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path) {
                எனில் (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        எனில் (empty_or_வேர்(வேர்))
                                ப்ரதிஜ்ஞா(path_is_absolute(p));
                        இல்லையெனில் {
                                எழுத்து *q;

                                ப்ரதிஜ்ஞா(!path_is_absolute(p));

                                q = path_join(வேர், p + STR_IN_SET(p, ".", "./"));
                                எனில் (!q)
                                        திரும்பு -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        எனில் (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        திரும்பு r;
}

முழு chaseat_prefix_வேர்(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், எழுத்து **ret) {
        எழுத்து *q;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        எனில் (!path_is_absolute(path)) {
                _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;

                r = empty_or_வேர்_harder_to_null(&வேர்);
                எனில் (r < 0 && r != -ENOENT)
                        திரும்பு r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                எனில் (empty_or_வேர்(வேர்))
                        திரும்பு -EINVAL;

                r = path_make_absolute_cwd(வேர், &வேர்_abs);
                எனில் (r < 0)
                        திரும்பு r;

                வேர் = path_simplify(வேர்_abs);

                q = path_join(வேர், path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } இல்லையெனில்
                q = strdup(path);
        எனில் (!q)
                திரும்பு -ENOMEM;

        *ret = q;
        திரும்பு 0;
}

முழு chase_extract_filename(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், எழுத்து **ret) {
        முழு r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        எனில் (isempty(path))
                திரும்பு -EINVAL;

        எனில் (!path_is_absolute(path))
                திரும்பு -EINVAL;

        r = empty_or_வேர்_harder_to_null(&வேர்);
        எனில் (r < 0 && r != -ENOENT)
                திரும்பு r;

        எனில் (!empty_or_வேர்(வேர்)) {
                _cleanup_free_ எழுத்து *வேர்_abs = வெற்று;

                r = path_make_absolute_cwd(வேர், &வேர்_abs);
                எனில் (r < 0)
                        திரும்பு r;

                path = path_startswith(path, வேர்_abs);
                எனில் (!path)
                        திரும்பு -EINVAL;
        }

        எனில் (!isempty(path)) {
                r = path_extract_filename(path, ret);
                எனில் (r != -EADDRNOTAVAIL)
                        திரும்பு r;
        }

        திரும்பு strdup_to(ret, ".");
}

முழு chase_and_open(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *வேர்,
                ChaseFlags chase_flags,
                முழு open_flags,
                எழுத்து **ret_path) {

        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று, *fname = வெற்று;
        மாறிலி எழுத்து *open_name = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        எனில் (empty_or_வேர்(வேர்) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                திரும்பு chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, வேர், (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        எனில் (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                எனில் (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                இல்லையெனில் {
                        r = chase_extract_filename(p, வேர், &fname);
                        எனில் (r < 0)
                                திரும்பு r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு r;
}

முழு chase_and_opendir(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, எழுத்து **ret_path, DIR **ret_dir) {
        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        DIR *d;
        முழு r;

        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ப்ரதிஜ்ஞா(ret_dir);

        எனில் (empty_or_வேர்(வேர்) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                எனில் (!d)
                        திரும்பு -errno;

                *ret_dir = d;
                திரும்பு 0;
        }

        r = chase(path, வேர், chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : வெற்று, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ வெற்று, /* flags= */ 0);
        எனில் (!d)
                திரும்பு -errno;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        திரும்பு 0;
}

முழு chase_and_stat(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, எழுத்து **ret_path, கட்டமைப்பு stat *ret_stat) {
        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        ப்ரதிஜ்ஞா(ret_stat);

        எனில் (empty_or_வேர்(வேர்) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                திரும்பு RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, வேர், chase_flags, ret_path ? &p : வெற்று, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        எனில் (fstat(path_fd, ret_stat) < 0)
                திரும்பு -errno;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு 0;
}

முழு chase_and_access(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, முழு access_mode, எழுத்து **ret_path) {
        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        எனில் (empty_or_வேர்(வேர்) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                திரும்பு RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, வேர், chase_flags, ret_path ? &p : வெற்று, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு 0;
}

முழு chase_and_fopen_unlocked(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *வேர்,
                ChaseFlags chase_flags,
                மாறிலி எழுத்து *open_flags,
                எழுத்து **ret_path,
                FILE **ret_file) {

        _cleanup_free_ எழுத்து *final_path = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு mode_flags, r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        ப்ரதிஜ்ஞா(open_flags);
        ப்ரதிஜ்ஞா(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        எனில் (mode_flags < 0)
                திரும்பு mode_flags;

        fd = chase_and_open(path, வேர், chase_flags, mode_flags, ret_path ? &final_path : வெற்று);
        எனில் (fd < 0)
                திரும்பு fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(final_path);

        திரும்பு 0;
}

முழு chase_and_unlink(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, முழு unlink_flags, எழுத்து **ret_path) {
        _cleanup_free_ எழுத்து *p = வெற்று, *fname = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, வேர், chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        எனில் (fd < 0)
                திரும்பு fd;

        r = path_extract_filename(p, &fname);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (unlinkat(fd, fname, unlink_flags) < 0)
                திரும்பு -errno;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு 0;
}

முழு chase_and_open_parent(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, எழுத்து **ret_filename) {
        முழு pfd, r;

        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, வேர், CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு pfd;
}

முழு chase_and_openat(
                முழு வேர்_fd,
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                ChaseFlags chase_flags,
                முழு open_flags,
                எழுத்து **ret_path) {

        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று, *fname = வெற்று;
        மாறிலி எழுத்து *open_name = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        எனில் (வேர்_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                திரும்பு chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(வேர்_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                எனில் (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                இல்லையெனில் {
                        r = path_extract_filename(p, &fname);
                        எனில் (r < 0 && r != -EADDRNOTAVAIL)
                                திரும்பு r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு r;
}

முழு chase_and_opendirat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, எழுத்து **ret_path, DIR **ret_dir) {
        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        DIR *d;
        முழு r;

        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ப்ரதிஜ்ஞா(ret_dir);

        எனில் (வேர்_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                எனில் (!d)
                        திரும்பு -errno;

                *ret_dir = d;
                திரும்பு 0;
        }

        r = chaseat(வேர்_fd, dir_fd, path, chase_flags, ret_path ? &p : வெற்று, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ வெற்று, /* flags= */ 0);
        எனில் (!d)
                திரும்பு -errno;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        திரும்பு 0;
}

முழு chase_and_statat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, எழுத்து **ret_path, கட்டமைப்பு stat *ret_stat) {
        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        ப்ரதிஜ்ஞா(ret_stat);

        எனில் (வேர்_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                திரும்பு RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(வேர்_fd, dir_fd, path, chase_flags, ret_path ? &p : வெற்று, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        எனில் (fstat(path_fd, ret_stat) < 0)
                திரும்பு -errno;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு 0;
}

முழு chase_and_accessat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, முழு access_mode, எழுத்து **ret_path) {
        _cleanup_close_ முழு path_fd = -EBADF;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        எனில் (வேர்_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                திரும்பு RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(வேர்_fd, dir_fd, path, chase_flags, ret_path ? &p : வெற்று, &path_fd);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு 0;
}

முழு chase_and_fopenat_unlocked(
                முழு வேர்_fd,
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                ChaseFlags chase_flags,
                மாறிலி எழுத்து *open_flags,
                எழுத்து **ret_path,
                FILE **ret_file) {

        _cleanup_free_ எழுத்து *final_path = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு mode_flags, r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        ப்ரதிஜ்ஞா(open_flags);
        ப்ரதிஜ்ஞா(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        எனில் (mode_flags < 0)
                திரும்பு mode_flags;

        fd = chase_and_openat(வேர்_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : வெற்று);
        எனில் (fd < 0)
                திரும்பு fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(final_path);

        திரும்பு 0;
}

முழு chase_and_unlinkat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, முழு unlink_flags, எழுத்து **ret_path) {
        _cleanup_free_ எழுத்து *p = வெற்று, *fname = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(வேர்_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        எனில் (fd < 0)
                திரும்பு fd;

        r = path_extract_filename(p, &fname);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (unlinkat(fd, fname, unlink_flags) < 0)
                திரும்பு -errno;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(p);

        திரும்பு 0;
}

முழு chase_and_open_parent_at(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, எழுத்து **ret_filename) {
        முழு pfd, r;

        ப்ரதிஜ்ஞா(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(வேர்_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு pfd;
}
