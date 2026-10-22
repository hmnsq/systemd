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
#परिभाषय READ_पूर्णम्_अष्टकानि_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#परिभाषय READ_पूर्णम्_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#परिभाषय READ_VIRTUAL_अष्टकानि_MAX (4U * U64_MB - UINT64_C(2))

पूर्णाङ्क fdopen_unlocked(पूर्णाङ्क fd, ध्रुव वर्ण *options, FILE **ret) {
        प्रतिज्ञा(ret);

        FILE *f = fdopen(fd, options);
        यदि (!f)
                प्रत्यागम -errno;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क take_fdopen_unlocked(पूर्णाङ्क *fd, ध्रुव वर्ण *options, FILE **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd);

        r = fdopen_unlocked(*fd, options, ret);
        यदि (r < 0)
                प्रत्यागम r;

        *fd = -EBADF;

        प्रत्यागम 0;
}

FILE* take_fdopen(पूर्णाङ्क *fd, ध्रुव वर्ण *options) {
        प्रतिज्ञा(fd);

        FILE *f = fdopen(*fd, options);
        यदि (!f)
                प्रत्यागम रिक्त;

        *fd = -EBADF;

        प्रत्यागम f;
}

DIR* take_fdopendir(पूर्णाङ्क *dfd) {
        प्रतिज्ञा(dfd);

        DIR *d = fdopendir(*dfd);
        यदि (!d)
                प्रत्यागम रिक्त;

        *dfd = -EBADF;

        प्रत्यागम d;
}

FILE* open_memstream_unlocked(वर्ण **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        यदि (!f)
                प्रत्यागम रिक्त;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        प्रत्यागम f;
}

FILE* fmemopen_unlocked(शून्य *buf, size_t आकार, ध्रुव वर्ण *बहुलकम्) {
        FILE *f = fmemopen(buf, आकार, बहुलकम्);
        यदि (!f)
                प्रत्यागम रिक्त;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        प्रत्यागम f;
}

