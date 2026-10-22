/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/magic.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "glyph-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "user-util.h"

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
#परिभाषय CHASE_NO_SHORTCUT_आवरणम्                          \
        (CHASE_NONEXISTENT |                            \
         CHASE_NO_AUTOFS |                              \
         CHASE_SAFE |                                   \
         CHASE_चरणः |                                   \
         CHASE_PROHIBIT_SYMLINKS |                      \
         CHASE_PARENT |                                 \
         CHASE_MKDIR_0755)

#परिभाषय CHASE_MUST_BE_ANY \
        (CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)

स्थिर पूर्णाङ्क chase_statx(पूर्णाङ्क fd, संरचना statx *ret) {
        प्रत्यागम xstatx_पूर्णम्(fd,
                        /* path= */ रिक्त,
                        /* statx_flags= */ 0,
                        XSTATX_MNT_ID_BEST,
                        STATX_TYPE|STATX_UID|STATX_INO,
                        /* optional_mask= */ 0,
                        /* mandatory_attributes= */ 0,
                        ret);
}

स्थिर पूर्णाङ्क chase_openat2(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags) {
        /* Open the target of a chase operation via openat2(), translating the relevant ChaseFlags into
         * RESOLVE_* and O_* flags and verifying MUST_BE_REGULAR/SOCKET via fstat after the open. Returns
         * -EOPNOTSUPP when openat2() is unavailable (older kernels) or blocked by a seccomp filter
         * (notably systemd's own filter, which returns ENOSYS to force programs onto the openat()
         * fallback path) — the verdict is cached so subsequent calls in the same process skip the syscall
         * entirely. */

        स्थिर bool can_openat2 = true;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        यदि (!can_openat2)
                प्रत्यागम -EOPNOTSUPP;

        /* openat2() can handle everything the regular shortcut handles, plus a real root boundary (via
         * RESOLVE_IN_ROOT) and CHASE_PROHIBIT_SYMLINKS (via RESOLVE_NO_SYMLINKS). It cannot model the other
         * CHASE_NO_SHORTCUT flags, cannot trigger automounts on O_PATH fds, and RESOLVE_IN_ROOT requires
         * the dirfd to be the root. Bail out so the caller falls back to the regular chase loop. */
        यदि ((chase_flags & (CHASE_NO_SHORTCUT_आवरणम् & ~CHASE_PROHIBIT_SYMLINKS)) != 0)
                प्रत्यागम -EOPNOTSUPP;
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_TRIGGER_AUTOFS))
                प्रत्यागम -EOPNOTSUPP;
        यदि (मूलम्_fd != XAT_FDROOT && मूलम्_fd != dir_fd)
                प्रत्यागम -EOPNOTSUPP;

        _cleanup_close_ पूर्णाङ्क dir_fd_local = -EBADF;
        यदि (dir_fd == XAT_FDROOT) {
                यदि (मार्गः_is_absolute(मार्गः))
                        dir_fd = AT_FDCWD;
                अन्यथा {
                        dir_fd_local = उद्घाटय("/", O_CLOEXEC|O_DIRECTORY|O_मार्गः);
                        यदि (dir_fd_local < 0)
                                प्रत्यागम -errno;
                        dir_fd = dir_fd_local;
                }
        }

        संरचना open_how how = {
                .flags = O_मार्गः|O_CLOEXEC,
        };
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_NOFOLLOW))
                how.flags |= O_NOFOLLOW;
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_MUST_BE_DIRECTORY))
                how.flags |= O_DIRECTORY;
        यदि (मूलम्_fd != XAT_FDROOT)
                how.resolve |= RESOLVE_अन्तर्_मूलम्;
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_PROHIBIT_SYMLINKS))
                how.resolve |= RESOLVE_NO_SYMLINKS;

        _cleanup_close_ पूर्णाङ्क fd = openat2(dir_fd, मार्गः, &how, परिमाण(how));
        यदि (fd < 0) {
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
                यदि (errno == ENOSYS)
                        can_openat2 = false;
                यदि (अन्तर्_समुच्चयः(errno, ENOSYS, EPERM, EAGAIN))
                        प्रत्यागम -EOPNOTSUPP;
                प्रत्यागम -errno;
        }

        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_MUST_BE_REGULAR)) {
                r = fd_verify_regular(fd);
                यदि (r < 0)
                        प्रत्यागम r;
        }
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_MUST_BE_SOCKET)) {
                r = fd_verify_socket(fd);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम TAKE_FD(fd);
}

स्थिर पूर्णाङ्क chase_xopenat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags, पूर्णाङ्क open_flags, XOpenFlags xopen_flags) {
        /* Wrapper around xopenat_full() that translates CHASE_NOFOLLOW, CHASE_MUST_BE_* and
         * CHASE_TRIGGER_AUTOFS into their xopenat_full() counterparts. Used by shortcuts that want to open
         * the final target of a chase operation: they all want O_NOFOLLOW honoured, MUST_BE_* verified on
         * the opened inode, and automounts triggered if requested. */

        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_NOFOLLOW))
                open_flags |= O_NOFOLLOW;
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_MUST_BE_DIRECTORY))
                open_flags |= O_DIRECTORY;
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_MUST_BE_REGULAR))
                xopen_flags |= XO_REGULAR;
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_MUST_BE_SOCKET))
                xopen_flags |= XO_SOCKET;
        /* Only needed for O_PATH since plain open() already triggers automounts */
        यदि (FLAGS_समुच्चयः(chase_flags, CHASE_TRIGGER_AUTOFS) && FLAGS_समुच्चयः(open_flags, O_मार्गः))
                xopen_flags |= XO_TRIGGER_AUTOMOUNT;

        प्रत्यागम xopenat_पूर्णम्(dir_fd, मार्गः, open_flags, xopen_flags, बहुलकम्_INVALID);
}

