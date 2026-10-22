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

ساكن صحيح fopen_temporary_داخلی(صحيح dir_fd, ثابت حرف *مسیر, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(مسیر);

        fd = openat(dir_fd, مسیر, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        إذا (fd < 0)
                أرجع -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        إذا (r < 0) {
                (فراغ) unlinkat(dir_fd, مسیر, 0);
                أرجع r;
        }

        إذا (ret_file)
                *ret_file = TAKE_PTR(f);

        أرجع 0;
}

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *مسیر, FILE **ret_file, حرف **ret_مسیر) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(مسیر);

        r = tempfn_random(مسیر, عدم, &t);
        إذا (r < 0)
                أرجع r;

        r = fopen_temporary_داخلی(dir_fd, t, ret_file);
        إذا (r < 0)
                أرجع r;

        إذا (ret_مسیر)
                *ret_مسیر = TAKE_PTR(t);

        أرجع 0;
}

صحيح fopen_temporary_فرزند_at(صحيح dir_fd, ثابت حرف *مسیر, FILE **ret_file, حرف **ret_مسیر) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        إذا (!مسیر) {
                r = tmp_dir(&مسیر);
                إذا (r < 0)
                        أرجع r;
        }

        r = tempfn_random_فرزند(مسیر, عدم, &t);
        إذا (r < 0)
                أرجع r;

        r = fopen_temporary_داخلی(dir_fd, t, ret_file);
        إذا (r < 0)
                أرجع r;

        إذا (ret_مسیر)
                *ret_مسیر = TAKE_PTR(t);

        أرجع 0;
}

/* This is much like mkostemp() but is subject to umask(). */
صحيح mkostemp_امن(حرف *الگو) {
        أكد(الگو);
        بلوک_WITH_UMASK(0077);
        أرجع RET_NERRNO(mkostemp(الگو, O_CLOEXEC));
}

صحيح fmkostemp_امن(حرف *الگو, ثابت حرف *نما, FILE **ret_f) {
        _cleanup_close_ صحيح fd = -EBADF;
        FILE *f;

        أكد(ret_f);

        fd = mkostemp_امن(الگو);
        إذا (fd < 0)
                أرجع fd;

        f = take_fdopen(&fd, نما);
        إذا (!f)
                أرجع -errno;

        *ret_f = f;
        أرجع 0;
}

