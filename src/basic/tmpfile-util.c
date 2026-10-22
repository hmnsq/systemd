/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "sync-util.h"
#ଅନ୍ତର୍ଭାଵ "tmpfile-util.h"
#ଅନ୍ତର୍ଭାଵ "umask-util.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ଯଦି (r < 0) {
                (ଶୂନ୍ୟ) unlinkat(dir_fd, path, 0);
                ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_file)
                *ret_file = TAKE_PTR(f);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file, ଅକ୍ଷର **ret_path) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(path);

        r = tempfn_random(path, ଖାଲି, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(t);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary_child_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file, ଅକ୍ଷର **ret_path) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ଯଦି (!path) {
                r = tmp_dir(&path);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        r = tempfn_random_child(path, ଖାଲି, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(t);

        ଫେରନ୍ତୁ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkostemp_safe(ଅକ୍ଷର *pattern) {
        ପ୍ରତିଜ୍ଞା(pattern);
        BLOCK_WITH_UMASK(0077);
        ଫେରନ୍ତୁ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmkostemp_safe(ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *mode, FILE **ret_f) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        FILE *f;

        ପ୍ରତିଜ୍ଞା(ret_f);

        fd = mkostemp_safe(pattern);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        f = take_fdopen(&fd, mode);
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        *ret_f = f;
        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ unlink_tempfilep(ଅକ୍ଷର (*p)[]) {
        ପ୍ରତିଜ୍ଞା(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ଯଦି (!endswith(*p, ".XXXXXX"))
                (ଶୂନ୍ୟ) unlink(*p);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_build(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *pre, ଅଚଳ ଅକ୍ଷର *post, bool child, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *d = ଖାଲି, *fn = ଖାଲି, *nf = ଖାଲି, *result = ଖାଲି;
        size_t len_pre, len_post, len_add;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        ଯଦି (pre && strchr(pre, '/'))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (post && strchr(post, '/'))
                ଫେରନ୍ତୁ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ଯଦି (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ଫେରନ୍ତୁ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ଯଦି (child) {
                d = strdup(p);
                ଯଦି (!d)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା {
                r = path_split_prefix_filename(p, &d, &fn);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ଯଦି (!nf)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (d) {
                ଯଦି (!path_extend(&d, nf))
                        ଫେରନ୍ତୁ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } ଅନ୍ୟଥା
                result = TAKE_PTR(nf);

        ଯଦି (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(result);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_xxxxxx(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *extra, ଅକ୍ଷର **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ଫେରନ୍ତୁ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_random(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *extra, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        ଯଦି (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ tempfn_build(p, extra, s, /* child= */ false, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_random_child(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *extra, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ଯଦି (!p) {
                r = tmp_dir(&p);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ tempfn_build(p, extra, s, /* child= */ true, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_tmpfile_unlinkable(ଅଚଳ ଅକ୍ଷର *directory, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!directory) {
                r = tmp_dir(&directory);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        } ଅନ୍ୟଥା ଯଦି (isempty(directory))
                ଫେରନ୍ତୁ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ଯଦି (fd >= 0)
                ଫେରନ୍ତୁ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ ଅକ୍ଷର *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        fd = mkostemp_safe(p);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        (ଶୂନ୍ୟ) unlink(p);

        ଫେରନ୍ତୁ fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_tmpfile_linkable_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_path) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(target);
        ପ୍ରତିଜ୍ଞା(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        ପ୍ରତିଜ୍ଞା((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        ଯଦି (fd >= 0) {
                *ret_path = ଖାଲି;
                ଫେରନ୍ତୁ fd;
        }

        ଯଦି (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ ଅକ୍ଷର *tmp = ଖାଲି;
        r = tempfn_random(target, ଖାଲି, &tmp);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        *ret_path = TAKE_PTR(tmp);

        ଫେରନ୍ତୁ fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_tmpfile_linkable_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_path, FILE **ret_file) {
        _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(target);
        ପ୍ରତିଜ୍ଞା(ret_file);
        ପ୍ରତିଜ୍ଞା(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        f = take_fdopen(&fd, "w");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା link_tmpfile_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *target, LinkTmpfileFlags flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ଯଦି (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (path) {
                ଯଦି (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                ଅନ୍ୟଥା
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } ଅନ୍ୟଥା {
                ଯଦି (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ଖାଲି, dir_fd, target);
                ଅନ୍ୟଥା
                        r = link_fd(fd, dir_fd, target);
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flink_tmpfile_at(FILE *f, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *target, LinkTmpfileFlags flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, r;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(target);

        fd = fileno(f);
        ଯଦି (fd < 0) /* Not all FILE* objects encapsulate fds */
                ଫେରନ୍ତୁ -EBADF;

        r = fflush_and_check(f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdtemp_malloc(ଅଚଳ ଅକ୍ଷର *template, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (template)
                p = strdup(template);
        ଅନ୍ୟଥା {
                ଅଚଳ ଅକ୍ଷର *tmp;

                r = tmp_dir(&tmp);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                p = path_join(tmp, "XXXXXX");
        }
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (!mkdtemp(p))
                ଫେରନ୍ତୁ -errno;

        *ret = TAKE_PTR(p);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdtemp_open(ଅଚଳ ଅକ୍ଷର *template, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, r;

        r = mkdtemp_malloc(template, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ଯଦି (fd < 0) {
                (ଶୂନ୍ୟ) rmdir(p);
                ଫେରନ୍ତୁ fd;
        }

        ଯଦି (ret)
                *ret = TAKE_PTR(p);

        ଫେରନ୍ତୁ fd;
}

ଶୂନ୍ୟ cleanup_tmpfile_data_done(ସଂରଚନା cleanup_tmpfile_data *d) {
        ପ୍ରତିଜ୍ଞା(d);

        ଯଦି (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ଫେରନ୍ତୁ;

        PROTECT_ERRNO;

        (ଶୂନ୍ୟ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ଖାଲି;
        d->filename = ଖାଲି;
}
