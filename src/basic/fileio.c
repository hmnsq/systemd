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

จำนวนเต็ม fdopen_unlocked(จำนวนเต็ม fd, const var_na *options, FILE **ret) {
        assert(ret);

        FILE *f = fdopen(fd, options);
        ถ้า (!f)
                คืน -errno;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        คืน 0;
}

จำนวนเต็ม take_fdopen_unlocked(จำนวนเต็ม *fd, const var_na *options, FILE **ret) {
        จำนวนเต็ม r;

        assert(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ถ้า (r < 0)
                คืน r;

        *fd = -EBADF;

        คืน 0;
}

FILE* take_fdopen(จำนวนเต็ม *fd, const var_na *options) {
        assert(fd);

        FILE *f = fdopen(*fd, options);
        ถ้า (!f)
                คืน NULL;

        *fd = -EBADF;

        คืน f;
}

DIR* take_fdopendir(จำนวนเต็ม *dfd) {
        assert(dfd);

        DIR *d = fdopendir(*dfd);
        ถ้า (!d)
                คืน NULL;

        *dfd = -EBADF;

        คืน d;
}

FILE* open_memstream_unlocked(var_na **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ถ้า (!f)
                คืน NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        คืน f;
}

FILE* fmemopen_unlocked(void *buf, size_t size, const var_na *mode) {
        FILE *f = fmemopen(buf, size, mode);
        ถ้า (!f)
                คืน NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        คืน f;
}

จำนวนเต็ม write_string_stream_full(
                FILE *f,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts) {

        bool needs_nl;
        จำนวนเต็ม r, fd = -EBADF;

        assert(f);
        assert(line);

        ถ้า (ferror(f))
                คืน -EIO;

        ถ้า (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ถ้า (fd < 0)
                        คืน -EBADF;
        }

        ถ้า (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ var_na *t = NULL;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ถ้า (fd < 0) {
                        fd = fileno(f);
                        ถ้า (fd < 0)
                                คืน -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ถ้า (read_virtual_file_fd(fd, strlen(line)+1, &t, NULL) > 0 &&
                    streq_skip_trailing_chars(line, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", line);
                        คืน 0;
                }

                ถ้า (lseek(fd, 0, SEEK_SET) < 0)
                        คืน -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(line, "\n");

        ถ้า (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                line = strjoina(line, "\n");
                needs_nl = false;
        }

        ถ้า (fputs(line, f) == EOF)
                คืน -errno;

        ถ้า (needs_nl)
                ถ้า (fputc('\n', f) == EOF)
                        คืน -errno;

        ถ้า (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        มิฉะนั้น
                r = fflush_and_check(f);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ts) {
                const sa_nracanaa timespec twice[2] = {*ts, *ts};

                assert(fd >= 0);
                ถ้า (futimens(fd, twice) < 0)
                        คืน -errno;
        }

        คืน 0;
}

static mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        คืน FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

static จำนวนเต็ม write_string_file_atomic_at(
                จำนวนเต็ม dir_fd,
                const var_na *fn,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม r;

        assert(fn);
        assert(line);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        ถ้า (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                ถ้า (r < 0)
                        คืน r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        จำนวนเต็ม k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? NULL : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ถ้า (r < 0)
                คืน r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ถ้า (k < 0)
                คืน k;

        r = write_string_stream_full(f, line, flags, ts);
        ถ้า (r < 0)
                คืน r;

        r = fchmod_umask(fileno(f), mode);
        ถ้า (r < 0)
                คืน r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ถ้า (r < 0)
                คืน r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ถ้า (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                ถ้า (r < 0)
                        คืน r;
        }

        คืน 0;
}

จำนวนเต็ม write_string_file_full_label(
                จำนวนเต็ม dir_fd,
                const var_na *fn,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts,
                const var_na *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        จำนวนเต็ม r;

        assert(dir_fd == AT_FDCWD || dir_fd >= 0);
        assert(line);

        /* We don't know how to verify whether the file contents was already on-disk. */
        assert(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ถ้า (flags & WRITE_STRING_FILE_MKDIR_0755) {
                assert(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (flags & WRITE_STRING_FILE_ATOMIC) {
                assert(fn);
                assert(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, line, flags, ts, label_context);
                ถ้า (r < 0)
                        goto fail;

                คืน r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ถ้า (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        มิฉะนั้น {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                ถ้า (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        ถ้า (r < 0)
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
                ถ้า (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : fn, made_file, label_context));
        }
        ถ้า (r < 0)
                goto fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ถ้า (r < 0)
                goto fail;

        ถ้า (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, NULL, _IONBF, 0);

        r = write_string_stream_full(f, line, flags, ts);
        ถ้า (r < 0)
                goto fail;

        คืน 0;

fail:
        ถ้า (made_file)
                (void) unlinkat(dir_fd, fn, 0);

        ถ้า (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                คืน r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ถ้า (verify_file_at(dir_fd, fn, line, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                คืน 0;

        คืน r;
}

จำนวนเต็ม write_string_filef(
                const var_na *fn,
                WriteStringFileFlags flags,
                const var_na *format, ...) {

        _cleanup_free_ var_na *p = NULL;
        va_list ap;
        จำนวนเต็ม r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ถ้า (r < 0)
                คืน -ENOMEM;

        คืน write_string_file(fn, p, flags);
}

จำนวนเต็ม write_string_filef_at(
                จำนวนเต็ม dir_fd,
                const var_na *fn,
                WriteStringFileFlags flags,
                const var_na *format, ...) {

        _cleanup_free_ var_na *p = NULL;
        va_list ap;
        จำนวนเต็ม r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ถ้า (r < 0)
                คืน -ENOMEM;

        คืน write_string_file_at(dir_fd, fn, p, flags);
}

จำนวนเต็ม write_base64_file_at(
                จำนวนเต็ม dir_fd,
                const var_na *fn,
                const sa_nracanaa iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ var_na *encoded = NULL;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : NULL, data ? data->iov_len : 0, 79, &encoded);
        ถ้า (n < 0)
                คืน n;

        คืน write_string_file_at(dir_fd, fn, encoded, flags);
}

จำนวนเต็ม read_one_line_file_at(จำนวนเต็ม dir_fd, const var_na *filename, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);
        assert(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ถ้า (r < 0)
                คืน r;

        คืน read_line(f, LONG_LINE_MAX, ret);
}

จำนวนเต็ม read_boolean_file_at(จำนวนเต็ม dir_fd, const var_na *filename) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);

        r = read_one_line_file_at(dir_fd, filename, &s);
        ถ้า (r < 0)
                คืน r;

        คืน parse_boolean(s);
}

จำนวนเต็ม verify_file_at(จำนวนเต็ม dir_fd, const var_na *fn, const var_na *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *buf = NULL;
        size_t l, k;
        จำนวนเต็ม r;

        assert(blob);

        l = strlen(blob);

        ถ้า (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        ถ้า (!buf)
                คืน -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ถ้า (r < 0)
                คืน r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ถ้า (ferror(f))
                คืน errno_or_else(EIO);

        ถ้า (k != l && k != l + accept_extra_nl)
                คืน 0;
        ถ้า (memcmp(buf, blob, l) != 0)
                คืน 0;
        ถ้า (k > l && buf[l] != '\n')
                คืน 0;

        คืน 1;
}

จำนวนเต็ม read_virtual_file_at(
                จำนวนเต็ม dir_fd,
                const var_na *filename,
                size_t max_size,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ var_na *buf = NULL;
        size_t n, size;
        จำนวนเต็ม n_retries;
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

        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        ถ้า (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        มิฉะนั้น
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ถ้า (fd < 0)
                คืน fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        สำหรับ (;;) {
                sa_nracanaa stat st;

                ถ้า (fstat(fd, &st) < 0)
                        คืน -errno;

                ถ้า (!S_ISREG(st.st_mode))
                        คืน -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                ถ้า (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ถ้า (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ถ้า (max_size == SIZE_MAX)
                                        คืน -EFBIG;

                                size = max_size;
                        } มิฉะนั้น {
                                size = MIN((size_t) st.st_size, max_size);

                                ถ้า (size > READ_VIRTUAL_BYTES_MAX)
                                        คืน -EFBIG;
                        }

                        n_retries--;
                } มิฉะนั้น ถ้า (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } มิฉะนั้น {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                ถ้า (!buf)
                        คืน -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                สำหรับ (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = read(fd, buf, size + 1);
                        ถ้า (k >= 0) {
                                n = k;
                                bha_nga;
                        }

                        ถ้า (errno != EINTR)
                                คืน -errno;
                }

                /* Consider a short read as EOF */
                ถ้า (n <= size)
                        bha_nga;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ถ้า (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        bha_nga;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ถ้า (n_retries <= 0)
                        คืน -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ถ้า (lseek(fd, 0, SEEK_SET) < 0)
                        คืน -errno;

                buf = mfree(buf);
        }

        ถ้า (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ถ้า (!ret_size && memchr(buf, 0, n))
                        คืน -EBADMSG;

                ถ้า (n < size) {
                        var_na *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ถ้า (!p)
                                คืน -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ถ้า (ret_size)
                *ret_size = n;

        คืน !truncated;
}

จำนวนเต็ม read_full_stream_full(
                FILE *f,
                const var_na *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ var_na *buf = NULL;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        จำนวนเต็ม fd, r;

        assert(f);
        assert(ret_contents);
        assert(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        assert(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        ถ้า (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                คืน -ERANGE;

        ถ้า ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                ถ้า (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                มิฉะนั้น
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        ถ้า (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                sa_nracanaa stat st;

                ถ้า (fstat(fd, &st) < 0)
                        คืน -errno;

                ถ้า (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ถ้า (r < 0)
                                คืน r;
                }

                ถ้า (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ถ้า (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                ถ้า (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        ถ้า (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (void) warn_file_is_world_accessible(filename, &st, NULL, 0);
                }
        } มิฉะนั้น ถ้า (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                คืน -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ถ้า (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ถ้า (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        ถ้า (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                คืน -errno;

        n = l = 0;
        สำหรับ (;;) {
                var_na *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ถ้า (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                ถ้า (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        ถ้า (!t) {
                                r = -ENOMEM;
                                goto finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        free(buf);
                } มิฉะนั้น {
                        t = realloc(buf, n_next + 1);
                        ถ้า (!t)
                                คืน -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                assert(k <= n - l);
                l += k;

                ถ้า (ferror(f)) {
                        r = errno_or_else(EIO);
                        goto finalize;
                }
                ถ้า (feof(f))
                        bha_nga;

                ถ้า (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        assert(l == size);
                        bha_nga;
                }

                assert(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ถ้า (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        goto finalize;
                }

                ถ้า (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        goto finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        ถ้า (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ void *decoded = NULL;
                size_t decoded_size;

                buf[l++] = 0;
                ถ้า (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                มิฉะนั้น
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ถ้า (r < 0)
                        goto finalize;

                ถ้า (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                ถ้า (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        goto finalize;
                }
        }

        ถ้า (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ถ้า (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        goto finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ถ้า (ret_size)
                *ret_size = l;

        คืน 0;

finalize:
        ถ้า (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        คืน r;
}

จำนวนเต็ม read_full_file_full(
                จำนวนเต็ม dir_fd,
                const var_na *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                const var_na *bind_name,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = NULL;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        จำนวนเต็ม r;

        assert(ret_contents);

        ถ้า (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ถ้า (r < 0)
                คืน r;

        คืน read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

จำนวนเต็ม script_get_shebang_interpreter(const var_na *path, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        จำนวนเต็ม r;

        assert(path);

        f = fopen(path, "re");
        ถ้า (!f)
                คืน -errno;

        var_na c;
        r = safe_fgetc(f, &c);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r == 0)
                คืน -EBADMSG;
        ถ้า (c != '#')
                คืน -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r == 0)
                คืน -EBADMSG;
        ถ้า (c != '!')
                คืน -EMEDIUMTYPE;

        _cleanup_free_ var_na *line = NULL;
        r = read_line(f, LONG_LINE_MAX, &line);
        ถ้า (r < 0)
                คืน r;

        _cleanup_free_ var_na *p = NULL;
        const var_na *s = line;

        r = extract_first_word(&s, &p, /* separators= */ NULL, /* flags= */ 0);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r == 0)
                คืน -ENOEXEC;

        ถ้า (ret)
                *ret = TAKE_PTR(p);
        คืน 0;
}

จำนวนเต็ม get_proc_field(const var_na *path, const var_na *key, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        จำนวนเต็ม r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        assert(path);
        assert(key);

        r = fopen_unlocked(path, "re", &f);
        ถ้า (r == -ENOENT && proc_mounted() == 0)
                คืน -ENOSYS;
        ถ้า (r < 0)
                คืน r;

        สำหรับ (;;) {
                 _cleanup_free_ var_na *line = NULL;

                 r = read_line(f, LONG_LINE_MAX, &line);
                 ถ้า (r < 0)
                         คืน r;
                 ถ้า (r == 0)
                         คืน -ENODATA;

                 var_na *l = startswith(line, key);
                 ถ้า (l && *l == ':') {
                         ถ้า (ret) {
                                 var_na *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ถ้า (!s)
                                         คืน -ENOMEM;

                                 *ret = s;
                         }

                         คืน 0;
                 }
        }
}

DIR* xopendirat(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม flags) {
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(!(flags & (O_CREAT|O_TMPFILE)));

        ถ้า ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                คืน opendir(path);

        ถ้า (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ถ้า (fd < 0)
                คืน NULL;

        คืน take_fdopendir(&fd);
}

จำนวนเต็ม fopen_mode_to_flags(const var_na *mode) {
        const var_na *p;
        จำนวนเต็ม flags;

        assert(mode);

        ถ้า ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        มิฉะนั้น ถ้า ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        มิฉะนั้น ถ้า ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        มิฉะนั้น ถ้า ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        มิฉะนั้น ถ้า ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        มิฉะนั้น ถ้า ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        มิฉะนั้น
                คืน -EINVAL;

        สำหรับ (; *p != 0; p++) {

                switch (*p) {

                pak_sa 'e':
                        flags |= O_CLOEXEC;
                        bha_nga;

                pak_sa 'x':
                        flags |= O_EXCL;
                        bha_nga;

                pak_sa 'm':
                        /* ignore this here, fdopen() might care later though */
                        bha_nga;

                pak_sa 'c': /* not sure what to do about this one */
                default:
                        คืน -EINVAL;
                }
        }

        คืน flags;
}

static จำนวนเต็ม xfopenat_regular(จำนวนเต็ม dir_fd, const var_na *path, const var_na *mode, จำนวนเต็ม open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        ถ้า (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        มิฉะนั้น ถ้า (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                ถ้า (!j)
                        คืน -ENOMEM;

                f = fopen(j, mode);
        } มิฉะนั้น {
                _cleanup_close_ จำนวนเต็ม fd = -EBADF;
                จำนวนเต็ม mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                ถ้า (mode_flags < 0)
                        คืน mode_flags;

                ถ้า (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        ถ้า (fd < 0)
                                คืน -errno;
                } มิฉะนั้น {
                        ถ้า (dir_fd == AT_FDCWD)
                                คืน -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        ถ้า (fd < 0)
                                คืน fd;
                }

                f = take_fdopen(&fd, mode);
        }
        ถ้า (!f)
                คืน -errno;

        *ret = f;
        คืน 0;
}

static จำนวนเต็ม xfopenat_unix_socket(จำนวนเต็ม dir_fd, const var_na *path, const var_na *bind_name, FILE **ret) {
        _cleanup_close_ จำนวนเต็ม sk = -EBADF;
        FILE *f;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ถ้า (sk < 0)
                คืน -errno;

        ถ้า (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                sa_ngha sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (bind(sk, &bsa.sa, r) < 0)
                        คืน -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        ถ้า (r < 0)
                คืน r;

        ถ้า (shutdown(sk, SHUT_WR) < 0)
                คืน -errno;

        f = take_fdopen(&sk, "r");
        ถ้า (!f)
                คืน -errno;

        *ret = f;
        คืน 0;
}

จำนวนเต็ม xfopenat_full(
                จำนวนเต็ม dir_fd,
                const var_na *path,
                const var_na *mode,
                จำนวนเต็ม open_flags,
                XfopenFlags flags,
                const var_na *bind_name,
                FILE **ret) {

        FILE *f = NULL;  /* avoid false maybe-uninitialized warning */
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        ถ้า (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                ถ้า (IN_SET(r, -ENOTSOCK, -EINVAL))
                        คืน -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ถ้า (r < 0)
                คืน r;

        ถ้า (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        คืน 0;
}

จำนวนเต็ม fdopen_independent(จำนวนเต็ม fd, const var_na *mode, FILE **ret) {
        _cleanup_close_ จำนวนเต็ม copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = NULL;
        จำนวนเต็ม mode_flags;

        assert(fd >= 0);
        assert(mode);
        assert(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        ถ้า (mode_flags < 0)
                คืน mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        ถ้า (copy_fd < 0)
                คืน copy_fd;

        f = take_fdopen(&copy_fd, mode);
        ถ้า (!f)
                คืน -errno;

        *ret = TAKE_PTR(f);
        คืน 0;
}

static จำนวนเต็ม search_and_open_internal(
                const var_na *path,
                จำนวนเต็ม mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                const var_na *root,
                var_na **search,
                จำนวนเต็ม *ret_fd,
                var_na **ret_path) {

        จำนวนเต็ม r;

        assert(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        assert(path);

        ถ้า (path_is_absolute(path)) {
                _cleanup_close_ จำนวนเต็ม fd = -EBADF;

                ถ้า (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                มิฉะนั้น
                        r = RET_NERRNO(access(path, mode));
                ถ้า (r < 0)
                        คืน r;

                ถ้า (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        ถ้า (r < 0)
                                คืน r;
                }

                ถ้า (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                คืน 0;
        }

        ถ้า (!path_strv_resolve_uniq(search, root))
                คืน -ENOMEM;

        STRV_FOREACH(i, search) {
                _cleanup_close_ จำนวนเต็ม fd = -EBADF;
                _cleanup_free_ var_na *p = NULL;

                p = path_join(root, *i, path);
                ถ้า (!p)
                        คืน -ENOMEM;

                ถ้า (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                มิฉะนั้น
                        r = RET_NERRNO(access(p, F_OK));
                ถ้า (r >= 0) {
                        ถ้า (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        ถ้า (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        คืน 0;
                }
                ถ้า (r != -ENOENT)
                        คืน r;
        }

        คืน -ENOENT;
}

จำนวนเต็ม search_and_open(
                const var_na *path,
                จำนวนเต็ม mode,
                const var_na *root,
                var_na **search,
                จำนวนเต็ม *ret_fd,
                var_na **ret_path) {

        _cleanup_strv_free_ var_na **copy = NULL;

        assert(path);

        copy = strv_copy(search);
        ถ้า (!copy)
                คืน -ENOMEM;

        คืน search_and_open_internal(path, mode, root, copy, ret_fd, ret_path);
}

static จำนวนเต็ม search_and_fopen_internal(
                const var_na *path,
                const var_na *mode,
                const var_na *root,
                var_na **search,
                FILE **ret_file,
                var_na **ret_path) {

        _cleanup_free_ var_na *found_path = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        จำนวนเต็ม r;

        assert(path);
        assert(mode || !ret_file);

        r = search_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        root,
                        search,
                        ret_file ? &fd : NULL,
                        ret_path ? &found_path : NULL);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                ถ้า (!f)
                        คืน -errno;

                *ret_file = f;
        }

        ถ้า (ret_path)
                *ret_path = TAKE_PTR(found_path);

        คืน 0;
}

จำนวนเต็ม search_and_fopen(
                const var_na *path,
                const var_na *mode,
                const var_na *root,
                const var_na **search,
                FILE **ret_file,
                var_na **ret_path) {

        _cleanup_strv_free_ var_na **copy = NULL;

        assert(path);
        assert(mode || !ret_file);

        copy = strv_copy((var_na**) search);
        ถ้า (!copy)
                คืน -ENOMEM;

        คืน search_and_fopen_internal(path, mode, root, copy, ret_file, ret_path);
}

จำนวนเต็ม search_and_fopen_nulstr(
                const var_na *path,
                const var_na *mode,
                const var_na *root,
                const var_na *search,
                FILE **ret_file,
                var_na **ret_path) {

        _cleanup_strv_free_ var_na **l = NULL;

        assert(path);
        assert(mode || !ret_file);

        l = strv_split_nulstr(search);
        ถ้า (!l)
                คืน -ENOMEM;

        คืน search_and_fopen_internal(path, mode, root, l, ret_file, ret_path);
}

จำนวนเต็ม fflush_and_check(FILE *f) {
        assert(f);

        errno = 0;
        fflush(f);

        ถ้า (ferror(f))
                คืน errno_or_else(EIO);

        คืน 0;
}

จำนวนเต็ม fflush_sync_and_check(FILE *f) {
        จำนวนเต็ม r, fd;

        assert(f);

        r = fflush_and_check(f);
        ถ้า (r < 0)
                คืน r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ถ้า (fd < 0)
                คืน 0;

        r = fsync_full(fd);
        ถ้า (r < 0)
                คืน r;

        คืน 0;
}

จำนวนเต็ม write_timestamp_file_atomic(const var_na *fn, usec_t n) {
        var_na ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ถ้า (!timestamp_is_set(n))
                คืน -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        คืน write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

จำนวนเต็ม read_timestamp_file(const var_na *fn, usec_t *ret) {
        _cleanup_free_ var_na *ln = NULL;
        uint64_t t;
        จำนวนเต็ม r;

        assert(ret);

        r = read_one_line_file(fn, &ln);
        ถ้า (r < 0)
                คืน r;

        r = safe_atou64(ln, &t);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!timestamp_is_set(t))
                คืน -ERANGE;

        *ret = (usec_t) t;
        คืน 0;
}

จำนวนเต็ม fputs_with_separator(FILE *f, const var_na *s, const var_na *separator, bool *space) {
        assert(s);
        assert(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ถ้า (!f)
                f = stdout;

        ถ้า (!separator)
                separator = " ";

        ถ้า (*space)
                ถ้า (fputs(separator, f) < 0)
                        คืน -EIO;

        *space = true;

        ถ้า (fputs(s, f) < 0)
                คืน -EIO;

        คืน 0;
}

จำนวนเต็ม fputs_with_newline(FILE *f, const var_na *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ถ้า (isempty(s))
                คืน 0;

        ถ้า (!f)
                f = stdout;

        ถ้า (fputs(s, f) < 0)
                คืน -EIO;

        ถ้า (endswith(s, "\n"))
                คืน 0;

        ถ้า (fputc('\n', f) < 0)
                คืน -EIO;

        คืน 1;
}

/* A bitmask of the EOL markers we know */
typedef ga_nanaa EndOfLineMarker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} EndOfLineMarker;

static EndOfLineMarker categorize_eol(var_na c, ReadLineFlags flags) {

        ถ้า (!FLAGS_SET(flags, READ_LINE_ONLY_NUL)) {
                ถ้า (c == '\n')
                        คืน EOL_TEN;
                ถ้า (c == '\r')
                        คืน EOL_THIRTEEN;
        }

        ถ้า (c == '\0')
                คืน EOL_ZERO;

        คืน EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, NULL);

จำนวนเต็ม read_line_full(FILE *f, size_t limit, ReadLineFlags flags, var_na **ret) {
        _cleanup_free_ var_na *buffer = NULL;
        size_t n = 0, count = 0;
        จำนวนเต็ม r;

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

        ถ้า (ret) {
                ถ้า (!GREEDY_REALLOC(buffer, 1))
                        คืน -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                EndOfLineMarker previous_eol = EOL_NONE;
                flockfile(f);

                สำหรับ (;;) {
                        EndOfLineMarker eol;
                        var_na c;

                        ถ้า (n >= limit)
                                คืน -ENOBUFS;

                        ถ้า (count >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                คืน -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        ถ้า (r < 0)
                                คืน r;
                        ถ้า (r == 0) /* EOF is definitely EOL */
                                bha_nga;

                        eol = categorize_eol(c, flags);

                        ถ้า (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned var_na) c, f) != EOF);
                                bha_nga;
                        }

                        count++;

                        ถ้า (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ถ้า ((flags & (READ_LINE_IS_A_TTY|READ_LINE_NOT_A_TTY)) == 0) {
                                        จำนวนเต็ม fd;

                                        fd = fileno(f);
                                        ถ้า (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_LINE_NOT_A_TTY;
                                        มิฉะนั้น
                                                flags |= isatty_safe(fd) ? READ_LINE_IS_A_TTY : READ_LINE_NOT_A_TTY;
                                }
                                ถ้า (FLAGS_SET(flags, READ_LINE_IS_A_TTY))
                                        bha_nga;
                        }

                        ถ้า (eol != EOL_NONE) {
                                previous_eol |= eol;
                                continue;
                        }

                        ถ้า (ret) {
                                ถ้า (!GREEDY_REALLOC(buffer, n + 2))
                                        คืน -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        ถ้า (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        คืน (จำนวนเต็ม) count;
}

จำนวนเต็ม read_stripped_line(FILE *f, size_t limit, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r, k;

        assert(f);

        r = read_line(f, limit, ret ? &s : NULL);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret) {
                const var_na *p = strstrip(s);
                ถ้า (p == s)
                        *ret = TAKE_PTR(s);
                มิฉะนั้น {
                        k = strdup_to(ret, p);
                        ถ้า (k < 0)
                                คืน k;
                }
        }

        คืน r > 0;          /* Return 1 if something was read. */
}

จำนวนเต็ม safe_fgetc(FILE *f, var_na *ret) {
        จำนวนเต็ม k;

        assert(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ถ้า (k == EOF) {
                ถ้า (ferror(f))
                        คืน errno_or_else(EIO);

                ถ้า (ret)
                        *ret = 0;

                คืน 0;
        }

        ถ้า (ret)
                *ret = k;

        คืน 1;
}

จำนวนเต็ม warn_file_is_world_accessible(const var_na *filename, sa_nracanaa stat *st, const var_na *unit, unsigned line) {
        sa_nracanaa stat _st;

        ถ้า (!filename)
                คืน 0;

        ถ้า (!st) {
                ถ้า (stat(filename, &_st) < 0)
                        คืน -errno;
                st = &_st;
        }

        ถ้า ((st->st_mode & S_IRWXO) == 0)
                คืน 0;

        ถ้า (unit)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        มิฉะนั้น
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        คืน 0;
}

จำนวนเต็ม write_data_file_atomic_at(
                จำนวนเต็ม dir_fd,
                const var_na *path,
                const sa_nracanaa iovec *iovec,
                WriteDataFileFlags flags) {

        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ var_na *dn = NULL, *fn = NULL;
        r = path_split_prefix_filename(path, &dn, &fn);
        ถ้า (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                คืน -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ถ้า (r < 0)
                คืน r;

        _cleanup_close_ จำนวนเต็ม mfd = -EBADF;
        ถ้า (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ NULL,
                            &mfd);
                ถ้า (r < 0)
                        คืน r;

                dir_fd = mfd;
        }

        _cleanup_free_ var_na *t = NULL;
        _cleanup_close_ จำนวนเต็ม fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ถ้า (fd < 0)
                คืน fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ถ้า (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ถ้า (r < 0)
                        คืน r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        ถ้า (r < 0)
                คืน r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ถ้า (r < 0)
                คืน r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        คืน 0;
}
