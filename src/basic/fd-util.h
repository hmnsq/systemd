/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <dirent.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "forward.h"

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
#എങ്കിൽ O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#അല്ലെങ്കിൽ
#  എങ്കിൽ defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  അല്ലെങ്കിൽ
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
#define NR_OPEN_MINIMUM ((unsigned) (വലുപ്പം(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(വലുപ്പം(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
സ്ഥിതം വരിയിൽ bool wildcard_fd_is_valid(പൂർണ്ണസംഖ്യ fd) {
        തിരികെ fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

പൂർണ്ണസംഖ്യ close_nointr(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ safe_close(പൂർണ്ണസംഖ്യ fd);
ശൂന്യം safe_close_pair(പൂർണ്ണസംഖ്യ p[സ്ഥിതം 2]);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ safe_close_above_stdio(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                തിരികെ -EBADF;

        തിരികെ safe_close(fd);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* close_fd_ptr(ശൂന്യം *p) {
        safe_close(PTR_TO_FD(p));
        തിരികെ ശൂന്യ;
}

ശൂന്യം close_many(സ്ഥിരം പൂർണ്ണസംഖ്യ fds[], size_t n_fds);
ശൂന്യം close_many_unset(പൂർണ്ണസംഖ്യ fds[], size_t n_fds);
ശൂന്യം close_many_and_free(പൂർണ്ണസംഖ്യ *fds, size_t n_fds);

പൂർണ്ണസംഖ്യ fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

സ്ഥിതം വരിയിൽ ശൂന്യം closep(പൂർണ്ണസംഖ്യ *fd) {
        safe_close(*fd);
}

സ്ഥിതം വരിയിൽ ശൂന്യം close_pairp(പൂർണ്ണസംഖ്യ (*p)[2]) {
        safe_close_pair(*p);
}

സ്ഥിതം വരിയിൽ ശൂന്യം fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, ശൂന്യ);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, ശൂന്യ);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

പൂർണ്ണസംഖ്യ fd_nonblock(പൂർണ്ണസംഖ്യ fd, bool nonblock);
പൂർണ്ണസംഖ്യ stdio_disable_nonblock(ശൂന്യം);

ശൂന്യം nonblock_resetp(പൂർണ്ണസംഖ്യ *fd);

പൂർണ്ണസംഖ്യ fd_cloexec(പൂർണ്ണസംഖ്യ fd, bool cloexec);
പൂർണ്ണസംഖ്യ fd_cloexec_many(സ്ഥിരം പൂർണ്ണസംഖ്യ fds[], size_t n_fds, bool cloexec);

പൂർണ്ണസംഖ്യ get_max_fd(ശൂന്യം);

പൂർണ്ണസംഖ്യ close_all_fds(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except);
പൂർണ്ണസംഖ്യ close_all_fds_without_malloc(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except);
പൂർണ്ണസംഖ്യ close_all_fds_frugal(സ്ഥിരം പൂർണ്ണസംഖ്യ except[], size_t n_except);

പൂർണ്ണസംഖ്യ pack_fds(പൂർണ്ണസംഖ്യ fds[], size_t n);

പൂർണ്ണസംഖ്യ fd_validate(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ same_fd(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b);

bool fdname_is_valid(സ്ഥിരം അക്ഷരം *s);

പൂർണ്ണസംഖ്യ fd_get_path(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ move_fd(പൂർണ്ണസംഖ്യ from, പൂർണ്ണസംഖ്യ to, പൂർണ്ണസംഖ്യ cloexec);

പൂർണ്ണസംഖ്യ fd_move_above_stdio(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ rearrange_stdio(പൂർണ്ണസംഖ്യ original_നിവേശം_fd, പൂർണ്ണസംഖ്യ original_output_fd, പൂർണ്ണസംഖ്യ original_error_fd);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ make_null_stdio(ശൂന്യം) {
        തിരികെ rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, പൂർണ്ണസംഖ്യ, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                പൂർണ്ണസംഖ്യ *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

പൂർണ്ണസംഖ്യ fd_reopen(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ fd_reopen_propagate_append_and_position(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ fd_reopen_condition(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ mask, പൂർണ്ണസംഖ്യ *ret_new_fd);

പൂർണ്ണസംഖ്യ fd_is_opath(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ fd_vet_accmode(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ mode);
പൂർണ്ണസംഖ്യ fd_is_writable(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ fd_verify_safe_flags_full(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ extra_flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_verify_safe_flags(പൂർണ്ണസംഖ്യ fd) {
        തിരികെ fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(ശൂന്യം);
പൂർണ്ണസംഖ്യ fd_get_diskseq(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);

പൂർണ്ണസംഖ്യ path_is_വേര്_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_is_വേര്(സ്ഥിരം അക്ഷരം *path) {
        തിരികെ path_is_വേര്_at(AT_FDCWD, path);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ dir_fd_is_വേര്(പൂർണ്ണസംഖ്യ dir_fd) {
        തിരികെ dir_fd == XAT_FDROOT ? true : path_is_വേര്_at(dir_fd, ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ dir_fd_is_വേര്_or_cwd(പൂർണ്ണസംഖ്യ dir_fd) {
        തിരികെ IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_വേര്_at(dir_fd, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ fds_inode_and_mount_same(പൂർണ്ണസംഖ്യ fd1, പൂർണ്ണസംഖ്യ fd2);

പൂർണ്ണസംഖ്യ resolve_xat_fdroot(പൂർണ്ണസംഖ്യ *fd, സ്ഥിരം അക്ഷരം **path, അക്ഷരം **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ))

അക്ഷരം* format_proc_fd_path(അക്ഷരം buf[സ്ഥിതം PROC_FD_PATH_MAX], പൂർണ്ണസംഖ്യ fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((അക്ഷരം[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ))

അക്ഷരം* format_proc_pid_fd_path(അക്ഷരം buf[സ്ഥിതം PROC_PID_FD_PATH_MAX], pid_t pid, പൂർണ്ണസംഖ്യ fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((അക്ഷരം[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

പൂർണ്ണസംഖ്യ proc_fd_enoent_errno(ശൂന്യം);

സ്ഥിരം അക്ഷരം* accmode_to_string(പൂർണ്ണസംഖ്യ flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                പൂർണ്ണസംഖ്യ _fd_ = (fd);                \
                പ്രതിജ്ഞാ(_fd_ >= 0);              \
                _fd_;                           \
        })
