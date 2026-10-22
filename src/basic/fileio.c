/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio_ext.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "label-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mkdir.h"
#ଅନ୍ତର୍ଭାଵ "nulstr-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "socket-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "sync-util.h"
#ଅନ୍ତର୍ଭାଵ "terminal-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "tmpfile-util.h"

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdopen_unlocked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *options, FILE **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        FILE *f = fdopen(fd, options);
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        (ଶୂନ୍ୟ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_fdopen_unlocked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd, ଅଚଳ ଅକ୍ଷର *options, FILE **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd);

        r = fdopen_unlocked(*fd, options, ret);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *fd = -EBADF;

        ଫେରନ୍ତୁ 0;
}

FILE* take_fdopen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd, ଅଚଳ ଅକ୍ଷର *options) {
        ପ୍ରତିଜ୍ଞା(fd);

        FILE *f = fdopen(*fd, options);
        ଯଦି (!f)
                ଫେରନ୍ତୁ ଖାଲି;

        *fd = -EBADF;

        ଫେରନ୍ତୁ f;
}

DIR* take_fdopendir(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dfd) {
        ପ୍ରତିଜ୍ଞା(dfd);

        DIR *d = fdopendir(*dfd);
        ଯଦି (!d)
                ଫେରନ୍ତୁ ଖାଲି;

        *dfd = -EBADF;

        ଫେରନ୍ତୁ d;
}

