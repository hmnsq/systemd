/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/magic.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "glyph-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "user-util.h"

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
#నిర్వచించు CHASE_NO_SHORTCUT_ముసుగు                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_సురక్షిత |                                   \
         CHASE_అడుగు |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_మాతృ |                                 \
         CHASE_MKDIR_0755)

#నిర్వచించు CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

స్థిర పూర్ణాంకం chase_statx(పూర్ణాంకం fd, నిర్మాణం statx *ret) {
        తిరిగి xstatx_నిండిన(fd,
                        /* path= */ ఖాళీ,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_ఉత్తమ,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

స్థిర పూర్ణాంకం chase_openat2(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        స్థిర bool can_openat2 = true;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));

        ఒకవేళ (!can_openat2)
                తిరిగి -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        ఒకవేళ ((chase_flags & (CHASE_NO_SHORTCUT_ముసుగు & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                తిరిగి -EOPNOTSUPP;
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_TRIGGER_AUTOFS))
                తిరిగి -EOPNOTSUPP;
        ఒకవేళ (మూలం_fd != XAT_FDROOT && మూలం_fd != dir_fd)
                తిరిగి -EOPNOTSUPP;

        _cleanup_close_ పూర్ణాంకం dir_fd_local = -EBADF;
        ఒకవేళ (dir_fd == XAT_FDROOT) {
                ఒకవేళ (మార్గం_is_absolute(మార్గం))
                        dir_fd = AT_FDCWD;
                లేకపోతే {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_మార్గం);
                        ఒకవేళ (dir_fd_local < 0)
                                తిరిగి -errno;
                        dir_fd = dir_fd_local;
                }
        }

        నిర్మాణం open_how how = {
                .flags = O_మార్గం|O_CLOEXEC,
        };
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        ఒకవేళ (మూలం_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_లోపల_మూలం;
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ పూర్ణాంకం fd = openat2(dir_fd, మార్గం, &how, పరిమాణం(how));
        ఒకవేళ (fd < 0) {
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
                ఒకవేళ (errno == ENOSYS)
                        can_openat2 = false;
                ఒకవేళ (లోపల_సమితి(errno, ENOSYS, EPERM, EAGAIN))
                        తిరిగి -EOPNOTSUPP;
                తిరిగి -errno;
        }

        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి TAKE_FD(fd);
}

స్థిర పూర్ణాంకం chase_xopenat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, పూర్ణాంకం open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_సమితి(open_flags, O_మార్గం))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        తిరిగి xopenat_నిండిన(dir_fd, మార్గం, open_flags, xopen_flags, బహుళకం_INVALID);
}

స్థిర bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        ఒకవేళ (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                తిరిగి false;

        తిరిగి a != b; /* Otherwise we need to stay within the same UID */
}

పూర్ణాంకం statx_unsafe_transition(స్థిరరాశి నిర్మాణం statx *a, స్థిరరాశి నిర్మాణం statx *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        ఒకవేళ (!FLAGS_సమితి(a->stx_ముసుగు, STATX_UID) || !FLAGS_సమితి(b->stx_ముసుగు, STATX_UID))
                తిరిగి -ENODATA;

        తిరిగి uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి uid_unsafe_transition(a->st_uid, b->st_uid);
}

స్థిర పూర్ణాంకం log_unsafe_transition(పూర్ణాంకం a, పూర్ణాంకం b, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags flags) {
        _cleanup_free_ అక్షరం *n1 = ఖాళీ, *n2 = ఖాళీ, *user_a = ఖాళీ, *user_b = ఖాళీ;
        నిర్మాణం stat st;

        ఒకవేళ (!FLAGS_సమితి(flags, CHASE_WARN))
                తిరిగి -ENOLINK;

        (శూన్య) fd_get_మార్గం(a, &n1);
        (శూన్య) fd_get_మార్గం(b, &n2);

        ఒకవేళ (fstat(a, &st) == 0)
                user_a = uid_గమ్యం_name(st.st_uid);
        ఒకవేళ (fstat(b, &st) == 0)
                user_b = uid_గమ్యం_name(st.st_uid);

        తిరిగి log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_కుడి), strna(n2), strna(user_b), మార్గం);
}

