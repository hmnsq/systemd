/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/file.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "lock-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "time-util.h"

పూర్ణాంకం make_తాళం_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *p, పూర్ణాంకం operation, తాళం_File *ret) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ అక్షరం *t = ఖాళీ;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(లోపల_సమితి(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (isempty(p))
                తిరిగి -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_మార్గం|O_DIRECTORY);
        ఒకవేళ (dfd < 0)
                తిరిగి dfd;

        t = strdup(p);
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        fd = xopenat_తాళం_నిండిన(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               తాళం_UNPOSIX,
                               operation);
        ఒకవేళ (fd < 0)
                తిరిగి fd == -EAGAIN ? -EBUSY : fd;

        *ret = (తాళం_File) {
                .dir_fd = TAKE_FD(dfd),
                .మార్గం = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        తిరిగి 0;
}

పూర్ణాంకం make_తాళం_file_for(స్థిరరాశి అక్షరం *p, పూర్ణాంకం operation, తాళం_File *ret) {
        _cleanup_free_ అక్షరం *fn = ఖాళీ, *dn = ఖాళీ, *t = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        r = మార్గం_విభజించు_ఉపసర్గ_filename(p, &dn, &fn);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        లేకపోతే
                t = strjoin(".#", fn, ".lck");
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        తిరిగి make_తాళం_file(t, operation, ret);
}

శూన్య release_తాళం_file(తాళం_File *f) {
        ఒకవేళ (!f)
                తిరిగి;

        ఒకవేళ (f->మార్గం) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ఒకవేళ (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_తాళం(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ఒకవేళ ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (శూన్య) unlinkat(f->dir_fd, f->మార్గం, 0);

                f->మార్గం = mfree(f->మార్గం);
        }

        f->dir_fd = సురక్షిత_close(f->dir_fd);
        f->fd = సురక్షిత_close(f->fd);
        f->operation = 0;
}

స్థిర పూర్ణాంకం fcntl_తాళం(పూర్ణాంకం fd, పూర్ణాంకం operation, bool ofd) {
        పూర్ణాంకం cmd, type, r;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        లేకపోతే
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        ఎంపిక (operation & ~LOCK_NB) {
                స్థితి LOCK_EX:
                        type = F_WRLCK;
                        ఆపు;
                స్థితి LOCK_SH:
                        type = F_RDLCK;
                        ఆపు;
                స్థితి LOCK_UN:
                        type = F_UNLCK;
                        ఆపు;
                అప్రమేయ:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(నిర్మాణం flock) {
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
        ఒకవేళ ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        తిరిగి r;
}

పూర్ణాంకం posix_తాళం(పూర్ణాంకం fd, పూర్ణాంకం operation) {
        తిరిగి fcntl_తాళం(fd, operation, /* ofd= */ false);
}

పూర్ణాంకం unposix_తాళం(పూర్ణాంకం fd, పూర్ణాంకం operation) {
        తిరిగి fcntl_తాళం(fd, operation, /* ofd= */ true);
}

శూన్య posix_unlockpp(పూర్ణాంకం **fd) {
        ప్రతిజ్ఞా(fd);

        ఒకవేళ (!*fd || **fd < 0)
                తిరిగి;

        (శూన్య) fcntl_తాళం(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ఖాళీ;
}

శూన్య unposix_unlockpp(పూర్ణాంకం **fd) {
        ప్రతిజ్ఞా(fd);

        ఒకవేళ (!*fd || **fd < 0)
                తిరిగి;

        (శూన్య) fcntl_తాళం(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ఖాళీ;
}

పూర్ణాంకం తాళం_generic(పూర్ణాంకం fd, తాళం_Type type, పూర్ణాంకం operation) {
        ప్రతిజ్ఞా(fd >= 0);

        ఎంపిక (type) {
        స్థితి తాళం_NONE:
                తిరిగి 0;
        స్థితి తాళం_BSD:
                తిరిగి RET_NERRNO(flock(fd, operation));
        స్థితి తాళం_POSIX:
                తిరిగి posix_తాళం(fd, operation);
        స్థితి తాళం_UNPOSIX:
                తిరిగి unposix_తాళం(fd, operation);
        అప్రమేయ:
                assert_not_reached();
        }
}

పూర్ణాంకం తాళం_generic_with_timeout(పూర్ణాంకం fd, తాళం_Type type, పూర్ణాంకం operation, usec_t timeout) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ఒకవేళ (type == తాళం_NONE)
                తిరిగి 0;
        ఒకవేళ (!లోపల_సమితి(type, తాళం_BSD, తాళం_UNPOSIX)) /* Not for POSIX locks, see above. */
                తిరిగి -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = తాళం_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ఒకవేళ (r != -EAGAIN || timeout == 0 || FLAGS_సమితి(operation, LOCK_NB))
                తిరిగి r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_సురక్షిత_fork("(sd-flock)", FORK_RESET_సంకేతాలు|FORK_DEATHSIG_SIGKILL, &pidref);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to flock block device in child process: %m");
        ఒకవేళ (r == 0) {
                ఒకవేళ (తాళం_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_నిండిన(&pidref, timeout, &status);
        ఒకవేళ (r < 0)
                తిరిగి r;

        pidref_పూర్తి(&pidref);

        ఎంపిక (status.si_code) {

        స్థితి CLD_EXITED:
                ఒకవేళ (status.si_status != EXIT_SUCCESS)
                        తిరిగి -EPROTO;

                తిరిగి 0;

        స్థితి CLD_KILLED:
        స్థితి CLD_DUMPED:
                తిరిగి -EPROTO;

        అప్రమేయ:
                assert_not_reached();
        }
}
