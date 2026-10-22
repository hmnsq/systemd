/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio_ext.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "label-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mkdir.h"
#അന്തര്ഭാവ "nulstr-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "socket-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "sync-util.h"
#അന്തര്ഭാവ "terminal-util.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "tmpfile-util.h"

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

പൂർണ്ണസംഖ്യ fdopen_unlocked(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *options, FILE **ret) {
        പ്രതിജ്ഞാ(ret);

        FILE *f = fdopen(fd, options);
        എങ്കിൽ (!f)
                തിരികെ -errno;

        (ശൂന്യം) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ take_fdopen_unlocked(പൂർണ്ണസംഖ്യ *fd, സ്ഥിരം അക്ഷരം *options, FILE **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd);

        r = fdopen_unlocked(*fd, options, ret);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *fd = -EBADF;

        തിരികെ 0;
}

FILE* take_fdopen(പൂർണ്ണസംഖ്യ *fd, സ്ഥിരം അക്ഷരം *options) {
        പ്രതിജ്ഞാ(fd);

        FILE *f = fdopen(*fd, options);
        എങ്കിൽ (!f)
                തിരികെ ശൂന്യ;

        *fd = -EBADF;

        തിരികെ f;
}

DIR* take_fdopendir(പൂർണ്ണസംഖ്യ *dfd) {
        പ്രതിജ്ഞാ(dfd);

        DIR *d = fdopendir(*dfd);
        എങ്കിൽ (!d)
                തിരികെ ശൂന്യ;

        *dfd = -EBADF;

        തിരികെ d;
}

FILE* open_memstream_unlocked(അക്ഷരം **ptr, size_t *sizeloc) {
        FILE *f = open_memstream(ptr, sizeloc);
        എങ്കിൽ (!f)
                തിരികെ ശൂന്യ;

        (ശൂന്യം) __fsetlocking(f, FSETLOCKING_BYCALLER);

        തിരികെ f;
}

FILE* fmemopen_unlocked(ശൂന്യം *buf, size_t size, സ്ഥിരം അക്ഷരം *mode) {
        FILE *f = fmemopen(buf, size, mode);
        എങ്കിൽ (!f)
                തിരികെ ശൂന്യ;

        (ശൂന്യം) __fsetlocking(f, FSETLOCKING_BYCALLER);

        തിരികെ f;
}

പൂർണ്ണസംഖ്യ write_string_stream_full(
                FILE *f,
                സ്ഥിരം അക്ഷരം *വരി,
                WriteStringFileFlags flags,
                സ്ഥിരം ഘടന timespec *ts) {

        bool needs_nl;
        പൂർണ്ണസംഖ്യ r, fd = -EBADF;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(വരി);

        എങ്കിൽ (ferror(f))
                തിരികെ -EIO;

        എങ്കിൽ (ts) {
                /* If we shall set the timestamp we need the fd. But fmemopen() streams generally don't have
                 * an fd. Let's fail early in that case. */
                fd = fileno(f);
                എങ്കിൽ (fd < 0)
                        തിരികെ -EBADF;
        }

        എങ്കിൽ (flags & WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) {
                _cleanup_free_ അക്ഷരം *t = ശൂന്യ;

                /* If value to be written is same as that of the existing value, then suppress the write. */

                എങ്കിൽ (fd < 0) {
                        fd = fileno(f);
                        എങ്കിൽ (fd < 0)
                                തിരികെ -EBADF;
                }

                /* Read an additional byte to detect cases where the prefix matches but the rest
                 * doesn't. Also, 0 returned by read_virtual_file_fd() means the read was truncated and
                 * it won't be equal to the new value. */
                എങ്കിൽ (read_virtual_file_fd(fd, strlen(വരി)+1, &t, ശൂന്യ) > 0 &&
                    streq_skip_trailing_chars(വരി, t, NEWLINE)) {
                        log_debug("No change in value '%s', suppressing write", വരി);
                        തിരികെ 0;
                }

                എങ്കിൽ (lseek(fd, 0, SEEK_SET) < 0)
                        തിരികെ -errno;
        }

        needs_nl = !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) && !endswith(വരി, "\n");

        എങ്കിൽ (needs_nl && (flags & WRITE_STRING_FILE_DISABLE_BUFFER)) {
                /* If STDIO buffering was disabled, then let's append the newline character to the string
                 * itself, so that the write goes out in one go, instead of two */

                വരി = strjoina(വരി, "\n");
                needs_nl = false;
        }

        എങ്കിൽ (fputs(വരി, f) == EOF)
                തിരികെ -errno;

        എങ്കിൽ (needs_nl)
                എങ്കിൽ (fputc('\n', f) == EOF)
                        തിരികെ -errno;

        എങ്കിൽ (flags & WRITE_STRING_FILE_SYNC)
                r = fflush_sync_and_check(f);
        അല്ലെങ്കിൽ
                r = fflush_and_check(f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ts) {
                സ്ഥിരം ഘടന timespec twice[2] = {*ts, *ts};

                പ്രതിജ്ഞാ(fd >= 0);
                എങ്കിൽ (futimens(fd, twice) < 0)
                        തിരികെ -errno;
        }

        തിരികെ 0;
}

