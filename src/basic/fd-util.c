/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <linux/fs.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/kcmp.h>
#அந்தர்பாவ <sys/resource.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "sort-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

முழு close_nointr(முழு fd) {
        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (close(fd) >= 0)
                திரும்பு 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        எனில் (errno == EINTR)
                திரும்பு 0;

        திரும்பு -errno;
}

முழு safe_close(முழு fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        எனில் (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        திரும்பு -EBADF;
}

வெறுமை safe_close_pair(முழு p[நிலையான 2]) {
        ப்ரதிஜ்ஞா(p);

        எனில் (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                திரும்பு;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

வெறுமை close_many(மாறிலி முழு fds[], size_t n_fds) {
        ப்ரதிஜ்ஞா(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

வெறுமை close_many_unset(முழு fds[], size_t n_fds) {
        ப்ரதிஜ்ஞா(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

வெறுமை close_many_and_free(முழு *fds, size_t n_fds) {
        ப்ரதிஜ்ஞா(fds || n_fds == 0);

        close_many(fds, n_fds);
        விடுவி(fds);
}

முழு fclose_nointr(FILE *f) {
        ப்ரதிஜ்ஞா(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        எனில் (fclose(f) == 0)
                திரும்பு 0;

        எனில் (errno == EINTR)
                திரும்பு 0;

        திரும்பு errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        எனில் (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        திரும்பு வெற்று;
}

DIR* safe_closedir(DIR *d) {

        எனில் (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        திரும்பு வெற்று;
}

முழு fd_nonblock(முழு fd, bool nonblock) {
        முழு flags, nflags;

        ப்ரதிஜ்ஞா(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        எனில் (flags < 0)
                திரும்பு -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        எனில் (nflags == flags)
                திரும்பு 0;

        எனில் (fcntl(fd, F_SETFL, nflags) < 0)
                திரும்பு -errno;

        திரும்பு 1;
}

வெறுமை nonblock_resetp(முழு *fd) {
        ப்ரதிஜ்ஞா(fd);

        PROTECT_ERRNO;

        எனில் (*fd >= 0)
                (வெறுமை) fd_nonblock(*fd, false);
}

முழு stdio_disable_nonblock(வெறுமை) {
        முழு ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        திரும்பு ret;
}

முழு fd_cloexec(முழு fd, bool cloexec) {
        முழு flags, nflags;

        ப்ரதிஜ்ஞா(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        எனில் (flags < 0)
                திரும்பு -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        எனில் (nflags == flags)
                திரும்பு 0;

        திரும்பு RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

முழு fd_cloexec_many(மாறிலி முழு fds[], size_t n_fds, bool cloexec) {
        முழு r = 0;

        ப்ரதிஜ்ஞா(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                எனில் (*fd < 0) /* Skip gracefully over already invalidated fds */
                        தொடர்;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        திரும்பு r;
}

நிலையான bool fd_in_set(முழு fd, மாறிலி முழு fds[], size_t n_fds) {
        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                எனில் (*i < 0)
                        தொடர்;

                எனில் (*i == fd)
                        திரும்பு true;
        }

        திரும்பு false;
}

முழு get_max_fd(வெறுமை) {
        கட்டமைப்பு rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        எனில் (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                திரும்பு -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        எனில் (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                திரும்பு FD_SETSIZE-1;

        எனில் (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                திரும்பு INT_MAX;

        திரும்பு (முழு) (m - 1);
}

முழு close_all_fds_frugal(மாறிலி முழு except[], size_t n_except) {
        முழு max_fd, r = 0;

        ப்ரதிஜ்ஞா(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        எனில் (max_fd < 0)
                திரும்பு max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        எனில் (max_fd > MAX_FD_LOOP_LIMIT)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        க்கு (முழு fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                முழு q;

                எனில் (fd_in_set(fd, except, n_except))
                        தொடர்;

                q = close_nointr(fd);
                எனில் (q != -EBADF)
                        RET_GATHER(r, q);
        }

        திரும்பு r;
}

நிலையான முழு close_all_fds_special_case(மாறிலி முழு except[], size_t n_except) {
        ப்ரதிஜ்ஞா(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        எனில் (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        தேர்வு (n_except) {

        நிலை 0:
                /* Close everything. Yay! */
                எனில் (close_range(3, INT_MAX, 0) < 0)
                        திரும்பு -errno;

                திரும்பு 1;

        நிலை 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                எனில் (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        திரும்பு -errno;

                எனில் (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        திரும்பு -errno;

                திரும்பு 1;

        இயல்பு:
                திரும்பு 0;
        }
}

முழு close_all_fds_without_malloc(மாறிலி முழு except[], size_t n_except) {
        முழு r;

        ப்ரதிஜ்ஞா(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r > 0) /* special case worked! */
                திரும்பு 0;

        திரும்பு close_all_fds_frugal(except, n_except);
}

முழு close_all_fds(மாறிலி முழு except[], size_t n_except) {
        முழு r;

        ப்ரதிஜ்ஞா(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r > 0) /* special case worked! */
                திரும்பு 0;

        _cleanup_free_ முழு *sorted_malloc = வெற்று;
        size_t n_sorted;
        முழு *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        ப்ரதிஜ்ஞா(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        எனில் (n_sorted > ALLOCA_MAX / அளவு(முழு)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(முழு, n_sorted);
        இல்லையெனில்
                sorted = newa(முழு, n_sorted);

        எனில் (!sorted) /* Fallback on OOM. */
                திரும்பு close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * அளவு(முழு));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        க்கு (size_t i = 0; i < n_sorted-1; i++) {
                முழு start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                ப்ரதிஜ்ஞா(end >= start);

                எனில் (end - start <= 1)
                        தொடர்;

                /* Close everything between the start and end fds (both of which shall stay open) */
                எனில் (close_range(start + 1, end - 1, 0) < 0)
                        திரும்பு -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        எனில் (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                திரும்பு 0;

        எனில் (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                திரும்பு -errno;

        திரும்பு 0;
}

முழு pack_fds(முழு fds[], size_t n_fds) {
        எனில் (n_fds <= 0)
                திரும்பு 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        ப்ரதிஜ்ஞா(fds);

        க்கு (முழு start = 0;;) {
                முழு restart_from = -1;

                க்கு (முழு i = start; i < (முழு) n_fds; i++) {
                        முழு nfd;

                        /* Already at right index? */
                        எனில் (fds[i] == i + 3)
                                தொடர்;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        எனில் (nfd < 0)
                                திரும்பு -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        எனில் (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                எனில் (restart_from < 0)
                        நிறுத்து;

                start = restart_from;
        }

        ப்ரதிஜ்ஞா(fds[0] == 3);

        திரும்பு 0;
}

முழு fd_validate(முழு fd) {
        எனில் (fd < 0)
                திரும்பு -EBADF;

        எனில் (fcntl(fd, F_GETFD) < 0)
                திரும்பு -errno;

        திரும்பு 0;
}

முழு same_fd(முழு a, முழு b) {
        கட்டமைப்பு stat sta, stb;
        pid_t pid;
        முழு r, fa, fb;

        ப்ரதிஜ்ஞா(a >= 0);
        ப்ரதிஜ்ஞா(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        எனில் (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        எனில் (r > 0)
                திரும்பு true;
        எனில் (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        எனில் (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                எனில் (r < 0)
                        திரும்பு r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } இல்லையெனில் எனில் (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                திரும்பு -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        எனில் (r >= 0)
                திரும்பு !r;
        எனில் (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                திரும்பு -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        எனில் (fstat(a, &sta) < 0)
                திரும்பு -errno;

        எனில் (fstat(b, &stb) < 0)
                திரும்பு -errno;

        எனில் (!stat_inode_same(&sta, &stb))
                திரும்பு false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        எனில் (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                திரும்பு false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        எனில் (fa < 0)
                திரும்பு -errno;

        fb = fcntl(b, F_GETFL);
        எனில் (fb < 0)
                திரும்பு -errno;

        திரும்பு fa == fb;
}

bool fdname_is_valid(மாறிலி எழுத்து *s) {
        மாறிலி எழுத்து *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        எனில் (!s)
                திரும்பு false;

        க்கு (p = s; *p; p++) {
                எனில் (*p < ' ')
                        திரும்பு false;
                எனில் (*p >= 127)
                        திரும்பு false;
                எனில் (*p == ':')
                        திரும்பு false;
        }

        திரும்பு p - s <= FDNAME_MAX;
}

முழு fd_get_path(முழு fd, எழுத்து **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(fd));

        எனில் (fd == AT_FDCWD)
                திரும்பு safe_getcwd(ret);
        எனில் (fd == XAT_FDROOT)
                திரும்பு strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        எனில் (r == -ENOENT)
                திரும்பு proc_fd_enoent_errno();
        திரும்பு r;
}

முழு move_fd(முழு from, முழு to, முழு cloexec) {
        முழு r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        எனில் (from < 0)
                திரும்பு -EBADF;
        எனில் (to < 0)
                திரும்பு -EBADF;

        எனில் (from == to) {

                எனில் (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                திரும்பு to;
        }

        எனில் (cloexec < 0) {
                முழு fl;

                fl = fcntl(from, F_GETFD, 0);
                எனில் (fl < 0)
                        திரும்பு -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        எனில் (r < 0)
                திரும்பு -errno;

        ப்ரதிஜ்ஞா(r == to);

        safe_close(from);

        திரும்பு to;
}

முழு fd_move_above_stdio(முழு fd) {
        முழு flags, copy;
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

        எனில் (fd < 0 || fd > 2)
                திரும்பு fd;

        flags = fcntl(fd, F_GETFD, 0);
        எனில் (flags < 0)
                திரும்பு fd;

        எனில் (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        இல்லையெனில்
                copy = fcntl(fd, F_DUPFD, 3);
        எனில் (copy < 0)
                திரும்பு fd;

        ப்ரதிஜ்ஞா(copy > 2);

        (வெறுமை) close(fd);
        திரும்பு copy;
}

முழு rearrange_stdio(முழு original_உள்ளீடு_fd, முழு original_output_fd, முழு original_error_fd) {
        முழு fd[3] = { original_உள்ளீடு_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_உள்ளீடு_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        எனில் (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                எனில் (null_fd < 0) {
                        r = -errno;
                        செல் finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                எனில் (null_fd < 3) {
                        முழு copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        எனில் (copy < 0) {
                                r = -errno;
                                செல் finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        க்கு (முழு i = 0; i < 3; i++)
                எனில் (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                இல்லையெனில் எனில் (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        எனில் (copy_fd[i] < 0) {
                                r = -errno;
                                செல் finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        க்கு (முழு i = 0; i < 3; i++)
                எனில் (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        எனில் (r < 0)
                                செல் finish;
                } இல்லையெனில் {
                        ப்ரதிஜ்ஞா(fd[i] > 2);

                        எனில் (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                செல் finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_உள்ளீடு_fd);
        எனில் (original_output_fd != original_உள்ளீடு_fd)
                safe_close_above_stdio(original_output_fd);
        எனில் (original_error_fd != original_உள்ளீடு_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        திரும்பு r;
}

முழு fd_reopen(முழு fd, முழு flags) {
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(fd));
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, O_CREAT));

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

        எனில் (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                திரும்பு -ELOOP;

        எனில் (fd == XAT_FDROOT)
                திரும்பு RET_NERRNO(open("/", flags | O_DIRECTORY));

        எனில் (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                திரும்பு RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        முழு new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        எனில் (new_fd < 0) {
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                திரும்பு proc_fd_enoent_errno();
        }

        திரும்பு new_fd;
}

முழு fd_reopen_propagate_append_and_position(முழு fd, முழு flags) {
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

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(!(flags & (O_APPEND|O_DIRECTORY)));

        முழு existing_flags = fcntl(fd, F_GETFL);
        எனில் (existing_flags < 0)
                திரும்பு -errno;

        முழு new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        எனில் (new_fd < 0)
                திரும்பு new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        எனில் (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                எனில் (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                இல்லையெனில் எனில் (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        திரும்பு new_fd;
}

முழு fd_reopen_condition(
                முழு fd,
                முழு flags,
                முழு mask,
                முழு *ret_new_fd) {

        முழு r, new_fd;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, O_CREAT));
        ப்ரதிஜ்ஞா(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        எனில் (r < 0)
                திரும்பு -errno;

        எனில் ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                திரும்பு fd;
        }

        new_fd = fd_reopen(fd, flags);
        எனில் (new_fd < 0)
                திரும்பு new_fd;

        *ret_new_fd = new_fd;
        திரும்பு new_fd;
}

முழு fd_is_opath(முழு fd) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = fcntl(fd, F_GETFL);
        எனில் (r < 0)
                திரும்பு -errno;

        திரும்பு FLAGS_SET(r, O_PATH);
}

முழு fd_vet_accmode(முழு fd, முழு mode) {
        முழு flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        எனில் (flags < 0)
                திரும்பு -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        எனில் (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                திரும்பு -EISDIR;

        எனில் (FLAGS_SET(flags, O_PATH))
                திரும்பு -EBADFD;

        flags &= O_ACCMODE_STRICT;

        எனில் (flags == mode)
                திரும்பு 1;

        எனில் (flags == O_RDWR)
                திரும்பு 0;

        திரும்பு -EPROTOTYPE;
}

முழு fd_is_writable(முழு fd) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        எனில் (r >= 0)
                திரும்பு true;

        எனில் (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                திரும்பு false;

        திரும்பு r;
}

முழு fd_verify_safe_flags_full(முழு fd, முழு extra_flags) {
        முழு flags, unexpected_flags;

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

        ப்ரதிஜ்ஞா(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        எனில் (flags < 0)
                திரும்பு -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        எனில் (unexpected_flags != 0)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        திரும்பு flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(வெறுமை) {
        _cleanup_free_ எழுத்து *nr_open = வெற்று;
        முழு r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_வரி_file("/proc/sys/fs/nr_open", &nr_open);
        எனில் (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        இல்லையெனில் {
                unsigned v;

                r = safe_atou(nr_open, &v);
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                இல்லையெனில்
                        திரும்பு v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        திரும்பு NR_OPEN_DEFAULT;
}

முழு fd_get_diskseq(முழு fd, uint64_t *ret) {
        uint64_t diskseq;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                எனில் (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        திரும்பு -errno;

                திரும்பு -EOPNOTSUPP;
        }

        *ret = diskseq;

        திரும்பு 0;
}

நிலையான bool is_literal_வேர்(மாறிலி எழுத்து *p) {
        எனில் (!p)
                திரும்பு false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        திரும்பு n >= 1 && p[n] == 0;
}

முழு path_is_வேர்_at(முழு dir_fd, மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));

        எனில் (dir_fd == XAT_FDROOT && isempty(path))
                திரும்பு true;

        எனில் (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_வேர்(path))
                திரும்பு true;

        _cleanup_close_ முழு fd = -EBADF;
        எனில் (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                எனில் (fd == -ENOTDIR)
                        திரும்பு false; /* the root dir must be a dir */
                எனில் (fd < 0)
                        திரும்பு fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        திரும்பு fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

முழு fds_inode_and_mount_same(முழு fd1, முழு fd2) {
        கட்டமைப்பு statx sx1, sx2;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(fd1));
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ வெற்று, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                திரும்பு true;

        r = xstatx(fd2, /* path = */ வெற்று, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        எனில் (r < 0)
                திரும்பு r;

        r = statx_mount_same(&sx1, &sx2);
        எனில் (r <= 0)
                திரும்பு r;

        திரும்பு statx_inode_same(&sx1, &sx2);
}

முழு resolve_xat_fdroot(முழு *fd, மாறிலி எழுத்து **path, எழுத்து **ret_buffer) {
        ப்ரதிஜ்ஞா(fd);
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret_buffer);

        எனில் (*fd != XAT_FDROOT) {
                *ret_buffer = வெற்று;
                திரும்பு 0;
        }

        எனில் (isempty(*path)) {
                *path = "/";
                *ret_buffer = வெற்று;
        } இல்லையெனில் எனில் (!path_is_absolute(*path)) {
                எழுத்து *p = strjoin("/", *path);
                எனில் (!p)
                        திரும்பு -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        திரும்பு 1;
}

எழுத்து* format_proc_fd_path(எழுத்து buf[நிலையான PROC_FD_PATH_MAX], முழு fd) {
        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        திரும்பு buf;
}

மாறிலி எழுத்து* accmode_to_string(முழு flags) {
        தேர்வு (flags & O_ACCMODE_STRICT) {
        நிலை O_RDONLY:
                திரும்பு "ro";
        நிலை O_WRONLY:
                திரும்பு "wo";
        நிலை O_RDWR:
                திரும்பு "rw";
        இயல்பு:
                திரும்பு வெற்று;
        }
}

எழுத்து* format_proc_pid_fd_path(எழுத்து buf[நிலையான PROC_PID_FD_PATH_MAX], pid_t pid, முழு fd) {
        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        திரும்பு buf;
}

முழு proc_fd_enoent_errno(வெறுமை) {
        முழு r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        எனில் (r == 0)
                திரும்பு -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        எனில் (r > 0)
                திரும்பு -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        திரும்பு -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
