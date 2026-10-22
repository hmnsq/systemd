/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/file.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "lock-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಪೂರ್ಣಾಙ್ಕ make_lock_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ operation, LockFile *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        ಯದಿ (dfd < 0)
                ಪ್ರತ್ಯಾಗಮ dfd;

        t = strdup(p);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ make_lock_file_for(ಧ್ರುವ ವರ್ಣ *p, ಪೂರ್ಣಾಙ್ಕ operation, LockFile *ret) {
        _cleanup_free_ ವರ್ಣ *fn = ರಿಕ್ತ, *dn = ರಿಕ್ತ, *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        ಅನ್ಯಥಾ
                t = strjoin(".#", fn, ".lck");
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ make_lock_file(t, operation, ret);
}

ಶೂನ್ಯ release_lock_file(LockFile *f) {
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ;

        ಯದಿ (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ಯದಿ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ಯದಿ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (ಶೂನ್ಯ) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fcntl_lock(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ operation, bool ofd) {
        ಪೂರ್ಣಾಙ್ಕ cmd, type, r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        ಅನ್ಯಥಾ
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        ವಿಕಲ್ಪ (operation & ~LOCK_NB) {
                ಪಕ್ಷ LOCK_EX:
                        type = F_WRLCK;
                        ಭಙ್ಗ;
                ಪಕ್ಷ LOCK_SH:
                        type = F_RDLCK;
                        ಭಙ್ಗ;
                ಪಕ್ಷ LOCK_UN:
                        type = F_UNLCK;
                        ಭಙ್ಗ;
                ಸ್ವಭಾವ:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ಸಂರಚನಾ flock) {
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
        ಯದಿ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ posix_lock(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ operation) {
        ಪ್ರತ್ಯಾಗಮ fcntl_lock(fd, operation, /* ofd= */ false);
}

ಪೂರ್ಣಾಙ್ಕ unposix_lock(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ operation) {
        ಪ್ರತ್ಯಾಗಮ fcntl_lock(fd, operation, /* ofd= */ true);
}

ಶೂನ್ಯ posix_unlockpp(ಪೂರ್ಣಾಙ್ಕ **fd) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        ಯದಿ (!*fd || **fd < 0)
                ಪ್ರತ್ಯಾಗಮ;

        (ಶೂನ್ಯ) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ರಿಕ್ತ;
}

ಶೂನ್ಯ unposix_unlockpp(ಪೂರ್ಣಾಙ್ಕ **fd) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        ಯದಿ (!*fd || **fd < 0)
                ಪ್ರತ್ಯಾಗಮ;

        (ಶೂನ್ಯ) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ lock_generic(ಪೂರ್ಣಾಙ್ಕ fd, LockType type, ಪೂರ್ಣಾಙ್ಕ operation) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ LOCK_NONE:
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪಕ್ಷ LOCK_BSD:
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(flock(fd, operation));
        ಪಕ್ಷ LOCK_POSIX:
                ಪ್ರತ್ಯಾಗಮ posix_lock(fd, operation);
        ಪಕ್ಷ LOCK_UNPOSIX:
                ಪ್ರತ್ಯಾಗಮ unposix_lock(fd, operation);
        ಸ್ವಭಾವ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಙ್ಕ lock_generic_with_timeout(ಪೂರ್ಣಾಙ್ಕ fd, LockType type, ಪೂರ್ಣಾಙ್ಕ operation, usec_t timeout) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ಯದಿ (type == LOCK_NONE)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ಯದಿ (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                ಪ್ರತ್ಯಾಗಮ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "Failed to flock block device in child process: %m");
        ಯದಿ (r == 0) {
                ಯದಿ (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        pidref_done(&pidref);

        ವಿಕಲ್ಪ (status.si_code) {

        ಪಕ್ಷ CLD_EXITED:
                ಯದಿ (status.si_status != EXIT_SUCCESS)
                        ಪ್ರತ್ಯಾಗಮ -EPROTO;

                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ CLD_KILLED:
        ಪಕ್ಷ CLD_DUMPED:
                ಪ್ರತ್ಯಾಗಮ -EPROTO;

        ಸ್ವಭಾವ:
                assert_not_reached();
        }
}