स्थिर bool uid_unsafe_transition(uid_t a, uid_t b) {
        /* Returns true if the transition from a to b is safe, i.e. that we never transition from
         * unprivileged to privileged files or directories. Why bother? So that unprivileged code can't
         * symlink to privileged files making us believe we read something safe even though it isn't safe in
         * the specific context we open it in. */

        यदि (a == 0) /* Transitioning from privileged to unprivileged is always fine */
                प्रत्यागम false;

        प्रत्यागम a != b; /* Otherwise we need to stay within the same UID */
}

पूर्णाङ्क statx_unsafe_transition(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        यदि (!FLAGS_समुच्चयः(a->stx_आवरणम्, STATX_UID) || !FLAGS_समुच्चयः(b->stx_आवरणम्, STATX_UID))
                प्रत्यागम -ENODATA;

        प्रत्यागम uid_unsafe_transition(a->stx_uid, b->stx_uid);
}

bool stat_unsafe_transition(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        प्रत्यागम uid_unsafe_transition(a->st_uid, b->st_uid);
}

स्थिर पूर्णाङ्क log_unsafe_transition(पूर्णाङ्क a, पूर्णाङ्क b, ध्रुव वर्ण *मार्गः, ChaseFlags flags) {
        _cleanup_free_ वर्ण *n1 = रिक्त, *n2 = रिक्त, *user_a = रिक्त, *user_b = रिक्त;
        संरचना stat st;

        यदि (!FLAGS_समुच्चयः(flags, CHASE_WARN))
                प्रत्यागम -ENOLINK;

        (शून्य) fd_get_मार्गः(a, &n1);
        (शून्य) fd_get_मार्गः(b, &n2);

        यदि (fstat(a, &st) == 0)
                user_a = uid_गन्तव्यम्_name(st.st_uid);
        यदि (fstat(b, &st) == 0)
                user_b = uid_गन्तव्यम्_name(st.st_uid);

        प्रत्यागम log_warning_errno(SYNTHETIC_ERRNO(ENOLINK),
                                 "Detected unsafe path transition %s (owned by %s) %s %s (owned by %s) during canonicalization of %s.",
                                 strna(n1), strna(user_a), glyph(GLYPH_ARROW_दक्षिणः), strna(n2), strna(user_b), मार्गः);
}

स्थिर पूर्णाङ्क log_autofs_mount_point(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, ChaseFlags flags) {
        _cleanup_free_ वर्ण *n1 = रिक्त;

        यदि (!FLAGS_समुच्चयः(flags, CHASE_WARN))
                प्रत्यागम -EREMOTE;

        (शून्य) fd_get_मार्गः(fd, &n1);

        प्रत्यागम log_warning_errno(SYNTHETIC_ERRNO(EREMOTE),
                                 "Detected autofs mount point '%s' during canonicalization of '%s'.",
                                 strna(n1), मार्गः);
}

स्थिर पूर्णाङ्क log_prohibited_symlink(पूर्णाङ्क fd, ChaseFlags flags) {
        _cleanup_free_ वर्ण *n1 = रिक्त;

        प्रतिज्ञा(fd >= 0);

        यदि (!FLAGS_समुच्चयः(flags, CHASE_WARN))
                प्रत्यागम -ELOOP;

        (शून्य) fd_get_मार्गः(fd, &n1);

        प्रत्यागम log_warning_errno(SYNTHETIC_ERRNO(ELOOP),
                                 "Detected symlink where no symlink is allowed at '%s', refusing.",
                                 strna(n1));
}

