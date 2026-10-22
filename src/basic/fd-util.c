/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <linux/fs.h>
#समावेश <sys/ioctl.h>
#समावेश <sys/kcmp.h>
#समावेश <sys/resource.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "dirent-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "format-util.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "process-util.h"
#समावेश "sort-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"

/* The maximum number of iterations in the loop to close descriptors in the fallback case
 * when /proc/self/fd/ is inaccessible. */
#घोषणा MAX_FD_LOOP_LIMIT (1024*1024)

पूर्णांक close_nointr(पूर्णांक fd) {
        निश्चित(fd >= 0);

        अगर (बंद(fd) >= 0)
                वापस 0;

        /*
         * Just ignore EINTR; a retry loop is the wrong thing to do on
         * Linux.
         *
         * http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
         * https://bugzilla.gnome.org/show_bug.cgi?id=682819
         * http://utcc.utoronto.ca/~cks/space/blog/unix/CloseEINTR
         * https://sites.google.com/site/michaelsafyan/software-engineering/checkforeintrwheninvokingclosethinkagain
         */
        अगर (त्रुटि_सं == EINTR)
                वापस 0;

        वापस -त्रुटि_सं;
}

पूर्णांक safe_close(पूर्णांक fd) {
        /*
         * Like close_nointr() but cannot fail. Guarantees errno is unchanged. Is a noop for negative fds,
         * and returns -EBADF, so that it can be used in this syntax:
         *
         * fd = safe_close(fd);
         */

        अगर (fd >= 0) {
                PROTECT_ERRNO;

                /* The kernel might return pretty much any error code
                 * via close(), but the fd will be closed anyway. The
                 * only condition we want to check for here is whether
                 * the fd was invalid at all... */

                assert_se(close_nointr(fd) != -EBADF);
        }

        वापस -EBADF;
}

व्योम safe_close_pair(पूर्णांक p[अटल 2]) {
        निश्चित(p);

        अगर (p[0] == p[1]) {
                /* Special case pairs which use the same fd in both
                 * directions... */
                p[0] = p[1] = safe_close(p[0]);
                वापस;
        }

        p[0] = safe_close(p[0]);
        p[1] = safe_close(p[1]);
}

