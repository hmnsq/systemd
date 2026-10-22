/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <linux/fs.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/kcmp.h>
#અન્તર્ભાવ <sys/resource.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "sort-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

પૂર્ણાંક close_nointr(પૂર્ણાંક fd) {
        પ્રતિજ્ઞા(fd >= 0);

        જો (close(fd) >= 0)
                પાછા 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        જો (errno == EINTR)
                પાછા 0;

        પાછા -errno;
}

પૂર્ણાંક safe_close(પૂર્ણાંક fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        જો (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        પાછા -EBADF;
}

શૂન્ય safe_close_pair(પૂર્ણાંક p[સ્થિર 2]) {
        પ્રતિજ્ઞા(p);

        જો (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                પાછા;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

શૂન્ય close_many(અચલ પૂર્ણાંક fds[], size_t n_fds) {
        પ્રતિજ્ઞા(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

શૂન્ય close_many_unset(પૂર્ણાંક fds[], size_t n_fds) {
        પ્રતિજ્ઞા(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

શૂન્ય close_many_and_free(પૂર્ણાંક *fds, size_t n_fds) {
        પ્રતિજ્ઞા(fds || n_fds == 0);

        close_many(fds, n_fds);
        મુક્ત(fds);
}

પૂર્ણાંક fclose_nointr(FILE *f) {
        પ્રતિજ્ઞા(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        જો (fclose(f) == 0)
                પાછા 0;

        જો (errno == EINTR)
                પાછા 0;

        પાછા errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        જો (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        પાછા ખાલી;
}

DIR* safe_closedir(DIR *d) {

        જો (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        પાછા ખાલી;
}

પૂર્ણાંક fd_nonblock(પૂર્ણાંક fd, bool nonblock) {
        પૂર્ણાંક flags, nflags;

        પ્રતિજ્ઞા(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        જો (flags < 0)
                પાછા -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        જો (nflags == flags)
                પાછા 0;

        જો (fcntl(fd, F_SETFL, nflags) < 0)
                પાછા -errno;

        પાછા 1;
}

શૂન્ય nonblock_resetp(પૂર્ણાંક *fd) {
        પ્રતિજ્ઞા(fd);

        PROTECT_ERRNO;

        જો (*fd >= 0)
                (શૂન્ય) fd_nonblock(*fd, false);
}

પૂર્ણાંક stdio_disable_nonblock(શૂન્ય) {
        પૂર્ણાંક ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        પાછા ret;
}

પૂર્ણાંક fd_cloexec(પૂર્ણાંક fd, bool cloexec) {
        પૂર્ણાંક flags, nflags;

        પ્રતિજ્ઞા(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        જો (flags < 0)
                પાછા -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        જો (nflags == flags)
                પાછા 0;

        પાછા RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

પૂર્ણાંક fd_cloexec_many(અચલ પૂર્ણાંક fds[], size_t n_fds, bool cloexec) {
        પૂર્ણાંક r = 0;

        પ્રતિજ્ઞા(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                જો (*fd < 0) /* Skip gracefully over already invalidated fds */
                        ચાલુ;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        પાછા r;
}

સ્થિર bool fd_in_set(પૂર્ણાંક fd, અચલ પૂર્ણાંક fds[], size_t n_fds) {
        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                જો (*i < 0)
                        ચાલુ;

                જો (*i == fd)
                        પાછા true;
        }

        પાછા false;
}

પૂર્ણાંક get_max_fd(શૂન્ય) {
        સંરચના rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        જો (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                પાછા -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        જો (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                પાછા FD_SETSIZE-1;

        જો (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                પાછા INT_MAX;

        પાછા (પૂર્ણાંક) (m - 1);
}

પૂર્ણાંક close_all_fds_frugal(અચલ પૂર્ણાંક except[], size_t n_except) {
        પૂર્ણાંક max_fd, r = 0;

        પ્રતિજ્ઞા(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        જો (max_fd < 0)
                પાછા max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        જો (max_fd > MAX_FD_LOOP_LIMIT)
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        માટે (પૂર્ણાંક fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                પૂર્ણાંક q;

                જો (fd_in_set(fd, except, n_except))
                        ચાલુ;

                q = close_nointr(fd);
                જો (q != -EBADF)
                        RET_GATHER(r, q);
        }

        પાછા r;
}

સ્થિર પૂર્ણાંક close_all_fds_special_case(અચલ પૂર્ણાંક except[], size_t n_except) {
        પ્રતિજ્ઞા(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        જો (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        પસંદગી (n_except) {

        સ્થિતિ 0:
                /* Close everything. Yay! */
                જો (close_range(3, INT_MAX, 0) < 0)
                        પાછા -errno;

                પાછા 1;

        સ્થિતિ 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                જો (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        પાછા -errno;

                જો (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        પાછા -errno;

                પાછા 1;

        મૂળભૂત:
                પાછા 0;
        }
}

પૂર્ણાંક close_all_fds_without_malloc(અચલ પૂર્ણાંક except[], size_t n_except) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        જો (r < 0)
                પાછા r;
        જો (r > 0) /* special case worked! */
                પાછા 0;

        પાછા close_all_fds_frugal(except, n_except);
}

પૂર્ણાંક close_all_fds(અચલ પૂર્ણાંક except[], size_t n_except) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        જો (r < 0)
                પાછા r;
        જો (r > 0) /* special case worked! */
                પાછા 0;

        _cleanup_free_ પૂર્ણાંક *sorted_malloc = ખાલી;
        size_t n_sorted;
        પૂર્ણાંક *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        પ્રતિજ્ઞા(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        જો (n_sorted > ALLOCA_MAX / કદ(પૂર્ણાંક)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(પૂર્ણાંક, n_sorted);
        નહીંતર
                sorted = newa(પૂર્ણાંક, n_sorted);

        જો (!sorted) /* Fallback on OOM. */
                પાછા close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * કદ(પૂર્ણાંક));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        માટે (size_t i = 0; i < n_sorted-1; i++) {
                પૂર્ણાંક start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                પ્રતિજ્ઞા(end >= start);

                જો (end - start <= 1)
                        ચાલુ;

                /* Close everything between the start and end fds (both of which shall stay open) */
                જો (close_range(start + 1, end - 1, 0) < 0)
                        પાછા -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        જો (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                પાછા 0;

        જો (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                પાછા -errno;

        પાછા 0;
}

પૂર્ણાંક pack_fds(પૂર્ણાંક fds[], size_t n_fds) {
        જો (n_fds <= 0)
                પાછા 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        પ્રતિજ્ઞા(fds);

        માટે (પૂર્ણાંક start = 0;;) {
                પૂર્ણાંક restart_from = -1;

                માટે (પૂર્ણાંક i = start; i < (પૂર્ણાંક) n_fds; i++) {
                        પૂર્ણાંક nfd;

                        /* Already at right index? */
                        જો (fds[i] == i + 3)
                                ચાલુ;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        જો (nfd < 0)
                                પાછા -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        જો (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                જો (restart_from < 0)
                        તોડો;

                start = restart_from;
        }

        પ્રતિજ્ઞા(fds[0] == 3);

        પાછા 0;
}

પૂર્ણાંક fd_validate(પૂર્ણાંક fd) {
        જો (fd < 0)
                પાછા -EBADF;

        જો (fcntl(fd, F_GETFD) < 0)
                પાછા -errno;

        પાછા 0;
}

પૂર્ણાંક same_fd(પૂર્ણાંક a, પૂર્ણાંક b) {
        સંરચના stat sta, stb;
        pid_t pid;
        પૂર્ણાંક r, fa, fb;

        પ્રતિજ્ઞા(a >= 0);
        પ્રતિજ્ઞા(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        જો (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                જો (r < 0)
                        પાછા r;

                પાછા true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        જો (r > 0)
                પાછા true;
        જો (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                જો (r < 0)
                        પાછા r;

                પાછા false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        જો (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                જો (r < 0)
                        પાછા r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } નહીંતર જો (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                પાછા -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        જો (r >= 0)
                પાછા !r;
        જો (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                પાછા -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        જો (fstat(a, &sta) < 0)
                પાછા -errno;

        જો (fstat(b, &stb) < 0)
                પાછા -errno;

        જો (!stat_inode_same(&sta, &stb))
                પાછા false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        જો (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                પાછા false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        જો (fa < 0)
                પાછા -errno;

        fb = fcntl(b, F_GETFL);
        જો (fb < 0)
                પાછા -errno;

        પાછા fa == fb;
}

bool fdname_is_valid(અચલ અક્ષર *s) {
        અચલ અક્ષર *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        જો (!s)
                પાછા false;

        માટે (p = s; *p; p++) {
                જો (*p < ' ')
                        પાછા false;
                જો (*p >= 127)
                        પાછા false;
                જો (*p == ':')
                        પાછા false;
        }

        પાછા p - s <= FDNAME_MAX;
}

પૂર્ણાંક fd_get_path(પૂર્ણાંક fd, અક્ષર **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(fd));

        જો (fd == AT_FDCWD)
                પાછા safe_getcwd(ret);
        જો (fd == XAT_FDROOT)
                પાછા strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        જો (r == -ENOENT)
                પાછા proc_fd_enoent_errno();
        પાછા r;
}

પૂર્ણાંક move_fd(પૂર્ણાંક from, પૂર્ણાંક to, પૂર્ણાંક cloexec) {
        પૂર્ણાંક r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        જો (from < 0)
                પાછા -EBADF;
        જો (to < 0)
                પાછા -EBADF;

        જો (from == to) {

                જો (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        જો (r < 0)
                                પાછા r;
                }

                પાછા to;
        }

        જો (cloexec < 0) {
                પૂર્ણાંક fl;

                fl = fcntl(from, F_GETFD, 0);
                જો (fl < 0)
                        પાછા -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        જો (r < 0)
                પાછા -errno;

        પ્રતિજ્ઞા(r == to);

        safe_close(from);

        પાછા to;
}

પૂર્ણાંક fd_move_above_stdio(પૂર્ણાંક fd) {
        પૂર્ણાંક flags, copy;
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

        જો (fd < 0 || fd > 2)
                પાછા fd;

        flags = fcntl(fd, F_GETFD, 0);
        જો (flags < 0)
                પાછા fd;

        જો (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        નહીંતર
                copy = fcntl(fd, F_DUPFD, 3);
        જો (copy < 0)
                પાછા fd;

        પ્રતિજ્ઞા(copy > 2);

        (શૂન્ય) close(fd);
        પાછા copy;
}

પૂર્ણાંક rearrange_stdio(પૂર્ણાંક original_આગત_fd, પૂર્ણાંક original_output_fd, પૂર્ણાંક original_error_fd) {
        પૂર્ણાંક fd[3] = { original_આગત_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_આગત_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        જો (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                જો (null_fd < 0) {
                        r = -errno;
                        જાઓ finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                જો (null_fd < 3) {
                        પૂર્ણાંક copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        જો (copy < 0) {
                                r = -errno;
                                જાઓ finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        માટે (પૂર્ણાંક i = 0; i < 3; i++)
                જો (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                નહીંતર જો (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        જો (copy_fd[i] < 0) {
                                r = -errno;
                                જાઓ finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        માટે (પૂર્ણાંક i = 0; i < 3; i++)
                જો (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        જો (r < 0)
                                જાઓ finish;
                } નહીંતર {
                        પ્રતિજ્ઞા(fd[i] > 2);

                        જો (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                જાઓ finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_આગત_fd);
        જો (original_output_fd != original_આગત_fd)
                safe_close_above_stdio(original_output_fd);
        જો (original_error_fd != original_આગત_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        પાછા r;
}

પૂર્ણાંક fd_reopen(પૂર્ણાંક fd, પૂર્ણાંક flags) {
        પ્રતિજ્ઞા(wildcard_fd_is_valid(fd));
        પ્રતિજ્ઞા(!FLAGS_SET(flags, O_CREAT));

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

        જો (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                પાછા -ELOOP;

        જો (fd == XAT_FDROOT)
                પાછા RET_NERRNO(open("/", flags | O_DIRECTORY));

        જો (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                પાછા RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        પૂર્ણાંક new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        જો (new_fd < 0) {
                જો (errno != ENOENT)
                        પાછા -errno;

                પાછા proc_fd_enoent_errno();
        }

        પાછા new_fd;
}

પૂર્ણાંક fd_reopen_propagate_append_and_position(પૂર્ણાંક fd, પૂર્ણાંક flags) {
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

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(!(flags & (O_APPEND|O_DIRECTORY)));

        પૂર્ણાંક existing_flags = fcntl(fd, F_GETFL);
        જો (existing_flags < 0)
                પાછા -errno;

        પૂર્ણાંક new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        જો (new_fd < 0)
                પાછા new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        જો (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                જો (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                નહીંતર જો (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        પાછા new_fd;
}

પૂર્ણાંક fd_reopen_condition(
                પૂર્ણાંક fd,
                પૂર્ણાંક flags,
                પૂર્ણાંક mask,
                પૂર્ણાંક *ret_new_fd) {

        પૂર્ણાંક r, new_fd;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(!FLAGS_SET(flags, O_CREAT));
        પ્રતિજ્ઞા(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        જો (r < 0)
                પાછા -errno;

        જો ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                પાછા fd;
        }

        new_fd = fd_reopen(fd, flags);
        જો (new_fd < 0)
                પાછા new_fd;

        *ret_new_fd = new_fd;
        પાછા new_fd;
}

પૂર્ણાંક fd_is_opath(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = fcntl(fd, F_GETFL);
        જો (r < 0)
                પાછા -errno;

        પાછા FLAGS_SET(r, O_PATH);
}

પૂર્ણાંક fd_vet_accmode(પૂર્ણાંક fd, પૂર્ણાંક mode) {
        પૂર્ણાંક flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        જો (flags < 0)
                પાછા -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        જો (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                પાછા -EISDIR;

        જો (FLAGS_SET(flags, O_PATH))
                પાછા -EBADFD;

        flags &= O_ACCMODE_STRICT;

        જો (flags == mode)
                પાછા 1;

        જો (flags == O_RDWR)
                પાછા 0;

        પાછા -EPROTOTYPE;
}

પૂર્ણાંક fd_is_writable(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        જો (r >= 0)
                પાછા true;

        જો (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                પાછા false;

        પાછા r;
}

પૂર્ણાંક fd_verify_safe_flags_full(પૂર્ણાંક fd, પૂર્ણાંક extra_flags) {
        પૂર્ણાંક flags, unexpected_flags;

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

        પ્રતિજ્ઞા(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        જો (flags < 0)
                પાછા -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        જો (unexpected_flags != 0)
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        પાછા flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(શૂન્ય) {
        _cleanup_free_ અક્ષર *nr_open = ખાલી;
        પૂર્ણાંક r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_લીટી_file("/proc/sys/fs/nr_open", &nr_open);
        જો (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        નહીંતર {
                unsigned v;

                r = safe_atou(nr_open, &v);
                જો (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                નહીંતર
                        પાછા v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        પાછા NR_OPEN_DEFAULT;
}

પૂર્ણાંક fd_get_diskseq(પૂર્ણાંક fd, uint64_t *ret) {
        uint64_t diskseq;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                જો (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        પાછા -errno;

                પાછા -EOPNOTSUPP;
        }

        *ret = diskseq;

        પાછા 0;
}

સ્થિર bool is_literal_મૂળ(અચલ અક્ષર *p) {
        જો (!p)
                પાછા false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        પાછા n >= 1 && p[n] == 0;
}

પૂર્ણાંક path_is_મૂળ_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));

        જો (dir_fd == XAT_FDROOT && isempty(path))
                પાછા true;

        જો (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_મૂળ(path))
                પાછા true;

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        જો (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                જો (fd == -ENOTDIR)
                        પાછા false; /* the root dir must be a dir */
                જો (fd < 0)
                        પાછા fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        પાછા fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

પૂર્ણાંક fds_inode_and_mount_same(પૂર્ણાંક fd1, પૂર્ણાંક fd2) {
        સંરચના statx sx1, sx2;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(fd1));
        પ્રતિજ્ઞા(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ખાલી, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        જો (r < 0)
                પાછા r;

        જો (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                પાછા true;

        r = xstatx(fd2, /* path = */ ખાલી, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        જો (r < 0)
                પાછા r;

        r = statx_mount_same(&sx1, &sx2);
        જો (r <= 0)
                પાછા r;

        પાછા statx_inode_same(&sx1, &sx2);
}

પૂર્ણાંક resolve_xat_fdroot(પૂર્ણાંક *fd, અચલ અક્ષર **path, અક્ષર **ret_buffer) {
        પ્રતિજ્ઞા(fd);
        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ret_buffer);

        જો (*fd != XAT_FDROOT) {
                *ret_buffer = ખાલી;
                પાછા 0;
        }

        જો (isempty(*path)) {
                *path = "/";
                *ret_buffer = ખાલી;
        } નહીંતર જો (!path_is_absolute(*path)) {
                અક્ષર *p = strjoin("/", *path);
                જો (!p)
                        પાછા -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        પાછા 1;
}

અક્ષર* format_proc_fd_path(અક્ષર buf[સ્થિર PROC_FD_PATH_MAX], પૂર્ણાંક fd) {
        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        પાછા buf;
}

અચલ અક્ષર* accmode_to_string(પૂર્ણાંક flags) {
        પસંદગી (flags & O_ACCMODE_STRICT) {
        સ્થિતિ O_RDONLY:
                પાછા "ro";
        સ્થિતિ O_WRONLY:
                પાછા "wo";
        સ્થિતિ O_RDWR:
                પાછા "rw";
        મૂળભૂત:
                પાછા ખાલી;
        }
}

અક્ષર* format_proc_pid_fd_path(અક્ષર buf[સ્થિર PROC_PID_FD_PATH_MAX], pid_t pid, પૂર્ણાંક fd) {
        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        પાછા buf;
}

પૂર્ણાંક proc_fd_enoent_errno(શૂન્ય) {
        પૂર્ણાંક r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        જો (r == 0)
                પાછા -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        જો (r > 0)
                પાછા -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        પાછા -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
