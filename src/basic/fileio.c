/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio_ext.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "label-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mkdir.h"
#अन्तर्भाव "nulstr-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "socket-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "sync-util.h"
#अन्तर्भाव "terminal-util.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "tmpfile-util.h"

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

पूर्णाङ्क fdopen_unlocked(पूर्णाङ्क fd, ध्रुव वर्ण *options, FILE **ret) {
        प्रतिज्ञा(ret);

        FILE *f = fdopen(fd, options);
        जइ (!f)
                प्रत्यागम -errno;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क take_fdopen_unlocked(पूर्णाङ्क *fd, ध्रुव वर्ण *options, FILE **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd);

        r = fdopen_unlocked(*fd, options, ret);
        जइ (r < 0)
                प्रत्यागम r;

        *fd = -EBADF;

        प्रत्यागम 0;
}

FILE* take_fdopen(पूर्णाङ्क *fd, ध्रुव वर्ण *options) {
        प्रतिज्ञा(fd);

        FILE *f = fdopen(*fd, options);
        जइ (!f)
                प्रत्यागम रिक्त;

        *fd = -EBADF;

        प्रत्यागम f;
}

DIR* take_fdopendir(पूर्णाङ्क *dfd) {
        प्रतिज्ञा(dfd);

        DIR *d = fdopendir(*dfd);
        जइ (!d)
                प्रत्यागम रिक्त;

        *dfd = -EBADF;

        प्रत्यागम d;
}

FILE* open_memstream_unlocked(वर्ण **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        जइ (!f)
                प्रत्यागम रिक्त;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        प्रत्यागम f;
}

FILE* fmemopen_unlocked(शून्य *buf, size_t size, ध्रुव वर्ण *mode) {
        FILE *f = fmemopen(buf, size, mode);
        जइ (!f)
                प्रत्यागम रिक्त;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        प्रत्यागम f;
}

पूर्णाङ्क write_string_stream_full(
                FILE *f,
                ध्रुव वर्ण *पङ्क्तिः,
                WriteStringFileFlags flags,
                ध्रुव संरचना timespec *ts) {

        bool needs_nl;
        पूर्णाङ्क r, fd = -EBADF;

        प्रतिज्ञा(f);
        प्रतिज्ञा(पङ्क्तिः);

        जइ (ferror(f))
                प्रत्यागम -EIO;

        जइ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                जइ (fd < 0)
                        प्रत्यागम -EBADF;
        }

        जइ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ वर्ण *t = रिक्त;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                जइ (fd < 0) {
                        fd = fileno(f);
                        जइ (fd < 0)
                                प्रत्यागम -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                जइ (read_virtual_file_fd(fd, strlen(पङ्क्तिः)+1, &t, रिक्त) > 0 &&
                    streq_skip_trailing_chars(पङ्क्तिः, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", पङ्क्तिः);
                        प्रत्यागम 0;
                }

                जइ (lseek(fd, 0, SEEK_SET) < 0)
                        प्रत्यागम -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(पङ्क्तिः, "\n");

        जइ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                पङ्क्तिः = strjoina(पङ्क्तिः, "\n");
                needs_nl = false;
        }

        जइ (fputs(पङ्क्तिः, f) == EOF)
                प्रत्यागम -errno;

        जइ (needs_nl)
                जइ (fputc('\n', f) == EOF)
                        प्रत्यागम -errno;

        जइ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        अण्णहा
                r = fflush_and_check(f);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (ts) {
                ध्रुव संरचना timespec twice[2] = {*ts, *ts};

                प्रतिज्ञा(fd >= 0);
                जइ (futimens(fd, twice) < 0)
                        प्रत्यागम -errno;
        }

        प्रत्यागम 0;
}

स्थिर mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        प्रत्यागम FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

