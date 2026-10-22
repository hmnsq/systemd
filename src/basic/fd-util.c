/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/fs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/kcmp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/resource.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FD_LOOP_ਹੱਦਬੰਦੀ (1024*1024)

ਪੂਰਨ close_nointr(ਪੂਰਨ fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (close(fd) >= 0)
                ਵਾਪਸ 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        ਜੇ (errno == EINTR)
                ਵਾਪਸ 0;

        ਵਾਪਸ -errno;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_close(ਪੂਰਨ fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        ਜੇ (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        ਵਾਪਸ -EBADF;
}

ਖਾਲੀ ਸੁਰੱਖਿਅਤ_close_pair(ਪੂਰਨ p[ਸਥਿਰ 2]) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = ਸੁਰੱਖਿਅਤ_close(p[0]);
                ਵਾਪਸ;
        }

        p[0] = ਸੁਰੱਖਿਅਤ_close(p[0]);
        p[1] = ਸੁਰੱਖਿਅਤ_close(p[1]);
}

ਖਾਲੀ close_many(ਅਚਲ ਪੂਰਨ fds[], size_t n_fds) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                ਸੁਰੱਖਿਅਤ_close(*fd);
}

ਖਾਲੀ close_many_unset(ਪੂਰਨ fds[], size_t n_fds) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = ਸੁਰੱਖਿਅਤ_close(*fd);
}

ਖਾਲੀ close_many_and_free(ਪੂਰਨ *fds, size_t n_fds) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fds || n_fds == 0);

        close_many(fds, n_fds);
        ਮੁਕਤ(fds);
}

ਪੂਰਨ fclose_nointr(FILE *f) {
        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        ਜੇ (fclose(f) == 0)
                ਵਾਪਸ 0;

        ਜੇ (errno == EINTR)
                ਵਾਪਸ 0;

        ਵਾਪਸ errno_or_else(EIO);
}

FILE* ਸੁਰੱਖਿਅਤ_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        ਜੇ (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        ਵਾਪਸ ਨਿਰਾ;
}

