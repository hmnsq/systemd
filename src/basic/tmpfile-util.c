/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "random-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "sync-util.h"
#অন্তর্ভাব "tmpfile-util.h"
#অন্তর্ভাব "umask-util.h"

স্থির পূর্ণসংখ্যা fopen_temporary_internal(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        যদি (fd < 0)
                ফেরত -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        যদি (r < 0) {
                (শূন্য) unlinkat(dir_fd, path, 0);
                ফেরত r;
        }

        যদি (ret_file)
                *ret_file = TAKE_PTR(f);

        ফেরত 0;
}

পূর্ণসংখ্যা fopen_temporary_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, FILE **ret_file, অক্ষর **ret_path) {
        _cleanup_free_ অক্ষর *t = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(path);

        r = tempfn_random(path, ফাঁকা, &t);
        যদি (r < 0)
                ফেরত r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(t);

        ফেরত 0;
}

পূর্ণসংখ্যা fopen_temporary_child_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, FILE **ret_file, অক্ষর **ret_path) {
        _cleanup_free_ অক্ষর *t = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);

        যদি (!path) {
                r = tmp_dir(&path);
                যদি (r < 0)
                        ফেরত r;
        }

        r = tempfn_random_child(path, ফাঁকা, &t);
        যদি (r < 0)
                ফেরত r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(t);

        ফেরত 0;
}

/* This is much like mkostemp() but is subject to umask(). */
পূর্ণসংখ্যা mkostemp_safe(অক্ষর *pattern) {
        প্রতিজ্ঞা(pattern);
        BLOCK_WITH_UMASK(0077);
        ফেরত RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

পূর্ণসংখ্যা fmkostemp_safe(অক্ষর *pattern, ধ্রুবক অক্ষর *mode, FILE **ret_f) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        FILE *f;

        প্রতিজ্ঞা(ret_f);

        fd = mkostemp_safe(pattern);
        যদি (fd < 0)
                ফেরত fd;

        f = take_fdopen(&fd, mode);
        যদি (!f)
                ফেরত -errno;

        *ret_f = f;
        ফেরত 0;
}

