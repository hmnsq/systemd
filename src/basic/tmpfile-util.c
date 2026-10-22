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

ساکن صحیح fopen_temporary_اندرونی(صحیح dir_fd, ثابت حرف *راستہ, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(راستہ);

        fd = openat(dir_fd, راستہ, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        اگر (fd < 0)
                واپس -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        اگر (r < 0) {
                (خالی) unlinkat(dir_fd, راستہ, 0);
                واپس r;
        }

        اگر (ret_file)
                *ret_file = TAKE_PTR(f);

        واپس 0;
}

صحیح fopen_temporary_at(صحیح dir_fd, ثابت حرف *راستہ, FILE **ret_file, حرف **ret_راستہ) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(راستہ);

        r = tempfn_random(راستہ, تہی, &t);
        اگر (r < 0)
                واپس r;

        r = fopen_temporary_اندرونی(dir_fd, t, ret_file);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(t);

        واپس 0;
}

صحیح fopen_temporary_بچہ_at(صحیح dir_fd, ثابت حرف *راستہ, FILE **ret_file, حرف **ret_راستہ) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        اگر (!راستہ) {
                r = tmp_dir(&راستہ);
                اگر (r < 0)
                        واپس r;
        }

        r = tempfn_random_بچہ(راستہ, تہی, &t);
        اگر (r < 0)
                واپس r;

        r = fopen_temporary_اندرونی(dir_fd, t, ret_file);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(t);

        واپس 0;
}

/* This is much like mkostemp() but is subject to umask(). */
صحیح mkostemp_محفوظ(حرف *نمونہ) {
        أكد(نمونہ);
        بلاک_WITH_UMASK(0077);
        واپس RET_NERRNO(mkostemp(نمونہ, O_CLOEXEC));
}

صحیح fmkostemp_محفوظ(حرف *نمونہ, ثابت حرف *منوال, FILE **ret_f) {
        _cleanup_close_ صحیح fd = -EBADF;
        FILE *f;

        أكد(ret_f);

        fd = mkostemp_محفوظ(نمونہ);
        اگر (fd < 0)
                واپس fd;

        f = take_fdopen(&fd, منوال);
        اگر (!f)
                واپس -errno;

        *ret_f = f;
        واپس 0;
}