व्योम close_many(स्थिर पूर्णांक fds[], माप_प्रकार n_fds) {
        निश्चित(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                safe_close(*fd);
}

व्योम close_many_unset(पूर्णांक fds[], माप_प्रकार n_fds) {
        निश्चित(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds)
                *fd = safe_close(*fd);
}

व्योम close_many_and_free(पूर्णांक *fds, माप_प्रकार n_fds) {
        निश्चित(fds || n_fds == 0);

        close_many(fds, n_fds);
        मुक्त(fds);
}

पूर्णांक fclose_nointr(खाता *f) {
        निश्चित(f);

        /* Same as close_nointr(), but for fclose() */

        त्रुटि_सं = 0; /* Extra safety: if the FILE* object is not encapsulating an fd, it might not set errno
                    * correctly. Let's hence initialize it to zero first, so that we aren't confused by any
                    * prior errno here */
        अगर (ख_बंद(f) == 0)
                वापस 0;

        अगर (त्रुटि_सं == EINTR)
                वापस 0;

        वापस errno_or_else(EIO);
}

खाता* safe_fclose(खाता *f) {

        /* Same as safe_close(), but for fclose() */

        अगर (f) {
                PROTECT_ERRNO;

                assert_se(fclose_nointr(f) != -EBADF);
        }

        वापस शून्य;
}

सूची* safe_closedir(सूची *d) {

        अगर (d) {
                PROTECT_ERRNO;

                assert_se(बंद_सूची(d) >= 0 || त्रुटि_सं != EBADF);
        }

        वापस शून्य;
}

पूर्णांक fd_nonblock(पूर्णांक fd, bool nonblock) {
        पूर्णांक flags, nflags;

        निश्चित(fd >= 0);

        flags = fcntl(fd, F_GETFL, 0);
        अगर (flags < 0)
                वापस -त्रुटि_सं;

        nflags = UPDATE_FLAG(flags, O_NONBLOCK, nonblock);
        अगर (nflags == flags)
                वापस 0;

        अगर (fcntl(fd, F_SETFL, nflags) < 0)
                वापस -त्रुटि_सं;

        वापस 1;
}

व्योम nonblock_resetp(पूर्णांक *fd) {
        निश्चित(fd);

        PROTECT_ERRNO;

        अगर (*fd >= 0)
                (व्योम) fd_nonblock(*fd, false);
}

पूर्णांक stdio_disable_nonblock(व्योम) {
        पूर्णांक ret = 0;

        /* stdin/stdout/stderr really should have O_NONBLOCK, which would confuse apps if left on, as
         * write()s might unexpectedly fail with EAGAIN. */

        RET_GATHER(ret, fd_nonblock(STDIN_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDOUT_FILENO, false));
        RET_GATHER(ret, fd_nonblock(STDERR_FILENO, false));

        वापस ret;
}

पूर्णांक fd_cloexec(पूर्णांक fd, bool cloexec) {
        पूर्णांक flags, nflags;

        निश्चित(fd >= 0);

        flags = fcntl(fd, F_GETFD, 0);
        अगर (flags < 0)
                वापस -त्रुटि_सं;

        nflags = UPDATE_FLAG(flags, FD_CLOEXEC, cloexec);
        अगर (nflags == flags)
                वापस 0;

        वापस RET_NERRNO(fcntl(fd, F_SETFD, nflags));
}

पूर्णांक fd_cloexec_many(स्थिर पूर्णांक fds[], माप_प्रकार n_fds, bool cloexec) {
        पूर्णांक r = 0;

        निश्चित(fds || n_fds == 0);

        FOREACH_ARRAY(fd, fds, n_fds) {
                अगर (*fd < 0) /* Skip gracefully over already invalidated fds */
                        जारी;

                RET_GATHER(r, fd_cloexec(*fd, cloexec));
        }

        वापस r;
}

अटल bool fd_in_set(पूर्णांक fd, स्थिर पूर्णांक fds[], माप_प्रकार n_fds) {
        निश्चित(fd >= 0);
        निश्चित(fds || n_fds == 0);

        FOREACH_ARRAY(i, fds, n_fds) {
                अगर (*i < 0)
                        जारी;

                अगर (*i == fd)
                        वापस true;
        }

        वापस false;
}

पूर्णांक get_max_fd(व्योम) {
        काष्ठा rlimit rl;
        rlim_t m;

        /* Return the highest possible fd, based RLIMIT_NOFILE, but enforcing FD_SETSIZE-1 as lower boundary
         * and INT_MAX as upper boundary. */

        अगर (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                वापस -त्रुटि_सं;

        m = MAX(rl.rlim_cur, rl.rlim_max);
        अगर (m < FD_SETSIZE) /* Let's always cover at least 1024 fds */
                वापस FD_SETSIZE-1;

        अगर (m == RLIM_INFINITY || m > पूर्णांक_उच्च) /* Saturate on overflow. After all fds are "int", hence can
                                                * never be above INT_MAX */
                वापस पूर्णांक_उच्च;

        वापस (पूर्णांक) (m - 1);
}

पूर्णांक close_all_fds_frugal(स्थिर पूर्णांक except[], माप_प्रकार n_except) {
        पूर्णांक max_fd, r = 0;

        निश्चित(except || n_except == 0);

        /* This is the inner fallback core of close_all_fds(). This never calls malloc() or so and hence is
         * safe to be called in signal handler context. Most users should call close_all_fds(), but when we
         * assume we are called from signal handler context, then use this simpler call instead. */

        max_fd = get_max_fd();
        अगर (max_fd < 0)
                वापस max_fd;

        /* Refuse to do the loop over more too many elements. It's better to fail immediately than to
         * spin the CPU for a long time. */
        अगर (max_fd > MAX_FD_LOOP_LIMIT)
                वापस log_debug_errno(SYNTHETIC_ERRNO(EPERM),
                                       "Refusing to loop over %d potential fds.", max_fd);

        क्रम (पूर्णांक fd = 3; fd >= 0; fd = fd < max_fd ? fd + 1 : -EBADF) {
                पूर्णांक q;

                अगर (fd_in_set(fd, except, n_except))
                        जारी;

                q = close_nointr(fd);
                अगर (q != -EBADF)
                        RET_GATHER(r, q);
        }

        वापस r;
}

अटल पूर्णांक close_all_fds_special_case(स्थिर पूर्णांक except[], माप_प्रकार n_except) {
        निश्चित(n_except == 0 || except);

        /* Handles a few common special cases separately, since they are common and can be optimized really
         * nicely, since we won't need sorting for them. Returns > 0 if the special casing worked, 0
         * otherwise. */

        अगर (n_except == 1 && except[0] < 0) /* Minor optimization: if we only got one fd, and it's invalid,
                                             * we got none */
                n_except = 0;

        चयन (n_except) {

        हाल 0:
                /* Close everything. Yay! */
                अगर (close_range(3, पूर्णांक_उच्च, 0) < 0)
                        वापस -त्रुटि_सं;

                वापस 1;

        हाल 1:
                /* Close all but exactly one, then we don't need no sorting. This is a pretty common
                 * case, hence let's handle it specially. */

                अगर (except[0] > 3 && close_range(3, except[0] - 1, 0) < 0)
                        वापस -त्रुटि_सं;

                अगर (except[0] < पूर्णांक_उच्च && close_range(MAX(3, except[0] + 1), -1, 0) < 0)
                        वापस -त्रुटि_सं;

                वापस 1;

        शेष:
                वापस 0;
        }
}

पूर्णांक close_all_fds_without_malloc(स्थिर पूर्णांक except[], माप_प्रकार n_except) {
        पूर्णांक r;

        निश्चित(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        अगर (r < 0)
                वापस r;
        अगर (r > 0) /* special case worked! */
                वापस 0;

        वापस close_all_fds_frugal(except, n_except);
}

पूर्णांक close_all_fds(स्थिर पूर्णांक except[], माप_प्रकार n_except) {
        पूर्णांक r;

        निश्चित(n_except == 0 || except);

        r = close_all_fds_special_case(except, n_except);
        अगर (r < 0)
                वापस r;
        अगर (r > 0) /* special case worked! */
                वापस 0;

        _cleanup_free_ पूर्णांक *sorted_malloc = शून्य;
        माप_प्रकार n_sorted;
        पूर्णांक *sorted;

        /* In the best case we have close_range() to close all fds between a start and an end fd, which we
         * can use on the "inverted" exception array, i.e. all intervals between all adjacent pairs from the
         * sorted exception array. This changes loop complexity from O(n) where n is number of open fds to
         * O(m⋅log(m)) where m is the number of fds to keep open. Given that we assume n ≫ m that's
         * preferable to us. */

        निश्चित(n_except < SIZE_MAX);
        n_sorted = n_except + 1;

        अगर (n_sorted > ALLOCA_MAX / माप(पूर्णांक)) /* Use heap for large numbers of fds, stack otherwise */
                sorted = sorted_malloc = new(पूर्णांक, n_sorted);
        अन्यथा
                sorted = newa(पूर्णांक, n_sorted);

        अगर (!sorted) /* Fallback on OOM. */
                वापस close_all_fds_frugal(except, n_except);

        स_नकल(sorted, except, n_except * माप(पूर्णांक));

        /* Let's add fd 2 to the list of fds, to simplify the loop below, as this
         * allows us to cover the head of the array the same way as the body */
        sorted[n_sorted-1] = 2;

        typesafe_qsort(sorted, n_sorted, cmp_int);

        क्रम (माप_प्रकार i = 0; i < n_sorted-1; i++) {
                पूर्णांक start, end;

                start = MAX(sorted[i], 2); /* The first three fds shall always remain open */
                end = MAX(sorted[i+1], 2);

                निश्चित(end >= start);

                अगर (end - start <= 1)
                        जारी;

                /* Close everything between the start and end fds (both of which shall stay open) */
                अगर (close_range(start + 1, end - 1, 0) < 0)
                        वापस -त्रुटि_सं;
        }

        /* The loop succeeded. Let's now close everything beyond the end */

        अगर (sorted[n_sorted-1] >= पूर्णांक_उच्च) /* Dont let the addition below overflow */
                वापस 0;

        अगर (close_range(sorted[n_sorted-1] + 1, पूर्णांक_उच्च, 0) < 0)
                वापस -त्रुटि_सं;

        वापस 0;
}

पूर्णांक pack_fds(पूर्णांक fds[], माप_प्रकार n_fds) {
        अगर (n_fds <= 0)
                वापस 0;

        /* Shifts around the fds in the provided array such that they
         * all end up packed next to each-other, in order, starting
         * from SD_LISTEN_FDS_START. This must be called after close_all_fds();
         * it is likely to freeze up otherwise. You should probably use safe_fork_full
         * with FORK_CLOSE_ALL_FDS|FORK_PACK_FDS set, to ensure that this is done correctly.
         * The fds array is modified in place with the new FD numbers. */

        निश्चित(fds);

        क्रम (पूर्णांक start = 0;;) {
                पूर्णांक restart_from = -1;

                क्रम (पूर्णांक i = start; i < (पूर्णांक) n_fds; i++) {
                        पूर्णांक nfd;

                        /* Already at right index? */
                        अगर (fds[i] == i + 3)
                                जारी;

                        nfd = fcntl(fds[i], F_DUPFD, i + 3);
                        अगर (nfd < 0)
                                वापस -त्रुटि_सं;

                        safe_close(fds[i]);
                        fds[i] = nfd;

                        /* Hmm, the fd we wanted isn't free? Then
                         * let's remember that and try again from here */
                        अगर (nfd != i + 3 && restart_from < 0)
                                restart_from = i;
                }

                अगर (restart_from < 0)
                        अवरोध;

                start = restart_from;
        }

        निश्चित(fds[0] == 3);

        वापस 0;
}

पूर्णांक fd_validate(पूर्णांक fd) {
        अगर (fd < 0)
                वापस -EBADF;

        अगर (fcntl(fd, F_GETFD) < 0)
                वापस -त्रुटि_सं;

        वापस 0;
}

पूर्णांक same_fd(पूर्णांक a, पूर्णांक b) {
        काष्ठा stat sta, stb;
        pid_t pid;
        पूर्णांक r, fa, fb;

        निश्चित(a >= 0);
        निश्चित(b >= 0);

        /* Compares two file descriptors. Note that semantics are quite different depending on whether we
         * have F_DUPFD_QUERY/kcmp() or we don't. If we have F_DUPFD_QUERY/kcmp() this will only return true
         * for dup()ed file descriptors, but not otherwise. If we don't have F_DUPFD_QUERY/kcmp() this will
         * also return true for two fds of the same file, created by separate open() calls. Since we use this
         * call mostly for filtering out duplicates in the fd store this difference hopefully doesn't matter
         * too much.
         *
         * Guarantees that if either of the passed fds is not allocated we'll return -EBADF. */

        अगर (a == b) {
                /* Let's validate that the fd is valid */
                r = fd_validate(a);
                अगर (r < 0)
                        वापस r;

                वापस true;
        }

        /* Try to use F_DUPFD_QUERY if we have it first, as it is the nicest API */
        r = fcntl(a, F_DUPFD_QUERY, b);
        अगर (r > 0)
                वापस true;
        अगर (r == 0) {
                /* The kernel will return 0 in case the first fd is allocated, but the 2nd is not. (Which is different in the kcmp() case) Explicitly validate it hence. */
                r = fd_validate(b);
                अगर (r < 0)
                        वापस r;

                वापस false;
        }
        /* On old kernels (< 6.10) that do not support F_DUPFD_QUERY this will return EINVAL for regular fds, and EBADF on O_PATH fds. Confusing. */
        अगर (त्रुटि_सं == EBADF) {
                /* EBADF could mean two things: the first fd is not valid, or it is valid and is O_PATH and
                 * F_DUPFD_QUERY is not supported. Let's validate the fd explicitly, to distinguish this
                 * case. */
                r = fd_validate(a);
                अगर (r < 0)
                        वापस r;

                /* If the fd is valid, but we got EBADF, then let's try kcmp(). */
        } अन्यथा अगर (!ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) && !ERRNO_IS_PRIVILEGE(त्रुटि_सं) && त्रुटि_सं != EINVAL)
                वापस -त्रुटि_सं;

        /* Try to use kcmp() if we have it. */
        pid = getpid_cached();
        r = kcmp(pid, pid, KCMP_FILE, a, b);
        अगर (r >= 0)
                वापस !r;
        अगर (!ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) && !ERRNO_IS_PRIVILEGE(त्रुटि_सं))
                वापस -त्रुटि_सं;

        /* We have neither F_DUPFD_QUERY nor kcmp(), use fstat() instead. */
        अगर (ख_स्थिति(a, &sta) < 0)
                वापस -त्रुटि_सं;

        अगर (ख_स्थिति(b, &stb) < 0)
                वापस -त्रुटि_सं;

        अगर (!stat_inode_same(&sta, &stb))
                वापस false;

        /* We consider all device fds different, since two device fds might refer to quite different device
         * contexts even though they share the same inode and backing dev_t. */

        अगर (S_ISCHR(sta.st_mode) || S_ISBLK(sta.st_mode))
                वापस false;

        /* The fds refer to the same inode on disk, let's also check if they have the same fd flags. This is
         * useful to distinguish the read and write side of a pipe created with pipe(). */
        fa = fcntl(a, F_GETFL);
        अगर (fa < 0)
                वापस -त्रुटि_सं;

        fb = fcntl(b, F_GETFL);
        अगर (fb < 0)
                वापस -त्रुटि_सं;

        वापस fa == fb;
}

bool fdname_is_valid(स्थिर अक्षर *s) {
        स्थिर अक्षर *p;

        /* Validates a name for $LISTEN_FDNAMES. We basically allow
         * everything ASCII that's not a control character. Also, as
         * special exception the ":" character is not allowed, as we
         * use that as field separator in $LISTEN_FDNAMES.
         *
         * Note that the empty string is explicitly allowed
         * here. However, we limit the length of the names to 255
         * characters. */

        अगर (!s)
                वापस false;

        क्रम (p = s; *p; p++) {
                अगर (*p < ' ')
                        वापस false;
                अगर (*p >= 127)
                        वापस false;
                अगर (*p == ':')
                        वापस false;
        }

        वापस p - s <= FDNAME_MAX;
}

पूर्णांक fd_get_path(पूर्णांक fd, अक्षर **ret) {
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(fd));

        अगर (fd == AT_FDCWD)
                वापस safe_getcwd(ret);
        अगर (fd == XAT_FDROOT)
                वापस strdup_to(ret, "/");

        r = readlink_malloc(FORMAT_PROC_FD_PATH(fd), ret);
        अगर (r == -ENOENT)
                वापस proc_fd_enoent_errno();
        वापस r;
}

