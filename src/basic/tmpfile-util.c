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

static 정수 fopen_temporary_internal(정수 dir_fd, const char *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ 정수 fd = -EBADF;
        정수 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        만약 (fd < 0)
                반환 -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        만약 (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                반환 r;
        }

        만약 (ret_file)
                *ret_file = TAKE_PTR(f);

        반환 0;
}

정수 fopen_temporary_at(정수 dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        정수 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        만약 (r < 0)
                반환 r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        만약 (r < 0)
                반환 r;

        만약 (ret_path)
                *ret_path = TAKE_PTR(t);

        반환 0;
}

정수 fopen_temporary_child_at(정수 dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        정수 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        만약 (!path) {
                r = tmp_dir(&path);
                만약 (r < 0)
                        반환 r;
        }

        r = tempfn_random_child(path, NULL, &t);
        만약 (r < 0)
                반환 r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        만약 (r < 0)
                반환 r;

        만약 (ret_path)
                *ret_path = TAKE_PTR(t);

        반환 0;
}

/* This is much like mkostemp() but is subject to umask(). */
정수 mkostemp_safe(char *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        반환 RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

정수 fmkostemp_safe(char *pattern, const char *mode, FILE **ret_f) {
        _cleanup_close_ 정수 fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        만약 (fd < 0)
                반환 fd;

        f = take_fdopen(&fd, mode);
        만약 (!f)
                반환 -errno;

        *ret_f = f;
        반환 0;
}

void unlink_tempfilep(char (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        만약 (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static 정수 tempfn_build(const char *p, const char *pre, const char *post, bool child, char **ret) {
        _cleanup_free_ char *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        정수 r;

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

        만약 (pre && strchr(pre, '/'))
                반환 -EINVAL;

        만약 (post && strchr(post, '/'))
                반환 -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        만약 (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                반환 -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        만약 (child) {
                d = strdup(p);
                만약 (!d)
                        반환 -ENOMEM;
        } 아니면 {
                r = path_split_prefix_filename(p, &d, &fn);
                만약 (r < 0)
                        반환 r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        만약 (!nf)
                반환 -ENOMEM;

        만약 (d) {
                만약 (!path_extend(&d, nf))
                        반환 -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } 아니면
                result = TAKE_PTR(nf);

        만약 (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                반환 -EINVAL;

        *ret = TAKE_PTR(result);
        반환 0;
}

정수 tempfn_xxxxxx(const char *p, const char *extra, char **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        반환 tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

정수 tempfn_random(const char *p, const char *extra, char **ret) {
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

        만약 (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                반환 -ENOMEM;

        반환 tempfn_build(p, extra, s, /* child= */ false, ret);
}

정수 tempfn_random_child(const char *p, const char *extra, char **ret) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        만약 (!p) {
                r = tmp_dir(&p);
                만약 (r < 0)
                        반환 r;
        }

        만약 (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                반환 -ENOMEM;

        반환 tempfn_build(p, extra, s, /* child= */ true, ret);
}

정수 open_tmpfile_unlinkable(const char *directory, 정수 flags) {
        정수 r;

        만약 (!directory) {
                r = tmp_dir(&directory);
                만약 (r < 0)
                        반환 r;
        } 아니면 만약 (isempty(directory))
                반환 -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        정수 fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        만약 (fd >= 0)
                반환 fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ char *p = path_join(directory, "/systemd-tmp-XXXXXX");
        만약 (!p)
                반환 -ENOMEM;

        fd = mkostemp_safe(p);
        만약 (fd < 0)
                반환 fd;

        (void) unlink(p);

        반환 fd;
}

정수 open_tmpfile_linkable_at(정수 dir_fd, const char *target, 정수 flags, char **ret_path) {
        정수 r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        정수 fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        만약 (fd >= 0) {
                *ret_path = NULL;
                반환 fd;
        }

        만약 (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ char *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        만약 (r < 0)
                반환 r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        만약 (fd < 0)
                반환 -errno;

        *ret_path = TAKE_PTR(tmp);

        반환 fd;
}

정수 fopen_tmpfile_linkable_at(정수 dir_fd, const char *target, 정수 flags, char **ret_path, FILE **ret_file) {
        _cleanup_free_ char *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ 정수 fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        만약 (fd < 0)
                반환 fd;

        f = take_fdopen(&fd, "w");
        만약 (!f)
                반환 -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        반환 0;
}

정수 link_tmpfile_at(정수 fd, 정수 dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        정수 r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        만약 (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                반환 -errno;

        만약 (path) {
                만약 (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                아니면
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } 아니면 {
                만약 (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                아니면
                        r = link_fd(fd, dir_fd, target);
        }
        만약 (r < 0)
                반환 r;

        만약 (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                만약 (r < 0)
                        반환 r;
        }

        반환 0;
}

정수 flink_tmpfile_at(FILE *f, 정수 dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        정수 fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        만약 (fd < 0) /* Not all FILE* objects encapsulate fds */
                반환 -EBADF;

        r = fflush_and_check(f);
        만약 (r < 0)
                반환 r;

        반환 link_tmpfile_at(fd, dir_fd, path, target, flags);
}

정수 mkdtemp_malloc(const char *template, char **ret) {
        _cleanup_free_ char *p = NULL;
        정수 r;

        assert(ret);

        만약 (template)
                p = strdup(template);
        아니면 {
                const char *tmp;

                r = tmp_dir(&tmp);
                만약 (r < 0)
                        반환 r;

                p = path_join(tmp, "XXXXXX");
        }
        만약 (!p)
                반환 -ENOMEM;

        만약 (!mkdtemp(p))
                반환 -errno;

        *ret = TAKE_PTR(p);
        반환 0;
}

정수 mkdtemp_open(const char *template, 정수 flags, char **ret) {
        _cleanup_free_ char *p = NULL;
        정수 fd, r;

        r = mkdtemp_malloc(template, &p);
        만약 (r < 0)
                반환 r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        만약 (fd < 0) {
                (void) rmdir(p);
                반환 fd;
        }

        만약 (ret)
                *ret = TAKE_PTR(p);

        반환 fd;
}

void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d) {
        assert(d);

        만약 (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                반환;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