സ്ഥിതം mode_t write_string_file_flags_to_mode(WriteStringFileFlags flags) {

        /* We support three different modes, that are the ones that really make sense for text files like this:
         *
         *     → 0600 (i.e. root-only)
         *     → 0444 (i.e. read-only)
         *     → 0644 (i.e. writable for root, readable for everyone else)
         */

        തിരികെ FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0600) ? 0600 :
                FLAGS_SET(flags, WRITE_STRING_FILE_MODE_0444) ? 0444 : 0644;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_string_file_atomic_at(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *fn,
                സ്ഥിരം അക്ഷരം *വരി,
                WriteStringFileFlags flags,
                സ്ഥിരം ഘടന timespec *ts,
                LabelContext *label_context) {

        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fn);
        പ്രതിജ്ഞാ(വരി);

        /* Note that we'd really like to use O_TMPFILE here, but can't really, since we want replacement
         * semantics here, and O_TMPFILE can't offer that. i.e. rename() replaces but linkat() doesn't. */

        mode_t mode = write_string_file_flags_to_mode(flags);

        bool call_label_ops_post = false;
        എങ്കിൽ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fn, mode, label_context);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                call_label_ops_post = true;
        }

        r = fopen_temporary_at(dir_fd, fn, &f, &p);
        പൂർണ്ണസംഖ്യ k = call_label_ops_post ? label_ops_post(f ? fileno(f) : dir_fd, f ? ശൂന്യ : fn, /* created= */ !!f, label_context) : 0;
        /* If fopen_temporary_at() failed in the above, propagate the error code, and ignore failures in
         * label_ops_post(). */
        എങ്കിൽ (r < 0)
                തിരികെ r;
        CLEANUP_TMPFILE_AT(dir_fd, p);
        എങ്കിൽ (k < 0)
                തിരികെ k;

        r = write_string_stream_full(f, വരി, flags, ts);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = fchmod_umask(fileno(f), mode);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = RET_NERRNO(renameat(dir_fd, p, dir_fd, fn));
        എങ്കിൽ (r < 0)
                തിരികെ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        എങ്കിൽ (FLAGS_SET(flags, WRITE_STRING_FILE_SYNC)) {
                /* Sync the rename, too */
                r = fsync_directory_of_file(fileno(f));
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ write_string_file_full_label(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *fn,
                സ്ഥിരം അക്ഷരം *വരി,
                WriteStringFileFlags flags,
                സ്ഥിരം ഘടന timespec *ts,
                സ്ഥിരം അക്ഷരം *label_fn,
                LabelContext *label_context) {

        bool made_file = false;
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd == AT_FDCWD || dir_fd >= 0);
        പ്രതിജ്ഞാ(വരി);

        /* We don't know how to verify whether the file contents was already on-disk. */
        പ്രതിജ്ഞാ(!((flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE) && (flags & WRITE_STRING_FILE_SYNC)));

        എങ്കിൽ (flags & WRITE_STRING_FILE_MKDIR_0755) {
                പ്രതിജ്ഞാ(fn);

                r = mkdirat_parents(dir_fd, fn, 0755);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (flags & WRITE_STRING_FILE_ATOMIC) {
                പ്രതിജ്ഞാ(fn);
                പ്രതിജ്ഞാ(flags & WRITE_STRING_FILE_CREATE);

                r = write_string_file_atomic_at(dir_fd, fn, വരി, flags, ts, label_context);
                എങ്കിൽ (r < 0)
                        പോകുക fail;

                തിരികെ r;
        }

        /* We manually build our own version of fopen(..., "we") that works without O_CREAT and with O_NOFOLLOW if needed. */
        എങ്കിൽ (isempty(fn))
                r = fd = fd_reopen(
                                ASSERT_FD(dir_fd), O_CLOEXEC | O_NOCTTY |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_TRUNCATE) ? O_TRUNC : 0) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL) ? O_RDWR : O_WRONLY) |
                                (FLAGS_SET(flags, WRITE_STRING_FILE_OPEN_NONBLOCKING) ? O_NONBLOCK : 0));
        അല്ലെങ്കിൽ {
                mode_t mode = write_string_file_flags_to_mode(flags);
                bool call_label_ops_post = false;

                എങ്കിൽ (FLAGS_SET(flags, WRITE_STRING_FILE_LABEL|WRITE_STRING_FILE_CREATE)) {
                        r = label_ops_pre(dir_fd, label_fn ?: fn, mode, label_context);
                        എങ്കിൽ (r < 0)
                                പോകുക fail;

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
                എങ്കിൽ (call_label_ops_post)
                        /* If openat_report_new() failed in the above, propagate the error code, and ignore
                         * failures in label_ops_post(). */
                        RET_GATHER(r, label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ശൂന്യ : fn, made_file, label_context));
        }
        എങ്കിൽ (r < 0)
                പോകുക fail;

        r = take_fdopen_unlocked(&fd, "w", &f);
        എങ്കിൽ (r < 0)
                പോകുക fail;

        എങ്കിൽ (flags & WRITE_STRING_FILE_DISABLE_BUFFER)
                setvbuf(f, ശൂന്യ, _IONBF, 0);

        r = write_string_stream_full(f, വരി, flags, ts);
        എങ്കിൽ (r < 0)
                പോകുക fail;

        തിരികെ 0;

fail:
        എങ്കിൽ (made_file)
                (ശൂന്യം) unlinkat(dir_fd, fn, 0);

        എങ്കിൽ (!(flags & WRITE_STRING_FILE_VERIFY_ON_FAILURE))
                തിരികെ r;

        f = safe_fclose(f);
        fd = safe_close(fd);

        /* OK, the operation failed, but let's see if the right contents in place already. If so, eat up the
         * error. */
        എങ്കിൽ (verify_file_at(dir_fd, fn, വരി, !(flags & WRITE_STRING_FILE_AVOID_NEWLINE) || (flags & WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE)) > 0)
                തിരികെ 0;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ write_string_filef(
                സ്ഥിരം അക്ഷരം *fn,
                WriteStringFileFlags flags,
                സ്ഥിരം അക്ഷരം *format, ...) {

        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        തിരികെ write_string_file(fn, p, flags);
}

പൂർണ്ണസംഖ്യ write_string_filef_at(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *fn,
                WriteStringFileFlags flags,
                സ്ഥിരം അക്ഷരം *format, ...) {

        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        തിരികെ write_string_file_at(dir_fd, fn, p, flags);
}

പൂർണ്ണസംഖ്യ write_base64_file_at(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *fn,
                സ്ഥിരം ഘടന iovec *data,
                WriteStringFileFlags flags) {

        _cleanup_free_ അക്ഷരം *encoded = ശൂന്യ;
        ssize_t n;

        n = base64mem_full(data ? data->iov_base : ശൂന്യ, data ? data->iov_len : 0, 79, &encoded);
        എങ്കിൽ (n < 0)
                തിരികെ n;

        തിരികെ write_string_file_at(dir_fd, fn, encoded, flags);
}

പൂർണ്ണസംഖ്യ read_one_വരി_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(filename);
        പ്രതിജ്ഞാ(ret);

        r = fopen_unlocked_at(dir_fd, filename, "re", /* open_flags= */ 0, &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ read_വരി(f, LONG_വരി_MAX, ret);
}

പൂർണ്ണസംഖ്യ read_boolean_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(filename);

        r = read_one_വരി_file_at(dir_fd, filename, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_boolean(s);
}

