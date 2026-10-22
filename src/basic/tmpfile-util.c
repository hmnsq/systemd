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
        𐡄𐡍 (fd < 0)
                أرجع -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        𐡄𐡍 (r < 0) {
                (فراغ) unlinkat(dir_fd, path, 0);
                أرجع r;
        }

        𐡄𐡍 (ret_file)
                *ret_file = TAKE_PTR(f);

        أرجع 0;
}

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *path, FILE **ret_file, حرف **ret_path) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        r = tempfn_random(path, عدم, &t);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (ret_path)
                *ret_path = TAKE_PTR(t);

        أرجع 0;
}

صحيح fopen_temporary_child_at(صحيح dir_fd, ثابت حرف *path, FILE **ret_file, حرف **ret_path) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        𐡄𐡍 (!path) {
                r = tmp_dir(&path);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        r = tempfn_random_child(path, عدم, &t);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (ret_path)
                *ret_path = TAKE_PTR(t);

        أرجع 0;
}

/* This is much like mkostemp() but is subject to umask(). */
صحيح mkostemp_safe(حرف *pattern) {
        أكد(pattern);
        BLOCK_WITH_UMASK(0077);
        أرجع RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

صحيح fmkostemp_safe(حرف *pattern, ثابت حرف *mode, FILE **ret_f) {
        _cleanup_close_ صحيح fd = -EBADF;
        FILE *f;

        أكد(ret_f);

        fd = mkostemp_safe(pattern);
        𐡄𐡍 (fd < 0)
                أرجع fd;

        f = take_fdopen(&fd, mode);
        𐡄𐡍 (!f)
                أرجع -errno;

        *ret_f = f;
        أرجع 0;
}

فراغ unlink_tempfilep(حرف (*p)[]) {
        أكد(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        𐡄𐡍 (!endswith(*p, ".XXXXXX"))
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

        𐡄𐡍 (pre && strchr(pre, '/'))
                أرجع -EINVAL;

        𐡄𐡍 (post && strchr(post, '/'))
                أرجع -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        𐡄𐡍 (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                أرجع -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        𐡄𐡍 (child) {
                d = strdup(p);
                𐡄𐡍 (!d)
                        أرجع -ENOMEM;
        } وإلا {
                r = path_split_prefix_filename(p, &d, &fn);
                𐡄𐡍 (r < 0)
                        أرجع r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        𐡄𐡍 (!nf)
                أرجع -ENOMEM;

        𐡄𐡍 (d) {
                𐡄𐡍 (!path_extend(&d, nf))
                        أرجع -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } وإلا
                result = TAKE_PTR(nf);

        𐡄𐡍 (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                أرجع -EINVAL;

        *ret = TAKE_PTR(result);
        أرجع 0;
}

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        أرجع tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
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

        𐡄𐡍 (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                أرجع -ENOMEM;

        أرجع tempfn_build(p, extra, s, /* child= */ false, ret);
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

        𐡄𐡍 (!p) {
                r = tmp_dir(&p);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        𐡄𐡍 (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                أرجع -ENOMEM;

        أرجع tempfn_build(p, extra, s, /* child= */ true, ret);
}

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags) {
        صحيح r;

        𐡄𐡍 (!directory) {
                r = tmp_dir(&directory);
                𐡄𐡍 (r < 0)
                        أرجع r;
        } وإلا 𐡄𐡍 (isempty(directory))
                أرجع -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        صحيح fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        𐡄𐡍 (fd >= 0)
                أرجع fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ حرف *p = path_join(directory, "/systemd-tmp-XXXXXX");
        𐡄𐡍 (!p)
                أرجع -ENOMEM;

        fd = mkostemp_safe(p);
        𐡄𐡍 (fd < 0)
                أرجع fd;

        (فراغ) unlink(p);

        أرجع fd;
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
        𐡄𐡍 (fd >= 0) {
                *ret_path = عدم;
                أرجع fd;
        }

        𐡄𐡍 (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ حرف *tmp = عدم;
        r = tempfn_random(target, عدم, &tmp);
        𐡄𐡍 (r < 0)
                أرجع r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        𐡄𐡍 (fd < 0)
                أرجع -errno;

        *ret_path = TAKE_PTR(tmp);

        أرجع fd;
}

صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *target, صحيح flags, حرف **ret_path, FILE **ret_file) {
        _cleanup_free_ حرف *path = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(target);
        أكد(ret_file);
        أكد(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        𐡄𐡍 (fd < 0)
                أرجع fd;

        f = take_fdopen(&fd, "w");
        𐡄𐡍 (!f)
                أرجع -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        أرجع 0;
}

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags) {
        صحيح r;

        أكد(fd >= 0);
        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        𐡄𐡍 (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                أرجع -errno;

        𐡄𐡍 (path) {
                𐡄𐡍 (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                وإلا
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } وإلا {
                𐡄𐡍 (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ عدم, dir_fd, target);
                وإلا
                        r = link_fd(fd, dir_fd, target);
        }
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags) {
        صحيح fd, r;

        أكد(f);
        أكد(target);

        fd = fileno(f);
        𐡄𐡍 (fd < 0) /* Not all FILE* objects encapsulate fds */
                أرجع -EBADF;

        r = fflush_and_check(f);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع link_tmpfile_at(fd, dir_fd, path, target, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *template, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(ret);

        𐡄𐡍 (template)
                p = strdup(template);
        وإلا {
                ثابت حرف *tmp;

                r = tmp_dir(&tmp);
                𐡄𐡍 (r < 0)
                        أرجع r;

                p = path_join(tmp, "XXXXXX");
        }
        𐡄𐡍 (!p)
                أرجع -ENOMEM;

        𐡄𐡍 (!mkdtemp(p))
                أرجع -errno;

        *ret = TAKE_PTR(p);
        أرجع 0;
}

صحيح mkdtemp_open(ثابت حرف *template, صحيح flags, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح fd, r;

        r = mkdtemp_malloc(template, &p);
        𐡄𐡍 (r < 0)
                أرجع r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        𐡄𐡍 (fd < 0) {
                (فراغ) rmdir(p);
                أرجع fd;
        }

        𐡄𐡍 (ret)
                *ret = TAKE_PTR(p);

        أرجع fd;
}

فراغ cleanup_tmpfile_data_done(بنية cleanup_tmpfile_data *d) {
        أكد(d);

        𐡄𐡍 (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                أرجع;

        PROTECT_ERRNO;

        (فراغ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = عدم;
        d->filename = عدم;
}
