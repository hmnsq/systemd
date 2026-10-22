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
#གལ་སྲིད O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#དེ་མིན
#  གལ་སྲིད defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  དེ་མིན
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
#define NR_OPEN_MINIMUM ((unsigned) (parimaa_na(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(parimaa_na(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
static anta_hstha bool wildcard_fd_is_valid(ཧྲིལ fd) {
        ལོག fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

ཧྲིལ close_nointr(ཧྲིལ fd);
ཧྲིལ safe_close(ཧྲིལ fd);
void safe_close_pair(ཧྲིལ p[static 2]);

static anta_hstha ཧྲིལ safe_close_above_stdio(ཧྲིལ fd) {
        གལ་སྲིད (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ལོག -EBADF;

        ལོག safe_close(fd);
}

static anta_hstha void* close_fd_ptr(void *p) {
        safe_close(PTR_TO_FD(p));
        ལོག NULL;
}

void close_many(const ཧྲིལ fds[], size_t n_fds);
void close_many_unset(ཧྲིལ fds[], size_t n_fds);
void close_many_and_free(ཧྲིལ *fds, size_t n_fds);

ཧྲིལ fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

static anta_hstha void closep(ཧྲིལ *fd) {
        safe_close(*fd);
}

static anta_hstha void close_pairp(ཧྲིལ (*p)[2]) {
        safe_close_pair(*p);
}

static anta_hstha void fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, NULL);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, NULL);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

ཧྲིལ fd_nonblock(ཧྲིལ fd, bool nonblock);
ཧྲིལ stdio_disable_nonblock(void);

void nonblock_resetp(ཧྲིལ *fd);

ཧྲིལ fd_cloexec(ཧྲིལ fd, bool cloexec);
ཧྲིལ fd_cloexec_many(const ཧྲིལ fds[], size_t n_fds, bool cloexec);

ཧྲིལ get_max_fd(void);

ཧྲིལ close_all_fds(const ཧྲིལ except[], size_t n_except);
ཧྲིལ close_all_fds_without_malloc(const ཧྲིལ except[], size_t n_except);
ཧྲིལ close_all_fds_frugal(const ཧྲིལ except[], size_t n_except);

ཧྲིལ pack_fds(ཧྲིལ fds[], size_t n);

ཧྲིལ fd_validate(ཧྲིལ fd);
ཧྲིལ same_fd(ཧྲིལ a, ཧྲིལ b);

bool fdname_is_valid(const var_na *s);

ཧྲིལ fd_get_path(ཧྲིལ fd, var_na **ret);

ཧྲིལ move_fd(ཧྲིལ from, ཧྲིལ to, ཧྲིལ cloexec);

ཧྲིལ fd_move_above_stdio(ཧྲིལ fd);

ཧྲིལ rearrange_stdio(ཧྲིལ original_input_fd, ཧྲིལ original_output_fd, ཧྲིལ original_error_fd);

static anta_hstha ཧྲིལ make_null_stdio(void) {
        ལོག rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, ཧྲིལ, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                ཧྲིལ *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

ཧྲིལ fd_reopen(ཧྲིལ fd, ཧྲིལ flags);
ཧྲིལ fd_reopen_propagate_append_and_position(ཧྲིལ fd, ཧྲིལ flags);
ཧྲིལ fd_reopen_condition(ཧྲིལ fd, ཧྲིལ flags, ཧྲིལ mask, ཧྲིལ *ret_new_fd);

ཧྲིལ fd_is_opath(ཧྲིལ fd);
ཧྲིལ fd_vet_accmode(ཧྲིལ fd, ཧྲིལ mode);
ཧྲིལ fd_is_writable(ཧྲིལ fd);

ཧྲིལ fd_verify_safe_flags_full(ཧྲིལ fd, ཧྲིལ extra_flags);
static anta_hstha ཧྲིལ fd_verify_safe_flags(ཧྲིལ fd) {
        ལོག fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(void);
ཧྲིལ fd_get_diskseq(ཧྲིལ fd, uint64_t *ret);

ཧྲིལ path_is_root_at(ཧྲིལ dir_fd, const var_na *path);
static anta_hstha ཧྲིལ path_is_root(const var_na *path) {
        ལོག path_is_root_at(AT_FDCWD, path);
}
static anta_hstha ཧྲིལ dir_fd_is_root(ཧྲིལ dir_fd) {
        ལོག dir_fd == XAT_FDROOT ? true : path_is_root_at(dir_fd, NULL);
}
static anta_hstha ཧྲིལ dir_fd_is_root_or_cwd(ཧྲིལ dir_fd) {
        ལོག IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_root_at(dir_fd, NULL);
}

ཧྲིལ fds_inode_and_mount_same(ཧྲིལ fd1, ཧྲིལ fd2);

ཧྲིལ resolve_xat_fdroot(ཧྲིལ *fd, const var_na **path, var_na **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(ཧྲིལ))

var_na* format_proc_fd_path(var_na buf[static PROC_FD_PATH_MAX], ཧྲིལ fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((var_na[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(ཧྲིལ))

var_na* format_proc_pid_fd_path(var_na buf[static PROC_PID_FD_PATH_MAX], pid_t pid, ཧྲིལ fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((var_na[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

ཧྲིལ proc_fd_enoent_errno(void);

const var_na* accmode_to_string(ཧྲིལ flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                ཧྲིལ _fd_ = (fd);                \
                assert(_fd_ >= 0);              \
                _fd_;                           \
        })
