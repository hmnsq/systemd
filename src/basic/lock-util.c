/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/file.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "lock-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "time-util.h"

முழு make_lock_file_at(முழு dir_fd, மாறிலி எழுத்து *p, முழு operation, LockFile *ret) {
        _cleanup_close_ முழு fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ எழுத்து *t = வெற்று;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        ப்ரதிஜ்ஞா(ret);

        எனில் (isempty(p))
                திரும்பு -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        எனில் (dfd < 0)
                திரும்பு dfd;

        t = strdup(p);
        எனில் (!t)
                திரும்பு -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        எனில் (fd < 0)
                திரும்பு fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        திரும்பு 0;
}

முழு make_lock_file_for(மாறிலி எழுத்து *p, முழு operation, LockFile *ret) {
        _cleanup_free_ எழுத்து *fn = வெற்று, *dn = வெற்று, *t = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        இல்லையெனில்
                t = strjoin(".#", fn, ".lck");
        எனில் (!t)
                திரும்பு -ENOMEM;

        திரும்பு make_lock_file(t, operation, ret);
}

வெறுமை release_lock_file(LockFile *f) {
        எனில் (!f)
                திரும்பு;

        எனில் (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                எனில் (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                எனில் ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (வெறுமை) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

நிலையான முழு fcntl_lock(முழு fd, முழு operation, bool ofd) {
        முழு cmd, type, r;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        இல்லையெனில்
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        தேர்வு (operation & ~LOCK_NB) {
                நிலை LOCK_EX:
                        type = F_WRLCK;
                        நிறுத்து;
                நிலை LOCK_SH:
                        type = F_RDLCK;
                        நிறுத்து;
                நிலை LOCK_UN:
                        type = F_UNLCK;
                        நிறுத்து;
                இயல்பு:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(கட்டமைப்பு flock) {
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
        எனில் ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        திரும்பு r;
}

முழு posix_lock(முழு fd, முழு operation) {
        திரும்பு fcntl_lock(fd, operation, /* ofd= */ false);
}

முழு unposix_lock(முழு fd, முழு operation) {
        திரும்பு fcntl_lock(fd, operation, /* ofd= */ true);
}

வெறுமை posix_unlockpp(முழு **fd) {
        ப்ரதிஜ்ஞா(fd);

        எனில் (!*fd || **fd < 0)
                திரும்பு;

        (வெறுமை) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = வெற்று;
}

வெறுமை unposix_unlockpp(முழு **fd) {
        ப்ரதிஜ்ஞா(fd);

        எனில் (!*fd || **fd < 0)
                திரும்பு;

        (வெறுமை) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = வெற்று;
}

முழு lock_generic(முழு fd, LockType type, முழு operation) {
        ப்ரதிஜ்ஞா(fd >= 0);

        தேர்வு (type) {
        நிலை LOCK_NONE:
                திரும்பு 0;
        நிலை LOCK_BSD:
                திரும்பு RET_NERRNO(flock(fd, operation));
        நிலை LOCK_POSIX:
                திரும்பு posix_lock(fd, operation);
        நிலை LOCK_UNPOSIX:
                திரும்பு unposix_lock(fd, operation);
        இயல்பு:
                assert_not_reached();
        }
}

முழு lock_generic_with_timeout(முழு fd, LockType type, முழு operation, usec_t timeout) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        எனில் (type == LOCK_NONE)
                திரும்பு 0;
        எனில் (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                திரும்பு -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        எனில் (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                திரும்பு r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to flock block device in child process: %m");
        எனில் (r == 0) {
                எனில் (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        எனில் (r < 0)
                திரும்பு r;

        pidref_done(&pidref);

        தேர்வு (status.si_code) {

        நிலை CLD_EXITED:
                எனில் (status.si_status != EXIT_SUCCESS)
                        திரும்பு -EPROTO;

                திரும்பு 0;

        நிலை CLD_KILLED:
        நிலை CLD_DUMPED:
                திரும்பு -EPROTO;

        இயல்பு:
                assert_not_reached();
        }
}
