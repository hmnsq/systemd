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

整数 fdopen_unlocked(整数 fd, const char *options, FILE **ret) {
        assert(ret);

        FILE *f = fdopen(fd, options);
        もし (!f)
                戻る -errno;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        戻る 0;
}

整数 take_fdopen_unlocked(整数 *fd, const char *options, FILE **ret) {
        整数 r;

        assert(fd);

        r = fdopen_unlocked(*fd, options, ret);
        もし (r < 0)
                戻る r;

        *fd = -EBADF;

        戻る 0;
}

FILE* take_fdopen(整数 *fd, const char *options) {
        assert(fd);

        FILE *f = fdopen(*fd, options);
        もし (!f)
                戻る NULL;

        *fd = -EBADF;

        戻る f;
}

DIR* take_fdopendir(整数 *dfd) {
        assert(dfd);

        DIR *d = fdopendir(*dfd);
        もし (!d)
                戻る NULL;

        *dfd = -EBADF;

        戻る d;
}

FILE* open_memstream_unlocked(char **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        もし (!f)
                戻る NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        戻る f;
}

FILE* fmemopen_unlocked(void *buf, size_t size, const char *mode) {
        FILE *f = fmemopen(buf, size, mode);
        もし (!f)
                戻る NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        戻る f;
}