শূন্য unlink_tempfilep(অক্ষর (*p)[]) {
        প্রতিজ্ঞা(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        যদি (!endswith(*p, ".XXXXXX"))
                (শূন্য) unlink(*p);
}

স্থির পূর্ণসংখ্যা tempfn_build(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *pre, ধ্রুবক অক্ষর *post, bool child, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *d = ফাঁকা, *fn = ফাঁকা, *nf = ফাঁকা, *ফলাফল = ফাঁকা;
        size_t len_pre, len_post, len_add;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        যদি (pre && strchr(pre, '/'))
                ফেরত -EINVAL;

        যদি (post && strchr(post, '/'))
                ফেরত -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        যদি (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ফেরত -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        যদি (child) {
                d = strdup(p);
                যদি (!d)
                        ফেরত -ENOMEM;
        } অন্যথা {
                r = path_split_prefix_filename(p, &d, &fn);
                যদি (r < 0)
                        ফেরত r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        যদি (!nf)
                ফেরত -ENOMEM;

        যদি (d) {
                যদি (!path_extend(&d, nf))
                        ফেরত -ENOMEM;

                ফলাফল = path_simplify(TAKE_PTR(d));
        } অন্যথা
                ফলাফল = TAKE_PTR(nf);

        যদি (!path_is_valid(ফলাফল)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ফেরত -EINVAL;

        *ret = TAKE_PTR(ফলাফল);
        ফেরত 0;
}

পূর্ণসংখ্যা tempfn_xxxxxx(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *extra, অক্ষর **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ফেরত tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

পূর্ণসংখ্যা tempfn_random(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *extra, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        যদি (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ফেরত -ENOMEM;

        ফেরত tempfn_build(p, extra, s, /* child= */ false, ret);
}

পূর্ণসংখ্যা tempfn_random_child(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *extra, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        যদি (!p) {
                r = tmp_dir(&p);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ফেরত -ENOMEM;

        ফেরত tempfn_build(p, extra, s, /* child= */ true, ret);
}

পূর্ণসংখ্যা open_tmpfile_unlinkable(ধ্রুবক অক্ষর *directory, পূর্ণসংখ্যা flags) {
        পূর্ণসংখ্যা r;

        যদি (!directory) {
                r = tmp_dir(&directory);
                যদি (r < 0)
                        ফেরত r;
        } অন্যথা যদি (isempty(directory))
                ফেরত -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        পূর্ণসংখ্যা fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        যদি (fd >= 0)
                ফেরত fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ অক্ষর *p = path_join(directory, "/systemd-tmp-XXXXXX");
        যদি (!p)
                ফেরত -ENOMEM;

        fd = mkostemp_safe(p);
        যদি (fd < 0)
                ফেরত fd;

        (শূন্য) unlink(p);

        ফেরত fd;
}

পূর্ণসংখ্যা open_tmpfile_linkable_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা flags, অক্ষর **ret_path) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        প্রতিজ্ঞা((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        পূর্ণসংখ্যা fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        যদি (fd >= 0) {
                *ret_path = ফাঁকা;
                ফেরত fd;
        }

        যদি (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ অক্ষর *tmp = ফাঁকা;
        r = tempfn_random(target, ফাঁকা, &tmp);
        যদি (r < 0)
                ফেরত r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        যদি (fd < 0)
                ফেরত -errno;

        *ret_path = TAKE_PTR(tmp);

        ফেরত fd;
}

পূর্ণসংখ্যা fopen_tmpfile_linkable_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা flags, অক্ষর **ret_path, FILE **ret_file) {
        _cleanup_free_ অক্ষর *path = ফাঁকা;
        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(ret_file);
        প্রতিজ্ঞা(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        যদি (fd < 0)
                ফেরত fd;

        f = take_fdopen(&fd, "w");
        যদি (!f)
                ফেরত -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ফেরত 0;
}

পূর্ণসংখ্যা link_tmpfile_at(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *target, LinkTmpfileFlags flags) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        যদি (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ফেরত -errno;

        যদি (path) {
                যদি (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                অন্যথা
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } অন্যথা {
                যদি (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ফাঁকা, dir_fd, target);
                অন্যথা
                        r = link_fd(fd, dir_fd, target);
        }
        যদি (r < 0)
                ফেরত r;

        যদি (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা flink_tmpfile_at(FILE *f, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *target, LinkTmpfileFlags flags) {
        পূর্ণসংখ্যা fd, r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(target);

        fd = fileno(f);
        যদি (fd < 0) /* Not all FILE* objects encapsulate fds */
                ফেরত -EBADF;

        r = fflush_and_check(f);
        যদি (r < 0)
                ফেরত r;

        ফেরত link_tmpfile_at(fd, dir_fd, path, target, flags);
}

পূর্ণসংখ্যা mkdtemp_malloc(ধ্রুবক অক্ষর *template, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        যদি (template)
                p = strdup(template);
        অন্যথা {
                ধ্রুবক অক্ষর *tmp;

                r = tmp_dir(&tmp);
                যদি (r < 0)
                        ফেরত r;

                p = path_join(tmp, "XXXXXX");
        }
        যদি (!p)
                ফেরত -ENOMEM;

        যদি (!mkdtemp(p))
                ফেরত -errno;

        *ret = TAKE_PTR(p);
        ফেরত 0;
}

পূর্ণসংখ্যা mkdtemp_open(ধ্রুবক অক্ষর *template, পূর্ণসংখ্যা flags, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা fd, r;

        r = mkdtemp_malloc(template, &p);
        যদি (r < 0)
                ফেরত r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        যদি (fd < 0) {
                (শূন্য) rmdir(p);
                ফেরত fd;
        }

        যদি (ret)
                *ret = TAKE_PTR(p);

        ফেরত fd;
}

শূন্য cleanup_tmpfile_তথ্য_done(কাঠামো cleanup_tmpfile_তথ্য *d) {
        প্রতিজ্ঞা(d);

        যদি (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ফেরত;

        PROTECT_ERRNO;

        (শূন্য) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ফাঁকা;
        d->filename = ফাঁকা;
}
