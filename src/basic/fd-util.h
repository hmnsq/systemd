/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <dirent.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "forward.h"

/* maximum length of fdname */
#define FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#define FD_TO_PTR(fd) INT_TO_PTR((fd)+1)
#define PTR_TO_FD(p) (PTR_TO_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#define EBADF_PAIR { -EBADF, -EBADF }
#define EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#যদি O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#অন্যথা
#  যদি defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  অন্যথা
#    define RAW_O_LARGEFILE 00100000
#  endif
#endif

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#define O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#define NR_OPEN_DEFAULT ((unsigned) (1024 * 1024))
#define NR_OPEN_MINIMUM ((unsigned) (আকার(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(আকার(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
স্থির অন্তঃরেখ bool wildcard_fd_is_valid(পূর্ণসংখ্যা fd) {
        ফেরত fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

পূর্ণসংখ্যা close_nointr(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা safe_close(পূর্ণসংখ্যা fd);
শূন্য safe_close_pair(পূর্ণসংখ্যা p[স্থির 2]);

স্থির অন্তঃরেখ পূর্ণসংখ্যা safe_close_above_stdio(পূর্ণসংখ্যা fd) {
        যদি (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ফেরত -EBADF;

        ফেরত safe_close(fd);
}

স্থির অন্তঃরেখ শূন্য* close_fd_ptr(শূন্য *p) {
        safe_close(PTR_TO_FD(p));
        ফেরত ফাঁকা;
}

শূন্য close_many(ধ্রুবক পূর্ণসংখ্যা fds[], size_t n_fds);
শূন্য close_many_unset(পূর্ণসংখ্যা fds[], size_t n_fds);
শূন্য close_many_and_free(পূর্ণসংখ্যা *fds, size_t n_fds);

পূর্ণসংখ্যা fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

স্থির অন্তঃরেখ শূন্য closep(পূর্ণসংখ্যা *fd) {
        safe_close(*fd);
}

স্থির অন্তঃরেখ শূন্য close_pairp(পূর্ণসংখ্যা (*p)[2]) {
        safe_close_pair(*p);
}

স্থির অন্তঃরেখ শূন্য fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, ফাঁকা);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, ফাঁকা);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

পূর্ণসংখ্যা fd_nonblock(পূর্ণসংখ্যা fd, bool nonblock);
পূর্ণসংখ্যা stdio_disable_nonblock(শূন্য);

শূন্য nonblock_resetp(পূর্ণসংখ্যা *fd);

পূর্ণসংখ্যা fd_cloexec(পূর্ণসংখ্যা fd, bool cloexec);
পূর্ণসংখ্যা fd_cloexec_many(ধ্রুবক পূর্ণসংখ্যা fds[], size_t n_fds, bool cloexec);

পূর্ণসংখ্যা get_max_fd(শূন্য);

পূর্ণসংখ্যা close_all_fds(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except);
পূর্ণসংখ্যা close_all_fds_without_malloc(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except);
পূর্ণসংখ্যা close_all_fds_frugal(ধ্রুবক পূর্ণসংখ্যা except[], size_t n_except);

পূর্ণসংখ্যা pack_fds(পূর্ণসংখ্যা fds[], size_t n);

পূর্ণসংখ্যা fd_validate(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা same_fd(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b);

bool fdname_is_valid(ধ্রুবক অক্ষর *s);

পূর্ণসংখ্যা fd_get_path(পূর্ণসংখ্যা fd, অক্ষর **ret);

পূর্ণসংখ্যা move_fd(পূর্ণসংখ্যা from, পূর্ণসংখ্যা to, পূর্ণসংখ্যা cloexec);

পূর্ণসংখ্যা fd_move_above_stdio(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা rearrange_stdio(পূর্ণসংখ্যা original_প্রবেশ_fd, পূর্ণসংখ্যা original_output_fd, পূর্ণসংখ্যা original_error_fd);

স্থির অন্তঃরেখ পূর্ণসংখ্যা make_null_stdio(শূন্য) {
        ফেরত rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, পূর্ণসংখ্যা, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                পূর্ণসংখ্যা *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

পূর্ণসংখ্যা fd_reopen(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা fd_reopen_propagate_append_and_position(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা fd_reopen_condition(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা flags, পূর্ণসংখ্যা mask, পূর্ণসংখ্যা *ret_new_fd);

পূর্ণসংখ্যা fd_is_opath(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা fd_vet_accmode(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা mode);
পূর্ণসংখ্যা fd_is_writable(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা fd_verify_safe_flags_full(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা extra_flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_verify_safe_flags(পূর্ণসংখ্যা fd) {
        ফেরত fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(শূন্য);
পূর্ণসংখ্যা fd_get_diskseq(পূর্ণসংখ্যা fd, uint64_t *ret);

পূর্ণসংখ্যা path_is_মূল_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path);
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_is_মূল(ধ্রুবক অক্ষর *path) {
        ফেরত path_is_মূল_at(AT_FDCWD, path);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা dir_fd_is_মূল(পূর্ণসংখ্যা dir_fd) {
        ফেরত dir_fd == XAT_FDROOT ? true : path_is_মূল_at(dir_fd, ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা dir_fd_is_মূল_or_cwd(পূর্ণসংখ্যা dir_fd) {
        ফেরত IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_মূল_at(dir_fd, ফাঁকা);
}

পূর্ণসংখ্যা fds_inode_and_mount_same(পূর্ণসংখ্যা fd1, পূর্ণসংখ্যা fd2);

পূর্ণসংখ্যা resolve_xat_fdroot(পূর্ণসংখ্যা *fd, ধ্রুবক অক্ষর **path, অক্ষর **ret_বাফার);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(পূর্ণসংখ্যা))

অক্ষর* format_proc_fd_path(অক্ষর buf[স্থির PROC_FD_PATH_MAX], পূর্ণসংখ্যা fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((অক্ষর[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(পূর্ণসংখ্যা))

অক্ষর* format_proc_pid_fd_path(অক্ষর buf[স্থির PROC_PID_FD_PATH_MAX], pid_t pid, পূর্ণসংখ্যা fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((অক্ষর[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

পূর্ণসংখ্যা proc_fd_enoent_errno(শূন্য);

ধ্রুবক অক্ষর* accmode_to_string(পূর্ণসংখ্যা flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                পূর্ণসংখ্যা _fd_ = (fd);                \
                প্রতিজ্ঞা(_fd_ >= 0);              \
                _fd_;                           \
        })
