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

số_nguyên close_nointr(số_nguyên fd) {
        assert(fd >= 0);

        nếu (close(fd) >= 0)
                trả_về 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        nếu (errno == EINTR)
                trả_về 0;

        trả_về -errno;
}

số_nguyên safe_close(số_nguyên fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        nếu (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        trả_về -EBADF;
}

void safe_close_pair(số_nguyên p[static 2]) {
        assert(p);

        nếu (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                trả_về;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

void close_many(const số_nguyên fds[], size_t n_fds) {
        assert(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

void close_many_unset(số_nguyên fds[], size_t n_fds) {
        assert(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

void close_many_and_free(số_nguyên *fds, size_t n_fds) {
        assert(fds || n_fds == 0);

        close_many(fds, n_fds);
        free(fds);
}

số_nguyên fclose_nointr(FILE *f) {
        assert(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        nếu (fclose(f) == 0)
                trả_về 0;

        nếu (errno == EINTR)
                trả_về 0;

        trả_về errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        nếu (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        trả_về NULL;
}

DIR* safe_closedir(DIR *d) {

        nếu (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        trả_về NULL;
}

số_nguyên fd_nonblock(số_nguyên fd, bool nonblock) {
        số_nguyên flags, nflags;

        assert(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        nếu (flags < 0)
                trả_về -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        nếu (nflags == flags)
                trả_về 0;

        nếu (fcntl(fd, F_SETFL, nflags) < 0)
                trả_về -errno;

        trả_về 1;
}

void nonblock_resetp(số_nguyên *fd) {
        assert(fd);

        PROTECT_ERRNO;

        nếu (*fd >= 0)
                (void) fd_nonblock(*fd, false);
}

số_nguyên stdio_disable_nonblock(void) {
        số_nguyên ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        trả_về ret;
}

số_nguyên fd_cloexec(số_nguyên fd, bool cloexec) {
        số_nguyên flags, nflags;

        assert(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        nếu (flags < 0)
                trả_về -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        nếu (nflags == flags)
                trả_về 0;

        trả_về RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

số_nguyên fd_cloexec_many(const số_nguyên fds[], size_t n_fds, bool cloexec) {
        số_nguyên r = 0;

        assert(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                nếu (*fd < 0) /* Skip gracefully over already invalidated fds */
                        continue;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        trả_về r;
}

static bool fd_in_set(số_nguyên fd, const số_nguyên fds[], size_t n_fds) {
        assert(fd >= 0);
        assert(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                nếu (*i < 0)
                        continue;

                nếu (*i == fd)
                        trả_về true;
        }

        trả_về false;
}

số_nguyên get_max_fd(void) {
        struct rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        nếu (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                trả_về -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        nếu (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                trả_về FD_SETSIZE-1;

        nếu (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                trả_về INT_MAX;

        trả_về (số_nguyên) (m - 1);
}

số_nguyên close_all_fds_frugal(const số_nguyên except[], size_t n_except) {
        số_nguyên max_fd, r = 0;

        assert(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        nếu (max_fd < 0)
                trả_về max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        nếu (max_fd > MAX_FD_LOOP_LIMIT)
                trả_về log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        cho (số_nguyên fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                số_nguyên q;

                nếu (fd_in_set(fd, except, n_except))
                        continue;

                q = close_nointr(fd);
                nếu (q != -EBADF)
                        RET_GATHER(r, q);
        }

        trả_về r;
}

static số_nguyên close_all_fds_special_case(const số_nguyên except[], size_t n_except) {
        assert(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        nếu (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        switch (n_except) {

        case 0:
                /* Close everything. Yay! */
                nếu (close_range(3, INT_MAX, 0) < 0)
                        trả_về -errno;

                trả_về 1;

        case 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                nếu (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        trả_về -errno;

                nếu (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        trả_về -errno;

                trả_về 1;

        default:
                trả_về 0;
        }
}

số_nguyên close_all_fds_without_malloc(const số_nguyên except[], size_t n_except) {
        số_nguyên r;

        assert(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        nếu (r < 0)
                trả_về r;
        nếu (r > 0) /* special case worked! */
                trả_về 0;

        trả_về close_all_fds_frugal(except, n_except);
}

số_nguyên close_all_fds(const số_nguyên except[], size_t n_except) {
        số_nguyên r;

        assert(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        nếu (r < 0)
                trả_về r;
        nếu (r > 0) /* special case worked! */
                trả_về 0;

        _cleanup_free_ số_nguyên *sorted_malloc = NULL;
        size_t n_sorted;
        số_nguyên *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        assert(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        nếu (n_sorted > ALLOCA_MAX / sizeof(số_nguyên)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(số_nguyên, n_sorted);
        không_thì
                sorted = newa(số_nguyên, n_sorted);

        nếu (!sorted) /* Fallback on OOM. */
                trả_về close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * sizeof(số_nguyên));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        cho (size_t i = 0; i < n_sorted-1; i++) {
                số_nguyên start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                assert(end >= start);

                nếu (end - start <= 1)
                        continue;

                /* Close everything between the start and end fds (both of which shall stay open) */
                nếu (close_range(start + 1, end - 1, 0) < 0)
                        trả_về -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        nếu (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                trả_về 0;

        nếu (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                trả_về -errno;

        trả_về 0;
}

số_nguyên pack_fds(số_nguyên fds[], size_t n_fds) {
        nếu (n_fds <= 0)
                trả_về 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        assert(fds);

        cho (số_nguyên start = 0;;) {
                số_nguyên restart_from = -1;

                cho (số_nguyên i = start; i < (số_nguyên) n_fds; i++) {
                        số_nguyên nfd;

                        /* Already at right index? */
                        nếu (fds[i] == i + 3)
                                continue;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        nếu (nfd < 0)
                                trả_về -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        nếu (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                nếu (restart_from < 0)
                        break;

                start = restart_from;
        }

        assert(fds[0] == 3);

        trả_về 0;
}

số_nguyên fd_validate(số_nguyên fd) {
        nếu (fd < 0)
                trả_về -EBADF;

        nếu (fcntl(fd, F_GETFD) < 0)
                trả_về -errno;

        trả_về 0;
}

số_nguyên same_fd(số_nguyên a, số_nguyên b) {
        struct stat sta, stb;
        pid_t pid;
        số_nguyên r, fa, fb;

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

        nếu (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                nếu (r < 0)
                        trả_về r;

                trả_về true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        nếu (r > 0)
                trả_về true;
        nếu (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                nếu (r < 0)
                        trả_về r;

                trả_về false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        nếu (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                nếu (r < 0)
                        trả_về r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } không_thì nếu (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                trả_về -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        nếu (r >= 0)
                trả_về !r;
        nếu (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                trả_về -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        nếu (fstat(a, &sta) < 0)
                trả_về -errno;

        nếu (fstat(b, &stb) < 0)
                trả_về -errno;

        nếu (!stat_inode_same(&sta, &stb))
                trả_về false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        nếu (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                trả_về false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        nếu (fa < 0)
                trả_về -errno;

        fb = fcntl(b, F_GETFL);
        nếu (fb < 0)
                trả_về -errno;

        trả_về fa == fb;
}

bool fdname_is_valid(const char *s) {
        const char *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        nếu (!s)
                trả_về false;

        cho (p = s; *p; p++) {
                nếu (*p < ' ')
                        trả_về false;
                nếu (*p >= 127)
                        trả_về false;
                nếu (*p == ':')
                        trả_về false;
        }

        trả_về p - s <= FDNAME_MAX;
}

số_nguyên fd_get_path(số_nguyên fd, char **ret) {
        số_nguyên r;

        assert(wildcard_fd_is_valid(fd));

        nếu (fd == AT_FDCWD)
                trả_về safe_getcwd(ret);
        nếu (fd == XAT_FDROOT)
                trả_về strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        nếu (r == -ENOENT)
                trả_về proc_fd_enoent_errno();
        trả_về r;
}

số_nguyên move_fd(số_nguyên from, số_nguyên to, số_nguyên cloexec) {
        số_nguyên r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        nếu (from < 0)
                trả_về -EBADF;
        nếu (to < 0)
                trả_về -EBADF;

        nếu (from == to) {

                nếu (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        nếu (r < 0)
                                trả_về r;
                }

                trả_về to;
        }

        nếu (cloexec < 0) {
                số_nguyên fl;

                fl = fcntl(from, F_GETFD, 0);
                nếu (fl < 0)
                        trả_về -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        nếu (r < 0)
                trả_về -errno;

        assert(r == to);

        safe_close(from);

        trả_về to;
}

số_nguyên fd_move_above_stdio(số_nguyên fd) {
        số_nguyên flags, copy;
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

        nếu (fd < 0 || fd > 2)
                trả_về fd;

        flags = fcntl(fd, F_GETFD, 0);
        nếu (flags < 0)
                trả_về fd;

        nếu (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        không_thì
                copy = fcntl(fd, F_DUPFD, 3);
        nếu (copy < 0)
                trả_về fd;

        assert(copy > 2);

        (void) close(fd);
        trả_về copy;
}

số_nguyên rearrange_stdio(số_nguyên original_input_fd, số_nguyên original_output_fd, số_nguyên original_error_fd) {
        số_nguyên fd[3] = { original_input_fd,             /* Put together an array of fds we work on */
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
        nếu (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                nếu (null_fd < 0) {
                        r = -errno;
                        goto finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                nếu (null_fd < 3) {
                        số_nguyên copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        nếu (copy < 0) {
                                r = -errno;
                                goto finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        cho (số_nguyên i = 0; i < 3; i++)
                nếu (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                không_thì nếu (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        nếu (copy_fd[i] < 0) {
                                r = -errno;
                                goto finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        cho (số_nguyên i = 0; i < 3; i++)
                nếu (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        nếu (r < 0)
                                goto finish;
                } không_thì {
                        assert(fd[i] > 2);

                        nếu (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                goto finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_input_fd);
        nếu (original_output_fd != original_input_fd)
                safe_close_above_stdio(original_output_fd);
        nếu (original_error_fd != original_input_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        trả_về r;
}

số_nguyên fd_reopen(số_nguyên fd, số_nguyên flags) {
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

        nếu (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                trả_về -ELOOP;

        nếu (fd == XAT_FDROOT)
                trả_về RET_NERRNO(open("/", flags | O_DIRECTORY));

        nếu (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                trả_về RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        số_nguyên new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        nếu (new_fd < 0) {
                nếu (errno != ENOENT)
                        trả_về -errno;

                trả_về proc_fd_enoent_errno();
        }

        trả_về new_fd;
}

số_nguyên fd_reopen_propagate_append_and_position(số_nguyên fd, số_nguyên flags) {
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

        số_nguyên existing_flags = fcntl(fd, F_GETFL);
        nếu (existing_flags < 0)
                trả_về -errno;

        số_nguyên new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        nếu (new_fd < 0)
                trả_về new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        nếu (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                nếu (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                không_thì nếu (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        trả_về new_fd;
}

số_nguyên fd_reopen_condition(
                số_nguyên fd,
                số_nguyên flags,
                số_nguyên mask,
                số_nguyên *ret_new_fd) {

        số_nguyên r, new_fd;

        assert(fd >= 0);
        assert(!FLAGS_SET(flags, O_CREAT));
        assert(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        nếu (r < 0)
                trả_về -errno;

        nếu ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                trả_về fd;
        }

        new_fd = fd_reopen(fd, flags);
        nếu (new_fd < 0)
                trả_về new_fd;

        *ret_new_fd = new_fd;
        trả_về new_fd;
}

số_nguyên fd_is_opath(số_nguyên fd) {
        số_nguyên r;

        assert(fd >= 0);

        r = fcntl(fd, F_GETFL);
        nếu (r < 0)
                trả_về -errno;

        trả_về FLAGS_SET(r, O_PATH);
}

số_nguyên fd_vet_accmode(số_nguyên fd, số_nguyên mode) {
        số_nguyên flags;

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
        nếu (flags < 0)
                trả_về -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        nếu (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                trả_về -EISDIR;

        nếu (FLAGS_SET(flags, O_PATH))
                trả_về -EBADFD;

        flags &= O_ACCMODE_STRICT;

        nếu (flags == mode)
                trả_về 1;

        nếu (flags == O_RDWR)
                trả_về 0;

        trả_về -EPROTOTYPE;
}

số_nguyên fd_is_writable(số_nguyên fd) {
        số_nguyên r;

        assert(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        nếu (r >= 0)
                trả_về true;

        nếu (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                trả_về false;

        trả_về r;
}

số_nguyên fd_verify_safe_flags_full(số_nguyên fd, số_nguyên extra_flags) {
        số_nguyên flags, unexpected_flags;

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
        nếu (flags < 0)
                trả_về -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        nếu (unexpected_flags != 0)
                trả_về log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        trả_về flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(void) {
        _cleanup_free_ char *nr_open = NULL;
        số_nguyên r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_line_file("/proc/sys/fs/nr_open", &nr_open);
        nếu (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        không_thì {
                unsigned v;

                r = safe_atou(nr_open, &v);
                nếu (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                không_thì
                        trả_về v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        trả_về NR_OPEN_DEFAULT;
}

số_nguyên fd_get_diskseq(số_nguyên fd, uint64_t *ret) {
        uint64_t diskseq;

        assert(fd >= 0);
        assert(ret);

        nếu (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                nếu (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        trả_về -errno;

                trả_về -EOPNOTSUPP;
        }

        *ret = diskseq;

        trả_về 0;
}

static bool is_literal_root(const char *p) {
        nếu (!p)
                trả_về false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        trả_về n >= 1 && p[n] == 0;
}

số_nguyên path_is_root_at(số_nguyên dir_fd, const char *path) {
        assert(wildcard_fd_is_valid(dir_fd));

        nếu (dir_fd == XAT_FDROOT && isempty(path))
                trả_về true;

        nếu (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_root(path))
                trả_về true;

        _cleanup_close_ số_nguyên fd = -EBADF;
        nếu (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                nếu (fd == -ENOTDIR)
                        trả_về false; /* the root dir must be a dir */
                nếu (fd < 0)
                        trả_về fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        trả_về fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

số_nguyên fds_inode_and_mount_same(số_nguyên fd1, số_nguyên fd2) {
        struct statx sx1, sx2;
        số_nguyên r;

        assert(wildcard_fd_is_valid(fd1));
        assert(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ NULL, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        nếu (r < 0)
                trả_về r;

        nếu (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                trả_về true;

        r = xstatx(fd2, /* path = */ NULL, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        nếu (r < 0)
                trả_về r;

        r = statx_mount_same(&sx1, &sx2);
        nếu (r <= 0)
                trả_về r;

        trả_về statx_inode_same(&sx1, &sx2);
}

số_nguyên resolve_xat_fdroot(số_nguyên *fd, const char **path, char **ret_buffer) {
        assert(fd);
        assert(path);
        assert(ret_buffer);

        nếu (*fd != XAT_FDROOT) {
                *ret_buffer = NULL;
                trả_về 0;
        }

        nếu (isempty(*path)) {
                *path = "/";
                *ret_buffer = NULL;
        } không_thì nếu (!path_is_absolute(*path)) {
                char *p = strjoin("/", *path);
                nếu (!p)
                        trả_về -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        trả_về 1;
}

char* format_proc_fd_path(char buf[static PROC_FD_PATH_MAX], số_nguyên fd) {
        assert(buf);
        assert(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        trả_về buf;
}

const char* accmode_to_string(số_nguyên flags) {
        switch (flags & O_ACCMODE_STRICT) {
        case O_RDONLY:
                trả_về "ro";
        case O_WRONLY:
                trả_về "wo";
        case O_RDWR:
                trả_về "rw";
        default:
                trả_về NULL;
        }
}

char* format_proc_pid_fd_path(char buf[static PROC_PID_FD_PATH_MAX], pid_t pid, số_nguyên fd) {
        assert(buf);
        assert(fd >= 0);
        assert(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        trả_về buf;
}

số_nguyên proc_fd_enoent_errno(void) {
        số_nguyên r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        nếu (r == 0)
                trả_về -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        nếu (r > 0)
                trả_về -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        trả_về -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
