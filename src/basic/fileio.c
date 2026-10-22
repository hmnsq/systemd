/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio_ext.h>
#ضمن <stdlib.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "hexdecoct.h"
#ضمن "io-util.h"
#ضمن "iovec-util.h"
#ضمن "label-util.h"
#ضمن "log.h"
#ضمن "mkdir.h"
#ضمن "nulstr-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "socket-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "sync-util.h"
#ضمن "terminal-util.h"
#ضمن "time-util.h"
#ضمن "tmpfile-util.h"

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

صحيح fdopen_unlocked(صحيح fd, ثابت حرف *options, FILE **ret) {
        أكد(ret);

        FILE *f = fdopen(fd, options);
        ܐܢ (!f)
                ܦܢܐ -errno;

        (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ܦܢܐ 0;
}

صحيح take_fdopen_unlocked(صحيح *fd, ثابت حرف *options, FILE **ret) {
        صحيح r;

        أكد(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ܐܢ (r < 0)
                ܦܢܐ r;

        *fd = -EBADF;

        ܦܢܐ 0;
}

FILE* take_fdopen(صحيح *fd, ثابت حرف *options) {
        أكد(fd);

        FILE *f = fdopen(*fd, options);
        ܐܢ (!f)
                ܦܢܐ عدم;

        *fd = -EBADF;

        ܦܢܐ f;
}

DIR* take_fdopendir(صحيح *dfd) {
        أكد(dfd);

        DIR *d = fdopendir(*dfd);
        ܐܢ (!d)
                ܦܢܐ عدم;

        *dfd = -EBADF;

        ܦܢܐ d;
}

FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ܐܢ (!f)
                ܦܢܐ عدم;

        (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ܦܢܐ f;
}

FILE* fmemopen_unlocked(فراغ *buf, size_t size, ثابت حرف *mode) {
        FILE *f = fmemopen(buf, size, mode);
        ܐܢ (!f)
                ܦܢܐ عدم;

        (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ܦܢܐ f;
}

صحيح write_string_stream_full(
                FILE *f,
                ثابت حرف *line,
                WriteStringFileFlags flags,
                ثابت بنية timespec *ts) {

        bool needs_nl;
        صحيح r, fd = -EBADF;

        أكد(f);
        أكد(line);

        ܐܢ (ferror(f))
                ܦܢܐ -EIO;

        ܐܢ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ܐܢ (fd < 0)
                        ܦܢܐ -EBADF;
        }

        ܐܢ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ حرف *t = عدم;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ܐܢ (fd < 0) {
                        fd = fileno(f);
                        ܐܢ (fd < 0)
                                ܦܢܐ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ܐܢ (read_virtual_file_fd(fd, strlen(line)+1, &t, عدم) > 0 &&
                    streq_skip_trailing_chars(line, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", line);
                        ܦܢܐ 0;
                }

                ܐܢ (lseek(fd, 0, SEEK_SET) < 0)
                        ܦܢܐ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(line, "\n");

        ܐܢ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                line = strjoina(line, "\n");
                needs_nl = false;
        }

        ܐܢ (fputs(line, f) == EOF)
                ܦܢܐ -errno;

        ܐܢ (needs_nl)
                ܐܢ (fputc('\n', f) == EOF)
                        ܦܢܐ -errno;

        ܐܢ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ܐܚܪܢܐ
                r = fflush_and_check(f);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ts) {
                ثابت بنية timespec twice[2] = {*ts, *ts};

                أكد(fd >= 0);
                ܐܢ (futimens(fd, twice) < 0)
                        ܦܢܐ -errno;
        }

        ܦܢܐ 0;
}

ساكن mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ܦܢܐ FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

ساكن صحيح write_string_file_atomic_at(
                صحيح dir_fd,
                ثابت حرف *fn,
                ثابت حرف *line,
                WriteStringFileFlags flags,
                ثابت بنية timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(fn);
        أكد(line);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        ܐܢ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        صحيح k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? عدم : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ܐܢ (r < 0)
                ܦܢܐ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ܐܢ (k < 0)
                ܦܢܐ k;

        r = write_string_stream_full(f, line, flags, ts);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = fchmod_umask(fileno(f), mode);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ܐܢ (r < 0)
                ܦܢܐ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ܐܢ (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
}

صحيح write_string_file_full_label(
                صحيح dir_fd,
                ثابت حرف *fn,
                ثابت حرف *line,
                WriteStringFileFlags flags,
                ثابت بنية timespec *ts,
                ثابت حرف *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(dir_fd == AT_FDCWD || dir_fd >= 0);
        أكد(line);

        /* We don't know how to verify whether the file contents was already on-disk. */
        أكد(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ܐܢ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                أكد(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܐܢ (flags & WRITE_STRING_FILE_ATOMIC) {
                أكد(fn);
                أكد(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, line, flags, ts, label_context);
                ܐܢ (r < 0)
                        اذهب fail;

                ܦܢܐ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ܐܢ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ܐܚܪܢܐ {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                ܐܢ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        ܐܢ (r < 0)
                                اذهب fail;

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
                ܐܢ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? عدم : fn, made_file, label_context));
        }
        ܐܢ (r < 0)
                اذهب fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ܐܢ (r < 0)
                اذهب fail;

        ܐܢ (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, عدم, _IONBF, 0);

        r = write_string_stream_full(f, line, flags, ts);
        ܐܢ (r < 0)
                اذهب fail;

        ܦܢܐ 0;

fail:
        ܐܢ (made_file)
                (فراغ) unlinkat(dir_fd, fn, 0);

        ܐܢ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ܦܢܐ r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ܐܢ (verify_file_at(dir_fd, fn, line, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ܦܢܐ 0;

        ܦܢܐ r;
}

صحيح write_string_filef(
                ثابت حرف *fn,
                WriteStringFileFlags flags,
                ثابت حرف *format, ...) {

        _cleanup_free_ حرف *p = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ܐܢ (r < 0)
                ܦܢܐ -ENOMEM;

        ܦܢܐ write_string_file(fn, p, flags);
}

صحيح write_string_filef_at(
                صحيح dir_fd,
                ثابت حرف *fn,
                WriteStringFileFlags flags,
                ثابت حرف *format, ...) {

        _cleanup_free_ حرف *p = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ܐܢ (r < 0)
                ܦܢܐ -ENOMEM;

        ܦܢܐ write_string_file_at(dir_fd, fn, p, flags);
}

صحيح write_base64_file_at(
                صحيح dir_fd,
                ثابت حرف *fn,
                ثابت بنية iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ حرف *encoded = عدم;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : عدم, data ? data->iov_len : 0, 79, &encoded);
        ܐܢ (n < 0)
                ܦܢܐ n;

        ܦܢܐ write_string_file_at(dir_fd, fn, encoded, flags);
}

صحيح read_one_line_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);
        أكد(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ read_line(f, LONG_LINE_MAX, ret);
}

صحيح read_boolean_file_at(صحيح dir_fd, ثابت حرف *filename) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);

        r = read_one_line_file_at(dir_fd, filename, &s);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ parse_boolean(s);
}

صحيح verify_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *buf = عدم;
        size_t l, k;
        صحيح r;

        أكد(blob);

        l = strlen(blob);

        ܐܢ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        ܐܢ (!buf)
                ܦܢܐ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ܐܢ (ferror(f))
                ܦܢܐ errno_or_else(EIO);

        ܐܢ (k != l && k != l + accept_extra_nl)
                ܦܢܐ 0;
        ܐܢ (memcmp(buf, blob, l) != 0)
                ܦܢܐ 0;
        ܐܢ (k > l && buf[l] != '\n')
                ܦܢܐ 0;

        ܦܢܐ 1;
}

صحيح read_virtual_file_at(
                صحيح dir_fd,
                ثابت حرف *filename,
                size_t max_size,
                حرف **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ حرف *buf = عدم;
        size_t n, size;
        صحيح n_retries;
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

        _cleanup_close_ صحيح fd = -EBADF;
        ܐܢ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ܐܚܪܢܐ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        لكل (;;) {
                بنية stat st;

                ܐܢ (fstat(fd, &st) < 0)
                        ܦܢܐ -errno;

                ܐܢ (!S_ISREG(st.st_mode))
                        ܦܢܐ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                ܐܢ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ܐܢ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ܐܢ (max_size == SIZE_MAX)
                                        ܦܢܐ -EFBIG;

                                size = max_size;
                        } ܐܚܪܢܐ {
                                size = MIN((size_t) st.st_size, max_size);

                                ܐܢ (size > READ_VIRTUAL_BYTES_MAX)
                                        ܦܢܐ -EFBIG;
                        }

                        n_retries--;
                } ܐܚܪܢܐ ܐܢ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } ܐܚܪܢܐ {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                ܐܢ (!buf)
                        ܦܢܐ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                لكل (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = اقرأ(fd, buf, size + 1);
                        ܐܢ (k >= 0) {
                                n = k;
                                اكسر;
                        }

                        ܐܢ (errno != EINTR)
                                ܦܢܐ -errno;
                }

                /* Consider a short read as EOF */
                ܐܢ (n <= size)
                        اكسر;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ܐܢ (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        اكسر;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ܐܢ (n_retries <= 0)
                        ܦܢܐ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ܐܢ (lseek(fd, 0, SEEK_SET) < 0)
                        ܦܢܐ -errno;

                buf = mfree(buf);
        }

        ܐܢ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ܐܢ (!ret_size && memchr(buf, 0, n))
                        ܦܢܐ -EBADMSG;

                ܐܢ (n < size) {
                        حرف *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ܐܢ (!p)
                                ܦܢܐ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ܐܢ (ret_size)
                *ret_size = n;

        ܦܢܐ !truncated;
}

صحيح read_full_stream_full(
                FILE *f,
                ثابت حرف *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                حرف **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ حرف *buf = عدم;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        صحيح fd, r;

        أكد(f);
        أكد(ret_contents);
        أكد(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        أكد(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        ܐܢ (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ܦܢܐ -ERANGE;

        ܐܢ ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                ܐܢ (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ܐܚܪܢܐ
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        ܐܢ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                بنية stat st;

                ܐܢ (fstat(fd, &st) < 0)
                        ܦܢܐ -errno;

                ܐܢ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                }

                ܐܢ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ܐܢ (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                ܐܢ (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        ܐܢ (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (فراغ) warn_file_is_world_accessible(filename, &st, عدم, 0);
                }
        } ܐܚܪܢܐ ܐܢ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ܦܢܐ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ܐܢ (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ܐܢ (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        ܐܢ (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ܦܢܐ -errno;

        n = l = 0;
        لكل (;;) {
                حرف *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ܐܢ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                ܐܢ (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        ܐܢ (!t) {
                                r = -ENOMEM;
                                اذهب finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        حرر(buf);
                } ܐܚܪܢܐ {
                        t = realloc(buf, n_next + 1);
                        ܐܢ (!t)
                                ܦܢܐ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                أكد(k <= n - l);
                l += k;

                ܐܢ (ferror(f)) {
                        r = errno_or_else(EIO);
                        اذهب finalize;
                }
                ܐܢ (feof(f))
                        اكسر;

                ܐܢ (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        أكد(l == size);
                        اكسر;
                }

                أكد(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ܐܢ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        اذهب finalize;
                }

                ܐܢ (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        اذهب finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        ܐܢ (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ فراغ *decoded = عدم;
                size_t decoded_size;

                buf[l++] = 0;
                ܐܢ (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ܐܚܪܢܐ
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ܐܢ (r < 0)
                        اذهب finalize;

                ܐܢ (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                ܐܢ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        اذهب finalize;
                }
        }

        ܐܢ (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ܐܢ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        اذهب finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ܐܢ (ret_size)
                *ret_size = l;

        ܦܢܐ 0;

finalize:
        ܐܢ (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ܦܢܐ r;
}

صحيح read_full_file_full(
                صحيح dir_fd,
                ثابت حرف *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ثابت حرف *bind_name,
                حرف **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = عدم;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        صحيح r;

        أكد(ret_contents);

        ܐܢ (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

صحيح script_get_shebang_interpreter(ثابت حرف *path, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(path);

        f = fopen(path, "re");
        ܐܢ (!f)
                ܦܢܐ -errno;

        حرف c;
        r = safe_fgetc(f, &c);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r == 0)
                ܦܢܐ -EBADMSG;
        ܐܢ (c != '#')
                ܦܢܐ -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r == 0)
                ܦܢܐ -EBADMSG;
        ܐܢ (c != '!')
                ܦܢܐ -EMEDIUMTYPE;

        _cleanup_free_ حرف *line = عدم;
        r = read_line(f, LONG_LINE_MAX, &line);
        ܐܢ (r < 0)
                ܦܢܐ r;

        _cleanup_free_ حرف *p = عدم;
        ثابت حرف *s = line;

        r = extract_first_word(&s, &p, /* separators= */ عدم, /* flags= */ 0);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r == 0)
                ܦܢܐ -ENOEXEC;

        ܐܢ (ret)
                *ret = TAKE_PTR(p);
        ܦܢܐ 0;
}

صحيح get_proc_field(ثابت حرف *path, ثابت حرف *key, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        أكد(path);
        أكد(key);

        r = fopen_unlocked(path, "re", &f);
        ܐܢ (r == -ENOENT && proc_mounted() == 0)
                ܦܢܐ -ENOSYS;
        ܐܢ (r < 0)
                ܦܢܐ r;

        لكل (;;) {
                 _cleanup_free_ حرف *line = عدم;

                 r = read_line(f, LONG_LINE_MAX, &line);
                 ܐܢ (r < 0)
                         ܦܢܐ r;
                 ܐܢ (r == 0)
                         ܦܢܐ -ENODATA;

                 حرف *l = startswith(line, key);
                 ܐܢ (l && *l == ':') {
                         ܐܢ (ret) {
                                 حرف *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ܐܢ (!s)
                                         ܦܢܐ -ENOMEM;

                                 *ret = s;
                         }

                         ܦܢܐ 0;
                 }
        }
}

DIR* xopendirat(صحيح dir_fd, ثابت حرف *path, صحيح flags) {
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(!(flags & (O_CREAT|O_TMPFILE)));

        ܐܢ ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ܦܢܐ opendir(path);

        ܐܢ (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ܐܢ (fd < 0)
                ܦܢܐ عدم;

        ܦܢܐ take_fdopendir(&fd);
}

صحيح fopen_mode_to_flags(ثابت حرف *mode) {
        ثابت حرف *p;
        صحيح flags;

        أكد(mode);

        ܐܢ ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        ܐܚܪܢܐ ܐܢ ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        ܐܚܪܢܐ ܐܢ ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ܐܚܪܢܐ ܐܢ ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ܐܚܪܢܐ ܐܢ ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ܐܚܪܢܐ ܐܢ ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ܐܚܪܢܐ
                ܦܢܐ -EINVAL;

        لكل (; *p != 0; p++) {

                بدل (*p) {

                حالة 'e':
                        flags |= O_CLOEXEC;
                        اكسر;

                حالة 'x':
                        flags |= O_EXCL;
                        اكسر;

                حالة 'm':
                        /* ignore this here, fdopen() might care later though */
                        اكسر;

                حالة 'c': /* not sure what to do about this one */
                افتراضي:
                        ܦܢܐ -EINVAL;
                }
        }

        ܦܢܐ flags;
}

ساكن صحيح xfopenat_regular(صحيح dir_fd, ثابت حرف *path, ثابت حرف *mode, صحيح open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(mode);
        أكد(ret);

        ܐܢ (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        ܐܚܪܢܐ ܐܢ (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ حرف *j = strjoin("/", path);
                ܐܢ (!j)
                        ܦܢܐ -ENOMEM;

                f = fopen(j, mode);
        } ܐܚܪܢܐ {
                _cleanup_close_ صحيح fd = -EBADF;
                صحيح mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                ܐܢ (mode_flags < 0)
                        ܦܢܐ mode_flags;

                ܐܢ (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        ܐܢ (fd < 0)
                                ܦܢܐ -errno;
                } ܐܚܪܢܐ {
                        ܐܢ (dir_fd == AT_FDCWD)
                                ܦܢܐ -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        ܐܢ (fd < 0)
                                ܦܢܐ fd;
                }

                f = take_fdopen(&fd, mode);
        }
        ܐܢ (!f)
                ܦܢܐ -errno;

        *ret = f;
        ܦܢܐ 0;
}

ساكن صحيح xfopenat_unix_socket(صحيح dir_fd, ثابت حرف *path, ثابت حرف *bind_name, FILE **ret) {
        _cleanup_close_ صحيح sk = -EBADF;
        FILE *f;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ܐܢ (sk < 0)
                ܦܢܐ -errno;

        ܐܢ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                اتحاد sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܐܢ (bind(sk, &bsa.sa, r) < 0)
                        ܦܢܐ -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (shutdown(sk, SHUT_WR) < 0)
                ܦܢܐ -errno;

        f = take_fdopen(&sk, "r");
        ܐܢ (!f)
                ܦܢܐ -errno;

        *ret = f;
        ܦܢܐ 0;
}

صحيح xfopenat_full(
                صحيح dir_fd,
                ثابت حرف *path,
                ثابت حرف *mode,
                صحيح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret) {

        FILE *f = عدم;  /* avoid false maybe-uninitialized warning */
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(mode);
        أكد(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        ܐܢ (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                ܐܢ (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ܦܢܐ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ܦܢܐ 0;
}

صحيح fdopen_independent(صحيح fd, ثابت حرف *mode, FILE **ret) {
        _cleanup_close_ صحيح copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح mode_flags;

        أكد(fd >= 0);
        أكد(mode);
        أكد(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        ܐܢ (mode_flags < 0)
                ܦܢܐ mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        ܐܢ (copy_fd < 0)
                ܦܢܐ copy_fd;

        f = take_fdopen(&copy_fd, mode);
        ܐܢ (!f)
                ܦܢܐ -errno;

        *ret = TAKE_PTR(f);
        ܦܢܐ 0;
}

ساكن صحيح search_and_open_internal(
                ثابت حرف *path,
                صحيح mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ثابت حرف *root,
                حرف **search,
                صحيح *ret_fd,
                حرف **ret_path) {

        صحيح r;

        أكد(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        أكد(path);

        ܐܢ (path_is_absolute(path)) {
                _cleanup_close_ صحيح fd = -EBADF;

                ܐܢ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                ܐܚܪܢܐ
                        r = RET_NERRNO(access(path, mode));
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܐܢ (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                }

                ܐܢ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ܦܢܐ 0;
        }

        ܐܢ (!path_strv_resolve_uniq(search, root))
                ܦܢܐ -ENOMEM;

        STRV_FOREACH(i, search) {
                _cleanup_close_ صحيح fd = -EBADF;
                _cleanup_free_ حرف *p = عدم;

                p = path_join(root, *i, path);
                ܐܢ (!p)
                        ܦܢܐ -ENOMEM;

                ܐܢ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                ܐܚܪܢܐ
                        r = RET_NERRNO(access(p, F_OK));
                ܐܢ (r >= 0) {
                        ܐܢ (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        ܐܢ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ܦܢܐ 0;
                }
                ܐܢ (r != -ENOENT)
                        ܦܢܐ r;
        }

        ܦܢܐ -ENOENT;
}

صحيح search_and_open(
                ثابت حرف *path,
                صحيح mode,
                ثابت حرف *root,
                حرف **search,
                صحيح *ret_fd,
                حرف **ret_path) {

        _cleanup_strv_free_ حرف **copy = عدم;

        أكد(path);

        copy = strv_copy(search);
        ܐܢ (!copy)
                ܦܢܐ -ENOMEM;

        ܦܢܐ search_and_open_internal(path, mode, root, copy, ret_fd, ret_path);
}

ساكن صحيح search_and_fopen_internal(
                ثابت حرف *path,
                ثابت حرف *mode,
                ثابت حرف *root,
                حرف **search,
                FILE **ret_file,
                حرف **ret_path) {

        _cleanup_free_ حرف *found_path = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(path);
        أكد(mode || !ret_file);

        r = search_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        root,
                        search,
                        ret_file ? &fd : عدم,
                        ret_path ? &found_path : عدم);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                ܐܢ (!f)
                        ܦܢܐ -errno;

                *ret_file = f;
        }

        ܐܢ (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ܦܢܐ 0;
}

صحيح search_and_fopen(
                ثابت حرف *path,
                ثابت حرف *mode,
                ثابت حرف *root,
                ثابت حرف **search,
                FILE **ret_file,
                حرف **ret_path) {

        _cleanup_strv_free_ حرف **copy = عدم;

        أكد(path);
        أكد(mode || !ret_file);

        copy = strv_copy((حرف**) search);
        ܐܢ (!copy)
                ܦܢܐ -ENOMEM;

        ܦܢܐ search_and_fopen_internal(path, mode, root, copy, ret_file, ret_path);
}

صحيح search_and_fopen_nulstr(
                ثابت حرف *path,
                ثابت حرف *mode,
                ثابت حرف *root,
                ثابت حرف *search,
                FILE **ret_file,
                حرف **ret_path) {

        _cleanup_strv_free_ حرف **l = عدم;

        أكد(path);
        أكد(mode || !ret_file);

        l = strv_split_nulstr(search);
        ܐܢ (!l)
                ܦܢܐ -ENOMEM;

        ܦܢܐ search_and_fopen_internal(path, mode, root, l, ret_file, ret_path);
}

صحيح fflush_and_check(FILE *f) {
        أكد(f);

        errno = 0;
        fflush(f);

        ܐܢ (ferror(f))
                ܦܢܐ errno_or_else(EIO);

        ܦܢܐ 0;
}

صحيح fflush_sync_and_check(FILE *f) {
        صحيح r, fd;

        أكد(f);

        r = fflush_and_check(f);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ܐܢ (fd < 0)
                ܦܢܐ 0;

        r = fsync_full(fd);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ 0;
}

صحيح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n) {
        حرف ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ܐܢ (!timestamp_is_set(n))
                ܦܢܐ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ܦܢܐ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

صحيح read_timestamp_file(ثابت حرف *fn, usec_t *ret) {
        _cleanup_free_ حرف *ln = عدم;
        uint64_t t;
        صحيح r;

        أكد(ret);

        r = read_one_line_file(fn, &ln);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = safe_atou64(ln, &t);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (!timestamp_is_set(t))
                ܦܢܐ -ERANGE;

        *ret = (usec_t) t;
        ܦܢܐ 0;
}

صحيح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space) {
        أكد(s);
        أكد(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ܐܢ (!f)
                f = stdout;

        ܐܢ (!separator)
                separator = " ";

        ܐܢ (*space)
                ܐܢ (fputs(separator, f) < 0)
                        ܦܢܐ -EIO;

        *space = true;

        ܐܢ (fputs(s, f) < 0)
                ܦܢܐ -EIO;

        ܦܢܐ 0;
}

صحيح fputs_with_newline(FILE *f, ثابت حرف *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ܐܢ (isempty(s))
                ܦܢܐ 0;

        ܐܢ (!f)
                f = stdout;

        ܐܢ (fputs(s, f) < 0)
                ܦܢܐ -EIO;

        ܐܢ (endswith(s, "\n"))
                ܦܢܐ 0;

        ܐܢ (fputc('\n', f) < 0)
                ܦܢܐ -EIO;

        ܦܢܐ 1;
}

/* A bitmask of the EOL markers we know */
تعريفنوع تعداد EndOfLineMarker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} EndOfLineMarker;

ساكن EndOfLineMarker categorize_eol(حرف c, ReadLineFlags flags) {

        ܐܢ (!FLAGS_SET(flags, READ_LINE_ONLY_NUL)) {
                ܐܢ (c == '\n')
                        ܦܢܐ EOL_TEN;
                ܐܢ (c == '\r')
                        ܦܢܐ EOL_THIRTEEN;
        }

        ܐܢ (c == '\0')
                ܦܢܐ EOL_ZERO;

        ܦܢܐ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, عدم);

صحيح read_line_full(FILE *f, size_t limit, ReadLineFlags flags, حرف **ret) {
        _cleanup_free_ حرف *buffer = عدم;
        size_t n = 0, count = 0;
        صحيح r;

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

        ܐܢ (ret) {
                ܐܢ (!GREEDY_REALLOC(buffer, 1))
                        ܦܢܐ -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                EndOfLineMarker previous_eol = EOL_NONE;
                flockfile(f);

                لكل (;;) {
                        EndOfLineMarker eol;
                        حرف c;

                        ܐܢ (n >= limit)
                                ܦܢܐ -ENOBUFS;

                        ܐܢ (count >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ܦܢܐ -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r == 0) /* EOF is definitely EOL */
                                اكسر;

                        eol = categorize_eol(c, flags);

                        ܐܢ (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned حرف) c, f) != EOF);
                                اكسر;
                        }

                        count++;

                        ܐܢ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ܐܢ ((flags & (READ_LINE_IS_A_TTY|READ_LINE_NOT_A_TTY)) == 0) {
                                        صحيح fd;

                                        fd = fileno(f);
                                        ܐܢ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_LINE_NOT_A_TTY;
                                        ܐܚܪܢܐ
                                                flags |= isatty_safe(fd) ? READ_LINE_IS_A_TTY : READ_LINE_NOT_A_TTY;
                                }
                                ܐܢ (FLAGS_SET(flags, READ_LINE_IS_A_TTY))
                                        اكسر;
                        }

                        ܐܢ (eol != EOL_NONE) {
                                previous_eol |= eol;
                                استمر;
                        }

                        ܐܢ (ret) {
                                ܐܢ (!GREEDY_REALLOC(buffer, n + 2))
                                        ܦܢܐ -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        ܐܢ (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        ܦܢܐ (صحيح) count;
}

صحيح read_stripped_line(FILE *f, size_t limit, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r, k;

        أكد(f);

        r = read_line(f, limit, ret ? &s : عدم);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ret) {
                ثابت حرف *p = strstrip(s);
                ܐܢ (p == s)
                        *ret = TAKE_PTR(s);
                ܐܚܪܢܐ {
                        k = strdup_to(ret, p);
                        ܐܢ (k < 0)
                                ܦܢܐ k;
                }
        }

        ܦܢܐ r > 0;          /* Return 1 if something was read. */
}

صحيح safe_fgetc(FILE *f, حرف *ret) {
        صحيح k;

        أكد(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ܐܢ (k == EOF) {
                ܐܢ (ferror(f))
                        ܦܢܐ errno_or_else(EIO);

                ܐܢ (ret)
                        *ret = 0;

                ܦܢܐ 0;
        }

        ܐܢ (ret)
                *ret = k;

        ܦܢܐ 1;
}

صحيح warn_file_is_world_accessible(ثابت حرف *filename, بنية stat *st, ثابت حرف *unit, unsigned line) {
        بنية stat _st;

        ܐܢ (!filename)
                ܦܢܐ 0;

        ܐܢ (!st) {
                ܐܢ (stat(filename, &_st) < 0)
                        ܦܢܐ -errno;
                st = &_st;
        }

        ܐܢ ((st->st_mode & S_IRWXO) == 0)
                ܦܢܐ 0;

        ܐܢ (unit)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ܐܚܪܢܐ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ܦܢܐ 0;
}

صحيح write_data_file_atomic_at(
                صحيح dir_fd,
                ثابت حرف *path,
                ثابت بنية iovec *iovec,
                WriteDataFileFlags flags) {

        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ حرف *dn = عدم, *fn = عدم;
        r = path_split_prefix_filename(path, &dn, &fn);
        ܐܢ (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ܦܢܐ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ܐܢ (r < 0)
                ܦܢܐ r;

        _cleanup_close_ صحيح mfd = -EBADF;
        ܐܢ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ عدم,
                            &mfd);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                dir_fd = mfd;
        }

        _cleanup_free_ حرف *t = عدم;
        _cleanup_close_ صحيح fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ܐܢ (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ܐܢ (r < 0)
                ܦܢܐ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ܦܢܐ 0;
}
