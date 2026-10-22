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

صحيح make_تالو_file_at(صحيح dir_fd, ثابت حرف *p, صحيح operation, تالو_File *ret) {
        _cleanup_close_ صحيح fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ حرف *t = عدم;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(p);
        أكد(اندريون_سيٽ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        أكد(ret);

        إذا (isempty(p))
                أرجع -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_رستو|O_DIRECTORY);
        إذا (dfd < 0)
                أرجع dfd;

        t = strdup(p);
        إذا (!t)
                أرجع -ENOMEM;

        fd = xopenat_تالو_ڀريل(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               تالو_UNPOSIX,
                               operation);
        إذا (fd < 0)
                أرجع fd == -EAGAIN ? -EBUSY : fd;

        *ret = (تالو_File) {
                .dir_fd = TAKE_FD(dfd),
                .رستو = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        أرجع 0;
}

صحيح make_تالو_file_for(ثابت حرف *p, صحيح operation, تالو_File *ret) {
        _cleanup_free_ حرف *fn = عدم, *dn = عدم, *t = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        r = رستو_ورهايو_اڳياڙي_filename(p, &dn, &fn);
        إذا (r < 0)
                أرجع r;

        إذا (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        وإلا
                t = strjoin(".#", fn, ".lck");
        إذا (!t)
                أرجع -ENOMEM;

        أرجع make_تالو_file(t, operation, ret);
}

فراغ release_تالو_file(تالو_File *f) {
        إذا (!f)
                أرجع;

        إذا (f->رستو) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                إذا (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_تالو(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                إذا ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (فراغ) unlinkat(f->dir_fd, f->رستو, 0);

                f->رستو = mfree(f->رستو);
        }

        f->dir_fd = محفوظ_close(f->dir_fd);
        f->fd = محفوظ_close(f->fd);
        f->operation = 0;
}

ساكن صحيح fcntl_تالو(صحيح fd, صحيح operation, bool ofd) {
        صحيح cmd, type, r;

        أكد(fd >= 0);

        إذا (ofd)
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
        إذا ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        أرجع r;
}

صحيح posix_تالو(صحيح fd, صحيح operation) {
        أرجع fcntl_تالو(fd, operation, /* ofd= */ false);
}

صحيح unposix_تالو(صحيح fd, صحيح operation) {
        أرجع fcntl_تالو(fd, operation, /* ofd= */ true);
}

فراغ posix_unlockpp(صحيح **fd) {
        أكد(fd);

        إذا (!*fd || **fd < 0)
                أرجع;

        (فراغ) fcntl_تالو(**fd, LOCK_UN, /* ofd= */ false);
        *fd = عدم;
}

فراغ unposix_unlockpp(صحيح **fd) {
        أكد(fd);

        إذا (!*fd || **fd < 0)
                أرجع;

        (فراغ) fcntl_تالو(**fd, LOCK_UN, /* ofd= */ true);
        *fd = عدم;
}

صحيح تالو_generic(صحيح fd, تالو_Type type, صحيح operation) {
        أكد(fd >= 0);

        بدل (type) {
        حالة تالو_NONE:
                أرجع 0;
        حالة تالو_BSD:
                أرجع RET_NERRNO(flock(fd, operation));
        حالة تالو_POSIX:
                أرجع posix_تالو(fd, operation);
        حالة تالو_UNPOSIX:
                أرجع unposix_تالو(fd, operation);
        افتراضي:
                assert_not_reached();
        }
}

صحيح تالو_generic_with_timeout(صحيح fd, تالو_Type type, صحيح operation, usec_t timeout) {
        صحيح r;

        أكد(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        إذا (type == تالو_NONE)
                أرجع 0;
        إذا (!اندريون_سيٽ(type, تالو_BSD, تالو_UNPOSIX)) /* Not for POSIX locks, see above. */
                أرجع -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = تالو_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        إذا (r != -EAGAIN || timeout == 0 || FLAGS_سيٽ(operation, LOCK_NB))
                أرجع r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_محفوظ_fork("(sd-flock)", FORK_RESET_سگنل|FORK_DEATHSIG_SIGKILL, &pidref);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to flock block device in child process: %m");
        إذا (r == 0) {
                إذا (تالو_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_ڀريل(&pidref, timeout, &status);
        إذا (r < 0)
                أرجع r;

        pidref_پورو(&pidref);

        بدل (status.si_code) {

        حالة CLD_EXITED:
                إذا (status.si_status != EXIT_SUCCESS)
                        أرجع -EPROTO;

                أرجع 0;

        حالة CLD_KILLED:
        حالة CLD_DUMPED:
                أرجع -EPROTO;

        افتراضي:
                assert_not_reached();
        }
}
