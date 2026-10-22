/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

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
#ಯದಿ O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#ಅನ್ಯಥಾ
#  ಯದಿ defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  ಅನ್ಯಥಾ
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
#define NR_OPEN_MINIMUM ((unsigned) (ಪರಿಮಾಣ(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(ಪರಿಮಾಣ(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool wildcard_fd_is_valid(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತ್ಯಾಗಮ fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

ಪೂರ್ಣಾಙ್ಕ close_nointr(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ safe_close(ಪೂರ್ಣಾಙ್ಕ fd);
ಶೂನ್ಯ safe_close_pair(ಪೂರ್ಣಾಙ್ಕ p[ಸ್ಥಿರ 2]);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ safe_close_above_stdio(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಪ್ರತ್ಯಾಗಮ safe_close(fd);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* close_fd_ptr(ಶೂನ್ಯ *p) {
        safe_close(PTR_TO_FD(p));
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ close_many(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds);
ಶೂನ್ಯ close_many_unset(ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds);
ಶೂನ್ಯ close_many_and_free(ಪೂರ್ಣಾಙ್ಕ *fds, size_t n_fds);

ಪೂರ್ಣಾಙ್ಕ fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ closep(ಪೂರ್ಣಾಙ್ಕ *fd) {
        safe_close(*fd);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ close_pairp(ಪೂರ್ಣಾಙ್ಕ (*p)[2]) {
        safe_close_pair(*p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, ರಿಕ್ತ);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, ರಿಕ್ತ);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

ಪೂರ್ಣಾಙ್ಕ fd_nonblock(ಪೂರ್ಣಾಙ್ಕ fd, bool nonblock);
ಪೂರ್ಣಾಙ್ಕ stdio_disable_nonblock(ಶೂನ್ಯ);

ಶೂನ್ಯ nonblock_resetp(ಪೂರ್ಣಾಙ್ಕ *fd);

ಪೂರ್ಣಾಙ್ಕ fd_cloexec(ಪೂರ್ಣಾಙ್ಕ fd, bool cloexec);
ಪೂರ್ಣಾಙ್ಕ fd_cloexec_many(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ fds[], size_t n_fds, bool cloexec);

ಪೂರ್ಣಾಙ್ಕ get_max_fd(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ close_all_fds(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except);
ಪೂರ್ಣಾಙ್ಕ close_all_fds_without_malloc(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except);
ಪೂರ್ಣಾಙ್ಕ close_all_fds_frugal(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ except[], size_t n_except);

ಪೂರ್ಣಾಙ್ಕ pack_fds(ಪೂರ್ಣಾಙ್ಕ fds[], size_t n);

ಪೂರ್ಣಾಙ್ಕ fd_validate(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ same_fd(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b);

bool fdname_is_valid(ಧ್ರುವ ವರ್ಣ *s);

ಪೂರ್ಣಾಙ್ಕ fd_get_path(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ move_fd(ಪೂರ್ಣಾಙ್ಕ from, ಪೂರ್ಣಾಙ್ಕ to, ಪೂರ್ಣಾಙ್ಕ cloexec);

ಪೂರ್ಣಾಙ್ಕ fd_move_above_stdio(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ rearrange_stdio(ಪೂರ್ಣಾಙ್ಕ original_ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ original_output_fd, ಪೂರ್ಣಾಙ್ಕ original_error_fd);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ make_null_stdio(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, ಪೂರ್ಣಾಙ್ಕ, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                ಪೂರ್ಣಾಙ್ಕ *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

ಪೂರ್ಣಾಙ್ಕ fd_reopen(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ flags);
ಪೂರ್ಣಾಙ್ಕ fd_reopen_propagate_append_and_position(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ flags);
ಪೂರ್ಣಾಙ್ಕ fd_reopen_condition(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ mask, ಪೂರ್ಣಾಙ್ಕ *ret_new_fd);

ಪೂರ್ಣಾಙ್ಕ fd_is_opath(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ fd_vet_accmode(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ mode);
ಪೂರ್ಣಾಙ್ಕ fd_is_writable(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ fd_verify_safe_flags_full(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ extra_flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fd_verify_safe_flags(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತ್ಯಾಗಮ fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ fd_get_diskseq(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);

ಪೂರ್ಣಾಙ್ಕ path_is_ಬೇರು_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ path_is_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತ್ಯಾಗಮ path_is_ಬೇರು_at(AT_FDCWD, path);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ dir_fd_is_ಬೇರು(ಪೂರ್ಣಾಙ್ಕ dir_fd) {
        ಪ್ರತ್ಯಾಗಮ dir_fd == XAT_FDROOT ? true : path_is_ಬೇರು_at(dir_fd, ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ dir_fd_is_ಬೇರು_or_cwd(ಪೂರ್ಣಾಙ್ಕ dir_fd) {
        ಪ್ರತ್ಯಾಗಮ IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_ಬೇರು_at(dir_fd, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ fds_inode_and_mount_same(ಪೂರ್ಣಾಙ್ಕ fd1, ಪೂರ್ಣಾಙ್ಕ fd2);

ಪೂರ್ಣಾಙ್ಕ resolve_xat_fdroot(ಪೂರ್ಣಾಙ್ಕ *fd, ಧ್ರುವ ವರ್ಣ **path, ವರ್ಣ **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ))

ವರ್ಣ* format_proc_fd_path(ವರ್ಣ buf[ಸ್ಥಿರ PROC_FD_PATH_MAX], ಪೂರ್ಣಾಙ್ಕ fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((ವರ್ಣ[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ))

ವರ್ಣ* format_proc_pid_fd_path(ವರ್ಣ buf[ಸ್ಥಿರ PROC_PID_FD_PATH_MAX], pid_t pid, ಪೂರ್ಣಾಙ್ಕ fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((ವರ್ಣ[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

ಪೂರ್ಣಾಙ್ಕ proc_fd_enoent_errno(ಶೂನ್ಯ);

ಧ್ರುವ ವರ್ಣ* accmode_to_string(ಪೂರ್ಣಾಙ್ಕ flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                ಪೂರ್ಣಾಙ್ಕ _fd_ = (fd);                \
                ಪ್ರತಿಜ್ಞಾ(_fd_ >= 0);              \
                _fd_;                           \
        })
