/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/file.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "lock-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "time-util.h"

પૂર્ણાંક make_lock_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *p, પૂર્ણાંક operation, LockFile *ret) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ અક્ષર *t = ખાલી;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        પ્રતિજ્ઞા(ret);

        જો (isempty(p))
                પાછા -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        જો (dfd < 0)
                પાછા dfd;

        t = strdup(p);
        જો (!t)
                પાછા -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        જો (fd < 0)
                પાછા fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        પાછા 0;
}

પૂર્ણાંક make_lock_file_for(અચલ અક્ષર *p, પૂર્ણાંક operation, LockFile *ret) {
        _cleanup_free_ અક્ષર *fn = ખાલી, *dn = ખાલી, *t = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        જો (r < 0)
                પાછા r;

        જો (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        નહીંતર
                t = strjoin(".#", fn, ".lck");
        જો (!t)
                પાછા -ENOMEM;

        પાછા make_lock_file(t, operation, ret);
}

શૂન્ય release_lock_file(LockFile *f) {
        જો (!f)
                પાછા;

        જો (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                જો (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                જો ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (શૂન્ય) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

સ્થિર પૂર્ણાંક fcntl_lock(પૂર્ણાંક fd, પૂર્ણાંક operation, bool ofd) {
        પૂર્ણાંક cmd, type, r;

        પ્રતિજ્ઞા(fd >= 0);

        જો (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        નહીંતર
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        પસંદગી (operation & ~LOCK_NB) {
                સ્થિતિ LOCK_EX:
                        type = F_WRLCK;
                        તોડો;
                સ્થિતિ LOCK_SH:
                        type = F_RDLCK;
                        તોડો;
                સ્થિતિ LOCK_UN:
                        type = F_UNLCK;
                        તોડો;
                મૂળભૂત:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(સંરચના flock) {
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
        જો ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        પાછા r;
}

પૂર્ણાંક posix_lock(પૂર્ણાંક fd, પૂર્ણાંક operation) {
        પાછા fcntl_lock(fd, operation, /* ofd= */ false);
}

પૂર્ણાંક unposix_lock(પૂર્ણાંક fd, પૂર્ણાંક operation) {
        પાછા fcntl_lock(fd, operation, /* ofd= */ true);
}

શૂન્ય posix_unlockpp(પૂર્ણાંક **fd) {
        પ્રતિજ્ઞા(fd);

        જો (!*fd || **fd < 0)
                પાછા;

        (શૂન્ય) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ખાલી;
}

શૂન્ય unposix_unlockpp(પૂર્ણાંક **fd) {
        પ્રતિજ્ઞા(fd);

        જો (!*fd || **fd < 0)
                પાછા;

        (શૂન્ય) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ખાલી;
}

પૂર્ણાંક lock_generic(પૂર્ણાંક fd, LockType type, પૂર્ણાંક operation) {
        પ્રતિજ્ઞા(fd >= 0);

        પસંદગી (type) {
        સ્થિતિ LOCK_NONE:
                પાછા 0;
        સ્થિતિ LOCK_BSD:
                પાછા RET_NERRNO(flock(fd, operation));
        સ્થિતિ LOCK_POSIX:
                પાછા posix_lock(fd, operation);
        સ્થિતિ LOCK_UNPOSIX:
                પાછા unposix_lock(fd, operation);
        મૂળભૂત:
                assert_not_reached();
        }
}

પૂર્ણાંક lock_generic_with_timeout(પૂર્ણાંક fd, LockType type, પૂર્ણાંક operation, usec_t timeout) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        જો (type == LOCK_NONE)
                પાછા 0;
        જો (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                પાછા -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        જો (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                પાછા r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to flock block device in child process: %m");
        જો (r == 0) {
                જો (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        જો (r < 0)
                પાછા r;

        pidref_done(&pidref);

        પસંદગી (status.si_code) {

        સ્થિતિ CLD_EXITED:
                જો (status.si_status != EXIT_SUCCESS)
                        પાછા -EPROTO;

                પાછા 0;

        સ્થિતિ CLD_KILLED:
        સ્થિતિ CLD_DUMPED:
                પાછા -EPROTO;

        મૂળભૂત:
                assert_not_reached();
        }
}
