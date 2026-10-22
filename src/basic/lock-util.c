/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/file.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "lock-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_lock_file_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, LockFile *ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF, dfd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ -EINVAL;

        /* We use UNPOSIX locks as they have nice semantics, and are mostly compatible with NFS. */

        dfd = fd_reopen(dir_fd, O_CLOEXEC|O_PATH|O_DIRECTORY);
        ଯଦି (dfd < 0)
                ଫେରନ୍ତୁ dfd;

        t = strdup(p);
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        fd = xopenat_lock_full(dfd,
                               p,
                               O_CREAT|O_RDWR|O_NOFOLLOW|O_CLOEXEC|O_NOCTTY,
                               /* xopen_flags= */ 0,
                               0600,
                               LOCK_UNPOSIX,
                               operation);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd == -EAGAIN ? -EBUSY : fd;

        *ret = (LockFile) {
                .dir_fd = TAKE_FD(dfd),
                .path = TAKE_PTR(t),
                .fd = TAKE_FD(fd),
                .operation = operation,
        };

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_lock_file_for(ଅଚଳ ଅକ୍ଷର *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, LockFile *ret) {
        _cleanup_free_ ଅକ୍ଷର *fn = ଖାଲି, *dn = ଖାଲି, *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        r = path_split_prefix_filename(p, &dn, &fn);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (dn)
                t = strjoin(dn, "/.#", fn, ".lck");
        ଅନ୍ୟଥା
                t = strjoin(".#", fn, ".lck");
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ make_lock_file(t, operation, ret);
}

ଶୂନ୍ୟ release_lock_file(LockFile *f) {
        ଯଦି (!f)
                ଫେରନ୍ତୁ;

        ଯଦି (f->path) {

                /* If we are the exclusive owner we can safely delete
                 * the lock file itself. If we are not the exclusive
                 * owner, we can try becoming it. */

                ଯଦି (f->fd >= 0 &&
                    (f->operation & ~LOCK_NB) == LOCK_SH &&
                    unposix_lock(f->fd, LOCK_EX|LOCK_NB) >= 0)
                        f->operation = LOCK_EX|LOCK_NB;

                ଯଦି ((f->operation & ~LOCK_NB) == LOCK_EX)
                        (ଶୂନ୍ୟ) unlinkat(f->dir_fd, f->path, 0);

                f->path = mfree(f->path);
        }

        f->dir_fd = safe_close(f->dir_fd);
        f->fd = safe_close(f->fd);
        f->operation = 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fcntl_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, bool ofd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, type, r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (ofd)
                cmd = (operation & LOCK_NB) ? F_OFD_SETLK : F_OFD_SETLKW;
        ଅନ୍ୟଥା
                cmd = (operation & LOCK_NB) ? F_SETLK : F_SETLKW;

        ବାଛ (operation & ~LOCK_NB) {
                ସ୍ଥିତି LOCK_EX:
                        type = F_WRLCK;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି LOCK_SH:
                        type = F_RDLCK;
                        ଭାଙ୍ଗ;
                ସ୍ଥିତି LOCK_UN:
                        type = F_UNLCK;
                        ଭାଙ୍ଗ;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
        }

        r = RET_NERRNO(fcntl(fd, cmd, &(ସଂରଚନା flock) {
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
        ଯଦି ((operation & LOCK_NB) && r == -EACCES)
                r = -EAGAIN;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation) {
        ଫେରନ୍ତୁ fcntl_lock(fd, operation, /* ofd= */ false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unposix_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation) {
        ଫେରନ୍ତୁ fcntl_lock(fd, operation, /* ofd= */ true);
}

ଶୂନ୍ୟ posix_unlockpp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା **fd) {
        ପ୍ରତିଜ୍ଞା(fd);

        ଯଦି (!*fd || **fd < 0)
                ଫେରନ୍ତୁ;

        (ଶୂନ୍ୟ) fcntl_lock(**fd, LOCK_UN, /* ofd= */ false);
        *fd = ଖାଲି;
}

ଶୂନ୍ୟ unposix_unlockpp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା **fd) {
        ପ୍ରତିଜ୍ଞା(fd);

        ଯଦି (!*fd || **fd < 0)
                ଫେରନ୍ତୁ;

        (ଶୂନ୍ୟ) fcntl_lock(**fd, LOCK_UN, /* ofd= */ true);
        *fd = ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_generic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, LockType type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ବାଛ (type) {
        ସ୍ଥିତି LOCK_NONE:
                ଫେରନ୍ତୁ 0;
        ସ୍ଥିତି LOCK_BSD:
                ଫେରନ୍ତୁ RET_NERRNO(flock(fd, operation));
        ସ୍ଥିତି LOCK_POSIX:
                ଫେରନ୍ତୁ posix_lock(fd, operation);
        ସ୍ଥିତି LOCK_UNPOSIX:
                ଫେରନ୍ତୁ unposix_lock(fd, operation);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_generic_with_timeout(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, LockType type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, usec_t timeout) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* A version of lock_generic(), but with a timeout. We do this in a child process, since the kernel
         * APIs natively don't support a timeout. We set a SIGALRM timer that will kill the child after the
         * timeout is hit. Returns -ETIMEDOUT if the timeout is hit, and 0 on success.
         *
         * This only works for BSD and UNPOSIX locks, as only those are fd-bound, and hence can be acquired
         * from any process that has access to the fd. POSIX locks OTOH are process-bound, and hence if we'd
         * acquire them in a child process they'd remain unlocked in the parent. */

        ଯଦି (type == LOCK_NONE)
                ଫେରନ୍ତୁ 0;
        ଯଦି (!IN_SET(type, LOCK_BSD, LOCK_UNPOSIX)) /* Not for POSIX locks, see above. */
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        /* First, try without forking anything off */
        r = lock_generic(fd, type, operation | (timeout == USEC_INFINITY ? 0 : LOCK_NB));
        ଯଦି (r != -EAGAIN || timeout == 0 || FLAGS_SET(operation, LOCK_NB))
                ଫେରନ୍ତୁ r;

        /* If that didn't work, try with a child */
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        r = pidref_safe_fork("(sd-flock)", FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL, &pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to flock block device in child process: %m");
        ଯଦି (r == 0) {
                ଯଦି (lock_generic(fd, type, operation) < 0) {
                        log_error_errno(errno, "Unable to get an exclusive lock on the device: %m");
                        _exit(EXIT_FAILURE);
                }

                _exit(EXIT_SUCCESS);
        }

        siginfo_t status;
        r = pidref_wait_for_terminate_full(&pidref, timeout, &status);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        pidref_done(&pidref);

        ବାଛ (status.si_code) {

        ସ୍ଥିତି CLD_EXITED:
                ଯଦି (status.si_status != EXIT_SUCCESS)
                        ଫେରନ୍ତୁ -EPROTO;

                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି CLD_KILLED:
        ସ୍ଥିତି CLD_DUMPED:
                ଫେରନ୍ତୁ -EPROTO;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }
}