FILE* open_memstream_unlocked(ଅକ୍ଷର **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        ଯଦି (!f)
                ଫେରନ୍ତୁ ଖାଲି;

        (ଶୂନ୍ୟ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ଫେରନ୍ତୁ f;
}

FILE* fmemopen_unlocked(ଶୂନ୍ୟ *buf, size_t size, ଅଚଳ ଅକ୍ଷର *mode) {
        FILE *f = fmemopen(buf, size, mode);
        ଯଦି (!f)
                ଫେରନ୍ତୁ ଖାଲି;

        (ଶୂନ୍ୟ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        ଫେରନ୍ତୁ f;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_string_stream_full(
                FILE *f,
                ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି,
                WriteStringFileFlags flags,
                ଅଚଳ ସଂରଚନା timespec *ts) {

        bool needs_nl;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ଧାଡ଼ି);

        ଯଦି (ferror(f))
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -EBADF;
        }

        ଯଦି (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                ଯଦି (fd < 0) {
                        fd = fileno(f);
                        ଯଦି (fd < 0)
                                ଫେରନ୍ତୁ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                ଯଦି (read_virtual_file_fd(fd, strlen(ଧାଡ଼ି)+1, &t, ଖାଲି) > 0 &&
                    streq_skip_trailing_chars(ଧାଡ଼ି, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", ଧାଡ଼ି);
                        ଫେରନ୍ତୁ 0;
                }

                ଯଦି (lseek(fd, 0, SEEK_SET) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(ଧାଡ଼ି, "\n");

        ଯଦି (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                ଧାଡ଼ି = strjoina(ଧାଡ଼ି, "\n");
                needs_nl = false;
        }

        ଯଦି (fputs(ଧାଡ଼ି, f) == EOF)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (needs_nl)
                ଯଦି (fputc('\n', f) == EOF)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        ଅନ୍ୟଥା
                r = fflush_and_check(f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ts) {
                ଅଚଳ ସଂରଚନା timespec twice[2] = {*ts, *ts};

                ପ୍ରତିଜ୍ଞା(fd >= 0);
                ଯଦି (futimens(fd, twice) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        ଫେରନ୍ତୁ FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_string_file_atomic_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *fn,
                ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି,
                WriteStringFileFlags flags,
                ଅଚଳ ସଂରଚନା timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fn);
        ପ୍ରତିଜ୍ଞା(ଧାଡ଼ି);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        ଯଦି (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ଖାଲି : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        ଯଦି (k < 0)
                ଫେରନ୍ତୁ k;

        r = write_string_stream_full(f, ଧାଡ଼ି, flags, ts);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = fchmod_umask(fileno(f), mode);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ଯଦି (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_string_file_full_label(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *fn,
                ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି,
                WriteStringFileFlags flags,
                ଅଚଳ ସଂରଚନା timespec *ts,
                ଅଚଳ ଅକ୍ଷର *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd == AT_FDCWD || dir_fd >= 0);
        ପ୍ରତିଜ୍ଞା(ଧାଡ଼ି);

        /* We don't know how to verify whether the file contents was already on-disk. */
        ପ୍ରତିଜ୍ଞା(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        ଯଦି (flags & WRITE_STRING_FILE_MKDIR_0755) {
                ପ୍ରତିଜ୍ଞା(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (flags & WRITE_STRING_FILE_ATOMIC) {
                ପ୍ରତିଜ୍ଞା(fn);
                ପ୍ରତିଜ୍ଞା(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, ଧାଡ଼ି, flags, ts, label_context);
                ଯଦି (r < 0)
                        ଯାଅ fail;

                ଫେରନ୍ତୁ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        ଯଦି (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        ଅନ୍ୟଥା {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                ଯଦି (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        ଯଦି (r < 0)
                                ଯାଅ fail;

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
                ଯଦି (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ଖାଲି : fn, made_file, label_context));
        }
        ଯଦି (r < 0)
                ଯାଅ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        ଯଦି (r < 0)
                ଯାଅ fail;

        ଯଦି (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, ଖାଲି, _IONBF, 0);

        r = write_string_stream_full(f, ଧାଡ଼ି, flags, ts);
        ଯଦି (r < 0)
                ଯାଅ fail;

        ଫେରନ୍ତୁ 0;

fail:
        ଯଦି (made_file)
                (ଶୂନ୍ୟ) unlinkat(dir_fd, fn, 0);

        ଯଦି (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                ଫେରନ୍ତୁ r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        ଯଦି (verify_file_at(dir_fd, fn, ଧାଡ଼ି, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_string_filef(
                ଅଚଳ ଅକ୍ଷର *fn,
                WriteStringFileFlags flags,
                ଅଚଳ ଅକ୍ଷର *format, ...) {

        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ write_string_file(fn, p, flags);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_string_filef_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *fn,
                WriteStringFileFlags flags,
                ଅଚଳ ଅକ୍ଷର *format, ...) {

        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ write_string_file_at(dir_fd, fn, p, flags);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_base64_file_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *fn,
                ଅଚଳ ସଂରଚନା iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ ଅକ୍ଷର *encoded = ଖାଲି;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : ଖାଲି, data ? data->iov_len : 0, 79, &encoded);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ n;

        ଫେରନ୍ତୁ write_string_file_at(dir_fd, fn, encoded, flags);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_one_ଧାଡ଼ି_file_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *filename, ଅକ୍ଷର **ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(filename);
        ପ୍ରତିଜ୍ଞା(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_boolean_file_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *filename) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(filename);

        r = read_one_ଧାଡ଼ି_file_at(dir_fd, filename, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_boolean(s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_file_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *fn, ଅଚଳ ଅକ୍ଷର *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        size_t l, k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(blob);

        l = strlen(blob);

        ଯଦି (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        ଯଦି (ferror(f))
                ଫେରନ୍ତୁ errno_or_else(EIO);

        ଯଦି (k != l && k != l + accept_extra_nl)
                ଫେରନ୍ତୁ 0;
        ଯଦି (memcmp(buf, blob, l) != 0)
                ଫେରନ୍ତୁ 0;
        ଯଦି (k > l && buf[l] != '\n')
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_virtual_file_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *filename,
                size_t max_size,
                ଅକ୍ଷର **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        size_t n, size;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_retries;
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

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ଯଦି (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        ଅନ୍ୟଥା
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        ପାଇଁ (;;) {
                ସଂରଚନା stat st;

                ଯଦି (fstat(fd, &st) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (!S_ISREG(st.st_mode))
                        ଫେରନ୍ତୁ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                ଯଦି (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        ଯଦି (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                ଯଦି (max_size == SIZE_MAX)
                                        ଫେରନ୍ତୁ -EFBIG;

                                size = max_size;
                        } ଅନ୍ୟଥା {
                                size = MIN((size_t) st.st_size, max_size);

                                ଯଦି (size > READ_VIRTUAL_BYTES_MAX)
                                        ଫେରନ୍ତୁ -EFBIG;
                        }

                        n_retries--;
                } ଅନ୍ୟଥା ଯଦି (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } ଅନ୍ୟଥା {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                ପାଇଁ (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = ପଠ(fd, buf, size + 1);
                        ଯଦି (k >= 0) {
                                n = k;
                                ଭାଙ୍ଗ;
                        }

                        ଯଦି (errno != EINTR)
                                ଫେରନ୍ତୁ -errno;
                }

                /* Consider a short read as EOF */
                ଯଦି (n <= size)
                        ଭାଙ୍ଗ;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                ଯଦି (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        ଭାଙ୍ଗ;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                ଯଦି (n_retries <= 0)
                        ଫେରନ୍ତୁ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                ଯଦି (lseek(fd, 0, SEEK_SET) < 0)
                        ଫେରନ୍ତୁ -errno;

                buf = mfree(buf);
        }

        ଯଦି (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                ଯଦି (!ret_size && memchr(buf, 0, n))
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (n < size) {
                        ଅକ୍ଷର *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        ଯଦି (!p)
                                ଫେରନ୍ତୁ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        ଯଦି (ret_size)
                *ret_size = n;

        ଫେରନ୍ତୁ !truncated;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_full_stream_full(
                FILE *f,
                ଅଚଳ ଅକ୍ଷର *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ଅକ୍ଷର **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, r;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ret_contents);
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        ପ୍ରତିଜ୍ଞା(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        ଯଦି (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                ଯଦି (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                ଅନ୍ୟଥା
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        ଯଦି (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ସଂରଚନା stat st;

                ଯଦି (fstat(fd, &st) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        ଯଦି (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                ଯଦି (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        ଯଦି (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (ଶୂନ୍ୟ) warn_file_is_world_accessible(filename, &st, ଖାଲି, 0);
                }
        } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                ଫେରନ୍ତୁ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        ଯଦି (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        ଯଦି (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        ଯଦି (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                ଫେରନ୍ତୁ -errno;

        n = l = 0;
        ପାଇଁ (;;) {
                ଅକ୍ଷର *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                ଯଦି (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                ଯଦି (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        ଯଦି (!t) {
                                r = -ENOMEM;
                                ଯାଅ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        ମୁକ୍ତ(buf);
                } ଅନ୍ୟଥା {
                        t = realloc(buf, n_next + 1);
                        ଯଦି (!t)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                ପ୍ରତିଜ୍ଞା(k <= n - l);
                l += k;

                ଯଦି (ferror(f)) {
                        r = errno_or_else(EIO);
                        ଯାଅ finalize;
                }
                ଯଦି (feof(f))
                        ଭାଙ୍ଗ;

                ଯଦି (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        ପ୍ରତିଜ୍ଞା(l == size);
                        ଭାଙ୍ଗ;
                }

                ପ୍ରତିଜ୍ଞା(k > 0); /* we can't have read zero bytes because that would have been EOF */

                ଯଦି (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        ଯାଅ finalize;
                }

                ଯଦି (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        ଯାଅ finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        ଯଦି (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ ଶୂନ୍ୟ *decoded = ଖାଲି;
                size_t decoded_size;

                buf[l++] = 0;
                ଯଦି (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ଅନ୍ୟଥା
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                ଯଦି (r < 0)
                        ଯାଅ finalize;

                ଯଦି (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                ଯଦି (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        ଯାଅ finalize;
                }
        }

        ଯଦି (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                ଯଦି (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        ଯାଅ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        ଯଦି (ret_size)
                *ret_size = l;

        ଫେରନ୍ତୁ 0;

finalize:
        ଯଦି (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_full_file_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ଅଚଳ ଅକ୍ଷର *bind_name,
                ଅକ୍ଷର **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = ଖାଲି;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_contents);

        ଯଦି (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା script_get_shebang_interpreter(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        f = fopen(path, "re");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        ଅକ୍ଷର c;
        r = safe_fgetc(f, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EBADMSG;
        ଯଦି (c != '#')
                ଫେରନ୍ତୁ -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EBADMSG;
        ଯଦି (c != '!')
                ଫେରନ୍ତୁ -EMEDIUMTYPE;

        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *s = ଧାଡ଼ି;

        r = extract_first_word(&s, &p, /* separators= */ ଖାଲି, /* flags= */ 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -ENOEXEC;

        ଯଦି (ret)
                *ret = TAKE_PTR(p);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_proc_field(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର **ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ଚାବି);

        r = fopen_unlocked(path, "re", &f);
        ଯଦି (r == -ENOENT && proc_mounted() == 0)
                ଫେରନ୍ତୁ -ENOSYS;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                 _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;

                 r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                 ଯଦି (r < 0)
                         ଫେରନ୍ତୁ r;
                 ଯଦି (r == 0)
                         ଫେରନ୍ତୁ -ENODATA;

                 ଅକ୍ଷର *l = startswith(ଧାଡ଼ି, ଚାବି);
                 ଯଦି (l && *l == ':') {
                         ଯଦି (ret) {
                                 ଅକ୍ଷର *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 ଯଦି (!s)
                                         ଫେରନ୍ତୁ -ENOMEM;

                                 *ret = s;
                         }

                         ଫେରନ୍ତୁ 0;
                 }
        }
}

DIR* xopendirat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(!(flags & (O_CREAT|O_TMPFILE)));

        ଯଦି ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                ଫେରନ୍ତୁ opendir(path);

        ଯଦି (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ take_fdopendir(&fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_mode_to_flags(ଅଚଳ ଅକ୍ଷର *mode) {
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;

        ପ୍ରତିଜ୍ଞା(mode);

        ଯଦି ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        ଅନ୍ୟଥା ଯଦି ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        ଅନ୍ୟଥା ଯଦି ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        ଅନ୍ୟଥା ଯଦି ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        ଅନ୍ୟଥା ଯଦି ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        ଅନ୍ୟଥା ଯଦି ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -EINVAL;

        ପାଇଁ (; *p != 0; p++) {

                ବାଛ (*p) {

                ସ୍ଥିତି 'e':
                        flags |= O_CLOEXEC;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି 'x':
                        flags |= O_EXCL;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି 'm':
                        /* ignore this here, fdopen() might care later though */
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି 'c': /* not sure what to do about this one */
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        ଫେରନ୍ତୁ -EINVAL;
                }
        }

        ଫେରନ୍ତୁ flags;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xfopenat_regular(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(mode);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        ଅନ୍ୟଥା ଯଦି (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ ଅକ୍ଷର *j = strjoin("/", path);
                ଯଦି (!j)
                        ଫେରନ୍ତୁ -ENOMEM;

                f = fopen(j, mode);
        } ଅନ୍ୟଥା {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                ଯଦି (mode_flags < 0)
                        ଫେରନ୍ତୁ mode_flags;

                ଯଦି (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        ଯଦି (fd < 0)
                                ଫେରନ୍ତୁ -errno;
                } ଅନ୍ୟଥା {
                        ଯଦି (dir_fd == AT_FDCWD)
                                ଫେରନ୍ତୁ -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        ଯଦି (fd < 0)
                                ଫେରନ୍ତୁ fd;
                }

                f = take_fdopen(&fd, mode);
        }
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        *ret = f;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xfopenat_unix_socket(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *bind_name, FILE **ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା sk = -EBADF;
        FILE *f;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        ଯଦି (sk < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                ସଂଘ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (bind(sk, &bsa.sa, r) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (shutdown(sk, SHUT_WR) < 0)
                ଫେରନ୍ତୁ -errno;

        f = take_fdopen(&sk, "r");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        *ret = f;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xfopenat_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *mode,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags,
                XfopenFlags flags,
                ଅଚଳ ଅକ୍ଷର *bind_name,
                FILE **ret) {

        FILE *f = ଖାଲି;  /* avoid false maybe-uninitialized warning */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(mode);
        ପ୍ରତିଜ୍ଞା(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        ଯଦି (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                ଯଦି (IN_SET(r, -ENOTSOCK, -EINVAL))
                        ଫେରନ୍ତୁ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (ଶୂନ୍ୟ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdopen_independent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *mode, FILE **ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_flags;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(mode);
        ପ୍ରତିଜ୍ଞା(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        ଯଦି (mode_flags < 0)
                ଫେରନ୍ତୁ mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        ଯଦି (copy_fd < 0)
                ଫେରନ୍ତୁ copy_fd;

        f = take_fdopen(&copy_fd, mode);
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        *ret = TAKE_PTR(f);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଧାନ_and_open_internal(
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର **ସନ୍ଧାନ,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd,
                ଅକ୍ଷର **ret_path) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        ପ୍ରତିଜ୍ଞା(path);

        ଯଦି (path_is_absolute(path)) {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

                ଯଦି (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                ଅନ୍ୟଥା
                        r = RET_NERRNO(access(path, mode));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (!path_strv_resolve_uniq(ସନ୍ଧାନ, ମୂଳ))
                ଫେରନ୍ତୁ -ENOMEM;

        STRV_FOREACH(i, ସନ୍ଧାନ) {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
                _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                p = path_join(ମୂଳ, *i, path);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                ଅନ୍ୟଥା
                        r = RET_NERRNO(access(p, F_OK));
                ଯଦି (r >= 0) {
                        ଯଦି (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        ଯଦି (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (r != -ENOENT)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ -ENOENT;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଧାନ_and_open(
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର **ସନ୍ଧାନ,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd,
                ଅକ୍ଷର **ret_path) {

        _cleanup_strv_free_ ଅକ୍ଷର **copy = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(path);

        copy = strv_copy(ସନ୍ଧାନ);
        ଯଦି (!copy)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ ସନ୍ଧାନ_and_open_internal(path, mode, ମୂଳ, copy, ret_fd, ret_path);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଧାନ_and_fopen_internal(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *mode,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର **ସନ୍ଧାନ,
                FILE **ret_file,
                ଅକ୍ଷର **ret_path) {

        _cleanup_free_ ଅକ୍ଷର *found_path = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(mode || !ret_file);

        r = ସନ୍ଧାନ_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        ମୂଳ,
                        ସନ୍ଧାନ,
                        ret_file ? &fd : ଖାଲି,
                        ret_path ? &found_path : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                ଯଦି (!f)
                        ଫେରନ୍ତୁ -errno;

                *ret_file = f;
        }

        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(found_path);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଧାନ_and_fopen(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *mode,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅଚଳ ଅକ୍ଷର **ସନ୍ଧାନ,
                FILE **ret_file,
                ଅକ୍ଷର **ret_path) {

        _cleanup_strv_free_ ଅକ୍ଷର **copy = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(mode || !ret_file);

        copy = strv_copy((ଅକ୍ଷର**) ସନ୍ଧାନ);
        ଯଦି (!copy)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ ସନ୍ଧାନ_and_fopen_internal(path, mode, ମୂଳ, copy, ret_file, ret_path);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଧାନ_and_fopen_nulstr(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *mode,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅଚଳ ଅକ୍ଷର *ସନ୍ଧାନ,
                FILE **ret_file,
                ଅକ୍ଷର **ret_path) {

        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(mode || !ret_file);

        l = strv_split_nulstr(ସନ୍ଧାନ);
        ଯଦି (!l)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ ସନ୍ଧାନ_and_fopen_internal(path, mode, ମୂଳ, l, ret_file, ret_path);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fflush_and_check(FILE *f) {
        ପ୍ରତିଜ୍ଞା(f);

        errno = 0;
        fflush(f);

        ଯଦି (ferror(f))
                ଫେରନ୍ତୁ errno_or_else(EIO);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fflush_sync_and_check(FILE *f) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, fd;

        ପ୍ରତିଜ୍ଞା(f);

        r = fflush_and_check(f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ 0;

        r = fsync_full(fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_timestamp_file_atomic(ଅଚଳ ଅକ୍ଷର *fn, usec_t n) {
        ଅକ୍ଷର ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        ଯଦି (!timestamp_is_set(n))
                ଫେରନ୍ତୁ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        ଫେରନ୍ତୁ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_timestamp_file(ଅଚଳ ଅକ୍ଷର *fn, usec_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ln = ଖାଲି;
        uint64_t t;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = read_one_ଧାଡ଼ି_file(fn, &ln);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = safe_atou64(ln, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!timestamp_is_set(t))
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (usec_t) t;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fputs_with_separator(FILE *f, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separator, bool *space) {
        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        ଯଦି (!f)
                f = stdout;

        ଯଦି (!separator)
                separator = " ";

        ଯଦି (*space)
                ଯଦି (fputs(separator, f) < 0)
                        ଫେରନ୍ତୁ -EIO;

        *space = true;

        ଯଦି (fputs(s, f) < 0)
                ଫେରନ୍ତୁ -EIO;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fputs_with_newline(FILE *f, ଅଚଳ ଅକ୍ଷର *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        ଯଦି (isempty(s))
                ଫେରନ୍ତୁ 0;

        ଯଦି (!f)
                f = stdout;

        ଯଦି (fputs(s, f) < 0)
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (endswith(s, "\n"))
                ଫେରନ୍ତୁ 0;

        ଯଦି (fputc('\n', f) < 0)
                ଫେରନ୍ତୁ -EIO;

        ଫେରନ୍ତୁ 1;
}

/* A bitmask of the EOL markers we know */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା End_Of_ଧାଡ଼ି_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_ଧାଡ଼ି_Marker;

ସ୍ଥିର End_Of_ଧାଡ଼ି_Marker categorize_eol(ଅକ୍ଷର c, Read_ଧାଡ଼ି_Flags flags) {

        ଯଦି (!FLAGS_SET(flags, READ_ଧାଡ଼ି_ONLY_NUL)) {
                ଯଦି (c == '\n')
                        ଫେରନ୍ତୁ EOL_TEN;
                ଯଦି (c == '\r')
                        ଫେରନ୍ତୁ EOL_THIRTEEN;
        }

        ଯଦି (c == '\0')
                ଫେରନ୍ତୁ EOL_ZERO;

        ଫେରନ୍ତୁ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, ଖାଲି);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_ଧାଡ଼ି_full(FILE *f, size_t limit, Read_ଧାଡ଼ି_Flags flags, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *buffer = ଖାଲି;
        size_t n = 0, ଗଣନା_2 = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f);

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

        ଯଦି (ret) {
                ଯଦି (!GREEDY_REALLOC(buffer, 1))
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_ଧାଡ଼ି_Marker previous_eol = EOL_NONE;
                flockfile(f);

                ପାଇଁ (;;) {
                        End_Of_ଧାଡ଼ି_Marker eol;
                        ଅକ୍ଷର c;

                        ଯଦି (n >= limit)
                                ଫେରନ୍ତୁ -ENOBUFS;

                        ଯଦି (ଗଣନା_2 >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                ଫେରନ୍ତୁ -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0) /* EOF is definitely EOL */
                                ଭାଙ୍ଗ;

                        eol = categorize_eol(c, flags);

                        ଯଦି (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned ଅକ୍ଷର) c, f) != EOF);
                                ଭାଙ୍ଗ;
                        }

                        ଗଣନା_2++;

                        ଯଦି (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                ଯଦି ((flags & (READ_ଧାଡ଼ି_IS_A_TTY|READ_ଧାଡ଼ି_NOT_A_TTY)) == 0) {
                                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

                                        fd = fileno(f);
                                        ଯଦି (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_ଧାଡ଼ି_NOT_A_TTY;
                                        ଅନ୍ୟଥା
                                                flags |= isatty_safe(fd) ? READ_ଧାଡ଼ି_IS_A_TTY : READ_ଧାଡ଼ି_NOT_A_TTY;
                                }
                                ଯଦି (FLAGS_SET(flags, READ_ଧାଡ଼ି_IS_A_TTY))
                                        ଭାଙ୍ଗ;
                        }

                        ଯଦି (eol != EOL_NONE) {
                                previous_eol |= eol;
                                ଜାରି;
                        }

                        ଯଦି (ret) {
                                ଯଦି (!GREEDY_REALLOC(buffer, n + 2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        ଯଦି (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଗଣନା_2;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_stripped_ଧାଡ଼ି(FILE *f, size_t limit, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, k;

        ପ୍ରତିଜ୍ଞା(f);

        r = read_ଧାଡ଼ି(f, limit, ret ? &s : ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret) {
                ଅଚଳ ଅକ୍ଷର *p = strstrip(s);
                ଯଦି (p == s)
                        *ret = TAKE_PTR(s);
                ଅନ୍ୟଥା {
                        k = strdup_to(ret, p);
                        ଯଦି (k < 0)
                                ଫେରନ୍ତୁ k;
                }
        }

        ଫେରନ୍ତୁ r > 0;          /* Return 1 if something was read. */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_fgetc(FILE *f, ଅକ୍ଷର *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;

        ପ୍ରତିଜ୍ଞା(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        ଯଦି (k == EOF) {
                ଯଦି (ferror(f))
                        ଫେରନ୍ତୁ errno_or_else(EIO);

                ଯଦି (ret)
                        *ret = 0;

                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (ret)
                *ret = k;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା warn_file_is_world_accessible(ଅଚଳ ଅକ୍ଷର *filename, ସଂରଚନା stat *st, ଅଚଳ ଅକ୍ଷର *unit, unsigned ଧାଡ଼ି) {
        ସଂରଚନା stat _st;

        ଯଦି (!filename)
                ଫେରନ୍ତୁ 0;

        ଯଦି (!st) {
                ଯଦି (stat(filename, &_st) < 0)
                        ଫେରନ୍ତୁ -errno;
                st = &_st;
        }

        ଯଦି ((st->st_mode & S_IRWXO) == 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (unit)
                log_syntax(unit, LOG_WARNING, filename, ଧାଡ଼ି, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        ଅନ୍ୟଥା
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_data_file_atomic_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ସଂରଚନା iovec *iovec,
                WriteDataFileFlags flags) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ ଅକ୍ଷର *dn = ଖାଲି, *fn = ଖାଲି;
        r = path_split_prefix_filename(path, &dn, &fn);
        ଯଦି (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                ଫେରନ୍ତୁ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mfd = -EBADF;
        ଯଦି (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ଖାଲି,
                            &mfd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                dir_fd = mfd;
        }

        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        ଯଦି (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        ଫେରନ୍ତୁ 0;
}
