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

정수 make_lock_file_at(정수 dir_fd, const char *p, 정수 operation, LockFile *ret) {
        _cleanup_close_ 정수 fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ char *t = NULL;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(p);
        assert(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        assert(ret);

        만약 (isempty(p))
                반환 -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        만약 (dfd < 0)
                반환 dfd;

        t = strdup(p);
        만약 (!t)
                반환 -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        만약 (fd < 0)
                반환 fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        반환 0;
}

정수 make_lock_file_for(const char *p, 정수 operation, LockFile *ret) {
        _cleanup_free_ char *fn = NULL, *dn = NULL, *t = NULL;
        정수 r;

        assert(p);
        assert(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        만약 (r < 0)
                반환 r;

        만약 (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        아니면
                t = strjoin(".#", fn, ".lck");
        만약 (!t)
                반환 -ENOMEM;

        반환 make_lock_file(t, operation, ret);
}

void release_lock_file(LockFile *f) {
        만약 (!f)
                반환;

        만약 (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                만약 (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                만약 ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (void) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

static 정수 fcntl_lock(정수 fd, 정수 operation, bool ofd) {
        정수 cmd, type, r;

        assert(fd >= 0);

        만약 (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        아니면
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
        만약 ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        반환 r;
}

정수 posix_lock(정수 fd, 정수 operation) {
        반환 fcntl_lock(fd, operation, /* ofd= */ false);
}

정수 unposix_lock(정수 fd, 정수 operation) {
        반환 fcntl_lock(fd, operation, /* ofd= */ true);
}

void posix_unlockpp(정수 **fd) {
        assert(fd);

        만약 (!*fd || **fd < 0)
                반환;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = NULL;
}

void unposix_unlockpp(정수 **fd) {
        assert(fd);

        만약 (!*fd || **fd < 0)
                반환;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = NULL;
}

정수 lock_generic(정수 fd, LockType type, 정수 operation) {
        assert(fd >= 0);

        switch (type) {
        case LOCK_NONE:
                반환 0;
        case LOCK_BSD:
                반환 RET_NERRNO(flock(fd, operation));
        case LOCK_POSIX:
                반환 posix_lock(fd, operation);
        case LOCK_UNPOSIX:
                반환 unposix_lock(fd, operation);
        default:
                assert_not_reached();
        }
}

정수 lock_generic_with_timeout(정수 fd, LockType type, 정수 operation, usec_t timeout) {
        정수 r;

        assert(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        만약 (type == LOCK_NONE)
                반환 0;
        만약 (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                반환 -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        만약 (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                반환 r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        만약 (r < 0)
                반환 log_error_errno(r, "Failed to flock block device in child process: %m");
        만약 (r == 0) {
                만약 (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        만약 (r < 0)
                반환 r;

        pidref_done(&pidref);

        switch (status.si_code) {

        case CLD_EXITED:
                만약 (status.si_status != EXIT_SUCCESS)
                        반환 -EPROTO;

                반환 0;

        case CLD_KILLED:
        case CLD_DUMPED:
                반환 -EPROTO;

        default:
                assert_not_reached();
        }
}
