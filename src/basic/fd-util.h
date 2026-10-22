/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <dirent.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "forward.h"

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
#જો O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#નહીંતર
#  જો defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  નહીંતર
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
#define NR_OPEN_MINIMUM ((unsigned) (કદ(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(કદ(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
સ્થિર અંતઃરેખ bool wildcard_fd_is_valid(પૂર્ણાંક fd) {
        પાછા fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

પૂર્ણાંક close_nointr(પૂર્ણાંક fd);
પૂર્ણાંક safe_close(પૂર્ણાંક fd);
શૂન્ય safe_close_pair(પૂર્ણાંક p[સ્થિર 2]);

સ્થિર અંતઃરેખ પૂર્ણાંક safe_close_above_stdio(પૂર્ણાંક fd) {
        જો (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                પાછા -EBADF;

        પાછા safe_close(fd);
}

સ્થિર અંતઃરેખ શૂન્ય* close_fd_ptr(શૂન્ય *p) {
        safe_close(PTR_TO_FD(p));
        પાછા ખાલી;
}

શૂન્ય close_many(અચલ પૂર્ણાંક fds[], size_t n_fds);
શૂન્ય close_many_unset(પૂર્ણાંક fds[], size_t n_fds);
શૂન્ય close_many_and_free(પૂર્ણાંક *fds, size_t n_fds);

પૂર્ણાંક fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

સ્થિર અંતઃરેખ શૂન્ય closep(પૂર્ણાંક *fd) {
        safe_close(*fd);
}

સ્થિર અંતઃરેખ શૂન્ય close_pairp(પૂર્ણાંક (*p)[2]) {
        safe_close_pair(*p);
}

સ્થિર અંતઃરેખ શૂન્ય fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, ખાલી);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, ખાલી);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

પૂર્ણાંક fd_nonblock(પૂર્ણાંક fd, bool nonblock);
પૂર્ણાંક stdio_disable_nonblock(શૂન્ય);

શૂન્ય nonblock_resetp(પૂર્ણાંક *fd);

પૂર્ણાંક fd_cloexec(પૂર્ણાંક fd, bool cloexec);
પૂર્ણાંક fd_cloexec_many(અચલ પૂર્ણાંક fds[], size_t n_fds, bool cloexec);

પૂર્ણાંક get_max_fd(શૂન્ય);

પૂર્ણાંક close_all_fds(અચલ પૂર્ણાંક except[], size_t n_except);
પૂર્ણાંક close_all_fds_without_malloc(અચલ પૂર્ણાંક except[], size_t n_except);
પૂર્ણાંક close_all_fds_frugal(અચલ પૂર્ણાંક except[], size_t n_except);

પૂર્ણાંક pack_fds(પૂર્ણાંક fds[], size_t n);

પૂર્ણાંક fd_validate(પૂર્ણાંક fd);
પૂર્ણાંક same_fd(પૂર્ણાંક a, પૂર્ણાંક b);

bool fdname_is_valid(અચલ અક્ષર *s);

પૂર્ણાંક fd_get_path(પૂર્ણાંક fd, અક્ષર **ret);

પૂર્ણાંક move_fd(પૂર્ણાંક from, પૂર્ણાંક to, પૂર્ણાંક cloexec);

પૂર્ણાંક fd_move_above_stdio(પૂર્ણાંક fd);

પૂર્ણાંક rearrange_stdio(પૂર્ણાંક original_આગત_fd, પૂર્ણાંક original_output_fd, પૂર્ણાંક original_error_fd);

સ્થિર અંતઃરેખ પૂર્ણાંક make_null_stdio(શૂન્ય) {
        પાછા rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, પૂર્ણાંક, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                પૂર્ણાંક *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

પૂર્ણાંક fd_reopen(પૂર્ણાંક fd, પૂર્ણાંક flags);
પૂર્ણાંક fd_reopen_propagate_append_and_position(પૂર્ણાંક fd, પૂર્ણાંક flags);
પૂર્ણાંક fd_reopen_condition(પૂર્ણાંક fd, પૂર્ણાંક flags, પૂર્ણાંક mask, પૂર્ણાંક *ret_new_fd);

પૂર્ણાંક fd_is_opath(પૂર્ણાંક fd);
પૂર્ણાંક fd_vet_accmode(પૂર્ણાંક fd, પૂર્ણાંક mode);
પૂર્ણાંક fd_is_writable(પૂર્ણાંક fd);

પૂર્ણાંક fd_verify_safe_flags_full(પૂર્ણાંક fd, પૂર્ણાંક extra_flags);
સ્થિર અંતઃરેખ પૂર્ણાંક fd_verify_safe_flags(પૂર્ણાંક fd) {
        પાછા fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(શૂન્ય);
પૂર્ણાંક fd_get_diskseq(પૂર્ણાંક fd, uint64_t *ret);

પૂર્ણાંક path_is_મૂળ_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path);
સ્થિર અંતઃરેખ પૂર્ણાંક path_is_મૂળ(અચલ અક્ષર *path) {
        પાછા path_is_મૂળ_at(AT_FDCWD, path);
}
સ્થિર અંતઃરેખ પૂર્ણાંક dir_fd_is_મૂળ(પૂર્ણાંક dir_fd) {
        પાછા dir_fd == XAT_FDROOT ? true : path_is_મૂળ_at(dir_fd, ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક dir_fd_is_મૂળ_or_cwd(પૂર્ણાંક dir_fd) {
        પાછા IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_મૂળ_at(dir_fd, ખાલી);
}

પૂર્ણાંક fds_inode_and_mount_same(પૂર્ણાંક fd1, પૂર્ણાંક fd2);

પૂર્ણાંક resolve_xat_fdroot(પૂર્ણાંક *fd, અચલ અક્ષર **path, અક્ષર **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(પૂર્ણાંક))

અક્ષર* format_proc_fd_path(અક્ષર buf[સ્થિર PROC_FD_PATH_MAX], પૂર્ણાંક fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((અક્ષર[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(પૂર્ણાંક))

અક્ષર* format_proc_pid_fd_path(અક્ષર buf[સ્થિર PROC_PID_FD_PATH_MAX], pid_t pid, પૂર્ણાંક fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((અક્ષર[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

પૂર્ણાંક proc_fd_enoent_errno(શૂન્ય);

અચલ અક્ષર* accmode_to_string(પૂર્ણાંક flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                પૂર્ણાંક _fd_ = (fd);                \
                પ્રતિજ્ઞા(_fd_ >= 0);              \
                _fd_;                           \
        })
