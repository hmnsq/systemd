/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio_ext.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "label-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mkdir.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "sync-util.h"
#ಅನ್ತರ್ಭಾವ "terminal-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "tmpfile-util.h"

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

ಪೂರ್ಣಾಙ್ಕ fdopen_unlocked(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *options, FILE **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        FILE *f = fdopen(fd, options);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ take_fdopen_unlocked(ಪೂರ್ಣಾಙ್ಕ *fd, ಧ್ರುವ ವರ್ಣ *options, FILE **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *fd = -EBADF;

        ಪ್ರತ್ಯಾಗಮ 0;
}

FILE* take_fdopen(ಪೂರ್ಣಾಙ್ಕ *fd, ಧ್ರುವ ವರ್ಣ *options) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        FILE *f = fdopen(*fd, options);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *fd = -EBADF;

        ಪ್ರತ್ಯಾಗಮ f;
}

DIR* take_fdopendir(ಪೂರ್ಣಾಙ್ಕ *dfd) {
        ಪ್ರತಿಜ್ಞಾ(dfd);

        DIR *d = fdopendir(*dfd);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *dfd = -EBADF;

        ಪ್ರತ್ಯಾಗಮ d;
}

FILE* open_memstream_unlocked(ವರ್ಣ **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ಪ್ರತ್ಯಾಗಮ f;
}

FILE* fmemopen_unlocked(ಶೂನ್ಯ *buf, size_t size, ಧ್ರುವ ವರ್ಣ *mode) {
        FILE *f = fmemopen(buf, size, mode);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ಪ್ರತ್ಯಾಗಮ f;
}

