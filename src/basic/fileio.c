/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <stdio_ext.h>
#समावेश <stdlib.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "hexdecoct.h"
#समावेश "io-util.h"
#समावेश "iovec-util.h"
#समावेश "label-util.h"
#समावेश "log.h"
#समावेश "mkdir.h"
#समावेश "nulstr-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "socket-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "sync-util.h"
#समावेश "terminal-util.h"
#समावेश "time-util.h"
#समावेश "tmpfile-util.h"

/* The maximum size of the file we'll read in one go in read_full_file() (64M). */
#घोषणा READ_FULL_BYTES_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#घोषणा READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#घोषणा READ_VIRTUAL_BYTES_MAX (4U * U64_MB - UINT64_C(2))

पूर्णांक fdopen_unlocked(पूर्णांक fd, स्थिर अक्षर *options, खाता **ret) {
        निश्चित(ret);

        खाता *f = fdopen(fd, options);
        अगर (!f)
                वापस -त्रुटि_सं;

        (व्योम) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        वापस 0;
}

पूर्णांक take_fdopen_unlocked(पूर्णांक *fd, स्थिर अक्षर *options, खाता **ret) {
        पूर्णांक r;

        निश्चित(fd);

        r = fdopen_unlocked(*fd, options, ret);
        अगर (r < 0)
                वापस r;

        *fd = -EBADF;

        वापस 0;
}

खाता* take_fdopen(पूर्णांक *fd, स्थिर अक्षर *options) {
        निश्चित(fd);

        खाता *f = fdopen(*fd, options);
        अगर (!f)
                वापस शून्य;

        *fd = -EBADF;

        वापस f;
}

सूची* take_fdopendir(पूर्णांक *dfd) {
        निश्चित(dfd);

        सूची *d = fdopendir(*dfd);
        अगर (!d)
                वापस शून्य;

        *dfd = -EBADF;

        वापस d;
}

खाता* open_memstream_unlocked(अक्षर **ptr, माप_प्रकार *sizeloc) {
        खाता *f = open_memstream(ptr, sizeloc);
        अगर (!f)
                वापस शून्य;

        (व्योम) __fsetlocking(f, FSETLOCKING_BYCALLER);

        वापस f;
}

खाता* fmemopen_unlocked(व्योम *buf, माप_प्रकार आकार, स्थिर अक्षर *mode) {
        खाता *f = fmemopen(buf, आकार, mode);
        अगर (!f)
                वापस शून्य;

        (व्योम) __fsetlocking(f, FSETLOCKING_BYCALLER);

        वापस f;
}

