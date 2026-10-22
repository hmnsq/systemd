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

static පූර්ණ_සංඛ්‍යා fopen_temporary_internal(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        පූර්ණ_සංඛ්‍යා r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        නම් (fd < 0)
                ආපසු -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        නම් (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                ආපසු r;
        }

        නම් (ret_file)
                *ret_file = TAKE_PTR(f);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fopen_temporary_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        නම් (r < 0)
                ආපසු r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        නම් (r < 0)
                ආපසු r;

        නම් (ret_path)
                *ret_path = TAKE_PTR(t);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fopen_temporary_child_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        නම් (!path) {
                r = tmp_dir(&path);
                නම් (r < 0)
                        ආපසු r;
        }

        r = tempfn_random_child(path, NULL, &t);
        නම් (r < 0)
                ආපසු r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        නම් (r < 0)
                ආපසු r;

        නම් (ret_path)
                *ret_path = TAKE_PTR(t);

        ආපසු 0;
}

/* This is much like mkostemp() but is subject to umask(). */
පූර්ණ_සංඛ්‍යා mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        ආපසු RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

පූර්ණ_සංඛ්‍යා fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        නම් (fd < 0)
                ආපසු fd;

        f = take_fdopen(&fd, mode);
        නම් (!f)
                ආපසු -errno;

        *ret_f = f;
        ආපසු 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        නම් (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static පූර්ණ_සංඛ්‍යා tempfn_build(const var_na *p, const var_na *pre, const var_na *post, bool child, var_na **ret) {
        _cleanup_free_ var_na *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        පූර්ණ_සංඛ්‍යා r;

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

        නම් (pre && strchr(pre, '/'))
                ආපසු -EINVAL;

        නම් (post && strchr(post, '/'))
                ආපසු -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        නම් (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ආපසු -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        නම් (child) {
                d = strdup(p);
                නම් (!d)
                        ආපසු -ENOMEM;
        } නැතහොත් {
                r = path_split_prefix_filename(p, &d, &fn);
                නම් (r < 0)
                        ආපසු r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        නම් (!nf)
                ආපසු -ENOMEM;

        නම් (d) {
                නම් (!path_extend(&d, nf))
                        ආපසු -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } නැතහොත්
                result = TAKE_PTR(nf);

        නම් (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ආපසු -EINVAL;

        *ret = TAKE_PTR(result);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ආපසු tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

පූර්ණ_සංඛ්‍යා tempfn_random(const var_na *p, const var_na *extra, var_na **ret) {
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

        නම් (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ආපසු -ENOMEM;

        ආපසු tempfn_build(p, extra, s, /* child= */ false, ret);
}

පූර්ණ_සංඛ්‍යා tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        නම් (!p) {
                r = tmp_dir(&p);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ආපසු -ENOMEM;

        ආපසු tempfn_build(p, extra, s, /* child= */ true, ret);
}

පූර්ණ_සංඛ්‍යා open_tmpfile_unlinkable(const var_na *directory, පූර්ණ_සංඛ්‍යා flags) {
        පූර්ණ_සංඛ්‍යා r;

        නම් (!directory) {
                r = tmp_dir(&directory);
                නම් (r < 0)
                        ආපසු r;
        } නැතහොත් නම් (isempty(directory))
                ආපසු -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        පූර්ණ_සංඛ්‍යා fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        නම් (fd >= 0)
                ආපසු fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        නම් (!p)
                ආපසු -ENOMEM;

        fd = mkostemp_safe(p);
        නම් (fd < 0)
                ආපසු fd;

        (void) unlink(p);

        ආපසු fd;
}

පූර්ණ_සංඛ්‍යා open_tmpfile_linkable_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *target, පූර්ණ_සංඛ්‍යා flags, var_na **ret_path) {
        පූර්ණ_සංඛ්‍යා r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        පූර්ණ_සංඛ්‍යා fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        නම් (fd >= 0) {
                *ret_path = NULL;
                ආපසු fd;
        }

        නම් (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        නම් (r < 0)
                ආපසු r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        නම් (fd < 0)
                ආපසු -errno;

        *ret_path = TAKE_PTR(tmp);

        ආපසු fd;
}

පූර්ණ_සංඛ්‍යා fopen_tmpfile_linkable_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *target, පූර්ණ_සංඛ්‍යා flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        නම් (fd < 0)
                ආපසු fd;

        f = take_fdopen(&fd, "w");
        නම් (!f)
                ආපසු -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා link_tmpfile_at(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        නම් (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ආපසු -errno;

        නම් (path) {
                නම් (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                නැතහොත්
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } නැතහොත් {
                නම් (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                නැතහොත්
                        r = link_fd(fd, dir_fd, target);
        }
        නම් (r < 0)
                ආපසු r;

        නම් (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                නම් (r < 0)
                        ආපසු r;
        }

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා flink_tmpfile_at(FILE *f, පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        පූර්ණ_සංඛ්‍යා fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        නම් (fd < 0) /* Not all FILE* objects encapsulate fds */
                ආපසු -EBADF;

        r = fflush_and_check(f);
        නම් (r < 0)
                ආපසු r;

        ආපසු link_tmpfile_at(fd, dir_fd, path, target, flags);
}

පූර්ණ_සංඛ්‍යා mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        නම් (template)
                p = strdup(template);
        නැතහොත් {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                නම් (r < 0)
                        ආපසු r;

                p = path_join(tmp, "XXXXXX");
        }
        නම් (!p)
                ආපසු -ENOMEM;

        නම් (!mkdtemp(p))
                ආපසු -errno;

        *ret = TAKE_PTR(p);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා mkdtemp_open(const var_na *template, පූර්ණ_සංඛ්‍යා flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        පූර්ණ_සංඛ්‍යා fd, r;

        r = mkdtemp_malloc(template, &p);
        නම් (r < 0)
                ආපසු r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        නම් (fd < 0) {
                (void) rmdir(p);
                ආපසු fd;
        }

        නම් (ret)
                *ret = TAKE_PTR(p);

        ආපසු fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        නම් (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ආපසු;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