DIR* ਸੁਰੱਖਿਅਤ_closedir(DIR *d) {

        ਜੇ (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ fd_nonblock(ਪੂਰਨ fd, bool nonblock) {
        ਪੂਰਨ flags, nflags;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        ਜੇ (flags < 0)
                ਵਾਪਸ -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        ਜੇ (nflags == flags)
                ਵਾਪਸ 0;

        ਜੇ (fcntl(fd, F_SETFL, nflags) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 1;
}

ਖਾਲੀ nonblock_resetp(ਪੂਰਨ *fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd);

        PROTECT_ERRNO;

        ਜੇ (*fd >= 0)
                (ਖਾਲੀ) fd_nonblock(*fd, false);
}

ਪੂਰਨ stdio_disable_nonblock(ਖਾਲੀ) {
        ਪੂਰਨ ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        ਵਾਪਸ ret;
}

ਪੂਰਨ fd_cloexec(ਪੂਰਨ fd, bool cloexec) {
        ਪੂਰਨ flags, nflags;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        ਜੇ (flags < 0)
                ਵਾਪਸ -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        ਜੇ (nflags == flags)
                ਵਾਪਸ 0;

        ਵਾਪਸ RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

ਪੂਰਨ fd_cloexec_many(ਅਚਲ ਪੂਰਨ fds[], size_t n_fds, bool cloexec) {
        ਪੂਰਨ r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                ਜੇ (*fd < 0) /* Skip gracefully over already invalidated fds */
                        ਜਾਰੀ;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        ਵਾਪਸ r;
}

ਸਥਿਰ bool fd_ਵਿੱਚ_ਸੈੱਟ(ਪੂਰਨ fd, ਅਚਲ ਪੂਰਨ fds[], size_t n_fds) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                ਜੇ (*i < 0)
                        ਜਾਰੀ;

                ਜੇ (*i == fd)
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਪੂਰਨ get_max_fd(ਖਾਲੀ) {
        ਬਣਤਰ rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        ਜੇ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ਵਾਪਸ -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        ਜੇ (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                ਵਾਪਸ FD_SETSIZE-1;

        ਜੇ (m == RLIM_INFINITY || m > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                ਵਾਪਸ ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

        ਵਾਪਸ (ਪੂਰਨ) (m - 1);
}

ਪੂਰਨ close_all_fds_frugal(ਅਚਲ ਪੂਰਨ except[], size_t n_except) {
        ਪੂਰਨ max_fd, r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        ਜੇ (max_fd < 0)
                ਵਾਪਸ max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        ਜੇ (max_fd > MAX_FD_LOOP_ਹੱਦਬੰਦੀ)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        ਲਈ (ਪੂਰਨ fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                ਪੂਰਨ q;

                ਜੇ (fd_ਵਿੱਚ_ਸੈੱਟ(fd, except, n_except))
                        ਜਾਰੀ;

                q = close_nointr(fd);
                ਜੇ (q != -EBADF)
                        RET_GATHER(r, q);
        }

        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ close_all_fds_special_case(ਅਚਲ ਪੂਰਨ except[], size_t n_except) {
        ਪ੍ਰਤਿਜ੍ਞਾ(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        ਜੇ (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        ਚੋਣ (n_except) {

        ਹਾਲਤ 0:
                /* Close everything. Yay! */
                ਜੇ (close_range(3, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ, 0) < 0)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ 1;

        ਹਾਲਤ 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                ਜੇ (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (except[0] < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ 1;

        ਮੂਲ:
                ਵਾਪਸ 0;
        }
}

ਪੂਰਨ close_all_fds_without_malloc(ਅਚਲ ਪੂਰਨ except[], size_t n_except) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r > 0) /* special case worked! */
                ਵਾਪਸ 0;

        ਵਾਪਸ close_all_fds_frugal(except, n_except);
}

ਪੂਰਨ close_all_fds(ਅਚਲ ਪੂਰਨ except[], size_t n_except) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r > 0) /* special case worked! */
                ਵਾਪਸ 0;

        _cleanup_free_ ਪੂਰਨ *sorted_malloc = ਨਿਰਾ;
        size_t n_sorted;
        ਪੂਰਨ *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        ਪ੍ਰਤਿਜ੍ਞਾ(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        ਜੇ (n_sorted > ALLOCA_MAX / ਆਕਾਰ(ਪੂਰਨ)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(ਪੂਰਨ, n_sorted);
        ਨਹੀਂਤਾਂ
                sorted = newa(ਪੂਰਨ, n_sorted);

        ਜੇ (!sorted) /* Fallback on OOM. */
                ਵਾਪਸ close_all_fds_frugal(except, n_except);

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(sorted, except, n_except * ਆਕਾਰ(ਪੂਰਨ));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        ਲਈ (size_t i = 0; i < n_sorted-1; i++) {
                ਪੂਰਨ ਸ਼ੁਰੂ, ਸਿਰਾ;

                ਸ਼ੁਰੂ = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                ਸਿਰਾ = MAX(sorted[i+1], 2);

                ਪ੍ਰਤਿਜ੍ਞਾ(ਸਿਰਾ >= ਸ਼ੁਰੂ);

                ਜੇ (ਸਿਰਾ - ਸ਼ੁਰੂ <= 1)
                        ਜਾਰੀ;

                /* Close everything between the start and end fds (both of which shall stay open) */
                ਜੇ (close_range(ਸ਼ੁਰੂ + 1, ਸਿਰਾ - 1, 0) < 0)
                        ਵਾਪਸ -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        ਜੇ (sorted[n_sorted-1] >= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* Dont let the addition below overflow */
                ਵਾਪਸ 0;

        ਜੇ (close_range(sorted[n_sorted-1] + 1, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ, 0) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

ਪੂਰਨ pack_fds(ਪੂਰਨ fds[], size_t n_fds) {
        ਜੇ (n_fds <= 0)
                ਵਾਪਸ 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        ਪ੍ਰਤਿਜ੍ਞਾ(fds);

        ਲਈ (ਪੂਰਨ ਸ਼ੁਰੂ = 0;;) {
                ਪੂਰਨ restart_from = -1;

                ਲਈ (ਪੂਰਨ i = ਸ਼ੁਰੂ; i < (ਪੂਰਨ) n_fds; i++) {
                        ਪੂਰਨ nfd;

                        /* Already at right index? */
                        ਜੇ (fds[i] == i + 3)
                                ਜਾਰੀ;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        ਜੇ (nfd < 0)
                                ਵਾਪਸ -errno;

                        ਸੁਰੱਖਿਅਤ_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        ਜੇ (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                ਜੇ (restart_from < 0)
                        ਰੋਕੋ;

                ਸ਼ੁਰੂ = restart_from;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(fds[0] == 3);

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_validate(ਪੂਰਨ fd) {
        ਜੇ (fd < 0)
                ਵਾਪਸ -EBADF;

        ਜੇ (fcntl(fd, F_GETFD) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

ਪੂਰਨ same_fd(ਪੂਰਨ a, ਪੂਰਨ b) {
        ਬਣਤਰ stat sta, stb;
        pid_t pid;
        ਪੂਰਨ r, fa, fb;

        ਪ੍ਰਤਿਜ੍ਞਾ(a >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        ਜੇ (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        ਜੇ (r > 0)
                ਵਾਪਸ true;
        ਜੇ (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        ਜੇ (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } ਨਹੀਂਤਾਂ ਜੇ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                ਵਾਪਸ -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        ਜੇ (r >= 0)
                ਵਾਪਸ !r;
        ਜੇ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                ਵਾਪਸ -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        ਜੇ (fstat(a, &sta) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (fstat(b, &stb) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (!stat_inode_same(&sta, &stb))
                ਵਾਪਸ false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        ਜੇ (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                ਵਾਪਸ false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        ਜੇ (fa < 0)
                ਵਾਪਸ -errno;

        fb = fcntl(b, F_GETFL);
        ਜੇ (fb < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ fa == fb;
}

bool fdname_is_valid(ਅਚਲ ਅੱਖਰ *s) {
        ਅਚਲ ਅੱਖਰ *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        ਜੇ (!s)
                ਵਾਪਸ false;

        ਲਈ (p = s; *p; p++) {
                ਜੇ (*p < ' ')
                        ਵਾਪਸ false;
                ਜੇ (*p >= 127)
                        ਵਾਪਸ false;
                ਜੇ (*p == ':')
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ p - s <= FDNAME_MAX;
}

ਪੂਰਨ fd_get_ਰਸਤਾ(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(fd));

        ਜੇ (fd == AT_FDCWD)
                ਵਾਪਸ ਸੁਰੱਖਿਅਤ_getcwd(ret);
        ਜੇ (fd == XAT_FDROOT)
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_ਰਸਤਾ(fd), ret);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ proc_fd_enoent_errno();
        ਵਾਪਸ r;
}

ਪੂਰਨ move_fd(ਪੂਰਨ from, ਪੂਰਨ to, ਪੂਰਨ cloexec) {
        ਪੂਰਨ r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        ਜੇ (from < 0)
                ਵਾਪਸ -EBADF;
        ਜੇ (to < 0)
                ਵਾਪਸ -EBADF;

        ਜੇ (from == to) {

                ਜੇ (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਵਾਪਸ to;
        }

        ਜੇ (cloexec < 0) {
                ਪੂਰਨ fl;

                fl = fcntl(from, F_GETFD, 0);
                ਜੇ (fl < 0)
                        ਵਾਪਸ -errno;

                cloexec = FLAGS_ਸੈੱਟ(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        ਜੇ (r < 0)
                ਵਾਪਸ -errno;

        ਪ੍ਰਤਿਜ੍ਞਾ(r == to);

        ਸੁਰੱਖਿਅਤ_close(from);

        ਵਾਪਸ to;
}

ਪੂਰਨ fd_move_above_stdio(ਪੂਰਨ fd) {
        ਪੂਰਨ flags, copy;
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

        ਜੇ (fd < 0 || fd > 2)
                ਵਾਪਸ fd;

        flags = fcntl(fd, F_GETFD, 0);
        ਜੇ (flags < 0)
                ਵਾਪਸ fd;

        ਜੇ (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ਨਹੀਂਤਾਂ
                copy = fcntl(fd, F_DUPFD, 3);
        ਜੇ (copy < 0)
                ਵਾਪਸ fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(copy > 2);

        (ਖਾਲੀ) close(fd);
        ਵਾਪਸ copy;
}

ਪੂਰਨ rearrange_stdio(ਪੂਰਨ original_ਨਿਵੇਸ਼_fd, ਪੂਰਨ original_output_fd, ਪੂਰਨ original_error_fd) {
        ਪੂਰਨ fd[3] = { original_ਨਿਵੇਸ਼_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_ਨਿਵੇਸ਼_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        ਜੇ (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                ਜੇ (null_fd < 0) {
                        r = -errno;
                        ਜਾਓ ਅੰਤ;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                ਜੇ (null_fd < 3) {
                        ਪੂਰਨ copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ਜੇ (copy < 0) {
                                r = -errno;
                                ਜਾਓ ਅੰਤ;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        ਲਈ (ਪੂਰਨ i = 0; i < 3; i++)
                ਜੇ (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                ਨਹੀਂਤਾਂ ਜੇ (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ਜੇ (copy_fd[i] < 0) {
                                r = -errno;
                                ਜਾਓ ਅੰਤ;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        ਲਈ (ਪੂਰਨ i = 0; i < 3; i++)
                ਜੇ (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        ਜੇ (r < 0)
                                ਜਾਓ ਅੰਤ;
                } ਨਹੀਂਤਾਂ {
                        ਪ੍ਰਤਿਜ੍ਞਾ(fd[i] > 2);

                        ਜੇ (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                ਜਾਓ ਅੰਤ;
                        }
                }

        r = 0;

ਅੰਤ:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        ਸੁਰੱਖਿਅਤ_close_above_stdio(original_ਨਿਵੇਸ਼_fd);
        ਜੇ (original_output_fd != original_ਨਿਵੇਸ਼_fd)
                ਸੁਰੱਖਿਅਤ_close_above_stdio(original_output_fd);
        ਜੇ (original_error_fd != original_ਨਿਵੇਸ਼_fd && original_error_fd != original_output_fd)
                ਸੁਰੱਖਿਅਤ_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        ਸੁਰੱਖਿਅਤ_close_above_stdio(null_fd);

        ਵਾਪਸ r;
}

ਪੂਰਨ fd_reopen(ਪੂਰਨ fd, ਪੂਰਨ flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, O_CREAT));

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

        ਜੇ (FLAGS_ਸੈੱਟ(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                ਵਾਪਸ -ELOOP;

        ਜੇ (fd == XAT_FDROOT)
                ਵਾਪਸ RET_NERRNO(open("/", flags | O_DIRECTORY));

        ਜੇ (FLAGS_ਸੈੱਟ(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                ਵਾਪਸ RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        ਪੂਰਨ new_fd = open(FORMAT_PROC_FD_ਰਸਤਾ(fd), flags);
        ਜੇ (new_fd < 0) {
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ proc_fd_enoent_errno();
        }

        ਵਾਪਸ new_fd;
}

ਪੂਰਨ fd_reopen_propagate_append_and_position(ਪੂਰਨ fd, ਪੂਰਨ flags) {
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

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(flags & (O_APPEND|O_DIRECTORY)));

        ਪੂਰਨ existing_flags = fcntl(fd, F_GETFL);
        ਜੇ (existing_flags < 0)
                ਵਾਪਸ -errno;

        ਪੂਰਨ new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        ਜੇ (new_fd < 0)
                ਵਾਪਸ new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        ਜੇ (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                ਜੇ (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                ਨਹੀਂਤਾਂ ਜੇ (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (ਲੰਮਾ ਲੰਮਾ) new_p, (ਲੰਮਾ ਲੰਮਾ) p);
        }

        ਵਾਪਸ new_fd;
}

ਪੂਰਨ fd_reopen_condition(
                ਪੂਰਨ fd,
                ਪੂਰਨ flags,
                ਪੂਰਨ ਨਕਾਬ_3,
                ਪੂਰਨ *ret_new_fd) {

        ਪੂਰਨ r, new_fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, O_CREAT));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        ਜੇ (r < 0)
                ਵਾਪਸ -errno;

        ਜੇ ((r & ਨਕਾਬ_3) == (flags & ਨਕਾਬ_3)) {
                *ret_new_fd = -EBADF;
                ਵਾਪਸ fd;
        }

        new_fd = fd_reopen(fd, flags);
        ਜੇ (new_fd < 0)
                ਵਾਪਸ new_fd;

        *ret_new_fd = new_fd;
        ਵਾਪਸ new_fd;
}

ਪੂਰਨ fd_is_opath(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = fcntl(fd, F_GETFL);
        ਜੇ (r < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ FLAGS_ਸੈੱਟ(r, O_ਰਸਤਾ);
}

ਪੂਰਨ fd_vet_accmode(ਪੂਰਨ fd, ਪੂਰਨ ਬਹੁਲਕ) {
        ਪੂਰਨ flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        ਜੇ (flags < 0)
                ਵਾਪਸ -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        ਜੇ (FLAGS_ਸੈੱਟ(flags, O_DIRECTORY) && !FLAGS_ਸੈੱਟ(flags, O_TMPFILE))
                ਵਾਪਸ -EISDIR;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, O_ਰਸਤਾ))
                ਵਾਪਸ -EBADFD;

        flags &= O_ACCMODE_STRICT;

        ਜੇ (flags == ਬਹੁਲਕ)
                ਵਾਪਸ 1;

        ਜੇ (flags == O_RDWR)
                ਵਾਪਸ 0;

        ਵਾਪਸ -EPROTOTYPE;
}

ਪੂਰਨ fd_is_writable(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        ਜੇ (r >= 0)
                ਵਾਪਸ true;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                ਵਾਪਸ false;

        ਵਾਪਸ r;
}

ਪੂਰਨ fd_verify_ਸੁਰੱਖਿਅਤ_flags_ਭਰਿਆ(ਪੂਰਨ fd, ਪੂਰਨ extra_flags) {
        ਪੂਰਨ flags, unexpected_flags;

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

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        ਜੇ (flags < 0)
                ਵਾਪਸ -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        ਜੇ (unexpected_flags != 0)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (ਬਿਨਾਨਿਸ਼ਾਨੀ) unexpected_flags);

        ਵਾਪਸ flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

ਬਿਨਾਨਿਸ਼ਾਨੀ read_nr_open(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *nr_open = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_ਸਤਰ_file("/proc/sys/fs/nr_open", &nr_open);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        ਨਹੀਂਤਾਂ {
                ਬਿਨਾਨਿਸ਼ਾਨੀ v;

                r = ਸੁਰੱਖਿਅਤ_atou(nr_open, &v);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                ਨਹੀਂਤਾਂ
                        ਵਾਪਸ v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        ਵਾਪਸ NR_OPEN_DEFAULT;
}

ਪੂਰਨ fd_get_diskseq(ਪੂਰਨ fd, uint64_t *ret) {
        uint64_t diskseq;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                ਜੇ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ -EOPNOTSUPP;
        }

        *ret = diskseq;

        ਵਾਪਸ 0;
}

ਸਥਿਰ bool is_literal_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *p) {
        ਜੇ (!p)
                ਵਾਪਸ false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        ਵਾਪਸ n >= 1 && p[n] == 0;
}

ਪੂਰਨ ਰਸਤਾ_is_ਜੜ੍ਹ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));

        ਜੇ (dir_fd == XAT_FDROOT && isempty(ਰਸਤਾ))
                ਵਾਪਸ true;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_ਜੜ੍ਹ(ਰਸਤਾ))
                ਵਾਪਸ true;

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਜੇ (!isempty(ਰਸਤਾ)) {
                fd = xopenat(dir_fd, ਰਸਤਾ, O_ਰਸਤਾ|O_DIRECTORY|O_CLOEXEC);
                ਜੇ (fd == -ENOTDIR)
                        ਵਾਪਸ false; /* the root dir must be a dir */
                ਜੇ (fd < 0)
                        ਵਾਪਸ fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        ਵਾਪਸ fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

ਪੂਰਨ fds_inode_and_mount_same(ਪੂਰਨ fd1, ਪੂਰਨ fd2) {
        ਬਣਤਰ statx sx1, sx2;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(fd1));
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ਨਿਰਾ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                ਵਾਪਸ true;

        r = xstatx(fd2, /* path = */ ਨਿਰਾ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = statx_mount_same(&sx1, &sx2);
        ਜੇ (r <= 0)
                ਵਾਪਸ r;

        ਵਾਪਸ statx_inode_same(&sx1, &sx2);
}

ਪੂਰਨ resolve_xat_fdroot(ਪੂਰਨ *fd, ਅਚਲ ਅੱਖਰ **ਰਸਤਾ, ਅੱਖਰ **ret_ਬਫਰ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਬਫਰ);

        ਜੇ (*fd != XAT_FDROOT) {
                *ret_ਬਫਰ = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਜੇ (isempty(*ਰਸਤਾ)) {
                *ਰਸਤਾ = "/";
                *ret_ਬਫਰ = ਨਿਰਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (!ਰਸਤਾ_is_absolute(*ਰਸਤਾ)) {
                ਅੱਖਰ *p = strjoin("/", *ਰਸਤਾ);
                ਜੇ (!p)
                        ਵਾਪਸ -ENOMEM;

                *ਰਸਤਾ = *ret_ਬਫਰ = p;
        }

        *fd = AT_FDCWD;

        ਵਾਪਸ 1;
}

ਅੱਖਰ* format_proc_fd_ਰਸਤਾ(ਅੱਖਰ buf[ਸਥਿਰ PROC_FD_ਰਸਤਾ_MAX], ਪੂਰਨ fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_ਰਸਤਾ_MAX, "/proc/self/fd/%i", fd));
        ਵਾਪਸ buf;
}

ਅਚਲ ਅੱਖਰ* accmode_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ flags) {
        ਚੋਣ (flags & O_ACCMODE_STRICT) {
        ਹਾਲਤ O_RDONLY:
                ਵਾਪਸ "ro";
        ਹਾਲਤ O_WRONLY:
                ਵਾਪਸ "wo";
        ਹਾਲਤ O_RDWR:
                ਵਾਪਸ "rw";
        ਮੂਲ:
                ਵਾਪਸ ਨਿਰਾ;
        }
}

ਅੱਖਰ* format_proc_pid_fd_ਰਸਤਾ(ਅੱਖਰ buf[ਸਥਿਰ PROC_PID_FD_ਰਸਤਾ_MAX], pid_t pid, ਪੂਰਨ fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_ਰਸਤਾ_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        ਵਾਪਸ buf;
}

ਪੂਰਨ proc_fd_enoent_errno(ਖਾਲੀ) {
        ਪੂਰਨ r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        ਜੇ (r == 0)
                ਵਾਪਸ -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        ਜੇ (r > 0)
                ਵਾਪਸ -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        ਵਾਪਸ -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