ಪೂರ್ಣಾಙ್ಕ write_string_stream_full(
                FILE *f,
                ಧ್ರುವ ವರ್ಣ *ಸಾಲು,
                WriteStringFileFlags flags,
                ಧ್ರುವ ಸಂರಚನಾ timespec *ts) {

        bool needs_nl;
        ಪೂರ್ಣಾಙ್ಕ r, fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು);

        ಯದಿ (ferror(f))
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -EBADF;
        }

        ಯದಿ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ಯದಿ (fd < 0) {
                        fd = fileno(f);
                        ಯದಿ (fd < 0)
                                ಪ್ರತ್ಯಾಗಮ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ಯದಿ (read_virtual_file_fd(fd, strlen(ಸಾಲು)+1, &t, ರಿಕ್ತ) > 0 &&
                    streq_skip_trailing_chars(ಸಾಲು, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", ಸಾಲು);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                ಯದಿ (lseek(fd, 0, SEEK_SET) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(ಸಾಲು, "\n");

        ಯದಿ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                ಸಾಲು = strjoina(ಸಾಲು, "\n");
                needs_nl = false;
        }

        ಯದಿ (fputs(ಸಾಲು, f) == EOF)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (needs_nl)
                ಯದಿ (fputc('\n', f) == EOF)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ಅನ್ಯಥಾ
                r = fflush_and_check(f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ts) {
                ಧ್ರುವ ಸಂರಚನಾ timespec twice[2] = {*ts, *ts};

                ಪ್ರತಿಜ್ಞಾ(fd >= 0);
                ಯದಿ (futimens(fd, twice) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_string_file_atomic_at(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *fn,
                ಧ್ರುವ ವರ್ಣ *ಸಾಲು,
                WriteStringFileFlags flags,
                ಧ್ರುವ ಸಂರಚನಾ timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fn);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        ಯದಿ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        ಪೂರ್ಣಾಙ್ಕ k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ರಿಕ್ತ : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ಯದಿ (k < 0)
                ಪ್ರತ್ಯಾಗಮ k;

        r = write_string_stream_full(f, ಸಾಲು, flags, ts);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = fchmod_umask(fileno(f), mode);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ಯದಿ (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ write_string_file_full_label(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *fn,
                ಧ್ರುವ ವರ್ಣ *ಸಾಲು,
                WriteStringFileFlags flags,
                ಧ್ರುವ ಸಂರಚನಾ timespec *ts,
                ಧ್ರುವ ವರ್ಣ *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd == AT_FDCWD || dir_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು);

        /* We don't know how to verify whether the file contents was already on-disk. */
        ಪ್ರತಿಜ್ಞಾ(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ಯದಿ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                ಪ್ರತಿಜ್ಞಾ(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (flags & WRITE_STRING_FILE_ATOMIC) {
                ಪ್ರತಿಜ್ಞಾ(fn);
                ಪ್ರತಿಜ್ಞಾ(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, ಸಾಲು, flags, ts, label_context);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ fail;

                ಪ್ರತ್ಯಾಗಮ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ಯದಿ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ಅನ್ಯಥಾ {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                ಯದಿ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        ಯದಿ (r < 0)
                                ಗಚ್ಛ fail;

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
                ಯದಿ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ರಿಕ್ತ : fn, made_file, label_context));
        }
        ಯದಿ (r < 0)
                ಗಚ್ಛ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ಯದಿ (r < 0)
                ಗಚ್ಛ fail;

        ಯದಿ (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, ರಿಕ್ತ, _IONBF, 0);

        r = write_string_stream_full(f, ಸಾಲು, flags, ts);
        ಯದಿ (r < 0)
                ಗಚ್ಛ fail;

        ಪ್ರತ್ಯಾಗಮ 0;

fail:
        ಯದಿ (made_file)
                (ಶೂನ್ಯ) unlinkat(dir_fd, fn, 0);

        ಯದಿ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ಪ್ರತ್ಯಾಗಮ r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ಯದಿ (verify_file_at(dir_fd, fn, ಸಾಲು, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ write_string_filef(
                ಧ್ರುವ ವರ್ಣ *fn,
                WriteStringFileFlags flags,
                ಧ್ರುವ ವರ್ಣ *format, ...) {

        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ write_string_file(fn, p, flags);
}

ಪೂರ್ಣಾಙ್ಕ write_string_filef_at(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *fn,
                WriteStringFileFlags flags,
                ಧ್ರುವ ವರ್ಣ *format, ...) {

        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ write_string_file_at(dir_fd, fn, p, flags);
}

ಪೂರ್ಣಾಙ್ಕ write_base64_file_at(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *fn,
                ಧ್ರುವ ಸಂರಚನಾ iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ ವರ್ಣ *encoded = ರಿಕ್ತ;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : ರಿಕ್ತ, data ? data->iov_len : 0, 79, &encoded);
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ n;

        ಪ್ರತ್ಯಾಗಮ write_string_file_at(dir_fd, fn, encoded, flags);
}

ಪೂರ್ಣಾಙ್ಕ read_one_ಸಾಲು_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(filename);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, ret);
}

ಪೂರ್ಣಾಙ್ಕ read_boolean_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(filename);

        r = read_one_ಸಾಲು_file_at(dir_fd, filename, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_boolean(s);
}

ಪೂರ್ಣಾಙ್ಕ verify_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        size_t l, k;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(blob);

        l = strlen(blob);

        ಯದಿ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ಯದಿ (ferror(f))
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

        ಯದಿ (k != l && k != l + accept_extra_nl)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (memcmp(buf, blob, l) != 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (k > l && buf[l] != '\n')
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ read_virtual_file_at(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *filename,
                size_t max_size,
                ವರ್ಣ **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        size_t n, size;
        ಪೂರ್ಣಾಙ್ಕ n_retries;
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

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಯದಿ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ಅನ್ಯಥಾ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        ಪ್ರತಿ (;;) {
                ಸಂರಚನಾ stat st;

                ಯದಿ (fstat(fd, &st) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (!S_ISREG(st.st_mode))
                        ಪ್ರತ್ಯಾಗಮ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                ಯದಿ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ಯದಿ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ಯದಿ (max_size == SIZE_MAX)
                                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                                size = max_size;
                        } ಅನ್ಯಥಾ {
                                size = MIN((size_t) st.st_size, max_size);

                                ಯದಿ (size > READ_VIRTUAL_BYTES_MAX)
                                        ಪ್ರತ್ಯಾಗಮ -EFBIG;
                        }

                        n_retries--;
                } ಅನ್ಯಥಾ ಯದಿ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } ಅನ್ಯಥಾ {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                ಪ್ರತಿ (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = ಪಠ(fd, buf, size + 1);
                        ಯದಿ (k >= 0) {
                                n = k;
                                ಭಙ್ಗ;
                        }

                        ಯದಿ (errno != EINTR)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                }

                /* Consider a short read as EOF */
                ಯದಿ (n <= size)
                        ಭಙ್ಗ;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ಯದಿ (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        ಭಙ್ಗ;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ಯದಿ (n_retries <= 0)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ಯದಿ (lseek(fd, 0, SEEK_SET) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                buf = mfree(buf);
        }

        ಯದಿ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ಯದಿ (!ret_size && memchr(buf, 0, n))
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (n < size) {
                        ವರ್ಣ *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ಯದಿ (!p)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ಯದಿ (ret_size)
                *ret_size = n;

        ಪ್ರತ್ಯಾಗಮ !truncated;
}

ಪೂರ್ಣಾಙ್ಕ read_full_stream_full(
                FILE *f,
                ಧ್ರುವ ವರ್ಣ *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ವರ್ಣ **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        ಪೂರ್ಣಾಙ್ಕ fd, r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret_contents);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        ಪ್ರತಿಜ್ಞಾ(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        ಯದಿ (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                ಯದಿ (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ಅನ್ಯಥಾ
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        ಯದಿ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ಸಂರಚನಾ stat st;

                ಯದಿ (fstat(fd, &st) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ಯದಿ (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                ಯದಿ (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        ಯದಿ (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (ಶೂನ್ಯ) warn_file_is_world_accessible(filename, &st, ರಿಕ್ತ, 0);
                }
        } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ಪ್ರತ್ಯಾಗಮ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ಯದಿ (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ಯದಿ (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        ಯದಿ (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        n = l = 0;
        ಪ್ರತಿ (;;) {
                ವರ್ಣ *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ಯದಿ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                ಯದಿ (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        ಯದಿ (!t) {
                                r = -ENOMEM;
                                ಗಚ್ಛ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        ಮುಕ್ತ(buf);
                } ಅನ್ಯಥಾ {
                        t = realloc(buf, n_next + 1);
                        ಯದಿ (!t)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                ಪ್ರತಿಜ್ಞಾ(k <= n - l);
                l += k;

                ಯದಿ (ferror(f)) {
                        r = errno_or_else(EIO);
                        ಗಚ್ಛ finalize;
                }
                ಯದಿ (feof(f))
                        ಭಙ್ಗ;

                ಯದಿ (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        ಪ್ರತಿಜ್ಞಾ(l == size);
                        ಭಙ್ಗ;
                }

                ಪ್ರತಿಜ್ಞಾ(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ಯದಿ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        ಗಚ್ಛ finalize;
                }

                ಯದಿ (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        ಗಚ್ಛ finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        ಯದಿ (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ ಶೂನ್ಯ *decoded = ರಿಕ್ತ;
                size_t decoded_size;

                buf[l++] = 0;
                ಯದಿ (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ಅನ್ಯಥಾ
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ finalize;

                ಯದಿ (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                ಯದಿ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        ಗಚ್ಛ finalize;
                }
        }

        ಯದಿ (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ಯದಿ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        ಗಚ್ಛ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ಯದಿ (ret_size)
                *ret_size = l;

        ಪ್ರತ್ಯಾಗಮ 0;

finalize:
        ಯದಿ (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ read_full_file_full(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ಧ್ರುವ ವರ್ಣ *bind_name,
                ವರ್ಣ **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_contents);

        ಯದಿ (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ script_get_shebang_interpreter(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        f = fopen(path, "re");
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ವರ್ಣ c;
        r = safe_fgetc(f, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;
        ಯದಿ (c != '#')
                ಪ್ರತ್ಯಾಗಮ -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;
        ಯದಿ (c != '!')
                ಪ್ರತ್ಯಾಗಮ -EMEDIUMTYPE;

        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *s = ಸಾಲು;

        r = extract_first_word(&s, &p, /* separators= */ ರಿಕ್ತ, /* flags= */ 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOEXEC;

        ಯದಿ (ret)
                *ret = TAKE_PTR(p);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_proc_field(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ **ret) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = fopen_unlocked(path, "re", &f);
        ಯದಿ (r == -ENOENT && proc_mounted() == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOSYS;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                 _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;

                 r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                 ಯದಿ (r < 0)
                         ಪ್ರತ್ಯಾಗಮ r;
                 ಯದಿ (r == 0)
                         ಪ್ರತ್ಯಾಗಮ -ENODATA;

                 ವರ್ಣ *l = startswith(ಸಾಲು, ಕೀಲಿ);
                 ಯದಿ (l && *l == ':') {
                         ಯದಿ (ret) {
                                 ವರ್ಣ *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ಯದಿ (!s)
                                         ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                 *ret = s;
                         }

                         ಪ್ರತ್ಯಾಗಮ 0;
                 }
        }
}

DIR* xopendirat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(!(flags & (O_CREAT|O_TMPFILE)));

        ಯದಿ ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ಪ್ರತ್ಯಾಗಮ opendir(path);

        ಯದಿ (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ take_fdopendir(&fd);
}

ಪೂರ್ಣಾಙ್ಕ fopen_mode_to_flags(ಧ್ರುವ ವರ್ಣ *mode) {
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ flags;

        ಪ್ರತಿಜ್ಞಾ(mode);

        ಯದಿ ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        ಅನ್ಯಥಾ ಯದಿ ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        ಅನ್ಯಥಾ ಯದಿ ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ಅನ್ಯಥಾ ಯದಿ ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ಅನ್ಯಥಾ ಯದಿ ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ಅನ್ಯಥಾ ಯದಿ ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತಿ (; *p != 0; p++) {

                ವಿಕಲ್ಪ (*p) {

                ಪಕ್ಷ 'e':
                        flags |= O_CLOEXEC;
                        ಭಙ್ಗ;

                ಪಕ್ಷ 'x':
                        flags |= O_EXCL;
                        ಭಙ್ಗ;

                ಪಕ್ಷ 'm':
                        /* ignore this here, fdopen() might care later though */
                        ಭಙ್ಗ;

                ಪಕ್ಷ 'c': /* not sure what to do about this one */
                ಸ್ವಭಾವ:
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                }
        }

        ಪ್ರತ್ಯಾಗಮ flags;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xfopenat_regular(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *mode, ಪೂರ್ಣಾಙ್ಕ open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(mode);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        ಅನ್ಯಥಾ ಯದಿ (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ ವರ್ಣ *j = strjoin("/", path);
                ಯದಿ (!j)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                f = fopen(j, mode);
        } ಅನ್ಯಥಾ {
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
                ಪೂರ್ಣಾಙ್ಕ mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                ಯದಿ (mode_flags < 0)
                        ಪ್ರತ್ಯಾಗಮ mode_flags;

                ಯದಿ (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        ಯದಿ (fd < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                } ಅನ್ಯಥಾ {
                        ಯದಿ (dir_fd == AT_FDCWD)
                                ಪ್ರತ್ಯಾಗಮ -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        ಯದಿ (fd < 0)
                                ಪ್ರತ್ಯಾಗಮ fd;
                }

                f = take_fdopen(&fd, mode);
        }
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = f;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xfopenat_unix_socket(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *bind_name, FILE **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ sk = -EBADF;
        FILE *f;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ಯದಿ (sk < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                ಸಙ್ಘ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (bind(sk, &bsa.sa, r) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (shutdown(sk, SHUT_WR) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        f = take_fdopen(&sk, "r");
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = f;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ xfopenat_full(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *mode,
                ಪೂರ್ಣಾಙ್ಕ open_flags,
                XfopenFlags flags,
                ಧ್ರುವ ವರ್ಣ *bind_name,
                FILE **ret) {

        FILE *f = ರಿಕ್ತ;  /* avoid false maybe-uninitialized warning */
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(mode);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        ಯದಿ (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                ಯದಿ (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ಪ್ರತ್ಯಾಗಮ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdopen_independent(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *mode, FILE **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ mode_flags;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(mode);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        ಯದಿ (mode_flags < 0)
                ಪ್ರತ್ಯಾಗಮ mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        ಯದಿ (copy_fd < 0)
                ಪ್ರತ್ಯಾಗಮ copy_fd;

        f = take_fdopen(&copy_fd, mode);
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = TAKE_PTR(f);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_open_internal(
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ **ಹುಡುಕು,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd,
                ವರ್ಣ **ret_path) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        ಪ್ರತಿಜ್ಞಾ(path);

        ಯದಿ (path_is_absolute(path)) {
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

                ಯದಿ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                ಅನ್ಯಥಾ
                        r = RET_NERRNO(access(path, mode));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (!path_strv_resolve_uniq(ಹುಡುಕು, ಬೇರು))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        STRV_FOREACH(i, ಹುಡುಕು) {
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
                _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                p = path_join(ಬೇರು, *i, path);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                ಅನ್ಯಥಾ
                        r = RET_NERRNO(access(p, F_OK));
                ಯದಿ (r >= 0) {
                        ಯದಿ (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        ಯದಿ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (r != -ENOENT)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ -ENOENT;
}

ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_open(
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ mode,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ **ಹುಡುಕು,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd,
                ವರ್ಣ **ret_path) {

        _cleanup_strv_free_ ವರ್ಣ **copy = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(path);

        copy = strv_copy(ಹುಡುಕು);
        ಯದಿ (!copy)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ ಹುಡುಕು_and_open_internal(path, mode, ಬೇರು, copy, ret_fd, ret_path);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_fopen_internal(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *mode,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ **ಹುಡುಕು,
                FILE **ret_file,
                ವರ್ಣ **ret_path) {

        _cleanup_free_ ವರ್ಣ *found_path = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(mode || !ret_file);

        r = ಹುಡುಕು_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        ಬೇರು,
                        ಹುಡುಕು,
                        ret_file ? &fd : ರಿಕ್ತ,
                        ret_path ? &found_path : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                ಯದಿ (!f)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                *ret_file = f;
        }

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_fopen(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *mode,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಧ್ರುವ ವರ್ಣ **ಹುಡುಕು,
                FILE **ret_file,
                ವರ್ಣ **ret_path) {

        _cleanup_strv_free_ ವರ್ಣ **copy = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(mode || !ret_file);

        copy = strv_copy((ವರ್ಣ**) ಹುಡುಕು);
        ಯದಿ (!copy)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ ಹುಡುಕು_and_fopen_internal(path, mode, ಬೇರು, copy, ret_file, ret_path);
}

ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_fopen_nulstr(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *mode,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಧ್ರುವ ವರ್ಣ *ಹುಡುಕು,
                FILE **ret_file,
                ವರ್ಣ **ret_path) {

        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(mode || !ret_file);

        l = strv_split_nulstr(ಹುಡುಕು);
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ ಹುಡುಕು_and_fopen_internal(path, mode, ಬೇರು, l, ret_file, ret_path);
}

ಪೂರ್ಣಾಙ್ಕ fflush_and_check(FILE *f) {
        ಪ್ರತಿಜ್ಞಾ(f);

        errno = 0;
        fflush(f);

        ಯದಿ (ferror(f))
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fflush_sync_and_check(FILE *f) {
        ಪೂರ್ಣಾಙ್ಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(f);

        r = fflush_and_check(f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        r = fsync_full(fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ write_timestamp_file_atomic(ಧ್ರುವ ವರ್ಣ *fn, usec_t n) {
        ವರ್ಣ ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ಯದಿ (!timestamp_is_set(n))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ಪ್ರತ್ಯಾಗಮ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

ಪೂರ್ಣಾಙ್ಕ read_timestamp_file(ಧ್ರುವ ವರ್ಣ *fn, usec_t *ret) {
        _cleanup_free_ ವರ್ಣ *ln = ರಿಕ್ತ;
        uint64_t t;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file(fn, &ln);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = safe_atou64(ln, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!timestamp_is_set(t))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (usec_t) t;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fputs_with_separator(FILE *f, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separator, bool *space) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ಯದಿ (!f)
                f = stdout;

        ಯದಿ (!separator)
                separator = " ";

        ಯದಿ (*space)
                ಯದಿ (fputs(separator, f) < 0)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

        *space = true;

        ಯದಿ (fputs(s, f) < 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fputs_with_newline(FILE *f, ಧ್ರುವ ವರ್ಣ *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ಯದಿ (isempty(s))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!f)
                f = stdout;

        ಯದಿ (fputs(s, f) < 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (endswith(s, "\n"))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (fputc('\n', f) < 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಪ್ರತ್ಯಾಗಮ 1;
}

/* A bitmask of the EOL markers we know */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ End_Of_ಸಾಲು_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_ಸಾಲು_Marker;

ಸ್ಥಿರ End_Of_ಸಾಲು_Marker categorize_eol(ವರ್ಣ c, Read_ಸಾಲು_Flags flags) {

        ಯದಿ (!FLAGS_SET(flags, READ_ಸಾಲು_ONLY_NUL)) {
                ಯದಿ (c == '\n')
                        ಪ್ರತ್ಯಾಗಮ EOL_TEN;
                ಯದಿ (c == '\r')
                        ಪ್ರತ್ಯಾಗಮ EOL_THIRTEEN;
        }

        ಯದಿ (c == '\0')
                ಪ್ರತ್ಯಾಗಮ EOL_ZERO;

        ಪ್ರತ್ಯಾಗಮ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, ರಿಕ್ತ);

ಪೂರ್ಣಾಙ್ಕ read_ಸಾಲು_full(FILE *f, size_t limit, Read_ಸಾಲು_Flags flags, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *buffer = ರಿಕ್ತ;
        size_t n = 0, ಎಣಿಕೆ = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

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

        ಯದಿ (ret) {
                ಯದಿ (!GREEDY_REALLOC(buffer, 1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_ಸಾಲು_Marker previous_eol = EOL_NONE;
                flockfile(f);

                ಪ್ರತಿ (;;) {
                        End_Of_ಸಾಲು_Marker eol;
                        ವರ್ಣ c;

                        ಯದಿ (n >= limit)
                                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                        ಯದಿ (ಎಣಿಕೆ >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0) /* EOF is definitely EOL */
                                ಭಙ್ಗ;

                        eol = categorize_eol(c, flags);

                        ಯದಿ (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned ವರ್ಣ) c, f) != EOF);
                                ಭಙ್ಗ;
                        }

                        ಎಣಿಕೆ++;

                        ಯದಿ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ಯದಿ ((flags & (READ_ಸಾಲು_IS_A_TTY|READ_ಸಾಲು_NOT_A_TTY)) == 0) {
                                        ಪೂರ್ಣಾಙ್ಕ fd;

                                        fd = fileno(f);
                                        ಯದಿ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_ಸಾಲು_NOT_A_TTY;
                                        ಅನ್ಯಥಾ
                                                flags |= isatty_safe(fd) ? READ_ಸಾಲು_IS_A_TTY : READ_ಸಾಲು_NOT_A_TTY;
                                }
                                ಯದಿ (FLAGS_SET(flags, READ_ಸಾಲು_IS_A_TTY))
                                        ಭಙ್ಗ;
                        }

                        ಯದಿ (eol != EOL_NONE) {
                                previous_eol |= eol;
                                ಅನುವರ್ತ;
                        }

                        ಯದಿ (ret) {
                                ಯದಿ (!GREEDY_REALLOC(buffer, n + 2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        ಯದಿ (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) ಎಣಿಕೆ;
}

ಪೂರ್ಣಾಙ್ಕ read_stripped_ಸಾಲು(FILE *f, size_t limit, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r, k;

        ಪ್ರತಿಜ್ಞಾ(f);

        r = read_ಸಾಲು(f, limit, ret ? &s : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret) {
                ಧ್ರುವ ವರ್ಣ *p = strstrip(s);
                ಯದಿ (p == s)
                        *ret = TAKE_PTR(s);
                ಅನ್ಯಥಾ {
                        k = strdup_to(ret, p);
                        ಯದಿ (k < 0)
                                ಪ್ರತ್ಯಾಗಮ k;
                }
        }

        ಪ್ರತ್ಯಾಗಮ r > 0;          /* Return 1 if something was read. */
}

ಪೂರ್ಣಾಙ್ಕ safe_fgetc(FILE *f, ವರ್ಣ *ret) {
        ಪೂರ್ಣಾಙ್ಕ k;

        ಪ್ರತಿಜ್ಞಾ(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ಯದಿ (k == EOF) {
                ಯದಿ (ferror(f))
                        ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

                ಯದಿ (ret)
                        *ret = 0;

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (ret)
                *ret = k;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ warn_file_is_world_accessible(ಧ್ರುವ ವರ್ಣ *filename, ಸಂರಚನಾ stat *st, ಧ್ರುವ ವರ್ಣ *unit, unsigned ಸಾಲು) {
        ಸಂರಚನಾ stat _st;

        ಯದಿ (!filename)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!st) {
                ಯದಿ (stat(filename, &_st) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                st = &_st;
        }

        ಯದಿ ((st->st_mode & S_IRWXO) == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (unit)
                log_syntax(unit, LOG_WARNING, filename, ಸಾಲು, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ಅನ್ಯಥಾ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ write_data_file_atomic_at(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ಸಂರಚನಾ iovec *iovec,
                WriteDataFileFlags flags) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ ವರ್ಣ *dn = ರಿಕ್ತ, *fn = ರಿಕ್ತ;
        r = path_split_prefix_filename(path, &dn, &fn);
        ಯದಿ (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ಪ್ರತ್ಯಾಗಮ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ mfd = -EBADF;
        ಯದಿ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ರಿಕ್ತ,
                            &mfd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                dir_fd = mfd;
        }

        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ಯದಿ (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ಪ್ರತ್ಯಾಗಮ 0;
}
