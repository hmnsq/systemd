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
#ᮚᮦᮔ᮪ O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#ᮜᮤᮚ
#  ᮚᮦᮔ᮪ defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  ᮜᮤᮚ
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
static anta_hstha bool wildcard_fd_is_valid(puur_naa_nka fd) {
        ᮝᮜᮤ fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

puur_naa_nka close_nointr(puur_naa_nka fd);
puur_naa_nka safe_close(puur_naa_nka fd);
void safe_close_pair(puur_naa_nka p[static 2]);

static anta_hstha puur_naa_nka safe_close_above_stdio(puur_naa_nka fd) {
        ᮚᮦᮔ᮪ (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ᮝᮜᮤ -EBADF;

        ᮝᮜᮤ safe_close(fd);
}

static anta_hstha void* close_fd_ptr(void *p) {
        safe_close(PTR_TO_FD(p));
        ᮝᮜᮤ NULL;
}

void close_many(const puur_naa_nka fds[], size_t n_fds);
void close_many_unset(puur_naa_nka fds[], size_t n_fds);
void close_many_and_free(puur_naa_nka *fds, size_t n_fds);

puur_naa_nka fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

static anta_hstha void closep(puur_naa_nka *fd) {
        safe_close(*fd);
}

static anta_hstha void close_pairp(puur_naa_nka (*p)[2]) {
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

puur_naa_nka fd_nonblock(puur_naa_nka fd, bool nonblock);
puur_naa_nka stdio_disable_nonblock(void);

void nonblock_resetp(puur_naa_nka *fd);

puur_naa_nka fd_cloexec(puur_naa_nka fd, bool cloexec);
puur_naa_nka fd_cloexec_many(const puur_naa_nka fds[], size_t n_fds, bool cloexec);

puur_naa_nka get_max_fd(void);

puur_naa_nka close_all_fds(const puur_naa_nka except[], size_t n_except);
puur_naa_nka close_all_fds_without_malloc(const puur_naa_nka except[], size_t n_except);
puur_naa_nka close_all_fds_frugal(const puur_naa_nka except[], size_t n_except);

puur_naa_nka pack_fds(puur_naa_nka fds[], size_t n);

puur_naa_nka fd_validate(puur_naa_nka fd);
puur_naa_nka same_fd(puur_naa_nka a, puur_naa_nka b);

bool fdname_is_valid(const var_na *s);

puur_naa_nka fd_get_path(puur_naa_nka fd, var_na **ret);

puur_naa_nka move_fd(puur_naa_nka from, puur_naa_nka to, puur_naa_nka cloexec);

puur_naa_nka fd_move_above_stdio(puur_naa_nka fd);

puur_naa_nka rearrange_stdio(puur_naa_nka original_input_fd, puur_naa_nka original_output_fd, puur_naa_nka original_error_fd);

static anta_hstha puur_naa_nka make_null_stdio(void) {
        ᮝᮜᮤ rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, puur_naa_nka, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                puur_naa_nka *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

puur_naa_nka fd_reopen(puur_naa_nka fd, puur_naa_nka flags);
puur_naa_nka fd_reopen_propagate_append_and_position(puur_naa_nka fd, puur_naa_nka flags);
puur_naa_nka fd_reopen_condition(puur_naa_nka fd, puur_naa_nka flags, puur_naa_nka mask, puur_naa_nka *ret_new_fd);

puur_naa_nka fd_is_opath(puur_naa_nka fd);
puur_naa_nka fd_vet_accmode(puur_naa_nka fd, puur_naa_nka mode);
puur_naa_nka fd_is_writable(puur_naa_nka fd);

puur_naa_nka fd_verify_safe_flags_full(puur_naa_nka fd, puur_naa_nka extra_flags);
static anta_hstha puur_naa_nka fd_verify_safe_flags(puur_naa_nka fd) {
        ᮝᮜᮤ fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(void);
puur_naa_nka fd_get_diskseq(puur_naa_nka fd, uint64_t *ret);

puur_naa_nka path_is_root_at(puur_naa_nka dir_fd, const var_na *path);
static anta_hstha puur_naa_nka path_is_root(const var_na *path) {
        ᮝᮜᮤ path_is_root_at(AT_FDCWD, path);
}
static anta_hstha puur_naa_nka dir_fd_is_root(puur_naa_nka dir_fd) {
        ᮝᮜᮤ dir_fd == XAT_FDROOT ? true : path_is_root_at(dir_fd, NULL);
}
static anta_hstha puur_naa_nka dir_fd_is_root_or_cwd(puur_naa_nka dir_fd) {
        ᮝᮜᮤ IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_root_at(dir_fd, NULL);
}

puur_naa_nka fds_inode_and_mount_same(puur_naa_nka fd1, puur_naa_nka fd2);

puur_naa_nka resolve_xat_fdroot(puur_naa_nka *fd, const var_na **path, var_na **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(puur_naa_nka))

var_na* format_proc_fd_path(var_na buf[static PROC_FD_PATH_MAX], puur_naa_nka fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((var_na[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(puur_naa_nka))

var_na* format_proc_pid_fd_path(var_na buf[static PROC_PID_FD_PATH_MAX], pid_t pid, puur_naa_nka fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((var_na[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

puur_naa_nka proc_fd_enoent_errno(void);

const var_na* accmode_to_string(puur_naa_nka flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                puur_naa_nka _fd_ = (fd);                \
                assert(_fd_ >= 0);              \
                _fd_;                           \
        })
