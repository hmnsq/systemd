/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "random-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "sync-util.h"
#अन्तर्भाव "tmpfile-util.h"
#अन्तर्भाव "umask-util.h"

स्थिर पूर्णाङ्क fopen_temporary_internal(पूर्णाङ्क dir_fd, अचर वर्ण *path, खाता **ret_file) {
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        यदि (fd < 0)
                फर्काऊ -त्रुटि_सं;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        यदि (r < 0) {
                (शून्य) unlinkat(dir_fd, path, 0);
                फर्काऊ r;
        }

        यदि (ret_file)
                *ret_file = TAKE_PTR(f);

        फर्काऊ 0;
}

पूर्णाङ्क fopen_temporary_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, खाता **ret_file, वर्ण **ret_path) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        r = tempfn_random(path, रिक्त, &t);
        यदि (r < 0)
                फर्काऊ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        यदि (r < 0)
                फर्काऊ r;

        यदि (ret_path)
                *ret_path = TAKE_PTR(t);

        फर्काऊ 0;
}

पूर्णाङ्क fopen_temporary_child_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, खाता **ret_file, वर्ण **ret_path) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        यदि (!path) {
                r = tmp_dir(&path);
                यदि (r < 0)
                        फर्काऊ r;
        }

        r = tempfn_random_child(path, रिक्त, &t);
        यदि (r < 0)
                फर्काऊ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        यदि (r < 0)
                फर्काऊ r;

        यदि (ret_path)
                *ret_path = TAKE_PTR(t);

        फर्काऊ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
पूर्णाङ्क mkostemp_safe(वर्ण *pattern) {
        प्रतिज्ञा(pattern);
        BLOCK_WITH_UMASK(0077);
        फर्काऊ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

पूर्णाङ्क fmkostemp_safe(वर्ण *pattern, अचर वर्ण *mode, खाता **ret_f) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        खाता *f;

        प्रतिज्ञा(ret_f);

        fd = mkostemp_safe(pattern);
        यदि (fd < 0)
                फर्काऊ fd;

        f = take_fdopen(&fd, mode);
        यदि (!f)
                फर्काऊ -त्रुटि_सं;

        *ret_f = f;
        फर्काऊ 0;
}

