/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "sync-util.h"
#ಅನ್ತರ್ಭಾವ "tmpfile-util.h"
#ಅನ್ತರ್ಭಾವ "umask-util.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fopen_temporary_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        fd = openat(dir_fd, ಮಾರ್ಗ, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ಆದರೆ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(dir_fd, ಮಾರ್ಗ, 0);
                ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_file)
                *ret_file = TAKE_PTR(f);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fopen_temporary_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = tempfn_random(ಮಾರ್ಗ, ಖಾಲಿ, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = fopen_temporary_ಆಂತರಿಕ(dir_fd, t, ret_file);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(t);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fopen_temporary_ಮಗು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ಆದರೆ (!ಮಾರ್ಗ) {
                r = tmp_dir(&ಮಾರ್ಗ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        r = tempfn_random_ಮಗು(ಮಾರ್ಗ, ಖಾಲಿ, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = fopen_temporary_ಆಂತರಿಕ(dir_fd, t, ret_file);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(t);

        ಹಿಂತಿರುಗಿ 0;
}

/* This is much like mkostemp() but is subject to umask(). */
ಪೂರ್ಣಾಂಕ mkostemp_ಸುರಕ್ಷಿತ(ಅಕ್ಷರ *ನಮೂನೆ) {
        ಪ್ರತಿಜ್ಞಾ(ನಮೂನೆ);
        ಖಂಡ_WITH_UMASK(0077);
        ಹಿಂತಿರುಗಿ RET_NERRNO(mkostemp(ನಮೂನೆ, O_CLOEXEC));
}

ಪೂರ್ಣಾಂಕ fmkostemp_ಸುರಕ್ಷಿತ(ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE **ret_f) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        FILE *f;

        ಪ್ರತಿಜ್ಞಾ(ret_f);

        fd = mkostemp_ಸುರಕ್ಷಿತ(ನಮೂನೆ);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        f = take_fdopen(&fd, ಬಹುಲಕ);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        *ret_f = f;
        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ unlink_tempfilep(ಅಕ್ಷರ (*p)[]) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ಆದರೆ (!endswith(*p, ".XXXXXX"))
                (ಶೂನ್ಯ) unlink(*p);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tempfn_build(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pre, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *post, bool ಮಗು, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *d = ಖಾಲಿ, *fn = ಖಾಲಿ, *nf = ಖಾಲಿ, *ಫಲಿತಾಂಶ = ಖಾಲಿ;
        size_t len_pre, len_post, len_ಕೂಡಿಸು;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        ಆದರೆ (pre && strchr(pre, '/'))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (post && strchr(post, '/'))
                ಹಿಂತಿರುಗಿ -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ಆದರೆ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                ಹಿಂತಿರುಗಿ -EINVAL;

        len_ಕೂಡಿಸು = len_pre + len_post + STRLEN(".#");

        ಆದರೆ (ಮಗು) {
                d = strdup(p);
                ಆದರೆ (!d)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(p, &d, &fn);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_ಕೂಡಿಸು);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ಆದರೆ (!nf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (d) {
                ಆದರೆ (!ಮಾರ್ಗ_extend(&d, nf))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಫಲಿತಾಂಶ = ಮಾರ್ಗ_simplify(TAKE_PTR(d));
        } ಇಲ್ಲದಿದ್ದರೆ
                ಫಲಿತಾಂಶ = TAKE_PTR(nf);

        ಆದರೆ (!ಮಾರ್ಗ_is_valid(ಫಲಿತಾಂಶ)) /* New path is not valid? (Maybe because too long?) Refuse. */
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ tempfn_xxxxxx(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra, ಅಕ್ಷರ **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        ಹಿಂತಿರುಗಿ tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

ಪೂರ್ಣಾಂಕ tempfn_random(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        ಆದರೆ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ tempfn_build(p, extra, s, /* child= */ false, ret);
}

ಪೂರ್ಣಾಂಕ tempfn_random_ಮಗು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ಆದರೆ (!p) {
                r = tmp_dir(&p);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ tempfn_build(p, extra, s, /* child= */ true, ret);
}

ಪೂರ್ಣಾಂಕ open_tmpfile_unlinkable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *directory, ಪೂರ್ಣಾಂಕ flags) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!directory) {
                r = tmp_dir(&directory);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isempty(directory))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        ಪೂರ್ಣಾಂಕ fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ಆದರೆ (fd >= 0)
                ಹಿಂತಿರುಗಿ fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ ಅಕ್ಷರ *p = ಮಾರ್ಗ_join(directory, "/systemd-tmp-XXXXXX");
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        fd = mkostemp_ಸುರಕ್ಷಿತ(p);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        (ಶೂನ್ಯ) unlink(p);

        ಹಿಂತಿರುಗಿ fd;
}

ಪೂರ್ಣಾಂಕ open_tmpfile_linkable_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);
        ಪ್ರತಿಜ್ಞಾ(ret_ಮಾರ್ಗ);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        ಪ್ರತಿಜ್ಞಾ((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        ಪೂರ್ಣಾಂಕ fd = open_ಪೋಷಕ_at(dir_fd, ಗುರಿ, O_TMPFILE|flags, 0640);
        ಆದರೆ (fd >= 0) {
                *ret_ಮಾರ್ಗ = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ fd;
        }

        ಆದರೆ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", ಗುರಿ);

        _cleanup_free_ ಅಕ್ಷರ *tmp = ಖಾಲಿ;
        r = tempfn_random(ಗುರಿ, ಖಾಲಿ, &tmp);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        *ret_ಮಾರ್ಗ = TAKE_PTR(tmp);

        ಹಿಂತಿರುಗಿ fd;
}

ಪೂರ್ಣಾಂಕ fopen_tmpfile_linkable_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, FILE **ret_file) {
        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);
        ಪ್ರತಿಜ್ಞಾ(ret_file);
        ಪ್ರತಿಜ್ಞಾ(ret_ಮಾರ್ಗ);

        fd = open_tmpfile_linkable_at(dir_fd, ಗುರಿ, flags, &ಮಾರ್ಗ);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        f = take_fdopen(&fd, "w");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret_ಮಾರ್ಗ = TAKE_PTR(ಮಾರ್ಗ);
        *ret_file = TAKE_PTR(f);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ link_tmpfile_at(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, LinkTmpfileFlags flags) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ಆದರೆ (FLAGS_ಗಣ(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ಮಾರ್ಗ) {
                ಆದರೆ (FLAGS_ಗಣ(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, ಮಾರ್ಗ, dir_fd, ಗುರಿ));
                ಇಲ್ಲದಿದ್ದರೆ
                        r = rename_noreplace(dir_fd, ಮಾರ್ಗ, dir_fd, ಗುರಿ);
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (FLAGS_ಗಣ(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ಖಾಲಿ, dir_fd, ಗುರಿ);
                ಇಲ್ಲದಿದ್ದರೆ
                        r = link_fd(fd, dir_fd, ಗುರಿ);
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (FLAGS_ಗಣ(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_ತುಂಬಿದ(fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ flink_tmpfile_at(FILE *f, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, LinkTmpfileFlags flags) {
        ಪೂರ್ಣಾಂಕ fd, r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);

        fd = fileno(f);
        ಆದರೆ (fd < 0) /* Not all FILE* objects encapsulate fds */
                ಹಿಂತಿರುಗಿ -EBADF;

        r = fflush_and_check(f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ link_tmpfile_at(fd, dir_fd, ಮಾರ್ಗ, ಗುರಿ, flags);
}

ಪೂರ್ಣಾಂಕ mkdtemp_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾದರಿ, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (ಮಾದರಿ)
                p = strdup(ಮಾದರಿ);
        ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tmp;

                r = tmp_dir(&tmp);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                p = ಮಾರ್ಗ_join(tmp, "XXXXXX");
        }
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (!mkdtemp(p))
                ಹಿಂತಿರುಗಿ -errno;

        *ret = TAKE_PTR(p);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ mkdtemp_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾದರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ fd, r;

        r = mkdtemp_malloc(ಮಾದರಿ, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ಆದರೆ (fd < 0) {
                (ಶೂನ್ಯ) rmdir(p);
                ಹಿಂತಿರುಗಿ fd;
        }

        ಆದರೆ (ret)
                *ret = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ fd;
}

ಶೂನ್ಯ cleanup_tmpfile_ದತ್ತಾಂಶ_ಮುಗಿದ(ರಚನೆ cleanup_tmpfile_ದತ್ತಾಂಶ *d) {
        ಪ್ರತಿಜ್ಞಾ(d);

        ಆದರೆ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                ಹಿಂತಿರುಗಿ;

        PROTECT_ERRNO;

        (ಶೂನ್ಯ) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ಖಾಲಿ;
        d->filename = ಖಾಲಿ;
}