पूर्णाङ्क write_string_प्रवाहः_पूर्णम्(
                FILE *f,
                ध्रुव वर्ण *पङ्क्तिः,
                WriteStringFileFlags flags,
                ध्रुव संरचना timespec *ts) {

        bool needs_nl;
        पूर्णाङ्क r, fd = -EBADF;

        प्रतिज्ञा(f);
        प्रतिज्ञा(पङ्क्तिः);

        यदि (ferror(f))
                प्रत्यागम -EIO;

        यदि (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                यदि (fd < 0)
                        प्रत्यागम -EBADF;
        }

        यदि (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ वर्ण *t = रिक्त;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                यदि (fd < 0) {
                        fd = fileno(f);
                        यदि (fd < 0)
                                प्रत्यागम -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                यदि (read_virtual_file_fd(fd, वाक्यदैर्घ्यम्(पङ्क्तिः)+1, &t, रिक्त) > 0 &&
                    streq_skip_trailing_chars(पङ्क्तिः, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", पङ्क्तिः);
                        प्रत्यागम 0;
                }

                यदि (lseek(fd, 0, SEEK_SET) < 0)
                        प्रत्यागम -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(पङ्क्तिः, "\n");

        यदि (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_बफर)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                पङ्क्तिः = strjoina(पङ्क्तिः, "\n");
                needs_nl = false;
        }

        यदि (fputs(पङ्क्तिः, f) == EOF)
                प्रत्यागम -errno;

        यदि (needs_nl)
                यदि (fputc('\n', f) == EOF)
                        प्रत्यागम -errno;

        यदि (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        अन्यथा
                r = fflush_and_check(f);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ts) {
                ध्रुव संरचना timespec twice[2] = {*ts, *ts};

                प्रतिज्ञा(fd >= 0);
                यदि (futimens(fd, twice) < 0)
                        प्रत्यागम -errno;
        }

        प्रत्यागम 0;
}

स्थिर mode_t write_string_file_flags_गन्तव्यम्_बहुलकम्(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        प्रत्यागम FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_बहुलकम्_0600) ? 0600 :
                FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_बहुलकम्_0444) ? 0444 : 0644;
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

        mode_t बहुलकम् = write_string_file_flags_गन्तव्यम्_बहुलकम्(flags);

        bool call_label_ops_post = false;
        यदि (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, बहुलकम्, label_context);
                यदि (r < 0)
                        प्रत्यागम r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        पूर्णाङ्क k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? रिक्त : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        यदि (r < 0)
                प्रत्यागम r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        यदि (k < 0)
                प्रत्यागम k;

        r = write_string_प्रवाहः_पूर्णम्(f, पङ्क्तिः, flags, ts);
        यदि (r < 0)
                प्रत्यागम r;

        r = fchmod_umask(fileno(f), बहुलकम्);
        यदि (r < 0)
                प्रत्यागम r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        यदि (r < 0)
                प्रत्यागम r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        यदि (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_सम्बन्धि_file(fileno(f));
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क write_string_file_पूर्णम्_label(
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

        यदि (flags & WRITE_STRING_FILE_MKDIR_0755) {
                प्रतिज्ञा(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (flags & WRITE_STRING_FILE_ATOMIC) {
                प्रतिज्ञा(fn);
                प्रतिज्ञा(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, पङ्क्तिः, flags, ts, label_context);
                यदि (r < 0)
                        गच्छ fail;

                प्रत्यागम r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        यदि (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        अन्यथा {
                mode_t बहुलकम् = write_string_file_flags_गन्तव्यम्_बहुलकम्(flags);
                bool call_label_ops_post = false;

                यदि (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, बहुलकम्, label_context);
                        यदि (r < 0)
                                गच्छ fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_समुच्चयः(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                बहुलकम्,
                                &made_file);
                यदि (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? रिक्त : fn, made_file, label_context));
        }
        यदि (r < 0)
                गच्छ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        यदि (r < 0)
                गच्छ fail;

        यदि (flags & WRITE_STRING_FILE_DISABLE_बफर)
                setvbuf(f, रिक्त, _IONBF, 0);

        r = write_string_प्रवाहः_पूर्णम्(f, पङ्क्तिः, flags, ts);
        यदि (r < 0)
                गच्छ fail;

        प्रत्यागम 0;

fail:
        यदि (made_file)
                (शून्य) unlinkat(dir_fd, fn, 0);

        यदि (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                प्रत्यागम r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        यदि (verify_file_at(dir_fd, fn, पङ्क्तिः, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
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

        यदि (r < 0)
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

        यदि (r < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम write_string_file_at(dir_fd, fn, p, flags);
}

पूर्णाङ्क write_base64_file_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *fn,
                ध्रुव संरचना iovec *दत्त,
                WriteStringFileFlags flags) {

        _cleanup_free_ वर्ण *encoded = रिक्त;
        ssize_t n;

        n = base64mem_पूर्णम्(दत्त ? दत्त->iov_base : रिक्त, दत्त ? दत्त->iov_len : 0, 79, &encoded);
        यदि (n < 0)
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
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, ret);
}

पूर्णाङ्क read_boolean_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(filename);

        r = read_one_पङ्क्तिः_file_at(dir_fd, filename, &s);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम parse_boolean(s);
}

पूर्णाङ्क verify_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_free_ वर्ण *buf = रिक्त;
        size_t l, k;
        पूर्णाङ्क r;

        प्रतिज्ञा(blob);

        l = वाक्यदैर्घ्यम्(blob);

        यदि (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = स्मृतिं_देहि(l + accept_extra_nl + 1);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        यदि (r < 0)
                प्रत्यागम r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        यदि (ferror(f))
                प्रत्यागम errno_or_else(EIO);

        यदि (k != l && k != l + accept_extra_nl)
                प्रत्यागम 0;
        यदि (memcmp(buf, blob, l) != 0)
                प्रत्यागम 0;
        यदि (k > l && buf[l] != '\n')
                प्रत्यागम 0;

        प्रत्यागम 1;
}

पूर्णाङ्क read_virtual_file_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *filename,
                size_t max_आकार,
                वर्ण **ret_contents,
                size_t *ret_आकार) {

        _cleanup_free_ वर्ण *buf = रिक्त;
        size_t n, आकार;
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
        प्रतिज्ञा(max_आकार <= READ_VIRTUAL_अष्टकानि_MAX || max_आकार == SIZE_MAX);

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        यदि (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        अन्यथा
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        यदि (fd < 0)
                प्रत्यागम fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        प्रति (;;) {
                संरचना stat st;

                यदि (fstat(fd, &st) < 0)
                        प्रत्यागम -errno;

                यदि (!S_ISREG(st.st_mode))
                        प्रत्यागम -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_अष्टकानि_MAX < SSIZE_MAX);
                यदि (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        यदि (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                यदि (max_आकार == SIZE_MAX)
                                        प्रत्यागम -EFBIG;

                                आकार = max_आकार;
                        } अन्यथा {
                                आकार = MIN((size_t) st.st_size, max_आकार);

                                यदि (आकार > READ_VIRTUAL_अष्टकानि_MAX)
                                        प्रत्यागम -EFBIG;
                        }

                        n_retries--;
                } अन्यथा यदि (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        आकार = MIN3(page_आकार() - 1, READ_VIRTUAL_अष्टकानि_MAX, max_आकार);
                        n_retries = 1;
                } अन्यथा {
                        आकार = MIN(READ_VIRTUAL_अष्टकानि_MAX, max_आकार);
                        n_retries = 0;
                }

                buf = स्मृतिं_देहि(आकार + 1);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                आकार = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_आकार, READ_VIRTUAL_अष्टकानि_MAX);

                प्रति (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = पठ(fd, buf, आकार + 1);
                        यदि (k >= 0) {
                                n = k;
                                भङ्ग;
                        }

                        यदि (errno != EINTR)
                                प्रत्यागम -errno;
                }

                /* Consider a short read as EOF */
                यदि (n <= आकार)
                        भङ्ग;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                यदि (max_आकार != SIZE_MAX && n > max_आकार) {
                        n = आकार; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        भङ्ग;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                यदि (n_retries <= 0)
                        प्रत्यागम -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                यदि (lseek(fd, 0, SEEK_SET) < 0)
                        प्रत्यागम -errno;

                buf = mfree(buf);
        }

        यदि (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                यदि (!ret_आकार && memchr(buf, 0, n))
                        प्रत्यागम -EBADMSG;

                यदि (n < आकार) {
                        वर्ण *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        यदि (!p)
                                प्रत्यागम -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        यदि (ret_आकार)
                *ret_आकार = n;

        प्रत्यागम !truncated;
}

पूर्णाङ्क read_पूर्णम्_प्रवाहः_पूर्णम्(
                FILE *f,
                ध्रुव वर्ण *filename,
                uint64_t विस्थापनम्,
                size_t आकार,
                Read_पूर्णम्_File_Flags flags,
                वर्ण **ret_contents,
                size_t *ret_आकार) {

        _cleanup_free_ वर्ण *buf = रिक्त;
        size_t n, n_अग्र = 0, l, expected_decoded_आकार = आकार;
        पूर्णाङ्क fd, r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret_contents);
        प्रतिज्ञा(!FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_UNBASE64 | READ_पूर्णम्_FILE_UNHEX));
        प्रतिज्ञा(आकार != SIZE_MAX || !FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER));

        यदि (विस्थापनम् != UINT64_MAX && विस्थापनम् > LONG_MAX) /* fseek() can only deal with "long" offsets */
                प्रत्यागम -ERANGE;

        यदि ((flags & (READ_पूर्णम्_FILE_UNBASE64 | READ_पूर्णम्_FILE_UNHEX)) != 0) {
                यदि (आकार <= SIZE_MAX / READ_पूर्णम्_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        आकार *= READ_पूर्णम्_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                अन्यथा
                        आकार = SIZE_MAX;
        }

        fd = fileno(f);
        यदि (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                संरचना stat st;

                यदि (fstat(fd, &st) < 0)
                        प्रत्यागम -errno;

                यदि (FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        यदि (st.st_size > 0 &&
                            (आकार == SIZE_MAX || FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, विस्थापनम् == UINT64_MAX ? 0 : विस्थापनम्);

                                यदि (rsize < SIZE_MAX) /* overflow check */
                                        n_अग्र = rsize + 1;
                        }

                        यदि (flags & READ_पूर्णम्_FILE_WARN_WORLD_READABLE)
                                (शून्य) warn_file_is_world_accessible(filename, &st, रिक्त, 0);
                }
        } अन्यथा यदि (FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_VERIFY_REGULAR))
                प्रत्यागम -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        यदि (n_अग्र == 0)
                n_अग्र = आकार != SIZE_MAX && !FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER) ? आकार : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        यदि (n_अग्र > READ_पूर्णम्_अष्टकानि_MAX)
                n_अग्र = READ_पूर्णम्_अष्टकानि_MAX + 1;

        यदि (विस्थापनम् != UINT64_MAX && fseek(f, विस्थापनम्, SEEK_SET) < 0)
                प्रत्यागम -errno;

        n = l = 0;
        प्रति (;;) {
                वर्ण *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                यदि (FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER) && n_अग्र > आकार)
                        n_अग्र = आकार + 1;

                यदि (flags & READ_पूर्णम्_FILE_SECURE) {
                        t = स्मृतिं_देहि(n_अग्र + 1);
                        यदि (!t) {
                                r = -ENOMEM;
                                गच्छ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        मुक्त(buf);
                } अन्यथा {
                        t = realloc(buf, n_अग्र + 1);
                        यदि (!t)
                                प्रत्यागम -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = आकार == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_अग्र;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                प्रतिज्ञा(k <= n - l);
                l += k;

                यदि (ferror(f)) {
                        r = errno_or_else(EIO);
                        गच्छ finalize;
                }
                यदि (feof(f))
                        भङ्ग;

                यदि (आकार != SIZE_MAX && !FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        प्रतिज्ञा(l == आकार);
                        भङ्ग;
                }

                प्रतिज्ञा(k > 0); /* we can't have read zero bytes because that would have been EOF */

                यदि (FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER) && l > आकार) {
                        r = -E2BIG;
                        गच्छ finalize;
                }

                यदि (n >= READ_पूर्णम्_अष्टकानि_MAX) {
                        r = -E2BIG;
                        गच्छ finalize;
                }

                n_अग्र = MIN(n * 2, READ_पूर्णम्_अष्टकानि_MAX);
        }

        यदि (flags & (READ_पूर्णम्_FILE_UNBASE64 | READ_पूर्णम्_FILE_UNHEX)) {
                _cleanup_free_ शून्य *decoded = रिक्त;
                size_t decoded_आकार;

                buf[l++] = 0;
                यदि (flags & READ_पूर्णम्_FILE_UNBASE64)
                        r = unbase64mem_पूर्णम्(buf, l, flags & READ_पूर्णम्_FILE_SECURE, &decoded, &decoded_आकार);
                अन्यथा
                        r = unhexmem_पूर्णम्(buf, l, flags & READ_पूर्णम्_FILE_SECURE, &decoded, &decoded_आकार);
                यदि (r < 0)
                        गच्छ finalize;

                यदि (flags & READ_पूर्णम्_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_आकार;

                यदि (FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_आकार) {
                        r = -E2BIG;
                        गच्छ finalize;
                }
        }

        यदि (!ret_आकार) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                यदि (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        गच्छ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        यदि (ret_आकार)
                *ret_आकार = l;

        प्रत्यागम 0;

finalize:
        यदि (flags & READ_पूर्णम्_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        प्रत्यागम r;
}

पूर्णाङ्क read_पूर्णम्_file_पूर्णम्(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *filename,
                uint64_t विस्थापनम्,
                size_t आकार,
                Read_पूर्णम्_File_Flags flags,
                ध्रुव वर्ण *bind_name,
                वर्ण **ret_contents,
                size_t *ret_आकार) {

        _cleanup_fclose_ FILE *f = रिक्त;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_contents);

        यदि (FLAGS_समुच्चयः(flags, READ_पूर्णम्_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            विस्थापनम् == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_पूर्णम्(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम read_पूर्णम्_प्रवाहः_पूर्णम्(f, filename, विस्थापनम्, आकार, flags, ret_contents, ret_आकार);
}

पूर्णाङ्क script_get_shebang_interpreter(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        f = fopen(मार्गः, "re");
        यदि (!f)
                प्रत्यागम -errno;

        वर्ण c;
        r = safe_fgetc(f, &c);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EBADMSG;
        यदि (c != '#')
                प्रत्यागम -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EBADMSG;
        यदि (c != '!')
                प्रत्यागम -EMEDIUMTYPE;

        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_free_ वर्ण *p = रिक्त;
        ध्रुव वर्ण *s = पङ्क्तिः;

        r = extract_first_word(&s, &p, /* separators= */ रिक्त, /* flags= */ 0);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -ENOEXEC;

        यदि (ret)
                *ret = TAKE_PTR(p);
        प्रत्यागम 0;
}

पूर्णाङ्क get_proc_क्षेत्रम्(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *कुञ्चिका, वर्ण **ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(कुञ्चिका);

        r = fopen_unlocked(मार्गः, "re", &f);
        यदि (r == -ENOENT && proc_mounted() == 0)
                प्रत्यागम -ENOSYS;
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (;;) {
                 _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;

                 r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                 यदि (r < 0)
                         प्रत्यागम r;
                 यदि (r == 0)
                         प्रत्यागम -ENODATA;

                 वर्ण *l = startswith(पङ्क्तिः, कुञ्चिका);
                 यदि (l && *l == ':') {
                         यदि (ret) {
                                 वर्ण *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 यदि (!s)
                                         प्रत्यागम -ENOMEM;

                                 *ret = s;
                         }

                         प्रत्यागम 0;
                 }
        }
}

DIR* xopendirat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(!(flags & (O_CREAT|O_TMPFILE)));

        यदि ((dir_fd == AT_FDCWD || मार्गः_is_absolute(मार्गः)) &&
            (flags &~ O_DIRECTORY) == 0)
                प्रत्यागम opendir(मार्गः);

        यदि (isempty(मार्गः)) {
                मार्गः = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, मार्गः, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        यदि (fd < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम take_fdopendir(&fd);
}

पूर्णाङ्क fopen_बहुलकम्_गन्तव्यम्_flags(ध्रुव वर्ण *बहुलकम्) {
        ध्रुव वर्ण *p;
        पूर्णाङ्क flags;

        प्रतिज्ञा(बहुलकम्);

        यदि ((p = startswith(बहुलकम्, "r+")))
                flags = O_RDWR;
        अन्यथा यदि ((p = startswith(बहुलकम्, "r")))
                flags = O_RDONLY;
        अन्यथा यदि ((p = startswith(बहुलकम्, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        अन्यथा यदि ((p = startswith(बहुलकम्, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        अन्यथा यदि ((p = startswith(बहुलकम्, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        अन्यथा यदि ((p = startswith(बहुलकम्, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        अन्यथा
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

स्थिर पूर्णाङ्क xfopenat_regular(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *बहुलकम्, पूर्णाङ्क open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(बहुलकम्);
        प्रतिज्ञा(ret);

        यदि (dir_fd == AT_FDCWD && मार्गः && open_flags == 0)
                f = fopen(मार्गः, बहुलकम्);
        अन्यथा यदि (dir_fd == XAT_FDROOT && मार्गः && open_flags == 0) {
                _cleanup_free_ वर्ण *j = strjoin("/", मार्गः);
                यदि (!j)
                        प्रत्यागम -ENOMEM;

                f = fopen(j, बहुलकम्);
        } अन्यथा {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                पूर्णाङ्क बहुलकम्_flags;

                बहुलकम्_flags = fopen_बहुलकम्_गन्तव्यम्_flags(बहुलकम्);
                यदि (बहुलकम्_flags < 0)
                        प्रत्यागम बहुलकम्_flags;

                यदि (मार्गः) {
                        fd = openat(dir_fd, मार्गः, बहुलकम्_flags | open_flags);
                        यदि (fd < 0)
                                प्रत्यागम -errno;
                } अन्यथा {
                        यदि (dir_fd == AT_FDCWD)
                                प्रत्यागम -EBADF;

                        fd = fd_reopen(dir_fd, (बहुलकम्_flags | open_flags) & ~O_NOFOLLOW);
                        यदि (fd < 0)
                                प्रत्यागम fd;
                }

                f = take_fdopen(&fd, बहुलकम्);
        }
        यदि (!f)
                प्रत्यागम -errno;

        *ret = f;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क xfopenat_unix_socket(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *bind_name, FILE **ret) {
        _cleanup_close_ पूर्णाङ्क sk = -EBADF;
        FILE *f;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        यदि (sk < 0)
                प्रत्यागम -errno;

        यदि (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                सङ्घ sockaddr_union bsa;

                r = sockaddr_un_समुच्चयः_मार्गः(&bsa.un, bind_name);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (bind(sk, &bsa.sa, r) < 0)
                        प्रत्यागम -errno;
        }

        r = connect_unix_मार्गः(sk, dir_fd, मार्गः);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (shutdown(sk, SHUT_WR) < 0)
                प्रत्यागम -errno;

        f = take_fdopen(&sk, "r");
        यदि (!f)
                प्रत्यागम -errno;

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क xfopenat_पूर्णम्(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *बहुलकम्,
                पूर्णाङ्क open_flags,
                XfopenFlags flags,
                ध्रुव वर्ण *bind_name,
                FILE **ret) {

        FILE *f = रिक्त;  /* avoid false maybe-uninitialized warning */
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(बहुलकम्);
        प्रतिज्ञा(ret);

        r = xfopenat_regular(dir_fd, मार्गः, बहुलकम्, open_flags, &f);
        यदि (r == -ENXIO && FLAGS_समुच्चयः(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, मार्गः, bind_name, &f);
                यदि (अन्तर्_समुच्चयः(r, -ENOTSOCK, -EINVAL))
                        प्रत्यागम -ENXIO; /* propagate original error if this is not a socket after all */
        }
        यदि (r < 0)
                प्रत्यागम r;

        यदि (FLAGS_समुच्चयः(flags, XFOPEN_UNLOCKED))
                (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        प्रत्यागम 0;
}

पूर्णाङ्क fdopen_independent(पूर्णाङ्क fd, ध्रुव वर्ण *बहुलकम्, FILE **ret) {
        _cleanup_close_ पूर्णाङ्क copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क बहुलकम्_flags;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(बहुलकम्);
        प्रतिज्ञा(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        बहुलकम्_flags = fopen_बहुलकम्_गन्तव्यम्_flags(बहुलकम्);
        यदि (बहुलकम्_flags < 0)
                प्रत्यागम बहुलकम्_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, बहुलकम्_flags & ~O_CREAT);
        यदि (copy_fd < 0)
                प्रत्यागम copy_fd;

        f = take_fdopen(&copy_fd, बहुलकम्);
        यदि (!f)
                प्रत्यागम -errno;

        *ret = TAKE_PTR(f);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क अन्वेषणम्_and_open_आन्तरिकम्(
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क बहुलकम्,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ध्रुव वर्ण *मूलम्,
                वर्ण **अन्वेषणम्,
                पूर्णाङ्क *ret_fd,
                वर्ण **ret_मार्गः) {

        पूर्णाङ्क r;

        प्रतिज्ञा(!ret_fd || !FLAGS_समुच्चयः(बहुलकम्, O_CREAT)); /* We don't support O_CREAT for this */
        प्रतिज्ञा(मार्गः);

        यदि (मार्गः_is_absolute(मार्गः)) {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;

                यदि (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(उद्घाटय(मार्गः, बहुलकम्, 0777));
                अन्यथा
                        r = RET_NERRNO(access(मार्गः, बहुलकम्));
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (ret_मार्गः) {
                        r = मार्गः_simplify_alloc(मार्गः, ret_मार्गः);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                प्रत्यागम 0;
        }

        यदि (!मार्गः_strv_resolve_uniq(अन्वेषणम्, मूलम्))
                प्रत्यागम -ENOMEM;

        STRV_FOREACH(i, अन्वेषणम्) {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                _cleanup_free_ वर्ण *p = रिक्त;

                p = मार्गः_join(मूलम्, *i, मार्गः);
                यदि (!p)
                        प्रत्यागम -ENOMEM;

                यदि (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(उद्घाटय(p, बहुलकम्, 0777));
                अन्यथा
                        r = RET_NERRNO(access(p, F_OK));
                यदि (r >= 0) {
                        यदि (ret_मार्गः)
                                *ret_मार्गः = मार्गः_simplify(TAKE_PTR(p));

                        यदि (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        प्रत्यागम 0;
                }
                यदि (r != -ENOENT)
                        प्रत्यागम r;
        }

        प्रत्यागम -ENOENT;
}

पूर्णाङ्क अन्वेषणम्_and_open(
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क बहुलकम्,
                ध्रुव वर्ण *मूलम्,
                वर्ण **अन्वेषणम्,
                पूर्णाङ्क *ret_fd,
                वर्ण **ret_मार्गः) {

        _cleanup_strv_free_ वर्ण **copy = रिक्त;

        प्रतिज्ञा(मार्गः);

        copy = strv_copy(अन्वेषणम्);
        यदि (!copy)
                प्रत्यागम -ENOMEM;

        प्रत्यागम अन्वेषणम्_and_open_आन्तरिकम्(मार्गः, बहुलकम्, मूलम्, copy, ret_fd, ret_मार्गः);
}

स्थिर पूर्णाङ्क अन्वेषणम्_and_fopen_आन्तरिकम्(
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *बहुलकम्,
                ध्रुव वर्ण *मूलम्,
                वर्ण **अन्वेषणम्,
                FILE **ret_file,
                वर्ण **ret_मार्गः) {

        _cleanup_free_ वर्ण *प्राप्तम्_मार्गः = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(बहुलकम् || !ret_file);

        r = अन्वेषणम्_and_open_आन्तरिकम्(
                        मार्गः,
                        बहुलकम् ? fopen_बहुलकम्_गन्तव्यम्_flags(बहुलकम्) : 0,
                        मूलम्,
                        अन्वेषणम्,
                        ret_file ? &fd : रिक्त,
                        ret_मार्गः ? &प्राप्तम्_मार्गः : रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_file) {
                FILE *f = take_fdopen(&fd, बहुलकम्);
                यदि (!f)
                        प्रत्यागम -errno;

                *ret_file = f;
        }

        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(प्राप्तम्_मार्गः);

        प्रत्यागम 0;
}

पूर्णाङ्क अन्वेषणम्_and_fopen(
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *बहुलकम्,
                ध्रुव वर्ण *मूलम्,
                ध्रुव वर्ण **अन्वेषणम्,
                FILE **ret_file,
                वर्ण **ret_मार्गः) {

        _cleanup_strv_free_ वर्ण **copy = रिक्त;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(बहुलकम् || !ret_file);

        copy = strv_copy((वर्ण**) अन्वेषणम्);
        यदि (!copy)
                प्रत्यागम -ENOMEM;

        प्रत्यागम अन्वेषणम्_and_fopen_आन्तरिकम्(मार्गः, बहुलकम्, मूलम्, copy, ret_file, ret_मार्गः);
}

पूर्णाङ्क अन्वेषणम्_and_fopen_nulstr(
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *बहुलकम्,
                ध्रुव वर्ण *मूलम्,
                ध्रुव वर्ण *अन्वेषणम्,
                FILE **ret_file,
                वर्ण **ret_मार्गः) {

        _cleanup_strv_free_ वर्ण **l = रिक्त;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(बहुलकम् || !ret_file);

        l = strv_विभज_nulstr(अन्वेषणम्);
        यदि (!l)
                प्रत्यागम -ENOMEM;

        प्रत्यागम अन्वेषणम्_and_fopen_आन्तरिकम्(मार्गः, बहुलकम्, मूलम्, l, ret_file, ret_मार्गः);
}

पूर्णाङ्क fflush_and_check(FILE *f) {
        प्रतिज्ञा(f);

        errno = 0;
        fflush(f);

        यदि (ferror(f))
                प्रत्यागम errno_or_else(EIO);

        प्रत्यागम 0;
}

पूर्णाङ्क fflush_sync_and_check(FILE *f) {
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(f);

        r = fflush_and_check(f);
        यदि (r < 0)
                प्रत्यागम r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        यदि (fd < 0)
                प्रत्यागम 0;

        r = fsync_पूर्णम्(fd);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम 0;
}

पूर्णाङ्क write_timestamp_file_atomic(ध्रुव वर्ण *fn, usec_t n) {
        वर्ण ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        यदि (!timestamp_is_समुच्चयः(n))
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
        यदि (r < 0)
                प्रत्यागम r;

        r = safe_atou64(ln, &t);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!timestamp_is_समुच्चयः(t))
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

        यदि (!f)
                f = stdout;

        यदि (!separator)
                separator = " ";

        यदि (*space)
                यदि (fputs(separator, f) < 0)
                        प्रत्यागम -EIO;

        *space = true;

        यदि (fputs(s, f) < 0)
                प्रत्यागम -EIO;

        प्रत्यागम 0;
}

पूर्णाङ्क fputs_with_newline(FILE *f, ध्रुव वर्ण *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        यदि (isempty(s))
                प्रत्यागम 0;

        यदि (!f)
                f = stdout;

        यदि (fputs(s, f) < 0)
                प्रत्यागम -EIO;

        यदि (endswith(s, "\n"))
                प्रत्यागम 0;

        यदि (fputc('\n', f) < 0)
                प्रत्यागम -EIO;

        प्रत्यागम 1;
}

/* A bitmask of the EOL markers we know */
प्रकारनिर्देश गणना अन्तः_सम्बन्धि_पङ्क्तिः_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} अन्तः_सम्बन्धि_पङ्क्तिः_Marker;

स्थिर अन्तः_सम्बन्धि_पङ्क्तिः_Marker categorize_eol(वर्ण c, Read_पङ्क्तिः_Flags flags) {

        यदि (!FLAGS_समुच्चयः(flags, READ_पङ्क्तिः_ONLY_NUL)) {
                यदि (c == '\n')
                        प्रत्यागम EOL_TEN;
                यदि (c == '\r')
                        प्रत्यागम EOL_THIRTEEN;
        }

        यदि (c == '\0')
                प्रत्यागम EOL_ZERO;

        प्रत्यागम EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_पूर्णम्(FILE*, funlockfile, रिक्त);

पूर्णाङ्क read_पङ्क्तिः_पूर्णम्(FILE *f, size_t limit, Read_पङ्क्तिः_Flags flags, वर्ण **ret) {
        _cleanup_free_ वर्ण *बफर = रिक्त;
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

        यदि (ret) {
                यदि (!GREEDY_REALLOC(बफर, 1))
                        प्रत्यागम -ENOMEM;
        }

        {
                अप्रयुक्तम् _cleanup_(funlockfilep) FILE *flocked = f;
                अन्तः_सम्बन्धि_पङ्क्तिः_Marker previous_eol = EOL_NONE;
                flockfile(f);

                प्रति (;;) {
                        अन्तः_सम्बन्धि_पङ्क्तिः_Marker eol;
                        वर्ण c;

                        यदि (n >= limit)
                                प्रत्यागम -ENOBUFS;

                        यदि (गणनम् >= अधिकतम_पूर्णाङ्कः) /* We couldn't return the counter anymore as "int", hence refuse this */
                                प्रत्यागम -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0) /* EOF is definitely EOL */
                                भङ्ग;

                        eol = categorize_eol(c, flags);

                        यदि (FLAGS_समुच्चयः(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((अचिह्नित वर्ण) c, f) != EOF);
                                भङ्ग;
                        }

                        गणनम्++;

                        यदि (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                यदि ((flags & (READ_पङ्क्तिः_IS_A_TTY|READ_पङ्क्तिः_NOT_A_TTY)) == 0) {
                                        पूर्णाङ्क fd;

                                        fd = fileno(f);
                                        यदि (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_पङ्क्तिः_NOT_A_TTY;
                                        अन्यथा
                                                flags |= isatty_safe(fd) ? READ_पङ्क्तिः_IS_A_TTY : READ_पङ्क्तिः_NOT_A_TTY;
                                }
                                यदि (FLAGS_समुच्चयः(flags, READ_पङ्क्तिः_IS_A_TTY))
                                        भङ्ग;
                        }

                        यदि (eol != EOL_NONE) {
                                previous_eol |= eol;
                                अनुवर्त;
                        }

                        यदि (ret) {
                                यदि (!GREEDY_REALLOC(बफर, n + 2))
                                        प्रत्यागम -ENOMEM;

                                बफर[n] = c;
                        }

                        n++;
                }
        }

        यदि (ret) {
                बफर[n] = 0;

                *ret = TAKE_PTR(बफर);
        }

        प्रत्यागम (पूर्णाङ्क) गणनम्;
}

पूर्णाङ्क read_stripped_पङ्क्तिः(FILE *f, size_t limit, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r, k;

        प्रतिज्ञा(f);

        r = read_पङ्क्तिः(f, limit, ret ? &s : रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret) {
                ध्रुव वर्ण *p = strstrip(s);
                यदि (p == s)
                        *ret = TAKE_PTR(s);
                अन्यथा {
                        k = strdup_गन्तव्यम्(ret, p);
                        यदि (k < 0)
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
        यदि (k == EOF) {
                यदि (ferror(f))
                        प्रत्यागम errno_or_else(EIO);

                यदि (ret)
                        *ret = 0;

                प्रत्यागम 0;
        }

        यदि (ret)
                *ret = k;

        प्रत्यागम 1;
}

पूर्णाङ्क warn_file_is_world_accessible(ध्रुव वर्ण *filename, संरचना stat *st, ध्रुव वर्ण *unit, अचिह्नित पङ्क्तिः) {
        संरचना stat _st;

        यदि (!filename)
                प्रत्यागम 0;

        यदि (!st) {
                यदि (stat(filename, &_st) < 0)
                        प्रत्यागम -errno;
                st = &_st;
        }

        यदि ((st->st_mode & S_IRWXO) == 0)
                प्रत्यागम 0;

        यदि (unit)
                log_syntax(unit, LOG_WARNING, filename, पङ्क्तिः, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        अन्यथा
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        प्रत्यागम 0;
}

पूर्णाङ्क write_दत्त_file_atomic_at(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *मार्गः,
                ध्रुव संरचना iovec *iovec,
                Write_दत्त_File_Flags flags) {

        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ वर्ण *dn = रिक्त, *fn = रिक्त;
        r = मार्गः_विभज_उपसर्गः_filename(मार्गः, &dn, &fn);
        यदि (अन्तर्_समुच्चयः(r, -EADDRNOTAVAIL, O_DIRECTORY))
                प्रत्यागम -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_close_ पूर्णाङ्क mfd = -EBADF;
        यदि (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_समुच्चयः(flags, WRITE_दत्त_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ रिक्त,
                            &mfd);
                यदि (r < 0)
                        प्रत्यागम r;

                dir_fd = mfd;
        }

        _cleanup_free_ वर्ण *t = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        यदि (fd < 0)
                प्रत्यागम fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        यदि (iovec_is_समुच्चयः(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        r = fchmod_umask(fd, FLAGS_समुच्चयः(flags, WRITE_दत्त_FILE_बहुलकम्_0400) ? 0400 : 0644);
        यदि (r < 0)
                प्रत्यागम r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        यदि (r < 0)
                प्रत्यागम r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        प्रत्यागम 0;
}
