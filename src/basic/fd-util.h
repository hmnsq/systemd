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
#nếu O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#không_thì
#  nếu defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  không_thì
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
static inline bool wildcard_fd_is_valid(số_nguyên fd) {
        trả_về fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

số_nguyên close_nointr(số_nguyên fd);
số_nguyên safe_close(số_nguyên fd);
void safe_close_pair(số_nguyên p[static 2]);

static inline số_nguyên safe_close_above_stdio(số_nguyên fd) {
        nếu (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                trả_về -EBADF;

        trả_về safe_close(fd);
}

static inline void* close_fd_ptr(void *p) {
        safe_close(PTR_TO_FD(p));
        trả_về NULL;
}

void close_many(const số_nguyên fds[], size_t n_fds);
void close_many_unset(số_nguyên fds[], size_t n_fds);
void close_many_and_free(số_nguyên *fds, size_t n_fds);

số_nguyên fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

static inline void closep(số_nguyên *fd) {
        safe_close(*fd);
}

static inline void close_pairp(số_nguyên (*p)[2]) {
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

số_nguyên fd_nonblock(số_nguyên fd, bool nonblock);
số_nguyên stdio_disable_nonblock(void);

void nonblock_resetp(số_nguyên *fd);

số_nguyên fd_cloexec(số_nguyên fd, bool cloexec);
số_nguyên fd_cloexec_many(const số_nguyên fds[], size_t n_fds, bool cloexec);

số_nguyên get_max_fd(void);

số_nguyên close_all_fds(const số_nguyên except[], size_t n_except);
số_nguyên close_all_fds_without_malloc(const số_nguyên except[], size_t n_except);
số_nguyên close_all_fds_frugal(const số_nguyên except[], size_t n_except);

số_nguyên pack_fds(số_nguyên fds[], size_t n);

số_nguyên fd_validate(số_nguyên fd);
số_nguyên same_fd(số_nguyên a, số_nguyên b);

bool fdname_is_valid(const char *s);

số_nguyên fd_get_path(số_nguyên fd, char **ret);

số_nguyên move_fd(số_nguyên from, số_nguyên to, số_nguyên cloexec);

số_nguyên fd_move_above_stdio(số_nguyên fd);

số_nguyên rearrange_stdio(số_nguyên original_input_fd, số_nguyên original_output_fd, số_nguyên original_error_fd);

static inline số_nguyên make_null_stdio(void) {
        trả_về rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, số_nguyên, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                số_nguyên *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

số_nguyên fd_reopen(số_nguyên fd, số_nguyên flags);
số_nguyên fd_reopen_propagate_append_and_position(số_nguyên fd, số_nguyên flags);
số_nguyên fd_reopen_condition(số_nguyên fd, số_nguyên flags, số_nguyên mask, số_nguyên *ret_new_fd);

số_nguyên fd_is_opath(số_nguyên fd);
số_nguyên fd_vet_accmode(số_nguyên fd, số_nguyên mode);
số_nguyên fd_is_writable(số_nguyên fd);

số_nguyên fd_verify_safe_flags_full(số_nguyên fd, số_nguyên extra_flags);
static inline số_nguyên fd_verify_safe_flags(số_nguyên fd) {
        trả_về fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(void);
số_nguyên fd_get_diskseq(số_nguyên fd, uint64_t *ret);

số_nguyên path_is_root_at(số_nguyên dir_fd, const char *path);
static inline số_nguyên path_is_root(const char *path) {
        trả_về path_is_root_at(AT_FDCWD, path);
}
static inline số_nguyên dir_fd_is_root(số_nguyên dir_fd) {
        trả_về dir_fd == XAT_FDROOT ? true : path_is_root_at(dir_fd, NULL);
}
static inline số_nguyên dir_fd_is_root_or_cwd(số_nguyên dir_fd) {
        trả_về IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_root_at(dir_fd, NULL);
}

số_nguyên fds_inode_and_mount_same(số_nguyên fd1, số_nguyên fd2);

số_nguyên resolve_xat_fdroot(số_nguyên *fd, const char **path, char **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(số_nguyên))

char* format_proc_fd_path(char buf[static PROC_FD_PATH_MAX], số_nguyên fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((char[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(số_nguyên))

char* format_proc_pid_fd_path(char buf[static PROC_PID_FD_PATH_MAX], pid_t pid, số_nguyên fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((char[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

số_nguyên proc_fd_enoent_errno(void);

const char* accmode_to_string(số_nguyên flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                số_nguyên _fd_ = (fd);                \
                assert(_fd_ >= 0);              \
                _fd_;                           \
        })
