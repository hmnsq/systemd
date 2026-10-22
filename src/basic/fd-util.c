/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <linux/fs.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/kcmp.h>
#അന്തര്ഭാവ <sys/resource.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "sort-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#define MAX_FD_LOOP_LIMIT (1024*1024)

പൂർണ്ണസംഖ്യ close_nointr(പൂർണ്ണസംഖ്യ fd) {
        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (close(fd) >= 0)
                തിരികെ 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        എങ്കിൽ (errno == EINTR)
                തിരികെ 0;

        തിരികെ -errno;
}

പൂർണ്ണസംഖ്യ safe_close(പൂർണ്ണസംഖ്യ fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        എങ്കിൽ (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        തിരികെ -EBADF;
}

ശൂന്യം safe_close_pair(പൂർണ്ണസംഖ്യ p[സ്ഥിതം 2]) {
        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                തിരികെ;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

ശൂന്യം close_many(സ്ഥിരം പൂർണ്ണസംഖ്യ fds[], size_t n_fds) {
        പ്രതിജ്ഞാ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

ശൂന്യം close_many_unset(പൂർണ്ണസംഖ്യ fds[], size_t n_fds) {
        പ്രതിജ്ഞാ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

ശൂന്യം close_many_and_free(പൂർണ്ണസംഖ്യ *fds, size_t n_fds) {
        പ്രതിജ്ഞാ(fds || n_fds == 0);

        close_many(fds, n_fds);
        സ്വതന്ത്രം(fds);
}

പൂർണ്ണസംഖ്യ fclose_nointr(FILE *f) {
        പ്രതിജ്ഞാ(f);

        /* Same as close_nointr(), but for fclose() */

        errno = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        എങ്കിൽ (fclose(f) == 0)
                തിരികെ 0;

        എങ്കിൽ (errno == EINTR)
                തിരികെ 0;

        തിരികെ errno_or_else(EIO);
}

FILE* safe_fclose(FILE *f) {

        /* Same as safe_close(), but for fclose() */

        എങ്കിൽ (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        തിരികെ ശൂന്യ;
}

DIR* safe_closedir(DIR *d) {

        എങ്കിൽ (d) {
                PROTECT_ERRNO;

                assert_se(closedir(d) >= 0 || errno != EBADF);
        }

        തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ fd_nonblock(പൂർണ്ണസംഖ്യ fd, bool nonblock) {
        പൂർണ്ണസംഖ്യ flags, nflags;

        പ്രതിജ്ഞാ(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        എങ്കിൽ (flags < 0)
                തിരികെ -errno;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        എങ്കിൽ (nflags == flags)
                തിരികെ 0;

        എങ്കിൽ (fcntl(fd, F_SETFL, nflags) < 0)
                തിരികെ -errno;

        തിരികെ 1;
}

ശൂന്യം nonblock_resetp(പൂർണ്ണസംഖ്യ *fd) {
        പ്രതിജ്ഞാ(fd);

        PROTECT_ERRNO;

        എങ്കിൽ (*fd >= 0)
                (ശൂന്യം) fd_nonblock(*fd, false);
}

പൂർണ്ണസംഖ്യ stdio_disable_nonblock(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ fd_cloexec(പൂർണ്ണസംഖ്യ fd, bool cloexec) {
        പൂർണ്ണസംഖ്യ flags, nflags;

        പ്രതിജ്ഞാ(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        എങ്കിൽ (flags < 0)
                തിരികെ -errno;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        എങ്കിൽ (nflags == flags)
                തിരികെ 0;

        തിരികെ RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

പൂർണ്ണസംഖ്യ fd_cloexec_many(സ്ഥിരം പൂർണ്ണസംഖ്യ fds[], size_t n_fds, bool cloexec) {
        പൂർണ്ണസംഖ്യ r = 0;

        പ്രതിജ്ഞാ(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                എങ്കിൽ (*fd < 0) /* Skip gracefully over already invalidated fds */
                        തുടരുക;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        തിരികെ r;
}

സ്ഥിതം bool fd_in_set(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം പൂർണ്ണസംഖ്യ fds[], size_t n_fds) {
        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                എങ്കിൽ (*i < 0)
                        തുടരുക;

                എങ്കിൽ (*i == fd)
                        തിരികെ true;
        }

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ get_max_fd(ശൂന്യം) {
        ഘടന rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        എങ്കിൽ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                തിരികെ -errno;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        എങ്കിൽ (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                തിരികെ FD_SETSIZE-1;

        എങ്കിൽ (m == RLIM_INFINITY || m > INT_MAX) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                തിരികെ INT_MAX;

        തിരികെ (പൂർണ്ണസംഖ്യ) (m - 1);
}

പൂർണ്ണസംഖ്യ close_all_fds_frugal(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except) {
        പൂർണ്ണസംഖ്യ max_fd, r = 0;

        പ്രതിജ്ഞാ(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        എങ്കിൽ (max_fd < 0)
                തിരികെ max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        എങ്കിൽ (max_fd > MAX_FD_LOOP_LIMIT)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        വേണ്ടി (പൂർണ്ണസംഖ്യ fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                പൂർണ്ണസംഖ്യ q;

                എങ്കിൽ (fd_in_set(fd, except, n_except))
                        തുടരുക;

                q = close_nointr(fd);
                എങ്കിൽ (q != -EBADF)
                        RET_GATHER(r, q);
        }

        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ close_all_fds_special_case(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except) {
        പ്രതിജ്ഞാ(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        എങ്കിൽ (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        തിരഞ്ഞെടുപ്പ് (n_except) {

        അവസ്ഥ 0:
                /* Close everything. Yay! */
                എങ്കിൽ (close_range(3, INT_MAX, 0) < 0)
                        തിരികെ -errno;

                തിരികെ 1;

        അവസ്ഥ 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                എങ്കിൽ (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        തിരികെ -errno;

                എങ്കിൽ (except[0] < INT_MAX && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        തിരികെ -errno;

                തിരികെ 1;

        സ്ഥിരസ്ഥിതി:
                തിരികെ 0;
        }
}

പൂർണ്ണസംഖ്യ close_all_fds_without_malloc(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r > 0) /* special case worked! */
                തിരികെ 0;

        തിരികെ close_all_fds_frugal(except, n_except);
}

പൂർണ്ണസംഖ്യ close_all_fds(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r > 0) /* special case worked! */
                തിരികെ 0;

        _cleanup_free_ പൂർണ്ണസംഖ്യ *sorted_malloc = ശൂന്യ;
        size_t n_sorted;
        പൂർണ്ണസംഖ്യ *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        പ്രതിജ്ഞാ(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        എങ്കിൽ (n_sorted > ALLOCA_MAX / വലുപ്പം(പൂർണ്ണസംഖ്യ)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(പൂർണ്ണസംഖ്യ, n_sorted);
        അല്ലെങ്കിൽ
                sorted = newa(പൂർണ്ണസംഖ്യ, n_sorted);

        എങ്കിൽ (!sorted) /* Fallback on OOM. */
                തിരികെ close_all_fds_frugal(except, n_except);

        memcpy(sorted, except, n_except * വലുപ്പം(പൂർണ്ണസംഖ്യ));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        വേണ്ടി (size_t i = 0; i < n_sorted-1; i++) {
                പൂർണ്ണസംഖ്യ start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                പ്രതിജ്ഞാ(end >= start);

                എങ്കിൽ (end - start <= 1)
                        തുടരുക;

                /* Close everything between the start and end fds (both of which shall stay open) */
                എങ്കിൽ (close_range(start + 1, end - 1, 0) < 0)
                        തിരികെ -errno;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        എങ്കിൽ (sorted[n_sorted-1] >= INT_MAX) /* Dont let the addition below overflow */
                തിരികെ 0;

        എങ്കിൽ (close_range(sorted[n_sorted-1] + 1, INT_MAX, 0) < 0)
                തിരികെ -errno;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pack_fds(പൂർണ്ണസംഖ്യ fds[], size_t n_fds) {
        എങ്കിൽ (n_fds <= 0)
                തിരികെ 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        പ്രതിജ്ഞാ(fds);

        വേണ്ടി (പൂർണ്ണസംഖ്യ start = 0;;) {
                പൂർണ്ണസംഖ്യ restart_from = -1;

                വേണ്ടി (പൂർണ്ണസംഖ്യ i = start; i < (പൂർണ്ണസംഖ്യ) n_fds; i++) {
                        പൂർണ്ണസംഖ്യ nfd;

                        /* Already at right index? */
                        എങ്കിൽ (fds[i] == i + 3)
                                തുടരുക;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        എങ്കിൽ (nfd < 0)
                                തിരികെ -errno;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        എങ്കിൽ (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                എങ്കിൽ (restart_from < 0)
                        നിർത്തുക;

                start = restart_from;
        }

        പ്രതിജ്ഞാ(fds[0] == 3);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_validate(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (fd < 0)
                തിരികെ -EBADF;

        എങ്കിൽ (fcntl(fd, F_GETFD) < 0)
                തിരികെ -errno;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ same_fd(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b) {
        ഘടന stat sta, stb;
        pid_t pid;
        പൂർണ്ണസംഖ്യ r, fa, fb;

        പ്രതിജ്ഞാ(a >= 0);
        പ്രതിജ്ഞാ(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        എങ്കിൽ (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        എങ്കിൽ (r > 0)
                തിരികെ true;
        എങ്കിൽ (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        എങ്കിൽ (errno == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } അല്ലെങ്കിൽ എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno) && errno != EINVAL)
                തിരികെ -errno;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        എങ്കിൽ (r >= 0)
                തിരികെ !r;
        എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ERRNO_IS_PRIVILEGE(errno))
                തിരികെ -errno;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        എങ്കിൽ (fstat(a, &sta) < 0)
                തിരികെ -errno;

        എങ്കിൽ (fstat(b, &stb) < 0)
                തിരികെ -errno;

        എങ്കിൽ (!stat_inode_same(&sta, &stb))
                തിരികെ false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        എങ്കിൽ (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                തിരികെ false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        എങ്കിൽ (fa < 0)
                തിരികെ -errno;

        fb = fcntl(b, F_GETFL);
        എങ്കിൽ (fb < 0)
                തിരികെ -errno;

        തിരികെ fa == fb;
}

bool fdname_is_valid(സ്ഥിരം അക്ഷരം *s) {
        സ്ഥിരം അക്ഷരം *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        എങ്കിൽ (!s)
                തിരികെ false;

        വേണ്ടി (p = s; *p; p++) {
                എങ്കിൽ (*p < ' ')
                        തിരികെ false;
                എങ്കിൽ (*p >= 127)
                        തിരികെ false;
                എങ്കിൽ (*p == ':')
                        തിരികെ false;
        }

        തിരികെ p - s <= FDNAME_MAX;
}

പൂർണ്ണസംഖ്യ fd_get_path(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(fd));

        എങ്കിൽ (fd == AT_FDCWD)
                തിരികെ safe_getcwd(ret);
        എങ്കിൽ (fd == XAT_FDROOT)
                തിരികെ strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ proc_fd_enoent_errno();
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ move_fd(പൂർണ്ണസംഖ്യ from, പൂർണ്ണസംഖ്യ to, പൂർണ്ണസംഖ്യ cloexec) {
        പൂർണ്ണസംഖ്യ r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        എങ്കിൽ (from < 0)
                തിരികെ -EBADF;
        എങ്കിൽ (to < 0)
                തിരികെ -EBADF;

        എങ്കിൽ (from == to) {

                എങ്കിൽ (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                തിരികെ to;
        }

        എങ്കിൽ (cloexec < 0) {
                പൂർണ്ണസംഖ്യ fl;

                fl = fcntl(from, F_GETFD, 0);
                എങ്കിൽ (fl < 0)
                        തിരികെ -errno;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        എങ്കിൽ (r < 0)
                തിരികെ -errno;

        പ്രതിജ്ഞാ(r == to);

        safe_close(from);

        തിരികെ to;
}

പൂർണ്ണസംഖ്യ fd_move_above_stdio(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ flags, copy;
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

        എങ്കിൽ (fd < 0 || fd > 2)
                തിരികെ fd;

        flags = fcntl(fd, F_GETFD, 0);
        എങ്കിൽ (flags < 0)
                തിരികെ fd;

        എങ്കിൽ (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        അല്ലെങ്കിൽ
                copy = fcntl(fd, F_DUPFD, 3);
        എങ്കിൽ (copy < 0)
                തിരികെ fd;

        പ്രതിജ്ഞാ(copy > 2);

        (ശൂന്യം) close(fd);
        തിരികെ copy;
}

പൂർണ്ണസംഖ്യ rearrange_stdio(പൂർണ്ണസംഖ്യ original_നിവേശം_fd, പൂർണ്ണസംഖ്യ original_output_fd, പൂർണ്ണസംഖ്യ original_error_fd) {
        പൂർണ്ണസംഖ്യ fd[3] = { original_നിവേശം_fd,             /* Put together an array of fds we work on */
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

        null_readable = original_നിവേശം_fd < 0;
        null_writable = original_output_fd < 0 || original_error_fd < 0;

        /* First step, open /dev/null once, if we need it */
        എങ്കിൽ (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = open("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                എങ്കിൽ (null_fd < 0) {
                        r = -errno;
                        പോകുക finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                എങ്കിൽ (null_fd < 3) {
                        പൂർണ്ണസംഖ്യ copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        എങ്കിൽ (copy < 0) {
                                r = -errno;
                                പോകുക finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 3; i++)
                എങ്കിൽ (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                അല്ലെങ്കിൽ എങ്കിൽ (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        എങ്കിൽ (copy_fd[i] < 0) {
                                r = -errno;
                                പോകുക finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 3; i++)
                എങ്കിൽ (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        എങ്കിൽ (r < 0)
                                പോകുക finish;
                } അല്ലെങ്കിൽ {
                        പ്രതിജ്ഞാ(fd[i] > 2);

                        എങ്കിൽ (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -errno;
                                പോകുക finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_നിവേശം_fd);
        എങ്കിൽ (original_output_fd != original_നിവേശം_fd)
                safe_close_above_stdio(original_output_fd);
        എങ്കിൽ (original_error_fd != original_നിവേശം_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ fd_reopen(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ flags) {
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(fd));
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, O_CREAT));

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

        എങ്കിൽ (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                തിരികെ -ELOOP;

        എങ്കിൽ (fd == XAT_FDROOT)
                തിരികെ RET_NERRNO(open("/", flags | O_DIRECTORY));

        എങ്കിൽ (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                തിരികെ RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        പൂർണ്ണസംഖ്യ new_fd = open(FORMAT_PROC_FD_PATH(fd), flags);
        എങ്കിൽ (new_fd < 0) {
                എങ്കിൽ (errno != ENOENT)
                        തിരികെ -errno;

                തിരികെ proc_fd_enoent_errno();
        }

        തിരികെ new_fd;
}

പൂർണ്ണസംഖ്യ fd_reopen_propagate_append_and_position(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ flags) {
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

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(!(flags & (O_APPEND|O_DIRECTORY)));

        പൂർണ്ണസംഖ്യ existing_flags = fcntl(fd, F_GETFL);
        എങ്കിൽ (existing_flags < 0)
                തിരികെ -errno;

        പൂർണ്ണസംഖ്യ new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        എങ്കിൽ (new_fd < 0)
                തിരികെ new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, SEEK_CUR);
        എങ്കിൽ (p > 0) {
                off_t new_p = lseek(new_fd, p, SEEK_SET);
                എങ്കിൽ (new_p < 0)
                        log_debug_errno(errno,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                അല്ലെങ്കിൽ എങ്കിൽ (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (long long) new_p, (long long) p);
        }

        തിരികെ new_fd;
}

പൂർണ്ണസംഖ്യ fd_reopen_condition(
                പൂർണ്ണസംഖ്യ fd,
                പൂർണ്ണസംഖ്യ flags,
                പൂർണ്ണസംഖ്യ mask,
                പൂർണ്ണസംഖ്യ *ret_new_fd) {

        പൂർണ്ണസംഖ്യ r, new_fd;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, O_CREAT));
        പ്രതിജ്ഞാ(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        എങ്കിൽ (r < 0)
                തിരികെ -errno;

        എങ്കിൽ ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                തിരികെ fd;
        }

        new_fd = fd_reopen(fd, flags);
        എങ്കിൽ (new_fd < 0)
                തിരികെ new_fd;

        *ret_new_fd = new_fd;
        തിരികെ new_fd;
}

പൂർണ്ണസംഖ്യ fd_is_opath(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = fcntl(fd, F_GETFL);
        എങ്കിൽ (r < 0)
                തിരികെ -errno;

        തിരികെ FLAGS_SET(r, O_PATH);
}

പൂർണ്ണസംഖ്യ fd_vet_accmode(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ mode) {
        പൂർണ്ണസംഖ്യ flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        എങ്കിൽ (flags < 0)
                തിരികെ -errno;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        എങ്കിൽ (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                തിരികെ -EISDIR;

        എങ്കിൽ (FLAGS_SET(flags, O_PATH))
                തിരികെ -EBADFD;

        flags &= O_ACCMODE_STRICT;

        എങ്കിൽ (flags == mode)
                തിരികെ 1;

        എങ്കിൽ (flags == O_RDWR)
                തിരികെ 0;

        തിരികെ -EPROTOTYPE;
}

പൂർണ്ണസംഖ്യ fd_is_writable(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        എങ്കിൽ (r >= 0)
                തിരികെ true;

        എങ്കിൽ (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                തിരികെ false;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ fd_verify_safe_flags_full(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ extra_flags) {
        പൂർണ്ണസംഖ്യ flags, unexpected_flags;

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

        പ്രതിജ്ഞാ(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        എങ്കിൽ (flags < 0)
                തിരികെ -errno;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        എങ്കിൽ (unexpected_flags != 0)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (unsigned) unexpected_flags);

        തിരികെ flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

unsigned read_nr_open(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *nr_open = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_വരി_file("/proc/sys/fs/nr_open", &nr_open);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        അല്ലെങ്കിൽ {
                unsigned v;

                r = safe_atou(nr_open, &v);
                എങ്കിൽ (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                അല്ലെങ്കിൽ
                        തിരികെ v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        തിരികെ NR_OPEN_DEFAULT;
}

പൂർണ്ണസംഖ്യ fd_get_diskseq(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        uint64_t diskseq;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                        തിരികെ -errno;

                തിരികെ -EOPNOTSUPP;
        }

        *ret = diskseq;

        തിരികെ 0;
}

സ്ഥിതം bool is_literal_വേര്(സ്ഥിരം അക്ഷരം *p) {
        എങ്കിൽ (!p)
                തിരികെ false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        size_t n = strspn(p, "/");
        തിരികെ n >= 1 && p[n] == 0;
}

പൂർണ്ണസംഖ്യ path_is_വേര്_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));

        എങ്കിൽ (dir_fd == XAT_FDROOT && isempty(path))
                തിരികെ true;

        എങ്കിൽ (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_വേര്(path))
                തിരികെ true;

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        എങ്കിൽ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                എങ്കിൽ (fd == -ENOTDIR)
                        തിരികെ false; /* the root dir must be a dir */
                എങ്കിൽ (fd < 0)
                        തിരികെ fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        തിരികെ fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

പൂർണ്ണസംഖ്യ fds_inode_and_mount_same(പൂർണ്ണസംഖ്യ fd1, പൂർണ്ണസംഖ്യ fd2) {
        ഘടന statx sx1, sx2;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(fd1));
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ ശൂന്യ, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                തിരികെ true;

        r = xstatx(fd2, /* path = */ ശൂന്യ, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = statx_mount_same(&sx1, &sx2);
        എങ്കിൽ (r <= 0)
                തിരികെ r;

        തിരികെ statx_inode_same(&sx1, &sx2);
}

പൂർണ്ണസംഖ്യ resolve_xat_fdroot(പൂർണ്ണസംഖ്യ *fd, സ്ഥിരം അക്ഷരം **path, അക്ഷരം **ret_buffer) {
        പ്രതിജ്ഞാ(fd);
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret_buffer);

        എങ്കിൽ (*fd != XAT_FDROOT) {
                *ret_buffer = ശൂന്യ;
                തിരികെ 0;
        }

        എങ്കിൽ (isempty(*path)) {
                *path = "/";
                *ret_buffer = ശൂന്യ;
        } അല്ലെങ്കിൽ എങ്കിൽ (!path_is_absolute(*path)) {
                അക്ഷരം *p = strjoin("/", *path);
                എങ്കിൽ (!p)
                        തിരികെ -ENOMEM;

                *path = *ret_buffer = p;
        }

        *fd = AT_FDCWD;

        തിരികെ 1;
}

അക്ഷരം* format_proc_fd_path(അക്ഷരം buf[സ്ഥിതം PROC_FD_PATH_MAX], പൂർണ്ണസംഖ്യ fd) {
        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        തിരികെ buf;
}

സ്ഥിരം അക്ഷരം* accmode_to_string(പൂർണ്ണസംഖ്യ flags) {
        തിരഞ്ഞെടുപ്പ് (flags & O_ACCMODE_STRICT) {
        അവസ്ഥ O_RDONLY:
                തിരികെ "ro";
        അവസ്ഥ O_WRONLY:
                തിരികെ "wo";
        അവസ്ഥ O_RDWR:
                തിരികെ "rw";
        സ്ഥിരസ്ഥിതി:
                തിരികെ ശൂന്യ;
        }
}

അക്ഷരം* format_proc_pid_fd_path(അക്ഷരം buf[സ്ഥിതം PROC_PID_FD_PATH_MAX], pid_t pid, പൂർണ്ണസംഖ്യ fd) {
        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        തിരികെ buf;
}

പൂർണ്ണസംഖ്യ proc_fd_enoent_errno(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        എങ്കിൽ (r == 0)
                തിരികെ -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        എങ്കിൽ (r > 0)
                തിരികെ -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        തിരികെ -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
