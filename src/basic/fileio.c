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
#ವ್ಯಾಖ್ಯಾನಿಸು READ_ತುಂಬಿದ_ಬೈಟುಗಳು_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#ವ್ಯಾಖ್ಯಾನಿಸು READ_ತುಂಬಿದ_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#ವ್ಯಾಖ್ಯಾನಿಸು READ_VIRTUAL_ಬೈಟುಗಳು_MAX (4U * U64_MB - UINT64_C(2))

ಪೂರ್ಣಾಂಕ fdopen_unlocked(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options, FILE **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        FILE *f = fdopen(fd, options);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ take_fdopen_unlocked(ಪೂರ್ಣಾಂಕ *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options, FILE **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *fd = -EBADF;

        ಹಿಂತಿರುಗಿ 0;
}

FILE* take_fdopen(ಪೂರ್ಣಾಂಕ *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options) {
        ಪ್ರತಿಜ್ಞಾ(fd);

        FILE *f = fdopen(*fd, options);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *fd = -EBADF;

        ಹಿಂತಿರುಗಿ f;
}

DIR* take_fdopendir(ಪೂರ್ಣಾಂಕ *dfd) {
        ಪ್ರತಿಜ್ಞಾ(dfd);

        DIR *d = fdopendir(*dfd);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *dfd = -EBADF;

        ಹಿಂತಿರುಗಿ d;
}

FILE* open_memstream_unlocked(ಅಕ್ಷರ **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ಹಿಂತಿರುಗಿ f;
}

FILE* fmemopen_unlocked(ಶೂನ್ಯ *buf, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ) {
        FILE *f = fmemopen(buf, ಗಾತ್ರ_2, ಬಹುಲಕ);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ಹಿಂತಿರುಗಿ f;
}

ಪೂರ್ಣಾಂಕ write_string_ಹರಿವು_ತುಂಬಿದ(
                FILE *f,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು,
                WriteStringFileFlags flags,
                ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts) {

        bool needs_nl;
        ಪೂರ್ಣಾಂಕ r, fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು);

        ಆದರೆ (ferror(f))
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -EBADF;
        }

        ಆದರೆ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ಆದರೆ (fd < 0) {
                        fd = fileno(f);
                        ಆದರೆ (fd < 0)
                                ಹಿಂತಿರುಗಿ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ಆದರೆ (read_virtual_file_fd(fd, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಾಲು)+1, &t, ಖಾಲಿ) > 0 &&
                    streq_skip_trailing_chars(ಸಾಲು, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", ಸಾಲು);
                        ಹಿಂತಿರುಗಿ 0;
                }

                ಆದರೆ (lseek(fd, 0, SEEK_SET) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(ಸಾಲು, "\n");

        ಆದರೆ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_ಬಫರ್)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                ಸಾಲು = strjoina(ಸಾಲು, "\n");
                needs_nl = false;
        }

        ಆದರೆ (fputs(ಸಾಲು, f) == EOF)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (needs_nl)
                ಆದರೆ (fputc('\n', f) == EOF)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ಇಲ್ಲದಿದ್ದರೆ
                r = fflush_and_check(f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ts) {
                ಸ್ಥಿರಾಂಕ ರಚನೆ timespec twice[2] = {*ts, *ts};

                ಪ್ರತಿಜ್ಞಾ(fd >= 0);
                ಆದರೆ (futimens(fd, twice) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ mode_t write_string_file_flags_ಗಮ್ಯ_ಬಹುಲಕ(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(flags, WRITE_STRING_FILE_ಬಹುಲಕ_0600) ? 0600 :
                FLAGS_ಗಣ(flags, WRITE_STRING_FILE_ಬಹುಲಕ_0444) ? 0444 : 0644;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_string_file_atomic_at(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು,
                WriteStringFileFlags flags,
                ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fn);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t ಬಹುಲಕ = write_string_file_flags_ಗಮ್ಯ_ಬಹುಲಕ(flags);

        bool call_label_ops_post = false;
        ಆದರೆ (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, ಬಹುಲಕ, label_context);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        ಪೂರ್ಣಾಂಕ k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ಖಾಲಿ : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ಆದರೆ (k < 0)
                ಹಿಂತಿರುಗಿ k;

        r = write_string_ಹರಿವು_ತುಂಬಿದ(f, ಸಾಲು, flags, ts);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = fchmod_umask(fileno(f), ಬಹುಲಕ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ಆದರೆ (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_ದ_file(fileno(f));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ write_string_file_ತುಂಬಿದ_label(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು,
                WriteStringFileFlags flags,
                ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd == AT_FDCWD || dir_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಸಾಲು);

        /* We don't know how to verify whether the file contents was already on-disk. */
        ಪ್ರತಿಜ್ಞಾ(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ಆದರೆ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                ಪ್ರತಿಜ್ಞಾ(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (flags & WRITE_STRING_FILE_ATOMIC) {
                ಪ್ರತಿಜ್ಞಾ(fn);
                ಪ್ರತಿಜ್ಞಾ(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, ಸಾಲು, flags, ts, label_context);
                ಆದರೆ (r < 0)
                        ಹೋಗು fail;

                ಹಿಂತಿರುಗಿ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ಆದರೆ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ಇಲ್ಲದಿದ್ದರೆ {
                mode_t ಬಹುಲಕ = write_string_file_flags_ಗಮ್ಯ_ಬಹುಲಕ(flags);
                bool call_label_ops_post = false;

                ಆದರೆ (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, ಬಹುಲಕ, label_context);
                        ಆದರೆ (r < 0)
                                ಹೋಗು fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_ಗಣ(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                ಬಹುಲಕ,
                                &made_file);
                ಆದರೆ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ಖಾಲಿ : fn, made_file, label_context));
        }
        ಆದರೆ (r < 0)
                ಹೋಗು fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ಆದರೆ (r < 0)
                ಹೋಗು fail;

        ಆದರೆ (flags & WRITE_STRING_FILE_DISABLE_ಬಫರ್)
                setvbuf(f, ಖಾಲಿ, _IONBF, 0);

        r = write_string_ಹರಿವು_ತುಂಬಿದ(f, ಸಾಲು, flags, ts);
        ಆದರೆ (r < 0)
                ಹೋಗು fail;

        ಹಿಂತಿರುಗಿ 0;

fail:
        ಆದರೆ (made_file)
                (ಶೂನ್ಯ) unlinkat(dir_fd, fn, 0);

        ಆದರೆ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ಹಿಂತಿರುಗಿ r;

        f = ಸುರಕ್ಷಿತ_fclose(f);
        fd = ಸುರಕ್ಷಿತ_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ಆದರೆ (verify_file_at(dir_fd, fn, ಸಾಲು, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ write_string_filef(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                WriteStringFileFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {

        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ write_string_file(fn, p, flags);
}

ಪೂರ್ಣಾಂಕ write_string_filef_at(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                WriteStringFileFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {

        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ write_string_file_at(dir_fd, fn, p, flags);
}

ಪೂರ್ಣಾಂಕ write_base64_file_at(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ದತ್ತಾಂಶ,
                WriteStringFileFlags flags) {

        _cleanup_free_ ಅಕ್ಷರ *encoded = ಖಾಲಿ;
        ssize_t n;

        n = base64mem_ತುಂಬಿದ(ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->iov_base : ಖಾಲಿ, ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->iov_len : 0, 79, &encoded);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ n;

        ಹಿಂತಿರುಗಿ write_string_file_at(dir_fd, fn, encoded, flags);
}

ಪೂರ್ಣಾಂಕ read_one_ಸಾಲು_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(filename);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, ret);
}

ಪೂರ್ಣಾಂಕ read_boolean_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(filename);

        r = read_one_ಸಾಲು_file_at(dir_fd, filename, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_boolean(s);
}

ಪೂರ್ಣಾಂಕ verify_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        size_t l, k;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(blob);

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(blob);

        ಆದರೆ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = ಸ್ಮರಣೆ_ಕೊಡು(l + accept_extra_nl + 1);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ಆದರೆ (ferror(f))
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

        ಆದರೆ (k != l && k != l + accept_extra_nl)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (memcmp(buf, blob, l) != 0)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (k > l && buf[l] != '\n')
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ read_virtual_file_at(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                size_t max_ಗಾತ್ರ,
                ಅಕ್ಷರ **ret_contents,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        size_t n, ಗಾತ್ರ_2;
        ಪೂರ್ಣಾಂಕ n_retries;
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
        ಪ್ರತಿಜ್ಞಾ(max_ಗಾತ್ರ <= READ_VIRTUAL_ಬೈಟುಗಳು_MAX || max_ಗಾತ್ರ == SIZE_MAX);

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಆದರೆ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ಇಲ್ಲದಿದ್ದರೆ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        ಗಾಗಿ (;;) {
                ರಚನೆ stat st;

                ಆದರೆ (fstat(fd, &st) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (!S_ISREG(st.st_mode))
                        ಹಿಂತಿರುಗಿ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_ಬೈಟುಗಳು_MAX < SSIZE_MAX);
                ಆದರೆ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ಆದರೆ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ಆದರೆ (max_ಗಾತ್ರ == SIZE_MAX)
                                        ಹಿಂತಿರುಗಿ -EFBIG;

                                ಗಾತ್ರ_2 = max_ಗಾತ್ರ;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಗಾತ್ರ_2 = MIN((size_t) st.st_size, max_ಗಾತ್ರ);

                                ಆದರೆ (ಗಾತ್ರ_2 > READ_VIRTUAL_ಬೈಟುಗಳು_MAX)
                                        ಹಿಂತಿರುಗಿ -EFBIG;
                        }

                        n_retries--;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        ಗಾತ್ರ_2 = MIN3(ಪುಟ_ಗಾತ್ರ() - 1, READ_VIRTUAL_ಬೈಟುಗಳು_MAX, max_ಗಾತ್ರ);
                        n_retries = 1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಗಾತ್ರ_2 = MIN(READ_VIRTUAL_ಬೈಟುಗಳು_MAX, max_ಗಾತ್ರ);
                        n_retries = 0;
                }

                buf = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2 + 1);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                ಗಾತ್ರ_2 = MIN3(MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(buf) - 1, max_ಗಾತ್ರ, READ_VIRTUAL_ಬೈಟುಗಳು_MAX);

                ಗಾಗಿ (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = ಪಠ(fd, buf, ಗಾತ್ರ_2 + 1);
                        ಆದರೆ (k >= 0) {
                                n = k;
                                ನಿಲ್ಲಿಸು;
                        }

                        ಆದರೆ (errno != EINTR)
                                ಹಿಂತಿರುಗಿ -errno;
                }

                /* Consider a short read as EOF */
                ಆದರೆ (n <= ಗಾತ್ರ_2)
                        ನಿಲ್ಲಿಸು;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ಆದರೆ (max_ಗಾತ್ರ != SIZE_MAX && n > max_ಗಾತ್ರ) {
                        n = ಗಾತ್ರ_2; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        ನಿಲ್ಲಿಸು;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ಆದರೆ (n_retries <= 0)
                        ಹಿಂತಿರುಗಿ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ಆದರೆ (lseek(fd, 0, SEEK_SET) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                buf = mfree(buf);
        }

        ಆದರೆ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ಆದರೆ (!ret_ಗಾತ್ರ && memchr(buf, 0, n))
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (n < ಗಾತ್ರ_2) {
                        ಅಕ್ಷರ *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ಆದರೆ (!p)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = n;

        ಹಿಂತಿರುಗಿ !truncated;
}

ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_ಹರಿವು_ತುಂಬಿದ(
                FILE *f,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                uint64_t ಸ್ಥಾನಾಂತರ,
                size_t ಗಾತ್ರ_2,
                Read_ತುಂಬಿದ_File_Flags flags,
                ಅಕ್ಷರ **ret_contents,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        size_t n, n_ಮುಂದಿನ = 0, l, expected_decoded_ಗಾತ್ರ = ಗಾತ್ರ_2;
        ಪೂರ್ಣಾಂಕ fd, r;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(ret_contents);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_UNBASE64 | READ_ತುಂಬಿದ_FILE_UNHEX));
        ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 != SIZE_MAX || !FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER));

        ಆದರೆ (ಸ್ಥಾನಾಂತರ != UINT64_MAX && ಸ್ಥಾನಾಂತರ > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ ((flags & (READ_ತುಂಬಿದ_FILE_UNBASE64 | READ_ತುಂಬಿದ_FILE_UNHEX)) != 0) {
                ಆದರೆ (ಗಾತ್ರ_2 <= SIZE_MAX / READ_ತುಂಬಿದ_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        ಗಾತ್ರ_2 *= READ_ತುಂಬಿದ_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ಇಲ್ಲದಿದ್ದರೆ
                        ಗಾತ್ರ_2 = SIZE_MAX;
        }

        fd = fileno(f);
        ಆದರೆ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ರಚನೆ stat st;

                ಆದರೆ (fstat(fd, &st) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ಆದರೆ (st.st_size > 0 &&
                            (ಗಾತ್ರ_2 == SIZE_MAX || FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, ಸ್ಥಾನಾಂತರ == UINT64_MAX ? 0 : ಸ್ಥಾನಾಂತರ);

                                ಆದರೆ (rsize < SIZE_MAX) /* overflow check */
                                        n_ಮುಂದಿನ = rsize + 1;
                        }

                        ಆದರೆ (flags & READ_ತುಂಬಿದ_FILE_WARN_WORLD_READABLE)
                                (ಶೂನ್ಯ) warn_file_is_world_accessible(filename, &st, ಖಾಲಿ, 0);
                }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_VERIFY_REGULAR))
                ಹಿಂತಿರುಗಿ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ಆದರೆ (n_ಮುಂದಿನ == 0)
                n_ಮುಂದಿನ = ಗಾತ್ರ_2 != SIZE_MAX && !FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER) ? ಗಾತ್ರ_2 : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ಆದರೆ (n_ಮುಂದಿನ > READ_ತುಂಬಿದ_ಬೈಟುಗಳು_MAX)
                n_ಮುಂದಿನ = READ_ತುಂಬಿದ_ಬೈಟುಗಳು_MAX + 1;

        ಆದರೆ (ಸ್ಥಾನಾಂತರ != UINT64_MAX && fseek(f, ಸ್ಥಾನಾಂತರ, SEEK_SET) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        n = l = 0;
        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ಆದರೆ (FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER) && n_ಮುಂದಿನ > ಗಾತ್ರ_2)
                        n_ಮುಂದಿನ = ಗಾತ್ರ_2 + 1;

                ಆದರೆ (flags & READ_ತುಂಬಿದ_FILE_SECURE) {
                        t = ಸ್ಮರಣೆ_ಕೊಡು(n_ಮುಂದಿನ + 1);
                        ಆದರೆ (!t) {
                                r = -ENOMEM;
                                ಹೋಗು finalize;
                        }
                        memcpy_ಸುರಕ್ಷಿತ(t, buf, n);
                        explicit_bzero_ಸುರಕ್ಷಿತ(buf, n);
                        ಬಿಡುಗಡೆ(buf);
                } ಇಲ್ಲದಿದ್ದರೆ {
                        t = realloc(buf, n_ಮುಂದಿನ + 1);
                        ಆದರೆ (!t)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = ಗಾತ್ರ_2 == SIZE_MAX ? MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(buf) - 1 : n_ಮುಂದಿನ;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                ಪ್ರತಿಜ್ಞಾ(k <= n - l);
                l += k;

                ಆದರೆ (ferror(f)) {
                        r = errno_or_else(EIO);
                        ಹೋಗು finalize;
                }
                ಆದರೆ (feof(f))
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (ಗಾತ್ರ_2 != SIZE_MAX && !FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        ಪ್ರತಿಜ್ಞಾ(l == ಗಾತ್ರ_2);
                        ನಿಲ್ಲಿಸು;
                }

                ಪ್ರತಿಜ್ಞಾ(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ಆದರೆ (FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER) && l > ಗಾತ್ರ_2) {
                        r = -E2BIG;
                        ಹೋಗು finalize;
                }

                ಆದರೆ (n >= READ_ತುಂಬಿದ_ಬೈಟುಗಳು_MAX) {
                        r = -E2BIG;
                        ಹೋಗು finalize;
                }

                n_ಮುಂದಿನ = MIN(n * 2, READ_ತುಂಬಿದ_ಬೈಟುಗಳು_MAX);
        }

        ಆದರೆ (flags & (READ_ತುಂಬಿದ_FILE_UNBASE64 | READ_ತುಂಬಿದ_FILE_UNHEX)) {
                _cleanup_free_ ಶೂನ್ಯ *decoded = ಖಾಲಿ;
                size_t decoded_ಗಾತ್ರ;

                buf[l++] = 0;
                ಆದರೆ (flags & READ_ತುಂಬಿದ_FILE_UNBASE64)
                        r = unbase64mem_ತುಂಬಿದ(buf, l, flags & READ_ತುಂಬಿದ_FILE_SECURE, &decoded, &decoded_ಗಾತ್ರ);
                ಇಲ್ಲದಿದ್ದರೆ
                        r = unhexmem_ತುಂಬಿದ(buf, l, flags & READ_ತುಂಬಿದ_FILE_SECURE, &decoded, &decoded_ಗಾತ್ರ);
                ಆದರೆ (r < 0)
                        ಹೋಗು finalize;

                ಆದರೆ (flags & READ_ತುಂಬಿದ_FILE_SECURE)
                        explicit_bzero_ಸುರಕ್ಷಿತ(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_ಗಾತ್ರ;

                ಆದರೆ (FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_ಗಾತ್ರ) {
                        r = -E2BIG;
                        ಹೋಗು finalize;
                }
        }

        ಆದರೆ (!ret_ಗಾತ್ರ) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ಆದರೆ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        ಹೋಗು finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = l;

        ಹಿಂತಿರುಗಿ 0;

finalize:
        ಆದರೆ (flags & READ_ತುಂಬಿದ_FILE_SECURE)
                explicit_bzero_ಸುರಕ್ಷಿತ(buf, n);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_file_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                uint64_t ಸ್ಥಾನಾಂತರ,
                size_t ಗಾತ್ರ_2,
                Read_ತುಂಬಿದ_File_Flags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bind_name,
                ಅಕ್ಷರ **ret_contents,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_contents);

        ಆದರೆ (FLAGS_ಗಣ(flags, READ_ತುಂಬಿದ_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            ಸ್ಥಾನಾಂತರ == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_ತುಂಬಿದ(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ read_ತುಂಬಿದ_ಹರಿವು_ತುಂಬಿದ(f, filename, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2, flags, ret_contents, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ script_get_shebang_interpreter(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        f = fopen(ಮಾರ್ಗ, "re");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        ಅಕ್ಷರ c;
        r = ಸುರಕ್ಷಿತ_fgetc(f, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EBADMSG;
        ಆದರೆ (c != '#')
                ಹಿಂತಿರುಗಿ -EMEDIUMTYPE;
        r = ಸುರಕ್ಷಿತ_fgetc(f, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EBADMSG;
        ಆದರೆ (c != '!')
                ಹಿಂತಿರುಗಿ -EMEDIUMTYPE;

        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = ಸಾಲು;

        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&s, &p, /* separators= */ ಖಾಲಿ, /* flags= */ 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -ENOEXEC;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(p);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_proc_ಕ್ಷೇತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ **ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = fopen_unlocked(ಮಾರ್ಗ, "re", &f);
        ಆದರೆ (r == -ENOENT && proc_mounted() == 0)
                ಹಿಂತಿರುಗಿ -ENOSYS;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                 _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;

                 r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                 ಆದರೆ (r < 0)
                         ಹಿಂತಿರುಗಿ r;
                 ಆದರೆ (r == 0)
                         ಹಿಂತಿರುಗಿ -ENODATA;

                 ಅಕ್ಷರ *l = startswith(ಸಾಲು, ಕೀಲಿ);
                 ಆದರೆ (l && *l == ':') {
                         ಆದರೆ (ret) {
                                 ಅಕ್ಷರ *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ಆದರೆ (!s)
                                         ಹಿಂತಿರುಗಿ -ENOMEM;

                                 *ret = s;
                         }

                         ಹಿಂತಿರುಗಿ 0;
                 }
        }
}

DIR* xopendirat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(!(flags & (O_CREAT|O_TMPFILE)));

        ಆದರೆ ((dir_fd == AT_FDCWD || ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ)) &&
            (flags &~ O_DIRECTORY) == 0)
                ಹಿಂತಿರುಗಿ opendir(ಮಾರ್ಗ);

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಮಾರ್ಗ = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, ಮಾರ್ಗ, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ take_fdopendir(&fd);
}

ಪೂರ್ಣಾಂಕ fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ flags;

        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ);

        ಆದರೆ ((p = startswith(ಬಹುಲಕ, "r+")))
                flags = O_RDWR;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p = startswith(ಬಹುಲಕ, "r")))
                flags = O_RDONLY;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p = startswith(ಬಹುಲಕ, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p = startswith(ಬಹುಲಕ, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p = startswith(ಬಹುಲಕ, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((p = startswith(ಬಹುಲಕ, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಗಾಗಿ (; *p != 0; p++) {

                ಆಯ್ಕೆ (*p) {

                ಸ್ಥಿತಿ 'e':
                        flags |= O_CLOEXEC;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ 'x':
                        flags |= O_EXCL;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ 'm':
                        /* ignore this here, fdopen() might care later though */
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ 'c': /* not sure what to do about this one */
                ಪೂರ್ವನಿಯೋಜಿತ:
                        ಹಿಂತಿರುಗಿ -EINVAL;
                }
        }

        ಹಿಂತಿರುಗಿ flags;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xfopenat_regular(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (dir_fd == AT_FDCWD && ಮಾರ್ಗ && open_flags == 0)
                f = fopen(ಮಾರ್ಗ, ಬಹುಲಕ);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir_fd == XAT_FDROOT && ಮಾರ್ಗ && open_flags == 0) {
                _cleanup_free_ ಅಕ್ಷರ *j = strjoin("/", ಮಾರ್ಗ);
                ಆದರೆ (!j)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                f = fopen(j, ಬಹುಲಕ);
        } ಇಲ್ಲದಿದ್ದರೆ {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
                ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_flags;

                ಬಹುಲಕ_flags = fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(ಬಹುಲಕ);
                ಆದರೆ (ಬಹುಲಕ_flags < 0)
                        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_flags;

                ಆದರೆ (ಮಾರ್ಗ) {
                        fd = openat(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ_flags | open_flags);
                        ಆದರೆ (fd < 0)
                                ಹಿಂತಿರುಗಿ -errno;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (dir_fd == AT_FDCWD)
                                ಹಿಂತಿರುಗಿ -EBADF;

                        fd = fd_reopen(dir_fd, (ಬಹುಲಕ_flags | open_flags) & ~O_NOFOLLOW);
                        ಆದರೆ (fd < 0)
                                ಹಿಂತಿರುಗಿ fd;
                }

                f = take_fdopen(&fd, ಬಹುಲಕ);
        }
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = f;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xfopenat_unix_socket(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bind_name, FILE **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ sk = -EBADF;
        FILE *f;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ಆದರೆ (sk < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                ಒಕ್ಕೂಟ sockaddr_union bsa;

                r = sockaddr_un_ಗಣ_ಮಾರ್ಗ(&bsa.un, bind_name);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (bind(sk, &bsa.sa, r) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        r = connect_unix_ಮಾರ್ಗ(sk, dir_fd, ಮಾರ್ಗ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (shutdown(sk, SHUT_WR) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        f = take_fdopen(&sk, "r");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = f;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ xfopenat_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                ಪೂರ್ಣಾಂಕ open_flags,
                XfopenFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bind_name,
                FILE **ret) {

        FILE *f = ಖಾಲಿ;  /* avoid false maybe-uninitialized warning */
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = xfopenat_regular(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, open_flags, &f);
        ಆದರೆ (r == -ENXIO && FLAGS_ಗಣ(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, ಮಾರ್ಗ, bind_name, &f);
                ಆದರೆ (ಒಳ_ಗಣ(r, -ENOTSOCK, -EINVAL))
                        ಹಿಂತಿರುಗಿ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (FLAGS_ಗಣ(flags, XFOPEN_UNLOCKED))
                (ಶೂನ್ಯ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdopen_independent(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_flags;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        ಬಹುಲಕ_flags = fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(ಬಹುಲಕ);
        ಆದರೆ (ಬಹುಲಕ_flags < 0)
                ಹಿಂತಿರುಗಿ ಬಹುಲಕ_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, ಬಹುಲಕ_flags & ~O_CREAT);
        ಆದರೆ (copy_fd < 0)
                ಹಿಂತಿರುಗಿ copy_fd;

        f = take_fdopen(&copy_fd, ಬಹುಲಕ);
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = TAKE_PTR(f);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_open_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ ಬಹುಲಕ,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ **ಹುಡುಕು,
                ಪೂರ್ಣಾಂಕ *ret_fd,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!ret_fd || !FLAGS_ಗಣ(ಬಹುಲಕ, O_CREAT)); /* We don't support O_CREAT for this */
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        ಆದರೆ (ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ)) {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

                ಆದರೆ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(ಮಾರ್ಗ, ಬಹುಲಕ, 0777));
                ಇಲ್ಲದಿದ್ದರೆ
                        r = RET_NERRNO(access(ಮಾರ್ಗ, ಬಹುಲಕ));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ret_ಮಾರ್ಗ) {
                        r = ಮಾರ್ಗ_simplify_alloc(ಮಾರ್ಗ, ret_ಮಾರ್ಗ);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (!ಮಾರ್ಗ_strv_resolve_uniq(ಹುಡುಕು, ಬೇರು))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        STRV_FOREACH(i, ಹುಡುಕು) {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
                _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                p = ಮಾರ್ಗ_join(ಬೇರು, *i, ಮಾರ್ಗ);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, ಬಹುಲಕ, 0777));
                ಇಲ್ಲದಿದ್ದರೆ
                        r = RET_NERRNO(access(p, F_OK));
                ಆದರೆ (r >= 0) {
                        ಆದರೆ (ret_ಮಾರ್ಗ)
                                *ret_ಮಾರ್ಗ = ಮಾರ್ಗ_simplify(TAKE_PTR(p));

                        ಆದರೆ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (r != -ENOENT)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ -ENOENT;
}

ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_open(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ ಬಹುಲಕ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ **ಹುಡುಕು,
                ಪೂರ್ಣಾಂಕ *ret_fd,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_strv_free_ ಅಕ್ಷರ **copy = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        copy = strv_copy(ಹುಡುಕು);
        ಆದರೆ (!copy)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ ಹುಡುಕು_and_open_ಆಂತರಿಕ(ಮಾರ್ಗ, ಬಹುಲಕ, ಬೇರು, copy, ret_fd, ret_ಮಾರ್ಗ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_fopen_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ **ಹುಡುಕು,
                FILE **ret_file,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_free_ ಅಕ್ಷರ *ಸಿಕ್ಕಿತು_ಮಾರ್ಗ = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ || !ret_file);

        r = ಹುಡುಕು_and_open_ಆಂತರಿಕ(
                        ಮಾರ್ಗ,
                        ಬಹುಲಕ ? fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(ಬಹುಲಕ) : 0,
                        ಬೇರು,
                        ಹುಡುಕು,
                        ret_file ? &fd : ಖಾಲಿ,
                        ret_ಮಾರ್ಗ ? &ಸಿಕ್ಕಿತು_ಮಾರ್ಗ : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_file) {
                FILE *f = take_fdopen(&fd, ಬಹುಲಕ);
                ಆದರೆ (!f)
                        ಹಿಂತಿರುಗಿ -errno;

                *ret_file = f;
        }

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(ಸಿಕ್ಕಿತು_ಮಾರ್ಗ);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_fopen(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಹುಡುಕು,
                FILE **ret_file,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_strv_free_ ಅಕ್ಷರ **copy = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ || !ret_file);

        copy = strv_copy((ಅಕ್ಷರ**) ಹುಡುಕು);
        ಆದರೆ (!copy)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ ಹುಡುಕು_and_fopen_ಆಂತರಿಕ(ಮಾರ್ಗ, ಬಹುಲಕ, ಬೇರು, copy, ret_file, ret_ಮಾರ್ಗ);
}

ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_fopen_nulstr(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಹುಡುಕು,
                FILE **ret_file,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ || !ret_file);

        l = strv_ವಿಭಜಿಸು_nulstr(ಹುಡುಕು);
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ ಹುಡುಕು_and_fopen_ಆಂತರಿಕ(ಮಾರ್ಗ, ಬಹುಲಕ, ಬೇರು, l, ret_file, ret_ಮಾರ್ಗ);
}

ಪೂರ್ಣಾಂಕ fflush_and_check(FILE *f) {
        ಪ್ರತಿಜ್ಞಾ(f);

        errno = 0;
        fflush(f);

        ಆದರೆ (ferror(f))
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fflush_sync_and_check(FILE *f) {
        ಪೂರ್ಣಾಂಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(f);

        r = fflush_and_check(f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ 0;

        r = fsync_ತುಂಬಿದ(fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ write_timestamp_file_atomic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, usec_t n) {
        ಅಕ್ಷರ ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ಆದರೆ (!timestamp_is_ಗಣ(n))
                ಹಿಂತಿರುಗಿ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ಹಿಂತಿರುಗಿ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

ಪೂರ್ಣಾಂಕ read_timestamp_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, usec_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ln = ಖಾಲಿ;
        uint64_t t;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = read_one_ಸಾಲು_file(fn, &ln);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಸುರಕ್ಷಿತ_atou64(ln, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!timestamp_is_ಗಣ(t))
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (usec_t) t;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fputs_with_separator(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, bool *space) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ಆದರೆ (!f)
                f = stdout;

        ಆದರೆ (!separator)
                separator = " ";

        ಆದರೆ (*space)
                ಆದರೆ (fputs(separator, f) < 0)
                        ಹಿಂತಿರುಗಿ -EIO;

        *space = true;

        ಆದರೆ (fputs(s, f) < 0)
                ಹಿಂತಿರುಗಿ -EIO;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fputs_with_newline(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ಆದರೆ (isempty(s))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!f)
                f = stdout;

        ಆದರೆ (fputs(s, f) < 0)
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (endswith(s, "\n"))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (fputc('\n', f) < 0)
                ಹಿಂತಿರುಗಿ -EIO;

        ಹಿಂತಿರುಗಿ 1;
}

/* A bitmask of the EOL markers we know */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಕೊನೆ_ದ_ಸಾಲು_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} ಕೊನೆ_ದ_ಸಾಲು_Marker;

ಸ್ಥಿರ ಕೊನೆ_ದ_ಸಾಲು_Marker categorize_eol(ಅಕ್ಷರ c, Read_ಸಾಲು_Flags flags) {

        ಆದರೆ (!FLAGS_ಗಣ(flags, READ_ಸಾಲು_ONLY_NUL)) {
                ಆದರೆ (c == '\n')
                        ಹಿಂತಿರುಗಿ EOL_TEN;
                ಆದರೆ (c == '\r')
                        ಹಿಂತಿರುಗಿ EOL_THIRTEEN;
        }

        ಆದರೆ (c == '\0')
                ಹಿಂತಿರುಗಿ EOL_ZERO;

        ಹಿಂತಿರುಗಿ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_ತುಂಬಿದ(FILE*, funlockfile, ಖಾಲಿ);

ಪೂರ್ಣಾಂಕ read_ಸಾಲು_ತುಂಬಿದ(FILE *f, size_t limit, Read_ಸಾಲು_Flags flags, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಬಫರ್ = ಖಾಲಿ;
        size_t n = 0, ಎಣಿಕೆ_2 = 0;
        ಪೂರ್ಣಾಂಕ r;

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

        ಆದರೆ (ret) {
                ಆದರೆ (!GREEDY_REALLOC(ಬಫರ್, 1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        {
                ಬಳಸದ _cleanup_(funlockfilep) FILE *flocked = f;
                ಕೊನೆ_ದ_ಸಾಲು_Marker ಹಿಂದಿನ_eol = EOL_NONE;
                flockfile(f);

                ಗಾಗಿ (;;) {
                        ಕೊನೆ_ದ_ಸಾಲು_Marker eol;
                        ಅಕ್ಷರ c;

                        ಆದರೆ (n >= limit)
                                ಹಿಂತಿರುಗಿ -ENOBUFS;

                        ಆದರೆ (ಎಣಿಕೆ_2 >= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ಹಿಂತಿರುಗಿ -ENOBUFS;

                        r = ಸುರಕ್ಷಿತ_fgetc(f, &c);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0) /* EOF is definitely EOL */
                                ನಿಲ್ಲಿಸು;

                        eol = categorize_eol(c, flags);

                        ಆದರೆ (FLAGS_ಗಣ(ಹಿಂದಿನ_eol, EOL_ZERO) ||
                            (eol == EOL_NONE && ಹಿಂದಿನ_eol != EOL_NONE) ||
                            (eol != EOL_NONE && (ಹಿಂದಿನ_eol & eol) != 0)) {
                                /* Previous char was a NUL? This is not an EOL, but the previous char was? This type of
                                 * EOL marker has been seen right before?  In either of these three cases we are
                                 * done. But first, let's put this character back in the queue. (Note that we have to
                                 * cast this to (unsigned char) here as ungetc() expects a positive 'int', and if we
                                 * are on an architecture where 'char' equals 'signed char' we need to ensure we don't
                                 * pass a negative value here. That said, to complicate things further ungetc() is
                                 * actually happy with most negative characters and implicitly casts them back to
                                 * positive ones as needed, except for \xff (aka -1, aka EOF), which it refuses. What a
                                 * godawful API!) */
                                assert_se(ungetc((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c, f) != EOF);
                                ನಿಲ್ಲಿಸು;
                        }

                        ಎಣಿಕೆ_2++;

                        ಆದರೆ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ಆದರೆ ((flags & (READ_ಸಾಲು_IS_A_TTY|READ_ಸಾಲು_NOT_A_TTY)) == 0) {
                                        ಪೂರ್ಣಾಂಕ fd;

                                        fd = fileno(f);
                                        ಆದರೆ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_ಸಾಲು_NOT_A_TTY;
                                        ಇಲ್ಲದಿದ್ದರೆ
                                                flags |= isatty_ಸುರಕ್ಷಿತ(fd) ? READ_ಸಾಲು_IS_A_TTY : READ_ಸಾಲು_NOT_A_TTY;
                                }
                                ಆದರೆ (FLAGS_ಗಣ(flags, READ_ಸಾಲು_IS_A_TTY))
                                        ನಿಲ್ಲಿಸು;
                        }

                        ಆದರೆ (eol != EOL_NONE) {
                                ಹಿಂದಿನ_eol |= eol;
                                ಮುಂದುವರಿ;
                        }

                        ಆದರೆ (ret) {
                                ಆದರೆ (!GREEDY_REALLOC(ಬಫರ್, n + 2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಬಫರ್[n] = c;
                        }

                        n++;
                }
        }

        ಆದರೆ (ret) {
                ಬಫರ್[n] = 0;

                *ret = TAKE_PTR(ಬಫರ್);
        }

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) ಎಣಿಕೆ_2;
}

ಪೂರ್ಣಾಂಕ read_stripped_ಸಾಲು(FILE *f, size_t limit, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r, k;

        ಪ್ರತಿಜ್ಞಾ(f);

        r = read_ಸಾಲು(f, limit, ret ? &s : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strstrip(s);
                ಆದರೆ (p == s)
                        *ret = TAKE_PTR(s);
                ಇಲ್ಲದಿದ್ದರೆ {
                        k = strdup_ಗಮ್ಯ(ret, p);
                        ಆದರೆ (k < 0)
                                ಹಿಂತಿರುಗಿ k;
                }
        }

        ಹಿಂತಿರುಗಿ r > 0;          /* Return 1 if something was read. */
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_fgetc(FILE *f, ಅಕ್ಷರ *ret) {
        ಪೂರ್ಣಾಂಕ k;

        ಪ್ರತಿಜ್ಞಾ(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ಆದರೆ (k == EOF) {
                ಆದರೆ (ferror(f))
                        ಹಿಂತಿರುಗಿ errno_or_else(EIO);

                ಆದರೆ (ret)
                        *ret = 0;

                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (ret)
                *ret = k;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ warn_file_is_world_accessible(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ರಚನೆ stat *st, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit, ಚಿಹ್ನೆರಹಿತ ಸಾಲು) {
        ರಚನೆ stat _st;

        ಆದರೆ (!filename)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!st) {
                ಆದರೆ (stat(filename, &_st) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                st = &_st;
        }

        ಆದರೆ ((st->st_mode & S_IRWXO) == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (unit)
                log_syntax(unit, LOG_WARNING, filename, ಸಾಲು, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ಇಲ್ಲದಿದ್ದರೆ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ write_ದತ್ತಾಂಶ_file_atomic_at(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iovec,
                Write_ದತ್ತಾಂಶ_File_Flags flags) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ ಅಕ್ಷರ *dn = ಖಾಲಿ, *fn = ಖಾಲಿ;
        r = ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(ಮಾರ್ಗ, &dn, &fn);
        ಆದರೆ (ಒಳ_ಗಣ(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ಹಿಂತಿರುಗಿ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ mfd = -EBADF;
        ಆದರೆ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_ಗಣ(flags, WRITE_ದತ್ತಾಂಶ_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ಖಾಲಿ,
                            &mfd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                dir_fd = mfd;
        }

        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ಆದರೆ (iovec_is_ಗಣ(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        r = fchmod_umask(fd, FLAGS_ಗಣ(flags, WRITE_ದತ್ತಾಂಶ_FILE_ಬಹುಲಕ_0400) ? 0400 : 0644);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ಹಿಂತಿರುಗಿ 0;
}