خالی unlink_tempfilep(حرف (*p)[]) {
        أكد(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        اگر (!endswith(*p, ".XXXXXX"))
                (خالی) unlink(*p);
}

ساکن صحیح tempfn_build(ثابت حرف *p, ثابت حرف *pre, ثابت حرف *post, bool بچہ, حرف **ret) {
        _cleanup_free_ حرف *d = تہی, *fn = تہی, *nf = تہی, *نتیجہ = تہی;
        size_t len_pre, len_post, len_جوڑو;
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
                واپس -EINVAL;

        اگر (post && strchr(post, '/'))
                واپس -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        اگر (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                واپس -EINVAL;

        len_جوڑو = len_pre + len_post + STRLEN(".#");

        اگر (بچہ) {
                d = strdup(p);
                اگر (!d)
                        واپس -ENOMEM;
        } ورنہ {
                r = راستہ_بانٹو_سابقہ_filename(p, &d, &fn);
                اگر (r < 0)
                        واپس r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_جوڑو);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        اگر (!nf)
                واپس -ENOMEM;

        اگر (d) {
                اگر (!راستہ_extend(&d, nf))
                        واپس -ENOMEM;

                نتیجہ = راستہ_simplify(TAKE_PTR(d));
        } ورنہ
                نتیجہ = TAKE_PTR(nf);

        اگر (!راستہ_is_valid(نتیجہ)) /* New path is not valid? (Maybe because too long?) Refuse. */
                واپس -EINVAL;

        *ret = TAKE_PTR(نتیجہ);
        واپس 0;
}

صحیح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        واپس tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

صحیح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;

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
                واپس -ENOMEM;

        واپس tempfn_build(p, extra, s, /* child= */ false, ret);
}

صحیح tempfn_random_بچہ(ثابت حرف *p, ثابت حرف *extra, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
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
                        واپس r;
        }

        اگر (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                واپس -ENOMEM;

        واپس tempfn_build(p, extra, s, /* child= */ true, ret);
}

صحیح open_tmpfile_unlinkable(ثابت حرف *directory, صحیح flags) {
        صحیح r;

        اگر (!directory) {
                r = tmp_dir(&directory);
                اگر (r < 0)
                        واپس r;
        } ورنہ اگر (isempty(directory))
                واپس -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        صحیح fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        اگر (fd >= 0)
                واپس fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ حرف *p = راستہ_join(directory, "/systemd-tmp-XXXXXX");
        اگر (!p)
                واپس -ENOMEM;

        fd = mkostemp_محفوظ(p);
        اگر (fd < 0)
                واپس fd;

        (خالی) unlink(p);

        واپس fd;
}

صحیح open_tmpfile_linkable_at(صحیح dir_fd, ثابت حرف *ہدف, صحیح flags, حرف **ret_راستہ) {
        صحیح r;

        أكد(ہدف);
        أكد(ret_راستہ);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        أكد((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        صحیح fd = open_والد_at(dir_fd, ہدف, O_TMPFILE|flags, 0640);
        اگر (fd >= 0) {
                *ret_راستہ = تہی;
                واپس fd;
        }

        اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", ہدف);

        _cleanup_free_ حرف *tmp = تہی;
        r = tempfn_random(ہدف, تہی, &tmp);
        اگر (r < 0)
                واپس r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        اگر (fd < 0)
                واپس -errno;

        *ret_راستہ = TAKE_PTR(tmp);

        واپس fd;
}

صحیح fopen_tmpfile_linkable_at(صحیح dir_fd, ثابت حرف *ہدف, صحیح flags, حرف **ret_راستہ, FILE **ret_file) {
        _cleanup_free_ حرف *راستہ = تہی;
        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(ہدف);
        أكد(ret_file);
        أكد(ret_راستہ);

        fd = open_tmpfile_linkable_at(dir_fd, ہدف, flags, &راستہ);
        اگر (fd < 0)
                واپس fd;

        f = take_fdopen(&fd, "w");
        اگر (!f)
                واپس -ENOMEM;

        *ret_راستہ = TAKE_PTR(راستہ);
        *ret_file = TAKE_PTR(f);
        واپس 0;
}

صحیح link_tmpfile_at(صحیح fd, صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *ہدف, LinkTmpfileFlags flags) {
        صحیح r;

        أكد(fd >= 0);
        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(ہدف);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        اگر (FLAGS_سیٹ(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                واپس -errno;

        اگر (راستہ) {
                اگر (FLAGS_سیٹ(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, راستہ, dir_fd, ہدف));
                ورنہ
                        r = rename_noreplace(dir_fd, راستہ, dir_fd, ہدف);
        } ورنہ {
                اگر (FLAGS_سیٹ(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ تہی, dir_fd, ہدف);
                ورنہ
                        r = link_fd(fd, dir_fd, ہدف);
        }
        اگر (r < 0)
                واپس r;

        اگر (FLAGS_سیٹ(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_بھرا(fd);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

صحیح flink_tmpfile_at(FILE *f, صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *ہدف, LinkTmpfileFlags flags) {
        صحیح fd, r;

        أكد(f);
        أكد(ہدف);

        fd = fileno(f);
        اگر (fd < 0) /* Not all FILE* objects encapsulate fds */
                واپس -EBADF;

        r = fflush_and_check(f);
        اگر (r < 0)
                واپس r;

        واپس link_tmpfile_at(fd, dir_fd, راستہ, ہدف, flags);
}

صحیح mkdtemp_malloc(ثابت حرف *سانچہ, حرف **ret) {
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(ret);

        اگر (سانچہ)
                p = strdup(سانچہ);
        ورنہ {
                ثابت حرف *tmp;

                r = tmp_dir(&tmp);
                اگر (r < 0)
                        واپس r;

                p = راستہ_join(tmp, "XXXXXX");
        }
        اگر (!p)
                واپس -ENOMEM;

        اگر (!mkdtemp(p))
                واپس -errno;

        *ret = TAKE_PTR(p);
        واپس 0;
}

صحیح mkdtemp_open(ثابت حرف *سانچہ, صحیح flags, حرف **ret) {
        _cleanup_free_ حرف *p = تہی;
        صحیح fd, r;

        r = mkdtemp_malloc(سانچہ, &p);
        اگر (r < 0)
                واپس r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        اگر (fd < 0) {
                (خالی) rmdir(p);
                واپس fd;
        }

        اگر (ret)
                *ret = TAKE_PTR(p);

        واپس fd;
}

خالی cleanup_tmpfile_ڈیٹا_مکمل(ساخت cleanup_tmpfile_ڈیٹا *d) {
        أكد(d);

        اگر (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                واپس;

        PROTECT_ERRNO;

        (خالی) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = تہی;
        d->filename = تہی;
}
