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
#تعریف_کرو READ_بھرا_بائٹیں_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#تعریف_کرو READ_بھرا_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#تعریف_کرو READ_VIRTUAL_بائٹیں_MAX (4U * U64_MB - UINT64_C(2))

صحیح fdopen_unlocked(صحیح fd, ثابت حرف *options, FILE **ret) {
        أكد(ret);

        FILE *f = fdopen(fd, options);
        اگر (!f)
                واپس -errno;

        (خالی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        واپس 0;
}

صحیح take_fdopen_unlocked(صحیح *fd, ثابت حرف *options, FILE **ret) {
        صحیح r;

        أكد(fd);

        r = fdopen_unlocked(*fd, options, ret);
        اگر (r < 0)
                واپس r;

        *fd = -EBADF;

        واپس 0;
}

FILE* take_fdopen(صحیح *fd, ثابت حرف *options) {
        أكد(fd);

        FILE *f = fdopen(*fd, options);
        اگر (!f)
                واپس تہی;

        *fd = -EBADF;

        واپس f;
}

DIR* take_fdopendir(صحیح *dfd) {
        أكد(dfd);

        DIR *d = fdopendir(*dfd);
        اگر (!d)
                واپس تہی;

        *dfd = -EBADF;

        واپس d;
}

FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        اگر (!f)
                واپس تہی;

        (خالی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        واپس f;
}

FILE* fmemopen_unlocked(خالی *buf, size_t سائز, ثابت حرف *منوال) {
        FILE *f = fmemopen(buf, سائز, منوال);
        اگر (!f)
                واپس تہی;

        (خالی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        واپس f;
}

صحیح write_string_دھارا_بھرا(
                FILE *f,
                ثابت حرف *سطر,
                WriteStringFileFlags flags,
                ثابت ساخت timespec *ts) {

        bool needs_nl;
        صحیح r, fd = -EBADF;

        أكد(f);
        أكد(سطر);

        اگر (ferror(f))
                واپس -EIO;

        اگر (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                اگر (fd < 0)
                        واپس -EBADF;
        }

        اگر (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ حرف *t = تہی;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                اگر (fd < 0) {
                        fd = fileno(f);
                        اگر (fd < 0)
                                واپس -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                اگر (read_virtual_file_fd(fd, لڑی_لمبائی(سطر)+1, &t, تہی) > 0 &&
                    streq_skip_trailing_chars(سطر, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", سطر);
                        واپس 0;
                }

                اگر (lseek(fd, 0, SEEK_SET) < 0)
                        واپس -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(سطر, "\n");

        اگر (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_بفر)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                سطر = strjoina(سطر, "\n");
                needs_nl = false;
        }

        اگر (fputs(سطر, f) == EOF)
                واپس -errno;

        اگر (needs_nl)
                اگر (fputc('\n', f) == EOF)
                        واپس -errno;

        اگر (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ورنہ
                r = fflush_and_check(f);
        اگر (r < 0)
                واپس r;

        اگر (ts) {
                ثابت ساخت timespec twice[2] = {*ts, *ts};

                أكد(fd >= 0);
                اگر (futimens(fd, twice) < 0)
                        واپس -errno;
        }

        واپس 0;
}

ساکن mode_t write_string_file_flags_مقصد_منوال(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        واپس FLAGS_سیٹ(flags, WRITE_STRING_FILE_منوال_0600) ? 0600 :
                FLAGS_سیٹ(flags, WRITE_STRING_FILE_منوال_0444) ? 0444 : 0644;
}

ساکن صحیح write_string_file_atomic_at(
                صحیح dir_fd,
                ثابت حرف *fn,
                ثابت حرف *سطر,
                WriteStringFileFlags flags,
                ثابت ساخت timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(fn);
        أكد(سطر);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t منوال = write_string_file_flags_مقصد_منوال(flags);

        bool call_label_ops_post = false;
        اگر (FLAGS_سیٹ(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, منوال, label_context);
                اگر (r < 0)
                        واپس r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        صحیح k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? تہی : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        اگر (r < 0)
                واپس r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        اگر (k < 0)
                واپس k;

        r = write_string_دھارا_بھرا(f, سطر, flags, ts);
        اگر (r < 0)
                واپس r;

        r = fchmod_umask(fileno(f), منوال);
        اگر (r < 0)
                واپس r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        اگر (r < 0)
                واپس r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        اگر (FLAGS_سیٹ(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_کا_file(fileno(f));
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

صحیح write_string_file_بھرا_label(
                صحیح dir_fd,
                ثابت حرف *fn,
                ثابت حرف *سطر,
                WriteStringFileFlags flags,
                ثابت ساخت timespec *ts,
                ثابت حرف *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = تہی;
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
                        واپس r;
        }

        اگر (flags & WRITE_STRING_FILE_ATOMIC) {
                أكد(fn);
                أكد(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, سطر, flags, ts, label_context);
                اگر (r < 0)
                        جاؤ fail;

                واپس r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        اگر (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ورنہ {
                mode_t منوال = write_string_file_flags_مقصد_منوال(flags);
                bool call_label_ops_post = false;

                اگر (FLAGS_سیٹ(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, منوال, label_context);
                        اگر (r < 0)
                                جاؤ fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_سیٹ(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                منوال,
                                &made_file);
                اگر (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? تہی : fn, made_file, label_context));
        }
        اگر (r < 0)
                جاؤ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        اگر (r < 0)
                جاؤ fail;

        اگر (flags & WRITE_STRING_FILE_DISABLE_بفر)
                setvbuf(f, تہی, _IONBF, 0);

        r = write_string_دھارا_بھرا(f, سطر, flags, ts);
        اگر (r < 0)
                جاؤ fail;

        واپس 0;

fail:
        اگر (made_file)
                (خالی) unlinkat(dir_fd, fn, 0);

        اگر (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                واپس r;

        f = محفوظ_fclose(f);
        fd = محفوظ_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        اگر (verify_file_at(dir_fd, fn, سطر, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                واپس 0;

        واپس r;
}

صحیح write_string_filef(
                ثابت حرف *fn,
                WriteStringFileFlags flags,
                ثابت حرف *format, ...) {

        _cleanup_free_ حرف *p = تہی;
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        اگر (r < 0)
                واپس -ENOMEM;

        واپس write_string_file(fn, p, flags);
}

صحیح write_string_filef_at(
                صحیح dir_fd,
                ثابت حرف *fn,
                WriteStringFileFlags flags,
                ثابت حرف *format, ...) {

        _cleanup_free_ حرف *p = تہی;
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        اگر (r < 0)
                واپس -ENOMEM;

        واپس write_string_file_at(dir_fd, fn, p, flags);
}

صحیح write_base64_file_at(
                صحیح dir_fd,
                ثابت حرف *fn,
                ثابت ساخت iovec *ڈیٹا,
                WriteStringFileFlags flags) {

        _cleanup_free_ حرف *encoded = تہی;
        ssize_t n;

        n = base64mem_بھرا(ڈیٹا ? ڈیٹا->iov_base : تہی, ڈیٹا ? ڈیٹا->iov_len : 0, 79, &encoded);
        اگر (n < 0)
                واپس n;

        واپس write_string_file_at(dir_fd, fn, encoded, flags);
}

صحیح read_one_سطر_file_at(صحیح dir_fd, ثابت حرف *filename, حرف **ret) {
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);
        أكد(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        اگر (r < 0)
                واپس r;

        واپس read_سطر(f, LONG_سطر_MAX, ret);
}

صحیح read_boolean_file_at(صحیح dir_fd, ثابت حرف *filename) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);

        r = read_one_سطر_file_at(dir_fd, filename, &s);
        اگر (r < 0)
                واپس r;

        واپس parse_boolean(s);
}

صحیح verify_file_at(صحیح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_free_ حرف *buf = تہی;
        size_t l, k;
        صحیح r;

        أكد(blob);

        l = لڑی_لمبائی(blob);

        اگر (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = حافظہ_دو(l + accept_extra_nl + 1);
        اگر (!buf)
                واپس -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        اگر (r < 0)
                واپس r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        اگر (ferror(f))
                واپس errno_or_else(EIO);

        اگر (k != l && k != l + accept_extra_nl)
                واپس 0;
        اگر (memcmp(buf, blob, l) != 0)
                واپس 0;
        اگر (k > l && buf[l] != '\n')
                واپس 0;

        واپس 1;
}

صحیح read_virtual_file_at(
                صحیح dir_fd,
                ثابت حرف *filename,
                size_t max_سائز,
                حرف **ret_contents,
                size_t *ret_سائز) {

        _cleanup_free_ حرف *buf = تہی;
        size_t n, سائز;
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
        أكد(max_سائز <= READ_VIRTUAL_بائٹیں_MAX || max_سائز == SIZE_MAX);

        _cleanup_close_ صحیح fd = -EBADF;
        اگر (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ورنہ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        اگر (fd < 0)
                واپس fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        برائے (;;) {
                ساخت stat st;

                اگر (fstat(fd, &st) < 0)
                        واپس -errno;

                اگر (!S_ISREG(st.st_mode))
                        واپس -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_بائٹیں_MAX < SSIZE_MAX);
                اگر (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        اگر (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                اگر (max_سائز == SIZE_MAX)
                                        واپس -EFBIG;

                                سائز = max_سائز;
                        } ورنہ {
                                سائز = MIN((size_t) st.st_size, max_سائز);

                                اگر (سائز > READ_VIRTUAL_بائٹیں_MAX)
                                        واپس -EFBIG;
                        }

                        n_retries--;
                } ورنہ اگر (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        سائز = MIN3(صفحہ_سائز() - 1, READ_VIRTUAL_بائٹیں_MAX, max_سائز);
                        n_retries = 1;
                } ورنہ {
                        سائز = MIN(READ_VIRTUAL_بائٹیں_MAX, max_سائز);
                        n_retries = 0;
                }

                buf = حافظہ_دو(سائز + 1);
                اگر (!buf)
                        واپس -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                سائز = MIN3(MALLOC_SIZEOF_محفوظ(buf) - 1, max_سائز, READ_VIRTUAL_بائٹیں_MAX);

                برائے (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = اقرأ(fd, buf, سائز + 1);
                        اگر (k >= 0) {
                                n = k;
                                توڑو;
                        }

                        اگر (errno != EINTR)
                                واپس -errno;
                }

                /* Consider a short read as EOF */
                اگر (n <= سائز)
                        توڑو;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                اگر (max_سائز != SIZE_MAX && n > max_سائز) {
                        n = سائز; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        توڑو;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                اگر (n_retries <= 0)
                        واپس -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                اگر (lseek(fd, 0, SEEK_SET) < 0)
                        واپس -errno;

                buf = mfree(buf);
        }

        اگر (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                اگر (!ret_سائز && memchr(buf, 0, n))
                        واپس -EBADMSG;

                اگر (n < سائز) {
                        حرف *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        اگر (!p)
                                واپس -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        اگر (ret_سائز)
                *ret_سائز = n;

        واپس !truncated;
}

صحیح read_بھرا_دھارا_بھرا(
                FILE *f,
                ثابت حرف *filename,
                uint64_t انحراف,
                size_t سائز,
                Read_بھرا_File_Flags flags,
                حرف **ret_contents,
                size_t *ret_سائز) {

        _cleanup_free_ حرف *buf = تہی;
        size_t n, n_اگلا = 0, l, expected_decoded_سائز = سائز;
        صحیح fd, r;

        أكد(f);
        أكد(ret_contents);
        أكد(!FLAGS_سیٹ(flags, READ_بھرا_FILE_UNBASE64 | READ_بھرا_FILE_UNHEX));
        أكد(سائز != SIZE_MAX || !FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER));

        اگر (انحراف != UINT64_MAX && انحراف > LONG_MAX) /* fseek() can only deal with "long" offsets */
                واپس -ERANGE;

        اگر ((flags & (READ_بھرا_FILE_UNBASE64 | READ_بھرا_FILE_UNHEX)) != 0) {
                اگر (سائز <= SIZE_MAX / READ_بھرا_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        سائز *= READ_بھرا_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ورنہ
                        سائز = SIZE_MAX;
        }

        fd = fileno(f);
        اگر (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ساخت stat st;

                اگر (fstat(fd, &st) < 0)
                        واپس -errno;

                اگر (FLAGS_سیٹ(flags, READ_بھرا_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        اگر (st.st_size > 0 &&
                            (سائز == SIZE_MAX || FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, انحراف == UINT64_MAX ? 0 : انحراف);

                                اگر (rsize < SIZE_MAX) /* overflow check */
                                        n_اگلا = rsize + 1;
                        }

                        اگر (flags & READ_بھرا_FILE_WARN_WORLD_READABLE)
                                (خالی) warn_file_is_world_accessible(filename, &st, تہی, 0);
                }
        } ورنہ اگر (FLAGS_سیٹ(flags, READ_بھرا_FILE_VERIFY_REGULAR))
                واپس -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        اگر (n_اگلا == 0)
                n_اگلا = سائز != SIZE_MAX && !FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER) ? سائز : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        اگر (n_اگلا > READ_بھرا_بائٹیں_MAX)
                n_اگلا = READ_بھرا_بائٹیں_MAX + 1;

        اگر (انحراف != UINT64_MAX && fseek(f, انحراف, SEEK_SET) < 0)
                واپس -errno;

        n = l = 0;
        برائے (;;) {
                حرف *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                اگر (FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER) && n_اگلا > سائز)
                        n_اگلا = سائز + 1;

                اگر (flags & READ_بھرا_FILE_SECURE) {
                        t = حافظہ_دو(n_اگلا + 1);
                        اگر (!t) {
                                r = -ENOMEM;
                                جاؤ finalize;
                        }
                        memcpy_محفوظ(t, buf, n);
                        explicit_bzero_محفوظ(buf, n);
                        آزاد(buf);
                } ورنہ {
                        t = realloc(buf, n_اگلا + 1);
                        اگر (!t)
                                واپس -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = سائز == SIZE_MAX ? MALLOC_SIZEOF_محفوظ(buf) - 1 : n_اگلا;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                أكد(k <= n - l);
                l += k;

                اگر (ferror(f)) {
                        r = errno_or_else(EIO);
                        جاؤ finalize;
                }
                اگر (feof(f))
                        توڑو;

                اگر (سائز != SIZE_MAX && !FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        أكد(l == سائز);
                        توڑو;
                }

                أكد(k > 0); /* we can't have read zero bytes because that would have been EOF */

                اگر (FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER) && l > سائز) {
                        r = -E2BIG;
                        جاؤ finalize;
                }

                اگر (n >= READ_بھرا_بائٹیں_MAX) {
                        r = -E2BIG;
                        جاؤ finalize;
                }

                n_اگلا = MIN(n * 2, READ_بھرا_بائٹیں_MAX);
        }

        اگر (flags & (READ_بھرا_FILE_UNBASE64 | READ_بھرا_FILE_UNHEX)) {
                _cleanup_free_ خالی *decoded = تہی;
                size_t decoded_سائز;

                buf[l++] = 0;
                اگر (flags & READ_بھرا_FILE_UNBASE64)
                        r = unbase64mem_بھرا(buf, l, flags & READ_بھرا_FILE_SECURE, &decoded, &decoded_سائز);
                ورنہ
                        r = unhexmem_بھرا(buf, l, flags & READ_بھرا_FILE_SECURE, &decoded, &decoded_سائز);
                اگر (r < 0)
                        جاؤ finalize;

                اگر (flags & READ_بھرا_FILE_SECURE)
                        explicit_bzero_محفوظ(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_سائز;

                اگر (FLAGS_سیٹ(flags, READ_بھرا_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_سائز) {
                        r = -E2BIG;
                        جاؤ finalize;
                }
        }

        اگر (!ret_سائز) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                اگر (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        جاؤ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        اگر (ret_سائز)
                *ret_سائز = l;

        واپس 0;

finalize:
        اگر (flags & READ_بھرا_FILE_SECURE)
                explicit_bzero_محفوظ(buf, n);

        واپس r;
}

صحیح read_بھرا_file_بھرا(
                صحیح dir_fd,
                ثابت حرف *filename,
                uint64_t انحراف,
                size_t سائز,
                Read_بھرا_File_Flags flags,
                ثابت حرف *bind_name,
                حرف **ret_contents,
                size_t *ret_سائز) {

        _cleanup_fclose_ FILE *f = تہی;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        صحیح r;

        أكد(ret_contents);

        اگر (FLAGS_سیٹ(flags, READ_بھرا_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            انحراف == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_بھرا(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        اگر (r < 0)
                واپس r;

        واپس read_بھرا_دھارا_بھرا(f, filename, انحراف, سائز, flags, ret_contents, ret_سائز);
}

صحیح script_get_shebang_interpreter(ثابت حرف *راستہ, حرف **ret) {
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        أكد(راستہ);

        f = fopen(راستہ, "re");
        اگر (!f)
                واپس -errno;

        حرف c;
        r = محفوظ_fgetc(f, &c);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EBADMSG;
        اگر (c != '#')
                واپس -EMEDIUMTYPE;
        r = محفوظ_fgetc(f, &c);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EBADMSG;
        اگر (c != '!')
                واپس -EMEDIUMTYPE;

        _cleanup_free_ حرف *سطر = تہی;
        r = read_سطر(f, LONG_سطر_MAX, &سطر);
        اگر (r < 0)
                واپس r;

        _cleanup_free_ حرف *p = تہی;
        ثابت حرف *s = سطر;

        r = نکالو_پہلا_لفظ(&s, &p, /* separators= */ تہی, /* flags= */ 0);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -ENOEXEC;

        اگر (ret)
                *ret = TAKE_PTR(p);
        واپس 0;
}

صحیح get_proc_میدان(ثابت حرف *راستہ, ثابت حرف *کلید, حرف **ret) {
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        أكد(راستہ);
        أكد(کلید);

        r = fopen_unlocked(راستہ, "re", &f);
        اگر (r == -ENOENT && proc_mounted() == 0)
                واپس -ENOSYS;
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                 _cleanup_free_ حرف *سطر = تہی;

                 r = read_سطر(f, LONG_سطر_MAX, &سطر);
                 اگر (r < 0)
                         واپس r;
                 اگر (r == 0)
                         واپس -ENODATA;

                 حرف *l = startswith(سطر, کلید);
                 اگر (l && *l == ':') {
                         اگر (ret) {
                                 حرف *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 اگر (!s)
                                         واپس -ENOMEM;

                                 *ret = s;
                         }

                         واپس 0;
                 }
        }
}

DIR* xopendirat(صحیح dir_fd, ثابت حرف *راستہ, صحیح flags) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(!(flags & (O_CREAT|O_TMPFILE)));

        اگر ((dir_fd == AT_FDCWD || راستہ_is_absolute(راستہ)) &&
            (flags &~ O_DIRECTORY) == 0)
                واپس opendir(راستہ);

        اگر (isempty(راستہ)) {
                راستہ = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, راستہ, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        اگر (fd < 0)
                واپس تہی;

        واپس take_fdopendir(&fd);
}

صحیح fopen_منوال_مقصد_flags(ثابت حرف *منوال) {
        ثابت حرف *p;
        صحیح flags;

        أكد(منوال);

        اگر ((p = startswith(منوال, "r+")))
                flags = O_RDWR;
        ورنہ اگر ((p = startswith(منوال, "r")))
                flags = O_RDONLY;
        ورنہ اگر ((p = startswith(منوال, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ورنہ اگر ((p = startswith(منوال, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ورنہ اگر ((p = startswith(منوال, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ورنہ اگر ((p = startswith(منوال, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ورنہ
                واپس -EINVAL;

        برائے (; *p != 0; p++) {

                انتخاب (*p) {

                حالت 'e':
                        flags |= O_CLOEXEC;
                        توڑو;

                حالت 'x':
                        flags |= O_EXCL;
                        توڑو;

                حالت 'm':
                        /* ignore this here, fdopen() might care later though */
                        توڑو;

                حالت 'c': /* not sure what to do about this one */
                طےشدہ:
                        واپس -EINVAL;
                }
        }

        واپس flags;
}

ساکن صحیح xfopenat_regular(صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *منوال, صحیح open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(منوال);
        أكد(ret);

        اگر (dir_fd == AT_FDCWD && راستہ && open_flags == 0)
                f = fopen(راستہ, منوال);
        ورنہ اگر (dir_fd == XAT_FDROOT && راستہ && open_flags == 0) {
                _cleanup_free_ حرف *j = strjoin("/", راستہ);
                اگر (!j)
                        واپس -ENOMEM;

                f = fopen(j, منوال);
        } ورنہ {
                _cleanup_close_ صحیح fd = -EBADF;
                صحیح منوال_flags;

                منوال_flags = fopen_منوال_مقصد_flags(منوال);
                اگر (منوال_flags < 0)
                        واپس منوال_flags;

                اگر (راستہ) {
                        fd = openat(dir_fd, راستہ, منوال_flags | open_flags);
                        اگر (fd < 0)
                                واپس -errno;
                } ورنہ {
                        اگر (dir_fd == AT_FDCWD)
                                واپس -EBADF;

                        fd = fd_reopen(dir_fd, (منوال_flags | open_flags) & ~O_NOFOLLOW);
                        اگر (fd < 0)
                                واپس fd;
                }

                f = take_fdopen(&fd, منوال);
        }
        اگر (!f)
                واپس -errno;

        *ret = f;
        واپس 0;
}

ساکن صحیح xfopenat_unix_socket(صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *bind_name, FILE **ret) {
        _cleanup_close_ صحیح sk = -EBADF;
        FILE *f;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        اگر (sk < 0)
                واپس -errno;

        اگر (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                اتحاد sockaddr_union bsa;

                r = sockaddr_un_سیٹ_راستہ(&bsa.un, bind_name);
                اگر (r < 0)
                        واپس r;

                اگر (bind(sk, &bsa.sa, r) < 0)
                        واپس -errno;
        }

        r = connect_unix_راستہ(sk, dir_fd, راستہ);
        اگر (r < 0)
                واپس r;

        اگر (shutdown(sk, SHUT_WR) < 0)
                واپس -errno;

        f = take_fdopen(&sk, "r");
        اگر (!f)
                واپس -errno;

        *ret = f;
        واپس 0;
}

صحیح xfopenat_بھرا(
                صحیح dir_fd,
                ثابت حرف *راستہ,
                ثابت حرف *منوال,
                صحیح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret) {

        FILE *f = تہی;  /* avoid false maybe-uninitialized warning */
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(منوال);
        أكد(ret);

        r = xfopenat_regular(dir_fd, راستہ, منوال, open_flags, &f);
        اگر (r == -ENXIO && FLAGS_سیٹ(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, راستہ, bind_name, &f);
                اگر (اندر_سیٹ(r, -ENOTSOCK, -EINVAL))
                        واپس -ENXIO; /* propagate original error if this is not a socket after all */
        }
        اگر (r < 0)
                واپس r;

        اگر (FLAGS_سیٹ(flags, XFOPEN_UNLOCKED))
                (خالی) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        واپس 0;
}

صحیح fdopen_independent(صحیح fd, ثابت حرف *منوال, FILE **ret) {
        _cleanup_close_ صحیح copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = تہی;
        صحیح منوال_flags;

        أكد(fd >= 0);
        أكد(منوال);
        أكد(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        منوال_flags = fopen_منوال_مقصد_flags(منوال);
        اگر (منوال_flags < 0)
                واپس منوال_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, منوال_flags & ~O_CREAT);
        اگر (copy_fd < 0)
                واپس copy_fd;

        f = take_fdopen(&copy_fd, منوال);
        اگر (!f)
                واپس -errno;

        *ret = TAKE_PTR(f);
        واپس 0;
}

ساکن صحیح تلاش_and_open_اندرونی(
                ثابت حرف *راستہ,
                صحیح منوال,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ثابت حرف *جڑ,
                حرف **تلاش,
                صحیح *ret_fd,
                حرف **ret_راستہ) {

        صحیح r;

        أكد(!ret_fd || !FLAGS_سیٹ(منوال, O_CREAT)); /* We don't support O_CREAT for this */
        أكد(راستہ);

        اگر (راستہ_is_absolute(راستہ)) {
                _cleanup_close_ صحیح fd = -EBADF;

                اگر (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(راستہ, منوال, 0777));
                ورنہ
                        r = RET_NERRNO(access(راستہ, منوال));
                اگر (r < 0)
                        واپس r;

                اگر (ret_راستہ) {
                        r = راستہ_simplify_alloc(راستہ, ret_راستہ);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                واپس 0;
        }

        اگر (!راستہ_strv_resolve_uniq(تلاش, جڑ))
                واپس -ENOMEM;

        STRV_FOREACH(i, تلاش) {
                _cleanup_close_ صحیح fd = -EBADF;
                _cleanup_free_ حرف *p = تہی;

                p = راستہ_join(جڑ, *i, راستہ);
                اگر (!p)
                        واپس -ENOMEM;

                اگر (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, منوال, 0777));
                ورنہ
                        r = RET_NERRNO(access(p, F_OK));
                اگر (r >= 0) {
                        اگر (ret_راستہ)
                                *ret_راستہ = راستہ_simplify(TAKE_PTR(p));

                        اگر (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        واپس 0;
                }
                اگر (r != -ENOENT)
                        واپس r;
        }

        واپس -ENOENT;
}

صحیح تلاش_and_open(
                ثابت حرف *راستہ,
                صحیح منوال,
                ثابت حرف *جڑ,
                حرف **تلاش,
                صحیح *ret_fd,
                حرف **ret_راستہ) {

        _cleanup_strv_free_ حرف **copy = تہی;

        أكد(راستہ);

        copy = strv_copy(تلاش);
        اگر (!copy)
                واپس -ENOMEM;

        واپس تلاش_and_open_اندرونی(راستہ, منوال, جڑ, copy, ret_fd, ret_راستہ);
}

ساکن صحیح تلاش_and_fopen_اندرونی(
                ثابت حرف *راستہ,
                ثابت حرف *منوال,
                ثابت حرف *جڑ,
                حرف **تلاش,
                FILE **ret_file,
                حرف **ret_راستہ) {

        _cleanup_free_ حرف *ملا_راستہ = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(راستہ);
        أكد(منوال || !ret_file);

        r = تلاش_and_open_اندرونی(
                        راستہ,
                        منوال ? fopen_منوال_مقصد_flags(منوال) : 0,
                        جڑ,
                        تلاش,
                        ret_file ? &fd : تہی,
                        ret_راستہ ? &ملا_راستہ : تہی);
        اگر (r < 0)
                واپس r;

        اگر (ret_file) {
                FILE *f = take_fdopen(&fd, منوال);
                اگر (!f)
                        واپس -errno;

                *ret_file = f;
        }

        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(ملا_راستہ);

        واپس 0;
}

صحیح تلاش_and_fopen(
                ثابت حرف *راستہ,
                ثابت حرف *منوال,
                ثابت حرف *جڑ,
                ثابت حرف **تلاش,
                FILE **ret_file,
                حرف **ret_راستہ) {

        _cleanup_strv_free_ حرف **copy = تہی;

        أكد(راستہ);
        أكد(منوال || !ret_file);

        copy = strv_copy((حرف**) تلاش);
        اگر (!copy)
                واپس -ENOMEM;

        واپس تلاش_and_fopen_اندرونی(راستہ, منوال, جڑ, copy, ret_file, ret_راستہ);
}

صحیح تلاش_and_fopen_nulstr(
                ثابت حرف *راستہ,
                ثابت حرف *منوال,
                ثابت حرف *جڑ,
                ثابت حرف *تلاش,
                FILE **ret_file,
                حرف **ret_راستہ) {

        _cleanup_strv_free_ حرف **l = تہی;

        أكد(راستہ);
        أكد(منوال || !ret_file);

        l = strv_بانٹو_nulstr(تلاش);
        اگر (!l)
                واپس -ENOMEM;

        واپس تلاش_and_fopen_اندرونی(راستہ, منوال, جڑ, l, ret_file, ret_راستہ);
}

صحیح fflush_and_check(FILE *f) {
        أكد(f);

        errno = 0;
        fflush(f);

        اگر (ferror(f))
                واپس errno_or_else(EIO);

        واپس 0;
}

صحیح fflush_sync_and_check(FILE *f) {
        صحیح r, fd;

        أكد(f);

        r = fflush_and_check(f);
        اگر (r < 0)
                واپس r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        اگر (fd < 0)
                واپس 0;

        r = fsync_بھرا(fd);
        اگر (r < 0)
                واپس r;

        واپس 0;
}

صحیح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n) {
        حرف ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        اگر (!timestamp_is_سیٹ(n))
                واپس -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        واپس write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

صحیح read_timestamp_file(ثابت حرف *fn, usec_t *ret) {
        _cleanup_free_ حرف *ln = تہی;
        uint64_t t;
        صحیح r;

        أكد(ret);

        r = read_one_سطر_file(fn, &ln);
        اگر (r < 0)
                واپس r;

        r = محفوظ_atou64(ln, &t);
        اگر (r < 0)
                واپس r;

        اگر (!timestamp_is_سیٹ(t))
                واپس -ERANGE;

        *ret = (usec_t) t;
        واپس 0;
}

صحیح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space) {
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
                        واپس -EIO;

        *space = true;

        اگر (fputs(s, f) < 0)
                واپس -EIO;

        واپس 0;
}

صحیح fputs_with_newline(FILE *f, ثابت حرف *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        اگر (isempty(s))
                واپس 0;

        اگر (!f)
                f = stdout;

        اگر (fputs(s, f) < 0)
                واپس -EIO;

        اگر (endswith(s, "\n"))
                واپس 0;

        اگر (fputc('\n', f) < 0)
                واپس -EIO;

        واپس 1;
}

/* A bitmask of the EOL markers we know */
قسمتعریف شمار انتہا_کا_سطر_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} انتہا_کا_سطر_Marker;

ساکن انتہا_کا_سطر_Marker categorize_eol(حرف c, Read_سطر_Flags flags) {

        اگر (!FLAGS_سیٹ(flags, READ_سطر_ONLY_NUL)) {
                اگر (c == '\n')
                        واپس EOL_TEN;
                اگر (c == '\r')
                        واپس EOL_THIRTEEN;
        }

        اگر (c == '\0')
                واپس EOL_ZERO;

        واپس EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_بھرا(FILE*, funlockfile, تہی);

صحیح read_سطر_بھرا(FILE *f, size_t limit, Read_سطر_Flags flags, حرف **ret) {
        _cleanup_free_ حرف *بفر = تہی;
        size_t n = 0, گنتی = 0;
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
                اگر (!GREEDY_REALLOC(بفر, 1))
                        واپس -ENOMEM;
        }

        {
                غیر_مستعمل _cleanup_(funlockfilep) FILE *flocked = f;
                انتہا_کا_سطر_Marker پچھلا_eol = EOL_NONE;
                flockfile(f);

                برائے (;;) {
                        انتہا_کا_سطر_Marker eol;
                        حرف c;

                        اگر (n >= limit)
                                واپس -ENOBUFS;

                        اگر (گنتی >= زیادہ_سے_زیادہ_عدد) /* We couldn't return the counter anymore as "int", hence refuse this */
                                واپس -ENOBUFS;

                        r = محفوظ_fgetc(f, &c);
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0) /* EOF is definitely EOL */
                                توڑو;

                        eol = categorize_eol(c, flags);

                        اگر (FLAGS_سیٹ(پچھلا_eol, EOL_ZERO) ||
                            (eol == EOL_NONE && پچھلا_eol != EOL_NONE) ||
                            (eol != EOL_NONE && (پچھلا_eol & eol) != 0)) {
                                /* Previous char was a NUL? This is not an EOL, but the previous char was? This type of
                                 * EOL marker has been seen right before?  In either of these three cases we are
                                 * done. But first, let's put this character back in the queue. (Note that we have to
                                 * cast this to (unsigned char) here as ungetc() expects a positive 'int', and if we
                                 * are on an architecture where 'char' equals 'signed char' we need to ensure we don't
                                 * pass a negative value here. That said, to complicate things further ungetc() is
                                 * actually happy with most negative characters and implicitly casts them back to
                                 * positive ones as needed, except for \xff (aka -1, aka EOF), which it refuses. What a
                                 * godawful API!) */
                                assert_se(ungetc((غیرعلامتی حرف) c, f) != EOF);
                                توڑو;
                        }

                        گنتی++;

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
                                        ورنہ
                                                flags |= isatty_محفوظ(fd) ? READ_سطر_IS_A_TTY : READ_سطر_NOT_A_TTY;
                                }
                                اگر (FLAGS_سیٹ(flags, READ_سطر_IS_A_TTY))
                                        توڑو;
                        }

                        اگر (eol != EOL_NONE) {
                                پچھلا_eol |= eol;
                                جاری;
                        }

                        اگر (ret) {
                                اگر (!GREEDY_REALLOC(بفر, n + 2))
                                        واپس -ENOMEM;

                                بفر[n] = c;
                        }

                        n++;
                }
        }

        اگر (ret) {
                بفر[n] = 0;

                *ret = TAKE_PTR(بفر);
        }

        واپس (صحیح) گنتی;
}

صحیح read_stripped_سطر(FILE *f, size_t limit, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r, k;

        أكد(f);

        r = read_سطر(f, limit, ret ? &s : تہی);
        اگر (r < 0)
                واپس r;

        اگر (ret) {
                ثابت حرف *p = strstrip(s);
                اگر (p == s)
                        *ret = TAKE_PTR(s);
                ورنہ {
                        k = strdup_مقصد(ret, p);
                        اگر (k < 0)
                                واپس k;
                }
        }

        واپس r > 0;          /* Return 1 if something was read. */
}

صحیح محفوظ_fgetc(FILE *f, حرف *ret) {
        صحیح k;

        أكد(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        اگر (k == EOF) {
                اگر (ferror(f))
                        واپس errno_or_else(EIO);

                اگر (ret)
                        *ret = 0;

                واپس 0;
        }

        اگر (ret)
                *ret = k;

        واپس 1;
}

صحیح warn_file_is_world_accessible(ثابت حرف *filename, ساخت stat *st, ثابت حرف *unit, غیرعلامتی سطر) {
        ساخت stat _st;

        اگر (!filename)
                واپس 0;

        اگر (!st) {
                اگر (stat(filename, &_st) < 0)
                        واپس -errno;
                st = &_st;
        }

        اگر ((st->st_mode & S_IRWXO) == 0)
                واپس 0;

        اگر (unit)
                log_syntax(unit, LOG_WARNING, filename, سطر, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ورنہ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        واپس 0;
}

صحیح write_ڈیٹا_file_atomic_at(
                صحیح dir_fd,
                ثابت حرف *راستہ,
                ثابت ساخت iovec *iovec,
                Write_ڈیٹا_File_Flags flags) {

        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ حرف *dn = تہی, *fn = تہی;
        r = راستہ_بانٹو_سابقہ_filename(راستہ, &dn, &fn);
        اگر (اندر_سیٹ(r, -EADDRNOTAVAIL, O_DIRECTORY))
                واپس -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        اگر (r < 0)
                واپس r;

        _cleanup_close_ صحیح mfd = -EBADF;
        اگر (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_سیٹ(flags, WRITE_ڈیٹا_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ تہی,
                            &mfd);
                اگر (r < 0)
                        واپس r;

                dir_fd = mfd;
        }

        _cleanup_free_ حرف *t = تہی;
        _cleanup_close_ صحیح fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        اگر (fd < 0)
                واپس fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        اگر (iovec_is_سیٹ(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                اگر (r < 0)
                        واپس r;
        }

        r = fchmod_umask(fd, FLAGS_سیٹ(flags, WRITE_ڈیٹا_FILE_منوال_0400) ? 0400 : 0644);
        اگر (r < 0)
                واپس r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        اگر (r < 0)
                واپس r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        واپس 0;
}
