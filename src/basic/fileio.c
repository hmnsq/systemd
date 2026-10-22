/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio_ext.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "label-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mkdir.h"
#অন্তর্ভাব "nulstr-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "socket-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "sync-util.h"
#অন্তর্ভাব "terminal-util.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "tmpfile-util.h"

/* The maximum size of the file we'll read in one go in read_full_file() (64M). */
#define READ_FULL_BYTES_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#define READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#define READ_VIRTUAL_BYTES_MAX (4U * U64_MB - UINT64_C(2))

পূর্ণসংখ্যা fdopen_unlocked(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *options, FILE **ret) {
        প্রতিজ্ঞা(ret);

        FILE *f = fdopen(fd, options);
        যদি (!f)
                ফেরত -errno;

        (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ফেরত 0;
}

পূর্ণসংখ্যা take_fdopen_unlocked(পূর্ণসংখ্যা *fd, ধ্রুবক অক্ষর *options, FILE **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd);

        r = fdopen_unlocked(*fd, options, ret);
        যদি (r < 0)
                ফেরত r;

        *fd = -EBADF;

        ফেরত 0;
}

FILE* take_fdopen(পূর্ণসংখ্যা *fd, ধ্রুবক অক্ষর *options) {
        প্রতিজ্ঞা(fd);

        FILE *f = fdopen(*fd, options);
        যদি (!f)
                ফেরত ফাঁকা;

        *fd = -EBADF;

        ফেরত f;
}

DIR* take_fdopendir(পূর্ণসংখ্যা *dfd) {
        প্রতিজ্ঞা(dfd);

        DIR *d = fdopendir(*dfd);
        যদি (!d)
                ফেরত ফাঁকা;

        *dfd = -EBADF;

        ফেরত d;
}

