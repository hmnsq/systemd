/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <linux/fs.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/kcmp.h>
#అన్తర్భావ <sys/resource.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "sort-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#నిర్వచించు MAX_FD_LOOP_హద్దు (1024*1024)

పూర్ణాంకం close_nointr(పూర్ణాంకం fd) {
        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (close(fd) >= 0)
                తిరిగి 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        ఒకవేళ (errno == EINTR)
                తిరిగి 0;

        తిరిగి -errno;
}

పూర్ణాంకం సురక్షిత_close(పూర్ణాంకం fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        ఒకవేళ (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        తిరిగి -EBADF;
}

శూన్య సురక్షిత_close_pair(పూర్ణాంకం p[స్థిర 2]) {
        ప్రతిజ్ఞా(p);

        ఒకవేళ (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = సురక్షిత_close(p[0]);
                తిరిగి;
        }

        p[0] = సురక్షిత_close(p[0]);
        p[1] = సురక్షిత_close(p[1]);
}

శూన్య close_many(స్థిరరాశి పూర్ణాంకం fds[], size_t n_fds) {
        ప్రతిజ్ఞా(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                సురక్షిత_close(*fd);
}

శూన్య close_many_unset(పూర్ణాంకం fds[], size_t n_fds) {
        ప్రతిజ్ఞా(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = సురక్షిత_close(*fd);
}

శూన్య close_many_and_free(పూర్ణాంకం *fds, size_t n_fds) {
        ప్రతిజ్ఞా(fds || n_fds == 0);

        close_many(fds, n_fds);
        విడుదల(fds);
}

పూర్ణాంకం fclose_nointr(FILE *f) {
        ప్రతిజ్ఞా(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        ఒకవేళ (fclose(f) == 0)
                తిరిగి 0;

        ఒకవేళ (errno == EINTR)
                తిరిగి 0;

        తిరిగి errno_or_else(EIO);
}

FILE* సురక్షిత_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        ఒకవేళ (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        తిరిగి ఖాళీ;
}

DIR* సురక్షిత_closedir(DIR *d) {

        ఒకవేళ (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        తిరిగి ఖాళీ;
}

పూర్ణాంకం fd_nonblock(పూర్ణాంకం fd, bool nonblock) {
        పూర్ణాంకం flags, nflags;

        ప్రతిజ్ఞా(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        ఒకవేళ (flags < 0)
                తిరిగి -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        ఒకవేళ (nflags == flags)
                తిరిగి 0;

        ఒకవేళ (fcntl(fd, F_SETFL, nflags) < 0)
                తిరిగి -errno;

        తిరిగి 1;
}

శూన్య nonblock_resetp(పూర్ణాంకం *fd) {
        ప్రతిజ్ఞా(fd);

        PROTECT_ERRNO;

        ఒకవేళ (*fd >= 0)
                (శూన్య) fd_nonblock(*fd, false);
}

పూర్ణాంకం stdio_disable_nonblock(శూన్య) {
        పూర్ణాంకం ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        తిరిగి ret;
}

పూర్ణాంకం fd_cloexec(పూర్ణాంకం fd, bool cloexec) {
        పూర్ణాంకం flags, nflags;

        ప్రతిజ్ఞా(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        ఒకవేళ (flags < 0)
                తిరిగి -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        ఒకవేళ (nflags == flags)
                తిరిగి 0;

        తిరిగి RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

పూర్ణాంకం fd_cloexec_many(స్థిరరాశి పూర్ణాంకం fds[], size_t n_fds, bool cloexec) {
        పూర్ణాంకం r = 0;

        ప్రతిజ్ఞా(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                ఒకవేళ (*fd < 0) /* Skip gracefully over already invalidated fds */
                        కొనసాగు;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        తిరిగి r;
}

స్థిర bool fd_లోపల_సమితి(పూర్ణాంకం fd, స్థిరరాశి పూర్ణాంకం fds[], size_t n_fds) {
        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                ఒకవేళ (*i < 0)
                        కొనసాగు;

                ఒకవేళ (*i == fd)
                        తిరిగి true;
        }

        తిరిగి false;
}

పూర్ణాంకం get_max_fd(శూన్య) {
        నిర్మాణం rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        ఒకవేళ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                తిరిగి -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        ఒకవేళ (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                తిరిగి FD_SETSIZE-1;

        ఒకవేళ (m == RLIM_INFINITY || m > గరిష్ఠ_పూర్ణాంకం) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                తిరిగి గరిష్ఠ_పూర్ణాంకం;

        తిరిగి (పూర్ణాంకం) (m - 1);
}

పూర్ణాంకం close_all_fds_frugal(స్థిరరాశి పూర్ణాంకం except[], size_t n_except) {
        పూర్ణాంకం max_fd, r = 0;

        ప్రతిజ్ఞా(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        ఒకవేళ (max_fd < 0)
                తిరిగి max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        ఒకవేళ (max_fd > MAX_FD_LOOP_హద్దు)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        కొరకు (పూర్ణాంకం fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                పూర్ణాంకం q;

                ఒకవేళ (fd_లోపల_సమితి(fd, except, n_except))
                        కొనసాగు;

                q = close_nointr(fd);
                ఒకవేళ (q != -EBADF)
                        RET_GATHER(r, q);
        }

        తిరిగి r;
}

స్థిర పూర్ణాంకం close_all_fds_special_case(స్థిరరాశి పూర్ణాంకం except[], size_t n_except) {
        ప్రతిజ్ఞా(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        ఒకవేళ (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        ఎంపిక (n_except) {

        స్థితి 0:
                /* Close everything. Yay! */
                ఒకవేళ (close_range(3, గరిష్ఠ_పూర్ణాంకం, 0) < 0)
                        తిరిగి -errno;

                తిరిగి 1;

        స్థితి 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                ఒకవేళ (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        తిరిగి -errno;

                ఒకవేళ (except[0] < గరిష్ఠ_పూర్ణాంకం && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        తిరిగి -errno;

                తిరిగి 1;

        అప్రమేయ:
                తిరిగి 0;
        }
}

పూర్ణాంకం close_all_fds_without_malloc(స్థిరరాశి పూర్ణాంకం except[], size_t n_except) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r > 0) /* special case worked! */
                తిరిగి 0;

        తిరిగి close_all_fds_frugal(except, n_except);
}

పూర్ణాంకం close_all_fds(స్థిరరాశి పూర్ణాంకం except[], size_t n_except) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r > 0) /* special case worked! */
                తిరిగి 0;

        _cleanup_free_ పూర్ణాంకం *sorted_malloc = ఖాళీ;
        size_t n_sorted;
        పూర్ణాంకం *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        ప్రతిజ్ఞా(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        ఒకవేళ (n_sorted > ALLOCA_MAX / పరిమాణం(పూర్ణాంకం)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(పూర్ణాంకం, n_sorted);
        లేకపోతే
                sorted = newa(పూర్ణాంకం, n_sorted);

        ఒకవేళ (!sorted) /* Fallback on OOM. */
                తిరిగి close_all_fds_frugal(except, n_except);

        జ్ఞాపకం_నకలు(sorted, except, n_except * పరిమాణం(పూర్ణాంకం));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        కొరకు (size_t i = 0; i < n_sorted-1; i++) {
                పూర్ణాంకం ప్రారంభం, చివర;

                ప్రారంభం = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                చివర = MAX(sorted[i+1], 2);

                ప్రతిజ్ఞా(చివర >= ప్రారంభం);

                ఒకవేళ (చివర - ప్రారంభం <= 1)
                        కొనసాగు;

                /* Close everything between the start and end fds (both of which shall stay open) */
                ఒకవేళ (close_range(ప్రారంభం + 1, చివర - 1, 0) < 0)
                        తిరిగి -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        ఒకవేళ (sorted[n_sorted-1] >= గరిష్ఠ_పూర్ణాంకం) /* Dont let the addition below overflow */
                తిరిగి 0;

        ఒకవేళ (close_range(sorted[n_sorted-1] + 1, గరిష్ఠ_పూర్ణాంకం, 0) < 0)
                తిరిగి -errno;

        తిరిగి 0;
}

పూర్ణాంకం pack_fds(పూర్ణాంకం fds[], size_t n_fds) {
        ఒకవేళ (n_fds <= 0)
                తిరిగి 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        ప్రతిజ్ఞా(fds);

        కొరకు (పూర్ణాంకం ప్రారంభం = 0;;) {
                పూర్ణాంకం restart_from = -1;

                కొరకు (పూర్ణాంకం i = ప్రారంభం; i < (పూర్ణాంకం) n_fds; i++) {
                        పూర్ణాంకం nfd;

                        /* Already at right index? */
                        ఒకవేళ (fds[i] == i + 3)
                                కొనసాగు;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        ఒకవేళ (nfd < 0)
                                తిరిగి -errno;

                        సురక్షిత_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        ఒకవేళ (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                ఒకవేళ (restart_from < 0)
                        ఆపు;

                ప్రారంభం = restart_from;
        }

        ప్రతిజ్ఞా(fds[0] == 3);

        తిరిగి 0;
}

పూర్ణాంకం fd_validate(పూర్ణాంకం fd) {
        ఒకవేళ (fd < 0)
                తిరిగి -EBADF;

        ఒకవేళ (fcntl(fd, F_GETFD) < 0)
                తిరిగి -errno;

        తిరిగి 0;
}

పూర్ణాంకం same_fd(పూర్ణాంకం a, పూర్ణాంకం b) {
        నిర్మాణం stat sta, stb;
        pid_t pid;
        పూర్ణాంకం r, fa, fb;

        ప్రతిజ్ఞా(a >= 0);
        ప్రతిజ్ఞా(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        ఒకవేళ (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        ఒకవేళ (r > 0)
                తిరిగి true;
        ఒకవేళ (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        ఒకవేళ (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } లేకపోతే ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                తిరిగి -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        ఒకవేళ (r >= 0)
                తిరిగి !r;
        ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                తిరిగి -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        ఒకవేళ (fstat(a, &sta) < 0)
                తిరిగి -errno;

        ఒకవేళ (fstat(b, &stb) < 0)
                తిరిగి -errno;

        ఒకవేళ (!stat_inode_same(&sta, &stb))
                తిరిగి false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        ఒకవేళ (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                తిరిగి false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        ఒకవేళ (fa < 0)
                తిరిగి -errno;

        fb = fcntl(b, F_GETFL);
        ఒకవేళ (fb < 0)
                తిరిగి -errno;

        తిరిగి fa == fb;
}

bool fdname_is_valid(స్థిరరాశి అక్షరం *s) {
        స్థిరరాశి అక్షరం *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        ఒకవేళ (!s)
                తిరిగి false;

        కొరకు (p = s; *p; p++) {
                ఒకవేళ (*p < ' ')
                        తిరిగి false;
                ఒకవేళ (*p >= 127)
                        తిరిగి false;
                ఒకవేళ (*p == ':')
                        తిరిగి false;
        }

        తిరిగి p - s <= FDNAME_MAX;
}

పూర్ణాంకం fd_get_మార్గం(పూర్ణాంకం fd, అక్షరం **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(fd));

        ఒకవేళ (fd == AT_FDCWD)
                తిరిగి సురక్షిత_getcwd(ret);
        ఒకవేళ (fd == XAT_FDROOT)
                తిరిగి strdup_గమ్యం(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_మార్గం(fd), ret);
        ఒకవేళ (r == -ENOENT)
                తిరిగి proc_fd_enoent_errno();
        తిరిగి r;
}

పూర్ణాంకం move_fd(పూర్ణాంకం from, పూర్ణాంకం to, పూర్ణాంకం cloexec) {
        పూర్ణాంకం r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        ఒకవేళ (from < 0)
                తిరిగి -EBADF;
        ఒకవేళ (to < 0)
                తిరిగి -EBADF;

        ఒకవేళ (from == to) {

                ఒకవేళ (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                తిరిగి to;
        }

        ఒకవేళ (cloexec < 0) {
                పూర్ణాంకం fl;

                fl = fcntl(from, F_GETFD, 0);
                ఒకవేళ (fl < 0)
                        తిరిగి -errno;

                cloexec = FLAGS_సమితి(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        ఒకవేళ (r < 0)
                తిరిగి -errno;

        ప్రతిజ్ఞా(r == to);

        సురక్షిత_close(from);

        తిరిగి to;
}

పూర్ణాంకం fd_move_above_stdio(పూర్ణాంకం fd) {
        పూర్ణాంకం flags, copy;
        PROTECT_ERRNO;

        /* Moves the specified file descriptor if possible out of the range [0…2], i.e. the range of
         * stdin/stdout/stderr. If it can't be moved outside of this range the original file descriptor is
         * returned. This call is supposed to be used for long-lasting file descriptors we allocate in our code that
         * might get loaded into foreign code, and where we want ensure our fds are unlikely used accidentally as
         * stdin/stdout/stderr of unrelated code.
         *
         * Note that this doesn't fix any real bugs, it just makes it less likely that our code will be affected by
         * buggy code from others that mindlessly invokes 'fprintf(stderr, …' or similar in places where stderr has
         * been closed before.
         *
         * This function is written in a "best-effort" and "least-impact" style. This means whenever we encounter an
         * error we simply return the original file descriptor, and we do not touch errno. */

        ఒకవేళ (fd < 0 || fd > 2)
                తిరిగి fd;

        flags = fcntl(fd, F_GETFD, 0);
        ఒకవేళ (flags < 0)
                తిరిగి fd;

        ఒకవేళ (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        లేకపోతే
                copy = fcntl(fd, F_DUPFD, 3);
        ఒకవేళ (copy < 0)
                తిరిగి fd;

        ప్రతిజ్ఞా(copy > 2);

        (శూన్య) close(fd);
        తిరిగి copy;
}

పూర్ణాంకం rearrange_stdio(పూర్ణాంకం original_ఆదానం_fd, పూర్ణాంకం original_output_fd, పూర్ణాంకం original_error_fd) {
        పూర్ణాంకం fd[3] = { original_ఆదానం_fd,             /* Put together an array of fds we work on */
                      original_output_fd,
                      original_error_fd },
            null_fd = -EBADF,                        /* If we open /dev/null, we store the fd to it here */
            copy_fd[3] = EBADF_TRIPLET,              /* This contains all fds we duplicate here
                                                      * temporarily, and hence need to close at the end. */
            r;
        bool null_readable, null_writable;

        /* Sets up stdin, stdout, stderr with the three file descriptors passed in. If any of the descriptors
         * is specified as -EBADF it will be connected with /dev/null instead. If any of the file descriptors
         * is passed as itself (e.g. stdin as STDIN_FILENO) it is left unmodified, but the O_CLOEXEC bit is
         * turned off should it be on.
         *
         * Note that if any of the passed file descriptors are > 2 they will be closed — both on success and
         * on failure! Thus, callers should assume that when this function returns the input fds are
         * invalidated.
         *
         * Note that when this function fails stdin/stdout/stderr might remain half set up!
         *
         * O_CLOEXEC is turned off for all three file descriptors (which is how it should be for
         * stdin/stdout/stderr). */

        null_readable = original_ఆదానం_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        ఒకవేళ (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                ఒకవేళ (null_fd < 0) {
                        r = -errno;
                        వెళ్ళు ముగింపు;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                ఒకవేళ (null_fd < 3) {
                        పూర్ణాంకం copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ఒకవేళ (copy < 0) {
                                r = -errno;
                                వెళ్ళు ముగింపు;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        కొరకు (పూర్ణాంకం i = 0; i < 3; i++)
                ఒకవేళ (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                లేకపోతే ఒకవేళ (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ఒకవేళ (copy_fd[i] < 0) {
                                r = -errno;
                                వెళ్ళు ముగింపు;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        కొరకు (పూర్ణాంకం i = 0; i < 3; i++)
                ఒకవేళ (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        ఒకవేళ (r < 0)
                                వెళ్ళు ముగింపు;
                } లేకపోతే {
                        ప్రతిజ్ఞా(fd[i] > 2);

                        ఒకవేళ (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                వెళ్ళు ముగింపు;
                        }
                }

        r = 0;

ముగింపు:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        సురక్షిత_close_above_stdio(original_ఆదానం_fd);
        ఒకవేళ (original_output_fd != original_ఆదానం_fd)
                సురక్షిత_close_above_stdio(original_output_fd);
        ఒకవేళ (original_error_fd != original_ఆదానం_fd && original_error_fd != original_output_fd)
                సురక్షిత_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        సురక్షిత_close_above_stdio(null_fd);

        తిరిగి r;
}

పూర్ణాంకం fd_reopen(పూర్ణాంకం fd, పూర్ణాంకం flags) {
        ప్రతిజ్ఞా(wildcard_fd_is_valid(fd));
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, O_CREAT));

        /* Reopens the specified fd with new flags. This is useful for convert an O_PATH fd into a regular one, or to
         * turn O_RDWR fds into O_RDONLY fds.
         *
         * This doesn't work on sockets (since they cannot be open()ed, ever).
         *
         * This implicitly resets the file read index to 0.
         *
         * If AT_FDCWD is specified as file descriptor gets an fd to the current cwd.
         *
         * If XAT_FDROOT is specified as fd get an fd to the root directory.
         *
         * If the specified file descriptor refers to a symlink via O_PATH, then this function cannot be used
         * to follow that symlink. Because we cannot have non-O_PATH fds to symlinks reopening it without
         * O_PATH will always result in -ELOOP. Or in other words: if you have an O_PATH fd to a symlink you
         * can reopen it only if you pass O_PATH again. */

        ఒకవేళ (FLAGS_సమితి(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                తిరిగి -ELOOP;

        ఒకవేళ (fd == XAT_FDROOT)
                తిరిగి RET_NERRNO(open("/", flags | O_DIRECTORY));

        ఒకవేళ (FLAGS_సమితి(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                తిరిగి RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        పూర్ణాంకం new_fd = open(FORMAT_PROC_FD_మార్గం(fd), flags);
        ఒకవేళ (new_fd < 0) {
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                తిరిగి proc_fd_enoent_errno();
        }

        తిరిగి new_fd;
}

పూర్ణాంకం fd_reopen_propagate_append_and_position(పూర్ణాంకం fd, పూర్ణాంకం flags) {
        /* Invokes fd_reopen(fd, flags), but propagates O_APPEND if set on original fd, and also tries to
         * keep current file position.
         *
         * You should use this if the original fd potentially is O_APPEND, otherwise we get rather
         * "unexpected" behavior. Unless you intentionally want to overwrite pre-existing data, and have
         * your output overwritten by the next user.
         *
         * Use case: "systemd-run --pty >> some-log".
         *
         * The "keep position" part is obviously nonsense for the O_APPEND case, but should reduce surprises
         * if someone carefully pre-positioned the passed in original input or non-append output FDs. */

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(!(flags & (O_APPEND|O_DIRECTORY)));

        పూర్ణాంకం existing_flags = fcntl(fd, F_GETFL);
        ఒకవేళ (existing_flags < 0)
                తిరిగి -errno;

        పూర్ణాంకం new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        ఒకవేళ (new_fd < 0)
                తిరిగి new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        ఒకవేళ (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                ఒకవేళ (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                లేకపోతే ఒకవేళ (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (దీర్ఘ దీర్ఘ) new_p, (దీర్ఘ దీర్ఘ) p);
        }

        తిరిగి new_fd;
}

పూర్ణాంకం fd_reopen_condition(
                పూర్ణాంకం fd,
                పూర్ణాంకం flags,
                పూర్ణాంకం ముసుగు_3,
                పూర్ణాంకం *ret_new_fd) {

        పూర్ణాంకం r, new_fd;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, O_CREAT));
        ప్రతిజ్ఞా(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        ఒకవేళ (r < 0)
                తిరిగి -errno;

        ఒకవేళ ((r & ముసుగు_3) == (flags & ముసుగు_3)) {
                *ret_new_fd = -EBADF;
                తిరిగి fd;
        }

        new_fd = fd_reopen(fd, flags);
        ఒకవేళ (new_fd < 0)
                తిరిగి new_fd;

        *ret_new_fd = new_fd;
        తిరిగి new_fd;
}

పూర్ణాంకం fd_is_opath(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = fcntl(fd, F_GETFL);
        ఒకవేళ (r < 0)
                తిరిగి -errno;

        తిరిగి FLAGS_సమితి(r, O_మార్గం);
}

పూర్ణాంకం fd_vet_accmode(పూర్ణాంకం fd, పూర్ణాంకం బహుళకం) {
        పూర్ణాంకం flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(లోపల_సమితి(బహుళకం, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        ఒకవేళ (flags < 0)
                తిరిగి -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        ఒకవేళ (FLAGS_సమితి(flags, O_DIRECTORY) && !FLAGS_సమితి(flags, O_TMPFILE))
                తిరిగి -EISDIR;

        ఒకవేళ (FLAGS_సమితి(flags, O_మార్గం))
                తిరిగి -EBADFD;

        flags &= O_ACCMODE_STRICT;

        ఒకవేళ (flags == బహుళకం)
                తిరిగి 1;

        ఒకవేళ (flags == O_RDWR)
                తిరిగి 0;

        తిరిగి -EPROTOTYPE;
}

పూర్ణాంకం fd_is_writable(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        ఒకవేళ (r >= 0)
                తిరిగి true;

        ఒకవేళ (లోపల_సమితి(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                తిరిగి false;

        తిరిగి r;
}

పూర్ణాంకం fd_verify_సురక్షిత_flags_నిండిన(పూర్ణాంకం fd, పూర్ణాంకం extra_flags) {
        పూర్ణాంకం flags, unexpected_flags;

        /* Check if an extrinsic fd is safe to work on (by a privileged service). This ensures that clients
         * can't trick a privileged service into giving access to a file the client doesn't already have
         * access to (especially via something like O_PATH).
         *
         * O_NOFOLLOW: For some reason the kernel will return this flag from fcntl(); it doesn't go away
         *             immediately after open(). It should have no effect whatsoever to an already-opened FD,
         *             and since we refuse O_PATH it should be safe.
         *
         * RAW_O_LARGEFILE: glibc secretly sets this and neglects to hide it from us if we call fcntl.
         *                  See comment in src/basic/include/fcntl.h for more details about this.
         *
         * If 'extra_flags' is specified as non-zero the included flags are also allowed.
         */

        ప్రతిజ్ఞా(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        ఒకవేళ (flags < 0)
                తిరిగి -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        ఒకవేళ (unexpected_flags != 0)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (గుర్తులేని) unexpected_flags);

        తిరిగి flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

గుర్తులేని read_nr_open(శూన్య) {
        _cleanup_free_ అక్షరం *nr_open = ఖాళీ;
        పూర్ణాంకం r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_పంక్తి_file("/proc/sys/fs/nr_open", &nr_open);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        లేకపోతే {
                గుర్తులేని v;

                r = సురక్షిత_atou(nr_open, &v);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                లేకపోతే
                        తిరిగి v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        తిరిగి NR_OPEN_DEFAULT;
}

పూర్ణాంకం fd_get_diskseq(పూర్ణాంకం fd, uint64_t *ret) {
        uint64_t diskseq;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        తిరిగి -errno;

                తిరిగి -EOPNOTSUPP;
        }

        *ret = diskseq;

        తిరిగి 0;
}

స్థిర bool is_literal_మూలం(స్థిరరాశి అక్షరం *p) {
        ఒకవేళ (!p)
                తిరిగి false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        తిరిగి n >= 1 && p[n] == 0;
}

పూర్ణాంకం మార్గం_is_మూలం_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));

        ఒకవేళ (dir_fd == XAT_FDROOT && isempty(మార్గం))
                తిరిగి true;

        ఒకవేళ (లోపల_సమితి(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_మూలం(మార్గం))
                తిరిగి true;

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        ఒకవేళ (!isempty(మార్గం)) {
                fd = xopenat(dir_fd, మార్గం, O_మార్గం|O_DIRECTORY|O_CLOEXEC);
                ఒకవేళ (fd == -ENOTDIR)
                        తిరిగి false; /* the root dir must be a dir */
                ఒకవేళ (fd < 0)
                        తిరిగి fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        తిరిగి fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

పూర్ణాంకం fds_inode_and_mount_same(పూర్ణాంకం fd1, పూర్ణాంకం fd2) {
        నిర్మాణం statx sx1, sx2;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(fd1));
        ప్రతిజ్ఞా(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ఖాళీ, AT_ఖాళీలు_మార్గం,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                తిరిగి true;

        r = xstatx(fd2, /* path = */ ఖాళీ, AT_ఖాళీలు_మార్గం,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = statx_mount_same(&sx1, &sx2);
        ఒకవేళ (r <= 0)
                తిరిగి r;

        తిరిగి statx_inode_same(&sx1, &sx2);
}

పూర్ణాంకం resolve_xat_fdroot(పూర్ణాంకం *fd, స్థిరరాశి అక్షరం **మార్గం, అక్షరం **ret_బఫరు) {
        ప్రతిజ్ఞా(fd);
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret_బఫరు);

        ఒకవేళ (*fd != XAT_FDROOT) {
                *ret_బఫరు = ఖాళీ;
                తిరిగి 0;
        }

        ఒకవేళ (isempty(*మార్గం)) {
                *మార్గం = "/";
                *ret_బఫరు = ఖాళీ;
        } లేకపోతే ఒకవేళ (!మార్గం_is_absolute(*మార్గం)) {
                అక్షరం *p = strjoin("/", *మార్గం);
                ఒకవేళ (!p)
                        తిరిగి -ENOMEM;

                *మార్గం = *ret_బఫరు = p;
        }

        *fd = AT_FDCWD;

        తిరిగి 1;
}

అక్షరం* format_proc_fd_మార్గం(అక్షరం buf[స్థిర PROC_FD_మార్గం_MAX], పూర్ణాంకం fd) {
        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_మార్గం_MAX, "/proc/self/fd/%i", fd));
        తిరిగి buf;
}

స్థిరరాశి అక్షరం* accmode_గమ్యం_string(పూర్ణాంకం flags) {
        ఎంపిక (flags & O_ACCMODE_STRICT) {
        స్థితి O_RDONLY:
                తిరిగి "ro";
        స్థితి O_WRONLY:
                తిరిగి "wo";
        స్థితి O_RDWR:
                తిరిగి "rw";
        అప్రమేయ:
                తిరిగి ఖాళీ;
        }
}

అక్షరం* format_proc_pid_fd_మార్గం(అక్షరం buf[స్థిర PROC_PID_FD_మార్గం_MAX], pid_t pid, పూర్ణాంకం fd) {
        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_మార్గం_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        తిరిగి buf;
}

పూర్ణాంకం proc_fd_enoent_errno(శూన్య) {
        పూర్ణాంకం r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        ఒకవేళ (r == 0)
                తిరిగి -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        ఒకవేళ (r > 0)
                తిరిగి -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        తిరిగి -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
