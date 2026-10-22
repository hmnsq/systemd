/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "path-util.h"
#ضمن "random-util.h"
#ضمن "string-util.h"
#ضمن "sync-util.h"
#ضمن "tmpfile-util.h"
#ضمن "umask-util.h"

ساكن صحيح fopen_temporary_internal(صحيح dir_fd, ثابت حرف *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        אם (fd < 0)
                החזר -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        אם (r < 0) {
                (فراغ) unlinkat(dir_fd, path, 0);
                החזר r;
        }

        אם (ret_file)
                *ret_file = TAKE_PTR(f);

        החזר 0;
}

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *path, FILE **ret_file, حرف **ret_path) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        r = tempfn_random(path, عدم, &t);
        אם (r < 0)
                החזר r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        אם (r < 0)
                החזר r;

        אם (ret_path)
                *ret_path = TAKE_PTR(t);

        החזר 0;
}

صحيح fopen_temporary_child_at(صحيح dir_fd, ثابت حرف *path, FILE **ret_file, حرف **ret_path) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        אם (!path) {
                r = tmp_dir(&path);
                אם (r < 0)
                        החזר r;
        }

        r = tempfn_random_child(path, عدم, &t);
        אם (r < 0)
                החזר r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        אם (r < 0)
                החזר r;

        אם (ret_path)
                *ret_path = TAKE_PTR(t);

        החזר 0;
}

/* This is much like mkostemp() but is subject to umask(). */
صحيح mkostemp_safe(حرف *pattern) {
        أكد(pattern);
        BLOCK_WITH_UMASK(0077);
        החזר RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

صحيح fmkostemp_safe(حرف *pattern, ثابت حرف *mode, FILE **ret_f) {
        _cleanup_close_ صحيح fd = -EBADF;
        FILE *f;

        أكد(ret_f);

        fd = mkostemp_safe(pattern);
        אם (fd < 0)
                החזר fd;

        f = take_fdopen(&fd, mode);
        אם (!f)
                החזר -errno;

        *ret_f = f;
        החזר 0;
}

فراغ unlink_tempfilep(حرف (*p)[]) {
        أكد(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        אם (!endswith(*p, ".XXXXXX"))
                (فراغ) unlink(*p);
}

ساكن صحيح tempfn_build(ثابت حرف *p, ثابت حرف *pre, ثابت حرف *post, bool child, حرف **ret) {
        _cleanup_free_ حرف *d = عدم, *fn = عدم, *nf = عدم, *result = عدم;
        size_t len_pre, len_post, len_add;
        صحيح r;

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

        אם (pre && strchr(pre, '/'))
                החזר -EINVAL;

        אם (post && strchr(post, '/'))
                החזר -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        אם (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                החזר -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        אם (child) {
                d = strdup(p);
                אם (!d)
                        החזר -ENOMEM;
        } אחרת {
                r = path_split_prefix_filename(p, &d, &fn);
                אם (r < 0)
                        החזר r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        אם (!nf)
                החזר -ENOMEM;

        אם (d) {
                אם (!path_extend(&d, nf))
                        החזר -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } אחרת
                result = TAKE_PTR(nf);

        אם (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                החזר -EINVAL;

        *ret = TAKE_PTR(result);
        החזר 0;
}

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        החזר tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

صحيح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;

        أكد(p);
        أكد(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        אם (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                החזר -ENOMEM;

        החזר tempfn_build(p, extra, s, /* child= */ false, ret);
}

صحيح tempfn_random_child(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        אם (!p) {
                r = tmp_dir(&p);
                אם (r < 0)
                        החזר r;
        }

        אם (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                החזר -ENOMEM;

        החזר tempfn_build(p, extra, s, /* child= */ true, ret);
}

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags) {
        صحيح r;

        אם (!directory) {
                r = tmp_dir(&directory);
                אם (r < 0)
                        החזר r;
        } אחרת אם (isempty(directory))
                החזר -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        صحيح fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        אם (fd >= 0)
                החזר fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ حرف *p = path_join(directory, "/systemd-tmp-XXXXXX");
        אם (!p)
                החזר -ENOMEM;

        fd = mkostemp_safe(p);
        אם (fd < 0)
                החזר fd;

        (فراغ) unlink(p);

        החזר fd;
}

صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *target, صحيح flags, حرف **ret_path) {
        صحيح r;

        أكد(target);
        أكد(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        أكد((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        صحيح fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        אם (fd >= 0) {
                *ret_path = عدم;
                החזר fd;
        }

        אם (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ حرف *tmp = عدم;
        r = tempfn_random(target, عدم, &tmp);
        אם (r < 0)
                החזר r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        אם (fd < 0)
                החזר -errno;

        *ret_path = TAKE_PTR(tmp);

        החזר fd;
}

صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *target, صحيح flags, حرف **ret_path, FILE **ret_file) {
        _cleanup_free_ حرف *path = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(target);
        أكد(ret_file);
        أكد(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        אם (fd < 0)
                החזר fd;

        f = take_fdopen(&fd, "w");
        אם (!f)
                החזר -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        החזר 0;
}

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags) {
        صحيح r;

        أكد(fd >= 0);
        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        אם (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                החזר -errno;

        אם (path) {
                אם (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                אחרת
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } אחרת {
                אם (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ عدم, dir_fd, target);
                אחרת
                        r = link_fd(fd, dir_fd, target);
        }
        אם (r < 0)
                החזר r;

        אם (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                אם (r < 0)
                        החזר r;
        }

        החזר 0;
}

صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags) {
        صحيح fd, r;

        أكد(f);
        أكد(target);

        fd = fileno(f);
        אם (fd < 0) /* Not all FILE* objects encapsulate fds */
                החזר -EBADF;

        r = fflush_and_check(f);
        אם (r < 0)
                החזר r;

        החזר link_tmpfile_at(fd, dir_fd, path, target, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *template, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(ret);

        אם (template)
                p = strdup(template);
        אחרת {
                ثابت حرف *tmp;

                r = tmp_dir(&tmp);
                אם (r < 0)
                        החזר r;

                p = path_join(tmp, "XXXXXX");
        }
        אם (!p)
                החזר -ENOMEM;

        אם (!mkdtemp(p))
                החזר -errno;

        *ret = TAKE_PTR(p);
        החזר 0;
}

صحيح mkdtemp_open(ثابت حرف *template, صحيح flags, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح fd, r;

        r = mkdtemp_malloc(template, &p);
        אם (r < 0)
                החזר r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        אם (fd < 0) {
                (فراغ) rmdir(p);
                החזר fd;
        }

        אם (ret)
                *ret = TAKE_PTR(p);

        החזר fd;
}

فراغ cleanup_tmpfile_data_done(بنية cleanup_tmpfile_data *d) {
        أكد(d);

        אם (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                החזר;

        PROTECT_ERRNO;

        (فراغ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = عدم;
        d->filename = عدم;
}
