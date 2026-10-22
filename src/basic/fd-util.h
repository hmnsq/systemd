/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <dirent.h>
#شامل <fcntl.h>
#شامل <stdio.h>

#شامل "forward.h"

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
#اگر O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#وگرنه
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
#  وگرنه
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
#define NR_OPEN_MINIMUM ((unsigned) (اندازه(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(اندازه(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ایستا درونخطی bool wildcard_fd_is_valid(صحیح fd) {
        بازگشت fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

صحیح close_nointr(صحیح fd);
صحیح safe_close(صحیح fd);
تهی safe_close_pair(صحیح p[ایستا 2]);

ایستا درونخطی صحیح safe_close_above_stdio(صحیح fd) {
        اگر (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                بازگشت -EBADF;

        بازگشت safe_close(fd);
}

ایستا درونخطی تهی* close_fd_ptr(تهی *p) {
        safe_close(PTR_TO_FD(p));
        بازگشت پوچ;
}

تهی close_many(ثابت صحیح fds[], size_t n_fds);
تهی close_many_unset(صحیح fds[], size_t n_fds);
تهی close_many_and_free(صحیح *fds, size_t n_fds);

صحیح fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

ایستا درونخطی تهی closep(صحیح *fd) {
        safe_close(*fd);
}

ایستا درونخطی تهی close_pairp(صحیح (*p)[2]) {
        safe_close_pair(*p);
}

ایستا درونخطی تهی fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, پوچ);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, پوچ);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

صحیح fd_nonblock(صحیح fd, bool nonblock);
صحیح stdio_disable_nonblock(تهی);

تهی nonblock_resetp(صحیح *fd);

صحیح fd_cloexec(صحیح fd, bool cloexec);
صحیح fd_cloexec_many(ثابت صحیح fds[], size_t n_fds, bool cloexec);

صحیح get_max_fd(تهی);

صحیح close_all_fds(ثابت صحیح except[], size_t n_except);
صحیح close_all_fds_without_malloc(ثابت صحیح except[], size_t n_except);
صحیح close_all_fds_frugal(ثابت صحیح except[], size_t n_except);

صحیح pack_fds(صحیح fds[], size_t n);

صحیح fd_validate(صحیح fd);
صحیح same_fd(صحیح a, صحیح b);

bool fdname_is_valid(ثابت نویسه *s);

صحیح fd_get_path(صحیح fd, نویسه **ret);

صحیح move_fd(صحیح from, صحیح to, صحیح cloexec);

صحیح fd_move_above_stdio(صحیح fd);

صحیح rearrange_stdio(صحیح original_درون_داد_fd, صحیح original_output_fd, صحیح original_error_fd);

ایستا درونخطی صحیح make_null_stdio(تهی) {
        بازگشت rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, صحیح, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                صحیح *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

صحیح fd_reopen(صحیح fd, صحیح flags);
صحیح fd_reopen_propagate_append_and_position(صحیح fd, صحیح flags);
صحیح fd_reopen_condition(صحیح fd, صحیح flags, صحیح mask, صحیح *ret_new_fd);

صحیح fd_is_opath(صحیح fd);
صحیح fd_vet_accmode(صحیح fd, صحیح mode);
صحیح fd_is_writable(صحیح fd);

صحیح fd_verify_safe_flags_full(صحیح fd, صحیح extra_flags);
ایستا درونخطی صحیح fd_verify_safe_flags(صحیح fd) {
        بازگشت fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(تهی);
صحیح fd_get_diskseq(صحیح fd, uint64_t *ret);

صحیح path_is_ریشه_at(صحیح dir_fd, ثابت نویسه *path);
ایستا درونخطی صحیح path_is_ریشه(ثابت نویسه *path) {
        بازگشت path_is_ریشه_at(AT_FDCWD, path);
}
ایستا درونخطی صحیح dir_fd_is_ریشه(صحیح dir_fd) {
        بازگشت dir_fd == XAT_FDROOT ? true : path_is_ریشه_at(dir_fd, پوچ);
}
ایستا درونخطی صحیح dir_fd_is_ریشه_or_cwd(صحیح dir_fd) {
        بازگشت IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_ریشه_at(dir_fd, پوچ);
}

صحیح fds_inode_and_mount_same(صحیح fd1, صحیح fd2);

صحیح resolve_xat_fdroot(صحیح *fd, ثابت نویسه **path, نویسه **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(صحیح))

نویسه* format_proc_fd_path(نویسه buf[ایستا PROC_FD_PATH_MAX], صحیح fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((نویسه[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(صحیح))

نویسه* format_proc_pid_fd_path(نویسه buf[ایستا PROC_PID_FD_PATH_MAX], pid_t pid, صحیح fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((نویسه[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

صحیح proc_fd_enoent_errno(تهی);

ثابت نویسه* accmode_to_string(صحیح flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                صحیح _fd_ = (fd);                \
                أكد(_fd_ >= 0);              \
                _fd_;                           \
        })
