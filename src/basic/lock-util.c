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

ຈຳນວນເຕັມ make_lock_file_at(ຈຳນວນເຕັມ dir_fd, const var_na *p, ຈຳນວນເຕັມ operation, LockFile *ret) {
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ var_na *t = NULL;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(p);
        assert(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        assert(ret);

        ຖ້າ (isempty(p))
                ສົ່ງຄືນ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        ຖ້າ (dfd < 0)
                ສົ່ງຄືນ dfd;

        t = strdup(p);
        ຖ້າ (!t)
                ສົ່ງຄືນ -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ make_lock_file_for(const var_na *p, ຈຳນວນເຕັມ operation, LockFile *ret) {
        _cleanup_free_ var_na *fn = NULL, *dn = NULL, *t = NULL;
        ຈຳນວນເຕັມ r;

        assert(p);
        assert(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        ບໍ່ດັ່ງນັ້ນ
                t = strjoin(".#", fn, ".lck");
        ຖ້າ (!t)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ make_lock_file(t, operation, ret);
}

void release_lock_file(LockFile *f) {
        ຖ້າ (!f)
                ສົ່ງຄືນ;

        ຖ້າ (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ຖ້າ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ຖ້າ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (void) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

static ຈຳນວນເຕັມ fcntl_lock(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ operation, bool ofd) {
        ຈຳນວນເຕັມ cmd, type, r;

        assert(fd >= 0);

        ຖ້າ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        ບໍ່ດັ່ງນັ້ນ
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
        ຖ້າ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ posix_lock(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ operation) {
        ສົ່ງຄືນ fcntl_lock(fd, operation, /* ofd= */ false);
}

ຈຳນວນເຕັມ unposix_lock(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ operation) {
        ສົ່ງຄືນ fcntl_lock(fd, operation, /* ofd= */ true);
}

void posix_unlockpp(ຈຳນວນເຕັມ **fd) {
        assert(fd);

        ຖ້າ (!*fd || **fd < 0)
                ສົ່ງຄືນ;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = NULL;
}

void unposix_unlockpp(ຈຳນວນເຕັມ **fd) {
        assert(fd);

        ຖ້າ (!*fd || **fd < 0)
                ສົ່ງຄືນ;

        (void) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = NULL;
}

ຈຳນວນເຕັມ lock_generic(ຈຳນວນເຕັມ fd, LockType type, ຈຳນວນເຕັມ operation) {
        assert(fd >= 0);

        switch (type) {
        pak_sa LOCK_NONE:
                ສົ່ງຄືນ 0;
        pak_sa LOCK_BSD:
                ສົ່ງຄືນ RET_NERRNO(flock(fd, operation));
        pak_sa LOCK_POSIX:
                ສົ່ງຄືນ posix_lock(fd, operation);
        pak_sa LOCK_UNPOSIX:
                ສົ່ງຄືນ unposix_lock(fd, operation);
        default:
                assert_not_reached();
        }
}

ຈຳນວນເຕັມ lock_generic_with_timeout(ຈຳນວນເຕັມ fd, LockType type, ຈຳນວນເຕັມ operation, usec_t timeout) {
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ຖ້າ (type == LOCK_NONE)
                ສົ່ງຄືນ 0;
        ຖ້າ (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                ສົ່ງຄືນ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ຖ້າ (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                ສົ່ງຄືນ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_error_errno(r, "Failed to flock block device in child process: %m");
        ຖ້າ (r == 0) {
                ຖ້າ (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        pidref_done(&pidref);

        switch (status.si_code) {

        pak_sa CLD_EXITED:
                ຖ້າ (status.si_status != EXIT_SUCCESS)
                        ສົ່ງຄືນ -EPROTO;

                ສົ່ງຄືນ 0;

        pak_sa CLD_KILLED:
        pak_sa CLD_DUMPED:
                ສົ່ງຄືນ -EPROTO;

        default:
                assert_not_reached();
        }
}
