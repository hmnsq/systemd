/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "path-util.h"
#شامل "random-util.h"
#شامل "string-util.h"
#شامل "sync-util.h"
#شامل "tmpfile-util.h"
#شامل "umask-util.h"

ایستا صحیح fopen_temporary_internal(صحیح dir_fd, ثابت نویسه *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        اگر (fd < 0)
                بازگشت -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        اگر (r < 0) {
                (تهی) unlinkat(dir_fd, path, 0);
                بازگشت r;
        }

        اگر (ret_file)
                *ret_file = TAKE_PTR(f);

        بازگشت 0;
}

صحیح fopen_temporary_at(صحیح dir_fd, ثابت نویسه *path, FILE **ret_file, نویسه **ret_path) {
        _cleanup_free_ نویسه *t = پوچ;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        r = tempfn_random(path, پوچ, &t);
        اگر (r < 0)
                بازگشت r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(t);

        بازگشت 0;
}

صحیح fopen_temporary_child_at(صحیح dir_fd, ثابت نویسه *path, FILE **ret_file, نویسه **ret_path) {
        _cleanup_free_ نویسه *t = پوچ;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        اگر (!path) {
                r = tmp_dir(&path);
                اگر (r < 0)
                        بازگشت r;
        }

        r = tempfn_random_child(path, پوچ, &t);
        اگر (r < 0)
                بازگشت r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path)
                *ret_path = TAKE_PTR(t);

        بازگشت 0;
}

