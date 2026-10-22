/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/file.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "lock-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "time-util.h"

പൂർണ്ണസംഖ്യ make_lock_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ operation, LockFile *ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (isempty(p))
                തിരികെ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        എങ്കിൽ (dfd < 0)
                തിരികെ dfd;

        t = strdup(p);
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        എങ്കിൽ (fd < 0)
                തിരികെ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ make_lock_file_for(സ്ഥിരം അക്ഷരം *p, പൂർണ്ണസംഖ്യ operation, LockFile *ret) {
        _cleanup_free_ അക്ഷരം *fn = ശൂന്യ, *dn = ശൂന്യ, *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        അല്ലെങ്കിൽ
                t = strjoin(".#", fn, ".lck");
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        തിരികെ make_lock_file(t, operation, ret);
}

ശൂന്യം release_lock_file(LockFile *f) {
        എങ്കിൽ (!f)
                തിരികെ;

        എങ്കിൽ (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                എങ്കിൽ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                എങ്കിൽ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (ശൂന്യം) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fcntl_lock(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ operation, bool ofd) {
        പൂർണ്ണസംഖ്യ cmd, type, r;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        അല്ലെങ്കിൽ
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        തിരഞ്ഞെടുപ്പ് (operation & ~LOCK_NB) {
                അവസ്ഥ LOCK_EX:
                        type = F_WRLCK;
                        നിർത്തുക;
                അവസ്ഥ LOCK_SH:
                        type = F_RDLCK;
                        നിർത്തുക;
                അവസ്ഥ LOCK_UN:
                        type = F_UNLCK;
                        നിർത്തുക;
                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ഘടന flock) {
                .l_type = type,
                .l_whence = SEEK_SET,
                .l_start = 0,
                .l_len = 0,
        }));

        /* If we are doing non-blocking operations, treat EACCES/EAGAIN the same as per man page. But if
         * not, propagate EACCES back, as it will likely be due to an LSM denying the operation (for example
         * LXC with AppArmor when running on kernel < 6.2), and in some cases we want to gracefully
         * fallback (e.g.: PrivateNetwork=yes). As per documentation, it's only the non-blocking operation
         * F_SETLK that might return EACCES on some platforms (although the Linux implementation doesn't
         * seem to), as F_SETLKW and F_OFD_SETLKW block so this is not an issue, and F_OFD_SETLK is documented
         * to only return EAGAIN if the lock is already held. */
        എങ്കിൽ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ posix_lock(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ operation) {
        തിരികെ fcntl_lock(fd, operation, /* ofd= */ false);
}

പൂർണ്ണസംഖ്യ unposix_lock(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ operation) {
        തിരികെ fcntl_lock(fd, operation, /* ofd= */ true);
}

ശൂന്യം posix_unlockpp(പൂർണ്ണസംഖ്യ **fd) {
        പ്രതിജ്ഞാ(fd);

        എങ്കിൽ (!*fd || **fd < 0)
                തിരികെ;

        (ശൂന്യം) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ശൂന്യ;
}

ശൂന്യം unposix_unlockpp(പൂർണ്ണസംഖ്യ **fd) {
        പ്രതിജ്ഞാ(fd);

        എങ്കിൽ (!*fd || **fd < 0)
                തിരികെ;

        (ശൂന്യം) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ശൂന്യ;
}

പൂർണ്ണസംഖ്യ lock_generic(പൂർണ്ണസംഖ്യ fd, LockType type, പൂർണ്ണസംഖ്യ operation) {
        പ്രതിജ്ഞാ(fd >= 0);

        തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ LOCK_NONE:
                തിരികെ 0;
        അവസ്ഥ LOCK_BSD:
                തിരികെ RET_NERRNO(flock(fd, operation));
        അവസ്ഥ LOCK_POSIX:
                തിരികെ posix_lock(fd, operation);
        അവസ്ഥ LOCK_UNPOSIX:
                തിരികെ unposix_lock(fd, operation);
        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }
}

പൂർണ്ണസംഖ്യ lock_generic_with_timeout(പൂർണ്ണസംഖ്യ fd, LockType type, പൂർണ്ണസംഖ്യ operation, usec_t timeout) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        എങ്കിൽ (type == LOCK_NONE)
                തിരികെ 0;
        എങ്കിൽ (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                തിരികെ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        എങ്കിൽ (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                തിരികെ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        എങ്കിൽ (r < 0)
                തിരികെ log_error_errno(r, "Failed to flock block device in child process: %m");
        എങ്കിൽ (r == 0) {
                എങ്കിൽ (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        pidref_done(&pidref);

        തിരഞ്ഞെടുപ്പ് (status.si_code) {

        അവസ്ഥ CLD_EXITED:
                എങ്കിൽ (status.si_status != EXIT_SUCCESS)
                        തിരികെ -EPROTO;

                തിരികെ 0;

        അവസ്ഥ CLD_KILLED:
        അവസ്ഥ CLD_DUMPED:
                തിരികെ -EPROTO;

        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }
}
