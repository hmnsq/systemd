/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "log.h"
#include "path-util.h"
#include "random-util.h"
#include "string-util.h"
#include "sync-util.h"
#include "tmpfile-util.h"
#include "umask-util.h"

static số_nguyên fopen_temporary_internal(số_nguyên dir_fd, const char *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ số_nguyên fd = -EBADF;
        số_nguyên r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        nếu (fd < 0)
                trả_về -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        nếu (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                trả_về r;
        }

        nếu (ret_file)
                *ret_file = TAKE_PTR(f);

        trả_về 0;
}

số_nguyên fopen_temporary_at(số_nguyên dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        số_nguyên r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        nếu (r < 0)
                trả_về r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        nếu (r < 0)
                trả_về r;

        nếu (ret_path)
                *ret_path = TAKE_PTR(t);

        trả_về 0;
}

số_nguyên fopen_temporary_child_at(số_nguyên dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        số_nguyên r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        nếu (!path) {
                r = tmp_dir(&path);
                nếu (r < 0)
                        trả_về r;
        }

        r = tempfn_random_child(path, NULL, &t);
        nếu (r < 0)
                trả_về r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        nếu (r < 0)
                trả_về r;

        nếu (ret_path)
                *ret_path = TAKE_PTR(t);

        trả_về 0;
}

/* This is much like mkostemp() but is subject to umask(). */
số_nguyên mkostemp_safe(char *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        trả_về RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

số_nguyên fmkostemp_safe(char *pattern, const char *mode, FILE **ret_f) {
        _cleanup_close_ số_nguyên fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        nếu (fd < 0)
                trả_về fd;

        f = take_fdopen(&fd, mode);
        nếu (!f)
                trả_về -errno;

        *ret_f = f;
        trả_về 0;
}

void unlink_tempfilep(char (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        nếu (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static số_nguyên tempfn_build(const char *p, const char *pre, const char *post, bool child, char **ret) {
        _cleanup_free_ char *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        số_nguyên r;

        assert(p);
        assert(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        nếu (pre && strchr(pre, '/'))
                trả_về -EINVAL;

        nếu (post && strchr(post, '/'))
                trả_về -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        nếu (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                trả_về -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        nếu (child) {
                d = strdup(p);
                nếu (!d)
                        trả_về -ENOMEM;
        } không_thì {
                r = path_split_prefix_filename(p, &d, &fn);
                nếu (r < 0)
                        trả_về r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        nếu (!nf)
                trả_về -ENOMEM;

        nếu (d) {
                nếu (!path_extend(&d, nf))
                        trả_về -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } không_thì
                result = TAKE_PTR(nf);

        nếu (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                trả_về -EINVAL;

        *ret = TAKE_PTR(result);
        trả_về 0;
}

số_nguyên tempfn_xxxxxx(const char *p, const char *extra, char **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        trả_về tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

số_nguyên tempfn_random(const char *p, const char *extra, char **ret) {
        _cleanup_free_ char *s = NULL;

        assert(p);
        assert(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        nếu (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                trả_về -ENOMEM;

        trả_về tempfn_build(p, extra, s, /* child= */ false, ret);
}

số_nguyên tempfn_random_child(const char *p, const char *extra, char **ret) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        nếu (!p) {
                r = tmp_dir(&p);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                trả_về -ENOMEM;

        trả_về tempfn_build(p, extra, s, /* child= */ true, ret);
}

số_nguyên open_tmpfile_unlinkable(const char *directory, số_nguyên flags) {
        số_nguyên r;

        nếu (!directory) {
                r = tmp_dir(&directory);
                nếu (r < 0)
                        trả_về r;
        } không_thì nếu (isempty(directory))
                trả_về -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        số_nguyên fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        nếu (fd >= 0)
                trả_về fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ char *p = path_join(directory, "/systemd-tmp-XXXXXX");
        nếu (!p)
                trả_về -ENOMEM;

        fd = mkostemp_safe(p);
        nếu (fd < 0)
                trả_về fd;

        (void) unlink(p);

        trả_về fd;
}

số_nguyên open_tmpfile_linkable_at(số_nguyên dir_fd, const char *target, số_nguyên flags, char **ret_path) {
        số_nguyên r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        số_nguyên fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        nếu (fd >= 0) {
                *ret_path = NULL;
                trả_về fd;
        }

        nếu (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ char *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        nếu (r < 0)
                trả_về r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        nếu (fd < 0)
                trả_về -errno;

        *ret_path = TAKE_PTR(tmp);

        trả_về fd;
}

số_nguyên fopen_tmpfile_linkable_at(số_nguyên dir_fd, const char *target, số_nguyên flags, char **ret_path, FILE **ret_file) {
        _cleanup_free_ char *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ số_nguyên fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        nếu (fd < 0)
                trả_về fd;

        f = take_fdopen(&fd, "w");
        nếu (!f)
                trả_về -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        trả_về 0;
}

số_nguyên link_tmpfile_at(số_nguyên fd, số_nguyên dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        số_nguyên r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        nếu (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                trả_về -errno;

        nếu (path) {
                nếu (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                không_thì
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } không_thì {
                nếu (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                không_thì
                        r = link_fd(fd, dir_fd, target);
        }
        nếu (r < 0)
                trả_về r;

        nếu (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                nếu (r < 0)
                        trả_về r;
        }

        trả_về 0;
}

số_nguyên flink_tmpfile_at(FILE *f, số_nguyên dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        số_nguyên fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        nếu (fd < 0) /* Not all FILE* objects encapsulate fds */
                trả_về -EBADF;

        r = fflush_and_check(f);
        nếu (r < 0)
                trả_về r;

        trả_về link_tmpfile_at(fd, dir_fd, path, target, flags);
}

số_nguyên mkdtemp_malloc(const char *template, char **ret) {
        _cleanup_free_ char *p = NULL;
        số_nguyên r;

        assert(ret);

        nếu (template)
                p = strdup(template);
        không_thì {
                const char *tmp;

                r = tmp_dir(&tmp);
                nếu (r < 0)
                        trả_về r;

                p = path_join(tmp, "XXXXXX");
        }
        nếu (!p)
                trả_về -ENOMEM;

        nếu (!mkdtemp(p))
                trả_về -errno;

        *ret = TAKE_PTR(p);
        trả_về 0;
}

số_nguyên mkdtemp_open(const char *template, số_nguyên flags, char **ret) {
        _cleanup_free_ char *p = NULL;
        số_nguyên fd, r;

        r = mkdtemp_malloc(template, &p);
        nếu (r < 0)
                trả_về r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        nếu (fd < 0) {
                (void) rmdir(p);
                trả_về fd;
        }

        nếu (ret)
                *ret = TAKE_PTR(p);

        trả_về fd;
}

void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d) {
        assert(d);

        nếu (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                trả_về;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
