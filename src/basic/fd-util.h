/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>

#include "forward.h"

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
#만약 O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#아니면
#  만약 defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  아니면
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
#define NR_OPEN_MINIMUM ((unsigned) (sizeof(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(sizeof(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
static inline bool wildcard_fd_is_valid(정수 fd) {
        반환 fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

정수 close_nointr(정수 fd);
정수 safe_close(정수 fd);
void safe_close_pair(정수 p[static 2]);

static inline 정수 safe_close_above_stdio(정수 fd) {
        만약 (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                반환 -EBADF;

        반환 safe_close(fd);
}

static inline void* close_fd_ptr(void *p) {
        safe_close(PTR_TO_FD(p));
        반환 NULL;
}

void close_many(const 정수 fds[], size_t n_fds);
void close_many_unset(정수 fds[], size_t n_fds);
void close_many_and_free(정수 *fds, size_t n_fds);

정수 fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

static inline void closep(정수 *fd) {
        safe_close(*fd);
}

static inline void close_pairp(정수 (*p)[2]) {
        safe_close_pair(*p);
}

static inline void fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, NULL);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, NULL);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

정수 fd_nonblock(정수 fd, bool nonblock);
정수 stdio_disable_nonblock(void);

void nonblock_resetp(정수 *fd);

정수 fd_cloexec(정수 fd, bool cloexec);
정수 fd_cloexec_many(const 정수 fds[], size_t n_fds, bool cloexec);

정수 get_max_fd(void);

정수 close_all_fds(const 정수 except[], size_t n_except);
정수 close_all_fds_without_malloc(const 정수 except[], size_t n_except);
정수 close_all_fds_frugal(const 정수 except[], size_t n_except);

정수 pack_fds(정수 fds[], size_t n);

정수 fd_validate(정수 fd);
정수 same_fd(정수 a, 정수 b);

bool fdname_is_valid(const char *s);

정수 fd_get_path(정수 fd, char **ret);

정수 move_fd(정수 from, 정수 to, 정수 cloexec);

정수 fd_move_above_stdio(정수 fd);

정수 rearrange_stdio(정수 original_input_fd, 정수 original_output_fd, 정수 original_error_fd);

static inline 정수 make_null_stdio(void) {
        반환 rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, 정수, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                정수 *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

정수 fd_reopen(정수 fd, 정수 flags);
정수 fd_reopen_propagate_append_and_position(정수 fd, 정수 flags);
정수 fd_reopen_condition(정수 fd, 정수 flags, 정수 mask, 정수 *ret_new_fd);

정수 fd_is_opath(정수 fd);
정수 fd_vet_accmode(정수 fd, 정수 mode);
정수 fd_is_writable(정수 fd);

정수 fd_verify_safe_flags_full(정수 fd, 정수 extra_flags);
static inline 정수 fd_verify_safe_flags(정수 fd) {
        반환 fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(void);
정수 fd_get_diskseq(정수 fd, uint64_t *ret);

정수 path_is_root_at(정수 dir_fd, const char *path);
static inline 정수 path_is_root(const char *path) {
        반환 path_is_root_at(AT_FDCWD, path);
}
static inline 정수 dir_fd_is_root(정수 dir_fd) {
        반환 dir_fd == XAT_FDROOT ? true : path_is_root_at(dir_fd, NULL);
}
static inline 정수 dir_fd_is_root_or_cwd(정수 dir_fd) {
        반환 IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_root_at(dir_fd, NULL);
}

정수 fds_inode_and_mount_same(정수 fd1, 정수 fd2);

정수 resolve_xat_fdroot(정수 *fd, const char **path, char **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(정수))

char* format_proc_fd_path(char buf[static PROC_FD_PATH_MAX], 정수 fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((char[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(정수))

char* format_proc_pid_fd_path(char buf[static PROC_PID_FD_PATH_MAX], pid_t pid, 정수 fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((char[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

정수 proc_fd_enoent_errno(void);

const char* accmode_to_string(정수 flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                정수 _fd_ = (fd);                \
                assert(_fd_ >= 0);              \
                _fd_;                           \
        })
