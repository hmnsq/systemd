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
#تعریف_کرو CHASE_NO_SHORTCUT_نقاب                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_محفوظ |                                   \
         CHASE_قدم |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_ماپا |                                 \
         CHASE_MKDIR_0755)

#تعریف_کرو CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

ساكن صحيح chase_statx(صحيح fd, بنية statx *ret) {
        أرجع xstatx_بھریا(fd,
                        /* path= */ عدم,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_بہترین,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ساكن صحيح chase_openat2(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ساكن bool can_openat2 = true;
        صحيح r;

        أكد(رستہ);
        أكد(wildcard_fd_is_valid(dir_fd));

        إذا (!can_openat2)
                أرجع -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        إذا ((chase_flags & (CHASE_NO_SHORTCUT_نقاب & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                أرجع -EOPNOTSUPP;
        إذا (FLAGS_سیٹ(chase_flags, CHASE_TRIGGER_AUTOFS))
                أرجع -EOPNOTSUPP;
        إذا (جڑ_fd != XAT_FDROOT && جڑ_fd != dir_fd)
                أرجع -EOPNOTSUPP;

        _cleanup_close_ صحيح dir_fd_local = -EBADF;
        إذا (dir_fd == XAT_FDROOT) {
                إذا (رستہ_is_absolute(رستہ))
                        dir_fd = AT_FDCWD;
                وإلا {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_رستہ);
                        إذا (dir_fd_local < 0)
                                أرجع -errno;
                        dir_fd = dir_fd_local;
                }
        }

        بنية open_how how = {
                .flags = O_رستہ|O_CLOEXEC,
        };
        إذا (FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        إذا (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        إذا (جڑ_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_وچ_جڑ;
        إذا (FLAGS_سیٹ(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ صحيح fd = openat2(dir_fd, رستہ, &how, حجم(how));
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
                إذا (وچ_سیٹ(errno, ENOSYS, EPERM, EAGAIN))
                        أرجع -EOPNOTSUPP;
                أرجع -errno;
        }

        إذا (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                إذا (r < 0)
                        أرجع r;
        }
        إذا (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع TAKE_FD(fd);
}

ساكن صحيح chase_xopenat(صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, صحيح open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        إذا (FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        إذا (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        إذا (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        إذا (FLAGS_سیٹ(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        إذا (FLAGS_سیٹ(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_سیٹ(open_flags, O_رستہ))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        أرجع xopenat_بھریا(dir_fd, رستہ, open_flags, xopen_flags, منوال_INVALID);
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

        إذا (!FLAGS_سیٹ(a->stx_نقاب, STATX_UID) || !FLAGS_سیٹ(b->stx_نقاب, STATX_UID))
                أرجع -ENODATA;

        أرجع uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ثابت بنية stat *a, ثابت بنية stat *b) {
        أكد(a);
        أكد(b);

        أرجع uid_unsafe_transition(a->st_uid, b->st_uid);
}

ساكن صحيح log_unsafe_transition(صحيح a, صحيح b, ثابت حرف *رستہ, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = عدم, *n2 = عدم, *user_a = عدم, *user_b = عدم;
        بنية stat st;

        إذا (!FLAGS_سیٹ(flags, CHASE_WARN))
                أرجع -ENOLINK;

        (فراغ) fd_get_رستہ(a, &n1);
        (فراغ) fd_get_رستہ(b, &n2);

        إذا (fstat(a, &st) == 0)
                user_a = uid_منزل_name(st.st_uid);
        إذا (fstat(b, &st) == 0)
                user_b = uid_منزل_name(st.st_uid);

        أرجع log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_دایاں), strna(n2), strna(user_b), رستہ);
}

ساكن صحيح log_autofs_mount_نقطہ(صحيح fd, ثابت حرف *رستہ, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = عدم;

        إذا (!FLAGS_سیٹ(flags, CHASE_WARN))
                أرجع -EREMOTE;

        (فراغ) fd_get_رستہ(fd, &n1);

        أرجع log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), رستہ);
}

ساكن صحيح log_prohibited_symlink(صحيح fd, ChaseFlags flags) {
        _cleanup_free_ حرف *n1 = عدم;

        أكد(fd >= 0);

        إذا (!FLAGS_سیٹ(flags, CHASE_WARN))
                أرجع -ELOOP;

        (فراغ) fd_get_رستہ(fd, &n1);

        أرجع log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

صحيح chaseat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags flags, حرف **ret_رستہ, صحيح *ret_fd) {
        صحيح r;

        أكد(!FLAGS_سیٹ(flags, CHASE_اگیتر_جڑ));
        أكد(!FLAGS_سیٹ(flags, CHASE_قدم|CHASE_کڈھو_FILENAME));
        أكد(!FLAGS_سیٹ(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(wildcard_fd_is_valid(جڑ_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        أكد(dir_fd != AT_FDCWD || وچ_سیٹ(جڑ_fd, AT_FDCWD, XAT_FDROOT));

        إذا (FLAGS_سیٹ(flags, CHASE_قدم))
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
        إذا (جڑ_fd == AT_FDCWD)
                جڑ_fd = XAT_FDROOT;
        وإلا {
                r = dir_fd_is_جڑ(جڑ_fd);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        جڑ_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_جڑ(dir_fd);
        إذا (r < 0)
                أرجع r;
        إذا (r > 0 && جڑ_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        إذا (dir_fd == XAT_FDROOT || رستہ_is_absolute(رستہ))
                dir_fd = جڑ_fd;

        إذا (isempty(رستہ))
                رستہ = ".";

        bool append_trail_slash = false;
        إذا (ENDSWITH_سیٹ(رستہ, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                إذا (FLAGS_سیٹ(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } وإلا إذا (dot_or_dot_dot(رستہ) || endswith(رستہ, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        إذا (FLAGS_سیٹ(flags, CHASE_ماپا))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        إذا (FLAGS_سیٹ(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_سیٹ(flags, CHASE_MUST_BE_REGULAR) + FLAGS_سیٹ(flags, CHASE_MUST_BE_SOCKET) > 1)
                أرجع -EBADSLT;

        إذا (!ret_رستہ) {
                r = chase_openat2(جڑ_fd, dir_fd, رستہ, flags);
                إذا (r >= 0) {
                        إذا (ret_fd)
                                *ret_fd = r;
                        وإلا
                                محفوظ_close(r);

                        أرجع 1;
                }
                إذا (r != -EOPNOTSUPP)
                        أرجع r;
        }

        إذا (جڑ_fd == XAT_FDROOT && !ret_رستہ && (flags & CHASE_NO_SHORTCUT_نقاب) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, رستہ, flags, O_رستہ|O_CLOEXEC, /* xopen_flags= */ 0);
                إذا (r < 0)
                        أرجع r;

                إذا (ret_fd)
                        *ret_fd = r;
                وإلا
                        محفوظ_close(r);

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
        bool need_absolute = (جڑ_fd == XAT_FDROOT || dir_fd != جڑ_fd) && (dir_fd == XAT_FDROOT || رستہ_is_absolute(رستہ));

        _cleanup_free_ حرف *پورا = عدم;
        إذا (need_absolute) {
                پورا = strdup("/");
                إذا (!پورا)
                        أرجع -ENOMEM;
        }

        _cleanup_close_ صحيح fd = xopenat(dir_fd, عدم, O_CLOEXEC|O_DIRECTORY|O_رستہ);
        إذا (fd < 0)
                أرجع fd;

        بنية statx stx;
        r = chase_statx(fd, &stx);
        إذا (r < 0)
                أرجع r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        بنية statx جڑ_stx;
        إذا (جڑ_fd != XAT_FDROOT) {
                إذا (جڑ_fd == dir_fd)
                        جڑ_stx = stx;
                وإلا {
                        r = chase_statx(جڑ_fd, &جڑ_stx);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        _cleanup_free_ حرف *بفر = strdup(رستہ);
        إذا (!بفر)
                أرجع -ENOMEM;

        ثابت حرف *todo = بفر;
        bool exists = true;
        لكل (بنانشانی n_قدماں = 0;; n_قدماں++) {
                _cleanup_free_ حرف *پہلا = عدم;
                _cleanup_close_ صحيح بچہ = -EBADF;
                بنية statx stx_بچہ;
                ثابت حرف *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                إذا (n_قدماں > CHASE_MAX)
                        أرجع -ELOOP;

                r = رستہ_لبھو_پہلا_component(&todo, /* accept_dot_dot= */ true, &e);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0) /* We reached the end. */
                        اكسر;

                پہلا = strndup(e, r);
                إذا (!پہلا)
                        أرجع -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                إذا (streq(پہلا, "..")) {
                        _cleanup_free_ حرف *ماپا = عدم;
                        _cleanup_close_ صحيح fd_ماپا = -EBADF;
                        بنية statx stx_ماپا;

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
                        إذا (جڑ_fd != XAT_FDROOT) {
                                bool is_جڑ = جڑ_fd == dir_fd && خالی_تھاواں_or_جڑ(پورا);
                                إذا (!is_جڑ && statx_inode_same(&stx, &جڑ_stx)) {
                                        r = statx_mount_same(&stx, &جڑ_stx);
                                        إذا (r < 0)
                                                أرجع r;

                                        is_جڑ = r > 0;
                                }
                                إذا (is_جڑ) {
                                        إذا (FLAGS_سیٹ(flags, CHASE_قدم))
                                                اذهب chased_one;
                                        استمر;
                                }
                        }

                        fd_ماپا = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_رستہ|O_DIRECTORY);
                        إذا (fd_ماپا < 0)
                                أرجع -errno;

                        r = chase_statx(fd_ماپا, &stx_ماپا);
                        إذا (r < 0)
                                أرجع r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        إذا (statx_inode_same(&stx_ماپا, &stx)) {
                                r = dir_fd_is_جڑ(fd);
                                إذا (r < 0)
                                        أرجع r;
                                إذا (r > 0) {
                                        إذا (FLAGS_سیٹ(flags, CHASE_قدم))
                                                اذهب chased_one;
                                        استمر;
                                }
                        }

                        r = رستہ_کڈھو_directory(پورا, &ماپا);
                        إذا (r >= 0) {
                                أكد(!need_absolute || رستہ_is_absolute(ماپا));
                                free_and_replace(پورا, ماپا);
                        } وإلا إذا (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                أكد(!need_absolute);
                                پورا = mfree(پورا);
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

                                إذا (!isempty(پورا) && !رستہ_is_valid(پورا))
                                        أرجع -EINVAL;

                                إذا (!رستہ_extend(&پورا, ".."))
                                        أرجع -ENOMEM;
                        } وإلا
                                أرجع r;

                        إذا (FLAGS_سیٹ(flags, CHASE_قدم))
                                اذهب chased_one;

                        إذا (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                                r = statx_unsafe_transition(&stx, &stx_ماپا);
                                إذا (r < 0)
                                        أرجع r;
                                إذا (r > 0)
                                        أرجع log_unsafe_transition(fd, fd_ماپا, رستہ, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        إذا (FLAGS_سیٹ(flags, CHASE_ماپا) && isempty(todo)) {
                                _cleanup_close_ صحيح fd_grandparent = -EBADF;
                                بنية statx stx_grandparent;

                                fd_grandparent = openat(fd_ماپا, "..", O_CLOEXEC|O_NOFOLLOW|O_رستہ|O_DIRECTORY);
                                إذا (fd_grandparent < 0)
                                        أرجع -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                                        r = statx_unsafe_transition(&stx_ماپا, &stx_grandparent);
                                        إذا (r < 0)
                                                أرجع r;
                                        إذا (r > 0)
                                                أرجع log_unsafe_transition(fd_ماپا, fd_grandparent, رستہ, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                اكسر;
                        }

                        /* update fd and stat */
                        stx = stx_ماپا;
                        close_and_replace(fd, fd_ماپا);
                        استمر;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                بچہ = r = xopenat_بھریا(fd, پہلا,
                                         O_رستہ|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_سیٹ(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         منوال_INVALID);
                إذا (r < 0) {
                        إذا (r != -ENOENT)
                                أرجع r;

                        إذا (!isempty(todo) && !رستہ_is_محفوظ(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                أرجع r;

                        إذا (FLAGS_سیٹ(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_ماپا|CHASE_NONEXISTENT)))) {
                                بچہ = xopenat(fd,
                                                پہلا,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_رستہ|O_CLOEXEC);
                                إذا (بچہ < 0)
                                        أرجع بچہ;
                        } وإلا إذا (FLAGS_سیٹ(flags, CHASE_ماپا) && isempty(todo)) {
                                إذا (!رستہ_extend(&پورا, پہلا))
                                        أرجع -ENOMEM;

                                اكسر;
                        } وإلا إذا (FLAGS_سیٹ(flags, CHASE_NONEXISTENT)) {
                                إذا (!رستہ_extend(&پورا, پہلا, todo))
                                        أرجع -ENOMEM;

                                exists = false;
                                اكسر;
                        } وإلا
                                أرجع r;
                }

                r = chase_statx(بچہ, &stx_بچہ);
                إذا (r < 0)
                        أرجع r;

                إذا (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                        r = statx_unsafe_transition(&stx, &stx_بچہ);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r > 0)
                                أرجع log_unsafe_transition(fd, بچہ, رستہ, flags);
                }

                إذا (FLAGS_سیٹ(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(بچہ, AUTOFS_SUPER_MAGIC) > 0)
                        أرجع log_autofs_mount_نقطہ(بچہ, رستہ, flags);

                إذا (S_ISLNK(stx_بچہ.stx_منوال) && !(FLAGS_سیٹ(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ حرف *destination = عدم;

                        إذا (FLAGS_سیٹ(flags, CHASE_PROHIBIT_SYMLINKS))
                                أرجع log_prohibited_symlink(بچہ, flags);

                        r = readlinkat_malloc(fd, پہلا, &destination);
                        إذا (r < 0)
                                أرجع r;
                        إذا (isempty(destination))
                                أرجع -EINVAL;

                        إذا (رستہ_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                محفوظ_close(fd);
                                fd = fd_reopen(جڑ_fd, O_CLOEXEC|O_رستہ|O_DIRECTORY);
                                إذا (fd < 0)
                                        أرجع fd;

                                r = chase_statx(fd, &stx);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (FLAGS_سیٹ(flags, CHASE_محفوظ)) {
                                        r = statx_unsafe_transition(&stx_بچہ, &stx);
                                        إذا (r < 0)
                                                أرجع r;
                                        إذا (r > 0)
                                                أرجع log_unsafe_transition(بچہ, fd, رستہ, flags);
                                }

                                إذا (dir_fd != جڑ_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&پورا, need_absolute ? "/" : عدم);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        إذا (!رستہ_extend(&destination, todo))
                                أرجع -ENOMEM;

                        free_and_replace(بفر, destination);
                        todo = بفر;

                        إذا (FLAGS_سیٹ(flags, CHASE_قدم))
                                اذهب chased_one;

                        استمر;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                إذا (!رستہ_extend(&پورا, پہلا))
                        أرجع -ENOMEM;

                إذا (FLAGS_سیٹ(flags, CHASE_ماپا) && isempty(todo))
                        اكسر;

                /* And iterate again, but go one directory further down. */
                stx = stx_بچہ;
                close_and_replace(fd, بچہ);
        }

        إذا (exists) {
                إذا (FLAGS_سیٹ(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        إذا (r < 0)
                                أرجع r;
                }

                إذا (FLAGS_سیٹ(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        إذا (r < 0)
                                أرجع r;
                }

                إذا (FLAGS_سیٹ(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        إذا (ret_رستہ) {
                إذا (FLAGS_سیٹ(flags, CHASE_کڈھو_FILENAME) && پورا) {
                        _cleanup_free_ حرف *f = عدم;

                        r = رستہ_کڈھو_filename(پورا, &f);
                        إذا (r < 0 && r != -EADDRNOTAVAIL)
                                أرجع r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(پورا, f);
                }

                إذا (!پورا) {
                        أكد(!need_absolute || FLAGS_سیٹ(flags, CHASE_کڈھو_FILENAME));
                        پورا = strdup(".");
                        إذا (!پورا)
                                أرجع -ENOMEM;
                }

                إذا (append_trail_slash)
                        إذا (!strextend(&پورا, "/"))
                                أرجع -ENOMEM;

                *ret_رستہ = TAKE_PTR(پورا);
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

        إذا (FLAGS_سیٹ(flags, CHASE_قدم))
                أرجع 1;

        أرجع exists;

chased_one:
        إذا (ret_رستہ) {
                ثابت حرف *e;

                إذا (!پورا) {
                        أكد(!need_absolute);
                        پورا = strdup(append_trail_slash ? "./" : ".");
                        إذا (!پورا)
                                أرجع -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = رستہ_لبھو_پہلا_component(&todo, /* accept_dot_dot= */ true, &e);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        *ret_رستہ = TAKE_PTR(پورا);
                وإلا {
                        حرف *c;

                        c = رستہ_join(پورا, e);
                        إذا (!c)
                                أرجع -ENOMEM;

                        *ret_رستہ = c;
                }
        }

        أرجع 0;
}

صحيح chase(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags flags, حرف **ret_رستہ, صحيح *ret_fd) {
        _cleanup_free_ حرف *جڑ_abs = عدم, *absolute = عدم, *p = عدم;
        _cleanup_close_ صحيح fd = -EBADF, pfd = -EBADF;
        صحيح r;

        أكد(رستہ);

        إذا (isempty(رستہ))
                أرجع -EINVAL;

        r = خالی_تھاواں_or_جڑ_harder_منزل_null(&جڑ);
        إذا (r < 0)
                أرجع r;

        /* A root directory of "/" or "" is identical to "/". */
        إذا (خالی_تھاواں_or_جڑ(جڑ))
                جڑ = "/";
        وإلا {
                r = رستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                إذا (r < 0)
                        أرجع r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                جڑ = رستہ_simplify(جڑ_abs);

                أكد(رستہ_is_absolute(جڑ));
                أكد(!خالی_تھاواں_or_جڑ(جڑ));

                إذا (FLAGS_سیٹ(flags, CHASE_اگیتر_جڑ)) {
                        absolute = رستہ_join(جڑ, رستہ);
                        إذا (!absolute)
                                أرجع -ENOMEM;
                }
        }

        إذا (!absolute) {
                r = رستہ_make_absolute_cwd(رستہ, &absolute);
                إذا (r < 0)
                        أرجع r;
        }

        رستہ = رستہ_startswith(absolute, جڑ);
        إذا (!رستہ)
                أرجع log_بھریا_errno(FLAGS_سیٹ(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, جڑ);

        إذا (خالی_تھاواں_or_جڑ(جڑ))
                fd = XAT_FDROOT;
        وإلا {
                fd = open(جڑ, O_CLOEXEC|O_DIRECTORY|O_رستہ);
                إذا (fd < 0)
                        أرجع -errno;
        }

        r = chaseat(fd, fd, رستہ, flags & ~CHASE_اگیتر_جڑ, ret_رستہ ? &p : عدم, ret_fd ? &pfd : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ) {
                إذا (!FLAGS_سیٹ(flags, CHASE_کڈھو_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        إذا (خالی_تھاواں_or_جڑ(جڑ))
                                أكد(رستہ_is_absolute(p));
                        وإلا {
                                حرف *q;

                                أكد(!رستہ_is_absolute(p));

                                q = رستہ_join(جڑ, p + STR_وچ_سیٹ(p, ".", "./"));
                                إذا (!q)
                                        أرجع -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_رستہ = TAKE_PTR(p);
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        أرجع r;
}

صحيح chaseat_اگیتر_جڑ(ثابت حرف *رستہ, ثابت حرف *جڑ, حرف **ret) {
        حرف *q;
        صحيح r;

        أكد(رستہ);
        أكد(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        إذا (!رستہ_is_absolute(رستہ)) {
                _cleanup_free_ حرف *جڑ_abs = عدم;

                r = خالی_تھاواں_or_جڑ_harder_منزل_null(&جڑ);
                إذا (r < 0 && r != -ENOENT)
                        أرجع r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                إذا (خالی_تھاواں_or_جڑ(جڑ))
                        أرجع -EINVAL;

                r = رستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                إذا (r < 0)
                        أرجع r;

                جڑ = رستہ_simplify(جڑ_abs);

                q = رستہ_join(جڑ, رستہ + (رستہ[0] == '.' && وچ_سیٹ(رستہ[1], '/', '\0')));
        } وإلا
                q = strdup(رستہ);
        إذا (!q)
                أرجع -ENOMEM;

        *ret = q;
        أرجع 0;
}

صحيح chase_کڈھو_filename(ثابت حرف *رستہ, ثابت حرف *جڑ, حرف **ret) {
        صحيح r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        أكد(رستہ);
        أكد(ret);

        إذا (isempty(رستہ))
                أرجع -EINVAL;

        إذا (!رستہ_is_absolute(رستہ))
                أرجع -EINVAL;

        r = خالی_تھاواں_or_جڑ_harder_منزل_null(&جڑ);
        إذا (r < 0 && r != -ENOENT)
                أرجع r;

        إذا (!خالی_تھاواں_or_جڑ(جڑ)) {
                _cleanup_free_ حرف *جڑ_abs = عدم;

                r = رستہ_make_absolute_cwd(جڑ, &جڑ_abs);
                إذا (r < 0)
                        أرجع r;

                رستہ = رستہ_startswith(رستہ, جڑ_abs);
                إذا (!رستہ)
                        أرجع -EINVAL;
        }

        إذا (!isempty(رستہ)) {
                r = رستہ_کڈھو_filename(رستہ, ret);
                إذا (r != -EADDRNOTAVAIL)
                        أرجع r;
        }

        أرجع strdup_منزل(ret, ".");
}

صحيح chase_and_open(
                ثابت حرف *رستہ,
                ثابت حرف *جڑ,
                ChaseFlags chase_flags,
                صحيح open_flags,
                حرف **ret_رستہ) {

        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        ثابت حرف *open_name = عدم;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        إذا (خالی_تھاواں_or_جڑ(جڑ) && !ret_رستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                أرجع chase_xopenat(AT_FDCWD, رستہ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(رستہ, جڑ, (CHASE_ماپا|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        إذا (!FLAGS_سیٹ(chase_flags, CHASE_ماپا)) {
                إذا (FLAGS_سیٹ(chase_flags, CHASE_کڈھو_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                وإلا {
                        r = chase_کڈھو_filename(p, جڑ, &fname);
                        إذا (r < 0)
                                أرجع r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(رستہ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع r;
}

صحيح chase_and_opendir(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_رستہ, DIR **ret_dir) {
        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        DIR *d;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        إذا (خالی_تھاواں_or_جڑ(جڑ) && !ret_رستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(رستہ);
                إذا (!d)
                        أرجع -errno;

                *ret_dir = d;
                أرجع 0;
        }

        r = chase(رستہ, جڑ, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_رستہ ? &p : عدم, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        d = xopendirat(رستہ_fd, /* path= */ عدم, /* flags= */ 0);
        إذا (!d)
                أرجع -errno;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        *ret_dir = d;
        أرجع 0;
}

صحيح chase_and_stat(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_رستہ, بنية stat *ret_stat) {
        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));
        أكد(ret_stat);

        إذا (خالی_تھاواں_or_جڑ(جڑ) && !ret_رستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                أرجع RET_NERRNO(fstatat(AT_FDCWD, رستہ, ret_stat,
                                          FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(رستہ, جڑ, chase_flags, ret_رستہ ? &p : عدم, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        إذا (fstat(رستہ_fd, ret_stat) < 0)
                أرجع -errno;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_access(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحيح access_منوال, حرف **ret_رستہ) {
        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        إذا (خالی_تھاواں_or_جڑ(جڑ) && !ret_رستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                أرجع RET_NERRNO(faccessat(AT_FDCWD, رستہ, access_منوال,
                                            FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(رستہ, جڑ, chase_flags, ret_رستہ ? &p : عدم, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        r = access_fd(رستہ_fd, access_منوال);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_fopen_unlocked(
                ثابت حرف *رستہ,
                ثابت حرف *جڑ,
                ChaseFlags chase_flags,
                ثابت حرف *open_flags,
                حرف **ret_رستہ,
                FILE **ret_file) {

        _cleanup_free_ حرف *final_رستہ = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح منوال_flags, r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_ماپا|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        أكد(open_flags);
        أكد(ret_file);

        منوال_flags = fopen_منوال_منزل_flags(open_flags);
        إذا (منوال_flags < 0)
                أرجع منوال_flags;

        fd = chase_and_open(رستہ, جڑ, chase_flags, منوال_flags, ret_رستہ ? &final_رستہ : عدم);
        إذا (fd < 0)
                أرجع fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(final_رستہ);

        أرجع 0;
}

صحيح chase_and_unlink(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحيح unlink_flags, حرف **ret_رستہ) {
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_ماپا|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_کڈھو_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(رستہ, جڑ, chase_flags|CHASE_ماپا|CHASE_NOFOLLOW, O_رستہ|O_DIRECTORY|O_CLOEXEC, &p);
        إذا (fd < 0)
                أرجع fd;

        r = رستہ_کڈھو_filename(p, &fname);
        إذا (r < 0)
                أرجع r;

        إذا (unlinkat(fd, fname, unlink_flags) < 0)
                أرجع -errno;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_open_ماپا(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_filename) {
        صحيح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        r = chase(رستہ, جڑ, CHASE_ماپا|CHASE_کڈھو_FILENAME|chase_flags, ret_filename, &pfd);
        إذا (r < 0)
                أرجع r;

        أرجع pfd;
}

صحيح chase_and_openat(
                صحيح جڑ_fd,
                صحيح dir_fd,
                ثابت حرف *رستہ,
                ChaseFlags chase_flags,
                صحيح open_flags,
                حرف **ret_رستہ) {

        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        ثابت حرف *open_name = عدم;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_MUST_BE_SOCKET)));

        إذا (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_رستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                أرجع chase_xopenat(dir_fd, رستہ, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(جڑ_fd, dir_fd, رستہ, (chase_flags|CHASE_ماپا)&~CHASE_MUST_BE_REGULAR, &p, &رستہ_fd);
        إذا (r < 0)
                أرجع r;

        إذا (!FLAGS_سیٹ(chase_flags, CHASE_ماپا)) {
                إذا (FLAGS_سیٹ(chase_flags, CHASE_کڈھو_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                وإلا {
                        r = رستہ_کڈھو_filename(p, &fname);
                        إذا (r < 0 && r != -EADDRNOTAVAIL)
                                أرجع r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(رستہ_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع r;
}

صحيح chase_and_opendirat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, حرف **ret_رستہ, DIR **ret_dir) {
        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        DIR *d;
        صحيح r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        أكد(ret_dir);

        إذا (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_رستہ && (chase_flags & CHASE_NO_SHORTCUT_نقاب) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(رستہ);
                إذا (!d)
                        أرجع -errno;

                *ret_dir = d;
                أرجع 0;
        }

        r = chaseat(جڑ_fd, dir_fd, رستہ, chase_flags, ret_رستہ ? &p : عدم, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        d = xopendirat(رستہ_fd, /* path= */ عدم, /* flags= */ 0);
        إذا (!d)
                أرجع -errno;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        *ret_dir = d;
        أرجع 0;
}

صحيح chase_and_statat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, حرف **ret_رستہ, بنية stat *ret_stat) {
        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));
        أكد(ret_stat);

        إذا (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_رستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                أرجع RET_NERRNO(fstatat(AT_FDCWD, رستہ, ret_stat,
                                          FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(جڑ_fd, dir_fd, رستہ, chase_flags, ret_رستہ ? &p : عدم, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        إذا (fstat(رستہ_fd, ret_stat) < 0)
                أرجع -errno;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_accessat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, صحيح access_منوال, حرف **ret_رستہ) {
        _cleanup_close_ صحيح رستہ_fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        إذا (جڑ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_رستہ && (chase_flags & (CHASE_NO_SHORTCUT_نقاب|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                أرجع RET_NERRNO(faccessat(AT_FDCWD, رستہ, access_منوال,
                                            FLAGS_سیٹ(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(جڑ_fd, dir_fd, رستہ, chase_flags, ret_رستہ ? &p : عدم, &رستہ_fd);
        إذا (r < 0)
                أرجع r;
        أكد(رستہ_fd >= 0);

        r = access_fd(رستہ_fd, access_منوال);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_fopenat_unlocked(
                صحيح جڑ_fd,
                صحيح dir_fd,
                ثابت حرف *رستہ,
                ChaseFlags chase_flags,
                ثابت حرف *open_flags,
                حرف **ret_رستہ,
                FILE **ret_file) {

        _cleanup_free_ حرف *final_رستہ = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح منوال_flags, r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_ماپا)));
        أكد(open_flags);
        أكد(ret_file);

        منوال_flags = fopen_منوال_منزل_flags(open_flags);
        إذا (منوال_flags < 0)
                أرجع منوال_flags;

        fd = chase_and_openat(جڑ_fd, dir_fd, رستہ, chase_flags, منوال_flags, ret_رستہ ? &final_رستہ : عدم);
        إذا (fd < 0)
                أرجع fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        إذا (r < 0)
                أرجع r;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(final_رستہ);

        أرجع 0;
}

صحيح chase_and_unlinkat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, صحيح unlink_flags, حرف **ret_رستہ) {
        _cleanup_free_ حرف *p = عدم, *fname = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(رستہ);
        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم|CHASE_ماپا|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_کڈھو_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(جڑ_fd, dir_fd, رستہ, chase_flags|CHASE_ماپا|CHASE_NOFOLLOW, O_رستہ|O_DIRECTORY|O_CLOEXEC, &p);
        إذا (fd < 0)
                أرجع fd;

        r = رستہ_کڈھو_filename(p, &fname);
        إذا (r < 0)
                أرجع r;

        إذا (unlinkat(fd, fname, unlink_flags) < 0)
                أرجع -errno;

        إذا (ret_رستہ)
                *ret_رستہ = TAKE_PTR(p);

        أرجع 0;
}

صحيح chase_and_open_ماپا_at(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, حرف **ret_filename) {
        صحيح pfd, r;

        أكد(!(chase_flags & (CHASE_NONEXISTENT|CHASE_قدم)));

        r = chaseat(جڑ_fd, dir_fd, رستہ, CHASE_ماپا|CHASE_کڈھو_FILENAME|chase_flags, ret_filename, &pfd);
        إذا (r < 0)
                أرجع r;

        أرجع pfd;
}
