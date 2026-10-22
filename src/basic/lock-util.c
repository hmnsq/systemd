/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "lock-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "time-util.h"

पूर्णाङ्क make_lock_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *p, पूर्णाङ्क operation, LockFile *ret) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ वर्ण *t = रिक्त;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(p);
        प्रतिज्ञा(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        प्रतिज्ञा(ret);

        यदि (isempty(p))
                फर्काऊ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        यदि (dfd < 0)
                फर्काऊ dfd;

        t = strdup(p);
        यदि (!t)
                फर्काऊ -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        यदि (fd < 0)
                फर्काऊ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        फर्काऊ 0;
}

पूर्णाङ्क make_lock_file_for(अचर वर्ण *p, पूर्णाङ्क operation, LockFile *ret) {
        _cleanup_free_ वर्ण *fn = रिक्त, *dn = रिक्त, *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        यदि (r < 0)
                फर्काऊ r;

        यदि (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        अन्यथा
                t = strjoin(".#", fn, ".lck");
        यदि (!t)
                फर्काऊ -ENOMEM;

        फर्काऊ make_lock_file(t, operation, ret);
}

शून्य release_lock_file(LockFile *f) {
        यदि (!f)
                फर्काऊ;

        यदि (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                यदि (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                यदि ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (शून्य) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

स्थिर पूर्णाङ्क fcntl_lock(पूर्णाङ्क fd, पूर्णाङ्क operation, bool ofd) {
        पूर्णाङ्क cmd, type, r;

        प्रतिज्ञा(fd >= 0);

        यदि (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        अन्यथा
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        विकल्प (operation & ~LOCK_NB) {
                पक्ष LOCK_EX:
                        type = F_WRLCK;
                        रोक;
                पक्ष LOCK_SH:
                        type = F_RDLCK;
                        रोक;
                पक्ष LOCK_UN:
                        type = F_UNLCK;
                        रोक;
                स्वभाव:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(संरचना flock) {
                .l_type = type,
                .l_whence = शुरू_से,
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
        यदि ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        फर्काऊ r;
}

पूर्णाङ्क posix_lock(पूर्णाङ्क fd, पूर्णाङ्क operation) {
        फर्काऊ fcntl_lock(fd, operation, /* ofd= */ false);
}

पूर्णाङ्क unposix_lock(पूर्णाङ्क fd, पूर्णाङ्क operation) {
        फर्काऊ fcntl_lock(fd, operation, /* ofd= */ true);
}

शून्य posix_unlockpp(पूर्णाङ्क **fd) {
        प्रतिज्ञा(fd);

        यदि (!*fd || **fd < 0)
                फर्काऊ;

        (शून्य) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = रिक्त;
}

शून्य unposix_unlockpp(पूर्णाङ्क **fd) {
        प्रतिज्ञा(fd);

        यदि (!*fd || **fd < 0)
                फर्काऊ;

        (शून्य) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = रिक्त;
}

पूर्णाङ्क lock_generic(पूर्णाङ्क fd, LockType type, पूर्णाङ्क operation) {
        प्रतिज्ञा(fd >= 0);

        विकल्प (type) {
        पक्ष LOCK_NONE:
                फर्काऊ 0;
        पक्ष LOCK_BSD:
                फर्काऊ RET_NERRNO(flock(fd, operation));
        पक्ष LOCK_POSIX:
                फर्काऊ posix_lock(fd, operation);
        पक्ष LOCK_UNPOSIX:
                फर्काऊ unposix_lock(fd, operation);
        स्वभाव:
                assert_not_reached();
        }
}

पूर्णाङ्क lock_generic_with_timeout(पूर्णाङ्क fd, LockType type, पूर्णाङ्क operation, usec_t timeout) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        यदि (type == LOCK_NONE)
                फर्काऊ 0;
        यदि (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                फर्काऊ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        यदि (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                फर्काऊ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        यदि (r < 0)
                फर्काऊ log_error_errno(r, "Failed to flock block device in child process: %m");
        यदि (r == 0) {
                यदि (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(त्रुटि_सं, "Unable to get an exclusive lock on the device: %m");
                        _exit(निकास_त्रुटि);
                }

                _exit(निकास_सफल);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        यदि (r < 0)
                फर्काऊ r;

        pidref_done(&pidref);

        विकल्प (status.si_code) {

        पक्ष CLD_EXITED:
                यदि (status.si_status != निकास_सफल)
                        फर्काऊ -EPROTO;

                फर्काऊ 0;

        पक्ष CLD_KILLED:
        पक्ष CLD_DUMPED:
                फर्काऊ -EPROTO;

        स्वभाव:
                assert_not_reached();
        }
}
