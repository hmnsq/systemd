/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <dirent.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "forward.h"

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
#எனில் O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#இல்லையெனில்
#  எனில் defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  இல்லையெனில்
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
#define NR_OPEN_MINIMUM ((unsigned) (அளவு(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(அளவு(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
நிலையான உள்வரி bool wildcard_fd_is_valid(முழு fd) {
        திரும்பு fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

முழு close_nointr(முழு fd);
முழு safe_close(முழு fd);
வெறுமை safe_close_pair(முழு p[நிலையான 2]);

நிலையான உள்வரி முழு safe_close_above_stdio(முழு fd) {
        எனில் (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                திரும்பு -EBADF;

        திரும்பு safe_close(fd);
}

நிலையான உள்வரி வெறுமை* close_fd_ptr(வெறுமை *p) {
        safe_close(PTR_TO_FD(p));
        திரும்பு வெற்று;
}

வெறுமை close_many(மாறிலி முழு fds[], size_t n_fds);
வெறுமை close_many_unset(முழு fds[], size_t n_fds);
வெறுமை close_many_and_free(முழு *fds, size_t n_fds);

முழு fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

நிலையான உள்வரி வெறுமை closep(முழு *fd) {
        safe_close(*fd);
}

நிலையான உள்வரி வெறுமை close_pairp(முழு (*p)[2]) {
        safe_close_pair(*p);
}

நிலையான உள்வரி வெறுமை fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, வெற்று);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, வெற்று);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

முழு fd_nonblock(முழு fd, bool nonblock);
முழு stdio_disable_nonblock(வெறுமை);

வெறுமை nonblock_resetp(முழு *fd);

முழு fd_cloexec(முழு fd, bool cloexec);
முழு fd_cloexec_many(மாறிலி முழு fds[], size_t n_fds, bool cloexec);

முழு get_max_fd(வெறுமை);

முழு close_all_fds(மாறிலி முழு except[], size_t n_except);
முழு close_all_fds_without_malloc(மாறிலி முழு except[], size_t n_except);
முழு close_all_fds_frugal(மாறிலி முழு except[], size_t n_except);

முழு pack_fds(முழு fds[], size_t n);

முழு fd_validate(முழு fd);
முழு same_fd(முழு a, முழு b);

bool fdname_is_valid(மாறிலி எழுத்து *s);

முழு fd_get_path(முழு fd, எழுத்து **ret);

முழு move_fd(முழு from, முழு to, முழு cloexec);

முழு fd_move_above_stdio(முழு fd);

முழு rearrange_stdio(முழு original_உள்ளீடு_fd, முழு original_output_fd, முழு original_error_fd);

நிலையான உள்வரி முழு make_null_stdio(வெறுமை) {
        திரும்பு rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, முழு, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                முழு *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

முழு fd_reopen(முழு fd, முழு flags);
முழு fd_reopen_propagate_append_and_position(முழு fd, முழு flags);
முழு fd_reopen_condition(முழு fd, முழு flags, முழு mask, முழு *ret_new_fd);

முழு fd_is_opath(முழு fd);
முழு fd_vet_accmode(முழு fd, முழு mode);
முழு fd_is_writable(முழு fd);

முழு fd_verify_safe_flags_full(முழு fd, முழு extra_flags);
நிலையான உள்வரி முழு fd_verify_safe_flags(முழு fd) {
        திரும்பு fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(வெறுமை);
முழு fd_get_diskseq(முழு fd, uint64_t *ret);

முழு path_is_வேர்_at(முழு dir_fd, மாறிலி எழுத்து *path);
நிலையான உள்வரி முழு path_is_வேர்(மாறிலி எழுத்து *path) {
        திரும்பு path_is_வேர்_at(AT_FDCWD, path);
}
நிலையான உள்வரி முழு dir_fd_is_வேர்(முழு dir_fd) {
        திரும்பு dir_fd == XAT_FDROOT ? true : path_is_வேர்_at(dir_fd, வெற்று);
}
நிலையான உள்வரி முழு dir_fd_is_வேர்_or_cwd(முழு dir_fd) {
        திரும்பு IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_வேர்_at(dir_fd, வெற்று);
}

முழு fds_inode_and_mount_same(முழு fd1, முழு fd2);

முழு resolve_xat_fdroot(முழு *fd, மாறிலி எழுத்து **path, எழுத்து **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(முழு))

எழுத்து* format_proc_fd_path(எழுத்து buf[நிலையான PROC_FD_PATH_MAX], முழு fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((எழுத்து[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(முழு))

எழுத்து* format_proc_pid_fd_path(எழுத்து buf[நிலையான PROC_PID_FD_PATH_MAX], pid_t pid, முழு fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((எழுத்து[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

முழு proc_fd_enoent_errno(வெறுமை);

மாறிலி எழுத்து* accmode_to_string(முழு flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                முழு _fd_ = (fd);                \
                ப்ரதிஜ்ஞா(_fd_ >= 0);              \
                _fd_;                           \
        })
