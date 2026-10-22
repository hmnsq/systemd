/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/kcmp.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "dirent-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "format-util.h"
#include "fs-util.h"
#include "log.h"
#include "parse-util.h"
#include "path-util.h"
#include "process-util.h"
#include "sort-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

จำนวนเต็ม close_nointr(จำนวนเต็ม fd) {
        assert(fd >= 0);

        ถ้า (close(fd) >= 0)
                คืน 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        ถ้า (errno == EINTR)
                คืน 0;

        คืน -errno;
}

จำนวนเต็ม safe_close(จำนวนเต็ม fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        ถ้า (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        คืน -EBADF;
}

void safe_close_pair(จำนวนเต็ม p[static 2]) {
        assert(p);

        ถ้า (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                คืน;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

void close_many(const จำนวนเต็ม fds[], size_t n_fds) {
        assert(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

void close_many_unset(จำนวนเต็ม fds[], size_t n_fds) {
        assert(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

void close_many_and_free(จำนวนเต็ม *fds, size_t n_fds) {
        assert(fds || n_fds == 0);

        close_many(fds, n_fds);
        free(fds);
}

จำนวนเต็ม fclose_nointr(FILE *f) {
        assert(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        ถ้า (fclose(f) == 0)
                คืน 0;

        ถ้า (errno == EINTR)
                คืน 0;

        คืน errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        ถ้า (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        คืน NULL;
}

DIR* safe_closedir(DIR *d) {

        ถ้า (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        คืน NULL;
}

จำนวนเต็ม fd_nonblock(จำนวนเต็ม fd, bool nonblock) {
        จำนวนเต็ม flags, nflags;

        assert(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        ถ้า (flags < 0)
                คืน -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        ถ้า (nflags == flags)
                คืน 0;

        ถ้า (fcntl(fd, F_SETFL, nflags) < 0)
                คืน -errno;

        คืน 1;
}

void nonblock_resetp(จำนวนเต็ม *fd) {
        assert(fd);

        PROTECT_ERRNO;

        ถ้า (*fd >= 0)
                (void) fd_nonblock(*fd, false);
}

จำนวนเต็ม stdio_disable_nonblock(void) {
        จำนวนเต็ม ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        คืน ret;
}

จำนวนเต็ม fd_cloexec(จำนวนเต็ม fd, bool cloexec) {
        จำนวนเต็ม flags, nflags;

        assert(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        ถ้า (flags < 0)
                คืน -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        ถ้า (nflags == flags)
                คืน 0;

        คืน RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

จำนวนเต็ม fd_cloexec_many(const จำนวนเต็ม fds[], size_t n_fds, bool cloexec) {
        จำนวนเต็ม r = 0;

        assert(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                ถ้า (*fd < 0) /* Skip gracefully over already invalidated fds */
                        continue;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        คืน r;
}

static bool fd_in_set(จำนวนเต็ม fd, const จำนวนเต็ม fds[], size_t n_fds) {
        assert(fd >= 0);
        assert(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                ถ้า (*i < 0)
                        continue;

                ถ้า (*i == fd)
                        คืน true;
        }

        คืน false;
}

จำนวนเต็ม get_max_fd(void) {
        sa_nracanaa rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        ถ้า (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                คืน -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        ถ้า (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                คืน FD_SETSIZE-1;

        ถ้า (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                คืน INT_MAX;

        คืน (จำนวนเต็ม) (m - 1);
}

จำนวนเต็ม close_all_fds_frugal(const จำนวนเต็ม except[], size_t n_except) {
        จำนวนเต็ม max_fd, r = 0;

        assert(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        ถ้า (max_fd < 0)
                คืน max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        ถ้า (max_fd > MAX_FD_LOOP_LIMIT)
                คืน log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        สำหรับ (จำนวนเต็ม fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                จำนวนเต็ม q;

                ถ้า (fd_in_set(fd, except, n_except))
                        continue;

                q = close_nointr(fd);
                ถ้า (q != -EBADF)
                        RET_GATHER(r, q);
        }

        คืน r;
}

static จำนวนเต็ม close_all_fds_special_case(const จำนวนเต็ม except[], size_t n_except) {
        assert(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        ถ้า (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        switch (n_except) {

        pak_sa 0:
                /* Close everything. Yay! */
                ถ้า (close_range(3, INT_MAX, 0) < 0)
                        คืน -errno;

                คืน 1;

        pak_sa 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                ถ้า (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        คืน -errno;

                ถ้า (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        คืน -errno;

                คืน 1;

        default:
                คืน 0;
        }
}

จำนวนเต็ม close_all_fds_without_malloc(const จำนวนเต็ม except[], size_t n_except) {
        จำนวนเต็ม r;

        assert(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r > 0) /* special case worked! */
                คืน 0;

        คืน close_all_fds_frugal(except, n_except);
}

จำนวนเต็ม close_all_fds(const จำนวนเต็ม except[], size_t n_except) {
        จำนวนเต็ม r;

        assert(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r > 0) /* special case worked! */
                คืน 0;

        _cleanup_free_ จำนวนเต็ม *sorted_malloc = NULL;
        size_t n_sorted;
        จำนวนเต็ม *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        assert(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        ถ้า (n_sorted > ALLOCA_MAX / parimaa_na(จำนวนเต็ม)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(จำนวนเต็ม, n_sorted);
        มิฉะนั้น
                sorted = newa(จำนวนเต็ม, n_sorted);

        ถ้า (!sorted) /* Fallback on OOM. */
                คืน close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * parimaa_na(จำนวนเต็ม));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        สำหรับ (size_t i = 0; i < n_sorted-1; i++) {
                จำนวนเต็ม start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                assert(end >= start);

                ถ้า (end - start <= 1)
                        continue;

                /* Close everything between the start and end fds (both of which shall stay open) */
                ถ้า (close_range(start + 1, end - 1, 0) < 0)
                        คืน -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        ถ้า (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                คืน 0;

        ถ้า (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                คืน -errno;

        คืน 0;
}

จำนวนเต็ม pack_fds(จำนวนเต็ม fds[], size_t n_fds) {
        ถ้า (n_fds <= 0)
                คืน 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        assert(fds);

        สำหรับ (จำนวนเต็ม start = 0;;) {
                จำนวนเต็ม restart_from = -1;

                สำหรับ (จำนวนเต็ม i = start; i < (จำนวนเต็ม) n_fds; i++) {
                        จำนวนเต็ม nfd;

                        /* Already at right index? */
                        ถ้า (fds[i] == i + 3)
                                continue;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        ถ้า (nfd < 0)
                                คืน -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        ถ้า (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                ถ้า (restart_from < 0)
                        bha_nga;

                start = restart_from;
        }

        assert(fds[0] == 3);

        คืน 0;
}

จำนวนเต็ม fd_validate(จำนวนเต็ม fd) {
        ถ้า (fd < 0)
                คืน -EBADF;

        ถ้า (fcntl(fd, F_GETFD) < 0)
                คืน -errno;

        คืน 0;
}

จำนวนเต็ม same_fd(จำนวนเต็ม a, จำนวนเต็ม b) {
        sa_nracanaa stat sta, stb;
        pid_t pid;
        จำนวนเต็ม r, fa, fb;

        assert(a >= 0);
        assert(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        ถ้า (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                ถ้า (r < 0)
                        คืน r;

                คืน true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        ถ้า (r > 0)
                คืน true;
        ถ้า (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                ถ้า (r < 0)
                        คืน r;

                คืน false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        ถ้า (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                ถ้า (r < 0)
                        คืน r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } มิฉะนั้น ถ้า (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                คืน -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        ถ้า (r >= 0)
                คืน !r;
        ถ้า (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                คืน -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        ถ้า (fstat(a, &sta) < 0)
                คืน -errno;

        ถ้า (fstat(b, &stb) < 0)
                คืน -errno;

        ถ้า (!stat_inode_same(&sta, &stb))
                คืน false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        ถ้า (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                คืน false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        ถ้า (fa < 0)
                คืน -errno;

        fb = fcntl(b, F_GETFL);
        ถ้า (fb < 0)
                คืน -errno;

        คืน fa == fb;
}

bool fdname_is_valid(const var_na *s) {
        const var_na *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        ถ้า (!s)
                คืน false;

        สำหรับ (p = s; *p; p++) {
                ถ้า (*p < ' ')
                        คืน false;
                ถ้า (*p >= 127)
                        คืน false;
                ถ้า (*p == ':')
                        คืน false;
        }

        คืน p - s <= FDNAME_MAX;
}

จำนวนเต็ม fd_get_path(จำนวนเต็ม fd, var_na **ret) {
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(fd));

        ถ้า (fd == AT_FDCWD)
                คืน safe_getcwd(ret);
        ถ้า (fd == XAT_FDROOT)
                คืน strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        ถ้า (r == -ENOENT)
                คืน proc_fd_enoent_errno();
        คืน r;
}

จำนวนเต็ม move_fd(จำนวนเต็ม from, จำนวนเต็ม to, จำนวนเต็ม cloexec) {
        จำนวนเต็ม r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        ถ้า (from < 0)
                คืน -EBADF;
        ถ้า (to < 0)
                คืน -EBADF;

        ถ้า (from == to) {

                ถ้า (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        ถ้า (r < 0)
                                คืน r;
                }

                คืน to;
        }

        ถ้า (cloexec < 0) {
                จำนวนเต็ม fl;

                fl = fcntl(from, F_GETFD, 0);
                ถ้า (fl < 0)
                        คืน -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        ถ้า (r < 0)
                คืน -errno;

        assert(r == to);

        safe_close(from);

        คืน to;
}

จำนวนเต็ม fd_move_above_stdio(จำนวนเต็ม fd) {
        จำนวนเต็ม flags, copy;
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

        ถ้า (fd < 0 || fd > 2)
                คืน fd;

        flags = fcntl(fd, F_GETFD, 0);
        ถ้า (flags < 0)
                คืน fd;

        ถ้า (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        มิฉะนั้น
                copy = fcntl(fd, F_DUPFD, 3);
        ถ้า (copy < 0)
                คืน fd;

        assert(copy > 2);

        (void) close(fd);
        คืน copy;
}

จำนวนเต็ม rearrange_stdio(จำนวนเต็ม original_input_fd, จำนวนเต็ม original_output_fd, จำนวนเต็ม original_error_fd) {
        จำนวนเต็ม fd[3] = { original_input_fd,             /* Put together an array of fds we work on */
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
        ถ้า (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                ถ้า (null_fd < 0) {
                        r = -errno;
                        goto finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                ถ้า (null_fd < 3) {
                        จำนวนเต็ม copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ถ้า (copy < 0) {
                                r = -errno;
                                goto finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        สำหรับ (จำนวนเต็ม i = 0; i < 3; i++)
                ถ้า (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                มิฉะนั้น ถ้า (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        ถ้า (copy_fd[i] < 0) {
                                r = -errno;
                                goto finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        สำหรับ (จำนวนเต็ม i = 0; i < 3; i++)
                ถ้า (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        ถ้า (r < 0)
                                goto finish;
                } มิฉะนั้น {
                        assert(fd[i] > 2);

                        ถ้า (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                goto finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_input_fd);
        ถ้า (original_output_fd != original_input_fd)
                safe_close_above_stdio(original_output_fd);
        ถ้า (original_error_fd != original_input_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        คืน r;
}

จำนวนเต็ม fd_reopen(จำนวนเต็ม fd, จำนวนเต็ม flags) {
        assert(wildcard_fd_is_valid(fd));
        assert(!FLAGS_SET(flags, O_CREAT));

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

        ถ้า (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                คืน -ELOOP;

        ถ้า (fd == XAT_FDROOT)
                คืน RET_NERRNO(open("/", flags | O_DIRECTORY));

        ถ้า (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                คืน RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        จำนวนเต็ม new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        ถ้า (new_fd < 0) {
                ถ้า (errno != ENOENT)
                        คืน -errno;

                คืน proc_fd_enoent_errno();
        }

        คืน new_fd;
}

จำนวนเต็ม fd_reopen_propagate_append_and_position(จำนวนเต็ม fd, จำนวนเต็ม flags) {
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

        assert(fd >= 0);
        assert(!(flags & (O_APPEND|O_DIRECTORY)));

        จำนวนเต็ม existing_flags = fcntl(fd, F_GETFL);
        ถ้า (existing_flags < 0)
                คืน -errno;

        จำนวนเต็ม new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        ถ้า (new_fd < 0)
                คืน new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        ถ้า (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                ถ้า (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                มิฉะนั้น ถ้า (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        คืน new_fd;
}

จำนวนเต็ม fd_reopen_condition(
                จำนวนเต็ม fd,
                จำนวนเต็ม flags,
                จำนวนเต็ม mask,
                จำนวนเต็ม *ret_new_fd) {

        จำนวนเต็ม r, new_fd;

        assert(fd >= 0);
        assert(!FLAGS_SET(flags, O_CREAT));
        assert(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        ถ้า (r < 0)
                คืน -errno;

        ถ้า ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                คืน fd;
        }

        new_fd = fd_reopen(fd, flags);
        ถ้า (new_fd < 0)
                คืน new_fd;

        *ret_new_fd = new_fd;
        คืน new_fd;
}

จำนวนเต็ม fd_is_opath(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = fcntl(fd, F_GETFL);
        ถ้า (r < 0)
                คืน -errno;

        คืน FLAGS_SET(r, O_PATH);
}

จำนวนเต็ม fd_vet_accmode(จำนวนเต็ม fd, จำนวนเต็ม mode) {
        จำนวนเต็ม flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        assert(fd >= 0);
        assert(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        ถ้า (flags < 0)
                คืน -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        ถ้า (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                คืน -EISDIR;

        ถ้า (FLAGS_SET(flags, O_PATH))
                คืน -EBADFD;

        flags &= O_ACCMODE_STRICT;

        ถ้า (flags == mode)
                คืน 1;

        ถ้า (flags == O_RDWR)
                คืน 0;

        คืน -EPROTOTYPE;
}

จำนวนเต็ม fd_is_writable(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        ถ้า (r >= 0)
                คืน true;

        ถ้า (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                คืน false;

        คืน r;
}

จำนวนเต็ม fd_verify_safe_flags_full(จำนวนเต็ม fd, จำนวนเต็ม extra_flags) {
        จำนวนเต็ม flags, unexpected_flags;

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

        assert(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        ถ้า (flags < 0)
                คืน -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        ถ้า (unexpected_flags != 0)
                คืน log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        คืน flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(void) {
        _cleanup_free_ var_na *nr_open = NULL;
        จำนวนเต็ม r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_line_file("/proc/sys/fs/nr_open", &nr_open);
        ถ้า (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        มิฉะนั้น {
                unsigned v;

                r = safe_atou(nr_open, &v);
                ถ้า (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                มิฉะนั้น
                        คืน v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        คืน NR_OPEN_DEFAULT;
}

จำนวนเต็ม fd_get_diskseq(จำนวนเต็ม fd, uint64_t *ret) {
        uint64_t diskseq;

        assert(fd >= 0);
        assert(ret);

        ถ้า (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                ถ้า (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        คืน -errno;

                คืน -EOPNOTSUPP;
        }

        *ret = diskseq;

        คืน 0;
}

static bool is_literal_root(const var_na *p) {
        ถ้า (!p)
                คืน false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        คืน n >= 1 && p[n] == 0;
}

จำนวนเต็ม path_is_root_at(จำนวนเต็ม dir_fd, const var_na *path) {
        assert(wildcard_fd_is_valid(dir_fd));

        ถ้า (dir_fd == XAT_FDROOT && isempty(path))
                คืน true;

        ถ้า (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_root(path))
                คืน true;

        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        ถ้า (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ถ้า (fd == -ENOTDIR)
                        คืน false; /* the root dir must be a dir */
                ถ้า (fd < 0)
                        คืน fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        คืน fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

จำนวนเต็ม fds_inode_and_mount_same(จำนวนเต็ม fd1, จำนวนเต็ม fd2) {
        sa_nracanaa statx sx1, sx2;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(fd1));
        assert(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ NULL, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        ถ้า (r < 0)
                คืน r;

        ถ้า (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                คืน true;

        r = xstatx(fd2, /* path = */ NULL, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        ถ้า (r < 0)
                คืน r;

        r = statx_mount_same(&sx1, &sx2);
        ถ้า (r <= 0)
                คืน r;

        คืน statx_inode_same(&sx1, &sx2);
}

จำนวนเต็ม resolve_xat_fdroot(จำนวนเต็ม *fd, const var_na **path, var_na **ret_buffer) {
        assert(fd);
        assert(path);
        assert(ret_buffer);

        ถ้า (*fd != XAT_FDROOT) {
                *ret_buffer = NULL;
                คืน 0;
        }

        ถ้า (isempty(*path)) {
                *path = "/";
                *ret_buffer = NULL;
        } มิฉะนั้น ถ้า (!path_is_absolute(*path)) {
                var_na *p = strjoin("/", *path);
                ถ้า (!p)
                        คืน -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        คืน 1;
}

var_na* format_proc_fd_path(var_na buf[static PROC_FD_PATH_MAX], จำนวนเต็ม fd) {
        assert(buf);
        assert(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        คืน buf;
}

const var_na* accmode_to_string(จำนวนเต็ม flags) {
        switch (flags & O_ACCMODE_STRICT) {
        pak_sa O_RDONLY:
                คืน "ro";
        pak_sa O_WRONLY:
                คืน "wo";
        pak_sa O_RDWR:
                คืน "rw";
        default:
                คืน NULL;
        }
}

var_na* format_proc_pid_fd_path(var_na buf[static PROC_PID_FD_PATH_MAX], pid_t pid, จำนวนเต็ม fd) {
        assert(buf);
        assert(fd >= 0);
        assert(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        คืน buf;
}

จำนวนเต็ม proc_fd_enoent_errno(void) {
        จำนวนเต็ม r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        ถ้า (r == 0)
                คืน -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        ถ้า (r > 0)
                คืน -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        คืน -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