स्थिर पूर्णाङ्क write_string_file_atomic_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *fn,
                ध्रुव वर्ण *पङ्क्तिः,
                WriteStringFileFlags flags,
                ध्रुव संरचना timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fn);
        प्रतिज्ञा(पङ्क्तिः);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        जइ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                जइ (r < 0)
                        प्रत्यागम r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        पूर्णाङ्क k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? रिक्त : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        जइ (r < 0)
                प्रत्यागम r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        जइ (k < 0)
                प्रत्यागम k;

        r = write_string_stream_full(f, पङ्क्तिः, flags, ts);
        जइ (r < 0)
                प्रत्यागम r;

        r = fchmod_umask(fileno(f), mode);
        जइ (r < 0)
                प्रत्यागम r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        जइ (r < 0)
                प्रत्यागम r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        जइ (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                जइ (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क write_string_file_full_label(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *fn,
                ध्रुव वर्ण *पङ्क्तिः,
                WriteStringFileFlags flags,
                ध्रुव संरचना timespec *ts,
                ध्रुव वर्ण *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd == AT_FDCWD || dir_fd >= 0);
        प्रतिज्ञा(पङ्क्तिः);

        /* We don't know how to verify whether the file contents was already on-disk. */
        प्रतिज्ञा(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        जइ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                प्रतिज्ञा(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        जइ (flags & WRITE_STRING_FILE_ATOMIC) {
                प्रतिज्ञा(fn);
                प्रतिज्ञा(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, पङ्क्तिः, flags, ts, label_context);
                जइ (r < 0)
                        गच्छ fail;

                प्रत्यागम r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        जइ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        अण्णहा {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                जइ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        जइ (r < 0)
                                गच्छ fail;

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
                जइ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? रिक्त : fn, made_file, label_context));
        }
        जइ (r < 0)
                गच्छ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        जइ (r < 0)
                गच्छ fail;

        जइ (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, रिक्त, _IONBF, 0);

        r = write_string_stream_full(f, पङ्क्तिः, flags, ts);
        जइ (r < 0)
                गच्छ fail;

        प्रत्यागम 0;

fail:
        जइ (made_file)
                (शून्य) unlinkat(dir_fd, fn, 0);

        जइ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                प्रत्यागम r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        जइ (verify_file_at(dir_fd, fn, पङ्क्तिः, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                प्रत्यागम 0;

        प्रत्यागम r;
}

पूर्णाङ्क write_string_filef(
                ध्रुव वर्ण *fn,
                WriteStringFileFlags flags,
                ध्रुव वर्ण *format, ...) {

        _cleanup_free_ वर्ण *p = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        जइ (r < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम write_string_file(fn, p, flags);
}

पूर्णाङ्क write_string_filef_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *fn,
                WriteStringFileFlags flags,
                ध्रुव वर्ण *format, ...) {

        _cleanup_free_ वर्ण *p = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        जइ (r < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम write_string_file_at(dir_fd, fn, p, flags);
}

पूर्णाङ्क write_base64_file_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *fn,
                ध्रुव संरचना iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ वर्ण *encoded = रिक्त;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : रिक्त, data ? data->iov_len : 0, 79, &encoded);
        जइ (n < 0)
                प्रत्यागम n;

        प्रत्यागम write_string_file_at(dir_fd, fn, encoded, flags);
}

पूर्णाङ्क read_one_पङ्क्तिः_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(filename);
        प्रतिज्ञा(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, ret);
}

पूर्णाङ्क read_boolean_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(filename);

        r = read_one_पङ्क्तिः_file_at(dir_fd, filename, &s);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम parse_boolean(s);
}

पूर्णाङ्क verify_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_free_ वर्ण *buf = रिक्त;
        size_t l, k;
        पूर्णाङ्क r;

        प्रतिज्ञा(blob);

        l = strlen(blob);

        जइ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        जइ (!buf)
                प्रत्यागम -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        जइ (r < 0)
                प्रत्यागम r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        जइ (ferror(f))
                प्रत्यागम errno_or_else(EIO);

        जइ (k != l && k != l + accept_extra_nl)
                प्रत्यागम 0;
        जइ (memcmp(buf, blob, l) != 0)
                प्रत्यागम 0;
        जइ (k > l && buf[l] != '\n')
                प्रत्यागम 0;

        प्रत्यागम 1;
}

