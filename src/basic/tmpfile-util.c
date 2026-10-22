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

स्थिर पूर्णांक fopen_temporary_internal(पूर्णांक dir_fd, स्थिरांक अक्षर *path, खाता **ret_file) {
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        जर (fd < 0)
                परत -त्रुटि_सं;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        जर (r < 0) {
                (शून्य) unlinkat(dir_fd, path, 0);
                परत r;
        }

        जर (ret_file)
                *ret_file = TAKE_PTR(f);

        परत 0;
}

पूर्णांक fopen_temporary_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        r = tempfn_random(path, रिक्त, &t);
        जर (r < 0)
                परत r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        जर (r < 0)
                परत r;

        जर (ret_path)
                *ret_path = TAKE_PTR(t);

        परत 0;
}

पूर्णांक fopen_temporary_child_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        जर (!path) {
                r = tmp_dir(&path);
                जर (r < 0)
                        परत r;
        }

        r = tempfn_random_child(path, रिक्त, &t);
        जर (r < 0)
                परत r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        जर (r < 0)
                परत r;

        जर (ret_path)
                *ret_path = TAKE_PTR(t);

        परत 0;
}

/* This is much like mkostemp() but is subject to umask(). */
पूर्णांक mkostemp_safe(अक्षर *pattern) {
        प्रतिज्ञा(pattern);
        BLOCK_WITH_UMASK(0077);
        परत RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

पूर्णांक fmkostemp_safe(अक्षर *pattern, स्थिरांक अक्षर *mode, खाता **ret_f) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        खाता *f;

        प्रतिज्ञा(ret_f);

        fd = mkostemp_safe(pattern);
        जर (fd < 0)
                परत fd;

        f = take_fdopen(&fd, mode);
        जर (!f)
                परत -त्रुटि_सं;

        *ret_f = f;
        परत 0;
}

