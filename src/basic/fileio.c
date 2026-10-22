/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio_ext.h>
#ضمن <stdlib.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "hexdecoct.h"
#ضمن "io-util.h"
#ضمن "iovec-util.h"
#ضمن "label-util.h"
#ضمن "log.h"
#ضمن "mkdir.h"
#ضمن "nulstr-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "socket-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "sync-util.h"
#ضمن "terminal-util.h"
#ضمن "time-util.h"
#ضمن "tmpfile-util.h"

/* The maximum size of the file we'll read in one go in read_full_file() (64M). */
#تعریف_کن READ_پر_بایتها_MAX (64U * U64_MB - UINT64_C(1))
/* Used when a size is specified for read_full_file() with READ_FULL_FILE_UNBASE64 or _UNHEX */
#تعریف_کن READ_پر_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY 3

/* The maximum size of virtual files (i.e. procfs, sysfs, and other virtual "API" files) we'll read in one go
 * in read_virtual_file(). Note that this limit is different (and much lower) than the READ_FULL_BYTES_MAX
 * limit. This reflects the fact that we use different strategies for reading virtual and regular files:
 * virtual files we generally have to read in a single read() syscall since the kernel doesn't support
 * continuation read()s for them. Thankfully they are somewhat size constrained. Thus we can allocate the
 * full potential buffer in advance. Regular files OTOH can be much larger, and there we grow the allocations
 * exponentially in a loop. We use a size limit of 4M-2 because 4M-1 is the maximum buffer that /proc/sys/
 * allows us to read() (larger reads will fail with ENOMEM), and we want to read one extra byte so that we
 * can detect EOFs. */
#تعریف_کن READ_VIRTUAL_بایتها_MAX (4U * U64_MB - UINT64_C(2))