शून्य unlink_tempfilep(वर्ण (*p)[]) {
        प्रतिज्ञा(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        यदि (!endswith(*p, ".XXXXXX"))
                (शून्य) unlink(*p);
}

स्थिर पूर्णाङ्क tempfn_build(अचर वर्ण *p, अचर वर्ण *pre, अचर वर्ण *post, bool child, वर्ण **ret) {
        _cleanup_free_ वर्ण *d = रिक्त, *fn = रिक्त, *nf = रिक्त, *result = रिक्त;
        माप_प्रकार len_pre, len_post, len_add;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        यदि (pre && म_अक्षर(pre, '/'))
                फर्काऊ -EINVAL;

        यदि (post && म_अक्षर(post, '/'))
                फर्काऊ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        यदि (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                फर्काऊ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        यदि (child) {
                d = strdup(p);
                यदि (!d)
                        फर्काऊ -ENOMEM;
        } अन्यथा {
                r = path_split_prefix_filename(p, &d, &fn);
                यदि (r < 0)
                        फर्काऊ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        यदि (!nf)
                फर्काऊ -ENOMEM;

        यदि (d) {
                यदि (!path_extend(&d, nf))
                        फर्काऊ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } अन्यथा
                result = TAKE_PTR(nf);

        यदि (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                फर्काऊ -EINVAL;

        *ret = TAKE_PTR(result);
        फर्काऊ 0;
}

पूर्णाङ्क tempfn_xxxxxx(अचर वर्ण *p, अचर वर्ण *extra, वर्ण **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        फर्काऊ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

पूर्णाङ्क tempfn_random(अचर वर्ण *p, अचर वर्ण *extra, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        यदि (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                फर्काऊ -ENOMEM;

        फर्काऊ tempfn_build(p, extra, s, /* child= */ false, ret);
}

पूर्णाङ्क tempfn_random_child(अचर वर्ण *p, अचर वर्ण *extra, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        यदि (!p) {
                r = tmp_dir(&p);
                यदि (r < 0)
                        फर्काऊ r;
        }

        यदि (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                फर्काऊ -ENOMEM;

        फर्काऊ tempfn_build(p, extra, s, /* child= */ true, ret);
}

पूर्णाङ्क open_tmpfile_unlinkable(अचर वर्ण *directory, पूर्णाङ्क flags) {
        पूर्णाङ्क r;

        यदि (!directory) {
                r = tmp_dir(&directory);
                यदि (r < 0)
                        फर्काऊ r;
        } अन्यथा यदि (isempty(directory))
                फर्काऊ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        पूर्णाङ्क fd = खोलो(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        यदि (fd >= 0)
                फर्काऊ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ वर्ण *p = path_join(directory, "/systemd-tmp-XXXXXX");
        यदि (!p)
                फर्काऊ -ENOMEM;

        fd = mkostemp_safe(p);
        यदि (fd < 0)
                फर्काऊ fd;

        (शून्य) unlink(p);

        फर्काऊ fd;
}

पूर्णाङ्क open_tmpfile_linkable_at(पूर्णाङ्क dir_fd, अचर वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path) {
        पूर्णाङ्क r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        प्रतिज्ञा((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        पूर्णाङ्क fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        यदि (fd >= 0) {
                *ret_path = रिक्त;
                फर्काऊ fd;
        }

        यदि (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ वर्ण *tmp = रिक्त;
        r = tempfn_random(target, रिक्त, &tmp);
        यदि (r < 0)
                फर्काऊ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        यदि (fd < 0)
                फर्काऊ -त्रुटि_सं;

        *ret_path = TAKE_PTR(tmp);

        फर्काऊ fd;
}

पूर्णाङ्क fopen_tmpfile_linkable_at(पूर्णाङ्क dir_fd, अचर वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path, खाता **ret_file) {
        _cleanup_free_ वर्ण *path = रिक्त;
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(target);
        प्रतिज्ञा(ret_file);
        प्रतिज्ञा(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        यदि (fd < 0)
                फर्काऊ fd;

        f = take_fdopen(&fd, "w");
        यदि (!f)
                फर्काऊ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        फर्काऊ 0;
}

पूर्णाङ्क link_tmpfile_at(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर वर्ण *target, LinkTmpfileFlags flags) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        यदि (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                फर्काऊ -त्रुटि_सं;

        यदि (path) {
                यदि (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                अन्यथा
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } अन्यथा {
                यदि (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ रिक्त, dir_fd, target);
                अन्यथा
                        r = link_fd(fd, dir_fd, target);
        }
        यदि (r < 0)
                फर्काऊ r;

        यदि (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क flink_tmpfile_at(खाता *f, पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर वर्ण *target, LinkTmpfileFlags flags) {
        पूर्णाङ्क fd, r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(target);

        fd = fileno(f);
        यदि (fd < 0) /* Not all FILE* objects encapsulate fds */
                फर्काऊ -EBADF;

        r = fflush_and_check(f);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

पूर्णाङ्क mkdtemp_malloc(अचर वर्ण *ढाँचा, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        यदि (ढाँचा)
                p = strdup(ढाँचा);
        अन्यथा {
                अचर वर्ण *tmp;

                r = tmp_dir(&tmp);
                यदि (r < 0)
                        फर्काऊ r;

                p = path_join(tmp, "XXXXXX");
        }
        यदि (!p)
                फर्काऊ -ENOMEM;

        यदि (!mkdtemp(p))
                फर्काऊ -त्रुटि_सं;

        *ret = TAKE_PTR(p);
        फर्काऊ 0;
}

पूर्णाङ्क mkdtemp_open(अचर वर्ण *ढाँचा, पूर्णाङ्क flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क fd, r;

        r = mkdtemp_malloc(ढाँचा, &p);
        यदि (r < 0)
                फर्काऊ r;

        fd = RET_NERRNO(खोलो(p, O_DIRECTORY|O_CLOEXEC|flags));
        यदि (fd < 0) {
                (शून्य) सूची_हटाओ(p);
                फर्काऊ fd;
        }

        यदि (ret)
                *ret = TAKE_PTR(p);

        फर्काऊ fd;
}

शून्य cleanup_tmpfile_data_done(संरचना cleanup_tmpfile_data *d) {
        प्रतिज्ञा(d);

        यदि (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                फर्काऊ;

        PROTECT_ERRNO;

        (शून्य) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = रिक्त;
        d->filename = रिक्त;
}
