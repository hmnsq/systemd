/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <dirent.h>
#ضمن <fcntl.h>
#ضمن <stdio.h>

#ضمن "forward.h"

/* maximum length of fdname */
#تعریف_کرو FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#تعریف_کرو FD_منزل_PTR(fd) INT_منزل_PTR((fd)+1)
#تعریف_کرو PTR_منزل_FD(p) (PTR_منزل_INT(p)-1)

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
#إذا O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#وإلا
#  إذا defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  وإلا
#    define RAW_O_LARGEFILE 00100000
#  endif
#جے_انت

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#تعریف_کرو O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#تعریف_کرو NR_OPEN_DEFAULT ((بنانشانی) (1024 * 1024))
#تعریف_کرو NR_OPEN_MINIMUM ((بنانشانی) (حجم(لما) * 8))
#تعریف_کرو NR_OPEN_ودھ_توں_ودھ ((بنانشانی) (CONST_MIN((size_t) ودھ_توں_ودھ_عدد, SIZE_MAX / __SIZEOF_POINTER__) & ~(حجم(لما) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#تعریف_کرو XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ساكن مضمن bool wildcard_fd_is_valid(صحيح fd) {
        أرجع fd >= 0 || وچ_سیٹ(fd, AT_FDCWD, XAT_FDROOT);
}

صحيح close_nointr(صحيح fd);
صحيح محفوظ_close(صحيح fd);
فراغ محفوظ_close_pair(صحيح p[ساكن 2]);

ساكن مضمن صحيح محفوظ_close_above_stdio(صحيح fd) {
        إذا (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                أرجع -EBADF;

        أرجع محفوظ_close(fd);
}

ساكن مضمن فراغ* close_fd_ptr(فراغ *p) {
        محفوظ_close(PTR_منزل_FD(p));
        أرجع عدم;
}

فراغ close_many(ثابت صحيح fds[], size_t n_fds);
فراغ close_many_unset(صحيح fds[], size_t n_fds);
فراغ close_many_and_free(صحيح *fds, size_t n_fds);

صحيح fclose_nointr(FILE *f);
FILE* محفوظ_fclose(FILE *f);
DIR* محفوظ_closedir(DIR *d);

ساكن مضمن فراغ closep(صحيح *fd) {
        محفوظ_close(*fd);
}

ساكن مضمن فراغ close_pairp(صحيح (*p)[2]) {
        محفوظ_close_pair(*p);
}

ساكن مضمن فراغ fclosep(FILE **f) {
        محفوظ_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_بھریا(FILE*, pclose, عدم);
DEFINE_TRIVIAL_CLEANUP_FUNC_بھریا(DIR*, closedir, عدم);

#تعریف_کرو _cleanup_close_ _cleanup_(closep)
#تعریف_کرو _cleanup_fclose_ _cleanup_(fclosep)
#تعریف_کرو _cleanup_pclose_ _cleanup_(pclosep)
#تعریف_کرو _cleanup_closedir_ _cleanup_(closedirp)
#تعریف_کرو _cleanup_close_pair_ _cleanup_(close_pairp)

صحيح fd_nonblock(صحيح fd, bool nonblock);
صحيح stdio_disable_nonblock(فراغ);

فراغ nonblock_resetp(صحيح *fd);

صحيح fd_cloexec(صحيح fd, bool cloexec);
صحيح fd_cloexec_many(ثابت صحيح fds[], size_t n_fds, bool cloexec);

صحيح get_max_fd(فراغ);

صحيح close_all_fds(ثابت صحيح except[], size_t n_except);
صحيح close_all_fds_without_malloc(ثابت صحيح except[], size_t n_except);
صحيح close_all_fds_frugal(ثابت صحيح except[], size_t n_except);

صحيح pack_fds(صحيح fds[], size_t n);

صحيح fd_validate(صحيح fd);
صحيح same_fd(صحيح a, صحيح b);

bool fdname_is_valid(ثابت حرف *s);

صحيح fd_get_رستہ(صحيح fd, حرف **ret);

صحيح move_fd(صحيح from, صحيح to, صحيح cloexec);

صحيح fd_move_above_stdio(صحيح fd);

صحيح rearrange_stdio(صحيح original_داخلہ_fd, صحيح original_output_fd, صحيح original_error_fd);

ساكن مضمن صحيح make_null_stdio(فراغ) {
        أرجع rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#تعریف_کرو TAKE_FD(fd) TAKE_GENERIC(fd, صحيح, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#تعریف_کرو close_and_replace(a, b)                 \
        ({                                      \
                صحيح *_fdp_ = &(a);              \
                محفوظ_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

صحيح fd_reopen(صحيح fd, صحيح flags);
صحيح fd_reopen_propagate_append_and_position(صحيح fd, صحيح flags);
صحيح fd_reopen_condition(صحيح fd, صحيح flags, صحيح نقاب_3, صحيح *ret_new_fd);

صحيح fd_is_opath(صحيح fd);
صحيح fd_vet_accmode(صحيح fd, صحيح منوال);
صحيح fd_is_writable(صحيح fd);

صحيح fd_verify_محفوظ_flags_بھریا(صحيح fd, صحيح extra_flags);
ساكن مضمن صحيح fd_verify_محفوظ_flags(صحيح fd) {
        أرجع fd_verify_محفوظ_flags_بھریا(fd, 0);
}

بنانشانی read_nr_open(فراغ);
صحيح fd_get_diskseq(صحيح fd, uint64_t *ret);

صحيح رستہ_is_جڑ_at(صحيح dir_fd, ثابت حرف *رستہ);
ساكن مضمن صحيح رستہ_is_جڑ(ثابت حرف *رستہ) {
        أرجع رستہ_is_جڑ_at(AT_FDCWD, رستہ);
}
ساكن مضمن صحيح dir_fd_is_جڑ(صحيح dir_fd) {
        أرجع dir_fd == XAT_FDROOT ? true : رستہ_is_جڑ_at(dir_fd, عدم);
}
ساكن مضمن صحيح dir_fd_is_جڑ_or_cwd(صحيح dir_fd) {
        أرجع وچ_سیٹ(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : رستہ_is_جڑ_at(dir_fd, عدم);
}

صحيح fds_inode_and_mount_same(صحيح fd1, صحيح fd2);

صحيح resolve_xat_fdroot(صحيح *fd, ثابت حرف **رستہ, حرف **ret_بفر);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#تعریف_کرو PROC_FD_رستہ_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(صحيح))

حرف* format_proc_fd_رستہ(حرف buf[ساكن PROC_FD_رستہ_MAX], صحيح fd);

#تعریف_کرو FORMAT_PROC_FD_رستہ(fd) \
        format_proc_fd_رستہ((حرف[PROC_FD_رستہ_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#تعریف_کرو PROC_PID_FD_رستہ_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(صحيح))

حرف* format_proc_pid_fd_رستہ(حرف buf[ساكن PROC_PID_FD_رستہ_MAX], pid_t pid, صحيح fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#تعریف_کرو FORMAT_PROC_PID_FD_رستہ(pid, fd)                                \
        format_proc_pid_fd_رستہ((حرف[PROC_PID_FD_رستہ_MAX]) {}, (pid), (fd))

صحيح proc_fd_enoent_errno(فراغ);

ثابت حرف* accmode_منزل_string(صحيح flags);

/* Like ASSERT_PTR, but for fds */
#تعریف_کرو ASSERT_FD(fd)                           \
        ({                                      \
                صحيح _fd_ = (fd);                \
                أكد(_fd_ >= 0);              \
                _fd_;                           \
        })
