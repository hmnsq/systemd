/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <linux/fs.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/kcmp.h>
#অন্তর্ভাব <sys/resource.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "sort-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

পূর্ণসংখ্যা close_nointr(পূর্ণসংখ্যা fd) {
        প্রতিজ্ঞা(fd >= 0);

        যদি (close(fd) >= 0)
                ফেরত 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        যদি (errno == EINTR)
                ফেরত 0;

        ফেরত -errno;
}

পূর্ণসংখ্যা safe_close(পূর্ণসংখ্যা fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        যদি (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        ফেরত -EBADF;
}

শূন্য safe_close_pair(পূর্ণসংখ্যা p[স্থির 2]) {
        প্রতিজ্ঞা(p);

        যদি (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                ফেরত;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

শূন্য close_many(ধ্রুবক পূর্ণসংখ্যা fds[], size_t n_fds) {
        প্রতিজ্ঞা(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

শূন্য close_many_unset(পূর্ণসংখ্যা fds[], size_t n_fds) {
        প্রতিজ্ঞা(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

শূন্য close_many_and_free(পূর্ণসংখ্যা *fds, size_t n_fds) {
        প্রতিজ্ঞা(fds || n_fds == 0);

        close_many(fds, n_fds);
        মুক্ত(fds);
}

পূর্ণসংখ্যা fclose_nointr(FILE *f) {
        প্রতিজ্ঞা(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        যদি (fclose(f) == 0)
                ফেরত 0;

        যদি (errno == EINTR)
                ফেরত 0;

        ফেরত errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        যদি (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        ফেরত ফাঁকা;
}

DIR* safe_closedir(DIR *d) {

        যদি (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        ফেরত ফাঁকা;
}

পূর্ণসংখ্যা fd_nonblock(পূর্ণসংখ্যা fd, bool nonblock) {
        পূর্ণসংখ্যা flags, nflags;

        প্রতিজ্ঞা(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        যদি (flags < 0)
                ফেরত -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        যদি (nflags == flags)
                ফেরত 0;

        যদি (fcntl(fd, F_SETFL, nflags) < 0)
                ফেরত -errno;

        ফেরত 1;
}

শূন্য nonblock_resetp(পূর্ণসংখ্যা *fd) {
        প্রতিজ্ঞা(fd);

        PROTECT_ERRNO;

        যদি (*fd >= 0)
                (শূন্য) fd_nonblock(*fd, false);
}

পূর্ণসংখ্যা stdio_disable_nonblock(শূন্য) {
        পূর্ণসংখ্যা ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        ফেরত ret;
}

পূর্ণসংখ্যা fd_cloexec(পূর্ণসংখ্যা fd, bool cloexec) {
        পূর্ণসংখ্যা flags, nflags;

        প্রতিজ্ঞা(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        যদি (flags < 0)
                ফেরত -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        যদি (nflags == flags)
                ফেরত 0;

        ফেরত RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

পূর্ণসংখ্যা fd_cloexec_many(ধ্রুবক পূর্ণসংখ্যা fds[], size_t n_fds, bool cloexec) {
        পূর্ণসংখ্যা r = 0;

        প্রতিজ্ঞা(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                যদি (*fd < 0) /* Skip gracefully over already invalidated fds */
                        চালু;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        ফেরত r;
}

স্থির bool fd_in_set(পূর্ণসংখ্যা fd, ধ্রুবক পূর্ণসংখ্যা fds[], size_t n_fds) {
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                যদি (*i < 0)
                        চালু;

                যদি (*i == fd)
                        ফেরত true;
        }

        ফেরত false;
}

পূর্ণসংখ্যা get_max_fd(শূন্য) {
        কাঠামো rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        যদি (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ফেরত -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        যদি (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                ফেরত FD_SETSIZE-1;

        যদি (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                ফেরত INT_MAX;

        ফেরত (পূর্ণসংখ্যা) (m - 1);
}

পূর্ণসংখ্যা close_all_fds_frugal(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except) {
        পূর্ণসংখ্যা max_fd, r = 0;

        প্রতিজ্ঞা(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        যদি (max_fd < 0)
                ফেরত max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        যদি (max_fd > MAX_FD_LOOP_LIMIT)
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        জন্য (পূর্ণসংখ্যা fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                পূর্ণসংখ্যা q;

                যদি (fd_in_set(fd, except, n_except))
                        চালু;

                q = close_nointr(fd);
                যদি (q != -EBADF)
                        RET_GATHER(r, q);
        }

        ফেরত r;
}

স্থির পূর্ণসংখ্যা close_all_fds_special_case(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except) {
        প্রতিজ্ঞা(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        যদি (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        নির্বাচন (n_except) {

        ক্ষেত্র 0:
                /* Close everything. Yay! */
                যদি (close_range(3, INT_MAX, 0) < 0)
                        ফেরত -errno;

                ফেরত 1;

        ক্ষেত্র 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                যদি (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        ফেরত -errno;

                যদি (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        ফেরত -errno;

                ফেরত 1;

        পূর্বনির্ধারিত:
                ফেরত 0;
        }
}

পূর্ণসংখ্যা close_all_fds_without_malloc(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        যদি (r < 0)
                ফেরত r;
        যদি (r > 0) /* special case worked! */
                ফেরত 0;

        ফেরত close_all_fds_frugal(except, n_except);
}

পূর্ণসংখ্যা close_all_fds(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        যদি (r < 0)
                ফেরত r;
        যদি (r > 0) /* special case worked! */
                ফেরত 0;

        _cleanup_free_ পূর্ণসংখ্যা *sorted_malloc = ফাঁকা;
        size_t n_sorted;
        পূর্ণসংখ্যা *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        প্রতিজ্ঞা(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        যদি (n_sorted > ALLOCA_MAX / আকার(পূর্ণসংখ্যা)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(পূর্ণসংখ্যা, n_sorted);
        অন্যথা
                sorted = newa(পূর্ণসংখ্যা, n_sorted);

        যদি (!sorted) /* Fallback on OOM. */
                ফেরত close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * আকার(পূর্ণসংখ্যা));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        জন্য (size_t i = 0; i < n_sorted-1; i++) {
                পূর্ণসংখ্যা start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                প্রতিজ্ঞা(end >= start);

                যদি (end - start <= 1)
                        চালু;

                /* Close everything between the start and end fds (both of which shall stay open) */
                যদি (close_range(start + 1, end - 1, 0) < 0)
                        ফেরত -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        যদি (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                ফেরত 0;

        যদি (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                ফেরত -errno;

        ফেরত 0;
}

পূর্ণসংখ্যা pack_fds(পূর্ণসংখ্যা fds[], size_t n_fds) {
        যদি (n_fds <= 0)
                ফেরত 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        প্রতিজ্ঞা(fds);

        জন্য (পূর্ণসংখ্যা start = 0;;) {
                পূর্ণসংখ্যা restart_from = -1;

                জন্য (পূর্ণসংখ্যা i = start; i < (পূর্ণসংখ্যা) n_fds; i++) {
                        পূর্ণসংখ্যা nfd;

                        /* Already at right index? */
                        যদি (fds[i] == i + 3)
                                চালু;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        যদি (nfd < 0)
                                ফেরত -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        যদি (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                যদি (restart_from < 0)
                        বিরতি;

                start = restart_from;
        }

        প্রতিজ্ঞা(fds[0] == 3);

        ফেরত 0;
}

পূর্ণসংখ্যা fd_validate(পূর্ণসংখ্যা fd) {
        যদি (fd < 0)
                ফেরত -EBADF;

        যদি (fcntl(fd, F_GETFD) < 0)
                ফেরত -errno;

        ফেরত 0;
}

পূর্ণসংখ্যা same_fd(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b) {
        কাঠামো stat sta, stb;
        pid_t pid;
        পূর্ণসংখ্যা r, fa, fb;

        প্রতিজ্ঞা(a >= 0);
        প্রতিজ্ঞা(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        যদি (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                যদি (r < 0)
                        ফেরত r;

                ফেরত true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        যদি (r > 0)
                ফেরত true;
        যদি (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                যদি (r < 0)
                        ফেরত r;

                ফেরত false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        যদি (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                যদি (r < 0)
                        ফেরত r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } অন্যথা যদি (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                ফেরত -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        যদি (r >= 0)
                ফেরত !r;
        যদি (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                ফেরত -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        যদি (fstat(a, &sta) < 0)
                ফেরত -errno;

        যদি (fstat(b, &stb) < 0)
                ফেরত -errno;

        যদি (!stat_inode_same(&sta, &stb))
                ফেরত false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        যদি (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                ফেরত false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        যদি (fa < 0)
                ফেরত -errno;

        fb = fcntl(b, F_GETFL);
        যদি (fb < 0)
                ফেরত -errno;

        ফেরত fa == fb;
}

bool fdname_is_valid(ধ্রুবক অক্ষর *s) {
        ধ্রুবক অক্ষর *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        যদি (!s)
                ফেরত false;

        জন্য (p = s; *p; p++) {
                যদি (*p < ' ')
                        ফেরত false;
                যদি (*p >= 127)
                        ফেরত false;
                যদি (*p == ':')
                        ফেরত false;
        }

        ফেরত p - s <= FDNAME_MAX;
}

পূর্ণসংখ্যা fd_get_path(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(fd));

        যদি (fd == AT_FDCWD)
                ফেরত safe_getcwd(ret);
        যদি (fd == XAT_FDROOT)
                ফেরত strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        যদি (r == -ENOENT)
                ফেরত proc_fd_enoent_errno();
        ফেরত r;
}

পূর্ণসংখ্যা move_fd(পূর্ণসংখ্যা from, পূর্ণসংখ্যা to, পূর্ণসংখ্যা cloexec) {
        পূর্ণসংখ্যা r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        যদি (from < 0)
                ফেরত -EBADF;
        যদি (to < 0)
                ফেরত -EBADF;

        যদি (from == to) {

                যদি (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        যদি (r < 0)
                                ফেরত r;
                }

                ফেরত to;
        }

        যদি (cloexec < 0) {
                পূর্ণসংখ্যা fl;

                fl = fcntl(from, F_GETFD, 0);
                যদি (fl < 0)
                        ফেরত -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        যদি (r < 0)
                ফেরত -errno;

        প্রতিজ্ঞা(r == to);

        safe_close(from);

        ফেরত to;
}

পূর্ণসংখ্যা fd_move_above_stdio(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা flags, copy;
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

        যদি (fd < 0 || fd > 2)
                ফেরত fd;

        flags = fcntl(fd, F_GETFD, 0);
        যদি (flags < 0)
                ফেরত fd;

        যদি (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        অন্যথা
                copy = fcntl(fd, F_DUPFD, 3);
        যদি (copy < 0)
                ফেরত fd;

        প্রতিজ্ঞা(copy > 2);

        (শূন্য) close(fd);
        ফেরত copy;
}

পূর্ণসংখ্যা rearrange_stdio(পূর্ণসংখ্যা original_প্রবেশ_fd, পূর্ণসংখ্যা original_output_fd, পূর্ণসংখ্যা original_error_fd) {
        পূর্ণসংখ্যা fd[3] = { original_প্রবেশ_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_প্রবেশ_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        যদি (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                যদি (null_fd < 0) {
                        r = -errno;
                        যাও finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                যদি (null_fd < 3) {
                        পূর্ণসংখ্যা copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        যদি (copy < 0) {
                                r = -errno;
                                যাও finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        জন্য (পূর্ণসংখ্যা i = 0; i < 3; i++)
                যদি (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                অন্যথা যদি (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        যদি (copy_fd[i] < 0) {
                                r = -errno;
                                যাও finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        জন্য (পূর্ণসংখ্যা i = 0; i < 3; i++)
                যদি (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        যদি (r < 0)
                                যাও finish;
                } অন্যথা {
                        প্রতিজ্ঞা(fd[i] > 2);

                        যদি (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                যাও finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_প্রবেশ_fd);
        যদি (original_output_fd != original_প্রবেশ_fd)
                safe_close_above_stdio(original_output_fd);
        যদি (original_error_fd != original_প্রবেশ_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        ফেরত r;
}

পূর্ণসংখ্যা fd_reopen(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা flags) {
        প্রতিজ্ঞা(wildcard_fd_is_valid(fd));
        প্রতিজ্ঞা(!FLAGS_SET(flags, O_CREAT));

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

        যদি (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                ফেরত -ELOOP;

        যদি (fd == XAT_FDROOT)
                ফেরত RET_NERRNO(open("/", flags | O_DIRECTORY));

        যদি (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                ফেরত RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        পূর্ণসংখ্যা new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        যদি (new_fd < 0) {
                যদি (errno != ENOENT)
                        ফেরত -errno;

                ফেরত proc_fd_enoent_errno();
        }

        ফেরত new_fd;
}

পূর্ণসংখ্যা fd_reopen_propagate_append_and_position(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা flags) {
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

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(!(flags & (O_APPEND|O_DIRECTORY)));

        পূর্ণসংখ্যা existing_flags = fcntl(fd, F_GETFL);
        যদি (existing_flags < 0)
                ফেরত -errno;

        পূর্ণসংখ্যা new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        যদি (new_fd < 0)
                ফেরত new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        যদি (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                যদি (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                অন্যথা যদি (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        ফেরত new_fd;
}

পূর্ণসংখ্যা fd_reopen_condition(
                পূর্ণসংখ্যা fd,
                পূর্ণসংখ্যা flags,
                পূর্ণসংখ্যা mask,
                পূর্ণসংখ্যা *ret_new_fd) {

        পূর্ণসংখ্যা r, new_fd;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(!FLAGS_SET(flags, O_CREAT));
        প্রতিজ্ঞা(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        যদি (r < 0)
                ফেরত -errno;

        যদি ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                ফেরত fd;
        }

        new_fd = fd_reopen(fd, flags);
        যদি (new_fd < 0)
                ফেরত new_fd;

        *ret_new_fd = new_fd;
        ফেরত new_fd;
}

পূর্ণসংখ্যা fd_is_opath(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = fcntl(fd, F_GETFL);
        যদি (r < 0)
                ফেরত -errno;

        ফেরত FLAGS_SET(r, O_PATH);
}

পূর্ণসংখ্যা fd_vet_accmode(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা mode) {
        পূর্ণসংখ্যা flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        যদি (flags < 0)
                ফেরত -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        যদি (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                ফেরত -EISDIR;

        যদি (FLAGS_SET(flags, O_PATH))
                ফেরত -EBADFD;

        flags &= O_ACCMODE_STRICT;

        যদি (flags == mode)
                ফেরত 1;

        যদি (flags == O_RDWR)
                ফেরত 0;

        ফেরত -EPROTOTYPE;
}

পূর্ণসংখ্যা fd_is_writable(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        যদি (r >= 0)
                ফেরত true;

        যদি (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                ফেরত false;

        ফেরত r;
}

পূর্ণসংখ্যা fd_verify_safe_flags_full(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা extra_flags) {
        পূর্ণসংখ্যা flags, unexpected_flags;

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

        প্রতিজ্ঞা(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        যদি (flags < 0)
                ফেরত -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        যদি (unexpected_flags != 0)
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        ফেরত flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(শূন্য) {
        _cleanup_free_ অক্ষর *nr_open = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_সারি_file("/proc/sys/fs/nr_open", &nr_open);
        যদি (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        অন্যথা {
                unsigned v;

                r = safe_atou(nr_open, &v);
                যদি (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                অন্যথা
                        ফেরত v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        ফেরত NR_OPEN_DEFAULT;
}

পূর্ণসংখ্যা fd_get_diskseq(পূর্ণসংখ্যা fd, uint64_t *ret) {
        uint64_t diskseq;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                যদি (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        ফেরত -errno;

                ফেরত -EOPNOTSUPP;
        }

        *ret = diskseq;

        ফেরত 0;
}

স্থির bool is_literal_মূল(ধ্রুবক অক্ষর *p) {
        যদি (!p)
                ফেরত false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        ফেরত n >= 1 && p[n] == 0;
}

পূর্ণসংখ্যা path_is_মূল_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path) {
        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));

        যদি (dir_fd == XAT_FDROOT && isempty(path))
                ফেরত true;

        যদি (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_মূল(path))
                ফেরত true;

        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        যদি (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                যদি (fd == -ENOTDIR)
                        ফেরত false; /* the root dir must be a dir */
                যদি (fd < 0)
                        ফেরত fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        ফেরত fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

পূর্ণসংখ্যা fds_inode_and_mount_same(পূর্ণসংখ্যা fd1, পূর্ণসংখ্যা fd2) {
        কাঠামো statx sx1, sx2;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(fd1));
        প্রতিজ্ঞা(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ফাঁকা, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        যদি (r < 0)
                ফেরত r;

        যদি (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                ফেরত true;

        r = xstatx(fd2, /* path = */ ফাঁকা, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        যদি (r < 0)
                ফেরত r;

        r = statx_mount_same(&sx1, &sx2);
        যদি (r <= 0)
                ফেরত r;

        ফেরত statx_inode_same(&sx1, &sx2);
}

পূর্ণসংখ্যা resolve_xat_fdroot(পূর্ণসংখ্যা *fd, ধ্রুবক অক্ষর **path, অক্ষর **ret_বাফার) {
        প্রতিজ্ঞা(fd);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret_বাফার);

        যদি (*fd != XAT_FDROOT) {
                *ret_বাফার = ফাঁকা;
                ফেরত 0;
        }

        যদি (isempty(*path)) {
                *path = "/";
                *ret_বাফার = ফাঁকা;
        } অন্যথা যদি (!path_is_absolute(*path)) {
                অক্ষর *p = strjoin("/", *path);
                যদি (!p)
                        ফেরত -ENOMEM;

                *path = *ret_বাফার = p;
        }

        *fd = AT_FDCWD;

        ফেরত 1;
}

অক্ষর* format_proc_fd_path(অক্ষর buf[স্থির PROC_FD_PATH_MAX], পূর্ণসংখ্যা fd) {
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        ফেরত buf;
}

ধ্রুবক অক্ষর* accmode_to_string(পূর্ণসংখ্যা flags) {
        নির্বাচন (flags & O_ACCMODE_STRICT) {
        ক্ষেত্র O_RDONLY:
                ফেরত "ro";
        ক্ষেত্র O_WRONLY:
                ফেরত "wo";
        ক্ষেত্র O_RDWR:
                ফেরত "rw";
        পূর্বনির্ধারিত:
                ফেরত ফাঁকা;
        }
}

অক্ষর* format_proc_pid_fd_path(অক্ষর buf[স্থির PROC_PID_FD_PATH_MAX], pid_t pid, পূর্ণসংখ্যা fd) {
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        ফেরত buf;
}

পূর্ণসংখ্যা proc_fd_enoent_errno(শূন্য) {
        পূর্ণসংখ্যা r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        যদি (r == 0)
                ফেরত -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        যদি (r > 0)
                ফেরত -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        ফেরত -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
