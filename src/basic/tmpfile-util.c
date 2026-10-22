/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "path-util.h"
#समावेश "random-util.h"
#समावेश "string-util.h"
#समावेश "sync-util.h"
#समावेश "tmpfile-util.h"
#समावेश "umask-util.h"

अटल पूर्णांक fopen_temporary_internal(पूर्णांक dir_fd, स्थिर अक्षर *path, खाता **ret_file) {
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        अगर (r < 0) {
                (व्योम) unlinkat(dir_fd, path, 0);
                वापस r;
        }

        अगर (ret_file)
                *ret_file = TAKE_PTR(f);

        वापस 0;
}

पूर्णांक fopen_temporary_at(पूर्णांक dir_fd, स्थिर अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *t = शून्य;
        पूर्णांक r;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(path);

        r = tempfn_random(path, शून्य, &t);
        अगर (r < 0)
                वापस r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(t);

        वापस 0;
}

पूर्णांक fopen_temporary_child_at(पूर्णांक dir_fd, स्थिर अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        _cleanup_free_ अक्षर *t = शून्य;
        पूर्णांक r;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);

        अगर (!path) {
                r = tmp_dir(&path);
                अगर (r < 0)
                        वापस r;
        }

        r = tempfn_random_child(path, शून्य, &t);
        अगर (r < 0)
                वापस r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        अगर (r < 0)
                वापस r;

        अगर (ret_path)
                *ret_path = TAKE_PTR(t);

        वापस 0;
}

/* This is much like mkostemp() but is subject to umask(). */
पूर्णांक mkostemp_safe(अक्षर *pattern) {
        निश्चित(pattern);
        BLOCK_WITH_UMASK(0077);
        वापस RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

पूर्णांक fmkostemp_safe(अक्षर *pattern, स्थिर अक्षर *mode, खाता **ret_f) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        खाता *f;

        निश्चित(ret_f);

        fd = mkostemp_safe(pattern);
        अगर (fd < 0)
                वापस fd;

        f = take_fdopen(&fd, mode);
        अगर (!f)
                वापस -त्रुटि_सं;

        *ret_f = f;
        वापस 0;
}

