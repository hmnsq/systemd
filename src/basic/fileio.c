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

पूर्णांक fdopen_unlocked(पूर्णांक fd, स्थिरांक अक्षर *options, खाता **ret) {
        प्रतिज्ञा(ret);

        खाता *f = fdopen(fd, options);
        जर (!f)
                परत -त्रुटि_सं;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        परत 0;
}

पूर्णांक take_fdopen_unlocked(पूर्णांक *fd, स्थिरांक अक्षर *options, खाता **ret) {
        पूर्णांक r;

        प्रतिज्ञा(fd);

        r = fdopen_unlocked(*fd, options, ret);
        जर (r < 0)
                परत r;

        *fd = -EBADF;

        परत 0;
}

खाता* take_fdopen(पूर्णांक *fd, स्थिरांक अक्षर *options) {
        प्रतिज्ञा(fd);

        खाता *f = fdopen(*fd, options);
        जर (!f)
                परत रिक्त;

        *fd = -EBADF;

        परत f;
}

सूची* take_fdopendir(पूर्णांक *dfd) {
        प्रतिज्ञा(dfd);

        सूची *d = fdopendir(*dfd);
        जर (!d)
                परत रिक्त;

        *dfd = -EBADF;

        परत d;
}

खाता* open_memstream_unlocked(अक्षर **ptr, माप_प्रकार *sizeloc) {
        खाता *f = open_memstream(ptr, sizeloc);
        जर (!f)
                परत रिक्त;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        परत f;
}

