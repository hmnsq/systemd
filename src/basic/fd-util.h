/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <dirent.h>
#شامل <fcntl.h>
#شامل <stdio.h>

#شامل "forward.h"

/* maximum length of fdname */
#تعریف_کرو FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#تعریف_کرو FD_مقصد_PTR(fd) INT_مقصد_PTR((fd)+1)
#تعریف_کرو PTR_مقصد_FD(p) (PTR_مقصد_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#تعریف_کرو EBADF_PAIR { -EBADF, -EBADF }
#تعریف_کرو EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#اگر O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#ورنہ
#  اگر defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  ورنہ
#    define RAW_O_LARGEFILE 00100000
#  endif
#اگر_ختم

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#تعریف_کرو O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#تعریف_کرو NR_OPEN_DEFAULT ((غیرعلامتی) (1024 * 1024))
#تعریف_کرو NR_OPEN_MINIMUM ((غیرعلامتی) (حجم(طویل) * 8))
#تعریف_کرو NR_OPEN_زیادہ_سے_زیادہ ((غیرعلامتی) (CONST_MIN((size_t) زیادہ_سے_زیادہ_عدد, SIZE_MAX / __SIZEOF_POINTER__) & ~(حجم(طویل) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#تعریف_کرو XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ساکن اندرونی bool wildcard_fd_is_valid(صحیح fd) {
        واپس fd >= 0 || اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT);
}

صحیح close_nointr(صحیح fd);
صحیح محفوظ_close(صحیح fd);
خالی محفوظ_close_pair(صحیح p[ساکن 2]);

ساکن اندرونی صحیح محفوظ_close_above_stdio(صحیح fd) {
        اگر (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                واپس -EBADF;

        واپس محفوظ_close(fd);
}

ساکن اندرونی خالی* close_fd_ptr(خالی *p) {
        محفوظ_close(PTR_مقصد_FD(p));
        واپس تہی;
}

خالی close_many(ثابت صحیح fds[], size_t n_fds);
خالی close_many_unset(صحیح fds[], size_t n_fds);
خالی close_many_and_free(صحیح *fds, size_t n_fds);

صحیح fclose_nointr(FILE *f);
FILE* محفوظ_fclose(FILE *f);
DIR* محفوظ_closedir(DIR *d);

ساکن اندرونی خالی closep(صحیح *fd) {
        محفوظ_close(*fd);
}

ساکن اندرونی خالی close_pairp(صحیح (*p)[2]) {
        محفوظ_close_pair(*p);
}

ساکن اندرونی خالی fclosep(FILE **f) {
        محفوظ_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_بھرا(FILE*, pclose, تہی);
DEFINE_TRIVIAL_CLEANUP_FUNC_بھرا(DIR*, closedir, تہی);

#تعریف_کرو _cleanup_close_ _cleanup_(closep)
#تعریف_کرو _cleanup_fclose_ _cleanup_(fclosep)
#تعریف_کرو _cleanup_pclose_ _cleanup_(pclosep)
#تعریف_کرو _cleanup_closedir_ _cleanup_(closedirp)
#تعریف_کرو _cleanup_close_pair_ _cleanup_(close_pairp)

صحیح fd_nonblock(صحیح fd, bool nonblock);
صحیح stdio_disable_nonblock(خالی);

خالی nonblock_resetp(صحیح *fd);

صحیح fd_cloexec(صحیح fd, bool cloexec);
صحیح fd_cloexec_many(ثابت صحیح fds[], size_t n_fds, bool cloexec);

صحیح get_max_fd(خالی);

صحیح close_all_fds(ثابت صحیح except[], size_t n_except);
صحیح close_all_fds_without_malloc(ثابت صحیح except[], size_t n_except);
صحیح close_all_fds_frugal(ثابت صحیح except[], size_t n_except);

صحیح pack_fds(صحیح fds[], size_t n);

صحیح fd_validate(صحیح fd);
صحیح same_fd(صحیح a, صحیح b);

bool fdname_is_valid(ثابت حرف *s);

صحیح fd_get_راستہ(صحیح fd, حرف **ret);

صحیح move_fd(صحیح from, صحیح to, صحیح cloexec);

صحیح fd_move_above_stdio(صحیح fd);

صحیح rearrange_stdio(صحیح original_داخلہ_fd, صحیح original_output_fd, صحیح original_error_fd);

ساکن اندرونی صحیح make_null_stdio(خالی) {
        واپس rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#تعریف_کرو TAKE_FD(fd) TAKE_GENERIC(fd, صحیح, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#تعریف_کرو close_and_replace(a, b)                 \
        ({                                      \
                صحیح *_fdp_ = &(a);              \
                محفوظ_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

صحیح fd_reopen(صحیح fd, صحیح flags);
صحیح fd_reopen_propagate_append_and_position(صحیح fd, صحیح flags);
صحیح fd_reopen_condition(صحیح fd, صحیح flags, صحیح نقاب_3, صحیح *ret_new_fd);

صحیح fd_is_opath(صحیح fd);
صحیح fd_vet_accmode(صحیح fd, صحیح منوال);
صحیح fd_is_writable(صحیح fd);

صحیح fd_verify_محفوظ_flags_بھرا(صحیح fd, صحیح extra_flags);
ساکن اندرونی صحیح fd_verify_محفوظ_flags(صحیح fd) {
        واپس fd_verify_محفوظ_flags_بھرا(fd, 0);
}

غیرعلامتی read_nr_open(خالی);
صحیح fd_get_diskseq(صحیح fd, uint64_t *ret);

صحیح راستہ_is_جڑ_at(صحیح dir_fd, ثابت حرف *راستہ);
ساکن اندرونی صحیح راستہ_is_جڑ(ثابت حرف *راستہ) {
        واپس راستہ_is_جڑ_at(AT_FDCWD, راستہ);
}
ساکن اندرونی صحیح dir_fd_is_جڑ(صحیح dir_fd) {
        واپس dir_fd == XAT_FDROOT ? true : راستہ_is_جڑ_at(dir_fd, تہی);
}
ساکن اندرونی صحیح dir_fd_is_جڑ_or_cwd(صحیح dir_fd) {
        واپس اندر_سیٹ(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : راستہ_is_جڑ_at(dir_fd, تہی);
}

صحیح fds_inode_and_mount_same(صحیح fd1, صحیح fd2);

صحیح resolve_xat_fdroot(صحیح *fd, ثابت حرف **راستہ, حرف **ret_بفر);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#تعریف_کرو PROC_FD_راستہ_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(صحیح))

حرف* format_proc_fd_راستہ(حرف buf[ساکن PROC_FD_راستہ_MAX], صحیح fd);

#تعریف_کرو FORMAT_PROC_FD_راستہ(fd) \
        format_proc_fd_راستہ((حرف[PROC_FD_راستہ_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#تعریف_کرو PROC_PID_FD_راستہ_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(صحیح))

حرف* format_proc_pid_fd_راستہ(حرف buf[ساکن PROC_PID_FD_راستہ_MAX], pid_t pid, صحیح fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#تعریف_کرو FORMAT_PROC_PID_FD_راستہ(pid, fd)                                \
        format_proc_pid_fd_راستہ((حرف[PROC_PID_FD_راستہ_MAX]) {}, (pid), (fd))

صحیح proc_fd_enoent_errno(خالی);

ثابت حرف* accmode_مقصد_string(صحیح flags);

/* Like ASSERT_PTR, but for fds */
#تعریف_کرو ASSERT_FD(fd)                           \
        ({                                      \
                صحیح _fd_ = (fd);                \
                أكد(_fd_ >= 0);              \
                _fd_;                           \
        })
