/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/magic.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "glyph-util.h"
#समावेश "log.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "user-util.h"

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
#घोषणा CHASE_NO_SHORTCUT_MASK                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_SAFE |                                   \
         CHASE_STEP |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_PARENT |                                 \
         CHASE_MKDIR_0755)

#घोषणा CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

अटल पूर्णांक chase_statx(पूर्णांक fd, काष्ठा statx *ret) {
        वापस xstatx_full(fd,
                        /* path= */ शून्य,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

अटल पूर्णांक chase_openat2(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        अटल bool can_openat2 = true;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(wildcard_fd_is_valid(dir_fd));

        अगर (!can_openat2)
                वापस -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        अगर ((chase_flags & (CHASE_NO_SHORTCUT_MASK & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                वापस -EOPNOTSUPP;
        अगर (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS))
                वापस -EOPNOTSUPP;
        अगर (मूल_fd != XAT_FDROOT && मूल_fd != dir_fd)
                वापस -EOPNOTSUPP;

        _cleanup_close_ पूर्णांक dir_fd_local = -EBADF;
        अगर (dir_fd == XAT_FDROOT) {
                अगर (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                अन्यथा {
                        dir_fd_local = खोलो("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        अगर (dir_fd_local < 0)
                                वापस -त्रुटि_सं;
                        dir_fd = dir_fd_local;
                }
        }

        काष्ठा open_how how = {
                .flags = O_PATH|O_CLOEXEC,
        };
        अगर (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        अगर (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        अगर (मूल_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_IN_मूल;
        अगर (FLAGS_SET(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ पूर्णांक fd = openat2(dir_fd, path, &how, माप(how));
        अगर (fd < 0) {
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
                अगर (त्रुटि_सं == ENOSYS)
                        can_openat2 = false;
                अगर (IN_SET(त्रुटि_सं, ENOSYS, EPERM, EAGAIN))
                        वापस -EOPNOTSUPP;
                वापस -त्रुटि_सं;
        }

        अगर (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                अगर (r < 0)
                        वापस r;
        }
        अगर (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                अगर (r < 0)
                        वापस r;
        }

        वापस TAKE_FD(fd);
}

अटल पूर्णांक chase_xopenat(पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags, पूर्णांक open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        अगर (FLAGS_SET(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        अगर (FLAGS_SET(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        अगर (FLAGS_SET(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        अगर (FLAGS_SET(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        अगर (FLAGS_SET(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_SET(open_flags, O_PATH))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        वापस xopenat_full(dir_fd, path, open_flags, xopen_flags, MODE_INVALID);
}

अटल bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        अगर (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                वापस false;

        वापस a != b; /* Otherwise we need to stay within the same UID */
}

पूर्णांक statx_unsafe_transition(स्थिर काष्ठा statx *a, स्थिर काष्ठा statx *b) {
        निश्चित(a);
        निश्चित(b);

        अगर (!FLAGS_SET(a->stx_mask, STATX_UID) || !FLAGS_SET(b->stx_mask, STATX_UID))
                वापस -ENODATA;

        वापस uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(स्थिर काष्ठा stat *a, स्थिर काष्ठा stat *b) {
        निश्चित(a);
        निश्चित(b);

        वापस uid_unsafe_transition(a->st_uid, b->st_uid);
}

अटल पूर्णांक log_unsafe_transition(पूर्णांक a, पूर्णांक b, स्थिर अक्षर *path, ChaseFlags flags) {
        _cleanup_free_ अक्षर *n1 = शून्य, *n2 = शून्य, *user_a = शून्य, *user_b = शून्य;
        काष्ठा stat st;

        अगर (!FLAGS_SET(flags, CHASE_WARN))
                वापस -ENOLINK;

        (व्योम) fd_get_path(a, &n1);
        (व्योम) fd_get_path(b, &n2);

        अगर (ख_स्थिति(a, &st) == 0)
                user_a = uid_to_name(st.st_uid);
        अगर (ख_स्थिति(b, &st) == 0)
                user_b = uid_to_name(st.st_uid);

        वापस log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_दायाँ), strna(n2), strna(user_b), path);
}

अटल पूर्णांक log_autofs_mount_point(पूर्णांक fd, स्थिर अक्षर *path, ChaseFlags flags) {
        _cleanup_free_ अक्षर *n1 = शून्य;

        अगर (!FLAGS_SET(flags, CHASE_WARN))
                वापस -EREMOTE;

        (व्योम) fd_get_path(fd, &n1);

        वापस log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), path);
}

अटल पूर्णांक log_prohibited_symlink(पूर्णांक fd, ChaseFlags flags) {
        _cleanup_free_ अक्षर *n1 = शून्य;

        निश्चित(fd >= 0);

        अगर (!FLAGS_SET(flags, CHASE_WARN))
                वापस -ELOOP;

        (व्योम) fd_get_path(fd, &n1);

        वापस log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

पूर्णांक chaseat(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags flags, अक्षर **ret_path, पूर्णांक *ret_fd) {
        पूर्णांक r;

        निश्चित(!FLAGS_SET(flags, CHASE_PREFIX_मूल));
        निश्चित(!FLAGS_SET(flags, CHASE_STEP|CHASE_EXTRACT_FILENAME));
        निश्चित(!FLAGS_SET(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(wildcard_fd_is_valid(मूल_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        निश्चित(dir_fd != AT_FDCWD || IN_SET(मूल_fd, AT_FDCWD, XAT_FDROOT));

        अगर (FLAGS_SET(flags, CHASE_STEP))
                निश्चित(!ret_fd);

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
        अगर (मूल_fd == AT_FDCWD)
                मूल_fd = XAT_FDROOT;
        अन्यथा {
                r = dir_fd_is_मूल(मूल_fd);
                अगर (r < 0)
                        वापस r;
                अगर (r > 0)
                        मूल_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_मूल(dir_fd);
        अगर (r < 0)
                वापस r;
        अगर (r > 0 && मूल_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        अगर (dir_fd == XAT_FDROOT || path_is_absolute(path))
                dir_fd = मूल_fd;

        अगर (isempty(path))
                path = ".";

        bool append_trail_slash = false;
        अगर (ENDSWITH_SET(path, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                अगर (FLAGS_SET(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } अन्यथा अगर (dot_or_dot_dot(path) || endswith(path, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        अगर (FLAGS_SET(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        अगर (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_SET(flags, CHASE_MUST_BE_REGULAR) + FLAGS_SET(flags, CHASE_MUST_BE_SOCKET) > 1)
                वापस -EBADSLT;

        अगर (!ret_path) {
                r = chase_openat2(मूल_fd, dir_fd, path, flags);
                अगर (r >= 0) {
                        अगर (ret_fd)
                                *ret_fd = r;
                        अन्यथा
                                safe_close(r);

                        वापस 1;
                }
                अगर (r != -EOPNOTSUPP)
                        वापस r;
        }

        अगर (मूल_fd == XAT_FDROOT && !ret_path && (flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, path, flags, O_PATH|O_CLOEXEC, /* xopen_flags= */ 0);
                अगर (r < 0)
                        वापस r;

                अगर (ret_fd)
                        *ret_fd = r;
                अन्यथा
                        safe_close(r);

                वापस 1;
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
        bool need_absolute = (मूल_fd == XAT_FDROOT || dir_fd != मूल_fd) && (dir_fd == XAT_FDROOT || path_is_absolute(path));

        _cleanup_free_ अक्षर *done = शून्य;
        अगर (need_absolute) {
                done = strdup("/");
                अगर (!done)
                        वापस -ENOMEM;
        }

        _cleanup_close_ पूर्णांक fd = xopenat(dir_fd, शून्य, O_CLOEXEC|O_DIRECTORY|O_PATH);
        अगर (fd < 0)
                वापस fd;

        काष्ठा statx stx;
        r = chase_statx(fd, &stx);
        अगर (r < 0)
                वापस r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        काष्ठा statx मूल_stx;
        अगर (मूल_fd != XAT_FDROOT) {
                अगर (मूल_fd == dir_fd)
                        मूल_stx = stx;
                अन्यथा {
                        r = chase_statx(मूल_fd, &मूल_stx);
                        अगर (r < 0)
                                वापस r;
                }
        }

        _cleanup_free_ अक्षर *बफर = strdup(path);
        अगर (!बफर)
                वापस -ENOMEM;

        स्थिर अक्षर *todo = बफर;
        bool exists = true;
        क्रम (अचिन्हित n_steps = 0;; n_steps++) {
                _cleanup_free_ अक्षर *first = शून्य;
                _cleanup_close_ पूर्णांक child = -EBADF;
                काष्ठा statx stx_child;
                स्थिर अक्षर *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                अगर (n_steps > CHASE_MAX)
                        वापस -ELOOP;

                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0) /* We reached the end. */
                        अवरोध;

                first = strndup(e, r);
                अगर (!first)
                        वापस -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                अगर (streq(first, "..")) {
                        _cleanup_free_ अक्षर *parent = शून्य;
                        _cleanup_close_ पूर्णांक fd_parent = -EBADF;
                        काष्ठा statx stx_parent;

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
                        अगर (मूल_fd != XAT_FDROOT) {
                                bool is_मूल = मूल_fd == dir_fd && empty_or_मूल(done);
                                अगर (!is_मूल && statx_inode_same(&stx, &मूल_stx)) {
                                        r = statx_mount_same(&stx, &मूल_stx);
                                        अगर (r < 0)
                                                वापस r;

                                        is_मूल = r > 0;
                                }
                                अगर (is_मूल) {
                                        अगर (FLAGS_SET(flags, CHASE_STEP))
                                                जाओ chased_one;
                                        जारी;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                        अगर (fd_parent < 0)
                                वापस -त्रुटि_सं;

                        r = chase_statx(fd_parent, &stx_parent);
                        अगर (r < 0)
                                वापस r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        अगर (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_मूल(fd);
                                अगर (r < 0)
                                        वापस r;
                                अगर (r > 0) {
                                        अगर (FLAGS_SET(flags, CHASE_STEP))
                                                जाओ chased_one;
                                        जारी;
                                }
                        }

                        r = path_extract_directory(done, &parent);
                        अगर (r >= 0) {
                                निश्चित(!need_absolute || path_is_absolute(parent));
                                free_and_replace(done, parent);
                        } अन्यथा अगर (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                निश्चित(!need_absolute);
                                done = mfree(done);
                        } अन्यथा अगर (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } अन्यथा अगर (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                निश्चित(!need_absolute);

                                अगर (!isempty(done) && !path_is_valid(done))
                                        वापस -EINVAL;

                                अगर (!path_extend(&done, ".."))
                                        वापस -ENOMEM;
                        } अन्यथा
                                वापस r;

                        अगर (FLAGS_SET(flags, CHASE_STEP))
                                जाओ chased_one;

                        अगर (FLAGS_SET(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                अगर (r < 0)
                                        वापस r;
                                अगर (r > 0)
                                        वापस log_unsafe_transition(fd, fd_parent, path, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        अगर (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ पूर्णांक fd_grandparent = -EBADF;
                                काष्ठा statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_PATH|O_DIRECTORY);
                                अगर (fd_grandparent < 0)
                                        वापस -त्रुटि_सं;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                अगर (r < 0)
                                        वापस r;

                                अगर (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        अगर (r < 0)
                                                वापस r;
                                        अगर (r > 0)
                                                वापस log_unsafe_transition(fd_parent, fd_grandparent, path, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                अवरोध;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        जारी;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_full(fd, first,
                                         O_PATH|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_SET(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         MODE_INVALID);
                अगर (r < 0) {
                        अगर (r != -ENOENT)
                                वापस r;

                        अगर (!isempty(todo) && !path_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                वापस r;

                        अगर (FLAGS_SET(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_PATH|O_CLOEXEC);
                                अगर (child < 0)
                                        वापस child;
                        } अन्यथा अगर (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo)) {
                                अगर (!path_extend(&done, first))
                                        वापस -ENOMEM;

                                अवरोध;
                        } अन्यथा अगर (FLAGS_SET(flags, CHASE_NONEXISTENT)) {
                                अगर (!path_extend(&done, first, todo))
                                        वापस -ENOMEM;

                                exists = false;
                                अवरोध;
                        } अन्यथा
                                वापस r;
                }

                r = chase_statx(child, &stx_child);
                अगर (r < 0)
                        वापस r;

                अगर (FLAGS_SET(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        अगर (r < 0)
                                वापस r;
                        अगर (r > 0)
                                वापस log_unsafe_transition(fd, child, path, flags);
                }

                अगर (FLAGS_SET(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        वापस log_autofs_mount_point(child, path, flags);

                अगर (S_ISLNK(stx_child.stx_mode) && !(FLAGS_SET(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ अक्षर *destination = शून्य;

                        अगर (FLAGS_SET(flags, CHASE_PROHIBIT_SYMLINKS))
                                वापस log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        अगर (r < 0)
                                वापस r;
                        अगर (isempty(destination))
                                वापस -EINVAL;

                        अगर (path_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(मूल_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
                                अगर (fd < 0)
                                        वापस fd;

                                r = chase_statx(fd, &stx);
                                अगर (r < 0)
                                        वापस r;

                                अगर (FLAGS_SET(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        अगर (r < 0)
                                                वापस r;
                                        अगर (r > 0)
                                                वापस log_unsafe_transition(child, fd, path, flags);
                                }

                                अगर (dir_fd != मूल_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&done, need_absolute ? "/" : शून्य);
                                अगर (r < 0)
                                        वापस r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        अगर (!path_extend(&destination, todo))
                                वापस -ENOMEM;

                        free_and_replace(बफर, destination);
                        todo = बफर;

                        अगर (FLAGS_SET(flags, CHASE_STEP))
                                जाओ chased_one;

                        जारी;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                अगर (!path_extend(&done, first))
                        वापस -ENOMEM;

                अगर (FLAGS_SET(flags, CHASE_PARENT) && isempty(todo))
                        अवरोध;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        अगर (exists) {
                अगर (FLAGS_SET(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        अगर (r < 0)
                                वापस r;
                }

                अगर (FLAGS_SET(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        अगर (r < 0)
                                वापस r;
                }

                अगर (FLAGS_SET(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        अगर (r < 0)
                                वापस r;
                }
        }

        अगर (ret_path) {
                अगर (FLAGS_SET(flags, CHASE_EXTRACT_FILENAME) && done) {
                        _cleanup_free_ अक्षर *f = शून्य;

                        r = path_extract_filename(done, &f);
                        अगर (r < 0 && r != -EADDRNOTAVAIL)
                                वापस r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(done, f);
                }

                अगर (!done) {
                        निश्चित(!need_absolute || FLAGS_SET(flags, CHASE_EXTRACT_FILENAME));
                        done = strdup(".");
                        अगर (!done)
                                वापस -ENOMEM;
                }

                अगर (append_trail_slash)
                        अगर (!strextend(&done, "/"))
                                वापस -ENOMEM;

                *ret_path = TAKE_PTR(done);
        }

        अगर (ret_fd) {
                अगर (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        निश्चित(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } अन्यथा
                        *ret_fd = -EBADF;
        }

        अगर (FLAGS_SET(flags, CHASE_STEP))
                वापस 1;

        वापस exists;

chased_one:
        अगर (ret_path) {
                स्थिर अक्षर *e;

                अगर (!done) {
                        निश्चित(!need_absolute);
                        done = strdup(append_trail_slash ? "./" : ".");
                        अगर (!done)
                                वापस -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = path_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        *ret_path = TAKE_PTR(done);
                अन्यथा {
                        अक्षर *c;

                        c = path_join(done, e);
                        अगर (!c)
                                वापस -ENOMEM;

                        *ret_path = c;
                }
        }

        वापस 0;
}

पूर्णांक chase(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ChaseFlags flags, अक्षर **ret_path, पूर्णांक *ret_fd) {
        _cleanup_free_ अक्षर *मूल_abs = शून्य, *absolute = शून्य, *p = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF, pfd = -EBADF;
        पूर्णांक r;

        निश्चित(path);

        अगर (isempty(path))
                वापस -EINVAL;

        r = empty_or_मूल_harder_to_null(&मूल);
        अगर (r < 0)
                वापस r;

        /* A root directory of "/" or "" is identical to "/". */
        अगर (empty_or_मूल(मूल))
                मूल = "/";
        अन्यथा {
                r = path_make_absolute_cwd(मूल, &मूल_abs);
                अगर (r < 0)
                        वापस r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                मूल = path_simplify(मूल_abs);

                निश्चित(path_is_absolute(मूल));
                निश्चित(!empty_or_मूल(मूल));

                अगर (FLAGS_SET(flags, CHASE_PREFIX_मूल)) {
                        absolute = path_join(मूल, path);
                        अगर (!absolute)
                                वापस -ENOMEM;
                }
        }

        अगर (!absolute) {
                r = path_make_absolute_cwd(path, &absolute);
                अगर (r < 0)
                        वापस r;
        }

        path = path_startswith(absolute, मूल);
        अगर (!path)
                वापस log_full_errno(FLAGS_SET(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, मूल);

        अगर (empty_or_मूल(मूल))
                fd = XAT_FDROOT;
        अन्यथा {
                fd = खोलो(मूल, O_CLOEXEC|O_DIRECTORY|O_PATH);
                अगर (fd < 0)
                        वापस -त्रुटि_सं;
        }

        r = chaseat(fd, fd, path, flags & ~CHASE_PREFIX_मूल, ret_path ? &p : शून्य, ret_fd ? &pfd : शून्य);
        अगर (r < 0)
                वापस r;

        अगर (ret_path) {
                अगर (!FLAGS_SET(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        अगर (empty_or_मूल(मूल))
                                निश्चित(path_is_absolute(p));
                        अन्यथा {
                                अक्षर *q;

                                निश्चित(!path_is_absolute(p));

                                q = path_join(मूल, p + STR_IN_SET(p, ".", "./"));
                                अगर (!q)
                                        वापस -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_path = TAKE_PTR(p);
        }

        अगर (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        वापस r;
}

पूर्णांक chaseat_prefix_मूल(स्थिर अक्षर *path, स्थिर अक्षर *मूल, अक्षर **ret) {
        अक्षर *q;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        अगर (!path_is_absolute(path)) {
                _cleanup_free_ अक्षर *मूल_abs = शून्य;

                r = empty_or_मूल_harder_to_null(&मूल);
                अगर (r < 0 && r != -ENOENT)
                        वापस r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                अगर (empty_or_मूल(मूल))
                        वापस -EINVAL;

                r = path_make_absolute_cwd(मूल, &मूल_abs);
                अगर (r < 0)
                        वापस r;

                मूल = path_simplify(मूल_abs);

                q = path_join(मूल, path + (path[0] == '.' && IN_SET(path[1], '/', '\0')));
        } अन्यथा
                q = strdup(path);
        अगर (!q)
                वापस -ENOMEM;

        *ret = q;
        वापस 0;
}

पूर्णांक chase_extract_filename(स्थिर अक्षर *path, स्थिर अक्षर *मूल, अक्षर **ret) {
        पूर्णांक r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        निश्चित(path);
        निश्चित(ret);

        अगर (isempty(path))
                वापस -EINVAL;

        अगर (!path_is_absolute(path))
                वापस -EINVAL;

        r = empty_or_मूल_harder_to_null(&मूल);
        अगर (r < 0 && r != -ENOENT)
                वापस r;

        अगर (!empty_or_मूल(मूल)) {
                _cleanup_free_ अक्षर *मूल_abs = शून्य;

                r = path_make_absolute_cwd(मूल, &मूल_abs);
                अगर (r < 0)
                        वापस r;

                path = path_startswith(path, मूल_abs);
                अगर (!path)
                        वापस -EINVAL;
        }

        अगर (!isempty(path)) {
                r = path_extract_filename(path, ret);
                अगर (r != -EADDRNOTAVAIL)
                        वापस r;
        }

        वापस strdup_to(ret, ".");
}

पूर्णांक chase_and_open(
                स्थिर अक्षर *path,
                स्थिर अक्षर *मूल,
                ChaseFlags chase_flags,
                पूर्णांक open_flags,
                अक्षर **ret_path) {

        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य, *fname = शून्य;
        स्थिर अक्षर *open_name = शून्य;
        पूर्णांक r;

        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        अगर (empty_or_मूल(मूल) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                वापस chase_xopenat(AT_FDCWD, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(path, मूल, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        अगर (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                अगर (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                अन्यथा {
                        r = chase_extract_filename(p, मूल, &fname);
                        अगर (r < 0)
                                वापस r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस r;
}

पूर्णांक chase_and_opendir(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ChaseFlags chase_flags, अक्षर **ret_path, सूची **ret_dir) {
        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        सूची *d;
        पूर्णांक r;

        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        निश्चित(ret_dir);

        अगर (empty_or_मूल(मूल) && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = सूची_खोलो(path);
                अगर (!d)
                        वापस -त्रुटि_सं;

                *ret_dir = d;
                वापस 0;
        }

        r = chase(path, मूल, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_path ? &p : शून्य, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ शून्य, /* flags= */ 0);
        अगर (!d)
                वापस -त्रुटि_सं;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        वापस 0;
}

पूर्णांक chase_and_stat(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ChaseFlags chase_flags, अक्षर **ret_path, काष्ठा stat *ret_stat) {
        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        निश्चित(ret_stat);

        अगर (empty_or_मूल(मूल) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                वापस RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, मूल, chase_flags, ret_path ? &p : शून्य, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        अगर (ख_स्थिति(path_fd, ret_stat) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस 0;
}

पूर्णांक chase_and_access(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ChaseFlags chase_flags, पूर्णांक access_mode, अक्षर **ret_path) {
        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        अगर (empty_or_मूल(मूल) && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                वापस RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(path, मूल, chase_flags, ret_path ? &p : शून्य, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस 0;
}

पूर्णांक chase_and_fopen_unlocked(
                स्थिर अक्षर *path,
                स्थिर अक्षर *मूल,
                ChaseFlags chase_flags,
                स्थिर अक्षर *open_flags,
                अक्षर **ret_path,
                खाता **ret_file) {

        _cleanup_free_ अक्षर *final_path = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक mode_flags, r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        निश्चित(open_flags);
        निश्चित(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        अगर (mode_flags < 0)
                वापस mode_flags;

        fd = chase_and_open(path, मूल, chase_flags, mode_flags, ret_path ? &final_path : शून्य);
        अगर (fd < 0)
                वापस fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(final_path);

        वापस 0;
}

पूर्णांक chase_and_unlink(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ChaseFlags chase_flags, पूर्णांक unlink_flags, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *p = शून्य, *fname = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(path, मूल, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        अगर (fd < 0)
                वापस fd;

        r = path_extract_filename(p, &fname);
        अगर (r < 0)
                वापस r;

        अगर (unlinkat(fd, fname, unlink_flags) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस 0;
}

पूर्णांक chase_and_open_parent(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ChaseFlags chase_flags, अक्षर **ret_filename) {
        पूर्णांक pfd, r;

        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chase(path, मूल, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        अगर (r < 0)
                वापस r;

        वापस pfd;
}

पूर्णांक chase_and_openat(
                पूर्णांक मूल_fd,
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                ChaseFlags chase_flags,
                पूर्णांक open_flags,
                अक्षर **ret_path) {

        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य, *fname = शून्य;
        स्थिर अक्षर *open_name = शून्य;
        पूर्णांक r;

        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_SOCKET)));

        अगर (मूल_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                वापस chase_xopenat(dir_fd, path, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(मूल_fd, dir_fd, path, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &path_fd);
        अगर (r < 0)
                वापस r;

        अगर (!FLAGS_SET(chase_flags, CHASE_PARENT)) {
                अगर (FLAGS_SET(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                अन्यथा {
                        r = path_extract_filename(p, &fname);
                        अगर (r < 0 && r != -EADDRNOTAVAIL)
                                वापस r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(path_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस r;
}

पूर्णांक chase_and_opendirat(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags, अक्षर **ret_path, सूची **ret_dir) {
        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        सूची *d;
        पूर्णांक r;

        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        निश्चित(ret_dir);

        अगर (मूल_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & CHASE_NO_SHORTCUT_MASK) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = सूची_खोलो(path);
                अगर (!d)
                        वापस -त्रुटि_सं;

                *ret_dir = d;
                वापस 0;
        }

        r = chaseat(मूल_fd, dir_fd, path, chase_flags, ret_path ? &p : शून्य, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        d = xopendirat(path_fd, /* path= */ शून्य, /* flags= */ 0);
        अगर (!d)
                वापस -त्रुटि_सं;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        *ret_dir = d;
        वापस 0;
}

पूर्णांक chase_and_statat(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags, अक्षर **ret_path, काष्ठा stat *ret_stat) {
        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));
        निश्चित(ret_stat);

        अगर (मूल_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                वापस RET_NERRNO(fstatat(AT_FDCWD, path, ret_stat,
                                          FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(मूल_fd, dir_fd, path, chase_flags, ret_path ? &p : शून्य, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        अगर (ख_स्थिति(path_fd, ret_stat) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस 0;
}

पूर्णांक chase_and_accessat(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags, पूर्णांक access_mode, अक्षर **ret_path) {
        _cleanup_close_ पूर्णांक path_fd = -EBADF;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        अगर (मूल_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_path && (chase_flags & (CHASE_NO_SHORTCUT_MASK|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                वापस RET_NERRNO(faccessat(AT_FDCWD, path, access_mode,
                                            FLAGS_SET(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(मूल_fd, dir_fd, path, chase_flags, ret_path ? &p : शून्य, &path_fd);
        अगर (r < 0)
                वापस r;
        निश्चित(path_fd >= 0);

        r = access_fd(path_fd, access_mode);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस 0;
}

पूर्णांक chase_and_fopenat_unlocked(
                पूर्णांक मूल_fd,
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                ChaseFlags chase_flags,
                स्थिर अक्षर *open_flags,
                अक्षर **ret_path,
                खाता **ret_file) {

        _cleanup_free_ अक्षर *final_path = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक mode_flags, r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT)));
        निश्चित(open_flags);
        निश्चित(ret_file);

        mode_flags = fopen_mode_to_flags(open_flags);
        अगर (mode_flags < 0)
                वापस mode_flags;

        fd = chase_and_openat(मूल_fd, dir_fd, path, chase_flags, mode_flags, ret_path ? &final_path : शून्य);
        अगर (fd < 0)
                वापस fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(final_path);

        वापस 0;
}

पूर्णांक chase_and_unlinkat(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags, पूर्णांक unlink_flags, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *p = शून्य, *fname = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(मूल_fd, dir_fd, path, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_PATH|O_DIRECTORY|O_CLOEXEC, &p);
        अगर (fd < 0)
                वापस fd;

        r = path_extract_filename(p, &fname);
        अगर (r < 0)
                वापस r;

        अगर (unlinkat(fd, fname, unlink_flags) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_path)
                *ret_path = TAKE_PTR(p);

        वापस 0;
}

पूर्णांक chase_and_open_parent_at(पूर्णांक मूल_fd, पूर्णांक dir_fd, स्थिर अक्षर *path, ChaseFlags chase_flags, अक्षर **ret_filename) {
        पूर्णांक pfd, r;

        निश्चित(!(chase_flags & (CHASE_NONEXISTENT|CHASE_STEP)));

        r = chaseat(मूल_fd, dir_fd, path, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        अगर (r < 0)
                वापस r;

        वापस pfd;
}
