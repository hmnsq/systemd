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

स्थिर पूर्णाङ्क fopen_temporary_internal(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        सचे (r < 0) {
                (शून्य) unlinkat(dir_fd, path, 0);
                पटिनिवत्त r;
        }

        सचे (ret_file)
                *ret_file = TAKE_PTR(f);

        पटिनिवत्त 0;
}

पूर्णाङ्क fopen_temporary_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, FILE **ret_file, वर्ण **ret_path) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        r = tempfn_random(path, रिक्त, &t);
        सचे (r < 0)
                पटिनिवत्त r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret_path)
                *ret_path = TAKE_PTR(t);

        पटिनिवत्त 0;
}

पूर्णाङ्क fopen_temporary_child_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, FILE **ret_file, वर्ण **ret_path) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        सचे (!path) {
                r = tmp_dir(&path);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        r = tempfn_random_child(path, रिक्त, &t);
        सचे (r < 0)
                पटिनिवत्त r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret_path)
                *ret_path = TAKE_PTR(t);

        पटिनिवत्त 0;
}

/* This is much like mkostemp() but is subject to umask(). */
पूर्णाङ्क mkostemp_safe(वर्ण *pattern) {
        प्रतिज्ञा(pattern);
        BLOCK_WITH_UMASK(0077);
        पटिनिवत्त RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

पूर्णाङ्क fmkostemp_safe(वर्ण *pattern, ध्रुव वर्ण *mode, FILE **ret_f) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        FILE *f;

        प्रतिज्ञा(ret_f);

        fd = mkostemp_safe(pattern);
        सचे (fd < 0)
                पटिनिवत्त fd;

        f = take_fdopen(&fd, mode);
        सचे (!f)
                पटिनिवत्त -errno;

        *ret_f = f;
        पटिनिवत्त 0;
}

शून्य unlink_tempfilep(वर्ण (*p)[]) {
        प्रतिज्ञा(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        सचे (!endswith(*p, ".XXXXXX"))
                (शून्य) unlink(*p);
}

स्थिर पूर्णाङ्क tempfn_build(ध्रुव वर्ण *p, ध्रुव वर्ण *pre, ध्रुव वर्ण *post, bool child, वर्ण **ret) {
        _cleanup_free_ वर्ण *d = रिक्त, *fn = रिक्त, *nf = रिक्त, *result = रिक्त;
        size_t len_pre, len_post, len_add;
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

        सचे (pre && strchr(pre, '/'))
                पटिनिवत्त -EINVAL;

        सचे (post && strchr(post, '/'))
                पटिनिवत्त -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        सचे (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                पटिनिवत्त -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        सचे (child) {
                d = strdup(p);
                सचे (!d)
                        पटिनिवत्त -ENOMEM;
        } अञ्ञथा {
                r = path_split_prefix_filename(p, &d, &fn);
                सचे (r < 0)
                        पटिनिवत्त r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        सचे (!nf)
                पटिनिवत्त -ENOMEM;

        सचे (d) {
                सचे (!path_extend(&d, nf))
                        पटिनिवत्त -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } अञ्ञथा
                result = TAKE_PTR(nf);

        सचे (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(result);
        पटिनिवत्त 0;
}

पूर्णाङ्क tempfn_xxxxxx(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        पटिनिवत्त tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

पूर्णाङ्क tempfn_random(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret) {
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

        सचे (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त tempfn_build(p, extra, s, /* child= */ false, ret);
}

पूर्णाङ्क tempfn_random_child(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        सचे (!p) {
                r = tmp_dir(&p);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त tempfn_build(p, extra, s, /* child= */ true, ret);
}

पूर्णाङ्क open_tmpfile_unlinkable(ध्रुव वर्ण *directory, पूर्णाङ्क flags) {
        पूर्णाङ्क r;

        सचे (!directory) {
                r = tmp_dir(&directory);
                सचे (r < 0)
                        पटिनिवत्त r;
        } अञ्ञथा सचे (isempty(directory))
                पटिनिवत्त -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        पूर्णाङ्क fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        सचे (fd >= 0)
                पटिनिवत्त fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ वर्ण *p = path_join(directory, "/systemd-tmp-XXXXXX");
        सचे (!p)
                पटिनिवत्त -ENOMEM;

        fd = mkostemp_safe(p);
        सचे (fd < 0)
                पटिनिवत्त fd;

        (शून्य) unlink(p);

        पटिनिवत्त fd;
}

पूर्णाङ्क open_tmpfile_linkable_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path) {
        पूर्णाङ्क r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        प्रतिज्ञा((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        पूर्णाङ्क fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        सचे (fd >= 0) {
                *ret_path = रिक्त;
                पटिनिवत्त fd;
        }

        सचे (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ वर्ण *tmp = रिक्त;
        r = tempfn_random(target, रिक्त, &tmp);
        सचे (r < 0)
                पटिनिवत्त r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        *ret_path = TAKE_PTR(tmp);

        पटिनिवत्त fd;
}

पूर्णाङ्क fopen_tmpfile_linkable_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path, FILE **ret_file) {
        _cleanup_free_ वर्ण *path = रिक्त;
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(target);
        प्रतिज्ञा(ret_file);
        प्रतिज्ञा(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        सचे (fd < 0)
                पटिनिवत्त fd;

        f = take_fdopen(&fd, "w");
        सचे (!f)
                पटिनिवत्त -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        पटिनिवत्त 0;
}

पूर्णाङ्क link_tmpfile_at(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *target, LinkTmpfileFlags flags) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        सचे (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                पटिनिवत्त -errno;

        सचे (path) {
                सचे (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                अञ्ञथा
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } अञ्ञथा {
                सचे (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ रिक्त, dir_fd, target);
                अञ्ञथा
                        r = link_fd(fd, dir_fd, target);
        }
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क flink_tmpfile_at(FILE *f, पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *target, LinkTmpfileFlags flags) {
        पूर्णाङ्क fd, r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(target);

        fd = fileno(f);
        सचे (fd < 0) /* Not all FILE* objects encapsulate fds */
                पटिनिवत्त -EBADF;

        r = fflush_and_check(f);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त link_tmpfile_at(fd, dir_fd, path, target, flags);
}

पूर्णाङ्क mkdtemp_malloc(ध्रुव वर्ण *template, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        सचे (template)
                p = strdup(template);
        अञ्ञथा {
                ध्रुव वर्ण *tmp;

                r = tmp_dir(&tmp);
                सचे (r < 0)
                        पटिनिवत्त r;

                p = path_join(tmp, "XXXXXX");
        }
        सचे (!p)
                पटिनिवत्त -ENOMEM;

        सचे (!mkdtemp(p))
                पटिनिवत्त -errno;

        *ret = TAKE_PTR(p);
        पटिनिवत्त 0;
}

पूर्णाङ्क mkdtemp_open(ध्रुव वर्ण *template, पूर्णाङ्क flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क fd, r;

        r = mkdtemp_malloc(template, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        सचे (fd < 0) {
                (शून्य) rmdir(p);
                पटिनिवत्त fd;
        }

        सचे (ret)
                *ret = TAKE_PTR(p);

        पटिनिवत्त fd;
}

शून्य cleanup_tmpfile_data_done(संरचना cleanup_tmpfile_data *d) {
        प्रतिज्ञा(d);

        सचे (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                पटिनिवत्त;

        PROTECT_ERRNO;

        (शून्य) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = रिक्त;
        d->filename = रिक्त;
}
