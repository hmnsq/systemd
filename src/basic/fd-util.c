/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <linux/fs.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/kcmp.h>
#ضمن <sys/resource.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "format-util.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "process-util.h"
#ضمن "sort-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

صحيح close_nointr(صحيح fd) {
        أكد(fd >= 0);

        𐡄𐡍 (close(fd) >= 0)
                أرجع 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        𐡄𐡍 (errno == EINTR)
                أرجع 0;

        أرجع -errno;
}

صحيح safe_close(صحيح fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        𐡄𐡍 (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        أرجع -EBADF;
}

فراغ safe_close_pair(صحيح p[ساكن 2]) {
        أكد(p);

        𐡄𐡍 (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                أرجع;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

فراغ close_many(ثابت صحيح fds[], size_t n_fds) {
        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

فراغ close_many_unset(صحيح fds[], size_t n_fds) {
        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

فراغ close_many_and_free(صحيح *fds, size_t n_fds) {
        أكد(fds || n_fds == 0);

        close_many(fds, n_fds);
        حرر(fds);
}

صحيح fclose_nointr(FILE *f) {
        أكد(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        𐡄𐡍 (fclose(f) == 0)
                أرجع 0;

        𐡄𐡍 (errno == EINTR)
                أرجع 0;

        أرجع errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        𐡄𐡍 (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        أرجع عدم;
}

DIR* safe_closedir(DIR *d) {

        𐡄𐡍 (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        أرجع عدم;
}

صحيح fd_nonblock(صحيح fd, bool nonblock) {
        صحيح flags, nflags;

        أكد(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        𐡄𐡍 (flags < 0)
                أرجع -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        𐡄𐡍 (nflags == flags)
                أرجع 0;

        𐡄𐡍 (fcntl(fd, F_SETFL, nflags) < 0)
                أرجع -errno;

        أرجع 1;
}

فراغ nonblock_resetp(صحيح *fd) {
        أكد(fd);

        PROTECT_ERRNO;

        𐡄𐡍 (*fd >= 0)
                (فراغ) fd_nonblock(*fd, false);
}

صحيح stdio_disable_nonblock(فراغ) {
        صحيح ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        أرجع ret;
}

صحيح fd_cloexec(صحيح fd, bool cloexec) {
        صحيح flags, nflags;

        أكد(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        𐡄𐡍 (flags < 0)
                أرجع -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        𐡄𐡍 (nflags == flags)
                أرجع 0;

        أرجع RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

صحيح fd_cloexec_many(ثابت صحيح fds[], size_t n_fds, bool cloexec) {
        صحيح r = 0;

        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                𐡄𐡍 (*fd < 0) /* Skip gracefully over already invalidated fds */
                        استمر;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        أرجع r;
}

ساكن bool fd_in_set(صحيح fd, ثابت صحيح fds[], size_t n_fds) {
        أكد(fd >= 0);
        أكد(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                𐡄𐡍 (*i < 0)
                        استمر;

                𐡄𐡍 (*i == fd)
                        أرجع true;
        }

        أرجع false;
}

صحيح get_max_fd(فراغ) {
        بنية rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        𐡄𐡍 (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                أرجع -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        𐡄𐡍 (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                أرجع FD_SETSIZE-1;

        𐡄𐡍 (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                أرجع INT_MAX;

        أرجع (صحيح) (m - 1);
}

صحيح close_all_fds_frugal(ثابت صحيح except[], size_t n_except) {
        صحيح max_fd, r = 0;

        أكد(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        𐡄𐡍 (max_fd < 0)
                أرجع max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        𐡄𐡍 (max_fd > MAX_FD_LOOP_LIMIT)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        لكل (صحيح fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                صحيح q;

                𐡄𐡍 (fd_in_set(fd, except, n_except))
                        استمر;

                q = close_nointr(fd);
                𐡄𐡍 (q != -EBADF)
                        RET_GATHER(r, q);
        }

        أرجع r;
}

ساكن صحيح close_all_fds_special_case(ثابت صحيح except[], size_t n_except) {
        أكد(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        𐡄𐡍 (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        بدل (n_except) {

        حالة 0:
                /* Close everything. Yay! */
                𐡄𐡍 (close_range(3, INT_MAX, 0) < 0)
                        أرجع -errno;

                أرجع 1;

        حالة 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                𐡄𐡍 (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        أرجع -errno;

                𐡄𐡍 (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        أرجع -errno;

                أرجع 1;

        افتراضي:
                أرجع 0;
        }
}

صحيح close_all_fds_without_malloc(ثابت صحيح except[], size_t n_except) {
        صحيح r;

        أكد(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (r > 0) /* special case worked! */
                أرجع 0;

        أرجع close_all_fds_frugal(except, n_except);
}

صحيح close_all_fds(ثابت صحيح except[], size_t n_except) {
        صحيح r;

        أكد(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (r > 0) /* special case worked! */
                أرجع 0;

        _cleanup_free_ صحيح *sorted_malloc = عدم;
        size_t n_sorted;
        صحيح *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        أكد(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        𐡄𐡍 (n_sorted > ALLOCA_MAX / حجم(صحيح)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(صحيح, n_sorted);
        وإلا
                sorted = newa(صحيح, n_sorted);

        𐡄𐡍 (!sorted) /* Fallback on OOM. */
                أرجع close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * حجم(صحيح));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        لكل (size_t i = 0; i < n_sorted-1; i++) {
                صحيح start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                أكد(end >= start);

                𐡄𐡍 (end - start <= 1)
                        استمر;

                /* Close everything between the start and end fds (both of which shall stay open) */
                𐡄𐡍 (close_range(start + 1, end - 1, 0) < 0)
                        أرجع -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        𐡄𐡍 (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                أرجع 0;

        𐡄𐡍 (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                أرجع -errno;

        أرجع 0;
}

صحيح pack_fds(صحيح fds[], size_t n_fds) {
        𐡄𐡍 (n_fds <= 0)
                أرجع 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        أكد(fds);

        لكل (صحيح start = 0;;) {
                صحيح restart_from = -1;

                لكل (صحيح i = start; i < (صحيح) n_fds; i++) {
                        صحيح nfd;

                        /* Already at right index? */
                        𐡄𐡍 (fds[i] == i + 3)
                                استمر;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        𐡄𐡍 (nfd < 0)
                                أرجع -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        𐡄𐡍 (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                𐡄𐡍 (restart_from < 0)
                        اكسر;

                start = restart_from;
        }

        أكد(fds[0] == 3);

        أرجع 0;
}

صحيح fd_validate(صحيح fd) {
        𐡄𐡍 (fd < 0)
                أرجع -EBADF;

        𐡄𐡍 (fcntl(fd, F_GETFD) < 0)
                أرجع -errno;

        أرجع 0;
}

صحيح same_fd(صحيح a, صحيح b) {
        بنية stat sta, stb;
        pid_t pid;
        صحيح r, fa, fb;

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

        𐡄𐡍 (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                𐡄𐡍 (r < 0)
                        أرجع r;

                أرجع true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        𐡄𐡍 (r > 0)
                أرجع true;
        𐡄𐡍 (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                𐡄𐡍 (r < 0)
                        أرجع r;

                أرجع false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        𐡄𐡍 (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                𐡄𐡍 (r < 0)
                        أرجع r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } وإلا 𐡄𐡍 (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                أرجع -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        𐡄𐡍 (r >= 0)
                أرجع !r;
        𐡄𐡍 (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                أرجع -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        𐡄𐡍 (fstat(a, &sta) < 0)
                أرجع -errno;

        𐡄𐡍 (fstat(b, &stb) < 0)
                أرجع -errno;

        𐡄𐡍 (!stat_inode_same(&sta, &stb))
                أرجع false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        𐡄𐡍 (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                أرجع false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        𐡄𐡍 (fa < 0)
                أرجع -errno;

        fb = fcntl(b, F_GETFL);
        𐡄𐡍 (fb < 0)
                أرجع -errno;

        أرجع fa == fb;
}

bool fdname_is_valid(ثابت حرف *s) {
        ثابت حرف *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        𐡄𐡍 (!s)
                أرجع false;

        لكل (p = s; *p; p++) {
                𐡄𐡍 (*p < ' ')
                        أرجع false;
                𐡄𐡍 (*p >= 127)
                        أرجع false;
                𐡄𐡍 (*p == ':')
                        أرجع false;
        }

        أرجع p - s <= FDNAME_MAX;
}

صحيح fd_get_path(صحيح fd, حرف **ret) {
        صحيح r;

        أكد(wildcard_fd_is_valid(fd));

        𐡄𐡍 (fd == AT_FDCWD)
                أرجع safe_getcwd(ret);
        𐡄𐡍 (fd == XAT_FDROOT)
                أرجع strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        𐡄𐡍 (r == -ENOENT)
                أرجع proc_fd_enoent_errno();
        أرجع r;
}

صحيح move_fd(صحيح from, صحيح to, صحيح cloexec) {
        صحيح r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        𐡄𐡍 (from < 0)
                أرجع -EBADF;
        𐡄𐡍 (to < 0)
                أرجع -EBADF;

        𐡄𐡍 (from == to) {

                𐡄𐡍 (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }

                أرجع to;
        }

        𐡄𐡍 (cloexec < 0) {
                صحيح fl;

                fl = fcntl(from, F_GETFD, 0);
                𐡄𐡍 (fl < 0)
                        أرجع -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        𐡄𐡍 (r < 0)
                أرجع -errno;

        أكد(r == to);

        safe_close(from);

        أرجع to;
}

صحيح fd_move_above_stdio(صحيح fd) {
        صحيح flags, copy;
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

        𐡄𐡍 (fd < 0 || fd > 2)
                أرجع fd;

        flags = fcntl(fd, F_GETFD, 0);
        𐡄𐡍 (flags < 0)
                أرجع fd;

        𐡄𐡍 (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        وإلا
                copy = fcntl(fd, F_DUPFD, 3);
        𐡄𐡍 (copy < 0)
                أرجع fd;

        أكد(copy > 2);

        (فراغ) close(fd);
        أرجع copy;
}

صحيح rearrange_stdio(صحيح original_input_fd, صحيح original_output_fd, صحيح original_error_fd) {
        صحيح fd[3] = { original_input_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_input_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        𐡄𐡍 (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                𐡄𐡍 (null_fd < 0) {
                        r = -errno;
                        اذهب finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                𐡄𐡍 (null_fd < 3) {
                        صحيح copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        𐡄𐡍 (copy < 0) {
                                r = -errno;
                                اذهب finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        لكل (صحيح i = 0; i < 3; i++)
                𐡄𐡍 (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                وإلا 𐡄𐡍 (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        𐡄𐡍 (copy_fd[i] < 0) {
                                r = -errno;
                                اذهب finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        لكل (صحيح i = 0; i < 3; i++)
                𐡄𐡍 (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        𐡄𐡍 (r < 0)
                                اذهب finish;
                } وإلا {
                        أكد(fd[i] > 2);

                        𐡄𐡍 (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                اذهب finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_input_fd);
        𐡄𐡍 (original_output_fd != original_input_fd)
                safe_close_above_stdio(original_output_fd);
        𐡄𐡍 (original_error_fd != original_input_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        أرجع r;
}

صحيح fd_reopen(صحيح fd, صحيح flags) {
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

        𐡄𐡍 (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                أرجع -ELOOP;

        𐡄𐡍 (fd == XAT_FDROOT)
                أرجع RET_NERRNO(open("/", flags | O_DIRECTORY));

        𐡄𐡍 (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                أرجع RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        صحيح new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        𐡄𐡍 (new_fd < 0) {
                𐡄𐡍 (errno != ENOENT)
                        أرجع -errno;

                أرجع proc_fd_enoent_errno();
        }

        أرجع new_fd;
}

صحيح fd_reopen_propagate_append_and_position(صحيح fd, صحيح flags) {
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

        صحيح existing_flags = fcntl(fd, F_GETFL);
        𐡄𐡍 (existing_flags < 0)
                أرجع -errno;

        صحيح new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        𐡄𐡍 (new_fd < 0)
                أرجع new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        𐡄𐡍 (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                𐡄𐡍 (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                وإلا 𐡄𐡍 (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        أرجع new_fd;
}

صحيح fd_reopen_condition(
                صحيح fd,
                صحيح flags,
                صحيح mask,
                صحيح *ret_new_fd) {

        صحيح r, new_fd;

        أكد(fd >= 0);
        أكد(!FLAGS_SET(flags, O_CREAT));
        أكد(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        𐡄𐡍 (r < 0)
                أرجع -errno;

        𐡄𐡍 ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                أرجع fd;
        }

        new_fd = fd_reopen(fd, flags);
        𐡄𐡍 (new_fd < 0)
                أرجع new_fd;

        *ret_new_fd = new_fd;
        أرجع new_fd;
}

صحيح fd_is_opath(صحيح fd) {
        صحيح r;

        أكد(fd >= 0);

        r = fcntl(fd, F_GETFL);
        𐡄𐡍 (r < 0)
                أرجع -errno;

        أرجع FLAGS_SET(r, O_PATH);
}

صحيح fd_vet_accmode(صحيح fd, صحيح mode) {
        صحيح flags;

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
        𐡄𐡍 (flags < 0)
                أرجع -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        𐡄𐡍 (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                أرجع -EISDIR;

        𐡄𐡍 (FLAGS_SET(flags, O_PATH))
                أرجع -EBADFD;

        flags &= O_ACCMODE_STRICT;

        𐡄𐡍 (flags == mode)
                أرجع 1;

        𐡄𐡍 (flags == O_RDWR)
                أرجع 0;

        أرجع -EPROTOTYPE;
}

صحيح fd_is_writable(صحيح fd) {
        صحيح r;

        أكد(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        𐡄𐡍 (r >= 0)
                أرجع true;

        𐡄𐡍 (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                أرجع false;

        أرجع r;
}

صحيح fd_verify_safe_flags_full(صحيح fd, صحيح extra_flags) {
        صحيح flags, unexpected_flags;

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
        𐡄𐡍 (flags < 0)
                أرجع -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        𐡄𐡍 (unexpected_flags != 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        أرجع flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(فراغ) {
        _cleanup_free_ حرف *nr_open = عدم;
        صحيح r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_line_file("/proc/sys/fs/nr_open", &nr_open);
        𐡄𐡍 (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        وإلا {
                unsigned v;

                r = safe_atou(nr_open, &v);
                𐡄𐡍 (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                وإلا
                        أرجع v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        أرجع NR_OPEN_DEFAULT;
}

صحيح fd_get_diskseq(صحيح fd, uint64_t *ret) {
        uint64_t diskseq;

        أكد(fd >= 0);
        أكد(ret);

        𐡄𐡍 (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                𐡄𐡍 (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        أرجع -errno;

                أرجع -EOPNOTSUPP;
        }

        *ret = diskseq;

        أرجع 0;
}

ساكن bool is_literal_root(ثابت حرف *p) {
        𐡄𐡍 (!p)
                أرجع false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        أرجع n >= 1 && p[n] == 0;
}

صحيح path_is_root_at(صحيح dir_fd, ثابت حرف *path) {
        أكد(wildcard_fd_is_valid(dir_fd));

        𐡄𐡍 (dir_fd == XAT_FDROOT && isempty(path))
                أرجع true;

        𐡄𐡍 (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_root(path))
                أرجع true;

        _cleanup_close_ صحيح fd = -EBADF;
        𐡄𐡍 (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                𐡄𐡍 (fd == -ENOTDIR)
                        أرجع false; /* the root dir must be a dir */
                𐡄𐡍 (fd < 0)
                        أرجع fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        أرجع fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

صحيح fds_inode_and_mount_same(صحيح fd1, صحيح fd2) {
        بنية statx sx1, sx2;
        صحيح r;

        أكد(wildcard_fd_is_valid(fd1));
        أكد(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ عدم, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                أرجع true;

        r = xstatx(fd2, /* path = */ عدم, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = statx_mount_same(&sx1, &sx2);
        𐡄𐡍 (r <= 0)
                أرجع r;

        أرجع statx_inode_same(&sx1, &sx2);
}

صحيح resolve_xat_fdroot(صحيح *fd, ثابت حرف **path, حرف **ret_buffer) {
        أكد(fd);
        أكد(path);
        أكد(ret_buffer);

        𐡄𐡍 (*fd != XAT_FDROOT) {
                *ret_buffer = عدم;
                أرجع 0;
        }

        𐡄𐡍 (isempty(*path)) {
                *path = "/";
                *ret_buffer = عدم;
        } وإلا 𐡄𐡍 (!path_is_absolute(*path)) {
                حرف *p = strjoin("/", *path);
                𐡄𐡍 (!p)
                        أرجع -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        أرجع 1;
}

حرف* format_proc_fd_path(حرف buf[ساكن PROC_FD_PATH_MAX], صحيح fd) {
        أكد(buf);
        أكد(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        أرجع buf;
}

ثابت حرف* accmode_to_string(صحيح flags) {
        بدل (flags & O_ACCMODE_STRICT) {
        حالة O_RDONLY:
                أرجع "ro";
        حالة O_WRONLY:
                أرجع "wo";
        حالة O_RDWR:
                أرجع "rw";
        افتراضي:
                أرجع عدم;
        }
}

حرف* format_proc_pid_fd_path(حرف buf[ساكن PROC_PID_FD_PATH_MAX], pid_t pid, صحيح fd) {
        أكد(buf);
        أكد(fd >= 0);
        أكد(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        أرجع buf;
}

صحيح proc_fd_enoent_errno(فراغ) {
        صحيح r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        𐡄𐡍 (r == 0)
                أرجع -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        𐡄𐡍 (r > 0)
                أرجع -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        أرجع -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
