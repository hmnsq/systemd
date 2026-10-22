/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "random-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "sync-util.h"
#અન્તર્ભાવ "tmpfile-util.h"
#અન્તર્ભાવ "umask-util.h"

સ્થિર પૂર્ણાંક fopen_temporary_internal(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        જો (fd < 0)
                પાછા -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        જો (r < 0) {
                (શૂન્ય) unlinkat(dir_fd, path, 0);
                પાછા r;
        }

        જો (ret_file)
                *ret_file = TAKE_PTR(f);

        પાછા 0;
}

પૂર્ણાંક fopen_temporary_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, FILE **ret_file, અક્ષર **ret_path) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(path);

        r = tempfn_random(path, ખાલી, &t);
        જો (r < 0)
                પાછા r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        જો (r < 0)
                પાછા r;

        જો (ret_path)
                *ret_path = TAKE_PTR(t);

        પાછા 0;
}

પૂર્ણાંક fopen_temporary_child_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, FILE **ret_file, અક્ષર **ret_path) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);

        જો (!path) {
                r = tmp_dir(&path);
                જો (r < 0)
                        પાછા r;
        }

        r = tempfn_random_child(path, ખાલી, &t);
        જો (r < 0)
                પાછા r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        જો (r < 0)
                પાછા r;

        જો (ret_path)
                *ret_path = TAKE_PTR(t);

        પાછા 0;
}

/* This is much like mkostemp() but is subject to umask(). */
પૂર્ણાંક mkostemp_safe(અક્ષર *pattern) {
        પ્રતિજ્ઞા(pattern);
        BLOCK_WITH_UMASK(0077);
        પાછા RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

પૂર્ણાંક fmkostemp_safe(અક્ષર *pattern, અચલ અક્ષર *mode, FILE **ret_f) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        FILE *f;

        પ્રતિજ્ઞા(ret_f);

        fd = mkostemp_safe(pattern);
        જો (fd < 0)
                પાછા fd;

        f = take_fdopen(&fd, mode);
        જો (!f)
                પાછા -errno;

        *ret_f = f;
        પાછા 0;
}

