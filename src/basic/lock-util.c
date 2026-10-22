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

صحیح make_تالا_file_at(صحیح dir_fd, ثابت حرف *p, صحیح operation, تالا_File *ret) {
        _cleanup_close_ صحیح fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ حرف *t = تہی;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(p);
        أكد(اندر_سیٹ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        أكد(ret);

        اگر (isempty(p))
                واپس -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_راستہ|O_DIRECTORY);
        اگر (dfd < 0)
                واپس dfd;

        t = strdup(p);
        اگر (!t)
                واپس -ENOMEM;

        fd = xopenat_تالا_بھرا(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               تالا_UNPOSIX,
                               operation);
        اگر (fd < 0)
                واپس fd == -EAGAIN ? -EBUSY : fd;

        *ret = (تالا_File) {
                .dir_fd = TAKE_FD(dfd),
                .راستہ = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        واپس 0;
}

صحیح make_تالا_file_for(ثابت حرف *p, صحیح operation, تالا_File *ret) {
        _cleanup_free_ حرف *fn = تہی, *dn = تہی, *t = تہی;
        صحیح r;

        أكد(p);
        أكد(ret);

        r = راستہ_بانٹو_سابقہ_filename(p, &dn, &fn);
        اگر (r < 0)
                واپس r;

        اگر (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        ورنہ
                t = strjoin(".#", fn, ".lck");
        اگر (!t)
                واپس -ENOMEM;

        واپس make_تالا_file(t, operation, ret);
}

خالی release_تالا_file(تالا_File *f) {
        اگر (!f)
                واپس;

        اگر (f->راستہ) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                اگر (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_تالا(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                اگر ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (خالی) unlinkat(f->dir_fd, f->راستہ, 0);

                f->راستہ = mfree(f->راستہ);
        }

        f->dir_fd = محفوظ_close(f->dir_fd);
        f->fd = محفوظ_close(f->fd);
        f->operation = 0;
}

ساکن صحیح fcntl_تالا(صحیح fd, صحیح operation, bool ofd) {
        صحیح cmd, type, r;

        أكد(fd >= 0);

        اگر (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        ورنہ
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        انتخاب (operation & ~LOCK_NB) {
                حالت LOCK_EX:
                        type = F_WRLCK;
                        توڑو;
                حالت LOCK_SH:
                        type = F_RDLCK;
                        توڑو;
                حالت LOCK_UN:
                        type = F_UNLCK;
                        توڑو;
                طےشدہ:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ساخت flock) {
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

        واپس r;
}

صحیح posix_تالا(صحیح fd, صحیح operation) {
        واپس fcntl_تالا(fd, operation, /* ofd= */ false);
}

صحیح unposix_تالا(صحیح fd, صحیح operation) {
        واپس fcntl_تالا(fd, operation, /* ofd= */ true);
}

خالی posix_unlockpp(صحیح **fd) {
        أكد(fd);

        اگر (!*fd || **fd < 0)
                واپس;

        (خالی) fcntl_تالا(**fd, LOCK_UN, /* ofd= */ false);
        *fd = تہی;
}

خالی unposix_unlockpp(صحیح **fd) {
        أكد(fd);

        اگر (!*fd || **fd < 0)
                واپس;

        (خالی) fcntl_تالا(**fd, LOCK_UN, /* ofd= */ true);
        *fd = تہی;
}

صحیح تالا_generic(صحیح fd, تالا_Type type, صحیح operation) {
        أكد(fd >= 0);

        انتخاب (type) {
        حالت تالا_NONE:
                واپس 0;
        حالت تالا_BSD:
                واپس RET_NERRNO(flock(fd, operation));
        حالت تالا_POSIX:
                واپس posix_تالا(fd, operation);
        حالت تالا_UNPOSIX:
                واپس unposix_تالا(fd, operation);
        طےشدہ:
                assert_not_reached();
        }
}

صحیح تالا_generic_with_timeout(صحیح fd, تالا_Type type, صحیح operation, usec_t timeout) {
        صحیح r;

        أكد(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        اگر (type == تالا_NONE)
                واپس 0;
        اگر (!اندر_سیٹ(type, تالا_BSD, تالا_UNPOSIX)) /* Not for POSIX locks, see above. */
                واپس -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = تالا_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        اگر (r != -EAGAIN || timeout == 0 || FLAGS_سیٹ(operation, LOCK_NB))
                واپس r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_مکمل_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_محفوظ_fork("(sd-flock)", FORK_RESET_اشارات|FORK_DEATHSIG_SIGKILL, &pidref);
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to flock block device in child process: %m");
        اگر (r == 0) {
                اگر (تالا_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_بھرا(&pidref, timeout, &status);
        اگر (r < 0)
                واپس r;

        pidref_مکمل(&pidref);

        انتخاب (status.si_code) {

        حالت CLD_EXITED:
                اگر (status.si_status != EXIT_SUCCESS)
                        واپس -EPROTO;

                واپس 0;

        حالت CLD_KILLED:
        حالت CLD_DUMPED:
                واپس -EPROTO;

        طےشدہ:
                assert_not_reached();
        }
}
