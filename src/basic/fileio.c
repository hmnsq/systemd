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

puur_naa_nka fdopen_unlocked(puur_naa_nka fd, const var_na *options, FILE **ret) {
        assert(ret);

        FILE *f = fdopen(fd, options);
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ -errno;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ᯀᯮᯞ 0;
}

puur_naa_nka take_fdopen_unlocked(puur_naa_nka *fd, const var_na *options, FILE **ret) {
        puur_naa_nka r;

        assert(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        *fd = -EBADF;

        ᯀᯮᯞ 0;
}

FILE* take_fdopen(puur_naa_nka *fd, const var_na *options) {
        assert(fd);

        FILE *f = fdopen(*fd, options);
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ NULL;

        *fd = -EBADF;

        ᯀᯮᯞ f;
}

DIR* take_fdopendir(puur_naa_nka *dfd) {
        assert(dfd);

        DIR *d = fdopendir(*dfd);
        ᯀᯮᯝ (!d)
                ᯀᯮᯞ NULL;

        *dfd = -EBADF;

        ᯀᯮᯞ d;
}

FILE* open_memstream_unlocked(var_na **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ᯀᯮᯞ f;
}

FILE* fmemopen_unlocked(void *buf, size_t size, const var_na *mode) {
        FILE *f = fmemopen(buf, size, mode);
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ NULL;

        (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ᯀᯮᯞ f;
}

puur_naa_nka write_string_stream_full(
                FILE *f,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts) {

        bool needs_nl;
        puur_naa_nka r, fd = -EBADF;

        assert(f);
        assert(line);

        ᯀᯮᯝ (ferror(f))
                ᯀᯮᯞ -EIO;

        ᯀᯮᯝ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ᯀᯮᯝ (fd < 0)
                        ᯀᯮᯞ -EBADF;
        }

        ᯀᯮᯝ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ var_na *t = NULL;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ᯀᯮᯝ (fd < 0) {
                        fd = fileno(f);
                        ᯀᯮᯝ (fd < 0)
                                ᯀᯮᯞ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ᯀᯮᯝ (read_virtual_file_fd(fd, strlen(line)+1, &t, NULL) > 0 &&
                    streq_skip_trailing_chars(line, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", line);
                        ᯀᯮᯞ 0;
                }

                ᯀᯮᯝ (lseek(fd, 0, SEEK_SET) < 0)
                        ᯀᯮᯞ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(line, "\n");

        ᯀᯮᯝ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                line = strjoina(line, "\n");
                needs_nl = false;
        }

        ᯀᯮᯝ (fputs(line, f) == EOF)
                ᯀᯮᯞ -errno;

        ᯀᯮᯝ (needs_nl)
                ᯀᯮᯝ (fputc('\n', f) == EOF)
                        ᯀᯮᯞ -errno;

        ᯀᯮᯝ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ᯀᯮᯝᯂᯪ
                r = fflush_and_check(f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (ts) {
                const sa_nracanaa timespec twice[2] = {*ts, *ts};

                assert(fd >= 0);
                ᯀᯮᯝ (futimens(fd, twice) < 0)
                        ᯀᯮᯞ -errno;
        }

        ᯀᯮᯞ 0;
}

static mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ᯀᯮᯞ FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

static puur_naa_nka write_string_file_atomic_at(
                puur_naa_nka dir_fd,
                const var_na *fn,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(fn);
        assert(line);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        ᯀᯮᯝ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        puur_naa_nka k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? NULL : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ᯀᯮᯝ (k < 0)
                ᯀᯮᯞ k;

        r = write_string_stream_full(f, line, flags, ts);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = fchmod_umask(fileno(f), mode);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ᯀᯮᯝ (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        ᯀᯮᯞ 0;
}

puur_naa_nka write_string_file_full_label(
                puur_naa_nka dir_fd,
                const var_na *fn,
                const var_na *line,
                WriteStringFileFlags flags,
                const sa_nracanaa timespec *ts,
                const var_na *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        assert(dir_fd == AT_FDCWD || dir_fd >= 0);
        assert(line);

        /* We don't know how to verify whether the file contents was already on-disk. */
        assert(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ᯀᯮᯝ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                assert(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        ᯀᯮᯝ (flags & WRITE_STRING_FILE_ATOMIC) {
                assert(fn);
                assert(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, line, flags, ts, label_context);
                ᯀᯮᯝ (r < 0)
                        goto fail;

                ᯀᯮᯞ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ᯀᯮᯝ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ᯀᯮᯝᯂᯪ {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                ᯀᯮᯝ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        ᯀᯮᯝ (r < 0)
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
                ᯀᯮᯝ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : fn, made_file, label_context));
        }
        ᯀᯮᯝ (r < 0)
                goto fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ᯀᯮᯝ (r < 0)
                goto fail;

        ᯀᯮᯝ (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, NULL, _IONBF, 0);

        r = write_string_stream_full(f, line, flags, ts);
        ᯀᯮᯝ (r < 0)
                goto fail;

        ᯀᯮᯞ 0;

fail:
        ᯀᯮᯝ (made_file)
                (void) unlinkat(dir_fd, fn, 0);

        ᯀᯮᯝ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ᯀᯮᯞ r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ᯀᯮᯝ (verify_file_at(dir_fd, fn, line, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ᯀᯮᯞ 0;

        ᯀᯮᯞ r;
}

puur_naa_nka write_string_filef(
                const var_na *fn,
                WriteStringFileFlags flags,
                const var_na *format, ...) {

        _cleanup_free_ var_na *p = NULL;
        va_list ap;
        puur_naa_nka r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ write_string_file(fn, p, flags);
}

puur_naa_nka write_string_filef_at(
                puur_naa_nka dir_fd,
                const var_na *fn,
                WriteStringFileFlags flags,
                const var_na *format, ...) {

        _cleanup_free_ var_na *p = NULL;
        va_list ap;
        puur_naa_nka r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ write_string_file_at(dir_fd, fn, p, flags);
}

puur_naa_nka write_base64_file_at(
                puur_naa_nka dir_fd,
                const var_na *fn,
                const sa_nracanaa iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ var_na *encoded = NULL;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : NULL, data ? data->iov_len : 0, 79, &encoded);
        ᯀᯮᯝ (n < 0)
                ᯀᯮᯞ n;

        ᯀᯮᯞ write_string_file_at(dir_fd, fn, encoded, flags);
}

puur_naa_nka read_one_line_file_at(puur_naa_nka dir_fd, const var_na *filename, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);
        assert(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ read_line(f, LONG_LINE_MAX, ret);
}

puur_naa_nka read_boolean_file_at(puur_naa_nka dir_fd, const var_na *filename) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(filename);

        r = read_one_line_file_at(dir_fd, filename, &s);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ parse_boolean(s);
}

puur_naa_nka verify_file_at(puur_naa_nka dir_fd, const var_na *fn, const var_na *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *buf = NULL;
        size_t l, k;
        puur_naa_nka r;

        assert(blob);

        l = strlen(blob);

        ᯀᯮᯝ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        ᯀᯮᯝ (!buf)
                ᯀᯮᯞ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ᯀᯮᯝ (ferror(f))
                ᯀᯮᯞ errno_or_else(EIO);

        ᯀᯮᯝ (k != l && k != l + accept_extra_nl)
                ᯀᯮᯞ 0;
        ᯀᯮᯝ (memcmp(buf, blob, l) != 0)
                ᯀᯮᯞ 0;
        ᯀᯮᯝ (k > l && buf[l] != '\n')
                ᯀᯮᯞ 0;

        ᯀᯮᯞ 1;
}

puur_naa_nka read_virtual_file_at(
                puur_naa_nka dir_fd,
                const var_na *filename,
                size_t max_size,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ var_na *buf = NULL;
        size_t n, size;
        puur_naa_nka n_retries;
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

        _cleanup_close_ puur_naa_nka fd = -EBADF;
        ᯀᯮᯝ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ᯀᯮᯝᯂᯪ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ᯀᯮᯝ (fd < 0)
                ᯀᯮᯞ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        for (;;) {
                sa_nracanaa stat st;

                ᯀᯮᯝ (fstat(fd, &st) < 0)
                        ᯀᯮᯞ -errno;

                ᯀᯮᯝ (!S_ISREG(st.st_mode))
                        ᯀᯮᯞ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                ᯀᯮᯝ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ᯀᯮᯝ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ᯀᯮᯝ (max_size == SIZE_MAX)
                                        ᯀᯮᯞ -EFBIG;

                                size = max_size;
                        } ᯀᯮᯝᯂᯪ {
                                size = MIN((size_t) st.st_size, max_size);

                                ᯀᯮᯝ (size > READ_VIRTUAL_BYTES_MAX)
                                        ᯀᯮᯞ -EFBIG;
                        }

                        n_retries--;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } ᯀᯮᯝᯂᯪ {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                ᯀᯮᯝ (!buf)
                        ᯀᯮᯞ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                for (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = read(fd, buf, size + 1);
                        ᯀᯮᯝ (k >= 0) {
                                n = k;
                                bha_nga;
                        }

                        ᯀᯮᯝ (errno != EINTR)
                                ᯀᯮᯞ -errno;
                }

                /* Consider a short read as EOF */
                ᯀᯮᯝ (n <= size)
                        bha_nga;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ᯀᯮᯝ (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        bha_nga;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ᯀᯮᯝ (n_retries <= 0)
                        ᯀᯮᯞ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ᯀᯮᯝ (lseek(fd, 0, SEEK_SET) < 0)
                        ᯀᯮᯞ -errno;

                buf = mfree(buf);
        }

        ᯀᯮᯝ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ᯀᯮᯝ (!ret_size && memchr(buf, 0, n))
                        ᯀᯮᯞ -EBADMSG;

                ᯀᯮᯝ (n < size) {
                        var_na *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ᯀᯮᯝ (!p)
                                ᯀᯮᯞ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ᯀᯮᯝ (ret_size)
                *ret_size = n;

        ᯀᯮᯞ !truncated;
}

puur_naa_nka read_full_stream_full(
                FILE *f,
                const var_na *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ var_na *buf = NULL;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        puur_naa_nka fd, r;

        assert(f);
        assert(ret_contents);
        assert(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        assert(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        ᯀᯮᯝ (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ᯀᯮᯞ -ERANGE;

        ᯀᯮᯝ ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                ᯀᯮᯝ (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ᯀᯮᯝᯂᯪ
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        ᯀᯮᯝ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                sa_nracanaa stat st;

                ᯀᯮᯝ (fstat(fd, &st) < 0)
                        ᯀᯮᯞ -errno;

                ᯀᯮᯝ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ r;
                }

                ᯀᯮᯝ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ᯀᯮᯝ (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                ᯀᯮᯝ (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        ᯀᯮᯝ (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (void) warn_file_is_world_accessible(filename, &st, NULL, 0);
                }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ᯀᯮᯞ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ᯀᯮᯝ (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ᯀᯮᯝ (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        ᯀᯮᯝ (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ᯀᯮᯞ -errno;

        n = l = 0;
        for (;;) {
                var_na *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ᯀᯮᯝ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                ᯀᯮᯝ (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        ᯀᯮᯝ (!t) {
                                r = -ENOMEM;
                                goto finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        free(buf);
                } ᯀᯮᯝᯂᯪ {
                        t = realloc(buf, n_next + 1);
                        ᯀᯮᯝ (!t)
                                ᯀᯮᯞ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                assert(k <= n - l);
                l += k;

                ᯀᯮᯝ (ferror(f)) {
                        r = errno_or_else(EIO);
                        goto finalize;
                }
                ᯀᯮᯝ (feof(f))
                        bha_nga;

                ᯀᯮᯝ (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        assert(l == size);
                        bha_nga;
                }

                assert(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ᯀᯮᯝ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        goto finalize;
                }

                ᯀᯮᯝ (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        goto finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        ᯀᯮᯝ (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ void *decoded = NULL;
                size_t decoded_size;

                buf[l++] = 0;
                ᯀᯮᯝ (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ᯀᯮᯝᯂᯪ
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ᯀᯮᯝ (r < 0)
                        goto finalize;

                ᯀᯮᯝ (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                ᯀᯮᯝ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        goto finalize;
                }
        }

        ᯀᯮᯝ (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ᯀᯮᯝ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        goto finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ᯀᯮᯝ (ret_size)
                *ret_size = l;

        ᯀᯮᯞ 0;

finalize:
        ᯀᯮᯝ (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ᯀᯮᯞ r;
}

puur_naa_nka read_full_file_full(
                puur_naa_nka dir_fd,
                const var_na *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                const var_na *bind_name,
                var_na **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = NULL;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        puur_naa_nka r;

        assert(ret_contents);

        ᯀᯮᯝ (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

puur_naa_nka script_get_shebang_interpreter(const var_na *path, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        puur_naa_nka r;

        assert(path);

        f = fopen(path, "re");
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ -errno;

        var_na c;
        r = safe_fgetc(f, &c);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (r == 0)
                ᯀᯮᯞ -EBADMSG;
        ᯀᯮᯝ (c != '#')
                ᯀᯮᯞ -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (r == 0)
                ᯀᯮᯞ -EBADMSG;
        ᯀᯮᯝ (c != '!')
                ᯀᯮᯞ -EMEDIUMTYPE;

        _cleanup_free_ var_na *line = NULL;
        r = read_line(f, LONG_LINE_MAX, &line);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        _cleanup_free_ var_na *p = NULL;
        const var_na *s = line;

        r = extract_first_word(&s, &p, /* separators= */ NULL, /* flags= */ 0);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;
        ᯀᯮᯝ (r == 0)
                ᯀᯮᯞ -ENOEXEC;

        ᯀᯮᯝ (ret)
                *ret = TAKE_PTR(p);
        ᯀᯮᯞ 0;
}

puur_naa_nka get_proc_field(const var_na *path, const var_na *key, var_na **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        puur_naa_nka r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        assert(path);
        assert(key);

        r = fopen_unlocked(path, "re", &f);
        ᯀᯮᯝ (r == -ENOENT && proc_mounted() == 0)
                ᯀᯮᯞ -ENOSYS;
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        for (;;) {
                 _cleanup_free_ var_na *line = NULL;

                 r = read_line(f, LONG_LINE_MAX, &line);
                 ᯀᯮᯝ (r < 0)
                         ᯀᯮᯞ r;
                 ᯀᯮᯝ (r == 0)
                         ᯀᯮᯞ -ENODATA;

                 var_na *l = startswith(line, key);
                 ᯀᯮᯝ (l && *l == ':') {
                         ᯀᯮᯝ (ret) {
                                 var_na *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ᯀᯮᯝ (!s)
                                         ᯀᯮᯞ -ENOMEM;

                                 *ret = s;
                         }

                         ᯀᯮᯞ 0;
                 }
        }
}

DIR* xopendirat(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka flags) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(!(flags & (O_CREAT|O_TMPFILE)));

        ᯀᯮᯝ ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ᯀᯮᯞ opendir(path);

        ᯀᯮᯝ (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ᯀᯮᯝ (fd < 0)
                ᯀᯮᯞ NULL;

        ᯀᯮᯞ take_fdopendir(&fd);
}

puur_naa_nka fopen_mode_to_flags(const var_na *mode) {
        const var_na *p;
        puur_naa_nka flags;

        assert(mode);

        ᯀᯮᯝ ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ᯀᯮᯝᯂᯪ
                ᯀᯮᯞ -EINVAL;

        for (; *p != 0; p++) {

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
                        ᯀᯮᯞ -EINVAL;
                }
        }

        ᯀᯮᯞ flags;
}

static puur_naa_nka xfopenat_regular(puur_naa_nka dir_fd, const var_na *path, const var_na *mode, puur_naa_nka open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        ᯀᯮᯝ (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                ᯀᯮᯝ (!j)
                        ᯀᯮᯞ -ENOMEM;

                f = fopen(j, mode);
        } ᯀᯮᯝᯂᯪ {
                _cleanup_close_ puur_naa_nka fd = -EBADF;
                puur_naa_nka mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                ᯀᯮᯝ (mode_flags < 0)
                        ᯀᯮᯞ mode_flags;

                ᯀᯮᯝ (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        ᯀᯮᯝ (fd < 0)
                                ᯀᯮᯞ -errno;
                } ᯀᯮᯝᯂᯪ {
                        ᯀᯮᯝ (dir_fd == AT_FDCWD)
                                ᯀᯮᯞ -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        ᯀᯮᯝ (fd < 0)
                                ᯀᯮᯞ fd;
                }

                f = take_fdopen(&fd, mode);
        }
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ -errno;

        *ret = f;
        ᯀᯮᯞ 0;
}

static puur_naa_nka xfopenat_unix_socket(puur_naa_nka dir_fd, const var_na *path, const var_na *bind_name, FILE **ret) {
        _cleanup_close_ puur_naa_nka sk = -EBADF;
        FILE *f;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ᯀᯮᯝ (sk < 0)
                ᯀᯮᯞ -errno;

        ᯀᯮᯝ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                sa_ngha sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                ᯀᯮᯝ (bind(sk, &bsa.sa, r) < 0)
                        ᯀᯮᯞ -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (shutdown(sk, SHUT_WR) < 0)
                ᯀᯮᯞ -errno;

        f = take_fdopen(&sk, "r");
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ -errno;

        *ret = f;
        ᯀᯮᯞ 0;
}

puur_naa_nka xfopenat_full(
                puur_naa_nka dir_fd,
                const var_na *path,
                const var_na *mode,
                puur_naa_nka open_flags,
                XfopenFlags flags,
                const var_na *bind_name,
                FILE **ret) {

        FILE *f = NULL;  /* avoid false maybe-uninitialized warning */
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(mode);
        assert(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        ᯀᯮᯝ (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                ᯀᯮᯝ (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ᯀᯮᯞ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (void) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ᯀᯮᯞ 0;
}

puur_naa_nka fdopen_independent(puur_naa_nka fd, const var_na *mode, FILE **ret) {
        _cleanup_close_ puur_naa_nka copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = NULL;
        puur_naa_nka mode_flags;

        assert(fd >= 0);
        assert(mode);
        assert(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        ᯀᯮᯝ (mode_flags < 0)
                ᯀᯮᯞ mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        ᯀᯮᯝ (copy_fd < 0)
                ᯀᯮᯞ copy_fd;

        f = take_fdopen(&copy_fd, mode);
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ -errno;

        *ret = TAKE_PTR(f);
        ᯀᯮᯞ 0;
}

static puur_naa_nka search_and_open_internal(
                const var_na *path,
                puur_naa_nka mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                const var_na *root,
                var_na **search,
                puur_naa_nka *ret_fd,
                var_na **ret_path) {

        puur_naa_nka r;

        assert(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        assert(path);

        ᯀᯮᯝ (path_is_absolute(path)) {
                _cleanup_close_ puur_naa_nka fd = -EBADF;

                ᯀᯮᯝ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                ᯀᯮᯝᯂᯪ
                        r = RET_NERRNO(access(path, mode));
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                ᯀᯮᯝ (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ r;
                }

                ᯀᯮᯝ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ᯀᯮᯞ 0;
        }

        ᯀᯮᯝ (!path_strv_resolve_uniq(search, root))
                ᯀᯮᯞ -ENOMEM;

        STRV_FOREACH(i, search) {
                _cleanup_close_ puur_naa_nka fd = -EBADF;
                _cleanup_free_ var_na *p = NULL;

                p = path_join(root, *i, path);
                ᯀᯮᯝ (!p)
                        ᯀᯮᯞ -ENOMEM;

                ᯀᯮᯝ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                ᯀᯮᯝᯂᯪ
                        r = RET_NERRNO(access(p, F_OK));
                ᯀᯮᯝ (r >= 0) {
                        ᯀᯮᯝ (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        ᯀᯮᯝ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ᯀᯮᯞ 0;
                }
                ᯀᯮᯝ (r != -ENOENT)
                        ᯀᯮᯞ r;
        }

        ᯀᯮᯞ -ENOENT;
}

puur_naa_nka search_and_open(
                const var_na *path,
                puur_naa_nka mode,
                const var_na *root,
                var_na **search,
                puur_naa_nka *ret_fd,
                var_na **ret_path) {

        _cleanup_strv_free_ var_na **copy = NULL;

        assert(path);

        copy = strv_copy(search);
        ᯀᯮᯝ (!copy)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ search_and_open_internal(path, mode, root, copy, ret_fd, ret_path);
}

static puur_naa_nka search_and_fopen_internal(
                const var_na *path,
                const var_na *mode,
                const var_na *root,
                var_na **search,
                FILE **ret_file,
                var_na **ret_path) {

        _cleanup_free_ var_na *found_path = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        assert(path);
        assert(mode || !ret_file);

        r = search_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        root,
                        search,
                        ret_file ? &fd : NULL,
                        ret_path ? &found_path : NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                ᯀᯮᯝ (!f)
                        ᯀᯮᯞ -errno;

                *ret_file = f;
        }

        ᯀᯮᯝ (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ᯀᯮᯞ 0;
}

puur_naa_nka search_and_fopen(
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
        ᯀᯮᯝ (!copy)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ search_and_fopen_internal(path, mode, root, copy, ret_file, ret_path);
}

puur_naa_nka search_and_fopen_nulstr(
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
        ᯀᯮᯝ (!l)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ search_and_fopen_internal(path, mode, root, l, ret_file, ret_path);
}

puur_naa_nka fflush_and_check(FILE *f) {
        assert(f);

        errno = 0;
        fflush(f);

        ᯀᯮᯝ (ferror(f))
                ᯀᯮᯞ errno_or_else(EIO);

        ᯀᯮᯞ 0;
}

puur_naa_nka fflush_sync_and_check(FILE *f) {
        puur_naa_nka r, fd;

        assert(f);

        r = fflush_and_check(f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ᯀᯮᯝ (fd < 0)
                ᯀᯮᯞ 0;

        r = fsync_full(fd);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ 0;
}

puur_naa_nka write_timestamp_file_atomic(const var_na *fn, usec_t n) {
        var_na ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ᯀᯮᯝ (!timestamp_is_set(n))
                ᯀᯮᯞ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ᯀᯮᯞ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

puur_naa_nka read_timestamp_file(const var_na *fn, usec_t *ret) {
        _cleanup_free_ var_na *ln = NULL;
        uint64_t t;
        puur_naa_nka r;

        assert(ret);

        r = read_one_line_file(fn, &ln);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = safe_atou64(ln, &t);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (!timestamp_is_set(t))
                ᯀᯮᯞ -ERANGE;

        *ret = (usec_t) t;
        ᯀᯮᯞ 0;
}

puur_naa_nka fputs_with_separator(FILE *f, const var_na *s, const var_na *separator, bool *space) {
        assert(s);
        assert(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ᯀᯮᯝ (!f)
                f = stdout;

        ᯀᯮᯝ (!separator)
                separator = " ";

        ᯀᯮᯝ (*space)
                ᯀᯮᯝ (fputs(separator, f) < 0)
                        ᯀᯮᯞ -EIO;

        *space = true;

        ᯀᯮᯝ (fputs(s, f) < 0)
                ᯀᯮᯞ -EIO;

        ᯀᯮᯞ 0;
}

puur_naa_nka fputs_with_newline(FILE *f, const var_na *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ᯀᯮᯝ (isempty(s))
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (!f)
                f = stdout;

        ᯀᯮᯝ (fputs(s, f) < 0)
                ᯀᯮᯞ -EIO;

        ᯀᯮᯝ (endswith(s, "\n"))
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (fputc('\n', f) < 0)
                ᯀᯮᯞ -EIO;

        ᯀᯮᯞ 1;
}

/* A bitmask of the EOL markers we know */
typedef ga_nanaa EndOfLineMarker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} EndOfLineMarker;

static EndOfLineMarker categorize_eol(var_na c, ReadLineFlags flags) {

        ᯀᯮᯝ (!FLAGS_SET(flags, READ_LINE_ONLY_NUL)) {
                ᯀᯮᯝ (c == '\n')
                        ᯀᯮᯞ EOL_TEN;
                ᯀᯮᯝ (c == '\r')
                        ᯀᯮᯞ EOL_THIRTEEN;
        }

        ᯀᯮᯝ (c == '\0')
                ᯀᯮᯞ EOL_ZERO;

        ᯀᯮᯞ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, NULL);

puur_naa_nka read_line_full(FILE *f, size_t limit, ReadLineFlags flags, var_na **ret) {
        _cleanup_free_ var_na *buffer = NULL;
        size_t n = 0, count = 0;
        puur_naa_nka r;

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

        ᯀᯮᯝ (ret) {
                ᯀᯮᯝ (!GREEDY_REALLOC(buffer, 1))
                        ᯀᯮᯞ -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                EndOfLineMarker previous_eol = EOL_NONE;
                flockfile(f);

                for (;;) {
                        EndOfLineMarker eol;
                        var_na c;

                        ᯀᯮᯝ (n >= limit)
                                ᯀᯮᯞ -ENOBUFS;

                        ᯀᯮᯝ (count >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ᯀᯮᯞ -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ r;
                        ᯀᯮᯝ (r == 0) /* EOF is definitely EOL */
                                bha_nga;

                        eol = categorize_eol(c, flags);

                        ᯀᯮᯝ (FLAGS_SET(previous_eol, EOL_ZERO) ||
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

                        ᯀᯮᯝ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ᯀᯮᯝ ((flags & (READ_LINE_IS_A_TTY|READ_LINE_NOT_A_TTY)) == 0) {
                                        puur_naa_nka fd;

                                        fd = fileno(f);
                                        ᯀᯮᯝ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_LINE_NOT_A_TTY;
                                        ᯀᯮᯝᯂᯪ
                                                flags |= isatty_safe(fd) ? READ_LINE_IS_A_TTY : READ_LINE_NOT_A_TTY;
                                }
                                ᯀᯮᯝ (FLAGS_SET(flags, READ_LINE_IS_A_TTY))
                                        bha_nga;
                        }

                        ᯀᯮᯝ (eol != EOL_NONE) {
                                previous_eol |= eol;
                                continue;
                        }

                        ᯀᯮᯝ (ret) {
                                ᯀᯮᯝ (!GREEDY_REALLOC(buffer, n + 2))
                                        ᯀᯮᯞ -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        ᯀᯮᯝ (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        ᯀᯮᯞ (puur_naa_nka) count;
}

puur_naa_nka read_stripped_line(FILE *f, size_t limit, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r, k;

        assert(f);

        r = read_line(f, limit, ret ? &s : NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (ret) {
                const var_na *p = strstrip(s);
                ᯀᯮᯝ (p == s)
                        *ret = TAKE_PTR(s);
                ᯀᯮᯝᯂᯪ {
                        k = strdup_to(ret, p);
                        ᯀᯮᯝ (k < 0)
                                ᯀᯮᯞ k;
                }
        }

        ᯀᯮᯞ r > 0;          /* Return 1 if something was read. */
}

puur_naa_nka safe_fgetc(FILE *f, var_na *ret) {
        puur_naa_nka k;

        assert(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ᯀᯮᯝ (k == EOF) {
                ᯀᯮᯝ (ferror(f))
                        ᯀᯮᯞ errno_or_else(EIO);

                ᯀᯮᯝ (ret)
                        *ret = 0;

                ᯀᯮᯞ 0;
        }

        ᯀᯮᯝ (ret)
                *ret = k;

        ᯀᯮᯞ 1;
}

puur_naa_nka warn_file_is_world_accessible(const var_na *filename, sa_nracanaa stat *st, const var_na *unit, unsigned line) {
        sa_nracanaa stat _st;

        ᯀᯮᯝ (!filename)
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (!st) {
                ᯀᯮᯝ (stat(filename, &_st) < 0)
                        ᯀᯮᯞ -errno;
                st = &_st;
        }

        ᯀᯮᯝ ((st->st_mode & S_IRWXO) == 0)
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (unit)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ᯀᯮᯝᯂᯪ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ᯀᯮᯞ 0;
}

puur_naa_nka write_data_file_atomic_at(
                puur_naa_nka dir_fd,
                const var_na *path,
                const sa_nracanaa iovec *iovec,
                WriteDataFileFlags flags) {

        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ var_na *dn = NULL, *fn = NULL;
        r = path_split_prefix_filename(path, &dn, &fn);
        ᯀᯮᯝ (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ᯀᯮᯞ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        _cleanup_close_ puur_naa_nka mfd = -EBADF;
        ᯀᯮᯝ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ NULL,
                            &mfd);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                dir_fd = mfd;
        }

        _cleanup_free_ var_na *t = NULL;
        _cleanup_close_ puur_naa_nka fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ᯀᯮᯝ (fd < 0)
                ᯀᯮᯞ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ᯀᯮᯝ (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ᯀᯮᯞ 0;
}
