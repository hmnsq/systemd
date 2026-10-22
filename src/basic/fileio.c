/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio_ext.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "label-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mkdir.h"
#અન્તર્ભાવ "nulstr-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "socket-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "sync-util.h"
#અન્તર્ભાવ "terminal-util.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "tmpfile-util.h"

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

પૂર્ણાંક fdopen_unlocked(પૂર્ણાંક fd, અચલ અક્ષર *options, FILE **ret) {
        પ્રતિજ્ઞા(ret);

        FILE *f = fdopen(fd, options);
        જો (!f)
                પાછા -errno;

        (શૂન્ય) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        પાછા 0;
}

પૂર્ણાંક take_fdopen_unlocked(પૂર્ણાંક *fd, અચલ અક્ષર *options, FILE **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd);

        r = fdopen_unlocked(*fd, options, ret);
        જો (r < 0)
                પાછા r;

        *fd = -EBADF;

        પાછા 0;
}

FILE* take_fdopen(પૂર્ણાંક *fd, અચલ અક્ષર *options) {
        પ્રતિજ્ઞા(fd);

        FILE *f = fdopen(*fd, options);
        જો (!f)
                પાછા ખાલી;

        *fd = -EBADF;

        પાછા f;
}

DIR* take_fdopendir(પૂર્ણાંક *dfd) {
        પ્રતિજ્ઞા(dfd);

        DIR *d = fdopendir(*dfd);
        જો (!d)
                પાછા ખાલી;

        *dfd = -EBADF;

        પાછા d;
}

FILE* open_memstream_unlocked(અક્ષર **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        જો (!f)
                પાછા ખાલી;

        (શૂન્ય) __fsetlocking(f, FSETLOCKING_BYCALLER);

        પાછા f;
}

FILE* fmemopen_unlocked(શૂન્ય *buf, size_t size, અચલ અક્ષર *mode) {
        FILE *f = fmemopen(buf, size, mode);
        જો (!f)
                પાછા ખાલી;

        (શૂન્ય) __fsetlocking(f, FSETLOCKING_BYCALLER);

        પાછા f;
}

