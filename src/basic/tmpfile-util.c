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

static ཧྲིལ fopen_temporary_internal(ཧྲིལ dir_fd, const var_na *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        གལ་ཏེ (fd < 0)
                ཕྱིར -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        གལ་ཏེ (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                ཕྱིར r;
        }

        གལ་ཏེ (ret_file)
                *ret_file = TAKE_PTR(f);

        ཕྱིར 0;
}

ཧྲིལ fopen_temporary_at(ཧྲིལ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        ཧྲིལ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_path)
                *ret_path = TAKE_PTR(t);

        ཕྱིར 0;
}

ཧྲིལ fopen_temporary_child_at(ཧྲིལ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        ཧྲིལ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        གལ་ཏེ (!path) {
                r = tmp_dir(&path);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        r = tempfn_random_child(path, NULL, &t);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_path)
                *ret_path = TAKE_PTR(t);

        ཕྱིར 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ཧྲིལ mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        ཕྱིར RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

ཧྲིལ fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        གལ་ཏེ (fd < 0)
                ཕྱིར fd;

        f = take_fdopen(&fd, mode);
        གལ་ཏེ (!f)
                ཕྱིར -errno;

        *ret_f = f;
        ཕྱིར 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        གལ་ཏེ (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static ཧྲིལ tempfn_build(const var_na *p, const var_na *pre, const var_na *post, bool child, var_na **ret) {
        _cleanup_free_ var_na *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        ཧྲིལ r;

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

        གལ་ཏེ (pre && strchr(pre, '/'))
                ཕྱིར -EINVAL;

        གལ་ཏེ (post && strchr(post, '/'))
                ཕྱིར -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        གལ་ཏེ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ཕྱིར -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        གལ་ཏེ (child) {
                d = strdup(p);
                གལ་ཏེ (!d)
                        ཕྱིར -ENOMEM;
        } དེ་མིན {
                r = path_split_prefix_filename(p, &d, &fn);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        གལ་ཏེ (!nf)
                ཕྱིར -ENOMEM;

        གལ་ཏེ (d) {
                གལ་ཏེ (!path_extend(&d, nf))
                        ཕྱིར -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } དེ་མིན
                result = TAKE_PTR(nf);

        གལ་ཏེ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(result);
        ཕྱིར 0;
}

ཧྲིལ tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ཕྱིར tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ཧྲིལ tempfn_random(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;

        assert(p);
        assert(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        གལ་ཏེ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ཕྱིར -ENOMEM;

        ཕྱིར tempfn_build(p, extra, s, /* child= */ false, ret);
}

ཧྲིལ tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        ཧྲིལ r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        གལ་ཏེ (!p) {
                r = tmp_dir(&p);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        གལ་ཏེ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ཕྱིར -ENOMEM;

        ཕྱིར tempfn_build(p, extra, s, /* child= */ true, ret);
}

ཧྲིལ open_tmpfile_unlinkable(const var_na *directory, ཧྲིལ flags) {
        ཧྲིལ r;

        གལ་ཏེ (!directory) {
                r = tmp_dir(&directory);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        } དེ་མིན གལ་ཏེ (isempty(directory))
                ཕྱིར -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ཧྲིལ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        གལ་ཏེ (fd >= 0)
                ཕྱིར fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        གལ་ཏེ (!p)
                ཕྱིར -ENOMEM;

        fd = mkostemp_safe(p);
        གལ་ཏེ (fd < 0)
                ཕྱིར fd;

        (void) unlink(p);

        ཕྱིར fd;
}

ཧྲིལ open_tmpfile_linkable_at(ཧྲིལ dir_fd, const var_na *target, ཧྲིལ flags, var_na **ret_path) {
        ཧྲིལ r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ཧྲིལ fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        གལ་ཏེ (fd >= 0) {
                *ret_path = NULL;
                ཕྱིར fd;
        }

        གལ་ཏེ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        གལ་ཏེ (fd < 0)
                ཕྱིར -errno;

        *ret_path = TAKE_PTR(tmp);

        ཕྱིར fd;
}

ཧྲིལ fopen_tmpfile_linkable_at(ཧྲིལ dir_fd, const var_na *target, ཧྲིལ flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        གལ་ཏེ (fd < 0)
                ཕྱིར fd;

        f = take_fdopen(&fd, "w");
        གལ་ཏེ (!f)
                ཕྱིར -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ཕྱིར 0;
}

ཧྲིལ link_tmpfile_at(ཧྲིལ fd, ཧྲིལ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ཧྲིལ r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        གལ་ཏེ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ཕྱིར -errno;

        གལ་ཏེ (path) {
                གལ་ཏེ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                དེ་མིན
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } དེ་མིན {
                གལ་ཏེ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                དེ་མིན
                        r = link_fd(fd, dir_fd, target);
        }
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        ཕྱིར 0;
}

ཧྲིལ flink_tmpfile_at(FILE *f, ཧྲིལ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ཧྲིལ fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        གལ་ཏེ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ཕྱིར -EBADF;

        r = fflush_and_check(f);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར link_tmpfile_at(fd, dir_fd, path, target, flags);
}

ཧྲིལ mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(ret);

        གལ་ཏེ (template)
                p = strdup(template);
        དེ་མིན {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                p = path_join(tmp, "XXXXXX");
        }
        གལ་ཏེ (!p)
                ཕྱིར -ENOMEM;

        གལ་ཏེ (!mkdtemp(p))
                ཕྱིར -errno;

        *ret = TAKE_PTR(p);
        ཕྱིར 0;
}

ཧྲིལ mkdtemp_open(const var_na *template, ཧྲིལ flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ fd, r;

        r = mkdtemp_malloc(template, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        གལ་ཏེ (fd < 0) {
                (void) rmdir(p);
                ཕྱིར fd;
        }

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(p);

        ཕྱིར fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        གལ་ཏེ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ཕྱིར;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
