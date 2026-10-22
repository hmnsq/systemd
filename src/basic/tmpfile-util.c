/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sync-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tmpfile-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umask-util.h"

ਸਥਿਰ ਪੂਰਨ fopen_temporary_ਅੰਦਰੂਨੀ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        fd = openat(dir_fd, ਰਸਤਾ, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ਜੇ (r < 0) {
                (ਖਾਲੀ) unlinkat(dir_fd, ਰਸਤਾ, 0);
                ਵਾਪਸ r;
        }

        ਜੇ (ret_file)
                *ret_file = TAKE_PTR(f);

        ਵਾਪਸ 0;
}

ਪੂਰਨ fopen_temporary_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = tempfn_random(ਰਸਤਾ, ਨਿਰਾ, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = fopen_temporary_ਅੰਦਰੂਨੀ(dir_fd, t, ret_file);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(t);

        ਵਾਪਸ 0;
}

ਪੂਰਨ fopen_temporary_ਬੱਚਾ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ਜੇ (!ਰਸਤਾ) {
                r = tmp_dir(&ਰਸਤਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        r = tempfn_random_ਬੱਚਾ(ਰਸਤਾ, ਨਿਰਾ, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = fopen_temporary_ਅੰਦਰੂਨੀ(dir_fd, t, ret_file);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(t);

        ਵਾਪਸ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ਪੂਰਨ mkostemp_ਸੁਰੱਖਿਅਤ(ਅੱਖਰ *ਨਮੂਨਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਮੂਨਾ);
        ਖੰਡ_WITH_UMASK(0077);
        ਵਾਪਸ RET_NERRNO(mkostemp(ਨਮੂਨਾ, O_CLOEXEC));
}

ਪੂਰਨ fmkostemp_ਸੁਰੱਖਿਅਤ(ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE **ret_f) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        FILE *f;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_f);

        fd = mkostemp_ਸੁਰੱਖਿਅਤ(ਨਮੂਨਾ);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        f = take_fdopen(&fd, ਬਹੁਲਕ);
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        *ret_f = f;
        ਵਾਪਸ 0;
}

ਖਾਲੀ unlink_tempfilep(ਅੱਖਰ (*p)[]) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ਜੇ (!endswith(*p, ".XXXXXX"))
                (ਖਾਲੀ) unlink(*p);
}

ਸਥਿਰ ਪੂਰਨ tempfn_build(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *pre, ਅਚਲ ਅੱਖਰ *post, bool ਬੱਚਾ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *d = ਨਿਰਾ, *fn = ਨਿਰਾ, *nf = ਨਿਰਾ, *ਨਤੀਜਾ = ਨਿਰਾ;
        size_t len_pre, len_post, len_ਜੋੜੋ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        ਜੇ (pre && strchr(pre, '/'))
                ਵਾਪਸ -EINVAL;

        ਜੇ (post && strchr(post, '/'))
                ਵਾਪਸ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ਜੇ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ਵਾਪਸ -EINVAL;

        len_ਜੋੜੋ = len_pre + len_post + STRLEN(".#");

        ਜੇ (ਬੱਚਾ) {
                d = strdup(p);
                ਜੇ (!d)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ {
                r = ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(p, &d, &fn);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_ਜੋੜੋ);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ਜੇ (!nf)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (d) {
                ਜੇ (!ਰਸਤਾ_extend(&d, nf))
                        ਵਾਪਸ -ENOMEM;

                ਨਤੀਜਾ = ਰਸਤਾ_simplify(TAKE_PTR(d));
        } ਨਹੀਂਤਾਂ
                ਨਤੀਜਾ = TAKE_PTR(nf);

        ਜੇ (!ਰਸਤਾ_is_valid(ਨਤੀਜਾ)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(ਨਤੀਜਾ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ tempfn_xxxxxx(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *extra, ਅੱਖਰ **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ਵਾਪਸ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ਪੂਰਨ tempfn_random(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *extra, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        ਜੇ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ tempfn_build(p, extra, s, /* child= */ false, ret);
}

ਪੂਰਨ tempfn_random_ਬੱਚਾ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *extra, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ਜੇ (!p) {
                r = tmp_dir(&p);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ tempfn_build(p, extra, s, /* child= */ true, ret);
}

ਪੂਰਨ open_tmpfile_unlinkable(ਅਚਲ ਅੱਖਰ *directory, ਪੂਰਨ flags) {
        ਪੂਰਨ r;

        ਜੇ (!directory) {
                r = tmp_dir(&directory);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        } ਨਹੀਂਤਾਂ ਜੇ (isempty(directory))
                ਵਾਪਸ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ਪੂਰਨ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ਜੇ (fd >= 0)
                ਵਾਪਸ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ ਅੱਖਰ *p = ਰਸਤਾ_join(directory, "/systemd-tmp-XXXXXX");
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        fd = mkostemp_ਸੁਰੱਖਿਅਤ(p);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        (ਖਾਲੀ) unlink(p);

        ਵਾਪਸ fd;
}

ਪੂਰਨ open_tmpfile_linkable_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਰਸਤਾ);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ਪੂਰਨ fd = open_ਮਾਪਾ_at(dir_fd, ਨਿਸ਼ਾਨਾ, O_TMPFILE|flags, 0640);
        ਜੇ (fd >= 0) {
                *ret_ਰਸਤਾ = ਨਿਰਾ;
                ਵਾਪਸ fd;
        }

        ਜੇ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", ਨਿਸ਼ਾਨਾ);

        _cleanup_free_ ਅੱਖਰ *tmp = ਨਿਰਾ;
        r = tempfn_random(ਨਿਸ਼ਾਨਾ, ਨਿਰਾ, &tmp);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        *ret_ਰਸਤਾ = TAKE_PTR(tmp);

        ਵਾਪਸ fd;
}

ਪੂਰਨ fopen_tmpfile_linkable_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret_ਰਸਤਾ, FILE **ret_file) {
        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_file);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਰਸਤਾ);

        fd = open_tmpfile_linkable_at(dir_fd, ਨਿਸ਼ਾਨਾ, flags, &ਰਸਤਾ);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        f = take_fdopen(&fd, "w");
        ਜੇ (!f)
                ਵਾਪਸ -ENOMEM;

        *ret_ਰਸਤਾ = TAKE_PTR(ਰਸਤਾ);
        *ret_file = TAKE_PTR(f);
        ਵਾਪਸ 0;
}

