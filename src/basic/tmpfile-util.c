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

static ចំនួនគត់ fopen_temporary_internal(ចំនួនគត់ dir_fd, const var_na *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        ចំនួនគត់ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ប្រសិនបើ (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_file)
                *ret_file = TAKE_PTR(f);

        ត្រឡប់ 0;
}

ចំនួនគត់ fopen_temporary_at(ចំនួនគត់ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        ចំនួនគត់ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (ret_path)
                *ret_path = TAKE_PTR(t);

        ត្រឡប់ 0;
}

ចំនួនគត់ fopen_temporary_child_at(ចំនួនគត់ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        ចំនួនគត់ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ប្រសិនបើ (!path) {
                r = tmp_dir(&path);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        r = tempfn_random_child(path, NULL, &t);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (ret_path)
                *ret_path = TAKE_PTR(t);

        ត្រឡប់ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ចំនួនគត់ mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        ត្រឡប់ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

ចំនួនគត់ fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd;

        f = take_fdopen(&fd, mode);
        ប្រសិនបើ (!f)
                ត្រឡប់ -errno;

        *ret_f = f;
        ត្រឡប់ 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ប្រសិនបើ (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static ចំនួនគត់ tempfn_build(const var_na *p, const var_na *pre, const var_na *post, bool child, var_na **ret) {
        _cleanup_free_ var_na *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        ចំនួនគត់ r;

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

        ប្រសិនបើ (pre && strchr(pre, '/'))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (post && strchr(post, '/'))
                ត្រឡប់ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ប្រសិនបើ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ត្រឡប់ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ប្រសិនបើ (child) {
                d = strdup(p);
                ប្រសិនបើ (!d)
                        ត្រឡប់ -ENOMEM;
        } បើមិនដូច្នេះ {
                r = path_split_prefix_filename(p, &d, &fn);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ប្រសិនបើ (!nf)
                ត្រឡប់ -ENOMEM;

        ប្រសិនបើ (d) {
                ប្រសិនបើ (!path_extend(&d, nf))
                        ត្រឡប់ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } បើមិនដូច្នេះ
                result = TAKE_PTR(nf);

        ប្រសិនបើ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ត្រឡប់ -EINVAL;

        *ret = TAKE_PTR(result);
        ត្រឡប់ 0;
}

ចំនួនគត់ tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ត្រឡប់ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ចំនួនគត់ tempfn_random(const var_na *p, const var_na *extra, var_na **ret) {
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

        ប្រសិនបើ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ tempfn_build(p, extra, s, /* child= */ false, ret);
}

ចំនួនគត់ tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        ចំនួនគត់ r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ប្រសិនបើ (!p) {
                r = tmp_dir(&p);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ tempfn_build(p, extra, s, /* child= */ true, ret);
}

ចំនួនគត់ open_tmpfile_unlinkable(const var_na *directory, ចំនួនគត់ flags) {
        ចំនួនគត់ r;

        ប្រសិនបើ (!directory) {
                r = tmp_dir(&directory);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        } បើមិនដូច្នេះ ប្រសិនបើ (isempty(directory))
                ត្រឡប់ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ចំនួនគត់ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ប្រសិនបើ (fd >= 0)
                ត្រឡប់ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ប្រសិនបើ (!p)
                ត្រឡប់ -ENOMEM;

        fd = mkostemp_safe(p);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd;

        (void) unlink(p);

        ត្រឡប់ fd;
}

ចំនួនគត់ open_tmpfile_linkable_at(ចំនួនគត់ dir_fd, const var_na *target, ចំនួនគត់ flags, var_na **ret_path) {
        ចំនួនគត់ r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ចំនួនគត់ fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        ប្រសិនបើ (fd >= 0) {
                *ret_path = NULL;
                ត្រឡប់ fd;
        }

        ប្រសិនបើ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        *ret_path = TAKE_PTR(tmp);

        ត្រឡប់ fd;
}

ចំនួនគត់ fopen_tmpfile_linkable_at(ចំនួនគត់ dir_fd, const var_na *target, ចំនួនគត់ flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd;

        f = take_fdopen(&fd, "w");
        ប្រសិនបើ (!f)
                ត្រឡប់ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ត្រឡប់ 0;
}

ចំនួនគត់ link_tmpfile_at(ចំនួនគត់ fd, ចំនួនគត់ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ចំនួនគត់ r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ប្រសិនបើ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (path) {
                ប្រសិនបើ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                បើមិនដូច្នេះ
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } បើមិនដូច្នេះ {
                ប្រសិនបើ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                បើមិនដូច្នេះ
                        r = link_fd(fd, dir_fd, target);
        }
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ flink_tmpfile_at(FILE *f, ចំនួនគត់ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ចំនួនគត់ fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        ប្រសិនបើ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ត្រឡប់ -EBADF;

        r = fflush_and_check(f);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

ចំនួនគត់ mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ចំនួនគត់ r;

        assert(ret);

        ប្រសិនបើ (template)
                p = strdup(template);
        បើមិនដូច្នេះ {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                p = path_join(tmp, "XXXXXX");
        }
        ប្រសិនបើ (!p)
                ត្រឡប់ -ENOMEM;

        ប្រសិនបើ (!mkdtemp(p))
                ត្រឡប់ -errno;

        *ret = TAKE_PTR(p);
        ត្រឡប់ 0;
}

ចំនួនគត់ mkdtemp_open(const var_na *template, ចំនួនគត់ flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ចំនួនគត់ fd, r;

        r = mkdtemp_malloc(template, &p);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ប្រសិនបើ (fd < 0) {
                (void) rmdir(p);
                ត្រឡប់ fd;
        }

        ប្រសិនបើ (ret)
                *ret = TAKE_PTR(p);

        ត្រឡប់ fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        ប្រសិនបើ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ត្រឡប់;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
