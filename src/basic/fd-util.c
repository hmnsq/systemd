/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/fs.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/kcmp.h>
#ಅನ್ತರ್ಭಾವ <sys/resource.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

ಪೂರ್ಣಾಙ್ಕ close_nointr(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (close(fd) >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        ಯದಿ (errno == EINTR)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ -errno;
}

ಪೂರ್ಣಾಙ್ಕ safe_close(ಪೂರ್ಣಾಙ್ಕ fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        ಯದಿ (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        ಪ್ರತ್ಯಾಗಮ -EBADF;
}

ಶೂನ್ಯ safe_close_pair(ಪೂರ್ಣಾಙ್ಕ p[ಸ್ಥಿರ 2]) {
        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                ಪ್ರತ್ಯಾಗಮ;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

ಶೂನ್ಯ close_many(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

ಶೂನ್ಯ close_many_unset(ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

ಶೂನ್ಯ close_many_and_free(ಪೂರ್ಣಾಙ್ಕ *fds, size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        close_many(fds, n_fds);
        ಮುಕ್ತ(fds);
}

ಪೂರ್ಣಾಙ್ಕ fclose_nointr(FILE *f) {
        ಪ್ರತಿಜ್ಞಾ(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        ಯದಿ (fclose(f) == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (errno == EINTR)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        ಯದಿ (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

DIR* safe_closedir(DIR *d) {

        ಯದಿ (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ fd_nonblock(ಪೂರ್ಣಾಙ್ಕ fd, bool nonblock) {
        ಪೂರ್ಣಾಙ್ಕ flags, nflags;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        ಯದಿ (flags < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        ಯದಿ (nflags == flags)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (fcntl(fd, F_SETFL, nflags) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಶೂನ್ಯ nonblock_resetp(ಪೂರ್ಣಾಙ್ಕ *fd) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        PROTECT_ERRNO;

        ಯದಿ (*fd >= 0)
                (ಶೂನ್ಯ) fd_nonblock(*fd, false);
}

ಪೂರ್ಣಾಙ್ಕ stdio_disable_nonblock(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ fd_cloexec(ಪೂರ್ಣಾಙ್ಕ fd, bool cloexec) {
        ಪೂರ್ಣಾಙ್ಕ flags, nflags;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        ಯದಿ (flags < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        ಯದಿ (nflags == flags)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

ಪೂರ್ಣಾಙ್ಕ fd_cloexec_many(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds, bool cloexec) {
        ಪೂರ್ಣಾಙ್ಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                ಯದಿ (*fd < 0) /* Skip gracefully over already invalidated fds */
                        ಅನುವರ್ತ;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ bool fd_in_set(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                ಯದಿ (*i < 0)
                        ಅನುವರ್ತ;

                ಯದಿ (*i == fd)
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ get_max_fd(ಶೂನ್ಯ) {
        ಸಂರಚನಾ rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        ಯದಿ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        ಯದಿ (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                ಪ್ರತ್ಯಾಗಮ FD_SETSIZE-1;

        ಯದಿ (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                ಪ್ರತ್ಯಾಗಮ INT_MAX;

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) (m - 1);
}

ಪೂರ್ಣಾಙ್ಕ close_all_fds_frugal(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except) {
        ಪೂರ್ಣಾಙ್ಕ max_fd, r = 0;

        ಪ್ರತಿಜ್ಞಾ(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        ಯದಿ (max_fd < 0)
                ಪ್ರತ್ಯಾಗಮ max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        ಯದಿ (max_fd > MAX_FD_LOOP_LIMIT)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                ಪೂರ್ಣಾಙ್ಕ q;

                ಯದಿ (fd_in_set(fd, except, n_except))
                        ಅನುವರ್ತ;

                q = close_nointr(fd);
                ಯದಿ (q != -EBADF)
                        RET_GATHER(r, q);
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ close_all_fds_special_case(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except) {
        ಪ್ರತಿಜ್ಞಾ(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        ಯದಿ (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        ವಿಕಲ್ಪ (n_except) {

        ಪಕ್ಷ 0:
                /* Close everything. Yay! */
                ಯದಿ (close_range(3, INT_MAX, 0) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ 1;

        ಪಕ್ಷ 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                ಯದಿ (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ 1;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ 0;
        }
}

ಪೂರ್ಣಾಙ್ಕ close_all_fds_without_malloc(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r > 0) /* special case worked! */
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ close_all_fds_frugal(except, n_except);
}

ಪೂರ್ಣಾಙ್ಕ close_all_fds(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r > 0) /* special case worked! */
                ಪ್ರತ್ಯಾಗಮ 0;

        _cleanup_free_ ಪೂರ್ಣಾಙ್ಕ *sorted_malloc = ರಿಕ್ತ;
        size_t n_sorted;
        ಪೂರ್ಣಾಙ್ಕ *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        ಪ್ರತಿಜ್ಞಾ(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        ಯದಿ (n_sorted > ALLOCA_MAX / ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(ಪೂರ್ಣಾಙ್ಕ, n_sorted);
        ಅನ್ಯಥಾ
                sorted = newa(ಪೂರ್ಣಾಙ್ಕ, n_sorted);

        ಯದಿ (!sorted) /* Fallback on OOM. */
                ಪ್ರತ್ಯಾಗಮ close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        ಪ್ರತಿ (size_t i = 0; i < n_sorted-1; i++) {
                ಪೂರ್ಣಾಙ್ಕ start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                ಪ್ರತಿಜ್ಞಾ(end >= start);

                ಯದಿ (end - start <= 1)
                        ಅನುವರ್ತ;

                /* Close everything between the start and end fds (both of which shall stay open) */
                ಯದಿ (close_range(start + 1, end - 1, 0) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        ಯದಿ (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pack_fds(ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds) {
        ಯದಿ (n_fds <= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        ಪ್ರತಿಜ್ಞಾ(fds);

        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ start = 0;;) {
                ಪೂರ್ಣಾಙ್ಕ restart_from = -1;

                ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = start; i < (ಪೂರ್ಣಾಙ್ಕ) n_fds; i++) {
                        ಪೂರ್ಣಾಙ್ಕ nfd;

                        /* Already at right index? */
                        ಯದಿ (fds[i] == i + 3)
                                ಅನುವರ್ತ;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        ಯದಿ (nfd < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        ಯದಿ (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                ಯದಿ (restart_from < 0)
                        ಭಙ್ಗ;

                start = restart_from;
        }

        ಪ್ರತಿಜ್ಞಾ(fds[0] == 3);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_validate(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಯದಿ (fcntl(fd, F_GETFD) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ same_fd(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b) {
        ಸಂರಚನಾ stat sta, stb;
        pid_t pid;
        ಪೂರ್ಣಾಙ್ಕ r, fa, fb;

        ಪ್ರತಿಜ್ಞಾ(a >= 0);
        ಪ್ರತಿಜ್ಞಾ(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        ಯದಿ (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ true;
        ಯದಿ (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        ಯದಿ (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } ಅನ್ಯಥಾ ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ !r;
        ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        ಯದಿ (fstat(a, &sta) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (fstat(b, &stb) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (!stat_inode_same(&sta, &stb))
                ಪ್ರತ್ಯಾಗಮ false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        ಯದಿ (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                ಪ್ರತ್ಯಾಗಮ false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        ಯದಿ (fa < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        fb = fcntl(b, F_GETFL);
        ಯದಿ (fb < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ fa == fb;
}

bool fdname_is_valid(ಧ್ರುವ ವರ್ಣ *s) {
        ಧ್ರುವ ವರ್ಣ *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (p = s; *p; p++) {
                ಯದಿ (*p < ' ')
                        ಪ್ರತ್ಯಾಗಮ false;
                ಯದಿ (*p >= 127)
                        ಪ್ರತ್ಯಾಗಮ false;
                ಯದಿ (*p == ':')
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ p - s <= FDNAME_MAX;
}

ಪೂರ್ಣಾಙ್ಕ fd_get_path(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));

        ಯದಿ (fd == AT_FDCWD)
                ಪ್ರತ್ಯಾಗಮ safe_getcwd(ret);
        ಯದಿ (fd == XAT_FDROOT)
                ಪ್ರತ್ಯಾಗಮ strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ proc_fd_enoent_errno();
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ move_fd(ಪೂರ್ಣಾಙ್ಕ from, ಪೂರ್ಣಾಙ್ಕ to, ಪೂರ್ಣಾಙ್ಕ cloexec) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        ಯದಿ (from < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;
        ಯದಿ (to < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಯದಿ (from == to) {

                ಯದಿ (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಪ್ರತ್ಯಾಗಮ to;
        }

        ಯದಿ (cloexec < 0) {
                ಪೂರ್ಣಾಙ್ಕ fl;

                fl = fcntl(from, F_GETFD, 0);
                ಯದಿ (fl < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿಜ್ಞಾ(r == to);

        safe_close(from);

        ಪ್ರತ್ಯಾಗಮ to;
}

ಪೂರ್ಣಾಙ್ಕ fd_move_above_stdio(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ flags, copy;
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

        ಯದಿ (fd < 0 || fd > 2)
                ಪ್ರತ್ಯಾಗಮ fd;

        flags = fcntl(fd, F_GETFD, 0);
        ಯದಿ (flags < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ಅನ್ಯಥಾ
                copy = fcntl(fd, F_DUPFD, 3);
        ಯದಿ (copy < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಪ್ರತಿಜ್ಞಾ(copy > 2);

        (ಶೂನ್ಯ) close(fd);
        ಪ್ರತ್ಯಾಗಮ copy;
}

ಪೂರ್ಣಾಙ್ಕ rearrange_stdio(ಪೂರ್ಣಾಙ್ಕ original_ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ original_output_fd, ಪೂರ್ಣಾಙ್ಕ original_error_fd) {
        ಪೂರ್ಣಾಙ್ಕ fd[3] = { original_ಒಳಹರಿವು_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_ಒಳಹರಿವು_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        ಯದಿ (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                ಯದಿ (null_fd < 0) {
                        r = -errno;
                        ಗಚ್ಛ finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                ಯದಿ (null_fd < 3) {
                        ಪೂರ್ಣಾಙ್ಕ copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ಯದಿ (copy < 0) {
                                r = -errno;
                                ಗಚ್ಛ finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++)
                ಯದಿ (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                ಅನ್ಯಥಾ ಯದಿ (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ಯದಿ (copy_fd[i] < 0) {
                                r = -errno;
                                ಗಚ್ಛ finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++)
                ಯದಿ (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        ಯದಿ (r < 0)
                                ಗಚ್ಛ finish;
                } ಅನ್ಯಥಾ {
                        ಪ್ರತಿಜ್ಞಾ(fd[i] > 2);

                        ಯದಿ (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                ಗಚ್ಛ finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_ಒಳಹರಿವು_fd);
        ಯದಿ (original_output_fd != original_ಒಳಹರಿವು_fd)
                safe_close_above_stdio(original_output_fd);
        ಯದಿ (original_error_fd != original_ಒಳಹರಿವು_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ fd_reopen(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, O_CREAT));

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

        ಯದಿ (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (fd == XAT_FDROOT)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(open("/", flags | O_DIRECTORY));

        ಯದಿ (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        ಪೂರ್ಣಾಙ್ಕ new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        ಯದಿ (new_fd < 0) {
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ proc_fd_enoent_errno();
        }

        ಪ್ರತ್ಯಾಗಮ new_fd;
}

ಪೂರ್ಣಾಙ್ಕ fd_reopen_propagate_append_and_position(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ flags) {
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

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(!(flags & (O_APPEND|O_DIRECTORY)));

        ಪೂರ್ಣಾಙ್ಕ existing_flags = fcntl(fd, F_GETFL);
        ಯದಿ (existing_flags < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪೂರ್ಣಾಙ್ಕ new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        ಯದಿ (new_fd < 0)
                ಪ್ರತ್ಯಾಗಮ new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        ಯದಿ (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                ಯದಿ (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                ಅನ್ಯಥಾ ಯದಿ (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        ಪ್ರತ್ಯಾಗಮ new_fd;
}

ಪೂರ್ಣಾಙ್ಕ fd_reopen_condition(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಪೂರ್ಣಾಙ್ಕ flags,
                ಪೂರ್ಣಾಙ್ಕ mask,
                ಪೂರ್ಣಾಙ್ಕ *ret_new_fd) {

        ಪೂರ್ಣಾಙ್ಕ r, new_fd;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, O_CREAT));
        ಪ್ರತಿಜ್ಞಾ(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                ಪ್ರತ್ಯಾಗಮ fd;
        }

        new_fd = fd_reopen(fd, flags);
        ಯದಿ (new_fd < 0)
                ಪ್ರತ್ಯಾಗಮ new_fd;

        *ret_new_fd = new_fd;
        ಪ್ರತ್ಯಾಗಮ new_fd;
}

ಪೂರ್ಣಾಙ್ಕ fd_is_opath(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fcntl(fd, F_GETFL);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(r, O_PATH);
}

ಪೂರ್ಣಾಙ್ಕ fd_vet_accmode(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ mode) {
        ಪೂರ್ಣಾಙ್ಕ flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        ಯದಿ (flags < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        ಯದಿ (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (FLAGS_SET(flags, O_PATH))
                ಪ್ರತ್ಯಾಗಮ -EBADFD;

        flags &= O_ACCMODE_STRICT;

        ಯದಿ (flags == mode)
                ಪ್ರತ್ಯಾಗಮ 1;

        ಯದಿ (flags == O_RDWR)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ -EPROTOTYPE;
}

ಪೂರ್ಣಾಙ್ಕ fd_is_writable(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_safe_flags_full(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ extra_flags) {
        ಪೂರ್ಣಾಙ್ಕ flags, unexpected_flags;

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

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        ಯದಿ (flags < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        ಯದಿ (unexpected_flags != 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        ಪ್ರತ್ಯಾಗಮ flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *nr_open = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_ಸಾಲು_file("/proc/sys/fs/nr_open", &nr_open);
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        ಅನ್ಯಥಾ {
                unsigned v;

                r = safe_atou(nr_open, &v);
                ಯದಿ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        ಪ್ರತ್ಯಾಗಮ NR_OPEN_DEFAULT;
}

ಪೂರ್ಣಾಙ್ಕ fd_get_diskseq(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        uint64_t diskseq;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }

        *ret = diskseq;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ bool is_literal_ಬೇರು(ಧ್ರುವ ವರ್ಣ *p) {
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        ಪ್ರತ್ಯಾಗಮ n >= 1 && p[n] == 0;
}

ಪೂರ್ಣಾಙ್ಕ path_is_ಬೇರು_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        ಯದಿ (dir_fd == XAT_FDROOT && isempty(path))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_ಬೇರು(path))
                ಪ್ರತ್ಯಾಗಮ true;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಯದಿ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ಯದಿ (fd == -ENOTDIR)
                        ಪ್ರತ್ಯಾಗಮ false; /* the root dir must be a dir */
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        ಪ್ರತ್ಯಾಗಮ fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

ಪೂರ್ಣಾಙ್ಕ fds_inode_and_mount_same(ಪೂರ್ಣಾಙ್ಕ fd1, ಪೂರ್ಣಾಙ್ಕ fd2) {
        ಸಂರಚನಾ statx sx1, sx2;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd1));
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ರಿಕ್ತ, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                ಪ್ರತ್ಯಾಗಮ true;

        r = xstatx(fd2, /* path = */ ರಿಕ್ತ, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = statx_mount_same(&sx1, &sx2);
        ಯದಿ (r <= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ statx_inode_same(&sx1, &sx2);
}

ಪೂರ್ಣಾಙ್ಕ resolve_xat_fdroot(ಪೂರ್ಣಾಙ್ಕ *fd, ಧ್ರುವ ವರ್ಣ **path, ವರ್ಣ **ret_buffer) {
        ಪ್ರತಿಜ್ಞಾ(fd);
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret_buffer);

        ಯದಿ (*fd != XAT_FDROOT) {
                *ret_buffer = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (isempty(*path)) {
                *path = "/";
                *ret_buffer = ರಿಕ್ತ;
        } ಅನ್ಯಥಾ ಯದಿ (!path_is_absolute(*path)) {
                ವರ್ಣ *p = strjoin("/", *path);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ವರ್ಣ* format_proc_fd_path(ವರ್ಣ buf[ಸ್ಥಿರ PROC_FD_PATH_MAX], ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        ಪ್ರತ್ಯಾಗಮ buf;
}

ಧ್ರುವ ವರ್ಣ* accmode_to_string(ಪೂರ್ಣಾಙ್ಕ flags) {
        ವಿಕಲ್ಪ (flags & O_ACCMODE_STRICT) {
        ಪಕ್ಷ O_RDONLY:
                ಪ್ರತ್ಯಾಗಮ "ro";
        ಪಕ್ಷ O_WRONLY:
                ಪ್ರತ್ಯಾಗಮ "wo";
        ಪಕ್ಷ O_RDWR:
                ಪ್ರತ್ಯಾಗಮ "rw";
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
}

ವರ್ಣ* format_proc_pid_fd_path(ವರ್ಣ buf[ಸ್ಥಿರ PROC_PID_FD_PATH_MAX], pid_t pid, ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        ಪ್ರತ್ಯಾಗಮ buf;
}

ಪೂರ್ಣಾಙ್ಕ proc_fd_enoent_errno(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        ಪ್ರತ್ಯಾಗಮ -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
