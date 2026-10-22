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
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ᰀᰧ (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                ᰀᰧᰌ r;
        }

        ᰀᰧ (ret_file)
                *ret_file = TAKE_PTR(f);

        ᰀᰧᰌ 0;
}

puur_naa_nka fopen_temporary_at(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (ret_path)
                *ret_path = TAKE_PTR(t);

        ᰀᰧᰌ 0;
}

puur_naa_nka fopen_temporary_child_at(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ᰀᰧ (!path) {
                r = tmp_dir(&path);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
        }

        r = tempfn_random_child(path, NULL, &t);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (ret_path)
                *ret_path = TAKE_PTR(t);

        ᰀᰧᰌ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
puur_naa_nka mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        ᰀᰧᰌ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

puur_naa_nka fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ fd;

        f = take_fdopen(&fd, mode);
        ᰀᰧ (!f)
                ᰀᰧᰌ -errno;

        *ret_f = f;
        ᰀᰧᰌ 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ᰀᰧ (!endswith(*p, ".XXXXXX"))
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

        ᰀᰧ (pre && strchr(pre, '/'))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (post && strchr(post, '/'))
                ᰀᰧᰌ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ᰀᰧ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ᰀᰧᰌ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ᰀᰧ (child) {
                d = strdup(p);
                ᰀᰧ (!d)
                        ᰀᰧᰌ -ENOMEM;
        } ᰀᰧᰀ {
                r = path_split_prefix_filename(p, &d, &fn);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ᰀᰧ (!nf)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧ (d) {
                ᰀᰧ (!path_extend(&d, nf))
                        ᰀᰧᰌ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } ᰀᰧᰀ
                result = TAKE_PTR(nf);

        ᰀᰧ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(result);
        ᰀᰧᰌ 0;
}

puur_naa_nka tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ᰀᰧᰌ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
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

        ᰀᰧ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧᰌ tempfn_build(p, extra, s, /* child= */ false, ret);
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

        ᰀᰧ (!p) {
                r = tmp_dir(&p);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
        }

        ᰀᰧ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧᰌ tempfn_build(p, extra, s, /* child= */ true, ret);
}

puur_naa_nka open_tmpfile_unlinkable(const var_na *directory, puur_naa_nka flags) {
        puur_naa_nka r;

        ᰀᰧ (!directory) {
                r = tmp_dir(&directory);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
        } ᰀᰧᰀ ᰀᰧ (isempty(directory))
                ᰀᰧᰌ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        puur_naa_nka fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ᰀᰧ (fd >= 0)
                ᰀᰧᰌ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ᰀᰧ (!p)
                ᰀᰧᰌ -ENOMEM;

        fd = mkostemp_safe(p);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ fd;

        (void) unlink(p);

        ᰀᰧᰌ fd;
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
        ᰀᰧ (fd >= 0) {
                *ret_path = NULL;
                ᰀᰧᰌ fd;
        }

        ᰀᰧ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -errno;

        *ret_path = TAKE_PTR(tmp);

        ᰀᰧᰌ fd;
}

puur_naa_nka fopen_tmpfile_linkable_at(puur_naa_nka dir_fd, const var_na *target, puur_naa_nka flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ fd;

        f = take_fdopen(&fd, "w");
        ᰀᰧ (!f)
                ᰀᰧᰌ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ᰀᰧᰌ 0;
}

puur_naa_nka link_tmpfile_at(puur_naa_nka fd, puur_naa_nka dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        puur_naa_nka r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ᰀᰧ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (path) {
                ᰀᰧ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                ᰀᰧᰀ
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } ᰀᰧᰀ {
                ᰀᰧ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                ᰀᰧᰀ
                        r = link_fd(fd, dir_fd, target);
        }
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
        }

        ᰀᰧᰌ 0;
}

puur_naa_nka flink_tmpfile_at(FILE *f, puur_naa_nka dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        puur_naa_nka fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        ᰀᰧ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ᰀᰧᰌ -EBADF;

        r = fflush_and_check(f);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

puur_naa_nka mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(ret);

        ᰀᰧ (template)
                p = strdup(template);
        ᰀᰧᰀ {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                p = path_join(tmp, "XXXXXX");
        }
        ᰀᰧ (!p)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧ (!mkdtemp(p))
                ᰀᰧᰌ -errno;

        *ret = TAKE_PTR(p);
        ᰀᰧᰌ 0;
}

puur_naa_nka mkdtemp_open(const var_na *template, puur_naa_nka flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka fd, r;

        r = mkdtemp_malloc(template, &p);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ᰀᰧ (fd < 0) {
                (void) rmdir(p);
                ᰀᰧᰌ fd;
        }

        ᰀᰧ (ret)
                *ret = TAKE_PTR(p);

        ᰀᰧᰌ fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        ᰀᰧ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ᰀᰧᰌ;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
