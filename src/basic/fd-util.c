/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <linux/fs.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/kcmp.h>
#अन्तर्भाव <sys/resource.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "sort-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

पूर्णाङ्क close_nointr(पूर्णाङ्क fd) {
        प्रतिज्ञा(fd >= 0);

        यदि (close(fd) >= 0)
                प्रत्यागम 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        यदि (errno == EINTR)
                प्रत्यागम 0;

        प्रत्यागम -errno;
}

पूर्णाङ्क safe_close(पूर्णाङ्क fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        यदि (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        प्रत्यागम -EBADF;
}

शून्य safe_close_pair(पूर्णाङ्क p[स्थिर 2]) {
        प्रतिज्ञा(p);

        यदि (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                प्रत्यागम;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

शून्य close_many(ध्रुव पूर्णाङ्क fds[], size_t n_fds) {
        प्रतिज्ञा(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

शून्य close_many_unset(पूर्णाङ्क fds[], size_t n_fds) {
        प्रतिज्ञा(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

शून्य close_many_and_free(पूर्णाङ्क *fds, size_t n_fds) {
        प्रतिज्ञा(fds || n_fds == 0);

        close_many(fds, n_fds);
        मुक्त(fds);
}

पूर्णाङ्क fclose_nointr(FILE *f) {
        प्रतिज्ञा(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        यदि (fclose(f) == 0)
                प्रत्यागम 0;

        यदि (errno == EINTR)
                प्रत्यागम 0;

        प्रत्यागम errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        यदि (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        प्रत्यागम रिक्त;
}

DIR* safe_closedir(DIR *d) {

        यदि (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        प्रत्यागम रिक्त;
}

पूर्णाङ्क fd_nonblock(पूर्णाङ्क fd, bool nonblock) {
        पूर्णाङ्क flags, nflags;

        प्रतिज्ञा(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        यदि (flags < 0)
                प्रत्यागम -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        यदि (nflags == flags)
                प्रत्यागम 0;

        यदि (fcntl(fd, F_SETFL, nflags) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 1;
}

शून्य nonblock_resetp(पूर्णाङ्क *fd) {
        प्रतिज्ञा(fd);

        PROTECT_ERRNO;

        यदि (*fd >= 0)
                (शून्य) fd_nonblock(*fd, false);
}

पूर्णाङ्क stdio_disable_nonblock(शून्य) {
        पूर्णाङ्क ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        प्रत्यागम ret;
}

पूर्णाङ्क fd_cloexec(पूर्णाङ्क fd, bool cloexec) {
        पूर्णाङ्क flags, nflags;

        प्रतिज्ञा(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        यदि (flags < 0)
                प्रत्यागम -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        यदि (nflags == flags)
                प्रत्यागम 0;

        प्रत्यागम RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

पूर्णाङ्क fd_cloexec_many(ध्रुव पूर्णाङ्क fds[], size_t n_fds, bool cloexec) {
        पूर्णाङ्क r = 0;

        प्रतिज्ञा(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                यदि (*fd < 0) /* Skip gracefully over already invalidated fds */
                        अनुवर्त;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        प्रत्यागम r;
}

स्थिर bool fd_in_set(पूर्णाङ्क fd, ध्रुव पूर्णाङ्क fds[], size_t n_fds) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                यदि (*i < 0)
                        अनुवर्त;

                यदि (*i == fd)
                        प्रत्यागम true;
        }

        प्रत्यागम false;
}

पूर्णाङ्क get_max_fd(शून्य) {
        संरचना rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        यदि (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                प्रत्यागम -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        यदि (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                प्रत्यागम FD_SETSIZE-1;

        यदि (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                प्रत्यागम INT_MAX;

        प्रत्यागम (पूर्णाङ्क) (m - 1);
}

पूर्णाङ्क close_all_fds_frugal(ध्रुव पूर्णाङ्क except[], size_t n_except) {
        पूर्णाङ्क max_fd, r = 0;

        प्रतिज्ञा(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        यदि (max_fd < 0)
                प्रत्यागम max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        यदि (max_fd > MAX_FD_LOOP_LIMIT)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        प्रति (पूर्णाङ्क fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                पूर्णाङ्क q;

                यदि (fd_in_set(fd, except, n_except))
                        अनुवर्त;

                q = close_nointr(fd);
                यदि (q != -EBADF)
                        RET_GATHER(r, q);
        }

        प्रत्यागम r;
}

स्थिर पूर्णाङ्क close_all_fds_special_case(ध्रुव पूर्णाङ्क except[], size_t n_except) {
        प्रतिज्ञा(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        यदि (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        विकल्प (n_except) {

        पक्ष 0:
                /* Close everything. Yay! */
                यदि (close_range(3, INT_MAX, 0) < 0)
                        प्रत्यागम -errno;

                प्रत्यागम 1;

        पक्ष 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                यदि (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        प्रत्यागम -errno;

                यदि (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        प्रत्यागम -errno;

                प्रत्यागम 1;

        स्वभाव:
                प्रत्यागम 0;
        }
}

पूर्णाङ्क close_all_fds_without_malloc(ध्रुव पूर्णाङ्क except[], size_t n_except) {
        पूर्णाङ्क r;

        प्रतिज्ञा(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r > 0) /* special case worked! */
                प्रत्यागम 0;

        प्रत्यागम close_all_fds_frugal(except, n_except);
}

पूर्णाङ्क close_all_fds(ध्रुव पूर्णाङ्क except[], size_t n_except) {
        पूर्णाङ्क r;

        प्रतिज्ञा(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r > 0) /* special case worked! */
                प्रत्यागम 0;

        _cleanup_free_ पूर्णाङ्क *sorted_malloc = रिक्त;
        size_t n_sorted;
        पूर्णाङ्क *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        प्रतिज्ञा(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        यदि (n_sorted > ALLOCA_MAX / परिमाण(पूर्णाङ्क)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(पूर्णाङ्क, n_sorted);
        अन्यथा
                sorted = newa(पूर्णाङ्क, n_sorted);

        यदि (!sorted) /* Fallback on OOM. */
                प्रत्यागम close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * परिमाण(पूर्णाङ्क));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        प्रति (size_t i = 0; i < n_sorted-1; i++) {
                पूर्णाङ्क start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                प्रतिज्ञा(end >= start);

                यदि (end - start <= 1)
                        अनुवर्त;

                /* Close everything between the start and end fds (both of which shall stay open) */
                यदि (close_range(start + 1, end - 1, 0) < 0)
                        प्रत्यागम -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        यदि (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                प्रत्यागम 0;

        यदि (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 0;
}

पूर्णाङ्क pack_fds(पूर्णाङ्क fds[], size_t n_fds) {
        यदि (n_fds <= 0)
                प्रत्यागम 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        प्रतिज्ञा(fds);

        प्रति (पूर्णाङ्क start = 0;;) {
                पूर्णाङ्क restart_from = -1;

                प्रति (पूर्णाङ्क i = start; i < (पूर्णाङ्क) n_fds; i++) {
                        पूर्णाङ्क nfd;

                        /* Already at right index? */
                        यदि (fds[i] == i + 3)
                                अनुवर्त;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        यदि (nfd < 0)
                                प्रत्यागम -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        यदि (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                यदि (restart_from < 0)
                        भङ्ग;

                start = restart_from;
        }

        प्रतिज्ञा(fds[0] == 3);

        प्रत्यागम 0;
}

पूर्णाङ्क fd_validate(पूर्णाङ्क fd) {
        यदि (fd < 0)
                प्रत्यागम -EBADF;

        यदि (fcntl(fd, F_GETFD) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 0;
}

पूर्णाङ्क same_fd(पूर्णाङ्क a, पूर्णाङ्क b) {
        संरचना stat sta, stb;
        pid_t pid;
        पूर्णाङ्क r, fa, fb;

        प्रतिज्ञा(a >= 0);
        प्रतिज्ञा(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        यदि (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रत्यागम true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        यदि (r > 0)
                प्रत्यागम true;
        यदि (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रत्यागम false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        यदि (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                यदि (r < 0)
                        प्रत्यागम r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } अन्यथा यदि (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                प्रत्यागम -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        यदि (r >= 0)
                प्रत्यागम !r;
        यदि (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                प्रत्यागम -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        यदि (fstat(a, &sta) < 0)
                प्रत्यागम -errno;

        यदि (fstat(b, &stb) < 0)
                प्रत्यागम -errno;

        यदि (!stat_inode_same(&sta, &stb))
                प्रत्यागम false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        यदि (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                प्रत्यागम false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        यदि (fa < 0)
                प्रत्यागम -errno;

        fb = fcntl(b, F_GETFL);
        यदि (fb < 0)
                प्रत्यागम -errno;

        प्रत्यागम fa == fb;
}

bool fdname_is_valid(ध्रुव वर्ण *s) {
        ध्रुव वर्ण *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        यदि (!s)
                प्रत्यागम false;

        प्रति (p = s; *p; p++) {
                यदि (*p < ' ')
                        प्रत्यागम false;
                यदि (*p >= 127)
                        प्रत्यागम false;
                यदि (*p == ':')
                        प्रत्यागम false;
        }

        प्रत्यागम p - s <= FDNAME_MAX;
}

पूर्णाङ्क fd_get_path(पूर्णाङ्क fd, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(fd));

        यदि (fd == AT_FDCWD)
                प्रत्यागम safe_getcwd(ret);
        यदि (fd == XAT_FDROOT)
                प्रत्यागम strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        यदि (r == -ENOENT)
                प्रत्यागम proc_fd_enoent_errno();
        प्रत्यागम r;
}

पूर्णाङ्क move_fd(पूर्णाङ्क from, पूर्णाङ्क to, पूर्णाङ्क cloexec) {
        पूर्णाङ्क r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        यदि (from < 0)
                प्रत्यागम -EBADF;
        यदि (to < 0)
                प्रत्यागम -EBADF;

        यदि (from == to) {

                यदि (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                प्रत्यागम to;
        }

        यदि (cloexec < 0) {
                पूर्णाङ्क fl;

                fl = fcntl(from, F_GETFD, 0);
                यदि (fl < 0)
                        प्रत्यागम -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        यदि (r < 0)
                प्रत्यागम -errno;

        प्रतिज्ञा(r == to);

        safe_close(from);

        प्रत्यागम to;
}

पूर्णाङ्क fd_move_above_stdio(पूर्णाङ्क fd) {
        पूर्णाङ्क flags, copy;
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

        यदि (fd < 0 || fd > 2)
                प्रत्यागम fd;

        flags = fcntl(fd, F_GETFD, 0);
        यदि (flags < 0)
                प्रत्यागम fd;

        यदि (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        अन्यथा
                copy = fcntl(fd, F_DUPFD, 3);
        यदि (copy < 0)
                प्रत्यागम fd;

        प्रतिज्ञा(copy > 2);

        (शून्य) close(fd);
        प्रत्यागम copy;
}

पूर्णाङ्क rearrange_stdio(पूर्णाङ्क original_निवेश_fd, पूर्णाङ्क original_output_fd, पूर्णाङ्क original_error_fd) {
        पूर्णाङ्क fd[3] = { original_निवेश_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_निवेश_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        यदि (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                यदि (null_fd < 0) {
                        r = -errno;
                        गच्छ finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                यदि (null_fd < 3) {
                        पूर्णाङ्क copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        यदि (copy < 0) {
                                r = -errno;
                                गच्छ finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        प्रति (पूर्णाङ्क i = 0; i < 3; i++)
                यदि (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                अन्यथा यदि (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        यदि (copy_fd[i] < 0) {
                                r = -errno;
                                गच्छ finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        प्रति (पूर्णाङ्क i = 0; i < 3; i++)
                यदि (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        यदि (r < 0)
                                गच्छ finish;
                } अन्यथा {
                        प्रतिज्ञा(fd[i] > 2);

                        यदि (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                गच्छ finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_निवेश_fd);
        यदि (original_output_fd != original_निवेश_fd)
                safe_close_above_stdio(original_output_fd);
        यदि (original_error_fd != original_निवेश_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        प्रत्यागम r;
}

पूर्णाङ्क fd_reopen(पूर्णाङ्क fd, पूर्णाङ्क flags) {
        प्रतिज्ञा(wildcard_fd_is_valid(fd));
        प्रतिज्ञा(!FLAGS_SET(flags, O_CREAT));

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

        यदि (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                प्रत्यागम -ELOOP;

        यदि (fd == XAT_FDROOT)
                प्रत्यागम RET_NERRNO(open("/", flags | O_DIRECTORY));

        यदि (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                प्रत्यागम RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        पूर्णाङ्क new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        यदि (new_fd < 0) {
                यदि (errno != ENOENT)
                        प्रत्यागम -errno;

                प्रत्यागम proc_fd_enoent_errno();
        }

        प्रत्यागम new_fd;
}

पूर्णाङ्क fd_reopen_propagate_append_and_position(पूर्णाङ्क fd, पूर्णाङ्क flags) {
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

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(!(flags & (O_APPEND|O_DIRECTORY)));

        पूर्णाङ्क existing_flags = fcntl(fd, F_GETFL);
        यदि (existing_flags < 0)
                प्रत्यागम -errno;

        पूर्णाङ्क new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        यदि (new_fd < 0)
                प्रत्यागम new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        यदि (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                यदि (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                अन्यथा यदि (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        प्रत्यागम new_fd;
}

पूर्णाङ्क fd_reopen_condition(
                पूर्णाङ्क fd,
                पूर्णाङ्क flags,
                पूर्णाङ्क mask,
                पूर्णाङ्क *ret_new_fd) {

        पूर्णाङ्क r, new_fd;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(!FLAGS_SET(flags, O_CREAT));
        प्रतिज्ञा(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        यदि (r < 0)
                प्रत्यागम -errno;

        यदि ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                प्रत्यागम fd;
        }

        new_fd = fd_reopen(fd, flags);
        यदि (new_fd < 0)
                प्रत्यागम new_fd;

        *ret_new_fd = new_fd;
        प्रत्यागम new_fd;
}

पूर्णाङ्क fd_is_opath(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fcntl(fd, F_GETFL);
        यदि (r < 0)
                प्रत्यागम -errno;

        प्रत्यागम FLAGS_SET(r, O_PATH);
}

पूर्णाङ्क fd_vet_accmode(पूर्णाङ्क fd, पूर्णाङ्क mode) {
        पूर्णाङ्क flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        यदि (flags < 0)
                प्रत्यागम -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        यदि (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                प्रत्यागम -EISDIR;

        यदि (FLAGS_SET(flags, O_PATH))
                प्रत्यागम -EBADFD;

        flags &= O_ACCMODE_STRICT;

        यदि (flags == mode)
                प्रत्यागम 1;

        यदि (flags == O_RDWR)
                प्रत्यागम 0;

        प्रत्यागम -EPROTOTYPE;
}

पूर्णाङ्क fd_is_writable(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        यदि (r >= 0)
                प्रत्यागम true;

        यदि (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                प्रत्यागम false;

        प्रत्यागम r;
}

पूर्णाङ्क fd_verify_safe_flags_full(पूर्णाङ्क fd, पूर्णाङ्क extra_flags) {
        पूर्णाङ्क flags, unexpected_flags;

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

        प्रतिज्ञा(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        यदि (flags < 0)
                प्रत्यागम -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        यदि (unexpected_flags != 0)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        प्रत्यागम flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(शून्य) {
        _cleanup_free_ वर्ण *nr_open = रिक्त;
        पूर्णाङ्क r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_पंक्ति_file("/proc/sys/fs/nr_open", &nr_open);
        यदि (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        अन्यथा {
                unsigned v;

                r = safe_atou(nr_open, &v);
                यदि (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                अन्यथा
                        प्रत्यागम v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        प्रत्यागम NR_OPEN_DEFAULT;
}

पूर्णाङ्क fd_get_diskseq(पूर्णाङ्क fd, uint64_t *ret) {
        uint64_t diskseq;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        यदि (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                यदि (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        प्रत्यागम -errno;

                प्रत्यागम -EOPNOTSUPP;
        }

        *ret = diskseq;

        प्रत्यागम 0;
}

स्थिर bool is_literal_मूल(ध्रुव वर्ण *p) {
        यदि (!p)
                प्रत्यागम false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        प्रत्यागम n >= 1 && p[n] == 0;
}

पूर्णाङ्क path_is_मूल_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path) {
        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        यदि (dir_fd == XAT_FDROOT && isempty(path))
                प्रत्यागम true;

        यदि (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_मूल(path))
                प्रत्यागम true;

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        यदि (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                यदि (fd == -ENOTDIR)
                        प्रत्यागम false; /* the root dir must be a dir */
                यदि (fd < 0)
                        प्रत्यागम fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        प्रत्यागम fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

पूर्णाङ्क fds_inode_and_mount_same(पूर्णाङ्क fd1, पूर्णाङ्क fd2) {
        संरचना statx sx1, sx2;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(fd1));
        प्रतिज्ञा(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ रिक्त, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                प्रत्यागम true;

        r = xstatx(fd2, /* path = */ रिक्त, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        यदि (r < 0)
                प्रत्यागम r;

        r = statx_mount_same(&sx1, &sx2);
        यदि (r <= 0)
                प्रत्यागम r;

        प्रत्यागम statx_inode_same(&sx1, &sx2);
}

पूर्णाङ्क resolve_xat_fdroot(पूर्णाङ्क *fd, ध्रुव वर्ण **path, वर्ण **ret_buffer) {
        प्रतिज्ञा(fd);
        प्रतिज्ञा(path);
        प्रतिज्ञा(ret_buffer);

        यदि (*fd != XAT_FDROOT) {
                *ret_buffer = रिक्त;
                प्रत्यागम 0;
        }

        यदि (isempty(*path)) {
                *path = "/";
                *ret_buffer = रिक्त;
        } अन्यथा यदि (!path_is_absolute(*path)) {
                वर्ण *p = strjoin("/", *path);
                यदि (!p)
                        प्रत्यागम -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        प्रत्यागम 1;
}

वर्ण* format_proc_fd_path(वर्ण buf[स्थिर PROC_FD_PATH_MAX], पूर्णाङ्क fd) {
        प्रतिज्ञा(buf);
        प्रतिज्ञा(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        प्रत्यागम buf;
}

ध्रुव वर्ण* accmode_to_string(पूर्णाङ्क flags) {
        विकल्प (flags & O_ACCMODE_STRICT) {
        पक्ष O_RDONLY:
                प्रत्यागम "ro";
        पक्ष O_WRONLY:
                प्रत्यागम "wo";
        पक्ष O_RDWR:
                प्रत्यागम "rw";
        स्वभाव:
                प्रत्यागम रिक्त;
        }
}

वर्ण* format_proc_pid_fd_path(वर्ण buf[स्थिर PROC_PID_FD_PATH_MAX], pid_t pid, पूर्णाङ्क fd) {
        प्रतिज्ञा(buf);
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        प्रत्यागम buf;
}

पूर्णाङ्क proc_fd_enoent_errno(शून्य) {
        पूर्णाङ्क r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        यदि (r == 0)
                प्रत्यागम -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        यदि (r > 0)
                प्रत्यागम -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        प्रत्यागम -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