整数 write_string_stream_full(
                FILE *f,
                const char *line,
                WriteStringFileFlags flags,
                const struct timespec *ts) {

        bool needs_nl;
        整数 r, fd = -EBADF;

        assert(f);
        assert(line);

        もし (ferror(f))
                戻る -EIO;

        もし (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                もし (fd < 0)
                        戻る -EBADF;
        }

        もし (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ char *t = NULL;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                もし (fd < 0) {
                        fd = fileno(f);
                        もし (fd < 0)
                                戻る -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                もし (read_virtual_file_fd(fd, strlen(line)+1, &t, NULL) > 0 &&
                    streq_skip_trailing_chars(line, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", line);
                        戻る 0;
                }

                もし (lseek(fd, 0, SEEK_SET) < 0)
                        戻る -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(line, "\n");

        もし (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                line = strjoina(line, "\n");
                needs_nl = false;
        }

        もし (fputs(line, f) == EOF)
                戻る -errno;

        もし (needs_nl)
                もし (fputc('\n', f) == EOF)
                        戻る -errno;

        もし (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        否则
                r = fflush_and_check(f);
        もし (r < 0)
                戻る r;

        もし (ts) {
                const struct timespec twice[2] = {*ts, *ts};

                assert(fd >= 0);
                もし (futimens(fd, twice) < 0)
                        戻る -errno;
        }

        戻る 0;
}

static mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        戻る FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

static 整数 write_string_file_atomic_at(
                整数 dir_fd,
                const char *fn,
                const char *line,
                WriteStringFileFlags flags,
                const struct timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *p = NULL;
        整数 r;

        assert(fn);
        assert(line);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        もし (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                もし (r < 0)
                        戻る r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        整数 k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? NULL : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        もし (r < 0)
                戻る r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        もし (k < 0)
                戻る k;

        r = write_string_stream_full(f, line, flags, ts);
        もし (r < 0)
                戻る r;

        r = fchmod_umask(fileno(f), mode);
        もし (r < 0)
                戻る r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        もし (r < 0)
                戻る r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        もし (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                もし (r < 0)
                        戻る r;
        }

        戻る 0;
}

整数 write_string_file_full_label(
                整数 dir_fd,
                const char *fn,
                const char *line,
                WriteStringFileFlags flags,
                const struct timespec *ts,
                const char *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        assert(dir_fd == AT_FDCWD || dir_fd >= 0);
        assert(line);

        /* We don't know how to verify whether the file contents was already on-disk. */
        assert(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        もし (flags & WRITE_STRING_FILE_MKDIR_0755) {
                assert(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                もし (r < 0)
                        戻る r;
        }

        もし (flags & WRITE_STRING_FILE_ATOMIC) {
                assert(fn);
                assert(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, line, flags, ts, label_context);
                もし (r < 0)
                        goto fail;

                戻る r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        もし (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        否则 {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                もし (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        もし (r < 0)
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
                もし (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : fn, made_file, label_context));
        }
        もし (r < 0)
                goto fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        もし (r < 0)
                goto fail;

        もし (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, NULL, _IONBF, 0);

        r = write_string_stream_full(f, line, flags, ts);
        もし (r < 0)
                goto fail;

        戻る 0;

fail:
        もし (made_file)
                (void) unlinkat(dir_fd, fn, 0);

        もし (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                戻る r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        もし (verify_file_at(dir_fd, fn, line, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                戻る 0;

        戻る r;
}

整数 write_string_filef(
                const char *fn,
                WriteStringFileFlags flags,
                const char *format, ...) {

        _cleanup_free_ char *p = NULL;
        va_list ap;
        整数 r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        もし (r < 0)
                戻る -ENOMEM;

        戻る write_string_file(fn, p, flags);
}

整数 write_string_filef_at(
                整数 dir_fd,
                const char *fn,
                WriteStringFileFlags flags,
                const char *format, ...) {

        _cleanup_free_ char *p = NULL;
        va_list ap;
        整数 r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        もし (r < 0)
                戻る -ENOMEM;

        戻る write_string_file_at(dir_fd, fn, p, flags);
}

整数 write_base64_file_at(
                整数 dir_fd,
                const char *fn,
                const struct iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ char *encoded = NULL;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : NULL, data ? data->iov_len : 0, 79, &encoded);
        もし (n < 0)
                戻る n;

        戻る write_string_file_at(dir_fd, fn, encoded, flags);
}

整数 read_one_line_file_at(整数 dir_fd, const char *filename, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);
        assert(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        もし (r < 0)
                戻る r;

        戻る read_line(f, LONG_LINE_MAX, ret);
}

整数 read_boolean_file_at(整数 dir_fd, const char *filename) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);

        r = read_one_line_file_at(dir_fd, filename, &s);
        もし (r < 0)
                戻る r;

        戻る parse_boolean(s);
}

整数 verify_file_at(整数 dir_fd, const char *fn, const char *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *buf = NULL;
        size_t l, k;
        整数 r;

        assert(blob);

        l = strlen(blob);

        もし (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        もし (!buf)
                戻る -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        もし (r < 0)
                戻る r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        もし (ferror(f))
                戻る errno_or_else(EIO);

        もし (k != l && k != l + accept_extra_nl)
                戻る 0;
        もし (memcmp(buf, blob, l) != 0)
                戻る 0;
        もし (k > l && buf[l] != '\n')
                戻る 0;

        戻る 1;
}

整数 read_virtual_file_at(
                整数 dir_fd,
                const char *filename,
                size_t max_size,
                char **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ char *buf = NULL;
        size_t n, size;
        整数 n_retries;
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

        _cleanup_close_ 整数 fd = -EBADF;
        もし (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        否则
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        もし (fd < 0)
                戻る fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        ために (;;) {
                struct stat st;

                もし (fstat(fd, &st) < 0)
                        戻る -errno;

                もし (!S_ISREG(st.st_mode))
                        戻る -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                もし (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        もし (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                もし (max_size == SIZE_MAX)
                                        戻る -EFBIG;

                                size = max_size;
                        } 否则 {
                                size = MIN((size_t) st.st_size, max_size);

                                もし (size > READ_VIRTUAL_BYTES_MAX)
                                        戻る -EFBIG;
                        }

                        n_retries--;
                } 否则 もし (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } 否则 {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                もし (!buf)
                        戻る -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                ために (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = read(fd, buf, size + 1);
                        もし (k >= 0) {
                                n = k;
                                break;
                        }

                        もし (errno != EINTR)
                                戻る -errno;
                }

                /* Consider a short read as EOF */
                もし (n <= size)
                        break;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                もし (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        break;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                もし (n_retries <= 0)
                        戻る -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                もし (lseek(fd, 0, SEEK_SET) < 0)
                        戻る -errno;

                buf = mfree(buf);
        }

        もし (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                もし (!ret_size && memchr(buf, 0, n))
                        戻る -EBADMSG;

                もし (n < size) {
                        char *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        もし (!p)
                                戻る -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        もし (ret_size)
                *ret_size = n;

        戻る !truncated;
}

整数 read_full_stream_full(
                FILE *f,
                const char *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                char **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ char *buf = NULL;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        整数 fd, r;

        assert(f);
        assert(ret_contents);
        assert(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        assert(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        もし (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                戻る -ERANGE;

        もし ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                もし (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                否则
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        もし (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                struct stat st;

                もし (fstat(fd, &st) < 0)
                        戻る -errno;

                もし (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        もし (r < 0)
                                戻る r;
                }

                もし (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        もし (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                もし (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        もし (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (void) warn_file_is_world_accessible(filename, &st, NULL, 0);
                }
        } 否则 もし (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                戻る -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        もし (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        もし (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        もし (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                戻る -errno;

        n = l = 0;
        ために (;;) {
                char *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                もし (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                もし (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        もし (!t) {
                                r = -ENOMEM;
                                goto finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        free(buf);
                } 否则 {
                        t = realloc(buf, n_next + 1);
                        もし (!t)
                                戻る -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                assert(k <= n - l);
                l += k;

                もし (ferror(f)) {
                        r = errno_or_else(EIO);
                        goto finalize;
                }
                もし (feof(f))
                        break;

                もし (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        assert(l == size);
                        break;
                }

                assert(k > 0); /* we can't have read zero bytes because that would have been EOF */

                もし (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        goto finalize;
                }

                もし (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        goto finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        もし (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ void *decoded = NULL;
                size_t decoded_size;

                buf[l++] = 0;
                もし (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                否则
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                もし (r < 0)
                        goto finalize;

                もし (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                もし (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        goto finalize;
                }
        }

        もし (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                もし (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        goto finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        もし (ret_size)
                *ret_size = l;

        戻る 0;

finalize:
        もし (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        戻る r;
}

整数 read_full_file_full(
                整数 dir_fd,
                const char *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                const char *bind_name,
                char **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = NULL;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        整数 r;

        assert(ret_contents);

        もし (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        もし (r < 0)
                戻る r;

        戻る read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

整数 script_get_shebang_interpreter(const char *path, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        整数 r;

        assert(path);

        f = fopen(path, "re");
        もし (!f)
                戻る -errno;

        char c;
        r = safe_fgetc(f, &c);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EBADMSG;
        もし (c != '#')
                戻る -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EBADMSG;
        もし (c != '!')
                戻る -EMEDIUMTYPE;

        _cleanup_free_ char *line = NULL;
        r = read_line(f, LONG_LINE_MAX, &line);
        もし (r < 0)
                戻る r;

        _cleanup_free_ char *p = NULL;
        const char *s = line;

        r = extract_first_word(&s, &p, /* separators= */ NULL, /* flags= */ 0);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -ENOEXEC;

        もし (ret)
                *ret = TAKE_PTR(p);
        戻る 0;
}

整数 get_proc_field(const char *path, const char *key, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        整数 r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        assert(path);
        assert(key);

        r = fopen_unlocked(path, "re", &f);
        もし (r == -ENOENT && proc_mounted() == 0)
                戻る -ENOSYS;
        もし (r < 0)
                戻る r;

        ために (;;) {
                 _cleanup_free_ char *line = NULL;

                 r = read_line(f, LONG_LINE_MAX, &line);
                 もし (r < 0)
                         戻る r;
                 もし (r == 0)
                         戻る -ENODATA;

                 char *l = startswith(line, key);
                 もし (l && *l == ':') {
                         もし (ret) {
                                 char *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 もし (!s)
                                         戻る -ENOMEM;

                                 *ret = s;
                         }

                         戻る 0;
                 }
        }
}

DIR* xopendirat(整数 dir_fd, const char *path, 整数 flags) {
        _cleanup_close_ 整数 fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(!(flags & (O_CREAT|O_TMPFILE)));

        もし ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                戻る opendir(path);

        もし (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        もし (fd < 0)
                戻る NULL;

        戻る take_fdopendir(&fd);
}

整数 fopen_mode_to_flags(const char *mode) {
        const char *p;
        整数 flags;

        assert(mode);

        もし ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        否则 もし ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        否则 もし ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        否则 もし ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        否则 もし ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        否则 もし ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        否则
                戻る -EINVAL;

        ために (; *p != 0; p++) {

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
                        戻る -EINVAL;
                }
        }

        戻る flags;
}

static 整数 xfopenat_regular(整数 dir_fd, const char *path, const char *mode, 整数 open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        もし (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        否则 もし (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ char *j = strjoin("/", path);
                もし (!j)
                        戻る -ENOMEM;

                f = fopen(j, mode);
        } 否则 {
                _cleanup_close_ 整数 fd = -EBADF;
                整数 mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                もし (mode_flags < 0)
                        戻る mode_flags;

                もし (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        もし (fd < 0)
                                戻る -errno;
                } 否则 {
                        もし (dir_fd == AT_FDCWD)
                                戻る -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        もし (fd < 0)
                                戻る fd;
                }

                f = take_fdopen(&fd, mode);
        }
        もし (!f)
                戻る -errno;

        *ret = f;
        戻る 0;
}

static 整数 xfopenat_unix_socket(整数 dir_fd, const char *path, const char *bind_name, FILE **ret) {
        _cleanup_close_ 整数 sk = -EBADF;
        FILE *f;
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        もし (sk < 0)
                戻る -errno;

        もし (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                union sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                もし (r < 0)
                        戻る r;

                もし (bind(sk, &bsa.sa, r) < 0)
                        戻る -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        もし (r < 0)
                戻る r;

        もし (shutdown(sk, SHUT_WR) < 0)
                戻る -errno;

        f = take_fdopen(&sk, "r");
        もし (!f)
                戻る -errno;

        *ret = f;
        戻る 0;
}

整数 xfopenat_full(
                整数 dir_fd,
                const char *path,
                const char *mode,
                整数 open_flags,
                XfopenFlags flags,
                const char *bind_name,
                FILE **ret) {

        FILE *f = NULL;  /* avoid false maybe-uninitialized warning */
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        もし (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                もし (IN_SET(r, -ENOTSOCK, -EINVAL))
                        戻る -ENXIO; /* propagate original error if this is not a socket after all */
        }
        もし (r < 0)
                戻る r;

        もし (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        戻る 0;
}

整数 fdopen_independent(整数 fd, const char *mode, FILE **ret) {
        _cleanup_close_ 整数 copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = NULL;
        整数 mode_flags;

        assert(fd >= 0);
        assert(mode);
        assert(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        もし (mode_flags < 0)
                戻る mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        もし (copy_fd < 0)
                戻る copy_fd;

        f = take_fdopen(&copy_fd, mode);
        もし (!f)
                戻る -errno;

        *ret = TAKE_PTR(f);
        戻る 0;
}

static 整数 search_and_open_internal(
                const char *path,
                整数 mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                const char *root,
                char **search,
                整数 *ret_fd,
                char **ret_path) {

        整数 r;

        assert(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        assert(path);

        もし (path_is_absolute(path)) {
                _cleanup_close_ 整数 fd = -EBADF;

                もし (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                否则
                        r = RET_NERRNO(access(path, mode));
                もし (r < 0)
                        戻る r;

                もし (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        もし (r < 0)
                                戻る r;
                }

                もし (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                戻る 0;
        }

        もし (!path_strv_resolve_uniq(search, root))
                戻る -ENOMEM;

        STRV_FOREACH(i, search) {
                _cleanup_close_ 整数 fd = -EBADF;
                _cleanup_free_ char *p = NULL;

                p = path_join(root, *i, path);
                もし (!p)
                        戻る -ENOMEM;

                もし (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                否则
                        r = RET_NERRNO(access(p, F_OK));
                もし (r >= 0) {
                        もし (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        もし (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        戻る 0;
                }
                もし (r != -ENOENT)
                        戻る r;
        }

        戻る -ENOENT;
}

整数 search_and_open(
                const char *path,
                整数 mode,
                const char *root,
                char **search,
                整数 *ret_fd,
                char **ret_path) {

        _cleanup_strv_free_ char **copy = NULL;

        assert(path);

        copy = strv_copy(search);
        もし (!copy)
                戻る -ENOMEM;

        戻る search_and_open_internal(path, mode, root, copy, ret_fd, ret_path);
}

static 整数 search_and_fopen_internal(
                const char *path,
                const char *mode,
                const char *root,
                char **search,
                FILE **ret_file,
                char **ret_path) {

        _cleanup_free_ char *found_path = NULL;
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        assert(path);
        assert(mode || !ret_file);

        r = search_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        root,
                        search,
                        ret_file ? &fd : NULL,
                        ret_path ? &found_path : NULL);
        もし (r < 0)
                戻る r;

        もし (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                もし (!f)
                        戻る -errno;

                *ret_file = f;
        }

        もし (ret_path)
                *ret_path = TAKE_PTR(found_path);

        戻る 0;
}

整数 search_and_fopen(
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
        もし (!copy)
                戻る -ENOMEM;

        戻る search_and_fopen_internal(path, mode, root, copy, ret_file, ret_path);
}

整数 search_and_fopen_nulstr(
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
        もし (!l)
                戻る -ENOMEM;

        戻る search_and_fopen_internal(path, mode, root, l, ret_file, ret_path);
}

整数 fflush_and_check(FILE *f) {
        assert(f);

        errno = 0;
        fflush(f);

        もし (ferror(f))
                戻る errno_or_else(EIO);

        戻る 0;
}

整数 fflush_sync_and_check(FILE *f) {
        整数 r, fd;

        assert(f);

        r = fflush_and_check(f);
        もし (r < 0)
                戻る r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        もし (fd < 0)
                戻る 0;

        r = fsync_full(fd);
        もし (r < 0)
                戻る r;

        戻る 0;
}

整数 write_timestamp_file_atomic(const char *fn, usec_t n) {
        char ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        もし (!timestamp_is_set(n))
                戻る -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        戻る write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

整数 read_timestamp_file(const char *fn, usec_t *ret) {
        _cleanup_free_ char *ln = NULL;
        uint64_t t;
        整数 r;

        assert(ret);

        r = read_one_line_file(fn, &ln);
        もし (r < 0)
                戻る r;

        r = safe_atou64(ln, &t);
        もし (r < 0)
                戻る r;

        もし (!timestamp_is_set(t))
                戻る -ERANGE;

        *ret = (usec_t) t;
        戻る 0;
}

整数 fputs_with_separator(FILE *f, const char *s, const char *separator, bool *space) {
        assert(s);
        assert(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        もし (!f)
                f = stdout;

        もし (!separator)
                separator = " ";

        もし (*space)
                もし (fputs(separator, f) < 0)
                        戻る -EIO;

        *space = true;

        もし (fputs(s, f) < 0)
                戻る -EIO;

        戻る 0;
}

整数 fputs_with_newline(FILE *f, const char *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        もし (isempty(s))
                戻る 0;

        もし (!f)
                f = stdout;

        もし (fputs(s, f) < 0)
                戻る -EIO;

        もし (endswith(s, "\n"))
                戻る 0;

        もし (fputc('\n', f) < 0)
                戻る -EIO;

        戻る 1;
}

/* A bitmask of the EOL markers we know */
typedef enum EndOfLineMarker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} EndOfLineMarker;

static EndOfLineMarker categorize_eol(char c, ReadLineFlags flags) {

        もし (!FLAGS_SET(flags, READ_LINE_ONLY_NUL)) {
                もし (c == '\n')
                        戻る EOL_TEN;
                もし (c == '\r')
                        戻る EOL_THIRTEEN;
        }

        もし (c == '\0')
                戻る EOL_ZERO;

        戻る EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, NULL);

整数 read_line_full(FILE *f, size_t limit, ReadLineFlags flags, char **ret) {
        _cleanup_free_ char *buffer = NULL;
        size_t n = 0, count = 0;
        整数 r;

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

        もし (ret) {
                もし (!GREEDY_REALLOC(buffer, 1))
                        戻る -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                EndOfLineMarker previous_eol = EOL_NONE;
                flockfile(f);

                ために (;;) {
                        EndOfLineMarker eol;
                        char c;

                        もし (n >= limit)
                                戻る -ENOBUFS;

                        もし (count >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                戻る -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        もし (r < 0)
                                戻る r;
                        もし (r == 0) /* EOF is definitely EOL */
                                break;

                        eol = categorize_eol(c, flags);

                        もし (FLAGS_SET(previous_eol, EOL_ZERO) ||
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

                        もし (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                もし ((flags & (READ_LINE_IS_A_TTY|READ_LINE_NOT_A_TTY)) == 0) {
                                        整数 fd;

                                        fd = fileno(f);
                                        もし (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_LINE_NOT_A_TTY;
                                        否则
                                                flags |= isatty_safe(fd) ? READ_LINE_IS_A_TTY : READ_LINE_NOT_A_TTY;
                                }
                                もし (FLAGS_SET(flags, READ_LINE_IS_A_TTY))
                                        break;
                        }

                        もし (eol != EOL_NONE) {
                                previous_eol |= eol;
                                continue;
                        }

                        もし (ret) {
                                もし (!GREEDY_REALLOC(buffer, n + 2))
                                        戻る -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        もし (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        戻る (整数) count;
}

整数 read_stripped_line(FILE *f, size_t limit, char **ret) {
        _cleanup_free_ char *s = NULL;
        整数 r, k;

        assert(f);

        r = read_line(f, limit, ret ? &s : NULL);
        もし (r < 0)
                戻る r;

        もし (ret) {
                const char *p = strstrip(s);
                もし (p == s)
                        *ret = TAKE_PTR(s);
                否则 {
                        k = strdup_to(ret, p);
                        もし (k < 0)
                                戻る k;
                }
        }

        戻る r > 0;          /* Return 1 if something was read. */
}

整数 safe_fgetc(FILE *f, char *ret) {
        整数 k;

        assert(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        もし (k == EOF) {
                もし (ferror(f))
                        戻る errno_or_else(EIO);

                もし (ret)
                        *ret = 0;

                戻る 0;
        }

        もし (ret)
                *ret = k;

        戻る 1;
}

整数 warn_file_is_world_accessible(const char *filename, struct stat *st, const char *unit, unsigned line) {
        struct stat _st;

        もし (!filename)
                戻る 0;

        もし (!st) {
                もし (stat(filename, &_st) < 0)
                        戻る -errno;
                st = &_st;
        }

        もし ((st->st_mode & S_IRWXO) == 0)
                戻る 0;

        もし (unit)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        否则
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        戻る 0;
}

整数 write_data_file_atomic_at(
                整数 dir_fd,
                const char *path,
                const struct iovec *iovec,
                WriteDataFileFlags flags) {

        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ char *dn = NULL, *fn = NULL;
        r = path_split_prefix_filename(path, &dn, &fn);
        もし (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                戻る -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        もし (r < 0)
                戻る r;

        _cleanup_close_ 整数 mfd = -EBADF;
        もし (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ NULL,
                            &mfd);
                もし (r < 0)
                        戻る r;

                dir_fd = mfd;
        }

        _cleanup_free_ char *t = NULL;
        _cleanup_close_ 整数 fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        もし (fd < 0)
                戻る fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        もし (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                もし (r < 0)
                        戻る r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        もし (r < 0)
                戻る r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        もし (r < 0)
                戻る r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        戻る 0;
}