પૂર્ણાંક write_string_stream_full(
                FILE *f,
                અચલ અક્ષર *લીટી,
                WriteStringFileFlags flags,
                અચલ સંરચના timespec *ts) {

        bool needs_nl;
        પૂર્ણાંક r, fd = -EBADF;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(લીટી);

        જો (ferror(f))
                પાછા -EIO;

        જો (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                જો (fd < 0)
                        પાછા -EBADF;
        }

        જો (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ અક્ષર *t = ખાલી;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                જો (fd < 0) {
                        fd = fileno(f);
                        જો (fd < 0)
                                પાછા -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                જો (read_virtual_file_fd(fd, strlen(લીટી)+1, &t, ખાલી) > 0 &&
                    streq_skip_trailing_chars(લીટી, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", લીટી);
                        પાછા 0;
                }

                જો (lseek(fd, 0, SEEK_SET) < 0)
                        પાછા -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(લીટી, "\n");

        જો (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                લીટી = strjoina(લીટી, "\n");
                needs_nl = false;
        }

        જો (fputs(લીટી, f) == EOF)
                પાછા -errno;

        જો (needs_nl)
                જો (fputc('\n', f) == EOF)
                        પાછા -errno;

        જો (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        નહીંતર
                r = fflush_and_check(f);
        જો (r < 0)
                પાછા r;

        જો (ts) {
                અચલ સંરચના timespec twice[2] = {*ts, *ts};

                પ્રતિજ્ઞા(fd >= 0);
                જો (futimens(fd, twice) < 0)
                        પાછા -errno;
        }

        પાછા 0;
}

સ્થિર mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        પાછા FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

સ્થિર પૂર્ણાંક write_string_file_atomic_at(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *fn,
                અચલ અક્ષર *લીટી,
                WriteStringFileFlags flags,
                અચલ સંરચના timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fn);
        પ્રતિજ્ઞા(લીટી);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        જો (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                જો (r < 0)
                        પાછા r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        પૂર્ણાંક k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ખાલી : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        જો (r < 0)
                પાછા r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        જો (k < 0)
                પાછા k;

        r = write_string_stream_full(f, લીટી, flags, ts);
        જો (r < 0)
                પાછા r;

        r = fchmod_umask(fileno(f), mode);
        જો (r < 0)
                પાછા r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        જો (r < 0)
                પાછા r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        જો (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક write_string_file_full_label(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *fn,
                અચલ અક્ષર *લીટી,
                WriteStringFileFlags flags,
                અચલ સંરચના timespec *ts,
                અચલ અક્ષર *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd == AT_FDCWD || dir_fd >= 0);
        પ્રતિજ્ઞા(લીટી);

        /* We don't know how to verify whether the file contents was already on-disk. */
        પ્રતિજ્ઞા(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        જો (flags & WRITE_STRING_FILE_MKDIR_0755) {
                પ્રતિજ્ઞા(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                જો (r < 0)
                        પાછા r;
        }

        જો (flags & WRITE_STRING_FILE_ATOMIC) {
                પ્રતિજ્ઞા(fn);
                પ્રતિજ્ઞા(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, લીટી, flags, ts, label_context);
                જો (r < 0)
                        જાઓ fail;

                પાછા r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        જો (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        નહીંતર {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                જો (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        જો (r < 0)
                                જાઓ fail;

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
                જો (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ખાલી : fn, made_file, label_context));
        }
        જો (r < 0)
                જાઓ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        જો (r < 0)
                જાઓ fail;

        જો (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, ખાલી, _IONBF, 0);

        r = write_string_stream_full(f, લીટી, flags, ts);
        જો (r < 0)
                જાઓ fail;

        પાછા 0;

fail:
        જો (made_file)
                (શૂન્ય) unlinkat(dir_fd, fn, 0);

        જો (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                પાછા r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        જો (verify_file_at(dir_fd, fn, લીટી, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                પાછા 0;

        પાછા r;
}

પૂર્ણાંક write_string_filef(
                અચલ અક્ષર *fn,
                WriteStringFileFlags flags,
                અચલ અક્ષર *format, ...) {

        _cleanup_free_ અક્ષર *p = ખાલી;
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        જો (r < 0)
                પાછા -ENOMEM;

        પાછા write_string_file(fn, p, flags);
}

પૂર્ણાંક write_string_filef_at(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *fn,
                WriteStringFileFlags flags,
                અચલ અક્ષર *format, ...) {

        _cleanup_free_ અક્ષર *p = ખાલી;
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        જો (r < 0)
                પાછા -ENOMEM;

        પાછા write_string_file_at(dir_fd, fn, p, flags);
}

પૂર્ણાંક write_base64_file_at(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *fn,
                અચલ સંરચના iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ અક્ષર *encoded = ખાલી;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : ખાલી, data ? data->iov_len : 0, 79, &encoded);
        જો (n < 0)
                પાછા n;

        પાછા write_string_file_at(dir_fd, fn, encoded, flags);
}

પૂર્ણાંક read_one_લીટી_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename, અક્ષર **ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(filename);
        પ્રતિજ્ઞા(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        જો (r < 0)
                પાછા r;

        પાછા read_લીટી(f, LONG_લીટી_MAX, ret);
}

પૂર્ણાંક read_boolean_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(filename);

        r = read_one_લીટી_file_at(dir_fd, filename, &s);
        જો (r < 0)
                પાછા r;

        પાછા parse_boolean(s);
}

પૂર્ણાંક verify_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, અચલ અક્ષર *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_free_ અક્ષર *buf = ખાલી;
        size_t l, k;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(blob);

        l = strlen(blob);

        જો (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        જો (!buf)
                પાછા -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        જો (r < 0)
                પાછા r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        જો (ferror(f))
                પાછા errno_or_else(EIO);

        જો (k != l && k != l + accept_extra_nl)
                પાછા 0;
        જો (memcmp(buf, blob, l) != 0)
                પાછા 0;
        જો (k > l && buf[l] != '\n')
                પાછા 0;

        પાછા 1;
}

પૂર્ણાંક read_virtual_file_at(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *filename,
                size_t max_size,
                અક્ષર **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ અક્ષર *buf = ખાલી;
        size_t n, size;
        પૂર્ણાંક n_retries;
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

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        જો (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        નહીંતર
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        જો (fd < 0)
                પાછા fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        માટે (;;) {
                સંરચના stat st;

                જો (fstat(fd, &st) < 0)
                        પાછા -errno;

                જો (!S_ISREG(st.st_mode))
                        પાછા -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                જો (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        જો (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                જો (max_size == SIZE_MAX)
                                        પાછા -EFBIG;

                                size = max_size;
                        } નહીંતર {
                                size = MIN((size_t) st.st_size, max_size);

                                જો (size > READ_VIRTUAL_BYTES_MAX)
                                        પાછા -EFBIG;
                        }

                        n_retries--;
                } નહીંતર જો (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } નહીંતર {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                જો (!buf)
                        પાછા -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                માટે (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = પઠ(fd, buf, size + 1);
                        જો (k >= 0) {
                                n = k;
                                તોડો;
                        }

                        જો (errno != EINTR)
                                પાછા -errno;
                }

                /* Consider a short read as EOF */
                જો (n <= size)
                        તોડો;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                જો (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        તોડો;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                જો (n_retries <= 0)
                        પાછા -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                જો (lseek(fd, 0, SEEK_SET) < 0)
                        પાછા -errno;

                buf = mfree(buf);
        }

        જો (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                જો (!ret_size && memchr(buf, 0, n))
                        પાછા -EBADMSG;

                જો (n < size) {
                        અક્ષર *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        જો (!p)
                                પાછા -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        જો (ret_size)
                *ret_size = n;

        પાછા !truncated;
}

પૂર્ણાંક read_full_stream_full(
                FILE *f,
                અચલ અક્ષર *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                અક્ષર **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ અક્ષર *buf = ખાલી;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        પૂર્ણાંક fd, r;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(ret_contents);
        પ્રતિજ્ઞા(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        પ્રતિજ્ઞા(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        જો (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                પાછા -ERANGE;

        જો ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                જો (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                નહીંતર
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        જો (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                સંરચના stat st;

                જો (fstat(fd, &st) < 0)
                        પાછા -errno;

                જો (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        જો (r < 0)
                                પાછા r;
                }

                જો (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        જો (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                જો (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        જો (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (શૂન્ય) warn_file_is_world_accessible(filename, &st, ખાલી, 0);
                }
        } નહીંતર જો (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                પાછા -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        જો (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        જો (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        જો (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                પાછા -errno;

        n = l = 0;
        માટે (;;) {
                અક્ષર *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                જો (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                જો (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        જો (!t) {
                                r = -ENOMEM;
                                જાઓ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        મુક્ત(buf);
                } નહીંતર {
                        t = realloc(buf, n_next + 1);
                        જો (!t)
                                પાછા -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                પ્રતિજ્ઞા(k <= n - l);
                l += k;

                જો (ferror(f)) {
                        r = errno_or_else(EIO);
                        જાઓ finalize;
                }
                જો (feof(f))
                        તોડો;

                જો (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        પ્રતિજ્ઞા(l == size);
                        તોડો;
                }

                પ્રતિજ્ઞા(k > 0); /* we can't have read zero bytes because that would have been EOF */

                જો (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        જાઓ finalize;
                }

                જો (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        જાઓ finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        જો (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ શૂન્ય *decoded = ખાલી;
                size_t decoded_size;

                buf[l++] = 0;
                જો (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                નહીંતર
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                જો (r < 0)
                        જાઓ finalize;

                જો (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                જો (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        જાઓ finalize;
                }
        }

        જો (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                જો (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        જાઓ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        જો (ret_size)
                *ret_size = l;

        પાછા 0;

finalize:
        જો (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        પાછા r;
}

પૂર્ણાંક read_full_file_full(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                અચલ અક્ષર *bind_name,
                અક્ષર **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = ખાલી;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_contents);

        જો (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        જો (r < 0)
                પાછા r;

        પાછા read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

પૂર્ણાંક script_get_shebang_interpreter(અચલ અક્ષર *path, અક્ષર **ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        f = fopen(path, "re");
        જો (!f)
                પાછા -errno;

        અક્ષર c;
        r = safe_fgetc(f, &c);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EBADMSG;
        જો (c != '#')
                પાછા -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EBADMSG;
        જો (c != '!')
                પાછા -EMEDIUMTYPE;

        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
        જો (r < 0)
                પાછા r;

        _cleanup_free_ અક્ષર *p = ખાલી;
        અચલ અક્ષર *s = લીટી;

        r = extract_first_word(&s, &p, /* separators= */ ખાલી, /* flags= */ 0);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -ENOEXEC;

        જો (ret)
                *ret = TAKE_PTR(p);
        પાછા 0;
}

પૂર્ણાંક get_proc_field(અચલ અક્ષર *path, અચલ અક્ષર *ચાવી, અક્ષર **ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(ચાવી);

        r = fopen_unlocked(path, "re", &f);
        જો (r == -ENOENT && proc_mounted() == 0)
                પાછા -ENOSYS;
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                 _cleanup_free_ અક્ષર *લીટી = ખાલી;

                 r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
                 જો (r < 0)
                         પાછા r;
                 જો (r == 0)
                         પાછા -ENODATA;

                 અક્ષર *l = startswith(લીટી, ચાવી);
                 જો (l && *l == ':') {
                         જો (ret) {
                                 અક્ષર *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 જો (!s)
                                         પાછા -ENOMEM;

                                 *ret = s;
                         }

                         પાછા 0;
                 }
        }
}

DIR* xopendirat(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક flags) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(!(flags & (O_CREAT|O_TMPFILE)));

        જો ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                પાછા opendir(path);

        જો (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        જો (fd < 0)
                પાછા ખાલી;

        પાછા take_fdopendir(&fd);
}

પૂર્ણાંક fopen_mode_to_flags(અચલ અક્ષર *mode) {
        અચલ અક્ષર *p;
        પૂર્ણાંક flags;

        પ્રતિજ્ઞા(mode);

        જો ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        નહીંતર જો ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        નહીંતર જો ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        નહીંતર જો ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        નહીંતર જો ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        નહીંતર જો ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        નહીંતર
                પાછા -EINVAL;

        માટે (; *p != 0; p++) {

                પસંદગી (*p) {

                સ્થિતિ 'e':
                        flags |= O_CLOEXEC;
                        તોડો;

                સ્થિતિ 'x':
                        flags |= O_EXCL;
                        તોડો;

                સ્થિતિ 'm':
                        /* ignore this here, fdopen() might care later though */
                        તોડો;

                સ્થિતિ 'c': /* not sure what to do about this one */
                મૂળભૂત:
                        પાછા -EINVAL;
                }
        }

        પાછા flags;
}

સ્થિર પૂર્ણાંક xfopenat_regular(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *mode, પૂર્ણાંક open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(mode);
        પ્રતિજ્ઞા(ret);

        જો (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        નહીંતર જો (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ અક્ષર *j = strjoin("/", path);
                જો (!j)
                        પાછા -ENOMEM;

                f = fopen(j, mode);
        } નહીંતર {
                _cleanup_close_ પૂર્ણાંક fd = -EBADF;
                પૂર્ણાંક mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                જો (mode_flags < 0)
                        પાછા mode_flags;

                જો (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        જો (fd < 0)
                                પાછા -errno;
                } નહીંતર {
                        જો (dir_fd == AT_FDCWD)
                                પાછા -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        જો (fd < 0)
                                પાછા fd;
                }

                f = take_fdopen(&fd, mode);
        }
        જો (!f)
                પાછા -errno;

        *ret = f;
        પાછા 0;
}

સ્થિર પૂર્ણાંક xfopenat_unix_socket(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *bind_name, FILE **ret) {
        _cleanup_close_ પૂર્ણાંક sk = -EBADF;
        FILE *f;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        જો (sk < 0)
                પાછા -errno;

        જો (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                સંઘ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                જો (r < 0)
                        પાછા r;

                જો (bind(sk, &bsa.sa, r) < 0)
                        પાછા -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        જો (r < 0)
                પાછા r;

        જો (shutdown(sk, SHUT_WR) < 0)
                પાછા -errno;

        f = take_fdopen(&sk, "r");
        જો (!f)
                પાછા -errno;

        *ret = f;
        પાછા 0;
}

પૂર્ણાંક xfopenat_full(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *path,
                અચલ અક્ષર *mode,
                પૂર્ણાંક open_flags,
                XfopenFlags flags,
                અચલ અક્ષર *bind_name,
                FILE **ret) {

        FILE *f = ખાલી;  /* avoid false maybe-uninitialized warning */
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(mode);
        પ્રતિજ્ઞા(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        જો (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                જો (IN_SET(r, -ENOTSOCK, -EINVAL))
                        પાછા -ENXIO; /* propagate original error if this is not a socket after all */
        }
        જો (r < 0)
                પાછા r;

        જો (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (શૂન્ય) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        પાછા 0;
}

પૂર્ણાંક fdopen_independent(પૂર્ણાંક fd, અચલ અક્ષર *mode, FILE **ret) {
        _cleanup_close_ પૂર્ણાંક copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક mode_flags;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(mode);
        પ્રતિજ્ઞા(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        જો (mode_flags < 0)
                પાછા mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        જો (copy_fd < 0)
                પાછા copy_fd;

        f = take_fdopen(&copy_fd, mode);
        જો (!f)
                પાછા -errno;

        *ret = TAKE_PTR(f);
        પાછા 0;
}

સ્થિર પૂર્ણાંક શોધ_and_open_internal(
                અચલ અક્ષર *path,
                પૂર્ણાંક mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                અચલ અક્ષર *મૂળ,
                અક્ષર **શોધ,
                પૂર્ણાંક *ret_fd,
                અક્ષર **ret_path) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        પ્રતિજ્ઞા(path);

        જો (path_is_absolute(path)) {
                _cleanup_close_ પૂર્ણાંક fd = -EBADF;

                જો (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                નહીંતર
                        r = RET_NERRNO(access(path, mode));
                જો (r < 0)
                        પાછા r;

                જો (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        જો (r < 0)
                                પાછા r;
                }

                જો (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                પાછા 0;
        }

        જો (!path_strv_resolve_uniq(શોધ, મૂળ))
                પાછા -ENOMEM;

        STRV_FOREACH(i, શોધ) {
                _cleanup_close_ પૂર્ણાંક fd = -EBADF;
                _cleanup_free_ અક્ષર *p = ખાલી;

                p = path_join(મૂળ, *i, path);
                જો (!p)
                        પાછા -ENOMEM;

                જો (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                નહીંતર
                        r = RET_NERRNO(access(p, F_OK));
                જો (r >= 0) {
                        જો (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        જો (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        પાછા 0;
                }
                જો (r != -ENOENT)
                        પાછા r;
        }

        પાછા -ENOENT;
}

પૂર્ણાંક શોધ_and_open(
                અચલ અક્ષર *path,
                પૂર્ણાંક mode,
                અચલ અક્ષર *મૂળ,
                અક્ષર **શોધ,
                પૂર્ણાંક *ret_fd,
                અક્ષર **ret_path) {

        _cleanup_strv_free_ અક્ષર **copy = ખાલી;

        પ્રતિજ્ઞા(path);

        copy = strv_copy(શોધ);
        જો (!copy)
                પાછા -ENOMEM;

        પાછા શોધ_and_open_internal(path, mode, મૂળ, copy, ret_fd, ret_path);
}

સ્થિર પૂર્ણાંક શોધ_and_fopen_internal(
                અચલ અક્ષર *path,
                અચલ અક્ષર *mode,
                અચલ અક્ષર *મૂળ,
                અક્ષર **શોધ,
                FILE **ret_file,
                અક્ષર **ret_path) {

        _cleanup_free_ અક્ષર *found_path = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(mode || !ret_file);

        r = શોધ_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        મૂળ,
                        શોધ,
                        ret_file ? &fd : ખાલી,
                        ret_path ? &found_path : ખાલી);
        જો (r < 0)
                પાછા r;

        જો (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                જો (!f)
                        પાછા -errno;

                *ret_file = f;
        }

        જો (ret_path)
                *ret_path = TAKE_PTR(found_path);

        પાછા 0;
}

પૂર્ણાંક શોધ_and_fopen(
                અચલ અક્ષર *path,
                અચલ અક્ષર *mode,
                અચલ અક્ષર *મૂળ,
                અચલ અક્ષર **શોધ,
                FILE **ret_file,
                અક્ષર **ret_path) {

        _cleanup_strv_free_ અક્ષર **copy = ખાલી;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(mode || !ret_file);

        copy = strv_copy((અક્ષર**) શોધ);
        જો (!copy)
                પાછા -ENOMEM;

        પાછા શોધ_and_fopen_internal(path, mode, મૂળ, copy, ret_file, ret_path);
}

પૂર્ણાંક શોધ_and_fopen_nulstr(
                અચલ અક્ષર *path,
                અચલ અક્ષર *mode,
                અચલ અક્ષર *મૂળ,
                અચલ અક્ષર *શોધ,
                FILE **ret_file,
                અક્ષર **ret_path) {

        _cleanup_strv_free_ અક્ષર **l = ખાલી;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(mode || !ret_file);

        l = strv_split_nulstr(શોધ);
        જો (!l)
                પાછા -ENOMEM;

        પાછા શોધ_and_fopen_internal(path, mode, મૂળ, l, ret_file, ret_path);
}

પૂર્ણાંક fflush_and_check(FILE *f) {
        પ્રતિજ્ઞા(f);

        errno = 0;
        fflush(f);

        જો (ferror(f))
                પાછા errno_or_else(EIO);

        પાછા 0;
}

પૂર્ણાંક fflush_sync_and_check(FILE *f) {
        પૂર્ણાંક r, fd;

        પ્રતિજ્ઞા(f);

        r = fflush_and_check(f);
        જો (r < 0)
                પાછા r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        જો (fd < 0)
                પાછા 0;

        r = fsync_full(fd);
        જો (r < 0)
                પાછા r;

        પાછા 0;
}

પૂર્ણાંક write_timestamp_file_atomic(અચલ અક્ષર *fn, usec_t n) {
        અક્ષર ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        જો (!timestamp_is_set(n))
                પાછા -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        પાછા write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

પૂર્ણાંક read_timestamp_file(અચલ અક્ષર *fn, usec_t *ret) {
        _cleanup_free_ અક્ષર *ln = ખાલી;
        uint64_t t;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = read_one_લીટી_file(fn, &ln);
        જો (r < 0)
                પાછા r;

        r = safe_atou64(ln, &t);
        જો (r < 0)
                પાછા r;

        જો (!timestamp_is_set(t))
                પાછા -ERANGE;

        *ret = (usec_t) t;
        પાછા 0;
}

પૂર્ણાંક fputs_with_separator(FILE *f, અચલ અક્ષર *s, અચલ અક્ષર *separator, bool *space) {
        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        જો (!f)
                f = stdout;

        જો (!separator)
                separator = " ";

        જો (*space)
                જો (fputs(separator, f) < 0)
                        પાછા -EIO;

        *space = true;

        જો (fputs(s, f) < 0)
                પાછા -EIO;

        પાછા 0;
}

પૂર્ણાંક fputs_with_newline(FILE *f, અચલ અક્ષર *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        જો (isempty(s))
                પાછા 0;

        જો (!f)
                f = stdout;

        જો (fputs(s, f) < 0)
                પાછા -EIO;

        જો (endswith(s, "\n"))
                પાછા 0;

        જો (fputc('\n', f) < 0)
                પાછા -EIO;

        પાછા 1;
}

/* A bitmask of the EOL markers we know */
પ્રકારવ્યાખ્યા ગણના End_Of_લીટી_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_લીટી_Marker;

સ્થિર End_Of_લીટી_Marker categorize_eol(અક્ષર c, Read_લીટી_Flags flags) {

        જો (!FLAGS_SET(flags, READ_લીટી_ONLY_NUL)) {
                જો (c == '\n')
                        પાછા EOL_TEN;
                જો (c == '\r')
                        પાછા EOL_THIRTEEN;
        }

        જો (c == '\0')
                પાછા EOL_ZERO;

        પાછા EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, ખાલી);

પૂર્ણાંક read_લીટી_full(FILE *f, size_t limit, Read_લીટી_Flags flags, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *buffer = ખાલી;
        size_t n = 0, ગણતરી = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f);

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

        જો (ret) {
                જો (!GREEDY_REALLOC(buffer, 1))
                        પાછા -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_લીટી_Marker previous_eol = EOL_NONE;
                flockfile(f);

                માટે (;;) {
                        End_Of_લીટી_Marker eol;
                        અક્ષર c;

                        જો (n >= limit)
                                પાછા -ENOBUFS;

                        જો (ગણતરી >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                પાછા -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0) /* EOF is definitely EOL */
                                તોડો;

                        eol = categorize_eol(c, flags);

                        જો (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned અક્ષર) c, f) != EOF);
                                તોડો;
                        }

                        ગણતરી++;

                        જો (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                જો ((flags & (READ_લીટી_IS_A_TTY|READ_લીટી_NOT_A_TTY)) == 0) {
                                        પૂર્ણાંક fd;

                                        fd = fileno(f);
                                        જો (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_લીટી_NOT_A_TTY;
                                        નહીંતર
                                                flags |= isatty_safe(fd) ? READ_લીટી_IS_A_TTY : READ_લીટી_NOT_A_TTY;
                                }
                                જો (FLAGS_SET(flags, READ_લીટી_IS_A_TTY))
                                        તોડો;
                        }

                        જો (eol != EOL_NONE) {
                                previous_eol |= eol;
                                ચાલુ;
                        }

                        જો (ret) {
                                જો (!GREEDY_REALLOC(buffer, n + 2))
                                        પાછા -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        જો (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        પાછા (પૂર્ણાંક) ગણતરી;
}

પૂર્ણાંક read_stripped_લીટી(FILE *f, size_t limit, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r, k;

        પ્રતિજ્ઞા(f);

        r = read_લીટી(f, limit, ret ? &s : ખાલી);
        જો (r < 0)
                પાછા r;

        જો (ret) {
                અચલ અક્ષર *p = strstrip(s);
                જો (p == s)
                        *ret = TAKE_PTR(s);
                નહીંતર {
                        k = strdup_to(ret, p);
                        જો (k < 0)
                                પાછા k;
                }
        }

        પાછા r > 0;          /* Return 1 if something was read. */
}

પૂર્ણાંક safe_fgetc(FILE *f, અક્ષર *ret) {
        પૂર્ણાંક k;

        પ્રતિજ્ઞા(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        જો (k == EOF) {
                જો (ferror(f))
                        પાછા errno_or_else(EIO);

                જો (ret)
                        *ret = 0;

                પાછા 0;
        }

        જો (ret)
                *ret = k;

        પાછા 1;
}

પૂર્ણાંક warn_file_is_world_accessible(અચલ અક્ષર *filename, સંરચના stat *st, અચલ અક્ષર *unit, unsigned લીટી) {
        સંરચના stat _st;

        જો (!filename)
                પાછા 0;

        જો (!st) {
                જો (stat(filename, &_st) < 0)
                        પાછા -errno;
                st = &_st;
        }

        જો ((st->st_mode & S_IRWXO) == 0)
                પાછા 0;

        જો (unit)
                log_syntax(unit, LOG_WARNING, filename, લીટી, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        નહીંતર
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        પાછા 0;
}

પૂર્ણાંક write_data_file_atomic_at(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *path,
                અચલ સંરચના iovec *iovec,
                WriteDataFileFlags flags) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ અક્ષર *dn = ખાલી, *fn = ખાલી;
        r = path_split_prefix_filename(path, &dn, &fn);
        જો (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                પાછા -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        જો (r < 0)
                પાછા r;

        _cleanup_close_ પૂર્ણાંક mfd = -EBADF;
        જો (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ખાલી,
                            &mfd);
                જો (r < 0)
                        પાછા r;

                dir_fd = mfd;
        }

        _cleanup_free_ અક્ષર *t = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        જો (fd < 0)
                પાછા fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        જો (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                જો (r < 0)
                        પાછા r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        જો (r < 0)
                પાછા r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        જો (r < 0)
                પાછા r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        પાછા 0;
}
