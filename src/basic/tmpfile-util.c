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

স্থির পূর্ণাঙ্ক fopen_temporary_internal(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        যদি (r < 0) {
                (শূন্য) unlinkat(dir_fd, path, 0);
                প্রত্যাগম r;
        }

        যদি (ret_file)
                *ret_file = TAKE_PTR(f);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fopen_temporary_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, FILE **ret_file, বর্ণ **ret_path) {
        _cleanup_free_ বর্ণ *t = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(path);

        r = tempfn_random(path, রিক্ত, &t);
        যদি (r < 0)
                প্রত্যাগম r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(t);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fopen_temporary_child_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, FILE **ret_file, বর্ণ **ret_path) {
        _cleanup_free_ বর্ণ *t = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);

        যদি (!path) {
                r = tmp_dir(&path);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        r = tempfn_random_child(path, রিক্ত, &t);
        যদি (r < 0)
                প্রত্যাগম r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ret_path)
                *ret_path = TAKE_PTR(t);

        প্রত্যাগম 0;
}

/* This is much like mkostemp() but is subject to umask(). */
পূর্ণাঙ্ক mkostemp_safe(বর্ণ *pattern) {
        প্রতিজ্ঞা(pattern);
        BLOCK_WITH_UMASK(0077);
        প্রত্যাগম RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

পূর্ণাঙ্ক fmkostemp_safe(বর্ণ *pattern, ধ্রুব বর্ণ *mode, FILE **ret_f) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        FILE *f;

        প্রতিজ্ঞা(ret_f);

        fd = mkostemp_safe(pattern);
        যদি (fd < 0)
                প্রত্যাগম fd;

        f = take_fdopen(&fd, mode);
        যদি (!f)
                প্রত্যাগম -errno;

        *ret_f = f;
        প্রত্যাগম 0;
}

শূন্য unlink_tempfilep(বর্ণ (*p)[]) {
        প্রতিজ্ঞা(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        যদি (!endswith(*p, ".XXXXXX"))
                (শূন্য) unlink(*p);
}

স্থির পূর্ণাঙ্ক tempfn_build(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *pre, ধ্রুব বর্ণ *post, bool child, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *d = রিক্ত, *fn = রিক্ত, *nf = রিক্ত, *result = রিক্ত;
        size_t len_pre, len_post, len_add;
        পূর্ণাঙ্ক r;

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
                প্রত্যাগম -EINVAL;

        যদি (post && strchr(post, '/'))
                প্রত্যাগম -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        যদি (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                প্রত্যাগম -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        যদি (child) {
                d = strdup(p);
                যদি (!d)
                        প্রত্যাগম -ENOMEM;
        } অন্যথা {
                r = path_split_prefix_filename(p, &d, &fn);
                যদি (r < 0)
                        প্রত্যাগম r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        যদি (!nf)
                প্রত্যাগম -ENOMEM;

        যদি (d) {
                যদি (!path_extend(&d, nf))
                        প্রত্যাগম -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } অন্যথা
                result = TAKE_PTR(nf);

        যদি (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(result);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক tempfn_xxxxxx(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *extra, বর্ণ **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        প্রত্যাগম tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

পূর্ণাঙ্ক tempfn_random(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *extra, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;

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
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম tempfn_build(p, extra, s, /* child= */ false, ret);
}

পূর্ণাঙ্ক tempfn_random_child(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *extra, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        যদি (!p) {
                r = tmp_dir(&p);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম tempfn_build(p, extra, s, /* child= */ true, ret);
}

পূর্ণাঙ্ক open_tmpfile_unlinkable(ধ্রুব বর্ণ *directory, পূর্ণাঙ্ক flags) {
        পূর্ণাঙ্ক r;

        যদি (!directory) {
                r = tmp_dir(&directory);
                যদি (r < 0)
                        প্রত্যাগম r;
        } অন্যথা যদি (isempty(directory))
                প্রত্যাগম -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        পূর্ণাঙ্ক fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        যদি (fd >= 0)
                প্রত্যাগম fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ বর্ণ *p = path_join(directory, "/systemd-tmp-XXXXXX");
        যদি (!p)
                প্রত্যাগম -ENOMEM;

        fd = mkostemp_safe(p);
        যদি (fd < 0)
                প্রত্যাগম fd;

        (শূন্য) unlink(p);

        প্রত্যাগম fd;
}

পূর্ণাঙ্ক open_tmpfile_linkable_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *target, পূর্ণাঙ্ক flags, বর্ণ **ret_path) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        প্রতিজ্ঞা((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        পূর্ণাঙ্ক fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        যদি (fd >= 0) {
                *ret_path = রিক্ত;
                প্রত্যাগম fd;
        }

        যদি (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ বর্ণ *tmp = রিক্ত;
        r = tempfn_random(target, রিক্ত, &tmp);
        যদি (r < 0)
                প্রত্যাগম r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        *ret_path = TAKE_PTR(tmp);

        প্রত্যাগম fd;
}

পূর্ণাঙ্ক fopen_tmpfile_linkable_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *target, পূর্ণাঙ্ক flags, বর্ণ **ret_path, FILE **ret_file) {
        _cleanup_free_ বর্ণ *path = রিক্ত;
        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(ret_file);
        প্রতিজ্ঞা(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        যদি (fd < 0)
                প্রত্যাগম fd;

        f = take_fdopen(&fd, "w");
        যদি (!f)
                প্রত্যাগম -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক link_tmpfile_at(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *target, LinkTmpfileFlags flags) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        যদি (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                প্রত্যাগম -errno;

        যদি (path) {
                যদি (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                অন্যথা
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } অন্যথা {
                যদি (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ রিক্ত, dir_fd, target);
                অন্যথা
                        r = link_fd(fd, dir_fd, target);
        }
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক flink_tmpfile_at(FILE *f, পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *target, LinkTmpfileFlags flags) {
        পূর্ণাঙ্ক fd, r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(target);

        fd = fileno(f);
        যদি (fd < 0) /* Not all FILE* objects encapsulate fds */
                প্রত্যাগম -EBADF;

        r = fflush_and_check(f);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম link_tmpfile_at(fd, dir_fd, path, target, flags);
}

পূর্ণাঙ্ক mkdtemp_malloc(ধ্রুব বর্ণ *template, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        যদি (template)
                p = strdup(template);
        অন্যথা {
                ধ্রুব বর্ণ *tmp;

                r = tmp_dir(&tmp);
                যদি (r < 0)
                        প্রত্যাগম r;

                p = path_join(tmp, "XXXXXX");
        }
        যদি (!p)
                প্রত্যাগম -ENOMEM;

        যদি (!mkdtemp(p))
                প্রত্যাগম -errno;

        *ret = TAKE_PTR(p);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক mkdtemp_open(ধ্রুব বর্ণ *template, পূর্ণাঙ্ক flags, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক fd, r;

        r = mkdtemp_malloc(template, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        যদি (fd < 0) {
                (শূন্য) rmdir(p);
                প্রত্যাগম fd;
        }

        যদি (ret)
                *ret = TAKE_PTR(p);

        প্রত্যাগম fd;
}

শূন্য cleanup_tmpfile_data_done(সংরচনা cleanup_tmpfile_data *d) {
        প্রতিজ্ঞা(d);

        যদি (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                প্রত্যাগম;

        PROTECT_ERRNO;

        (শূন্য) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = রিক্ত;
        d->filename = রিক্ত;
}