స్థిర పూర్ణాంకం log_autofs_mount_బిందువు(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags flags) {
        _cleanup_free_ అక్షరం *n1 = ఖాళీ;

        ఒకవేళ (!FLAGS_సమితి(flags, CHASE_WARN))
                తిరిగి -EREMOTE;

        (శూన్య) fd_get_మార్గం(fd, &n1);

        తిరిగి log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), మార్గం);
}

స్థిర పూర్ణాంకం log_prohibited_symlink(పూర్ణాంకం fd, ChaseFlags flags) {
        _cleanup_free_ అక్షరం *n1 = ఖాళీ;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (!FLAGS_సమితి(flags, CHASE_WARN))
                తిరిగి -ELOOP;

        (శూన్య) fd_get_మార్గం(fd, &n1);

        తిరిగి log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

పూర్ణాంకం chaseat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags flags, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, CHASE_ఉపసర్గ_మూలం));
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, CHASE_అడుగు|CHASE_వెలికితీయి_FILENAME));
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(wildcard_fd_is_valid(మూలం_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        ప్రతిజ్ఞా(dir_fd != AT_FDCWD || లోపల_సమితి(మూలం_fd, AT_FDCWD, XAT_FDROOT));

        ఒకవేళ (FLAGS_సమితి(flags, CHASE_అడుగు))
                ప్రతిజ్ఞా(!ret_fd);

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
        ఒకవేళ (మూలం_fd == AT_FDCWD)
                మూలం_fd = XAT_FDROOT;
        లేకపోతే {
                r = dir_fd_is_మూలం(మూలం_fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        మూలం_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_మూలం(dir_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r > 0 && మూలం_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        ఒకవేళ (dir_fd == XAT_FDROOT || మార్గం_is_absolute(మార్గం))
                dir_fd = మూలం_fd;

        ఒకవేళ (isempty(మార్గం))
                మార్గం = ".";

        bool append_trail_slash = false;
        ఒకవేళ (ENDSWITH_సమితి(మార్గం, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                ఒకవేళ (FLAGS_సమితి(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } లేకపోతే ఒకవేళ (dot_or_dot_dot(మార్గం) || endswith(మార్గం, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        ఒకవేళ (FLAGS_సమితి(flags, CHASE_మాతృ))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        ఒకవేళ (FLAGS_సమితి(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_సమితి(flags, CHASE_MUST_BE_REGULAR) + FLAGS_సమితి(flags, CHASE_MUST_BE_SOCKET) > 1)
                తిరిగి -EBADSLT;

        ఒకవేళ (!ret_మార్గం) {
                r = chase_openat2(మూలం_fd, dir_fd, మార్గం, flags);
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (ret_fd)
                                *ret_fd = r;
                        లేకపోతే
                                సురక్షిత_close(r);

                        తిరిగి 1;
                }
                ఒకవేళ (r != -EOPNOTSUPP)
                        తిరిగి r;
        }

        ఒకవేళ (మూలం_fd == XAT_FDROOT && !ret_మార్గం && (flags & CHASE_NO_SHORTCUT_ముసుగు) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, మార్గం, flags, O_మార్గం|O_CLOEXEC, /* xopen_flags= */ 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (ret_fd)
                        *ret_fd = r;
                లేకపోతే
                        సురక్షిత_close(r);

                తిరిగి 1;
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
        bool need_absolute = (మూలం_fd == XAT_FDROOT || dir_fd != మూలం_fd) && (dir_fd == XAT_FDROOT || మార్గం_is_absolute(మార్గం));

        _cleanup_free_ అక్షరం *పూర్తి = ఖాళీ;
        ఒకవేళ (need_absolute) {
                పూర్తి = strdup("/");
                ఒకవేళ (!పూర్తి)
                        తిరిగి -ENOMEM;
        }

        _cleanup_close_ పూర్ణాంకం fd = xopenat(dir_fd, ఖాళీ, O_CLOEXEC|O_DIRECTORY|O_మార్గం);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        నిర్మాణం statx stx;
        r = chase_statx(fd, &stx);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        నిర్మాణం statx మూలం_stx;
        ఒకవేళ (మూలం_fd != XAT_FDROOT) {
                ఒకవేళ (మూలం_fd == dir_fd)
                        మూలం_stx = stx;
                లేకపోతే {
                        r = chase_statx(మూలం_fd, &మూలం_stx);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }
        }

        _cleanup_free_ అక్షరం *బఫరు = strdup(మార్గం);
        ఒకవేళ (!బఫరు)
                తిరిగి -ENOMEM;

        స్థిరరాశి అక్షరం *todo = బఫరు;
        bool exists = true;
        కొరకు (గుర్తులేని n_అడుగులు = 0;; n_అడుగులు++) {
                _cleanup_free_ అక్షరం *మొదటి = ఖాళీ;
                _cleanup_close_ పూర్ణాంకం బిడ్డ = -EBADF;
                నిర్మాణం statx stx_బిడ్డ;
                స్థిరరాశి అక్షరం *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                ఒకవేళ (n_అడుగులు > CHASE_MAX)
                        తిరిగి -ELOOP;

                r = మార్గం_కనుగొను_మొదటి_component(&todo, /* accept_dot_dot= */ true, &e);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0) /* We reached the end. */
                        ఆపు;

                మొదటి = strndup(e, r);
                ఒకవేళ (!మొదటి)
                        తిరిగి -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                ఒకవేళ (streq(మొదటి, "..")) {
                        _cleanup_free_ అక్షరం *మాతృ = ఖాళీ;
                        _cleanup_close_ పూర్ణాంకం fd_మాతృ = -EBADF;
                        నిర్మాణం statx stx_మాతృ;

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
                        ఒకవేళ (మూలం_fd != XAT_FDROOT) {
                                bool is_మూలం = మూలం_fd == dir_fd && ఖాళీలు_or_మూలం(పూర్తి);
                                ఒకవేళ (!is_మూలం && statx_inode_same(&stx, &మూలం_stx)) {
                                        r = statx_mount_same(&stx, &మూలం_stx);
                                        ఒకవేళ (r < 0)
                                                తిరిగి r;

                                        is_మూలం = r > 0;
                                }
                                ఒకవేళ (is_మూలం) {
                                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_అడుగు))
                                                వెళ్ళు chased_one;
                                        కొనసాగు;
                                }
                        }

                        fd_మాతృ = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_మార్గం|O_DIRECTORY);
                        ఒకవేళ (fd_మాతృ < 0)
                                తిరిగి -errno;

                        r = chase_statx(fd_మాతృ, &stx_మాతృ);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        ఒకవేళ (statx_inode_same(&stx_మాతృ, &stx)) {
                                r = dir_fd_is_మూలం(fd);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                                ఒకవేళ (r > 0) {
                                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_అడుగు))
                                                వెళ్ళు chased_one;
                                        కొనసాగు;
                                }
                        }

                        r = మార్గం_వెలికితీయి_directory(పూర్తి, &మాతృ);
                        ఒకవేళ (r >= 0) {
                                ప్రతిజ్ఞా(!need_absolute || మార్గం_is_absolute(మాతృ));
                                free_and_replace(పూర్తి, మాతృ);
                        } లేకపోతే ఒకవేళ (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                ప్రతిజ్ఞా(!need_absolute);
                                పూర్తి = mfree(పూర్తి);
                        } లేకపోతే ఒకవేళ (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } లేకపోతే ఒకవేళ (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                ప్రతిజ్ఞా(!need_absolute);

                                ఒకవేళ (!isempty(పూర్తి) && !మార్గం_is_valid(పూర్తి))
                                        తిరిగి -EINVAL;

                                ఒకవేళ (!మార్గం_extend(&పూర్తి, ".."))
                                        తిరిగి -ENOMEM;
                        } లేకపోతే
                                తిరిగి r;

                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_అడుగు))
                                వెళ్ళు chased_one;

                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_సురక్షిత)) {
                                r = statx_unsafe_transition(&stx, &stx_మాతృ);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                                ఒకవేళ (r > 0)
                                        తిరిగి log_unsafe_transition(fd, fd_మాతృ, మార్గం, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_మాతృ) && isempty(todo)) {
                                _cleanup_close_ పూర్ణాంకం fd_grandparent = -EBADF;
                                నిర్మాణం statx stx_grandparent;

                                fd_grandparent = openat(fd_మాతృ, "..", O_CLOEXEC|O_NOFOLLOW|O_మార్గం|O_DIRECTORY);
                                ఒకవేళ (fd_grandparent < 0)
                                        తిరిగి -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (FLAGS_సమితి(flags, CHASE_సురక్షిత)) {
                                        r = statx_unsafe_transition(&stx_మాతృ, &stx_grandparent);
                                        ఒకవేళ (r < 0)
                                                తిరిగి r;
                                        ఒకవేళ (r > 0)
                                                తిరిగి log_unsafe_transition(fd_మాతృ, fd_grandparent, మార్గం, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                ఆపు;
                        }

                        /* update fd and stat */
                        stx = stx_మాతృ;
                        close_and_replace(fd, fd_మాతృ);
                        కొనసాగు;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                బిడ్డ = r = xopenat_నిండిన(fd, మొదటి,
                                         O_మార్గం|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_సమితి(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         బహుళకం_INVALID);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (r != -ENOENT)
                                తిరిగి r;

                        ఒకవేళ (!isempty(todo) && !మార్గం_is_సురక్షిత(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                తిరిగి r;

                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_మాతృ|CHASE_NONEXISTENT)))) {
                                బిడ్డ = xopenat(fd,
                                                మొదటి,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_మార్గం|O_CLOEXEC);
                                ఒకవేళ (బిడ్డ < 0)
                                        తిరిగి బిడ్డ;
                        } లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, CHASE_మాతృ) && isempty(todo)) {
                                ఒకవేళ (!మార్గం_extend(&పూర్తి, మొదటి))
                                        తిరిగి -ENOMEM;

                                ఆపు;
                        } లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, CHASE_NONEXISTENT)) {
                                ఒకవేళ (!మార్గం_extend(&పూర్తి, మొదటి, todo))
                                        తిరిగి -ENOMEM;

                                exists = false;
                                ఆపు;
                        } లేకపోతే
                                తిరిగి r;
                }

                r = chase_statx(బిడ్డ, &stx_బిడ్డ);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (FLAGS_సమితి(flags, CHASE_సురక్షిత)) {
                        r = statx_unsafe_transition(&stx, &stx_బిడ్డ);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r > 0)
                                తిరిగి log_unsafe_transition(fd, బిడ్డ, మార్గం, flags);
                }

                ఒకవేళ (FLAGS_సమితి(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(బిడ్డ, AUTOFS_SUPER_MAGIC) > 0)
                        తిరిగి log_autofs_mount_బిందువు(బిడ్డ, మార్గం, flags);

                ఒకవేళ (S_ISLNK(stx_బిడ్డ.stx_బహుళకం) && !(FLAGS_సమితి(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ అక్షరం *destination = ఖాళీ;

                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_PROHIBIT_SYMLINKS))
                                తిరిగి log_prohibited_symlink(బిడ్డ, flags);

                        r = readlinkat_malloc(fd, మొదటి, &destination);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (isempty(destination))
                                తిరిగి -EINVAL;

                        ఒకవేళ (మార్గం_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                సురక్షిత_close(fd);
                                fd = fd_reopen(మూలం_fd, O_CLOEXEC|O_మార్గం|O_DIRECTORY);
                                ఒకవేళ (fd < 0)
                                        తిరిగి fd;

                                r = chase_statx(fd, &stx);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (FLAGS_సమితి(flags, CHASE_సురక్షిత)) {
                                        r = statx_unsafe_transition(&stx_బిడ్డ, &stx);
                                        ఒకవేళ (r < 0)
                                                తిరిగి r;
                                        ఒకవేళ (r > 0)
                                                తిరిగి log_unsafe_transition(బిడ్డ, fd, మార్గం, flags);
                                }

                                ఒకవేళ (dir_fd != మూలం_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&పూర్తి, need_absolute ? "/" : ఖాళీ);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        ఒకవేళ (!మార్గం_extend(&destination, todo))
                                తిరిగి -ENOMEM;

                        free_and_replace(బఫరు, destination);
                        todo = బఫరు;

                        ఒకవేళ (FLAGS_సమితి(flags, CHASE_అడుగు))
                                వెళ్ళు chased_one;

                        కొనసాగు;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                ఒకవేళ (!మార్గం_extend(&పూర్తి, మొదటి))
                        తిరిగి -ENOMEM;

                ఒకవేళ (FLAGS_సమితి(flags, CHASE_మాతృ) && isempty(todo))
                        ఆపు;

                /* And iterate again, but go one directory further down. */
                stx = stx_బిడ్డ;
                close_and_replace(fd, బిడ్డ);
        }

        ఒకవేళ (exists) {
                ఒకవేళ (FLAGS_సమితి(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (FLAGS_సమితి(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (FLAGS_సమితి(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }
        }

        ఒకవేళ (ret_మార్గం) {
                ఒకవేళ (FLAGS_సమితి(flags, CHASE_వెలికితీయి_FILENAME) && పూర్తి) {
                        _cleanup_free_ అక్షరం *f = ఖాళీ;

                        r = మార్గం_వెలికితీయి_filename(పూర్తి, &f);
                        ఒకవేళ (r < 0 && r != -EADDRNOTAVAIL)
                                తిరిగి r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(పూర్తి, f);
                }

                ఒకవేళ (!పూర్తి) {
                        ప్రతిజ్ఞా(!need_absolute || FLAGS_సమితి(flags, CHASE_వెలికితీయి_FILENAME));
                        పూర్తి = strdup(".");
                        ఒకవేళ (!పూర్తి)
                                తిరిగి -ENOMEM;
                }

                ఒకవేళ (append_trail_slash)
                        ఒకవేళ (!strextend(&పూర్తి, "/"))
                                తిరిగి -ENOMEM;

                *ret_మార్గం = TAKE_PTR(పూర్తి);
        }

        ఒకవేళ (ret_fd) {
                ఒకవేళ (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        ప్రతిజ్ఞా(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } లేకపోతే
                        *ret_fd = -EBADF;
        }

        ఒకవేళ (FLAGS_సమితి(flags, CHASE_అడుగు))
                తిరిగి 1;

        తిరిగి exists;

chased_one:
        ఒకవేళ (ret_మార్గం) {
                స్థిరరాశి అక్షరం *e;

                ఒకవేళ (!పూర్తి) {
                        ప్రతిజ్ఞా(!need_absolute);
                        పూర్తి = strdup(append_trail_slash ? "./" : ".");
                        ఒకవేళ (!పూర్తి)
                                తిరిగి -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = మార్గం_కనుగొను_మొదటి_component(&todo, /* accept_dot_dot= */ true, &e);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        *ret_మార్గం = TAKE_PTR(పూర్తి);
                లేకపోతే {
                        అక్షరం *c;

                        c = మార్గం_join(పూర్తి, e);
                        ఒకవేళ (!c)
                                తిరిగి -ENOMEM;

                        *ret_మార్గం = c;
                }
        }

        తిరిగి 0;
}

పూర్ణాంకం chase(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags flags, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd) {
        _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ, *absolute = ఖాళీ, *p = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF, pfd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        ఒకవేళ (isempty(మార్గం))
                తిరిగి -EINVAL;

        r = ఖాళీలు_or_మూలం_harder_గమ్యం_null(&మూలం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* A root directory of "/" or "" is identical to "/". */
        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం))
                మూలం = "/";
        లేకపోతే {
                r = మార్గం_make_absolute_cwd(మూలం, &మూలం_abs);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                మూలం = మార్గం_simplify(మూలం_abs);

                ప్రతిజ్ఞా(మార్గం_is_absolute(మూలం));
                ప్రతిజ్ఞా(!ఖాళీలు_or_మూలం(మూలం));

                ఒకవేళ (FLAGS_సమితి(flags, CHASE_ఉపసర్గ_మూలం)) {
                        absolute = మార్గం_join(మూలం, మార్గం);
                        ఒకవేళ (!absolute)
                                తిరిగి -ENOMEM;
                }
        }

        ఒకవేళ (!absolute) {
                r = మార్గం_make_absolute_cwd(మార్గం, &absolute);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        మార్గం = మార్గం_startswith(absolute, మూలం);
        ఒకవేళ (!మార్గం)
                తిరిగి log_నిండిన_errno(FLAGS_సమితి(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, మూలం);

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం))
                fd = XAT_FDROOT;
        లేకపోతే {
                fd = open(మూలం, O_CLOEXEC|O_DIRECTORY|O_మార్గం);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;
        }

        r = chaseat(fd, fd, మార్గం, flags & ~CHASE_ఉపసర్గ_మూలం, ret_మార్గం ? &p : ఖాళీ, ret_fd ? &pfd : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం) {
                ఒకవేళ (!FLAGS_సమితి(flags, CHASE_వెలికితీయి_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం))
                                ప్రతిజ్ఞా(మార్గం_is_absolute(p));
                        లేకపోతే {
                                అక్షరం *q;

                                ప్రతిజ్ఞా(!మార్గం_is_absolute(p));

                                q = మార్గం_join(మూలం, p + STR_లోపల_సమితి(p, ".", "./"));
                                ఒకవేళ (!q)
                                        తిరిగి -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_మార్గం = TAKE_PTR(p);
        }

        ఒకవేళ (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        తిరిగి r;
}

పూర్ణాంకం chaseat_ఉపసర్గ_మూలం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret) {
        అక్షరం *q;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        ఒకవేళ (!మార్గం_is_absolute(మార్గం)) {
                _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;

                r = ఖాళీలు_or_మూలం_harder_గమ్యం_null(&మూలం);
                ఒకవేళ (r < 0 && r != -ENOENT)
                        తిరిగి r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                ఒకవేళ (ఖాళీలు_or_మూలం(మూలం))
                        తిరిగి -EINVAL;

                r = మార్గం_make_absolute_cwd(మూలం, &మూలం_abs);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                మూలం = మార్గం_simplify(మూలం_abs);

                q = మార్గం_join(మూలం, మార్గం + (మార్గం[0] == '.' && లోపల_సమితి(మార్గం[1], '/', '\0')));
        } లేకపోతే
                q = strdup(మార్గం);
        ఒకవేళ (!q)
                తిరిగి -ENOMEM;

        *ret = q;
        తిరిగి 0;
}

పూర్ణాంకం chase_వెలికితీయి_filename(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret) {
        పూర్ణాంకం r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (isempty(మార్గం))
                తిరిగి -EINVAL;

        ఒకవేళ (!మార్గం_is_absolute(మార్గం))
                తిరిగి -EINVAL;

        r = ఖాళీలు_or_మూలం_harder_గమ్యం_null(&మూలం);
        ఒకవేళ (r < 0 && r != -ENOENT)
                తిరిగి r;

        ఒకవేళ (!ఖాళీలు_or_మూలం(మూలం)) {
                _cleanup_free_ అక్షరం *మూలం_abs = ఖాళీ;

                r = మార్గం_make_absolute_cwd(మూలం, &మూలం_abs);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                మార్గం = మార్గం_startswith(మార్గం, మూలం_abs);
                ఒకవేళ (!మార్గం)
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (!isempty(మార్గం)) {
                r = మార్గం_వెలికితీయి_filename(మార్గం, ret);
                ఒకవేళ (r != -EADDRNOTAVAIL)
                        తిరిగి r;
        }

        తిరిగి strdup_గమ్యం(ret, ".");
}

పూర్ణాంకం chase_and_open(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *మూలం,
                ChaseFlags chase_flags,
                పూర్ణాంకం open_flags,
                అక్షరం **ret_మార్గం) {

        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ, *fname = ఖాళీ;
        స్థిరరాశి అక్షరం *open_name = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం) && !ret_మార్గం && (chase_flags & CHASE_NO_SHORTCUT_ముసుగు) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                తిరిగి chase_xopenat(AT_FDCWD, మార్గం, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(మార్గం, మూలం, (CHASE_మాతృ|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        ఒకవేళ (!FLAGS_సమితి(chase_flags, CHASE_మాతృ)) {
                ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_వెలికితీయి_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                లేకపోతే {
                        r = chase_వెలికితీయి_filename(p, మూలం, &fname);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(మార్గం_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి r;
}

పూర్ణాంకం chase_and_opendir(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, DIR **ret_dir) {
        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        DIR *d;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ప్రతిజ్ఞా(ret_dir);

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం) && !ret_మార్గం && (chase_flags & CHASE_NO_SHORTCUT_ముసుగు) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(మార్గం);
                ఒకవేళ (!d)
                        తిరిగి -errno;

                *ret_dir = d;
                తిరిగి 0;
        }

        r = chase(మార్గం, మూలం, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_మార్గం ? &p : ఖాళీ, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        d = xopendirat(మార్గం_fd, /* path= */ ఖాళీ, /* flags= */ 0);
        ఒకవేళ (!d)
                తిరిగి -errno;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        *ret_dir = d;
        తిరిగి 0;
}

పూర్ణాంకం chase_and_stat(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, నిర్మాణం stat *ret_stat) {
        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));
        ప్రతిజ్ఞా(ret_stat);

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం) && !ret_మార్గం && (chase_flags & (CHASE_NO_SHORTCUT_ముసుగు|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                తిరిగి RET_NERRNO(fstatat(AT_FDCWD, మార్గం, ret_stat,
                                          FLAGS_సమితి(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(మార్గం, మూలం, chase_flags, ret_మార్గం ? &p : ఖాళీ, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        ఒకవేళ (fstat(మార్గం_fd, ret_stat) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_access(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, పూర్ణాంకం access_బహుళకం, అక్షరం **ret_మార్గం) {
        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం) && !ret_మార్గం && (chase_flags & (CHASE_NO_SHORTCUT_ముసుగు|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                తిరిగి RET_NERRNO(faccessat(AT_FDCWD, మార్గం, access_బహుళకం,
                                            FLAGS_సమితి(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(మార్గం, మూలం, chase_flags, ret_మార్గం ? &p : ఖాళీ, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        r = access_fd(మార్గం_fd, access_బహుళకం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_fopen_unlocked(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *మూలం,
                ChaseFlags chase_flags,
                స్థిరరాశి అక్షరం *open_flags,
                అక్షరం **ret_మార్గం,
                FILE **ret_file) {

        _cleanup_free_ అక్షరం *final_మార్గం = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం బహుళకం_flags, r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_మాతృ|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        ప్రతిజ్ఞా(open_flags);
        ప్రతిజ్ఞా(ret_file);

        బహుళకం_flags = fopen_బహుళకం_గమ్యం_flags(open_flags);
        ఒకవేళ (బహుళకం_flags < 0)
                తిరిగి బహుళకం_flags;

        fd = chase_and_open(మార్గం, మూలం, chase_flags, బహుళకం_flags, ret_మార్గం ? &final_మార్గం : ఖాళీ);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(final_మార్గం);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_unlink(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, పూర్ణాంకం unlink_flags, అక్షరం **ret_మార్గం) {
        _cleanup_free_ అక్షరం *p = ఖాళీ, *fname = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_మాతృ|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_వెలికితీయి_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(మార్గం, మూలం, chase_flags|CHASE_మాతృ|CHASE_NOFOLLOW, O_మార్గం|O_DIRECTORY|O_CLOEXEC, &p);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        r = మార్గం_వెలికితీయి_filename(p, &fname);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (unlinkat(fd, fname, unlink_flags) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_open_మాతృ(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, అక్షరం **ret_filename) {
        పూర్ణాంకం pfd, r;

        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));

        r = chase(మార్గం, మూలం, CHASE_మాతృ|CHASE_వెలికితీయి_FILENAME|chase_flags, ret_filename, &pfd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి pfd;
}

పూర్ణాంకం chase_and_openat(
                పూర్ణాంకం మూలం_fd,
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                ChaseFlags chase_flags,
                పూర్ణాంకం open_flags,
                అక్షరం **ret_మార్గం) {

        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ, *fname = ఖాళీ;
        స్థిరరాశి అక్షరం *open_name = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_MUST_BE_SOCKET)));

        ఒకవేళ (మూలం_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_మార్గం && (chase_flags & CHASE_NO_SHORTCUT_ముసుగు) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                తిరిగి chase_xopenat(dir_fd, మార్గం, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(మూలం_fd, dir_fd, మార్గం, (chase_flags|CHASE_మాతృ)&~CHASE_MUST_BE_REGULAR, &p, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!FLAGS_సమితి(chase_flags, CHASE_మాతృ)) {
                ఒకవేళ (FLAGS_సమితి(chase_flags, CHASE_వెలికితీయి_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                లేకపోతే {
                        r = మార్గం_వెలికితీయి_filename(p, &fname);
                        ఒకవేళ (r < 0 && r != -EADDRNOTAVAIL)
                                తిరిగి r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(మార్గం_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి r;
}

పూర్ణాంకం chase_and_opendirat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, DIR **ret_dir) {
        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        DIR *d;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ప్రతిజ్ఞా(ret_dir);

        ఒకవేళ (మూలం_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_మార్గం && (chase_flags & CHASE_NO_SHORTCUT_ముసుగు) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(మార్గం);
                ఒకవేళ (!d)
                        తిరిగి -errno;

                *ret_dir = d;
                తిరిగి 0;
        }

        r = chaseat(మూలం_fd, dir_fd, మార్గం, chase_flags, ret_మార్గం ? &p : ఖాళీ, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        d = xopendirat(మార్గం_fd, /* path= */ ఖాళీ, /* flags= */ 0);
        ఒకవేళ (!d)
                తిరిగి -errno;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        *ret_dir = d;
        తిరిగి 0;
}

పూర్ణాంకం chase_and_statat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, నిర్మాణం stat *ret_stat) {
        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));
        ప్రతిజ్ఞా(ret_stat);

        ఒకవేళ (మూలం_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_మార్గం && (chase_flags & (CHASE_NO_SHORTCUT_ముసుగు|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                తిరిగి RET_NERRNO(fstatat(AT_FDCWD, మార్గం, ret_stat,
                                          FLAGS_సమితి(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(మూలం_fd, dir_fd, మార్గం, chase_flags, ret_మార్గం ? &p : ఖాళీ, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        ఒకవేళ (fstat(మార్గం_fd, ret_stat) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_accessat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, పూర్ణాంకం access_బహుళకం, అక్షరం **ret_మార్గం) {
        _cleanup_close_ పూర్ణాంకం మార్గం_fd = -EBADF;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));

        ఒకవేళ (మూలం_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_మార్గం && (chase_flags & (CHASE_NO_SHORTCUT_ముసుగు|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                తిరిగి RET_NERRNO(faccessat(AT_FDCWD, మార్గం, access_బహుళకం,
                                            FLAGS_సమితి(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(మూలం_fd, dir_fd, మార్గం, chase_flags, ret_మార్గం ? &p : ఖాళీ, &మార్గం_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(మార్గం_fd >= 0);

        r = access_fd(మార్గం_fd, access_బహుళకం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_fopenat_unlocked(
                పూర్ణాంకం మూలం_fd,
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                ChaseFlags chase_flags,
                స్థిరరాశి అక్షరం *open_flags,
                అక్షరం **ret_మార్గం,
                FILE **ret_file) {

        _cleanup_free_ అక్షరం *final_మార్గం = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం బహుళకం_flags, r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_మాతృ)));
        ప్రతిజ్ఞా(open_flags);
        ప్రతిజ్ఞా(ret_file);

        బహుళకం_flags = fopen_బహుళకం_గమ్యం_flags(open_flags);
        ఒకవేళ (బహుళకం_flags < 0)
                తిరిగి బహుళకం_flags;

        fd = chase_and_openat(మూలం_fd, dir_fd, మార్గం, chase_flags, బహుళకం_flags, ret_మార్గం ? &final_మార్గం : ఖాళీ);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(final_మార్గం);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_unlinkat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, పూర్ణాంకం unlink_flags, అక్షరం **ret_మార్గం) {
        _cleanup_free_ అక్షరం *p = ఖాళీ, *fname = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు|CHASE_మాతృ|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_వెలికితీయి_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(మూలం_fd, dir_fd, మార్గం, chase_flags|CHASE_మాతృ|CHASE_NOFOLLOW, O_మార్గం|O_DIRECTORY|O_CLOEXEC, &p);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        r = మార్గం_వెలికితీయి_filename(p, &fname);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (unlinkat(fd, fname, unlink_flags) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(p);

        తిరిగి 0;
}

పూర్ణాంకం chase_and_open_మాతృ_at(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, అక్షరం **ret_filename) {
        పూర్ణాంకం pfd, r;

        ప్రతిజ్ఞా(!(chase_flags & (CHASE_NONEXISTENT|CHASE_అడుగు)));

        r = chaseat(మూలం_fd, dir_fd, మార్గం, CHASE_మాతృ|CHASE_వెలికితీయి_FILENAME|chase_flags, ret_filename, &pfd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి pfd;
}
