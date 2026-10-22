/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/magic.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "glyph-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "user-util.h"

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

স্থির পূর্ণসংখ্যা chase_statx(পূর্ণসংখ্যা fd, কাঠামো statx *ret) {
        ফেরত xstatx_full(fd,
                        /* path= */ ফাঁকা,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

স্থির পূর্ণসংখ্যা chase_openat2(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        স্থির bool can_openat2 = true;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));

        যদি (!can_openat2)
                ফেরত -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        যদি ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                ফেরত -EOPNOTSUPP;
        যদি (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                ফেরত -EOPNOTSUPP;
        যদি (মূল_fd != XAT_FDROOT && মূল_fd != dir_fd)
                ফেরত -EOPNOTSUPP;

        _cleanup_close_ পূর্ণসংখ্যা dir_fd_local = -EBADF;
        যদি (dir_fd == XAT_FDROOT) {
                যদি (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                অন্যথা {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        যদি (dir_fd_local < 0)
                                ফেরত -errno;
                        dir_fd = dir_fd_local;
                }
        }

        কাঠামো open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        যদি (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        যদি (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        যদি (মূল_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_মূল;
        যদি (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ পূর্ণসংখ্যা fd = openat2(dir_fd, path, &how, আকার(how));
        যদি (fd < 0) {
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
                যদি (errno == ENOSYS)
                        can_openat2 = false;
                যদি (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        ফেরত -EOPNOTSUPP;
                ফেরত -errno;
        }

        যদি (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                যদি (r < 0)
                        ফেরত r;
        }
        যদি (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত TAKE_FD(fd);
}

স্থির পূর্ণসংখ্যা chase_xopenat(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, পূর্ণসংখ্যা open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        যদি (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        যদি (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        যদি (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        যদি (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        যদি (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        ফেরত xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

স্থির bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        যদি (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                ফেরত false;

        ফেরত a != b; /* Otherwise we need to stay within the same UID */
}

পূর্ণসংখ্যা statx_unsafe_transition(ধ্রুবক কাঠামো statx *a, ধ্রুবক কাঠামো statx *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                ফেরত -ENODATA;

        ফেরত uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ধ্রুবক কাঠামো stat *a, ধ্রুবক কাঠামো stat *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত uid_unsafe_transition(a->st_uid, b->st_uid);
}

স্থির পূর্ণসংখ্যা log_unsafe_transition(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b, ধ্রুবক অক্ষর *path, ChaseFlags flags) {
        _cleanup_free_ অক্ষর *n1 = ফাঁকা, *n2 = ফাঁকা, *user_a = ফাঁকা, *user_b = ফাঁকা;
        কাঠামো stat st;

        যদি (!FLAGS_SET(flags, CHASE_WARN))
                ফেরত -ENOLINK;

        (শূন্য) fd_get_path(a, &n1);
        (শূন্য) fd_get_path(b, &n2);

        যদি (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        যদি (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        ফেরত log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_ডান), strna(n2), strna(user_b), path);
}

স্থির পূর্ণসংখ্যা log_autofs_mount_point(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ChaseFlags flags) {
        _cleanup_free_ অক্ষর *n1 = ফাঁকা;

        যদি (!FLAGS_SET(flags, CHASE_WARN))
                ফেরত -EREMOTE;

        (শূন্য) fd_get_path(fd, &n1);

        ফেরত log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

স্থির পূর্ণসংখ্যা log_prohibited_symlink(পূর্ণসংখ্যা fd, ChaseFlags flags) {
        _cleanup_free_ অক্ষর *n1 = ফাঁকা;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!FLAGS_SET(flags, CHASE_WARN))
                ফেরত -ELOOP;

        (শূন্য) fd_get_path(fd, &n1);

        ফেরত log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

পূর্ণসংখ্যা chaseat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags flags, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!FLAGS_SET(flags, CHASE_PREFIX_মূল));
        প্রতিজ্ঞা(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        প্রতিজ্ঞা(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(wildcard_fd_is_valid(মূল_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        প্রতিজ্ঞা(dir_fd != AT_FDCWD || IN_SET(মূল_fd, AT_FDCWD, XAT_FDROOT));

        যদি (FLAGS_SET(flags, CHASE_STEP))
                প্রতিজ্ঞা(!ret_fd);

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
        যদি (মূল_fd == AT_FDCWD)
                মূল_fd = XAT_FDROOT;
        অন্যথা {
                r = dir_fd_is_মূল(মূল_fd);
                যদি (r < 0)
                        ফেরত r;
                যদি (r > 0)
                        মূল_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_মূল(dir_fd);
        যদি (r < 0)
                ফেরত r;
        যদি (r > 0 && মূল_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        যদি (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = মূল_fd;

        যদি (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        যদি (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                যদি (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } অন্যথা যদি (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        যদি (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        যদি (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                ফেরত -EBADSLT;

        যদি (!ret_path) {
                r = chase_openat2(মূল_fd, dir_fd, path, flags);
                যদি (r >= 0) {
                        যদি (ret_fd)
                                *ret_fd = r;
                        অন্যথা
                                safe_close(r);

                        ফেরত 1;
                }
                যদি (r != -EOPNOTSUPP)
                        ফেরত r;
        }

        যদি (মূল_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                যদি (r < 0)
                        ফেরত r;

                যদি (ret_fd)
                        *ret_fd = r;
                অন্যথা
                        safe_close(r);

                ফেরত 1;
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
        bool need_absolute = (মূল_fd == XAT_FDROOT || dir_fd != মূল_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ অক্ষর *done = ফাঁকা;
        যদি (need_absolute) {
                done = strdup("/");
                যদি (!done)
                        ফেরত -ENOMEM;
        }

        _cleanup_close_ পূর্ণসংখ্যা fd = xopenat(dir_fd, ফাঁকা, O_CLOEXEC|O_DIRECTORY|O_PATH);
        যদি (fd < 0)
                ফেরত fd;

        কাঠামো statx stx;
        r = chase_statx(fd, &stx);
        যদি (r < 0)
                ফেরত r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        কাঠামো statx মূল_stx;
        যদি (মূল_fd != XAT_FDROOT) {
                যদি (মূল_fd == dir_fd)
                        মূল_stx = stx;
                অন্যথা {
                        r = chase_statx(মূল_fd, &মূল_stx);
                        যদি (r < 0)
                                ফেরত r;
                }
        }

        _cleanup_free_ অক্ষর *বাফার = strdup(path);
        যদি (!বাফার)
                ফেরত -ENOMEM;

        ধ্রুবক অক্ষর *todo = বাফার;
        bool exists = true;
        জন্য (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ অক্ষর *first = ফাঁকা;
                _cleanup_close_ পূর্ণসংখ্যা child = -EBADF;
                কাঠামো statx stx_child;
                ধ্রুবক অক্ষর *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                যদি (n_steps > CHASE_MAX)
                        ফেরত -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0) /* We reached the end. */
                        বিরতি;

                first = strndup(e, r);
                যদি (!first)
                        ফেরত -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                যদি (streq(first, "..")) {
                        _cleanup_free_ অক্ষর *parent = ফাঁকা;
                        _cleanup_close_ পূর্ণসংখ্যা fd_parent = -EBADF;
                        কাঠামো statx stx_parent;

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
                        যদি (মূল_fd != XAT_FDROOT) {
                                bool is_মূল = মূল_fd == dir_fd && empty_or_মূল(done);
                                যদি (!is_মূল && statx_inode_same(&stx, &মূল_stx)) {
                                        r = statx_mount_same(&stx, &মূল_stx);
                                        যদি (r < 0)
                                                ফেরত r;

                                        is_মূল = r > 0;
                                }
                                যদি (is_মূল) {
                                        যদি (FLAGS_SET(flags, CHASE_STEP))
                                                যাও chased_one;
                                        চালু;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        যদি (fd_parent < 0)
                                ফেরত -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        যদি (r < 0)
                                ফেরত r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        যদি (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_মূল(fd);
                                যদি (r < 0)
                                        ফেরত r;
                                যদি (r > 0) {
                                        যদি (FLAGS_SET(flags, CHASE_STEP))
                                                যাও chased_one;
                                        চালু;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        যদি (r >= 0) {
                                প্রতিজ্ঞা(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } অন্যথা যদি (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                প্রতিজ্ঞা(!need_absolute);
                                done = mfree(done);
                        } অন্যথা যদি (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } অন্যথা যদি (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                প্রতিজ্ঞা(!need_absolute);

                                যদি (!isempty(done) && !path_is_valid(done))
                                        ফেরত -EINVAL;

                                যদি (!path_extend(&done, ".."))
                                        ফেরত -ENOMEM;
                        } অন্যথা
                                ফেরত r;

                        যদি (FLAGS_SET(flags, CHASE_STEP))
                                যাও chased_one;

                        যদি (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                যদি (r < 0)
                                        ফেরত r;
                                যদি (r > 0)
                                        ফেরত log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        যদি (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ পূর্ণসংখ্যা fd_grandparent = -EBADF;
                                কাঠামো statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                যদি (fd_grandparent < 0)
                                        ফেরত -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        যদি (r < 0)
                                                ফেরত r;
                                        যদি (r > 0)
                                                ফেরত log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                বিরতি;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        চালু;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                যদি (r < 0) {
                        যদি (r != -ENOENT)
                                ফেরত r;

                        যদি (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                ফেরত r;

                        যদি (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                যদি (child < 0)
                                        ফেরত child;
                        } অন্যথা যদি (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                যদি (!path_extend(&done, first))
                                        ফেরত -ENOMEM;

                                বিরতি;
                        } অন্যথা যদি (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                যদি (!path_extend(&done, first, todo))
                                        ফেরত -ENOMEM;

                                exists = false;
                                বিরতি;
                        } অন্যথা
                                ফেরত r;
                }

                r = chase_statx(child, &stx_child);
                যদি (r < 0)
                        ফেরত r;

                যদি (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r > 0)
                                ফেরত log_unsafe_transition(fd, child, path, flags);
                }

                যদি (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        ফেরত log_autofs_mount_point(child, path, flags);

                যদি (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ অক্ষর *destination = ফাঁকা;

                        যদি (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                ফেরত log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (isempty(destination))
                                ফেরত -EINVAL;

                        যদি (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(মূল_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                যদি (fd < 0)
                                        ফেরত fd;

                                r = chase_statx(fd, &stx);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        যদি (r < 0)
                                                ফেরত r;
                                        যদি (r > 0)
                                                ফেরত log_unsafe_transition(child, fd, path, flags);
                                }

                                যদি (dir_fd != মূল_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : ফাঁকা);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        যদি (!path_extend(&destination, todo))
                                ফেরত -ENOMEM;

                        free_and_replace(বাফার, destination);
                        todo = বাফার;

                        যদি (FLAGS_SET(flags, CHASE_STEP))
                                যাও chased_one;

                        চালু;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                যদি (!path_extend(&done, first))
                        ফেরত -ENOMEM;

                যদি (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        বিরতি;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        যদি (exists) {
                যদি (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        যদি (r < 0)
                                ফেরত r;
                }

                যদি (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        যদি (r < 0)
                                ফেরত r;
                }

                যদি (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        যদি (r < 0)
                                ফেরত r;
                }
        }

        যদি (ret_path) {
                যদি (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ অক্ষর *f = ফাঁকা;

                        r = path_extract_filename(done, &f);
                        যদি (r < 0 && r != -EADDRNOTAVAIL)
                                ফেরত r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                যদি (!done) {
                        প্রতিজ্ঞা(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        যদি (!done)
                                ফেরত -ENOMEM;
                }

                যদি (append_trail_slash)
                        যদি (!strextend(&done, "/"))
                                ফেরত -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        যদি (ret_fd) {
                যদি (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        প্রতিজ্ঞা(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } অন্যথা
                        *ret_fd = -EBADF;
        }

        যদি (FLAGS_SET(flags, CHASE_STEP))
                ফেরত 1;

        ফেরত exists;

chased_one:
        যদি (ret_path) {
                ধ্রুবক অক্ষর *e;

                যদি (!done) {
                        প্রতিজ্ঞা(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        যদি (!done)
                                ফেরত -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        *ret_path = TAKE_PTR(done);
                অন্যথা {
                        অক্ষর *c;

                        c = path_join(done, e);
                        যদি (!c)
                                ফেরত -ENOMEM;

                        *ret_path = c;
                }
        }

        ফেরত 0;
}

পূর্ণসংখ্যা chase(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags flags, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd) {
        _cleanup_free_ অক্ষর *মূল_abs = ফাঁকা, *absolute = ফাঁকা, *p = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF, pfd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        যদি (isempty(path))
                ফেরত -EINVAL;

        r = empty_or_মূল_harder_to_null(&মূল);
        যদি (r < 0)
                ফেরত r;

        /* A root directory of "/" or "" is identical to "/". */
        যদি (empty_or_মূল(মূল))
                মূল = "/";
        অন্যথা {
                r = path_make_absolute_cwd(মূল, &মূল_abs);
                যদি (r < 0)
                        ফেরত r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                মূল = path_simplify(মূল_abs);

                প্রতিজ্ঞা(path_is_absolute(মূল));
                প্রতিজ্ঞা(!empty_or_মূল(মূল));

                যদি (FLAGS_SET(flags, CHASE_PREFIX_মূল)) {
                        absolute = path_join(মূল, path);
                        যদি (!absolute)
                                ফেরত -ENOMEM;
                }
        }

        যদি (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                যদি (r < 0)
                        ফেরত r;
        }

        path = path_startswith(absolute, মূল);
        যদি (!path)
                ফেরত log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, মূল);

        যদি (empty_or_মূল(মূল))
                fd = XAT_FDROOT;
        অন্যথা {
                fd = open(মূল, O_CLOEXEC|O_DIRECTORY|O_PATH);
                যদি (fd < 0)
                        ফেরত -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_মূল, ret_path ? &p : ফাঁকা, ret_fd ? &pfd : ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path) {
                যদি (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        যদি (empty_or_মূল(মূল))
                                প্রতিজ্ঞা(path_is_absolute(p));
                        অন্যথা {
                                অক্ষর *q;

                                প্রতিজ্ঞা(!path_is_absolute(p));

                                q = path_join(মূল, p + STR_IN_SET(p, ".", "./"));
                                যদি (!q)
                                        ফেরত -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        ফেরত r;
}

পূর্ণসংখ্যা chaseat_prefix_মূল(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, অক্ষর **ret) {
        অক্ষর *q;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        যদি (!path_is_absolute(path)) {
                _cleanup_free_ অক্ষর *মূল_abs = ফাঁকা;

                r = empty_or_মূল_harder_to_null(&মূল);
                যদি (r < 0 && r != -ENOENT)
                        ফেরত r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                যদি (empty_or_মূল(মূল))
                        ফেরত -EINVAL;

                r = path_make_absolute_cwd(মূল, &মূল_abs);
                যদি (r < 0)
                        ফেরত r;

                মূল = path_simplify(মূল_abs);

                q = path_join(মূল, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } অন্যথা
                q = strdup(path);
        যদি (!q)
                ফেরত -ENOMEM;

        *ret = q;
        ফেরত 0;
}

পূর্ণসংখ্যা chase_extract_filename(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        যদি (isempty(path))
                ফেরত -EINVAL;

        যদি (!path_is_absolute(path))
                ফেরত -EINVAL;

        r = empty_or_মূল_harder_to_null(&মূল);
        যদি (r < 0 && r != -ENOENT)
                ফেরত r;

        যদি (!empty_or_মূল(মূল)) {
                _cleanup_free_ অক্ষর *মূল_abs = ফাঁকা;

                r = path_make_absolute_cwd(মূল, &মূল_abs);
                যদি (r < 0)
                        ফেরত r;

                path = path_startswith(path, মূল_abs);
                যদি (!path)
                        ফেরত -EINVAL;
        }

        যদি (!isempty(path)) {
                r = path_extract_filename(path, ret);
                যদি (r != -EADDRNOTAVAIL)
                        ফেরত r;
        }

        ফেরত strdup_to(ret, ".");
}

পূর্ণসংখ্যা chase_and_open(
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *মূল,
                ChaseFlags chase_flags,
                পূর্ণসংখ্যা open_flags,
                অক্ষর **ret_path) {

        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা, *fname = ফাঁকা;
        ধ্রুবক অক্ষর *open_name = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        যদি (empty_or_মূল(মূল) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ফেরত chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, মূল, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        যদি (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                যদি (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                অন্যথা {
                        r = chase_extract_filename(p, মূল, &fname);
                        যদি (r < 0)
                                ফেরত r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত r;
}

পূর্ণসংখ্যা chase_and_opendir(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, অক্ষর **ret_path, DIR **ret_dir) {
        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        DIR *d;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        প্রতিজ্ঞা(ret_dir);

        যদি (empty_or_মূল(মূল) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                যদি (!d)
                        ফেরত -errno;

                *ret_dir = d;
                ফেরত 0;
        }

        r = chase(path, মূল, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : ফাঁকা, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ফাঁকা, /* flags= */ 0);
        যদি (!d)
                ফেরত -errno;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_stat(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, অক্ষর **ret_path, কাঠামো stat *ret_stat) {
        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        প্রতিজ্ঞা(ret_stat);

        যদি (empty_or_মূল(মূল) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ফেরত RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, মূল, chase_flags, ret_path ? &p : ফাঁকা, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        যদি (fstat(path_fd, ret_stat) < 0)
                ফেরত -errno;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_access(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, পূর্ণসংখ্যা access_mode, অক্ষর **ret_path) {
        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        যদি (empty_or_মূল(মূল) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ফেরত RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, মূল, chase_flags, ret_path ? &p : ফাঁকা, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_fopen_unlocked(
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *মূল,
                ChaseFlags chase_flags,
                ধ্রুবক অক্ষর *open_flags,
                অক্ষর **ret_path,
                FILE **ret_file) {

        _cleanup_free_ অক্ষর *final_path = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা mode_flags, r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        প্রতিজ্ঞা(open_flags);
        প্রতিজ্ঞা(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        যদি (mode_flags < 0)
                ফেরত mode_flags;

        fd = chase_and_open(path, মূল, chase_flags, mode_flags, ret_path ? &final_path : ফাঁকা);
        যদি (fd < 0)
                ফেরত fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_unlink(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, পূর্ণসংখ্যা unlink_flags, অক্ষর **ret_path) {
        _cleanup_free_ অক্ষর *p = ফাঁকা, *fname = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, মূল, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        যদি (fd < 0)
                ফেরত fd;

        r = path_extract_filename(p, &fname);
        যদি (r < 0)
                ফেরত r;

        যদি (unlinkat(fd, fname, unlink_flags) < 0)
                ফেরত -errno;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_open_parent(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, অক্ষর **ret_filename) {
        পূর্ণসংখ্যা pfd, r;

        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, মূল, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        যদি (r < 0)
                ফেরত r;

        ফেরত pfd;
}

পূর্ণসংখ্যা chase_and_openat(
                পূর্ণসংখ্যা মূল_fd,
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *path,
                ChaseFlags chase_flags,
                পূর্ণসংখ্যা open_flags,
                অক্ষর **ret_path) {

        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা, *fname = ফাঁকা;
        ধ্রুবক অক্ষর *open_name = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        যদি (মূল_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ফেরত chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(মূল_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        যদি (r < 0)
                ফেরত r;

        যদি (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                যদি (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                অন্যথা {
                        r = path_extract_filename(p, &fname);
                        যদি (r < 0 && r != -EADDRNOTAVAIL)
                                ফেরত r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত r;
}

পূর্ণসংখ্যা chase_and_opendirat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, অক্ষর **ret_path, DIR **ret_dir) {
        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        DIR *d;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        প্রতিজ্ঞা(ret_dir);

        যদি (মূল_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                যদি (!d)
                        ফেরত -errno;

                *ret_dir = d;
                ফেরত 0;
        }

        r = chaseat(মূল_fd, dir_fd, path, chase_flags, ret_path ? &p : ফাঁকা, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ফাঁকা, /* flags= */ 0);
        যদি (!d)
                ফেরত -errno;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_statat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, অক্ষর **ret_path, কাঠামো stat *ret_stat) {
        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        প্রতিজ্ঞা(ret_stat);

        যদি (মূল_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ফেরত RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(মূল_fd, dir_fd, path, chase_flags, ret_path ? &p : ফাঁকা, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        যদি (fstat(path_fd, ret_stat) < 0)
                ফেরত -errno;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_accessat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, পূর্ণসংখ্যা access_mode, অক্ষর **ret_path) {
        _cleanup_close_ পূর্ণসংখ্যা path_fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        যদি (মূল_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ফেরত RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(মূল_fd, dir_fd, path, chase_flags, ret_path ? &p : ফাঁকা, &path_fd);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_fopenat_unlocked(
                পূর্ণসংখ্যা মূল_fd,
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *path,
                ChaseFlags chase_flags,
                ধ্রুবক অক্ষর *open_flags,
                অক্ষর **ret_path,
                FILE **ret_file) {

        _cleanup_free_ অক্ষর *final_path = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা mode_flags, r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        প্রতিজ্ঞা(open_flags);
        প্রতিজ্ঞা(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        যদি (mode_flags < 0)
                ফেরত mode_flags;

        fd = chase_and_openat(মূল_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : ফাঁকা);
        যদি (fd < 0)
                ফেরত fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_unlinkat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, পূর্ণসংখ্যা unlink_flags, অক্ষর **ret_path) {
        _cleanup_free_ অক্ষর *p = ফাঁকা, *fname = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(মূল_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        যদি (fd < 0)
                ফেরত fd;

        r = path_extract_filename(p, &fname);
        যদি (r < 0)
                ফেরত r;

        যদি (unlinkat(fd, fname, unlink_flags) < 0)
                ফেরত -errno;

        যদি (ret_path)
                *ret_path = TAKE_PTR(p);

        ফেরত 0;
}

পূর্ণসংখ্যা chase_and_open_parent_at(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, অক্ষর **ret_filename) {
        পূর্ণসংখ্যা pfd, r;

        প্রতিজ্ঞা(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(মূল_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        যদি (r < 0)
                ফেরত r;

        ফেরত pfd;
}
