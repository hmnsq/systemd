/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio_ext.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "label-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mkdir.h"
#అన్తర్భావ "nulstr-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "socket-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "sync-util.h"
#అన్తర్భావ "terminal-util.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "tmpfile-util.h"

/* The maximum size of the file we'll read in one go in read_full_file() (64M). */
#నిర్వచించు READ_నిండిన_బైట్లు_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#నిర్వచించు READ_నిండిన_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#నిర్వచించు READ_VIRTUAL_బైట్లు_MAX (4U * U64_MB - UINT64_C(2))

పూర్ణాంకం fdopen_unlocked(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *options, FILE **ret) {
        ప్రతిజ్ఞా(ret);

        FILE *f = fdopen(fd, options);
        ఒకవేళ (!f)
                తిరిగి -errno;

        (శూన్య) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        తిరిగి 0;
}

పూర్ణాంకం take_fdopen_unlocked(పూర్ణాంకం *fd, స్థిరరాశి అక్షరం *options, FILE **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *fd = -EBADF;

        తిరిగి 0;
}

FILE* take_fdopen(పూర్ణాంకం *fd, స్థిరరాశి అక్షరం *options) {
        ప్రతిజ్ఞా(fd);

        FILE *f = fdopen(*fd, options);
        ఒకవేళ (!f)
                తిరిగి ఖాళీ;

        *fd = -EBADF;

        తిరిగి f;
}

DIR* take_fdopendir(పూర్ణాంకం *dfd) {
        ప్రతిజ్ఞా(dfd);

        DIR *d = fdopendir(*dfd);
        ఒకవేళ (!d)
                తిరిగి ఖాళీ;

        *dfd = -EBADF;

        తిరిగి d;
}

FILE* open_memstream_unlocked(అక్షరం **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ఒకవేళ (!f)
                తిరిగి ఖాళీ;

        (శూన్య) __fsetlocking(f, FSETLOCKING_BYCALLER);

        తిరిగి f;
}

FILE* fmemopen_unlocked(శూన్య *buf, size_t కొలత, స్థిరరాశి అక్షరం *బహుళకం) {
        FILE *f = fmemopen(buf, కొలత, బహుళకం);
        ఒకవేళ (!f)
                తిరిగి ఖాళీ;

        (శూన్య) __fsetlocking(f, FSETLOCKING_BYCALLER);

        తిరిగి f;
}

పూర్ణాంకం write_string_ప్రవాహం_నిండిన(
                FILE *f,
                స్థిరరాశి అక్షరం *పంక్తి,
                WriteStringFileFlags flags,
                స్థిరరాశి నిర్మాణం timespec *ts) {

        bool needs_nl;
        పూర్ణాంకం r, fd = -EBADF;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(పంక్తి);

        ఒకవేళ (ferror(f))
                తిరిగి -EIO;

        ఒకవేళ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ఒకవేళ (fd < 0)
                        తిరిగి -EBADF;
        }

        ఒకవేళ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ అక్షరం *t = ఖాళీ;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ఒకవేళ (fd < 0) {
                        fd = fileno(f);
                        ఒకవేళ (fd < 0)
                                తిరిగి -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ఒకవేళ (read_virtual_file_fd(fd, అక్షరశ్రేణి_పొడవు(పంక్తి)+1, &t, ఖాళీ) > 0 &&
                    streq_skip_trailing_chars(పంక్తి, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", పంక్తి);
                        తిరిగి 0;
                }

                ఒకవేళ (lseek(fd, 0, SEEK_SET) < 0)
                        తిరిగి -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(పంక్తి, "\n");

        ఒకవేళ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_బఫరు)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                పంక్తి = strjoina(పంక్తి, "\n");
                needs_nl = false;
        }

        ఒకవేళ (fputs(పంక్తి, f) == EOF)
                తిరిగి -errno;

        ఒకవేళ (needs_nl)
                ఒకవేళ (fputc('\n', f) == EOF)
                        తిరిగి -errno;

        ఒకవేళ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        లేకపోతే
                r = fflush_and_check(f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ts) {
                స్థిరరాశి నిర్మాణం timespec twice[2] = {*ts, *ts};

                ప్రతిజ్ఞా(fd >= 0);
                ఒకవేళ (futimens(fd, twice) < 0)
                        తిరిగి -errno;
        }

        తిరిగి 0;
}

స్థిర mode_t write_string_file_flags_గమ్యం_బహుళకం(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        తిరిగి FLAGS_సమితి(flags, WRITE_STRING_FILE_బహుళకం_0600) ? 0600 :
                FLAGS_సమితి(flags, WRITE_STRING_FILE_బహుళకం_0444) ? 0444 : 0644;
}

