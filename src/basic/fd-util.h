/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <dirent.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

/* maximum length of fdname */
#ವ್ಯಾಖ್ಯಾನಿಸು FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#ವ್ಯಾಖ್ಯಾನಿಸು FD_ಗಮ್ಯ_PTR(fd) INT_ಗಮ್ಯ_PTR((fd)+1)
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_FD(p) (PTR_ಗಮ್ಯ_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#ವ್ಯಾಖ್ಯಾನಿಸು EBADF_PAIR { -EBADF, -EBADF }
#ವ್ಯಾಖ್ಯಾನಿಸು EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#ಆದರೆ O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  ಇಲ್ಲದಿದ್ದರೆ
#    define RAW_O_LARGEFILE 00100000
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#ವ್ಯಾಖ್ಯಾನಿಸು O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#ವ್ಯಾಖ್ಯಾನಿಸು NR_OPEN_DEFAULT ((ಚಿಹ್ನೆರಹಿತ) (1024 * 1024))
#ವ್ಯಾಖ್ಯಾನಿಸು NR_OPEN_MINIMUM ((ಚಿಹ್ನೆರಹಿತ) (ಗಾತ್ರ(ದೀರ್ಘ) * 8))
#ವ್ಯಾಖ್ಯಾನಿಸು NR_OPEN_ಗರಿಷ್ಠ ((ಚಿಹ್ನೆರಹಿತ) (CONST_MIN((size_t) ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ, SIZE_MAX / __SIZEOF_POINTER__) & ~(ಗಾತ್ರ(ದೀರ್ಘ) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#ವ್ಯಾಖ್ಯಾನಿಸು XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool wildcard_fd_is_valid(ಪೂರ್ಣಾಂಕ fd) {
        ಹಿಂತಿರುಗಿ fd >= 0 || ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT);
}

