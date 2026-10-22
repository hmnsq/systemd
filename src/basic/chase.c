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

static целое chase_statx(целое fd, struct statx *ret) {
        вернуть xstatx_full(fd,
                        /* path= */ NULL,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

static целое chase_openat2(целое root_fd, целое dir_fd, const char *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        static bool can_openat2 = true;
        целое r;

        assert(path);
        assert(wildcard_fd_is_valid(dir_fd));

        если (!can_openat2)
                вернуть -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        если ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                вернуть -EOPNOTSUPP;
        если (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                вернуть -EOPNOTSUPP;
        если (root_fd != XAT_FDROOT && root_fd != dir_fd)
                вернуть -EOPNOTSUPP;

        _cleanup_close_ целое dir_fd_local = -EBADF;
        если (dir_fd == XAT_FDROOT) {
                если (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                иначе {
                        dir_fd_local = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        если (dir_fd_local < 0)
                                вернуть -errno;
                        dir_fd = dir_fd_local;
                }
        }

        struct open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        если (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        если (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        если (root_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_ROOT;
        если (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ целое fd = openat2(dir_fd, path, &how, sizeof(how));
        если (fd < 0) {
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
                если (errno == ENOSYS)
                        can_openat2 = false;
                если (IN_SET(errno, ENOSYS, EPERM, EAGAIN))
                        вернуть -EOPNOTSUPP;
                вернуть -errno;
        }

        если (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                если (r < 0)
                        вернуть r;
        }
        если (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                если (r < 0)
                        вернуть r;
        }

        вернуть TAKE_FD(fd);
}

static целое chase_xopenat(целое dir_fd, const char *path, ChaseFlags chase_flags, целое open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        если (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        если (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        если (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        если (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        если (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        вернуть xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

static bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        если (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                вернуть false;

        вернуть a != b; /* Otherwise we need to stay within the same UID */
}

целое statx_unsafe_transition(const struct statx *a, const struct statx *b) {
        assert(a);
        assert(b);

        если (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                вернуть -ENODATA;

        вернуть uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(const struct stat *a, const struct stat *b) {
        assert(a);
        assert(b);

        вернуть uid_unsafe_transition(a->st_uid, b->st_uid);
}

static целое log_unsafe_transition(целое a, целое b, const char *path, ChaseFlags flags) {
        _cleanup_free_ char *n1 = NULL, *n2 = NULL, *user_a = NULL, *user_b = NULL;
        struct stat st;

        если (!FLAGS_SET(flags, CHASE_WARN))
                вернуть -ENOLINK;

        (void) fd_get_path(a, &n1);
        (void) fd_get_path(b, &n2);

        если (fstat(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        если (fstat(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        вернуть log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_RIGHT), strna(n2), strna(user_b), path);
}

static целое log_autofs_mount_point(целое fd, const char *path, ChaseFlags flags) {
        _cleanup_free_ char *n1 = NULL;

        если (!FLAGS_SET(flags, CHASE_WARN))
                вернуть -EREMOTE;

        (void) fd_get_path(fd, &n1);

        вернуть log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

static целое log_prohibited_symlink(целое fd, ChaseFlags flags) {
        _cleanup_free_ char *n1 = NULL;

        assert(fd >= 0);

        если (!FLAGS_SET(flags, CHASE_WARN))
                вернуть -ELOOP;

        (void) fd_get_path(fd, &n1);

        вернуть log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

целое chaseat(целое root_fd, целое dir_fd, const char *path, ChaseFlags flags, char **ret_path, целое *ret_fd) {
        целое r;

        assert(!FLAGS_SET(flags, CHASE_PREFIX_ROOT));
        assert(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        assert(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        assert(wildcard_fd_is_valid(dir_fd));
        assert(wildcard_fd_is_valid(root_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        assert(dir_fd != AT_FDCWD || IN_SET(root_fd, AT_FDCWD, XAT_FDROOT));

        если (FLAGS_SET(flags, CHASE_STEP))
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
        если (root_fd == AT_FDCWD)
                root_fd = XAT_FDROOT;
        иначе {
                r = dir_fd_is_root(root_fd);
                если (r < 0)
                        вернуть r;
                если (r > 0)
                        root_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_root(dir_fd);
        если (r < 0)
                вернуть r;
        если (r > 0 && root_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        если (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = root_fd;

        если (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        если (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                если (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } иначе если (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        если (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        если (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                вернуть -EBADSLT;

        если (!ret_path) {
                r = chase_openat2(root_fd, dir_fd, path, flags);
                если (r >= 0) {
                        если (ret_fd)
                                *ret_fd = r;
                        иначе
                                safe_close(r);

                        вернуть 1;
                }
                если (r != -EOPNOTSUPP)
                        вернуть r;
        }

        если (root_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                если (r < 0)
                        вернуть r;

                если (ret_fd)
                        *ret_fd = r;
                иначе
                        safe_close(r);

                вернуть 1;
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

        _cleanup_free_ char *done = NULL;
        если (need_absolute) {
                done = strdup("/");
                если (!done)
                        вернуть -ENOMEM;
        }

        _cleanup_close_ целое fd = xopenat(dir_fd, NULL, O_CLOEXEC|O_DIRECTORY|O_PATH);
        если (fd < 0)
                вернуть fd;

        struct statx stx;
        r = chase_statx(fd, &stx);
        если (r < 0)
                вернуть r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        struct statx root_stx;
        если (root_fd != XAT_FDROOT) {
                если (root_fd == dir_fd)
                        root_stx = stx;
                иначе {
                        r = chase_statx(root_fd, &root_stx);
                        если (r < 0)
                                вернуть r;
                }
        }

        _cleanup_free_ char *buffer = strdup(path);
        если (!buffer)
                вернуть -ENOMEM;

        const char *todo = buffer;
        bool exists = true;
        для (unsigned n_steps = 0;; n_steps++) {
                _cleanup_free_ char *first = NULL;
                _cleanup_close_ целое child = -EBADF;
                struct statx stx_child;
                const char *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                если (n_steps > CHASE_MAX)
                        вернуть -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                если (r < 0)
                        вернуть r;
                если (r == 0) /* We reached the end. */
                        break;

                first = strndup(e, r);
                если (!first)
                        вернуть -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                если (streq(first, "..")) {
                        _cleanup_free_ char *parent = NULL;
                        _cleanup_close_ целое fd_parent = -EBADF;
                        struct statx stx_parent;

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
                        если (root_fd != XAT_FDROOT) {
                                bool is_root = root_fd == dir_fd && empty_or_root(done);
                                если (!is_root && statx_inode_same(&stx, &root_stx)) {
                                        r = statx_mount_same(&stx, &root_stx);
                                        если (r < 0)
                                                вернуть r;

                                        is_root = r > 0;
                                }
                                если (is_root) {
                                        если (FLAGS_SET(flags, CHASE_STEP))
                                                goto chased_one;
                                        continue;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        если (fd_parent < 0)
                                вернуть -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        если (r < 0)
                                вернуть r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        если (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_root(fd);
                                если (r < 0)
                                        вернуть r;
                                если (r > 0) {
                                        если (FLAGS_SET(flags, CHASE_STEP))
                                                goto chased_one;
                                        continue;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        если (r >= 0) {
                                assert(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } иначе если (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                assert(!need_absolute);
                                done = mfree(done);
                        } иначе если (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } иначе если (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                assert(!need_absolute);

                                если (!isempty(done) && !path_is_valid(done))
                                        вернуть -EINVAL;

                                если (!path_extend(&done, ".."))
                                        вернуть -ENOMEM;
                        } иначе
                                вернуть r;

                        если (FLAGS_SET(flags, CHASE_STEP))
                                goto chased_one;

                        если (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                если (r < 0)
                                        вернуть r;
                                если (r > 0)
                                        вернуть log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        если (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ целое fd_grandparent = -EBADF;
                                struct statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                если (fd_grandparent < 0)
                                        вернуть -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                если (r < 0)
                                        вернуть r;

                                если (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        если (r < 0)
                                                вернуть r;
                                        если (r > 0)
                                                вернуть log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                break;
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
                если (r < 0) {
                        если (r != -ENOENT)
                                вернуть r;

                        если (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                вернуть r;

                        если (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                если (child < 0)
                                        вернуть child;
                        } иначе если (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                если (!path_extend(&done, first))
                                        вернуть -ENOMEM;

                                break;
                        } иначе если (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                если (!path_extend(&done, first, todo))
                                        вернуть -ENOMEM;

                                exists = false;
                                break;
                        } иначе
                                вернуть r;
                }

                r = chase_statx(child, &stx_child);
                если (r < 0)
                        вернуть r;

                если (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        если (r < 0)
                                вернуть r;
                        если (r > 0)
                                вернуть log_unsafe_transition(fd, child, path, flags);
                }

                если (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        вернуть log_autofs_mount_point(child, path, flags);

                если (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ char *destination = NULL;

                        если (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                вернуть log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        если (r < 0)
                                вернуть r;
                        если (isempty(destination))
                                вернуть -EINVAL;

                        если (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(root_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                если (fd < 0)
                                        вернуть fd;

                                r = chase_statx(fd, &stx);
                                если (r < 0)
                                        вернуть r;

                                если (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        если (r < 0)
                                                вернуть r;
                                        если (r > 0)
                                                вернуть log_unsafe_transition(child, fd, path, flags);
                                }

                                если (dir_fd != root_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : NULL);
                                если (r < 0)
                                        вернуть r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        если (!path_extend(&destination, todo))
                                вернуть -ENOMEM;

                        free_and_replace(buffer, destination);
                        todo = buffer;

                        если (FLAGS_SET(flags, CHASE_STEP))
                                goto chased_one;

                        continue;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                если (!path_extend(&done, first))
                        вернуть -ENOMEM;

                если (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        break;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        если (exists) {
                если (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        если (r < 0)
                                вернуть r;
                }

                если (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        если (r < 0)
                                вернуть r;
                }

                если (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        если (r < 0)
                                вернуть r;
                }
        }

        если (ret_path) {
                если (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ char *f = NULL;

                        r = path_extract_filename(done, &f);
                        если (r < 0 && r != -EADDRNOTAVAIL)
                                вернуть r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                если (!done) {
                        assert(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        если (!done)
                                вернуть -ENOMEM;
                }

                если (append_trail_slash)
                        если (!strextend(&done, "/"))
                                вернуть -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        если (ret_fd) {
                если (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        assert(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } иначе
                        *ret_fd = -EBADF;
        }

        если (FLAGS_SET(flags, CHASE_STEP))
                вернуть 1;

        вернуть exists;

chased_one:
        если (ret_path) {
                const char *e;

                если (!done) {
                        assert(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        если (!done)
                                вернуть -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        *ret_path = TAKE_PTR(done);
                иначе {
                        char *c;

                        c = path_join(done, e);
                        если (!c)
                                вернуть -ENOMEM;

                        *ret_path = c;
                }
        }

        вернуть 0;
}

целое chase(const char *path, const char *root, ChaseFlags flags, char **ret_path, целое *ret_fd) {
        _cleanup_free_ char *root_abs = NULL, *absolute = NULL, *p = NULL;
        _cleanup_close_ целое fd = -EBADF, pfd = -EBADF;
        целое r;

        assert(path);

        если (isempty(path))
                вернуть -EINVAL;

        r = empty_or_root_harder_to_null(&root);
        если (r < 0)
                вернуть r;

        /* A root directory of "/" or "" is identical to "/". */
        если (empty_or_root(root))
                root = "/";
        иначе {
                r = path_make_absolute_cwd(root, &root_abs);
                если (r < 0)
                        вернуть r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                root = path_simplify(root_abs);

                assert(path_is_absolute(root));
                assert(!empty_or_root(root));

                если (FLAGS_SET(flags, CHASE_PREFIX_ROOT)) {
                        absolute = path_join(root, path);
                        если (!absolute)
                                вернуть -ENOMEM;
                }
        }

        если (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                если (r < 0)
                        вернуть r;
        }

        path = path_startswith(absolute, root);
        если (!path)
                вернуть log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, root);

        если (empty_or_root(root))
                fd = XAT_FDROOT;
        иначе {
                fd = open(root, O_CLOEXEC|O_DIRECTORY|O_PATH);
                если (fd < 0)
                        вернуть -errno;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_ROOT, ret_path ? &p : NULL, ret_fd ? &pfd : NULL);
        если (r < 0)
                вернуть r;

        если (ret_path) {
                если (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        если (empty_or_root(root))
                                assert(path_is_absolute(p));
                        иначе {
                                char *q;

                                assert(!path_is_absolute(p));

                                q = path_join(root, p + STR_IN_SET(p, ".", "./"));
                                если (!q)
                                        вернуть -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        если (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        вернуть r;
}

целое chaseat_prefix_root(const char *path, const char *root, char **ret) {
        char *q;
        целое r;

        assert(path);
        assert(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        если (!path_is_absolute(path)) {
                _cleanup_free_ char *root_abs = NULL;

                r = empty_or_root_harder_to_null(&root);
                если (r < 0 && r != -ENOENT)
                        вернуть r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                если (empty_or_root(root))
                        вернуть -EINVAL;

                r = path_make_absolute_cwd(root, &root_abs);
                если (r < 0)
                        вернуть r;

                root = path_simplify(root_abs);

                q = path_join(root, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } иначе
                q = strdup(path);
        если (!q)
                вернуть -ENOMEM;

        *ret = q;
        вернуть 0;
}

целое chase_extract_filename(const char *path, const char *root, char **ret) {
        целое r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        assert(path);
        assert(ret);

        если (isempty(path))
                вернуть -EINVAL;

        если (!path_is_absolute(path))
                вернуть -EINVAL;

        r = empty_or_root_harder_to_null(&root);
        если (r < 0 && r != -ENOENT)
                вернуть r;

        если (!empty_or_root(root)) {
                _cleanup_free_ char *root_abs = NULL;

                r = path_make_absolute_cwd(root, &root_abs);
                если (r < 0)
                        вернуть r;

                path = path_startswith(path, root_abs);
                если (!path)
                        вернуть -EINVAL;
        }

        если (!isempty(path)) {
                r = path_extract_filename(path, ret);
                если (r != -EADDRNOTAVAIL)
                        вернуть r;
        }

        вернуть strdup_to(ret, ".");
}

целое chase_and_open(
                const char *path,
                const char *root,
                ChaseFlags chase_flags,
                целое open_flags,
                char **ret_path) {

        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL, *fname = NULL;
        const char *open_name = NULL;
        целое r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        если (empty_or_root(root) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                вернуть chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, root, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        если (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                если (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                иначе {
                        r = chase_extract_filename(p, root, &fname);
                        если (r < 0)
                                вернуть r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть r;
}

целое chase_and_opendir(const char *path, const char *root, ChaseFlags chase_flags, char **ret_path, DIR **ret_dir) {
        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        DIR *d;
        целое r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        assert(ret_dir);

        если (empty_or_root(root) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                если (!d)
                        вернуть -errno;

                *ret_dir = d;
                вернуть 0;
        }

        r = chase(path, root, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : NULL, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ NULL, /* flags= */ 0);
        если (!d)
                вернуть -errno;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        вернуть 0;
}

целое chase_and_stat(const char *path, const char *root, ChaseFlags chase_flags, char **ret_path, struct stat *ret_stat) {
        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        целое r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        assert(ret_stat);

        если (empty_or_root(root) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                вернуть RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, root, chase_flags, ret_path ? &p : NULL, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        если (fstat(path_fd, ret_stat) < 0)
                вернуть -errno;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть 0;
}

целое chase_and_access(const char *path, const char *root, ChaseFlags chase_flags, целое access_mode, char **ret_path) {
        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        целое r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        если (empty_or_root(root) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                вернуть RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, root, chase_flags, ret_path ? &p : NULL, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть 0;
}

целое chase_and_fopen_unlocked(
                const char *path,
                const char *root,
                ChaseFlags chase_flags,
                const char *open_flags,
                char **ret_path,
                FILE **ret_file) {

        _cleanup_free_ char *final_path = NULL;
        _cleanup_close_ целое fd = -EBADF;
        целое mode_flags, r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        assert(open_flags);
        assert(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        если (mode_flags < 0)
                вернуть mode_flags;

        fd = chase_and_open(path, root, chase_flags, mode_flags, ret_path ? &final_path : NULL);
        если (fd < 0)
                вернуть fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(final_path);

        вернуть 0;
}

целое chase_and_unlink(const char *path, const char *root, ChaseFlags chase_flags, целое unlink_flags, char **ret_path) {
        _cleanup_free_ char *p = NULL, *fname = NULL;
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, root, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        если (fd < 0)
                вернуть fd;

        r = path_extract_filename(p, &fname);
        если (r < 0)
                вернуть r;

        если (unlinkat(fd, fname, unlink_flags) < 0)
                вернуть -errno;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть 0;
}

целое chase_and_open_parent(const char *path, const char *root, ChaseFlags chase_flags, char **ret_filename) {
        целое pfd, r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, root, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        если (r < 0)
                вернуть r;

        вернуть pfd;
}

целое chase_and_openat(
                целое root_fd,
                целое dir_fd,
                const char *path,
                ChaseFlags chase_flags,
                целое open_flags,
                char **ret_path) {

        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL, *fname = NULL;
        const char *open_name = NULL;
        целое r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        если (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                вернуть chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(root_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        если (r < 0)
                вернуть r;

        если (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                если (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                иначе {
                        r = path_extract_filename(p, &fname);
                        если (r < 0 && r != -EADDRNOTAVAIL)
                                вернуть r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть r;
}

целое chase_and_opendirat(целое root_fd, целое dir_fd, const char *path, ChaseFlags chase_flags, char **ret_path, DIR **ret_dir) {
        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        DIR *d;
        целое r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        assert(ret_dir);

        если (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(path);
                если (!d)
                        вернуть -errno;

                *ret_dir = d;
                вернуть 0;
        }

        r = chaseat(root_fd, dir_fd, path, chase_flags, ret_path ? &p : NULL, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ NULL, /* flags= */ 0);
        если (!d)
                вернуть -errno;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        вернуть 0;
}

целое chase_and_statat(целое root_fd, целое dir_fd, const char *path, ChaseFlags chase_flags, char **ret_path, struct stat *ret_stat) {
        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        целое r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        assert(ret_stat);

        если (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                вернуть RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(root_fd, dir_fd, path, chase_flags, ret_path ? &p : NULL, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        если (fstat(path_fd, ret_stat) < 0)
                вернуть -errno;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть 0;
}

целое chase_and_accessat(целое root_fd, целое dir_fd, const char *path, ChaseFlags chase_flags, целое access_mode, char **ret_path) {
        _cleanup_close_ целое path_fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        целое r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        если (root_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                вернуть RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(root_fd, dir_fd, path, chase_flags, ret_path ? &p : NULL, &path_fd);
        если (r < 0)
                вернуть r;
        assert(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть 0;
}

целое chase_and_fopenat_unlocked(
                целое root_fd,
                целое dir_fd,
                const char *path,
                ChaseFlags chase_flags,
                const char *open_flags,
                char **ret_path,
                FILE **ret_file) {

        _cleanup_free_ char *final_path = NULL;
        _cleanup_close_ целое fd = -EBADF;
        целое mode_flags, r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        assert(open_flags);
        assert(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        если (mode_flags < 0)
                вернуть mode_flags;

        fd = chase_and_openat(root_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : NULL);
        если (fd < 0)
                вернуть fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(final_path);

        вернуть 0;
}

целое chase_and_unlinkat(целое root_fd, целое dir_fd, const char *path, ChaseFlags chase_flags, целое unlink_flags, char **ret_path) {
        _cleanup_free_ char *p = NULL, *fname = NULL;
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        assert(path);
        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(root_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        если (fd < 0)
                вернуть fd;

        r = path_extract_filename(p, &fname);
        если (r < 0)
                вернуть r;

        если (unlinkat(fd, fname, unlink_flags) < 0)
                вернуть -errno;

        если (ret_path)
                *ret_path = TAKE_PTR(p);

        вернуть 0;
}

целое chase_and_open_parent_at(целое root_fd, целое dir_fd, const char *path, ChaseFlags chase_flags, char **ret_filename) {
        целое pfd, r;

        assert(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(root_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        если (r < 0)
                вернуть r;

        вернуть pfd;
}
