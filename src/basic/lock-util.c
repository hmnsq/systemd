/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <sys/file.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "lock-util.h"
#समावेश "log.h"
#समावेश "path-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "string-util.h"
#समावेश "time-util.h"

पूर्णांक make_lock_file_at(पूर्णांक dir_fd, स्थिर अक्षर *p, पूर्णांक operation, LockFile *ret) {
        _cleanup_close_ पूर्णांक fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ अक्षर *t = शून्य;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(p);
        निश्चित(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        निश्चित(ret);

        अगर (isempty(p))
                वापस -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        अगर (dfd < 0)
                वापस dfd;

        t = strdup(p);
        अगर (!t)
                वापस -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        अगर (fd < 0)
                वापस fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        वापस 0;
}

पूर्णांक make_lock_file_for(स्थिर अक्षर *p, पूर्णांक operation, LockFile *ret) {
        _cleanup_free_ अक्षर *fn = शून्य, *dn = शून्य, *t = शून्य;
        पूर्णांक r;

        निश्चित(p);
        निश्चित(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        अगर (r < 0)
                वापस r;

        अगर (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        अन्यथा
                t = strjoin(".#", fn, ".lck");
        अगर (!t)
                वापस -ENOMEM;

        वापस make_lock_file(t, operation, ret);
}

व्योम release_lock_file(LockFile *f) {
        अगर (!f)
                वापस;

        अगर (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                अगर (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                अगर ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (व्योम) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

अटल पूर्णांक fcntl_lock(पूर्णांक fd, पूर्णांक operation, bool ofd) {
        पूर्णांक cmd, type, r;

        निश्चित(fd >= 0);

        अगर (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        अन्यथा
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        चयन (operation & ~LOCK_NB) {
                हाल LOCK_EX:
                        type = F_WRLCK;
                        अवरोध;
                हाल LOCK_SH:
                        type = F_RDLCK;
                        अवरोध;
                हाल LOCK_UN:
                        type = F_UNLCK;
                        अवरोध;
                शेष:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(काष्ठा flock) {
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
        अगर ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        वापस r;
}

पूर्णांक posix_lock(पूर्णांक fd, पूर्णांक operation) {
        वापस fcntl_lock(fd, operation, /* ofd= */ false);
}

पूर्णांक unposix_lock(पूर्णांक fd, पूर्णांक operation) {
        वापस fcntl_lock(fd, operation, /* ofd= */ true);
}

व्योम posix_unlockpp(पूर्णांक **fd) {
        निश्चित(fd);

        अगर (!*fd || **fd < 0)
                वापस;

        (व्योम) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = शून्य;
}

व्योम unposix_unlockpp(पूर्णांक **fd) {
        निश्चित(fd);

        अगर (!*fd || **fd < 0)
                वापस;

        (व्योम) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = शून्य;
}

पूर्णांक lock_generic(पूर्णांक fd, LockType type, पूर्णांक operation) {
        निश्चित(fd >= 0);

        चयन (type) {
        हाल LOCK_NONE:
                वापस 0;
        हाल LOCK_BSD:
                वापस RET_NERRNO(flock(fd, operation));
        हाल LOCK_POSIX:
                वापस posix_lock(fd, operation);
        हाल LOCK_UNPOSIX:
                वापस unposix_lock(fd, operation);
        शेष:
                assert_not_reached();
        }
}

पूर्णांक lock_generic_with_timeout(पूर्णांक fd, LockType type, पूर्णांक operation, usec_t timeout) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        अगर (type == LOCK_NONE)
                वापस 0;
        अगर (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                वापस -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        अगर (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                वापस r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to flock block device in child process: %m");
        अगर (r == 0) {
                अगर (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(त्रुटि_सं, "Unable to get an exclusive lock on the device: %m");
                        _निकास(निकास_त्रुटि);
                }

                _निकास(निकास_सफल);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        अगर (r < 0)
                वापस r;

        pidref_done(&pidref);

        चयन (status.si_code) {

        हाल CLD_EXITED:
                अगर (status.si_status != निकास_सफल)
                        वापस -EPROTO;

                वापस 0;

        हाल CLD_KILLED:
        हाल CLD_DUMPED:
                वापस -EPROTO;

        शेष:
                assert_not_reached();
        }
}