ਪੂਰਨ link_tmpfile_at(ਪੂਰਨ fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, LinkTmpfileFlags flags) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ਜੇ (FLAGS_ਸੈੱਟ(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ਰਸਤਾ) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, ਰਸਤਾ, dir_fd, ਨਿਸ਼ਾਨਾ));
                ਨਹੀਂਤਾਂ
                        r = rename_noreplace(dir_fd, ਰਸਤਾ, dir_fd, ਨਿਸ਼ਾਨਾ);
        } ਨਹੀਂਤਾਂ {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ਨਿਰਾ, dir_fd, ਨਿਸ਼ਾਨਾ);
                ਨਹੀਂਤਾਂ
                        r = link_fd(fd, dir_fd, ਨਿਸ਼ਾਨਾ);
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_ਭਰਿਆ(fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ flink_tmpfile_at(FILE *f, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, LinkTmpfileFlags flags) {
        ਪੂਰਨ fd, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);

        fd = fileno(f);
        ਜੇ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ਵਾਪਸ -EBADF;

        r = fflush_and_check(f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ link_tmpfile_at(fd, dir_fd, ਰਸਤਾ, ਨਿਸ਼ਾਨਾ, flags);
}

ਪੂਰਨ mkdtemp_malloc(ਅਚਲ ਅੱਖਰ *ਸਾਂਚਾ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (ਸਾਂਚਾ)
                p = strdup(ਸਾਂਚਾ);
        ਨਹੀਂਤਾਂ {
                ਅਚਲ ਅੱਖਰ *tmp;

                r = tmp_dir(&tmp);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                p = ਰਸਤਾ_join(tmp, "XXXXXX");
        }
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (!mkdtemp(p))
                ਵਾਪਸ -errno;

        *ret = TAKE_PTR(p);
        ਵਾਪਸ 0;
}

ਪੂਰਨ mkdtemp_open(ਅਚਲ ਅੱਖਰ *ਸਾਂਚਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ fd, r;

        r = mkdtemp_malloc(ਸਾਂਚਾ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ਜੇ (fd < 0) {
                (ਖਾਲੀ) rmdir(p);
                ਵਾਪਸ fd;
        }

        ਜੇ (ret)
                *ret = TAKE_PTR(p);

        ਵਾਪਸ fd;
}

ਖਾਲੀ cleanup_tmpfile_ਅੰਕੜੇ_ਪੂਰਾ(ਬਣਤਰ cleanup_tmpfile_ਅੰਕੜੇ *d) {
        ਪ੍ਰਤਿਜ੍ਞਾ(d);

        ਜੇ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ਵਾਪਸ;

        PROTECT_ERRNO;

        (ਖਾਲੀ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ਨਿਰਾ;
        d->filename = ਨਿਰਾ;
}