पूर्णाङ्क read_virtual_file_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *filename,
                size_t max_size,
                वर्ण **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ वर्ण *buf = रिक्त;
        size_t n, size;
        पूर्णाङ्क n_retries;
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

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        जइ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        अण्णहा
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        जइ (fd < 0)
                प्रत्यागम fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        प्रति (;;) {
                संरचना stat st;

                जइ (fstat(fd, &st) < 0)
                        प्रत्यागम -errno;

                जइ (!S_ISREG(st.st_mode))
                        प्रत्यागम -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                जइ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        जइ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                जइ (max_size == SIZE_MAX)
                                        प्रत्यागम -EFBIG;

                                size = max_size;
                        } अण्णहा {
                                size = MIN((size_t) st.st_size, max_size);

                                जइ (size > READ_VIRTUAL_BYTES_MAX)
                                        प्रत्यागम -EFBIG;
                        }

                        n_retries--;
                } अण्णहा जइ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } अण्णहा {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                जइ (!buf)
                        प्रत्यागम -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                प्रति (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = पठ(fd, buf, size + 1);
                        जइ (k >= 0) {
                                n = k;
                                भङ्ग;
                        }

                        जइ (errno != EINTR)
                                प्रत्यागम -errno;
                }

                /* Consider a short read as EOF */
                जइ (n <= size)
                        भङ्ग;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                जइ (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        भङ्ग;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                जइ (n_retries <= 0)
                        प्रत्यागम -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                जइ (lseek(fd, 0, SEEK_SET) < 0)
                        प्रत्यागम -errno;

                buf = mfree(buf);
        }

        जइ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                जइ (!ret_size && memchr(buf, 0, n))
                        प्रत्यागम -EBADMSG;

                जइ (n < size) {
                        वर्ण *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        जइ (!p)
                                प्रत्यागम -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        जइ (ret_size)
                *ret_size = n;

        प्रत्यागम !truncated;
}

पूर्णाङ्क read_full_stream_full(
                FILE *f,
                ध्रुव वर्ण *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                वर्ण **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ वर्ण *buf = रिक्त;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        पूर्णाङ्क fd, r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret_contents);
        प्रतिज्ञा(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        प्रतिज्ञा(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        जइ (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                प्रत्यागम -ERANGE;

        जइ ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                जइ (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                अण्णहा
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        जइ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                संरचना stat st;

                जइ (fstat(fd, &st) < 0)
                        प्रत्यागम -errno;

                जइ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        जइ (r < 0)
                                प्रत्यागम r;
                }

                जइ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        जइ (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                जइ (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        जइ (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (शून्य) warn_file_is_world_accessible(filename, &st, रिक्त, 0);
                }
        } अण्णहा जइ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                प्रत्यागम -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        जइ (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        जइ (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        जइ (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                प्रत्यागम -errno;

        n = l = 0;
        प्रति (;;) {
                वर्ण *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                जइ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                जइ (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        जइ (!t) {
                                r = -ENOMEM;
                                गच्छ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        मुक्त(buf);
                } अण्णहा {
                        t = realloc(buf, n_next + 1);
                        जइ (!t)
                                प्रत्यागम -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                प्रतिज्ञा(k <= n - l);
                l += k;

                जइ (ferror(f)) {
                        r = errno_or_else(EIO);
                        गच्छ finalize;
                }
                जइ (feof(f))
                        भङ्ग;

                जइ (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        प्रतिज्ञा(l == size);
                        भङ्ग;
                }

                प्रतिज्ञा(k > 0); /* we can't have read zero bytes because that would have been EOF */

                जइ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        गच्छ finalize;
                }

                जइ (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        गच्छ finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        जइ (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ शून्य *decoded = रिक्त;
                size_t decoded_size;

                buf[l++] = 0;
                जइ (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                अण्णहा
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                जइ (r < 0)
                        गच्छ finalize;

                जइ (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                जइ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        गच्छ finalize;
                }
        }

        जइ (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                जइ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        गच्छ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        जइ (ret_size)
                *ret_size = l;

        प्रत्यागम 0;

finalize:
        जइ (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        प्रत्यागम r;
}

पूर्णाङ्क read_full_file_full(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                ध्रुव वर्ण *bind_name,
                वर्ण **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = रिक्त;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_contents);

        जइ (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

पूर्णाङ्क script_get_shebang_interpreter(ध्रुव वर्ण *path, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        f = fopen(path, "re");
        जइ (!f)
                प्रत्यागम -errno;

        वर्ण c;
        r = safe_fgetc(f, &c);
        जइ (r < 0)
                प्रत्यागम r;
        जइ (r == 0)
                प्रत्यागम -EBADMSG;
        जइ (c != '#')
                प्रत्यागम -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        जइ (r < 0)
                प्रत्यागम r;
        जइ (r == 0)
                प्रत्यागम -EBADMSG;
        जइ (c != '!')
                प्रत्यागम -EMEDIUMTYPE;

        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
        जइ (r < 0)
                प्रत्यागम r;

        _cleanup_free_ वर्ण *p = रिक्त;
        ध्रुव वर्ण *s = पङ्क्तिः;

        r = extract_first_word(&s, &p, /* separators= */ रिक्त, /* flags= */ 0);
        जइ (r < 0)
                प्रत्यागम r;
        जइ (r == 0)
                प्रत्यागम -ENOEXEC;

        जइ (ret)
                *ret = TAKE_PTR(p);
        प्रत्यागम 0;
}

पूर्णाङ्क get_proc_field(ध्रुव वर्ण *path, ध्रुव वर्ण *कुञ्चिका, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        प्रतिज्ञा(path);
        प्रतिज्ञा(कुञ्चिका);

        r = fopen_unlocked(path, "re", &f);
        जइ (r == -ENOENT && proc_mounted() == 0)
                प्रत्यागम -ENOSYS;
        जइ (r < 0)
                प्रत्यागम r;

        प्रति (;;) {
                 _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;

                 r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                 जइ (r < 0)
                         प्रत्यागम r;
                 जइ (r == 0)
                         प्रत्यागम -ENODATA;

                 वर्ण *l = startswith(पङ्क्तिः, कुञ्चिका);
                 जइ (l && *l == ':') {
                         जइ (ret) {
                                 वर्ण *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 जइ (!s)
                                         प्रत्यागम -ENOMEM;

                                 *ret = s;
                         }

                         प्रत्यागम 0;
                 }
        }
}

DIR* xopendirat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क flags) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(!(flags & (O_CREAT|O_TMPFILE)));

        जइ ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                प्रत्यागम opendir(path);

        जइ (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        जइ (fd < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम take_fdopendir(&fd);
}

पूर्णाङ्क fopen_mode_to_flags(ध्रुव वर्ण *mode) {
        ध्रुव वर्ण *p;
        पूर्णाङ्क flags;

        प्रतिज्ञा(mode);

        जइ ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        अण्णहा जइ ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        अण्णहा जइ ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        अण्णहा जइ ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        अण्णहा जइ ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        अण्णहा जइ ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        अण्णहा
                प्रत्यागम -EINVAL;

        प्रति (; *p != 0; p++) {

                विकल्प (*p) {

                पक्ष 'e':
                        flags |= O_CLOEXEC;
                        भङ्ग;

                पक्ष 'x':
                        flags |= O_EXCL;
                        भङ्ग;

                पक्ष 'm':
                        /* ignore this here, fdopen() might care later though */
                        भङ्ग;

                पक्ष 'c': /* not sure what to do about this one */
                स्वभाव:
                        प्रत्यागम -EINVAL;
                }
        }

        प्रत्यागम flags;
}

स्थिर पूर्णाङ्क xfopenat_regular(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *mode, पूर्णाङ्क open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(mode);
        प्रतिज्ञा(ret);

        जइ (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        अण्णहा जइ (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ वर्ण *j = strjoin("/", path);
                जइ (!j)
                        प्रत्यागम -ENOMEM;

                f = fopen(j, mode);
        } अण्णहा {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                पूर्णाङ्क mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                जइ (mode_flags < 0)
                        प्रत्यागम mode_flags;

                जइ (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        जइ (fd < 0)
                                प्रत्यागम -errno;
                } अण्णहा {
                        जइ (dir_fd == AT_FDCWD)
                                प्रत्यागम -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        जइ (fd < 0)
                                प्रत्यागम fd;
                }

                f = take_fdopen(&fd, mode);
        }
        जइ (!f)
                प्रत्यागम -errno;

        *ret = f;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क xfopenat_unix_socket(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *bind_name, FILE **ret) {
        _cleanup_close_ पूर्णाङ्क sk = -EBADF;
        FILE *f;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        जइ (sk < 0)
                प्रत्यागम -errno;

        जइ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                सङ्घ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                जइ (r < 0)
                        प्रत्यागम r;

                जइ (bind(sk, &bsa.sa, r) < 0)
                        प्रत्यागम -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (shutdown(sk, SHUT_WR) < 0)
                प्रत्यागम -errno;

        f = take_fdopen(&sk, "r");
        जइ (!f)
                प्रत्यागम -errno;

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क xfopenat_full(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *mode,
                पूर्णाङ्क open_flags,
                XfopenFlags flags,
                ध्रुव वर्ण *bind_name,
                FILE **ret) {

        FILE *f = रिक्त;  /* avoid false maybe-uninitialized warning */
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(mode);
        प्रतिज्ञा(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        जइ (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                जइ (IN_SET(r, -ENOTSOCK, -EINVAL))
                        प्रत्यागम -ENXIO; /* propagate original error if this is not a socket after all */
        }
        जइ (r < 0)
                प्रत्यागम r;

        जइ (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क fdopen_independent(पूर्णाङ्क fd, ध्रुव वर्ण *mode, FILE **ret) {
        _cleanup_close_ पूर्णाङ्क copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क mode_flags;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(mode);
        प्रतिज्ञा(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        जइ (mode_flags < 0)
                प्रत्यागम mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        जइ (copy_fd < 0)
                प्रत्यागम copy_fd;

        f = take_fdopen(&copy_fd, mode);
        जइ (!f)
                प्रत्यागम -errno;

        *ret = TAKE_PTR(f);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क अन्वेषणम्_and_open_internal(
                ध्रुव वर्ण *path,
                पूर्णाङ्क mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ध्रुव वर्ण *मूलम्,
                वर्ण **अन्वेषणम्,
                पूर्णाङ्क *ret_fd,
                वर्ण **ret_path) {

        पूर्णाङ्क r;

        प्रतिज्ञा(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        प्रतिज्ञा(path);

        जइ (path_is_absolute(path)) {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;

                जइ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                अण्णहा
                        r = RET_NERRNO(access(path, mode));
                जइ (r < 0)
                        प्रत्यागम r;

                जइ (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        जइ (r < 0)
                                प्रत्यागम r;
                }

                जइ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                प्रत्यागम 0;
        }

        जइ (!path_strv_resolve_uniq(अन्वेषणम्, मूलम्))
                प्रत्यागम -ENOMEM;

        STRV_FOREACH(i, अन्वेषणम्) {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                _cleanup_free_ वर्ण *p = रिक्त;

                p = path_join(मूलम्, *i, path);
                जइ (!p)
                        प्रत्यागम -ENOMEM;

                जइ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                अण्णहा
                        r = RET_NERRNO(access(p, F_OK));
                जइ (r >= 0) {
                        जइ (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        जइ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        प्रत्यागम 0;
                }
                जइ (r != -ENOENT)
                        प्रत्यागम r;
        }

        प्रत्यागम -ENOENT;
}

पूर्णाङ्क अन्वेषणम्_and_open(
                ध्रुव वर्ण *path,
                पूर्णाङ्क mode,
                ध्रुव वर्ण *मूलम्,
                वर्ण **अन्वेषणम्,
                पूर्णाङ्क *ret_fd,
                वर्ण **ret_path) {

        _cleanup_strv_free_ वर्ण **copy = रिक्त;

        प्रतिज्ञा(path);

        copy = strv_copy(अन्वेषणम्);
        जइ (!copy)
                प्रत्यागम -ENOMEM;

        प्रत्यागम अन्वेषणम्_and_open_internal(path, mode, मूलम्, copy, ret_fd, ret_path);
}

स्थिर पूर्णाङ्क अन्वेषणम्_and_fopen_internal(
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *mode,
                ध्रुव वर्ण *मूलम्,
                वर्ण **अन्वेषणम्,
                FILE **ret_file,
                वर्ण **ret_path) {

        _cleanup_free_ वर्ण *found_path = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode || !ret_file);

        r = अन्वेषणम्_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        मूलम्,
                        अन्वेषणम्,
                        ret_file ? &fd : रिक्त,
                        ret_path ? &found_path : रिक्त);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                जइ (!f)
                        प्रत्यागम -errno;

                *ret_file = f;
        }

        जइ (ret_path)
                *ret_path = TAKE_PTR(found_path);

        प्रत्यागम 0;
}

पूर्णाङ्क अन्वेषणम्_and_fopen(
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *mode,
                ध्रुव वर्ण *मूलम्,
                ध्रुव वर्ण **अन्वेषणम्,
                FILE **ret_file,
                वर्ण **ret_path) {

        _cleanup_strv_free_ वर्ण **copy = रिक्त;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode || !ret_file);

        copy = strv_copy((वर्ण**) अन्वेषणम्);
        जइ (!copy)
                प्रत्यागम -ENOMEM;

        प्रत्यागम अन्वेषणम्_and_fopen_internal(path, mode, मूलम्, copy, ret_file, ret_path);
}

पूर्णाङ्क अन्वेषणम्_and_fopen_nulstr(
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *mode,
                ध्रुव वर्ण *मूलम्,
                ध्रुव वर्ण *अन्वेषणम्,
                FILE **ret_file,
                वर्ण **ret_path) {

        _cleanup_strv_free_ वर्ण **l = रिक्त;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode || !ret_file);

        l = strv_split_nulstr(अन्वेषणम्);
        जइ (!l)
                प्रत्यागम -ENOMEM;

        प्रत्यागम अन्वेषणम्_and_fopen_internal(path, mode, मूलम्, l, ret_file, ret_path);
}

पूर्णाङ्क fflush_and_check(FILE *f) {
        प्रतिज्ञा(f);

        errno = 0;
        fflush(f);

        जइ (ferror(f))
                प्रत्यागम errno_or_else(EIO);

        प्रत्यागम 0;
}

पूर्णाङ्क fflush_sync_and_check(FILE *f) {
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(f);

        r = fflush_and_check(f);
        जइ (r < 0)
                प्रत्यागम r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        जइ (fd < 0)
                प्रत्यागम 0;

        r = fsync_full(fd);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम 0;
}

पूर्णाङ्क write_timestamp_file_atomic(ध्रुव वर्ण *fn, usec_t n) {
        वर्ण ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        जइ (!timestamp_is_set(n))
                प्रत्यागम -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        प्रत्यागम write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

पूर्णाङ्क read_timestamp_file(ध्रुव वर्ण *fn, usec_t *ret) {
        _cleanup_free_ वर्ण *ln = रिक्त;
        uint64_t t;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = read_one_पङ्क्तिः_file(fn, &ln);
        जइ (r < 0)
                प्रत्यागम r;

        r = safe_atou64(ln, &t);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (!timestamp_is_set(t))
                प्रत्यागम -ERANGE;

        *ret = (usec_t) t;
        प्रत्यागम 0;
}

पूर्णाङ्क fputs_with_separator(FILE *f, ध्रुव वर्ण *s, ध्रुव वर्ण *separator, bool *space) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        जइ (!f)
                f = stdout;

        जइ (!separator)
                separator = " ";

        जइ (*space)
                जइ (fputs(separator, f) < 0)
                        प्रत्यागम -EIO;

        *space = true;

        जइ (fputs(s, f) < 0)
                प्रत्यागम -EIO;

        प्रत्यागम 0;
}

पूर्णाङ्क fputs_with_newline(FILE *f, ध्रुव वर्ण *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        जइ (isempty(s))
                प्रत्यागम 0;

        जइ (!f)
                f = stdout;

        जइ (fputs(s, f) < 0)
                प्रत्यागम -EIO;

        जइ (endswith(s, "\n"))
                प्रत्यागम 0;

        जइ (fputc('\n', f) < 0)
                प्रत्यागम -EIO;

        प्रत्यागम 1;
}

/* A bitmask of the EOL markers we know */
प्रकारनिर्देश गणना End_Of_पङ्क्तिः_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_पङ्क्तिः_Marker;

स्थिर End_Of_पङ्क्तिः_Marker categorize_eol(वर्ण c, Read_पङ्क्तिः_Flags flags) {

        जइ (!FLAGS_SET(flags, READ_पङ्क्तिः_ONLY_NUL)) {
                जइ (c == '\n')
                        प्रत्यागम EOL_TEN;
                जइ (c == '\r')
                        प्रत्यागम EOL_THIRTEEN;
        }

        जइ (c == '\0')
                प्रत्यागम EOL_ZERO;

        प्रत्यागम EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, रिक्त);

पूर्णाङ्क read_पङ्क्तिः_full(FILE *f, size_t limit, Read_पङ्क्तिः_Flags flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *buffer = रिक्त;
        size_t n = 0, गणनम् = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

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

        जइ (ret) {
                जइ (!GREEDY_REALLOC(buffer, 1))
                        प्रत्यागम -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_पङ्क्तिः_Marker previous_eol = EOL_NONE;
                flockfile(f);

                प्रति (;;) {
                        End_Of_पङ्क्तिः_Marker eol;
                        वर्ण c;

                        जइ (n >= limit)
                                प्रत्यागम -ENOBUFS;

                        जइ (गणनम् >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                प्रत्यागम -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        जइ (r < 0)
                                प्रत्यागम r;
                        जइ (r == 0) /* EOF is definitely EOL */
                                भङ्ग;

                        eol = categorize_eol(c, flags);

                        जइ (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned वर्ण) c, f) != EOF);
                                भङ्ग;
                        }

                        गणनम्++;

                        जइ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                जइ ((flags & (READ_पङ्क्तिः_IS_A_TTY|READ_पङ्क्तिः_NOT_A_TTY)) == 0) {
                                        पूर्णाङ्क fd;

                                        fd = fileno(f);
                                        जइ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_पङ्क्तिः_NOT_A_TTY;
                                        अण्णहा
                                                flags |= isatty_safe(fd) ? READ_पङ्क्तिः_IS_A_TTY : READ_पङ्क्तिः_NOT_A_TTY;
                                }
                                जइ (FLAGS_SET(flags, READ_पङ्क्तिः_IS_A_TTY))
                                        भङ्ग;
                        }

                        जइ (eol != EOL_NONE) {
                                previous_eol |= eol;
                                अनुवर्त;
                        }

                        जइ (ret) {
                                जइ (!GREEDY_REALLOC(buffer, n + 2))
                                        प्रत्यागम -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        जइ (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        प्रत्यागम (पूर्णाङ्क) गणनम्;
}

पूर्णाङ्क read_stripped_पङ्क्तिः(FILE *f, size_t limit, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r, k;

        प्रतिज्ञा(f);

        r = read_पङ्क्तिः(f, limit, ret ? &s : रिक्त);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (ret) {
                ध्रुव वर्ण *p = strstrip(s);
                जइ (p == s)
                        *ret = TAKE_PTR(s);
                अण्णहा {
                        k = strdup_to(ret, p);
                        जइ (k < 0)
                                प्रत्यागम k;
                }
        }

        प्रत्यागम r > 0;          /* Return 1 if something was read. */
}

पूर्णाङ्क safe_fgetc(FILE *f, वर्ण *ret) {
        पूर्णाङ्क k;

        प्रतिज्ञा(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        जइ (k == EOF) {
                जइ (ferror(f))
                        प्रत्यागम errno_or_else(EIO);

                जइ (ret)
                        *ret = 0;

                प्रत्यागम 0;
        }

        जइ (ret)
                *ret = k;

        प्रत्यागम 1;
}

पूर्णाङ्क warn_file_is_world_accessible(ध्रुव वर्ण *filename, संरचना stat *st, ध्रुव वर्ण *unit, unsigned पङ्क्तिः) {
        संरचना stat _st;

        जइ (!filename)
                प्रत्यागम 0;

        जइ (!st) {
                जइ (stat(filename, &_st) < 0)
                        प्रत्यागम -errno;
                st = &_st;
        }

        जइ ((st->st_mode & S_IRWXO) == 0)
                प्रत्यागम 0;

        जइ (unit)
                log_syntax(unit, LOG_WARNING, filename, पङ्क्तिः, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        अण्णहा
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        प्रत्यागम 0;
}

पूर्णाङ्क write_data_file_atomic_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *path,
                ध्रुव संरचना iovec *iovec,
                WriteDataFileFlags flags) {

        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ वर्ण *dn = रिक्त, *fn = रिक्त;
        r = path_split_prefix_filename(path, &dn, &fn);
        जइ (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                प्रत्यागम -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        जइ (r < 0)
                प्रत्यागम r;

        _cleanup_close_ पूर्णाङ्क mfd = -EBADF;
        जइ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ रिक्त,
                            &mfd);
                जइ (r < 0)
                        प्रत्यागम r;

                dir_fd = mfd;
        }

        _cleanup_free_ वर्ण *t = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        जइ (fd < 0)
                प्रत्यागम fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        जइ (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        जइ (r < 0)
                प्रत्यागम r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        जइ (r < 0)
                प्रत्यागम r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        प्रत्यागम 0;
}