صحيح fdopen_unlocked(صحيح fd, ثابت حرف *options, FILE **ret) {
        أكد(ret);

        FILE *f = fdopen(fd, options);
        إذا (!f)
                أرجع -errno;

        (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        أرجع 0;
}

صحيح take_fdopen_unlocked(صحيح *fd, ثابت حرف *options, FILE **ret) {
        صحيح r;

        أكد(fd);

        r = fdopen_unlocked(*fd, options, ret);
        إذا (r < 0)
                أرجع r;

        *fd = -EBADF;

        أرجع 0;
}

FILE* take_fdopen(صحيح *fd, ثابت حرف *options) {
        أكد(fd);

        FILE *f = fdopen(*fd, options);
        إذا (!f)
                أرجع عدم;

        *fd = -EBADF;

        أرجع f;
}

DIR* take_fdopendir(صحيح *dfd) {
        أكد(dfd);

        DIR *d = fdopendir(*dfd);
        إذا (!d)
                أرجع عدم;

        *dfd = -EBADF;

        أرجع d;
}

FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        إذا (!f)
                أرجع عدم;

        (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        أرجع f;
}

FILE* fmemopen_unlocked(فراغ *buf, size_t اندازه, ثابت حرف *نما) {
        FILE *f = fmemopen(buf, اندازه, نما);
        إذا (!f)
                أرجع عدم;

        (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        أرجع f;
}

صحيح write_string_جریان_پر(
                FILE *f,
                ثابت حرف *سطر,
                WriteStringFileFlags flags,
                ثابت بنية timespec *ts) {

        bool needs_nl;
        صحيح r, fd = -EBADF;

        أكد(f);
        أكد(سطر);

        إذا (ferror(f))
                أرجع -EIO;

        إذا (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                إذا (fd < 0)
                        أرجع -EBADF;
        }

        إذا (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ حرف *t = عدم;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                إذا (fd < 0) {
                        fd = fileno(f);
                        إذا (fd < 0)
                                أرجع -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                إذا (read_virtual_file_fd(fd, طول_رشته(سطر)+1, &t, عدم) > 0 &&
                    streq_skip_trailing_chars(سطر, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", سطر);
                        أرجع 0;
                }

                إذا (lseek(fd, 0, SEEK_SET) < 0)
                        أرجع -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(سطر, "\n");

        إذا (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_میانگیر)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                سطر = strjoina(سطر, "\n");
                needs_nl = false;
        }

        إذا (fputs(سطر, f) == EOF)
                أرجع -errno;

        إذا (needs_nl)
                إذا (fputc('\n', f) == EOF)
                        أرجع -errno;

        إذا (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        وإلا
                r = fflush_and_check(f);
        إذا (r < 0)
                أرجع r;

        إذا (ts) {
                ثابت بنية timespec twice[2] = {*ts, *ts};

                أكد(fd >= 0);
                إذا (futimens(fd, twice) < 0)
                        أرجع -errno;
        }

        أرجع 0;
}

ساكن mode_t write_string_file_flags_مقصد_نما(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        أرجع FLAGS_مجموعه(flags, WRITE_STRING_FILE_نما_0600) ? 0600 :
                FLAGS_مجموعه(flags, WRITE_STRING_FILE_نما_0444) ? 0444 : 0644;
}

ساكن صحيح write_string_file_atomic_at(
                صحيح dir_fd,
                ثابت حرف *fn,
                ثابت حرف *سطر,
                WriteStringFileFlags flags,
                ثابت بنية timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(fn);
        أكد(سطر);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t نما = write_string_file_flags_مقصد_نما(flags);

        bool call_label_ops_post = false;
        إذا (FLAGS_مجموعه(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, نما, label_context);
                إذا (r < 0)
                        أرجع r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        صحيح k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? عدم : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        إذا (r < 0)
                أرجع r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        إذا (k < 0)
                أرجع k;

        r = write_string_جریان_پر(f, سطر, flags, ts);
        إذا (r < 0)
                أرجع r;

        r = fchmod_umask(fileno(f), نما);
        إذا (r < 0)
                أرجع r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        إذا (r < 0)
                أرجع r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        إذا (FLAGS_مجموعه(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_از_file(fileno(f));
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح write_string_file_پر_label(
                صحيح dir_fd,
                ثابت حرف *fn,
                ثابت حرف *سطر,
                WriteStringFileFlags flags,
                ثابت بنية timespec *ts,
                ثابت حرف *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(dir_fd == AT_FDCWD || dir_fd >= 0);
        أكد(سطر);

        /* We don't know how to verify whether the file contents was already on-disk. */
        أكد(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        إذا (flags & WRITE_STRING_FILE_MKDIR_0755) {
                أكد(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (flags & WRITE_STRING_FILE_ATOMIC) {
                أكد(fn);
                أكد(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, سطر, flags, ts, label_context);
                إذا (r < 0)
                        اذهب fail;

                أرجع r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        إذا (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        وإلا {
                mode_t نما = write_string_file_flags_مقصد_نما(flags);
                bool call_label_ops_post = false;

                إذا (FLAGS_مجموعه(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, نما, label_context);
                        إذا (r < 0)
                                اذهب fail;

                        call_label_ops_post = true;
                }

                r = fd = openat_report_new(
                                dir_fd, fn, O_CLOEXEC | O_NOCTTY |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_NOFOLLOW) ? O_NOFOLLOW : 0) |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_CREATE) ? O_CREAT : 0) |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_مجموعه(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0),
                                نما,
                                &made_file);
                إذا (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? عدم : fn, made_file, label_context));
        }
        إذا (r < 0)
                اذهب fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        إذا (r < 0)
                اذهب fail;

        إذا (flags & WRITE_STRING_FILE_DISABLE_میانگیر)
                setvbuf(f, عدم, _IONBF, 0);

        r = write_string_جریان_پر(f, سطر, flags, ts);
        إذا (r < 0)
                اذهب fail;

        أرجع 0;

fail:
        إذا (made_file)
                (فراغ) unlinkat(dir_fd, fn, 0);

        إذا (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                أرجع r;

        f = امن_fclose(f);
        fd = امن_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        إذا (verify_file_at(dir_fd, fn, سطر, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                أرجع 0;

        أرجع r;
}

صحيح write_string_filef(
                ثابت حرف *fn,
                WriteStringFileFlags flags,
                ثابت حرف *format, ...) {

        _cleanup_free_ حرف *p = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع -ENOMEM;

        أرجع write_string_file(fn, p, flags);
}

صحيح write_string_filef_at(
                صحيح dir_fd,
                ثابت حرف *fn,
                WriteStringFileFlags flags,
                ثابت حرف *format, ...) {

        _cleanup_free_ حرف *p = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع -ENOMEM;

        أرجع write_string_file_at(dir_fd, fn, p, flags);
}

صحيح write_base64_file_at(
                صحيح dir_fd,
                ثابت حرف *fn,
                ثابت بنية iovec *داده,
                WriteStringFileFlags flags) {

        _cleanup_free_ حرف *encoded = عدم;
        ssize_t n;

        n = base64mem_پر(داده ? داده->iov_base : عدم, داده ? داده->iov_len : 0, 79, &encoded);
        إذا (n < 0)
                أرجع n;

        أرجع write_string_file_at(dir_fd, fn, encoded, flags);
}

صحيح read_one_سطر_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);
        أكد(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        إذا (r < 0)
                أرجع r;

        أرجع read_سطر(f, LONG_سطر_MAX, ret);
}

صحيح read_boolean_file_at(صحيح dir_fd, ثابت حرف *filename) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(filename);

        r = read_one_سطر_file_at(dir_fd, filename, &s);
        إذا (r < 0)
                أرجع r;

        أرجع parse_boolean(s);
}

صحيح verify_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *buf = عدم;
        size_t l, k;
        صحيح r;

        أكد(blob);

        l = طول_رشته(blob);

        إذا (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = حافظه_بده(l + accept_extra_nl + 1);
        إذا (!buf)
                أرجع -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        إذا (r < 0)
                أرجع r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        إذا (ferror(f))
                أرجع errno_or_else(EIO);

        إذا (k != l && k != l + accept_extra_nl)
                أرجع 0;
        إذا (memcmp(buf, blob, l) != 0)
                أرجع 0;
        إذا (k > l && buf[l] != '\n')
                أرجع 0;

        أرجع 1;
}

صحيح read_virtual_file_at(
                صحيح dir_fd,
                ثابت حرف *filename,
                size_t max_اندازه,
                حرف **ret_contents,
                size_t *ret_اندازه) {

        _cleanup_free_ حرف *buf = عدم;
        size_t n, اندازه;
        صحيح n_retries;
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

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(max_اندازه <= READ_VIRTUAL_بایتها_MAX || max_اندازه == SIZE_MAX);

        _cleanup_close_ صحيح fd = -EBADF;
        إذا (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        وإلا
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        إذا (fd < 0)
                أرجع fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        لكل (;;) {
                بنية stat st;

                إذا (fstat(fd, &st) < 0)
                        أرجع -errno;

                إذا (!S_ISREG(st.st_mode))
                        أرجع -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_بایتها_MAX < SSIZE_MAX);
                إذا (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        إذا (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                إذا (max_اندازه == SIZE_MAX)
                                        أرجع -EFBIG;

                                اندازه = max_اندازه;
                        } وإلا {
                                اندازه = MIN((size_t) st.st_size, max_اندازه);

                                إذا (اندازه > READ_VIRTUAL_بایتها_MAX)
                                        أرجع -EFBIG;
                        }

                        n_retries--;
                } وإلا إذا (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        اندازه = MIN3(صفحه_اندازه() - 1, READ_VIRTUAL_بایتها_MAX, max_اندازه);
                        n_retries = 1;
                } وإلا {
                        اندازه = MIN(READ_VIRTUAL_بایتها_MAX, max_اندازه);
                        n_retries = 0;
                }

                buf = حافظه_بده(اندازه + 1);
                إذا (!buf)
                        أرجع -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                اندازه = MIN3(MALLOC_SIZEOF_امن(buf) - 1, max_اندازه, READ_VIRTUAL_بایتها_MAX);

                لكل (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = اقرأ(fd, buf, اندازه + 1);
                        إذا (k >= 0) {
                                n = k;
                                اكسر;
                        }

                        إذا (errno != EINTR)
                                أرجع -errno;
                }

                /* Consider a short read as EOF */
                إذا (n <= اندازه)
                        اكسر;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                إذا (max_اندازه != SIZE_MAX && n > max_اندازه) {
                        n = اندازه; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        اكسر;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                إذا (n_retries <= 0)
                        أرجع -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                إذا (lseek(fd, 0, SEEK_SET) < 0)
                        أرجع -errno;

                buf = mfree(buf);
        }

        إذا (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                إذا (!ret_اندازه && memchr(buf, 0, n))
                        أرجع -EBADMSG;

                إذا (n < اندازه) {
                        حرف *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        إذا (!p)
                                أرجع -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        إذا (ret_اندازه)
                *ret_اندازه = n;

        أرجع !truncated;
}

صحيح read_پر_جریان_پر(
                FILE *f,
                ثابت حرف *filename,
                uint64_t جابه_جایی,
                size_t اندازه,
                Read_پر_File_Flags flags,
                حرف **ret_contents,
                size_t *ret_اندازه) {

        _cleanup_free_ حرف *buf = عدم;
        size_t n, n_بعدی = 0, l, expected_decoded_اندازه = اندازه;
        صحيح fd, r;

        أكد(f);
        أكد(ret_contents);
        أكد(!FLAGS_مجموعه(flags, READ_پر_FILE_UNBASE64 | READ_پر_FILE_UNHEX));
        أكد(اندازه != SIZE_MAX || !FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER));

        إذا (جابه_جایی != UINT64_MAX && جابه_جایی > LONG_MAX) /* fseek() can only deal with "long" offsets */
                أرجع -ERANGE;

        إذا ((flags & (READ_پر_FILE_UNBASE64 | READ_پر_FILE_UNHEX)) != 0) {
                إذا (اندازه <= SIZE_MAX / READ_پر_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        اندازه *= READ_پر_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                وإلا
                        اندازه = SIZE_MAX;
        }

        fd = fileno(f);
        إذا (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                بنية stat st;

                إذا (fstat(fd, &st) < 0)
                        أرجع -errno;

                إذا (FLAGS_مجموعه(flags, READ_پر_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        إذا (r < 0)
                                أرجع r;
                }

                إذا (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        إذا (st.st_size > 0 &&
                            (اندازه == SIZE_MAX || FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, جابه_جایی == UINT64_MAX ? 0 : جابه_جایی);

                                إذا (rsize < SIZE_MAX) /* overflow check */
                                        n_بعدی = rsize + 1;
                        }

                        إذا (flags & READ_پر_FILE_WARN_WORLD_READABLE)
                                (فراغ) warn_file_is_world_accessible(filename, &st, عدم, 0);
                }
        } وإلا إذا (FLAGS_مجموعه(flags, READ_پر_FILE_VERIFY_REGULAR))
                أرجع -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        إذا (n_بعدی == 0)
                n_بعدی = اندازه != SIZE_MAX && !FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER) ? اندازه : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        إذا (n_بعدی > READ_پر_بایتها_MAX)
                n_بعدی = READ_پر_بایتها_MAX + 1;

        إذا (جابه_جایی != UINT64_MAX && fseek(f, جابه_جایی, SEEK_SET) < 0)
                أرجع -errno;

        n = l = 0;
        لكل (;;) {
                حرف *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                إذا (FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER) && n_بعدی > اندازه)
                        n_بعدی = اندازه + 1;

                إذا (flags & READ_پر_FILE_SECURE) {
                        t = حافظه_بده(n_بعدی + 1);
                        إذا (!t) {
                                r = -ENOMEM;
                                اذهب finalize;
                        }
                        memcpy_امن(t, buf, n);
                        explicit_bzero_امن(buf, n);
                        حرر(buf);
                } وإلا {
                        t = realloc(buf, n_بعدی + 1);
                        إذا (!t)
                                أرجع -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = اندازه == SIZE_MAX ? MALLOC_SIZEOF_امن(buf) - 1 : n_بعدی;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                أكد(k <= n - l);
                l += k;

                إذا (ferror(f)) {
                        r = errno_or_else(EIO);
                        اذهب finalize;
                }
                إذا (feof(f))
                        اكسر;

                إذا (اندازه != SIZE_MAX && !FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        أكد(l == اندازه);
                        اكسر;
                }

                أكد(k > 0); /* we can't have read zero bytes because that would have been EOF */

                إذا (FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER) && l > اندازه) {
                        r = -E2BIG;
                        اذهب finalize;
                }

                إذا (n >= READ_پر_بایتها_MAX) {
                        r = -E2BIG;
                        اذهب finalize;
                }

                n_بعدی = MIN(n * 2, READ_پر_بایتها_MAX);
        }

        إذا (flags & (READ_پر_FILE_UNBASE64 | READ_پر_FILE_UNHEX)) {
                _cleanup_free_ فراغ *decoded = عدم;
                size_t decoded_اندازه;

                buf[l++] = 0;
                إذا (flags & READ_پر_FILE_UNBASE64)
                        r = unbase64mem_پر(buf, l, flags & READ_پر_FILE_SECURE, &decoded, &decoded_اندازه);
                وإلا
                        r = unhexmem_پر(buf, l, flags & READ_پر_FILE_SECURE, &decoded, &decoded_اندازه);
                إذا (r < 0)
                        اذهب finalize;

                إذا (flags & READ_پر_FILE_SECURE)
                        explicit_bzero_امن(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_اندازه;

                إذا (FLAGS_مجموعه(flags, READ_پر_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_اندازه) {
                        r = -E2BIG;
                        اذهب finalize;
                }
        }

        إذا (!ret_اندازه) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                إذا (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        اذهب finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        إذا (ret_اندازه)
                *ret_اندازه = l;

        أرجع 0;

finalize:
        إذا (flags & READ_پر_FILE_SECURE)
                explicit_bzero_امن(buf, n);

        أرجع r;
}

صحيح read_پر_file_پر(
                صحيح dir_fd,
                ثابت حرف *filename,
                uint64_t جابه_جایی,
                size_t اندازه,
                Read_پر_File_Flags flags,
                ثابت حرف *bind_name,
                حرف **ret_contents,
                size_t *ret_اندازه) {

        _cleanup_fclose_ FILE *f = عدم;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        صحيح r;

        أكد(ret_contents);

        إذا (FLAGS_مجموعه(flags, READ_پر_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            جابه_جایی == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_پر(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        إذا (r < 0)
                أرجع r;

        أرجع read_پر_جریان_پر(f, filename, جابه_جایی, اندازه, flags, ret_contents, ret_اندازه);
}

صحيح script_get_shebang_interpreter(ثابت حرف *مسیر, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(مسیر);

        f = fopen(مسیر, "re");
        إذا (!f)
                أرجع -errno;

        حرف c;
        r = امن_fgetc(f, &c);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EBADMSG;
        إذا (c != '#')
                أرجع -EMEDIUMTYPE;
        r = امن_fgetc(f, &c);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EBADMSG;
        إذا (c != '!')
                أرجع -EMEDIUMTYPE;

        _cleanup_free_ حرف *سطر = عدم;
        r = read_سطر(f, LONG_سطر_MAX, &سطر);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *p = عدم;
        ثابت حرف *s = سطر;

        r = استخراج_اولی_کلمه(&s, &p, /* separators= */ عدم, /* flags= */ 0);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -ENOEXEC;

        إذا (ret)
                *ret = TAKE_PTR(p);
        أرجع 0;
}

صحيح get_proc_فیلد(ثابت حرف *مسیر, ثابت حرف *کلید, حرف **ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        أكد(مسیر);
        أكد(کلید);

        r = fopen_unlocked(مسیر, "re", &f);
        إذا (r == -ENOENT && proc_mounted() == 0)
                أرجع -ENOSYS;
        إذا (r < 0)
                أرجع r;

        لكل (;;) {
                 _cleanup_free_ حرف *سطر = عدم;

                 r = read_سطر(f, LONG_سطر_MAX, &سطر);
                 إذا (r < 0)
                         أرجع r;
                 إذا (r == 0)
                         أرجع -ENODATA;

                 حرف *l = startswith(سطر, کلید);
                 إذا (l && *l == ':') {
                         إذا (ret) {
                                 حرف *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 إذا (!s)
                                         أرجع -ENOMEM;

                                 *ret = s;
                         }

                         أرجع 0;
                 }
        }
}

DIR* xopendirat(صحيح dir_fd, ثابت حرف *مسیر, صحيح flags) {
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(!(flags & (O_CREAT|O_TMPFILE)));

        إذا ((dir_fd == AT_FDCWD || مسیر_is_absolute(مسیر)) &&
            (flags &~ O_DIRECTORY) == 0)
                أرجع opendir(مسیر);

        إذا (isempty(مسیر)) {
                مسیر = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, مسیر, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        إذا (fd < 0)
                أرجع عدم;

        أرجع take_fdopendir(&fd);
}

صحيح fopen_نما_مقصد_flags(ثابت حرف *نما) {
        ثابت حرف *p;
        صحيح flags;

        أكد(نما);

        إذا ((p = startswith(نما, "r+")))
                flags = O_RDWR;
        وإلا إذا ((p = startswith(نما, "r")))
                flags = O_RDONLY;
        وإلا إذا ((p = startswith(نما, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        وإلا إذا ((p = startswith(نما, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        وإلا إذا ((p = startswith(نما, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        وإلا إذا ((p = startswith(نما, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        وإلا
                أرجع -EINVAL;

        لكل (; *p != 0; p++) {

                بدل (*p) {

                حالة 'e':
                        flags |= O_CLOEXEC;
                        اكسر;

                حالة 'x':
                        flags |= O_EXCL;
                        اكسر;

                حالة 'm':
                        /* ignore this here, fdopen() might care later though */
                        اكسر;

                حالة 'c': /* not sure what to do about this one */
                افتراضي:
                        أرجع -EINVAL;
                }
        }

        أرجع flags;
}

ساكن صحيح xfopenat_regular(صحيح dir_fd, ثابت حرف *مسیر, ثابت حرف *نما, صحيح open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(نما);
        أكد(ret);

        إذا (dir_fd == AT_FDCWD && مسیر && open_flags == 0)
                f = fopen(مسیر, نما);
        وإلا إذا (dir_fd == XAT_FDROOT && مسیر && open_flags == 0) {
                _cleanup_free_ حرف *j = strjoin("/", مسیر);
                إذا (!j)
                        أرجع -ENOMEM;

                f = fopen(j, نما);
        } وإلا {
                _cleanup_close_ صحيح fd = -EBADF;
                صحيح نما_flags;

                نما_flags = fopen_نما_مقصد_flags(نما);
                إذا (نما_flags < 0)
                        أرجع نما_flags;

                إذا (مسیر) {
                        fd = openat(dir_fd, مسیر, نما_flags | open_flags);
                        إذا (fd < 0)
                                أرجع -errno;
                } وإلا {
                        إذا (dir_fd == AT_FDCWD)
                                أرجع -EBADF;

                        fd = fd_reopen(dir_fd, (نما_flags | open_flags) & ~O_NOFOLLOW);
                        إذا (fd < 0)
                                أرجع fd;
                }

                f = take_fdopen(&fd, نما);
        }
        إذا (!f)
                أرجع -errno;

        *ret = f;
        أرجع 0;
}

ساكن صحيح xfopenat_unix_socket(صحيح dir_fd, ثابت حرف *مسیر, ثابت حرف *bind_name, FILE **ret) {
        _cleanup_close_ صحيح sk = -EBADF;
        FILE *f;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        إذا (sk < 0)
                أرجع -errno;

        إذا (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                اتحاد sockaddr_union bsa;

                r = sockaddr_un_مجموعه_مسیر(&bsa.un, bind_name);
                إذا (r < 0)
                        أرجع r;

                إذا (bind(sk, &bsa.sa, r) < 0)
                        أرجع -errno;
        }

        r = connect_unix_مسیر(sk, dir_fd, مسیر);
        إذا (r < 0)
                أرجع r;

        إذا (shutdown(sk, SHUT_WR) < 0)
                أرجع -errno;

        f = take_fdopen(&sk, "r");
        إذا (!f)
                أرجع -errno;

        *ret = f;
        أرجع 0;
}

صحيح xfopenat_پر(
                صحيح dir_fd,
                ثابت حرف *مسیر,
                ثابت حرف *نما,
                صحيح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret) {

        FILE *f = عدم;  /* avoid false maybe-uninitialized warning */
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(نما);
        أكد(ret);

        r = xfopenat_regular(dir_fd, مسیر, نما, open_flags, &f);
        إذا (r == -ENXIO && FLAGS_مجموعه(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, مسیر, bind_name, &f);
                إذا (درون_مجموعه(r, -ENOTSOCK, -EINVAL))
                        أرجع -ENXIO; /* propagate original error if this is not a socket after all */
        }
        إذا (r < 0)
                أرجع r;

        إذا (FLAGS_مجموعه(flags, XFOPEN_UNLOCKED))
                (فراغ) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        أرجع 0;
}

صحيح fdopen_independent(صحيح fd, ثابت حرف *نما, FILE **ret) {
        _cleanup_close_ صحيح copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح نما_flags;

        أكد(fd >= 0);
        أكد(نما);
        أكد(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        نما_flags = fopen_نما_مقصد_flags(نما);
        إذا (نما_flags < 0)
                أرجع نما_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, نما_flags & ~O_CREAT);
        إذا (copy_fd < 0)
                أرجع copy_fd;

        f = take_fdopen(&copy_fd, نما);
        إذا (!f)
                أرجع -errno;

        *ret = TAKE_PTR(f);
        أرجع 0;
}

ساكن صحيح جستجو_and_open_داخلی(
                ثابت حرف *مسیر,
                صحيح نما,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                ثابت حرف *ریشه,
                حرف **جستجو,
                صحيح *ret_fd,
                حرف **ret_مسیر) {

        صحيح r;

        أكد(!ret_fd || !FLAGS_مجموعه(نما, O_CREAT)); /* We don't support O_CREAT for this */
        أكد(مسیر);

        إذا (مسیر_is_absolute(مسیر)) {
                _cleanup_close_ صحيح fd = -EBADF;

                إذا (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(مسیر, نما, 0777));
                وإلا
                        r = RET_NERRNO(access(مسیر, نما));
                إذا (r < 0)
                        أرجع r;

                إذا (ret_مسیر) {
                        r = مسیر_simplify_alloc(مسیر, ret_مسیر);
                        إذا (r < 0)
                                أرجع r;
                }

                إذا (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                أرجع 0;
        }

        إذا (!مسیر_strv_resolve_uniq(جستجو, ریشه))
                أرجع -ENOMEM;

        STRV_FOREACH(i, جستجو) {
                _cleanup_close_ صحيح fd = -EBADF;
                _cleanup_free_ حرف *p = عدم;

                p = مسیر_join(ریشه, *i, مسیر);
                إذا (!p)
                        أرجع -ENOMEM;

                إذا (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, نما, 0777));
                وإلا
                        r = RET_NERRNO(access(p, F_OK));
                إذا (r >= 0) {
                        إذا (ret_مسیر)
                                *ret_مسیر = مسیر_simplify(TAKE_PTR(p));

                        إذا (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        أرجع 0;
                }
                إذا (r != -ENOENT)
                        أرجع r;
        }

        أرجع -ENOENT;
}

صحيح جستجو_and_open(
                ثابت حرف *مسیر,
                صحيح نما,
                ثابت حرف *ریشه,
                حرف **جستجو,
                صحيح *ret_fd,
                حرف **ret_مسیر) {

        _cleanup_strv_free_ حرف **copy = عدم;

        أكد(مسیر);

        copy = strv_copy(جستجو);
        إذا (!copy)
                أرجع -ENOMEM;

        أرجع جستجو_and_open_داخلی(مسیر, نما, ریشه, copy, ret_fd, ret_مسیر);
}

ساكن صحيح جستجو_and_fopen_داخلی(
                ثابت حرف *مسیر,
                ثابت حرف *نما,
                ثابت حرف *ریشه,
                حرف **جستجو,
                FILE **ret_file,
                حرف **ret_مسیر) {

        _cleanup_free_ حرف *یافت_مسیر = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(مسیر);
        أكد(نما || !ret_file);

        r = جستجو_and_open_داخلی(
                        مسیر,
                        نما ? fopen_نما_مقصد_flags(نما) : 0,
                        ریشه,
                        جستجو,
                        ret_file ? &fd : عدم,
                        ret_مسیر ? &یافت_مسیر : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (ret_file) {
                FILE *f = take_fdopen(&fd, نما);
                إذا (!f)
                        أرجع -errno;

                *ret_file = f;
        }

        إذا (ret_مسیر)
                *ret_مسیر = TAKE_PTR(یافت_مسیر);

        أرجع 0;
}

صحيح جستجو_and_fopen(
                ثابت حرف *مسیر,
                ثابت حرف *نما,
                ثابت حرف *ریشه,
                ثابت حرف **جستجو,
                FILE **ret_file,
                حرف **ret_مسیر) {

        _cleanup_strv_free_ حرف **copy = عدم;

        أكد(مسیر);
        أكد(نما || !ret_file);

        copy = strv_copy((حرف**) جستجو);
        إذا (!copy)
                أرجع -ENOMEM;

        أرجع جستجو_and_fopen_داخلی(مسیر, نما, ریشه, copy, ret_file, ret_مسیر);
}

صحيح جستجو_and_fopen_nulstr(
                ثابت حرف *مسیر,
                ثابت حرف *نما,
                ثابت حرف *ریشه,
                ثابت حرف *جستجو,
                FILE **ret_file,
                حرف **ret_مسیر) {

        _cleanup_strv_free_ حرف **l = عدم;

        أكد(مسیر);
        أكد(نما || !ret_file);

        l = strv_تقسیم_nulstr(جستجو);
        إذا (!l)
                أرجع -ENOMEM;

        أرجع جستجو_and_fopen_داخلی(مسیر, نما, ریشه, l, ret_file, ret_مسیر);
}

صحيح fflush_and_check(FILE *f) {
        أكد(f);

        errno = 0;
        fflush(f);

        إذا (ferror(f))
                أرجع errno_or_else(EIO);

        أرجع 0;
}

صحيح fflush_sync_and_check(FILE *f) {
        صحيح r, fd;

        أكد(f);

        r = fflush_and_check(f);
        إذا (r < 0)
                أرجع r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        إذا (fd < 0)
                أرجع 0;

        r = fsync_پر(fd);
        إذا (r < 0)
                أرجع r;

        أرجع 0;
}

صحيح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n) {
        حرف ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        إذا (!timestamp_is_مجموعه(n))
                أرجع -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        أرجع write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

صحيح read_timestamp_file(ثابت حرف *fn, usec_t *ret) {
        _cleanup_free_ حرف *ln = عدم;
        uint64_t t;
        صحيح r;

        أكد(ret);

        r = read_one_سطر_file(fn, &ln);
        إذا (r < 0)
                أرجع r;

        r = امن_atou64(ln, &t);
        إذا (r < 0)
                أرجع r;

        إذا (!timestamp_is_مجموعه(t))
                أرجع -ERANGE;

        *ret = (usec_t) t;
        أرجع 0;
}

صحيح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space) {
        أكد(s);
        أكد(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        إذا (!f)
                f = stdout;

        إذا (!separator)
                separator = " ";

        إذا (*space)
                إذا (fputs(separator, f) < 0)
                        أرجع -EIO;

        *space = true;

        إذا (fputs(s, f) < 0)
                أرجع -EIO;

        أرجع 0;
}

صحيح fputs_with_newline(FILE *f, ثابت حرف *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        إذا (isempty(s))
                أرجع 0;

        إذا (!f)
                f = stdout;

        إذا (fputs(s, f) < 0)
                أرجع -EIO;

        إذا (endswith(s, "\n"))
                أرجع 0;

        إذا (fputc('\n', f) < 0)
                أرجع -EIO;

        أرجع 1;
}

/* A bitmask of the EOL markers we know */
تعريفنوع تعداد انتها_از_سطر_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} انتها_از_سطر_Marker;

ساكن انتها_از_سطر_Marker categorize_eol(حرف c, Read_سطر_Flags flags) {

        إذا (!FLAGS_مجموعه(flags, READ_سطر_ONLY_NUL)) {
                إذا (c == '\n')
                        أرجع EOL_TEN;
                إذا (c == '\r')
                        أرجع EOL_THIRTEEN;
        }

        إذا (c == '\0')
                أرجع EOL_ZERO;

        أرجع EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_پر(FILE*, funlockfile, عدم);

صحيح read_سطر_پر(FILE *f, size_t limit, Read_سطر_Flags flags, حرف **ret) {
        _cleanup_free_ حرف *میانگیر = عدم;
        size_t n = 0, شمار = 0;
        صحيح r;

        أكد(f);

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

        إذا (ret) {
                إذا (!GREEDY_REALLOC(میانگیر, 1))
                        أرجع -ENOMEM;
        }

        {
                استفادهنشده _cleanup_(funlockfilep) FILE *flocked = f;
                انتها_از_سطر_Marker قبلی_eol = EOL_NONE;
                flockfile(f);

                لكل (;;) {
                        انتها_از_سطر_Marker eol;
                        حرف c;

                        إذا (n >= limit)
                                أرجع -ENOBUFS;

                        إذا (شمار >= بیشترین_صحیح) /* We couldn't return the counter anymore as "int", hence refuse this */
                                أرجع -ENOBUFS;

                        r = امن_fgetc(f, &c);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0) /* EOF is definitely EOL */
                                اكسر;

                        eol = categorize_eol(c, flags);

                        إذا (FLAGS_مجموعه(قبلی_eol, EOL_ZERO) ||
                            (eol == EOL_NONE && قبلی_eol != EOL_NONE) ||
                            (eol != EOL_NONE && (قبلی_eol & eol) != 0)) {
                                /* Previous char was a NUL? This is not an EOL, but the previous char was? This type of
                                 * EOL marker has been seen right before?  In either of these three cases we are
                                 * done. But first, let's put this character back in the queue. (Note that we have to
                                 * cast this to (unsigned char) here as ungetc() expects a positive 'int', and if we
                                 * are on an architecture where 'char' equals 'signed char' we need to ensure we don't
                                 * pass a negative value here. That said, to complicate things further ungetc() is
                                 * actually happy with most negative characters and implicitly casts them back to
                                 * positive ones as needed, except for \xff (aka -1, aka EOF), which it refuses. What a
                                 * godawful API!) */
                                assert_se(ungetc((بیعلامت حرف) c, f) != EOF);
                                اكسر;
                        }

                        شمار++;

                        إذا (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                إذا ((flags & (READ_سطر_IS_A_TTY|READ_سطر_NOT_A_TTY)) == 0) {
                                        صحيح fd;

                                        fd = fileno(f);
                                        إذا (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_سطر_NOT_A_TTY;
                                        وإلا
                                                flags |= isatty_امن(fd) ? READ_سطر_IS_A_TTY : READ_سطر_NOT_A_TTY;
                                }
                                إذا (FLAGS_مجموعه(flags, READ_سطر_IS_A_TTY))
                                        اكسر;
                        }

                        إذا (eol != EOL_NONE) {
                                قبلی_eol |= eol;
                                استمر;
                        }

                        إذا (ret) {
                                إذا (!GREEDY_REALLOC(میانگیر, n + 2))
                                        أرجع -ENOMEM;

                                میانگیر[n] = c;
                        }

                        n++;
                }
        }

        إذا (ret) {
                میانگیر[n] = 0;

                *ret = TAKE_PTR(میانگیر);
        }

        أرجع (صحيح) شمار;
}

صحيح read_stripped_سطر(FILE *f, size_t limit, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r, k;

        أكد(f);

        r = read_سطر(f, limit, ret ? &s : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (ret) {
                ثابت حرف *p = strstrip(s);
                إذا (p == s)
                        *ret = TAKE_PTR(s);
                وإلا {
                        k = strdup_مقصد(ret, p);
                        إذا (k < 0)
                                أرجع k;
                }
        }

        أرجع r > 0;          /* Return 1 if something was read. */
}

صحيح امن_fgetc(FILE *f, حرف *ret) {
        صحيح k;

        أكد(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        إذا (k == EOF) {
                إذا (ferror(f))
                        أرجع errno_or_else(EIO);

                إذا (ret)
                        *ret = 0;

                أرجع 0;
        }

        إذا (ret)
                *ret = k;

        أرجع 1;
}

صحيح warn_file_is_world_accessible(ثابت حرف *filename, بنية stat *st, ثابت حرف *unit, بیعلامت سطر) {
        بنية stat _st;

        إذا (!filename)
                أرجع 0;

        إذا (!st) {
                إذا (stat(filename, &_st) < 0)
                        أرجع -errno;
                st = &_st;
        }

        إذا ((st->st_mode & S_IRWXO) == 0)
                أرجع 0;

        إذا (unit)
                log_syntax(unit, LOG_WARNING, filename, سطر, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        وإلا
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        أرجع 0;
}

صحيح write_داده_file_atomic_at(
                صحيح dir_fd,
                ثابت حرف *مسیر,
                ثابت بنية iovec *iovec,
                Write_داده_File_Flags flags) {

        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ حرف *dn = عدم, *fn = عدم;
        r = مسیر_تقسیم_پیشوند_filename(مسیر, &dn, &fn);
        إذا (درون_مجموعه(r, -EADDRNOTAVAIL, O_DIRECTORY))
                أرجع -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        إذا (r < 0)
                أرجع r;

        _cleanup_close_ صحيح mfd = -EBADF;
        إذا (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_مجموعه(flags, WRITE_داده_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ عدم,
                            &mfd);
                إذا (r < 0)
                        أرجع r;

                dir_fd = mfd;
        }

        _cleanup_free_ حرف *t = عدم;
        _cleanup_close_ صحيح fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        إذا (fd < 0)
                أرجع fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        إذا (iovec_is_مجموعه(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                إذا (r < 0)
                        أرجع r;
        }

        r = fchmod_umask(fd, FLAGS_مجموعه(flags, WRITE_داده_FILE_نما_0400) ? 0400 : 0644);
        إذا (r < 0)
                أرجع r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        إذا (r < 0)
                أرجع r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        أرجع 0;
}