പൂർണ്ണസംഖ്യ verify_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *blob, bool accept_extra_nl) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        size_t l, k;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(blob);

        l = strlen(blob);

        എങ്കിൽ (accept_extra_nl && endswith(blob, "\n"))
                accept_extra_nl = false;

        buf = malloc(l + accept_extra_nl + 1);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        r = fopen_unlocked_at(dir_fd, strempty(fn), "re", 0, &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* We try to read one byte more than we need, so that we know whether we hit eof */
        errno = 0;
        k = fread(buf, 1, l + accept_extra_nl + 1, f);
        എങ്കിൽ (ferror(f))
                തിരികെ errno_or_else(EIO);

        എങ്കിൽ (k != l && k != l + accept_extra_nl)
                തിരികെ 0;
        എങ്കിൽ (memcmp(buf, blob, l) != 0)
                തിരികെ 0;
        എങ്കിൽ (k > l && buf[l] != '\n')
                തിരികെ 0;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ read_virtual_file_at(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *filename,
                size_t max_size,
                അക്ഷരം **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        size_t n, size;
        പൂർണ്ണസംഖ്യ n_retries;
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

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(max_size <= READ_VIRTUAL_BYTES_MAX || max_size == SIZE_MAX);

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        എങ്കിൽ (isempty(filename))
                fd = fd_reopen(ASSERT_FD(dir_fd), O_RDONLY | O_NOCTTY | O_CLOEXEC);
        അല്ലെങ്കിൽ
                fd = RET_NERRNO(openat(dir_fd, filename, O_RDONLY | O_NOCTTY | O_CLOEXEC));
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        /* Limit the number of attempts to read the number of bytes returned by fstat(). */
        n_retries = 3;

        വേണ്ടി (;;) {
                ഘടന stat st;

                എങ്കിൽ (fstat(fd, &st) < 0)
                        തിരികെ -errno;

                എങ്കിൽ (!S_ISREG(st.st_mode))
                        തിരികെ -EBADF;

                /* Be prepared for files from /proc which generally report a file size of 0. */
                assert_cc(READ_VIRTUAL_BYTES_MAX < SSIZE_MAX);
                എങ്കിൽ (st.st_size > 0 && n_retries > 1) {
                        /* Let's use the file size if we have more than 1 attempt left. On the last attempt
                         * we'll ignore the file size */

                        എങ്കിൽ (st.st_size > SSIZE_MAX) { /* Avoid overflow with 32-bit size_t and 64-bit off_t. */

                                എങ്കിൽ (max_size == SIZE_MAX)
                                        തിരികെ -EFBIG;

                                size = max_size;
                        } അല്ലെങ്കിൽ {
                                size = MIN((size_t) st.st_size, max_size);

                                എങ്കിൽ (size > READ_VIRTUAL_BYTES_MAX)
                                        തിരികെ -EFBIG;
                        }

                        n_retries--;
                } അല്ലെങ്കിൽ എങ്കിൽ (n_retries > 1) {
                        /* Files in /proc are generally smaller than the page size so let's start with
                         * a page size buffer from malloc and only use the max buffer on the final try. */
                        size = MIN3(page_size() - 1, READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 1;
                } അല്ലെങ്കിൽ {
                        size = MIN(READ_VIRTUAL_BYTES_MAX, max_size);
                        n_retries = 0;
                }

                buf = malloc(size + 1);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                /* Use a bigger allocation if we got it anyway, but not more than the limit. */
                size = MIN3(MALLOC_SIZEOF_SAFE(buf) - 1, max_size, READ_VIRTUAL_BYTES_MAX);

                വേണ്ടി (;;) {
                        ssize_t k;

                        /* Read one more byte so we can detect whether the content of the
                         * file has already changed or the guessed size for files from /proc
                         * wasn't large enough . */
                        k = പഠ(fd, buf, size + 1);
                        എങ്കിൽ (k >= 0) {
                                n = k;
                                നിർത്തുക;
                        }

                        എങ്കിൽ (errno != EINTR)
                                തിരികെ -errno;
                }

                /* Consider a short read as EOF */
                എങ്കിൽ (n <= size)
                        നിർത്തുക;

                /* If a maximum size is specified and we already read more we know the file is larger, and
                 * can handle this as truncation case. Note that if the size of what we read equals the
                 * maximum size then this doesn't mean truncation, the file might or might not end on that
                 * byte. We need to rerun the loop in that case, with a larger buffer size, so that we read
                 * at least one more byte to be able to distinguish EOF from truncation. */
                എങ്കിൽ (max_size != SIZE_MAX && n > max_size) {
                        n = size; /* Make sure we never use more than what we sized the buffer for (so that
                                   * we have one free byte in it for the trailing NUL we add below). */
                        truncated = true;
                        നിർത്തുക;
                }

                /* We have no further attempts left? Then the file is apparently larger than our limits. Give up. */
                എങ്കിൽ (n_retries <= 0)
                        തിരികെ -EFBIG;

                /* Hmm... either we read too few bytes from /proc or less likely the content of the file
                 * might have been changed (and is now bigger) while we were processing, let's try again
                 * either with the new file size. */

                എങ്കിൽ (lseek(fd, 0, SEEK_SET) < 0)
                        തിരികെ -errno;

                buf = mfree(buf);
        }

        എങ്കിൽ (ret_contents) {

                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */
                എങ്കിൽ (!ret_size && memchr(buf, 0, n))
                        തിരികെ -EBADMSG;

                എങ്കിൽ (n < size) {
                        അക്ഷരം *p;

                        /* Return rest of the buffer to libc */
                        p = realloc(buf, n + 1);
                        എങ്കിൽ (!p)
                                തിരികെ -ENOMEM;
                        buf = p;
                }

                buf[n] = 0;
                *ret_contents = TAKE_PTR(buf);
        }

        എങ്കിൽ (ret_size)
                *ret_size = n;

        തിരികെ !truncated;
}

പൂർണ്ണസംഖ്യ read_full_stream_full(
                FILE *f,
                സ്ഥിരം അക്ഷരം *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                അക്ഷരം **ret_contents,
                size_t *ret_size) {

        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        size_t n, n_next = 0, l, expected_decoded_size = size;
        പൂർണ്ണസംഖ്യ fd, r;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(ret_contents);
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX));
        പ്രതിജ്ഞാ(size != SIZE_MAX || !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER));

        എങ്കിൽ (offset != UINT64_MAX && offset > LONG_MAX) /* fseek() can only deal with "long" offsets */
                തിരികെ -ERANGE;

        എങ്കിൽ ((flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) != 0) {
                എങ്കിൽ (size <= SIZE_MAX / READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY)
                        size *= READ_FULL_FILE_ENCODED_STRING_AMPLIFICATION_BOUNDARY;
                അല്ലെങ്കിൽ
                        size = SIZE_MAX;
        }

        fd = fileno(f);
        എങ്കിൽ (fd >= 0) { /* If the FILE* object is backed by an fd (as opposed to memory or such, see
                        * fmemopen()), let's optimize our buffering */
                ഘടന stat st;

                എങ്കിൽ (fstat(fd, &st) < 0)
                        തിരികെ -errno;

                എങ്കിൽ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR)) {
                        r = stat_verify_regular(&st);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (S_ISREG(st.st_mode)) {

                        /* Try to start with the right file size if we shall read the file in full. Note
                         * that we increase the size to read here by one, so that the first read attempt
                         * already makes us notice the EOF. If the reported size of the file is zero, we
                         * avoid this logic however, since quite likely it might be a virtual file in procfs
                         * that all report a zero file size. */

                        എങ്കിൽ (st.st_size > 0 &&
                            (size == SIZE_MAX || FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER))) {

                                uint64_t rsize =
                                        LESS_BY((uint64_t) st.st_size, offset == UINT64_MAX ? 0 : offset);

                                എങ്കിൽ (rsize < SIZE_MAX) /* overflow check */
                                        n_next = rsize + 1;
                        }

                        എങ്കിൽ (flags & READ_FULL_FILE_WARN_WORLD_READABLE)
                                (ശൂന്യം) warn_file_is_world_accessible(filename, &st, ശൂന്യ, 0);
                }
        } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, READ_FULL_FILE_VERIFY_REGULAR))
                തിരികെ -EBADFD;

        /* If we don't know how much to read, figure it out now. If we shall read a part of the file, then
         * allocate the requested size. If we shall load the full file start with LINE_MAX. Note that if
         * READ_FULL_FILE_FAIL_WHEN_LARGER we consider the specified size a safety limit, and thus also start
         * with LINE_MAX, under assumption the file is most likely much shorter. */
        എങ്കിൽ (n_next == 0)
                n_next = size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) ? size : LINE_MAX;

        /* Never read more than we need to determine that our own limit is hit */
        എങ്കിൽ (n_next > READ_FULL_BYTES_MAX)
                n_next = READ_FULL_BYTES_MAX + 1;

        എങ്കിൽ (offset != UINT64_MAX && fseek(f, offset, SEEK_SET) < 0)
                തിരികെ -errno;

        n = l = 0;
        വേണ്ടി (;;) {
                അക്ഷരം *t;
                size_t k;

                /* If we shall fail when reading overly large data, then read exactly one byte more than the
                 * specified size at max, since that'll tell us if there's anymore data beyond the limit. */
                എങ്കിൽ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && n_next > size)
                        n_next = size + 1;

                എങ്കിൽ (flags & READ_FULL_FILE_SECURE) {
                        t = malloc(n_next + 1);
                        എങ്കിൽ (!t) {
                                r = -ENOMEM;
                                പോകുക finalize;
                        }
                        memcpy_safe(t, buf, n);
                        explicit_bzero_safe(buf, n);
                        സ്വതന്ത്രം(buf);
                } അല്ലെങ്കിൽ {
                        t = realloc(buf, n_next + 1);
                        എങ്കിൽ (!t)
                                തിരികെ -ENOMEM;
                }

                buf = t;
                /* Unless a size has been explicitly specified, try to read as much as fits into the memory
                 * we allocated (minus 1, to leave one byte for the safety NUL byte) */
                n = size == SIZE_MAX ? MALLOC_SIZEOF_SAFE(buf) - 1 : n_next;

                errno = 0;
                k = fread(buf + l, 1, n - l, f);

                പ്രതിജ്ഞാ(k <= n - l);
                l += k;

                എങ്കിൽ (ferror(f)) {
                        r = errno_or_else(EIO);
                        പോകുക finalize;
                }
                എങ്കിൽ (feof(f))
                        നിർത്തുക;

                എങ്കിൽ (size != SIZE_MAX && !FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER)) {
                        /* If we got asked to read some specific size, we already sized the buffer right,
                         * hence leave. */
                        പ്രതിജ്ഞാ(l == size);
                        നിർത്തുക;
                }

                പ്രതിജ്ഞാ(k > 0); /* we can't have read zero bytes because that would have been EOF */

                എങ്കിൽ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > size) {
                        r = -E2BIG;
                        പോകുക finalize;
                }

                എങ്കിൽ (n >= READ_FULL_BYTES_MAX) {
                        r = -E2BIG;
                        പോകുക finalize;
                }

                n_next = MIN(n * 2, READ_FULL_BYTES_MAX);
        }

        എങ്കിൽ (flags & (READ_FULL_FILE_UNBASE64 | READ_FULL_FILE_UNHEX)) {
                _cleanup_free_ ശൂന്യം *decoded = ശൂന്യ;
                size_t decoded_size;

                buf[l++] = 0;
                എങ്കിൽ (flags & READ_FULL_FILE_UNBASE64)
                        r = unbase64mem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                അല്ലെങ്കിൽ
                        r = unhexmem_full(buf, l, flags & READ_FULL_FILE_SECURE, &decoded, &decoded_size);
                എങ്കിൽ (r < 0)
                        പോകുക finalize;

                എങ്കിൽ (flags & READ_FULL_FILE_SECURE)
                        explicit_bzero_safe(buf, n);
                free_and_replace(buf, decoded);
                n = l = decoded_size;

                എങ്കിൽ (FLAGS_SET(flags, READ_FULL_FILE_FAIL_WHEN_LARGER) && l > expected_decoded_size) {
                        r = -E2BIG;
                        പോകുക finalize;
                }
        }

        എങ്കിൽ (!ret_size) {
                /* Safety check: if the caller doesn't want to know the size of what we just read it will
                 * rely on the trailing NUL byte. But if there's an embedded NUL byte, then we should refuse
                 * operation as otherwise there'd be ambiguity about what we just read. */

                എങ്കിൽ (memchr(buf, 0, l)) {
                        r = -EBADMSG;
                        പോകുക finalize;
                }
        }

        buf[l] = 0;
        *ret_contents = TAKE_PTR(buf);

        എങ്കിൽ (ret_size)
                *ret_size = l;

        തിരികെ 0;

