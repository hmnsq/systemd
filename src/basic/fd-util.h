/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* maximum length of fdname */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FD_ਮੰਜ਼ਿਲ_PTR(fd) INT_ਮੰਜ਼ਿਲ_PTR((fd)+1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_FD(p) (PTR_ਮੰਜ਼ਿਲ_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EBADF_PAIR { -EBADF, -EBADF }
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#ਜੇ O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#ਨਹੀਂਤਾਂ
#  ਜੇ defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  ਨਹੀਂਤਾਂ
#    define RAW_O_LARGEFILE 00100000
#  endif
#ਜੇ_ਅੰਤ

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NR_OPEN_DEFAULT ((ਬਿਨਾਨਿਸ਼ਾਨੀ) (1024 * 1024))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NR_OPEN_MINIMUM ((ਬਿਨਾਨਿਸ਼ਾਨੀ) (ਆਕਾਰ(ਲੰਮਾ) * 8))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NR_OPEN_ਵੱਧ_ਤੋਂ_ਵੱਧ ((ਬਿਨਾਨਿਸ਼ਾਨੀ) (CONST_MIN((size_t) ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ, SIZE_MAX / __SIZEOF_POINTER__) & ~(ਆਕਾਰ(ਲੰਮਾ) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool wildcard_fd_is_valid(ਪੂਰਨ fd) {
        ਵਾਪਸ fd >= 0 || ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT);
}

ਪੂਰਨ close_nointr(ਪੂਰਨ fd);
ਪੂਰਨ ਸੁਰੱਖਿਅਤ_close(ਪੂਰਨ fd);
ਖਾਲੀ ਸੁਰੱਖਿਅਤ_close_pair(ਪੂਰਨ p[ਸਥਿਰ 2]);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_close_above_stdio(ਪੂਰਨ fd) {
        ਜੇ (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ਵਾਪਸ -EBADF;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_close(fd);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* close_fd_ptr(ਖਾਲੀ *p) {
        ਸੁਰੱਖਿਅਤ_close(PTR_ਮੰਜ਼ਿਲ_FD(p));
        ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ close_many(ਅਚਲ ਪੂਰਨ fds[], size_t n_fds);
ਖਾਲੀ close_many_unset(ਪੂਰਨ fds[], size_t n_fds);
ਖਾਲੀ close_many_and_free(ਪੂਰਨ *fds, size_t n_fds);

ਪੂਰਨ fclose_nointr(FILE *f);
FILE* ਸੁਰੱਖਿਅਤ_fclose(FILE *f);
DIR* ਸੁਰੱਖਿਅਤ_closedir(DIR *d);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ closep(ਪੂਰਨ *fd) {
        ਸੁਰੱਖਿਅਤ_close(*fd);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ close_pairp(ਪੂਰਨ (*p)[2]) {
        ਸੁਰੱਖਿਅਤ_close_pair(*p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ fclosep(FILE **f) {
        ਸੁਰੱਖਿਅਤ_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_ਭਰਿਆ(FILE*, pclose, ਨਿਰਾ);
DEFINE_TRIVIAL_CLEANUP_FUNC_ਭਰਿਆ(DIR*, closedir, ਨਿਰਾ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_close_ _cleanup_(closep)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_fclose_ _cleanup_(fclosep)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_pclose_ _cleanup_(pclosep)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_closedir_ _cleanup_(closedirp)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_close_pair_ _cleanup_(close_pairp)

ਪੂਰਨ fd_nonblock(ਪੂਰਨ fd, bool nonblock);
ਪੂਰਨ stdio_disable_nonblock(ਖਾਲੀ);

ਖਾਲੀ nonblock_resetp(ਪੂਰਨ *fd);

ਪੂਰਨ fd_cloexec(ਪੂਰਨ fd, bool cloexec);
ਪੂਰਨ fd_cloexec_many(ਅਚਲ ਪੂਰਨ fds[], size_t n_fds, bool cloexec);

ਪੂਰਨ get_max_fd(ਖਾਲੀ);

ਪੂਰਨ close_all_fds(ਅਚਲ ਪੂਰਨ except[], size_t n_except);
ਪੂਰਨ close_all_fds_without_malloc(ਅਚਲ ਪੂਰਨ except[], size_t n_except);
ਪੂਰਨ close_all_fds_frugal(ਅਚਲ ਪੂਰਨ except[], size_t n_except);

ਪੂਰਨ pack_fds(ਪੂਰਨ fds[], size_t n);

ਪੂਰਨ fd_validate(ਪੂਰਨ fd);
ਪੂਰਨ same_fd(ਪੂਰਨ a, ਪੂਰਨ b);

bool fdname_is_valid(ਅਚਲ ਅੱਖਰ *s);

ਪੂਰਨ fd_get_ਰਸਤਾ(ਪੂਰਨ fd, ਅੱਖਰ **ret);

ਪੂਰਨ move_fd(ਪੂਰਨ from, ਪੂਰਨ to, ਪੂਰਨ cloexec);

ਪੂਰਨ fd_move_above_stdio(ਪੂਰਨ fd);

ਪੂਰਨ rearrange_stdio(ਪੂਰਨ original_ਨਿਵੇਸ਼_fd, ਪੂਰਨ original_output_fd, ਪੂਰਨ original_error_fd);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ make_null_stdio(ਖਾਲੀ) {
        ਵਾਪਸ rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TAKE_FD(fd) TAKE_GENERIC(fd, ਪੂਰਨ, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ close_and_replace(a, b)                 \
        ({                                      \
                ਪੂਰਨ *_fdp_ = &(a);              \
                ਸੁਰੱਖਿਅਤ_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

ਪੂਰਨ fd_reopen(ਪੂਰਨ fd, ਪੂਰਨ flags);
ਪੂਰਨ fd_reopen_propagate_append_and_position(ਪੂਰਨ fd, ਪੂਰਨ flags);
ਪੂਰਨ fd_reopen_condition(ਪੂਰਨ fd, ਪੂਰਨ flags, ਪੂਰਨ ਨਕਾਬ_3, ਪੂਰਨ *ret_new_fd);

ਪੂਰਨ fd_is_opath(ਪੂਰਨ fd);
ਪੂਰਨ fd_vet_accmode(ਪੂਰਨ fd, ਪੂਰਨ ਬਹੁਲਕ);
ਪੂਰਨ fd_is_writable(ਪੂਰਨ fd);

ਪੂਰਨ fd_verify_ਸੁਰੱਖਿਅਤ_flags_ਭਰਿਆ(ਪੂਰਨ fd, ਪੂਰਨ extra_flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_verify_ਸੁਰੱਖਿਅਤ_flags(ਪੂਰਨ fd) {
        ਵਾਪਸ fd_verify_ਸੁਰੱਖਿਅਤ_flags_ਭਰਿਆ(fd, 0);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ read_nr_open(ਖਾਲੀ);
ਪੂਰਨ fd_get_diskseq(ਪੂਰਨ fd, uint64_t *ret);

ਪੂਰਨ ਰਸਤਾ_is_ਜੜ੍ਹ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_is_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਵਾਪਸ ਰਸਤਾ_is_ਜੜ੍ਹ_at(AT_FDCWD, ਰਸਤਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ dir_fd_is_ਜੜ੍ਹ(ਪੂਰਨ dir_fd) {
        ਵਾਪਸ dir_fd == XAT_FDROOT ? true : ਰਸਤਾ_is_ਜੜ੍ਹ_at(dir_fd, ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ dir_fd_is_ਜੜ੍ਹ_or_cwd(ਪੂਰਨ dir_fd) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : ਰਸਤਾ_is_ਜੜ੍ਹ_at(dir_fd, ਨਿਰਾ);
}

ਪੂਰਨ fds_inode_and_mount_same(ਪੂਰਨ fd1, ਪੂਰਨ fd2);

ਪੂਰਨ resolve_xat_fdroot(ਪੂਰਨ *fd, ਅਚਲ ਅੱਖਰ **ਰਸਤਾ, ਅੱਖਰ **ret_ਬਫਰ);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROC_FD_ਰਸਤਾ_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(ਪੂਰਨ))

ਅੱਖਰ* format_proc_fd_ਰਸਤਾ(ਅੱਖਰ buf[ਸਥਿਰ PROC_FD_ਰਸਤਾ_MAX], ਪੂਰਨ fd);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_PROC_FD_ਰਸਤਾ(fd) \
        format_proc_fd_ਰਸਤਾ((ਅੱਖਰ[PROC_FD_ਰਸਤਾ_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROC_PID_FD_ਰਸਤਾ_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(ਪੂਰਨ))

ਅੱਖਰ* format_proc_pid_fd_ਰਸਤਾ(ਅੱਖਰ buf[ਸਥਿਰ PROC_PID_FD_ਰਸਤਾ_MAX], pid_t pid, ਪੂਰਨ fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_PROC_PID_FD_ਰਸਤਾ(pid, fd)                                \
        format_proc_pid_fd_ਰਸਤਾ((ਅੱਖਰ[PROC_PID_FD_ਰਸਤਾ_MAX]) {}, (pid), (fd))

ਪੂਰਨ proc_fd_enoent_errno(ਖਾਲੀ);

ਅਚਲ ਅੱਖਰ* accmode_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ flags);

/* Like ASSERT_PTR, but for fds */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASSERT_FD(fd)                           \
        ({                                      \
                ਪੂਰਨ _fd_ = (fd);                \
                ਪ੍ਰਤਿਜ੍ਞਾ(_fd_ >= 0);              \
                _fd_;                           \
        })