पूर्णाङ्क chaseat(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags flags, वर्ण **ret_मार्गः, पूर्णाङ्क *ret_fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(!FLAGS_समुच्चयः(flags, CHASE_उपसर्गः_मूलम्));
        प्रतिज्ञा(!FLAGS_समुच्चयः(flags, CHASE_चरणः|CHASE_EXTRACT_FILENAME));
        प्रतिज्ञा(!FLAGS_समुच्चयः(flags, CHASE_NO_AUTOFS|CHASE_TRIGGER_AUTOFS));
        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(wildcard_fd_is_valid(मूलम्_fd));
        /* AT_FDCWD for dir_fd is only allowed when there is no chroot boundary: otherwise the current
         * working directory might live outside root_fd's subtree. */
        प्रतिज्ञा(dir_fd != AT_FDCWD || अन्तर्_समुच्चयः(मूलम्_fd, AT_FDCWD, XAT_FDROOT));

        यदि (FLAGS_समुच्चयः(flags, CHASE_चरणः))
                प्रतिज्ञा(!ret_fd);

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
        यदि (मूलम्_fd == AT_FDCWD)
                मूलम्_fd = XAT_FDROOT;
        अन्यथा {
                r = dir_fd_is_मूलम्(मूलम्_fd);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r > 0)
                        मूलम्_fd = XAT_FDROOT;
        }

        /* If dir_fd points to the host's root directory and there is no chroot boundary, normalize it
         * to XAT_FDROOT so the shortcut path can kick in. */
        r = dir_fd_is_मूलम्(dir_fd);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r > 0 && मूलम्_fd == XAT_FDROOT)
                dir_fd = XAT_FDROOT;

        /* dir_fd == XAT_FDROOT means "start at root_fd". An absolute path is always resolved relative to
         * root_fd, regardless of what dir_fd points to. */
        यदि (dir_fd == XAT_FDROOT || मार्गः_is_absolute(मार्गः))
                dir_fd = मूलम्_fd;

        यदि (isempty(मार्गः))
                मार्गः = ".";

        bool append_trail_slash = false;
        यदि (ENDSWITH_समुच्चयः(मार्गः, "/", "/.")) {
                flags |= CHASE_MUST_BE_DIRECTORY;
                यदि (FLAGS_समुच्चयः(flags, CHASE_TRAIL_SLASH))
                        append_trail_slash = true;
        } अन्यथा यदि (dot_or_dot_dot(मार्गः) || endswith(मार्गः, "/.."))
                flags |= CHASE_MUST_BE_DIRECTORY;

        यदि (FLAGS_समुच्चयः(flags, CHASE_PARENT))
                flags |= CHASE_MUST_BE_DIRECTORY;

        /* If multiple flags are set now, fail immediately */
        यदि (FLAGS_समुच्चयः(flags, CHASE_MUST_BE_DIRECTORY) + FLAGS_समुच्चयः(flags, CHASE_MUST_BE_REGULAR) + FLAGS_समुच्चयः(flags, CHASE_MUST_BE_SOCKET) > 1)
                प्रत्यागम -EBADSLT;

        यदि (!ret_मार्गः) {
                r = chase_openat2(मूलम्_fd, dir_fd, मार्गः, flags);
                यदि (r >= 0) {
                        यदि (ret_fd)
                                *ret_fd = r;
                        अन्यथा
                                safe_close(r);

                        प्रत्यागम 1;
                }
                यदि (r != -EOPNOTSUPP)
                        प्रत्यागम r;
        }

        यदि (मूलम्_fd == XAT_FDROOT && !ret_मार्गः && (flags & CHASE_NO_SHORTCUT_आवरणम्) == 0) {
                /* Shortcut the common case where we don't have a real root boundary and no fancy features
                 * are requested: open the target directly via xopenat_full() which applies any MUST_BE_*
                 * verification and automount triggering for us. */

                r = chase_xopenat(dir_fd, मार्गः, flags, O_मार्गः|O_CLOEXEC, /* xopen_flags= */ 0);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (ret_fd)
                        *ret_fd = r;
                अन्यथा
                        safe_close(r);

                प्रत्यागम 1;
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
        bool need_absolute = (मूलम्_fd == XAT_FDROOT || dir_fd != मूलम्_fd) && (dir_fd == XAT_FDROOT || मार्गः_is_absolute(मार्गः));

        _cleanup_free_ वर्ण *समाप्तम् = रिक्त;
        यदि (need_absolute) {
                समाप्तम् = strdup("/");
                यदि (!समाप्तम्)
                        प्रत्यागम -ENOMEM;
        }

        _cleanup_close_ पूर्णाङ्क fd = xopenat(dir_fd, रिक्त, O_CLOEXEC|O_DIRECTORY|O_मार्गः);
        यदि (fd < 0)
                प्रत्यागम fd;

        संरचना statx stx;
        r = chase_statx(fd, &stx);
        यदि (r < 0)
                प्रत्यागम r;

        /* Remember stat data of the root, so that we can recognize it later during .. handling. Only
         * needed when there is an actual chroot boundary — with root_fd == XAT_FDROOT the boundary
         * check in the .. loop below is skipped and root_stx is never consulted. */
        संरचना statx मूलम्_stx;
        यदि (मूलम्_fd != XAT_FDROOT) {
                यदि (मूलम्_fd == dir_fd)
                        मूलम्_stx = stx;
                अन्यथा {
                        r = chase_statx(मूलम्_fd, &मूलम्_stx);
                        यदि (r < 0)
                                प्रत्यागम r;
                }
        }

        _cleanup_free_ वर्ण *बफर = strdup(मार्गः);
        यदि (!बफर)
                प्रत्यागम -ENOMEM;

        ध्रुव वर्ण *todo = बफर;
        bool exists = true;
        प्रति (अचिह्नित n_चरणाः = 0;; n_चरणाः++) {
                _cleanup_free_ वर्ण *first = रिक्त;
                _cleanup_close_ पूर्णाङ्क child = -EBADF;
                संरचना statx stx_child;
                ध्रुव वर्ण *e;

                /* If people change our tree behind our back, they might send us in circles. Put a limit on
                 * things */
                यदि (n_चरणाः > CHASE_MAX)
                        प्रत्यागम -ELOOP;

                r = मार्गः_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0) /* We reached the end. */
                        भङ्ग;

                first = strndup(e, r);
                यदि (!first)
                        प्रत्यागम -ENOMEM;

                /* Two dots? Then chop off the last bit of what we already found out. */
                यदि (streq(first, "..")) {
                        _cleanup_free_ वर्ण *parent = रिक्त;
                        _cleanup_close_ पूर्णाङ्क fd_parent = -EBADF;
                        संरचना statx stx_parent;

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
                        यदि (मूलम्_fd != XAT_FDROOT) {
                                bool is_मूलम् = मूलम्_fd == dir_fd && रिक्तानि_or_मूलम्(समाप्तम्);
                                यदि (!is_मूलम् && statx_inode_same(&stx, &मूलम्_stx)) {
                                        r = statx_mount_same(&stx, &मूलम्_stx);
                                        यदि (r < 0)
                                                प्रत्यागम r;

                                        is_मूलम् = r > 0;
                                }
                                यदि (is_मूलम्) {
                                        यदि (FLAGS_समुच्चयः(flags, CHASE_चरणः))
                                                गच्छ chased_one;
                                        अनुवर्त;
                                }
                        }

                        fd_parent = openat(fd, "..", O_CLOEXEC|O_NOFOLLOW|O_मार्गः|O_DIRECTORY);
                        यदि (fd_parent < 0)
                                प्रत्यागम -errno;

                        r = chase_statx(fd_parent, &stx_parent);
                        यदि (r < 0)
                                प्रत्यागम r;

                        /* If we opened the same directory, that _may_ indicate that we're at the host root
                         * directory. Let's confirm that in more detail with dir_fd_is_root(). And if so,
                         * going up won't change anything. */
                        यदि (statx_inode_same(&stx_parent, &stx)) {
                                r = dir_fd_is_मूलम्(fd);
                                यदि (r < 0)
                                        प्रत्यागम r;
                                यदि (r > 0) {
                                        यदि (FLAGS_समुच्चयः(flags, CHASE_चरणः))
                                                गच्छ chased_one;
                                        अनुवर्त;
                                }
                        }

                        r = मार्गः_extract_directory(समाप्तम्, &parent);
                        यदि (r >= 0) {
                                प्रतिज्ञा(!need_absolute || मार्गः_is_absolute(parent));
                                free_and_replace(समाप्तम्, parent);
                        } अन्यथा यदि (r == -EDESTADDRREQ) {
                                /* 'done' contains filename only (i.e. no slash). */
                                प्रतिज्ञा(!need_absolute);
                                समाप्तम् = mfree(समाप्तम्);
                        } अन्यथा यदि (r == -EADDRNOTAVAIL) {
                                /* 'done' is "/". This branch should already be handled above via the
                                 * is_root check. */
                                assert_not_reached();
                        } अन्यथा यदि (r == -EINVAL) {
                                /* 'done' is empty (we haven't descended past the starting dir_fd yet), or
                                 * ends with '..'. In both cases we're traversing above the starting point
                                 * (valid when root_fd is XAT_FDROOT, or when dir_fd was below root_fd to
                                 * start with), so record another '..' in 'done'. */
                                प्रतिज्ञा(!need_absolute);

                                यदि (!isempty(समाप्तम्) && !मार्गः_is_valid(समाप्तम्))
                                        प्रत्यागम -EINVAL;

                                यदि (!मार्गः_extend(&समाप्तम्, ".."))
                                        प्रत्यागम -ENOMEM;
                        } अन्यथा
                                प्रत्यागम r;

                        यदि (FLAGS_समुच्चयः(flags, CHASE_चरणः))
                                गच्छ chased_one;

                        यदि (FLAGS_समुच्चयः(flags, CHASE_SAFE)) {
                                r = statx_unsafe_transition(&stx, &stx_parent);
                                यदि (r < 0)
                                        प्रत्यागम r;
                                यदि (r > 0)
                                        प्रत्यागम log_unsafe_transition(fd, fd_parent, मार्गः, flags);
                        }

                        /* If the path ends on a "..", and CHASE_PARENT is specified then our current 'fd' is
                         * the child of the returned normalized path, not the parent as requested. To correct
                         * this we have to go *two* levels up. */
                        यदि (FLAGS_समुच्चयः(flags, CHASE_PARENT) && isempty(todo)) {
                                _cleanup_close_ पूर्णाङ्क fd_grandparent = -EBADF;
                                संरचना statx stx_grandparent;

                                fd_grandparent = openat(fd_parent, "..", O_CLOEXEC|O_NOFOLLOW|O_मार्गः|O_DIRECTORY);
                                यदि (fd_grandparent < 0)
                                        प्रत्यागम -errno;

                                r = chase_statx(fd_grandparent, &stx_grandparent);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (FLAGS_समुच्चयः(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_parent, &stx_grandparent);
                                        यदि (r < 0)
                                                प्रत्यागम r;
                                        यदि (r > 0)
                                                प्रत्यागम log_unsafe_transition(fd_parent, fd_grandparent, मार्गः, flags);
                                }

                                stx = stx_grandparent;
                                close_and_replace(fd, fd_grandparent);
                                भङ्ग;
                        }

                        /* update fd and stat */
                        stx = stx_parent;
                        close_and_replace(fd, fd_parent);
                        अनुवर्त;
                }

                /* Otherwise let's pin it by file descriptor, via O_PATH. */
                child = r = xopenat_पूर्णम्(fd, first,
                                         O_मार्गः|O_NOFOLLOW|O_CLOEXEC,
                                         FLAGS_समुच्चयः(flags, CHASE_TRIGGER_AUTOFS) ? XO_TRIGGER_AUTOMOUNT : 0,
                                         बहुलकम्_INVALID);
                यदि (r < 0) {
                        यदि (r != -ENOENT)
                                प्रत्यागम r;

                        यदि (!isempty(todo) && !मार्गः_is_safe(todo)) /* Refuse parent/mkdir handling if suffix contains ".." or something weird */
                                प्रत्यागम r;

                        यदि (FLAGS_समुच्चयः(flags, CHASE_MKDIR_0755) && (!isempty(todo) || !(flags & (CHASE_PARENT|CHASE_NONEXISTENT)))) {
                                child = xopenat(fd,
                                                first,
                                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_मार्गः|O_CLOEXEC);
                                यदि (child < 0)
                                        प्रत्यागम child;
                        } अन्यथा यदि (FLAGS_समुच्चयः(flags, CHASE_PARENT) && isempty(todo)) {
                                यदि (!मार्गः_extend(&समाप्तम्, first))
                                        प्रत्यागम -ENOMEM;

                                भङ्ग;
                        } अन्यथा यदि (FLAGS_समुच्चयः(flags, CHASE_NONEXISTENT)) {
                                यदि (!मार्गः_extend(&समाप्तम्, first, todo))
                                        प्रत्यागम -ENOMEM;

                                exists = false;
                                भङ्ग;
                        } अन्यथा
                                प्रत्यागम r;
                }

                r = chase_statx(child, &stx_child);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (FLAGS_समुच्चयः(flags, CHASE_SAFE)) {
                        r = statx_unsafe_transition(&stx, &stx_child);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r > 0)
                                प्रत्यागम log_unsafe_transition(fd, child, मार्गः, flags);
                }

                यदि (FLAGS_समुच्चयः(flags, CHASE_NO_AUTOFS) &&
                    fd_is_fs_type(child, AUTOFS_SUPER_MAGIC) > 0)
                        प्रत्यागम log_autofs_mount_point(child, मार्गः, flags);

                यदि (S_ISLNK(stx_child.stx_बहुलकम्) && !(FLAGS_समुच्चयः(flags, CHASE_NOFOLLOW) && isempty(todo))) {
                        _cleanup_free_ वर्ण *destination = रिक्त;

                        यदि (FLAGS_समुच्चयः(flags, CHASE_PROHIBIT_SYMLINKS))
                                प्रत्यागम log_prohibited_symlink(child, flags);

                        r = readlinkat_malloc(fd, first, &destination);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (isempty(destination))
                                प्रत्यागम -EINVAL;

                        यदि (मार्गः_is_absolute(destination)) {

                                /* An absolute destination. Start the loop from the beginning, but use the
                                 * root file descriptor as base. */

                                safe_close(fd);
                                fd = fd_reopen(मूलम्_fd, O_CLOEXEC|O_मार्गः|O_DIRECTORY);
                                यदि (fd < 0)
                                        प्रत्यागम fd;

                                r = chase_statx(fd, &stx);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (FLAGS_समुच्चयः(flags, CHASE_SAFE)) {
                                        r = statx_unsafe_transition(&stx_child, &stx);
                                        यदि (r < 0)
                                                प्रत्यागम r;
                                        यदि (r > 0)
                                                प्रत्यागम log_unsafe_transition(child, fd, मार्गः, flags);
                                }

                                यदि (dir_fd != मूलम्_fd)
                                        need_absolute = true;

                                r = free_and_strdup(&समाप्तम्, need_absolute ? "/" : रिक्त);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        /* Prefix what's left to do with what we just read, and start the loop again, but
                         * remain in the current directory. */
                        यदि (!मार्गः_extend(&destination, todo))
                                प्रत्यागम -ENOMEM;

                        free_and_replace(बफर, destination);
                        todo = बफर;

                        यदि (FLAGS_समुच्चयः(flags, CHASE_चरणः))
                                गच्छ chased_one;

                        अनुवर्त;
                }

                /* If this is not a symlink, then let's just add the name we read to what we already verified. */
                यदि (!मार्गः_extend(&समाप्तम्, first))
                        प्रत्यागम -ENOMEM;

                यदि (FLAGS_समुच्चयः(flags, CHASE_PARENT) && isempty(todo))
                        भङ्ग;

                /* And iterate again, but go one directory further down. */
                stx = stx_child;
                close_and_replace(fd, child);
        }

        यदि (exists) {
                यदि (FLAGS_समुच्चयः(flags, CHASE_MUST_BE_DIRECTORY)) {
                        r = statx_verify_directory(&stx);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (FLAGS_समुच्चयः(flags, CHASE_MUST_BE_REGULAR)) {
                        r = statx_verify_regular(&stx);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (FLAGS_समुच्चयः(flags, CHASE_MUST_BE_SOCKET)) {
                        r = statx_verify_socket(&stx);
                        यदि (r < 0)
                                प्रत्यागम r;
                }
        }

        यदि (ret_मार्गः) {
                यदि (FLAGS_समुच्चयः(flags, CHASE_EXTRACT_FILENAME) && समाप्तम्) {
                        _cleanup_free_ वर्ण *f = रिक्त;

                        r = मार्गः_extract_filename(समाप्तम्, &f);
                        यदि (r < 0 && r != -EADDRNOTAVAIL)
                                प्रत्यागम r;

                        /* If we get EADDRNOTAVAIL we clear done and it will get reinitialized by the next block. */
                        free_and_replace(समाप्तम्, f);
                }

                यदि (!समाप्तम्) {
                        प्रतिज्ञा(!need_absolute || FLAGS_समुच्चयः(flags, CHASE_EXTRACT_FILENAME));
                        समाप्तम् = strdup(".");
                        यदि (!समाप्तम्)
                                प्रत्यागम -ENOMEM;
                }

                यदि (append_trail_slash)
                        यदि (!strextend(&समाप्तम्, "/"))
                                प्रत्यागम -ENOMEM;

                *ret_मार्गः = TAKE_PTR(समाप्तम्);
        }

        यदि (ret_fd) {
                यदि (exists) {
                        /* Return the O_PATH fd we currently are looking to the caller. It can translate it
                         * to a proper fd by opening /proc/self/fd/xyz. */
                        प्रतिज्ञा(fd >= 0);
                        *ret_fd = TAKE_FD(fd);
                } अन्यथा
                        *ret_fd = -EBADF;
        }

        यदि (FLAGS_समुच्चयः(flags, CHASE_चरणः))
                प्रत्यागम 1;

        प्रत्यागम exists;

chased_one:
        यदि (ret_मार्गः) {
                ध्रुव वर्ण *e;

                यदि (!समाप्तम्) {
                        प्रतिज्ञा(!need_absolute);
                        समाप्तम् = strdup(append_trail_slash ? "./" : ".");
                        यदि (!समाप्तम्)
                                प्रत्यागम -ENOMEM;
                }

                /* todo may contain slashes at the beginning. */
                r = मार्गः_find_first_component(&todo, /* accept_dot_dot= */ true, &e);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        *ret_मार्गः = TAKE_PTR(समाप्तम्);
                अन्यथा {
                        वर्ण *c;

                        c = मार्गः_join(समाप्तम्, e);
                        यदि (!c)
                                प्रत्यागम -ENOMEM;

                        *ret_मार्गः = c;
                }
        }

        प्रत्यागम 0;
}

पूर्णाङ्क chase(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, ChaseFlags flags, वर्ण **ret_मार्गः, पूर्णाङ्क *ret_fd) {
        _cleanup_free_ वर्ण *मूलम्_abs = रिक्त, *absolute = रिक्त, *p = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF, pfd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        यदि (isempty(मार्गः))
                प्रत्यागम -EINVAL;

        r = रिक्तानि_or_मूलम्_harder_गन्तव्यम्_null(&मूलम्);
        यदि (r < 0)
                प्रत्यागम r;

        /* A root directory of "/" or "" is identical to "/". */
        यदि (रिक्तानि_or_मूलम्(मूलम्))
                मूलम् = "/";
        अन्यथा {
                r = मार्गः_make_absolute_cwd(मूलम्, &मूलम्_abs);
                यदि (r < 0)
                        प्रत्यागम r;

                /* Simplify the root directory, so that it has no duplicate slashes and nothing at the
                 * end. While we won't resolve the root path we still simplify it. */
                मूलम् = मार्गः_simplify(मूलम्_abs);

                प्रतिज्ञा(मार्गः_is_absolute(मूलम्));
                प्रतिज्ञा(!रिक्तानि_or_मूलम्(मूलम्));

                यदि (FLAGS_समुच्चयः(flags, CHASE_उपसर्गः_मूलम्)) {
                        absolute = मार्गः_join(मूलम्, मार्गः);
                        यदि (!absolute)
                                प्रत्यागम -ENOMEM;
                }
        }

        यदि (!absolute) {
                r = मार्गः_make_absolute_cwd(मार्गः, &absolute);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        मार्गः = मार्गः_startswith(absolute, मूलम्);
        यदि (!मार्गः)
                प्रत्यागम log_पूर्णम्_errno(FLAGS_समुच्चयः(flags, CHASE_WARN) ? LOG_WARNING : LOG_DEBUG,
                                      SYNTHETIC_ERRNO(ECHRNG),
                                      "Specified path '%s' is outside of specified root directory '%s', refusing to resolve.",
                                      absolute, मूलम्);

        यदि (रिक्तानि_or_मूलम्(मूलम्))
                fd = XAT_FDROOT;
        अन्यथा {
                fd = उद्घाटय(मूलम्, O_CLOEXEC|O_DIRECTORY|O_मार्गः);
                यदि (fd < 0)
                        प्रत्यागम -errno;
        }

        r = chaseat(fd, fd, मार्गः, flags & ~CHASE_उपसर्गः_मूलम्, ret_मार्गः ? &p : रिक्त, ret_fd ? &pfd : रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः) {
                यदि (!FLAGS_समुच्चयः(flags, CHASE_EXTRACT_FILENAME)) {

                        /* When "root" points to the root directory, the result of chaseat() is always
                         * absolute, hence it is not necessary to prefix with the root. When "root" points to
                         * a non-root directory, the result path is always normalized and relative, hence
                         * we can simply call path_join() and not necessary to call path_simplify().
                         * As a special case, chaseat() may return "." or "./", which are normalized too,
                         * but we need to drop "." before merging with root. */

                        यदि (रिक्तानि_or_मूलम्(मूलम्))
                                प्रतिज्ञा(मार्गः_is_absolute(p));
                        अन्यथा {
                                वर्ण *q;

                                प्रतिज्ञा(!मार्गः_is_absolute(p));

                                q = मार्गः_join(मूलम्, p + STR_अन्तर्_समुच्चयः(p, ".", "./"));
                                यदि (!q)
                                        प्रत्यागम -ENOMEM;

                                free_and_replace(p, q);
                        }
                }

                *ret_मार्गः = TAKE_PTR(p);
        }

        यदि (ret_fd)
                *ret_fd = TAKE_FD(pfd);

        प्रत्यागम r;
}

पूर्णाङ्क chaseat_उपसर्गः_मूलम्(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, वर्ण **ret) {
        वर्ण *q;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(ret);

        /* This is mostly for prefixing the result of chaseat(). */

        यदि (!मार्गः_is_absolute(मार्गः)) {
                _cleanup_free_ वर्ण *मूलम्_abs = रिक्त;

                r = रिक्तानि_or_मूलम्_harder_गन्तव्यम्_null(&मूलम्);
                यदि (r < 0 && r != -ENOENT)
                        प्रत्यागम r;

                /* If the dir_fd points to the root directory, chaseat() always returns an absolute path. */
                यदि (रिक्तानि_or_मूलम्(मूलम्))
                        प्रत्यागम -EINVAL;

                r = मार्गः_make_absolute_cwd(मूलम्, &मूलम्_abs);
                यदि (r < 0)
                        प्रत्यागम r;

                मूलम् = मार्गः_simplify(मूलम्_abs);

                q = मार्गः_join(मूलम्, मार्गः + (मार्गः[0] == '.' && अन्तर्_समुच्चयः(मार्गः[1], '/', '\0')));
        } अन्यथा
                q = strdup(मार्गः);
        यदि (!q)
                प्रत्यागम -ENOMEM;

        *ret = q;
        प्रत्यागम 0;
}

पूर्णाङ्क chase_extract_filename(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, वर्ण **ret) {
        पूर्णाङ्क r;

        /* This is similar to path_extract_filename(), but takes root directory.
         * The result should be consistent with chase() with CHASE_EXTRACT_FILENAME. */

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(ret);

        यदि (isempty(मार्गः))
                प्रत्यागम -EINVAL;

        यदि (!मार्गः_is_absolute(मार्गः))
                प्रत्यागम -EINVAL;

        r = रिक्तानि_or_मूलम्_harder_गन्तव्यम्_null(&मूलम्);
        यदि (r < 0 && r != -ENOENT)
                प्रत्यागम r;

        यदि (!रिक्तानि_or_मूलम्(मूलम्)) {
                _cleanup_free_ वर्ण *मूलम्_abs = रिक्त;

                r = मार्गः_make_absolute_cwd(मूलम्, &मूलम्_abs);
                यदि (r < 0)
                        प्रत्यागम r;

                मार्गः = मार्गः_startswith(मार्गः, मूलम्_abs);
                यदि (!मार्गः)
                        प्रत्यागम -EINVAL;
        }

        यदि (!isempty(मार्गः)) {
                r = मार्गः_extract_filename(मार्गः, ret);
                यदि (r != -EADDRNOTAVAIL)
                        प्रत्यागम r;
        }

        प्रत्यागम strdup_गन्तव्यम्(ret, ".");
}

पूर्णाङ्क chase_and_open(
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *मूलम्,
                ChaseFlags chase_flags,
                पूर्णाङ्क open_flags,
                वर्ण **ret_मार्गः) {

        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त, *fname = रिक्त;
        ध्रुव वर्ण *open_name = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));

        यदि (रिक्तानि_or_मूलम्(मूलम्) && !ret_मार्गः && (chase_flags & CHASE_NO_SHORTCUT_आवरणम्) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                प्रत्यागम chase_xopenat(AT_FDCWD, मार्गः, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chase(मार्गः, मूलम्, (CHASE_PARENT|chase_flags)&~CHASE_MUST_BE_REGULAR, &p, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        यदि (!FLAGS_समुच्चयः(chase_flags, CHASE_PARENT)) {
                यदि (FLAGS_समुच्चयः(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chase() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                अन्यथा {
                        r = chase_extract_filename(p, मूलम्, &fname);
                        यदि (r < 0)
                                प्रत्यागम r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(मार्गः_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम r;
}

पूर्णाङ्क chase_and_opendir(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, ChaseFlags chase_flags, वर्ण **ret_मार्गः, DIR **ret_dir) {
        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        DIR *d;
        पूर्णाङ्क r;

        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        प्रतिज्ञा(ret_dir);

        यदि (रिक्तानि_or_मूलम्(मूलम्) && !ret_मार्गः && (chase_flags & CHASE_NO_SHORTCUT_आवरणम्) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(मार्गः);
                यदि (!d)
                        प्रत्यागम -errno;

                *ret_dir = d;
                प्रत्यागम 0;
        }

        r = chase(मार्गः, मूलम्, chase_flags|CHASE_MUST_BE_DIRECTORY, ret_मार्गः ? &p : रिक्त, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        d = xopendirat(मार्गः_fd, /* path= */ रिक्त, /* flags= */ 0);
        यदि (!d)
                प्रत्यागम -errno;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        *ret_dir = d;
        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_stat(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, ChaseFlags chase_flags, वर्ण **ret_मार्गः, संरचना stat *ret_stat) {
        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));
        प्रतिज्ञा(ret_stat);

        यदि (रिक्तानि_or_मूलम्(मूलम्) && !ret_मार्गः && (chase_flags & (CHASE_NO_SHORTCUT_आवरणम्|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                प्रत्यागम RET_NERRNO(fstatat(AT_FDCWD, मार्गः, ret_stat,
                                          FLAGS_समुच्चयः(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(मार्गः, मूलम्, chase_flags, ret_मार्गः ? &p : रिक्त, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        यदि (fstat(मार्गः_fd, ret_stat) < 0)
                प्रत्यागम -errno;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_access(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, ChaseFlags chase_flags, पूर्णाङ्क access_बहुलकम्, वर्ण **ret_मार्गः) {
        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));

        यदि (रिक्तानि_or_मूलम्(मूलम्) && !ret_मार्गः && (chase_flags & (CHASE_NO_SHORTCUT_आवरणम्|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                प्रत्यागम RET_NERRNO(faccessat(AT_FDCWD, मार्गः, access_बहुलकम्,
                                            FLAGS_समुच्चयः(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chase(मार्गः, मूलम्, chase_flags, ret_मार्गः ? &p : रिक्त, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        r = access_fd(मार्गः_fd, access_बहुलकम्);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_fopen_unlocked(
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *मूलम्,
                ChaseFlags chase_flags,
                ध्रुव वर्ण *open_flags,
                वर्ण **ret_मार्गः,
                FILE **ret_file) {

        _cleanup_free_ वर्ण *final_मार्गः = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क बहुलकम्_flags, r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_PARENT|CHASE_MUST_BE_DIRECTORY|CHASE_MUST_BE_SOCKET)));
        प्रतिज्ञा(open_flags);
        प्रतिज्ञा(ret_file);

        बहुलकम्_flags = fopen_बहुलकम्_गन्तव्यम्_flags(open_flags);
        यदि (बहुलकम्_flags < 0)
                प्रत्यागम बहुलकम्_flags;

        fd = chase_and_open(मार्गः, मूलम्, chase_flags, बहुलकम्_flags, ret_मार्गः ? &final_मार्गः : रिक्त);
        यदि (fd < 0)
                प्रत्यागम fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(final_मार्गः);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_unlink(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, ChaseFlags chase_flags, पूर्णाङ्क unlink_flags, वर्ण **ret_मार्गः) {
        _cleanup_free_ वर्ण *p = रिक्त, *fname = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_open(मार्गः, मूलम्, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_मार्गः|O_DIRECTORY|O_CLOEXEC, &p);
        यदि (fd < 0)
                प्रत्यागम fd;

        r = मार्गः_extract_filename(p, &fname);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (unlinkat(fd, fname, unlink_flags) < 0)
                प्रत्यागम -errno;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_open_parent(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *मूलम्, ChaseFlags chase_flags, वर्ण **ret_filename) {
        पूर्णाङ्क pfd, r;

        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));

        r = chase(मार्गः, मूलम्, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम pfd;
}

पूर्णाङ्क chase_and_openat(
                पूर्णाङ्क मूलम्_fd,
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *मार्गः,
                ChaseFlags chase_flags,
                पूर्णाङ्क open_flags,
                वर्ण **ret_मार्गः) {

        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त, *fname = रिक्त;
        ध्रुव वर्ण *open_name = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_MUST_BE_SOCKET)));

        यदि (मूलम्_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_मार्गः && (chase_flags & CHASE_NO_SHORTCUT_आवरणम्) == 0)
                /* Shortcut this call if none of the special features of this call are requested */
                प्रत्यागम chase_xopenat(dir_fd, मार्गः, chase_flags, open_flags, /* xopen_flags= */ 0);

        r = chaseat(मूलम्_fd, dir_fd, मार्गः, (chase_flags|CHASE_PARENT)&~CHASE_MUST_BE_REGULAR, &p, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!FLAGS_समुच्चयः(chase_flags, CHASE_PARENT)) {
                यदि (FLAGS_समुच्चयः(chase_flags, CHASE_EXTRACT_FILENAME))
                        /* chaseat() with CHASE_EXTRACT_FILENAME already returns just the filename in
                         * p — use it directly without redundant extraction. */
                        open_name = p;
                अन्यथा {
                        r = मार्गः_extract_filename(p, &fname);
                        यदि (r < 0 && r != -EADDRNOTAVAIL)
                                प्रत्यागम r;
                        open_name = fname;
                }
        }

        r = chase_xopenat(मार्गः_fd, strempty(open_name), chase_flags, open_flags|O_NOFOLLOW, /* xopen_flags= */ 0);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम r;
}

पूर्णाङ्क chase_and_opendirat(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags, वर्ण **ret_मार्गः, DIR **ret_dir) {
        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        DIR *d;
        पूर्णाङ्क r;

        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_SOCKET)));
        प्रतिज्ञा(ret_dir);

        यदि (मूलम्_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_मार्गः && (chase_flags & CHASE_NO_SHORTCUT_आवरणम्) == 0) {
                /* Shortcut this call if none of the special features of this call are requested */
                d = opendir(मार्गः);
                यदि (!d)
                        प्रत्यागम -errno;

                *ret_dir = d;
                प्रत्यागम 0;
        }

        r = chaseat(मूलम्_fd, dir_fd, मार्गः, chase_flags, ret_मार्गः ? &p : रिक्त, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        d = xopendirat(मार्गः_fd, /* path= */ रिक्त, /* flags= */ 0);
        यदि (!d)
                प्रत्यागम -errno;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        *ret_dir = d;
        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_statat(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags, वर्ण **ret_मार्गः, संरचना stat *ret_stat) {
        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));
        प्रतिज्ञा(ret_stat);

        यदि (मूलम्_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_मार्गः && (chase_flags & (CHASE_NO_SHORTCUT_आवरणम्|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. We can't
                 * take the shortcut if CHASE_MUST_BE_* is set because fstatat() alone does not verify the
                 * inode type. */
                प्रत्यागम RET_NERRNO(fstatat(AT_FDCWD, मार्गः, ret_stat,
                                          FLAGS_समुच्चयः(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(मूलम्_fd, dir_fd, मार्गः, chase_flags, ret_मार्गः ? &p : रिक्त, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        यदि (fstat(मार्गः_fd, ret_stat) < 0)
                प्रत्यागम -errno;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_accessat(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags, पूर्णाङ्क access_बहुलकम्, वर्ण **ret_मार्गः) {
        _cleanup_close_ पूर्णाङ्क मार्गः_fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));

        यदि (मूलम्_fd == XAT_FDROOT && dir_fd == AT_FDCWD && !ret_मार्गः && (chase_flags & (CHASE_NO_SHORTCUT_आवरणम्|CHASE_MUST_BE_ANY)) == 0)
                /* Shortcut this call if none of the special features of this call are requested. */
                प्रत्यागम RET_NERRNO(faccessat(AT_FDCWD, मार्गः, access_बहुलकम्,
                                            FLAGS_समुच्चयः(chase_flags, CHASE_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0));

        r = chaseat(मूलम्_fd, dir_fd, मार्गः, chase_flags, ret_मार्गः ? &p : रिक्त, &मार्गः_fd);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(मार्गः_fd >= 0);

        r = access_fd(मार्गः_fd, access_बहुलकम्);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_fopenat_unlocked(
                पूर्णाङ्क मूलम्_fd,
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *मार्गः,
                ChaseFlags chase_flags,
                ध्रुव वर्ण *open_flags,
                वर्ण **ret_मार्गः,
                FILE **ret_file) {

        _cleanup_free_ वर्ण *final_मार्गः = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क बहुलकम्_flags, r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_PARENT)));
        प्रतिज्ञा(open_flags);
        प्रतिज्ञा(ret_file);

        बहुलकम्_flags = fopen_बहुलकम्_गन्तव्यम्_flags(open_flags);
        यदि (बहुलकम्_flags < 0)
                प्रत्यागम बहुलकम्_flags;

        fd = chase_and_openat(मूलम्_fd, dir_fd, मार्गः, chase_flags, बहुलकम्_flags, ret_मार्गः ? &final_मार्गः : रिक्त);
        यदि (fd < 0)
                प्रत्यागम fd;

        r = take_fdopen_unlocked(&fd, open_flags, ret_file);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(final_मार्गः);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_unlinkat(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags, पूर्णाङ्क unlink_flags, वर्ण **ret_मार्गः) {
        _cleanup_free_ वर्ण *p = रिक्त, *fname = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः|CHASE_PARENT|CHASE_MUST_BE_SOCKET|CHASE_MUST_BE_REGULAR|CHASE_MUST_BE_DIRECTORY|CHASE_EXTRACT_FILENAME|CHASE_MKDIR_0755)));

        fd = chase_and_openat(मूलम्_fd, dir_fd, मार्गः, chase_flags|CHASE_PARENT|CHASE_NOFOLLOW, O_मार्गः|O_DIRECTORY|O_CLOEXEC, &p);
        यदि (fd < 0)
                प्रत्यागम fd;

        r = मार्गः_extract_filename(p, &fname);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (unlinkat(fd, fname, unlink_flags) < 0)
                प्रत्यागम -errno;

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(p);

        प्रत्यागम 0;
}

पूर्णाङ्क chase_and_open_parent_at(पूर्णाङ्क मूलम्_fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ChaseFlags chase_flags, वर्ण **ret_filename) {
        पूर्णाङ्क pfd, r;

        प्रतिज्ञा(!(chase_flags & (CHASE_NONEXISTENT|CHASE_चरणः)));

        r = chaseat(मूलम्_fd, dir_fd, मार्गः, CHASE_PARENT|CHASE_EXTRACT_FILENAME|chase_flags, ret_filename, &pfd);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम pfd;
}
