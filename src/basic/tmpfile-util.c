/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "log.h"
#include "path-util.h"
#include "random-util.h"
#include "string-util.h"
#include "sync-util.h"
#include "tmpfile-util.h"
#include "umask-util.h"

static จำนวนเต็ม fopen_temporary_internal(จำนวนเต็ม dir_fd, const var_na *path, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        จำนวนเต็ม r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        fd = openat(dir_fd, path, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ถ้า (fd < 0)
                คืน -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ถ้า (r < 0) {
                (void) unlinkat(dir_fd, path, 0);
                คืน r;
        }

        ถ้า (ret_file)
                *ret_file = TAKE_PTR(f);

        คืน 0;
}

จำนวนเต็ม fopen_temporary_at(จำนวนเต็ม dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        จำนวนเต็ม r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = tempfn_random(path, NULL, &t);
        ถ้า (r < 0)
                คืน r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_path)
                *ret_path = TAKE_PTR(t);

        คืน 0;
}

จำนวนเต็ม fopen_temporary_child_at(จำนวนเต็ม dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path) {
        _cleanup_free_ var_na *t = NULL;
        จำนวนเต็ม r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ถ้า (!path) {
                r = tmp_dir(&path);
                ถ้า (r < 0)
                        คืน r;
        }

        r = tempfn_random_child(path, NULL, &t);
        ถ้า (r < 0)
                คืน r;

        r = fopen_temporary_internal(dir_fd, t, ret_file);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_path)
                *ret_path = TAKE_PTR(t);

        คืน 0;
}

/* This is much like mkostemp() but is subject to umask(). */
จำนวนเต็ม mkostemp_safe(var_na *pattern) {
        assert(pattern);
        BLOCK_WITH_UMASK(0077);
        คืน RET_NERRNO(mkostemp(pattern, O_CLOEXEC));
}

จำนวนเต็ม fmkostemp_safe(var_na *pattern, const var_na *mode, FILE **ret_f) {
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        FILE *f;

        assert(ret_f);

        fd = mkostemp_safe(pattern);
        ถ้า (fd < 0)
                คืน fd;

        f = take_fdopen(&fd, mode);
        ถ้า (!f)
                คืน -errno;

        *ret_f = f;
        คืน 0;
}

