/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "random-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "sync-util.h"
#అన్తర్భావ "tmpfile-util.h"
#అన్తర్భావ "umask-util.h"

స్థిర పూర్ణాంకం fopen_temporary_అంతర్గత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(మార్గం);

        fd = openat(dir_fd, మార్గం, O_CLOEXEC|O_NOCTTY|O_RDWR|O_CREAT|O_EXCL, 0600);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        /* This assumes that returned FILE object is short-lived and used within the same single-threaded
         * context and never shared externally, hence locking is not necessary. */

        r = take_fdopen_unlocked(&fd, "w", &f);
        ఒకవేళ (r < 0) {
                (శూన్య) unlinkat(dir_fd, మార్గం, 0);
                తిరిగి r;
        }

        ఒకవేళ (ret_file)
                *ret_file = TAKE_PTR(f);

        తిరిగి 0;
}

పూర్ణాంకం fopen_temporary_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file, అక్షరం **ret_మార్గం) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(మార్గం);

        r = tempfn_random(మార్గం, ఖాళీ, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = fopen_temporary_అంతర్గత(dir_fd, t, ret_file);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(t);

        తిరిగి 0;
}

పూర్ణాంకం fopen_temporary_బిడ్డ_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file, అక్షరం **ret_మార్గం) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ఒకవేళ (!మార్గం) {
                r = tmp_dir(&మార్గం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        r = tempfn_random_బిడ్డ(మార్గం, ఖాళీ, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = fopen_temporary_అంతర్గత(dir_fd, t, ret_file);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(t);

        తిరిగి 0;
}

/* This is much like mkostemp() but is subject to umask(). */
పూర్ణాంకం mkostemp_సురక్షిత(అక్షరం *నమూనా) {
        ప్రతిజ్ఞా(నమూనా);
        ఖండం_WITH_UMASK(0077);
        తిరిగి RET_NERRNO(mkostemp(నమూనా, O_CLOEXEC));
}

పూర్ణాంకం fmkostemp_సురక్షిత(అక్షరం *నమూనా, స్థిరరాశి అక్షరం *బహుళకం, FILE **ret_f) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        FILE *f;

        ప్రతిజ్ఞా(ret_f);

        fd = mkostemp_సురక్షిత(నమూనా);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        f = take_fdopen(&fd, బహుళకం);
        ఒకవేళ (!f)
                తిరిగి -errno;

        *ret_f = f;
        తిరిగి 0;
}

శూన్య unlink_tempfilep(అక్షరం (*p)[]) {
        ప్రతిజ్ఞా(p);

        /* If the file is created with mkstemp(), it will (almost always) change the suffix.
         * Treat this as a sign that the file was successfully created. We ignore both the rare case
         * where the original suffix is used and unlink failures. */

        ఒకవేళ (!endswith(*p, ".XXXXXX"))
                (శూన్య) unlink(*p);
}

స్థిర పూర్ణాంకం tempfn_build(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *pre, స్థిరరాశి అక్షరం *post, bool బిడ్డ, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *d = ఖాళీ, *fn = ఖాళీ, *nf = ఖాళీ, *ఫలితం = ఖాళీ;
        size_t len_pre, len_post, len_కూడు;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this :
         *         /foo/bar/waldo/.#<pre><post> (child == true)
         *         /foo/bar/.#<pre>waldo<post> (child == false)
         */

        ఒకవేళ (pre && strchr(pre, '/'))
                తిరిగి -EINVAL;

        ఒకవేళ (post && strchr(post, '/'))
                తిరిగి -EINVAL;

        len_pre = strlen_ptr(pre);
        len_post = strlen_ptr(post);
        /* NAME_MAX is counted *without* the trailing NUL byte. */
        ఒకవేళ (len_pre > NAME_MAX - STRLEN(".#") ||
            len_post > NAME_MAX - STRLEN(".#") - len_pre)
                తిరిగి -EINVAL;

        len_కూడు = len_pre + len_post + STRLEN(".#");

        ఒకవేళ (బిడ్డ) {
                d = strdup(p);
                ఒకవేళ (!d)
                        తిరిగి -ENOMEM;
        } లేకపోతే {
                r = మార్గం_విభజించు_ఉపసర్గ_filename(p, &d, &fn);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* Truncate the filename if it would become too long after mangling. */
                strshorten(fn, NAME_MAX - len_కూడు);
        }

        nf = strjoin(".#", strempty(pre), strempty(fn), strempty(post));
        ఒకవేళ (!nf)
                తిరిగి -ENOMEM;

        ఒకవేళ (d) {
                ఒకవేళ (!మార్గం_extend(&d, nf))
                        తిరిగి -ENOMEM;

                ఫలితం = మార్గం_simplify(TAKE_PTR(d));
        } లేకపోతే
                ఫలితం = TAKE_PTR(nf);

        ఒకవేళ (!మార్గం_is_valid(ఫలితం)) /* New path is not valid? (Maybe because too long?) Refuse. */
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(ఫలితం);
        తిరిగి 0;
}

పూర్ణాంకం tempfn_xxxxxx(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *extra, అక్షరం **ret) {
        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldoXXXXXX
         */

        తిరిగి tempfn_build(p, extra, "XXXXXX", /* child= */ false, ret);
}

పూర్ణాంకం tempfn_random(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *extra, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        /*
         * Turns this:
         *         /foo/bar/waldo
         *
         * Into this:
         *         /foo/bar/.#<extra>waldobaa2a261115984a9
         */

        ఒకవేళ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                తిరిగి -ENOMEM;

        తిరిగి tempfn_build(p, extra, s, /* child= */ false, ret);
}

పూర్ణాంకం tempfn_random_బిడ్డ(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *extra, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Turns this:
         *         /foo/bar/waldo
         * Into this:
         *         /foo/bar/waldo/.#<extra>3c2b6219aa75d7d0
         */

        ఒకవేళ (!p) {
                r = tmp_dir(&p);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (asprintf(&s, "%016" PRIx64, random_u64()) < 0)
                తిరిగి -ENOMEM;

        తిరిగి tempfn_build(p, extra, s, /* child= */ true, ret);
}

పూర్ణాంకం open_tmpfile_unlinkable(స్థిరరాశి అక్షరం *directory, పూర్ణాంకం flags) {
        పూర్ణాంకం r;

        ఒకవేళ (!directory) {
                r = tmp_dir(&directory);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        } లేకపోతే ఒకవేళ (isempty(directory))
                తిరిగి -EINVAL;

        /* Returns an unlinked temporary file that cannot be linked into the file system anymore */

        /* Try O_TMPFILE first, if it is supported */
        పూర్ణాంకం fd = open(directory, flags|O_TMPFILE|O_EXCL, S_IRUSR|S_IWUSR);
        ఒకవేళ (fd >= 0)
                తిరిగి fd;

        /* Fall back to unguessable name + unlinking */
        _cleanup_free_ అక్షరం *p = మార్గం_join(directory, "/systemd-tmp-XXXXXX");
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        fd = mkostemp_సురక్షిత(p);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        (శూన్య) unlink(p);

        తిరిగి fd;
}

పూర్ణాంకం open_tmpfile_linkable_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం flags, అక్షరం **ret_మార్గం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(లక్ష్యం);
        ప్రతిజ్ఞా(ret_మార్గం);

        /* Don't allow O_EXCL, as that has a special meaning for O_TMPFILE */
        ప్రతిజ్ఞా((flags & O_EXCL) == 0);

        /* Creates a temporary file, that shall be renamed to "target" later. If possible, this uses O_TMPFILE – in
         * which case "ret_path" will be returned as NULL. If not possible the temporary path name used is returned in
         * "ret_path". Use link_tmpfile() below to rename the result after writing the file in full. */

        పూర్ణాంకం fd = open_మాతృ_at(dir_fd, లక్ష్యం, O_TMPFILE|flags, 0640);
        ఒకవేళ (fd >= 0) {
                *ret_మార్గం = ఖాళీ;
                తిరిగి fd;
        }

        ఒకవేళ (!ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                log_debug_errno(fd, "Failed to use O_TMPFILE for %s: %m", లక్ష్యం);

        _cleanup_free_ అక్షరం *tmp = ఖాళీ;
        r = tempfn_random(లక్ష్యం, ఖాళీ, &tmp);
        ఒకవేళ (r < 0)
                తిరిగి r;

        fd = openat(dir_fd, tmp, O_CREAT|O_EXCL|O_NOFOLLOW|O_NOCTTY|flags, 0640);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        *ret_మార్గం = TAKE_PTR(tmp);

        తిరిగి fd;
}

పూర్ణాంకం fopen_tmpfile_linkable_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం flags, అక్షరం **ret_మార్గం, FILE **ret_file) {
        _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(లక్ష్యం);
        ప్రతిజ్ఞా(ret_file);
        ప్రతిజ్ఞా(ret_మార్గం);

        fd = open_tmpfile_linkable_at(dir_fd, లక్ష్యం, flags, &మార్గం);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        f = take_fdopen(&fd, "w");
        ఒకవేళ (!f)
                తిరిగి -ENOMEM;

        *ret_మార్గం = TAKE_PTR(మార్గం);
        *ret_file = TAKE_PTR(f);
        తిరిగి 0;
}

పూర్ణాంకం link_tmpfile_at(పూర్ణాంకం fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *లక్ష్యం, LinkTmpfileFlags flags) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(లక్ష్యం);

        /* Moves a temporary file created with open_tmpfile() above into its final place. If "path" is NULL
         * an fd created with O_TMPFILE is assumed, and linkat() is used. Otherwise it is assumed O_TMPFILE
         * is not supported on the directory, and renameat2() is used instead. */

        ఒకవేళ (FLAGS_సమితి(flags, LINK_TMPFILE_SYNC) && fsync(fd) < 0)
                తిరిగి -errno;

        ఒకవేళ (మార్గం) {
                ఒకవేళ (FLAGS_సమితి(flags, LINK_TMPFILE_REPLACE))
                        r = RET_NERRNO(renameat(dir_fd, మార్గం, dir_fd, లక్ష్యం));
                లేకపోతే
                        r = rename_noreplace(dir_fd, మార్గం, dir_fd, లక్ష్యం);
        } లేకపోతే {
                ఒకవేళ (FLAGS_సమితి(flags, LINK_TMPFILE_REPLACE))
                        r = linkat_replace(fd, /* oldpath= */ ఖాళీ, dir_fd, లక్ష్యం);
                లేకపోతే
                        r = link_fd(fd, dir_fd, లక్ష్యం);
        }
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (FLAGS_సమితి(flags, LINK_TMPFILE_SYNC)) {
                r = fsync_నిండిన(fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం flink_tmpfile_at(FILE *f, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *లక్ష్యం, LinkTmpfileFlags flags) {
        పూర్ణాంకం fd, r;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(లక్ష్యం);

        fd = fileno(f);
        ఒకవేళ (fd < 0) /* Not all FILE* objects encapsulate fds */
                తిరిగి -EBADF;

        r = fflush_and_check(f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి link_tmpfile_at(fd, dir_fd, మార్గం, లక్ష్యం, flags);
}

పూర్ణాంకం mkdtemp_malloc(స్థిరరాశి అక్షరం *మూస, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (మూస)
                p = strdup(మూస);
        లేకపోతే {
                స్థిరరాశి అక్షరం *tmp;

                r = tmp_dir(&tmp);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                p = మార్గం_join(tmp, "XXXXXX");
        }
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        ఒకవేళ (!mkdtemp(p))
                తిరిగి -errno;

        *ret = TAKE_PTR(p);
        తిరిగి 0;
}

పూర్ణాంకం mkdtemp_open(స్థిరరాశి అక్షరం *మూస, పూర్ణాంకం flags, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం fd, r;

        r = mkdtemp_malloc(మూస, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        fd = RET_NERRNO(open(p, O_DIRECTORY|O_CLOEXEC|flags));
        ఒకవేళ (fd < 0) {
                (శూన్య) rmdir(p);
                తిరిగి fd;
        }

        ఒకవేళ (ret)
                *ret = TAKE_PTR(p);

        తిరిగి fd;
}

శూన్య cleanup_tmpfile_దత్తాంశం_పూర్తి(నిర్మాణం cleanup_tmpfile_దత్తాంశం *d) {
        ప్రతిజ్ఞా(d);

        ఒకవేళ (!d->dir_fd ||
            (*d->dir_fd < 0 && *d->dir_fd != AT_FDCWD) ||
            !d->filename ||
            !*d->filename)
                తిరిగి;

        PROTECT_ERRNO;

        (శూన్య) unlinkat(*d->dir_fd, *d->filename, /* flags= */ 0);
        d->dir_fd = ఖాళీ;
        d->filename = ఖాళీ;
}