ಪೂರ್ಣಾಂಕ close_nointr(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_close(ಪೂರ್ಣಾಂಕ fd);
ಶೂನ್ಯ ಸುರಕ್ಷಿತ_close_pair(ಪೂರ್ಣಾಂಕ p[ಸ್ಥಿರ 2]);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_close_above_stdio(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ಹಿಂತಿರುಗಿ -EBADF;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_close(fd);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* close_fd_ptr(ಶೂನ್ಯ *p) {
        ಸುರಕ್ಷಿತ_close(PTR_ಗಮ್ಯ_FD(p));
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ close_many(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fds[], size_t n_fds);
ಶೂನ್ಯ close_many_unset(ಪೂರ್ಣಾಂಕ fds[], size_t n_fds);
ಶೂನ್ಯ close_many_and_free(ಪೂರ್ಣಾಂಕ *fds, size_t n_fds);

ಪೂರ್ಣಾಂಕ fclose_nointr(FILE *f);
FILE* ಸುರಕ್ಷಿತ_fclose(FILE *f);
DIR* ಸುರಕ್ಷಿತ_closedir(DIR *d);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ closep(ಪೂರ್ಣಾಂಕ *fd) {
        ಸುರಕ್ಷಿತ_close(*fd);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ close_pairp(ಪೂರ್ಣಾಂಕ (*p)[2]) {
        ಸುರಕ್ಷಿತ_close_pair(*p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ fclosep(FILE **f) {
        ಸುರಕ್ಷಿತ_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_ತುಂಬಿದ(FILE*, pclose, ಖಾಲಿ);
DEFINE_TRIVIAL_CLEANUP_FUNC_ತುಂಬಿದ(DIR*, closedir, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_close_ _cleanup_(closep)
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_fclose_ _cleanup_(fclosep)
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_pclose_ _cleanup_(pclosep)
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_closedir_ _cleanup_(closedirp)
#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_close_pair_ _cleanup_(close_pairp)

ಪೂರ್ಣಾಂಕ fd_nonblock(ಪೂರ್ಣಾಂಕ fd, bool nonblock);
ಪೂರ್ಣಾಂಕ stdio_disable_nonblock(ಶೂನ್ಯ);

ಶೂನ್ಯ nonblock_resetp(ಪೂರ್ಣಾಂಕ *fd);

ಪೂರ್ಣಾಂಕ fd_cloexec(ಪೂರ್ಣಾಂಕ fd, bool cloexec);
ಪೂರ್ಣಾಂಕ fd_cloexec_many(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ fds[], size_t n_fds, bool cloexec);

ಪೂರ್ಣಾಂಕ get_max_fd(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ close_all_fds(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except);
ಪೂರ್ಣಾಂಕ close_all_fds_without_malloc(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except);
ಪೂರ್ಣಾಂಕ close_all_fds_frugal(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ except[], size_t n_except);

ಪೂರ್ಣಾಂಕ pack_fds(ಪೂರ್ಣಾಂಕ fds[], size_t n);

ಪೂರ್ಣಾಂಕ fd_validate(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ same_fd(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b);

bool fdname_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

ಪೂರ್ಣಾಂಕ fd_get_ಮಾರ್ಗ(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ move_fd(ಪೂರ್ಣಾಂಕ from, ಪೂರ್ಣಾಂಕ to, ಪೂರ್ಣಾಂಕ cloexec);

ಪೂರ್ಣಾಂಕ fd_move_above_stdio(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ rearrange_stdio(ಪೂರ್ಣಾಂಕ original_ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ original_output_fd, ಪೂರ್ಣಾಂಕ original_error_fd);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ make_null_stdio(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#ವ್ಯಾಖ್ಯಾನಿಸು TAKE_FD(fd) TAKE_GENERIC(fd, ಪೂರ್ಣಾಂಕ, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#ವ್ಯಾಖ್ಯಾನಿಸು close_and_replace(a, b)                 \
        ({                                      \
                ಪೂರ್ಣಾಂಕ *_fdp_ = &(a);              \
                ಸುರಕ್ಷಿತ_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

ಪೂರ್ಣಾಂಕ fd_reopen(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ fd_reopen_propagate_append_and_position(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ fd_reopen_condition(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ_3, ಪೂರ್ಣಾಂಕ *ret_new_fd);

ಪೂರ್ಣಾಂಕ fd_is_opath(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ fd_vet_accmode(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ fd_is_writable(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ fd_verify_ಸುರಕ್ಷಿತ_flags_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ extra_flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_verify_ಸುರಕ್ಷಿತ_flags(ಪೂರ್ಣಾಂಕ fd) {
        ಹಿಂತಿರುಗಿ fd_verify_ಸುರಕ್ಷಿತ_flags_ತುಂಬಿದ(fd, 0);
}

ಚಿಹ್ನೆರಹಿತ read_nr_open(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ fd_get_diskseq(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_ಬೇರು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_ಬೇರು_at(AT_FDCWD, ಮಾರ್ಗ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ dir_fd_is_ಬೇರು(ಪೂರ್ಣಾಂಕ dir_fd) {
        ಹಿಂತಿರುಗಿ dir_fd == XAT_FDROOT ? true : ಮಾರ್ಗ_is_ಬೇರು_at(dir_fd, ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ dir_fd_is_ಬೇರು_or_cwd(ಪೂರ್ಣಾಂಕ dir_fd) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : ಮಾರ್ಗ_is_ಬೇರು_at(dir_fd, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ fds_inode_and_mount_same(ಪೂರ್ಣಾಂಕ fd1, ಪೂರ್ಣಾಂಕ fd2);

ಪೂರ್ಣಾಂಕ resolve_xat_fdroot(ಪೂರ್ಣಾಂಕ *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮಾರ್ಗ, ಅಕ್ಷರ **ret_ಬಫರ್);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#ವ್ಯಾಖ್ಯಾನಿಸು PROC_FD_ಮಾರ್ಗ_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ))

ಅಕ್ಷರ* format_proc_fd_ಮಾರ್ಗ(ಅಕ್ಷರ buf[ಸ್ಥಿರ PROC_FD_ಮಾರ್ಗ_MAX], ಪೂರ್ಣಾಂಕ fd);

#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_PROC_FD_ಮಾರ್ಗ(fd) \
        format_proc_fd_ಮಾರ್ಗ((ಅಕ್ಷರ[PROC_FD_ಮಾರ್ಗ_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#ವ್ಯಾಖ್ಯಾನಿಸು PROC_PID_FD_ಮಾರ್ಗ_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ))

ಅಕ್ಷರ* format_proc_pid_fd_ಮಾರ್ಗ(ಅಕ್ಷರ buf[ಸ್ಥಿರ PROC_PID_FD_ಮಾರ್ಗ_MAX], pid_t pid, ಪೂರ್ಣಾಂಕ fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_PROC_PID_FD_ಮಾರ್ಗ(pid, fd)                                \
        format_proc_pid_fd_ಮಾರ್ಗ((ಅಕ್ಷರ[PROC_PID_FD_ಮಾರ್ಗ_MAX]) {}, (pid), (fd))

ಪೂರ್ಣಾಂಕ proc_fd_enoent_errno(ಶೂನ್ಯ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* accmode_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ flags);

/* Like ASSERT_PTR, but for fds */
#ವ್ಯಾಖ್ಯಾನಿಸು ASSERT_FD(fd)                           \
        ({                                      \
                ಪೂರ್ಣಾಂಕ _fd_ = (fd);                \
                ಪ್ರತಿಜ್ಞಾ(_fd_ >= 0);              \
                _fd_;                           \
        })
