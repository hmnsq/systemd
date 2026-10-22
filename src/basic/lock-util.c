/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/file.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lock-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

ਪੂਰਨ make_ਤਾਲਾ_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ operation, ਤਾਲਾ_File *ret) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (isempty(p))
                ਵਾਪਸ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_ਰਸਤਾ|O_DIRECTORY);
        ਜੇ (dfd < 0)
                ਵਾਪਸ dfd;

        t = strdup(p);
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        fd = xopenat_ਤਾਲਾ_ਭਰਿਆ(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               ਤਾਲਾ_UNPOSIX,
                               operation);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (ਤਾਲਾ_File) {
                .dir_fd = TAKE_FD(dfd),
                .ਰਸਤਾ = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ਵਾਪਸ 0;
}

ਪੂਰਨ make_ਤਾਲਾ_file_for(ਅਚਲ ਅੱਖਰ *p, ਪੂਰਨ operation, ਤਾਲਾ_File *ret) {
        _cleanup_free_ ਅੱਖਰ *fn = ਨਿਰਾ, *dn = ਨਿਰਾ, *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(p, &dn, &fn);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        ਨਹੀਂਤਾਂ
                t = strjoin(".#", fn, ".lck");
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ make_ਤਾਲਾ_file(t, operation, ret);
}

ਖਾਲੀ release_ਤਾਲਾ_file(ਤਾਲਾ_File *f) {
        ਜੇ (!f)
                ਵਾਪਸ;

        ਜੇ (f->ਰਸਤਾ) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ਜੇ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_ਤਾਲਾ(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ਜੇ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (ਖਾਲੀ) unlinkat(f->dir_fd, f->ਰਸਤਾ, 0);

                f->ਰਸਤਾ = mfree(f->ਰਸਤਾ);
        }

        f->dir_fd = ਸੁਰੱਖਿਅਤ_close(f->dir_fd);
        f->fd = ਸੁਰੱਖਿਅਤ_close(f->fd);
        f->operation = 0;
}

ਸਥਿਰ ਪੂਰਨ fcntl_ਤਾਲਾ(ਪੂਰਨ fd, ਪੂਰਨ operation, bool ofd) {
        ਪੂਰਨ cmd, type, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        ਨਹੀਂਤਾਂ
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        ਚੋਣ (operation & ~LOCK_NB) {
                ਹਾਲਤ LOCK_EX:
                        type = F_WRLCK;
                        ਰੋਕੋ;
                ਹਾਲਤ LOCK_SH:
                        type = F_RDLCK;
                        ਰੋਕੋ;
                ਹਾਲਤ LOCK_UN:
                        type = F_UNLCK;
                        ਰੋਕੋ;
                ਮੂਲ:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ਬਣਤਰ flock) {
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
        ਜੇ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ਵਾਪਸ r;
}

ਪੂਰਨ posix_ਤਾਲਾ(ਪੂਰਨ fd, ਪੂਰਨ operation) {
        ਵਾਪਸ fcntl_ਤਾਲਾ(fd, operation, /* ofd= */ false);
}

ਪੂਰਨ unposix_ਤਾਲਾ(ਪੂਰਨ fd, ਪੂਰਨ operation) {
        ਵਾਪਸ fcntl_ਤਾਲਾ(fd, operation, /* ofd= */ true);
}

ਖਾਲੀ posix_unlockpp(ਪੂਰਨ **fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd);

        ਜੇ (!*fd || **fd < 0)
                ਵਾਪਸ;

        (ਖਾਲੀ) fcntl_ਤਾਲਾ(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ਨਿਰਾ;
}

ਖਾਲੀ unposix_unlockpp(ਪੂਰਨ **fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd);

        ਜੇ (!*fd || **fd < 0)
                ਵਾਪਸ;

        (ਖਾਲੀ) fcntl_ਤਾਲਾ(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ਨਿਰਾ;
}

ਪੂਰਨ ਤਾਲਾ_generic(ਪੂਰਨ fd, ਤਾਲਾ_Type type, ਪੂਰਨ operation) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਚੋਣ (type) {
        ਹਾਲਤ ਤਾਲਾ_NONE:
                ਵਾਪਸ 0;
        ਹਾਲਤ ਤਾਲਾ_BSD:
                ਵਾਪਸ RET_NERRNO(flock(fd, operation));
        ਹਾਲਤ ਤਾਲਾ_POSIX:
                ਵਾਪਸ posix_ਤਾਲਾ(fd, operation);
        ਹਾਲਤ ਤਾਲਾ_UNPOSIX:
                ਵਾਪਸ unposix_ਤਾਲਾ(fd, operation);
        ਮੂਲ:
                assert_not_reached();
        }
}

ਪੂਰਨ ਤਾਲਾ_generic_with_timeout(ਪੂਰਨ fd, ਤਾਲਾ_Type type, ਪੂਰਨ operation, usec_t timeout) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ਜੇ (type == ਤਾਲਾ_NONE)
                ਵਾਪਸ 0;
        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(type, ਤਾਲਾ_BSD, ਤਾਲਾ_UNPOSIX)) /* Not for POSIX locks, see above. */
                ਵਾਪਸ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = ਤਾਲਾ_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ਜੇ (r != -EAGAIN || timeout == 0 || FLAGS_ਸੈੱਟ(operation, LOCK_NB))
                ਵਾਪਸ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_ਸੁਰੱਖਿਅਤ_fork("(sd-flock)", FORK_RESET_ਸੰਕੇਤ|FORK_DEATHSIG_SIGKILL, &pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to flock block device in child process: %m");
        ਜੇ (r == 0) {
                ਜੇ (ਤਾਲਾ_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_ਭਰਿਆ(&pidref, timeout, &status);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        pidref_ਪੂਰਾ(&pidref);

        ਚੋਣ (status.si_code) {

        ਹਾਲਤ CLD_EXITED:
                ਜੇ (status.si_status != EXIT_SUCCESS)
                        ਵਾਪਸ -EPROTO;

                ਵਾਪਸ 0;

        ਹਾਲਤ CLD_KILLED:
        ਹਾਲਤ CLD_DUMPED:
                ਵਾਪਸ -EPROTO;

        ਮੂਲ:
                assert_not_reached();
        }
}
