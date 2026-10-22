/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stdio_ext.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "chase.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "io-util.h"
#অন্তর্ভাৱ "iovec-util.h"
#অন্তর্ভাৱ "label-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "mkdir.h"
#অন্তর্ভাৱ "nulstr-util.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "socket-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "sync-util.h"
#অন্তর্ভাৱ "terminal-util.h"
#অন্তর্ভাৱ "time-util.h"
#অন্তর্ভাৱ "tmpfile-util.h"

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

পূর্ণাঙ্ক fdopen_unlocked(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *options, FILE **ret) {
        প্রতিজ্ঞা(ret);

        FILE *f = fdopen(fd, options);
        যদি (!f)
                ঘূৰাই -errno;

        (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক take_fdopen_unlocked(পূর্ণাঙ্ক *fd, ধ্রুৱ ৱর্ণ *options, FILE **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd);

        r = fdopen_unlocked(*fd, options, ret);
        যদি (r < 0)
                ঘূৰাই r;

        *fd = -EBADF;

        ঘূৰাই 0;
}

FILE* take_fdopen(পূর্ণাঙ্ক *fd, ধ্রুৱ ৱর্ণ *options) {
        প্রতিজ্ঞা(fd);

        FILE *f = fdopen(*fd, options);
        যদি (!f)
                ঘূৰাই রিক্ত;

        *fd = -EBADF;

        ঘূৰাই f;
}

DIR* take_fdopendir(পূর্ণাঙ্ক *dfd) {
        প্রতিজ্ঞা(dfd);

        DIR *d = fdopendir(*dfd);
        যদি (!d)
                ঘূৰাই রিক্ত;

        *dfd = -EBADF;

        ঘূৰাই d;
}

FILE* open_memstream_unlocked(ৱর্ণ **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        যদি (!f)
                ঘূৰাই রিক্ত;

        (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ঘূৰাই f;
}

FILE* fmemopen_unlocked(শূন্য *buf, size_t size, ধ্রুৱ ৱর্ণ *mode) {
        FILE *f = fmemopen(buf, size, mode);
        যদি (!f)
                ঘূৰাই রিক্ত;

        (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ঘূৰাই f;
}

পূর্ণাঙ্ক write_string_stream_full(
                FILE *f,
                ধ্রুৱ ৱর্ণ *সাৰি,
                WriteStringFileFlags flags,
                ধ্রুৱ সংরচনা timespec *ts) {

        bool needs_nl;
        পূর্ণাঙ্ক r, fd = -EBADF;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(সাৰি);

        যদি (ferror(f))
                ঘূৰাই -EIO;

        যদি (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                যদি (fd < 0)
                        ঘূৰাই -EBADF;
        }

        যদি (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ ৱর্ণ *t = রিক্ত;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                যদি (fd < 0) {
                        fd = fileno(f);
                        যদি (fd < 0)
                                ঘূৰাই -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                যদি (read_virtual_file_fd(fd, strlen(সাৰি)+1, &t, রিক্ত) > 0 &&
                    streq_skip_trailing_chars(সাৰি, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", সাৰি);
                        ঘূৰাই 0;
                }

                যদি (lseek(fd, 0, SEEK_SET) < 0)
                        ঘূৰাই -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(সাৰি, "\n");

        যদি (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                সাৰি = strjoina(সাৰি, "\n");
                needs_nl = false;
        }

        যদি (fputs(সাৰি, f) == EOF)
                ঘূৰাই -errno;

        যদি (needs_nl)
                যদি (fputc('\n', f) == EOF)
                        ঘূৰাই -errno;

        যদি (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        অন্যথা
                r = fflush_and_check(f);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ts) {
                ধ্রুৱ সংরচনা timespec twice[2] = {*ts, *ts};

                প্রতিজ্ঞা(fd >= 0);
                যদি (futimens(fd, twice) < 0)
                        ঘূৰাই -errno;
        }

        ঘূৰাই 0;
}

স্থির mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ঘূৰাই FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

স্থির পূর্ণাঙ্ক write_string_file_atomic_at(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *fn,
                ধ্রুৱ ৱর্ণ *সাৰি,
                WriteStringFileFlags flags,
                ধ্রুৱ সংরচনা timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fn);
        প্রতিজ্ঞা(সাৰি);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        যদি (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                যদি (r < 0)
                        ঘূৰাই r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        পূর্ণাঙ্ক k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? রিক্ত : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        যদি (r < 0)
                ঘূৰাই r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        যদি (k < 0)
                ঘূৰাই k;

        r = write_string_stream_full(f, সাৰি, flags, ts);
        যদি (r < 0)
                ঘূৰাই r;

        r = fchmod_umask(fileno(f), mode);
        যদি (r < 0)
                ঘূৰাই r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        যদি (r < 0)
                ঘূৰাই r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        যদি (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক write_string_file_full_label(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *fn,
                ধ্রুৱ ৱর্ণ *সাৰি,
                WriteStringFileFlags flags,
                ধ্রুৱ সংরচনা timespec *ts,
                ধ্রুৱ ৱর্ণ *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd == AT_FDCWD || dir_fd >= 0);
        প্রতিজ্ঞা(সাৰি);

        /* We don't know how to verify whether the file contents was already on-disk. */
        প্রতিজ্ঞা(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        যদি (flags & WRITE_STRING_FILE_MKDIR_0755) {
                প্রতিজ্ঞা(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (flags & WRITE_STRING_FILE_ATOMIC) {
                প্রতিজ্ঞা(fn);
                প্রতিজ্ঞা(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, সাৰি, flags, ts, label_context);
                যদি (r < 0)
                        গচ্ছ fail;

                ঘূৰাই r;
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
                                গচ্ছ fail;

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
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? রিক্ত : fn, made_file, label_context));
        }
        যদি (r < 0)
                গচ্ছ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        যদি (r < 0)
                গচ্ছ fail;

        যদি (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, রিক্ত, _IONBF, 0);

        r = write_string_stream_full(f, সাৰি, flags, ts);
        যদি (r < 0)
                গচ্ছ fail;

        ঘূৰাই 0;

fail:
        যদি (made_file)
                (শূন্য) unlinkat(dir_fd, fn, 0);

        যদি (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ঘূৰাই r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        যদি (verify_file_at(dir_fd, fn, সাৰি, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ঘূৰাই 0;

        ঘূৰাই r;
}

পূর্ণাঙ্ক write_string_filef(
                ধ্রুৱ ৱর্ণ *fn,
                WriteStringFileFlags flags,
                ধ্রুৱ ৱর্ণ *format, ...) {

        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই -ENOMEM;

        ঘূৰাই write_string_file(fn, p, flags);
}

পূর্ণাঙ্ক write_string_filef_at(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *fn,
                WriteStringFileFlags flags,
                ধ্রুৱ ৱর্ণ *format, ...) {

        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই -ENOMEM;

        ঘূৰাই write_string_file_at(dir_fd, fn, p, flags);
}

পূর্ণাঙ্ক write_base64_file_at(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *fn,
                ধ্রুৱ সংরচনা iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ ৱর্ণ *encoded = রিক্ত;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : রিক্ত, data ? data->iov_len : 0, 79, &encoded);
        যদি (n < 0)
                ঘূৰাই n;

        ঘূৰাই write_string_file_at(dir_fd, fn, encoded, flags);
}

পূর্ণাঙ্ক read_one_সাৰি_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *filename, ৱর্ণ **ret) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(filename);
        প্রতিজ্ঞা(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই read_সাৰি(f, LONG_সাৰি_MAX, ret);
}

পূর্ণাঙ্ক read_boolean_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *filename) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(filename);

        r = read_one_সাৰি_file_at(dir_fd, filename, &s);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই parse_boolean(s);
}

পূর্ণাঙ্ক verify_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *fn, ধ্রুৱ ৱর্ণ *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_free_ ৱর্ণ *buf = রিক্ত;
        size_t l, k;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(blob);

        l = strlen(blob);

        যদি (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        যদি (!buf)
                ঘূৰাই -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        যদি (r < 0)
                ঘূৰাই r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        যদি (ferror(f))
                ঘূৰাই errno_or_else(EIO);

        যদি (k != l && k != l + accept_extra_nl)
                ঘূৰাই 0;
        যদি (memcmp(buf, blob, l) != 0)
                ঘূৰাই 0;
        যদি (k > l && buf[l] != '\n')
                ঘূৰাই 0;

        ঘূৰাই 1;
}

পূর্ণাঙ্ক read_virtual_file_at(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *filename,
                size_t max_size,
                ৱর্ণ **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ ৱর্ণ *buf = রিক্ত;
        size_t n, size;
        পূর্ণাঙ্ক n_retries;
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
        প্রতিজ্ঞা(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        যদি (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        অন্যথা
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        যদি (fd < 0)
                ঘূৰাই fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        বাবে (;;) {
                সংরচনা stat st;

                যদি (fstat(fd, &st) < 0)
                        ঘূৰাই -errno;

                যদি (!S_ISREG(st.st_mode))
                        ঘূৰাই -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                যদি (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        যদি (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                যদি (max_size == SIZE_MAX)
                                        ঘূৰাই -EFBIG;

                                size = max_size;
                        } অন্যথা {
                                size = MIN((size_t) st.st_size, max_size);

                                যদি (size > READ_VIRTUAL_BYTES_MAX)
                                        ঘূৰাই -EFBIG;
                        }

                        n_retries--;
                } অন্যথা যদি (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } অন্যথা {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                যদি (!buf)
                        ঘূৰাই -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                বাবে (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = পঠ(fd, buf, size + 1);
                        যদি (k >= 0) {
                                n = k;
                                ভঙ্গ;
                        }

                        যদি (errno != EINTR)
                                ঘূৰাই -errno;
                }

                /* Consider a short read as EOF */
                যদি (n <= size)
                        ভঙ্গ;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                যদি (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        ভঙ্গ;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                যদি (n_retries <= 0)
                        ঘূৰাই -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                যদি (lseek(fd, 0, SEEK_SET) < 0)
                        ঘূৰাই -errno;

                buf = mfree(buf);
        }

        যদি (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                যদি (!ret_size && memchr(buf, 0, n))
                        ঘূৰাই -EBADMSG;

                যদি (n < size) {
                        ৱর্ণ *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        যদি (!p)
                                ঘূৰাই -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        যদি (ret_size)
                *ret_size = n;

        ঘূৰাই !truncated;
}

পূর্ণাঙ্ক read_full_stream_full(
                FILE *f,
                ধ্রুৱ ৱর্ণ *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ৱর্ণ **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ ৱর্ণ *buf = রিক্ত;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        পূর্ণাঙ্ক fd, r;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(ret_contents);
        প্রতিজ্ঞা(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        প্রতিজ্ঞা(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        যদি (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ঘূৰাই -ERANGE;

        যদি ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                যদি (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                অন্যথা
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        যদি (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                সংরচনা stat st;

                যদি (fstat(fd, &st) < 0)
                        ঘূৰাই -errno;

                যদি (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        যদি (r < 0)
                                ঘূৰাই r;
                }

                যদি (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        যদি (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                যদি (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        যদি (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (শূন্য) warn_file_is_world_accessible(filename, &st, রিক্ত, 0);
                }
        } অন্যথা যদি (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ঘূৰাই -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        যদি (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        যদি (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        যদি (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ঘূৰাই -errno;

        n = l = 0;
        বাবে (;;) {
                ৱর্ণ *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                যদি (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                যদি (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        যদি (!t) {
                                r = -ENOMEM;
                                গচ্ছ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        মুক্ত(buf);
                } অন্যথা {
                        t = realloc(buf, n_next + 1);
                        যদি (!t)
                                ঘূৰাই -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                প্রতিজ্ঞা(k <= n - l);
                l += k;

                যদি (ferror(f)) {
                        r = errno_or_else(EIO);
                        গচ্ছ finalize;
                }
                যদি (feof(f))
                        ভঙ্গ;

                যদি (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        প্রতিজ্ঞা(l == size);
                        ভঙ্গ;
                }

                প্রতিজ্ঞা(k > 0); /* we can't have read zero bytes because that would have been EOF */

                যদি (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        গচ্ছ finalize;
                }

                যদি (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        গচ্ছ finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        যদি (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ শূন্য *decoded = রিক্ত;
                size_t decoded_size;

                buf[l++] = 0;
                যদি (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                অন্যথা
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                যদি (r < 0)
                        গচ্ছ finalize;

                যদি (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                যদি (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        গচ্ছ finalize;
                }
        }

        যদি (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                যদি (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        গচ্ছ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        যদি (ret_size)
                *ret_size = l;

        ঘূৰাই 0;

finalize:
        যদি (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ঘূৰাই r;
}

পূর্ণাঙ্ক read_full_file_full(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ধ্রুৱ ৱর্ণ *bind_name,
                ৱর্ণ **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = রিক্ত;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_contents);

        যদি (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

পূর্ণাঙ্ক script_get_shebang_interpreter(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        f = fopen(path, "re");
        যদি (!f)
                ঘূৰাই -errno;

        ৱর্ণ c;
        r = safe_fgetc(f, &c);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -EBADMSG;
        যদি (c != '#')
                ঘূৰাই -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -EBADMSG;
        যদি (c != '!')
                ঘূৰাই -EMEDIUMTYPE;

        _cleanup_free_ ৱর্ণ *সাৰি = রিক্ত;
        r = read_সাৰি(f, LONG_সাৰি_MAX, &সাৰি);
        যদি (r < 0)
                ঘূৰাই r;

        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        ধ্রুৱ ৱর্ণ *s = সাৰি;

        r = extract_first_word(&s, &p, /* separators= */ রিক্ত, /* flags= */ 0);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -ENOEXEC;

        যদি (ret)
                *ret = TAKE_PTR(p);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক get_proc_field(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ **ret) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(চাবি);

        r = fopen_unlocked(path, "re", &f);
        যদি (r == -ENOENT && proc_mounted() == 0)
                ঘূৰাই -ENOSYS;
        যদি (r < 0)
                ঘূৰাই r;

        বাবে (;;) {
                 _cleanup_free_ ৱর্ণ *সাৰি = রিক্ত;

                 r = read_সাৰি(f, LONG_সাৰি_MAX, &সাৰি);
                 যদি (r < 0)
                         ঘূৰাই r;
                 যদি (r == 0)
                         ঘূৰাই -ENODATA;

                 ৱর্ণ *l = startswith(সাৰি, চাবি);
                 যদি (l && *l == ':') {
                         যদি (ret) {
                                 ৱর্ণ *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 যদি (!s)
                                         ঘূৰাই -ENOMEM;

                                 *ret = s;
                         }

                         ঘূৰাই 0;
                 }
        }
}

DIR* xopendirat(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(!(flags & (O_CREAT|O_TMPFILE)));

        যদি ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ঘূৰাই opendir(path);

        যদি (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        যদি (fd < 0)
                ঘূৰাই রিক্ত;

        ঘূৰাই take_fdopendir(&fd);
}

পূর্ণাঙ্ক fopen_mode_to_flags(ধ্রুৱ ৱর্ণ *mode) {
        ধ্রুৱ ৱর্ণ *p;
        পূর্ণাঙ্ক flags;

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
                ঘূৰাই -EINVAL;

        বাবে (; *p != 0; p++) {

                ৱিকল্প (*p) {

                পক্ষ 'e':
                        flags |= O_CLOEXEC;
                        ভঙ্গ;

                পক্ষ 'x':
                        flags |= O_EXCL;
                        ভঙ্গ;

                পক্ষ 'm':
                        /* ignore this here, fdopen() might care later though */
                        ভঙ্গ;

                পক্ষ 'c': /* not sure what to do about this one */
                স্ৱভাৱ:
                        ঘূৰাই -EINVAL;
                }
        }

        ঘূৰাই flags;
}

স্থির পূর্ণাঙ্ক xfopenat_regular(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *mode, পূর্ণাঙ্ক open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(mode);
        প্রতিজ্ঞা(ret);

        যদি (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        অন্যথা যদি (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ ৱর্ণ *j = strjoin("/", path);
                যদি (!j)
                        ঘূৰাই -ENOMEM;

                f = fopen(j, mode);
        } অন্যথা {
                _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
                পূর্ণাঙ্ক mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                যদি (mode_flags < 0)
                        ঘূৰাই mode_flags;

                যদি (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        যদি (fd < 0)
                                ঘূৰাই -errno;
                } অন্যথা {
                        যদি (dir_fd == AT_FDCWD)
                                ঘূৰাই -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        যদি (fd < 0)
                                ঘূৰাই fd;
                }

                f = take_fdopen(&fd, mode);
        }
        যদি (!f)
                ঘূৰাই -errno;

        *ret = f;
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক xfopenat_unix_socket(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *bind_name, FILE **ret) {
        _cleanup_close_ পূর্ণাঙ্ক sk = -EBADF;
        FILE *f;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        যদি (sk < 0)
                ঘূৰাই -errno;

        যদি (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                সঙ্ঘ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (bind(sk, &bsa.sa, r) < 0)
                        ঘূৰাই -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (shutdown(sk, SHUT_WR) < 0)
                ঘূৰাই -errno;

        f = take_fdopen(&sk, "r");
        যদি (!f)
                ঘূৰাই -errno;

        *ret = f;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক xfopenat_full(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ ৱর্ণ *mode,
                পূর্ণাঙ্ক open_flags,
                XfopenFlags flags,
                ধ্রুৱ ৱর্ণ *bind_name,
                FILE **ret) {

        FILE *f = রিক্ত;  /* avoid false maybe-uninitialized warning */
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(mode);
        প্রতিজ্ঞা(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        যদি (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                যদি (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ঘূৰাই -ENXIO; /* propagate original error if this is not a socket after all */
        }
        যদি (r < 0)
                ঘূৰাই r;

        যদি (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (শূন্য) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক fdopen_independent(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *mode, FILE **ret) {
        _cleanup_close_ পূর্ণাঙ্ক copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক mode_flags;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(mode);
        প্রতিজ্ঞা(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        যদি (mode_flags < 0)
                ঘূৰাই mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        যদি (copy_fd < 0)
                ঘূৰাই copy_fd;

        f = take_fdopen(&copy_fd, mode);
        যদি (!f)
                ঘূৰাই -errno;

        *ret = TAKE_PTR(f);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক অনুসন্ধান_and_open_internal(
                ধ্রুৱ ৱর্ণ *path,
                পূর্ণাঙ্ক mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ধ্রুৱ ৱর্ণ *মূল,
                ৱর্ণ **অনুসন্ধান,
                পূর্ণাঙ্ক *ret_fd,
                ৱর্ণ **ret_path) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        প্রতিজ্ঞা(path);

        যদি (path_is_absolute(path)) {
                _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

                যদি (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                অন্যথা
                        r = RET_NERRNO(access(path, mode));
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        যদি (r < 0)
                                ঘূৰাই r;
                }

                যদি (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ঘূৰাই 0;
        }

        যদি (!path_strv_resolve_uniq(অনুসন্ধান, মূল))
                ঘূৰাই -ENOMEM;

        STRV_FOREACH(i, অনুসন্ধান) {
                _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
                _cleanup_free_ ৱর্ণ *p = রিক্ত;

                p = path_join(মূল, *i, path);
                যদি (!p)
                        ঘূৰাই -ENOMEM;

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

                        ঘূৰাই 0;
                }
                যদি (r != -ENOENT)
                        ঘূৰাই r;
        }

        ঘূৰাই -ENOENT;
}

পূর্ণাঙ্ক অনুসন্ধান_and_open(
                ধ্রুৱ ৱর্ণ *path,
                পূর্ণাঙ্ক mode,
                ধ্রুৱ ৱর্ণ *মূল,
                ৱর্ণ **অনুসন্ধান,
                পূর্ণাঙ্ক *ret_fd,
                ৱর্ণ **ret_path) {

        _cleanup_strv_free_ ৱর্ণ **copy = রিক্ত;

        প্রতিজ্ঞা(path);

        copy = strv_copy(অনুসন্ধান);
        যদি (!copy)
                ঘূৰাই -ENOMEM;

        ঘূৰাই অনুসন্ধান_and_open_internal(path, mode, মূল, copy, ret_fd, ret_path);
}

স্থির পূর্ণাঙ্ক অনুসন্ধান_and_fopen_internal(
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ ৱর্ণ *mode,
                ধ্রুৱ ৱর্ণ *মূল,
                ৱর্ণ **অনুসন্ধান,
                FILE **ret_file,
                ৱর্ণ **ret_path) {

        _cleanup_free_ ৱর্ণ *found_path = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode || !ret_file);

        r = অনুসন্ধান_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        মূল,
                        অনুসন্ধান,
                        ret_file ? &fd : রিক্ত,
                        ret_path ? &found_path : রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                যদি (!f)
                        ঘূৰাই -errno;

                *ret_file = f;
        }

        যদি (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক অনুসন্ধান_and_fopen(
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ ৱর্ণ *mode,
                ধ্রুৱ ৱর্ণ *মূল,
                ধ্রুৱ ৱর্ণ **অনুসন্ধান,
                FILE **ret_file,
                ৱর্ণ **ret_path) {

        _cleanup_strv_free_ ৱর্ণ **copy = রিক্ত;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode || !ret_file);

        copy = strv_copy((ৱর্ণ**) অনুসন্ধান);
        যদি (!copy)
                ঘূৰাই -ENOMEM;

        ঘূৰাই অনুসন্ধান_and_fopen_internal(path, mode, মূল, copy, ret_file, ret_path);
}

পূর্ণাঙ্ক অনুসন্ধান_and_fopen_nulstr(
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ ৱর্ণ *mode,
                ধ্রুৱ ৱর্ণ *মূল,
                ধ্রুৱ ৱর্ণ *অনুসন্ধান,
                FILE **ret_file,
                ৱর্ণ **ret_path) {

        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode || !ret_file);

        l = strv_split_nulstr(অনুসন্ধান);
        যদি (!l)
                ঘূৰাই -ENOMEM;

        ঘূৰাই অনুসন্ধান_and_fopen_internal(path, mode, মূল, l, ret_file, ret_path);
}

পূর্ণাঙ্ক fflush_and_check(FILE *f) {
        প্রতিজ্ঞা(f);

        errno = 0;
        fflush(f);

        যদি (ferror(f))
                ঘূৰাই errno_or_else(EIO);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক fflush_sync_and_check(FILE *f) {
        পূর্ণাঙ্ক r, fd;

        প্রতিজ্ঞা(f);

        r = fflush_and_check(f);
        যদি (r < 0)
                ঘূৰাই r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        যদি (fd < 0)
                ঘূৰাই 0;

        r = fsync_full(fd);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক write_timestamp_file_atomic(ধ্রুৱ ৱর্ণ *fn, usec_t n) {
        ৱর্ণ ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        যদি (!timestamp_is_set(n))
                ঘূৰাই -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ঘূৰাই write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

পূর্ণাঙ্ক read_timestamp_file(ধ্রুৱ ৱর্ণ *fn, usec_t *ret) {
        _cleanup_free_ ৱর্ণ *ln = রিক্ত;
        uint64_t t;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = read_one_সাৰি_file(fn, &ln);
        যদি (r < 0)
                ঘূৰাই r;

        r = safe_atou64(ln, &t);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (!timestamp_is_set(t))
                ঘূৰাই -ERANGE;

        *ret = (usec_t) t;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক fputs_with_separator(FILE *f, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separator, bool *space) {
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
                        ঘূৰাই -EIO;

        *space = true;

        যদি (fputs(s, f) < 0)
                ঘূৰাই -EIO;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক fputs_with_newline(FILE *f, ধ্রুৱ ৱর্ণ *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        যদি (isempty(s))
                ঘূৰাই 0;

        যদি (!f)
                f = stdout;

        যদি (fputs(s, f) < 0)
                ঘূৰাই -EIO;

        যদি (endswith(s, "\n"))
                ঘূৰাই 0;

        যদি (fputc('\n', f) < 0)
                ঘূৰাই -EIO;

        ঘূৰাই 1;
}

/* A bitmask of the EOL markers we know */
প্রকারনির্দেশ গণনা End_Of_সাৰি_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_সাৰি_Marker;

স্থির End_Of_সাৰি_Marker categorize_eol(ৱর্ণ c, Read_সাৰি_Flags flags) {

        যদি (!FLAGS_SET(flags, READ_সাৰি_ONLY_NUL)) {
                যদি (c == '\n')
                        ঘূৰাই EOL_TEN;
                যদি (c == '\r')
                        ঘূৰাই EOL_THIRTEEN;
        }

        যদি (c == '\0')
                ঘূৰাই EOL_ZERO;

        ঘূৰাই EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, রিক্ত);

পূর্ণাঙ্ক read_সাৰি_full(FILE *f, size_t limit, Read_সাৰি_Flags flags, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *buffer = রিক্ত;
        size_t n = 0, গুনতি = 0;
        পূর্ণাঙ্ক r;

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
                যদি (!GREEDY_REALLOC(buffer, 1))
                        ঘূৰাই -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_সাৰি_Marker previous_eol = EOL_NONE;
                flockfile(f);

                বাবে (;;) {
                        End_Of_সাৰি_Marker eol;
                        ৱর্ণ c;

                        যদি (n >= limit)
                                ঘূৰাই -ENOBUFS;

                        যদি (গুনতি >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ঘূৰাই -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r == 0) /* EOF is definitely EOL */
                                ভঙ্গ;

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
                                assert_se(ungetc((unsigned ৱর্ণ) c, f) != EOF);
                                ভঙ্গ;
                        }

                        গুনতি++;

                        যদি (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                যদি ((flags & (READ_সাৰি_IS_A_TTY|READ_সাৰি_NOT_A_TTY)) == 0) {
                                        পূর্ণাঙ্ক fd;

                                        fd = fileno(f);
                                        যদি (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_সাৰি_NOT_A_TTY;
                                        অন্যথা
                                                flags |= isatty_safe(fd) ? READ_সাৰি_IS_A_TTY : READ_সাৰি_NOT_A_TTY;
                                }
                                যদি (FLAGS_SET(flags, READ_সাৰি_IS_A_TTY))
                                        ভঙ্গ;
                        }

                        যদি (eol != EOL_NONE) {
                                previous_eol |= eol;
                                অনুৱর্ত;
                        }

                        যদি (ret) {
                                যদি (!GREEDY_REALLOC(buffer, n + 2))
                                        ঘূৰাই -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        যদি (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        ঘূৰাই (পূর্ণাঙ্ক) গুনতি;
}

পূর্ণাঙ্ক read_stripped_সাৰি(FILE *f, size_t limit, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r, k;

        প্রতিজ্ঞা(f);

        r = read_সাৰি(f, limit, ret ? &s : রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ret) {
                ধ্রুৱ ৱর্ণ *p = strstrip(s);
                যদি (p == s)
                        *ret = TAKE_PTR(s);
                অন্যথা {
                        k = strdup_to(ret, p);
                        যদি (k < 0)
                                ঘূৰাই k;
                }
        }

        ঘূৰাই r > 0;          /* Return 1 if something was read. */
}

পূর্ণাঙ্ক safe_fgetc(FILE *f, ৱর্ণ *ret) {
        পূর্ণাঙ্ক k;

        প্রতিজ্ঞা(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        যদি (k == EOF) {
                যদি (ferror(f))
                        ঘূৰাই errno_or_else(EIO);

                যদি (ret)
                        *ret = 0;

                ঘূৰাই 0;
        }

        যদি (ret)
                *ret = k;

        ঘূৰাই 1;
}

পূর্ণাঙ্ক warn_file_is_world_accessible(ধ্রুৱ ৱর্ণ *filename, সংরচনা stat *st, ধ্রুৱ ৱর্ণ *unit, unsigned সাৰি) {
        সংরচনা stat _st;

        যদি (!filename)
                ঘূৰাই 0;

        যদি (!st) {
                যদি (stat(filename, &_st) < 0)
                        ঘূৰাই -errno;
                st = &_st;
        }

        যদি ((st->st_mode & S_IRWXO) == 0)
                ঘূৰাই 0;

        যদি (unit)
                log_syntax(unit, LOG_WARNING, filename, সাৰি, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        অন্যথা
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক write_data_file_atomic_at(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *path,
                ধ্রুৱ সংরচনা iovec *iovec,
                WriteDataFileFlags flags) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ ৱর্ণ *dn = রিক্ত, *fn = রিক্ত;
        r = path_split_prefix_filename(path, &dn, &fn);
        যদি (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ঘূৰাই -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        যদি (r < 0)
                ঘূৰাই r;

        _cleanup_close_ পূর্ণাঙ্ক mfd = -EBADF;
        যদি (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ রিক্ত,
                            &mfd);
                যদি (r < 0)
                        ঘূৰাই r;

                dir_fd = mfd;
        }

        _cleanup_free_ ৱর্ণ *t = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        যদি (fd < 0)
                ঘূৰাই fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        যদি (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        যদি (r < 0)
                ঘূৰাই r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        যদি (r < 0)
                ঘূৰাই r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ঘূৰাই 0;
}
