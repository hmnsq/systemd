/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/file.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "lock-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "time-util.h"

পূর্ণসংখ্যা make_lock_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা operation, LockFile *ret) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ অক্ষর *t = ফাঁকা;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        প্রতিজ্ঞা(ret);

        যদি (isempty(p))
                ফেরত -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        যদি (dfd < 0)
                ফেরত dfd;

        t = strdup(p);
        যদি (!t)
                ফেরত -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        যদি (fd < 0)
                ফেরত fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ফেরত 0;
}

পূর্ণসংখ্যা make_lock_file_for(ধ্রুবক অক্ষর *p, পূর্ণসংখ্যা operation, LockFile *ret) {
        _cleanup_free_ অক্ষর *fn = ফাঁকা, *dn = ফাঁকা, *t = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        যদি (r < 0)
                ফেরত r;

        যদি (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        অন্যথা
                t = strjoin(".#", fn, ".lck");
        যদি (!t)
                ফেরত -ENOMEM;

        ফেরত make_lock_file(t, operation, ret);
}

শূন্য release_lock_file(LockFile *f) {
        যদি (!f)
                ফেরত;

        যদি (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                যদি (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                যদি ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (শূন্য) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

স্থির পূর্ণসংখ্যা fcntl_lock(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা operation, bool ofd) {
        পূর্ণসংখ্যা cmd, type, r;

        প্রতিজ্ঞা(fd >= 0);

        যদি (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        অন্যথা
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        নির্বাচন (operation & ~LOCK_NB) {
                ক্ষেত্র LOCK_EX:
                        type = F_WRLCK;
                        বিরতি;
                ক্ষেত্র LOCK_SH:
                        type = F_RDLCK;
                        বিরতি;
                ক্ষেত্র LOCK_UN:
                        type = F_UNLCK;
                        বিরতি;
                পূর্বনির্ধারিত:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(কাঠামো flock) {
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
        যদি ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ফেরত r;
}

পূর্ণসংখ্যা posix_lock(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা operation) {
        ফেরত fcntl_lock(fd, operation, /* ofd= */ false);
}

পূর্ণসংখ্যা unposix_lock(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা operation) {
        ফেরত fcntl_lock(fd, operation, /* ofd= */ true);
}

শূন্য posix_unlockpp(পূর্ণসংখ্যা **fd) {
        প্রতিজ্ঞা(fd);

        যদি (!*fd || **fd < 0)
                ফেরত;

        (শূন্য) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ফাঁকা;
}

শূন্য unposix_unlockpp(পূর্ণসংখ্যা **fd) {
        প্রতিজ্ঞা(fd);

        যদি (!*fd || **fd < 0)
                ফেরত;

        (শূন্য) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ফাঁকা;
}

পূর্ণসংখ্যা lock_generic(পূর্ণসংখ্যা fd, LockType type, পূর্ণসংখ্যা operation) {
        প্রতিজ্ঞা(fd >= 0);

        নির্বাচন (type) {
        ক্ষেত্র LOCK_NONE:
                ফেরত 0;
        ক্ষেত্র LOCK_BSD:
                ফেরত RET_NERRNO(flock(fd, operation));
        ক্ষেত্র LOCK_POSIX:
                ফেরত posix_lock(fd, operation);
        ক্ষেত্র LOCK_UNPOSIX:
                ফেরত unposix_lock(fd, operation);
        পূর্বনির্ধারিত:
                assert_not_reached();
        }
}

পূর্ণসংখ্যা lock_generic_with_timeout(পূর্ণসংখ্যা fd, LockType type, পূর্ণসংখ্যা operation, usec_t timeout) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        যদি (type == LOCK_NONE)
                ফেরত 0;
        যদি (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                ফেরত -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        যদি (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                ফেরত r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        যদি (r < 0)
                ফেরত log_error_errno(r, "Failed to flock block device in child process: %m");
        যদি (r == 0) {
                যদি (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        যদি (r < 0)
                ফেরত r;

        pidref_done(&pidref);

        নির্বাচন (status.si_code) {

        ক্ষেত্র CLD_EXITED:
                যদি (status.si_status != EXIT_SUCCESS)
                        ফেরত -EPROTO;

                ফেরত 0;

        ক্ষেত্র CLD_KILLED:
        ক্ষেত্র CLD_DUMPED:
                ফেরত -EPROTO;

        পূর্বনির্ধারিত:
                assert_not_reached();
        }
}