खाता* fmemopen_unlocked(शून्य *buf, माप_प्रकार size, स्थिरांक अक्षर *mode) {
        खाता *f = fmemopen(buf, size, mode);
        जर (!f)
                परत रिक्त;

        (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        परत f;
}

पूर्णांक write_string_stream_full(
                खाता *f,
                स्थिरांक अक्षर *ओळ,
                WriteStringFileFlags flags,
                स्थिरांक संरचना timespec *ts) {

        bool needs_nl;
        पूर्णांक r, fd = -EBADF;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ओळ);

        जर (ख_त्रुटि(f))
                परत -EIO;

        जर (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                जर (fd < 0)
                        परत -EBADF;
        }

        जर (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ अक्षर *t = रिक्त;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                जर (fd < 0) {
                        fd = fileno(f);
                        जर (fd < 0)
                                परत -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                जर (read_virtual_file_fd(fd, म_माप(ओळ)+1, &t, रिक्त) > 0 &&
                    streq_skip_trailing_chars(ओळ, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", ओळ);
                        परत 0;
                }

                जर (lseek(fd, 0, शुरू_से) < 0)
                        परत -त्रुटि_सं;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(ओळ, "\n");

        जर (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                ओळ = strjoina(ओळ, "\n");
                needs_nl = false;
        }

        जर (ख_माला_दो(ओळ, f) == खातापूर्ण)
                परत -त्रुटि_सं;

        जर (needs_nl)
                जर (ख_अक्षर_दो('\n', f) == खातापूर्ण)
                        परत -त्रुटि_सं;

        जर (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        अन्यथा
                r = fflush_and_check(f);
        जर (r < 0)
                परत r;

        जर (ts) {
                स्थिरांक संरचना timespec twice[2] = {*ts, *ts};

                प्रतिज्ञा(fd >= 0);
                जर (futimens(fd, twice) < 0)
                        परत -त्रुटि_सं;
        }

        परत 0;
}

स्थिर mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        परत FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

स्थिर पूर्णांक write_string_file_atomic_at(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *fn,
                स्थिरांक अक्षर *ओळ,
                WriteStringFileFlags flags,
                स्थिरांक संरचना timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_free_ अक्षर *p = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(fn);
        प्रतिज्ञा(ओळ);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        जर (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                जर (r < 0)
                        परत r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        पूर्णांक k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? रिक्त : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        जर (r < 0)
                परत r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        जर (k < 0)
                परत k;

        r = write_string_stream_full(f, ओळ, flags, ts);
        जर (r < 0)
                परत r;

        r = fchmod_umask(fileno(f), mode);
        जर (r < 0)
                परत r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        जर (r < 0)
                परत r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        जर (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

पूर्णांक write_string_file_full_label(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *fn,
                स्थिरांक अक्षर *ओळ,
                WriteStringFileFlags flags,
                स्थिरांक संरचना timespec *ts,
                स्थिरांक अक्षर *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(dir_fd == AT_FDCWD || dir_fd >= 0);
        प्रतिज्ञा(ओळ);

        /* We don't know how to verify whether the file contents was already on-disk. */
        प्रतिज्ञा(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        जर (flags & WRITE_STRING_FILE_MKDIR_0755) {
                प्रतिज्ञा(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                जर (r < 0)
                        परत r;
        }

        जर (flags & WRITE_STRING_FILE_ATOMIC) {
                प्रतिज्ञा(fn);
                प्रतिज्ञा(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, ओळ, flags, ts, label_context);
                जर (r < 0)
                        जा fail;

                परत r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        जर (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        अन्यथा {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                जर (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        जर (r < 0)
                                जा fail;

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
                जर (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? रिक्त : fn, made_file, label_context));
        }
        जर (r < 0)
                जा fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        जर (r < 0)
                जा fail;

        जर (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                रखो_भबफ(f, रिक्त, _IONBF, 0);

        r = write_string_stream_full(f, ओळ, flags, ts);
        जर (r < 0)
                जा fail;

        परत 0;

fail:
        जर (made_file)
                (शून्य) unlinkat(dir_fd, fn, 0);

        जर (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                परत r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        जर (verify_file_at(dir_fd, fn, ओळ, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                परत 0;

        परत r;
}

पूर्णांक write_string_filef(
                स्थिरांक अक्षर *fn,
                WriteStringFileFlags flags,
                स्थिरांक अक्षर *format, ...) {

        _cleanup_free_ अक्षर *p = रिक्त;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&p, format, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत -ENOMEM;

        परत write_string_file(fn, p, flags);
}

पूर्णांक write_string_filef_at(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *fn,
                WriteStringFileFlags flags,
                स्थिरांक अक्षर *format, ...) {

        _cleanup_free_ अक्षर *p = रिक्त;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&p, format, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत -ENOMEM;

        परत write_string_file_at(dir_fd, fn, p, flags);
}

पूर्णांक write_base64_file_at(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *fn,
                स्थिरांक संरचना iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ अक्षर *encoded = रिक्त;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : रिक्त, data ? data->iov_len : 0, 79, &encoded);
        जर (n < 0)
                परत n;

        परत write_string_file_at(dir_fd, fn, encoded, flags);
}

पूर्णांक read_one_ओळ_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *filename, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(filename);
        प्रतिज्ञा(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        जर (r < 0)
                परत r;

        परत read_ओळ(f, LONG_ओळ_MAX, ret);
}

पूर्णांक read_boolean_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *filename) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(filename);

        r = read_one_ओळ_file_at(dir_fd, filename, &s);
        जर (r < 0)
                परत r;

        परत parse_boolean(s);
}

पूर्णांक verify_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, स्थिरांक अक्षर *blob, bool accept_extra_nl) {
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_free_ अक्षर *buf = रिक्त;
        माप_प्रकार l, k;
        पूर्णांक r;

        प्रतिज्ञा(blob);

        l = म_माप(blob);

        जर (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = दो_स्मृति(l + accept_extra_nl + 1);
        जर (!buf)
                परत -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        जर (r < 0)
                परत r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        त्रुटि_सं = 0;
        k = ख_पढ़ो(buf, 1, l + accept_extra_nl + 1, f);
        जर (ख_त्रुटि(f))
                परत errno_or_else(EIO);

        जर (k != l && k != l + accept_extra_nl)
                परत 0;
        जर (स_भेद(buf, blob, l) != 0)
                परत 0;
        जर (k > l && buf[l] != '\n')
                परत 0;

        परत 1;
}

पूर्णांक read_virtual_file_at(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *filename,
                माप_प्रकार max_size,
                अक्षर **ret_contents,
                माप_प्रकार *ret_size) {

        _cleanup_free_ अक्षर *buf = रिक्त;
        माप_प्रकार n, size;
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

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ पूर्णांक fd = -EBADF;
        जर (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        अन्यथा
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        जर (fd < 0)
                परत fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        साठी (;;) {
                संरचना stat st;

                जर (ख_स्थिति(fd, &st) < 0)
                        परत -त्रुटि_सं;

                जर (!S_ISREG(st.st_mode))
                        परत -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                जर (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        जर (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                जर (max_size == SIZE_MAX)
                                        परत -EFBIG;

                                size = max_size;
                        } अन्यथा {
                                size = MIN((माप_प्रकार) st.st_size, max_size);

                                जर (size > READ_VIRTUAL_BYTES_MAX)
                                        परत -EFBIG;
                        }

                        n_retries--;
                } अन्यथा जर (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } अन्यथा {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = दो_स्मृति(size + 1);
                जर (!buf)
                        परत -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                साठी (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = पठ(fd, buf, size + 1);
                        जर (k >= 0) {
                                n = k;
                                थांबा;
                        }

                        जर (त्रुटि_सं != EINTR)
                                परत -त्रुटि_सं;
                }

                /* Consider a short read as EOF */
                जर (n <= size)
                        थांबा;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                जर (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        थांबा;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                जर (n_retries <= 0)
                        परत -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                जर (lseek(fd, 0, शुरू_से) < 0)
                        परत -त्रुटि_सं;

                buf = mfree(buf);
        }

        जर (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                जर (!ret_size && स_प्रथम(buf, 0, n))
                        परत -EBADMSG;

                जर (n < size) {
                        अक्षर *p;

                        /* Return rest of the buffer to libc */
                        p = पुनः_स्मृति(buf, n + 1);
                        जर (!p)
                                परत -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        जर (ret_size)
                *ret_size = n;

        परत !truncated;
}

पूर्णांक read_full_stream_full(
                खाता *f,
                स्थिरांक अक्षर *filename,
                uint64_t offset,
                माप_प्रकार size,
                ReadFullFileFlags flags,
                अक्षर **ret_contents,
                माप_प्रकार *ret_size) {

        _cleanup_free_ अक्षर *buf = रिक्त;
        माप_प्रकार n, n_next = 0, l, expected_decoded_size = size;
        पूर्णांक fd, r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret_contents);
        प्रतिज्ञा(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        प्रतिज्ञा(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        जर (offset != UINT64_MAX && offset > दीर्घ_उच्च) /* fseek() can only deal with "long" offsets */
                परत -दुस्फल;

        जर ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                जर (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                अन्यथा
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        जर (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                संरचना stat st;

                जर (ख_स्थिति(fd, &st) < 0)
                        परत -त्रुटि_सं;

                जर (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        जर (r < 0)
                                परत r;
                }

                जर (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        जर (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                जर (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        जर (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (शून्य) warn_file_is_world_accessible(filename, &st, रिक्त, 0);
                }
        } अन्यथा जर (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                परत -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        जर (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        जर (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        जर (offset != UINT64_MAX && ख_जाओ(f, offset, शुरू_से) < 0)
                परत -त्रुटि_सं;

        n = l = 0;
        साठी (;;) {
                अक्षर *t;
                माप_प्रकार k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                जर (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                जर (flags & READ_FULL_FILE_SECURE) {
                        t = दो_स्मृति(n_next + 1);
                        जर (!t) {
                                r = -ENOMEM;
                                जा finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        मुक्त(buf);
                } अन्यथा {
                        t = पुनः_स्मृति(buf, n_next + 1);
                        जर (!t)
                                परत -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                त्रुटि_सं = 0;
                k = ख_पढ़ो(buf + l, 1, n - l, f);

                प्रतिज्ञा(k <= n - l);
                l += k;

                जर (ख_त्रुटि(f)) {
                        r = errno_or_else(EIO);
                        जा finalize;
                }
                जर (ख_पूर्ण(f))
                        थांबा;

                जर (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        प्रतिज्ञा(l == size);
                        थांबा;
                }

                प्रतिज्ञा(k > 0); /* we can't have read zero bytes because that would have been EOF */

                जर (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        जा finalize;
                }

                जर (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        जा finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        जर (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ शून्य *decoded = रिक्त;
                माप_प्रकार decoded_size;

                buf[l++] = 0;
                जर (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                अन्यथा
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                जर (r < 0)
                        जा finalize;

                जर (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                जर (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        जा finalize;
                }
        }

        जर (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                जर (स_प्रथम(buf, 0, l)) {
                        r = -EBADMSG;
                        जा finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        जर (ret_size)
                *ret_size = l;

        परत 0;

finalize:
        जर (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        परत r;
}

पूर्णांक read_full_file_full(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *filename,
                uint64_t offset,
                माप_प्रकार size,
                ReadFullFileFlags flags,
                स्थिरांक अक्षर *bind_name,
                अक्षर **ret_contents,
                माप_प्रकार *ret_size) {

        _cleanup_fclose_ खाता *f = रिक्त;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        पूर्णांक r;

        प्रतिज्ञा(ret_contents);

        जर (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        जर (r < 0)
                परत r;

        परत read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

पूर्णांक script_get_shebang_interpreter(स्थिरांक अक्षर *path, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);

        f = ख_खोलो(path, "re");
        जर (!f)
                परत -त्रुटि_सं;

        अक्षर c;
        r = safe_fgetc(f, &c);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EBADMSG;
        जर (c != '#')
                परत -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EBADMSG;
        जर (c != '!')
                परत -EMEDIUMTYPE;

        _cleanup_free_ अक्षर *ओळ = रिक्त;
        r = read_ओळ(f, LONG_ओळ_MAX, &ओळ);
        जर (r < 0)
                परत r;

        _cleanup_free_ अक्षर *p = रिक्त;
        स्थिरांक अक्षर *s = ओळ;

        r = extract_first_word(&s, &p, /* separators= */ रिक्त, /* flags= */ 0);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -ENOEXEC;

        जर (ret)
                *ret = TAKE_PTR(p);
        परत 0;
}

पूर्णांक get_proc_field(स्थिरांक अक्षर *path, स्थिरांक अक्षर *किल्ली, अक्षर **ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णांक r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        प्रतिज्ञा(path);
        प्रतिज्ञा(किल्ली);

        r = fopen_unlocked(path, "re", &f);
        जर (r == -ENOENT && proc_mounted() == 0)
                परत -ENOSYS;
        जर (r < 0)
                परत r;

        साठी (;;) {
                 _cleanup_free_ अक्षर *ओळ = रिक्त;

                 r = read_ओळ(f, LONG_ओळ_MAX, &ओळ);
                 जर (r < 0)
                         परत r;
                 जर (r == 0)
                         परत -ENODATA;

                 अक्षर *l = startswith(ओळ, किल्ली);
                 जर (l && *l == ':') {
                         जर (ret) {
                                 अक्षर *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 जर (!s)
                                         परत -ENOMEM;

                                 *ret = s;
                         }

                         परत 0;
                 }
        }
}

सूची* xopendirat(पूर्णांक dir_fd, स्थिरांक अक्षर *path, पूर्णांक flags) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(!(flags & (O_CREAT|O_TMPFILE)));

        जर ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                परत सूची_खोलो(path);

        जर (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        जर (fd < 0)
                परत रिक्त;

        परत take_fdopendir(&fd);
}

पूर्णांक fopen_mode_to_flags(स्थिरांक अक्षर *mode) {
        स्थिरांक अक्षर *p;
        पूर्णांक flags;

        प्रतिज्ञा(mode);

        जर ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        अन्यथा जर ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        अन्यथा जर ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        अन्यथा जर ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        अन्यथा जर ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        अन्यथा जर ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        अन्यथा
                परत -EINVAL;

        साठी (; *p != 0; p++) {

                निवड (*p) {

                स्थिती 'e':
                        flags |= O_CLOEXEC;
                        थांबा;

                स्थिती 'x':
                        flags |= O_EXCL;
                        थांबा;

                स्थिती 'm':
                        /* ignore this here, fdopen() might care later though */
                        थांबा;

                स्थिती 'c': /* not sure what to do about this one */
                पूर्वनिर्धारित:
                        परत -EINVAL;
                }
        }

        परत flags;
}

स्थिर पूर्णांक xfopenat_regular(पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *mode, पूर्णांक open_flags, खाता **ret) {
        खाता *f;

        /* A combination of fopen() with openat() */

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(mode);
        प्रतिज्ञा(ret);

        जर (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = ख_खोलो(path, mode);
        अन्यथा जर (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ अक्षर *j = strjoin("/", path);
                जर (!j)
                        परत -ENOMEM;

                f = ख_खोलो(j, mode);
        } अन्यथा {
                _cleanup_close_ पूर्णांक fd = -EBADF;
                पूर्णांक mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                जर (mode_flags < 0)
                        परत mode_flags;

                जर (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        जर (fd < 0)
                                परत -त्रुटि_सं;
                } अन्यथा {
                        जर (dir_fd == AT_FDCWD)
                                परत -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        जर (fd < 0)
                                परत fd;
                }

                f = take_fdopen(&fd, mode);
        }
        जर (!f)
                परत -त्रुटि_सं;

        *ret = f;
        परत 0;
}

स्थिर पूर्णांक xfopenat_unix_socket(पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *bind_name, खाता **ret) {
        _cleanup_close_ पूर्णांक sk = -EBADF;
        खाता *f;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        जर (sk < 0)
                परत -त्रुटि_सं;

        जर (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                संघ sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                जर (r < 0)
                        परत r;

                जर (bind(sk, &bsa.sa, r) < 0)
                        परत -त्रुटि_सं;
        }

        r = connect_unix_path(sk, dir_fd, path);
        जर (r < 0)
                परत r;

        जर (shutdown(sk, SHUT_WR) < 0)
                परत -त्रुटि_सं;

        f = take_fdopen(&sk, "r");
        जर (!f)
                परत -त्रुटि_सं;

        *ret = f;
        परत 0;
}

पूर्णांक xfopenat_full(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *mode,
                पूर्णांक open_flags,
                XfopenFlags flags,
                स्थिरांक अक्षर *bind_name,
                खाता **ret) {

        खाता *f = रिक्त;  /* avoid false maybe-uninitialized warning */
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(mode);
        प्रतिज्ञा(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        जर (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                जर (IN_SET(r, -ENOTSOCK, -EINVAL))
                        परत -ENXIO; /* propagate original error if this is not a socket after all */
        }
        जर (r < 0)
                परत r;

        जर (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (शून्य) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        परत 0;
}

पूर्णांक fdopen_independent(पूर्णांक fd, स्थिरांक अक्षर *mode, खाता **ret) {
        _cleanup_close_ पूर्णांक copy_fd = -EBADF;
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णांक mode_flags;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(mode);
        प्रतिज्ञा(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        जर (mode_flags < 0)
                परत mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        जर (copy_fd < 0)
                परत copy_fd;

        f = take_fdopen(&copy_fd, mode);
        जर (!f)
                परत -त्रुटि_सं;

        *ret = TAKE_PTR(f);
        परत 0;
}

स्थिर पूर्णांक शोध_and_open_internal(
                स्थिरांक अक्षर *path,
                पूर्णांक mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                स्थिरांक अक्षर *मूळ,
                अक्षर **शोध,
                पूर्णांक *ret_fd,
                अक्षर **ret_path) {

        पूर्णांक r;

        प्रतिज्ञा(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        प्रतिज्ञा(path);

        जर (path_is_absolute(path)) {
                _cleanup_close_ पूर्णांक fd = -EBADF;

                जर (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(खोलो(path, mode, 0777));
                अन्यथा
                        r = RET_NERRNO(access(path, mode));
                जर (r < 0)
                        परत r;

                जर (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        जर (r < 0)
                                परत r;
                }

                जर (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                परत 0;
        }

        जर (!path_strv_resolve_uniq(शोध, मूळ))
                परत -ENOMEM;

        STRV_FOREACH(i, शोध) {
                _cleanup_close_ पूर्णांक fd = -EBADF;
                _cleanup_free_ अक्षर *p = रिक्त;

                p = path_join(मूळ, *i, path);
                जर (!p)
                        परत -ENOMEM;

                जर (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(खोलो(p, mode, 0777));
                अन्यथा
                        r = RET_NERRNO(access(p, F_OK));
                जर (r >= 0) {
                        जर (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        जर (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        परत 0;
                }
                जर (r != -ENOENT)
                        परत r;
        }

        परत -ENOENT;
}

पूर्णांक शोध_and_open(
                स्थिरांक अक्षर *path,
                पूर्णांक mode,
                स्थिरांक अक्षर *मूळ,
                अक्षर **शोध,
                पूर्णांक *ret_fd,
                अक्षर **ret_path) {

        _cleanup_strv_free_ अक्षर **copy = रिक्त;

        प्रतिज्ञा(path);

        copy = strv_copy(शोध);
        जर (!copy)
                परत -ENOMEM;

        परत शोध_and_open_internal(path, mode, मूळ, copy, ret_fd, ret_path);
}

स्थिर पूर्णांक शोध_and_fopen_internal(
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *mode,
                स्थिरांक अक्षर *मूळ,
                अक्षर **शोध,
                खाता **ret_file,
                अक्षर **ret_path) {

        _cleanup_free_ अक्षर *found_path = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode || !ret_file);

        r = शोध_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        मूळ,
                        शोध,
                        ret_file ? &fd : रिक्त,
                        ret_path ? &found_path : रिक्त);
        जर (r < 0)
                परत r;

        जर (ret_file) {
                खाता *f = take_fdopen(&fd, mode);
                जर (!f)
                        परत -त्रुटि_सं;

                *ret_file = f;
        }

        जर (ret_path)
                *ret_path = TAKE_PTR(found_path);

        परत 0;
}

पूर्णांक शोध_and_fopen(
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *mode,
                स्थिरांक अक्षर *मूळ,
                स्थिरांक अक्षर **शोध,
                खाता **ret_file,
                अक्षर **ret_path) {

        _cleanup_strv_free_ अक्षर **copy = रिक्त;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode || !ret_file);

        copy = strv_copy((अक्षर**) शोध);
        जर (!copy)
                परत -ENOMEM;

        परत शोध_and_fopen_internal(path, mode, मूळ, copy, ret_file, ret_path);
}

पूर्णांक शोध_and_fopen_nulstr(
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *mode,
                स्थिरांक अक्षर *मूळ,
                स्थिरांक अक्षर *शोध,
                खाता **ret_file,
                अक्षर **ret_path) {

        _cleanup_strv_free_ अक्षर **l = रिक्त;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode || !ret_file);

        l = strv_split_nulstr(शोध);
        जर (!l)
                परत -ENOMEM;

        परत शोध_and_fopen_internal(path, mode, मूळ, l, ret_file, ret_path);
}

पूर्णांक fflush_and_check(खाता *f) {
        प्रतिज्ञा(f);

        त्रुटि_सं = 0;
        ख_साफ(f);

        जर (ख_त्रुटि(f))
                परत errno_or_else(EIO);

        परत 0;
}

पूर्णांक fflush_sync_and_check(खाता *f) {
        पूर्णांक r, fd;

        प्रतिज्ञा(f);

        r = fflush_and_check(f);
        जर (r < 0)
                परत r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        जर (fd < 0)
                परत 0;

        r = fsync_full(fd);
        जर (r < 0)
                परत r;

        परत 0;
}

पूर्णांक write_timestamp_file_atomic(स्थिरांक अक्षर *fn, usec_t n) {
        अक्षर ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        जर (!timestamp_is_set(n))
                परत -दुस्फल;

        xsprintf(ln, USEC_FMT "\n", n);

        परत write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

पूर्णांक read_timestamp_file(स्थिरांक अक्षर *fn, usec_t *ret) {
        _cleanup_free_ अक्षर *ln = रिक्त;
        uint64_t t;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = read_one_ओळ_file(fn, &ln);
        जर (r < 0)
                परत r;

        r = safe_atou64(ln, &t);
        जर (r < 0)
                परत r;

        जर (!timestamp_is_set(t))
                परत -दुस्फल;

        *ret = (usec_t) t;
        परत 0;
}

पूर्णांक fputs_with_separator(खाता *f, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separator, bool *space) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        जर (!f)
                f = मानक_निकास;

        जर (!separator)
                separator = " ";

        जर (*space)
                जर (ख_माला_दो(separator, f) < 0)
                        परत -EIO;

        *space = true;

        जर (ख_माला_दो(s, f) < 0)
                परत -EIO;

        परत 0;
}

पूर्णांक fputs_with_newline(खाता *f, स्थिरांक अक्षर *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        जर (isempty(s))
                परत 0;

        जर (!f)
                f = मानक_निकास;

        जर (ख_माला_दो(s, f) < 0)
                परत -EIO;

        जर (endswith(s, "\n"))
                परत 0;

        जर (ख_अक्षर_दो('\n', f) < 0)
                परत -EIO;

        परत 1;
}

/* A bitmask of the EOL markers we know */
प्रकारव्याख्या गणना End_Of_ओळ_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_ओळ_Marker;

स्थिर End_Of_ओळ_Marker categorize_eol(अक्षर c, Read_ओळ_Flags flags) {

        जर (!FLAGS_SET(flags, READ_ओळ_ONLY_NUL)) {
                जर (c == '\n')
                        परत EOL_TEN;
                जर (c == '\r')
                        परत EOL_THIRTEEN;
        }

        जर (c == '\0')
                परत EOL_ZERO;

        परत EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(खाता*, funlockfile, रिक्त);

पूर्णांक read_ओळ_full(खाता *f, माप_प्रकार limit, Read_ओळ_Flags flags, अक्षर **ret) {
        _cleanup_free_ अक्षर *buffer = रिक्त;
        माप_प्रकार n = 0, मोजणी = 0;
        पूर्णांक r;

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

        जर (ret) {
                जर (!GREEDY_REALLOC(buffer, 1))
                        परत -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) खाता *flocked = f;
                End_Of_ओळ_Marker previous_eol = EOL_NONE;
                flockfile(f);

                साठी (;;) {
                        End_Of_ओळ_Marker eol;
                        अक्षर c;

                        जर (n >= limit)
                                परत -ENOBUFS;

                        जर (मोजणी >= पूर्णांक_उच्च) /* We couldn't return the counter anymore as "int", hence refuse this */
                                परत -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        जर (r < 0)
                                परत r;
                        जर (r == 0) /* EOF is definitely EOL */
                                थांबा;

                        eol = categorize_eol(c, flags);

                        जर (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                थांबा;
                        }

                        मोजणी++;

                        जर (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                जर ((flags & (READ_ओळ_IS_A_TTY|READ_ओळ_NOT_A_TTY)) == 0) {
                                        पूर्णांक fd;

                                        fd = fileno(f);
                                        जर (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_ओळ_NOT_A_TTY;
                                        अन्यथा
                                                flags |= isatty_safe(fd) ? READ_ओळ_IS_A_TTY : READ_ओळ_NOT_A_TTY;
                                }
                                जर (FLAGS_SET(flags, READ_ओळ_IS_A_TTY))
                                        थांबा;
                        }

                        जर (eol != EOL_NONE) {
                                previous_eol |= eol;
                                सुरूठेवा;
                        }

                        जर (ret) {
                                जर (!GREEDY_REALLOC(buffer, n + 2))
                                        परत -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        जर (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        परत (पूर्णांक) मोजणी;
}

पूर्णांक read_stripped_ओळ(खाता *f, माप_प्रकार limit, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r, k;

        प्रतिज्ञा(f);

        r = read_ओळ(f, limit, ret ? &s : रिक्त);
        जर (r < 0)
                परत r;

        जर (ret) {
                स्थिरांक अक्षर *p = strstrip(s);
                जर (p == s)
                        *ret = TAKE_PTR(s);
                अन्यथा {
                        k = strdup_to(ret, p);
                        जर (k < 0)
                                परत k;
                }
        }

        परत r > 0;          /* Return 1 if something was read. */
}

पूर्णांक safe_fgetc(खाता *f, अक्षर *ret) {
        पूर्णांक k;

        प्रतिज्ञा(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        त्रुटि_सं = 0;
        k = ख_अक्षर_लो(f);
        जर (k == खातापूर्ण) {
                जर (ख_त्रुटि(f))
                        परत errno_or_else(EIO);

                जर (ret)
                        *ret = 0;

                परत 0;
        }

        जर (ret)
                *ret = k;

        परत 1;
}

पूर्णांक warn_file_is_world_accessible(स्थिरांक अक्षर *filename, संरचना stat *st, स्थिरांक अक्षर *unit, अचिन्हित ओळ) {
        संरचना stat _st;

        जर (!filename)
                परत 0;

        जर (!st) {
                जर (stat(filename, &_st) < 0)
                        परत -त्रुटि_सं;
                st = &_st;
        }

        जर ((st->st_mode & S_IRWXO) == 0)
                परत 0;

        जर (unit)
                log_syntax(unit, LOG_WARNING, filename, ओळ, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        अन्यथा
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        परत 0;
}

पूर्णांक write_data_file_atomic_at(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *path,
                स्थिरांक संरचना iovec *iovec,
                WriteDataFileFlags flags) {

        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ अक्षर *dn = रिक्त, *fn = रिक्त;
        r = path_split_prefix_filename(path, &dn, &fn);
        जर (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                परत -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        जर (r < 0)
                परत r;

        _cleanup_close_ पूर्णांक mfd = -EBADF;
        जर (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ रिक्त,
                            &mfd);
                जर (r < 0)
                        परत r;

                dir_fd = mfd;
        }

        _cleanup_free_ अक्षर *t = रिक्त;
        _cleanup_close_ पूर्णांक fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        जर (fd < 0)
                परत fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        जर (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                जर (r < 0)
                        परत r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        जर (r < 0)
                परत r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        जर (r < 0)
                परत r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        परत 0;
}