पूर्णांक move_fd(पूर्णांक from, पूर्णांक to, पूर्णांक cloexec) {
        पूर्णांक r;

        /* Move fd 'from' to 'to', make sure FD_CLOEXEC remains equal if requested, and release the old fd. If
         * 'cloexec' is passed as -1, the original FD_CLOEXEC is inherited for the new fd. If it is 0, it is turned
         * off, if it is > 0 it is turned on. */

        अगर (from < 0)
                वापस -EBADF;
        अगर (to < 0)
                वापस -EBADF;

        अगर (from == to) {

                अगर (cloexec >= 0) {
                        r = fd_cloexec(to, cloexec);
                        अगर (r < 0)
                                वापस r;
                }

                वापस to;
        }

        अगर (cloexec < 0) {
                पूर्णांक fl;

                fl = fcntl(from, F_GETFD, 0);
                अगर (fl < 0)
                        वापस -त्रुटि_सं;

                cloexec = FLAGS_SET(fl, FD_CLOEXEC);
        }

        r = dup3(from, to, cloexec ? O_CLOEXEC : 0);
        अगर (r < 0)
                वापस -त्रुटि_सं;

        निश्चित(r == to);

        safe_close(from);

        वापस to;
}

पूर्णांक fd_move_above_stdio(पूर्णांक fd) {
        पूर्णांक flags, copy;
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

        अगर (fd < 0 || fd > 2)
                वापस fd;

        flags = fcntl(fd, F_GETFD, 0);
        अगर (flags < 0)
                वापस fd;

        अगर (flags & FD_CLOEXEC)
                copy = fcntl(fd, F_DUPFD_CLOEXEC, 3);
        अन्यथा
                copy = fcntl(fd, F_DUPFD, 3);
        अगर (copy < 0)
                वापस fd;

        निश्चित(copy > 2);

        (व्योम) बंद(fd);
        वापस copy;
}

