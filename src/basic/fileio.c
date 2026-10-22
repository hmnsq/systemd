/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio_ext.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "label-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mkdir.h"
#அந்தர்பாவ "nulstr-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "socket-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "sync-util.h"
#அந்தர்பாவ "terminal-util.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "tmpfile-util.h"

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

முழு fdopen_unlocked(முழு fd, மாறிலி எழுத்து *options, FILE **ret) {
        ப்ரதிஜ்ஞா(ret);

        FILE *f = fdopen(fd, options);
        எனில் (!f)
                திரும்பு -errno;

        (வெறுமை) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        திரும்பு 0;
}

முழு take_fdopen_unlocked(முழு *fd, மாறிலி எழுத்து *options, FILE **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(fd);

        r = fdopen_unlocked(*fd, options, ret);
        எனில் (r < 0)
                திரும்பு r;

        *fd = -EBADF;

        திரும்பு 0;
}

FILE* take_fdopen(முழு *fd, மாறிலி எழுத்து *options) {
        ப்ரதிஜ்ஞா(fd);

        FILE *f = fdopen(*fd, options);
        எனில் (!f)
                திரும்பு வெற்று;

        *fd = -EBADF;

        திரும்பு f;
}

DIR* take_fdopendir(முழு *dfd) {
        ப்ரதிஜ்ஞா(dfd);

        DIR *d = fdopendir(*dfd);
        எனில் (!d)
                திரும்பு வெற்று;

        *dfd = -EBADF;

        திரும்பு d;
}

FILE* open_memstream_unlocked(எழுத்து **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        எனில் (!f)
                திரும்பு வெற்று;

        (வெறுமை) __fsetlocking(f, FSETLOCKING_BYCALLER);

        திரும்பு f;
}

FILE* fmemopen_unlocked(வெறுமை *buf, size_t size, மாறிலி எழுத்து *mode) {
        FILE *f = fmemopen(buf, size, mode);
        எனில் (!f)
                திரும்பு வெற்று;

        (வெறுமை) __fsetlocking(f, FSETLOCKING_BYCALLER);

        திரும்பு f;
}

