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

पूर्णांक make_lock_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *p, पूर्णांक operation, LockFile *ret) {
        _cleanup_close_ पूर्णांक fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ अक्षर *t = रिक्त;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(p);
        प्रतिज्ञा(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        प्रतिज्ञा(ret);

        जर (isempty(p))
                परत -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        जर (dfd < 0)
                परत dfd;

        t = strdup(p);
        जर (!t)
                परत -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        जर (fd < 0)
                परत fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        परत 0;
}

पूर्णांक make_lock_file_for(स्थिरांक अक्षर *p, पूर्णांक operation, LockFile *ret) {
        _cleanup_free_ अक्षर *fn = रिक्त, *dn = रिक्त, *t = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        जर (r < 0)
                परत r;

        जर (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        अन्यथा
                t = strjoin(".#", fn, ".lck");
        जर (!t)
                परत -ENOMEM;

        परत make_lock_file(t, operation, ret);
}

शून्य release_lock_file(LockFile *f) {
        जर (!f)
                परत;

        जर (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                जर (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                जर ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (शून्य) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

स्थिर पूर्णांक fcntl_lock(पूर्णांक fd, पूर्णांक operation, bool ofd) {
        पूर्णांक cmd, type, r;

        प्रतिज्ञा(fd >= 0);

        जर (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        अन्यथा
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        निवड (operation & ~LOCK_NB) {
                स्थिती LOCK_EX:
                        type = F_WRLCK;
                        थांबा;
                स्थिती LOCK_SH:
                        type = F_RDLCK;
                        थांबा;
                स्थिती LOCK_UN:
                        type = F_UNLCK;
                        थांबा;
                पूर्वनिर्धारित:
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
        जर ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        परत r;
}

पूर्णांक posix_lock(पूर्णांक fd, पूर्णांक operation) {
        परत fcntl_lock(fd, operation, /* ofd= */ false);
}

पूर्णांक unposix_lock(पूर्णांक fd, पूर्णांक operation) {
        परत fcntl_lock(fd, operation, /* ofd= */ true);
}

शून्य posix_unlockpp(पूर्णांक **fd) {
        प्रतिज्ञा(fd);

        जर (!*fd || **fd < 0)
                परत;

        (शून्य) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = रिक्त;
}

शून्य unposix_unlockpp(पूर्णांक **fd) {
        प्रतिज्ञा(fd);

        जर (!*fd || **fd < 0)
                परत;

        (शून्य) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = रिक्त;
}

पूर्णांक lock_generic(पूर्णांक fd, LockType type, पूर्णांक operation) {
        प्रतिज्ञा(fd >= 0);

        निवड (type) {
        स्थिती LOCK_NONE:
                परत 0;
        स्थिती LOCK_BSD:
                परत RET_NERRNO(flock(fd, operation));
        स्थिती LOCK_POSIX:
                परत posix_lock(fd, operation);
        स्थिती LOCK_UNPOSIX:
                परत unposix_lock(fd, operation);
        पूर्वनिर्धारित:
                assert_not_reached();
        }
}

पूर्णांक lock_generic_with_timeout(पूर्णांक fd, LockType type, पूर्णांक operation, usec_t timeout) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        जर (type == LOCK_NONE)
                परत 0;
        जर (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                परत -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        जर (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                परत r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        जर (r < 0)
                परत log_error_errno(r, "Failed to flock block device in child process: %m");
        जर (r == 0) {
                जर (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(त्रुटि_सं, "Unable to get an exclusive lock on the device: %m");
                        _exit(निकास_त्रुटि);
                }

                _exit(निकास_सफल);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        जर (r < 0)
                परत r;

        pidref_done(&pidref);

        निवड (status.si_code) {

        स्थिती CLD_EXITED:
                जर (status.si_status != निकास_सफल)
                        परत -EPROTO;

                परत 0;

        स्थिती CLD_KILLED:
        स्थिती CLD_DUMPED:
                परत -EPROTO;

        पूर्वनिर्धारित:
                assert_not_reached();
        }
}