पूर्णांक rearrange_stdio(पूर्णांक original_निवेश_fd, पूर्णांक original_output_fd, पूर्णांक original_error_fd) {
        पूर्णांक fd[3] = { original_निवेश_fd,             /* Put together an array of fds we work on */
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
        अगर (null_readable || null_writable) {

                /* Let's open this with O_CLOEXEC first, and convert it to non-O_CLOEXEC when we move the fd to the final position. */
                null_fd = खोलो("/dev/null", (null_readable && null_writable ? O_RDWR :
                                             null_readable ? O_RDONLY : O_WRONLY) | O_CLOEXEC);
                अगर (null_fd < 0) {
                        r = -त्रुटि_सं;
                        जाओ finish;
                }

                /* If this fd is in the 0…2 range, let's move it out of it */
                अगर (null_fd < 3) {
                        पूर्णांक copy;

                        copy = fcntl(null_fd, F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        अगर (copy < 0) {
                                r = -त्रुटि_सं;
                                जाओ finish;
                        }

                        close_and_replace(null_fd, copy);
                }
        }

        /* Let's assemble fd[] with the fds to install in place of stdin/stdout/stderr */
        क्रम (पूर्णांक i = 0; i < 3; i++)
                अगर (fd[i] < 0)
                        fd[i] = null_fd;        /* A negative parameter means: connect this one to /dev/null */
                अन्यथा अगर (fd[i] != i && fd[i] < 3) {
                        /* This fd is in the 0…2 territory, but not at its intended place, move it out of there, so that we can work there. */
                        copy_fd[i] = fcntl(fd[i], F_DUPFD_CLOEXEC, 3); /* Duplicate this with O_CLOEXEC set */
                        अगर (copy_fd[i] < 0) {
                                r = -त्रुटि_सं;
                                जाओ finish;
                        }

                        fd[i] = copy_fd[i];
                }

        /* At this point we now have the fds to use in fd[], and they are all above the stdio range, so that
         * we have freedom to move them around. If the fds already were at the right places then the specific
         * fds are -EBADF. Let's now move them to the right places. This is the point of no return. */
        क्रम (पूर्णांक i = 0; i < 3; i++)
                अगर (fd[i] == i) {
                        /* fd is already in place, but let's make sure O_CLOEXEC is off */
                        r = fd_cloexec(i, false);
                        अगर (r < 0)
                                जाओ finish;
                } अन्यथा {
                        निश्चित(fd[i] > 2);

                        अगर (dup2(fd[i], i) < 0) { /* Turns off O_CLOEXEC on the new fd. */
                                r = -त्रुटि_सं;
                                जाओ finish;
                        }
                }

        r = 0;

finish:
        /* Close the original fds, but only if they were outside of the stdio range. Also, properly check for the same
         * fd passed in multiple times. */
        safe_close_above_stdio(original_निवेश_fd);
        अगर (original_output_fd != original_निवेश_fd)
                safe_close_above_stdio(original_output_fd);
        अगर (original_error_fd != original_निवेश_fd && original_error_fd != original_output_fd)
                safe_close_above_stdio(original_error_fd);

        /* Close the copies we moved > 2 */
        close_many(copy_fd, 3);

        /* Close our null fd, if it's > 2 */
        safe_close_above_stdio(null_fd);

        वापस r;
}

पूर्णांक fd_reopen(पूर्णांक fd, पूर्णांक flags) {
        निश्चित(wildcard_fd_is_valid(fd));
        निश्चित(!FLAGS_SET(flags, O_CREAT));

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

        अगर (FLAGS_SET(flags, O_NOFOLLOW))
                /* O_NOFOLLOW is not allowed in fd_reopen(), because after all this is primarily implemented
                 * via a symlink-based interface in /proc/self/fd. Let's refuse this here early. Note that
                 * the kernel would generate ELOOP here too, hence this manual check is mostly redundant –
                 * the only reason we add it here is so that the O_DIRECTORY special case (see below) behaves
                 * the same way as the non-O_DIRECTORY case. */
                वापस -ELOOP;

        अगर (fd == XAT_FDROOT)
                वापस RET_NERRNO(खोलो("/", flags | O_DIRECTORY));

        अगर (FLAGS_SET(flags, O_DIRECTORY) || fd == AT_FDCWD)
                /* If we shall reopen the fd as directory we can just go via "." and thus bypass the whole
                 * magic /proc/ directory, and make ourselves independent of that being mounted. */
                वापस RET_NERRNO(openat(fd, ".", flags | O_DIRECTORY));

        पूर्णांक new_fd = खोलो(FORMAT_PROC_FD_PATH(fd), flags);
        अगर (new_fd < 0) {
                अगर (त्रुटि_सं != ENOENT)
                        वापस -त्रुटि_सं;

                वापस proc_fd_enoent_errno();
        }

        वापस new_fd;
}

पूर्णांक fd_reopen_propagate_append_and_position(पूर्णांक fd, पूर्णांक flags) {
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

        निश्चित(fd >= 0);
        निश्चित(!(flags & (O_APPEND|O_DIRECTORY)));

        पूर्णांक existing_flags = fcntl(fd, F_GETFL);
        अगर (existing_flags < 0)
                वापस -त्रुटि_सं;

        पूर्णांक new_fd = fd_reopen(fd, flags | (existing_flags & O_APPEND));
        अगर (new_fd < 0)
                वापस new_fd;

        /* Try to adjust the offset, but ignore errors. */
        off_t p = lseek(fd, 0, प्रस्तुत_से);
        अगर (p > 0) {
                off_t new_p = lseek(new_fd, p, शुरू_से);
                अगर (new_p < 0)
                        log_debug_errno(त्रुटि_सं,
                                        "Failed to propagate file position for re-opened fd %d, ignoring: %m",
                                        fd);
                अन्यथा अगर (new_p != p)
                        log_debug("Failed to propagate file position for re-opened fd %d (%lld != %lld), ignoring.",
                                  fd, (दीर्घ दीर्घ) new_p, (दीर्घ दीर्घ) p);
        }

        वापस new_fd;
}

पूर्णांक fd_reopen_condition(
                पूर्णांक fd,
                पूर्णांक flags,
                पूर्णांक mask,
                पूर्णांक *ret_new_fd) {

        पूर्णांक r, new_fd;

        निश्चित(fd >= 0);
        निश्चित(!FLAGS_SET(flags, O_CREAT));
        निश्चित(ret_new_fd);

        /* Invokes fd_reopen(fd, flags), but only if the existing F_GETFL flags don't match the specified
         * flags (masked by the specified mask). This is useful for converting O_PATH fds into real fds if
         * needed, but only then. */

        r = fcntl(fd, F_GETFL);
        अगर (r < 0)
                वापस -त्रुटि_सं;

        अगर ((r & mask) == (flags & mask)) {
                *ret_new_fd = -EBADF;
                वापस fd;
        }

        new_fd = fd_reopen(fd, flags);
        अगर (new_fd < 0)
                वापस new_fd;

        *ret_new_fd = new_fd;
        वापस new_fd;
}

पूर्णांक fd_is_opath(पूर्णांक fd) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = fcntl(fd, F_GETFL);
        अगर (r < 0)
                वापस -त्रुटि_सं;

        वापस FLAGS_SET(r, O_PATH);
}

पूर्णांक fd_vet_accmode(पूर्णांक fd, पूर्णांक mode) {
        पूर्णांक flags;

        /* Check if fd is opened with desired access mode.
         *
         * Returns > 0 on strict match, == 0 if opened for both reading and writing (partial match),
         * -EPROTOTYPE otherwise. O_PATH fds are always refused with -EBADFD.
         *
         * Note that while on O_DIRECTORY -EISDIR will be returned, this should not be relied upon as
         * the flag might not have been specified when open() was called originally. */

        निश्चित(fd >= 0);
        निश्चित(IN_SET(mode, O_RDONLY, O_WRONLY, O_RDWR));

        flags = fcntl(fd, F_GETFL);
        अगर (flags < 0)
                वापस -त्रुटि_सं;

        /* O_TMPFILE in userspace is defined with O_DIRECTORY OR'ed in, so explicitly permit it.
         *
         * C.f. https://elixir.bootlin.com/linux/v6.17.7/source/include/uapi/asm-generic/fcntl.h#L92 */
        अगर (FLAGS_SET(flags, O_DIRECTORY) && !FLAGS_SET(flags, O_TMPFILE))
                वापस -EISDIR;

        अगर (FLAGS_SET(flags, O_PATH))
                वापस -EBADFD;

        flags &= O_ACCMODE_STRICT;

        अगर (flags == mode)
                वापस 1;

        अगर (flags == O_RDWR)
                वापस 0;

        वापस -EPROTOTYPE;
}

पूर्णांक fd_is_writable(पूर्णांक fd) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = fd_vet_accmode(fd, O_WRONLY);
        अगर (r >= 0)
                वापस true;

        अगर (IN_SET(r, -EPROTOTYPE, -EBADFD, -EISDIR))
                वापस false;

        वापस r;
}

