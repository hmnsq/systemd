/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "random-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "sync-util.h"
#அந்தர்பாவ "tmpfile-util.h"
#அந்தர்பாவ "umask-util.h"

நிலையான முழு fopen_temporary_internal(முழு dir_fd, மாறிலி எழுத்து *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        எனில் (fd < 0)
                திரும்பு -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        எனில் (r < 0) {
                (வெறுமை) unlinkat(dir_fd, path, 0);
                திரும்பு r;
        }

        எனில் (ret_file)
                *ret_file = TAKE_PTR(f);

        திரும்பு 0;
}

முழு fopen_temporary_at(முழு dir_fd, மாறிலி எழுத்து *path, FILE **ret_file, எழுத்து **ret_path) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(path);

        r = tempfn_random(path, வெற்று, &t);
        எனில் (r < 0)
                திரும்பு r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(t);

        திரும்பு 0;
}

முழு fopen_temporary_child_at(முழு dir_fd, மாறிலி எழுத்து *path, FILE **ret_file, எழுத்து **ret_path) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);

        எனில் (!path) {
                r = tmp_dir(&path);
                எனில் (r < 0)
                        திரும்பு r;
        }

        r = tempfn_random_child(path, வெற்று, &t);
        எனில் (r < 0)
                திரும்பு r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path)
                *ret_path = TAKE_PTR(t);

        திரும்பு 0;
}

