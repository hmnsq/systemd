/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "lock-util.h"
#include "log.h"
#include "path-util.h"
#include "pidref.h"
#include "process-util.h"
#include "string-util.h"
#include "time-util.h"

ціле make_lock_file_at(ціле dir_fd, const char *p, ціле operation, LockFile *ret) {
        _cleanup_close_ ціле fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ char *t = NULL;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(p);
        assert(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        assert(ret);

        якщо (isempty(p))
                повернути -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        якщо (dfd < 0)
                повернути dfd;

        t = strdup(p);
        якщо (!t)
                повернути -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        якщо (fd < 0)
                повернути fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        повернути 0;
}

ціле make_lock_file_for(const char *p, ціле operation, LockFile *ret) {
        _cleanup_free_ char *fn = NULL, *dn = NULL, *t = NULL;
        ціле r;

        assert(p);
        assert(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        якщо (r < 0)
                повернути r;

        якщо (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        інакше
                t = strjoin(".#", fn, ".lck");
        якщо (!t)
                повернути -ENOMEM;

        повернути make_lock_file(t, operation, ret);
}

void release_lock_file(LockFile *f) {
        якщо (!f)
                повернути;

        якщо (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                якщо (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                якщо ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (void) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

static ціле fcntl_lock(ціле fd, ціле operation, bool ofd) {
        ціле cmd, type, r;

        assert(fd >= 0);

        якщо (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        інакше
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        switch (operation & ~LOCK_NB) {
                case LOCK_EX:
                        type = F_WRLCK;
                        break;
                case LOCK_SH:
                        type = F_RDLCK;
                        break;
                case LOCK_UN:
                        type = F_UNLCK;
                        break;
                default:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(struct flock) {
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
        якщо ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        повернути r;
}

ціле posix_lock(ціле fd, ціле operation) {
        повернути fcntl_lock(fd, operation, /* ofd= */ false);
}

ціле unposix_lock(ціле fd, ціле operation) {
        повернути fcntl_lock(fd, operation, /* ofd= */ true);
}

void posix_unlockpp(ціле **fd) {
        assert(fd);

        якщо (!*fd || **fd < 0)
                повернути;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = NULL;
}

void unposix_unlockpp(ціле **fd) {
        assert(fd);

        якщо (!*fd || **fd < 0)
                повернути;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = NULL;
}

ціле lock_generic(ціле fd, LockType type, ціле operation) {
        assert(fd >= 0);

        switch (type) {
        case LOCK_NONE:
                повернути 0;
        case LOCK_BSD:
                повернути RET_NERRNO(flock(fd, operation));
        case LOCK_POSIX:
                повернути posix_lock(fd, operation);
        case LOCK_UNPOSIX:
                повернути unposix_lock(fd, operation);
        default:
                assert_not_reached();
        }
}

ціле lock_generic_with_timeout(ціле fd, LockType type, ціле operation, usec_t timeout) {
        ціле r;

        assert(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        якщо (type == LOCK_NONE)
                повернути 0;
        якщо (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                повернути -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        якщо (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                повернути r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to flock block device in child process: %m");
        якщо (r == 0) {
                якщо (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        якщо (r < 0)
                повернути r;

        pidref_done(&pidref);

        switch (status.si_code) {

        case CLD_EXITED:
                якщо (status.si_status != EXIT_SUCCESS)
                        повернути -EPROTO;

                повернути 0;

        case CLD_KILLED:
        case CLD_DUMPED:
                повернути -EPROTO;

        default:
                assert_not_reached();
        }
}