finalize:
        എങ്കിൽ (flags & READ_FULL_FILE_SECURE)
                explicit_bzero_safe(buf, n);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ read_full_file_full(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *filename,
                uint64_t offset,
                size_t size,
                ReadFullFileFlags flags,
                സ്ഥിരം അക്ഷരം *bind_name,
                അക്ഷരം **ret_contents,
                size_t *ret_size) {

        _cleanup_fclose_ FILE *f = ശൂന്യ;
        XfopenFlags xflags = XFOPEN_UNLOCKED;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_contents);

        എങ്കിൽ (FLAGS_SET(flags, READ_FULL_FILE_CONNECT_SOCKET) && /* If this is enabled, let's try to connect to it */
            offset == UINT64_MAX)                              /* Seeking is not supported on AF_UNIX sockets */
                xflags |= XFOPEN_SOCKET;

        r = xfopenat_full(dir_fd, filename, "re", 0, xflags, bind_name, &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ read_full_stream_full(f, filename, offset, size, flags, ret_contents, ret_size);
}

പൂർണ്ണസംഖ്യ script_get_shebang_interpreter(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        f = fopen(path, "re");
        എങ്കിൽ (!f)
                തിരികെ -errno;

        അക്ഷരം c;
        r = safe_fgetc(f, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EBADMSG;
        എങ്കിൽ (c != '#')
                തിരികെ -EMEDIUMTYPE;
        r = safe_fgetc(f, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EBADMSG;
        എങ്കിൽ (c != '!')
                തിരികെ -EMEDIUMTYPE;

        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        r = read_വരി(f, LONG_വരി_MAX, &വരി);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *s = വരി;

        r = extract_first_word(&s, &p, /* separators= */ ശൂന്യ, /* flags= */ 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -ENOEXEC;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(p);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_proc_field(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം **ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Retrieve one field from a file like /proc/self/status. "key" matches the beginning of the line
         * and should not include whitespace or the delimiter (':').
         * Whitespaces after the ':' will be skipped. Only the first element is returned
         * (i.e. for /proc/meminfo line "MemTotal: 1024 kB" -> return "1024"). */

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(താക്കോൽ);

        r = fopen_unlocked(path, "re", &f);
        എങ്കിൽ (r == -ENOENT && proc_mounted() == 0)
                തിരികെ -ENOSYS;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                 _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;

                 r = read_വരി(f, LONG_വരി_MAX, &വരി);
                 എങ്കിൽ (r < 0)
                         തിരികെ r;
                 എങ്കിൽ (r == 0)
                         തിരികെ -ENODATA;

                 അക്ഷരം *l = startswith(വരി, താക്കോൽ);
                 എങ്കിൽ (l && *l == ':') {
                         എങ്കിൽ (ret) {
                                 അക്ഷരം *s = strdupcspn(skip_leading_chars(l + 1, " \t"), WHITESPACE);
                                 എങ്കിൽ (!s)
                                         തിരികെ -ENOMEM;

                                 *ret = s;
                         }

                         തിരികെ 0;
                 }
        }
}

DIR* xopendirat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(!(flags & (O_CREAT|O_TMPFILE)));

        എങ്കിൽ ((dir_fd == AT_FDCWD || path_is_absolute(path)) &&
            (flags &~ O_DIRECTORY) == 0)
                തിരികെ opendir(path);

        എങ്കിൽ (isempty(path)) {
                path = ".";
                flags |= O_NOFOLLOW;
        }

        fd = openat(dir_fd, path, O_NONBLOCK|O_DIRECTORY|O_CLOEXEC|flags);
        എങ്കിൽ (fd < 0)
                തിരികെ ശൂന്യ;

        തിരികെ take_fdopendir(&fd);
}

പൂർണ്ണസംഖ്യ fopen_mode_to_flags(സ്ഥിരം അക്ഷരം *mode) {
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ flags;

        പ്രതിജ്ഞാ(mode);

        എങ്കിൽ ((p = startswith(mode, "r+")))
                flags = O_RDWR;
        അല്ലെങ്കിൽ എങ്കിൽ ((p = startswith(mode, "r")))
                flags = O_RDONLY;
        അല്ലെങ്കിൽ എങ്കിൽ ((p = startswith(mode, "w+")))
                flags = O_RDWR|O_CREAT|O_TRUNC;
        അല്ലെങ്കിൽ എങ്കിൽ ((p = startswith(mode, "w")))
                flags = O_WRONLY|O_CREAT|O_TRUNC;
        അല്ലെങ്കിൽ എങ്കിൽ ((p = startswith(mode, "a+")))
                flags = O_RDWR|O_CREAT|O_APPEND;
        അല്ലെങ്കിൽ എങ്കിൽ ((p = startswith(mode, "a")))
                flags = O_WRONLY|O_CREAT|O_APPEND;
        അല്ലെങ്കിൽ
                തിരികെ -EINVAL;

        വേണ്ടി (; *p != 0; p++) {

                തിരഞ്ഞെടുപ്പ് (*p) {

                അവസ്ഥ 'e':
                        flags |= O_CLOEXEC;
                        നിർത്തുക;

                അവസ്ഥ 'x':
                        flags |= O_EXCL;
                        നിർത്തുക;

                അവസ്ഥ 'm':
                        /* ignore this here, fdopen() might care later though */
                        നിർത്തുക;

                അവസ്ഥ 'c': /* not sure what to do about this one */
                സ്ഥിരസ്ഥിതി:
                        തിരികെ -EINVAL;
                }
        }

        തിരികെ flags;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ xfopenat_regular(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *mode, പൂർണ്ണസംഖ്യ open_flags, FILE **ret) {
        FILE *f;

        /* A combination of fopen() with openat() */

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(mode);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (dir_fd == AT_FDCWD && path && open_flags == 0)
                f = fopen(path, mode);
        അല്ലെങ്കിൽ എങ്കിൽ (dir_fd == XAT_FDROOT && path && open_flags == 0) {
                _cleanup_free_ അക്ഷരം *j = strjoin("/", path);
                എങ്കിൽ (!j)
                        തിരികെ -ENOMEM;

                f = fopen(j, mode);
        } അല്ലെങ്കിൽ {
                _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
                പൂർണ്ണസംഖ്യ mode_flags;

                mode_flags = fopen_mode_to_flags(mode);
                എങ്കിൽ (mode_flags < 0)
                        തിരികെ mode_flags;

                എങ്കിൽ (path) {
                        fd = openat(dir_fd, path, mode_flags | open_flags);
                        എങ്കിൽ (fd < 0)
                                തിരികെ -errno;
                } അല്ലെങ്കിൽ {
                        എങ്കിൽ (dir_fd == AT_FDCWD)
                                തിരികെ -EBADF;

                        fd = fd_reopen(dir_fd, (mode_flags | open_flags) & ~O_NOFOLLOW);
                        എങ്കിൽ (fd < 0)
                                തിരികെ fd;
                }

                f = take_fdopen(&fd, mode);
        }
        എങ്കിൽ (!f)
                തിരികെ -errno;

        *ret = f;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ xfopenat_unix_socket(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *bind_name, FILE **ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ sk = -EBADF;
        FILE *f;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(ret);

        sk = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0);
        എങ്കിൽ (sk < 0)
                തിരികെ -errno;

        എങ്കിൽ (bind_name) {
                /* If the caller specified a socket name to bind to, do so before connecting. This is
                 * useful to communicate some minor, short meta-information token from the client to
                 * the server. */
                സംഘം sockaddr_union bsa;

                r = sockaddr_un_set_path(&bsa.un, bind_name);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (bind(sk, &bsa.sa, r) < 0)
                        തിരികെ -errno;
        }

        r = connect_unix_path(sk, dir_fd, path);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (shutdown(sk, SHUT_WR) < 0)
                തിരികെ -errno;

        f = take_fdopen(&sk, "r");
        എങ്കിൽ (!f)
                തിരികെ -errno;

        *ret = f;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ xfopenat_full(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *mode,
                പൂർണ്ണസംഖ്യ open_flags,
                XfopenFlags flags,
                സ്ഥിരം അക്ഷരം *bind_name,
                FILE **ret) {

        FILE *f = ശൂന്യ;  /* avoid false maybe-uninitialized warning */
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(mode);
        പ്രതിജ്ഞാ(ret);

        r = xfopenat_regular(dir_fd, path, mode, open_flags, &f);
        എങ്കിൽ (r == -ENXIO && FLAGS_SET(flags, XFOPEN_SOCKET)) {
                /* ENXIO is what Linux returns if we open a node that is an AF_UNIX socket */
                r = xfopenat_unix_socket(dir_fd, path, bind_name, &f);
                എങ്കിൽ (IN_SET(r, -ENOTSOCK, -EINVAL))
                        തിരികെ -ENXIO; /* propagate original error if this is not a socket after all */
        }
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (FLAGS_SET(flags, XFOPEN_UNLOCKED))
                (ശൂന്യം) __fsetlocking(f, FSETLOCKING_BYCALLER);

        *ret = f;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdopen_independent(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *mode, FILE **ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ copy_fd = -EBADF;
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ mode_flags;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(mode);
        പ്രതിജ്ഞാ(ret);

        /* A combination of fdopen() + fd_reopen(). i.e. reopens the inode the specified fd points to and
         * returns a FILE* for it */

        mode_flags = fopen_mode_to_flags(mode);
        എങ്കിൽ (mode_flags < 0)
                തിരികെ mode_flags;

        /* Flags returned by fopen_mode_to_flags might contain O_CREAT, but it doesn't make sense for fd_reopen
         * since we're working on an existing fd anyway. Let's drop it here to avoid triggering assertion. */
        copy_fd = fd_reopen(fd, mode_flags & ~O_CREAT);
        എങ്കിൽ (copy_fd < 0)
                തിരികെ copy_fd;

        f = take_fdopen(&copy_fd, mode);
        എങ്കിൽ (!f)
                തിരികെ -errno;

        *ret = TAKE_PTR(f);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ തിരയൽ_and_open_internal(
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ mode,            /* if ret_fd is NULL this is an [FRWX]_OK mode for access(), otherwise an open mode for open() */
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം **തിരയൽ,
                പൂർണ്ണസംഖ്യ *ret_fd,
                അക്ഷരം **ret_path) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!ret_fd || !FLAGS_SET(mode, O_CREAT)); /* We don't support O_CREAT for this */
        പ്രതിജ്ഞാ(path);

        എങ്കിൽ (path_is_absolute(path)) {
                _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

                എങ്കിൽ (ret_fd)
                        /* We only specify 0777 here to appease static analyzers, it's never used since we
                         * don't support O_CREAT here */
                        r = fd = RET_NERRNO(open(path, mode, 0777));
                അല്ലെങ്കിൽ
                        r = RET_NERRNO(access(path, mode));
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (ret_path) {
                        r = path_simplify_alloc(path, ret_path);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (ret_fd)
                        *ret_fd = TAKE_FD(fd);

                തിരികെ 0;
        }

        എങ്കിൽ (!path_strv_resolve_uniq(തിരയൽ, വേര്))
                തിരികെ -ENOMEM;

        STRV_FOREACH(i, തിരയൽ) {
                _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
                _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                p = path_join(വേര്, *i, path);
                എങ്കിൽ (!p)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (ret_fd)
                        /* as above, 0777 is static analyzer appeasement */
                        r = fd = RET_NERRNO(open(p, mode, 0777));
                അല്ലെങ്കിൽ
                        r = RET_NERRNO(access(p, F_OK));
                എങ്കിൽ (r >= 0) {
                        എങ്കിൽ (ret_path)
                                *ret_path = path_simplify(TAKE_PTR(p));

                        എങ്കിൽ (ret_fd)
                                *ret_fd = TAKE_FD(fd);

                        തിരികെ 0;
                }
                എങ്കിൽ (r != -ENOENT)
                        തിരികെ r;
        }

        തിരികെ -ENOENT;
}

പൂർണ്ണസംഖ്യ തിരയൽ_and_open(
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ mode,
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം **തിരയൽ,
                പൂർണ്ണസംഖ്യ *ret_fd,
                അക്ഷരം **ret_path) {

        _cleanup_strv_free_ അക്ഷരം **copy = ശൂന്യ;

        പ്രതിജ്ഞാ(path);

        copy = strv_copy(തിരയൽ);
        എങ്കിൽ (!copy)
                തിരികെ -ENOMEM;

        തിരികെ തിരയൽ_and_open_internal(path, mode, വേര്, copy, ret_fd, ret_path);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ തിരയൽ_and_fopen_internal(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *mode,
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം **തിരയൽ,
                FILE **ret_file,
                അക്ഷരം **ret_path) {

        _cleanup_free_ അക്ഷരം *found_path = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(mode || !ret_file);

        r = തിരയൽ_and_open_internal(
                        path,
                        mode ? fopen_mode_to_flags(mode) : 0,
                        വേര്,
                        തിരയൽ,
                        ret_file ? &fd : ശൂന്യ,
                        ret_path ? &found_path : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_file) {
                FILE *f = take_fdopen(&fd, mode);
                എങ്കിൽ (!f)
                        തിരികെ -errno;

                *ret_file = f;
        }

        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(found_path);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ തിരയൽ_and_fopen(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *mode,
                സ്ഥിരം അക്ഷരം *വേര്,
                സ്ഥിരം അക്ഷരം **തിരയൽ,
                FILE **ret_file,
                അക്ഷരം **ret_path) {

        _cleanup_strv_free_ അക്ഷരം **copy = ശൂന്യ;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(mode || !ret_file);

        copy = strv_copy((അക്ഷരം**) തിരയൽ);
        എങ്കിൽ (!copy)
                തിരികെ -ENOMEM;

        തിരികെ തിരയൽ_and_fopen_internal(path, mode, വേര്, copy, ret_file, ret_path);
}

പൂർണ്ണസംഖ്യ തിരയൽ_and_fopen_nulstr(
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *mode,
                സ്ഥിരം അക്ഷരം *വേര്,
                സ്ഥിരം അക്ഷരം *തിരയൽ,
                FILE **ret_file,
                അക്ഷരം **ret_path) {

        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(mode || !ret_file);

        l = strv_split_nulstr(തിരയൽ);
        എങ്കിൽ (!l)
                തിരികെ -ENOMEM;

        തിരികെ തിരയൽ_and_fopen_internal(path, mode, വേര്, l, ret_file, ret_path);
}

പൂർണ്ണസംഖ്യ fflush_and_check(FILE *f) {
        പ്രതിജ്ഞാ(f);

        errno = 0;
        fflush(f);

        എങ്കിൽ (ferror(f))
                തിരികെ errno_or_else(EIO);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fflush_sync_and_check(FILE *f) {
        പൂർണ്ണസംഖ്യ r, fd;

        പ്രതിജ്ഞാ(f);

        r = fflush_and_check(f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Not all file streams have an fd associated (think: fmemopen()), let's handle this gracefully and
         * assume that in that case we need no explicit syncing */
        fd = fileno(f);
        എങ്കിൽ (fd < 0)
                തിരികെ 0;

        r = fsync_full(fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ write_timestamp_file_atomic(സ്ഥിരം അക്ഷരം *fn, usec_t n) {
        അക്ഷരം ln[DECIMAL_STR_MAX(n)+2];

        /* Creates a "timestamp" file, that contains nothing but a
         * usec_t timestamp, formatted in ASCII. */

        എങ്കിൽ (!timestamp_is_set(n))
                തിരികെ -ERANGE;

        xsprintf(ln, USEC_FMT "\n", n);

        തിരികെ write_string_file(fn, ln, WRITE_STRING_FILE_CREATE|WRITE_STRING_FILE_ATOMIC);
}

പൂർണ്ണസംഖ്യ read_timestamp_file(സ്ഥിരം അക്ഷരം *fn, usec_t *ret) {
        _cleanup_free_ അക്ഷരം *ln = ശൂന്യ;
        uint64_t t;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = read_one_വരി_file(fn, &ln);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = safe_atou64(ln, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!timestamp_is_set(t))
                തിരികെ -ERANGE;

        *ret = (usec_t) t;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fputs_with_separator(FILE *f, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separator, bool *space) {
        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(space);

        /* Outputs the specified string with fputs(), but optionally prefixes it with a separator.
         * The *space parameter when specified shall initially point to a boolean variable initialized
         * to false. It is set to true after the first invocation. This call is supposed to be use in loops,
         * where a separator shall be inserted between each element, but not before the first one. */

        എങ്കിൽ (!f)
                f = stdout;

        എങ്കിൽ (!separator)
                separator = " ";

        എങ്കിൽ (*space)
                എങ്കിൽ (fputs(separator, f) < 0)
                        തിരികെ -EIO;

        *space = true;

        എങ്കിൽ (fputs(s, f) < 0)
                തിരികെ -EIO;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fputs_with_newline(FILE *f, സ്ഥിരം അക്ഷരം *s) {

        /* This is like fputs() but outputs a trailing newline char, but only if the string isn't empty
         * and doesn't end in a newline already. Returns 0 in case we didn't append a newline, > 0 otherwise. */

        എങ്കിൽ (isempty(s))
                തിരികെ 0;

        എങ്കിൽ (!f)
                f = stdout;

        എങ്കിൽ (fputs(s, f) < 0)
                തിരികെ -EIO;

        എങ്കിൽ (endswith(s, "\n"))
                തിരികെ 0;

        എങ്കിൽ (fputc('\n', f) < 0)
                തിരികെ -EIO;

        തിരികെ 1;
}

/* A bitmask of the EOL markers we know */
തരനിർവചനം എണ്ണൽ End_Of_വരി_Marker {
        EOL_NONE     = 0,
        EOL_ZERO     = 1 << 0,  /* \0 (aka NUL) */
        EOL_TEN      = 1 << 1,  /* \n (aka NL, aka LF)  */
        EOL_THIRTEEN = 1 << 2,  /* \r (aka CR)  */
} End_Of_വരി_Marker;

സ്ഥിതം End_Of_വരി_Marker categorize_eol(അക്ഷരം c, Read_വരി_Flags flags) {

        എങ്കിൽ (!FLAGS_SET(flags, READ_വരി_ONLY_NUL)) {
                എങ്കിൽ (c == '\n')
                        തിരികെ EOL_TEN;
                എങ്കിൽ (c == '\r')
                        തിരികെ EOL_THIRTEEN;
        }

        എങ്കിൽ (c == '\0')
                തിരികെ EOL_ZERO;

        തിരികെ EOL_NONE;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, funlockfile, ശൂന്യ);

പൂർണ്ണസംഖ്യ read_വരി_full(FILE *f, size_t limit, Read_വരി_Flags flags, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *buffer = ശൂന്യ;
        size_t n = 0, എണ്ണം = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f);

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

        എങ്കിൽ (ret) {
                എങ്കിൽ (!GREEDY_REALLOC(buffer, 1))
                        തിരികെ -ENOMEM;
        }

        {
                _unused_ _cleanup_(funlockfilep) FILE *flocked = f;
                End_Of_വരി_Marker previous_eol = EOL_NONE;
                flockfile(f);

                വേണ്ടി (;;) {
                        End_Of_വരി_Marker eol;
                        അക്ഷരം c;

                        എങ്കിൽ (n >= limit)
                                തിരികെ -ENOBUFS;

                        എങ്കിൽ (എണ്ണം >= INT_MAX) /* We couldn't return the counter anymore as "int", hence refuse this */
                                തിരികെ -ENOBUFS;

                        r = safe_fgetc(f, &c);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0) /* EOF is definitely EOL */
                                നിർത്തുക;

                        eol = categorize_eol(c, flags);

                        എങ്കിൽ (FLAGS_SET(previous_eol, EOL_ZERO) ||
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
                                assert_se(ungetc((unsigned അക്ഷരം) c, f) != EOF);
                                നിർത്തുക;
                        }

                        എണ്ണം++;

                        എങ്കിൽ (eol != EOL_NONE) {
                                /* If we are on a tty, we can't shouldn't wait for more input, because that
                                 * generally means waiting for the user, interactively. In the case of a TTY
                                 * we expect only \n as the single EOL marker, so we are in the lucky
                                 * position that there is no need to wait. We check this condition last, to
                                 * avoid isatty() check if not necessary. */

                                എങ്കിൽ ((flags & (READ_വരി_IS_A_TTY|READ_വരി_NOT_A_TTY)) == 0) {
                                        പൂർണ്ണസംഖ്യ fd;

                                        fd = fileno(f);
                                        എങ്കിൽ (fd < 0) /* Maybe an fmemopen() stream? Handle this gracefully,
                                                     * and don't call isatty() on an invalid fd */
                                                flags |= READ_വരി_NOT_A_TTY;
                                        അല്ലെങ്കിൽ
                                                flags |= isatty_safe(fd) ? READ_വരി_IS_A_TTY : READ_വരി_NOT_A_TTY;
                                }
                                എങ്കിൽ (FLAGS_SET(flags, READ_വരി_IS_A_TTY))
                                        നിർത്തുക;
                        }

                        എങ്കിൽ (eol != EOL_NONE) {
                                previous_eol |= eol;
                                തുടരുക;
                        }

                        എങ്കിൽ (ret) {
                                എങ്കിൽ (!GREEDY_REALLOC(buffer, n + 2))
                                        തിരികെ -ENOMEM;

                                buffer[n] = c;
                        }

                        n++;
                }
        }

        എങ്കിൽ (ret) {
                buffer[n] = 0;

                *ret = TAKE_PTR(buffer);
        }

        തിരികെ (പൂർണ്ണസംഖ്യ) എണ്ണം;
}

പൂർണ്ണസംഖ്യ read_stripped_വരി(FILE *f, size_t limit, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r, k;

        പ്രതിജ്ഞാ(f);

        r = read_വരി(f, limit, ret ? &s : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret) {
                സ്ഥിരം അക്ഷരം *p = strstrip(s);
                എങ്കിൽ (p == s)
                        *ret = TAKE_PTR(s);
                അല്ലെങ്കിൽ {
                        k = strdup_to(ret, p);
                        എങ്കിൽ (k < 0)
                                തിരികെ k;
                }
        }

        തിരികെ r > 0;          /* Return 1 if something was read. */
}

പൂർണ്ണസംഖ്യ safe_fgetc(FILE *f, അക്ഷരം *ret) {
        പൂർണ്ണസംഖ്യ k;

        പ്രതിജ്ഞാ(f);

        /* A safer version of plain fgetc(): let's propagate the error that happened while reading as such, and
         * separate the EOF condition from the byte read, to avoid those confusion signed/unsigned issues fgetc()
         * has. */

        errno = 0;
        k = fgetc(f);
        എങ്കിൽ (k == EOF) {
                എങ്കിൽ (ferror(f))
                        തിരികെ errno_or_else(EIO);

                എങ്കിൽ (ret)
                        *ret = 0;

                തിരികെ 0;
        }

        എങ്കിൽ (ret)
                *ret = k;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ warn_file_is_world_accessible(സ്ഥിരം അക്ഷരം *filename, ഘടന stat *st, സ്ഥിരം അക്ഷരം *unit, unsigned വരി) {
        ഘടന stat _st;

        എങ്കിൽ (!filename)
                തിരികെ 0;

        എങ്കിൽ (!st) {
                എങ്കിൽ (stat(filename, &_st) < 0)
                        തിരികെ -errno;
                st = &_st;
        }

        എങ്കിൽ ((st->st_mode & S_IRWXO) == 0)
                തിരികെ 0;

        എങ്കിൽ (unit)
                log_syntax(unit, LOG_WARNING, filename, വരി, 0,
                           "%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                           filename, st->st_mode & 07777);
        അല്ലെങ്കിൽ
                log_warning("%s has %04o mode that is too permissive, please adjust the ownership and access mode.",
                            filename, st->st_mode & 07777);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ write_data_file_atomic_at(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം ഘടന iovec *iovec,
                WriteDataFileFlags flags) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));

        /* This is a cousin of write_string_file_atomic(), but operates with arbitrary struct iovec binary
         * data (rather than strings), works without FILE* streams, and does direct syscalls instead. */

        _cleanup_free_ അക്ഷരം *dn = ശൂന്യ, *fn = ശൂന്യ;
        r = path_split_prefix_filename(path, &dn, &fn);
        എങ്കിൽ (IN_SET(r, -EADDRNOTAVAIL, O_DIRECTORY))
                തിരികെ -EISDIR; /* path refers to "." or "/" (which are dirs, which we cannot write), or is suffixed with "/" */
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_close_ പൂർണ്ണസംഖ്യ mfd = -EBADF;
        എങ്കിൽ (dn) {
                /* If there's a directory component, readjust our position */
                r = chaseat(XAT_FDROOT,
                            dir_fd,
                            dn,
                            FLAGS_SET(flags, WRITE_DATA_FILE_MKDIR_0755) ? CHASE_MKDIR_0755 : 0,
                            /* ret_path= */ ശൂന്യ,
                            &mfd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                dir_fd = mfd;
        }

        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = open_tmpfile_linkable_at(dir_fd, fn, O_WRONLY|O_CLOEXEC, &t);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        CLEANUP_TMPFILE_AT(dir_fd, t);

        എങ്കിൽ (iovec_is_set(iovec)) {
                r = loop_write(fd, iovec->iov_base, iovec->iov_len);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        r = fchmod_umask(fd, FLAGS_SET(flags, WRITE_DATA_FILE_MODE_0400) ? 0400 : 0644);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = link_tmpfile_at(fd, dir_fd, t, fn, LINK_TMPFILE_REPLACE);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        t = mfree(t); /* disarm CLEANUP_TMPFILE_AT */
        തിരികെ 0;
}