શૂન્ય unlink_tempfilep(અક્ષર (*p)[]) {
        પ્રતિજ્ઞા(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        જો (!endswith(*p, ".XXXXXX"))
                (શૂન્ય) unlink(*p);
}

સ્થિર પૂર્ણાંક tempfn_build(અચલ અક્ષર *p, અચલ અક્ષર *pre, અચલ અક્ષર *post, bool child, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *d = ખાલી, *fn = ખાલી, *nf = ખાલી, *result = ખાલી;
        size_t len_pre, len_post, len_add;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        જો (pre && strchr(pre, '/'))
                પાછા -EINVAL;

        જો (post && strchr(post, '/'))
                પાછા -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        જો (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                પાછા -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        જો (child) {
                d = strdup(p);
                જો (!d)
                        પાછા -ENOMEM;
        } નહીંતર {
                r = path_split_prefix_filename(p, &d, &fn);
                જો (r < 0)
                        પાછા r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        જો (!nf)
                પાછા -ENOMEM;

        જો (d) {
                જો (!path_extend(&d, nf))
                        પાછા -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } નહીંતર
                result = TAKE_PTR(nf);

        જો (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                પાછા -EINVAL;

        *ret = TAKE_PTR(result);
        પાછા 0;
}

પૂર્ણાંક tempfn_xxxxxx(અચલ અક્ષર *p, અચલ અક્ષર *extra, અક્ષર **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        પાછા tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

પૂર્ણાંક tempfn_random(અચલ અક્ષર *p, અચલ અક્ષર *extra, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        જો (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                પાછા -ENOMEM;

        પાછા tempfn_build(p, extra, s, /* child= */ false, ret);
}

પૂર્ણાંક tempfn_random_child(અચલ અક્ષર *p, અચલ અક્ષર *extra, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        જો (!p) {
                r = tmp_dir(&p);
                જો (r < 0)
                        પાછા r;
        }

        જો (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                પાછા -ENOMEM;

        પાછા tempfn_build(p, extra, s, /* child= */ true, ret);
}

પૂર્ણાંક open_tmpfile_unlinkable(અચલ અક્ષર *directory, પૂર્ણાંક flags) {
        પૂર્ણાંક r;

        જો (!directory) {
                r = tmp_dir(&directory);
                જો (r < 0)
                        પાછા r;
        } નહીંતર જો (isempty(directory))
                પાછા -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        પૂર્ણાંક fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        જો (fd >= 0)
                પાછા fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ અક્ષર *p = path_join(directory, "/systemd-tmp-XXXXXX");
        જો (!p)
                પાછા -ENOMEM;

        fd = mkostemp_safe(p);
        જો (fd < 0)
                પાછા fd;

        (શૂન્ય) unlink(p);

        પાછા fd;
}

પૂર્ણાંક open_tmpfile_linkable_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *target, પૂર્ણાંક flags, અક્ષર **ret_path) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(target);
        પ્રતિજ્ઞા(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        પ્રતિજ્ઞા((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        પૂર્ણાંક fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        જો (fd >= 0) {
                *ret_path = ખાલી;
                પાછા fd;
        }

        જો (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ અક્ષર *tmp = ખાલી;
        r = tempfn_random(target, ખાલી, &tmp);
        જો (r < 0)
                પાછા r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        જો (fd < 0)
                પાછા -errno;

        *ret_path = TAKE_PTR(tmp);

        પાછા fd;
}

પૂર્ણાંક fopen_tmpfile_linkable_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *target, પૂર્ણાંક flags, અક્ષર **ret_path, FILE **ret_file) {
        _cleanup_free_ અક્ષર *path = ખાલી;
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(target);
        પ્રતિજ્ઞા(ret_file);
        પ્રતિજ્ઞા(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        જો (fd < 0)
                પાછા fd;

        f = take_fdopen(&fd, "w");
        જો (!f)
                પાછા -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        પાછા 0;
}

પૂર્ણાંક link_tmpfile_at(પૂર્ણાંક fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *target, LinkTmpfileFlags flags) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        જો (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                પાછા -errno;

        જો (path) {
                જો (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                નહીંતર
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } નહીંતર {
                જો (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ખાલી, dir_fd, target);
                નહીંતર
                        r = link_fd(fd, dir_fd, target);
        }
        જો (r < 0)
                પાછા r;

        જો (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક flink_tmpfile_at(FILE *f, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *target, LinkTmpfileFlags flags) {
        પૂર્ણાંક fd, r;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(target);

        fd = fileno(f);
        જો (fd < 0) /* Not all FILE* objects encapsulate fds */
                પાછા -EBADF;

        r = fflush_and_check(f);
        જો (r < 0)
                પાછા r;

        પાછા link_tmpfile_at(fd, dir_fd, path, target, flags);
}

પૂર્ણાંક mkdtemp_malloc(અચલ અક્ષર *template, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        જો (template)
                p = strdup(template);
        નહીંતર {
                અચલ અક્ષર *tmp;

                r = tmp_dir(&tmp);
                જો (r < 0)
                        પાછા r;

                p = path_join(tmp, "XXXXXX");
        }
        જો (!p)
                પાછા -ENOMEM;

        જો (!mkdtemp(p))
                પાછા -errno;

        *ret = TAKE_PTR(p);
        પાછા 0;
}

પૂર્ણાંક mkdtemp_open(અચલ અક્ષર *template, પૂર્ણાંક flags, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક fd, r;

        r = mkdtemp_malloc(template, &p);
        જો (r < 0)
                પાછા r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        જો (fd < 0) {
                (શૂન્ય) rmdir(p);
                પાછા fd;
        }

        જો (ret)
                *ret = TAKE_PTR(p);

        પાછા fd;
}

શૂન્ય cleanup_tmpfile_data_done(સંરચના cleanup_tmpfile_data *d) {
        પ્રતિજ્ઞા(d);

        જો (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                પાછા;

        PROTECT_ERRNO;

        (શૂન્ય) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ખાલી;
        d->filename = ખાલી;
}