void unlink_tempfilep(var_na (*p)[]) {
        assert(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ถ้า (!endswith(*p, ".XXXXXX"))
                (void) unlink(*p);
}

static จำนวนเต็ม tempfn_build(const var_na *p, const var_na *pre, const var_na *post, bool child, var_na **ret) {
        _cleanup_free_ var_na *d = NULL, *fn = NULL, *nf = NULL, *result = NULL;
        size_t len_pre, len_post, len_add;
        จำนวนเต็ม r;

        assert(p);
        assert(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        ถ้า (pre && strchr(pre, '/'))
                คืน -EINVAL;

        ถ้า (post && strchr(post, '/'))
                คืน -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ถ้า (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                คืน -EINVAL;

        len_add = len_pre + len_post + STRLEN(".#");

        ถ้า (child) {
                d = strdup(p);
                ถ้า (!d)
                        คืน -ENOMEM;
        } มิฉะนั้น {
                r = path_split_prefix_filename(p, &d, &fn);
                ถ้า (r < 0)
                        คืน r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_add);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ถ้า (!nf)
                คืน -ENOMEM;

        ถ้า (d) {
                ถ้า (!path_extend(&d, nf))
                        คืน -ENOMEM;

                result = path_simplify(TAKE_PTR(d));
        } มิฉะนั้น
                result = TAKE_PTR(nf);

        ถ้า (!path_is_valid(result)) /* New path is not valid? (Maybe because too long?) Refuse. */
                คืน -EINVAL;

        *ret = TAKE_PTR(result);
        คืน 0;
}

จำนวนเต็ม tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        คืน tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

จำนวนเต็ม tempfn_random(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;

        assert(p);
        assert(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        ถ้า (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                คืน -ENOMEM;

        คืน tempfn_build(p, extra, s, /* child= */ false, ret);
}

จำนวนเต็ม tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r;

        assert(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ถ้า (!p) {
                r = tmp_dir(&p);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                คืน -ENOMEM;

        คืน tempfn_build(p, extra, s, /* child= */ true, ret);
}

จำนวนเต็ม open_tmpfile_unlinkable(const var_na *directory, จำนวนเต็ม flags) {
        จำนวนเต็ม r;

        ถ้า (!directory) {
                r = tmp_dir(&directory);
                ถ้า (r < 0)
                        คืน r;
        } มิฉะนั้น ถ้า (isempty(directory))
                คืน -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        จำนวนเต็ม fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ถ้า (fd >= 0)
                คืน fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ var_na *p = path_join(directory, "/systemd-tmp-XXXXXX");
        ถ้า (!p)
                คืน -ENOMEM;

        fd = mkostemp_safe(p);
        ถ้า (fd < 0)
                คืน fd;

        (void) unlink(p);

        คืน fd;
}

จำนวนเต็ม open_tmpfile_linkable_at(จำนวนเต็ม dir_fd, const var_na *target, จำนวนเต็ม flags, var_na **ret_path) {
        จำนวนเต็ม r;

        assert(target);
        assert(ret_path);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        assert((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        จำนวนเต็ม fd = open_parent_at(dir_fd, target, O_TMPFILE|flags, 0640);
        ถ้า (fd >= 0) {
                *ret_path = NULL;
                คืน fd;
        }

        ถ้า (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", target);

        _cleanup_free_ var_na *tmp = NULL;
        r = tempfn_random(target, NULL, &tmp);
        ถ้า (r < 0)
                คืน r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ถ้า (fd < 0)
                คืน -errno;

        *ret_path = TAKE_PTR(tmp);

        คืน fd;
}

จำนวนเต็ม fopen_tmpfile_linkable_at(จำนวนเต็ม dir_fd, const var_na *target, จำนวนเต็ม flags, var_na **ret_path, FILE **ret_file) {
        _cleanup_free_ var_na *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;

        assert(target);
        assert(ret_file);
        assert(ret_path);

        fd = open_tmpfile_linkable_at(dir_fd, target, flags, &path);
        ถ้า (fd < 0)
                คืน fd;

        f = take_fdopen(&fd, "w");
        ถ้า (!f)
                คืน -ENOMEM;

        *ret_path = TAKE_PTR(path);
        *ret_file = TAKE_PTR(f);
        คืน 0;
}

จำนวนเต็ม link_tmpfile_at(จำนวนเต็ม fd, จำนวนเต็ม dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        จำนวนเต็ม r;

        assert(fd >= 0);
        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(target);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ถ้า (FLAGS_SET(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                คืน -errno;

        ถ้า (path) {
                ถ้า (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, path, dir_fd, target));
                มิฉะนั้น
                        r = rename_noreplace(dir_fd, path, dir_fd, target);
        } มิฉะนั้น {
                ถ้า (FLAGS_SET(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ NULL, dir_fd, target);
                มิฉะนั้น
                        r = link_fd(fd, dir_fd, target);
        }
        ถ้า (r < 0)
                คืน r;

        ถ้า (FLAGS_SET(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_full(fd);
                ถ้า (r < 0)
                        คืน r;
        }

        คืน 0;
}

จำนวนเต็ม flink_tmpfile_at(FILE *f, จำนวนเต็ม dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        จำนวนเต็ม fd, r;

        assert(f);
        assert(target);

        fd = fileno(f);
        ถ้า (fd < 0) /* Not all FILE* objects encapsulate fds */
                คืน -EBADF;

        r = fflush_and_check(f);
        ถ้า (r < 0)
                คืน r;

        คืน link_tmpfile_at(fd, dir_fd, path, target, flags);
}

จำนวนเต็ม mkdtemp_malloc(const var_na *template, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม r;

        assert(ret);

        ถ้า (template)
                p = strdup(template);
        มิฉะนั้น {
                const var_na *tmp;

                r = tmp_dir(&tmp);
                ถ้า (r < 0)
                        คืน r;

                p = path_join(tmp, "XXXXXX");
        }
        ถ้า (!p)
                คืน -ENOMEM;

        ถ้า (!mkdtemp(p))
                คืน -errno;

        *ret = TAKE_PTR(p);
        คืน 0;
}

จำนวนเต็ม mkdtemp_open(const var_na *template, จำนวนเต็ม flags, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม fd, r;

        r = mkdtemp_malloc(template, &p);
        ถ้า (r < 0)
                คืน r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ถ้า (fd < 0) {
                (void) rmdir(p);
                คืน fd;
        }

        ถ้า (ret)
                *ret = TAKE_PTR(p);

        คืน fd;
}

void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d) {
        assert(d);

        ถ้า (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                คืน;

        PROTECT_ERRNO;

        (void) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = NULL;
        d->filename = NULL;
}
