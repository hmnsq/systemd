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
#якщо O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#інакше
#  якщо defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  інакше
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
static inline bool wildcard_fd_is_valid(ціле fd) {
        повернути fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

ціле close_nointr(ціле fd);
ціле safe_close(ціле fd);
void safe_close_pair(ціле p[static 2]);

static inline ціле safe_close_above_stdio(ціле fd) {
        якщо (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                повернути -EBADF;

        повернути safe_close(fd);
}

static inline void* close_fd_ptr(void *p) {
        safe_close(PTR_TO_FD(p));
        повернути NULL;
}

void close_many(const ціле fds[], size_t n_fds);
void close_many_unset(ціле fds[], size_t n_fds);
void close_many_and_free(ціле *fds, size_t n_fds);

ціле fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

static inline void closep(ціле *fd) {
        safe_close(*fd);
}

static inline void close_pairp(ціле (*p)[2]) {
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

ціле fd_nonblock(ціле fd, bool nonblock);
ціле stdio_disable_nonblock(void);

void nonblock_resetp(ціле *fd);

ціле fd_cloexec(ціле fd, bool cloexec);
ціле fd_cloexec_many(const ціле fds[], size_t n_fds, bool cloexec);

ціле get_max_fd(void);

ціле close_all_fds(const ціле except[], size_t n_except);
ціле close_all_fds_without_malloc(const ціле except[], size_t n_except);
ціле close_all_fds_frugal(const ціле except[], size_t n_except);

ціле pack_fds(ціле fds[], size_t n);

ціле fd_validate(ціле fd);
ціле same_fd(ціле a, ціле b);

bool fdname_is_valid(const char *s);

ціле fd_get_path(ціле fd, char **ret);

ціле move_fd(ціле from, ціле to, ціле cloexec);

ціле fd_move_above_stdio(ціле fd);

ціле rearrange_stdio(ціле original_input_fd, ціле original_output_fd, ціле original_error_fd);

static inline ціле make_null_stdio(void) {
        повернути rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, ціле, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                ціле *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

ціле fd_reopen(ціле fd, ціле flags);
ціле fd_reopen_propagate_append_and_position(ціле fd, ціле flags);
ціле fd_reopen_condition(ціле fd, ціле flags, ціле mask, ціле *ret_new_fd);

ціле fd_is_opath(ціле fd);
ціле fd_vet_accmode(ціле fd, ціле mode);
ціле fd_is_writable(ціле fd);

ціле fd_verify_safe_flags_full(ціле fd, ціле extra_flags);
static inline ціле fd_verify_safe_flags(ціле fd) {
        повернути fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(void);
ціле fd_get_diskseq(ціле fd, uint64_t *ret);

ціле path_is_root_at(ціле dir_fd, const char *path);
static inline ціле path_is_root(const char *path) {
        повернути path_is_root_at(AT_FDCWD, path);
}
static inline ціле dir_fd_is_root(ціле dir_fd) {
        повернути dir_fd == XAT_FDROOT ? true : path_is_root_at(dir_fd, NULL);
}
static inline ціле dir_fd_is_root_or_cwd(ціле dir_fd) {
        повернути IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_root_at(dir_fd, NULL);
}

ціле fds_inode_and_mount_same(ціле fd1, ціле fd2);

ціле resolve_xat_fdroot(ціле *fd, const char **path, char **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(ціле))

char* format_proc_fd_path(char buf[static PROC_FD_PATH_MAX], ціле fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((char[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(ціле))

char* format_proc_pid_fd_path(char buf[static PROC_PID_FD_PATH_MAX], pid_t pid, ціле fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((char[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

ціле proc_fd_enoent_errno(void);

const char* accmode_to_string(ціле flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                ціле _fd_ = (fd);                \
                assert(_fd_ >= 0);              \
                _fd_;                           \
        })
