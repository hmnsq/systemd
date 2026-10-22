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
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FD_LOOP_ಎಲ್ಲೆ (1024*1024)

ಪೂರ್ಣಾಂಕ close_nointr(ಪೂರ್ಣಾಂಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (close(fd) >= 0)
                ಹಿಂತಿರುಗಿ 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        ಆದರೆ (errno == EINTR)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ -errno;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_close(ಪೂರ್ಣಾಂಕ fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        ಆದರೆ (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        ಹಿಂತಿರುಗಿ -EBADF;
}

ಶೂನ್ಯ ಸುರಕ್ಷಿತ_close_pair(ಪೂರ್ಣಾಂಕ p[ಸ್ಥಿರ 2]) {
        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = ಸುರಕ್ಷಿತ_close(p[0]);
                ಹಿಂತಿರುಗಿ;
        }

        p[0] = ಸುರಕ್ಷಿತ_close(p[0]);
        p[1] = ಸುರಕ್ಷಿತ_close(p[1]);
}

ಶೂನ್ಯ close_many(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fds[], size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                ಸುರಕ್ಷಿತ_close(*fd);
}

ಶೂನ್ಯ close_many_unset(ಪೂರ್ಣಾಂಕ fds[], size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = ಸುರಕ್ಷಿತ_close(*fd);
}

ಶೂನ್ಯ close_many_and_free(ಪೂರ್ಣಾಂಕ *fds, size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        close_many(fds, n_fds);
        ಬಿಡುಗಡೆ(fds);
}

ಪೂರ್ಣಾಂಕ fclose_nointr(FILE *f) {
        ಪ್ರತಿಜ್ಞಾ(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        ಆದರೆ (fclose(f) == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (errno == EINTR)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ errno_or_else(EIO);
}

FILE* ಸುರಕ್ಷಿತ_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        ಆದರೆ (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

DIR* ಸುರಕ್ಷಿತ_closedir(DIR *d) {

        ಆದರೆ (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ fd_nonblock(ಪೂರ್ಣಾಂಕ fd, bool nonblock) {
        ಪೂರ್ಣಾಂಕ flags, nflags;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        ಆದರೆ (flags < 0)
                ಹಿಂತಿರುಗಿ -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        ಆದರೆ (nflags == flags)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (fcntl(fd, F_SETFL, nflags) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ nonblock_resetp(ಪೂರ್ಣಾಂಕ *fd) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        PROTECT_ERRNO;

        ಆದರೆ (*fd >= 0)
                (ಶೂನ್ಯ) fd_nonblock(*fd, false);
}

ಪೂರ್ಣಾಂಕ stdio_disable_nonblock(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ fd_cloexec(ಪೂರ್ಣಾಂಕ fd, bool cloexec) {
        ಪೂರ್ಣಾಂಕ flags, nflags;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        ಆದರೆ (flags < 0)
                ಹಿಂತಿರುಗಿ -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        ಆದರೆ (nflags == flags)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

ಪೂರ್ಣಾಂಕ fd_cloexec_many(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fds[], size_t n_fds, bool cloexec) {
        ಪೂರ್ಣಾಂಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                ಆದರೆ (*fd < 0) /* Skip gracefully over already invalidated fds */
                        ಮುಂದುವರಿ;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ bool fd_ಒಳ_ಗಣ(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fds[], size_t n_fds) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                ಆದರೆ (*i < 0)
                        ಮುಂದುವರಿ;

                ಆದರೆ (*i == fd)
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ get_max_fd(ಶೂನ್ಯ) {
        ರಚನೆ rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        ಆದರೆ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        ಆದರೆ (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                ಹಿಂತಿರುಗಿ FD_SETSIZE-1;

        ಆದರೆ (m == RLIM_INFINITY || m > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                ಹಿಂತಿರುಗಿ ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) (m - 1);
}

ಪೂರ್ಣಾಂಕ close_all_fds_frugal(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except) {
        ಪೂರ್ಣಾಂಕ max_fd, r = 0;

        ಪ್ರತಿಜ್ಞಾ(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        ಆದರೆ (max_fd < 0)
                ಹಿಂತಿರುಗಿ max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        ಆದರೆ (max_fd > MAX_FD_LOOP_ಎಲ್ಲೆ)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                ಪೂರ್ಣಾಂಕ q;

                ಆದರೆ (fd_ಒಳ_ಗಣ(fd, except, n_except))
                        ಮುಂದುವರಿ;

                q = close_nointr(fd);
                ಆದರೆ (q != -EBADF)
                        RET_GATHER(r, q);
        }

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ close_all_fds_special_case(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except) {
        ಪ್ರತಿಜ್ಞಾ(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        ಆದರೆ (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        ಆಯ್ಕೆ (n_except) {

        ಸ್ಥಿತಿ 0:
                /* Close everything. Yay! */
                ಆದರೆ (close_range(3, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ, 0) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ 1;

        ಸ್ಥಿತಿ 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                ಆದರೆ (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (except[0] < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ 1;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ 0;
        }
}

ಪೂರ್ಣಾಂಕ close_all_fds_without_malloc(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r > 0) /* special case worked! */
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ close_all_fds_frugal(except, n_except);
}

ಪೂರ್ಣಾಂಕ close_all_fds(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r > 0) /* special case worked! */
                ಹಿಂತಿರುಗಿ 0;

        _cleanup_free_ ಪೂರ್ಣಾಂಕ *sorted_malloc = ಖಾಲಿ;
        size_t n_sorted;
        ಪೂರ್ಣಾಂಕ *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        ಪ್ರತಿಜ್ಞಾ(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        ಆದರೆ (n_sorted > ALLOCA_MAX / ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(ಪೂರ್ಣಾಂಕ, n_sorted);
        ಇಲ್ಲದಿದ್ದರೆ
                sorted = newa(ಪೂರ್ಣಾಂಕ, n_sorted);

        ಆದರೆ (!sorted) /* Fallback on OOM. */
                ಹಿಂತಿರುಗಿ close_all_fds_frugal(except, n_except);

        ಸ್ಮರಣೆ_ನಕಲು(sorted, except, n_except * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        ಗಾಗಿ (size_t i = 0; i < n_sorted-1; i++) {
                ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಕೊನೆ;

                ಆರಂಭ = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                ಕೊನೆ = MAX(sorted[i+1], 2);

                ಪ್ರತಿಜ್ಞಾ(ಕೊನೆ >= ಆರಂಭ);

                ಆದರೆ (ಕೊನೆ - ಆರಂಭ <= 1)
                        ಮುಂದುವರಿ;

                /* Close everything between the start and end fds (both of which shall stay open) */
                ಆದರೆ (close_range(ಆರಂಭ + 1, ಕೊನೆ - 1, 0) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        ಆದರೆ (sorted[n_sorted-1] >= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* Dont let the addition below overflow */
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (close_range(sorted[n_sorted-1] + 1, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ, 0) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pack_fds(ಪೂರ್ಣಾಂಕ fds[], size_t n_fds) {
        ಆದರೆ (n_fds <= 0)
                ಹಿಂತಿರುಗಿ 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        ಪ್ರತಿಜ್ಞಾ(fds);

        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಆರಂಭ = 0;;) {
                ಪೂರ್ಣಾಂಕ restart_from = -1;

                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಆರಂಭ; i < (ಪೂರ್ಣಾಂಕ) n_fds; i++) {
                        ಪೂರ್ಣಾಂಕ nfd;

                        /* Already at right index? */
                        ಆದರೆ (fds[i] == i + 3)
                                ಮುಂದುವರಿ;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        ಆದರೆ (nfd < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        ಸುರಕ್ಷಿತ_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        ಆದರೆ (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                ಆದರೆ (restart_from < 0)
                        ನಿಲ್ಲಿಸು;

                ಆರಂಭ = restart_from;
        }

        ಪ್ರತಿಜ್ಞಾ(fds[0] == 3);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_validate(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        ಆದರೆ (fcntl(fd, F_GETFD) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ same_fd(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b) {
        ರಚನೆ stat sta, stb;
        pid_t pid;
        ಪೂರ್ಣಾಂಕ r, fa, fb;

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

        ಆದರೆ (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ true;
        ಆದರೆ (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        ಆದರೆ (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                ಹಿಂತಿರುಗಿ -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ !r;
        ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                ಹಿಂತಿರುಗಿ -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        ಆದರೆ (fstat(a, &sta) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (fstat(b, &stb) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (!stat_inode_same(&sta, &stb))
                ಹಿಂತಿರುಗಿ false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        ಆದರೆ (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                ಹಿಂತಿರುಗಿ false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        ಆದರೆ (fa < 0)
                ಹಿಂತಿರುಗಿ -errno;

        fb = fcntl(b, F_GETFL);
        ಆದರೆ (fb < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ fa == fb;
}

bool fdname_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (p = s; *p; p++) {
                ಆದರೆ (*p < ' ')
                        ಹಿಂತಿರುಗಿ false;
                ಆದರೆ (*p >= 127)
                        ಹಿಂತಿರುಗಿ false;
                ಆದರೆ (*p == ':')
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ p - s <= FDNAME_MAX;
}

ಪೂರ್ಣಾಂಕ fd_get_ಮಾರ್ಗ(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));

        ಆದರೆ (fd == AT_FDCWD)
                ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_getcwd(ret);
        ಆದರೆ (fd == XAT_FDROOT)
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), ret);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ proc_fd_enoent_errno();
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ move_fd(ಪೂರ್ಣಾಂಕ from, ಪೂರ್ಣಾಂಕ to, ಪೂರ್ಣಾಂಕ cloexec) {
        ಪೂರ್ಣಾಂಕ r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        ಆದರೆ (from < 0)
                ಹಿಂತಿರುಗಿ -EBADF;
        ಆದರೆ (to < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        ಆದರೆ (from == to) {

                ಆದರೆ (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಹಿಂತಿರುಗಿ to;
        }

        ಆದರೆ (cloexec < 0) {
                ಪೂರ್ಣಾಂಕ fl;

                fl = fcntl(from, F_GETFD, 0);
                ಆದರೆ (fl < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                cloexec = FLAGS_ಗಣ(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಪ್ರತಿಜ್ಞಾ(r == to);

        ಸುರಕ್ಷಿತ_close(from);

        ಹಿಂತಿರುಗಿ to;
}

ಪೂರ್ಣಾಂಕ fd_move_above_stdio(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ flags, copy;
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

        ಆದರೆ (fd < 0 || fd > 2)
                ಹಿಂತಿರುಗಿ fd;

        flags = fcntl(fd, F_GETFD, 0);
        ಆದರೆ (flags < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ಇಲ್ಲದಿದ್ದರೆ
                copy = fcntl(fd, F_DUPFD, 3);
        ಆದರೆ (copy < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಪ್ರತಿಜ್ಞಾ(copy > 2);

        (ಶೂನ್ಯ) close(fd);
        ಹಿಂತಿರುಗಿ copy;
}

ಪೂರ್ಣಾಂಕ rearrange_stdio(ಪೂರ್ಣಾಂಕ original_ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ original_output_fd, ಪೂರ್ಣಾಂಕ original_error_fd) {
        ಪೂರ್ಣಾಂಕ fd[3] = { original_ಒಳಹರಿವು_fd,             /* Put together an array of fds we work on */
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
        ಆದರೆ (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                ಆದರೆ (null_fd < 0) {
                        r = -errno;
                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                ಆದರೆ (null_fd < 3) {
                        ಪೂರ್ಣಾಂಕ copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ಆದರೆ (copy < 0) {
                                r = -errno;
                                ಹೋಗು ಮುಕ್ತಾಯ;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++)
                ಆದರೆ (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ಆದರೆ (copy_fd[i] < 0) {
                                r = -errno;
                                ಹೋಗು ಮುಕ್ತಾಯ;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++)
                ಆದರೆ (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        ಆದರೆ (r < 0)
                                ಹೋಗು ಮುಕ್ತಾಯ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಪ್ರತಿಜ್ಞಾ(fd[i] > 2);

                        ಆದರೆ (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                ಹೋಗು ಮುಕ್ತಾಯ;
                        }
                }

        r = 0;

ಮುಕ್ತಾಯ:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        ಸುರಕ್ಷಿತ_close_above_stdio(original_ಒಳಹರಿವು_fd);
        ಆದರೆ (original_output_fd != original_ಒಳಹರಿವು_fd)
                ಸುರಕ್ಷಿತ_close_above_stdio(original_output_fd);
        ಆದರೆ (original_error_fd != original_ಒಳಹರಿವು_fd && original_error_fd != original_output_fd)
                ಸುರಕ್ಷಿತ_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        ಸುರಕ್ಷಿತ_close_above_stdio(null_fd);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ fd_reopen(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ flags) {
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, O_CREAT));

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

        ಆದರೆ (FLAGS_ಗಣ(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (fd == XAT_FDROOT)
                ಹಿಂತಿರುಗಿ RET_NERRNO(open("/", flags | O_DIRECTORY));

        ಆದರೆ (FLAGS_ಗಣ(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                ಹಿಂತಿರುಗಿ RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        ಪೂರ್ಣಾಂಕ new_fd = open(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), flags);
        ಆದರೆ (new_fd < 0) {
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ proc_fd_enoent_errno();
        }

        ಹಿಂತಿರುಗಿ new_fd;
}

ಪೂರ್ಣಾಂಕ fd_reopen_propagate_append_and_position(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ flags) {
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

        ಪೂರ್ಣಾಂಕ existing_flags = fcntl(fd, F_GETFL);
        ಆದರೆ (existing_flags < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಪೂರ್ಣಾಂಕ new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        ಆದರೆ (new_fd < 0)
                ಹಿಂತಿರುಗಿ new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        ಆದರೆ (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                ಆದರೆ (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (ದೀರ್ಘ ದೀರ್ಘ) new_p, (ದೀರ್ಘ ದೀರ್ಘ) p);
        }

        ಹಿಂತಿರುಗಿ new_fd;
}

ಪೂರ್ಣಾಂಕ fd_reopen_condition(
                ಪೂರ್ಣಾಂಕ fd,
                ಪೂರ್ಣಾಂಕ flags,
                ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_3,
                ಪೂರ್ಣಾಂಕ *ret_new_fd) {

        ಪೂರ್ಣಾಂಕ r, new_fd;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, O_CREAT));
        ಪ್ರತಿಜ್ಞಾ(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ ((r & ಮುಖವಾಡ_3) == (flags & ಮುಖವಾಡ_3)) {
                *ret_new_fd = -EBADF;
                ಹಿಂತಿರುಗಿ fd;
        }

        new_fd = fd_reopen(fd, flags);
        ಆದರೆ (new_fd < 0)
                ಹಿಂತಿರುಗಿ new_fd;

        *ret_new_fd = new_fd;
        ಹಿಂತಿರುಗಿ new_fd;
}

ಪೂರ್ಣಾಂಕ fd_is_opath(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fcntl(fd, F_GETFL);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(r, O_ಮಾರ್ಗ);
}

ಪೂರ್ಣಾಂಕ fd_vet_accmode(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ) {
        ಪೂರ್ಣಾಂಕ flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(ಬಹುಲಕ, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        ಆದರೆ (flags < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        ಆದರೆ (FLAGS_ಗಣ(flags, O_DIRECTORY) && !FLAGS_ಗಣ(flags, O_TMPFILE))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (FLAGS_ಗಣ(flags, O_ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EBADFD;

        flags &= O_ACCMODE_STRICT;

        ಆದರೆ (flags == ಬಹುಲಕ)
                ಹಿಂತಿರುಗಿ 1;

        ಆದರೆ (flags == O_RDWR)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ -EPROTOTYPE;
}

ಪೂರ್ಣಾಂಕ fd_is_writable(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಒಳ_ಗಣ(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ fd_verify_ಸುರಕ್ಷಿತ_flags_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ extra_flags) {
        ಪೂರ್ಣಾಂಕ flags, unexpected_flags;

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
        ಆದರೆ (flags < 0)
                ಹಿಂತಿರುಗಿ -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        ಆದರೆ (unexpected_flags != 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (ಚಿಹ್ನೆರಹಿತ) unexpected_flags);

        ಹಿಂತಿರುಗಿ flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

ಚಿಹ್ನೆರಹಿತ read_nr_open(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *nr_open = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_ಸಾಲು_file("/proc/sys/fs/nr_open", &nr_open);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        ಇಲ್ಲದಿದ್ದರೆ {
                ಚಿಹ್ನೆರಹಿತ v;

                r = ಸುರಕ್ಷಿತ_atou(nr_open, &v);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        ಹಿಂತಿರುಗಿ NR_OPEN_DEFAULT;
}

ಪೂರ್ಣಾಂಕ fd_get_diskseq(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        uint64_t diskseq;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }

        *ret = diskseq;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ bool is_literal_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        ಹಿಂತಿರುಗಿ n >= 1 && p[n] == 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_ಬೇರು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        ಆದರೆ (dir_fd == XAT_FDROOT && isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಒಳ_ಗಣ(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_ಬೇರು(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ true;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಆದರೆ (!isempty(ಮಾರ್ಗ)) {
                fd = xopenat(dir_fd, ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_DIRECTORY|O_CLOEXEC);
                ಆದರೆ (fd == -ENOTDIR)
                        ಹಿಂತಿರುಗಿ false; /* the root dir must be a dir */
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        ಹಿಂತಿರುಗಿ fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

ಪೂರ್ಣಾಂಕ fds_inode_and_mount_same(ಪೂರ್ಣಾಂಕ fd1, ಪೂರ್ಣಾಂಕ fd2) {
        ರಚನೆ statx sx1, sx2;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd1));
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ಖಾಲಿ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                ಹಿಂತಿರುಗಿ true;

        r = xstatx(fd2, /* path = */ ಖಾಲಿ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = statx_mount_same(&sx1, &sx2);
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ statx_inode_same(&sx1, &sx2);
}

ಪೂರ್ಣಾಂಕ resolve_xat_fdroot(ಪೂರ್ಣಾಂಕ *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮಾರ್ಗ, ಅಕ್ಷರ **ret_ಬಫರ್) {
        ಪ್ರತಿಜ್ಞಾ(fd);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret_ಬಫರ್);

        ಆದರೆ (*fd != XAT_FDROOT) {
                *ret_ಬಫರ್ = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (isempty(*ಮಾರ್ಗ)) {
                *ಮಾರ್ಗ = "/";
                *ret_ಬಫರ್ = ಖಾಲಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಮಾರ್ಗ_is_absolute(*ಮಾರ್ಗ)) {
                ಅಕ್ಷರ *p = strjoin("/", *ಮಾರ್ಗ);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *ಮಾರ್ಗ = *ret_ಬಫರ್ = p;
        }

        *fd = AT_FDCWD;

        ಹಿಂತಿರುಗಿ 1;
}

ಅಕ್ಷರ* format_proc_fd_ಮಾರ್ಗ(ಅಕ್ಷರ buf[ಸ್ಥಿರ PROC_FD_ಮಾರ್ಗ_MAX], ಪೂರ್ಣಾಂಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_ಮಾರ್ಗ_MAX, "/proc/self/fd/%i", fd));
        ಹಿಂತಿರುಗಿ buf;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* accmode_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ flags) {
        ಆಯ್ಕೆ (flags & O_ACCMODE_STRICT) {
        ಸ್ಥಿತಿ O_RDONLY:
                ಹಿಂತಿರುಗಿ "ro";
        ಸ್ಥಿತಿ O_WRONLY:
                ಹಿಂತಿರುಗಿ "wo";
        ಸ್ಥಿತಿ O_RDWR:
                ಹಿಂತಿರುಗಿ "rw";
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
}

ಅಕ್ಷರ* format_proc_pid_fd_ಮಾರ್ಗ(ಅಕ್ಷರ buf[ಸ್ಥಿರ PROC_PID_FD_ಮಾರ್ಗ_MAX], pid_t pid, ಪೂರ್ಣಾಂಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_ಮಾರ್ಗ_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        ಹಿಂತಿರುಗಿ buf;
}

ಪೂರ್ಣಾಂಕ proc_fd_enoent_errno(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        ಹಿಂತಿರುಗಿ -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
