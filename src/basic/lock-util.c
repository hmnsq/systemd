/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <sys/file.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "lock-util.h"
#ضمن "log.h"
#ضمن "path-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "string-util.h"
#ضمن "time-util.h"

صحيح make_lock_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, LockFile *ret) {
        _cleanup_close_ صحيح fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ حرف *t = عدم;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(p);
        أكد(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        أكد(ret);

        𐤀𐤌 (isempty(p))
                أرجع -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        𐤀𐤌 (dfd < 0)
                أرجع dfd;

        t = strdup(p);
        𐤀𐤌 (!t)
                أرجع -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        𐤀𐤌 (fd < 0)
                أرجع fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        أرجع 0;
}

صحيح make_lock_file_for(ثابت حرف *p, صحيح operation, LockFile *ret) {
        _cleanup_free_ حرف *fn = عدم, *dn = عدم, *t = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        وإلا
                t = strjoin(".#", fn, ".lck");
        𐤀𐤌 (!t)
                أرجع -ENOMEM;

        أرجع make_lock_file(t, operation, ret);
}

فراغ release_lock_file(LockFile *f) {
        𐤀𐤌 (!f)
                أرجع;

        𐤀𐤌 (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                𐤀𐤌 (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                𐤀𐤌 ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (فراغ) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

ساكن صحيح fcntl_lock(صحيح fd, صحيح operation, bool ofd) {
        صحيح cmd, type, r;

        أكد(fd >= 0);

        𐤀𐤌 (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        وإلا
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        بدل (operation & ~LOCK_NB) {
                حالة LOCK_EX:
                        type = F_WRLCK;
                        اكسر;
                حالة LOCK_SH:
                        type = F_RDLCK;
                        اكسر;
                حالة LOCK_UN:
                        type = F_UNLCK;
                        اكسر;
                افتراضي:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(بنية flock) {
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
        𐤀𐤌 ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        أرجع r;
}

صحيح posix_lock(صحيح fd, صحيح operation) {
        أرجع fcntl_lock(fd, operation, /* ofd= */ false);
}

صحيح unposix_lock(صحيح fd, صحيح operation) {
        أرجع fcntl_lock(fd, operation, /* ofd= */ true);
}

فراغ posix_unlockpp(صحيح **fd) {
        أكد(fd);

        𐤀𐤌 (!*fd || **fd < 0)
                أرجع;

        (فراغ) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = عدم;
}

فراغ unposix_unlockpp(صحيح **fd) {
        أكد(fd);

        𐤀𐤌 (!*fd || **fd < 0)
                أرجع;

        (فراغ) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = عدم;
}

صحيح lock_generic(صحيح fd, LockType type, صحيح operation) {
        أكد(fd >= 0);

        بدل (type) {
        حالة LOCK_NONE:
                أرجع 0;
        حالة LOCK_BSD:
                أرجع RET_NERRNO(flock(fd, operation));
        حالة LOCK_POSIX:
                أرجع posix_lock(fd, operation);
        حالة LOCK_UNPOSIX:
                أرجع unposix_lock(fd, operation);
        افتراضي:
                assert_not_reached();
        }
}

صحيح lock_generic_with_timeout(صحيح fd, LockType type, صحيح operation, usec_t timeout) {
        صحيح r;

        أكد(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        𐤀𐤌 (type == LOCK_NONE)
                أرجع 0;
        𐤀𐤌 (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                أرجع -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        𐤀𐤌 (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                أرجع r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        𐤀𐤌 (r < 0)
                أرجع log_error_errno(r, "Failed to flock block device in child process: %m");
        𐤀𐤌 (r == 0) {
                𐤀𐤌 (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        𐤀𐤌 (r < 0)
                أرجع r;

        pidref_done(&pidref);

        بدل (status.si_code) {

        حالة CLD_EXITED:
                𐤀𐤌 (status.si_status != EXIT_SUCCESS)
                        أرجع -EPROTO;

                أرجع 0;

        حالة CLD_KILLED:
        حالة CLD_DUMPED:
                أرجع -EPROTO;

        افتراضي:
                assert_not_reached();
        }
}
