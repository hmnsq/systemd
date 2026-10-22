/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/magic.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "glyph-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "user-util.h"

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

സ്ഥിതം പൂർണ്ണസംഖ്യ chase_statx(പൂർണ്ണസംഖ്യ fd, ഘടന statx *ret) {
        തിരികെ xstatx_full(fd,
                        /* path= */ ശൂന്യ,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ chase_openat2(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        സ്ഥിതം bool can_openat2 = true;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));

        എങ്കിൽ (!can_openat2)
                തിരികെ -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        എങ്കിൽ ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                തിരികെ -EOPNOTSUPP;
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                തിരികെ -EOPNOTSUPP;
        എങ്കിൽ (വേര്_fd != XAT_FDROOT && വേര്_fd != dir_fd)
                തിരികെ -EOPNOTSUPP;

        _cleanup_close_ പൂർണ്ണസംഖ്യ dir_fd_local = -EBADF;
        എങ്കിൽ (dir_fd == XAT_FDROOT) {
                എങ്കിൽ (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                അല്ലെങ്കിൽ {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        എങ്കിൽ (dir_fd_local < 0)
                                തിരികെ -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ഘടന open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        എങ്കിൽ (വേര്_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_വേര്;
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = openat2(dir_fd, path, &how, വലുപ്പം(how));
        എങ്കിൽ (fd < 0) {
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
                എങ്കിൽ (errno == ENOSYS)
                        can_openat2 = false;
                എങ്കിൽ (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        തിരികെ -EOPNOTSUPP;
                തിരികെ -errno;
        }

        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ TAKE_FD(fd);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ chase_xopenat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        തിരികെ xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

സ്ഥിതം bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        എങ്കിൽ (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                തിരികെ false;

        തിരികെ a != b; /* Otherwise we need to stay within the same UID */
}

പൂർണ്ണസംഖ്യ statx_unsafe_transition(സ്ഥിരം ഘടന statx *a, സ്ഥിരം ഘടന statx *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        എങ്കിൽ (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                തിരികെ -ENODATA;

        തിരികെ uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ uid_unsafe_transition(a->st_uid, b->st_uid);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_unsafe_transition(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b, സ്ഥിരം അക്ഷരം *path, ChaseFlags flags) {
        _cleanup_free_ അക്ഷരം *n1 = ശൂന്യ, *n2 = ശൂന്യ, *user_a = ശൂന്യ, *user_b = ശൂന്യ;
        ഘടന stat st;

        എങ്കിൽ (!FLAGS_SET(flags, CHASE_WARN))
                തിരികെ -ENOLINK;

        (ശൂന്യം) fd_get_path(a, &n1);
        (ശൂന്യം) fd_get_path(b, &n2);

        എങ്കിൽ (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        എങ്കിൽ (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        തിരികെ log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_വലത്), strna(n2), strna(user_b), path);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_autofs_mount_point(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags flags) {
        _cleanup_free_ അക്ഷരം *n1 = ശൂന്യ;

        എങ്കിൽ (!FLAGS_SET(flags, CHASE_WARN))
                തിരികെ -EREMOTE;

        (ശൂന്യം) fd_get_path(fd, &n1);

        തിരികെ log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ log_prohibited_symlink(പൂർണ്ണസംഖ്യ fd, ChaseFlags flags) {
        _cleanup_free_ അക്ഷരം *n1 = ശൂന്യ;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (!FLAGS_SET(flags, CHASE_WARN))
                തിരികെ -ELOOP;

        (ശൂന്യം) fd_get_path(fd, &n1);

        തിരികെ log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

പൂർണ്ണസംഖ്യ chaseat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags flags, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!FLAGS_SET(flags, CHASE_PREFIX_വേര്));
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(വേര്_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        പ്രതിജ്ഞാ(dir_fd != AT_FDCWD || IN_SET(വേര്_fd, AT_FDCWD, XAT_FDROOT));

        എങ്കിൽ (FLAGS_SET(flags, CHASE_STEP))
                പ്രതിജ്ഞാ(!ret_fd);

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
        എങ്കിൽ (വേര്_fd == AT_FDCWD)
                വേര്_fd = XAT_FDROOT;
        അല്ലെങ്കിൽ {
                r = dir_fd_is_വേര്(വേര്_fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r > 0)
                        വേര്_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_വേര്(dir_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r > 0 && വേര്_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        എങ്കിൽ (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = വേര്_fd;

        എങ്കിൽ (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        എങ്കിൽ (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                എങ്കിൽ (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } അല്ലെങ്കിൽ എങ്കിൽ (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        എങ്കിൽ (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        എങ്കിൽ (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                തിരികെ -EBADSLT;

        എങ്കിൽ (!ret_path) {
                r = chase_openat2(വേര്_fd, dir_fd, path, flags);
                എങ്കിൽ (r >= 0) {
                        എങ്കിൽ (ret_fd)
                                *ret_fd = r;
                        അല്ലെങ്കിൽ
                                safe_close(r);

                        തിരികെ 1;
                }
                എങ്കിൽ (r != -EOPNOTSUPP)
                        തിരികെ r;
        }

        എങ്കിൽ (വേര്_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (ret_fd)
                        *ret_fd = r;
                അല്ലെങ്കിൽ
                        safe_close(r);

                തിരികെ 1;
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
        bool need_absolute = (വേര്_fd == XAT_FDROOT || dir_fd != വേര്_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ അക്ഷരം *done = ശൂന്യ;
        എങ്കിൽ (need_absolute) {
                done = strdup("/");
                എങ്കിൽ (!done)
                        തിരികെ -ENOMEM;
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = xopenat(dir_fd, ശൂന്യ, O_CLOEXEC|O_DIRECTORY|O_PATH);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        ഘടന statx stx;
        r = chase_statx(fd, &stx);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ഘടന statx വേര്_stx;
        എങ്കിൽ (വേര്_fd != XAT_FDROOT) {
                എങ്കിൽ (വേര്_fd == dir_fd)
                        വേര്_stx = stx;
                അല്ലെങ്കിൽ {
                        r = chase_statx(വേര്_fd, &വേര്_stx);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }
        }

        _cleanup_free_ അക്ഷരം *buffer = strdup(path);
        എങ്കിൽ (!buffer)
                തിരികെ -ENOMEM;

        സ്ഥിരം അക്ഷരം *todo = buffer;
        bool exists = true;
        വേണ്ടി (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ അക്ഷരം *first = ശൂന്യ;
                _cleanup_close_ പൂർണ്ണസംഖ്യ child = -EBADF;
                ഘടന statx stx_child;
                സ്ഥിരം അക്ഷരം *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                എങ്കിൽ (n_steps > CHASE_MAX)
                        തിരികെ -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0) /* We reached the end. */
                        നിർത്തുക;

                first = strndup(e, r);
                എങ്കിൽ (!first)
                        തിരികെ -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                എങ്കിൽ (streq(first, "..")) {
                        _cleanup_free_ അക്ഷരം *parent = ശൂന്യ;
                        _cleanup_close_ പൂർണ്ണസംഖ്യ fd_parent = -EBADF;
                        ഘടന statx stx_parent;

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
                        എങ്കിൽ (വേര്_fd != XAT_FDROOT) {
                                bool is_വേര് = വേര്_fd == dir_fd && empty_or_വേര്(done);
                                എങ്കിൽ (!is_വേര് && statx_inode_same(&stx, &വേര്_stx)) {
                                        r = statx_mount_same(&stx, &വേര്_stx);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ r;

                                        is_വേര് = r > 0;
                                }
                                എങ്കിൽ (is_വേര്) {
                                        എങ്കിൽ (FLAGS_SET(flags, CHASE_STEP))
                                                പോകുക chased_one;
                                        തുടരുക;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        എങ്കിൽ (fd_parent < 0)
                                തിരികെ -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        എങ്കിൽ (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_വേര്(fd);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                                എങ്കിൽ (r > 0) {
                                        എങ്കിൽ (FLAGS_SET(flags, CHASE_STEP))
                                                പോകുക chased_one;
                                        തുടരുക;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        എങ്കിൽ (r >= 0) {
                                പ്രതിജ്ഞാ(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } അല്ലെങ്കിൽ എങ്കിൽ (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                പ്രതിജ്ഞാ(!need_absolute);
                                done = mfree(done);
                        } അല്ലെങ്കിൽ എങ്കിൽ (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } അല്ലെങ്കിൽ എങ്കിൽ (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                പ്രതിജ്ഞാ(!need_absolute);

                                എങ്കിൽ (!isempty(done) && !path_is_valid(done))
                                        തിരികെ -EINVAL;

                                എങ്കിൽ (!path_extend(&done, ".."))
                                        തിരികെ -ENOMEM;
                        } അല്ലെങ്കിൽ
                                തിരികെ r;

                        എങ്കിൽ (FLAGS_SET(flags, CHASE_STEP))
                                പോകുക chased_one;

                        എങ്കിൽ (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                                എങ്കിൽ (r > 0)
                                        തിരികെ log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        എങ്കിൽ (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ പൂർണ്ണസംഖ്യ fd_grandparent = -EBADF;
                                ഘടന statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                എങ്കിൽ (fd_grandparent < 0)
                                        തിരികെ -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ r;
                                        എങ്കിൽ (r > 0)
                                                തിരികെ log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                നിർത്തുക;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        തുടരുക;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (r != -ENOENT)
                                തിരികെ r;

                        എങ്കിൽ (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                തിരികെ r;

                        എങ്കിൽ (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                എങ്കിൽ (child < 0)
                                        തിരികെ child;
                        } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                എങ്കിൽ (!path_extend(&done, first))
                                        തിരികെ -ENOMEM;

                                നിർത്തുക;
                        } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                എങ്കിൽ (!path_extend(&done, first, todo))
                                        തിരികെ -ENOMEM;

                                exists = false;
                                നിർത്തുക;
                        } അല്ലെങ്കിൽ
                                തിരികെ r;
                }

                r = chase_statx(child, &stx_child);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r > 0)
                                തിരികെ log_unsafe_transition(fd, child, path, flags);
                }

                എങ്കിൽ (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        തിരികെ log_autofs_mount_point(child, path, flags);

                എങ്കിൽ (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ അക്ഷരം *destination = ശൂന്യ;

                        എങ്കിൽ (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                തിരികെ log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (isempty(destination))
                                തിരികെ -EINVAL;

                        എങ്കിൽ (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(വേര്_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                എങ്കിൽ (fd < 0)
                                        തിരികെ fd;

                                r = chase_statx(fd, &stx);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ r;
                                        എങ്കിൽ (r > 0)
                                                തിരികെ log_unsafe_transition(child, fd, path, flags);
                                }

                                എങ്കിൽ (dir_fd != വേര്_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : ശൂന്യ);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        എങ്കിൽ (!path_extend(&destination, todo))
                                തിരികെ -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        എങ്കിൽ (FLAGS_SET(flags, CHASE_STEP))
                                പോകുക chased_one;

                        തുടരുക;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                എങ്കിൽ (!path_extend(&done, first))
                        തിരികെ -ENOMEM;

                എങ്കിൽ (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        നിർത്തുക;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        എങ്കിൽ (exists) {
                എങ്കിൽ (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }
        }

        എങ്കിൽ (ret_path) {
                എങ്കിൽ (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ അക്ഷരം *f = ശൂന്യ;

                        r = path_extract_filename(done, &f);
                        എങ്കിൽ (r < 0 && r != -EADDRNOTAVAIL)
                                തിരികെ r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                എങ്കിൽ (!done) {
                        പ്രതിജ്ഞാ(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        എങ്കിൽ (!done)
                                തിരികെ -ENOMEM;
                }

                എങ്കിൽ (append_trail_slash)
                        എങ്കിൽ (!strextend(&done, "/"))
                                തിരികെ -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        എങ്കിൽ (ret_fd) {
                എങ്കിൽ (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        പ്രതിജ്ഞാ(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } അല്ലെങ്കിൽ
                        *ret_fd = -EBADF;
        }

        എങ്കിൽ (FLAGS_SET(flags, CHASE_STEP))
                തിരികെ 1;

        തിരികെ exists;

chased_one:
        എങ്കിൽ (ret_path) {
                സ്ഥിരം അക്ഷരം *e;

                എങ്കിൽ (!done) {
                        പ്രതിജ്ഞാ(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        എങ്കിൽ (!done)
                                തിരികെ -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        *ret_path = TAKE_PTR(done);
                അല്ലെങ്കിൽ {
                        അക്ഷരം *c;

                        c = path_join(done, e);
                        എങ്കിൽ (!c)
                                തിരികെ -ENOMEM;

                        *ret_path = c;
                }
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags flags, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd) {
        _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ, *absolute = ശൂന്യ, *p = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF, pfd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        എങ്കിൽ (isempty(path))
                തിരികെ -EINVAL;

        r = empty_or_വേര്_harder_to_null(&വേര്);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* A root directory of "/" or "" is identical to "/". */
        എങ്കിൽ (empty_or_വേര്(വേര്))
                വേര് = "/";
        അല്ലെങ്കിൽ {
                r = path_make_absolute_cwd(വേര്, &വേര്_abs);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                വേര് = path_simplify(വേര്_abs);

                പ്രതിജ്ഞാ(path_is_absolute(വേര്));
                പ്രതിജ്ഞാ(!empty_or_വേര്(വേര്));

                എങ്കിൽ (FLAGS_SET(flags, CHASE_PREFIX_വേര്)) {
                        absolute = path_join(വേര്, path);
                        എങ്കിൽ (!absolute)
                                തിരികെ -ENOMEM;
                }
        }

        എങ്കിൽ (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        path = path_startswith(absolute, വേര്);
        എങ്കിൽ (!path)
                തിരികെ log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, വേര്);

        എങ്കിൽ (empty_or_വേര്(വേര്))
                fd = XAT_FDROOT;
        അല്ലെങ്കിൽ {
                fd = open(വേര്, O_CLOEXEC|O_DIRECTORY|O_PATH);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_വേര്, ret_path ? &p : ശൂന്യ, ret_fd ? &pfd : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path) {
                എങ്കിൽ (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        എങ്കിൽ (empty_or_വേര്(വേര്))
                                പ്രതിജ്ഞാ(path_is_absolute(p));
                        അല്ലെങ്കിൽ {
                                അക്ഷരം *q;

                                പ്രതിജ്ഞാ(!path_is_absolute(p));

                                q = path_join(വേര്, p + STR_IN_SET(p, ".", "./"));
                                എങ്കിൽ (!q)
                                        തിരികെ -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        എങ്കിൽ (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ chaseat_prefix_വേര്(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret) {
        അക്ഷരം *q;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        എങ്കിൽ (!path_is_absolute(path)) {
                _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;

                r = empty_or_വേര്_harder_to_null(&വേര്);
                എങ്കിൽ (r < 0 && r != -ENOENT)
                        തിരികെ r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                എങ്കിൽ (empty_or_വേര്(വേര്))
                        തിരികെ -EINVAL;

                r = path_make_absolute_cwd(വേര്, &വേര്_abs);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                വേര് = path_simplify(വേര്_abs);

                q = path_join(വേര്, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } അല്ലെങ്കിൽ
                q = strdup(path);
        എങ്കിൽ (!q)
                തിരികെ -ENOMEM;

        *ret = q;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_extract_filename(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (isempty(path))
                തിരികെ -EINVAL;

        എങ്കിൽ (!path_is_absolute(path))
                തിരികെ -EINVAL;

        r = empty_or_വേര്_harder_to_null(&വേര്);
        എങ്കിൽ (r < 0 && r != -ENOENT)
                തിരികെ r;

        എങ്കിൽ (!empty_or_വേര്(വേര്)) {
                _cleanup_free_ അക്ഷരം *വേര്_abs = ശൂന്യ;

                r = path_make_absolute_cwd(വേര്, &വേര്_abs);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                path = path_startswith(path, വേര്_abs);
                എങ്കിൽ (!path)
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (!isempty(path)) {
                r = path_extract_filename(path, ret);
                എങ്കിൽ (r != -EADDRNOTAVAIL)
                        തിരികെ r;
        }

        തിരികെ strdup_to(ret, ".");
}

പൂർണ്ണസംഖ്യ chase_and_open(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *വേര്,
                ChaseFlags chase_flags,
                പൂർണ്ണസംഖ്യ open_flags,
                അക്ഷരം **ret_path) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *fname = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *open_name = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        എങ്കിൽ (empty_or_വേര്(വേര്) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                തിരികെ chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, വേര്, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        എങ്കിൽ (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                അല്ലെങ്കിൽ {
                        r = chase_extract_filename(p, വേര്, &fname);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ chase_and_opendir(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, അക്ഷരം **ret_path, DIR **ret_dir) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        DIR *d;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        പ്രതിജ്ഞാ(ret_dir);

        എങ്കിൽ (empty_or_വേര്(വേര്) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                എങ്കിൽ (!d)
                        തിരികെ -errno;

                *ret_dir = d;
                തിരികെ 0;
        }

        r = chase(path, വേര്, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : ശൂന്യ, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ശൂന്യ, /* flags= */ 0);
        എങ്കിൽ (!d)
                തിരികെ -errno;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_stat(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, അക്ഷരം **ret_path, ഘടന stat *ret_stat) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        പ്രതിജ്ഞാ(ret_stat);

        എങ്കിൽ (empty_or_വേര്(വേര്) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                തിരികെ RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, വേര്, chase_flags, ret_path ? &p : ശൂന്യ, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        എങ്കിൽ (fstat(path_fd, ret_stat) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_access(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ access_mode, അക്ഷരം **ret_path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        എങ്കിൽ (empty_or_വേര്(വേര്) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                തിരികെ RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, വേര്, chase_flags, ret_path ? &p : ശൂന്യ, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_fopen_unlocked(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *വേര്,
                ChaseFlags chase_flags,
                സ്ഥിരം അക്ഷരം *open_flags,
                അക്ഷരം **ret_path,
                FILE **ret_file) {

        _cleanup_free_ അക്ഷരം *final_path = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ mode_flags, r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        പ്രതിജ്ഞാ(open_flags);
        പ്രതിജ്ഞാ(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        എങ്കിൽ (mode_flags < 0)
                തിരികെ mode_flags;

        fd = chase_and_open(path, വേര്, chase_flags, mode_flags, ret_path ? &final_path : ശൂന്യ);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(final_path);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_unlink(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ unlink_flags, അക്ഷരം **ret_path) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *fname = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, വേര്, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        r = path_extract_filename(p, &fname);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (unlinkat(fd, fname, unlink_flags) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_open_parent(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, അക്ഷരം **ret_filename) {
        പൂർണ്ണസംഖ്യ pfd, r;

        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, വേര്, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ pfd;
}

പൂർണ്ണസംഖ്യ chase_and_openat(
                പൂർണ്ണസംഖ്യ വേര്_fd,
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                ChaseFlags chase_flags,
                പൂർണ്ണസംഖ്യ open_flags,
                അക്ഷരം **ret_path) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *fname = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *open_name = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        എങ്കിൽ (വേര്_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                തിരികെ chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(വേര്_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                എങ്കിൽ (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                അല്ലെങ്കിൽ {
                        r = path_extract_filename(p, &fname);
                        എങ്കിൽ (r < 0 && r != -EADDRNOTAVAIL)
                                തിരികെ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ chase_and_opendirat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, അക്ഷരം **ret_path, DIR **ret_dir) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        DIR *d;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        പ്രതിജ്ഞാ(ret_dir);

        എങ്കിൽ (വേര്_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                എങ്കിൽ (!d)
                        തിരികെ -errno;

                *ret_dir = d;
                തിരികെ 0;
        }

        r = chaseat(വേര്_fd, dir_fd, path, chase_flags, ret_path ? &p : ശൂന്യ, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ശൂന്യ, /* flags= */ 0);
        എങ്കിൽ (!d)
                തിരികെ -errno;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_statat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, അക്ഷരം **ret_path, ഘടന stat *ret_stat) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        പ്രതിജ്ഞാ(ret_stat);

        എങ്കിൽ (വേര്_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                തിരികെ RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(വേര്_fd, dir_fd, path, chase_flags, ret_path ? &p : ശൂന്യ, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        എങ്കിൽ (fstat(path_fd, ret_stat) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_accessat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ access_mode, അക്ഷരം **ret_path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ path_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        എങ്കിൽ (വേര്_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                തിരികെ RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(വേര്_fd, dir_fd, path, chase_flags, ret_path ? &p : ശൂന്യ, &path_fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_fopenat_unlocked(
                പൂർണ്ണസംഖ്യ വേര്_fd,
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                ChaseFlags chase_flags,
                സ്ഥിരം അക്ഷരം *open_flags,
                അക്ഷരം **ret_path,
                FILE **ret_file) {

        _cleanup_free_ അക്ഷരം *final_path = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ mode_flags, r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        പ്രതിജ്ഞാ(open_flags);
        പ്രതിജ്ഞാ(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        എങ്കിൽ (mode_flags < 0)
                തിരികെ mode_flags;

        fd = chase_and_openat(വേര്_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : ശൂന്യ);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(final_path);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_unlinkat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ unlink_flags, അക്ഷരം **ret_path) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *fname = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(വേര്_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        r = path_extract_filename(p, &fname);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (unlinkat(fd, fname, unlink_flags) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(p);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ chase_and_open_parent_at(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, അക്ഷരം **ret_filename) {
        പൂർണ്ണസംഖ്യ pfd, r;

        പ്രതിജ്ഞാ(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(വേര്_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ pfd;
}