पूर्णांक fd_verify_safe_flags_full(पूर्णांक fd, पूर्णांक extra_flags) {
        पूर्णांक flags, unexpected_flags;

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

        निश्चित(fd >= 0);

        flags = fcntl(fd, F_GETFL);
        अगर (flags < 0)
                वापस -त्रुटि_सं;

        unexpected_flags = flags & ~(O_ACCMODE_STRICT|O_NOFOLLOW|RAW_O_LARGEFILE|extra_flags);
        अगर (unexpected_flags != 0)
                वापस log_debug_errno(SYNTHETIC_ERRNO(EREMOTEIO),
                                       "Unexpected flags set for extrinsic fd: 0%o",
                                       (अचिन्हित) unexpected_flags);

        वापस flags & (O_ACCMODE_STRICT | extra_flags); /* return the flags variable, but remove the noise */
}

अचिन्हित read_nr_open(व्योम) {
        _cleanup_free_ अक्षर *nr_open = शून्य;
        पूर्णांक r;

        /* Returns the kernel's current fd limit, either by reading it of /proc/sys if that works, or using the
         * hard-coded default compiled-in value of current kernels (1M) if not. This call will never fail. */

        r = read_one_पंक्ति_file("/proc/sys/fs/nr_open", &nr_open);
        अगर (r < 0)
                log_debug_errno(r, "Failed to read /proc/sys/fs/nr_open, ignoring: %m");
        अन्यथा {
                अचिन्हित v;

                r = safe_atou(nr_open, &v);
                अगर (r < 0)
                        log_debug_errno(r, "Failed to parse /proc/sys/fs/nr_open value '%s', ignoring: %m", nr_open);
                अन्यथा
                        वापस v;
        }

        /* If we fail, fall back to the hard-coded kernel limit of 1024 * 1024. */
        वापस NR_OPEN_DEFAULT;
}

