/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/magic.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "glyph-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_statx(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା statx *ret) {
        ଫେରନ୍ତୁ xstatx_full(fd,
                        /* path= */ ଖାଲି,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_openat2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        ସ୍ଥିର bool can_openat2 = true;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));

        ଯଦି (!can_openat2)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        ଯଦି ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        ଯଦି (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        ଯଦି (ମୂଳ_fd != XAT_FDROOT && ମୂଳ_fd != dir_fd)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd_local = -EBADF;
        ଯଦି (dir_fd == XAT_FDROOT) {
                ଯଦି (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                ଅନ୍ୟଥା {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        ଯଦି (dir_fd_local < 0)
                                ଫେରନ୍ତୁ -errno;
                        dir_fd = dir_fd_local;
                }
        }

        ସଂରଚନା open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        ଯଦି (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        ଯଦି (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        ଯଦି (ମୂଳ_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_ମୂଳ;
        ଯଦି (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = openat2(dir_fd, path, &how, ଆକାର(how));
        ଯଦି (fd < 0) {
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
                ଯଦି (errno == ENOSYS)
                        can_openat2 = false;
                ଯଦି (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        ଫେରନ୍ତୁ -EOPNOTSUPP;
                ଫେରନ୍ତୁ -errno;
        }

        ଯଦି (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }
        ଯଦି (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_xopenat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        ଯଦି (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        ଯଦି (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        ଯଦି (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        ଯଦି (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        ଯଦି (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        ଫେରନ୍ତୁ xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

ସ୍ଥିର bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        ଯଦି (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ a != b; /* Otherwise we need to stay within the same UID */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_unsafe_transition(ଅଚଳ ସଂରଚନା statx *a, ଅଚଳ ସଂରଚନା statx *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଯଦି (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                ଫେରନ୍ତୁ -ENODATA;

        ଫେରନ୍ତୁ uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ uid_unsafe_transition(a->st_uid, b->st_uid);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_unsafe_transition(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags flags) {
        _cleanup_free_ ଅକ୍ଷର *n1 = ଖାଲି, *n2 = ଖାଲି, *user_a = ଖାଲି, *user_b = ଖାଲି;
        ସଂରଚନା stat st;

        ଯଦି (!FLAGS_SET(flags, CHASE_WARN))
                ଫେରନ୍ତୁ -ENOLINK;

        (ଶୂନ୍ୟ) fd_get_path(a, &n1);
        (ଶୂନ୍ୟ) fd_get_path(b, &n2);

        ଯଦି (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        ଯଦି (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        ଫେରନ୍ତୁ log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_ଡାହାଣ), strna(n2), strna(user_b), path);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_autofs_mount_point(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags flags) {
        _cleanup_free_ ଅକ୍ଷର *n1 = ଖାଲି;

        ଯଦି (!FLAGS_SET(flags, CHASE_WARN))
                ଫେରନ୍ତୁ -EREMOTE;

        (ଶୂନ୍ୟ) fd_get_path(fd, &n1);

        ଫେରନ୍ତୁ log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_prohibited_symlink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ChaseFlags flags) {
        _cleanup_free_ ଅକ୍ଷର *n1 = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (!FLAGS_SET(flags, CHASE_WARN))
                ଫେରନ୍ତୁ -ELOOP;

        (ଶୂନ୍ୟ) fd_get_path(fd, &n1);

        ଫେରନ୍ତୁ log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chaseat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags flags, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, CHASE_PREFIX_ମୂଳ));
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(ମୂଳ_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        ପ୍ରତିଜ୍ଞା(dir_fd != AT_FDCWD || IN_SET(ମୂଳ_fd, AT_FDCWD, XAT_FDROOT));

        ଯଦି (FLAGS_SET(flags, CHASE_STEP))
                ପ୍ରତିଜ୍ଞା(!ret_fd);

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
        ଯଦି (ମୂଳ_fd == AT_FDCWD)
                ମୂଳ_fd = XAT_FDROOT;
        ଅନ୍ୟଥା {
                r = dir_fd_is_ମୂଳ(ମୂଳ_fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        ମୂଳ_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_ମୂଳ(dir_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r > 0 && ମୂଳ_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        ଯଦି (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = ମୂଳ_fd;

        ଯଦି (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        ଯଦି (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                ଯଦି (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } ଅନ୍ୟଥା ଯଦି (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        ଯଦି (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        ଯଦି (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                ଫେରନ୍ତୁ -EBADSLT;

        ଯଦି (!ret_path) {
                r = chase_openat2(ମୂଳ_fd, dir_fd, path, flags);
                ଯଦି (r >= 0) {
                        ଯଦି (ret_fd)
                                *ret_fd = r;
                        ଅନ୍ୟଥା
                                safe_close(r);

                        ଫେରନ୍ତୁ 1;
                }
                ଯଦି (r != -EOPNOTSUPP)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ମୂଳ_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (ret_fd)
                        *ret_fd = r;
                ଅନ୍ୟଥା
                        safe_close(r);

                ଫେରନ୍ତୁ 1;
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
        bool need_absolute = (ମୂଳ_fd == XAT_FDROOT || dir_fd != ମୂଳ_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ ଅକ୍ଷର *done = ଖାଲି;
        ଯଦି (need_absolute) {
                done = strdup("/");
                ଯଦି (!done)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = xopenat(dir_fd, ଖାଲି, O_CLOEXEC|O_DIRECTORY|O_PATH);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ସଂରଚନା statx stx;
        r = chase_statx(fd, &stx);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        ସଂରଚନା statx ମୂଳ_stx;
        ଯଦି (ମୂଳ_fd != XAT_FDROOT) {
                ଯଦି (ମୂଳ_fd == dir_fd)
                        ମୂଳ_stx = stx;
                ଅନ୍ୟଥା {
                        r = chase_statx(ମୂଳ_fd, &ମୂଳ_stx);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }
        }

        _cleanup_free_ ଅକ୍ଷର *buffer = strdup(path);
        ଯଦି (!buffer)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅଚଳ ଅକ୍ଷର *todo = buffer;
        bool exists = true;
        ପାଇଁ (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ ଅକ୍ଷର *first = ଖାଲି;
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା child = -EBADF;
                ସଂରଚନା statx stx_child;
                ଅଚଳ ଅକ୍ଷର *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                ଯଦି (n_steps > CHASE_MAX)
                        ଫେରନ୍ତୁ -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0) /* We reached the end. */
                        ଭାଙ୍ଗ;

                first = strndup(e, r);
                ଯଦି (!first)
                        ଫେରନ୍ତୁ -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                ଯଦି (streq(first, "..")) {
                        _cleanup_free_ ଅକ୍ଷର *parent = ଖାଲି;
                        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_parent = -EBADF;
                        ସଂରଚନା statx stx_parent;

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
                        ଯଦି (ମୂଳ_fd != XAT_FDROOT) {
                                bool is_ମୂଳ = ମୂଳ_fd == dir_fd && empty_or_ମୂଳ(done);
                                ଯଦି (!is_ମୂଳ && statx_inode_same(&stx, &ମୂଳ_stx)) {
                                        r = statx_mount_same(&stx, &ମୂଳ_stx);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ r;

                                        is_ମୂଳ = r > 0;
                                }
                                ଯଦି (is_ମୂଳ) {
                                        ଯଦି (FLAGS_SET(flags, CHASE_STEP))
                                                ଯାଅ chased_one;
                                        ଜାରି;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        ଯଦି (fd_parent < 0)
                                ଫେରନ୍ତୁ -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        ଯଦି (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_ମୂଳ(fd);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                                ଯଦି (r > 0) {
                                        ଯଦି (FLAGS_SET(flags, CHASE_STEP))
                                                ଯାଅ chased_one;
                                        ଜାରି;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        ଯଦି (r >= 0) {
                                ପ୍ରତିଜ୍ଞା(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } ଅନ୍ୟଥା ଯଦି (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                ପ୍ରତିଜ୍ଞା(!need_absolute);
                                done = mfree(done);
                        } ଅନ୍ୟଥା ଯଦି (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } ଅନ୍ୟଥା ଯଦି (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                ପ୍ରତିଜ୍ଞା(!need_absolute);

                                ଯଦି (!isempty(done) && !path_is_valid(done))
                                        ଫେରନ୍ତୁ -EINVAL;

                                ଯଦି (!path_extend(&done, ".."))
                                        ଫେରନ୍ତୁ -ENOMEM;
                        } ଅନ୍ୟଥା
                                ଫେରନ୍ତୁ r;

                        ଯଦି (FLAGS_SET(flags, CHASE_STEP))
                                ଯାଅ chased_one;

                        ଯଦି (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                                ଯଦି (r > 0)
                                        ଫେରନ୍ତୁ log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        ଯଦି (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_grandparent = -EBADF;
                                ସଂରଚନା statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                ଯଦି (fd_grandparent < 0)
                                        ଫେରନ୍ତୁ -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ r;
                                        ଯଦି (r > 0)
                                                ଫେରନ୍ତୁ log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                ଭାଙ୍ଗ;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        ଜାରି;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                ଯଦି (r < 0) {
                        ଯଦି (r != -ENOENT)
                                ଫେରନ୍ତୁ r;

                        ଯଦି (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                ଫେରନ୍ତୁ r;

                        ଯଦି (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                ଯଦି (child < 0)
                                        ଫେରନ୍ତୁ child;
                        } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                ଯଦି (!path_extend(&done, first))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                ଭାଙ୍ଗ;
                        } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                ଯଦି (!path_extend(&done, first, todo))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                exists = false;
                                ଭାଙ୍ଗ;
                        } ଅନ୍ୟଥା
                                ଫେରନ୍ତୁ r;
                }

                r = chase_statx(child, &stx_child);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r > 0)
                                ଫେରନ୍ତୁ log_unsafe_transition(fd, child, path, flags);
                }

                ଯଦି (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        ଫେରନ୍ତୁ log_autofs_mount_point(child, path, flags);

                ଯଦି (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ ଅକ୍ଷର *destination = ଖାଲି;

                        ଯଦି (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                ଫେରନ୍ତୁ log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (isempty(destination))
                                ଫେରନ୍ତୁ -EINVAL;

                        ଯଦି (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(ମୂଳ_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                ଯଦି (fd < 0)
                                        ଫେରନ୍ତୁ fd;

                                r = chase_statx(fd, &stx);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ r;
                                        ଯଦି (r > 0)
                                                ଫେରନ୍ତୁ log_unsafe_transition(child, fd, path, flags);
                                }

                                ଯଦି (dir_fd != ମୂଳ_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : ଖାଲି);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        ଯଦି (!path_extend(&destination, todo))
                                ଫେରନ୍ତୁ -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        ଯଦି (FLAGS_SET(flags, CHASE_STEP))
                                ଯାଅ chased_one;

                        ଜାରି;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                ଯଦି (!path_extend(&done, first))
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        ଭାଙ୍ଗ;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        ଯଦି (exists) {
                ଯଦି (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }
        }

        ଯଦି (ret_path) {
                ଯଦି (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ ଅକ୍ଷର *f = ଖାଲି;

                        r = path_extract_filename(done, &f);
                        ଯଦି (r < 0 && r != -EADDRNOTAVAIL)
                                ଫେରନ୍ତୁ r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                ଯଦି (!done) {
                        ପ୍ରତିଜ୍ଞା(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        ଯଦି (!done)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                ଯଦି (append_trail_slash)
                        ଯଦି (!strextend(&done, "/"))
                                ଫେରନ୍ତୁ -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        ଯଦି (ret_fd) {
                ଯଦି (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        ପ୍ରତିଜ୍ଞା(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } ଅନ୍ୟଥା
                        *ret_fd = -EBADF;
        }

        ଯଦି (FLAGS_SET(flags, CHASE_STEP))
                ଫେରନ୍ତୁ 1;

        ଫେରନ୍ତୁ exists;

chased_one:
        ଯଦି (ret_path) {
                ଅଚଳ ଅକ୍ଷର *e;

                ଯଦି (!done) {
                        ପ୍ରତିଜ୍ଞା(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        ଯଦି (!done)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        *ret_path = TAKE_PTR(done);
                ଅନ୍ୟଥା {
                        ଅକ୍ଷର *c;

                        c = path_join(done, e);
                        ଯଦି (!c)
                                ଫେରନ୍ତୁ -ENOMEM;

                        *ret_path = c;
                }
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags flags, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି, *absolute = ଖାଲି, *p = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF, pfd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ -EINVAL;

        r = empty_or_ମୂଳ_harder_to_null(&ମୂଳ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* A root directory of "/" or "" is identical to "/". */
        ଯଦି (empty_or_ମୂଳ(ମୂଳ))
                ମୂଳ = "/";
        ଅନ୍ୟଥା {
                r = path_make_absolute_cwd(ମୂଳ, &ମୂଳ_abs);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                ମୂଳ = path_simplify(ମୂଳ_abs);

                ପ୍ରତିଜ୍ଞା(path_is_absolute(ମୂଳ));
                ପ୍ରତିଜ୍ଞା(!empty_or_ମୂଳ(ମୂଳ));

                ଯଦି (FLAGS_SET(flags, CHASE_PREFIX_ମୂଳ)) {
                        absolute = path_join(ମୂଳ, path);
                        ଯଦି (!absolute)
                                ଫେରନ୍ତୁ -ENOMEM;
                }
        }

        ଯଦି (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        path = path_startswith(absolute, ମୂଳ);
        ଯଦି (!path)
                ଫେରନ୍ତୁ log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, ମୂଳ);

        ଯଦି (empty_or_ମୂଳ(ମୂଳ))
                fd = XAT_FDROOT;
        ଅନ୍ୟଥା {
                fd = open(ମୂଳ, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_ମୂଳ, ret_path ? &p : ଖାଲି, ret_fd ? &pfd : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path) {
                ଯଦି (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        ଯଦି (empty_or_ମୂଳ(ମୂଳ))
                                ପ୍ରତିଜ୍ଞା(path_is_absolute(p));
                        ଅନ୍ୟଥା {
                                ଅକ୍ଷର *q;

                                ପ୍ରତିଜ୍ଞା(!path_is_absolute(p));

                                q = path_join(ମୂଳ, p + STR_IN_SET(p, ".", "./"));
                                ଯଦି (!q)
                                        ଫେରନ୍ତୁ -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        ଯଦି (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chaseat_prefix_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *q;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        ଯଦି (!path_is_absolute(path)) {
                _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;

                r = empty_or_ମୂଳ_harder_to_null(&ମୂଳ);
                ଯଦି (r < 0 && r != -ENOENT)
                        ଫେରନ୍ତୁ r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                ଯଦି (empty_or_ମୂଳ(ମୂଳ))
                        ଫେରନ୍ତୁ -EINVAL;

                r = path_make_absolute_cwd(ମୂଳ, &ମୂଳ_abs);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ମୂଳ = path_simplify(ମୂଳ_abs);

                q = path_join(ମୂଳ, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } ଅନ୍ୟଥା
                q = strdup(path);
        ଯଦି (!q)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = q;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_extract_filename(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!path_is_absolute(path))
                ଫେରନ୍ତୁ -EINVAL;

        r = empty_or_ମୂଳ_harder_to_null(&ମୂଳ);
        ଯଦି (r < 0 && r != -ENOENT)
                ଫେରନ୍ତୁ r;

        ଯଦି (!empty_or_ମୂଳ(ମୂଳ)) {
                _cleanup_free_ ଅକ୍ଷର *ମୂଳ_abs = ଖାଲି;

                r = path_make_absolute_cwd(ମୂଳ, &ମୂଳ_abs);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                path = path_startswith(path, ମୂଳ_abs);
                ଯଦି (!path)
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (!isempty(path)) {
                r = path_extract_filename(path, ret);
                ଯଦି (r != -EADDRNOTAVAIL)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ strdup_to(ret, ".");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_open(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ChaseFlags chase_flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags,
                ଅକ୍ଷର **ret_path) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *fname = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *open_name = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        ଯଦି (empty_or_ମୂଳ(ମୂଳ) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ଫେରନ୍ତୁ chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, ମୂଳ, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        ଯଦି (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                ଯଦି (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ଅନ୍ୟଥା {
                        r = chase_extract_filename(p, ମୂଳ, &fname);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_opendir(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, DIR **ret_dir) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        DIR *d;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ପ୍ରତିଜ୍ଞା(ret_dir);

        ଯଦି (empty_or_ମୂଳ(ମୂଳ) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                ଯଦି (!d)
                        ଫେରନ୍ତୁ -errno;

                *ret_dir = d;
                ଫେରନ୍ତୁ 0;
        }

        r = chase(path, ମୂଳ, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : ଖାଲି, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ଖାଲି, /* flags= */ 0);
        ଯଦି (!d)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_stat(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, ସଂରଚନା stat *ret_stat) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        ପ୍ରତିଜ୍ଞା(ret_stat);

        ଯଦି (empty_or_ମୂଳ(ମୂଳ) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ଫେରନ୍ତୁ RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, ମୂଳ, chase_flags, ret_path ? &p : ଖାଲି, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        ଯଦି (fstat(path_fd, ret_stat) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_access(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_mode, ଅକ୍ଷର **ret_path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        ଯଦି (empty_or_ମୂଳ(ମୂଳ) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ଫେରନ୍ତୁ RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, ମୂଳ, chase_flags, ret_path ? &p : ଖାଲି, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_fopen_unlocked(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ChaseFlags chase_flags,
                ଅଚଳ ଅକ୍ଷର *open_flags,
                ଅକ୍ଷର **ret_path,
                FILE **ret_file) {

        _cleanup_free_ ଅକ୍ଷର *final_path = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_flags, r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        ପ୍ରତିଜ୍ଞା(open_flags);
        ପ୍ରତିଜ୍ଞା(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        ଯଦି (mode_flags < 0)
                ଫେରନ୍ତୁ mode_flags;

        fd = chase_and_open(path, ମୂଳ, chase_flags, mode_flags, ret_path ? &final_path : ଖାଲି);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_unlink(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlink_flags, ଅକ୍ଷର **ret_path) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *fname = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, ମୂଳ, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        r = path_extract_filename(p, &fname);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (unlinkat(fd, fname, unlink_flags) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_open_parent(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅକ୍ଷର **ret_filename) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pfd, r;

        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, ମୂଳ, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pfd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_openat(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ChaseFlags chase_flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags,
                ଅକ୍ଷର **ret_path) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *fname = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *open_name = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        ଯଦି (ମୂଳ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ଫେରନ୍ତୁ chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(ମୂଳ_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                ଯଦି (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                ଅନ୍ୟଥା {
                        r = path_extract_filename(p, &fname);
                        ଯଦି (r < 0 && r != -EADDRNOTAVAIL)
                                ଫେରନ୍ତୁ r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_opendirat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, DIR **ret_dir) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        DIR *d;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        ପ୍ରତିଜ୍ଞା(ret_dir);

        ଯଦି (ମୂଳ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                ଯଦି (!d)
                        ଫେରନ୍ତୁ -errno;

                *ret_dir = d;
                ଫେରନ୍ତୁ 0;
        }

        r = chaseat(ମୂଳ_fd, dir_fd, path, chase_flags, ret_path ? &p : ଖାଲି, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ ଖାଲି, /* flags= */ 0);
        ଯଦି (!d)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_statat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, ସଂରଚନା stat *ret_stat) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        ପ୍ରତିଜ୍ଞା(ret_stat);

        ଯଦି (ମୂଳ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ଫେରନ୍ତୁ RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ମୂଳ_fd, dir_fd, path, chase_flags, ret_path ? &p : ଖାଲି, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        ଯଦି (fstat(path_fd, ret_stat) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_accessat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_mode, ଅକ୍ଷର **ret_path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        ଯଦି (ମୂଳ_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ଫେରନ୍ତୁ RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(ମୂଳ_fd, dir_fd, path, chase_flags, ret_path ? &p : ଖାଲି, &path_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_fopenat_unlocked(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ChaseFlags chase_flags,
                ଅଚଳ ଅକ୍ଷର *open_flags,
                ଅକ୍ଷର **ret_path,
                FILE **ret_file) {

        _cleanup_free_ ଅକ୍ଷର *final_path = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_flags, r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        ପ୍ରତିଜ୍ଞା(open_flags);
        ପ୍ରତିଜ୍ଞା(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        ଯଦି (mode_flags < 0)
                ଫେରନ୍ତୁ mode_flags;

        fd = chase_and_openat(ମୂଳ_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : ଖାଲି);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_unlinkat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlink_flags, ଅକ୍ଷର **ret_path) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *fname = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(ମୂଳ_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        r = path_extract_filename(p, &fname);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (unlinkat(fd, fname, unlink_flags) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_open_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅକ୍ଷର **ret_filename) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pfd, r;

        ପ୍ରତିଜ୍ଞା(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(ମୂଳ_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pfd;
}
