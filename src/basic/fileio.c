/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio_ext.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "label-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mkdir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "nulstr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socket-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sync-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "terminal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tmpfile-util.h"

/* The maximum size of the file we'll read in one go in read_full_file() (64M). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_ਭਰਿਆ_ਬਾਈਟਾਂ_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_ਭਰਿਆ_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_VIRTUAL_ਬਾਈਟਾਂ_MAX (4U * U64_MB - UINT64_C(2))

ਪੂਰਨ fdopen_unlocked(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *options, FILE **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        FILE *f = fdopen(fd, options);
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        (ਖਾਲੀ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ਵਾਪਸ 0;
}

ਪੂਰਨ take_fdopen_unlocked(ਪੂਰਨ *fd, ਅਚਲ ਅੱਖਰ *options, FILE **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *fd = -EBADF;

        ਵਾਪਸ 0;
}

FILE* take_fdopen(ਪੂਰਨ *fd, ਅਚਲ ਅੱਖਰ *options) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd);

        FILE *f = fdopen(*fd, options);
        ਜੇ (!f)
                ਵਾਪਸ ਨਿਰਾ;

        *fd = -EBADF;

        ਵਾਪਸ f;
}

DIR* take_fdopendir(ਪੂਰਨ *dfd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(dfd);

        DIR *d = fdopendir(*dfd);
        ਜੇ (!d)
                ਵਾਪਸ ਨਿਰਾ;

        *dfd = -EBADF;

        ਵਾਪਸ d;
}

FILE* open_memstream_unlocked(ਅੱਖਰ **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ਜੇ (!f)
                ਵਾਪਸ ਨਿਰਾ;

        (ਖਾਲੀ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ਵਾਪਸ f;
}

FILE* fmemopen_unlocked(ਖਾਲੀ *buf, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ) {
        FILE *f = fmemopen(buf, ਮਾਪ, ਬਹੁਲਕ);
        ਜੇ (!f)
                ਵਾਪਸ ਨਿਰਾ;

        (ਖਾਲੀ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ਵਾਪਸ f;
}

ਪੂਰਨ write_string_ਧਾਰਾ_ਭਰਿਆ(
                FILE *f,
                ਅਚਲ ਅੱਖਰ *ਸਤਰ,
                WriteStringFileFlags flags,
                ਅਚਲ ਬਣਤਰ timespec *ts) {

        bool needs_nl;
        ਪੂਰਨ r, fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਤਰ);

        ਜੇ (ferror(f))
                ਵਾਪਸ -EIO;

        ਜੇ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -EBADF;
        }

        ਜੇ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ਜੇ (fd < 0) {
                        fd = fileno(f);
                        ਜੇ (fd < 0)
                                ਵਾਪਸ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ਜੇ (read_virtual_file_fd(fd, ਲੜੀ_ਲੰਬਾਈ(ਸਤਰ)+1, &t, ਨਿਰਾ) > 0 &&
                    streq_skip_trailing_chars(ਸਤਰ, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", ਸਤਰ);
                        ਵਾਪਸ 0;
                }

                ਜੇ (lseek(fd, 0, SEEK_SET) < 0)
                        ਵਾਪਸ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(ਸਤਰ, "\n");

        ਜੇ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_ਬਫਰ)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                ਸਤਰ = strjoina(ਸਤਰ, "\n");
                needs_nl = false;
        }

        ਜੇ (fputs(ਸਤਰ, f) == EOF)
                ਵਾਪਸ -errno;

        ਜੇ (needs_nl)
                ਜੇ (fputc('\n', f) == EOF)
                        ਵਾਪਸ -errno;

        ਜੇ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ਨਹੀਂਤਾਂ
                r = fflush_and_check(f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ts) {
                ਅਚਲ ਬਣਤਰ timespec twice[2] = {*ts, *ts};

                ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
                ਜੇ (futimens(fd, twice) < 0)
                        ਵਾਪਸ -errno;
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ mode_t write_string_file_flags_ਮੰਜ਼ਿਲ_ਬਹੁਲਕ(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ਵਾਪਸ FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_ਬਹੁਲਕ_0600) ? 0600 :
                FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_ਬਹੁਲਕ_0444) ? 0444 : 0644;
}

ਸਥਿਰ ਪੂਰਨ write_string_file_atomic_at(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *fn,
                ਅਚਲ ਅੱਖਰ *ਸਤਰ,
                WriteStringFileFlags flags,
                ਅਚਲ ਬਣਤਰ timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fn);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਤਰ);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t ਬਹੁਲਕ = write_string_file_flags_ਮੰਜ਼ਿਲ_ਬਹੁਲਕ(flags);

        bool call_label_ops_post = false;
        ਜੇ (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, ਬਹੁਲਕ, label_context);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        ਪੂਰਨ k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ਨਿਰਾ : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ਜੇ (k < 0)
                ਵਾਪਸ k;

        r = write_string_ਧਾਰਾ_ਭਰਿਆ(f, ਸਤਰ, flags, ts);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = fchmod_umask(fileno(f), ਬਹੁਲਕ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ਜੇ (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_ਦਾ_file(fileno(f));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ write_string_file_ਭਰਿਆ_label(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *fn,
                ਅਚਲ ਅੱਖਰ *ਸਤਰ,
                WriteStringFileFlags flags,
                ਅਚਲ ਬਣਤਰ timespec *ts,
                ਅਚਲ ਅੱਖਰ *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd == AT_FDCWD || dir_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਤਰ);

        /* We don't know how to verify whether the file contents was already on-disk. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ਜੇ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                ਪ੍ਰਤਿਜ੍ਞਾ(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (flags & WRITE_STRING_FILE_ATOMIC) {
                ਪ੍ਰਤਿਜ੍ਞਾ(fn);
                ਪ੍ਰਤਿਜ੍ਞਾ(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, ਸਤਰ, flags, ts, label_context);
                ਜੇ (r < 0)
                        ਜਾਓ fail;

                ਵਾਪਸ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ਜੇ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ਨਹੀਂਤਾਂ {
                mode_t ਬਹੁਲਕ = write_string_file_flags_ਮੰਜ਼ਿਲ_ਬਹੁਲਕ(flags);
                bool call_label_ops_post = false;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, ਬਹੁਲਕ, label_context);
                        ਜੇ (r < 0)
                                ਜਾਓ fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_ਸੈੱਟ(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                ਬਹੁਲਕ,
                                &made_file);
                ਜੇ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ਨਿਰਾ : fn, made_file, label_context));
        }
        ਜੇ (r < 0)
                ਜਾਓ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ਜੇ (r < 0)
                ਜਾਓ fail;

        ਜੇ (flags & WRITE_STRING_FILE_DISABLE_ਬਫਰ)
                setvbuf(f, ਨਿਰਾ, _IONBF, 0);

        r = write_string_ਧਾਰਾ_ਭਰਿਆ(f, ਸਤਰ, flags, ts);
        ਜੇ (r < 0)
                ਜਾਓ fail;

        ਵਾਪਸ 0;

fail:
        ਜੇ (made_file)
                (ਖਾਲੀ) unlinkat(dir_fd, fn, 0);

        ਜੇ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ਵਾਪਸ r;

        f = ਸੁਰੱਖਿਅਤ_fclose(f);
        fd = ਸੁਰੱਖਿਅਤ_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ਜੇ (verify_file_at(dir_fd, fn, ਸਤਰ, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ਵਾਪਸ 0;

        ਵਾਪਸ r;
}

ਪੂਰਨ write_string_filef(
                ਅਚਲ ਅੱਖਰ *fn,
                WriteStringFileFlags flags,
                ਅਚਲ ਅੱਖਰ *format, ...) {

        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ write_string_file(fn, p, flags);
}

ਪੂਰਨ write_string_filef_at(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *fn,
                WriteStringFileFlags flags,
                ਅਚਲ ਅੱਖਰ *format, ...) {

        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ write_string_file_at(dir_fd, fn, p, flags);
}

ਪੂਰਨ write_base64_file_at(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *fn,
                ਅਚਲ ਬਣਤਰ iovec *ਅੰਕੜੇ,
                WriteStringFileFlags flags) {

        _cleanup_free_ ਅੱਖਰ *encoded = ਨਿਰਾ;
        ssize_t n;

        n = base64mem_ਭਰਿਆ(ਅੰਕੜੇ ? ਅੰਕੜੇ->iov_base : ਨਿਰਾ, ਅੰਕੜੇ ? ਅੰਕੜੇ->iov_len : 0, 79, &encoded);
        ਜੇ (n < 0)
                ਵਾਪਸ n;

        ਵਾਪਸ write_string_file_at(dir_fd, fn, encoded, flags);
}

ਪੂਰਨ read_one_ਸਤਰ_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(filename);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, ret);
}

ਪੂਰਨ read_boolean_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(filename);

        r = read_one_ਸਤਰ_file_at(dir_fd, filename, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_boolean(s);
}

ਪੂਰਨ verify_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        size_t l, k;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(blob);

        l = ਲੜੀ_ਲੰਬਾਈ(blob);

        ਜੇ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(l + accept_extra_nl + 1);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ਜੇ (ferror(f))
                ਵਾਪਸ errno_or_else(EIO);

        ਜੇ (k != l && k != l + accept_extra_nl)
                ਵਾਪਸ 0;
        ਜੇ (memcmp(buf, blob, l) != 0)
                ਵਾਪਸ 0;
        ਜੇ (k > l && buf[l] != '\n')
                ਵਾਪਸ 0;

        ਵਾਪਸ 1;
}

ਪੂਰਨ read_virtual_file_at(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *filename,
                size_t max_ਮਾਪ,
                ਅੱਖਰ **ret_contents,
                size_t *ret_ਮਾਪ) {

        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        size_t n, ਮਾਪ;
        ਪੂਰਨ n_retries;
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

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(max_ਮਾਪ <= READ_VIRTUAL_ਬਾਈਟਾਂ_MAX || max_ਮਾਪ == SIZE_MAX);

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਜੇ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ਨਹੀਂਤਾਂ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        ਲਈ (;;) {
                ਬਣਤਰ stat st;

                ਜੇ (fstat(fd, &st) < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (!S_ISREG(st.st_mode))
                        ਵਾਪਸ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_ਬਾਈਟਾਂ_MAX < SSIZE_MAX);
                ਜੇ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ਜੇ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ਜੇ (max_ਮਾਪ == SIZE_MAX)
                                        ਵਾਪਸ -EFBIG;

                                ਮਾਪ = max_ਮਾਪ;
                        } ਨਹੀਂਤਾਂ {
                                ਮਾਪ = MIN((size_t) st.st_size, max_ਮਾਪ);

                                ਜੇ (ਮਾਪ > READ_VIRTUAL_ਬਾਈਟਾਂ_MAX)
                                        ਵਾਪਸ -EFBIG;
                        }

                        n_retries--;
                } ਨਹੀਂਤਾਂ ਜੇ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        ਮਾਪ = MIN3(ਪੰਨਾ_ਮਾਪ() - 1, READ_VIRTUAL_ਬਾਈਟਾਂ_MAX, max_ਮਾਪ);
                        n_retries = 1;
                } ਨਹੀਂਤਾਂ {
                        ਮਾਪ = MIN(READ_VIRTUAL_ਬਾਈਟਾਂ_MAX, max_ਮਾਪ);
                        n_retries = 0;
                }

                buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ + 1);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                ਮਾਪ = MIN3(MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(buf) - 1, max_ਮਾਪ, READ_VIRTUAL_ਬਾਈਟਾਂ_MAX);

                ਲਈ (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = ਪਠ(fd, buf, ਮਾਪ + 1);
                        ਜੇ (k >= 0) {
                                n = k;
                                ਰੋਕੋ;
                        }

                        ਜੇ (errno != EINTR)
                                ਵਾਪਸ -errno;
                }

                /* Consider a short read as EOF */
                ਜੇ (n <= ਮਾਪ)
                        ਰੋਕੋ;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ਜੇ (max_ਮਾਪ != SIZE_MAX && n > max_ਮਾਪ) {
                        n = ਮਾਪ; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        ਰੋਕੋ;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ਜੇ (n_retries <= 0)
                        ਵਾਪਸ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ਜੇ (lseek(fd, 0, SEEK_SET) < 0)
                        ਵਾਪਸ -errno;

                buf = mfree(buf);
        }

        ਜੇ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ਜੇ (!ret_ਮਾਪ && memchr(buf, 0, n))
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (n < ਮਾਪ) {
                        ਅੱਖਰ *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ਜੇ (!p)
                                ਵਾਪਸ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = n;

        ਵਾਪਸ !truncated;
}