స్థిర పూర్ణాంకం write_string_file_atomic_at(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *fn,
                స్థిరరాశి అక్షరం *పంక్తి,
                WriteStringFileFlags flags,
                స్థిరరాశి నిర్మాణం timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fn);
        ప్రతిజ్ఞా(పంక్తి);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t బహుళకం = write_string_file_flags_గమ్యం_బహుళకం(flags);

        bool call_label_ops_post = false;
        ఒకవేళ (FLAGS_సమితి(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, బహుళకం, label_context);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        పూర్ణాంకం k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ఖాళీ : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ఒకవేళ (r < 0)
                తిరిగి r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ఒకవేళ (k < 0)
                తిరిగి k;

        r = write_string_ప్రవాహం_నిండిన(f, పంక్తి, flags, ts);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = fchmod_umask(fileno(f), బహుళకం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ఒకవేళ (r < 0)
                తిరిగి r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ఒకవేళ (FLAGS_సమితి(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_యొక్క_file(fileno(f));
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం write_string_file_నిండిన_label(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *fn,
                స్థిరరాశి అక్షరం *పంక్తి,
                WriteStringFileFlags flags,
                స్థిరరాశి నిర్మాణం timespec *ts,
                స్థిరరాశి అక్షరం *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd == AT_FDCWD || dir_fd >= 0);
        ప్రతిజ్ఞా(పంక్తి);

        /* We don't know how to verify whether the file contents was already on-disk. */
        ప్రతిజ్ఞా(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ఒకవేళ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                ప్రతిజ్ఞా(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (flags & WRITE_STRING_FILE_ATOMIC) {
                ప్రతిజ్ఞా(fn);
                ప్రతిజ్ఞా(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, పంక్తి, flags, ts, label_context);
                ఒకవేళ (r < 0)
                        వెళ్ళు fail;

                తిరిగి r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ఒకవేళ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        లేకపోతే {
                mode_t బహుళకం = write_string_file_flags_గమ్యం_బహుళకం(flags);
                bool call_label_ops_post = false;

                ఒకవేళ (FLAGS_సమితి(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, బహుళకం, label_context);
                        ఒకవేళ (r < 0)
                                వెళ్ళు fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_సమితి(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                బహుళకం,
                                &made_file);
                ఒకవేళ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ఖాళీ : fn, made_file, label_context));
        }
        ఒకవేళ (r < 0)
                వెళ్ళు fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ఒకవేళ (r < 0)
                వెళ్ళు fail;

        ఒకవేళ (flags & WRITE_STRING_FILE_DISABLE_బఫరు)
                setvbuf(f, ఖాళీ, _IONBF, 0);

        r = write_string_ప్రవాహం_నిండిన(f, పంక్తి, flags, ts);
        ఒకవేళ (r < 0)
                వెళ్ళు fail;

        తిరిగి 0;

fail:
        ఒకవేళ (made_file)
                (శూన్య) unlinkat(dir_fd, fn, 0);

        ఒకవేళ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                తిరిగి r;

        f = సురక్షిత_fclose(f);
        fd = సురక్షిత_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ఒకవేళ (verify_file_at(dir_fd, fn, పంక్తి, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                తిరిగి 0;

        తిరిగి r;
}

పూర్ణాంకం write_string_filef(
                స్థిరరాశి అక్షరం *fn,
                WriteStringFileFlags flags,
                స్థిరరాశి అక్షరం *format, ...) {

        _cleanup_free_ అక్షరం *p = ఖాళీ;
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        తిరిగి write_string_file(fn, p, flags);
}

పూర్ణాంకం write_string_filef_at(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *fn,
                WriteStringFileFlags flags,
                స్థిరరాశి అక్షరం *format, ...) {

        _cleanup_free_ అక్షరం *p = ఖాళీ;
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        తిరిగి write_string_file_at(dir_fd, fn, p, flags);
}

పూర్ణాంకం write_base64_file_at(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *fn,
                స్థిరరాశి నిర్మాణం iovec *దత్తాంశం,
                WriteStringFileFlags flags) {

        _cleanup_free_ అక్షరం *encoded = ఖాళీ;
        ssize_t n;

        n = base64mem_నిండిన(దత్తాంశం ? దత్తాంశం->iov_base : ఖాళీ, దత్తాంశం ? దత్తాంశం->iov_len : 0, 79, &encoded);
        ఒకవేళ (n < 0)
                తిరిగి n;

        తిరిగి write_string_file_at(dir_fd, fn, encoded, flags);
}

పూర్ణాంకం read_one_పంక్తి_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename, అక్షరం **ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(filename);
        ప్రతిజ్ఞా(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి read_పంక్తి(f, LONG_పంక్తి_MAX, ret);
}

పూర్ణాంకం read_boolean_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(filename);

        r = read_one_పంక్తి_file_at(dir_fd, filename, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_boolean(s);
}

పూర్ణాంకం verify_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        size_t l, k;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(blob);

        l = అక్షరశ్రేణి_పొడవు(blob);

        ఒకవేళ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = జ్ఞాపకం_ఇవ్వు(l + accept_extra_nl + 1);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ఒకవేళ (ferror(f))
                తిరిగి errno_or_else(EIO);

        ఒకవేళ (k != l && k != l + accept_extra_nl)
                తిరిగి 0;
        ఒకవేళ (memcmp(buf, blob, l) != 0)
                తిరిగి 0;
        ఒకవేళ (k > l && buf[l] != '\n')
                తిరిగి 0;

        తిరిగి 1;
}

పూర్ణాంకం read_virtual_file_at(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *filename,
                size_t max_కొలత,
                అక్షరం **ret_contents,
                size_t *ret_కొలత) {

        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        size_t n, కొలత;
        పూర్ణాంకం n_retries;
        bool truncated = false;

        /* Virtual filesystems such as sysfs or procfs use kernfs, and kernfs can work with two sorts of
         * virtual files. One sort uses "seq_file", and the results of the first read are buffered for the
         * second read. The other sort uses "raw" reads which always go direct to the device. In the latter
         * case, the content of the virtual file must be retrieved with a single read otherwise a second read
         * might get the new value instead of finding EOF immediately. That's the reason why the usage of
         * fread(3) is prohibited in this case as it always performs a second call to read(2) looking for
         * EOF. See issue #13585.
         *
         * max_size specifies a limit on the bytes read. If max_size is SIZE_MAX, the full file is read. If
         * the full file is too large to read, an error is returned. For other values of max_size, *partial
         * contents* may be returned. (Though the read is still done using one syscall.) Returns 0 on
         * partial success, 1 if untruncated contents were read.
         *
         * Rule: for kernfs files using "seq_file" → use regular read_full_file_at()
         *       for kernfs files using "raw" → use read_virtual_file_at()
         */

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(max_కొలత <= READ_VIRTUAL_బైట్లు_MAX || max_కొలత == SIZE_MAX);

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        ఒకవేళ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        లేకపోతే
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        కొరకు (;;) {
                నిర్మాణం stat st;

                ఒకవేళ (fstat(fd, &st) < 0)
                        తిరిగి -errno;

                ఒకవేళ (!S_ISREG(st.st_mode))
                        తిరిగి -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_బైట్లు_MAX < SSIZE_MAX);
                ఒకవేళ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ఒకవేళ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ఒకవేళ (max_కొలత == SIZE_MAX)
                                        తిరిగి -EFBIG;

                                కొలత = max_కొలత;
                        } లేకపోతే {
                                కొలత = MIN((size_t) st.st_size, max_కొలత);

                                ఒకవేళ (కొలత > READ_VIRTUAL_బైట్లు_MAX)
                                        తిరిగి -EFBIG;
                        }

                        n_retries--;
                } లేకపోతే ఒకవేళ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        కొలత = MIN3(పుట_కొలత() - 1, READ_VIRTUAL_బైట్లు_MAX, max_కొలత);
                        n_retries = 1;
                } లేకపోతే {
                        కొలత = MIN(READ_VIRTUAL_బైట్లు_MAX, max_కొలత);
                        n_retries = 0;
                }

                buf = జ్ఞాపకం_ఇవ్వు(కొలత + 1);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                కొలత = MIN3(MALLOC_SIZEOF_సురక్షిత(buf) - 1, max_కొలత, READ_VIRTUAL_బైట్లు_MAX);

                కొరకు (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = పఠ(fd, buf, కొలత + 1);
                        ఒకవేళ (k >= 0) {
                                n = k;
                                ఆపు;
                        }

                        ఒకవేళ (errno != EINTR)
                                తిరిగి -errno;
                }

                /* Consider a short read as EOF */
                ఒకవేళ (n <= కొలత)
                        ఆపు;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ఒకవేళ (max_కొలత != SIZE_MAX && n > max_కొలత) {
                        n = కొలత; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        ఆపు;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ఒకవేళ (n_retries <= 0)
                        తిరిగి -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ఒకవేళ (lseek(fd, 0, SEEK_SET) < 0)
                        తిరిగి -errno;

                buf = mfree(buf);
        }

        ఒకవేళ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ఒకవేళ (!ret_కొలత && memchr(buf, 0, n))
                        తిరిగి -EBADMSG;

                ఒకవేళ (n < కొలత) {
                        అక్షరం *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ఒకవేళ (!p)
                                తిరిగి -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ఒకవేళ (ret_కొలత)
                *ret_కొలత = n;

        తిరిగి !truncated;
}

పూర్ణాంకం read_నిండిన_ప్రవాహం_నిండిన(
                FILE *f,
                స్థిరరాశి అక్షరం *filename,
                uint64_t స్థానభ్రంశం,
                size_t కొలత,
                Read_నిండిన_File_Flags flags,
                అక్షరం **ret_contents,
                size_t *ret_కొలత) {

        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        size_t n, n_తదుపరి = 0, l, expected_decoded_కొలత = కొలత;
        పూర్ణాంకం fd, r;

        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(ret_contents);
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, READ_నిండిన_FILE_UNBASE64 | READ_నిండిన_FILE_UNHEX));
        ప్రతిజ్ఞా(కొలత != SIZE_MAX || !FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER));

        ఒకవేళ (స్థానభ్రంశం != UINT64_MAX && స్థానభ్రంశం > LONG_MAX) /* fseek() can only deal with "long" offsets */
                తిరిగి -ERANGE;

        ఒకవేళ ((flags & (READ_నిండిన_FILE_UNBASE64 | READ_నిండిన_FILE_UNHEX)) != 0) {
                ఒకవేళ (కొలత <= SIZE_MAX / READ_నిండిన_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        కొలత *= READ_నిండిన_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                లేకపోతే
                        కొలత = SIZE_MAX;
        }

        fd = fileno(f);
        ఒకవేళ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                నిర్మాణం stat st;

                ఒకవేళ (fstat(fd, &st) < 0)
                        తిరిగి -errno;

                ఒకవేళ (FLAGS_సమితి(flags, READ_నిండిన_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ఒకవేళ (st.st_size > 0 &&
                            (కొలత == SIZE_MAX || FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, స్థానభ్రంశం == UINT64_MAX ? 0 : స్థానభ్రంశం);

                                ఒకవేళ (rsize < SIZE_MAX) /* overflow check */
                                        n_తదుపరి = rsize + 1;
                        }

                        ఒకవేళ (flags & READ_నిండిన_FILE_WARN_WORLD_READABLE)
                                (శూన్య) warn_file_is_world_accessible(filename, &st, ఖాళీ, 0);
                }
        } లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, READ_నిండిన_FILE_VERIFY_REGULAR))
                తిరిగి -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ఒకవేళ (n_తదుపరి == 0)
                n_తదుపరి = కొలత != SIZE_MAX && !FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER) ? కొలత : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ఒకవేళ (n_తదుపరి > READ_నిండిన_బైట్లు_MAX)
                n_తదుపరి = READ_నిండిన_బైట్లు_MAX + 1;

        ఒకవేళ (స్థానభ్రంశం != UINT64_MAX && fseek(f, స్థానభ్రంశం, SEEK_SET) < 0)
                తిరిగి -errno;

        n = l = 0;
        కొరకు (;;) {
                అక్షరం *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ఒకవేళ (FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER) && n_తదుపరి > కొలత)
                        n_తదుపరి = కొలత + 1;

                ఒకవేళ (flags & READ_నిండిన_FILE_SECURE) {
                        t = జ్ఞాపకం_ఇవ్వు(n_తదుపరి + 1);
                        ఒకవేళ (!t) {
                                r = -ENOMEM;
                                వెళ్ళు finalize;
                        }
                        memcpy_సురక్షిత(t, buf, n);
                        explicit_bzero_సురక్షిత(buf, n);
                        విడుదల(buf);
                } లేకపోతే {
                        t = realloc(buf, n_తదుపరి + 1);
                        ఒకవేళ (!t)
                                తిరిగి -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = కొలత == SIZE_MAX ? MALLOC_SIZEOF_సురక్షిత(buf) - 1 : n_తదుపరి;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                ప్రతిజ్ఞా(k <= n - l);
                l += k;

                ఒకవేళ (ferror(f)) {
                        r = errno_or_else(EIO);
                        వెళ్ళు finalize;
                }
                ఒకవేళ (feof(f))
                        ఆపు;

                ఒకవేళ (కొలత != SIZE_MAX && !FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        ప్రతిజ్ఞా(l == కొలత);
                        ఆపు;
                }

                ప్రతిజ్ఞా(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ఒకవేళ (FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER) && l > కొలత) {
                        r = -E2BIG;
                        వెళ్ళు finalize;
                }

                ఒకవేళ (n >= READ_నిండిన_బైట్లు_MAX) {
                        r = -E2BIG;
                        వెళ్ళు finalize;
                }

                n_తదుపరి = MIN(n * 2, READ_నిండిన_బైట్లు_MAX);
        }

        ఒకవేళ (flags & (READ_నిండిన_FILE_UNBASE64 | READ_నిండిన_FILE_UNHEX)) {
                _cleanup_free_ శూన్య *decoded = ఖాళీ;
                size_t decoded_కొలత;

                buf[l++] = 0;
                ఒకవేళ (flags & READ_నిండిన_FILE_UNBASE64)
                        r = unbase64mem_నిండిన(buf, l, flags & READ_నిండిన_FILE_SECURE, &decoded, &decoded_కొలత);
                లేకపోతే
                        r = unhexmem_నిండిన(buf, l, flags & READ_నిండిన_FILE_SECURE, &decoded, &decoded_కొలత);
                ఒకవేళ (r < 0)
                        వెళ్ళు finalize;

                ఒకవేళ (flags & READ_నిండిన_FILE_SECURE)
                        explicit_bzero_సురక్షిత(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_కొలత;

                ఒకవేళ (FLAGS_సమితి(flags, READ_నిండిన_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_కొలత) {
                        r = -E2BIG;
                        వెళ్ళు finalize;
                }
        }

        ఒకవేళ (!ret_కొలత) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ఒకవేళ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        వెళ్ళు finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ఒకవేళ (ret_కొలత)
                *ret_కొలత = l;

        తిరిగి 0;

finalize:
        ఒకవేళ (flags & READ_నిండిన_FILE_SECURE)
                explicit_bzero_సురక్షిత(buf, n);

        తిరిగి r;
}

పూర్ణాంకం read_నిండిన_file_నిండిన(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *filename,
                uint64_t స్థానభ్రంశం,
                size_t కొలత,
                Read_నిండిన_File_Flags flags,
                స్థిరరాశి అక్షరం *bind_name,
                అక్షరం **ret_contents,
                size_t *ret_కొలత) {

        _cleanup_fclose_ FILE *f = ఖాళీ;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_contents);

        ఒకవేళ (FLAGS_సమితి(flags, READ_నిండిన_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            స్థానభ్రంశం == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_నిండిన(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి read_నిండిన_ప్రవాహం_నిండిన(f, filename, స్థానభ్రంశం, కొలత, flags, ret_contents, ret_కొలత);
}

పూర్ణాంకం script_get_shebang_interpreter(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        f = fopen(మార్గం, "re");
        ఒకవేళ (!f)
                తిరిగి -errno;

        అక్షరం c;
        r = సురక్షిత_fgetc(f, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EBADMSG;
        ఒకవేళ (c != '#')
                తిరిగి -EMEDIUMTYPE;
        r = సురక్షిత_fgetc(f, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EBADMSG;
        ఒకవేళ (c != '!')
                తిరిగి -EMEDIUMTYPE;

        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_free_ అక్షరం *p = ఖాళీ;
        స్థిరరాశి అక్షరం *s = పంక్తి;

        r = వెలికితీయి_మొదటి_మాట(&s, &p, /* separators= */ ఖాళీ, /* flags= */ 0);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -ENOEXEC;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(p);
        తిరిగి 0;
}

పూర్ణాంకం get_proc_క్షేత్రం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *కీలకం, అక్షరం **ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(కీలకం);

        r = fopen_unlocked(మార్గం, "re", &f);
        ఒకవేళ (r == -ENOENT && proc_mounted() == 0)
                తిరిగి -ENOSYS;
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                 _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;

                 r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                 ఒకవేళ (r < 0)
                         తిరిగి r;
                 ఒకవేళ (r == 0)
                         తిరిగి -ENODATA;

                 అక్షరం *l = startswith(పంక్తి, కీలకం);
                 ఒకవేళ (l && *l == ':') {
                         ఒకవేళ (ret) {
                                 అక్షరం *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ఒకవేళ (!s)
                                         తిరిగి -ENOMEM;

                                 *ret = s;
                         }

                         తిరిగి 0;
                 }
        }
}

DIR* xopendirat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(!(flags & (O_CREAT|O_TMPFILE)));

        ఒకవేళ ((dir_fd == AT_FDCWD || మార్గం_is_absolute(మార్గం)) &&
            (flags &~ O_DIRECTORY) == 0)
                తిరిగి opendir(మార్గం);

        ఒకవేళ (isempty(మార్గం)) {
                మార్గం = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, మార్గం, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ఒకవేళ (fd < 0)
                తిరిగి ఖాళీ;

        తిరిగి take_fdopendir(&fd);
}

పూర్ణాంకం fopen_బహుళకం_గమ్యం_flags(స్థిరరాశి అక్షరం *బహుళకం) {
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం flags;

        ప్రతిజ్ఞా(బహుళకం);

        ఒకవేళ ((p = startswith(బహుళకం, "r+")))
                flags = O_RDWR;
        లేకపోతే ఒకవేళ ((p = startswith(బహుళకం, "r")))
                flags = O_RDONLY;
        లేకపోతే ఒకవేళ ((p = startswith(బహుళకం, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        లేకపోతే ఒకవేళ ((p = startswith(బహుళకం, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        లేకపోతే ఒకవేళ ((p = startswith(బహుళకం, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        లేకపోతే ఒకవేళ ((p = startswith(బహుళకం, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        లేకపోతే
                తిరిగి -EINVAL;

        కొరకు (; *p != 0; p++) {

                ఎంపిక (*p) {

                స్థితి 'e':
                        flags |= O_CLOEXEC;
                        ఆపు;

                స్థితి 'x':
                        flags |= O_EXCL;
                        ఆపు;

                స్థితి 'm':
                        /* ignore this here, fdopen() might care later though */
                        ఆపు;

                స్థితి 'c': /* not sure what to do about this one */
                అప్రమేయ:
                        తిరిగి -EINVAL;
                }
        }

        తిరిగి flags;
}

స్థిర పూర్ణాంకం xfopenat_regular(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *బహుళకం, పూర్ణాంకం open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(బహుళకం);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (dir_fd == AT_FDCWD && మార్గం && open_flags == 0)
                f = fopen(మార్గం, బహుళకం);
        లేకపోతే ఒకవేళ (dir_fd == XAT_FDROOT && మార్గం && open_flags == 0) {
                _cleanup_free_ అక్షరం *j = strjoin("/", మార్గం);
                ఒకవేళ (!j)
                        తిరిగి -ENOMEM;

                f = fopen(j, బహుళకం);
        } లేకపోతే {
                _cleanup_close_ పూర్ణాంకం fd = -EBADF;
                పూర్ణాంకం బహుళకం_flags;

                బహుళకం_flags = fopen_బహుళకం_గమ్యం_flags(బహుళకం);
                ఒకవేళ (బహుళకం_flags < 0)
                        తిరిగి బహుళకం_flags;

                ఒకవేళ (మార్గం) {
                        fd = openat(dir_fd, మార్గం, బహుళకం_flags | open_flags);
                        ఒకవేళ (fd < 0)
                                తిరిగి -errno;
                } లేకపోతే {
                        ఒకవేళ (dir_fd == AT_FDCWD)
                                తిరిగి -EBADF;

                        fd = fd_reopen(dir_fd, (బహుళకం_flags | open_flags) & ~O_NOFOLLOW);
                        ఒకవేళ (fd < 0)
                                తిరిగి fd;
                }

                f = take_fdopen(&fd, బహుళకం);
        }
        ఒకవేళ (!f)
                తిరిగి -errno;

        *ret = f;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం xfopenat_unix_socket(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *bind_name, FILE **ret) {
        _cleanup_close_ పూర్ణాంకం sk = -EBADF;
        FILE *f;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ఒకవేళ (sk < 0)
                తిరిగి -errno;

        ఒకవేళ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                సంఘం sockaddr_union bsa;

                r = sockaddr_un_సమితి_మార్గం(&bsa.un, bind_name);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (bind(sk, &bsa.sa, r) < 0)
                        తిరిగి -errno;
        }

        r = connect_unix_మార్గం(sk, dir_fd, మార్గం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (shutdown(sk, SHUT_WR) < 0)
                తిరిగి -errno;

        f = take_fdopen(&sk, "r");
        ఒకవేళ (!f)
                తిరిగి -errno;

        *ret = f;
        తిరిగి 0;
}

పూర్ణాంకం xfopenat_నిండిన(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *బహుళకం,
                పూర్ణాంకం open_flags,
                XfopenFlags flags,
                స్థిరరాశి అక్షరం *bind_name,
                FILE **ret) {

        FILE *f = ఖాళీ;  /* avoid false maybe-uninitialized warning */
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(బహుళకం);
        ప్రతిజ్ఞా(ret);

        r = xfopenat_regular(dir_fd, మార్గం, బహుళకం, open_flags, &f);
        ఒకవేళ (r == -ENXIO && FLAGS_సమితి(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, మార్గం, bind_name, &f);
                ఒకవేళ (లోపల_సమితి(r, -ENOTSOCK, -EINVAL))
                        తిరిగి -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (FLAGS_సమితి(flags, XFOPEN_UNLOCKED))
                (శూన్య) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        తిరిగి 0;
}

పూర్ణాంకం fdopen_independent(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *బహుళకం, FILE **ret) {
        _cleanup_close_ పూర్ణాంకం copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం బహుళకం_flags;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(బహుళకం);
        ప్రతిజ్ఞా(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        బహుళకం_flags = fopen_బహుళకం_గమ్యం_flags(బహుళకం);
        ఒకవేళ (బహుళకం_flags < 0)
                తిరిగి బహుళకం_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, బహుళకం_flags & ~O_CREAT);
        ఒకవేళ (copy_fd < 0)
                తిరిగి copy_fd;

        f = take_fdopen(&copy_fd, బహుళకం);
        ఒకవేళ (!f)
                తిరిగి -errno;

        *ret = TAKE_PTR(f);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం శోధన_and_open_అంతర్గత(
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం బహుళకం,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం **శోధన,
                పూర్ణాంకం *ret_fd,
                అక్షరం **ret_మార్గం) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!ret_fd || !FLAGS_సమితి(బహుళకం, O_CREAT)); /* We don't support O_CREAT for this */
        ప్రతిజ్ఞా(మార్గం);

        ఒకవేళ (మార్గం_is_absolute(మార్గం)) {
                _cleanup_close_ పూర్ణాంకం fd = -EBADF;

                ఒకవేళ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(మార్గం, బహుళకం, 0777));
                లేకపోతే
                        r = RET_NERRNO(access(మార్గం, బహుళకం));
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (ret_మార్గం) {
                        r = మార్గం_simplify_alloc(మార్గం, ret_మార్గం);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                తిరిగి 0;
        }

        ఒకవేళ (!మార్గం_strv_resolve_uniq(శోధన, మూలం))
                తిరిగి -ENOMEM;

        STRV_FOREACH(i, శోధన) {
                _cleanup_close_ పూర్ణాంకం fd = -EBADF;
                _cleanup_free_ అక్షరం *p = ఖాళీ;

                p = మార్గం_join(మూలం, *i, మార్గం);
                ఒకవేళ (!p)
                        తిరిగి -ENOMEM;

                ఒకవేళ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, బహుళకం, 0777));
                లేకపోతే
                        r = RET_NERRNO(access(p, F_OK));
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (ret_మార్గం)
                                *ret_మార్గం = మార్గం_simplify(TAKE_PTR(p));

                        ఒకవేళ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        తిరిగి 0;
                }
                ఒకవేళ (r != -ENOENT)
                        తిరిగి r;
        }

        తిరిగి -ENOENT;
}

పూర్ణాంకం శోధన_and_open(
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం బహుళకం,
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం **శోధన,
                పూర్ణాంకం *ret_fd,
                అక్షరం **ret_మార్గం) {

        _cleanup_strv_free_ అక్షరం **copy = ఖాళీ;

        ప్రతిజ్ఞా(మార్గం);

        copy = strv_copy(శోధన);
        ఒకవేళ (!copy)
                తిరిగి -ENOMEM;

        తిరిగి శోధన_and_open_అంతర్గత(మార్గం, బహుళకం, మూలం, copy, ret_fd, ret_మార్గం);
}

స్థిర పూర్ణాంకం శోధన_and_fopen_అంతర్గత(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *బహుళకం,
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం **శోధన,
                FILE **ret_file,
                అక్షరం **ret_మార్గం) {

        _cleanup_free_ అక్షరం *దొరికింది_మార్గం = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(బహుళకం || !ret_file);

        r = శోధన_and_open_అంతర్గత(
                        మార్గం,
                        బహుళకం ? fopen_బహుళకం_గమ్యం_flags(బహుళకం) : 0,
                        మూలం,
                        శోధన,
                        ret_file ? &fd : ఖాళీ,
                        ret_మార్గం ? &దొరికింది_మార్గం : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_file) {
                FILE *f = take_fdopen(&fd, బహుళకం);
                ఒకవేళ (!f)
                        తిరిగి -errno;

                *ret_file = f;
        }

        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(దొరికింది_మార్గం);

        తిరిగి 0;
}

పూర్ణాంకం శోధన_and_fopen(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *బహుళకం,
                స్థిరరాశి అక్షరం *మూలం,
                స్థిరరాశి అక్షరం **శోధన,
                FILE **ret_file,
                అక్షరం **ret_మార్గం) {

        _cleanup_strv_free_ అక్షరం **copy = ఖాళీ;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(బహుళకం || !ret_file);

        copy = strv_copy((అక్షరం**) శోధన);
        ఒకవేళ (!copy)
                తిరిగి -ENOMEM;

        తిరిగి శోధన_and_fopen_అంతర్గత(మార్గం, బహుళకం, మూలం, copy, ret_file, ret_మార్గం);
}

పూర్ణాంకం శోధన_and_fopen_nulstr(
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *బహుళకం,
                స్థిరరాశి అక్షరం *మూలం,
                స్థిరరాశి అక్షరం *శోధన,
                FILE **ret_file,
                అక్షరం **ret_మార్గం) {

        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(బహుళకం || !ret_file);

        l = strv_విభజించు_nulstr(శోధన);
        ఒకవేళ (!l)
                తిరిగి -ENOMEM;

        తిరిగి శోధన_and_fopen_అంతర్గత(మార్గం, బహుళకం, మూలం, l, ret_file, ret_మార్గం);
}

పూర్ణాంకం fflush_and_check(FILE *f) {
        ప్రతిజ్ఞా(f);

        errno = 0;
        fflush(f);

        ఒకవేళ (ferror(f))
                తిరిగి errno_or_else(EIO);

        తిరిగి 0;
}

పూర్ణాంకం fflush_sync_and_check(FILE *f) {
        పూర్ణాంకం r, fd;

        ప్రతిజ్ఞా(f);

        r = fflush_and_check(f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ఒకవేళ (fd < 0)
                తిరిగి 0;

        r = fsync_నిండిన(fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 0;
}

పూర్ణాంకం write_timestamp_file_atomic(స్థిరరాశి అక్షరం *fn, usec_t n) {
        అక్షరం ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ఒకవేళ (!timestamp_is_సమితి(n))
                తిరిగి -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        తిరిగి write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

పూర్ణాంకం read_timestamp_file(స్థిరరాశి అక్షరం *fn, usec_t *ret) {
        _cleanup_free_ అక్షరం *ln = ఖాళీ;
        uint64_t t;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = read_one_పంక్తి_file(fn, &ln);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = సురక్షిత_atou64(ln, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!timestamp_is_సమితి(t))
                తిరిగి -ERANGE;

        *ret = (usec_t) t;
        తిరిగి 0;
}

పూర్ణాంకం fputs_with_separator(FILE *f, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separator, bool *space) {
        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ఒకవేళ (!f)
                f = stdout;

        ఒకవేళ (!separator)
                separator = " ";

        ఒకవేళ (*space)
                ఒకవేళ (fputs(separator, f) < 0)
                        తిరిగి -EIO;

        *space = true;

        ఒకవేళ (fputs(s, f) < 0)
                తిరిగి -EIO;

        తిరిగి 0;
}

పూర్ణాంకం fputs_with_newline(FILE *f, స్థిరరాశి అక్షరం *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ఒకవేళ (isempty(s))
                తిరిగి 0;

        ఒకవేళ (!f)
                f = stdout;

        ఒకవేళ (fputs(s, f) < 0)
                తిరిగి -EIO;

        ఒకవేళ (endswith(s, "\n"))
                తిరిగి 0;

        ఒకవేళ (fputc('\n', f) < 0)
                తిరిగి -EIO;

        తిరిగి 1;
}

/* A bitmask of the EOL markers we know */
రకనిర్వచనం గణన చివర_యొక్క_పంక్తి_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} చివర_యొక్క_పంక్తి_Marker;

స్థిర చివర_యొక్క_పంక్తి_Marker categorize_eol(అక్షరం c, Read_పంక్తి_Flags flags) {

        ఒకవేళ (!FLAGS_సమితి(flags, READ_పంక్తి_ONLY_NUL)) {
                ఒకవేళ (c == '\n')
                        తిరిగి EOL_TEN;
                ఒకవేళ (c == '\r')
                        తిరిగి EOL_THIRTEEN;
        }

        ఒకవేళ (c == '\0')
                తిరిగి EOL_ZERO;

        తిరిగి EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_నిండిన(FILE*, funlockfile, ఖాళీ);

పూర్ణాంకం read_పంక్తి_నిండిన(FILE *f, size_t limit, Read_పంక్తి_Flags flags, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *బఫరు = ఖాళీ;
        size_t n = 0, లెక్క = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f);

        /* Something like a bounded version of getline().
         *
         * Considers EOF, \n, \r and \0 end of line delimiters (or combinations of these), and does not include these
         * delimiters in the string returned. Specifically, recognizes the following combinations of markers as line
         * endings:
         *
         *     • \n        (UNIX)
         *     • \r        (old MacOS)
         *     • \0        (C strings)
         *     • \n\0
         *     • \r\0
         *     • \r\n      (Windows)
         *     • \n\r
         *     • \r\n\0
         *     • \n\r\0
         *
         * Returns the number of bytes read from the files (i.e. including delimiters — this hence usually differs from
         * the number of characters in the returned string). When EOF is hit, 0 is returned.
         *
         * The input parameter limit is the maximum numbers of characters in the returned string, i.e. excluding
         * delimiters. If the limit is hit we fail and return -ENOBUFS.
         *
         * If a line shall be skipped ret may be initialized as NULL. */

        ఒకవేళ (ret) {
                ఒకవేళ (!GREEDY_REALLOC(బఫరు, 1))
                        తిరిగి -ENOMEM;
        }

        {
                వాడని _cleanup_(funlockfilep) FILE *flocked = f;
                చివర_యొక్క_పంక్తి_Marker మునుపటి_eol = EOL_NONE;
                flockfile(f);

                కొరకు (;;) {
                        చివర_యొక్క_పంక్తి_Marker eol;
                        అక్షరం c;

                        ఒకవేళ (n >= limit)
                                తిరిగి -ENOBUFS;

                        ఒకవేళ (లెక్క >= గరిష్ఠ_పూర్ణాంకం) /* We couldn't return the counter anymore as "int", hence refuse this */
                                తిరిగి -ENOBUFS;

                        r = సురక్షిత_fgetc(f, &c);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0) /* EOF is definitely EOL */
                                ఆపు;

                        eol = categorize_eol(c, flags);

                        ఒకవేళ (FLAGS_సమితి(మునుపటి_eol, EOL_ZERO) ||
                            (eol == EOL_NONE && మునుపటి_eol != EOL_NONE) ||
                            (eol != EOL_NONE && (మునుపటి_eol & eol) != 0)) {
                                /* Previous char was a NUL? This is not an EOL, but the previous char was? This type of
                                 * EOL marker has been seen right before?  In either of these three cases we are
                                 * done. But first, let's put this character back in the queue. (Note that we have to
                                 * cast this to (unsigned char) here as ungetc() expects a positive 'int', and if we
                                 * are on an architecture where 'char' equals 'signed char' we need to ensure we don't
                                 * pass a negative value here. That said, to complicate things further ungetc() is
                                 * actually happy with most negative characters and implicitly casts them back to
                                 * positive ones as needed, except for \xff (aka -1, aka EOF), which it refuses. What a
                                 * godawful API!) */
                                assert_se(ungetc((గుర్తులేని అక్షరం) c, f) != EOF);
                                ఆపు;
                        }

                        లెక్క++;

                        ఒకవేళ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ఒకవేళ ((flags & (READ_పంక్తి_IS_A_TTY|READ_పంక్తి_NOT_A_TTY)) == 0) {
                                        పూర్ణాంకం fd;

                                        fd = fileno(f);
                                        ఒకవేళ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_పంక్తి_NOT_A_TTY;
                                        లేకపోతే
                                                flags |= isatty_సురక్షిత(fd) ? READ_పంక్తి_IS_A_TTY : READ_పంక్తి_NOT_A_TTY;
                                }
                                ఒకవేళ (FLAGS_సమితి(flags, READ_పంక్తి_IS_A_TTY))
                                        ఆపు;
                        }

                        ఒకవేళ (eol != EOL_NONE) {
                                మునుపటి_eol |= eol;
                                కొనసాగు;
                        }

                        ఒకవేళ (ret) {
                                ఒకవేళ (!GREEDY_REALLOC(బఫరు, n + 2))
                                        తిరిగి -ENOMEM;

                                బఫరు[n] = c;
                        }

                        n++;
                }
        }

        ఒకవేళ (ret) {
                బఫరు[n] = 0;

                *ret = TAKE_PTR(బఫరు);
        }

        తిరిగి (పూర్ణాంకం) లెక్క;
}

పూర్ణాంకం read_stripped_పంక్తి(FILE *f, size_t limit, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r, k;

        ప్రతిజ్ఞా(f);

        r = read_పంక్తి(f, limit, ret ? &s : ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret) {
                స్థిరరాశి అక్షరం *p = strstrip(s);
                ఒకవేళ (p == s)
                        *ret = TAKE_PTR(s);
                లేకపోతే {
                        k = strdup_గమ్యం(ret, p);
                        ఒకవేళ (k < 0)
                                తిరిగి k;
                }
        }

        తిరిగి r > 0;          /* Return 1 if something was read. */
}

పూర్ణాంకం సురక్షిత_fgetc(FILE *f, అక్షరం *ret) {
        పూర్ణాంకం k;

        ప్రతిజ్ఞా(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ఒకవేళ (k == EOF) {
                ఒకవేళ (ferror(f))
                        తిరిగి errno_or_else(EIO);

                ఒకవేళ (ret)
                        *ret = 0;

                తిరిగి 0;
        }

        ఒకవేళ (ret)
                *ret = k;

        తిరిగి 1;
}

పూర్ణాంకం warn_file_is_world_accessible(స్థిరరాశి అక్షరం *filename, నిర్మాణం stat *st, స్థిరరాశి అక్షరం *unit, గుర్తులేని పంక్తి) {
        నిర్మాణం stat _st;

        ఒకవేళ (!filename)
                తిరిగి 0;

        ఒకవేళ (!st) {
                ఒకవేళ (stat(filename, &_st) < 0)
                        తిరిగి -errno;
                st = &_st;
        }

        ఒకవేళ ((st->st_mode & S_IRWXO) == 0)
                తిరిగి 0;

        ఒకవేళ (unit)
                log_syntax(unit, LOG_WARNING, filename, పంక్తి, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        లేకపోతే
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        తిరిగి 0;
}

పూర్ణాంకం write_దత్తాంశం_file_atomic_at(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి నిర్మాణం iovec *iovec,
                Write_దత్తాంశం_File_Flags flags) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ అక్షరం *dn = ఖాళీ, *fn = ఖాళీ;
        r = మార్గం_విభజించు_ఉపసర్గ_filename(మార్గం, &dn, &fn);
        ఒకవేళ (లోపల_సమితి(r, -EADDRNOTAVAIL, O_DIRECTORY))
                తిరిగి -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_close_ పూర్ణాంకం mfd = -EBADF;
        ఒకవేళ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_సమితి(flags, WRITE_దత్తాంశం_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ఖాళీ,
                            &mfd);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                dir_fd = mfd;
        }

        _cleanup_free_ అక్షరం *t = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ఒకవేళ (iovec_is_సమితి(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        r = fchmod_umask(fd, FLAGS_సమితి(flags, WRITE_దత్తాంశం_FILE_బహుళకం_0400) ? 0400 : 0644);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ఒకవేళ (r < 0)
                తిరిగి r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        తిరిగి 0;
}