शून्य unlink_tempfilep(अक्षर (*p)[]) {
        प्रतिज्ञा(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        जर (!endswith(*p, ".XXXXXX"))
                (शून्य) unlink(*p);
}

स्थिर पूर्णांक tempfn_build(स्थिरांक अक्षर *p, स्थिरांक अक्षर *pre, स्थिरांक अक्षर *post, bool child, अक्षर **ret) {
        _cleanup_free_ अक्षर *d = रिक्त, *fn = रिक्त, *nf = रिक्त, *result = रिक्त;
        माप_प्रकार len_pre, len_post, len_add;
        पूर्णांक r;

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

        जर (pre && म_अक्षर(pre, '/'))
                परत -EINVAL;

        जर (post && म_अक्षर(post, '/'))
                परत -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        जर (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                परत -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        जर (child) {
                d = strdup(p);
                जर (!d)
                        परत -ENOMEM;
        } अन्यथा {
                r = path_split_prefix_filename(p, &d, &fn);
                जर (r < 0)
                        परत r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        जर (!nf)
                परत -ENOMEM;

        जर (d) {
                जर (!path_extend(&d, nf))
                        परत -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } अन्यथा
                result = TAKE_PTR(nf);

        जर (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                परत -EINVAL;

        *ret = TAKE_PTR(result);
        परत 0;
}

पूर्णांक tempfn_xxxxxx(स्थिरांक अक्षर *p, स्थिरांक अक्षर *extra, अक्षर **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        परत tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

पूर्णांक tempfn_random(स्थिरांक अक्षर *p, स्थिरांक अक्षर *extra, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        जर (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                परत -ENOMEM;

        परत tempfn_build(p, extra, s, /* child= */ false, ret);
}

पूर्णांक tempfn_random_child(स्थिरांक अक्षर *p, स्थिरांक अक्षर *extra, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        जर (!p) {
                r = tmp_dir(&p);
                जर (r < 0)
                        परत r;
        }

        जर (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                परत -ENOMEM;

        परत tempfn_build(p, extra, s, /* child= */ true, ret);
}

पूर्णांक open_tmpfile_unlinkable(स्थिरांक अक्षर *directory, पूर्णांक flags) {
        पूर्णांक r;

        जर (!directory) {
                r = tmp_dir(&directory);
                जर (r < 0)
                        परत r;
        } अन्यथा जर (isempty(directory))
                परत -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        पूर्णांक fd = खोलो(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        जर (fd >= 0)
                परत fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ अक्षर *p = path_join(directory, "/systemd-tmp-XXXXXX");
        जर (!p)
                परत -ENOMEM;

        fd = mkostemp_safe(p);
        जर (fd < 0)
                परत fd;

        (शून्य) unlink(p);

        परत fd;
}

पूर्णांक open_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिरांक अक्षर *target, पूर्णांक flags, अक्षर **ret_path) {
        पूर्णांक r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        प्रतिज्ञा((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        पूर्णांक fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        जर (fd >= 0) {
                *ret_path = रिक्त;
                परत fd;
        }

        जर (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ अक्षर *tmp = रिक्त;
        r = tempfn_random(target, रिक्त, &tmp);
        जर (r < 0)
                परत r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        जर (fd < 0)
                परत -त्रुटि_सं;

        *ret_path = TAKE_PTR(tmp);

        परत fd;
}

पूर्णांक fopen_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिरांक अक्षर *target, पूर्णांक flags, अक्षर **ret_path, खाता **ret_file) {
        _cleanup_free_ अक्षर *path = रिक्त;
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;

        प्रतिज्ञा(target);
        प्रतिज्ञा(ret_file);
        प्रतिज्ञा(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        जर (fd < 0)
                परत fd;

        f = take_fdopen(&fd, "w");
        जर (!f)
                परत -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        परत 0;
}

पूर्णांक link_tmpfile_at(पूर्णांक fd, पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *target, LinkTmpfileFlags flags) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        जर (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                परत -त्रुटि_सं;

        जर (path) {
                जर (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                अन्यथा
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } अन्यथा {
                जर (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ रिक्त, dir_fd, target);
                अन्यथा
                        r = link_fd(fd, dir_fd, target);
        }
        जर (r < 0)
                परत r;

        जर (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

पूर्णांक flink_tmpfile_at(खाता *f, पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *target, LinkTmpfileFlags flags) {
        पूर्णांक fd, r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(target);

        fd = fileno(f);
        जर (fd < 0) /* Not all FILE* objects encapsulate fds */
                परत -EBADF;

        r = fflush_and_check(f);
        जर (r < 0)
                परत r;

        परत link_tmpfile_at(fd, dir_fd, path, target, flags);
}

पूर्णांक mkdtemp_malloc(स्थिरांक अक्षर *ढाँचा, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        जर (ढाँचा)
                p = strdup(ढाँचा);
        अन्यथा {
                स्थिरांक अक्षर *tmp;

                r = tmp_dir(&tmp);
                जर (r < 0)
                        परत r;

                p = path_join(tmp, "XXXXXX");
        }
        जर (!p)
                परत -ENOMEM;

        जर (!mkdtemp(p))
                परत -त्रुटि_सं;

        *ret = TAKE_PTR(p);
        परत 0;
}

पूर्णांक mkdtemp_open(स्थिरांक अक्षर *ढाँचा, पूर्णांक flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त;
        पूर्णांक fd, r;

        r = mkdtemp_malloc(ढाँचा, &p);
        जर (r < 0)
                परत r;

        fd = RET_NERRNO(खोलो(p, O_DIRECTORY|O_CLOEXEC|flags));
        जर (fd < 0) {
                (शून्य) सूची_हटाओ(p);
                परत fd;
        }

        जर (ret)
                *ret = TAKE_PTR(p);

        परत fd;
}

शून्य cleanup_tmpfile_data_done(संरचना cleanup_tmpfile_data *d) {
        प्रतिज्ञा(d);

        जर (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                परत;

        PROTECT_ERRNO;

        (शून्य) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = रिक्त;
        d->filename = रिक्त;
}
