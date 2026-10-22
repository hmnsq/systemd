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

static целое fopen_temporary_internal(целое dir_fd, const char *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        если (fd < 0)
                вернуть -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        если (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                вернуть r;
        }

        если (ret_file)
                *ret_file = TAKE_PTR(f);

        вернуть 0;
}

целое fopen_temporary_at(целое dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        целое r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        если (r < 0)
                вернуть r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(t);

        вернуть 0;
}

целое fopen_temporary_child_at(целое dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        целое r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        если (!path) {
                r = tmp_dir(&path);
                если (r < 0)
                        вернуть r;
        }

        r = tempfn_random_child(path, NULL, &t);
        если (r < 0)
                вернуть r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        если (r < 0)
                вернуть r;

        если (ret_path)
                *ret_path = TAKE_PTR(t);

        вернуть 0;
}

/* This is much like mkostemp() but is subject to umask(). */
целое mkostemp_safe(char *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        вернуть RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

целое fmkostemp_safe(char *pattern, const char *mode, FILE **ret_f) {
        _cleanup_close_ целое fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        если (fd < 0)
                вернуть fd;

        f = take_fdopen(&fd, mode);
        если (!f)
                вернуть -errno;

        *ret_f = f;
        вернуть 0;
}

void unlink_tempfilep(char (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        если (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static целое tempfn_build(const char *p, const char *pre, const char *post, bool child, char **ret) {
        _cleanup_free_ char *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        целое r;

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

        если (pre && strchr(pre, '/'))
                вернуть -EINVAL;

        если (post && strchr(post, '/'))
                вернуть -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        если (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                вернуть -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        если (child) {
                d = strdup(p);
                если (!d)
                        вернуть -ENOMEM;
        } иначе {
                r = path_split_prefix_filename(p, &d, &fn);
                если (r < 0)
                        вернуть r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        если (!nf)
                вернуть -ENOMEM;

        если (d) {
                если (!path_extend(&d, nf))
                        вернуть -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } иначе
                result = TAKE_PTR(nf);

        если (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                вернуть -EINVAL;

        *ret = TAKE_PTR(result);
        вернуть 0;
}

целое tempfn_xxxxxx(const char *p, const char *extra, char **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        вернуть tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

целое tempfn_random(const char *p, const char *extra, char **ret) {
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

        если (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                вернуть -ENOMEM;

        вернуть tempfn_build(p, extra, s, /* child= */ false, ret);
}

целое tempfn_random_child(const char *p, const char *extra, char **ret) {
        _cleanup_free_ char *s = NULL;
        целое r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        если (!p) {
                r = tmp_dir(&p);
                если (r < 0)
                        вернуть r;
        }

        если (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                вернуть -ENOMEM;

        вернуть tempfn_build(p, extra, s, /* child= */ true, ret);
}

целое open_tmpfile_unlinkable(const char *directory, целое flags) {
        целое r;

        если (!directory) {
                r = tmp_dir(&directory);
                если (r < 0)
                        вернуть r;
        } иначе если (isempty(directory))
                вернуть -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        целое fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        если (fd >= 0)
                вернуть fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ char *p = path_join(directory, "/systemd-tmp-XXXXXX");
        если (!p)
                вернуть -ENOMEM;

        fd = mkostemp_safe(p);
        если (fd < 0)
                вернуть fd;

        (void) unlink(p);

        вернуть fd;
}

целое open_tmpfile_linkable_at(целое dir_fd, const char *target, целое flags, char **ret_path) {
        целое r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        целое fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        если (fd >= 0) {
                *ret_path = NULL;
                вернуть fd;
        }

        если (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ char *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        если (r < 0)
                вернуть r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        если (fd < 0)
                вернуть -errno;

        *ret_path = TAKE_PTR(tmp);

        вернуть fd;
}

целое fopen_tmpfile_linkable_at(целое dir_fd, const char *target, целое flags, char **ret_path, FILE **ret_file) {
        _cleanup_free_ char *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ целое fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        если (fd < 0)
                вернуть fd;

        f = take_fdopen(&fd, "w");
        если (!f)
                вернуть -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        вернуть 0;
}

целое link_tmpfile_at(целое fd, целое dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        целое r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        если (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                вернуть -errno;

        если (path) {
                если (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                иначе
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } иначе {
                если (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                иначе
                        r = link_fd(fd, dir_fd, target);
        }
        если (r < 0)
                вернуть r;

        если (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                если (r < 0)
                        вернуть r;
        }

        вернуть 0;
}

целое flink_tmpfile_at(FILE *f, целое dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        целое fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        если (fd < 0) /* Not all FILE* objects encapsulate fds */
                вернуть -EBADF;

        r = fflush_and_check(f);
        если (r < 0)
                вернуть r;

        вернуть link_tmpfile_at(fd, dir_fd, path, target, flags);
}

целое mkdtemp_malloc(const char *template, char **ret) {
        _cleanup_free_ char *p = NULL;
        целое r;

        assert(ret);

        если (template)
                p = strdup(template);
        иначе {
                const char *tmp;

                r = tmp_dir(&tmp);
                если (r < 0)
                        вернуть r;

                p = path_join(tmp, "XXXXXX");
        }
        если (!p)
                вернуть -ENOMEM;

        если (!mkdtemp(p))
                вернуть -errno;

        *ret = TAKE_PTR(p);
        вернуть 0;
}

целое mkdtemp_open(const char *template, целое flags, char **ret) {
        _cleanup_free_ char *p = NULL;
        целое fd, r;

        r = mkdtemp_malloc(template, &p);
        если (r < 0)
                вернуть r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        если (fd < 0) {
                (void) rmdir(p);
                вернуть fd;
        }

        если (ret)
                *ret = TAKE_PTR(p);

        вернуть fd;
}

void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d) {
        assert(d);

        если (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                вернуть;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
