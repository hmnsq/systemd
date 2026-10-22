/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <stdio_ext.h>
#شامل <stdlib.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "hexdecoct.h"
#شامل "io-util.h"
#شامل "iovec-util.h"
#شامل "label-util.h"
#شامل "log.h"
#شامل "mkdir.h"
#شامل "nulstr-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "socket-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "sync-util.h"
#شامل "terminal-util.h"
#شامل "time-util.h"
#شامل "tmpfile-util.h"

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

صحیح fdopen_unlocked(صحیح fd, ثابت نویسه *options, FILE **ret) {
        أكد(ret);

        FILE *f = fdopen(fd, options);
        اگر (!f)
                بازگشت -errno;

        (تهی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        بازگشت 0;
}

صحیح take_fdopen_unlocked(صحیح *fd, ثابت نویسه *options, FILE **ret) {
        صحیح r;

        أكد(fd);

        r = fdopen_unlocked(*fd, options, ret);
        اگر (r < 0)
                بازگشت r;

        *fd = -EBADF;

        بازگشت 0;
}

FILE* take_fdopen(صحیح *fd, ثابت نویسه *options) {
        أكد(fd);

        FILE *f = fdopen(*fd, options);
        اگر (!f)
                بازگشت پوچ;

        *fd = -EBADF;

        بازگشت f;
}

DIR* take_fdopendir(صحیح *dfd) {
        أكد(dfd);

        DIR *d = fdopendir(*dfd);
        اگر (!d)
                بازگشت پوچ;

        *dfd = -EBADF;

        بازگشت d;
}

FILE* open_memstream_unlocked(نویسه **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        اگر (!f)
                بازگشت پوچ;

        (تهی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        بازگشت f;
}

FILE* fmemopen_unlocked(تهی *buf, size_t size, ثابت نویسه *mode) {
        FILE *f = fmemopen(buf, size, mode);
        اگر (!f)
                بازگشت پوچ;

        (تهی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        بازگشت f;
}

صحیح write_string_stream_full(
                FILE *f,
                ثابت نویسه *سطر,
                WriteStringFileFlags flags,
                ثابت ساختار timespec *ts) {

        bool needs_nl;
        صحیح r, fd = -EBADF;

        أكد(f);
        أكد(سطر);

        اگر (ferror(f))
                بازگشت -EIO;

        اگر (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                اگر (fd < 0)
                        بازگشت -EBADF;
        }

        اگر (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ نویسه *t = پوچ;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                اگر (fd < 0) {
                        fd = fileno(f);
                        اگر (fd < 0)
                                بازگشت -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                اگر (read_virtual_file_fd(fd, strlen(سطر)+1, &t, پوچ) > 0 &&
                    streq_skip_trailing_chars(سطر, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", سطر);
                        بازگشت 0;
                }

                اگر (lseek(fd, 0, SEEK_SET) < 0)
                        بازگشت -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(سطر, "\n");

        اگر (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                سطر = strjoina(سطر, "\n");
                needs_nl = false;
        }

        اگر (fputs(سطر, f) == EOF)
                بازگشت -errno;

        اگر (needs_nl)
                اگر (fputc('\n', f) == EOF)
                        بازگشت -errno;

        اگر (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        وگرنه
                r = fflush_and_check(f);
        اگر (r < 0)
                بازگشت r;

        اگر (ts) {
                ثابت ساختار timespec twice[2] = {*ts, *ts};

                أكد(fd >= 0);
                اگر (futimens(fd, twice) < 0)
                        بازگشت -errno;
        }

        بازگشت 0;
}

ایستا mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        بازگشت FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

ایستا صحیح write_string_file_atomic_at(
                صحیح dir_fd,
                ثابت نویسه *fn,
                ثابت نویسه *سطر,
                WriteStringFileFlags flags,
                ثابت ساختار timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(fn);
        أكد(سطر);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        اگر (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                اگر (r < 0)
                        بازگشت r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        صحیح k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? پوچ : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        اگر (r < 0)
                بازگشت r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        اگر (k < 0)
                بازگشت k;

        r = write_string_stream_full(f, سطر, flags, ts);
        اگر (r < 0)
                بازگشت r;

        r = fchmod_umask(fileno(f), mode);
        اگر (r < 0)
                بازگشت r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        اگر (r < 0)
                بازگشت r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        اگر (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح write_string_file_full_label(
                صحیح dir_fd,
                ثابت نویسه *fn,
                ثابت نویسه *سطر,
                WriteStringFileFlags flags,
                ثابت ساختار timespec *ts,
                ثابت نویسه *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(dir_fd == AT_FDCWD || dir_fd >= 0);
        أكد(سطر);

        /* We don't know how to verify whether the file contents was already on-disk. */
        أكد(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        اگر (flags & WRITE_STRING_FILE_MKDIR_0755) {
                أكد(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (flags & WRITE_STRING_FILE_ATOMIC) {
                أكد(fn);
                أكد(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, سطر, flags, ts, label_context);
                اگر (r < 0)
                        برو fail;

                بازگشت r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        اگر (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        وگرنه {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                اگر (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        اگر (r < 0)
                                برو fail;

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
                اگر (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? پوچ : fn, made_file, label_context));
        }
        اگر (r < 0)
                برو fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        اگر (r < 0)
                برو fail;

        اگر (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, پوچ, _IONBF, 0);

        r = write_string_stream_full(f, سطر, flags, ts);
        اگر (r < 0)
                برو fail;

        بازگشت 0;

fail:
        اگر (made_file)
                (تهی) unlinkat(dir_fd, fn, 0);

        اگر (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                بازگشت r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        اگر (verify_file_at(dir_fd, fn, سطر, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                بازگشت 0;

        بازگشت r;
}

صحیح write_string_filef(
                ثابت نویسه *fn,
                WriteStringFileFlags flags,
                ثابت نویسه *format, ...) {

        _cleanup_free_ نویسه *p = پوچ;
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت -ENOMEM;

        بازگشت write_string_file(fn, p, flags);
}

صحیح write_string_filef_at(
                صحیح dir_fd,
                ثابت نویسه *fn,
                WriteStringFileFlags flags,
                ثابت نویسه *format, ...) {

        _cleanup_free_ نویسه *p = پوچ;
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت -ENOMEM;

        بازگشت write_string_file_at(dir_fd, fn, p, flags);
}

صحیح write_base64_file_at(
                صحیح dir_fd,
                ثابت نویسه *fn,
                ثابت ساختار iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ نویسه *encoded = پوچ;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : پوچ, data ? data->iov_len : 0, 79, &encoded);
        اگر (n < 0)
                بازگشت n;

        بازگشت write_string_file_at(dir_fd, fn, encoded, flags);
}

صحیح read_one_سطر_file_at(صحیح dir_fd, ثابت نویسه *filename, نویسه **ret) {
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);
        أكد(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        اگر (r < 0)
                بازگشت r;

        بازگشت read_سطر(f, LONG_سطر_MAX, ret);
}

صحیح read_boolean_file_at(صحیح dir_fd, ثابت نویسه *filename) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);

        r = read_one_سطر_file_at(dir_fd, filename, &s);
        اگر (r < 0)
                بازگشت r;

        بازگشت parse_boolean(s);
}

صحیح verify_file_at(صحیح dir_fd, ثابت نویسه *fn, ثابت نویسه *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_free_ نویسه *buf = پوچ;
        size_t l, k;
        صحیح r;

        أكد(blob);

        l = strlen(blob);

        اگر (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        اگر (!buf)
                بازگشت -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        اگر (r < 0)
                بازگشت r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        اگر (ferror(f))
                بازگشت errno_or_else(EIO);

        اگر (k != l && k != l + accept_extra_nl)
                بازگشت 0;
        اگر (memcmp(buf, blob, l) != 0)
                بازگشت 0;
        اگر (k > l && buf[l] != '\n')
                بازگشت 0;

        بازگشت 1;
}

صحیح read_virtual_file_at(
                صحیح dir_fd,
                ثابت نویسه *filename,
                size_t max_size,
                نویسه **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ نویسه *buf = پوچ;
        size_t n, size;
        صحیح n_retries;
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

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ صحیح fd = -EBADF;
        اگر (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        وگرنه
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        اگر (fd < 0)
                بازگشت fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        برای (;;) {
                ساختار stat st;

                اگر (fstat(fd, &st) < 0)
                        بازگشت -errno;

                اگر (!S_ISREG(st.st_mode))
                        بازگشت -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                اگر (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        اگر (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                اگر (max_size == SIZE_MAX)
                                        بازگشت -EFBIG;

                                size = max_size;
                        } وگرنه {
                                size = MIN((size_t) st.st_size, max_size);

                                اگر (size > READ_VIRTUAL_BYTES_MAX)
                                        بازگشت -EFBIG;
                        }

                        n_retries--;
                } وگرنه اگر (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } وگرنه {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                اگر (!buf)
                        بازگشت -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                برای (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = اقرأ(fd, buf, size + 1);
                        اگر (k >= 0) {
                                n = k;
                                بشکن;
                        }

                        اگر (errno != EINTR)
                                بازگشت -errno;
                }

                /* Consider a short read as EOF */
                اگر (n <= size)
                        بشکن;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                اگر (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        بشکن;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                اگر (n_retries <= 0)
                        بازگشت -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                اگر (lseek(fd, 0, SEEK_SET) < 0)
                        بازگشت -errno;

                buf = mfree(buf);
        }

        اگر (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                اگر (!ret_size && memchr(buf, 0, n))
                        بازگشت -EBADMSG;

                اگر (n < size) {
                        نویسه *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        اگر (!p)
                                بازگشت -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        اگر (ret_size)
                *ret_size = n;

        بازگشت !truncated;
}

صحیح read_full_stream_full(
                FILE *f,
                ثابت نویسه *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                نویسه **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ نویسه *buf = پوچ;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        صحیح fd, r;

        أكد(f);
        أكد(ret_contents);
        أكد(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        أكد(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        اگر (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                بازگشت -ERANGE;

        اگر ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                اگر (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                وگرنه
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        اگر (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ساختار stat st;

                اگر (fstat(fd, &st) < 0)
                        بازگشت -errno;

                اگر (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        اگر (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                اگر (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        اگر (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (تهی) warn_file_is_world_accessible(filename, &st, پوچ, 0);
                }
        } وگرنه اگر (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                بازگشت -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        اگر (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        اگر (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        اگر (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                بازگشت -errno;

        n = l = 0;
        برای (;;) {
                نویسه *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                اگر (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                اگر (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        اگر (!t) {
                                r = -ENOMEM;
                                برو finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        آزاد(buf);
                } وگرنه {
                        t = realloc(buf, n_next + 1);
                        اگر (!t)
                                بازگشت -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                أكد(k <= n - l);
                l += k;

                اگر (ferror(f)) {
                        r = errno_or_else(EIO);
                        برو finalize;
                }
                اگر (feof(f))
                        بشکن;

                اگر (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        أكد(l == size);
                        بشکن;
                }

                أكد(k > 0); /* we can't have read zero bytes because that would have been EOF */

                اگر (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        برو finalize;
                }

                اگر (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        برو finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        اگر (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ تهی *decoded = پوچ;
                size_t decoded_size;

                buf[l++] = 0;
                اگر (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                وگرنه
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                اگر (r < 0)
                        برو finalize;

                اگر (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                اگر (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        برو finalize;
                }
        }

        اگر (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                اگر (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        برو finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        اگر (ret_size)
                *ret_size = l;

        بازگشت 0;

finalize:
        اگر (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        بازگشت r;
}

صحیح read_full_file_full(
                صحیح dir_fd,
                ثابت نویسه *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ثابت نویسه *bind_name,
                نویسه **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = پوچ;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        صحیح r;

        أكد(ret_contents);

        اگر (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        اگر (r < 0)
                بازگشت r;

        بازگشت read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

صحیح script_get_shebang_interpreter(ثابت نویسه *path, نویسه **ret) {
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح r;

        أكد(path);

        f = fopen(path, "re");
        اگر (!f)
                بازگشت -errno;

        نویسه c;
        r = safe_fgetc(f, &c);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EBADMSG;
        اگر (c != '#')
                بازگشت -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EBADMSG;
        اگر (c != '!')
                بازگشت -EMEDIUMTYPE;

        _cleanup_free_ نویسه *سطر = پوچ;
        r = read_سطر(f, LONG_سطر_MAX, &سطر);
        اگر (r < 0)
                بازگشت r;

        _cleanup_free_ نویسه *p = پوچ;
        ثابت نویسه *s = سطر;

        r = extract_first_word(&s, &p, /* separators= */ پوچ, /* flags= */ 0);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -ENOEXEC;

        اگر (ret)
                *ret = TAKE_PTR(p);
        بازگشت 0;
}

صحیح get_proc_field(ثابت نویسه *path, ثابت نویسه *کلید, نویسه **ret) {
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        أكد(path);
        أكد(کلید);

        r = fopen_unlocked(path, "re", &f);
        اگر (r == -ENOENT && proc_mounted() == 0)
                بازگشت -ENOSYS;
        اگر (r < 0)
                بازگشت r;

        برای (;;) {
                 _cleanup_free_ نویسه *سطر = پوچ;

                 r = read_سطر(f, LONG_سطر_MAX, &سطر);
                 اگر (r < 0)
                         بازگشت r;
                 اگر (r == 0)
                         بازگشت -ENODATA;

                 نویسه *l = startswith(سطر, کلید);
                 اگر (l && *l == ':') {
                         اگر (ret) {
                                 نویسه *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 اگر (!s)
                                         بازگشت -ENOMEM;

                                 *ret = s;
                         }

                         بازگشت 0;
                 }
        }
}

DIR* xopendirat(صحیح dir_fd, ثابت نویسه *path, صحیح flags) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(!(flags & (O_CREAT|O_TMPFILE)));

        اگر ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                بازگشت opendir(path);

        اگر (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        اگر (fd < 0)
                بازگشت پوچ;

        بازگشت take_fdopendir(&fd);
}

صحیح fopen_mode_to_flags(ثابت نویسه *mode) {
        ثابت نویسه *p;
        صحیح flags;

        أكد(mode);

        اگر ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        وگرنه اگر ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        وگرنه اگر ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        وگرنه اگر ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        وگرنه اگر ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        وگرنه اگر ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        وگرنه
                بازگشت -EINVAL;

        برای (; *p != 0; p++) {

                انتخاب (*p) {

                حالت 'e':
                        flags |= O_CLOEXEC;
                        بشکن;

                حالت 'x':
                        flags |= O_EXCL;
                        بشکن;

                حالت 'm':
                        /* ignore this here, fdopen() might care later though */
                        بشکن;

                حالت 'c': /* not sure what to do about this one */
                پیشفرض:
                        بازگشت -EINVAL;
                }
        }

        بازگشت flags;
}

ایستا صحیح xfopenat_regular(صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *mode, صحیح open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(mode);
        أكد(ret);

        اگر (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        وگرنه اگر (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ نویسه *j = strjoin("/", path);
                اگر (!j)
                        بازگشت -ENOMEM;

                f = fopen(j, mode);
        } وگرنه {
                _cleanup_close_ صحیح fd = -EBADF;
                صحیح mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                اگر (mode_flags < 0)
                        بازگشت mode_flags;

                اگر (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        اگر (fd < 0)
                                بازگشت -errno;
                } وگرنه {
                        اگر (dir_fd == AT_FDCWD)
                                بازگشت -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        اگر (fd < 0)
                                بازگشت fd;
                }

                f = take_fdopen(&fd, mode);
        }
        اگر (!f)
                بازگشت -errno;

        *ret = f;
        بازگشت 0;
}

ایستا صحیح xfopenat_unix_socket(صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *bind_name, FILE **ret) {
        _cleanup_close_ صحیح sk = -EBADF;
        FILE *f;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        اگر (sk < 0)
                بازگشت -errno;

        اگر (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                اجتماع sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                اگر (r < 0)
                        بازگشت r;

                اگر (bind(sk, &bsa.sa, r) < 0)
                        بازگشت -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        اگر (r < 0)
                بازگشت r;

        اگر (shutdown(sk, SHUT_WR) < 0)
                بازگشت -errno;

        f = take_fdopen(&sk, "r");
        اگر (!f)
                بازگشت -errno;

        *ret = f;
        بازگشت 0;
}

صحیح xfopenat_full(
                صحیح dir_fd,
                ثابت نویسه *path,
                ثابت نویسه *mode,
                صحیح open_flags,
                XfopenFlags flags,
                ثابت نویسه *bind_name,
                FILE **ret) {

        FILE *f = پوچ;  /* avoid false maybe-uninitialized warning */
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(mode);
        أكد(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        اگر (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                اگر (IN_SET(r, -ENOTSOCK, -EINVAL))
                        بازگشت -ENXIO; /* propagate original error if this is not a socket after all */
        }
        اگر (r < 0)
                بازگشت r;

        اگر (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (تهی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        بازگشت 0;
}

صحیح fdopen_independent(صحیح fd, ثابت نویسه *mode, FILE **ret) {
        _cleanup_close_ صحیح copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح mode_flags;

        أكد(fd >= 0);
        أكد(mode);
        أكد(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        اگر (mode_flags < 0)
                بازگشت mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        اگر (copy_fd < 0)
                بازگشت copy_fd;

        f = take_fdopen(&copy_fd, mode);
        اگر (!f)
                بازگشت -errno;

        *ret = TAKE_PTR(f);
        بازگشت 0;
}

ایستا صحیح جستجو_and_open_internal(
                ثابت نویسه *path,
                صحیح mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ثابت نویسه *ریشه,
                نویسه **جستجو,
                صحیح *ret_fd,
                نویسه **ret_path) {

        صحیح r;

        أكد(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        أكد(path);

        اگر (path_is_absolute(path)) {
                _cleanup_close_ صحیح fd = -EBADF;

                اگر (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                وگرنه
                        r = RET_NERRNO(access(path, mode));
                اگر (r < 0)
                        بازگشت r;

                اگر (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                بازگشت 0;
        }

        اگر (!path_strv_resolve_uniq(جستجو, ریشه))
                بازگشت -ENOMEM;

        STRV_FOREACH(i, جستجو) {
                _cleanup_close_ صحیح fd = -EBADF;
                _cleanup_free_ نویسه *p = پوچ;

                p = path_join(ریشه, *i, path);
                اگر (!p)
                        بازگشت -ENOMEM;

                اگر (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                وگرنه
                        r = RET_NERRNO(access(p, F_OK));
                اگر (r >= 0) {
                        اگر (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        اگر (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        بازگشت 0;
                }
                اگر (r != -ENOENT)
                        بازگشت r;
        }

        بازگشت -ENOENT;
}

صحیح جستجو_and_open(
                ثابت نویسه *path,
                صحیح mode,
                ثابت نویسه *ریشه,
                نویسه **جستجو,
                صحیح *ret_fd,
                نویسه **ret_path) {

        _cleanup_strv_free_ نویسه **copy = پوچ;

        أكد(path);

        copy = strv_copy(جستجو);
        اگر (!copy)
                بازگشت -ENOMEM;

        بازگشت جستجو_and_open_internal(path, mode, ریشه, copy, ret_fd, ret_path);
}

ایستا صحیح جستجو_and_fopen_internal(
                ثابت نویسه *path,
                ثابت نویسه *mode,
                ثابت نویسه *ریشه,
                نویسه **جستجو,
                FILE **ret_file,
                نویسه **ret_path) {

        _cleanup_free_ نویسه *found_path = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(path);
        أكد(mode || !ret_file);

        r = جستجو_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        ریشه,
                        جستجو,
                        ret_file ? &fd : پوچ,
                        ret_path ? &found_path : پوچ);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                اگر (!f)
                        بازگشت -errno;

                *ret_file = f;
        }

        اگر (ret_path)
                *ret_path = TAKE_PTR(found_path);

        بازگشت 0;
}

صحیح جستجو_and_fopen(
                ثابت نویسه *path,
                ثابت نویسه *mode,
                ثابت نویسه *ریشه,
                ثابت نویسه **جستجو,
                FILE **ret_file,
                نویسه **ret_path) {

        _cleanup_strv_free_ نویسه **copy = پوچ;

        أكد(path);
        أكد(mode || !ret_file);

        copy = strv_copy((نویسه**) جستجو);
        اگر (!copy)
                بازگشت -ENOMEM;

        بازگشت جستجو_and_fopen_internal(path, mode, ریشه, copy, ret_file, ret_path);
}

صحیح جستجو_and_fopen_nulstr(
                ثابت نویسه *path,
                ثابت نویسه *mode,
                ثابت نویسه *ریشه,
                ثابت نویسه *جستجو,
                FILE **ret_file,
                نویسه **ret_path) {

        _cleanup_strv_free_ نویسه **l = پوچ;

        أكد(path);
        أكد(mode || !ret_file);

        l = strv_split_nulstr(جستجو);
        اگر (!l)
                بازگشت -ENOMEM;

        بازگشت جستجو_and_fopen_internal(path, mode, ریشه, l, ret_file, ret_path);
}

صحیح fflush_and_check(FILE *f) {
        أكد(f);

        errno = 0;
        fflush(f);

        اگر (ferror(f))
                بازگشت errno_or_else(EIO);

        بازگشت 0;
}

صحیح fflush_sync_and_check(FILE *f) {
        صحیح r, fd;

        أكد(f);

        r = fflush_and_check(f);
        اگر (r < 0)
                بازگشت r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        اگر (fd < 0)
                بازگشت 0;

        r = fsync_full(fd);
        اگر (r < 0)
                بازگشت r;

        بازگشت 0;
}

صحیح write_timestamp_file_atomic(ثابت نویسه *fn, usec_t n) {
        نویسه ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        اگر (!timestamp_is_set(n))
                بازگشت -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        بازگشت write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

صحیح read_timestamp_file(ثابت نویسه *fn, usec_t *ret) {
        _cleanup_free_ نویسه *ln = پوچ;
        uint64_t t;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file(fn, &ln);
        اگر (r < 0)
                بازگشت r;

        r = safe_atou64(ln, &t);
        اگر (r < 0)
                بازگشت r;

        اگر (!timestamp_is_set(t))
                بازگشت -ERANGE;

        *ret = (usec_t) t;
        بازگشت 0;
}

صحیح fputs_with_separator(FILE *f, ثابت نویسه *s, ثابت نویسه *separator, bool *space) {
        أكد(s);
        أكد(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        اگر (!f)
                f = stdout;

        اگر (!separator)
                separator = " ";

        اگر (*space)
                اگر (fputs(separator, f) < 0)
                        بازگشت -EIO;

        *space = true;

        اگر (fputs(s, f) < 0)
                بازگشت -EIO;

        بازگشت 0;
}

صحیح fputs_with_newline(FILE *f, ثابت نویسه *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        اگر (isempty(s))
                بازگشت 0;

        اگر (!f)
                f = stdout;

        اگر (fputs(s, f) < 0)
                بازگشت -EIO;

        اگر (endswith(s, "\n"))
                بازگشت 0;

        اگر (fputc('\n', f) < 0)
                بازگشت -EIO;

        بازگشت 1;
}

/* A bitmask of the EOL markers we know */
تعریفنوع شمارش End_Of_سطر_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_سطر_Marker;

ایستا End_Of_سطر_Marker categorize_eol(نویسه c, Read_سطر_Flags flags) {

        اگر (!FLAGS_SET(flags, READ_سطر_ONLY_NUL)) {
                اگر (c == '\n')
                        بازگشت EOL_TEN;
                اگر (c == '\r')
                        بازگشت EOL_THIRTEEN;
        }

        اگر (c == '\0')
                بازگشت EOL_ZERO;

        بازگشت EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, پوچ);

صحیح read_سطر_full(FILE *f, size_t limit, Read_سطر_Flags flags, نویسه **ret) {
        _cleanup_free_ نویسه *buffer = پوچ;
        size_t n = 0, شمار = 0;
        صحیح r;

        أكد(f);

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

        اگر (ret) {
                اگر (!GREEDY_REALLOC(buffer, 1))
                        بازگشت -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_سطر_Marker previous_eol = EOL_NONE;
                flockfile(f);

                برای (;;) {
                        End_Of_سطر_Marker eol;
                        نویسه c;

                        اگر (n >= limit)
                                بازگشت -ENOBUFS;

                        اگر (شمار >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                بازگشت -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0) /* EOF is definitely EOL */
                                بشکن;

                        eol = categorize_eol(c, flags);

                        اگر (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned نویسه) c, f) != EOF);
                                بشکن;
                        }

                        شمار++;

                        اگر (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                اگر ((flags & (READ_سطر_IS_A_TTY|READ_سطر_NOT_A_TTY)) == 0) {
                                        صحیح fd;

                                        fd = fileno(f);
                                        اگر (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_سطر_NOT_A_TTY;
                                        وگرنه
                                                flags |= isatty_safe(fd) ? READ_سطر_IS_A_TTY : READ_سطر_NOT_A_TTY;
                                }
                                اگر (FLAGS_SET(flags, READ_سطر_IS_A_TTY))
                                        بشکن;
                        }

                        اگر (eol != EOL_NONE) {
                                previous_eol |= eol;
                                ادامه;
                        }

                        اگر (ret) {
                                اگر (!GREEDY_REALLOC(buffer, n + 2))
                                        بازگشت -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        اگر (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        بازگشت (صحیح) شمار;
}

صحیح read_stripped_سطر(FILE *f, size_t limit, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r, k;

        أكد(f);

        r = read_سطر(f, limit, ret ? &s : پوچ);
        اگر (r < 0)
                بازگشت r;

        اگر (ret) {
                ثابت نویسه *p = strstrip(s);
                اگر (p == s)
                        *ret = TAKE_PTR(s);
                وگرنه {
                        k = strdup_to(ret, p);
                        اگر (k < 0)
                                بازگشت k;
                }
        }

        بازگشت r > 0;          /* Return 1 if something was read. */
}

صحیح safe_fgetc(FILE *f, نویسه *ret) {
        صحیح k;

        أكد(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        اگر (k == EOF) {
                اگر (ferror(f))
                        بازگشت errno_or_else(EIO);

                اگر (ret)
                        *ret = 0;

                بازگشت 0;
        }

        اگر (ret)
                *ret = k;

        بازگشت 1;
}

صحیح warn_file_is_world_accessible(ثابت نویسه *filename, ساختار stat *st, ثابت نویسه *unit, unsigned سطر) {
        ساختار stat _st;

        اگر (!filename)
                بازگشت 0;

        اگر (!st) {
                اگر (stat(filename, &_st) < 0)
                        بازگشت -errno;
                st = &_st;
        }

        اگر ((st->st_mode & S_IRWXO) == 0)
                بازگشت 0;

        اگر (unit)
                log_syntax(unit, LOG_WARNING, filename, سطر, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        وگرنه
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        بازگشت 0;
}

صحیح write_data_file_atomic_at(
                صحیح dir_fd,
                ثابت نویسه *path,
                ثابت ساختار iovec *iovec,
                WriteDataFileFlags flags) {

        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ نویسه *dn = پوچ, *fn = پوچ;
        r = path_split_prefix_filename(path, &dn, &fn);
        اگر (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                بازگشت -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        اگر (r < 0)
                بازگشت r;

        _cleanup_close_ صحیح mfd = -EBADF;
        اگر (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ پوچ,
                            &mfd);
                اگر (r < 0)
                        بازگشت r;

                dir_fd = mfd;
        }

        _cleanup_free_ نویسه *t = پوچ;
        _cleanup_close_ صحیح fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        اگر (fd < 0)
                بازگشت fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        اگر (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                اگر (r < 0)
                        بازگشت r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        اگر (r < 0)
                بازگشت r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        اگر (r < 0)
                بازگشت r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        بازگشت 0;
}
