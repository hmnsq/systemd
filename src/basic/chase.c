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
#تعریف_کرو CHASE_NO_SHORTCUT_نقاب                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_محفوظ |                                   \
         CHASE_قدم |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_والد |                                 \
         CHASE_MKDIR_0755)

#تعریف_کرو CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

ساکن صحیح chase_statx(صحیح fd, ساخت statx *ret) {
        واپس xstatx_بھرا(fd,
                        /* path= */ تہی,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_بہترین,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ساکن صحیح chase_openat2(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ساکن bool can_openat2 = true;
        صحیح r;

        أكد(راستہ);
        أكد(wildcard_fd_is_valid(dir_fd));

        اگر (!can_openat2)
                واپس -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        اگر ((chase_flags & (CHASE_NO_SHORTCUT_نقاب & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                واپس -EOPNOTSUPP;
        اگر (FLAGS_سیٹ(chase_flags, CHASE_TRIGGER_AUTOFS))
                واپس -EOPNOTSUPP;
        اگر (جڑ_fd != XAT_FDROOT && جڑ_fd != dir_fd)
                واپس -EOPNOTSUPP;

        _cleanup_close_ صحیح dir_fd_local = -EBADF;
        اگر (dir_fd == XAT_FDROOT) {
                اگر (راستہ_is_absolute(راستہ))
                        dir_fd = AT_FDCWD;
                ورنہ {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_راستہ);
                        اگر (dir_fd_local < 0)
                                واپس -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ساخت open_how how = {
                .flags = O_راستہ|O_CLOEXEC,
        };
        اگر (FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        اگر (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        اگر (جڑ_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_اندر_جڑ;
        اگر (FLAGS_سیٹ(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ صحیح fd = openat2(dir_fd, راستہ, &how, حجم(how));
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
                اگر (اندر_سیٹ(errno, ENOSYS, EPERM, EAGAIN))
                        واپس -EOPNOTSUPP;
                واپس -errno;
        }

        اگر (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                اگر (r < 0)
                        واپس r;
        }
        اگر (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                اگر (r < 0)
                        واپس r;
        }

        واپس TAKE_FD(fd);
}

ساکن صحیح chase_xopenat(صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags, صحیح open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        اگر (FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        اگر (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        اگر (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        اگر (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        اگر (FLAGS_سیٹ(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_سیٹ(open_flags, O_راستہ))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        واپس xopenat_بھرا(dir_fd, راستہ, open_flags, xopen_flags, منوال_INVALID);
}

ساکن bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        اگر (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                واپس false;

        واپس a != b; /* Otherwise we need to stay within the same UID */
}

صحیح statx_unsafe_transition(ثابت ساخت statx *a, ثابت ساخت statx *b) {
        أكد(a);
        أكد(b);

        اگر (!FLAGS_سیٹ(a->stx_نقاب, STATX_UID) || !FLAGS_سیٹ(b->stx_نقاب, STATX_UID))
                واپس -ENODATA;

        واپس uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ثابت ساخت stat *a, ثابت ساخت stat *b) {
        أكد(a);
        أكد(b);

        واپس uid_unsafe_transition(a->st_uid, b->st_uid);
}

ساکن صحیح log_unsafe_transition(صحیح a, صحیح b, ثابت حرف *راستہ, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = تہی, *n2 = تہی, *user_a = تہی, *user_b = تہی;
        ساخت stat st;

        اگر (!FLAGS_سیٹ(flags, CHASE_WARN))
                واپس -ENOLINK;

        (خالی) fd_get_راستہ(a, &n1);
        (خالی) fd_get_راستہ(b, &n2);

        اگر (fstat(a, &st) == 0)
                user_a = uid_مقصد_name(st.st_uid);
        اگر (fstat(b, &st) == 0)
                user_b = uid_مقصد_name(st.st_uid);

        واپس log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_دایاں), strna(n2), strna(user_b), راستہ);
}

ساکن صحیح log_autofs_mount_نقطہ(صحیح fd, ثابت حرف *راستہ, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = تہی;

        اگر (!FLAGS_سیٹ(flags, CHASE_WARN))
                واپس -EREMOTE;

        (خالی) fd_get_راستہ(fd, &n1);

        واپس log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), راستہ);
}

ساکن صحیح log_prohibited_symlink(صحیح fd, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = تہی;

        أكد(fd >= 0);

        اگر (!FLAGS_سیٹ(flags, CHASE_WARN))
                واپس -ELOOP;

        (خالی) fd_get_راستہ(fd, &n1);

        واپس log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

صحیح chaseat(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags flags, حرف **ret_راستہ, صحیح *ret_fd) {
        صحیح r;

        أكد(!FLAGS_سیٹ(flags, CHASE_سابقہ_جڑ));
        أكد(!FLAGS_سیٹ(flags, CHASE_قدم|CHASE_نکالو_FILENAME));
        أكد(!FLAGS_سیٹ(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(wildcard_fd_is_valid(جڑ_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        أكد(dir_fd != AT_FDCWD || اندر_سیٹ(جڑ_fd, AT_FDCWD, XAT_FDROOT));

        اگر (FLAGS_سیٹ(flags, CHASE_قدم))
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
        اگر (جڑ_fd == AT_FDCWD)
                جڑ_fd = XAT_FDROOT;
        ورنہ {
                r = dir_fd_is_جڑ(جڑ_fd);
                اگر (r < 0)
                        واپس r;
                اگر (r > 0)
                        جڑ_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_جڑ(dir_fd);
        اگر (r < 0)
                واپس r;
        اگر (r > 0 && جڑ_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        اگر (dir_fd == XAT_FDROOT || راستہ_is_absolute(راستہ))
                dir_fd = جڑ_fd;

        اگر (isempty(راستہ))
                راستہ = ".";

        bool append_trail_slash = false;
        اگر (ENDSWITH_سیٹ(راستہ, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                اگر (FLAGS_سیٹ(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } ورنہ اگر (dot_or_dot_dot(راستہ) || endswith(راستہ, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        اگر (FLAGS_سیٹ(flags, CHASE_والد))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        اگر (FLAGS_سیٹ(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_سیٹ(flags, CHASE_MUST_BE_REGULAR) + FLAGS_سیٹ(flags, CHASE_MUST_BE_SOCKET) > 1)
                واپس -EBADSLT;

        اگر (!ret_راستہ) {
                r = chase_openat2(جڑ_fd, dir_fd, راستہ, flags);
                اگر (r >= 0) {
                        اگر (ret_fd)
                                *ret_fd = r;
                        ورنہ
                                محفوظ_close(r);

                        واپس 1;
                }
                اگر (r != -EOPNOTSUPP)
                        واپس r;
        }

        اگر (جڑ_fd == XAT_FDROOT && !ret_راستہ && (flags & CHASE_NO_SHORTCUT_نقاب) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, راستہ, flags, O_راستہ|O_CLOEXEC, /* xopen_flags= */ 0);
                اگر (r < 0)
                        واپس r;

                اگر (ret_fd)
                        *ret_fd = r;
                ورنہ
                        محفوظ_close(r);

                واپس 1;
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
        bool need_absolute = (جڑ_fd == XAT_FDROOT || dir_fd != جڑ_fd) && (dir_fd == XAT_FDROOT || راستہ_is_absolute(راستہ));

        _cleanup_free_ حرف *مکمل = تہی;
        اگر (need_absolute) {
                مکمل = strdup("/");
                اگر (!مکمل)
                        واپس -ENOMEM;
        }

        _cleanup_close_ صحیح fd = xopenat(dir_fd, تہی, O_CLOEXEC|O_DIRECTORY|O_راستہ);
        اگر (fd < 0)
                واپس fd;

        ساخت statx stx;
        r = chase_statx(fd, &stx);
        اگر (r < 0)
                واپس r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ساخت statx جڑ_stx;
        اگر (جڑ_fd != XAT_FDROOT) {
                اگر (جڑ_fd == dir_fd)
                        جڑ_stx = stx;
                ورنہ {
                        r = chase_statx(جڑ_fd, &جڑ_stx);
                        اگر (r < 0)
                                واپس r;
                }
        }

        _cleanup_free_ حرف *بفر = strdup(راستہ);
        اگر (!بفر)
                واپس -ENOMEM;

        ثابت حرف *todo = بفر;
        bool exists = true;
        برائے (غیرعلامتی n_اقدام = 0;; n_اقدام++) {
                _cleanup_free_ حرف *پہلا = تہی;
                _cleanup_close_ صحیح بچہ = -EBADF;
                ساخت statx stx_بچہ;
                ثابت حرف *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                اگر (n_اقدام > CHASE_MAX)
                        واپس -ELOOP;

                r = راستہ_ڈھونڈو_پہلا_component(&todo, /* accept_dot_dot= */ true, &e);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0) /* We reached the end. */
                        توڑو;

                پہلا = strndup(e, r);
                اگر (!پہلا)
                        واپس -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                اگر (streq(پہلا, "..")) {
                        _cleanup_free_ حرف *والد = تہی;
                        _cleanup_close_ صحیح fd_والد = -EBADF;
                        ساخت statx stx_والد;

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
                        اگر (جڑ_fd != XAT_FDROOT) {
                                bool is_جڑ = جڑ_fd == dir_fd && خالی_خانے_or_جڑ(مکمل);
                                اگر (!is_جڑ && statx_inode_same(&stx, &جڑ_stx)) {
                                        r = statx_mount_same(&stx, &جڑ_stx);
                                        اگر (r < 0)
                                                واپس r;

                                        is_جڑ = r > 0;
                                }
                                اگر (is_جڑ) {
                                        اگر (FLAGS_سیٹ(flags, CHASE_قدم))
                                                جاؤ chased_one;
                                        جاری;
                                }
                        }

                        fd_والد = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_راستہ|O_DIRECTORY);
                        اگر (fd_والد < 0)
                                واپس -errno;

                        r = chase_statx(fd_والد, &stx_والد);
                        اگر (r < 0)
                                واپس r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        اگر (statx_inode_same(&stx_والد, &stx)) {
                                r = dir_fd_is_جڑ(fd);
                                اگر (r < 0)
                                        واپس r;
                                اگر (r > 0) {
                                        اگر (FLAGS_سیٹ(flags, CHASE_قدم))
                                                جاؤ chased_one;
                                        جاری;
                                }
                        }

                        r = راستہ_نکالو_directory(مکمل, &والد);
                        اگر (r >= 0) {
                                أكد(!need_absolute || راستہ_is_absolute(والد));
                                free_and_replace(مکمل, والد);
                        } ورنہ اگر (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                أكد(!need_absolute);
                                مکمل = mfree(مکمل);
                        } ورنہ اگر (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } ورنہ اگر (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                أكد(!need_absolute);

                                اگر (!isempty(مکمل) && !راستہ_is_valid(مکمل))
                                        واپس -EINVAL;

                                اگر (!راستہ_extend(&مکمل, ".."))
                                        واپس -ENOMEM;
                        } ورنہ
                                واپس r;

                        اگر (FLAGS_سیٹ(flags, CHASE_قدم))
                                جاؤ chased_one;

                        اگر (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                                r = statx_unsafe_transition(&stx, &stx_والد);
                                اگر (r < 0)
                                        واپس r;
                                اگر (r > 0)
                                        واپس log_unsafe_transition(fd, fd_والد, راستہ, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        اگر (FLAGS_سیٹ(flags, CHASE_والد) && isempty(todo)) {
                                _cleanup_close_ صحیح fd_grandparent = -EBADF;
                                ساخت statx stx_grandparent;

                                fd_grandparent = openat(fd_والد, "..", O_CLOEXEC|O_NOFOLLOW|O_راستہ|O_DIRECTORY);
                                اگر (fd_grandparent < 0)
                                        واپس -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                اگر (r < 0)
                                        واپس r;

                                اگر (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                                        r = statx_unsafe_transition(&stx_والد, &stx_grandparent);
                                        اگر (r < 0)
                                                واپس r;
                                        اگر (r > 0)
                                                واپس log_unsafe_transition(fd_والد, fd_grandparent, راستہ, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                توڑو;
                        }

                        /* update fd and stat */
                        stx = stx_والد;
                        close_and_replace(fd, fd_والد);
                        جاری;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                بچہ = r = xopenat_بھرا(fd, پہلا,
                                         O_راستہ|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_سیٹ(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         منوال_INVALID);
                اگر (r < 0) {
                        اگر (r != -ENOENT)
                                واپس r;

                        اگر (!isempty(todo) && !راستہ_is_محفوظ(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                واپس r;

                        اگر (FLAGS_سیٹ(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_والد|CHASE_NONEXISTENT)))) {
                                بچہ = xopenat(fd,
                                                پہلا,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_راستہ|O_CLOEXEC);
                                اگر (بچہ < 0)
                                        واپس بچہ;
                        } ورنہ اگر (FLAGS_سیٹ(flags, CHASE_والد) && isempty(todo)) {
                                اگر (!راستہ_extend(&مکمل, پہلا))
                                        واپس -ENOMEM;

                                توڑو;
                        } ورنہ اگر (FLAGS_سیٹ(flags, CHASE_NONEXISTENT)) {
                                اگر (!راستہ_extend(&مکمل, پہلا, todo))
                                        واپس -ENOMEM;

                                exists = false;
                                توڑو;
                        } ورنہ
                                واپس r;
                }

                r = chase_statx(بچہ, &stx_بچہ);
                اگر (r < 0)
                        واپس r;

                اگر (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                        r = statx_unsafe_transition(&stx, &stx_بچہ);
                        اگر (r < 0)
                                واپس r;
                        اگر (r > 0)
                                واپس log_unsafe_transition(fd, بچہ, راستہ, flags);
                }

                اگر (FLAGS_سیٹ(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(بچہ, AUTOFS_SUPER_MAGIC) > 0)
                        واپس log_autofs_mount_نقطہ(بچہ, راستہ, flags);

                اگر (S_ISLNK(stx_بچہ.stx_منوال) && !(FLAGS_سیٹ(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ حرف *destination = تہی;

                        اگر (FLAGS_سیٹ(flags, CHASE_PROHIBIT_SYMLINKS))
                                واپس log_prohibited_symlink(بچہ, flags);

                        r = readlinkat_malloc(fd, پہلا, &destination);
                        اگر (r < 0)
                                واپس r;
                        اگر (isempty(destination))
                                واپس -EINVAL;

                        اگر (راستہ_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                محفوظ_close(fd);
                                fd = fd_reopen(جڑ_fd, O_CLOEXEC|O_راستہ|O_DIRECTORY);
                                اگر (fd < 0)
                                        واپس fd;

                                r = chase_statx(fd, &stx);
                                اگر (r < 0)
                                        واپس r;

                                اگر (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                                        r = statx_unsafe_transition(&stx_بچہ, &stx);
                                        اگر (r < 0)
                                                واپس r;
                                        اگر (r > 0)
                                                واپس log_unsafe_transition(بچہ, fd, راستہ, flags);
                                }

                                اگر (dir_fd != جڑ_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&مکمل, need_absolute ? "/" : تہی);
                                اگر (r < 0)
                                        واپس r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        اگر (!راستہ_extend(&destination, todo))
                                واپس -ENOMEM;

                        free_and_replace(بفر, destination);
                        todo = بفر;

                        اگر (FLAGS_سیٹ(flags, CHASE_قدم))
                                جاؤ chased_one;

                        جاری;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                اگر (!راستہ_extend(&مکمل, پہلا))
                        واپس -ENOMEM;

                اگر (FLAGS_سیٹ(flags, CHASE_والد) && isempty(todo))
                        توڑو;

                /* And iterate again, but go one directory further down. */
                stx = stx_بچہ;
                close_and_replace(fd, بچہ);
        }

        اگر (exists) {
                اگر (FLAGS_سیٹ(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (FLAGS_سیٹ(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (FLAGS_سیٹ(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        اگر (r < 0)
                                واپس r;
                }
        }

        اگر (ret_راستہ) {
                اگر (FLAGS_سیٹ(flags, CHASE_نکالو_FILENAME) && مکمل) {
                        _cleanup_free_ حرف *f = تہی;

                        r = راستہ_نکالو_filename(مکمل, &f);
                        اگر (r < 0 && r != -EADDRNOTAVAIL)
                                واپس r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(مکمل, f);
                }

                اگر (!مکمل) {
                        أكد(!need_absolute || FLAGS_سیٹ(flags, CHASE_نکالو_FILENAME));
                        مکمل = strdup(".");
                        اگر (!مکمل)
                                واپس -ENOMEM;
                }

                اگر (append_trail_slash)
                        اگر (!strextend(&مکمل, "/"))
                                واپس -ENOMEM;

                *ret_راستہ = TAKE_PTR(مکمل);
        }

        اگر (ret_fd) {
                اگر (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        أكد(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } ورنہ
                        *ret_fd = -EBADF;
        }

        اگر (FLAGS_سیٹ(flags, CHASE_قدم))
                واپس 1;

        واپس exists;

chased_one:
        اگر (ret_راستہ) {
                ثابت حرف *e;

                اگر (!مکمل) {
                        أكد(!need_absolute);
                        مکمل = strdup(append_trail_slash ? "./" : ".");
                        اگر (!مکمل)
                                واپس -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = راستہ_ڈھونڈو_پہلا_component(&todo, /* accept_dot_dot= */ true, &e);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        *ret_راستہ = TAKE_PTR(مکمل);
                ورنہ {
                        حرف *c;

                        c = راستہ_join(مکمل, e);
                        اگر (!c)
                                واپس -ENOMEM;

                        *ret_راستہ = c;
                }
        }

        واپس 0;
}

صحیح chase(ثابت حرف *راستہ, ثابت حرف *جڑ, ChaseFlags flags, حرف **ret_راستہ, صحیح *ret_fd) {
        _cleanup_free_ حرف *جڑ_abs = تہی, *absolute = تہی, *p = تہی;
        _cleanup_close_ صحیح fd = -EBADF, pfd = -EBADF;
        صحیح r;

        أكد(راستہ);

        اگر (isempty(راستہ))
                واپس -EINVAL;

        r = خالی_خانے_or_جڑ_harder_مقصد_null(&جڑ);
        اگر (r < 0)
                واپس r;

        /* A root directory of "/" or "" is identical to "/". */
        اگر (خالی_خانے_or_جڑ(جڑ))
                جڑ = "/";
        ورنہ {
                r = راستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                اگر (r < 0)
                        واپس r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                جڑ = راستہ_simplify(جڑ_abs);

                أكد(راستہ_is_absolute(جڑ));
                أكد(!خالی_خانے_or_جڑ(جڑ));

                اگر (FLAGS_سیٹ(flags, CHASE_سابقہ_جڑ)) {
                        absolute = راستہ_join(جڑ, راستہ);
                        اگر (!absolute)
                                واپس -ENOMEM;
                }
        }

        اگر (!absolute) {
                r = راستہ_make_absolute_cwd(راستہ, &absolute);
                اگر (r < 0)
                        واپس r;
        }

        راستہ = راستہ_startswith(absolute, جڑ);
        اگر (!راستہ)
                واپس log_بھرا_errno(FLAGS_سیٹ(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, جڑ);

        اگر (خالی_خانے_or_جڑ(جڑ))
                fd = XAT_FDROOT;
        ورنہ {
                fd = open(جڑ, O_CLOEXEC|O_DIRECTORY|O_راستہ);
                اگر (fd < 0)
                        واپس -errno;
        }

        r = chaseat(fd, fd, راستہ, flags & ~CHASE_سابقہ_جڑ, ret_راستہ ? &p : تہی, ret_fd ? &pfd : تہی);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ) {
                اگر (!FLAGS_سیٹ(flags, CHASE_نکالو_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        اگر (خالی_خانے_or_جڑ(جڑ))
                                أكد(راستہ_is_absolute(p));
                        ورنہ {
                                حرف *q;

                                أكد(!راستہ_is_absolute(p));

                                q = راستہ_join(جڑ, p + STR_اندر_سیٹ(p, ".", "./"));
                                اگر (!q)
                                        واپس -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_راستہ = TAKE_PTR(p);
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        واپس r;
}

صحیح chaseat_سابقہ_جڑ(ثابت حرف *راستہ, ثابت حرف *جڑ, حرف **ret) {
        حرف *q;
        صحیح r;

        أكد(راستہ);
        أكد(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        اگر (!راستہ_is_absolute(راستہ)) {
                _cleanup_free_ حرف *جڑ_abs = تہی;

                r = خالی_خانے_or_جڑ_harder_مقصد_null(&جڑ);
                اگر (r < 0 && r != -ENOENT)
                        واپس r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                اگر (خالی_خانے_or_جڑ(جڑ))
                        واپس -EINVAL;

                r = راستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                اگر (r < 0)
                        واپس r;

                جڑ = راستہ_simplify(جڑ_abs);

                q = راستہ_join(جڑ, راستہ + (راستہ[0] == '.' && اندر_سیٹ(راستہ[1], '/', '\0')));
        } ورنہ
                q = strdup(راستہ);
        اگر (!q)
                واپس -ENOMEM;

        *ret = q;
        واپس 0;
}

صحیح chase_نکالو_filename(ثابت حرف *راستہ, ثابت حرف *جڑ, حرف **ret) {
        صحیح r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        أكد(راستہ);
        أكد(ret);

        اگر (isempty(راستہ))
                واپس -EINVAL;

        اگر (!راستہ_is_absolute(راستہ))
                واپس -EINVAL;

        r = خالی_خانے_or_جڑ_harder_مقصد_null(&جڑ);
        اگر (r < 0 && r != -ENOENT)
                واپس r;

        اگر (!خالی_خانے_or_جڑ(جڑ)) {
                _cleanup_free_ حرف *جڑ_abs = تہی;

                r = راستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                اگر (r < 0)
                        واپس r;

                راستہ = راستہ_startswith(راستہ, جڑ_abs);
                اگر (!راستہ)
                        واپس -EINVAL;
        }

        اگر (!isempty(راستہ)) {
                r = راستہ_نکالو_filename(راستہ, ret);
                اگر (r != -EADDRNOTAVAIL)
                        واپس r;
        }

        واپس strdup_مقصد(ret, ".");
}

صحیح chase_and_open(
                ثابت حرف *راستہ,
                ثابت حرف *جڑ,
                ChaseFlags chase_flags,
                صحیح open_flags,
                حرف **ret_راستہ) {

        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی, *fname = تہی;
        ثابت حرف *open_name = تہی;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        اگر (خالی_خانے_or_جڑ(جڑ) && !ret_راستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                واپس chase_xopenat(AT_FDCWD, راستہ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(راستہ, جڑ, (CHASE_والد|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        اگر (!FLAGS_سیٹ(chase_flags, CHASE_والد)) {
                اگر (FLAGS_سیٹ(chase_flags, CHASE_نکالو_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ورنہ {
                        r = chase_نکالو_filename(p, جڑ, &fname);
                        اگر (r < 0)
                                واپس r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(راستہ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس r;
}

صحیح chase_and_opendir(ثابت حرف *راستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_راستہ, DIR **ret_dir) {
        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        DIR *d;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        اگر (خالی_خانے_or_جڑ(جڑ) && !ret_راستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(راستہ);
                اگر (!d)
                        واپس -errno;

                *ret_dir = d;
                واپس 0;
        }

        r = chase(راستہ, جڑ, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_راستہ ? &p : تہی, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        d = xopendirat(راستہ_fd, /* path= */ تہی, /* flags= */ 0);
        اگر (!d)
                واپس -errno;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        *ret_dir = d;
        واپس 0;
}

صحیح chase_and_stat(ثابت حرف *راستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_راستہ, ساخت stat *ret_stat) {
        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));
        أكد(ret_stat);

        اگر (خالی_خانے_or_جڑ(جڑ) && !ret_راستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                واپس RET_NERRNO(fstatat(AT_FDCWD, راستہ, ret_stat,
                                          FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(راستہ, جڑ, chase_flags, ret_راستہ ? &p : تہی, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        اگر (fstat(راستہ_fd, ret_stat) < 0)
                واپس -errno;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس 0;
}

صحیح chase_and_access(ثابت حرف *راستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحیح access_منوال, حرف **ret_راستہ) {
        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        اگر (خالی_خانے_or_جڑ(جڑ) && !ret_راستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                واپس RET_NERRNO(faccessat(AT_FDCWD, راستہ, access_منوال,
                                            FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(راستہ, جڑ, chase_flags, ret_راستہ ? &p : تہی, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        r = access_fd(راستہ_fd, access_منوال);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس 0;
}

صحیح chase_and_fopen_unlocked(
                ثابت حرف *راستہ,
                ثابت حرف *جڑ,
                ChaseFlags chase_flags,
                ثابت حرف *open_flags,
                حرف **ret_راستہ,
                FILE **ret_file) {

        _cleanup_free_ حرف *final_راستہ = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح منوال_flags, r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_والد|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        أكد(open_flags);
        أكد(ret_file);

        منوال_flags = fopen_منوال_مقصد_flags(open_flags);
        اگر (منوال_flags < 0)
                واپس منوال_flags;

        fd = chase_and_open(راستہ, جڑ, chase_flags, منوال_flags, ret_راستہ ? &final_راستہ : تہی);
        اگر (fd < 0)
                واپس fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(final_راستہ);

        واپس 0;
}

صحیح chase_and_unlink(ثابت حرف *راستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحیح unlink_flags, حرف **ret_راستہ) {
        _cleanup_free_ حرف *p = تہی, *fname = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_والد|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_نکالو_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(راستہ, جڑ, chase_flags|CHASE_والد|CHASE_NOFOLLOW, O_راستہ|O_DIRECTORY|O_CLOEXEC, &p);
        اگر (fd < 0)
                واپس fd;

        r = راستہ_نکالو_filename(p, &fname);
        اگر (r < 0)
                واپس r;

        اگر (unlinkat(fd, fname, unlink_flags) < 0)
                واپس -errno;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس 0;
}

صحیح chase_and_open_والد(ثابت حرف *راستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_filename) {
        صحیح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        r = chase(راستہ, جڑ, CHASE_والد|CHASE_نکالو_FILENAME|chase_flags, ret_filename, &pfd);
        اگر (r < 0)
                واپس r;

        واپس pfd;
}

صحیح chase_and_openat(
                صحیح جڑ_fd,
                صحیح dir_fd,
                ثابت حرف *راستہ,
                ChaseFlags chase_flags,
                صحیح open_flags,
                حرف **ret_راستہ) {

        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی, *fname = تہی;
        ثابت حرف *open_name = تہی;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_MUST_BE_SOCKET)));

        اگر (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_راستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                واپس chase_xopenat(dir_fd, راستہ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(جڑ_fd, dir_fd, راستہ, (chase_flags|CHASE_والد)&~CHASE_MUST_BE_REGULAR, &p, &راستہ_fd);
        اگر (r < 0)
                واپس r;

        اگر (!FLAGS_سیٹ(chase_flags, CHASE_والد)) {
                اگر (FLAGS_سیٹ(chase_flags, CHASE_نکالو_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ورنہ {
                        r = راستہ_نکالو_filename(p, &fname);
                        اگر (r < 0 && r != -EADDRNOTAVAIL)
                                واپس r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(راستہ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس r;
}

صحیح chase_and_opendirat(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags, حرف **ret_راستہ, DIR **ret_dir) {
        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        DIR *d;
        صحیح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        اگر (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_راستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(راستہ);
                اگر (!d)
                        واپس -errno;

                *ret_dir = d;
                واپس 0;
        }

        r = chaseat(جڑ_fd, dir_fd, راستہ, chase_flags, ret_راستہ ? &p : تہی, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        d = xopendirat(راستہ_fd, /* path= */ تہی, /* flags= */ 0);
        اگر (!d)
                واپس -errno;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        *ret_dir = d;
        واپس 0;
}

صحیح chase_and_statat(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags, حرف **ret_راستہ, ساخت stat *ret_stat) {
        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));
        أكد(ret_stat);

        اگر (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_راستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                واپس RET_NERRNO(fstatat(AT_FDCWD, راستہ, ret_stat,
                                          FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(جڑ_fd, dir_fd, راستہ, chase_flags, ret_راستہ ? &p : تہی, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        اگر (fstat(راستہ_fd, ret_stat) < 0)
                واپس -errno;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس 0;
}

صحیح chase_and_accessat(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags, صحیح access_منوال, حرف **ret_راستہ) {
        _cleanup_close_ صحیح راستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        اگر (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_راستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                واپس RET_NERRNO(faccessat(AT_FDCWD, راستہ, access_منوال,
                                            FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(جڑ_fd, dir_fd, راستہ, chase_flags, ret_راستہ ? &p : تہی, &راستہ_fd);
        اگر (r < 0)
                واپس r;
        أكد(راستہ_fd >= 0);

        r = access_fd(راستہ_fd, access_منوال);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس 0;
}

صحیح chase_and_fopenat_unlocked(
                صحیح جڑ_fd,
                صحیح dir_fd,
                ثابت حرف *راستہ,
                ChaseFlags chase_flags,
                ثابت حرف *open_flags,
                حرف **ret_راستہ,
                FILE **ret_file) {

        _cleanup_free_ حرف *final_راستہ = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح منوال_flags, r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_والد)));
        أكد(open_flags);
        أكد(ret_file);

        منوال_flags = fopen_منوال_مقصد_flags(open_flags);
        اگر (منوال_flags < 0)
                واپس منوال_flags;

        fd = chase_and_openat(جڑ_fd, dir_fd, راستہ, chase_flags, منوال_flags, ret_راستہ ? &final_راستہ : تہی);
        اگر (fd < 0)
                واپس fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(final_راستہ);

        واپس 0;
}

صحیح chase_and_unlinkat(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags, صحیح unlink_flags, حرف **ret_راستہ) {
        _cleanup_free_ حرف *p = تہی, *fname = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(راستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_والد|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_نکالو_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(جڑ_fd, dir_fd, راستہ, chase_flags|CHASE_والد|CHASE_NOFOLLOW, O_راستہ|O_DIRECTORY|O_CLOEXEC, &p);
        اگر (fd < 0)
                واپس fd;

        r = راستہ_نکالو_filename(p, &fname);
        اگر (r < 0)
                واپس r;

        اگر (unlinkat(fd, fname, unlink_flags) < 0)
                واپس -errno;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(p);

        واپس 0;
}

صحیح chase_and_open_والد_at(صحیح جڑ_fd, صحیح dir_fd, ثابت حرف *راستہ, ChaseFlags chase_flags, حرف **ret_filename) {
        صحیح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        r = chaseat(جڑ_fd, dir_fd, راستہ, CHASE_والد|CHASE_نکالو_FILENAME|chase_flags, ret_filename, &pfd);
        اگر (r < 0)
                واپس r;

        واپس pfd;
}