व्योम unlink_tempfilep(अक्षर (*p)[]) {
        निश्चित(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        अगर (!endswith(*p, ".XXXXXX"))
                (व्योम) unlink(*p);
}

अटल पूर्णांक tempfn_build(स्थिर अक्षर *p, स्थिर अक्षर *pre, स्थिर अक्षर *post, bool child, अक्षर **ret) {
        _cleanup_free_ अक्षर *d = शून्य, *fn = शून्य, *nf = शून्य, *परिणाम = शून्य;
        माप_प्रकार len_pre, len_post, len_add;
        पूर्णांक r;

        निश्चित(p);
        निश्चित(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        अगर (pre && म_अक्षर(pre, '/'))
                वापस -EINVAL;

        अगर (post && म_अक्षर(post, '/'))
                वापस -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        अगर (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                वापस -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        अगर (child) {
                d = strdup(p);
                अगर (!d)
                        वापस -ENOMEM;
        } अन्यथा {
                r = path_split_prefix_filename(p, &d, &fn);
                अगर (r < 0)
                        वापस r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        अगर (!nf)
                वापस -ENOMEM;

        अगर (d) {
                अगर (!path_extend(&d, nf))
                        वापस -ENOMEM;

                परिणाम = path_simplify(TAKE_PTR(d));
        } अन्यथा
                परिणाम = TAKE_PTR(nf);

        अगर (!path_is_valid(परिणाम)) /* New path is not valid? (Maybe because too long?) Refuse. */
                वापस -EINVAL;

        *ret = TAKE_PTR(परिणाम);
        वापस 0;
}

पूर्णांक tempfn_xxxxxx(स्थिर अक्षर *p, स्थिर अक्षर *extra, अक्षर **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        वापस tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

पूर्णांक tempfn_random(स्थिर अक्षर *p, स्थिर अक्षर *extra, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;

        निश्चित(p);
        निश्चित(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        अगर (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                वापस -ENOMEM;

        वापस tempfn_build(p, extra, s, /* child= */ false, ret);
}

पूर्णांक tempfn_random_child(स्थिर अक्षर *p, स्थिर अक्षर *extra, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        अगर (!p) {
                r = tmp_dir(&p);
                अगर (r < 0)
                        वापस r;
        }

        अगर (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                वापस -ENOMEM;

        वापस tempfn_build(p, extra, s, /* child= */ true, ret);
}

पूर्णांक open_tmpfile_unlinkable(स्थिर अक्षर *directory, पूर्णांक flags) {
        पूर्णांक r;

        अगर (!directory) {
                r = tmp_dir(&directory);
                अगर (r < 0)
                        वापस r;
        } अन्यथा अगर (isempty(directory))
                वापस -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        पूर्णांक fd = खोलो(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        अगर (fd >= 0)
                वापस fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ अक्षर *p = path_join(directory, "/systemd-tmp-XXXXXX");
        अगर (!p)
                वापस -ENOMEM;

        fd = mkostemp_safe(p);
        अगर (fd < 0)
                वापस fd;

        (व्योम) unlink(p);

        वापस fd;
}

पूर्णांक open_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिर अक्षर *target, पूर्णांक flags, अक्षर **ret_path) {
        पूर्णांक r;

        निश्चित(target);
        निश्चित(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        निश्चित((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        पूर्णांक fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        अगर (fd >= 0) {
                *ret_path = शून्य;
                वापस fd;
        }

        अगर (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ अक्षर *tmp = शून्य;
        r = tempfn_random(target, शून्य, &tmp);
        अगर (r < 0)
                वापस r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        *ret_path = TAKE_PTR(tmp);

        वापस fd;
}

पूर्णांक fopen_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिर अक्षर *target, पूर्णांक flags, अक्षर **ret_path, खाता **ret_file) {
        _cleanup_free_ अक्षर *path = शून्य;
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(target);
        निश्चित(ret_file);
        निश्चित(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        अगर (fd < 0)
                वापस fd;

        f = take_fdopen(&fd, "w");
        अगर (!f)
                वापस -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        वापस 0;
}

पूर्णांक link_tmpfile_at(पूर्णांक fd, पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *target, LinkTmpfileFlags flags) {
        पूर्णांक r;

        निश्चित(fd >= 0);
        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        अगर (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                वापस -त्रुटि_सं;

        अगर (path) {
                अगर (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                अन्यथा
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } अन्यथा {
                अगर (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ शून्य, dir_fd, target);
                अन्यथा
                        r = link_fd(fd, dir_fd, target);
        }
        अगर (r < 0)
                वापस r;

        अगर (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक flink_tmpfile_at(खाता *f, पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *target, LinkTmpfileFlags flags) {
        पूर्णांक fd, r;

        निश्चित(f);
        निश्चित(target);

        fd = fileno(f);
        अगर (fd < 0) /* Not all FILE* objects encapsulate fds */
                वापस -EBADF;

        r = fflush_and_check(f);
        अगर (r < 0)
                वापस r;

        वापस link_tmpfile_at(fd, dir_fd, path, target, flags);
}

पूर्णांक mkdtemp_malloc(स्थिर अक्षर *ढाँचा, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        अगर (ढाँचा)
                p = strdup(ढाँचा);
        अन्यथा {
                स्थिर अक्षर *tmp;

                r = tmp_dir(&tmp);
                अगर (r < 0)
                        वापस r;

                p = path_join(tmp, "XXXXXX");
        }
        अगर (!p)
                वापस -ENOMEM;

        अगर (!mkdtemp(p))
                वापस -त्रुटि_सं;

        *ret = TAKE_PTR(p);
        वापस 0;
}

पूर्णांक mkdtemp_open(स्थिर अक्षर *ढाँचा, पूर्णांक flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक fd, r;

        r = mkdtemp_malloc(ढाँचा, &p);
        अगर (r < 0)
                वापस r;

        fd = RET_NERRNO(खोलो(p, O_DIRECTORY|O_CLOEXEC|flags));
        अगर (fd < 0) {
                (व्योम) सूची_हटाओ(p);
                वापस fd;
        }

        अगर (ret)
                *ret = TAKE_PTR(p);

        वापस fd;
}

व्योम cleanup_tmpfile_तथ्य_done(काष्ठा cleanup_tmpfile_तथ्य *d) {
        निश्चित(d);

        अगर (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                वापस;

        PROTECT_ERRNO;

        (व्योम) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = शून्य;
        d->filename = शून्य;
}
