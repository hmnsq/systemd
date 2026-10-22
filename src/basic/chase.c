/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/magic.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "glyph-util.h"
#include "log.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"
#include "user-util.h"

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

static ཧྲིལ chase_statx(ཧྲིལ fd, sa_nracanaa statx *ret) {
        ལོག xstatx_full(fd,
                        /* path= */ NULL,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

static ཧྲིལ chase_openat2(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        static bool can_openat2 = true;
        ཧྲིལ r;

        assert(path);
        assert(wildcard_fd_is_valid(dir_fd));

        གལ་སྲིད (!can_openat2)
                ལོག -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        གལ་སྲིད ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                ལོག -EOPNOTSUPP;
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                ལོག -EOPNOTSUPP;
        གལ་སྲིད (root_fd != XAT_FDROOT && root_fd != dir_fd)
                ལོག -EOPNOTSUPP;

        _cleanup_close_ ཧྲིལ dir_fd_local = -EBADF;
        གལ་སྲིད (dir_fd == XAT_FDROOT) {
                གལ་སྲིད (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                དེ་མིན {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        གལ་སྲིད (dir_fd_local < 0)
                                ལོག -errno;
                        dir_fd = dir_fd_local;
                }
        }

        sa_nracanaa open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        གལ་སྲིད (root_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_ROOT;
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ ཧྲིལ fd = openat2(dir_fd, path, &how, parimaa_na(how));
        གལ་སྲིད (fd < 0) {
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
                གལ་སྲིད (errno == ENOSYS)
                        can_openat2 = false;
                གལ་སྲིད (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        ལོག -EOPNOTSUPP;
                ལོག -errno;
        }

        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        ལོག TAKE_FD(fd);
}

static ཧྲིལ chase_xopenat(ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, ཧྲིལ open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        ལོག xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

static bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        གལ་སྲིད (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                ལོག false;

        ལོག a != b; /* Otherwise we need to stay within the same UID */
}

ཧྲིལ statx_unsafe_transition(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {
        assert(a);
        assert(b);

        གལ་སྲིད (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                ལོག -ENODATA;

        ལོག uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        assert(a);
        assert(b);

        ལོག uid_unsafe_transition(a->st_uid, b->st_uid);
}

static ཧྲིལ log_unsafe_transition(ཧྲིལ a, ཧྲིལ b, const var_na *path, ChaseFlags flags) {
        _cleanup_free_ var_na *n1 = NULL, *n2 = NULL, *user_a = NULL, *user_b = NULL;
        sa_nracanaa stat st;

        གལ་སྲིད (!FLAGS_SET(flags, CHASE_WARN))
                ལོག -ENOLINK;

        (void) fd_get_path(a, &n1);
        (void) fd_get_path(b, &n2);

        གལ་སྲིད (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        གལ་སྲིད (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        ལོག log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_RIGHT), strna(n2), strna(user_b), path);
}

static ཧྲིལ log_autofs_mount_point(ཧྲིལ fd, const var_na *path, ChaseFlags flags) {
        _cleanup_free_ var_na *n1 = NULL;

        གལ་སྲིད (!FLAGS_SET(flags, CHASE_WARN))
                ལོག -EREMOTE;

        (void) fd_get_path(fd, &n1);

        ལོག log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

static ཧྲིལ log_prohibited_symlink(ཧྲིལ fd, ChaseFlags flags) {
        _cleanup_free_ var_na *n1 = NULL;

        assert(fd >= 0);

        གལ་སྲིད (!FLAGS_SET(flags, CHASE_WARN))
                ལོག -ELOOP;

        (void) fd_get_path(fd, &n1);

        ལོག log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

ཧྲིལ chaseat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags flags, var_na **ret_path, ཧྲིལ *ret_fd) {
        ཧྲིལ r;

        assert(!FLAGS_SET(flags, CHASE_PREFIX_ROOT));
        assert(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        assert(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        assert(wildcard_fd_is_valid(dir_fd));
        assert(wildcard_fd_is_valid(root_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        assert(dir_fd != AT_FDCWD || IN_SET(root_fd, AT_FDCWD, XAT_FDROOT));

        གལ་སྲིད (FLAGS_SET(flags, CHASE_STEP))
                assert(!ret_fd);

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
        གལ་སྲིད (root_fd == AT_FDCWD)
                root_fd = XAT_FDROOT;
        དེ་མིན {
                r = dir_fd_is_root(root_fd);
                གལ་སྲིད (r < 0)
                        ལོག r;
                གལ་སྲིད (r > 0)
                        root_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_root(dir_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (r > 0 && root_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        གལ་སྲིད (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = root_fd;

        གལ་སྲིད (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        གལ་སྲིད (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                གལ་སྲིད (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } དེ་མིན གལ་སྲིད (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        གལ་སྲིད (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        གལ་སྲིད (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                ལོག -EBADSLT;

        གལ་སྲིད (!ret_path) {
                r = chase_openat2(root_fd, dir_fd, path, flags);
                གལ་སྲིད (r >= 0) {
                        གལ་སྲིད (ret_fd)
                                *ret_fd = r;
                        དེ་མིན
                                safe_close(r);

                        ལོག 1;
                }
                གལ་སྲིད (r != -EOPNOTSUPP)
                        ལོག r;
        }

        གལ་སྲིད (root_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (ret_fd)
                        *ret_fd = r;
                དེ་མིན
                        safe_close(r);

                ལོག 1;
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
        bool need_absolute = (root_fd == XAT_FDROOT || dir_fd != root_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ var_na *done = NULL;
        གལ་སྲིད (need_absolute) {
                done = strdup("/");
                གལ་སྲིད (!done)
                        ལོག -ENOMEM;
        }

        _cleanup_close_ ཧྲིལ fd = xopenat(dir_fd, NULL, O_CLOEXEC|O_DIRECTORY|O_PATH);
        གལ་སྲིད (fd < 0)
                ལོག fd;

        sa_nracanaa statx stx;
        r = chase_statx(fd, &stx);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        sa_nracanaa statx root_stx;
        གལ་སྲིད (root_fd != XAT_FDROOT) {
                གལ་སྲིད (root_fd == dir_fd)
                        root_stx = stx;
                དེ་མིན {
                        r = chase_statx(root_fd, &root_stx);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                }
        }

        _cleanup_free_ var_na *buffer = strdup(path);
        གལ་སྲིད (!buffer)
                ལོག -ENOMEM;

        const var_na *todo = buffer;
        bool exists = true;
        དོན་ལུ (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ var_na *first = NULL;
                _cleanup_close_ ཧྲིལ child = -EBADF;
                sa_nracanaa statx stx_child;
                const var_na *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                གལ་སྲིད (n_steps > CHASE_MAX)
                        ལོག -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                གལ་སྲིད (r < 0)
                        ལོག r;
                གལ་སྲིད (r == 0) /* We reached the end. */
                        bha_nga;

                first = strndup(e, r);
                གལ་སྲིད (!first)
                        ལོག -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                གལ་སྲིད (streq(first, "..")) {
                        _cleanup_free_ var_na *parent = NULL;
                        _cleanup_close_ ཧྲིལ fd_parent = -EBADF;
                        sa_nracanaa statx stx_parent;

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
                        གལ་སྲིད (root_fd != XAT_FDROOT) {
                                bool is_root = root_fd == dir_fd && empty_or_root(done);
                                གལ་སྲིད (!is_root && statx_inode_same(&stx, &root_stx)) {
                                        r = statx_mount_same(&stx, &root_stx);
                                        གལ་སྲིད (r < 0)
                                                ལོག r;

                                        is_root = r > 0;
                                }
                                གལ་སྲིད (is_root) {
                                        གལ་སྲིད (FLAGS_SET(flags, CHASE_STEP))
                                                goto chased_one;
                                        continue;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        གལ་སྲིད (fd_parent < 0)
                                ལོག -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        གལ་སྲིད (r < 0)
                                ལོག r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        གལ་སྲིད (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_root(fd);
                                གལ་སྲིད (r < 0)
                                        ལོག r;
                                གལ་སྲིད (r > 0) {
                                        གལ་སྲིད (FLAGS_SET(flags, CHASE_STEP))
                                                goto chased_one;
                                        continue;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        གལ་སྲིད (r >= 0) {
                                assert(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } དེ་མིན གལ་སྲིད (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                assert(!need_absolute);
                                done = mfree(done);
                        } དེ་མིན གལ་སྲིད (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } དེ་མིན གལ་སྲིད (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                assert(!need_absolute);

                                གལ་སྲིད (!isempty(done) && !path_is_valid(done))
                                        ལོག -EINVAL;

                                གལ་སྲིད (!path_extend(&done, ".."))
                                        ལོག -ENOMEM;
                        } དེ་མིན
                                ལོག r;

                        གལ་སྲིད (FLAGS_SET(flags, CHASE_STEP))
                                goto chased_one;

                        གལ་སྲིད (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                གལ་སྲིད (r < 0)
                                        ལོག r;
                                གལ་སྲིད (r > 0)
                                        ལོག log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        གལ་སྲིད (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ ཧྲིལ fd_grandparent = -EBADF;
                                sa_nracanaa statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                གལ་སྲིད (fd_grandparent < 0)
                                        ལོག -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                གལ་སྲིད (r < 0)
                                        ལོག r;

                                གལ་སྲིད (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        གལ་སྲིད (r < 0)
                                                ལོག r;
                                        གལ་སྲིད (r > 0)
                                                ལོག log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                bha_nga;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        continue;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                གལ་སྲིད (r < 0) {
                        གལ་སྲིད (r != -ENOENT)
                                ལོག r;

                        གལ་སྲིད (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                ལོག r;

                        གལ་སྲིད (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                གལ་སྲིད (child < 0)
                                        ལོག child;
                        } དེ་མིན གལ་སྲིད (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                གལ་སྲིད (!path_extend(&done, first))
                                        ལོག -ENOMEM;

                                bha_nga;
                        } དེ་མིན གལ་སྲིད (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                གལ་སྲིད (!path_extend(&done, first, todo))
                                        ལོག -ENOMEM;

                                exists = false;
                                bha_nga;
                        } དེ་མིན
                                ལོག r;
                }

                r = chase_statx(child, &stx_child);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                        གལ་སྲིད (r > 0)
                                ལོག log_unsafe_transition(fd, child, path, flags);
                }

                གལ་སྲིད (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        ལོག log_autofs_mount_point(child, path, flags);

                གལ་སྲིད (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ var_na *destination = NULL;

                        གལ་སྲིད (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                ལོག log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                        གལ་སྲིད (isempty(destination))
                                ལོག -EINVAL;

                        གལ་སྲིད (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(root_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                གལ་སྲིད (fd < 0)
                                        ལོག fd;

                                r = chase_statx(fd, &stx);
                                གལ་སྲིད (r < 0)
                                        ལོག r;

                                གལ་སྲིད (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        གལ་སྲིད (r < 0)
                                                ལོག r;
                                        གལ་སྲིད (r > 0)
                                                ལོག log_unsafe_transition(child, fd, path, flags);
                                }

                                གལ་སྲིད (dir_fd != root_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : NULL);
                                གལ་སྲིད (r < 0)
                                        ལོག r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        གལ་སྲིད (!path_extend(&destination, todo))
                                ལོག -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        གལ་སྲིད (FLAGS_SET(flags, CHASE_STEP))
                                goto chased_one;

                        continue;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                གལ་སྲིད (!path_extend(&done, first))
                        ལོག -ENOMEM;

                གལ་སྲིད (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        bha_nga;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        གལ་སྲིད (exists) {
                གལ་སྲིད (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                }

                གལ་སྲིད (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                }

                གལ་སྲིད (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                }
        }

        གལ་སྲིད (ret_path) {
                གལ་སྲིད (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ var_na *f = NULL;

                        r = path_extract_filename(done, &f);
                        གལ་སྲིད (r < 0 && r != -EADDRNOTAVAIL)
                                ལོག r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                གལ་སྲིད (!done) {
                        assert(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        གལ་སྲིད (!done)
                                ལོག -ENOMEM;
                }

                གལ་སྲིད (append_trail_slash)
                        གལ་སྲིད (!strextend(&done, "/"))
                                ལོག -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        གལ་སྲིད (ret_fd) {
                གལ་སྲིད (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        assert(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } དེ་མིན
                        *ret_fd = -EBADF;
        }

        གལ་སྲིད (FLAGS_SET(flags, CHASE_STEP))
                ལོག 1;

        ལོག exists;

chased_one:
        གལ་སྲིད (ret_path) {
                const var_na *e;

                གལ་སྲིད (!done) {
                        assert(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        གལ་སྲིད (!done)
                                ལོག -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                གལ་སྲིད (r < 0)
                        ལོག r;
                གལ་སྲིད (r == 0)
                        *ret_path = TAKE_PTR(done);
                དེ་མིན {
                        var_na *c;

                        c = path_join(done, e);
                        གལ་སྲིད (!c)
                                ལོག -ENOMEM;

                        *ret_path = c;
                }
        }

        ལོག 0;
}

ཧྲིལ chase(const var_na *path, const var_na *root, ChaseFlags flags, var_na **ret_path, ཧྲིལ *ret_fd) {
        _cleanup_free_ var_na *root_abs = NULL, *absolute = NULL, *p = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF, pfd = -EBADF;
        ཧྲིལ r;

        assert(path);

        གལ་སྲིད (isempty(path))
                ལོག -EINVAL;

        r = empty_or_root_harder_to_null(&root);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* A root directory of "/" or "" is identical to "/". */
        གལ་སྲིད (empty_or_root(root))
                root = "/";
        དེ་མིན {
                r = path_make_absolute_cwd(root, &root_abs);
                གལ་སྲིད (r < 0)
                        ལོག r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                root = path_simplify(root_abs);

                assert(path_is_absolute(root));
                assert(!empty_or_root(root));

                གལ་སྲིད (FLAGS_SET(flags, CHASE_PREFIX_ROOT)) {
                        absolute = path_join(root, path);
                        གལ་སྲིད (!absolute)
                                ལོག -ENOMEM;
                }
        }

        གལ་སྲིད (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        path = path_startswith(absolute, root);
        གལ་སྲིད (!path)
                ལོག log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, root);

        གལ་སྲིད (empty_or_root(root))
                fd = XAT_FDROOT;
        དེ་མིན {
                fd = open(root, O_CLOEXEC|O_DIRECTORY|O_PATH);
                གལ་སྲིད (fd < 0)
                        ལོག -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_ROOT, ret_path ? &p : NULL, ret_fd ? &pfd : NULL);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path) {
                གལ་སྲིད (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        གལ་སྲིད (empty_or_root(root))
                                assert(path_is_absolute(p));
                        དེ་མིན {
                                var_na *q;

                                assert(!path_is_absolute(p));

                                q = path_join(root, p + STR_IN_SET(p, ".", "./"));
                                གལ་སྲིད (!q)
                                        ལོག -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        གལ་སྲིད (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        ལོག r;
}

ཧྲིལ chaseat_prefix_root(const var_na *path, const var_na *root, var_na **ret) {
        var_na *q;
        ཧྲིལ r;

        assert(path);
        assert(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        གལ་སྲིད (!path_is_absolute(path)) {
                _cleanup_free_ var_na *root_abs = NULL;

                r = empty_or_root_harder_to_null(&root);
                གལ་སྲིད (r < 0 && r != -ENOENT)
                        ལོག r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                གལ་སྲིད (empty_or_root(root))
                        ལོག -EINVAL;

                r = path_make_absolute_cwd(root, &root_abs);
                གལ་སྲིད (r < 0)
                        ལོག r;

                root = path_simplify(root_abs);

                q = path_join(root, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } དེ་མིན
                q = strdup(path);
        གལ་སྲིད (!q)
                ལོག -ENOMEM;

        *ret = q;
        ལོག 0;
}

ཧྲིལ chase_extract_filename(const var_na *path, const var_na *root, var_na **ret) {
        ཧྲིལ r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        assert(path);
        assert(ret);

        གལ་སྲིད (isempty(path))
                ལོག -EINVAL;

        གལ་སྲིད (!path_is_absolute(path))
                ལོག -EINVAL;

        r = empty_or_root_harder_to_null(&root);
        གལ་སྲིད (r < 0 && r != -ENOENT)
                ལོག r;

        གལ་སྲིད (!empty_or_root(root)) {
                _cleanup_free_ var_na *root_abs = NULL;

                r = path_make_absolute_cwd(root, &root_abs);
                གལ་སྲིད (r < 0)
                        ལོག r;

                path = path_startswith(path, root_abs);
                གལ་སྲིད (!path)
                        ལོག -EINVAL;
        }

        གལ་སྲིད (!isempty(path)) {
                r = path_extract_filename(path, ret);
                གལ་སྲིད (r != -EADDRNOTAVAIL)
                        ལོག r;
        }

        ལོག strdup_to(ret, ".");
}

ཧྲིལ chase_and_open(
                const var_na *path,
                const var_na *root,
                ChaseFlags chase_flags,
                ཧྲིལ open_flags,
                var_na **ret_path) {

        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL, *fname = NULL;
        const var_na *open_name = NULL;
        ཧྲིལ r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        གལ་སྲིད (empty_or_root(root) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ལོག chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, root, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        གལ་སྲིད (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                དེ་མིན {
                        r = chase_extract_filename(p, root, &fname);
                        གལ་སྲིད (r < 0)
                                ལོག r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག r;
}

ཧྲིལ chase_and_opendir(const var_na *path, const var_na *root, ChaseFlags chase_flags, var_na **ret_path, DIR **ret_dir) {
        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        DIR *d;
        ཧྲིལ r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        assert(ret_dir);

        གལ་སྲིད (empty_or_root(root) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                གལ་སྲིད (!d)
                        ལོག -errno;

                *ret_dir = d;
                ལོག 0;
        }

        r = chase(path, root, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : NULL, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ NULL, /* flags= */ 0);
        གལ་སྲིད (!d)
                ལོག -errno;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ལོག 0;
}

ཧྲིལ chase_and_stat(const var_na *path, const var_na *root, ChaseFlags chase_flags, var_na **ret_path, sa_nracanaa stat *ret_stat) {
        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        assert(ret_stat);

        གལ་སྲིད (empty_or_root(root) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ལོག RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, root, chase_flags, ret_path ? &p : NULL, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        གལ་སྲིད (fstat(path_fd, ret_stat) < 0)
                ལོག -errno;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག 0;
}

ཧྲིལ chase_and_access(const var_na *path, const var_na *root, ChaseFlags chase_flags, ཧྲིལ access_mode, var_na **ret_path) {
        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        གལ་སྲིད (empty_or_root(root) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ལོག RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, root, chase_flags, ret_path ? &p : NULL, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག 0;
}

ཧྲིལ chase_and_fopen_unlocked(
                const var_na *path,
                const var_na *root,
                ChaseFlags chase_flags,
                const var_na *open_flags,
                var_na **ret_path,
                FILE **ret_file) {

        _cleanup_free_ var_na *final_path = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ mode_flags, r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        assert(open_flags);
        assert(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        གལ་སྲིད (mode_flags < 0)
                ལོག mode_flags;

        fd = chase_and_open(path, root, chase_flags, mode_flags, ret_path ? &final_path : NULL);
        གལ་སྲིད (fd < 0)
                ལོག fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ལོག 0;
}

ཧྲིལ chase_and_unlink(const var_na *path, const var_na *root, ChaseFlags chase_flags, ཧྲིལ unlink_flags, var_na **ret_path) {
        _cleanup_free_ var_na *p = NULL, *fname = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, root, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        གལ་སྲིད (fd < 0)
                ལོག fd;

        r = path_extract_filename(p, &fname);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (unlinkat(fd, fname, unlink_flags) < 0)
                ལོག -errno;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག 0;
}

ཧྲིལ chase_and_open_parent(const var_na *path, const var_na *root, ChaseFlags chase_flags, var_na **ret_filename) {
        ཧྲིལ pfd, r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, root, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག pfd;
}

ཧྲིལ chase_and_openat(
                ཧྲིལ root_fd,
                ཧྲིལ dir_fd,
                const var_na *path,
                ChaseFlags chase_flags,
                ཧྲིལ open_flags,
                var_na **ret_path) {

        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL, *fname = NULL;
        const var_na *open_name = NULL;
        ཧྲིལ r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        གལ་སྲིད (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                ལོག chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(root_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                གལ་སྲིད (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                དེ་མིན {
                        r = path_extract_filename(p, &fname);
                        གལ་སྲིད (r < 0 && r != -EADDRNOTAVAIL)
                                ལོག r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག r;
}

ཧྲིལ chase_and_opendirat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, var_na **ret_path, DIR **ret_dir) {
        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        DIR *d;
        ཧྲིལ r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        assert(ret_dir);

        གལ་སྲིད (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                གལ་སྲིད (!d)
                        ལོག -errno;

                *ret_dir = d;
                ལོག 0;
        }

        r = chaseat(root_fd, dir_fd, path, chase_flags, ret_path ? &p : NULL, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ NULL, /* flags= */ 0);
        གལ་སྲིད (!d)
                ལོག -errno;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        ལོག 0;
}

ཧྲིལ chase_and_statat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, var_na **ret_path, sa_nracanaa stat *ret_stat) {
        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        assert(ret_stat);

        གལ་སྲིད (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                ལོག RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(root_fd, dir_fd, path, chase_flags, ret_path ? &p : NULL, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        གལ་སྲིད (fstat(path_fd, ret_stat) < 0)
                ལོག -errno;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག 0;
}

ཧྲིལ chase_and_accessat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, ཧྲིལ access_mode, var_na **ret_path) {
        _cleanup_close_ ཧྲིལ path_fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        གལ་སྲིད (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                ལོག RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(root_fd, dir_fd, path, chase_flags, ret_path ? &p : NULL, &path_fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        assert(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག 0;
}

ཧྲིལ chase_and_fopenat_unlocked(
                ཧྲིལ root_fd,
                ཧྲིལ dir_fd,
                const var_na *path,
                ChaseFlags chase_flags,
                const var_na *open_flags,
                var_na **ret_path,
                FILE **ret_file) {

        _cleanup_free_ var_na *final_path = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ mode_flags, r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        assert(open_flags);
        assert(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        གལ་སྲིད (mode_flags < 0)
                ལོག mode_flags;

        fd = chase_and_openat(root_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : NULL);
        གལ་སྲིད (fd < 0)
                ལོག fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(final_path);

        ལོག 0;
}

ཧྲིལ chase_and_unlinkat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, ཧྲིལ unlink_flags, var_na **ret_path) {
        _cleanup_free_ var_na *p = NULL, *fname = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(root_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        གལ་སྲིད (fd < 0)
                ལོག fd;

        r = path_extract_filename(p, &fname);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (unlinkat(fd, fname, unlink_flags) < 0)
                ལོག -errno;

        གལ་སྲིད (ret_path)
                *ret_path = TAKE_PTR(p);

        ལོག 0;
}

ཧྲིལ chase_and_open_parent_at(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, var_na **ret_filename) {
        ཧྲིལ pfd, r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(root_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག pfd;
}