ਪੂਰਨ read_ਭਰਿਆ_ਧਾਰਾ_ਭਰਿਆ(
                FILE *f,
                ਅਚਲ ਅੱਖਰ *filename,
                uint64_t ਵਿੱਥ,
                size_t ਮਾਪ,
                Read_ਭਰਿਆ_File_Flags flags,
                ਅੱਖਰ **ret_contents,
                size_t *ret_ਮਾਪ) {

        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        size_t n, n_ਅਗਲਾ = 0, l, expected_decoded_ਮਾਪ = ਮਾਪ;
        ਪੂਰਨ fd, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_contents);
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_UNBASE64 | READ_ਭਰਿਆ_FILE_UNHEX));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ != SIZE_MAX || !FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER));

        ਜੇ (ਵਿੱਥ != UINT64_MAX && ਵਿੱਥ > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ਵਾਪਸ -ERANGE;

        ਜੇ ((flags & (READ_ਭਰਿਆ_FILE_UNBASE64 | READ_ਭਰਿਆ_FILE_UNHEX)) != 0) {
                ਜੇ (ਮਾਪ <= SIZE_MAX / READ_ਭਰਿਆ_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        ਮਾਪ *= READ_ਭਰਿਆ_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ਨਹੀਂਤਾਂ
                        ਮਾਪ = SIZE_MAX;
        }

        fd = fileno(f);
        ਜੇ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ਬਣਤਰ stat st;

                ਜੇ (fstat(fd, &st) < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ਜੇ (st.st_size > 0 &&
                            (ਮਾਪ == SIZE_MAX || FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, ਵਿੱਥ == UINT64_MAX ? 0 : ਵਿੱਥ);

                                ਜੇ (rsize < SIZE_MAX) /* overflow check */
                                        n_ਅਗਲਾ = rsize + 1;
                        }

                        ਜੇ (flags & READ_ਭਰਿਆ_FILE_WARN_WORLD_READABLE)
                                (ਖਾਲੀ) warn_file_is_world_accessible(filename, &st, ਨਿਰਾ, 0);
                }
        } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_VERIFY_REGULAR))
                ਵਾਪਸ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ਜੇ (n_ਅਗਲਾ == 0)
                n_ਅਗਲਾ = ਮਾਪ != SIZE_MAX && !FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER) ? ਮਾਪ : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ਜੇ (n_ਅਗਲਾ > READ_ਭਰਿਆ_ਬਾਈਟਾਂ_MAX)
                n_ਅਗਲਾ = READ_ਭਰਿਆ_ਬਾਈਟਾਂ_MAX + 1;

        ਜੇ (ਵਿੱਥ != UINT64_MAX && fseek(f, ਵਿੱਥ, SEEK_SET) < 0)
                ਵਾਪਸ -errno;

        n = l = 0;
        ਲਈ (;;) {
                ਅੱਖਰ *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER) && n_ਅਗਲਾ > ਮਾਪ)
                        n_ਅਗਲਾ = ਮਾਪ + 1;

                ਜੇ (flags & READ_ਭਰਿਆ_FILE_SECURE) {
                        t = ਸਮ੍ਰਿਤੀ_ਦਿਓ(n_ਅਗਲਾ + 1);
                        ਜੇ (!t) {
                                r = -ENOMEM;
                                ਜਾਓ finalize;
                        }
                        memcpy_ਸੁਰੱਖਿਅਤ(t, buf, n);
                        explicit_bzero_ਸੁਰੱਖਿਅਤ(buf, n);
                        ਮੁਕਤ(buf);
                } ਨਹੀਂਤਾਂ {
                        t = realloc(buf, n_ਅਗਲਾ + 1);
                        ਜੇ (!t)
                                ਵਾਪਸ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = ਮਾਪ == SIZE_MAX ? MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(buf) - 1 : n_ਅਗਲਾ;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                ਪ੍ਰਤਿਜ੍ਞਾ(k <= n - l);
                l += k;

                ਜੇ (ferror(f)) {
                        r = errno_or_else(EIO);
                        ਜਾਓ finalize;
                }
                ਜੇ (feof(f))
                        ਰੋਕੋ;

                ਜੇ (ਮਾਪ != SIZE_MAX && !FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        ਪ੍ਰਤਿਜ੍ਞਾ(l == ਮਾਪ);
                        ਰੋਕੋ;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER) && l > ਮਾਪ) {
                        r = -E2BIG;
                        ਜਾਓ finalize;
                }

                ਜੇ (n >= READ_ਭਰਿਆ_ਬਾਈਟਾਂ_MAX) {
                        r = -E2BIG;
                        ਜਾਓ finalize;
                }

                n_ਅਗਲਾ = MIN(n * 2, READ_ਭਰਿਆ_ਬਾਈਟਾਂ_MAX);
        }

        ਜੇ (flags & (READ_ਭਰਿਆ_FILE_UNBASE64 | READ_ਭਰਿਆ_FILE_UNHEX)) {
                _cleanup_free_ ਖਾਲੀ *decoded = ਨਿਰਾ;
                size_t decoded_ਮਾਪ;

                buf[l++] = 0;
                ਜੇ (flags & READ_ਭਰਿਆ_FILE_UNBASE64)
                        r = unbase64mem_ਭਰਿਆ(buf, l, flags & READ_ਭਰਿਆ_FILE_SECURE, &decoded, &decoded_ਮਾਪ);
                ਨਹੀਂਤਾਂ
                        r = unhexmem_ਭਰਿਆ(buf, l, flags & READ_ਭਰਿਆ_FILE_SECURE, &decoded, &decoded_ਮਾਪ);
                ਜੇ (r < 0)
                        ਜਾਓ finalize;

                ਜੇ (flags & READ_ਭਰਿਆ_FILE_SECURE)
                        explicit_bzero_ਸੁਰੱਖਿਅਤ(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_ਮਾਪ;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_ਮਾਪ) {
                        r = -E2BIG;
                        ਜਾਓ finalize;
                }
        }

        ਜੇ (!ret_ਮਾਪ) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ਜੇ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        ਜਾਓ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = l;

        ਵਾਪਸ 0;