पूर्णांक fd_get_diskseq(पूर्णांक fd, uint64_t *ret) {
        uint64_t diskseq;

        निश्चित(fd >= 0);
        निश्चित(ret);

        अगर (ioctl(fd, BLKGETDISKSEQ, &diskseq) < 0) {
                /* Note that the kernel is weird: non-existing ioctls currently return EINVAL
                 * rather than ENOTTY on loopback block devices. They should fix that in the kernel,
                 * but in the meantime we accept both here. */
                अगर (!ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) && त्रुटि_सं != EINVAL)
                        वापस -त्रुटि_सं;

                वापस -EOPNOTSUPP;
        }

        *ret = diskseq;

        वापस 0;
}

अटल bool is_literal_मूल(स्थिर अक्षर *p) {
        अगर (!p)
                वापस false;

        /* Check if string consists of at least one '/', and possibly more, but nothing else */
        माप_प्रकार n = म_अखोज(p, "/");
        वापस n >= 1 && p[n] == 0;
}

पूर्णांक path_is_मूल_at(पूर्णांक dir_fd, स्थिर अक्षर *path) {
        निश्चित(wildcard_fd_is_valid(dir_fd));

        अगर (dir_fd == XAT_FDROOT && isempty(path))
                वापस true;

        अगर (IN_SET(dir_fd, XAT_FDROOT, AT_FDCWD) && is_literal_मूल(path))
                वापस true;

        _cleanup_close_ पूर्णांक fd = -EBADF;
        अगर (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_DIRECTORY|O_CLOEXEC);
                अगर (fd == -ENOTDIR)
                        वापस false; /* the root dir must be a dir */
                अगर (fd < 0)
                        वापस fd;

                dir_fd = fd;
        }

        /* Even if the root directory has the same inode as our fd, the fd may not point to the root
         * directory "/", and we also need to check that the mount ids are the same. Otherwise, a construct
         * like the following could be used to trick us:
         *
         * $ mkdir /tmp/x
         * $ mount --bind / /tmp/x
         */

        वापस fds_inode_and_mount_same(dir_fd, XAT_FDROOT);
}

