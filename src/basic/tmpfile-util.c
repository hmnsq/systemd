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

static 整数 fopen_temporary_internal(整数 dir_fd, const char *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        もし (fd < 0)
                戻る -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        もし (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                戻る r;
        }

        もし (ret_file)
                *ret_file = TAKE_PTR(f);

        戻る 0;
}

整数 fopen_temporary_at(整数 dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        整数 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        もし (r < 0)
                戻る r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        もし (r < 0)
                戻る r;

        もし (ret_path)
                *ret_path = TAKE_PTR(t);

        戻る 0;
}

整数 fopen_temporary_child_at(整数 dir_fd, const char *path, FILE **ret_file, char **ret_path) {
        _cleanup_free_ char *t = NULL;
        整数 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        もし (!path) {
                r = tmp_dir(&path);
                もし (r < 0)
                        戻る r;
        }

        r = tempfn_random_child(path, NULL, &t);
        もし (r < 0)
                戻る r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        もし (r < 0)
                戻る r;

        もし (ret_path)
                *ret_path = TAKE_PTR(t);

        戻る 0;
}

/* This is much like mkostemp() but is subject to umask(). */
整数 mkostemp_safe(char *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        戻る RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

整数 fmkostemp_safe(char *pattern, const char *mode, FILE **ret_f) {
        _cleanup_close_ 整数 fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        もし (fd < 0)
                戻る fd;

        f = take_fdopen(&fd, mode);
        もし (!f)
                戻る -errno;

        *ret_f = f;
        戻る 0;
}

void unlink_tempfilep(char (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        もし (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static 整数 tempfn_build(const char *p, const char *pre, const char *post, bool child, char **ret) {
        _cleanup_free_ char *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        整数 r;

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

        もし (pre && strchr(pre, '/'))
                戻る -EINVAL;

        もし (post && strchr(post, '/'))
                戻る -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        もし (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                戻る -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        もし (child) {
                d = strdup(p);
                もし (!d)
                        戻る -ENOMEM;
        } 否则 {
                r = path_split_prefix_filename(p, &d, &fn);
                もし (r < 0)
                        戻る r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        もし (!nf)
                戻る -ENOMEM;

        もし (d) {
                もし (!path_extend(&d, nf))
                        戻る -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } 否则
                result = TAKE_PTR(nf);

        もし (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                戻る -EINVAL;

        *ret = TAKE_PTR(result);
        戻る 0;
}

整数 tempfn_xxxxxx(const char *p, const char *extra, char **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        戻る tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

整数 tempfn_random(const char *p, const char *extra, char **ret) {
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

        もし (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                戻る -ENOMEM;

        戻る tempfn_build(p, extra, s, /* child= */ false, ret);
}

整数 tempfn_random_child(const char *p, const char *extra, char **ret) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        もし (!p) {
                r = tmp_dir(&p);
                もし (r < 0)
                        戻る r;
        }

        もし (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                戻る -ENOMEM;

        戻る tempfn_build(p, extra, s, /* child= */ true, ret);
}

整数 open_tmpfile_unlinkable(const char *directory, 整数 flags) {
        整数 r;

        もし (!directory) {
                r = tmp_dir(&directory);
                もし (r < 0)
                        戻る r;
        } 否则 もし (isempty(directory))
                戻る -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        整数 fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        もし (fd >= 0)
                戻る fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ char *p = path_join(directory, "/systemd-tmp-XXXXXX");
        もし (!p)
                戻る -ENOMEM;

        fd = mkostemp_safe(p);
        もし (fd < 0)
                戻る fd;

        (void) unlink(p);

        戻る fd;
}

整数 open_tmpfile_linkable_at(整数 dir_fd, const char *target, 整数 flags, char **ret_path) {
        整数 r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        整数 fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        もし (fd >= 0) {
                *ret_path = NULL;
                戻る fd;
        }

        もし (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ char *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        もし (r < 0)
                戻る r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        もし (fd < 0)
                戻る -errno;

        *ret_path = TAKE_PTR(tmp);

        戻る fd;
}

整数 fopen_tmpfile_linkable_at(整数 dir_fd, const char *target, 整数 flags, char **ret_path, FILE **ret_file) {
        _cleanup_free_ char *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ 整数 fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        もし (fd < 0)
                戻る fd;

        f = take_fdopen(&fd, "w");
        もし (!f)
                戻る -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        戻る 0;
}

整数 link_tmpfile_at(整数 fd, 整数 dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        整数 r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        もし (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                戻る -errno;

        もし (path) {
                もし (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                否则
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } 否则 {
                もし (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                否则
                        r = link_fd(fd, dir_fd, target);
        }
        もし (r < 0)
                戻る r;

        もし (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                もし (r < 0)
                        戻る r;
        }

        戻る 0;
}

整数 flink_tmpfile_at(FILE *f, 整数 dir_fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        整数 fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        もし (fd < 0) /* Not all FILE* objects encapsulate fds */
                戻る -EBADF;

        r = fflush_and_check(f);
        もし (r < 0)
                戻る r;

        戻る link_tmpfile_at(fd, dir_fd, path, target, flags);
}

整数 mkdtemp_malloc(const char *template, char **ret) {
        _cleanup_free_ char *p = NULL;
        整数 r;

        assert(ret);

        もし (template)
                p = strdup(template);
        否则 {
                const char *tmp;

                r = tmp_dir(&tmp);
                もし (r < 0)
                        戻る r;

                p = path_join(tmp, "XXXXXX");
        }
        もし (!p)
                戻る -ENOMEM;

        もし (!mkdtemp(p))
                戻る -errno;

        *ret = TAKE_PTR(p);
        戻る 0;
}

整数 mkdtemp_open(const char *template, 整数 flags, char **ret) {
        _cleanup_free_ char *p = NULL;
        整数 fd, r;

        r = mkdtemp_malloc(template, &p);
        もし (r < 0)
                戻る r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        もし (fd < 0) {
                (void) rmdir(p);
                戻る fd;
        }

        もし (ret)
                *ret = TAKE_PTR(p);

        戻る fd;
}

void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d) {
        assert(d);

        もし (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                戻る;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
