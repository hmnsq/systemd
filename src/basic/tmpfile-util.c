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

static ຈຳນວນເຕັມ fopen_temporary_internal(ຈຳນວນເຕັມ dir_fd, const var_na *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF;
        ຈຳນວນເຕັມ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ຖ້າ (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                ສົ່ງຄືນ r;
        }

        ຖ້າ (ret_file)
                *ret_file = TAKE_PTR(f);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fopen_temporary_at(ຈຳນວນເຕັມ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        ຈຳນວນເຕັມ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (ret_path)
                *ret_path = TAKE_PTR(t);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fopen_temporary_child_at(ຈຳນວນເຕັມ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        ຈຳນວນເຕັມ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ຖ້າ (!path) {
                r = tmp_dir(&path);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        r = tempfn_random_child(path, NULL, &t);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (ret_path)
                *ret_path = TAKE_PTR(t);

        ສົ່ງຄືນ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ຈຳນວນເຕັມ mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        ສົ່ງຄືນ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

ຈຳນວນເຕັມ fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ fd;

        f = take_fdopen(&fd, mode);
        ຖ້າ (!f)
                ສົ່ງຄືນ -errno;

        *ret_f = f;
        ສົ່ງຄືນ 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ຖ້າ (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static ຈຳນວນເຕັມ tempfn_build(const var_na *p, const var_na *pre, const var_na *post, bool child, var_na **ret) {
        _cleanup_free_ var_na *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        ຈຳນວນເຕັມ r;

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

        ຖ້າ (pre && strchr(pre, '/'))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (post && strchr(post, '/'))
                ສົ່ງຄືນ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ຖ້າ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ສົ່ງຄືນ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ຖ້າ (child) {
                d = strdup(p);
                ຖ້າ (!d)
                        ສົ່ງຄືນ -ENOMEM;
        } ບໍ່ດັ່ງນັ້ນ {
                r = path_split_prefix_filename(p, &d, &fn);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ຖ້າ (!nf)
                ສົ່ງຄືນ -ENOMEM;

        ຖ້າ (d) {
                ຖ້າ (!path_extend(&d, nf))
                        ສົ່ງຄືນ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } ບໍ່ດັ່ງນັ້ນ
                result = TAKE_PTR(nf);

        ຖ້າ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(result);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ສົ່ງຄືນ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ຈຳນວນເຕັມ tempfn_random(const var_na *p, const var_na *extra, var_na **ret) {
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

        ຖ້າ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ tempfn_build(p, extra, s, /* child= */ false, ret);
}

ຈຳນວນເຕັມ tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        ຈຳນວນເຕັມ r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ຖ້າ (!p) {
                r = tmp_dir(&p);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ tempfn_build(p, extra, s, /* child= */ true, ret);
}

ຈຳນວນເຕັມ open_tmpfile_unlinkable(const var_na *directory, ຈຳນວນເຕັມ flags) {
        ຈຳນວນເຕັມ r;

        ຖ້າ (!directory) {
                r = tmp_dir(&directory);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (isempty(directory))
                ສົ່ງຄືນ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ຈຳນວນເຕັມ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ຖ້າ (fd >= 0)
                ສົ່ງຄືນ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ຖ້າ (!p)
                ສົ່ງຄືນ -ENOMEM;

        fd = mkostemp_safe(p);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ fd;

        (void) unlink(p);

        ສົ່ງຄືນ fd;
}

ຈຳນວນເຕັມ open_tmpfile_linkable_at(ຈຳນວນເຕັມ dir_fd, const var_na *target, ຈຳນວນເຕັມ flags, var_na **ret_path) {
        ຈຳນວນເຕັມ r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ຈຳນວນເຕັມ fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        ຖ້າ (fd >= 0) {
                *ret_path = NULL;
                ສົ່ງຄືນ fd;
        }

        ຖ້າ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ -errno;

        *ret_path = TAKE_PTR(tmp);

        ສົ່ງຄືນ fd;
}

ຈຳນວນເຕັມ fopen_tmpfile_linkable_at(ຈຳນວນເຕັມ dir_fd, const var_na *target, ຈຳນວນເຕັມ flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ fd;

        f = take_fdopen(&fd, "w");
        ຖ້າ (!f)
                ສົ່ງຄືນ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ link_tmpfile_at(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ຖ້າ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ສົ່ງຄືນ -errno;

        ຖ້າ (path) {
                ຖ້າ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                ບໍ່ດັ່ງນັ້ນ
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } ບໍ່ດັ່ງນັ້ນ {
                ຖ້າ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                ບໍ່ດັ່ງນັ້ນ
                        r = link_fd(fd, dir_fd, target);
        }
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ flink_tmpfile_at(FILE *f, ຈຳນວນເຕັມ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ຈຳນວນເຕັມ fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        ຖ້າ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ສົ່ງຄືນ -EBADF;

        r = fflush_and_check(f);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

ຈຳນວນເຕັມ mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ຈຳນວນເຕັມ r;

        assert(ret);

        ຖ້າ (template)
                p = strdup(template);
        ບໍ່ດັ່ງນັ້ນ {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                p = path_join(tmp, "XXXXXX");
        }
        ຖ້າ (!p)
                ສົ່ງຄືນ -ENOMEM;

        ຖ້າ (!mkdtemp(p))
                ສົ່ງຄືນ -errno;

        *ret = TAKE_PTR(p);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ mkdtemp_open(const var_na *template, ຈຳນວນເຕັມ flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ຈຳນວນເຕັມ fd, r;

        r = mkdtemp_malloc(template, &p);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ຖ້າ (fd < 0) {
                (void) rmdir(p);
                ສົ່ງຄືນ fd;
        }

        ຖ້າ (ret)
                *ret = TAKE_PTR(p);

        ສົ່ງຄືນ fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        ຖ້າ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ສົ່ງຄືນ;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
