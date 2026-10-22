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

පූර්ණ_සංඛ්‍යා fdopen_unlocked(පූර්ණ_සංඛ්‍යා fd, const var_na *options, FILE **ret) {
        assert(ret);

        FILE *f = fdopen(fd, options);
        නම් (!f)
                ආපසු -errno;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා take_fdopen_unlocked(පූර්ණ_සංඛ්‍යා *fd, const var_na *options, FILE **ret) {
        පූර්ණ_සංඛ්‍යා r;

        assert(fd);

        r = fdopen_unlocked(*fd, options, ret);
        නම් (r < 0)
                ආපසු r;

        *fd = -EBADF;

        ආපසු 0;
}

FILE* take_fdopen(පූර්ණ_සංඛ්‍යා *fd, const var_na *options) {
        assert(fd);

        FILE *f = fdopen(*fd, options);
        නම් (!f)
                ආපසු NULL;

        *fd = -EBADF;

        ආපසු f;
}

DIR* take_fdopendir(පූර්ණ_සංඛ්‍යා *dfd) {
        assert(dfd);

        DIR *d = fdopendir(*dfd);
        නම් (!d)
                ආපසු NULL;

        *dfd = -EBADF;

        ආපසු d;
}

FILE* open_memstream_unlocked(var_na **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        නම් (!f)
                ආපසු NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ආපසු f;
}

FILE* fmemopen_unlocked(void *buf, size_t size, const var_na *mode) {
        FILE *f = fmemopen(buf, size, mode);
        නම් (!f)
                ආපසු NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ආපසු f;
}

පූර්ණ_සංඛ්‍යා write_string_stream_full(
                FILE *f,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts) {

        bool needs_nl;
        පූර්ණ_සංඛ්‍යා r, fd = -EBADF;

        assert(f);
        assert(line);

        නම් (ferror(f))
                ආපසු -EIO;

        නම් (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                නම් (fd < 0)
                        ආපසු -EBADF;
        }

        නම් (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ var_na *t = NULL;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                නම් (fd < 0) {
                        fd = fileno(f);
                        නම් (fd < 0)
                                ආපසු -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                නම් (read_virtual_file_fd(fd, strlen(line)+1, &t, NULL) > 0 &&
                    streq_skip_trailing_chars(line, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", line);
                        ආපසු 0;
                }

                නම් (lseek(fd, 0, SEEK_SET) < 0)
                        ආපසු -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(line, "\n");

        නම් (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                line = strjoina(line, "\n");
                needs_nl = false;
        }

        නම් (fputs(line, f) == EOF)
                ආපසු -errno;

        නම් (needs_nl)
                නම් (fputc('\n', f) == EOF)
                        ආපසු -errno;

        නම් (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        නැතහොත්
                r = fflush_and_check(f);
        නම් (r < 0)
                ආපසු r;

        නම් (ts) {
                const sa_nracanaa timespec twice[2] = {*ts, *ts};

                assert(fd >= 0);
                නම් (futimens(fd, twice) < 0)
                        ආපසු -errno;
        }

        ආපසු 0;
}

static mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ආපසු FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

static පූර්ණ_සංඛ්‍යා write_string_file_atomic_at(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *fn,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(fn);
        assert(line);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        නම් (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                නම් (r < 0)
                        ආපසු r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        පූර්ණ_සංඛ්‍යා k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? NULL : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        නම් (r < 0)
                ආපසු r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        නම් (k < 0)
                ආපසු k;

        r = write_string_stream_full(f, line, flags, ts);
        නම් (r < 0)
                ආපසු r;

        r = fchmod_umask(fileno(f), mode);
        නම් (r < 0)
                ආපසු r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        නම් (r < 0)
                ආපසු r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        නම් (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                නම් (r < 0)
                        ආපසු r;
        }

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා write_string_file_full_label(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *fn,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts,
                const var_na *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        පූර්ණ_සංඛ්‍යා r;

        assert(dir_fd == AT_FDCWD || dir_fd >= 0);
        assert(line);

        /* We don't know how to verify whether the file contents was already on-disk. */
        assert(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        නම් (flags & WRITE_STRING_FILE_MKDIR_0755) {
                assert(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (flags & WRITE_STRING_FILE_ATOMIC) {
                assert(fn);
                assert(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, line, flags, ts, label_context);
                නම් (r < 0)
                        goto fail;

                ආපසු r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        නම් (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        නැතහොත් {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                නම් (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        නම් (r < 0)
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
                නම් (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : fn, made_file, label_context));
        }
        නම් (r < 0)
                goto fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        නම් (r < 0)
                goto fail;

        නම් (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, NULL, _IONBF, 0);

        r = write_string_stream_full(f, line, flags, ts);
        නම් (r < 0)
                goto fail;

        ආපසු 0;

fail:
        නම් (made_file)
                (void) unlinkat(dir_fd, fn, 0);

        නම් (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ආපසු r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        නම් (verify_file_at(dir_fd, fn, line, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ආපසු 0;

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා write_string_filef(
                const var_na *fn,
                WriteStringFileFlags flags,
                const var_na *format, ...) {

        _cleanup_free_ var_na *p = NULL;
        va_list ap;
        පූර්ණ_සංඛ්‍යා r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        නම් (r < 0)
                ආපසු -ENOMEM;

        ආපසු write_string_file(fn, p, flags);
}

පූර්ණ_සංඛ්‍යා write_string_filef_at(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *fn,
                WriteStringFileFlags flags,
                const var_na *format, ...) {

        _cleanup_free_ var_na *p = NULL;
        va_list ap;
        පූර්ණ_සංඛ්‍යා r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        නම් (r < 0)
                ආපසු -ENOMEM;

        ආපසු write_string_file_at(dir_fd, fn, p, flags);
}

පූර්ණ_සංඛ්‍යා write_base64_file_at(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *fn,
                const sa_nracanaa iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ var_na *encoded = NULL;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : NULL, data ? data->iov_len : 0, 79, &encoded);
        නම් (n < 0)
                ආපසු n;

        ආපසු write_string_file_at(dir_fd, fn, encoded, flags);
}

පූර්ණ_සංඛ්‍යා read_one_line_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);
        assert(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        නම් (r < 0)
                ආපසු r;

        ආපසු read_line(f, LONG_LINE_MAX, ret);
}

පූර්ණ_සංඛ්‍යා read_boolean_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename) {
        _cleanup_free_ var_na *s = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);

        r = read_one_line_file_at(dir_fd, filename, &s);
        නම් (r < 0)
                ආපසු r;

        ආපසු parse_boolean(s);
}

පූර්ණ_සංඛ්‍යා verify_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, const var_na *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *buf = NULL;
        size_t l, k;
        පූර්ණ_සංඛ්‍යා r;

        assert(blob);

        l = strlen(blob);

        නම් (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        නම් (!buf)
                ආපසු -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        නම් (r < 0)
                ආපසු r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        නම් (ferror(f))
                ආපසු errno_or_else(EIO);

        නම් (k != l && k != l + accept_extra_nl)
                ආපසු 0;
        නම් (memcmp(buf, blob, l) != 0)
                ආපසු 0;
        නම් (k > l && buf[l] != '\n')
                ආපසු 0;

        ආපසු 1;
}

පූර්ණ_සංඛ්‍යා read_virtual_file_at(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *filename,
                size_t max_size,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ var_na *buf = NULL;
        size_t n, size;
        පූර්ණ_සංඛ්‍යා n_retries;
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

        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        නම් (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        නැතහොත්
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        නම් (fd < 0)
                ආපසු fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        සඳහා (;;) {
                sa_nracanaa stat st;

                නම් (fstat(fd, &st) < 0)
                        ආපසු -errno;

                නම් (!S_ISREG(st.st_mode))
                        ආපසු -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                නම් (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        නම් (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                නම් (max_size == SIZE_MAX)
                                        ආපසු -EFBIG;

                                size = max_size;
                        } නැතහොත් {
                                size = MIN((size_t) st.st_size, max_size);

                                නම් (size > READ_VIRTUAL_BYTES_MAX)
                                        ආපසු -EFBIG;
                        }

                        n_retries--;
                } නැතහොත් නම් (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } නැතහොත් {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                නම් (!buf)
                        ආපසු -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                සඳහා (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = read(fd, buf, size + 1);
                        නම් (k >= 0) {
                                n = k;
                                bha_nga;
                        }

                        නම් (errno != EINTR)
                                ආපසු -errno;
                }

                /* Consider a short read as EOF */
                නම් (n <= size)
                        bha_nga;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                නම් (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        bha_nga;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                නම් (n_retries <= 0)
                        ආපසු -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                නම් (lseek(fd, 0, SEEK_SET) < 0)
                        ආපසු -errno;

                buf = mfree(buf);
        }

        නම් (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                නම් (!ret_size && memchr(buf, 0, n))
                        ආපසු -EBADMSG;

                නම් (n < size) {
                        var_na *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        නම් (!p)
                                ආපසු -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        නම් (ret_size)
                *ret_size = n;

        ආපසු !truncated;
}

පූර්ණ_සංඛ්‍යා read_full_stream_full(
                FILE *f,
                const var_na *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ var_na *buf = NULL;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        පූර්ණ_සංඛ්‍යා fd, r;

        assert(f);
        assert(ret_contents);
        assert(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        assert(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        නම් (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ආපසු -ERANGE;

        නම් ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                නම් (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                නැතහොත්
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        නම් (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                sa_nracanaa stat st;

                නම් (fstat(fd, &st) < 0)
                        ආපසු -errno;

                නම් (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        නම් (r < 0)
                                ආපසු r;
                }

                නම් (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        නම් (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                නම් (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        නම් (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (void) warn_file_is_world_accessible(filename, &st, NULL, 0);
                }
        } නැතහොත් නම් (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ආපසු -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        නම් (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        නම් (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        නම් (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ආපසු -errno;

        n = l = 0;
        සඳහා (;;) {
                var_na *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                නම් (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                නම් (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        නම් (!t) {
                                r = -ENOMEM;
                                goto finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        free(buf);
                } නැතහොත් {
                        t = realloc(buf, n_next + 1);
                        නම් (!t)
                                ආපසු -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                assert(k <= n - l);
                l += k;

                නම් (ferror(f)) {
                        r = errno_or_else(EIO);
                        goto finalize;
                }
                නම් (feof(f))
                        bha_nga;

                නම් (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        assert(l == size);
                        bha_nga;
                }

                assert(k > 0); /* we can't have read zero bytes because that would have been EOF */

                නම් (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        goto finalize;
                }

                නම් (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        goto finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        නම් (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ void *decoded = NULL;
                size_t decoded_size;

                buf[l++] = 0;
                නම් (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                නැතහොත්
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                නම් (r < 0)
                        goto finalize;

                නම් (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                නම් (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        goto finalize;
                }
        }

        නම් (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                නම් (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        goto finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        නම් (ret_size)
                *ret_size = l;

        ආපසු 0;

finalize:
        නම් (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා read_full_file_full(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                const var_na *bind_name,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = NULL;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret_contents);

        නම් (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        නම් (r < 0)
                ආපසු r;

        ආපසු read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

පූර්ණ_සංඛ්‍යා script_get_shebang_interpreter(const var_na *path, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);

        f = fopen(path, "re");
        නම් (!f)
                ආපසු -errno;

        var_na c;
        r = safe_fgetc(f, &c);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -EBADMSG;
        නම් (c != '#')
                ආපසු -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -EBADMSG;
        නම් (c != '!')
                ආපසු -EMEDIUMTYPE;

        _cleanup_free_ var_na *line = NULL;
        r = read_line(f, LONG_LINE_MAX, &line);
        නම් (r < 0)
                ආපසු r;

        _cleanup_free_ var_na *p = NULL;
        const var_na *s = line;

        r = extract_first_word(&s, &p, /* separators= */ NULL, /* flags= */ 0);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -ENOEXEC;

        නම් (ret)
                *ret = TAKE_PTR(p);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා get_proc_field(const var_na *path, const var_na *key, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        පූර්ණ_සංඛ්‍යා r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        assert(path);
        assert(key);

        r = fopen_unlocked(path, "re", &f);
        නම් (r == -ENOENT && proc_mounted() == 0)
                ආපසු -ENOSYS;
        නම් (r < 0)
                ආපසු r;

        සඳහා (;;) {
                 _cleanup_free_ var_na *line = NULL;

                 r = read_line(f, LONG_LINE_MAX, &line);
                 නම් (r < 0)
                         ආපසු r;
                 නම් (r == 0)
                         ආපසු -ENODATA;

                 var_na *l = startswith(line, key);
                 නම් (l && *l == ':') {
                         නම් (ret) {
                                 var_na *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 නම් (!s)
                                         ආපසු -ENOMEM;

                                 *ret = s;
                         }

                         ආපසු 0;
                 }
        }
}

DIR* xopendirat(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා flags) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(!(flags & (O_CREAT|O_TMPFILE)));

        නම් ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ආපසු opendir(path);

        නම් (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        නම් (fd < 0)
                ආපසු NULL;

        ආපසු take_fdopendir(&fd);
}

පූර්ණ_සංඛ්‍යා fopen_mode_to_flags(const var_na *mode) {
        const var_na *p;
        පූර්ණ_සංඛ්‍යා flags;

        assert(mode);

        නම් ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        නැතහොත් නම් ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        නැතහොත් නම් ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        නැතහොත් නම් ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        නැතහොත් නම් ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        නැතහොත් නම් ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        නැතහොත්
                ආපසු -EINVAL;

        සඳහා (; *p != 0; p++) {

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
                        ආපසු -EINVAL;
                }
        }

        ආපසු flags;
}

static පූර්ණ_සංඛ්‍යා xfopenat_regular(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const var_na *mode, පූර්ණ_සංඛ්‍යා open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        නම් (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        නැතහොත් නම් (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                නම් (!j)
                        ආපසු -ENOMEM;

                f = fopen(j, mode);
        } නැතහොත් {
                _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
                පූර්ණ_සංඛ්‍යා mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                නම් (mode_flags < 0)
                        ආපසු mode_flags;

                නම් (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        නම් (fd < 0)
                                ආපසු -errno;
                } නැතහොත් {
                        නම් (dir_fd == AT_FDCWD)
                                ආපසු -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        නම් (fd < 0)
                                ආපසු fd;
                }

                f = take_fdopen(&fd, mode);
        }
        නම් (!f)
                ආපසු -errno;

        *ret = f;
        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා xfopenat_unix_socket(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const var_na *bind_name, FILE **ret) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා sk = -EBADF;
        FILE *f;
        පූර්ණ_සංඛ්‍යා r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        නම් (sk < 0)
                ආපසු -errno;

        නම් (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                sa_ngha sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                නම් (r < 0)
                        ආපසු r;

                නම් (bind(sk, &bsa.sa, r) < 0)
                        ආපසු -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        නම් (r < 0)
                ආපසු r;

        නම් (shutdown(sk, SHUT_WR) < 0)
                ආපසු -errno;

        f = take_fdopen(&sk, "r");
        නම් (!f)
                ආපසු -errno;

        *ret = f;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා xfopenat_full(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *path,
                const var_na *mode,
                පූර්ණ_සංඛ්‍යා open_flags,
                XfopenFlags flags,
                const var_na *bind_name,
                FILE **ret) {

        FILE *f = NULL;  /* avoid false maybe-uninitialized warning */
        පූර්ණ_සංඛ්‍යා r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        නම් (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                නම් (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ආපසු -ENXIO; /* propagate original error if this is not a socket after all */
        }
        නම් (r < 0)
                ආපසු r;

        නම් (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdopen_independent(පූර්ණ_සංඛ්‍යා fd, const var_na *mode, FILE **ret) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = NULL;
        පූර්ණ_සංඛ්‍යා mode_flags;

        assert(fd >= 0);
        assert(mode);
        assert(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        නම් (mode_flags < 0)
                ආපසු mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        නම් (copy_fd < 0)
                ආපසු copy_fd;

        f = take_fdopen(&copy_fd, mode);
        නම් (!f)
                ආපසු -errno;

        *ret = TAKE_PTR(f);
        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා search_and_open_internal(
                const var_na *path,
                පූර්ණ_සංඛ්‍යා mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                const var_na *root,
                var_na **search,
                පූර්ණ_සංඛ්‍යා *ret_fd,
                var_na **ret_path) {

        පූර්ණ_සංඛ්‍යා r;

        assert(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        assert(path);

        නම් (path_is_absolute(path)) {
                _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;

                නම් (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                නැතහොත්
                        r = RET_NERRNO(access(path, mode));
                නම් (r < 0)
                        ආපසු r;

                නම් (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        නම් (r < 0)
                                ආපසු r;
                }

                නම් (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ආපසු 0;
        }

        නම් (!path_strv_resolve_uniq(search, root))
                ආපසු -ENOMEM;

        STRV_FOREACH(i, search) {
                _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
                _cleanup_free_ var_na *p = NULL;

                p = path_join(root, *i, path);
                නම් (!p)
                        ආපසු -ENOMEM;

                නම් (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                නැතහොත්
                        r = RET_NERRNO(access(p, F_OK));
                නම් (r >= 0) {
                        නම් (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        නම් (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ආපසු 0;
                }
                නම් (r != -ENOENT)
                        ආපසු r;
        }

        ආපසු -ENOENT;
}

පූර්ණ_සංඛ්‍යා search_and_open(
                const var_na *path,
                පූර්ණ_සංඛ්‍යා mode,
                const var_na *root,
                var_na **search,
                පූර්ණ_සංඛ්‍යා *ret_fd,
                var_na **ret_path) {

        _cleanup_strv_free_ var_na **copy = NULL;

        assert(path);

        copy = strv_copy(search);
        නම් (!copy)
                ආපසු -ENOMEM;

        ආපසු search_and_open_internal(path, mode, root, copy, ret_fd, ret_path);
}

static පූර්ණ_සංඛ්‍යා search_and_fopen_internal(
                const var_na *path,
                const var_na *mode,
                const var_na *root,
                var_na **search,
                FILE **ret_file,
                var_na **ret_path) {

        _cleanup_free_ var_na *found_path = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);
        assert(mode || !ret_file);

        r = search_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        root,
                        search,
                        ret_file ? &fd : NULL,
                        ret_path ? &found_path : NULL);
        නම් (r < 0)
                ආපසු r;

        නම් (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                නම් (!f)
                        ආපසු -errno;

                *ret_file = f;
        }

        නම් (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා search_and_fopen(
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
        නම් (!copy)
                ආපසු -ENOMEM;

        ආපසු search_and_fopen_internal(path, mode, root, copy, ret_file, ret_path);
}

පූර්ණ_සංඛ්‍යා search_and_fopen_nulstr(
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
        නම් (!l)
                ආපසු -ENOMEM;

        ආපසු search_and_fopen_internal(path, mode, root, l, ret_file, ret_path);
}

පූර්ණ_සංඛ්‍යා fflush_and_check(FILE *f) {
        assert(f);

        errno = 0;
        fflush(f);

        නම් (ferror(f))
                ආපසු errno_or_else(EIO);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fflush_sync_and_check(FILE *f) {
        පූර්ණ_සංඛ්‍යා r, fd;

        assert(f);

        r = fflush_and_check(f);
        නම් (r < 0)
                ආපසු r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        නම් (fd < 0)
                ආපසු 0;

        r = fsync_full(fd);
        නම් (r < 0)
                ආපසු r;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා write_timestamp_file_atomic(const var_na *fn, usec_t n) {
        var_na ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        නම් (!timestamp_is_set(n))
                ආපසු -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ආපසු write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

පූර්ණ_සංඛ්‍යා read_timestamp_file(const var_na *fn, usec_t *ret) {
        _cleanup_free_ var_na *ln = NULL;
        uint64_t t;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = read_one_line_file(fn, &ln);
        නම් (r < 0)
                ආපසු r;

        r = safe_atou64(ln, &t);
        නම් (r < 0)
                ආපසු r;

        නම් (!timestamp_is_set(t))
                ආපසු -ERANGE;

        *ret = (usec_t) t;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fputs_with_separator(FILE *f, const var_na *s, const var_na *separator, bool *space) {
        assert(s);
        assert(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        නම් (!f)
                f = stdout;

        නම් (!separator)
                separator = " ";

        නම් (*space)
                නම් (fputs(separator, f) < 0)
                        ආපසු -EIO;

        *space = true;

        නම් (fputs(s, f) < 0)
                ආපසු -EIO;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fputs_with_newline(FILE *f, const var_na *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        නම් (isempty(s))
                ආපසු 0;

        නම් (!f)
                f = stdout;

        නම් (fputs(s, f) < 0)
                ආපසු -EIO;

        නම් (endswith(s, "\n"))
                ආපසු 0;

        නම් (fputc('\n', f) < 0)
                ආපසු -EIO;

        ආපසු 1;
}

/* A bitmask of the EOL markers we know */
typedef ga_nanaa EndOfLineMarker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} EndOfLineMarker;

static EndOfLineMarker categorize_eol(var_na c, ReadLineFlags flags) {

        නම් (!FLAGS_SET(flags, READ_LINE_ONLY_NUL)) {
                නම් (c == '\n')
                        ආපසු EOL_TEN;
                නම් (c == '\r')
                        ආපසු EOL_THIRTEEN;
        }

        නම් (c == '\0')
                ආපසු EOL_ZERO;

        ආපසු EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, NULL);

පූර්ණ_සංඛ්‍යා read_line_full(FILE *f, size_t limit, ReadLineFlags flags, var_na **ret) {
        _cleanup_free_ var_na *buffer = NULL;
        size_t n = 0, count = 0;
        පූර්ණ_සංඛ්‍යා r;

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

        නම් (ret) {
                නම් (!GREEDY_REALLOC(buffer, 1))
                        ආපසු -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                EndOfLineMarker previous_eol = EOL_NONE;
                flockfile(f);

                සඳහා (;;) {
                        EndOfLineMarker eol;
                        var_na c;

                        නම් (n >= limit)
                                ආපසු -ENOBUFS;

                        නම් (count >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ආපසු -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        නම් (r < 0)
                                ආපසු r;
                        නම් (r == 0) /* EOF is definitely EOL */
                                bha_nga;

                        eol = categorize_eol(c, flags);

                        නම් (FLAGS_SET(previous_eol, EOL_ZERO) ||
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

                        නම් (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                නම් ((flags & (READ_LINE_IS_A_TTY|READ_LINE_NOT_A_TTY)) == 0) {
                                        පූර්ණ_සංඛ්‍යා fd;

                                        fd = fileno(f);
                                        නම් (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_LINE_NOT_A_TTY;
                                        නැතහොත්
                                                flags |= isatty_safe(fd) ? READ_LINE_IS_A_TTY : READ_LINE_NOT_A_TTY;
                                }
                                නම් (FLAGS_SET(flags, READ_LINE_IS_A_TTY))
                                        bha_nga;
                        }

                        නම් (eol != EOL_NONE) {
                                previous_eol |= eol;
                                continue;
                        }

                        නම් (ret) {
                                නම් (!GREEDY_REALLOC(buffer, n + 2))
                                        ආපසු -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        නම් (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        ආපසු (පූර්ණ_සංඛ්‍යා) count;
}

පූර්ණ_සංඛ්‍යා read_stripped_line(FILE *f, size_t limit, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        පූර්ණ_සංඛ්‍යා r, k;

        assert(f);

        r = read_line(f, limit, ret ? &s : NULL);
        නම් (r < 0)
                ආපසු r;

        නම් (ret) {
                const var_na *p = strstrip(s);
                නම් (p == s)
                        *ret = TAKE_PTR(s);
                නැතහොත් {
                        k = strdup_to(ret, p);
                        නම් (k < 0)
                                ආපසු k;
                }
        }

        ආපසු r > 0;          /* Return 1 if something was read. */
}

පූර්ණ_සංඛ්‍යා safe_fgetc(FILE *f, var_na *ret) {
        පූර්ණ_සංඛ්‍යා k;

        assert(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        නම් (k == EOF) {
                නම් (ferror(f))
                        ආපසු errno_or_else(EIO);

                නම් (ret)
                        *ret = 0;

                ආපසු 0;
        }

        නම් (ret)
                *ret = k;

        ආපසු 1;
}

පූර්ණ_සංඛ්‍යා warn_file_is_world_accessible(const var_na *filename, sa_nracanaa stat *st, const var_na *unit, unsigned line) {
        sa_nracanaa stat _st;

        නම් (!filename)
                ආපසු 0;

        නම් (!st) {
                නම් (stat(filename, &_st) < 0)
                        ආපසු -errno;
                st = &_st;
        }

        නම් ((st->st_mode & S_IRWXO) == 0)
                ආපසු 0;

        නම් (unit)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        නැතහොත්
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා write_data_file_atomic_at(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *path,
                const sa_nracanaa iovec *iovec,
                WriteDataFileFlags flags) {

        පූර්ණ_සංඛ්‍යා r;

        assert(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ var_na *dn = NULL, *fn = NULL;
        r = path_split_prefix_filename(path, &dn, &fn);
        නම් (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ආපසු -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        නම් (r < 0)
                ආපසු r;

        _cleanup_close_ පූර්ණ_සංඛ්‍යා mfd = -EBADF;
        නම් (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ NULL,
                            &mfd);
                නම් (r < 0)
                        ආපසු r;

                dir_fd = mfd;
        }

        _cleanup_free_ var_na *t = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        නම් (fd < 0)
                ආපසු fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        නම් (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                නම් (r < 0)
                        ආපසු r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        නම් (r < 0)
                ආපසු r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        නම් (r < 0)
                ආපසු r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ආපසු 0;
}