पूर्णांक write_string_stream_full(
                खाता *f,
                स्थिर अक्षर *पंक्ति,
                WriteStringFileFlags flags,
                स्थिर काष्ठा timespec *ts) {

        bool needs_nl;
        पूर्णांक r, fd = -EBADF;

        निश्चित(f);
        निश्चित(पंक्ति);

        अगर (ख_त्रुटि(f))
                वापस -EIO;

        अगर (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                अगर (fd < 0)
                        वापस -EBADF;
        }

        अगर (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ अक्षर *t = शून्य;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                अगर (fd < 0) {
                        fd = fileno(f);
                        अगर (fd < 0)
                                वापस -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                अगर (read_virtual_file_fd(fd, म_माप(पंक्ति)+1, &t, शून्य) > 0 &&
                    streq_skip_trailing_chars(पंक्ति, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", पंक्ति);
                        वापस 0;
                }

                अगर (lseek(fd, 0, शुरू_से) < 0)
                        वापस -त्रुटि_सं;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(पंक्ति, "\n");

        अगर (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_बफर)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                पंक्ति = strjoina(पंक्ति, "\n");
                needs_nl = false;
        }

        अगर (ख_माला_दो(पंक्ति, f) == खातापूर्ण)
                वापस -त्रुटि_सं;

        अगर (needs_nl)
                अगर (ख_अक्षर_दो('\n', f) == खातापूर्ण)
                        वापस -त्रुटि_सं;

        अगर (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        अन्यथा
                r = fflush_and_check(f);
        अगर (r < 0)
                वापस r;

        अगर (ts) {
                स्थिर काष्ठा timespec twice[2] = {*ts, *ts};

                निश्चित(fd >= 0);
                अगर (futimens(fd, twice) < 0)
                        वापस -त्रुटि_सं;
        }

        वापस 0;
}

अटल mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        वापस FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

अटल पूर्णांक write_string_file_atomic_at(
                पूर्णांक dir_fd,
                स्थिर अक्षर *fn,
                स्थिर अक्षर *पंक्ति,
                WriteStringFileFlags flags,
                स्थिर काष्ठा timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(fn);
        निश्चित(पंक्ति);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        अगर (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                अगर (r < 0)
                        वापस r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        पूर्णांक k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? शून्य : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        अगर (r < 0)
                वापस r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        अगर (k < 0)
                वापस k;

        r = write_string_stream_full(f, पंक्ति, flags, ts);
        अगर (r < 0)
                वापस r;

        r = fchmod_umask(fileno(f), mode);
        अगर (r < 0)
                वापस r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        अगर (r < 0)
                वापस r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        अगर (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक write_string_file_full_label(
                पूर्णांक dir_fd,
                स्थिर अक्षर *fn,
                स्थिर अक्षर *पंक्ति,
                WriteStringFileFlags flags,
                स्थिर काष्ठा timespec *ts,
                स्थिर अक्षर *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(dir_fd == AT_FDCWD || dir_fd >= 0);
        निश्चित(पंक्ति);

        /* We don't know how to verify whether the file contents was already on-disk. */
        निश्चित(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        अगर (flags & WRITE_STRING_FILE_MKDIR_0755) {
                निश्चित(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                अगर (r < 0)
                        वापस r;
        }

        अगर (flags & WRITE_STRING_FILE_ATOMIC) {
                निश्चित(fn);
                निश्चित(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, पंक्ति, flags, ts, label_context);
                अगर (r < 0)
                        जाओ fail;

                वापस r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        अगर (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        अन्यथा {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                अगर (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        अगर (r < 0)
                                जाओ fail;

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
                अगर (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? शून्य : fn, made_file, label_context));
        }
        अगर (r < 0)
                जाओ fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        अगर (r < 0)
                जाओ fail;

        अगर (flags & WRITE_STRING_FILE_DISABLE_बफर)
                रखो_भबफ(f, शून्य, _IONBF, 0);

        r = write_string_stream_full(f, पंक्ति, flags, ts);
        अगर (r < 0)
                जाओ fail;

        वापस 0;

fail:
        अगर (made_file)
                (व्योम) unlinkat(dir_fd, fn, 0);

        अगर (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                वापस r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        अगर (verify_file_at(dir_fd, fn, पंक्ति, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                वापस 0;

        वापस r;
}

पूर्णांक write_string_filef(
                स्थिर अक्षर *fn,
                WriteStringFileFlags flags,
                स्थिर अक्षर *format, ...) {

        _cleanup_free_ अक्षर *p = शून्य;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&p, format, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस -ENOMEM;

        वापस write_string_file(fn, p, flags);
}

पूर्णांक write_string_filef_at(
                पूर्णांक dir_fd,
                स्थिर अक्षर *fn,
                WriteStringFileFlags flags,
                स्थिर अक्षर *format, ...) {

        _cleanup_free_ अक्षर *p = शून्य;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&p, format, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस -ENOMEM;

        वापस write_string_file_at(dir_fd, fn, p, flags);
}

पूर्णांक write_base64_file_at(
                पूर्णांक dir_fd,
                स्थिर अक्षर *fn,
                स्थिर काष्ठा iovec *तथ्य,
                WriteStringFileFlags flags) {

        _cleanup_free_ अक्षर *encoded = शून्य;
        ssize_t n;

        n = base64mem_full(तथ्य ? तथ्य->iov_base : शून्य, तथ्य ? तथ्य->iov_len : 0, 79, &encoded);
        अगर (n < 0)
                वापस n;

        वापस write_string_file_at(dir_fd, fn, encoded, flags);
}

पूर्णांक read_one_पंक्ति_file_at(पूर्णांक dir_fd, स्थिर अक्षर *filename, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(filename);
        निश्चित(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        अगर (r < 0)
                वापस r;

        वापस read_पंक्ति(f, LONG_पंक्ति_MAX, ret);
}

पूर्णांक read_boolean_file_at(पूर्णांक dir_fd, स्थिर अक्षर *filename) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(filename);

        r = read_one_पंक्ति_file_at(dir_fd, filename, &s);
        अगर (r < 0)
                वापस r;

        वापस parse_boolean(s);
}

पूर्णांक verify_file_at(पूर्णांक dir_fd, स्थिर अक्षर *fn, स्थिर अक्षर *blob, bool accept_extra_nl) {
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_free_ अक्षर *buf = शून्य;
        माप_प्रकार l, k;
        पूर्णांक r;

        निश्चित(blob);

        l = म_माप(blob);

        अगर (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = दो_स्मृति(l + accept_extra_nl + 1);
        अगर (!buf)
                वापस -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        अगर (r < 0)
                वापस r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        त्रुटि_सं = 0;
        k = ख_पढ़ो(buf, 1, l + accept_extra_nl + 1, f);
        अगर (ख_त्रुटि(f))
                वापस errno_or_else(EIO);

        अगर (k != l && k != l + accept_extra_nl)
                वापस 0;
        अगर (स_भेद(buf, blob, l) != 0)
                वापस 0;
        अगर (k > l && buf[l] != '\n')
                वापस 0;

        वापस 1;
}

पूर्णांक read_virtual_file_at(
                पूर्णांक dir_fd,
                स्थिर अक्षर *filename,
                माप_प्रकार max_आकार,
                अक्षर **ret_contents,
                माप_प्रकार *ret_आकार) {

        _cleanup_free_ अक्षर *buf = शून्य;
        माप_प्रकार n, आकार;
        पूर्णांक n_retries;
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

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(max_आकार <= READ_VIRTUAL_BYTES_MAX || max_आकार == SIZE_MAX);

        _cleanup_close_ पूर्णांक fd = -EBADF;
        अगर (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        अन्यथा
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        अगर (fd < 0)
                वापस fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        क्रम (;;) {
                काष्ठा stat st;

                अगर (ख_स्थिति(fd, &st) < 0)
                        वापस -त्रुटि_सं;

                अगर (!S_ISREG(st.st_mode))
                        वापस -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                अगर (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        अगर (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                अगर (max_आकार == SIZE_MAX)
                                        वापस -EFBIG;

                                आकार = max_आकार;
                        } अन्यथा {
                                आकार = MIN((माप_प्रकार) st.st_size, max_आकार);

                                अगर (आकार > READ_VIRTUAL_BYTES_MAX)
                                        वापस -EFBIG;
                        }

                        n_retries--;
                } अन्यथा अगर (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        आकार = MIN3(page_आकार() - 1, READ_VIRTUAL_BYTES_MAX, max_आकार);
                        n_retries = 1;
                } अन्यथा {
                        आकार = MIN(READ_VIRTUAL_BYTES_MAX, max_आकार);
                        n_retries = 0;
                }

                buf = दो_स्मृति(आकार + 1);
                अगर (!buf)
                        वापस -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                आकार = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_आकार, READ_VIRTUAL_BYTES_MAX);

                क्रम (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = पढ़ो(fd, buf, आकार + 1);
                        अगर (k >= 0) {
                                n = k;
                                अवरोध;
                        }

                        अगर (त्रुटि_सं != EINTR)
                                वापस -त्रुटि_सं;
                }

                /* Consider a short read as EOF */
                अगर (n <= आकार)
                        अवरोध;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                अगर (max_आकार != SIZE_MAX && n > max_आकार) {
                        n = आकार; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        अवरोध;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                अगर (n_retries <= 0)
                        वापस -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                अगर (lseek(fd, 0, शुरू_से) < 0)
                        वापस -त्रुटि_सं;

                buf = mfree(buf);
        }

        अगर (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                अगर (!ret_आकार && स_प्रथम(buf, 0, n))
                        वापस -EBADMSG;

                अगर (n < आकार) {
                        अक्षर *p;

                        /* Return rest of the buffer to libc */
                        p = पुनः_स्मृति(buf, n + 1);
                        अगर (!p)
                                वापस -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        अगर (ret_आकार)
                *ret_आकार = n;

        वापस !truncated;
}

पूर्णांक read_full_stream_full(
                खाता *f,
                स्थिर अक्षर *filename,
                uint64_t offset,
                माप_प्रकार आकार,
                ReadFullFileFlags flags,
                अक्षर **ret_contents,
                माप_प्रकार *ret_आकार) {

        _cleanup_free_ अक्षर *buf = शून्य;
        माप_प्रकार n, n_अग्र = 0, l, expected_decoded_आकार = आकार;
        पूर्णांक fd, r;

        निश्चित(f);
        निश्चित(ret_contents);
        निश्चित(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        निश्चित(आकार != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        अगर (offset != UINT64_MAX && offset > दीर्घ_उच्च) /* fseek() can only deal with "long" offsets */
                वापस -दुस्फल;

        अगर ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                अगर (आकार <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        आकार *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                अन्यथा
                        आकार = SIZE_MAX;
        }

        fd = fileno(f);
        अगर (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                काष्ठा stat st;

                अगर (ख_स्थिति(fd, &st) < 0)
                        वापस -त्रुटि_सं;

                अगर (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        अगर (r < 0)
                                वापस r;
                }

                अगर (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        अगर (st.st_size > 0 &&
                            (आकार == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                अगर (rsize < SIZE_MAX) /* overflow check */
                                        n_अग्र = rsize + 1;
                        }

                        अगर (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (व्योम) warn_file_is_world_accessible(filename, &st, शून्य, 0);
                }
        } अन्यथा अगर (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                वापस -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        अगर (n_अग्र == 0)
                n_अग्र = आकार != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? आकार : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        अगर (n_अग्र > READ_FULL_BYTES_MAX)
                n_अग्र = READ_FULL_BYTES_MAX + 1;

        अगर (offset != UINT64_MAX && ख_जाओ(f, offset, शुरू_से) < 0)
                वापस -त्रुटि_सं;

        n = l = 0;
        क्रम (;;) {
                अक्षर *t;
                माप_प्रकार k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                अगर (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_अग्र > आकार)
                        n_अग्र = आकार + 1;

                अगर (flags & READ_FULL_FILE_SECURE) {
                        t = दो_स्मृति(n_अग्र + 1);
                        अगर (!t) {
                                r = -ENOMEM;
                                जाओ finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        मुक्त(buf);
                } अन्यथा {
                        t = पुनः_स्मृति(buf, n_अग्र + 1);
                        अगर (!t)
                                वापस -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = आकार == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_अग्र;

                त्रुटि_सं = 0;
                k = ख_पढ़ो(buf + l, 1, n - l, f);

                निश्चित(k <= n - l);
                l += k;

                अगर (ख_त्रुटि(f)) {
                        r = errno_or_else(EIO);
                        जाओ finalize;
                }
                अगर (ख_पूर्ण(f))
                        अवरोध;

                अगर (आकार != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        निश्चित(l == आकार);
                        अवरोध;
                }

                निश्चित(k > 0); /* we can't have read zero bytes because that would have been EOF */

                अगर (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > आकार) {
                        r = -E2BIG;
                        जाओ finalize;
                }

                अगर (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        जाओ finalize;
                }

                n_अग्र = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        अगर (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ व्योम *decoded = शून्य;
                माप_प्रकार decoded_आकार;

                buf[l++] = 0;
                अगर (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_आकार);
                अन्यथा
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_आकार);
                अगर (r < 0)
                        जाओ finalize;

                अगर (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_आकार;

                अगर (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_आकार) {
                        r = -E2BIG;
                        जाओ finalize;
                }
        }

        अगर (!ret_आकार) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                अगर (स_प्रथम(buf, 0, l)) {
                        r = -EBADMSG;
                        जाओ finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        अगर (ret_आकार)
                *ret_आकार = l;

        वापस 0;

finalize:
        अगर (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        वापस r;
}

पूर्णांक read_full_file_full(
                पूर्णांक dir_fd,
                स्थिर अक्षर *filename,
                uint64_t offset,
                माप_प्रकार आकार,
                ReadFullFileFlags flags,
                स्थिर अक्षर *bind_name,
                अक्षर **ret_contents,
                माप_प्रकार *ret_आकार) {

        _cleanup_fclose_ खाता *f = शून्य;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        पूर्णांक r;

        निश्चित(ret_contents);

        अगर (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        अगर (r < 0)
                वापस r;

        वापस read_full_stream_full(f, filename, offset, आकार, flags, ret_contents, ret_आकार);
}

पूर्णांक script_get_shebang_interpreter(स्थिर अक्षर *path, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक r;

        निश्चित(path);

        f = ख_खोलो(path, "re");
        अगर (!f)
                वापस -त्रुटि_सं;

        अक्षर c;
        r = safe_fgetc(f, &c);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EBADMSG;
        अगर (c != '#')
                वापस -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EBADMSG;
        अगर (c != '!')
                वापस -EMEDIUMTYPE;

        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
        अगर (r < 0)
                वापस r;

        _cleanup_free_ अक्षर *p = शून्य;
        स्थिर अक्षर *s = पंक्ति;

        r = extract_first_word(&s, &p, /* separators= */ शून्य, /* flags= */ 0);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -ENOEXEC;

        अगर (ret)
                *ret = TAKE_PTR(p);
        वापस 0;
}

पूर्णांक get_proc_field(स्थिर अक्षर *path, स्थिर अक्षर *कुंजी, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        निश्चित(path);
        निश्चित(कुंजी);

        r = fopen_unlocked(path, "re", &f);
        अगर (r == -ENOENT && proc_mounted() == 0)
                वापस -ENOSYS;
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                 _cleanup_free_ अक्षर *पंक्ति = शून्य;

                 r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                 अगर (r < 0)
                         वापस r;
                 अगर (r == 0)
                         वापस -ENODATA;

                 अक्षर *l = startswith(पंक्ति, कुंजी);
                 अगर (l && *l == ':') {
                         अगर (ret) {
                                 अक्षर *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 अगर (!s)
                                         वापस -ENOMEM;

                                 *ret = s;
                         }

                         वापस 0;
                 }
        }
}

सूची* xopendirat(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक flags) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(!(flags & (O_CREAT|O_TMPFILE)));

        अगर ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                वापस सूची_खोलो(path);

        अगर (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        अगर (fd < 0)
                वापस शून्य;

        वापस take_fdopendir(&fd);
}

पूर्णांक fopen_mode_to_flags(स्थिर अक्षर *mode) {
        स्थिर अक्षर *p;
        पूर्णांक flags;

        निश्चित(mode);

        अगर ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        अन्यथा अगर ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        अन्यथा अगर ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        अन्यथा अगर ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        अन्यथा अगर ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        अन्यथा अगर ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        अन्यथा
                वापस -EINVAL;

        क्रम (; *p != 0; p++) {

                चयन (*p) {

                हाल 'e':
                        flags |= O_CLOEXEC;
                        अवरोध;

                हाल 'x':
                        flags |= O_EXCL;
                        अवरोध;

                हाल 'm':
                        /* ignore this here, fdopen() might care later though */
                        अवरोध;

                हाल 'c': /* not sure what to do about this one */
                शेष:
                        वापस -EINVAL;
                }
        }

        वापस flags;
}

अटल पूर्णांक xfopenat_regular(पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *mode, पूर्णांक open_flags, खाता **ret) {
        खाता *f;

        /* A combination of fopen() with openat() */

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(mode);
        निश्चित(ret);

        अगर (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = ख_खोलो(path, mode);
        अन्यथा अगर (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ अक्षर *j = strjoin("/", path);
                अगर (!j)
                        वापस -ENOMEM;

                f = ख_खोलो(j, mode);
        } अन्यथा {
                _cleanup_close_ पूर्णांक fd = -EBADF;
                पूर्णांक mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                अगर (mode_flags < 0)
                        वापस mode_flags;

                अगर (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        अगर (fd < 0)
                                वापस -त्रुटि_सं;
                } अन्यथा {
                        अगर (dir_fd == AT_FDCWD)
                                वापस -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        अगर (fd < 0)
                                वापस fd;
                }

                f = take_fdopen(&fd, mode);
        }
        अगर (!f)
                वापस -त्रुटि_सं;

        *ret = f;
        वापस 0;
}

अटल पूर्णांक xfopenat_unix_socket(पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *bind_name, खाता **ret) {
        _cleanup_close_ पूर्णांक sk = -EBADF;
        खाता *f;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        अगर (sk < 0)
                वापस -त्रुटि_सं;

        अगर (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                जोड़ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                अगर (r < 0)
                        वापस r;

                अगर (bind(sk, &bsa.sa, r) < 0)
                        वापस -त्रुटि_सं;
        }

        r = connect_unix_path(sk, dir_fd, path);
        अगर (r < 0)
                वापस r;

        अगर (shutdown(sk, SHUT_WR) < 0)
                वापस -त्रुटि_सं;

        f = take_fdopen(&sk, "r");
        अगर (!f)
                वापस -त्रुटि_सं;

        *ret = f;
        वापस 0;
}

पूर्णांक xfopenat_full(
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                स्थिर अक्षर *mode,
                पूर्णांक open_flags,
                XfopenFlags flags,
                स्थिर अक्षर *bind_name,
                खाता **ret) {

        खाता *f = शून्य;  /* avoid false maybe-uninitialized warning */
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(mode);
        निश्चित(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        अगर (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                अगर (IN_SET(r, -ENOTSOCK, -EINVAL))
                        वापस -ENXIO; /* propagate original error if this is not a socket after all */
        }
        अगर (r < 0)
                वापस r;

        अगर (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (व्योम) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        वापस 0;
}

पूर्णांक fdopen_independent(पूर्णांक fd, स्थिर अक्षर *mode, खाता **ret) {
        _cleanup_close_ पूर्णांक copy_fd = -EBADF;
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक mode_flags;

        निश्चित(fd >= 0);
        निश्चित(mode);
        निश्चित(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        अगर (mode_flags < 0)
                वापस mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        अगर (copy_fd < 0)
                वापस copy_fd;

        f = take_fdopen(&copy_fd, mode);
        अगर (!f)
                वापस -त्रुटि_सं;

        *ret = TAKE_PTR(f);
        वापस 0;
}

अटल पूर्णांक खोज_and_open_internal(
                स्थिर अक्षर *path,
                पूर्णांक mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                स्थिर अक्षर *मूल,
                अक्षर **खोज,
                पूर्णांक *ret_fd,
                अक्षर **ret_path) {

        पूर्णांक r;

        निश्चित(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        निश्चित(path);

        अगर (path_is_absolute(path)) {
                _cleanup_close_ पूर्णांक fd = -EBADF;

                अगर (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(खोलो(path, mode, 0777));
                अन्यथा
                        r = RET_NERRNO(access(path, mode));
                अगर (r < 0)
                        वापस r;

                अगर (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        अगर (r < 0)
                                वापस r;
                }

                अगर (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                वापस 0;
        }

        अगर (!path_strv_resolve_uniq(खोज, मूल))
                वापस -ENOMEM;

        STRV_FOREACH(i, खोज) {
                _cleanup_close_ पूर्णांक fd = -EBADF;
                _cleanup_free_ अक्षर *p = शून्य;

                p = path_join(मूल, *i, path);
                अगर (!p)
                        वापस -ENOMEM;

                अगर (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(खोलो(p, mode, 0777));
                अन्यथा
                        r = RET_NERRNO(access(p, F_OK));
                अगर (r >= 0) {
                        अगर (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        अगर (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        वापस 0;
                }
                अगर (r != -ENOENT)
                        वापस r;
        }

        वापस -ENOENT;
}

पूर्णांक खोज_and_open(
                स्थिर अक्षर *path,
                पूर्णांक mode,
                स्थिर अक्षर *मूल,
                अक्षर **खोज,
                पूर्णांक *ret_fd,
                अक्षर **ret_path) {

        _cleanup_strv_free_ अक्षर **copy = शून्य;

        निश्चित(path);

        copy = strv_copy(खोज);
        अगर (!copy)
                वापस -ENOMEM;

        वापस खोज_and_open_internal(path, mode, मूल, copy, ret_fd, ret_path);
}

अटल पूर्णांक खोज_and_fopen_internal(
                स्थिर अक्षर *path,
                स्थिर अक्षर *mode,
                स्थिर अक्षर *मूल,
                अक्षर **खोज,
                खाता **ret_file,
                अक्षर **ret_path) {

        _cleanup_free_ अक्षर *found_path = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(mode || !ret_file);

        r = खोज_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        मूल,
                        खोज,
                        ret_file ? &fd : शून्य,
                        ret_path ? &found_path : शून्य);
        अगर (r < 0)
                वापस r;

        अगर (ret_file) {
                खाता *f = take_fdopen(&fd, mode);
                अगर (!f)
                        वापस -त्रुटि_सं;

                *ret_file = f;
        }

        अगर (ret_path)
                *ret_path = TAKE_PTR(found_path);

        वापस 0;
}

पूर्णांक खोज_and_fopen(
                स्थिर अक्षर *path,
                स्थिर अक्षर *mode,
                स्थिर अक्षर *मूल,
                स्थिर अक्षर **खोज,
                खाता **ret_file,
                अक्षर **ret_path) {

        _cleanup_strv_free_ अक्षर **copy = शून्य;

        निश्चित(path);
        निश्चित(mode || !ret_file);

        copy = strv_copy((अक्षर**) खोज);
        अगर (!copy)
                वापस -ENOMEM;

        वापस खोज_and_fopen_internal(path, mode, मूल, copy, ret_file, ret_path);
}

पूर्णांक खोज_and_fopen_nulstr(
                स्थिर अक्षर *path,
                स्थिर अक्षर *mode,
                स्थिर अक्षर *मूल,
                स्थिर अक्षर *खोज,
                खाता **ret_file,
                अक्षर **ret_path) {

        _cleanup_strv_free_ अक्षर **l = शून्य;

        निश्चित(path);
        निश्चित(mode || !ret_file);

        l = strv_split_nulstr(खोज);
        अगर (!l)
                वापस -ENOMEM;

        वापस खोज_and_fopen_internal(path, mode, मूल, l, ret_file, ret_path);
}

पूर्णांक fflush_and_check(खाता *f) {
        निश्चित(f);

        त्रुटि_सं = 0;
        ख_साफ(f);

        अगर (ख_त्रुटि(f))
                वापस errno_or_else(EIO);

        वापस 0;
}

पूर्णांक fflush_sync_and_check(खाता *f) {
        पूर्णांक r, fd;

        निश्चित(f);

        r = fflush_and_check(f);
        अगर (r < 0)
                वापस r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        अगर (fd < 0)
                वापस 0;

        r = fsync_full(fd);
        अगर (r < 0)
                वापस r;

        वापस 0;
}

पूर्णांक write_timestamp_file_atomic(स्थिर अक्षर *fn, usec_t n) {
        अक्षर ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        अगर (!timestamp_is_set(n))
                वापस -दुस्फल;

        xsprintf(ln, USEC_FMT "\n", n);

        वापस write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

पूर्णांक read_timestamp_file(स्थिर अक्षर *fn, usec_t *ret) {
        _cleanup_free_ अक्षर *ln = शून्य;
        uint64_t t;
        पूर्णांक r;

        निश्चित(ret);

        r = read_one_पंक्ति_file(fn, &ln);
        अगर (r < 0)
                वापस r;

        r = safe_atou64(ln, &t);
        अगर (r < 0)
                वापस r;

        अगर (!timestamp_is_set(t))
                वापस -दुस्फल;

        *ret = (usec_t) t;
        वापस 0;
}

पूर्णांक fputs_with_separator(खाता *f, स्थिर अक्षर *s, स्थिर अक्षर *separator, bool *space) {
        निश्चित(s);
        निश्चित(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        अगर (!f)
                f = मानक_निकास;

        अगर (!separator)
                separator = " ";

        अगर (*space)
                अगर (ख_माला_दो(separator, f) < 0)
                        वापस -EIO;

        *space = true;

        अगर (ख_माला_दो(s, f) < 0)
                वापस -EIO;

        वापस 0;
}

पूर्णांक fputs_with_newline(खाता *f, स्थिर अक्षर *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        अगर (isempty(s))
                वापस 0;

        अगर (!f)
                f = मानक_निकास;

        अगर (ख_माला_दो(s, f) < 0)
                वापस -EIO;

        अगर (endswith(s, "\n"))
                वापस 0;

        अगर (ख_अक्षर_दो('\n', f) < 0)
                वापस -EIO;

        वापस 1;
}

/* A bitmask of the EOL markers we know */
प्रकार क्रमागत End_Of_पंक्ति_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_पंक्ति_Marker;

अटल End_Of_पंक्ति_Marker categorize_eol(अक्षर c, Read_पंक्ति_Flags flags) {

        अगर (!FLAGS_SET(flags, READ_पंक्ति_ONLY_NUL)) {
                अगर (c == '\n')
                        वापस EOL_TEN;
                अगर (c == '\r')
                        वापस EOL_THIRTEEN;
        }

        अगर (c == '\0')
                वापस EOL_ZERO;

        वापस EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(खाता*, funlockfile, शून्य);

पूर्णांक read_पंक्ति_full(खाता *f, माप_प्रकार limit, Read_पंक्ति_Flags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *बफर = शून्य;
        माप_प्रकार n = 0, गिनती = 0;
        पूर्णांक r;

        निश्चित(f);

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

        अगर (ret) {
                अगर (!GREEDY_REALLOC(बफर, 1))
                        वापस -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) खाता *flocked = f;
                End_Of_पंक्ति_Marker previous_eol = EOL_NONE;
                flockfile(f);

                क्रम (;;) {
                        End_Of_पंक्ति_Marker eol;
                        अक्षर c;

                        अगर (n >= limit)
                                वापस -ENOBUFS;

                        अगर (गिनती >= पूर्णांक_उच्च) /* We couldn't return the counter anymore as "int", hence refuse this */
                                वापस -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0) /* EOF is definitely EOL */
                                अवरोध;

                        eol = categorize_eol(c, flags);

                        अगर (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(अक्षर_वापस((अचिन्हित अक्षर) c, f) != खातापूर्ण);
                                अवरोध;
                        }

                        गिनती++;

                        अगर (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                अगर ((flags & (READ_पंक्ति_IS_A_TTY|READ_पंक्ति_NOT_A_TTY)) == 0) {
                                        पूर्णांक fd;

                                        fd = fileno(f);
                                        अगर (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_पंक्ति_NOT_A_TTY;
                                        अन्यथा
                                                flags |= isatty_safe(fd) ? READ_पंक्ति_IS_A_TTY : READ_पंक्ति_NOT_A_TTY;
                                }
                                अगर (FLAGS_SET(flags, READ_पंक्ति_IS_A_TTY))
                                        अवरोध;
                        }

                        अगर (eol != EOL_NONE) {
                                previous_eol |= eol;
                                जारी;
                        }

                        अगर (ret) {
                                अगर (!GREEDY_REALLOC(बफर, n + 2))
                                        वापस -ENOMEM;

                                बफर[n] = c;
                        }

                        n++;
                }
        }

        अगर (ret) {
                बफर[n] = 0;

                *ret = TAKE_PTR(बफर);
        }

        वापस (पूर्णांक) गिनती;
}

पूर्णांक read_stripped_पंक्ति(खाता *f, माप_प्रकार limit, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r, k;

        निश्चित(f);

        r = read_पंक्ति(f, limit, ret ? &s : शून्य);
        अगर (r < 0)
                वापस r;

        अगर (ret) {
                स्थिर अक्षर *p = strstrip(s);
                अगर (p == s)
                        *ret = TAKE_PTR(s);
                अन्यथा {
                        k = strdup_to(ret, p);
                        अगर (k < 0)
                                वापस k;
                }
        }

        वापस r > 0;          /* Return 1 if something was read. */
}

पूर्णांक safe_fgetc(खाता *f, अक्षर *ret) {
        पूर्णांक k;

        निश्चित(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        त्रुटि_सं = 0;
        k = ख_अक्षर_लो(f);
        अगर (k == खातापूर्ण) {
                अगर (ख_त्रुटि(f))
                        वापस errno_or_else(EIO);

                अगर (ret)
                        *ret = 0;

                वापस 0;
        }

        अगर (ret)
                *ret = k;

        वापस 1;
}

पूर्णांक warn_file_is_world_accessible(स्थिर अक्षर *filename, काष्ठा stat *st, स्थिर अक्षर *unit, अचिन्हित पंक्ति) {
        काष्ठा stat _st;

        अगर (!filename)
                वापस 0;

        अगर (!st) {
                अगर (stat(filename, &_st) < 0)
                        वापस -त्रुटि_सं;
                st = &_st;
        }

        अगर ((st->st_mode & S_IRWXO) == 0)
                वापस 0;

        अगर (unit)
                log_syntax(unit, LOG_WARNING, filename, पंक्ति, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        अन्यथा
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        वापस 0;
}

पूर्णांक write_तथ्य_file_atomic_at(
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                स्थिर काष्ठा iovec *iovec,
                Write_तथ्य_File_Flags flags) {

        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ अक्षर *dn = शून्य, *fn = शून्य;
        r = path_split_prefix_filename(path, &dn, &fn);
        अगर (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                वापस -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        अगर (r < 0)
                वापस r;

        _cleanup_close_ पूर्णांक mfd = -EBADF;
        अगर (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_तथ्य_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ शून्य,
                            &mfd);
                अगर (r < 0)
                        वापस r;

                dir_fd = mfd;
        }

        _cleanup_free_ अक्षर *t = शून्य;
        _cleanup_close_ पूर्णांक fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        अगर (fd < 0)
                वापस fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        अगर (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                अगर (r < 0)
                        वापस r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_तथ्य_FILE_MODE_0400) ? 0400 : 0644);
        अगर (r < 0)
                वापस r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        अगर (r < 0)
                वापस r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        वापस 0;
}
