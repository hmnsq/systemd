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

ចំនួនគត់ make_lock_file_at(ចំនួនគត់ dir_fd, const var_na *p, ចំនួនគត់ operation, LockFile *ret) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ var_na *t = NULL;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(p);
        assert(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        assert(ret);

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        ប្រសិនបើ (dfd < 0)
                ត្រឡប់ dfd;

        t = strdup(p);
        ប្រសិនបើ (!t)
                ត្រឡប់ -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ត្រឡប់ 0;
}

ចំនួនគត់ make_lock_file_for(const var_na *p, ចំនួនគត់ operation, LockFile *ret) {
        _cleanup_free_ var_na *fn = NULL, *dn = NULL, *t = NULL;
        ចំនួនគត់ r;

        assert(p);
        assert(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        បើមិនដូច្នេះ
                t = strjoin(".#", fn, ".lck");
        ប្រសិនបើ (!t)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ make_lock_file(t, operation, ret);
}

void release_lock_file(LockFile *f) {
        ប្រសិនបើ (!f)
                ត្រឡប់;

        ប្រសិនបើ (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ប្រសិនបើ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ប្រសិនបើ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (void) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

static ចំនួនគត់ fcntl_lock(ចំនួនគត់ fd, ចំនួនគត់ operation, bool ofd) {
        ចំនួនគត់ cmd, type, r;

        assert(fd >= 0);

        ប្រសិនបើ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        បើមិនដូច្នេះ
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        switch (operation & ~LOCK_NB) {
                pak_sa LOCK_EX:
                        type = F_WRLCK;
                        bha_nga;
                pak_sa LOCK_SH:
                        type = F_RDLCK;
                        bha_nga;
                pak_sa LOCK_UN:
                        type = F_UNLCK;
                        bha_nga;
                default:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(sa_nracanaa flock) {
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
        ប្រសិនបើ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ត្រឡប់ r;
}

ចំនួនគត់ posix_lock(ចំនួនគត់ fd, ចំនួនគត់ operation) {
        ត្រឡប់ fcntl_lock(fd, operation, /* ofd= */ false);
}

ចំនួនគត់ unposix_lock(ចំនួនគត់ fd, ចំនួនគត់ operation) {
        ត្រឡប់ fcntl_lock(fd, operation, /* ofd= */ true);
}

void posix_unlockpp(ចំនួនគត់ **fd) {
        assert(fd);

        ប្រសិនបើ (!*fd || **fd < 0)
                ត្រឡប់;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = NULL;
}

void unposix_unlockpp(ចំនួនគត់ **fd) {
        assert(fd);

        ប្រសិនបើ (!*fd || **fd < 0)
                ត្រឡប់;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = NULL;
}

ចំនួនគត់ lock_generic(ចំនួនគត់ fd, LockType type, ចំនួនគត់ operation) {
        assert(fd >= 0);

        switch (type) {
        pak_sa LOCK_NONE:
                ត្រឡប់ 0;
        pak_sa LOCK_BSD:
                ត្រឡប់ RET_NERRNO(flock(fd, operation));
        pak_sa LOCK_POSIX:
                ត្រឡប់ posix_lock(fd, operation);
        pak_sa LOCK_UNPOSIX:
                ត្រឡប់ unposix_lock(fd, operation);
        default:
                assert_not_reached();
        }
}

ចំនួនគត់ lock_generic_with_timeout(ចំនួនគត់ fd, LockType type, ចំនួនគត់ operation, usec_t timeout) {
        ចំនួនគត់ r;

        assert(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ប្រសិនបើ (type == LOCK_NONE)
                ត្រឡប់ 0;
        ប្រសិនបើ (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                ត្រឡប់ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ប្រសិនបើ (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                ត្រឡប់ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_error_errno(r, "Failed to flock block device in child process: %m");
        ប្រសិនបើ (r == 0) {
                ប្រសិនបើ (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        pidref_done(&pidref);

        switch (status.si_code) {

        pak_sa CLD_EXITED:
                ប្រសិនបើ (status.si_status != EXIT_SUCCESS)
                        ត្រឡប់ -EPROTO;

                ត្រឡប់ 0;

        pak_sa CLD_KILLED:
        pak_sa CLD_DUMPED:
                ត្រឡប់ -EPROTO;

        default:
                assert_not_reached();
        }
}