முழு write_string_stream_full(
                FILE *f,
                மாறிலி எழுத்து *வரி,
                WriteStringFileFlags flags,
                மாறிலி கட்டமைப்பு timespec *ts) {

        bool needs_nl;
        முழு r, fd = -EBADF;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(வரி);

        எனில் (ferror(f))
                திரும்பு -EIO;

        எனில் (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                எனில் (fd < 0)
                        திரும்பு -EBADF;
        }

        எனில் (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ எழுத்து *t = வெற்று;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                எனில் (fd < 0) {
                        fd = fileno(f);
                        எனில் (fd < 0)
                                திரும்பு -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                எனில் (read_virtual_file_fd(fd, strlen(வரி)+1, &t, வெற்று) > 0 &&
                    streq_skip_trailing_chars(வரி, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", வரி);
                        திரும்பு 0;
                }

                எனில் (lseek(fd, 0, SEEK_SET) < 0)
                        திரும்பு -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(வரி, "\n");

        எனில் (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                வரி = strjoina(வரி, "\n");
                needs_nl = false;
        }

        எனில் (fputs(வரி, f) == EOF)
                திரும்பு -errno;

        எனில் (needs_nl)
                எனில் (fputc('\n', f) == EOF)
                        திரும்பு -errno;

        எனில் (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        இல்லையெனில்
                r = fflush_and_check(f);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ts) {
                மாறிலி கட்டமைப்பு timespec twice[2] = {*ts, *ts};

                ப்ரதிஜ்ஞா(fd >= 0);
                எனில் (futimens(fd, twice) < 0)
                        திரும்பு -errno;
        }

        திரும்பு 0;
}

நிலையான mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        திரும்பு FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

நிலையான முழு write_string_file_atomic_at(
                முழு dir_fd,
                மாறிலி எழுத்து *fn,
                மாறிலி எழுத்து *வரி,
                WriteStringFileFlags flags,
                மாறிலி கட்டமைப்பு timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(fn);
        ப்ரதிஜ்ஞா(வரி);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        எனில் (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                எனில் (r < 0)
                        திரும்பு r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        முழு k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? வெற்று : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        எனில் (r < 0)
                திரும்பு r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        எனில் (k < 0)
                திரும்பு k;

        r = write_string_stream_full(f, வரி, flags, ts);
        எனில் (r < 0)
                திரும்பு r;

        r = fchmod_umask(fileno(f), mode);
        எனில் (r < 0)
                திரும்பு r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        எனில் (r < 0)
                திரும்பு r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        எனில் (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு write_string_file_full_label(
                முழு dir_fd,
                மாறிலி எழுத்து *fn,
                மாறிலி எழுத்து *வரி,
                WriteStringFileFlags flags,
                மாறிலி கட்டமைப்பு timespec *ts,
                மாறிலி எழுத்து *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd == AT_FDCWD || dir_fd >= 0);
        ப்ரதிஜ்ஞா(வரி);

        /* We don't know how to verify whether the file contents was already on-disk. */
        ப்ரதிஜ்ஞா(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        எனில் (flags & WRITE_STRING_FILE_MKDIR_0755) {
                ப்ரதிஜ்ஞா(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (flags & WRITE_STRING_FILE_ATOMIC) {
                ப்ரதிஜ்ஞா(fn);
                ப்ரதிஜ்ஞா(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, வரி, flags, ts, label_context);
                எனில் (r < 0)
                        செல் fail;

                திரும்பு r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        எனில் (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        இல்லையெனில் {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                எனில் (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        எனில் (r < 0)
                                செல் fail;

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
                எனில் (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? வெற்று : fn, made_file, label_context));
        }
        எனில் (r < 0)
                செல் fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        எனில் (r < 0)
                செல் fail;

        எனில் (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, வெற்று, _IONBF, 0);

        r = write_string_stream_full(f, வரி, flags, ts);
        எனில் (r < 0)
                செல் fail;

        திரும்பு 0;

fail:
        எனில் (made_file)
                (வெறுமை) unlinkat(dir_fd, fn, 0);

        எனில் (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                திரும்பு r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        எனில் (verify_file_at(dir_fd, fn, வரி, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                திரும்பு 0;

        திரும்பு r;
}

முழு write_string_filef(
                மாறிலி எழுத்து *fn,
                WriteStringFileFlags flags,
                மாறிலி எழுத்து *format, ...) {

        _cleanup_free_ எழுத்து *p = வெற்று;
        va_list ap;
        முழு r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு -ENOMEM;

        திரும்பு write_string_file(fn, p, flags);
}

முழு write_string_filef_at(
                முழு dir_fd,
                மாறிலி எழுத்து *fn,
                WriteStringFileFlags flags,
                மாறிலி எழுத்து *format, ...) {

        _cleanup_free_ எழுத்து *p = வெற்று;
        va_list ap;
        முழு r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு -ENOMEM;

        திரும்பு write_string_file_at(dir_fd, fn, p, flags);
}

முழு write_base64_file_at(
                முழு dir_fd,
                மாறிலி எழுத்து *fn,
                மாறிலி கட்டமைப்பு iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ எழுத்து *encoded = வெற்று;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : வெற்று, data ? data->iov_len : 0, 79, &encoded);
        எனில் (n < 0)
                திரும்பு n;

        திரும்பு write_string_file_at(dir_fd, fn, encoded, flags);
}

முழு read_one_வரி_file_at(முழு dir_fd, மாறிலி எழுத்து *filename, எழுத்து **ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(filename);
        ப்ரதிஜ்ஞா(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு read_வரி(f, LONG_வரி_MAX, ret);
}

முழு read_boolean_file_at(முழு dir_fd, மாறிலி எழுத்து *filename) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(filename);

        r = read_one_வரி_file_at(dir_fd, filename, &s);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_boolean(s);
}

முழு verify_file_at(முழு dir_fd, மாறிலி எழுத்து *fn, மாறிலி எழுத்து *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_free_ எழுத்து *buf = வெற்று;
        size_t l, k;
        முழு r;

        ப்ரதிஜ்ஞா(blob);

        l = strlen(blob);

        எனில் (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        எனில் (r < 0)
                திரும்பு r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        எனில் (ferror(f))
                திரும்பு errno_or_else(EIO);

        எனில் (k != l && k != l + accept_extra_nl)
                திரும்பு 0;
        எனில் (memcmp(buf, blob, l) != 0)
                திரும்பு 0;
        எனில் (k > l && buf[l] != '\n')
                திரும்பு 0;

        திரும்பு 1;
}

முழு read_virtual_file_at(
                முழு dir_fd,
                மாறிலி எழுத்து *filename,
                size_t max_size,
                எழுத்து **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ எழுத்து *buf = வெற்று;
        size_t n, size;
        முழு n_retries;
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

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ முழு fd = -EBADF;
        எனில் (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        இல்லையெனில்
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        எனில் (fd < 0)
                திரும்பு fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        க்கு (;;) {
                கட்டமைப்பு stat st;

                எனில் (fstat(fd, &st) < 0)
                        திரும்பு -errno;

                எனில் (!S_ISREG(st.st_mode))
                        திரும்பு -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                எனில் (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        எனில் (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                எனில் (max_size == SIZE_MAX)
                                        திரும்பு -EFBIG;

                                size = max_size;
                        } இல்லையெனில் {
                                size = MIN((size_t) st.st_size, max_size);

                                எனில் (size > READ_VIRTUAL_BYTES_MAX)
                                        திரும்பு -EFBIG;
                        }

                        n_retries--;
                } இல்லையெனில் எனில் (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } இல்லையெனில் {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                க்கு (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = பட(fd, buf, size + 1);
                        எனில் (k >= 0) {
                                n = k;
                                நிறுத்து;
                        }

                        எனில் (errno != EINTR)
                                திரும்பு -errno;
                }

                /* Consider a short read as EOF */
                எனில் (n <= size)
                        நிறுத்து;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                எனில் (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        நிறுத்து;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                எனில் (n_retries <= 0)
                        திரும்பு -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                எனில் (lseek(fd, 0, SEEK_SET) < 0)
                        திரும்பு -errno;

                buf = mfree(buf);
        }

        எனில் (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                எனில் (!ret_size && memchr(buf, 0, n))
                        திரும்பு -EBADMSG;

                எனில் (n < size) {
                        எழுத்து *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        எனில் (!p)
                                திரும்பு -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        எனில் (ret_size)
                *ret_size = n;

        திரும்பு !truncated;
}

முழு read_full_stream_full(
                FILE *f,
                மாறிலி எழுத்து *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                எழுத்து **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ எழுத்து *buf = வெற்று;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        முழு fd, r;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(ret_contents);
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        ப்ரதிஜ்ஞா(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        எனில் (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                திரும்பு -ERANGE;

        எனில் ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                எனில் (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                இல்லையெனில்
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        எனில் (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                கட்டமைப்பு stat st;

                எனில் (fstat(fd, &st) < 0)
                        திரும்பு -errno;

                எனில் (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        எனில் (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                எனில் (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        எனில் (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (வெறுமை) warn_file_is_world_accessible(filename, &st, வெற்று, 0);
                }
        } இல்லையெனில் எனில் (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                திரும்பு -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        எனில் (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        எனில் (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        எனில் (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                திரும்பு -errno;

        n = l = 0;
        க்கு (;;) {
                எழுத்து *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                எனில் (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                எனில் (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        எனில் (!t) {
                                r = -ENOMEM;
                                செல் finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        விடுவி(buf);
                } இல்லையெனில் {
                        t = realloc(buf, n_next + 1);
                        எனில் (!t)
                                திரும்பு -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                ப்ரதிஜ்ஞா(k <= n - l);
                l += k;

                எனில் (ferror(f)) {
                        r = errno_or_else(EIO);
                        செல் finalize;
                }
                எனில் (feof(f))
                        நிறுத்து;

                எனில் (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        ப்ரதிஜ்ஞா(l == size);
                        நிறுத்து;
                }

                ப்ரதிஜ்ஞா(k > 0); /* we can't have read zero bytes because that would have been EOF */

                எனில் (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        செல் finalize;
                }

                எனில் (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        செல் finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        எனில் (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ வெறுமை *decoded = வெற்று;
                size_t decoded_size;

                buf[l++] = 0;
                எனில் (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                இல்லையெனில்
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                எனில் (r < 0)
                        செல் finalize;

                எனில் (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                எனில் (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        செல் finalize;
                }
        }

        எனில் (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                எனில் (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        செல் finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        எனில் (ret_size)
                *ret_size = l;

        திரும்பு 0;

finalize:
        எனில் (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        திரும்பு r;
}

முழு read_full_file_full(
                முழு dir_fd,
                மாறிலி எழுத்து *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                மாறிலி எழுத்து *bind_name,
                எழுத்து **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = வெற்று;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        முழு r;

        ப்ரதிஜ்ஞா(ret_contents);

        எனில் (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

முழு script_get_shebang_interpreter(மாறிலி எழுத்து *path, எழுத்து **ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        f = fopen(path, "re");
        எனில் (!f)
                திரும்பு -errno;

        எழுத்து c;
        r = safe_fgetc(f, &c);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EBADMSG;
        எனில் (c != '#')
                திரும்பு -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EBADMSG;
        எனில் (c != '!')
                திரும்பு -EMEDIUMTYPE;

        _cleanup_free_ எழுத்து *வரி = வெற்று;
        r = read_வரி(f, LONG_வரி_MAX, &வரி);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_free_ எழுத்து *p = வெற்று;
        மாறிலி எழுத்து *s = வரி;

        r = extract_first_word(&s, &p, /* separators= */ வெற்று, /* flags= */ 0);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -ENOEXEC;

        எனில் (ret)
                *ret = TAKE_PTR(p);
        திரும்பு 0;
}

முழு get_proc_field(மாறிலி எழுத்து *path, மாறிலி எழுத்து *சாவி, எழுத்து **ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(சாவி);

        r = fopen_unlocked(path, "re", &f);
        எனில் (r == -ENOENT && proc_mounted() == 0)
                திரும்பு -ENOSYS;
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                 _cleanup_free_ எழுத்து *வரி = வெற்று;

                 r = read_வரி(f, LONG_வரி_MAX, &வரி);
                 எனில் (r < 0)
                         திரும்பு r;
                 எனில் (r == 0)
                         திரும்பு -ENODATA;

                 எழுத்து *l = startswith(வரி, சாவி);
                 எனில் (l && *l == ':') {
                         எனில் (ret) {
                                 எழுத்து *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 எனில் (!s)
                                         திரும்பு -ENOMEM;

                                 *ret = s;
                         }

                         திரும்பு 0;
                 }
        }
}

DIR* xopendirat(முழு dir_fd, மாறிலி எழுத்து *path, முழு flags) {
        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(!(flags & (O_CREAT|O_TMPFILE)));

        எனில் ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                திரும்பு opendir(path);

        எனில் (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        எனில் (fd < 0)
                திரும்பு வெற்று;

        திரும்பு take_fdopendir(&fd);
}

முழு fopen_mode_to_flags(மாறிலி எழுத்து *mode) {
        மாறிலி எழுத்து *p;
        முழு flags;

        ப்ரதிஜ்ஞா(mode);

        எனில் ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        இல்லையெனில் எனில் ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        இல்லையெனில் எனில் ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        இல்லையெனில் எனில் ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        இல்லையெனில் எனில் ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        இல்லையெனில் எனில் ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        இல்லையெனில்
                திரும்பு -EINVAL;

        க்கு (; *p != 0; p++) {

                தேர்வு (*p) {

                நிலை 'e':
                        flags |= O_CLOEXEC;
                        நிறுத்து;

                நிலை 'x':
                        flags |= O_EXCL;
                        நிறுத்து;

                நிலை 'm':
                        /* ignore this here, fdopen() might care later though */
                        நிறுத்து;

                நிலை 'c': /* not sure what to do about this one */
                இயல்பு:
                        திரும்பு -EINVAL;
                }
        }

        திரும்பு flags;
}

நிலையான முழு xfopenat_regular(முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *mode, முழு open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(mode);
        ப்ரதிஜ்ஞா(ret);

        எனில் (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        இல்லையெனில் எனில் (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ எழுத்து *j = strjoin("/", path);
                எனில் (!j)
                        திரும்பு -ENOMEM;

                f = fopen(j, mode);
        } இல்லையெனில் {
                _cleanup_close_ முழு fd = -EBADF;
                முழு mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                எனில் (mode_flags < 0)
                        திரும்பு mode_flags;

                எனில் (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        எனில் (fd < 0)
                                திரும்பு -errno;
                } இல்லையெனில் {
                        எனில் (dir_fd == AT_FDCWD)
                                திரும்பு -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        எனில் (fd < 0)
                                திரும்பு fd;
                }

                f = take_fdopen(&fd, mode);
        }
        எனில் (!f)
                திரும்பு -errno;

        *ret = f;
        திரும்பு 0;
}

நிலையான முழு xfopenat_unix_socket(முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *bind_name, FILE **ret) {
        _cleanup_close_ முழு sk = -EBADF;
        FILE *f;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        எனில் (sk < 0)
                திரும்பு -errno;

        எனில் (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                ஒன்றிப்பு sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (bind(sk, &bsa.sa, r) < 0)
                        திரும்பு -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (shutdown(sk, SHUT_WR) < 0)
                திரும்பு -errno;

        f = take_fdopen(&sk, "r");
        எனில் (!f)
                திரும்பு -errno;

        *ret = f;
        திரும்பு 0;
}

முழு xfopenat_full(
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *mode,
                முழு open_flags,
                XfopenFlags flags,
                மாறிலி எழுத்து *bind_name,
                FILE **ret) {

        FILE *f = வெற்று;  /* avoid false maybe-uninitialized warning */
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(mode);
        ப்ரதிஜ்ஞா(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        எனில் (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                எனில் (IN_SET(r, -ENOTSOCK, -EINVAL))
                        திரும்பு -ENXIO; /* propagate original error if this is not a socket after all */
        }
        எனில் (r < 0)
                திரும்பு r;

        எனில் (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (வெறுமை) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        திரும்பு 0;
}

முழு fdopen_independent(முழு fd, மாறிலி எழுத்து *mode, FILE **ret) {
        _cleanup_close_ முழு copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு mode_flags;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(mode);
        ப்ரதிஜ்ஞா(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        எனில் (mode_flags < 0)
                திரும்பு mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        எனில் (copy_fd < 0)
                திரும்பு copy_fd;

        f = take_fdopen(&copy_fd, mode);
        எனில் (!f)
                திரும்பு -errno;

        *ret = TAKE_PTR(f);
        திரும்பு 0;
}

நிலையான முழு தேடல்_and_open_internal(
                மாறிலி எழுத்து *path,
                முழு mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                மாறிலி எழுத்து *வேர்,
                எழுத்து **தேடல்,
                முழு *ret_fd,
                எழுத்து **ret_path) {

        முழு r;

        ப்ரதிஜ்ஞா(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        ப்ரதிஜ்ஞா(path);

        எனில் (path_is_absolute(path)) {
                _cleanup_close_ முழு fd = -EBADF;

                எனில் (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                இல்லையெனில்
                        r = RET_NERRNO(access(path, mode));
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                திரும்பு 0;
        }

        எனில் (!path_strv_resolve_uniq(தேடல், வேர்))
                திரும்பு -ENOMEM;

        STRV_FOREACH(i, தேடல்) {
                _cleanup_close_ முழு fd = -EBADF;
                _cleanup_free_ எழுத்து *p = வெற்று;

                p = path_join(வேர், *i, path);
                எனில் (!p)
                        திரும்பு -ENOMEM;

                எனில் (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                இல்லையெனில்
                        r = RET_NERRNO(access(p, F_OK));
                எனில் (r >= 0) {
                        எனில் (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        எனில் (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        திரும்பு 0;
                }
                எனில் (r != -ENOENT)
                        திரும்பு r;
        }

        திரும்பு -ENOENT;
}

முழு தேடல்_and_open(
                மாறிலி எழுத்து *path,
                முழு mode,
                மாறிலி எழுத்து *வேர்,
                எழுத்து **தேடல்,
                முழு *ret_fd,
                எழுத்து **ret_path) {

        _cleanup_strv_free_ எழுத்து **copy = வெற்று;

        ப்ரதிஜ்ஞா(path);

        copy = strv_copy(தேடல்);
        எனில் (!copy)
                திரும்பு -ENOMEM;

        திரும்பு தேடல்_and_open_internal(path, mode, வேர், copy, ret_fd, ret_path);
}

நிலையான முழு தேடல்_and_fopen_internal(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *mode,
                மாறிலி எழுத்து *வேர்,
                எழுத்து **தேடல்,
                FILE **ret_file,
                எழுத்து **ret_path) {

        _cleanup_free_ எழுத்து *found_path = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(mode || !ret_file);

        r = தேடல்_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        வேர்,
                        தேடல்,
                        ret_file ? &fd : வெற்று,
                        ret_path ? &found_path : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                எனில் (!f)
                        திரும்பு -errno;

                *ret_file = f;
        }

        எனில் (ret_path)
                *ret_path = TAKE_PTR(found_path);

        திரும்பு 0;
}

முழு தேடல்_and_fopen(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *mode,
                மாறிலி எழுத்து *வேர்,
                மாறிலி எழுத்து **தேடல்,
                FILE **ret_file,
                எழுத்து **ret_path) {

        _cleanup_strv_free_ எழுத்து **copy = வெற்று;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(mode || !ret_file);

        copy = strv_copy((எழுத்து**) தேடல்);
        எனில் (!copy)
                திரும்பு -ENOMEM;

        திரும்பு தேடல்_and_fopen_internal(path, mode, வேர், copy, ret_file, ret_path);
}

முழு தேடல்_and_fopen_nulstr(
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *mode,
                மாறிலி எழுத்து *வேர்,
                மாறிலி எழுத்து *தேடல்,
                FILE **ret_file,
                எழுத்து **ret_path) {

        _cleanup_strv_free_ எழுத்து **l = வெற்று;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(mode || !ret_file);

        l = strv_split_nulstr(தேடல்);
        எனில் (!l)
                திரும்பு -ENOMEM;

        திரும்பு தேடல்_and_fopen_internal(path, mode, வேர், l, ret_file, ret_path);
}

முழு fflush_and_check(FILE *f) {
        ப்ரதிஜ்ஞா(f);

        errno = 0;
        fflush(f);

        எனில் (ferror(f))
                திரும்பு errno_or_else(EIO);

        திரும்பு 0;
}

முழு fflush_sync_and_check(FILE *f) {
        முழு r, fd;

        ப்ரதிஜ்ஞா(f);

        r = fflush_and_check(f);
        எனில் (r < 0)
                திரும்பு r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        எனில் (fd < 0)
                திரும்பு 0;

        r = fsync_full(fd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 0;
}

முழு write_timestamp_file_atomic(மாறிலி எழுத்து *fn, usec_t n) {
        எழுத்து ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        எனில் (!timestamp_is_set(n))
                திரும்பு -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        திரும்பு write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

முழு read_timestamp_file(மாறிலி எழுத்து *fn, usec_t *ret) {
        _cleanup_free_ எழுத்து *ln = வெற்று;
        uint64_t t;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = read_one_வரி_file(fn, &ln);
        எனில் (r < 0)
                திரும்பு r;

        r = safe_atou64(ln, &t);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!timestamp_is_set(t))
                திரும்பு -ERANGE;

        *ret = (usec_t) t;
        திரும்பு 0;
}

முழு fputs_with_separator(FILE *f, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separator, bool *space) {
        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        எனில் (!f)
                f = stdout;

        எனில் (!separator)
                separator = " ";

        எனில் (*space)
                எனில் (fputs(separator, f) < 0)
                        திரும்பு -EIO;

        *space = true;

        எனில் (fputs(s, f) < 0)
                திரும்பு -EIO;

        திரும்பு 0;
}

முழு fputs_with_newline(FILE *f, மாறிலி எழுத்து *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        எனில் (isempty(s))
                திரும்பு 0;

        எனில் (!f)
                f = stdout;

        எனில் (fputs(s, f) < 0)
                திரும்பு -EIO;

        எனில் (endswith(s, "\n"))
                திரும்பு 0;

        எனில் (fputc('\n', f) < 0)
                திரும்பு -EIO;

        திரும்பு 1;
}

/* A bitmask of the EOL markers we know */
வகைவரை எண்ணிக்கை End_Of_வரி_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_வரி_Marker;

நிலையான End_Of_வரி_Marker categorize_eol(எழுத்து c, Read_வரி_Flags flags) {

        எனில் (!FLAGS_SET(flags, READ_வரி_ONLY_NUL)) {
                எனில் (c == '\n')
                        திரும்பு EOL_TEN;
                எனில் (c == '\r')
                        திரும்பு EOL_THIRTEEN;
        }

        எனில் (c == '\0')
                திரும்பு EOL_ZERO;

        திரும்பு EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, வெற்று);

முழு read_வரி_full(FILE *f, size_t limit, Read_வரி_Flags flags, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *buffer = வெற்று;
        size_t n = 0, மொத்தம் = 0;
        முழு r;

        ப்ரதிஜ்ஞா(f);

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

        எனில் (ret) {
                எனில் (!GREEDY_REALLOC(buffer, 1))
                        திரும்பு -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_வரி_Marker previous_eol = EOL_NONE;
                flockfile(f);

                க்கு (;;) {
                        End_Of_வரி_Marker eol;
                        எழுத்து c;

                        எனில் (n >= limit)
                                திரும்பு -ENOBUFS;

                        எனில் (மொத்தம் >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                திரும்பு -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0) /* EOF is definitely EOL */
                                நிறுத்து;

                        eol = categorize_eol(c, flags);

                        எனில் (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned எழுத்து) c, f) != EOF);
                                நிறுத்து;
                        }

                        மொத்தம்++;

                        எனில் (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                எனில் ((flags & (READ_வரி_IS_A_TTY|READ_வரி_NOT_A_TTY)) == 0) {
                                        முழு fd;

                                        fd = fileno(f);
                                        எனில் (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_வரி_NOT_A_TTY;
                                        இல்லையெனில்
                                                flags |= isatty_safe(fd) ? READ_வரி_IS_A_TTY : READ_வரி_NOT_A_TTY;
                                }
                                எனில் (FLAGS_SET(flags, READ_வரி_IS_A_TTY))
                                        நிறுத்து;
                        }

                        எனில் (eol != EOL_NONE) {
                                previous_eol |= eol;
                                தொடர்;
                        }

                        எனில் (ret) {
                                எனில் (!GREEDY_REALLOC(buffer, n + 2))
                                        திரும்பு -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        எனில் (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        திரும்பு (முழு) மொத்தம்;
}

முழு read_stripped_வரி(FILE *f, size_t limit, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r, k;

        ப்ரதிஜ்ஞா(f);

        r = read_வரி(f, limit, ret ? &s : வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret) {
                மாறிலி எழுத்து *p = strstrip(s);
                எனில் (p == s)
                        *ret = TAKE_PTR(s);
                இல்லையெனில் {
                        k = strdup_to(ret, p);
                        எனில் (k < 0)
                                திரும்பு k;
                }
        }

        திரும்பு r > 0;          /* Return 1 if something was read. */
}

முழு safe_fgetc(FILE *f, எழுத்து *ret) {
        முழு k;

        ப்ரதிஜ்ஞா(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        எனில் (k == EOF) {
                எனில் (ferror(f))
                        திரும்பு errno_or_else(EIO);

                எனில் (ret)
                        *ret = 0;

                திரும்பு 0;
        }

        எனில் (ret)
                *ret = k;

        திரும்பு 1;
}

முழு warn_file_is_world_accessible(மாறிலி எழுத்து *filename, கட்டமைப்பு stat *st, மாறிலி எழுத்து *unit, unsigned வரி) {
        கட்டமைப்பு stat _st;

        எனில் (!filename)
                திரும்பு 0;

        எனில் (!st) {
                எனில் (stat(filename, &_st) < 0)
                        திரும்பு -errno;
                st = &_st;
        }

        எனில் ((st->st_mode & S_IRWXO) == 0)
                திரும்பு 0;

        எனில் (unit)
                log_syntax(unit, LOG_WARNING, filename, வரி, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        இல்லையெனில்
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        திரும்பு 0;
}

முழு write_data_file_atomic_at(
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                மாறிலி கட்டமைப்பு iovec *iovec,
                WriteDataFileFlags flags) {

        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ எழுத்து *dn = வெற்று, *fn = வெற்று;
        r = path_split_prefix_filename(path, &dn, &fn);
        எனில் (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                திரும்பு -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_close_ முழு mfd = -EBADF;
        எனில் (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ வெற்று,
                            &mfd);
                எனில் (r < 0)
                        திரும்பு r;

                dir_fd = mfd;
        }

        _cleanup_free_ எழுத்து *t = வெற்று;
        _cleanup_close_ முழு fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        எனில் (fd < 0)
                திரும்பு fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        எனில் (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                எனில் (r < 0)
                        திரும்பு r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        எனில் (r < 0)
                திரும்பு r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        எனில் (r < 0)
                திரும்பு r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        திரும்பு 0;
}
