/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <linux/fs.h>
#شامل <sys/ioctl.h>
#شامل <sys/kcmp.h>
#شامل <sys/resource.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "format-util.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "sort-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

صحیح close_nointr(صحیح fd) {
        أكد(fd >= 0);

        اگر (close(fd) >= 0)
                بازگشت 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        اگر (errno == EINTR)
                بازگشت 0;

        بازگشت -errno;
}

صحیح safe_close(صحیح fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        اگر (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        بازگشت -EBADF;
}

تهی safe_close_pair(صحیح p[ایستا 2]) {
        أكد(p);

        اگر (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                بازگشت;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

تهی close_many(ثابت صحیح fds[], size_t n_fds) {
        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

تهی close_many_unset(صحیح fds[], size_t n_fds) {
        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

تهی close_many_and_free(صحیح *fds, size_t n_fds) {
        أكد(fds || n_fds == 0);

        close_many(fds, n_fds);
        آزاد(fds);
}

صحیح fclose_nointr(FILE *f) {
        أكد(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        اگر (fclose(f) == 0)
                بازگشت 0;

        اگر (errno == EINTR)
                بازگشت 0;

        بازگشت errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        اگر (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        بازگشت پوچ;
}

DIR* safe_closedir(DIR *d) {

        اگر (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        بازگشت پوچ;
}

صحیح fd_nonblock(صحیح fd, bool nonblock) {
        صحیح flags, nflags;

        أكد(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        اگر (flags < 0)
                بازگشت -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        اگر (nflags == flags)
                بازگشت 0;

        اگر (fcntl(fd, F_SETFL, nflags) < 0)
                بازگشت -errno;

        بازگشت 1;
}

تهی nonblock_resetp(صحیح *fd) {
        أكد(fd);

        PROTECT_ERRNO;

        اگر (*fd >= 0)
                (تهی) fd_nonblock(*fd, false);
}

صحیح stdio_disable_nonblock(تهی) {
        صحیح ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        بازگشت ret;
}

صحیح fd_cloexec(صحیح fd, bool cloexec) {
        صحیح flags, nflags;

        أكد(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        اگر (flags < 0)
                بازگشت -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        اگر (nflags == flags)
                بازگشت 0;

        بازگشت RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

صحیح fd_cloexec_many(ثابت صحیح fds[], size_t n_fds, bool cloexec) {
        صحیح r = 0;

        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                اگر (*fd < 0) /* Skip gracefully over already invalidated fds */
                        ادامه;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        بازگشت r;
}

ایستا bool fd_in_set(صحیح fd, ثابت صحیح fds[], size_t n_fds) {
        أكد(fd >= 0);
        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                اگر (*i < 0)
                        ادامه;

                اگر (*i == fd)
                        بازگشت true;
        }

        بازگشت false;
}

صحیح get_max_fd(تهی) {
        ساختار rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        اگر (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                بازگشت -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        اگر (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                بازگشت FD_SETSIZE-1;

        اگر (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                بازگشت INT_MAX;

        بازگشت (صحیح) (m - 1);
}

صحیح close_all_fds_frugal(ثابت صحیح except[], size_t n_except) {
        صحیح max_fd, r = 0;

        أكد(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        اگر (max_fd < 0)
                بازگشت max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        اگر (max_fd > MAX_FD_LOOP_LIMIT)
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        برای (صحیح fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                صحیح q;

                اگر (fd_in_set(fd, except, n_except))
                        ادامه;

                q = close_nointr(fd);
                اگر (q != -EBADF)
                        RET_GATHER(r, q);
        }

        بازگشت r;
}

ایستا صحیح close_all_fds_special_case(ثابت صحیح except[], size_t n_except) {
        أكد(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        اگر (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        انتخاب (n_except) {

        حالت 0:
                /* Close everything. Yay! */
                اگر (close_range(3, INT_MAX, 0) < 0)
                        بازگشت -errno;

                بازگشت 1;

        حالت 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                اگر (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        بازگشت -errno;

                اگر (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        بازگشت -errno;

                بازگشت 1;

        پیشفرض:
                بازگشت 0;
        }
}

صحیح close_all_fds_without_malloc(ثابت صحیح except[], size_t n_except) {
        صحیح r;

        أكد(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        اگر (r < 0)
                بازگشت r;
        اگر (r > 0) /* special case worked! */
                بازگشت 0;

        بازگشت close_all_fds_frugal(except, n_except);
}

صحیح close_all_fds(ثابت صحیح except[], size_t n_except) {
        صحیح r;

        أكد(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        اگر (r < 0)
                بازگشت r;
        اگر (r > 0) /* special case worked! */
                بازگشت 0;

        _cleanup_free_ صحیح *sorted_malloc = پوچ;
        size_t n_sorted;
        صحیح *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        أكد(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        اگر (n_sorted > ALLOCA_MAX / اندازه(صحیح)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(صحیح, n_sorted);
        وگرنه
                sorted = newa(صحیح, n_sorted);

        اگر (!sorted) /* Fallback on OOM. */
                بازگشت close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * اندازه(صحیح));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        برای (size_t i = 0; i < n_sorted-1; i++) {
                صحیح start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                أكد(end >= start);

                اگر (end - start <= 1)
                        ادامه;

                /* Close everything between the start and end fds (both of which shall stay open) */
                اگر (close_range(start + 1, end - 1, 0) < 0)
                        بازگشت -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        اگر (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                بازگشت 0;

        اگر (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                بازگشت -errno;

        بازگشت 0;
}

صحیح pack_fds(صحیح fds[], size_t n_fds) {
        اگر (n_fds <= 0)
                بازگشت 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        أكد(fds);

        برای (صحیح start = 0;;) {
                صحیح restart_from = -1;

                برای (صحیح i = start; i < (صحیح) n_fds; i++) {
                        صحیح nfd;

                        /* Already at right index? */
                        اگر (fds[i] == i + 3)
                                ادامه;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        اگر (nfd < 0)
                                بازگشت -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        اگر (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                اگر (restart_from < 0)
                        بشکن;

                start = restart_from;
        }

        أكد(fds[0] == 3);

        بازگشت 0;
}

صحیح fd_validate(صحیح fd) {
        اگر (fd < 0)
                بازگشت -EBADF;

        اگر (fcntl(fd, F_GETFD) < 0)
                بازگشت -errno;

        بازگشت 0;
}

صحیح same_fd(صحیح a, صحیح b) {
        ساختار stat sta, stb;
        pid_t pid;
        صحیح r, fa, fb;

        أكد(a >= 0);
        أكد(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        اگر (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                اگر (r < 0)
                        بازگشت r;

                بازگشت true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        اگر (r > 0)
                بازگشت true;
        اگر (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                اگر (r < 0)
                        بازگشت r;

                بازگشت false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        اگر (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                اگر (r < 0)
                        بازگشت r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } وگرنه اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                بازگشت -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        اگر (r >= 0)
                بازگشت !r;
        اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                بازگشت -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        اگر (fstat(a, &sta) < 0)
                بازگشت -errno;

        اگر (fstat(b, &stb) < 0)
                بازگشت -errno;

        اگر (!stat_inode_same(&sta, &stb))
                بازگشت false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        اگر (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                بازگشت false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        اگر (fa < 0)
                بازگشت -errno;

        fb = fcntl(b, F_GETFL);
        اگر (fb < 0)
                بازگشت -errno;

        بازگشت fa == fb;
}

bool fdname_is_valid(ثابت نویسه *s) {
        ثابت نویسه *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        اگر (!s)
                بازگشت false;

        برای (p = s; *p; p++) {
                اگر (*p < ' ')
                        بازگشت false;
                اگر (*p >= 127)
                        بازگشت false;
                اگر (*p == ':')
                        بازگشت false;
        }

        بازگشت p - s <= FDNAME_MAX;
}

صحیح fd_get_path(صحیح fd, نویسه **ret) {
        صحیح r;

        أكد(wildcard_fd_is_valid(fd));

        اگر (fd == AT_FDCWD)
                بازگشت safe_getcwd(ret);
        اگر (fd == XAT_FDROOT)
                بازگشت strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        اگر (r == -ENOENT)
                بازگشت proc_fd_enoent_errno();
        بازگشت r;
}

صحیح move_fd(صحیح from, صحیح to, صحیح cloexec) {
        صحیح r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        اگر (from < 0)
                بازگشت -EBADF;
        اگر (to < 0)
                بازگشت -EBADF;

        اگر (from == to) {

                اگر (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        اگر (r < 0)
                                بازگشت r;
                }

                بازگشت to;
        }

        اگر (cloexec < 0) {
                صحیح fl;

                fl = fcntl(from, F_GETFD, 0);
                اگر (fl < 0)
                        بازگشت -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        اگر (r < 0)
                بازگشت -errno;

        أكد(r == to);

        safe_close(from);

        بازگشت to;
}

صحیح fd_move_above_stdio(صحیح fd) {
        صحیح flags, copy;
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

        اگر (fd < 0 || fd > 2)
                بازگشت fd;

        flags = fcntl(fd, F_GETFD, 0);
        اگر (flags < 0)
                بازگشت fd;

        اگر (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        وگرنه
                copy = fcntl(fd, F_DUPFD, 3);
        اگر (copy < 0)
                بازگشت fd;

        أكد(copy > 2);

        (تهی) close(fd);
        بازگشت copy;
}

صحیح rearrange_stdio(صحیح original_درون_داد_fd, صحیح original_output_fd, صحیح original_error_fd) {
        صحیح fd[3] = { original_درون_داد_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_درون_داد_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        اگر (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                اگر (null_fd < 0) {
                        r = -errno;
                        برو finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                اگر (null_fd < 3) {
                        صحیح copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        اگر (copy < 0) {
                                r = -errno;
                                برو finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        برای (صحیح i = 0; i < 3; i++)
                اگر (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                وگرنه اگر (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        اگر (copy_fd[i] < 0) {
                                r = -errno;
                                برو finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        برای (صحیح i = 0; i < 3; i++)
                اگر (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        اگر (r < 0)
                                برو finish;
                } وگرنه {
                        أكد(fd[i] > 2);

                        اگر (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                برو finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_درون_داد_fd);
        اگر (original_output_fd != original_درون_داد_fd)
                safe_close_above_stdio(original_output_fd);
        اگر (original_error_fd != original_درون_داد_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        بازگشت r;
}

صحیح fd_reopen(صحیح fd, صحیح flags) {
        أكد(wildcard_fd_is_valid(fd));
        أكد(!FLAGS_SET(flags, O_CREAT));

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

        اگر (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                بازگشت -ELOOP;

        اگر (fd == XAT_FDROOT)
                بازگشت RET_NERRNO(open("/", flags | O_DIRECTORY));

        اگر (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                بازگشت RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        صحیح new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        اگر (new_fd < 0) {
                اگر (errno != ENOENT)
                        بازگشت -errno;

                بازگشت proc_fd_enoent_errno();
        }

        بازگشت new_fd;
}

صحیح fd_reopen_propagate_append_and_position(صحیح fd, صحیح flags) {
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

        أكد(fd >= 0);
        أكد(!(flags & (O_APPEND|O_DIRECTORY)));

        صحیح existing_flags = fcntl(fd, F_GETFL);
        اگر (existing_flags < 0)
                بازگشت -errno;

        صحیح new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        اگر (new_fd < 0)
                بازگشت new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        اگر (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                اگر (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                وگرنه اگر (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        بازگشت new_fd;
}

صحیح fd_reopen_condition(
                صحیح fd,
                صحیح flags,
                صحیح mask,
                صحیح *ret_new_fd) {

        صحیح r, new_fd;

        أكد(fd >= 0);
        أكد(!FLAGS_SET(flags, O_CREAT));
        أكد(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        اگر (r < 0)
                بازگشت -errno;

        اگر ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                بازگشت fd;
        }

        new_fd = fd_reopen(fd, flags);
        اگر (new_fd < 0)
                بازگشت new_fd;

        *ret_new_fd = new_fd;
        بازگشت new_fd;
}

صحیح fd_is_opath(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        r = fcntl(fd, F_GETFL);
        اگر (r < 0)
                بازگشت -errno;

        بازگشت FLAGS_SET(r, O_PATH);
}

صحیح fd_vet_accmode(صحیح fd, صحیح mode) {
        صحیح flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        أكد(fd >= 0);
        أكد(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        اگر (flags < 0)
                بازگشت -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        اگر (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                بازگشت -EISDIR;

        اگر (FLAGS_SET(flags, O_PATH))
                بازگشت -EBADFD;

        flags &= O_ACCMODE_STRICT;

        اگر (flags == mode)
                بازگشت 1;

        اگر (flags == O_RDWR)
                بازگشت 0;

        بازگشت -EPROTOTYPE;
}

صحیح fd_is_writable(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        اگر (r >= 0)
                بازگشت true;

        اگر (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                بازگشت false;

        بازگشت r;
}

صحیح fd_verify_safe_flags_full(صحیح fd, صحیح extra_flags) {
        صحیح flags, unexpected_flags;

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

        أكد(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        اگر (flags < 0)
                بازگشت -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        اگر (unexpected_flags != 0)
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        بازگشت flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(تهی) {
        _cleanup_free_ نویسه *nr_open = پوچ;
        صحیح r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_سطر_file("/proc/sys/fs/nr_open", &nr_open);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        وگرنه {
                unsigned v;

                r = safe_atou(nr_open, &v);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                وگرنه
                        بازگشت v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        بازگشت NR_OPEN_DEFAULT;
}

صحیح fd_get_diskseq(صحیح fd, uint64_t *ret) {
        uint64_t diskseq;

        أكد(fd >= 0);
        أكد(ret);

        اگر (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        بازگشت -errno;

                بازگشت -EOPNOTSUPP;
        }

        *ret = diskseq;

        بازگشت 0;
}

ایستا bool is_literal_ریشه(ثابت نویسه *p) {
        اگر (!p)
                بازگشت false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        بازگشت n >= 1 && p[n] == 0;
}

صحیح path_is_ریشه_at(صحیح dir_fd, ثابت نویسه *path) {
        أكد(wildcard_fd_is_valid(dir_fd));

        اگر (dir_fd == XAT_FDROOT && isempty(path))
                بازگشت true;

        اگر (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_ریشه(path))
                بازگشت true;

        _cleanup_close_ صحیح fd = -EBADF;
        اگر (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                اگر (fd == -ENOTDIR)
                        بازگشت false; /* the root dir must be a dir */
                اگر (fd < 0)
                        بازگشت fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        بازگشت fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

صحیح fds_inode_and_mount_same(صحیح fd1, صحیح fd2) {
        ساختار statx sx1, sx2;
        صحیح r;

        أكد(wildcard_fd_is_valid(fd1));
        أكد(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ پوچ, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        اگر (r < 0)
                بازگشت r;

        اگر (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                بازگشت true;

        r = xstatx(fd2, /* path = */ پوچ, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        اگر (r < 0)
                بازگشت r;

        r = statx_mount_same(&sx1, &sx2);
        اگر (r <= 0)
                بازگشت r;

        بازگشت statx_inode_same(&sx1, &sx2);
}

صحیح resolve_xat_fdroot(صحیح *fd, ثابت نویسه **path, نویسه **ret_buffer) {
        أكد(fd);
        أكد(path);
        أكد(ret_buffer);

        اگر (*fd != XAT_FDROOT) {
                *ret_buffer = پوچ;
                بازگشت 0;
        }

        اگر (isempty(*path)) {
                *path = "/";
                *ret_buffer = پوچ;
        } وگرنه اگر (!path_is_absolute(*path)) {
                نویسه *p = strjoin("/", *path);
                اگر (!p)
                        بازگشت -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        بازگشت 1;
}

نویسه* format_proc_fd_path(نویسه buf[ایستا PROC_FD_PATH_MAX], صحیح fd) {
        أكد(buf);
        أكد(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        بازگشت buf;
}

ثابت نویسه* accmode_to_string(صحیح flags) {
        انتخاب (flags & O_ACCMODE_STRICT) {
        حالت O_RDONLY:
                بازگشت "ro";
        حالت O_WRONLY:
                بازگشت "wo";
        حالت O_RDWR:
                بازگشت "rw";
        پیشفرض:
                بازگشت پوچ;
        }
}

نویسه* format_proc_pid_fd_path(نویسه buf[ایستا PROC_PID_FD_PATH_MAX], pid_t pid, صحیح fd) {
        أكد(buf);
        أكد(fd >= 0);
        أكد(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        بازگشت buf;
}

صحیح proc_fd_enoent_errno(تهی) {
        صحیح r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        اگر (r == 0)
                بازگشت -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        اگر (r > 0)
                بازگشت -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        بازگشت -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
