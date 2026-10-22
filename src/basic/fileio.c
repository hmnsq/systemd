/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "hexdecoct.h"
#include "io-util.h"
#include "iovec-util.h"
#include "label-util.h"
#include "log.h"
#include "mkdir.h"
#include "nulstr-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "socket-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"
#include "strv.h"
#include "sync-util.h"
#include "terminal-util.h"
#include "time-util.h"
#include "tmpfile-util.h"

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

số_nguyên fdopen_unlocked(số_nguyên fd, const char *options, FILE **ret) {
        assert(ret);

        FILE *f = fdopen(fd, options);
        nếu (!f)
                trả_về -errno;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        trả_về 0;
}

số_nguyên take_fdopen_unlocked(số_nguyên *fd, const char *options, FILE **ret) {
        số_nguyên r;

        assert(fd);

        r = fdopen_unlocked(*fd, options, ret);
        nếu (r < 0)
                trả_về r;

        *fd = -EBADF;

        trả_về 0;
}

FILE* take_fdopen(số_nguyên *fd, const char *options) {
        assert(fd);

        FILE *f = fdopen(*fd, options);
        nếu (!f)
                trả_về NULL;

        *fd = -EBADF;

        trả_về f;
}

DIR* take_fdopendir(số_nguyên *dfd) {
        assert(dfd);

        DIR *d = fdopendir(*dfd);
        nếu (!d)
                trả_về NULL;

        *dfd = -EBADF;

        trả_về d;
}

FILE* open_memstream_unlocked(char **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        nếu (!f)
                trả_về NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        trả_về f;
}

FILE* fmemopen_unlocked(void *buf, size_t size, const char *mode) {
        FILE *f = fmemopen(buf, size, mode);
        nếu (!f)
                trả_về NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        trả_về f;
}