पूर्णांक fds_inode_and_mount_same(पूर्णांक fd1, पूर्णांक fd2) {
        काष्ठा statx sx1, sx2;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(fd1));
        निश्चित(wildcard_fd_is_valid(fd2));

        r = xstatx(fd1, /* path = */ शून्य, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx1);
        अगर (r < 0)
                वापस r;

        अगर (fd1 == fd2) /* Shortcut things if fds are the same (only after validating the fd) */
                वापस true;

        r = xstatx(fd2, /* path = */ शून्य, AT_EMPTY_PATH,
                   STATX_TYPE|STATX_INO|STATX_MNT_ID,
                   &sx2);
        अगर (r < 0)
                वापस r;

        r = statx_mount_same(&sx1, &sx2);
        अगर (r <= 0)
                वापस r;

        वापस statx_inode_same(&sx1, &sx2);
}

पूर्णांक resolve_xat_fdroot(पूर्णांक *fd, स्थिर अक्षर **path, अक्षर **ret_बफर) {
        निश्चित(fd);
        निश्चित(path);
        निश्चित(ret_बफर);

        अगर (*fd != XAT_FDROOT) {
                *ret_बफर = शून्य;
                वापस 0;
        }

        अगर (isempty(*path)) {
                *path = "/";
                *ret_बफर = शून्य;
        } अन्यथा अगर (!path_is_absolute(*path)) {
                अक्षर *p = strjoin("/", *path);
                अगर (!p)
                        वापस -ENOMEM;

                *path = *ret_बफर = p;
        }

        *fd = AT_FDCWD;

        वापस 1;
}

