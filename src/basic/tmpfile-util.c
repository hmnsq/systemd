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

static puur_naa_nka fopen_temporary_internal(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ꦪꦼꦤ꧀ (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                ꦮꦭꦶ r;
        }

        ꦪꦼꦤ꧀ (ret_file)
                *ret_file = TAKE_PTR(f);

        ꦮꦭꦶ 0;
}

puur_naa_nka fopen_temporary_at(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret_path)
                *ret_path = TAKE_PTR(t);

        ꦮꦭꦶ 0;
}

puur_naa_nka fopen_temporary_child_at(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ꦪꦼꦤ꧀ (!path) {
                r = tmp_dir(&path);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        r = tempfn_random_child(path, NULL, &t);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (ret_path)
                *ret_path = TAKE_PTR(t);

        ꦮꦭꦶ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
puur_naa_nka mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        ꦮꦭꦶ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

puur_naa_nka fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ fd;

        f = take_fdopen(&fd, mode);
        ꦪꦼꦤ꧀ (!f)
                ꦮꦭꦶ -errno;

        *ret_f = f;
        ꦮꦭꦶ 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ꦪꦼꦤ꧀ (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static puur_naa_nka tempfn_build(const var_na *p, const var_na *pre, const var_na *post, bool child, var_na **ret) {
        _cleanup_free_ var_na *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        puur_naa_nka r;

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

        ꦪꦼꦤ꧀ (pre && strchr(pre, '/'))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (post && strchr(post, '/'))
                ꦮꦭꦶ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ꦪꦼꦤ꧀ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ꦮꦭꦶ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ꦪꦼꦤ꧀ (child) {
                d = strdup(p);
                ꦪꦼꦤ꧀ (!d)
                        ꦮꦭꦶ -ENOMEM;
        } ꦭꦶꦪ {
                r = path_split_prefix_filename(p, &d, &fn);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ꦪꦼꦤ꧀ (!nf)
                ꦮꦭꦶ -ENOMEM;

        ꦪꦼꦤ꧀ (d) {
                ꦪꦼꦤ꧀ (!path_extend(&d, nf))
                        ꦮꦭꦶ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } ꦭꦶꦪ
                result = TAKE_PTR(nf);

        ꦪꦼꦤ꧀ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(result);
        ꦮꦭꦶ 0;
}

puur_naa_nka tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ꦮꦭꦶ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

puur_naa_nka tempfn_random(const var_na *p, const var_na *extra, var_na **ret) {
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

        ꦪꦼꦤ꧀ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ꦮꦭꦶ -ENOMEM;

        ꦮꦭꦶ tempfn_build(p, extra, s, /* child= */ false, ret);
}

puur_naa_nka tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ꦪꦼꦤ꧀ (!p) {
                r = tmp_dir(&p);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        ꦪꦼꦤ꧀ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ꦮꦭꦶ -ENOMEM;

        ꦮꦭꦶ tempfn_build(p, extra, s, /* child= */ true, ret);
}

puur_naa_nka open_tmpfile_unlinkable(const var_na *directory, puur_naa_nka flags) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (!directory) {
                r = tmp_dir(&directory);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (isempty(directory))
                ꦮꦭꦶ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        puur_naa_nka fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ꦪꦼꦤ꧀ (fd >= 0)
                ꦮꦭꦶ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ -ENOMEM;

        fd = mkostemp_safe(p);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ fd;

        (void) unlink(p);

        ꦮꦭꦶ fd;
}

puur_naa_nka open_tmpfile_linkable_at(puur_naa_nka dir_fd, const var_na *target, puur_naa_nka flags, var_na **ret_path) {
        puur_naa_nka r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        puur_naa_nka fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        ꦪꦼꦤ꧀ (fd >= 0) {
                *ret_path = NULL;
                ꦮꦭꦶ fd;
        }

        ꦪꦼꦤ꧀ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ -errno;

        *ret_path = TAKE_PTR(tmp);

        ꦮꦭꦶ fd;
}

puur_naa_nka fopen_tmpfile_linkable_at(puur_naa_nka dir_fd, const var_na *target, puur_naa_nka flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ fd;

        f = take_fdopen(&fd, "w");
        ꦪꦼꦤ꧀ (!f)
                ꦮꦭꦶ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ꦮꦭꦶ 0;
}

puur_naa_nka link_tmpfile_at(puur_naa_nka fd, puur_naa_nka dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        puur_naa_nka r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ꦪꦼꦤ꧀ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ꦮꦭꦶ -errno;

        ꦪꦼꦤ꧀ (path) {
                ꦪꦼꦤ꧀ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                ꦭꦶꦪ
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } ꦭꦶꦪ {
                ꦪꦼꦤ꧀ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                ꦭꦶꦪ
                        r = link_fd(fd, dir_fd, target);
        }
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka flink_tmpfile_at(FILE *f, puur_naa_nka dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        puur_naa_nka fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        ꦪꦼꦤ꧀ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ꦮꦭꦶ -EBADF;

        r = fflush_and_check(f);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

puur_naa_nka mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(ret);

        ꦪꦼꦤ꧀ (template)
                p = strdup(template);
        ꦭꦶꦪ {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                p = path_join(tmp, "XXXXXX");
        }
        ꦪꦼꦤ꧀ (!p)
                ꦮꦭꦶ -ENOMEM;

        ꦪꦼꦤ꧀ (!mkdtemp(p))
                ꦮꦭꦶ -errno;

        *ret = TAKE_PTR(p);
        ꦮꦭꦶ 0;
}

puur_naa_nka mkdtemp_open(const var_na *template, puur_naa_nka flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka fd, r;

        r = mkdtemp_malloc(template, &p);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ꦪꦼꦤ꧀ (fd < 0) {
                (void) rmdir(p);
                ꦮꦭꦶ fd;
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(p);

        ꦮꦭꦶ fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        ꦪꦼꦤ꧀ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ꦮꦭꦶ;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