số_nguyên write_string_stream_full(
                FILE *f,
                const char *line,
                WriteStringFileFlags flags,
                const struct timespec *ts) {

        bool needs_nl;
        số_nguyên r, fd = -EBADF;

        assert(f);
        assert(line);

        nếu (ferror(f))
                trả_về -EIO;

        nếu (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                nếu (fd < 0)
                        trả_về -EBADF;
        }

        nếu (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ char *t = NULL;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                nếu (fd < 0) {
                        fd = fileno(f);
                        nếu (fd < 0)
                                trả_về -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                nếu (read_virtual_file_fd(fd, strlen(line)+1, &t, NULL) > 0 &&
                    streq_skip_trailing_chars(line, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", line);
                        trả_về 0;
                }

                nếu (lseek(fd, 0, SEEK_SET) < 0)
                        trả_về -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(line, "\n");

        nếu (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                line = strjoina(line, "\n");
                needs_nl = false;
        }

        nếu (fputs(line, f) == EOF)
                trả_về -errno;

        nếu (needs_nl)
                nếu (fputc('\n', f) == EOF)
                        trả_về -errno;

        nếu (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        không_thì
                r = fflush_and_check(f);
        nếu (r < 0)
                trả_về r;

        nếu (ts) {
                const struct timespec twice[2] = {*ts, *ts};

                assert(fd >= 0);
                nếu (futimens(fd, twice) < 0)
                        trả_về -errno;
        }

        trả_về 0;
}

static mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        trả_về FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

static số_nguyên write_string_file_atomic_at(
                số_nguyên dir_fd,
                const char *fn,
                const char *line,
                WriteStringFileFlags flags,
                const struct timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *p = NULL;
        số_nguyên r;

        assert(fn);
        assert(line);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        nếu (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                nếu (r < 0)
                        trả_về r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        số_nguyên k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? NULL : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        nếu (r < 0)
                trả_về r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        nếu (k < 0)
                trả_về k;

        r = write_string_stream_full(f, line, flags, ts);
        nếu (r < 0)
                trả_về r;

        r = fchmod_umask(fileno(f), mode);
        nếu (r < 0)
                trả_về r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        nếu (r < 0)
                trả_về r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        nếu (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                nếu (r < 0)
                        trả_về r;
        }

        trả_về 0;
}

số_nguyên write_string_file_full_label(
                số_nguyên dir_fd,
                const char *fn,
                const char *line,
                WriteStringFileFlags flags,
                const struct timespec *ts,
                const char *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ số_nguyên fd = -EBADF;
        số_nguyên r;

        assert(dir_fd == AT_FDCWD || dir_fd >= 0);
        assert(line);

        /* We don't know how to verify whether the file contents was already on-disk. */
        assert(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        nếu (flags & WRITE_STRING_FILE_MKDIR_0755) {
                assert(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (flags & WRITE_STRING_FILE_ATOMIC) {
                assert(fn);
                assert(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, line, flags, ts, label_context);
                nếu (r < 0)
                        goto fail;

                trả_về r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        nếu (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        không_thì {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                nếu (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        nếu (r < 0)
                                goto fail;

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
                nếu (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : fn, made_file, label_context));
        }
        nếu (r < 0)
                goto fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        nếu (r < 0)
                goto fail;

        nếu (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, NULL, _IONBF, 0);

        r = write_string_stream_full(f, line, flags, ts);
        nếu (r < 0)
                goto fail;

        trả_về 0;

fail:
        nếu (made_file)
                (void) unlinkat(dir_fd, fn, 0);

        nếu (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                trả_về r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        nếu (verify_file_at(dir_fd, fn, line, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                trả_về 0;

        trả_về r;
}

số_nguyên write_string_filef(
                const char *fn,
                WriteStringFileFlags flags,
                const char *format, ...) {

        _cleanup_free_ char *p = NULL;
        va_list ap;
        số_nguyên r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về -ENOMEM;

        trả_về write_string_file(fn, p, flags);
}

số_nguyên write_string_filef_at(
                số_nguyên dir_fd,
                const char *fn,
                WriteStringFileFlags flags,
                const char *format, ...) {

        _cleanup_free_ char *p = NULL;
        va_list ap;
        số_nguyên r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về -ENOMEM;

        trả_về write_string_file_at(dir_fd, fn, p, flags);
}

số_nguyên write_base64_file_at(
                số_nguyên dir_fd,
                const char *fn,
                const struct iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ char *encoded = NULL;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : NULL, data ? data->iov_len : 0, 79, &encoded);
        nếu (n < 0)
                trả_về n;

        trả_về write_string_file_at(dir_fd, fn, encoded, flags);
}

số_nguyên read_one_line_file_at(số_nguyên dir_fd, const char *filename, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        số_nguyên r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);
        assert(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        nếu (r < 0)
                trả_về r;

        trả_về read_line(f, LONG_LINE_MAX, ret);
}

số_nguyên read_boolean_file_at(số_nguyên dir_fd, const char *filename) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);

        r = read_one_line_file_at(dir_fd, filename, &s);
        nếu (r < 0)
                trả_về r;

        trả_về parse_boolean(s);
}

số_nguyên verify_file_at(số_nguyên dir_fd, const char *fn, const char *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *buf = NULL;
        size_t l, k;
        số_nguyên r;

        assert(blob);

        l = strlen(blob);

        nếu (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        nếu (!buf)
                trả_về -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        nếu (r < 0)
                trả_về r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        nếu (ferror(f))
                trả_về errno_or_else(EIO);

        nếu (k != l && k != l + accept_extra_nl)
                trả_về 0;
        nếu (memcmp(buf, blob, l) != 0)
                trả_về 0;
        nếu (k > l && buf[l] != '\n')
                trả_về 0;

        trả_về 1;
}

số_nguyên read_virtual_file_at(
                số_nguyên dir_fd,
                const char *filename,
                size_t max_size,
                char **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ char *buf = NULL;
        size_t n, size;
        số_nguyên n_retries;
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

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ số_nguyên fd = -EBADF;
        nếu (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        không_thì
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        nếu (fd < 0)
                trả_về fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        cho (;;) {
                struct stat st;

                nếu (fstat(fd, &st) < 0)
                        trả_về -errno;

                nếu (!S_ISREG(st.st_mode))
                        trả_về -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                nếu (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        nếu (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                nếu (max_size == SIZE_MAX)
                                        trả_về -EFBIG;

                                size = max_size;
                        } không_thì {
                                size = MIN((size_t) st.st_size, max_size);

                                nếu (size > READ_VIRTUAL_BYTES_MAX)
                                        trả_về -EFBIG;
                        }

                        n_retries--;
                } không_thì nếu (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } không_thì {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                nếu (!buf)
                        trả_về -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                cho (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = read(fd, buf, size + 1);
                        nếu (k >= 0) {
                                n = k;
                                break;
                        }

                        nếu (errno != EINTR)
                                trả_về -errno;
                }

                /* Consider a short read as EOF */
                nếu (n <= size)
                        break;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                nếu (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        break;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                nếu (n_retries <= 0)
                        trả_về -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                nếu (lseek(fd, 0, SEEK_SET) < 0)
                        trả_về -errno;

                buf = mfree(buf);
        }

        nếu (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                nếu (!ret_size && memchr(buf, 0, n))
                        trả_về -EBADMSG;

                nếu (n < size) {
                        char *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        nếu (!p)
                                trả_về -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        nếu (ret_size)
                *ret_size = n;

        trả_về !truncated;
}

số_nguyên read_full_stream_full(
                FILE *f,
                const char *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                char **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ char *buf = NULL;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        số_nguyên fd, r;

        assert(f);
        assert(ret_contents);
        assert(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        assert(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        nếu (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                trả_về -ERANGE;

        nếu ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                nếu (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                không_thì
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        nếu (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                struct stat st;

                nếu (fstat(fd, &st) < 0)
                        trả_về -errno;

                nếu (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        nếu (r < 0)
                                trả_về r;
                }

                nếu (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        nếu (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                nếu (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        nếu (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (void) warn_file_is_world_accessible(filename, &st, NULL, 0);
                }
        } không_thì nếu (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                trả_về -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        nếu (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        nếu (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        nếu (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                trả_về -errno;

        n = l = 0;
        cho (;;) {
                char *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                nếu (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                nếu (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        nếu (!t) {
                                r = -ENOMEM;
                                goto finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        free(buf);
                } không_thì {
                        t = realloc(buf, n_next + 1);
                        nếu (!t)
                                trả_về -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                assert(k <= n - l);
                l += k;

                nếu (ferror(f)) {
                        r = errno_or_else(EIO);
                        goto finalize;
                }
                nếu (feof(f))
                        break;

                nếu (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        assert(l == size);
                        break;
                }

                assert(k > 0); /* we can't have read zero bytes because that would have been EOF */

                nếu (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        goto finalize;
                }

                nếu (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        goto finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        nếu (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ void *decoded = NULL;
                size_t decoded_size;

                buf[l++] = 0;
                nếu (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                không_thì
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                nếu (r < 0)
                        goto finalize;

                nếu (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                nếu (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        goto finalize;
                }
        }

        nếu (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                nếu (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        goto finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        nếu (ret_size)
                *ret_size = l;

        trả_về 0;

finalize:
        nếu (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        trả_về r;
}

số_nguyên read_full_file_full(
                số_nguyên dir_fd,
                const char *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                const char *bind_name,
                char **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = NULL;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        số_nguyên r;

        assert(ret_contents);

        nếu (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        nếu (r < 0)
                trả_về r;

        trả_về read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

số_nguyên script_get_shebang_interpreter(const char *path, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        số_nguyên r;

        assert(path);

        f = fopen(path, "re");
        nếu (!f)
                trả_về -errno;

        char c;
        r = safe_fgetc(f, &c);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EBADMSG;
        nếu (c != '#')
                trả_về -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EBADMSG;
        nếu (c != '!')
                trả_về -EMEDIUMTYPE;

        _cleanup_free_ char *line = NULL;
        r = read_line(f, LONG_LINE_MAX, &line);
        nếu (r < 0)
                trả_về r;

        _cleanup_free_ char *p = NULL;
        const char *s = line;

        r = extract_first_word(&s, &p, /* separators= */ NULL, /* flags= */ 0);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -ENOEXEC;

        nếu (ret)
                *ret = TAKE_PTR(p);
        trả_về 0;
}

số_nguyên get_proc_field(const char *path, const char *key, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        số_nguyên r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        assert(path);
        assert(key);

        r = fopen_unlocked(path, "re", &f);
        nếu (r == -ENOENT && proc_mounted() == 0)
                trả_về -ENOSYS;
        nếu (r < 0)
                trả_về r;

        cho (;;) {
                 _cleanup_free_ char *line = NULL;

                 r = read_line(f, LONG_LINE_MAX, &line);
                 nếu (r < 0)
                         trả_về r;
                 nếu (r == 0)
                         trả_về -ENODATA;

                 char *l = startswith(line, key);
                 nếu (l && *l == ':') {
                         nếu (ret) {
                                 char *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 nếu (!s)
                                         trả_về -ENOMEM;

                                 *ret = s;
                         }

                         trả_về 0;
                 }
        }
}

DIR* xopendirat(số_nguyên dir_fd, const char *path, số_nguyên flags) {
        _cleanup_close_ số_nguyên fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(!(flags & (O_CREAT|O_TMPFILE)));

        nếu ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                trả_về opendir(path);

        nếu (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        nếu (fd < 0)
                trả_về NULL;

        trả_về take_fdopendir(&fd);
}

số_nguyên fopen_mode_to_flags(const char *mode) {
        const char *p;
        số_nguyên flags;

        assert(mode);

        nếu ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        không_thì nếu ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        không_thì nếu ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        không_thì nếu ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        không_thì nếu ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        không_thì nếu ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        không_thì
                trả_về -EINVAL;

        cho (; *p != 0; p++) {

                switch (*p) {

                case 'e':
                        flags |= O_CLOEXEC;
                        break;

                case 'x':
                        flags |= O_EXCL;
                        break;

                case 'm':
                        /* ignore this here, fdopen() might care later though */
                        break;

                case 'c': /* not sure what to do about this one */
                default:
                        trả_về -EINVAL;
                }
        }

        trả_về flags;
}

static số_nguyên xfopenat_regular(số_nguyên dir_fd, const char *path, const char *mode, số_nguyên open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        nếu (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        không_thì nếu (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ char *j = strjoin("/", path);
                nếu (!j)
                        trả_về -ENOMEM;

                f = fopen(j, mode);
        } không_thì {
                _cleanup_close_ số_nguyên fd = -EBADF;
                số_nguyên mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                nếu (mode_flags < 0)
                        trả_về mode_flags;

                nếu (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        nếu (fd < 0)
                                trả_về -errno;
                } không_thì {
                        nếu (dir_fd == AT_FDCWD)
                                trả_về -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        nếu (fd < 0)
                                trả_về fd;
                }

                f = take_fdopen(&fd, mode);
        }
        nếu (!f)
                trả_về -errno;

        *ret = f;
        trả_về 0;
}

static số_nguyên xfopenat_unix_socket(số_nguyên dir_fd, const char *path, const char *bind_name, FILE **ret) {
        _cleanup_close_ số_nguyên sk = -EBADF;
        FILE *f;
        số_nguyên r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        nếu (sk < 0)
                trả_về -errno;

        nếu (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                union sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                nếu (r < 0)
                        trả_về r;

                nếu (bind(sk, &bsa.sa, r) < 0)
                        trả_về -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        nếu (r < 0)
                trả_về r;

        nếu (shutdown(sk, SHUT_WR) < 0)
                trả_về -errno;

        f = take_fdopen(&sk, "r");
        nếu (!f)
                trả_về -errno;

        *ret = f;
        trả_về 0;
}

số_nguyên xfopenat_full(
                số_nguyên dir_fd,
                const char *path,
                const char *mode,
                số_nguyên open_flags,
                XfopenFlags flags,
                const char *bind_name,
                FILE **ret) {

        FILE *f = NULL;  /* avoid false maybe-uninitialized warning */
        số_nguyên r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        nếu (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                nếu (IN_SET(r, -ENOTSOCK, -EINVAL))
                        trả_về -ENXIO; /* propagate original error if this is not a socket after all */
        }
        nếu (r < 0)
                trả_về r;

        nếu (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        trả_về 0;
}

số_nguyên fdopen_independent(số_nguyên fd, const char *mode, FILE **ret) {
        _cleanup_close_ số_nguyên copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = NULL;
        số_nguyên mode_flags;

        assert(fd >= 0);
        assert(mode);
        assert(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        nếu (mode_flags < 0)
                trả_về mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        nếu (copy_fd < 0)
                trả_về copy_fd;

        f = take_fdopen(&copy_fd, mode);
        nếu (!f)
                trả_về -errno;

        *ret = TAKE_PTR(f);
        trả_về 0;
}

static số_nguyên search_and_open_internal(
                const char *path,
                số_nguyên mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                const char *root,
                char **search,
                số_nguyên *ret_fd,
                char **ret_path) {

        số_nguyên r;

        assert(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        assert(path);

        nếu (path_is_absolute(path)) {
                _cleanup_close_ số_nguyên fd = -EBADF;

                nếu (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                không_thì
                        r = RET_NERRNO(access(path, mode));
                nếu (r < 0)
                        trả_về r;

                nếu (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        nếu (r < 0)
                                trả_về r;
                }

                nếu (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                trả_về 0;
        }

        nếu (!path_strv_resolve_uniq(search, root))
                trả_về -ENOMEM;

        STRV_FOREACH(i, search) {
                _cleanup_close_ số_nguyên fd = -EBADF;
                _cleanup_free_ char *p = NULL;

                p = path_join(root, *i, path);
                nếu (!p)
                        trả_về -ENOMEM;

                nếu (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                không_thì
                        r = RET_NERRNO(access(p, F_OK));
                nếu (r >= 0) {
                        nếu (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        nếu (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        trả_về 0;
                }
                nếu (r != -ENOENT)
                        trả_về r;
        }

        trả_về -ENOENT;
}

số_nguyên search_and_open(
                const char *path,
                số_nguyên mode,
                const char *root,
                char **search,
                số_nguyên *ret_fd,
                char **ret_path) {

        _cleanup_strv_free_ char **copy = NULL;

        assert(path);

        copy = strv_copy(search);
        nếu (!copy)
                trả_về -ENOMEM;

        trả_về search_and_open_internal(path, mode, root, copy, ret_fd, ret_path);
}

static số_nguyên search_and_fopen_internal(
                const char *path,
                const char *mode,
                const char *root,
                char **search,
                FILE **ret_file,
                char **ret_path) {

        _cleanup_free_ char *found_path = NULL;
        _cleanup_close_ số_nguyên fd = -EBADF;
        số_nguyên r;

        assert(path);
        assert(mode || !ret_file);

        r = search_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        root,
                        search,
                        ret_file ? &fd : NULL,
                        ret_path ? &found_path : NULL);
        nếu (r < 0)
                trả_về r;

        nếu (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                nếu (!f)
                        trả_về -errno;

                *ret_file = f;
        }

        nếu (ret_path)
                *ret_path = TAKE_PTR(found_path);

        trả_về 0;
}

số_nguyên search_and_fopen(
                const char *path,
                const char *mode,
                const char *root,
                const char **search,
                FILE **ret_file,
                char **ret_path) {

        _cleanup_strv_free_ char **copy = NULL;

        assert(path);
        assert(mode || !ret_file);

        copy = strv_copy((char**) search);
        nếu (!copy)
                trả_về -ENOMEM;

        trả_về search_and_fopen_internal(path, mode, root, copy, ret_file, ret_path);
}

số_nguyên search_and_fopen_nulstr(
                const char *path,
                const char *mode,
                const char *root,
                const char *search,
                FILE **ret_file,
                char **ret_path) {

        _cleanup_strv_free_ char **l = NULL;

        assert(path);
        assert(mode || !ret_file);

        l = strv_split_nulstr(search);
        nếu (!l)
                trả_về -ENOMEM;

        trả_về search_and_fopen_internal(path, mode, root, l, ret_file, ret_path);
}

số_nguyên fflush_and_check(FILE *f) {
        assert(f);

        errno = 0;
        fflush(f);

        nếu (ferror(f))
                trả_về errno_or_else(EIO);

        trả_về 0;
}

số_nguyên fflush_sync_and_check(FILE *f) {
        số_nguyên r, fd;

        assert(f);

        r = fflush_and_check(f);
        nếu (r < 0)
                trả_về r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        nếu (fd < 0)
                trả_về 0;

        r = fsync_full(fd);
        nếu (r < 0)
                trả_về r;

        trả_về 0;
}

số_nguyên write_timestamp_file_atomic(const char *fn, usec_t n) {
        char ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        nếu (!timestamp_is_set(n))
                trả_về -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        trả_về write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

số_nguyên read_timestamp_file(const char *fn, usec_t *ret) {
        _cleanup_free_ char *ln = NULL;
        uint64_t t;
        số_nguyên r;

        assert(ret);

        r = read_one_line_file(fn, &ln);
        nếu (r < 0)
                trả_về r;

        r = safe_atou64(ln, &t);
        nếu (r < 0)
                trả_về r;

        nếu (!timestamp_is_set(t))
                trả_về -ERANGE;

        *ret = (usec_t) t;
        trả_về 0;
}

số_nguyên fputs_with_separator(FILE *f, const char *s, const char *separator, bool *space) {
        assert(s);
        assert(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        nếu (!f)
                f = stdout;

        nếu (!separator)
                separator = " ";

        nếu (*space)
                nếu (fputs(separator, f) < 0)
                        trả_về -EIO;

        *space = true;

        nếu (fputs(s, f) < 0)
                trả_về -EIO;

        trả_về 0;
}

số_nguyên fputs_with_newline(FILE *f, const char *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        nếu (isempty(s))
                trả_về 0;

        nếu (!f)
                f = stdout;

        nếu (fputs(s, f) < 0)
                trả_về -EIO;

        nếu (endswith(s, "\n"))
                trả_về 0;

        nếu (fputc('\n', f) < 0)
                trả_về -EIO;

        trả_về 1;
}

/* A bitmask of the EOL markers we know */
typedef enum EndOfLineMarker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} EndOfLineMarker;

static EndOfLineMarker categorize_eol(char c, ReadLineFlags flags) {

        nếu (!FLAGS_SET(flags, READ_LINE_ONLY_NUL)) {
                nếu (c == '\n')
                        trả_về EOL_TEN;
                nếu (c == '\r')
                        trả_về EOL_THIRTEEN;
        }

        nếu (c == '\0')
                trả_về EOL_ZERO;

        trả_về EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, NULL);

số_nguyên read_line_full(FILE *f, size_t limit, ReadLineFlags flags, char **ret) {
        _cleanup_free_ char *buffer = NULL;
        size_t n = 0, count = 0;
        số_nguyên r;

        assert(f);

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

        nếu (ret) {
                nếu (!GREEDY_REALLOC(buffer, 1))
                        trả_về -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                EndOfLineMarker previous_eol = EOL_NONE;
                flockfile(f);

                cho (;;) {
                        EndOfLineMarker eol;
                        char c;

                        nếu (n >= limit)
                                trả_về -ENOBUFS;

                        nếu (count >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                trả_về -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        nếu (r < 0)
                                trả_về r;
                        nếu (r == 0) /* EOF is definitely EOL */
                                break;

                        eol = categorize_eol(c, flags);

                        nếu (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned char) c, f) != EOF);
                                break;
                        }

                        count++;

                        nếu (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                nếu ((flags & (READ_LINE_IS_A_TTY|READ_LINE_NOT_A_TTY)) == 0) {
                                        số_nguyên fd;

                                        fd = fileno(f);
                                        nếu (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_LINE_NOT_A_TTY;
                                        không_thì
                                                flags |= isatty_safe(fd) ? READ_LINE_IS_A_TTY : READ_LINE_NOT_A_TTY;
                                }
                                nếu (FLAGS_SET(flags, READ_LINE_IS_A_TTY))
                                        break;
                        }

                        nếu (eol != EOL_NONE) {
                                previous_eol |= eol;
                                continue;
                        }

                        nếu (ret) {
                                nếu (!GREEDY_REALLOC(buffer, n + 2))
                                        trả_về -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        nếu (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        trả_về (số_nguyên) count;
}

số_nguyên read_stripped_line(FILE *f, size_t limit, char **ret) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r, k;

        assert(f);

        r = read_line(f, limit, ret ? &s : NULL);
        nếu (r < 0)
                trả_về r;

        nếu (ret) {
                const char *p = strstrip(s);
                nếu (p == s)
                        *ret = TAKE_PTR(s);
                không_thì {
                        k = strdup_to(ret, p);
                        nếu (k < 0)
                                trả_về k;
                }
        }

        trả_về r > 0;          /* Return 1 if something was read. */
}

số_nguyên safe_fgetc(FILE *f, char *ret) {
        số_nguyên k;

        assert(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        nếu (k == EOF) {
                nếu (ferror(f))
                        trả_về errno_or_else(EIO);

                nếu (ret)
                        *ret = 0;

                trả_về 0;
        }

        nếu (ret)
                *ret = k;

        trả_về 1;
}

số_nguyên warn_file_is_world_accessible(const char *filename, struct stat *st, const char *unit, unsigned line) {
        struct stat _st;

        nếu (!filename)
                trả_về 0;

        nếu (!st) {
                nếu (stat(filename, &_st) < 0)
                        trả_về -errno;
                st = &_st;
        }

        nếu ((st->st_mode & S_IRWXO) == 0)
                trả_về 0;

        nếu (unit)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        không_thì
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        trả_về 0;
}

số_nguyên write_data_file_atomic_at(
                số_nguyên dir_fd,
                const char *path,
                const struct iovec *iovec,
                WriteDataFileFlags flags) {

        số_nguyên r;

        assert(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ char *dn = NULL, *fn = NULL;
        r = path_split_prefix_filename(path, &dn, &fn);
        nếu (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                trả_về -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        nếu (r < 0)
                trả_về r;

        _cleanup_close_ số_nguyên mfd = -EBADF;
        nếu (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ NULL,
                            &mfd);
                nếu (r < 0)
                        trả_về r;

                dir_fd = mfd;
        }

        _cleanup_free_ char *t = NULL;
        _cleanup_close_ số_nguyên fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        nếu (fd < 0)
                trả_về fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        nếu (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                nếu (r < 0)
                        trả_về r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        nếu (r < 0)
                trả_về r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        nếu (r < 0)
                trả_về r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        trả_về 0;
}
