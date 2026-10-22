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

ಪೂರ್ಣಾಂಕ make_ಬೀಗ_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ operation, ಬೀಗ_File *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_ಮಾರ್ಗ|O_DIRECTORY);
        ಆದರೆ (dfd < 0)
                ಹಿಂತಿರುಗಿ dfd;

        t = strdup(p);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        fd = xopenat_ಬೀಗ_ತುಂಬಿದ(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               ಬೀಗ_UNPOSIX,
                               operation);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (ಬೀಗ_File) {
                .dir_fd = TAKE_FD(dfd),
                .ಮಾರ್ಗ = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ make_ಬೀಗ_file_for(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಪೂರ್ಣಾಂಕ operation, ಬೀಗ_File *ret) {
        _cleanup_free_ ಅಕ್ಷರ *fn = ಖಾಲಿ, *dn = ಖಾಲಿ, *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(p, &dn, &fn);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        ಇಲ್ಲದಿದ್ದರೆ
                t = strjoin(".#", fn, ".lck");
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ make_ಬೀಗ_file(t, operation, ret);
}

ಶೂನ್ಯ release_ಬೀಗ_file(ಬೀಗ_File *f) {
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ;

        ಆದರೆ (f->ಮಾರ್ಗ) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ಆದರೆ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_ಬೀಗ(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ಆದರೆ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (ಶೂನ್ಯ) unlinkat(f->dir_fd, f->ಮಾರ್ಗ, 0);

                f->ಮಾರ್ಗ = mfree(f->ಮಾರ್ಗ);
        }

        f->dir_fd = ಸುರಕ್ಷಿತ_close(f->dir_fd);
        f->fd = ಸುರಕ್ಷಿತ_close(f->fd);
        f->operation = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fcntl_ಬೀಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ operation, bool ofd) {
        ಪೂರ್ಣಾಂಕ cmd, type, r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        ಇಲ್ಲದಿದ್ದರೆ
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        ಆಯ್ಕೆ (operation & ~LOCK_NB) {
                ಸ್ಥಿತಿ LOCK_EX:
                        type = F_WRLCK;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ LOCK_SH:
                        type = F_RDLCK;
                        ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ LOCK_UN:
                        type = F_UNLCK;
                        ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ರಚನೆ flock) {
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
        ಆದರೆ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ posix_ಬೀಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ operation) {
        ಹಿಂತಿರುಗಿ fcntl_ಬೀಗ(fd, operation, /* ofd= */ false);
}

ಪೂರ್ಣಾಂಕ unposix_ಬೀಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ operation) {
        ಹಿಂತಿರುಗಿ fcntl_ಬೀಗ(fd, operation, /* ofd= */ true);
}

ಶೂನ್ಯ posix_unlockpp(ಪೂರ್ಣಾಂಕ **fd) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        ಆದರೆ (!*fd || **fd < 0)
                ಹಿಂತಿರುಗಿ;

        (ಶೂನ್ಯ) fcntl_ಬೀಗ(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ಖಾಲಿ;
}

ಶೂನ್ಯ unposix_unlockpp(ಪೂರ್ಣಾಂಕ **fd) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        ಆದರೆ (!*fd || **fd < 0)
                ಹಿಂತಿರುಗಿ;

        (ಶೂನ್ಯ) fcntl_ಬೀಗ(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ ಬೀಗ_generic(ಪೂರ್ಣಾಂಕ fd, ಬೀಗ_Type type, ಪೂರ್ಣಾಂಕ operation) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ ಬೀಗ_NONE:
                ಹಿಂತಿರುಗಿ 0;
        ಸ್ಥಿತಿ ಬೀಗ_BSD:
                ಹಿಂತಿರುಗಿ RET_NERRNO(flock(fd, operation));
        ಸ್ಥಿತಿ ಬೀಗ_POSIX:
                ಹಿಂತಿರುಗಿ posix_ಬೀಗ(fd, operation);
        ಸ್ಥಿತಿ ಬೀಗ_UNPOSIX:
                ಹಿಂತಿರುಗಿ unposix_ಬೀಗ(fd, operation);
        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಂಕ ಬೀಗ_generic_with_timeout(ಪೂರ್ಣಾಂಕ fd, ಬೀಗ_Type type, ಪೂರ್ಣಾಂಕ operation, usec_t timeout) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ಆದರೆ (type == ಬೀಗ_NONE)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (!ಒಳ_ಗಣ(type, ಬೀಗ_BSD, ಬೀಗ_UNPOSIX)) /* Not for POSIX locks, see above. */
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = ಬೀಗ_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ಆದರೆ (r != -EAGAIN || timeout == 0 || FLAGS_ಗಣ(operation, LOCK_NB))
                ಹಿಂತಿರುಗಿ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_ಸುರಕ್ಷಿತ_fork("(sd-flock)", FORK_RESET_ಸಂಕೇತಗಳು|FORK_DEATHSIG_SIGKILL, &pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to flock block device in child process: %m");
        ಆದರೆ (r == 0) {
                ಆದರೆ (ಬೀಗ_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_ತುಂಬಿದ(&pidref, timeout, &status);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        pidref_ಮುಗಿದ(&pidref);

        ಆಯ್ಕೆ (status.si_code) {

        ಸ್ಥಿತಿ CLD_EXITED:
                ಆದರೆ (status.si_status != EXIT_SUCCESS)
                        ಹಿಂತಿರುಗಿ -EPROTO;

                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ CLD_KILLED:
        ಸ್ಥಿತಿ CLD_DUMPED:
                ಹಿಂತಿರುಗಿ -EPROTO;

        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }
}