finalize:
        ਜੇ (flags & READ_ਭਰਿਆ_FILE_SECURE)
                explicit_bzero_ਸੁਰੱਖਿਅਤ(buf, n);

        ਵਾਪਸ r;
}

ਪੂਰਨ read_ਭਰਿਆ_file_ਭਰਿਆ(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *filename,
                uint64_t ਵਿੱਥ,
                size_t ਮਾਪ,
                Read_ਭਰਿਆ_File_Flags flags,
                ਅਚਲ ਅੱਖਰ *bind_name,
                ਅੱਖਰ **ret_contents,
                size_t *ret_ਮਾਪ) {

        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_contents);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਭਰਿਆ_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            ਵਿੱਥ == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_ਭਰਿਆ(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ read_ਭਰਿਆ_ਧਾਰਾ_ਭਰਿਆ(f, filename, ਵਿੱਥ, ਮਾਪ, flags, ret_contents, ret_ਮਾਪ);
}

ਪੂਰਨ script_get_shebang_interpreter(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        f = fopen(ਰਸਤਾ, "re");
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        ਅੱਖਰ c;
        r = ਸੁਰੱਖਿਅਤ_fgetc(f, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EBADMSG;
        ਜੇ (c != '#')
                ਵਾਪਸ -EMEDIUMTYPE;
        r = ਸੁਰੱਖਿਅਤ_fgetc(f, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EBADMSG;
        ਜੇ (c != '!')
                ਵਾਪਸ -EMEDIUMTYPE;

        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *s = ਸਤਰ;

        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&s, &p, /* separators= */ ਨਿਰਾ, /* flags= */ 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -ENOEXEC;

        ਜੇ (ret)
                *ret = TAKE_PTR(p);
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_proc_ਖੇਤਰ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ **ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        r = fopen_unlocked(ਰਸਤਾ, "re", &f);
        ਜੇ (r == -ENOENT && proc_mounted() == 0)
                ਵਾਪਸ -ENOSYS;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                 _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;

                 r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                 ਜੇ (r < 0)
                         ਵਾਪਸ r;
                 ਜੇ (r == 0)
                         ਵਾਪਸ -ENODATA;

                 ਅੱਖਰ *l = startswith(ਸਤਰ, ਕੁੰਜੀ);
                 ਜੇ (l && *l == ':') {
                         ਜੇ (ret) {
                                 ਅੱਖਰ *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ਜੇ (!s)
                                         ਵਾਪਸ -ENOMEM;

                                 *ret = s;
                         }

                         ਵਾਪਸ 0;
                 }
        }
}

DIR* xopendirat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(!(flags & (O_CREAT|O_TMPFILE)));

        ਜੇ ((dir_fd == AT_FDCWD || ਰਸਤਾ_is_absolute(ਰਸਤਾ)) &&
            (flags &~ O_DIRECTORY) == 0)
                ਵਾਪਸ opendir(ਰਸਤਾ);

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਰਸਤਾ = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, ਰਸਤਾ, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ਜੇ (fd < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ take_fdopendir(&fd);
}

ਪੂਰਨ fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ) {
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ flags;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ);

        ਜੇ ((p = startswith(ਬਹੁਲਕ, "r+")))
                flags = O_RDWR;
        ਨਹੀਂਤਾਂ ਜੇ ((p = startswith(ਬਹੁਲਕ, "r")))
                flags = O_RDONLY;
        ਨਹੀਂਤਾਂ ਜੇ ((p = startswith(ਬਹੁਲਕ, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ਨਹੀਂਤਾਂ ਜੇ ((p = startswith(ਬਹੁਲਕ, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ਨਹੀਂਤਾਂ ਜੇ ((p = startswith(ਬਹੁਲਕ, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ਨਹੀਂਤਾਂ ਜੇ ((p = startswith(ਬਹੁਲਕ, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -EINVAL;

        ਲਈ (; *p != 0; p++) {

                ਚੋਣ (*p) {

                ਹਾਲਤ 'e':
                        flags |= O_CLOEXEC;
                        ਰੋਕੋ;

                ਹਾਲਤ 'x':
                        flags |= O_EXCL;
                        ਰੋਕੋ;

                ਹਾਲਤ 'm':
                        /* ignore this here, fdopen() might care later though */
                        ਰੋਕੋ;

                ਹਾਲਤ 'c': /* not sure what to do about this one */
                ਮੂਲ:
                        ਵਾਪਸ -EINVAL;
                }
        }

        ਵਾਪਸ flags;
}

ਸਥਿਰ ਪੂਰਨ xfopenat_regular(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, ਪੂਰਨ open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (dir_fd == AT_FDCWD && ਰਸਤਾ && open_flags == 0)
                f = fopen(ਰਸਤਾ, ਬਹੁਲਕ);
        ਨਹੀਂਤਾਂ ਜੇ (dir_fd == XAT_FDROOT && ਰਸਤਾ && open_flags == 0) {
                _cleanup_free_ ਅੱਖਰ *j = strjoin("/", ਰਸਤਾ);
                ਜੇ (!j)
                        ਵਾਪਸ -ENOMEM;

                f = fopen(j, ਬਹੁਲਕ);
        } ਨਹੀਂਤਾਂ {
                _cleanup_close_ ਪੂਰਨ fd = -EBADF;
                ਪੂਰਨ ਬਹੁਲਕ_flags;

                ਬਹੁਲਕ_flags = fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(ਬਹੁਲਕ);
                ਜੇ (ਬਹੁਲਕ_flags < 0)
                        ਵਾਪਸ ਬਹੁਲਕ_flags;

                ਜੇ (ਰਸਤਾ) {
                        fd = openat(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ_flags | open_flags);
                        ਜੇ (fd < 0)
                                ਵਾਪਸ -errno;
                } ਨਹੀਂਤਾਂ {
                        ਜੇ (dir_fd == AT_FDCWD)
                                ਵਾਪਸ -EBADF;

                        fd = fd_reopen(dir_fd, (ਬਹੁਲਕ_flags | open_flags) & ~O_NOFOLLOW);
                        ਜੇ (fd < 0)
                                ਵਾਪਸ fd;
                }

                f = take_fdopen(&fd, ਬਹੁਲਕ);
        }
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        *ret = f;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ xfopenat_unix_socket(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *bind_name, FILE **ret) {
        _cleanup_close_ ਪੂਰਨ sk = -EBADF;
        FILE *f;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ਜੇ (sk < 0)
                ਵਾਪਸ -errno;

        ਜੇ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                ਸੰਘ sockaddr_union bsa;

                r = sockaddr_un_ਸੈੱਟ_ਰਸਤਾ(&bsa.un, bind_name);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (bind(sk, &bsa.sa, r) < 0)
                        ਵਾਪਸ -errno;
        }

        r = connect_unix_ਰਸਤਾ(sk, dir_fd, ਰਸਤਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (shutdown(sk, SHUT_WR) < 0)
                ਵਾਪਸ -errno;

        f = take_fdopen(&sk, "r");
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        *ret = f;
        ਵਾਪਸ 0;
}

ਪੂਰਨ xfopenat_ਭਰਿਆ(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                ਪੂਰਨ open_flags,
                XfopenFlags flags,
                ਅਚਲ ਅੱਖਰ *bind_name,
                FILE **ret) {

        FILE *f = ਨਿਰਾ;  /* avoid false maybe-uninitialized warning */
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = xfopenat_regular(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, open_flags, &f);
        ਜੇ (r == -ENXIO && FLAGS_ਸੈੱਟ(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, ਰਸਤਾ, bind_name, &f);
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(r, -ENOTSOCK, -EINVAL))
                        ਵਾਪਸ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, XFOPEN_UNLOCKED))
                (ਖਾਲੀ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ਵਾਪਸ 0;
}

ਪੂਰਨ fdopen_independent(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE **ret) {
        _cleanup_close_ ਪੂਰਨ copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ ਬਹੁਲਕ_flags;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        ਬਹੁਲਕ_flags = fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(ਬਹੁਲਕ);
        ਜੇ (ਬਹੁਲਕ_flags < 0)
                ਵਾਪਸ ਬਹੁਲਕ_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, ਬਹੁਲਕ_flags & ~O_CREAT);
        ਜੇ (copy_fd < 0)
                ਵਾਪਸ copy_fd;

        f = take_fdopen(&copy_fd, ਬਹੁਲਕ);
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        *ret = TAKE_PTR(f);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਖੋਜ_and_open_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ ਬਹੁਲਕ,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ **ਖੋਜ,
                ਪੂਰਨ *ret_fd,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!ret_fd || !FLAGS_ਸੈੱਟ(ਬਹੁਲਕ, O_CREAT)); /* We don't support O_CREAT for this */
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        ਜੇ (ਰਸਤਾ_is_absolute(ਰਸਤਾ)) {
                _cleanup_close_ ਪੂਰਨ fd = -EBADF;

                ਜੇ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(ਰਸਤਾ, ਬਹੁਲਕ, 0777));
                ਨਹੀਂਤਾਂ
                        r = RET_NERRNO(access(ਰਸਤਾ, ਬਹੁਲਕ));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (ret_ਰਸਤਾ) {
                        r = ਰਸਤਾ_simplify_alloc(ਰਸਤਾ, ret_ਰਸਤਾ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ਵਾਪਸ 0;
        }

        ਜੇ (!ਰਸਤਾ_strv_resolve_uniq(ਖੋਜ, ਜੜ੍ਹ))
                ਵਾਪਸ -ENOMEM;

        STRV_FOREACH(i, ਖੋਜ) {
                _cleanup_close_ ਪੂਰਨ fd = -EBADF;
                _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                p = ਰਸਤਾ_join(ਜੜ੍ਹ, *i, ਰਸਤਾ);
                ਜੇ (!p)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, ਬਹੁਲਕ, 0777));
                ਨਹੀਂਤਾਂ
                        r = RET_NERRNO(access(p, F_OK));
                ਜੇ (r >= 0) {
                        ਜੇ (ret_ਰਸਤਾ)
                                *ret_ਰਸਤਾ = ਰਸਤਾ_simplify(TAKE_PTR(p));

                        ਜੇ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ਵਾਪਸ 0;
                }
                ਜੇ (r != -ENOENT)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ -ENOENT;
}

ਪੂਰਨ ਖੋਜ_and_open(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ ਬਹੁਲਕ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ **ਖੋਜ,
                ਪੂਰਨ *ret_fd,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_strv_free_ ਅੱਖਰ **copy = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        copy = strv_copy(ਖੋਜ);
        ਜੇ (!copy)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ ਖੋਜ_and_open_ਅੰਦਰੂਨੀ(ਰਸਤਾ, ਬਹੁਲਕ, ਜੜ੍ਹ, copy, ret_fd, ret_ਰਸਤਾ);
}

ਸਥਿਰ ਪੂਰਨ ਖੋਜ_and_fopen_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ **ਖੋਜ,
                FILE **ret_file,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_free_ ਅੱਖਰ *ਮਿਲਿਆ_ਰਸਤਾ = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ || !ret_file);

        r = ਖੋਜ_and_open_ਅੰਦਰੂਨੀ(
                        ਰਸਤਾ,
                        ਬਹੁਲਕ ? fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(ਬਹੁਲਕ) : 0,
                        ਜੜ੍ਹ,
                        ਖੋਜ,
                        ret_file ? &fd : ਨਿਰਾ,
                        ret_ਰਸਤਾ ? &ਮਿਲਿਆ_ਰਸਤਾ : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_file) {
                FILE *f = take_fdopen(&fd, ਬਹੁਲਕ);
                ਜੇ (!f)
                        ਵਾਪਸ -errno;

                *ret_file = f;
        }

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(ਮਿਲਿਆ_ਰਸਤਾ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੋਜ_and_fopen(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅਚਲ ਅੱਖਰ **ਖੋਜ,
                FILE **ret_file,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_strv_free_ ਅੱਖਰ **copy = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ || !ret_file);

        copy = strv_copy((ਅੱਖਰ**) ਖੋਜ);
        ਜੇ (!copy)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ ਖੋਜ_and_fopen_ਅੰਦਰੂਨੀ(ਰਸਤਾ, ਬਹੁਲਕ, ਜੜ੍ਹ, copy, ret_file, ret_ਰਸਤਾ);
}

ਪੂਰਨ ਖੋਜ_and_fopen_nulstr(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅਚਲ ਅੱਖਰ *ਖੋਜ,
                FILE **ret_file,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ || !ret_file);

        l = strv_ਵੰਡੋ_nulstr(ਖੋਜ);
        ਜੇ (!l)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ ਖੋਜ_and_fopen_ਅੰਦਰੂਨੀ(ਰਸਤਾ, ਬਹੁਲਕ, ਜੜ੍ਹ, l, ret_file, ret_ਰਸਤਾ);
}

ਪੂਰਨ fflush_and_check(FILE *f) {
        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        errno = 0;
        fflush(f);

        ਜੇ (ferror(f))
                ਵਾਪਸ errno_or_else(EIO);

        ਵਾਪਸ 0;
}

ਪੂਰਨ fflush_sync_and_check(FILE *f) {
        ਪੂਰਨ r, fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        r = fflush_and_check(f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ਜੇ (fd < 0)
                ਵਾਪਸ 0;

        r = fsync_ਭਰਿਆ(fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 0;
}

ਪੂਰਨ write_timestamp_file_atomic(ਅਚਲ ਅੱਖਰ *fn, usec_t n) {
        ਅੱਖਰ ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ਜੇ (!timestamp_is_ਸੈੱਟ(n))
                ਵਾਪਸ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ਵਾਪਸ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

ਪੂਰਨ read_timestamp_file(ਅਚਲ ਅੱਖਰ *fn, usec_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ln = ਨਿਰਾ;
        uint64_t t;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = read_one_ਸਤਰ_file(fn, &ln);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਸੁਰੱਖਿਅਤ_atou64(ln, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!timestamp_is_ਸੈੱਟ(t))
                ਵਾਪਸ -ERANGE;

        *ret = (usec_t) t;
        ਵਾਪਸ 0;
}

ਪੂਰਨ fputs_with_separator(FILE *f, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separator, bool *space) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ਜੇ (!f)
                f = stdout;

        ਜੇ (!separator)
                separator = " ";

        ਜੇ (*space)
                ਜੇ (fputs(separator, f) < 0)
                        ਵਾਪਸ -EIO;

        *space = true;

        ਜੇ (fputs(s, f) < 0)
                ਵਾਪਸ -EIO;

        ਵਾਪਸ 0;
}

ਪੂਰਨ fputs_with_newline(FILE *f, ਅਚਲ ਅੱਖਰ *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ਜੇ (isempty(s))
                ਵਾਪਸ 0;

        ਜੇ (!f)
                f = stdout;

        ਜੇ (fputs(s, f) < 0)
                ਵਾਪਸ -EIO;

        ਜੇ (endswith(s, "\n"))
                ਵਾਪਸ 0;

        ਜੇ (fputc('\n', f) < 0)
                ਵਾਪਸ -EIO;

        ਵਾਪਸ 1;
}

/* A bitmask of the EOL markers we know */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਸਿਰਾ_ਦਾ_ਸਤਰ_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} ਸਿਰਾ_ਦਾ_ਸਤਰ_Marker;

ਸਥਿਰ ਸਿਰਾ_ਦਾ_ਸਤਰ_Marker categorize_eol(ਅੱਖਰ c, Read_ਸਤਰ_Flags flags) {

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, READ_ਸਤਰ_ONLY_NUL)) {
                ਜੇ (c == '\n')
                        ਵਾਪਸ EOL_TEN;
                ਜੇ (c == '\r')
                        ਵਾਪਸ EOL_THIRTEEN;
        }

        ਜੇ (c == '\0')
                ਵਾਪਸ EOL_ZERO;

        ਵਾਪਸ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_ਭਰਿਆ(FILE*, funlockfile, ਨਿਰਾ);

ਪੂਰਨ read_ਸਤਰ_ਭਰਿਆ(FILE *f, size_t limit, Read_ਸਤਰ_Flags flags, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ਬਫਰ = ਨਿਰਾ;
        size_t n = 0, ਗਿਣਤੀ_2 = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

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

        ਜੇ (ret) {
                ਜੇ (!GREEDY_REALLOC(ਬਫਰ, 1))
                        ਵਾਪਸ -ENOMEM;
        }

        {
                ਅਣਵਰਤਿਆ _cleanup_(funlockfilep) FILE *flocked = f;
                ਸਿਰਾ_ਦਾ_ਸਤਰ_Marker ਪਿਛਲਾ_eol = EOL_NONE;
                flockfile(f);

                ਲਈ (;;) {
                        ਸਿਰਾ_ਦਾ_ਸਤਰ_Marker eol;
                        ਅੱਖਰ c;

                        ਜੇ (n >= limit)
                                ਵਾਪਸ -ENOBUFS;

                        ਜੇ (ਗਿਣਤੀ_2 >= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ਵਾਪਸ -ENOBUFS;

                        r = ਸੁਰੱਖਿਅਤ_fgetc(f, &c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0) /* EOF is definitely EOL */
                                ਰੋਕੋ;

                        eol = categorize_eol(c, flags);

                        ਜੇ (FLAGS_ਸੈੱਟ(ਪਿਛਲਾ_eol, EOL_ZERO) ||
                            (eol == EOL_NONE && ਪਿਛਲਾ_eol != EOL_NONE) ||
                            (eol != EOL_NONE && (ਪਿਛਲਾ_eol & eol) != 0)) {
                                /* Previous char was a NUL? This is not an EOL, but the previous char was? This type of
                                 * EOL marker has been seen right before?  In either of these three cases we are
                                 * done. But first, let's put this character back in the queue. (Note that we have to
                                 * cast this to (unsigned char) here as ungetc() expects a positive 'int', and if we
                                 * are on an architecture where 'char' equals 'signed char' we need to ensure we don't
                                 * pass a negative value here. That said, to complicate things further ungetc() is
                                 * actually happy with most negative characters and implicitly casts them back to
                                 * positive ones as needed, except for \xff (aka -1, aka EOF), which it refuses. What a
                                 * godawful API!) */
                                assert_se(ungetc((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c, f) != EOF);
                                ਰੋਕੋ;
                        }

                        ਗਿਣਤੀ_2++;

                        ਜੇ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ਜੇ ((flags & (READ_ਸਤਰ_IS_A_TTY|READ_ਸਤਰ_NOT_A_TTY)) == 0) {
                                        ਪੂਰਨ fd;

                                        fd = fileno(f);
                                        ਜੇ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_ਸਤਰ_NOT_A_TTY;
                                        ਨਹੀਂਤਾਂ
                                                flags |= isatty_ਸੁਰੱਖਿਅਤ(fd) ? READ_ਸਤਰ_IS_A_TTY : READ_ਸਤਰ_NOT_A_TTY;
                                }
                                ਜੇ (FLAGS_ਸੈੱਟ(flags, READ_ਸਤਰ_IS_A_TTY))
                                        ਰੋਕੋ;
                        }

                        ਜੇ (eol != EOL_NONE) {
                                ਪਿਛਲਾ_eol |= eol;
                                ਜਾਰੀ;
                        }

                        ਜੇ (ret) {
                                ਜੇ (!GREEDY_REALLOC(ਬਫਰ, n + 2))
                                        ਵਾਪਸ -ENOMEM;

                                ਬਫਰ[n] = c;
                        }

                        n++;
                }
        }

        ਜੇ (ret) {
                ਬਫਰ[n] = 0;

                *ret = TAKE_PTR(ਬਫਰ);
        }

        ਵਾਪਸ (ਪੂਰਨ) ਗਿਣਤੀ_2;
}

ਪੂਰਨ read_stripped_ਸਤਰ(FILE *f, size_t limit, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r, k;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        r = read_ਸਤਰ(f, limit, ret ? &s : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret) {
                ਅਚਲ ਅੱਖਰ *p = strstrip(s);
                ਜੇ (p == s)
                        *ret = TAKE_PTR(s);
                ਨਹੀਂਤਾਂ {
                        k = strdup_ਮੰਜ਼ਿਲ(ret, p);
                        ਜੇ (k < 0)
                                ਵਾਪਸ k;
                }
        }

        ਵਾਪਸ r > 0;          /* Return 1 if something was read. */
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_fgetc(FILE *f, ਅੱਖਰ *ret) {
        ਪੂਰਨ k;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ਜੇ (k == EOF) {
                ਜੇ (ferror(f))
                        ਵਾਪਸ errno_or_else(EIO);

                ਜੇ (ret)
                        *ret = 0;

                ਵਾਪਸ 0;
        }

        ਜੇ (ret)
                *ret = k;

        ਵਾਪਸ 1;
}

ਪੂਰਨ warn_file_is_world_accessible(ਅਚਲ ਅੱਖਰ *filename, ਬਣਤਰ stat *st, ਅਚਲ ਅੱਖਰ *unit, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ) {
        ਬਣਤਰ stat _st;

        ਜੇ (!filename)
                ਵਾਪਸ 0;

        ਜੇ (!st) {
                ਜੇ (stat(filename, &_st) < 0)
                        ਵਾਪਸ -errno;
                st = &_st;
        }

        ਜੇ ((st->st_mode & S_IRWXO) == 0)
                ਵਾਪਸ 0;

        ਜੇ (unit)
                log_syntax(unit, LOG_WARNING, filename, ਸਤਰ, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ਨਹੀਂਤਾਂ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ਵਾਪਸ 0;
}

ਪੂਰਨ write_ਅੰਕੜੇ_file_atomic_at(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਬਣਤਰ iovec *iovec,
                Write_ਅੰਕੜੇ_File_Flags flags) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ ਅੱਖਰ *dn = ਨਿਰਾ, *fn = ਨਿਰਾ;
        r = ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(ਰਸਤਾ, &dn, &fn);
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ਵਾਪਸ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_close_ ਪੂਰਨ mfd = -EBADF;
        ਜੇ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_ਸੈੱਟ(flags, WRITE_ਅੰਕੜੇ_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ਨਿਰਾ,
                            &mfd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                dir_fd = mfd;
        }

        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ਜੇ (iovec_is_ਸੈੱਟ(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        r = fchmod_umask(fd, FLAGS_ਸੈੱਟ(flags, WRITE_ਅੰਕੜੇ_FILE_ਬਹੁਲਕ_0400) ? 0400 : 0644);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ਵਾਪਸ 0;
}
