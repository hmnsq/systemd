/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "random-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "sync-util.h"
#അന്തര്ഭാവ "tmpfile-util.h"
#അന്തര്ഭാവ "umask-util.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ fopen_temporary_internal(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        എങ്കിൽ (r < 0) {
                (ശൂന്യം) unlinkat(dir_fd, path, 0);
                തിരികെ r;
        }

        എങ്കിൽ (ret_file)
                *ret_file = TAKE_PTR(f);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fopen_temporary_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, FILE **ret_file, അക്ഷരം **ret_path) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(path);

        r = tempfn_random(path, ശൂന്യ, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(t);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fopen_temporary_child_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, FILE **ret_file, അക്ഷരം **ret_path) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        എങ്കിൽ (!path) {
                r = tmp_dir(&path);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        r = tempfn_random_child(path, ശൂന്യ, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(t);

        തിരികെ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
പൂർണ്ണസംഖ്യ mkostemp_safe(അക്ഷരം *pattern) {
        പ്രതിജ്ഞാ(pattern);
        BLOCK_WITH_UMASK(0077);
        തിരികെ RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

പൂർണ്ണസംഖ്യ fmkostemp_safe(അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *mode, FILE **ret_f) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        FILE *f;

        പ്രതിജ്ഞാ(ret_f);

        fd = mkostemp_safe(pattern);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        f = take_fdopen(&fd, mode);
        എങ്കിൽ (!f)
                തിരികെ -errno;

        *ret_f = f;
        തിരികെ 0;
}

ശൂന്യം unlink_tempfilep(അക്ഷരം (*p)[]) {
        പ്രതിജ്ഞാ(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        എങ്കിൽ (!endswith(*p, ".XXXXXX"))
                (ശൂന്യം) unlink(*p);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tempfn_build(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *pre, സ്ഥിരം അക്ഷരം *post, bool child, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *d = ശൂന്യ, *fn = ശൂന്യ, *nf = ശൂന്യ, *result = ശൂന്യ;
        size_t len_pre, len_post, len_add;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        എങ്കിൽ (pre && strchr(pre, '/'))
                തിരികെ -EINVAL;

        എങ്കിൽ (post && strchr(post, '/'))
                തിരികെ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        എങ്കിൽ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                തിരികെ -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        എങ്കിൽ (child) {
                d = strdup(p);
                എങ്കിൽ (!d)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ {
                r = path_split_prefix_filename(p, &d, &fn);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        എങ്കിൽ (!nf)
                തിരികെ -ENOMEM;

        എങ്കിൽ (d) {
                എങ്കിൽ (!path_extend(&d, nf))
                        തിരികെ -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } അല്ലെങ്കിൽ
                result = TAKE_PTR(nf);

        എങ്കിൽ (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(result);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ tempfn_xxxxxx(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *extra, അക്ഷരം **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        തിരികെ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

പൂർണ്ണസംഖ്യ tempfn_random(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *extra, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        എങ്കിൽ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                തിരികെ -ENOMEM;

        തിരികെ tempfn_build(p, extra, s, /* child= */ false, ret);
}

പൂർണ്ണസംഖ്യ tempfn_random_child(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *extra, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        എങ്കിൽ (!p) {
                r = tmp_dir(&p);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                തിരികെ -ENOMEM;

        തിരികെ tempfn_build(p, extra, s, /* child= */ true, ret);
}

പൂർണ്ണസംഖ്യ open_tmpfile_unlinkable(സ്ഥിരം അക്ഷരം *directory, പൂർണ്ണസംഖ്യ flags) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!directory) {
                r = tmp_dir(&directory);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        } അല്ലെങ്കിൽ എങ്കിൽ (isempty(directory))
                തിരികെ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        പൂർണ്ണസംഖ്യ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        എങ്കിൽ (fd >= 0)
                തിരികെ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ അക്ഷരം *p = path_join(directory, "/systemd-tmp-XXXXXX");
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        fd = mkostemp_safe(p);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        (ശൂന്യം) unlink(p);

        തിരികെ fd;
}

പൂർണ്ണസംഖ്യ open_tmpfile_linkable_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_path) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(target);
        പ്രതിജ്ഞാ(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        പ്രതിജ്ഞാ((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        പൂർണ്ണസംഖ്യ fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        എങ്കിൽ (fd >= 0) {
                *ret_path = ശൂന്യ;
                തിരികെ fd;
        }

        എങ്കിൽ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ അക്ഷരം *tmp = ശൂന്യ;
        r = tempfn_random(target, ശൂന്യ, &tmp);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        *ret_path = TAKE_PTR(tmp);

        തിരികെ fd;
}

പൂർണ്ണസംഖ്യ fopen_tmpfile_linkable_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_path, FILE **ret_file) {
        _cleanup_free_ അക്ഷരം *path = ശൂന്യ;
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(target);
        പ്രതിജ്ഞാ(ret_file);
        പ്രതിജ്ഞാ(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        f = take_fdopen(&fd, "w");
        എങ്കിൽ (!f)
                തിരികെ -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ link_tmpfile_at(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *target, LinkTmpfileFlags flags) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        എങ്കിൽ (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                തിരികെ -errno;

        എങ്കിൽ (path) {
                എങ്കിൽ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                അല്ലെങ്കിൽ
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ശൂന്യ, dir_fd, target);
                അല്ലെങ്കിൽ
                        r = link_fd(fd, dir_fd, target);
        }
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ flink_tmpfile_at(FILE *f, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *target, LinkTmpfileFlags flags) {
        പൂർണ്ണസംഖ്യ fd, r;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(target);

        fd = fileno(f);
        എങ്കിൽ (fd < 0) /* Not all FILE* objects encapsulate fds */
                തിരികെ -EBADF;

        r = fflush_and_check(f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ link_tmpfile_at(fd, dir_fd, path, target, flags);
}

പൂർണ്ണസംഖ്യ mkdtemp_malloc(സ്ഥിരം അക്ഷരം *template, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (template)
                p = strdup(template);
        അല്ലെങ്കിൽ {
                സ്ഥിരം അക്ഷരം *tmp;

                r = tmp_dir(&tmp);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                p = path_join(tmp, "XXXXXX");
        }
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        എങ്കിൽ (!mkdtemp(p))
                തിരികെ -errno;

        *ret = TAKE_PTR(p);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ mkdtemp_open(സ്ഥിരം അക്ഷരം *template, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ fd, r;

        r = mkdtemp_malloc(template, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        എങ്കിൽ (fd < 0) {
                (ശൂന്യം) rmdir(p);
                തിരികെ fd;
        }

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(p);

        തിരികെ fd;
}

ശൂന്യം cleanup_tmpfile_data_done(ഘടന cleanup_tmpfile_data *d) {
        പ്രതിജ്ഞാ(d);

        എങ്കിൽ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                തിരികെ;

        PROTECT_ERRNO;

        (ശൂന്യം) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ശൂന്യ;
        d->filename = ശൂന്യ;
}