/* This is much like mkostemp() but is subject to umask(). */
முழு mkostemp_safe(எழுத்து *pattern) {
        ப்ரதிஜ்ஞா(pattern);
        BLOCK_WITH_UMASK(0077);
        திரும்பு RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

முழு fmkostemp_safe(எழுத்து *pattern, மாறிலி எழுத்து *mode, FILE **ret_f) {
        _cleanup_close_ முழு fd = -EBADF;
        FILE *f;

        ப்ரதிஜ்ஞா(ret_f);

        fd = mkostemp_safe(pattern);
        எனில் (fd < 0)
                திரும்பு fd;

        f = take_fdopen(&fd, mode);
        எனில் (!f)
                திரும்பு -errno;

        *ret_f = f;
        திரும்பு 0;
}

வெறுமை unlink_tempfilep(எழுத்து (*p)[]) {
        ப்ரதிஜ்ஞா(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        எனில் (!endswith(*p, ".XXXXXX"))
                (வெறுமை) unlink(*p);
}

நிலையான முழு tempfn_build(மாறிலி எழுத்து *p, மாறிலி எழுத்து *pre, மாறிலி எழுத்து *post, bool child, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *d = வெற்று, *fn = வெற்று, *nf = வெற்று, *result = வெற்று;
        size_t len_pre, len_post, len_add;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        எனில் (pre && strchr(pre, '/'))
                திரும்பு -EINVAL;

        எனில் (post && strchr(post, '/'))
                திரும்பு -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        எனில் (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                திரும்பு -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        எனில் (child) {
                d = strdup(p);
                எனில் (!d)
                        திரும்பு -ENOMEM;
        } இல்லையெனில் {
                r = path_split_prefix_filename(p, &d, &fn);
                எனில் (r < 0)
                        திரும்பு r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        எனில் (!nf)
                திரும்பு -ENOMEM;

        எனில் (d) {
                எனில் (!path_extend(&d, nf))
                        திரும்பு -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } இல்லையெனில்
                result = TAKE_PTR(nf);

        எனில் (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(result);
        திரும்பு 0;
}

முழு tempfn_xxxxxx(மாறிலி எழுத்து *p, மாறிலி எழுத்து *extra, எழுத்து **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        திரும்பு tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

முழு tempfn_random(மாறிலி எழுத்து *p, மாறிலி எழுத்து *extra, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        எனில் (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                திரும்பு -ENOMEM;

        திரும்பு tempfn_build(p, extra, s, /* child= */ false, ret);
}

முழு tempfn_random_child(மாறிலி எழுத்து *p, மாறிலி எழுத்து *extra, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        எனில் (!p) {
                r = tmp_dir(&p);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                திரும்பு -ENOMEM;

        திரும்பு tempfn_build(p, extra, s, /* child= */ true, ret);
}

முழு open_tmpfile_unlinkable(மாறிலி எழுத்து *directory, முழு flags) {
        முழு r;

        எனில் (!directory) {
                r = tmp_dir(&directory);
                எனில் (r < 0)
                        திரும்பு r;
        } இல்லையெனில் எனில் (isempty(directory))
                திரும்பு -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        முழு fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        எனில் (fd >= 0)
                திரும்பு fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ எழுத்து *p = path_join(directory, "/systemd-tmp-XXXXXX");
        எனில் (!p)
                திரும்பு -ENOMEM;

        fd = mkostemp_safe(p);
        எனில் (fd < 0)
                திரும்பு fd;

        (வெறுமை) unlink(p);

        திரும்பு fd;
}

முழு open_tmpfile_linkable_at(முழு dir_fd, மாறிலி எழுத்து *target, முழு flags, எழுத்து **ret_path) {
        முழு r;

        ப்ரதிஜ்ஞா(target);
        ப்ரதிஜ்ஞா(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        ப்ரதிஜ்ஞா((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        முழு fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        எனில் (fd >= 0) {
                *ret_path = வெற்று;
                திரும்பு fd;
        }

        எனில் (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ எழுத்து *tmp = வெற்று;
        r = tempfn_random(target, வெற்று, &tmp);
        எனில் (r < 0)
                திரும்பு r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        எனில் (fd < 0)
                திரும்பு -errno;

        *ret_path = TAKE_PTR(tmp);

        திரும்பு fd;
}

முழு fopen_tmpfile_linkable_at(முழு dir_fd, மாறிலி எழுத்து *target, முழு flags, எழுத்து **ret_path, FILE **ret_file) {
        _cleanup_free_ எழுத்து *path = வெற்று;
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(target);
        ப்ரதிஜ்ஞா(ret_file);
        ப்ரதிஜ்ஞா(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        எனில் (fd < 0)
                திரும்பு fd;

        f = take_fdopen(&fd, "w");
        எனில் (!f)
                திரும்பு -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        திரும்பு 0;
}

முழு link_tmpfile_at(முழு fd, முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *target, LinkTmpfileFlags flags) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        எனில் (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                திரும்பு -errno;

        எனில் (path) {
                எனில் (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                இல்லையெனில்
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } இல்லையெனில் {
                எனில் (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ வெற்று, dir_fd, target);
                இல்லையெனில்
                        r = link_fd(fd, dir_fd, target);
        }
        எனில் (r < 0)
                திரும்பு r;

        எனில் (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு flink_tmpfile_at(FILE *f, முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *target, LinkTmpfileFlags flags) {
        முழு fd, r;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(target);

        fd = fileno(f);
        எனில் (fd < 0) /* Not all FILE* objects encapsulate fds */
                திரும்பு -EBADF;

        r = fflush_and_check(f);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு link_tmpfile_at(fd, dir_fd, path, target, flags);
}

முழு mkdtemp_malloc(மாறிலி எழுத்து *template, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        எனில் (template)
                p = strdup(template);
        இல்லையெனில் {
                மாறிலி எழுத்து *tmp;

                r = tmp_dir(&tmp);
                எனில் (r < 0)
                        திரும்பு r;

                p = path_join(tmp, "XXXXXX");
        }
        எனில் (!p)
                திரும்பு -ENOMEM;

        எனில் (!mkdtemp(p))
                திரும்பு -errno;

        *ret = TAKE_PTR(p);
        திரும்பு 0;
}

முழு mkdtemp_open(மாறிலி எழுத்து *template, முழு flags, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு fd, r;

        r = mkdtemp_malloc(template, &p);
        எனில் (r < 0)
                திரும்பு r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        எனில் (fd < 0) {
                (வெறுமை) rmdir(p);
                திரும்பு fd;
        }

        எனில் (ret)
                *ret = TAKE_PTR(p);

        திரும்பு fd;
}

வெறுமை cleanup_tmpfile_data_done(கட்டமைப்பு cleanup_tmpfile_data *d) {
        ப்ரதிஜ்ஞா(d);

        எனில் (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                திரும்பு;

        PROTECT_ERRNO;

        (வெறுமை) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = வெற்று;
        d->filename = வெற்று;
}
