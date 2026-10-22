/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <sys/file.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "lock-util.h"
#شامل "log.h"
#شامل "path-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "string-util.h"
#شامل "time-util.h"

صحیح make_lock_file_at(صحیح dir_fd, ثابت نویسه *p, صحیح operation, LockFile *ret) {
        _cleanup_close_ صحیح fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ نویسه *t = پوچ;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(p);
        أكد(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        أكد(ret);

        اگر (isempty(p))
                بازگشت -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        اگر (dfd < 0)
                بازگشت dfd;

        t = strdup(p);
        اگر (!t)
                بازگشت -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        اگر (fd < 0)
                بازگشت fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        بازگشت 0;
}

صحیح make_lock_file_for(ثابت نویسه *p, صحیح operation, LockFile *ret) {
        _cleanup_free_ نویسه *fn = پوچ, *dn = پوچ, *t = پوچ;
        صحیح r;

        أكد(p);
        أكد(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        اگر (r < 0)
                بازگشت r;

        اگر (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        وگرنه
                t = strjoin(".#", fn, ".lck");
        اگر (!t)
                بازگشت -ENOMEM;

        بازگشت make_lock_file(t, operation, ret);
}

تهی release_lock_file(LockFile *f) {
        اگر (!f)
                بازگشت;

        اگر (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                اگر (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                اگر ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (تهی) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

ایستا صحیح fcntl_lock(صحیح fd, صحیح operation, bool ofd) {
        صحیح cmd, type, r;

        أكد(fd >= 0);

        اگر (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        وگرنه
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        انتخاب (operation & ~LOCK_NB) {
                حالت LOCK_EX:
                        type = F_WRLCK;
                        بشکن;
                حالت LOCK_SH:
                        type = F_RDLCK;
                        بشکن;
                حالت LOCK_UN:
                        type = F_UNLCK;
                        بشکن;
                پیشفرض:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ساختار flock) {
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
        اگر ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        بازگشت r;
}

صحیح posix_lock(صحیح fd, صحیح operation) {
        بازگشت fcntl_lock(fd, operation, /* ofd= */ false);
}

صحیح unposix_lock(صحیح fd, صحیح operation) {
        بازگشت fcntl_lock(fd, operation, /* ofd= */ true);
}

تهی posix_unlockpp(صحیح **fd) {
        أكد(fd);

        اگر (!*fd || **fd < 0)
                بازگشت;

        (تهی) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = پوچ;
}

تهی unposix_unlockpp(صحیح **fd) {
        أكد(fd);

        اگر (!*fd || **fd < 0)
                بازگشت;

        (تهی) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = پوچ;
}

صحیح lock_generic(صحیح fd, LockType type, صحیح operation) {
        أكد(fd >= 0);

        انتخاب (type) {
        حالت LOCK_NONE:
                بازگشت 0;
        حالت LOCK_BSD:
                بازگشت RET_NERRNO(flock(fd, operation));
        حالت LOCK_POSIX:
                بازگشت posix_lock(fd, operation);
        حالت LOCK_UNPOSIX:
                بازگشت unposix_lock(fd, operation);
        پیشفرض:
                assert_not_reached();
        }
}

صحیح lock_generic_with_timeout(صحیح fd, LockType type, صحیح operation, usec_t timeout) {
        صحیح r;

        أكد(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        اگر (type == LOCK_NONE)
                بازگشت 0;
        اگر (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                بازگشت -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        اگر (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                بازگشت r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        اگر (r < 0)
                بازگشت log_error_errno(r, "Failed to flock block device in child process: %m");
        اگر (r == 0) {
                اگر (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        اگر (r < 0)
                بازگشت r;

        pidref_done(&pidref);

        انتخاب (status.si_code) {

        حالت CLD_EXITED:
                اگر (status.si_status != EXIT_SUCCESS)
                        بازگشت -EPROTO;

                بازگشت 0;

        حالت CLD_KILLED:
        حالت CLD_DUMPED:
                بازگشت -EPROTO;

        پیشفرض:
                assert_not_reached();
        }
}