FILE* open_memstream_unlocked(অক্ষর **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        যদি (!f)
                ফেরত ফাঁকা;

        (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ফেরত f;
}

FILE* fmemopen_unlocked(শূন্য *buf, size_t আকার_2, ধ্রুবক অক্ষর *mode) {
        FILE *f = fmemopen(buf, আকার_2, mode);
        যদি (!f)
                ফেরত ফাঁকা;

        (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ফেরত f;
}

পূর্ণসংখ্যা write_string_stream_full(
                FILE *f,
                ধ্রুবক অক্ষর *সারি,
                WriteStringFileFlags flags,
                ধ্রুবক কাঠামো timespec *ts) {

        bool needs_nl;
        পূর্ণসংখ্যা r, fd = -EBADF;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(সারি);

        যদি (ferror(f))
                ফেরত -EIO;

        যদি (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                যদি (fd < 0)
                        ফেরত -EBADF;
        }

        যদি (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ অক্ষর *t = ফাঁকা;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                যদি (fd < 0) {
                        fd = fileno(f);
                        যদি (fd < 0)
                                ফেরত -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                যদি (read_virtual_file_fd(fd, strlen(সারি)+1, &t, ফাঁকা) > 0 &&
                    streq_skip_trailing_chars(সারি, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", সারি);
                        ফেরত 0;
                }

                যদি (lseek(fd, 0, SEEK_SET) < 0)
                        ফেরত -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(সারি, "\n");

        যদি (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_বাফার)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                সারি = strjoina(সারি, "\n");
                needs_nl = false;
        }

        যদি (fputs(সারি, f) == EOF)
                ফেরত -errno;

        যদি (needs_nl)
                যদি (fputc('\n', f) == EOF)
                        ফেরত -errno;

        যদি (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        অন্যথা
                r = fflush_and_check(f);
        যদি (r < 0)
                ফেরত r;

        যদি (ts) {
                ধ্রুবক কাঠামো timespec twice[2] = {*ts, *ts};

                প্রতিজ্ঞা(fd >= 0);
                যদি (futimens(fd, twice) < 0)
                        ফেরত -errno;
        }

        ফেরত 0;
}

স্থির mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ফেরত FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

স্থির পূর্ণসংখ্যা write_string_file_atomic_at(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *fn,
                ধ্রুবক অক্ষর *সারি,
                WriteStringFileFlags flags,
                ধ্রুবক কাঠামো timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fn);
        প্রতিজ্ঞা(সারি);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        যদি (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                যদি (r < 0)
                        ফেরত r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        পূর্ণসংখ্যা k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ফাঁকা : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        যদি (r < 0)
                ফেরত r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        যদি (k < 0)
                ফেরত k;

        r = write_string_stream_full(f, সারি, flags, ts);
        যদি (r < 0)
                ফেরত r;

        r = fchmod_umask(fileno(f), mode);
        যদি (r < 0)
                ফেরত r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        যদি (r < 0)
                ফেরত r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        যদি (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা write_string_file_full_label(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *fn,
                ধ্রুবক অক্ষর *সারি,
                WriteStringFileFlags flags,
                ধ্রুবক কাঠামো timespec *ts,
                ধ্রুবক অক্ষর *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(dir_fd == AT_FDCWD || dir_fd >= 0);
        প্রতিজ্ঞা(সারি);

        /* We don't know how to verify whether the file contents was already on-disk. */
        প্রতিজ্ঞা(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        যদি (flags & WRITE_STRING_FILE_MKDIR_0755) {
                প্রতিজ্ঞা(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (flags & WRITE_STRING_FILE_ATOMIC) {
                প্রতিজ্ঞা(fn);
                প্রতিজ্ঞা(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, সারি, flags, ts, label_context);
                যদি (r < 0)
                        যাও fail;

                ফেরত r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        যদি (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        অন্যথা {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                যদি (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        যদি (r < 0)
                                যাও fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                mode,
                                &made_file);
                যদি (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ফাঁকা : fn, made_file, label_context));
        }
        যদি (r < 0)
                যাও fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        যদি (r < 0)
                যাও fail;

        যদি (flags & WRITE_STRING_FILE_DISABLE_বাফার)
                setvbuf(f, ফাঁকা, _IONBF, 0);

        r = write_string_stream_full(f, সারি, flags, ts);
        যদি (r < 0)
                যাও fail;

        ফেরত 0;

fail:
        যদি (made_file)
                (শূন্য) unlinkat(dir_fd, fn, 0);

        যদি (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ফেরত r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        যদি (verify_file_at(dir_fd, fn, সারি, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ফেরত 0;

        ফেরত r;
}

পূর্ণসংখ্যা write_string_filef(
                ধ্রুবক অক্ষর *fn,
                WriteStringFileFlags flags,
                ধ্রুবক অক্ষর *format, ...) {

        _cleanup_free_ অক্ষর *p = ফাঁকা;
        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        যদি (r < 0)
                ফেরত -ENOMEM;

        ফেরত write_string_file(fn, p, flags);
}

পূর্ণসংখ্যা write_string_filef_at(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *fn,
                WriteStringFileFlags flags,
                ধ্রুবক অক্ষর *format, ...) {

        _cleanup_free_ অক্ষর *p = ফাঁকা;
        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        যদি (r < 0)
                ফেরত -ENOMEM;

        ফেরত write_string_file_at(dir_fd, fn, p, flags);
}

পূর্ণসংখ্যা write_base64_file_at(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *fn,
                ধ্রুবক কাঠামো iovec *তথ্য,
                WriteStringFileFlags flags) {

        _cleanup_free_ অক্ষর *encoded = ফাঁকা;
        ssize_t n;

        n = base64mem_full(তথ্য ? তথ্য->iov_base : ফাঁকা, তথ্য ? তথ্য->iov_len : 0, 79, &encoded);
        যদি (n < 0)
                ফেরত n;

        ফেরত write_string_file_at(dir_fd, fn, encoded, flags);
}

পূর্ণসংখ্যা read_one_সারি_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename, অক্ষর **ret) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(filename);
        প্রতিজ্ঞা(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        যদি (r < 0)
                ফেরত r;

        ফেরত read_সারি(f, LONG_সারি_MAX, ret);
}

পূর্ণসংখ্যা read_boolean_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(filename);

        r = read_one_সারি_file_at(dir_fd, filename, &s);
        যদি (r < 0)
                ফেরত r;

        ফেরত parse_boolean(s);
}

পূর্ণসংখ্যা verify_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        size_t l, k;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(blob);

        l = strlen(blob);

        যদি (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        যদি (!buf)
                ফেরত -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        যদি (r < 0)
                ফেরত r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        যদি (ferror(f))
                ফেরত errno_or_else(EIO);

        যদি (k != l && k != l + accept_extra_nl)
                ফেরত 0;
        যদি (memcmp(buf, blob, l) != 0)
                ফেরত 0;
        যদি (k > l && buf[l] != '\n')
                ফেরত 0;

        ফেরত 1;
}

পূর্ণসংখ্যা read_virtual_file_at(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *filename,
                size_t max_আকার,
                অক্ষর **ret_contents,
                size_t *ret_আকার) {

        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        size_t n, আকার_2;
        পূর্ণসংখ্যা n_retries;
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

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(max_আকার <= READ_VIRTUAL_BYTES_MAX || max_আকার == SIZE_MAX);

        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        যদি (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        অন্যথা
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        যদি (fd < 0)
                ফেরত fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        জন্য (;;) {
                কাঠামো stat st;

                যদি (fstat(fd, &st) < 0)
                        ফেরত -errno;

                যদি (!S_ISREG(st.st_mode))
                        ফেরত -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                যদি (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        যদি (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                যদি (max_আকার == SIZE_MAX)
                                        ফেরত -EFBIG;

                                আকার_2 = max_আকার;
                        } অন্যথা {
                                আকার_2 = MIN((size_t) st.st_size, max_আকার);

                                যদি (আকার_2 > READ_VIRTUAL_BYTES_MAX)
                                        ফেরত -EFBIG;
                        }

                        n_retries--;
                } অন্যথা যদি (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        আকার_2 = MIN3(page_আকার() - 1, READ_VIRTUAL_BYTES_MAX, max_আকার);
                        n_retries = 1;
                } অন্যথা {
                        আকার_2 = MIN(READ_VIRTUAL_BYTES_MAX, max_আকার);
                        n_retries = 0;
                }

                buf = malloc(আকার_2 + 1);
                যদি (!buf)
                        ফেরত -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                আকার_2 = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_আকার, READ_VIRTUAL_BYTES_MAX);

                জন্য (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = পঠ(fd, buf, আকার_2 + 1);
                        যদি (k >= 0) {
                                n = k;
                                বিরতি;
                        }

                        যদি (errno != EINTR)
                                ফেরত -errno;
                }

                /* Consider a short read as EOF */
                যদি (n <= আকার_2)
                        বিরতি;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                যদি (max_আকার != SIZE_MAX && n > max_আকার) {
                        n = আকার_2; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        বিরতি;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                যদি (n_retries <= 0)
                        ফেরত -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                যদি (lseek(fd, 0, SEEK_SET) < 0)
                        ফেরত -errno;

                buf = mfree(buf);
        }

        যদি (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                যদি (!ret_আকার && memchr(buf, 0, n))
                        ফেরত -EBADMSG;

                যদি (n < আকার_2) {
                        অক্ষর *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        যদি (!p)
                                ফেরত -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        যদি (ret_আকার)
                *ret_আকার = n;

        ফেরত !truncated;
}

পূর্ণসংখ্যা read_full_stream_full(
                FILE *f,
                ধ্রুবক অক্ষর *filename,
                uint64_t offset,
                size_t আকার_2,
                ReadFullFileFlags flags,
                অক্ষর **ret_contents,
                size_t *ret_আকার) {

        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        size_t n, n_পরবর্তী = 0, l, expected_decoded_আকার = আকার_2;
        পূর্ণসংখ্যা fd, r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret_contents);
        প্রতিজ্ঞা(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        প্রতিজ্ঞা(আকার_2 != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        যদি (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ফেরত -ERANGE;

        যদি ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                যদি (আকার_2 <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        আকার_2 *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                অন্যথা
                        আকার_2 = SIZE_MAX;
        }

        fd = fileno(f);
        যদি (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                কাঠামো stat st;

                যদি (fstat(fd, &st) < 0)
                        ফেরত -errno;

                যদি (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        যদি (r < 0)
                                ফেরত r;
                }

                যদি (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        যদি (st.st_size > 0 &&
                            (আকার_2 == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                যদি (rsize < SIZE_MAX) /* overflow check */
                                        n_পরবর্তী = rsize + 1;
                        }

                        যদি (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (শূন্য) warn_file_is_world_accessible(filename, &st, ফাঁকা, 0);
                }
        } অন্যথা যদি (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ফেরত -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        যদি (n_পরবর্তী == 0)
                n_পরবর্তী = আকার_2 != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? আকার_2 : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        যদি (n_পরবর্তী > READ_FULL_BYTES_MAX)
                n_পরবর্তী = READ_FULL_BYTES_MAX + 1;

        যদি (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ফেরত -errno;

        n = l = 0;
        জন্য (;;) {
                অক্ষর *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                যদি (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_পরবর্তী > আকার_2)
                        n_পরবর্তী = আকার_2 + 1;

                যদি (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_পরবর্তী + 1);
                        যদি (!t) {
                                r = -ENOMEM;
                                যাও finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        মুক্ত(buf);
                } অন্যথা {
                        t = realloc(buf, n_পরবর্তী + 1);
                        যদি (!t)
                                ফেরত -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = আকার_2 == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_পরবর্তী;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                প্রতিজ্ঞা(k <= n - l);
                l += k;

                যদি (ferror(f)) {
                        r = errno_or_else(EIO);
                        যাও finalize;
                }
                যদি (feof(f))
                        বিরতি;

                যদি (আকার_2 != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        প্রতিজ্ঞা(l == আকার_2);
                        বিরতি;
                }

                প্রতিজ্ঞা(k > 0); /* we can't have read zero bytes because that would have been EOF */

                যদি (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > আকার_2) {
                        r = -E2BIG;
                        যাও finalize;
                }

                যদি (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        যাও finalize;
                }

                n_পরবর্তী = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        যদি (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ শূন্য *decoded = ফাঁকা;
                size_t decoded_আকার;

                buf[l++] = 0;
                যদি (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_আকার);
                অন্যথা
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_আকার);
                যদি (r < 0)
                        যাও finalize;

                যদি (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_আকার;

                যদি (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_আকার) {
                        r = -E2BIG;
                        যাও finalize;
                }
        }

        যদি (!ret_আকার) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                যদি (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        যাও finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        যদি (ret_আকার)
                *ret_আকার = l;

        ফেরত 0;

finalize:
        যদি (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ফেরত r;
}

পূর্ণসংখ্যা read_full_file_full(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *filename,
                uint64_t offset,
                size_t আকার_2,
                ReadFullFileFlags flags,
                ধ্রুবক অক্ষর *bind_name,
                অক্ষর **ret_contents,
                size_t *ret_আকার) {

        _cleanup_fclose_ FILE *f = ফাঁকা;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_contents);

        যদি (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        যদি (r < 0)
                ফেরত r;

        ফেরত read_full_stream_full(f, filename, offset, আকার_2, flags, ret_contents, ret_আকার);
}

পূর্ণসংখ্যা script_get_shebang_interpreter(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        f = fopen(path, "re");
        যদি (!f)
                ফেরত -errno;

        অক্ষর c;
        r = safe_fgetc(f, &c);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EBADMSG;
        যদি (c != '#')
                ফেরত -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EBADMSG;
        যদি (c != '!')
                ফেরত -EMEDIUMTYPE;

        _cleanup_free_ অক্ষর *সারি = ফাঁকা;
        r = read_সারি(f, LONG_সারি_MAX, &সারি);
        যদি (r < 0)
                ফেরত r;

        _cleanup_free_ অক্ষর *p = ফাঁকা;
        ধ্রুবক অক্ষর *s = সারি;

        r = extract_first_word(&s, &p, /* separators= */ ফাঁকা, /* flags= */ 0);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -ENOEXEC;

        যদি (ret)
                *ret = TAKE_PTR(p);
        ফেরত 0;
}

পূর্ণসংখ্যা get_proc_field(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *চাবি, অক্ষর **ret) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(চাবি);

        r = fopen_unlocked(path, "re", &f);
        যদি (r == -ENOENT && proc_mounted() == 0)
                ফেরত -ENOSYS;
        যদি (r < 0)
                ফেরত r;

        জন্য (;;) {
                 _cleanup_free_ অক্ষর *সারি = ফাঁকা;

                 r = read_সারি(f, LONG_সারি_MAX, &সারি);
                 যদি (r < 0)
                         ফেরত r;
                 যদি (r == 0)
                         ফেরত -ENODATA;

                 অক্ষর *l = startswith(সারি, চাবি);
                 যদি (l && *l == ':') {
                         যদি (ret) {
                                 অক্ষর *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 যদি (!s)
                                         ফেরত -ENOMEM;

                                 *ret = s;
                         }

                         ফেরত 0;
                 }
        }
}

DIR* xopendirat(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(!(flags & (O_CREAT|O_TMPFILE)));

        যদি ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ফেরত opendir(path);

        যদি (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        যদি (fd < 0)
                ফেরত ফাঁকা;

        ফেরত take_fdopendir(&fd);
}

পূর্ণসংখ্যা fopen_mode_to_flags(ধ্রুবক অক্ষর *mode) {
        ধ্রুবক অক্ষর *p;
        পূর্ণসংখ্যা flags;

        প্রতিজ্ঞা(mode);

        যদি ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        অন্যথা যদি ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        অন্যথা যদি ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        অন্যথা যদি ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        অন্যথা যদি ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        অন্যথা যদি ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        অন্যথা
                ফেরত -EINVAL;

        জন্য (; *p != 0; p++) {

                নির্বাচন (*p) {

                ক্ষেত্র 'e':
                        flags |= O_CLOEXEC;
                        বিরতি;

                ক্ষেত্র 'x':
                        flags |= O_EXCL;
                        বিরতি;

                ক্ষেত্র 'm':
                        /* ignore this here, fdopen() might care later though */
                        বিরতি;

                ক্ষেত্র 'c': /* not sure what to do about this one */
                পূর্বনির্ধারিত:
                        ফেরত -EINVAL;
                }
        }

        ফেরত flags;
}

স্থির পূর্ণসংখ্যা xfopenat_regular(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *mode, পূর্ণসংখ্যা open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(mode);
        প্রতিজ্ঞা(ret);

        যদি (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        অন্যথা যদি (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ অক্ষর *j = strjoin("/", path);
                যদি (!j)
                        ফেরত -ENOMEM;

                f = fopen(j, mode);
        } অন্যথা {
                _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
                পূর্ণসংখ্যা mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                যদি (mode_flags < 0)
                        ফেরত mode_flags;

                যদি (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        যদি (fd < 0)
                                ফেরত -errno;
                } অন্যথা {
                        যদি (dir_fd == AT_FDCWD)
                                ফেরত -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        যদি (fd < 0)
                                ফেরত fd;
                }

                f = take_fdopen(&fd, mode);
        }
        যদি (!f)
                ফেরত -errno;

        *ret = f;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা xfopenat_unix_socket(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *bind_name, FILE **ret) {
        _cleanup_close_ পূর্ণসংখ্যা sk = -EBADF;
        FILE *f;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        যদি (sk < 0)
                ফেরত -errno;

        যদি (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                সংঘ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                যদি (r < 0)
                        ফেরত r;

                যদি (bind(sk, &bsa.sa, r) < 0)
                        ফেরত -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        যদি (r < 0)
                ফেরত r;

        যদি (shutdown(sk, SHUT_WR) < 0)
                ফেরত -errno;

        f = take_fdopen(&sk, "r");
        যদি (!f)
                ফেরত -errno;

        *ret = f;
        ফেরত 0;
}

পূর্ণসংখ্যা xfopenat_full(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *mode,
                পূর্ণসংখ্যা open_flags,
                XfopenFlags flags,
                ধ্রুবক অক্ষর *bind_name,
                FILE **ret) {

        FILE *f = ফাঁকা;  /* avoid false maybe-uninitialized warning */
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(mode);
        প্রতিজ্ঞা(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        যদি (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                যদি (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ফেরত -ENXIO; /* propagate original error if this is not a socket after all */
        }
        যদি (r < 0)
                ফেরত r;

        যদি (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ফেরত 0;
}

পূর্ণসংখ্যা fdopen_independent(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *mode, FILE **ret) {
        _cleanup_close_ পূর্ণসংখ্যা copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা mode_flags;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(mode);
        প্রতিজ্ঞা(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        যদি (mode_flags < 0)
                ফেরত mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        যদি (copy_fd < 0)
                ফেরত copy_fd;

        f = take_fdopen(&copy_fd, mode);
        যদি (!f)
                ফেরত -errno;

        *ret = TAKE_PTR(f);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা অনুসন্ধান_and_open_internal(
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ধ্রুবক অক্ষর *মূল,
                অক্ষর **অনুসন্ধান,
                পূর্ণসংখ্যা *ret_fd,
                অক্ষর **ret_path) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        প্রতিজ্ঞা(path);

        যদি (path_is_absolute(path)) {
                _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

                যদি (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                অন্যথা
                        r = RET_NERRNO(access(path, mode));
                যদি (r < 0)
                        ফেরত r;

                যদি (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        যদি (r < 0)
                                ফেরত r;
                }

                যদি (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ফেরত 0;
        }

        যদি (!path_strv_resolve_uniq(অনুসন্ধান, মূল))
                ফেরত -ENOMEM;

        STRV_FOREACH(i, অনুসন্ধান) {
                _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
                _cleanup_free_ অক্ষর *p = ফাঁকা;

                p = path_join(মূল, *i, path);
                যদি (!p)
                        ফেরত -ENOMEM;

                যদি (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                অন্যথা
                        r = RET_NERRNO(access(p, F_OK));
                যদি (r >= 0) {
                        যদি (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        যদি (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ফেরত 0;
                }
                যদি (r != -ENOENT)
                        ফেরত r;
        }

        ফেরত -ENOENT;
}

পূর্ণসংখ্যা অনুসন্ধান_and_open(
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা mode,
                ধ্রুবক অক্ষর *মূল,
                অক্ষর **অনুসন্ধান,
                পূর্ণসংখ্যা *ret_fd,
                অক্ষর **ret_path) {

        _cleanup_strv_free_ অক্ষর **copy = ফাঁকা;

        প্রতিজ্ঞা(path);

        copy = strv_copy(অনুসন্ধান);
        যদি (!copy)
                ফেরত -ENOMEM;

        ফেরত অনুসন্ধান_and_open_internal(path, mode, মূল, copy, ret_fd, ret_path);
}

স্থির পূর্ণসংখ্যা অনুসন্ধান_and_fopen_internal(
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *mode,
                ধ্রুবক অক্ষর *মূল,
                অক্ষর **অনুসন্ধান,
                FILE **ret_file,
                অক্ষর **ret_path) {

        _cleanup_free_ অক্ষর *found_path = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode || !ret_file);

        r = অনুসন্ধান_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        মূল,
                        অনুসন্ধান,
                        ret_file ? &fd : ফাঁকা,
                        ret_path ? &found_path : ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                যদি (!f)
                        ফেরত -errno;

                *ret_file = f;
        }

        যদি (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ফেরত 0;
}

পূর্ণসংখ্যা অনুসন্ধান_and_fopen(
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *mode,
                ধ্রুবক অক্ষর *মূল,
                ধ্রুবক অক্ষর **অনুসন্ধান,
                FILE **ret_file,
                অক্ষর **ret_path) {

        _cleanup_strv_free_ অক্ষর **copy = ফাঁকা;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode || !ret_file);

        copy = strv_copy((অক্ষর**) অনুসন্ধান);
        যদি (!copy)
                ফেরত -ENOMEM;

        ফেরত অনুসন্ধান_and_fopen_internal(path, mode, মূল, copy, ret_file, ret_path);
}

পূর্ণসংখ্যা অনুসন্ধান_and_fopen_nulstr(
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *mode,
                ধ্রুবক অক্ষর *মূল,
                ধ্রুবক অক্ষর *অনুসন্ধান,
                FILE **ret_file,
                অক্ষর **ret_path) {

        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode || !ret_file);

        l = strv_split_nulstr(অনুসন্ধান);
        যদি (!l)
                ফেরত -ENOMEM;

        ফেরত অনুসন্ধান_and_fopen_internal(path, mode, মূল, l, ret_file, ret_path);
}

পূর্ণসংখ্যা fflush_and_check(FILE *f) {
        প্রতিজ্ঞা(f);

        errno = 0;
        fflush(f);

        যদি (ferror(f))
                ফেরত errno_or_else(EIO);

        ফেরত 0;
}

পূর্ণসংখ্যা fflush_sync_and_check(FILE *f) {
        পূর্ণসংখ্যা r, fd;

        প্রতিজ্ঞা(f);

        r = fflush_and_check(f);
        যদি (r < 0)
                ফেরত r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        যদি (fd < 0)
                ফেরত 0;

        r = fsync_full(fd);
        যদি (r < 0)
                ফেরত r;

        ফেরত 0;
}

পূর্ণসংখ্যা write_timestamp_file_atomic(ধ্রুবক অক্ষর *fn, usec_t n) {
        অক্ষর ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        যদি (!timestamp_is_set(n))
                ফেরত -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ফেরত write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

পূর্ণসংখ্যা read_timestamp_file(ধ্রুবক অক্ষর *fn, usec_t *ret) {
        _cleanup_free_ অক্ষর *ln = ফাঁকা;
        uint64_t t;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = read_one_সারি_file(fn, &ln);
        যদি (r < 0)
                ফেরত r;

        r = safe_atou64(ln, &t);
        যদি (r < 0)
                ফেরত r;

        যদি (!timestamp_is_set(t))
                ফেরত -ERANGE;

        *ret = (usec_t) t;
        ফেরত 0;
}

পূর্ণসংখ্যা fputs_with_separator(FILE *f, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separator, bool *space) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        যদি (!f)
                f = stdout;

        যদি (!separator)
                separator = " ";

        যদি (*space)
                যদি (fputs(separator, f) < 0)
                        ফেরত -EIO;

        *space = true;

        যদি (fputs(s, f) < 0)
                ফেরত -EIO;

        ফেরত 0;
}

পূর্ণসংখ্যা fputs_with_newline(FILE *f, ধ্রুবক অক্ষর *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        যদি (isempty(s))
                ফেরত 0;

        যদি (!f)
                f = stdout;

        যদি (fputs(s, f) < 0)
                ফেরত -EIO;

        যদি (endswith(s, "\n"))
                ফেরত 0;

        যদি (fputc('\n', f) < 0)
                ফেরত -EIO;

        ফেরত 1;
}

/* A bitmask of the EOL markers we know */
প্রকারলেখ গণনা End_Of_সারি_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_সারি_Marker;

স্থির End_Of_সারি_Marker categorize_eol(অক্ষর c, Read_সারি_Flags flags) {

        যদি (!FLAGS_SET(flags, READ_সারি_ONLY_NUL)) {
                যদি (c == '\n')
                        ফেরত EOL_TEN;
                যদি (c == '\r')
                        ফেরত EOL_THIRTEEN;
        }

        যদি (c == '\0')
                ফেরত EOL_ZERO;

        ফেরত EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, ফাঁকা);

পূর্ণসংখ্যা read_সারি_full(FILE *f, size_t limit, Read_সারি_Flags flags, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *বাফার = ফাঁকা;
        size_t n = 0, গুনতি = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f);

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

        যদি (ret) {
                যদি (!GREEDY_REALLOC(বাফার, 1))
                        ফেরত -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_সারি_Marker previous_eol = EOL_NONE;
                flockfile(f);

                জন্য (;;) {
                        End_Of_সারি_Marker eol;
                        অক্ষর c;

                        যদি (n >= limit)
                                ফেরত -ENOBUFS;

                        যদি (গুনতি >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ফেরত -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0) /* EOF is definitely EOL */
                                বিরতি;

                        eol = categorize_eol(c, flags);

                        যদি (FLAGS_SET(previous_eol, EOL_ZERO) ||
                            (eol == EOL_NONE && previous_eol != EOL_NONE) ||
                            (eol != EOL_NONE && (previous_eol & eol) != 0)) {
                                /* Previous char was a NUL? This is not an EOL, but the previous char was? This type of
                                 * EOL marker has been seen right before?  In either of these three cases we are
                                 * done. But first, let's put this character back in the queue. (Note that we have to
                                 * cast this to (unsigned char) here as ungetc() expects a positive 'int', and if we
                                 * are on an architecture where 'char' equals 'signed char' we need to ensure we don't
                                 * pass a negative value here. That said, to complicate things further ungetc() is
                                 * actually happy with most negative characters and implicitly casts them back to
                                 * positive ones as needed, except for \xff (aka -1, aka EOF), which it refuses. What a
                                 * godawful API!) */
                                assert_se(ungetc((unsigned অক্ষর) c, f) != EOF);
                                বিরতি;
                        }

                        গুনতি++;

                        যদি (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                যদি ((flags & (READ_সারি_IS_A_TTY|READ_সারি_NOT_A_TTY)) == 0) {
                                        পূর্ণসংখ্যা fd;

                                        fd = fileno(f);
                                        যদি (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_সারি_NOT_A_TTY;
                                        অন্যথা
                                                flags |= isatty_safe(fd) ? READ_সারি_IS_A_TTY : READ_সারি_NOT_A_TTY;
                                }
                                যদি (FLAGS_SET(flags, READ_সারি_IS_A_TTY))
                                        বিরতি;
                        }

                        যদি (eol != EOL_NONE) {
                                previous_eol |= eol;
                                চালু;
                        }

                        যদি (ret) {
                                যদি (!GREEDY_REALLOC(বাফার, n + 2))
                                        ফেরত -ENOMEM;

                                বাফার[n] = c;
                        }

                        n++;
                }
        }

        যদি (ret) {
                বাফার[n] = 0;

                *ret = TAKE_PTR(বাফার);
        }

        ফেরত (পূর্ণসংখ্যা) গুনতি;
}

পূর্ণসংখ্যা read_stripped_সারি(FILE *f, size_t limit, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r, k;

        প্রতিজ্ঞা(f);

        r = read_সারি(f, limit, ret ? &s : ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        যদি (ret) {
                ধ্রুবক অক্ষর *p = strstrip(s);
                যদি (p == s)
                        *ret = TAKE_PTR(s);
                অন্যথা {
                        k = strdup_to(ret, p);
                        যদি (k < 0)
                                ফেরত k;
                }
        }

        ফেরত r > 0;          /* Return 1 if something was read. */
}

পূর্ণসংখ্যা safe_fgetc(FILE *f, অক্ষর *ret) {
        পূর্ণসংখ্যা k;

        প্রতিজ্ঞা(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        যদি (k == EOF) {
                যদি (ferror(f))
                        ফেরত errno_or_else(EIO);

                যদি (ret)
                        *ret = 0;

                ফেরত 0;
        }

        যদি (ret)
                *ret = k;

        ফেরত 1;
}

পূর্ণসংখ্যা warn_file_is_world_accessible(ধ্রুবক অক্ষর *filename, কাঠামো stat *st, ধ্রুবক অক্ষর *unit, unsigned সারি) {
        কাঠামো stat _st;

        যদি (!filename)
                ফেরত 0;

        যদি (!st) {
                যদি (stat(filename, &_st) < 0)
                        ফেরত -errno;
                st = &_st;
        }

        যদি ((st->st_mode & S_IRWXO) == 0)
                ফেরত 0;

        যদি (unit)
                log_syntax(unit, LOG_WARNING, filename, সারি, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        অন্যথা
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ফেরত 0;
}

পূর্ণসংখ্যা write_তথ্য_file_atomic_at(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *path,
                ধ্রুবক কাঠামো iovec *iovec,
                Write_তথ্য_File_Flags flags) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ অক্ষর *dn = ফাঁকা, *fn = ফাঁকা;
        r = path_split_prefix_filename(path, &dn, &fn);
        যদি (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ফেরত -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        যদি (r < 0)
                ফেরত r;

        _cleanup_close_ পূর্ণসংখ্যা mfd = -EBADF;
        যদি (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_তথ্য_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ফাঁকা,
                            &mfd);
                যদি (r < 0)
                        ফেরত r;

                dir_fd = mfd;
        }

        _cleanup_free_ অক্ষর *t = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        যদি (fd < 0)
                ফেরত fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        যদি (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                যদি (r < 0)
                        ফেরত r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_তথ্য_FILE_MODE_0400) ? 0400 : 0644);
        যদি (r < 0)
                ফেরত r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        যদি (r < 0)
                ফেরত r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ফেরত 0;
}