فراغ unlink_tempfilep(حرف (*p)[]) {
        أكد(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        إذا (!endswith(*p, ".XXXXXX"))
                (فراغ) unlink(*p);
}

ساكن صحيح tempfn_build(ثابت حرف *p, ثابت حرف *pre, ثابت حرف *post, bool فرزند, حرف **ret) {
        _cleanup_free_ حرف *d = عدم, *fn = عدم, *nf = عدم, *نتیجه = عدم;
        size_t len_pre, len_post, len_جمع_کن;
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

        إذا (pre && strchr(pre, '/'))
                أرجع -EINVAL;

        إذا (post && strchr(post, '/'))
                أرجع -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        إذا (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                أرجع -EINVAL;

        len_جمع_کن = len_pre + len_post + STRLEN(".#");

        إذا (فرزند) {
                d = strdup(p);
                إذا (!d)
                        أرجع -ENOMEM;
        } وإلا {
                r = مسیر_تقسیم_پیشوند_filename(p, &d, &fn);
                إذا (r < 0)
                        أرجع r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_جمع_کن);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        إذا (!nf)
                أرجع -ENOMEM;

        إذا (d) {
                إذا (!مسیر_extend(&d, nf))
                        أرجع -ENOMEM;

                نتیجه = مسیر_simplify(TAKE_PTR(d));
        } وإلا
                نتیجه = TAKE_PTR(nf);

        إذا (!مسیر_is_valid(نتیجه)) /* New path is not valid? (Maybe because too long?) Refuse. */
                أرجع -EINVAL;

        *ret = TAKE_PTR(نتیجه);
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

        إذا (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                أرجع -ENOMEM;

        أرجع tempfn_build(p, extra, s, /* child= */ false, ret);
}

صحيح tempfn_random_فرزند(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        إذا (!p) {
                r = tmp_dir(&p);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                أرجع -ENOMEM;

        أرجع tempfn_build(p, extra, s, /* child= */ true, ret);
}

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags) {
        صحيح r;

        إذا (!directory) {
                r = tmp_dir(&directory);
                إذا (r < 0)
                        أرجع r;
        } وإلا إذا (isempty(directory))
                أرجع -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        صحيح fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        إذا (fd >= 0)
                أرجع fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ حرف *p = مسیر_join(directory, "/systemd-tmp-XXXXXX");
        إذا (!p)
                أرجع -ENOMEM;

        fd = mkostemp_امن(p);
        إذا (fd < 0)
                أرجع fd;

        (فراغ) unlink(p);

        أرجع fd;
}

صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *هدف, صحيح flags, حرف **ret_مسیر) {
        صحيح r;

        أكد(هدف);
        أكد(ret_مسیر);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        أكد((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        صحيح fd = open_والد_at(dir_fd, هدف, O_TMPFILE|flags, 0640);
        إذا (fd >= 0) {
                *ret_مسیر = عدم;
                أرجع fd;
        }

        إذا (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", هدف);

        _cleanup_free_ حرف *tmp = عدم;
        r = tempfn_random(هدف, عدم, &tmp);
        إذا (r < 0)
                أرجع r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        إذا (fd < 0)
                أرجع -errno;

        *ret_مسیر = TAKE_PTR(tmp);

        أرجع fd;
}

صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *هدف, صحيح flags, حرف **ret_مسیر, FILE **ret_file) {
        _cleanup_free_ حرف *مسیر = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(هدف);
        أكد(ret_file);
        أكد(ret_مسیر);

        fd = open_tmpfile_linkable_at(dir_fd, هدف, flags, &مسیر);
        إذا (fd < 0)
                أرجع fd;

        f = take_fdopen(&fd, "w");
        إذا (!f)
                أرجع -ENOMEM;

        *ret_مسیر = TAKE_PTR(مسیر);
        *ret_file = TAKE_PTR(f);
        أرجع 0;
}

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *مسیر, ثابت حرف *هدف, LinkTmpfileFlags flags) {
        صحيح r;

        أكد(fd >= 0);
        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(هدف);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        إذا (FLAGS_مجموعه(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                أرجع -errno;

        إذا (مسیر) {
                إذا (FLAGS_مجموعه(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, مسیر, dir_fd, هدف));
                وإلا
                        r = rename_noreplace(dir_fd, مسیر, dir_fd, هدف);
        } وإلا {
                إذا (FLAGS_مجموعه(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ عدم, dir_fd, هدف);
                وإلا
                        r = link_fd(fd, dir_fd, هدف);
        }
        إذا (r < 0)
                أرجع r;

        إذا (FLAGS_مجموعه(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_پر(fd);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *مسیر, ثابت حرف *هدف, LinkTmpfileFlags flags) {
        صحيح fd, r;

        أكد(f);
        أكد(هدف);

        fd = fileno(f);
        إذا (fd < 0) /* Not all FILE* objects encapsulate fds */
                أرجع -EBADF;

        r = fflush_and_check(f);
        إذا (r < 0)
                أرجع r;

        أرجع link_tmpfile_at(fd, dir_fd, مسیر, هدف, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *قالب, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(ret);

        إذا (قالب)
                p = strdup(قالب);
        وإلا {
                ثابت حرف *tmp;

                r = tmp_dir(&tmp);
                إذا (r < 0)
                        أرجع r;

                p = مسیر_join(tmp, "XXXXXX");
        }
        إذا (!p)
                أرجع -ENOMEM;

        إذا (!mkdtemp(p))
                أرجع -errno;

        *ret = TAKE_PTR(p);
        أرجع 0;
}

صحيح mkdtemp_open(ثابت حرف *قالب, صحيح flags, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح fd, r;

        r = mkdtemp_malloc(قالب, &p);
        إذا (r < 0)
                أرجع r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        إذا (fd < 0) {
                (فراغ) rmdir(p);
                أرجع fd;
        }

        إذا (ret)
                *ret = TAKE_PTR(p);

        أرجع fd;
}

فراغ cleanup_tmpfile_داده_انجامشده(بنية cleanup_tmpfile_داده *d) {
        أكد(d);

        إذا (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                أرجع;

        PROTECT_ERRNO;

        (فراغ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = عدم;
        d->filename = عدم;
}
