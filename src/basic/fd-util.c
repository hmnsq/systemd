/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <linux/fs.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/kcmp.h>
#ଅନ୍ତର୍ଭାଵ <sys/resource.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_nointr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (close(fd) >= 0)
                ଫେରନ୍ତୁ 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        ଯଦି (errno == EINTR)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ -errno;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        ଯଦି (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        ଫେରନ୍ତୁ -EBADF;
}

ଶୂନ୍ୟ safe_close_pair(ପୂର୍ଣ୍ଣସଂଖ୍ୟା p[ସ୍ଥିର 2]) {
        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                ଫେରନ୍ତୁ;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

ଶୂନ୍ୟ close_many(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds) {
        ପ୍ରତିଜ୍ଞା(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

ଶୂନ୍ୟ close_many_unset(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds) {
        ପ୍ରତିଜ୍ଞା(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

ଶୂନ୍ୟ close_many_and_free(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fds, size_t n_fds) {
        ପ୍ରତିଜ୍ଞା(fds || n_fds == 0);

        close_many(fds, n_fds);
        ମୁକ୍ତ(fds);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fclose_nointr(FILE *f) {
        ପ୍ରତିଜ୍ଞା(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        ଯଦି (fclose(f) == 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (errno == EINTR)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        ଯଦି (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

DIR* safe_closedir(DIR *d) {

        ଯଦି (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_nonblock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool nonblock) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, nflags;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        ଯଦି (flags < 0)
                ଫେରନ୍ତୁ -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        ଯଦି (nflags == flags)
                ଫେରନ୍ତୁ 0;

        ଯଦି (fcntl(fd, F_SETFL, nflags) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 1;
}

ଶୂନ୍ୟ nonblock_resetp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd) {
        ପ୍ରତିଜ୍ଞା(fd);

        PROTECT_ERRNO;

        ଯଦି (*fd >= 0)
                (ଶୂନ୍ୟ) fd_nonblock(*fd, false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stdio_disable_nonblock(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_cloexec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool cloexec) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, nflags;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        ଯଦି (flags < 0)
                ଫେରନ୍ତୁ -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        ଯଦି (nflags == flags)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_cloexec_many(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds, bool cloexec) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;

        ପ୍ରତିଜ୍ଞା(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                ଯଦି (*fd < 0) /* Skip gracefully over already invalidated fds */
                        ଜାରି;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର bool fd_in_set(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                ଯଦି (*i < 0)
                        ଜାରି;

                ଯଦି (*i == fd)
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_max_fd(ଶୂନ୍ୟ) {
        ସଂରଚନା rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        ଯଦି (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ଫେରନ୍ତୁ -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        ଯଦି (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                ଫେରନ୍ତୁ FD_SETSIZE-1;

        ଯଦି (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                ଫେରନ୍ତୁ INT_MAX;

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (m - 1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds_frugal(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_fd, r = 0;

        ପ୍ରତିଜ୍ଞା(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        ଯଦି (max_fd < 0)
                ଫେରନ୍ତୁ max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        ଯଦି (max_fd > MAX_FD_LOOP_LIMIT)
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା q;

                ଯଦି (fd_in_set(fd, except, n_except))
                        ଜାରି;

                q = close_nointr(fd);
                ଯଦି (q != -EBADF)
                        RET_GATHER(r, q);
        }

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds_special_case(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except) {
        ପ୍ରତିଜ୍ଞା(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        ଯଦି (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        ବାଛ (n_except) {

        ସ୍ଥିତି 0:
                /* Close everything. Yay! */
                ଯଦି (close_range(3, INT_MAX, 0) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ 1;

        ସ୍ଥିତି 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                ଯଦି (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ 1;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ 0;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds_without_malloc(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r > 0) /* special case worked! */
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ close_all_fds_frugal(except, n_except);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r > 0) /* special case worked! */
                ଫେରନ୍ତୁ 0;

        _cleanup_free_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *sorted_malloc = ଖାଲି;
        size_t n_sorted;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        ପ୍ରତିଜ୍ଞା(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        ଯଦି (n_sorted > ALLOCA_MAX / ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, n_sorted);
        ଅନ୍ୟଥା
                sorted = newa(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, n_sorted);

        ଯଦି (!sorted) /* Fallback on OOM. */
                ଫେରନ୍ତୁ close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        ପାଇଁ (size_t i = 0; i < n_sorted-1; i++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                ପ୍ରତିଜ୍ଞା(end >= start);

                ଯଦି (end - start <= 1)
                        ଜାରି;

                /* Close everything between the start and end fds (both of which shall stay open) */
                ଯଦି (close_range(start + 1, end - 1, 0) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        ଯଦି (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                ଫେରନ୍ତୁ 0;

        ଯଦି (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pack_fds(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds) {
        ଯଦି (n_fds <= 0)
                ଫେରନ୍ତୁ 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        ପ୍ରତିଜ୍ଞା(fds);

        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା start = 0;;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା restart_from = -1;

                ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = start; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n_fds; i++) {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfd;

                        /* Already at right index? */
                        ଯଦି (fds[i] == i + 3)
                                ଜାରି;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        ଯଦି (nfd < 0)
                                ଫେରନ୍ତୁ -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        ଯଦି (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                ଯଦି (restart_from < 0)
                        ଭାଙ୍ଗ;

                start = restart_from;
        }

        ପ୍ରତିଜ୍ଞା(fds[0] == 3);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_validate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -EBADF;

        ଯଦି (fcntl(fd, F_GETFD) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା same_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) {
        ସଂରଚନା stat sta, stb;
        pid_t pid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, fa, fb;

        ପ୍ରତିଜ୍ଞା(a >= 0);
        ପ୍ରତିଜ୍ଞା(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        ଯଦି (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ true;
        ଯଦି (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        ଯଦି (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } ଅନ୍ୟଥା ଯଦି (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                ଫେରନ୍ତୁ -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ !r;
        ଯଦି (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                ଫେରନ୍ତୁ -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        ଯଦି (fstat(a, &sta) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (fstat(b, &stb) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (!stat_inode_same(&sta, &stb))
                ଫେରନ୍ତୁ false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        ଯଦି (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                ଫେରନ୍ତୁ false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        ଯଦି (fa < 0)
                ଫେରନ୍ତୁ -errno;

        fb = fcntl(b, F_GETFL);
        ଯଦି (fb < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ fa == fb;
}

bool fdname_is_valid(ଅଚଳ ଅକ୍ଷର *s) {
        ଅଚଳ ଅକ୍ଷର *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        ଯଦି (!s)
                ଫେରନ୍ତୁ false;

        ପାଇଁ (p = s; *p; p++) {
                ଯଦି (*p < ' ')
                        ଫେରନ୍ତୁ false;
                ଯଦି (*p >= 127)
                        ଫେରନ୍ତୁ false;
                ଯଦି (*p == ':')
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ p - s <= FDNAME_MAX;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_get_path(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(fd));

        ଯଦି (fd == AT_FDCWD)
                ଫେରନ୍ତୁ safe_getcwd(ret);
        ଯଦି (fd == XAT_FDROOT)
                ଫେରନ୍ତୁ strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ proc_fd_enoent_errno();
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା move_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା from, ପୂର୍ଣ୍ଣସଂଖ୍ୟା to, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cloexec) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        ଯଦି (from < 0)
                ଫେରନ୍ତୁ -EBADF;
        ଯଦି (to < 0)
                ଫେରନ୍ତୁ -EBADF;

        ଯଦି (from == to) {

                ଯଦି (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଫେରନ୍ତୁ to;
        }

        ଯଦି (cloexec < 0) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fl;

                fl = fcntl(from, F_GETFD, 0);
                ଯଦି (fl < 0)
                        ଫେରନ୍ତୁ -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -errno;

        ପ୍ରତିଜ୍ଞା(r == to);

        safe_close(from);

        ଫେରନ୍ତୁ to;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_move_above_stdio(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, copy;
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

        ଯଦି (fd < 0 || fd > 2)
                ଫେରନ୍ତୁ fd;

        flags = fcntl(fd, F_GETFD, 0);
        ଯଦି (flags < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        ଅନ୍ୟଥା
                copy = fcntl(fd, F_DUPFD, 3);
        ଯଦି (copy < 0)
                ଫେରନ୍ତୁ fd;

        ପ୍ରତିଜ୍ଞା(copy > 2);

        (ଶୂନ୍ୟ) close(fd);
        ଫେରନ୍ତୁ copy;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rearrange_stdio(ପୂର୍ଣ୍ଣସଂଖ୍ୟା original_ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା original_output_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା original_error_fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd[3] = { original_ନିବେଶ_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_ନିବେଶ_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        ଯଦି (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                ଯଦି (null_fd < 0) {
                        r = -errno;
                        ଯାଅ finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                ଯଦି (null_fd < 3) {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ଯଦି (copy < 0) {
                                r = -errno;
                                ଯାଅ finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 3; i++)
                ଯଦି (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                ଅନ୍ୟଥା ଯଦି (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ଯଦି (copy_fd[i] < 0) {
                                r = -errno;
                                ଯାଅ finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 3; i++)
                ଯଦି (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        ଯଦି (r < 0)
                                ଯାଅ finish;
                } ଅନ୍ୟଥା {
                        ପ୍ରତିଜ୍ଞା(fd[i] > 2);

                        ଯଦି (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                ଯାଅ finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_ନିବେଶ_fd);
        ଯଦି (original_output_fd != original_ନିବେଶ_fd)
                safe_close_above_stdio(original_output_fd);
        ଯଦି (original_error_fd != original_ନିବେଶ_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(fd));
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, O_CREAT));

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

        ଯଦି (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (fd == XAT_FDROOT)
                ଫେରନ୍ତୁ RET_NERRNO(open("/", flags | O_DIRECTORY));

        ଯଦି (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                ଫେରନ୍ତୁ RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        ଯଦି (new_fd < 0) {
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ proc_fd_enoent_errno();
        }

        ଫେରନ୍ତୁ new_fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopen_propagate_append_and_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
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

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(!(flags & (O_APPEND|O_DIRECTORY)));

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା existing_flags = fcntl(fd, F_GETFL);
        ଯଦି (existing_flags < 0)
                ଫେରନ୍ତୁ -errno;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        ଯଦି (new_fd < 0)
                ଫେରନ୍ତୁ new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        ଯଦି (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                ଯଦି (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                ଅନ୍ୟଥା ଯଦି (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        ଫେରନ୍ତୁ new_fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopen_condition(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_new_fd) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, new_fd;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, O_CREAT));
        ପ୍ରତିଜ୍ଞା(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                ଫେରନ୍ତୁ fd;
        }

        new_fd = fd_reopen(fd, flags);
        ଯଦି (new_fd < 0)
                ଫେରନ୍ତୁ new_fd;

        *ret_new_fd = new_fd;
        ଫେରନ୍ତୁ new_fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_opath(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = fcntl(fd, F_GETFL);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ FLAGS_SET(r, O_PATH);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_vet_accmode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        ଯଦି (flags < 0)
                ଫେରନ୍ତୁ -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        ଯଦି (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (FLAGS_SET(flags, O_PATH))
                ଫେରନ୍ତୁ -EBADFD;

        flags &= O_ACCMODE_STRICT;

        ଯଦି (flags == mode)
                ଫେରନ୍ତୁ 1;

        ଯଦି (flags == O_RDWR)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ -EPROTOTYPE;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_writable(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ true;

        ଯଦି (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_safe_flags_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, unexpected_flags;

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

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        ଯଦି (flags < 0)
                ଫେରନ୍ତୁ -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        ଯଦି (unexpected_flags != 0)
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        ଫେରନ୍ତୁ flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *nr_open = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_ଧାଡ଼ି_file("/proc/sys/fs/nr_open", &nr_open);
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        ଅନ୍ୟଥା {
                unsigned v;

                r = safe_atou(nr_open, &v);
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        ଫେରନ୍ତୁ NR_OPEN_DEFAULT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_get_diskseq(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        uint64_t diskseq;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                ଯଦି (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }

        *ret = diskseq;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର bool is_literal_ମୂଳ(ଅଚଳ ଅକ୍ଷର *p) {
        ଯଦି (!p)
                ଫେରନ୍ତୁ false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        ଫେରନ୍ତୁ n >= 1 && p[n] == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_ମୂଳ_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));

        ଯଦି (dir_fd == XAT_FDROOT && isempty(path))
                ଫେରନ୍ତୁ true;

        ଯଦି (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_ମୂଳ(path))
                ଫେରନ୍ତୁ true;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ଯଦି (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ଯଦି (fd == -ENOTDIR)
                        ଫେରନ୍ତୁ false; /* the root dir must be a dir */
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        ଫେରନ୍ତୁ fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds_inode_and_mount_same(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd2) {
        ସଂରଚନା statx sx1, sx2;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(fd1));
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ଖାଲି, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                ଫେରନ୍ତୁ true;

        r = xstatx(fd2, /* path = */ ଖାଲି, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = statx_mount_same(&sx1, &sx2);
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ statx_inode_same(&sx1, &sx2);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve_xat_fdroot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd, ଅଚଳ ଅକ୍ଷର **path, ଅକ୍ଷର **ret_buffer) {
        ପ୍ରତିଜ୍ଞା(fd);
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret_buffer);

        ଯଦି (*fd != XAT_FDROOT) {
                *ret_buffer = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (isempty(*path)) {
                *path = "/";
                *ret_buffer = ଖାଲି;
        } ଅନ୍ୟଥା ଯଦି (!path_is_absolute(*path)) {
                ଅକ୍ଷର *p = strjoin("/", *path);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        ଫେରନ୍ତୁ 1;
}

ଅକ୍ଷର* format_proc_fd_path(ଅକ୍ଷର buf[ସ୍ଥିର PROC_FD_PATH_MAX], ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        ଫେରନ୍ତୁ buf;
}

ଅଚଳ ଅକ୍ଷର* accmode_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ବାଛ (flags & O_ACCMODE_STRICT) {
        ସ୍ଥିତି O_RDONLY:
                ଫେରନ୍ତୁ "ro";
        ସ୍ଥିତି O_WRONLY:
                ଫେରନ୍ତୁ "wo";
        ସ୍ଥିତି O_RDWR:
                ଫେରନ୍ତୁ "rw";
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ ଖାଲି;
        }
}

ଅକ୍ଷର* format_proc_pid_fd_path(ଅକ୍ଷର buf[ସ୍ଥିର PROC_PID_FD_PATH_MAX], pid_t pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        ଫେରନ୍ତୁ buf;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_fd_enoent_errno(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        ଫେରନ୍ତୁ -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