अक्षर* format_proc_fd_path(अक्षर buf[अटल PROC_FD_PATH_MAX], पूर्णांक fd) {
        निश्चित(buf);
        निश्चित(fd >= 0);
        assert_se(snprintf_ok(buf, PROC_FD_PATH_MAX, "/proc/self/fd/%i", fd));
        वापस buf;
}

स्थिर अक्षर* accmode_to_string(पूर्णांक flags) {
        चयन (flags & O_ACCMODE_STRICT) {
        हाल O_RDONLY:
                वापस "ro";
        हाल O_WRONLY:
                वापस "wo";
        हाल O_RDWR:
                वापस "rw";
        शेष:
                वापस शून्य;
        }
}

अक्षर* format_proc_pid_fd_path(अक्षर buf[अटल PROC_PID_FD_PATH_MAX], pid_t pid, पूर्णांक fd) {
        निश्चित(buf);
        निश्चित(fd >= 0);
        निश्चित(pid >= 0);
        assert_se(snprintf_ok(buf, PROC_PID_FD_PATH_MAX, "/proc/" PID_FMT "/fd/%i", pid == 0 ? getpid_cached() : pid, fd));
        वापस buf;
}

पूर्णांक proc_fd_enoent_errno(व्योम) {
        पूर्णांक r;

        /* When ENOENT is returned during the use of FORMAT_PROC_FD_PATH, it can mean two things:
         * that the fd does not exist or that /proc/ is not mounted.
         * Let's make things debuggable and figure out the most appropriate errno. */

        r = proc_mounted();
        अगर (r == 0)
                वापस -ENOSYS;  /* /proc/ is not available or not set up properly, we're most likely
                                    in some chroot environment. */
        अगर (r > 0)
                वापस -EBADF;   /* If /proc/ is definitely around then this means the fd is not valid. */

        वापस -ENOENT;          /* Otherwise let's propagate the original ENOENT. */
}
