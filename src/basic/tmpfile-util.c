/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "sync-util.h"
#ಅನ್ತರ್ಭಾವ "tmpfile-util.h"
#ಅನ್ತರ್ಭಾವ "umask-util.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fopen_temporary_internal(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ಯದಿ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(dir_fd, path, 0);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_file)
                *ret_file = TAKE_PTR(f);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fopen_temporary_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file, ವರ್ಣ **ret_path) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(path);

        r = tempfn_random(path, ರಿಕ್ತ, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(t);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fopen_temporary_child_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file, ವರ್ಣ **ret_path) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ಯದಿ (!path) {
                r = tmp_dir(&path);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        r = tempfn_random_child(path, ರಿಕ್ತ, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(t);

        ಪ್ರತ್ಯಾಗಮ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ಪೂರ್ಣಾಙ್ಕ mkostemp_safe(ವರ್ಣ *pattern) {
        ಪ್ರತಿಜ್ಞಾ(pattern);
        BLOCK_WITH_UMASK(0077);
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

ಪೂರ್ಣಾಙ್ಕ fmkostemp_safe(ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *mode, FILE **ret_f) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        FILE *f;

        ಪ್ರತಿಜ್ಞಾ(ret_f);

        fd = mkostemp_safe(pattern);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        f = take_fdopen(&fd, mode);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret_f = f;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ unlink_tempfilep(ವರ್ಣ (*p)[]) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ಯದಿ (!endswith(*p, ".XXXXXX"))
                (ಶೂನ್ಯ) unlink(*p);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tempfn_build(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *pre, ಧ್ರುವ ವರ್ಣ *post, bool child, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *d = ರಿಕ್ತ, *fn = ರಿಕ್ತ, *nf = ರಿಕ್ತ, *result = ರಿಕ್ತ;
        size_t len_pre, len_post, len_add;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        ಯದಿ (pre && strchr(pre, '/'))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (post && strchr(post, '/'))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ಯದಿ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ಯದಿ (child) {
                d = strdup(p);
                ಯದಿ (!d)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ {
                r = path_split_prefix_filename(p, &d, &fn);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ಯದಿ (!nf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (d) {
                ಯದಿ (!path_extend(&d, nf))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } ಅನ್ಯಥಾ
                result = TAKE_PTR(nf);

        ಯದಿ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(result);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ tempfn_xxxxxx(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *extra, ವರ್ಣ **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ಪ್ರತ್ಯಾಗಮ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ಪೂರ್ಣಾಙ್ಕ tempfn_random(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *extra, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        ಯದಿ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ tempfn_build(p, extra, s, /* child= */ false, ret);
}

ಪೂರ್ಣಾಙ್ಕ tempfn_random_child(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *extra, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ಯದಿ (!p) {
                r = tmp_dir(&p);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ tempfn_build(p, extra, s, /* child= */ true, ret);
}

ಪೂರ್ಣಾಙ್ಕ open_tmpfile_unlinkable(ಧ್ರುವ ವರ್ಣ *directory, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!directory) {
                r = tmp_dir(&directory);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        } ಅನ್ಯಥಾ ಯದಿ (isempty(directory))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ಪೂರ್ಣಾಙ್ಕ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ಯದಿ (fd >= 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ ವರ್ಣ *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        fd = mkostemp_safe(p);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        (ಶೂನ್ಯ) unlink(p);

        ಪ್ರತ್ಯಾಗಮ fd;
}

ಪೂರ್ಣಾಙ್ಕ open_tmpfile_linkable_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_path) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(target);
        ಪ್ರತಿಜ್ಞಾ(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        ಪ್ರತಿಜ್ಞಾ((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ಪೂರ್ಣಾಙ್ಕ fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        ಯದಿ (fd >= 0) {
                *ret_path = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ fd;
        }

        ಯದಿ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ ವರ್ಣ *tmp = ರಿಕ್ತ;
        r = tempfn_random(target, ರಿಕ್ತ, &tmp);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret_path = TAKE_PTR(tmp);

        ಪ್ರತ್ಯಾಗಮ fd;
}

ಪೂರ್ಣಾಙ್ಕ fopen_tmpfile_linkable_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_path, FILE **ret_file) {
        _cleanup_free_ ವರ್ಣ *path = ರಿಕ್ತ;
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(target);
        ಪ್ರತಿಜ್ಞಾ(ret_file);
        ಪ್ರತಿಜ್ಞಾ(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        f = take_fdopen(&fd, "w");
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ link_tmpfile_at(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *target, LinkTmpfileFlags flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ಯದಿ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (path) {
                ಯದಿ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                ಅನ್ಯಥಾ
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } ಅನ್ಯಥಾ {
                ಯದಿ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ರಿಕ್ತ, dir_fd, target);
                ಅನ್ಯಥಾ
                        r = link_fd(fd, dir_fd, target);
        }
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ flink_tmpfile_at(FILE *f, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *target, LinkTmpfileFlags flags) {
        ಪೂರ್ಣಾಙ್ಕ fd, r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(target);

        fd = fileno(f);
        ಯದಿ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        r = fflush_and_check(f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

ಪೂರ್ಣಾಙ್ಕ mkdtemp_malloc(ಧ್ರುವ ವರ್ಣ *template, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (template)
                p = strdup(template);
        ಅನ್ಯಥಾ {
                ಧ್ರುವ ವರ್ಣ *tmp;

                r = tmp_dir(&tmp);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                p = path_join(tmp, "XXXXXX");
        }
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (!mkdtemp(p))
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = TAKE_PTR(p);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ mkdtemp_open(ಧ್ರುವ ವರ್ಣ *template, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ fd, r;

        r = mkdtemp_malloc(template, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ಯದಿ (fd < 0) {
                (ಶೂನ್ಯ) rmdir(p);
                ಪ್ರತ್ಯಾಗಮ fd;
        }

        ಯದಿ (ret)
                *ret = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ fd;
}

ಶೂನ್ಯ cleanup_tmpfile_data_done(ಸಂರಚನಾ cleanup_tmpfile_data *d) {
        ಪ್ರತಿಜ್ಞಾ(d);

        ಯದಿ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ಪ್ರತ್ಯಾಗಮ;

        PROTECT_ERRNO;

        (ಶೂನ್ಯ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ರಿಕ್ತ;
        d->filename = ರಿಕ್ತ;
}