/* This is much like mkostemp() but is subject to umask(). */
صحیح mkostemp_safe(نویسه *pattern) {
        أكد(pattern);
        BLOCK_WITH_UMASK(0077);
        بازگشت RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

صحیح fmkostemp_safe(نویسه *pattern, ثابت نویسه *mode, FILE **ret_f) {
        _cleanup_close_ صحیح fd = -EBADF;
        FILE *f;

        أكد(ret_f);

        fd = mkostemp_safe(pattern);
        اگر (fd < 0)
                بازگشت fd;

        f = take_fdopen(&fd, mode);
        اگر (!f)
                بازگشت -errno;

        *ret_f = f;
        بازگشت 0;
}

تهی unlink_tempfilep(نویسه (*p)[]) {
        أكد(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        اگر (!endswith(*p, ".XXXXXX"))
                (تهی) unlink(*p);
}

ایستا صحیح tempfn_build(ثابت نویسه *p, ثابت نویسه *pre, ثابت نویسه *post, bool child, نویسه **ret) {
        _cleanup_free_ نویسه *d = پوچ, *fn = پوچ, *nf = پوچ, *result = پوچ;
        size_t len_pre, len_post, len_add;
        صحیح r;

        أكد(p);
        أكد(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        اگر (pre && strchr(pre, '/'))
                بازگشت -EINVAL;

        اگر (post && strchr(post, '/'))
                بازگشت -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        اگر (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                بازگشت -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        اگر (child) {
                d = strdup(p);
                اگر (!d)
                        بازگشت -ENOMEM;
        } وگرنه {
                r = path_split_prefix_filename(p, &d, &fn);
                اگر (r < 0)
                        بازگشت r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        اگر (!nf)
                بازگشت -ENOMEM;

        اگر (d) {
                اگر (!path_extend(&d, nf))
                        بازگشت -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } وگرنه
                result = TAKE_PTR(nf);

        اگر (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                بازگشت -EINVAL;

        *ret = TAKE_PTR(result);
        بازگشت 0;
}

صحیح tempfn_xxxxxx(ثابت نویسه *p, ثابت نویسه *extra, نویسه **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        بازگشت tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

صحیح tempfn_random(ثابت نویسه *p, ثابت نویسه *extra, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;

        أكد(p);
        أكد(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        اگر (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                بازگشت -ENOMEM;

        بازگشت tempfn_build(p, extra, s, /* child= */ false, ret);
}

صحیح tempfn_random_child(ثابت نویسه *p, ثابت نویسه *extra, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        اگر (!p) {
                r = tmp_dir(&p);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                بازگشت -ENOMEM;

        بازگشت tempfn_build(p, extra, s, /* child= */ true, ret);
}

صحیح open_tmpfile_unlinkable(ثابت نویسه *directory, صحیح flags) {
        صحیح r;

        اگر (!directory) {
                r = tmp_dir(&directory);
                اگر (r < 0)
                        بازگشت r;
        } وگرنه اگر (isempty(directory))
                بازگشت -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        صحیح fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        اگر (fd >= 0)
                بازگشت fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ نویسه *p = path_join(directory, "/systemd-tmp-XXXXXX");
        اگر (!p)
                بازگشت -ENOMEM;

        fd = mkostemp_safe(p);
        اگر (fd < 0)
                بازگشت fd;

        (تهی) unlink(p);

        بازگشت fd;
}

صحیح open_tmpfile_linkable_at(صحیح dir_fd, ثابت نویسه *target, صحیح flags, نویسه **ret_path) {
        صحیح r;

        أكد(target);
        أكد(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        أكد((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        صحیح fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        اگر (fd >= 0) {
                *ret_path = پوچ;
                بازگشت fd;
        }

        اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ نویسه *tmp = پوچ;
        r = tempfn_random(target, پوچ, &tmp);
        اگر (r < 0)
                بازگشت r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        اگر (fd < 0)
                بازگشت -errno;

        *ret_path = TAKE_PTR(tmp);

        بازگشت fd;
}

صحیح fopen_tmpfile_linkable_at(صحیح dir_fd, ثابت نویسه *target, صحیح flags, نویسه **ret_path, FILE **ret_file) {
        _cleanup_free_ نویسه *path = پوچ;
        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(target);
        أكد(ret_file);
        أكد(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        اگر (fd < 0)
                بازگشت fd;

        f = take_fdopen(&fd, "w");
        اگر (!f)
                بازگشت -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        بازگشت 0;
}

صحیح link_tmpfile_at(صحیح fd, صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *target, LinkTmpfileFlags flags) {
        صحیح r;

        أكد(fd >= 0);
        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        اگر (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                بازگشت -errno;

        اگر (path) {
                اگر (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                وگرنه
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } وگرنه {
                اگر (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ پوچ, dir_fd, target);
                وگرنه
                        r = link_fd(fd, dir_fd, target);
        }
        اگر (r < 0)
                بازگشت r;

        اگر (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح flink_tmpfile_at(FILE *f, صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *target, LinkTmpfileFlags flags) {
        صحیح fd, r;

        أكد(f);
        أكد(target);

        fd = fileno(f);
        اگر (fd < 0) /* Not all FILE* objects encapsulate fds */
                بازگشت -EBADF;

        r = fflush_and_check(f);
        اگر (r < 0)
                بازگشت r;

        بازگشت link_tmpfile_at(fd, dir_fd, path, target, flags);
}

صحیح mkdtemp_malloc(ثابت نویسه *template, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(ret);

        اگر (template)
                p = strdup(template);
        وگرنه {
                ثابت نویسه *tmp;

                r = tmp_dir(&tmp);
                اگر (r < 0)
                        بازگشت r;

                p = path_join(tmp, "XXXXXX");
        }
        اگر (!p)
                بازگشت -ENOMEM;

        اگر (!mkdtemp(p))
                بازگشت -errno;

        *ret = TAKE_PTR(p);
        بازگشت 0;
}

صحیح mkdtemp_open(ثابت نویسه *template, صحیح flags, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ;
        صحیح fd, r;

        r = mkdtemp_malloc(template, &p);
        اگر (r < 0)
                بازگشت r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        اگر (fd < 0) {
                (تهی) rmdir(p);
                بازگشت fd;
        }

        اگر (ret)
                *ret = TAKE_PTR(p);

        بازگشت fd;
}

تهی cleanup_tmpfile_data_done(ساختار cleanup_tmpfile_data *d) {
        أكد(d);

        اگر (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                بازگشت;

        PROTECT_ERRNO;

        (تهی) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = پوچ;
        d->filename = پوچ;
}
