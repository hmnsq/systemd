/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <dirent.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

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
#ଯଦି O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#ଅନ୍ୟଥା
#  ଯଦି defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  ଅନ୍ୟଥା
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
#define NR_OPEN_MINIMUM ((unsigned) (ଆକାର(long) * 8))
#define NR_OPEN_MAXIMUM ((unsigned) (CONST_MIN((size_t) INT_MAX, SIZE_MAX / __SIZEOF_POINTER__) & ~(ଆକାର(long) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool wildcard_fd_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଫେରନ୍ତୁ fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_nointr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ଶୂନ୍ୟ safe_close_pair(ପୂର୍ଣ୍ଣସଂଖ୍ୟା p[ସ୍ଥିର 2]);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_close_above_stdio(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                ଫେରନ୍ତୁ -EBADF;

        ଫେରନ୍ତୁ safe_close(fd);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* close_fd_ptr(ଶୂନ୍ୟ *p) {
        safe_close(PTR_TO_FD(p));
        ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ close_many(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds);
ଶୂନ୍ୟ close_many_unset(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds);
ଶୂନ୍ୟ close_many_and_free(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fds, size_t n_fds);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ closep(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd) {
        safe_close(*fd);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ close_pairp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*p)[2]) {
        safe_close_pair(*p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(FILE*, pclose, ଖାଲି);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(DIR*, closedir, ଖାଲି);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_nonblock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool nonblock);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା stdio_disable_nonblock(ଶୂନ୍ୟ);

ଶୂନ୍ୟ nonblock_resetp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_cloexec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool cloexec);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_cloexec_many(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n_fds, bool cloexec);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_max_fd(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds_without_malloc(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_all_fds_frugal(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା except[], size_t n_except);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pack_fds(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds[], size_t n);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_validate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା same_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b);

bool fdname_is_valid(ଅଚଳ ଅକ୍ଷର *s);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_get_path(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା move_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା from, ପୂର୍ଣ୍ଣସଂଖ୍ୟା to, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cloexec);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_move_above_stdio(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rearrange_stdio(ପୂର୍ଣ୍ଣସଂଖ୍ୟା original_ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା original_output_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା original_error_fd);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_null_stdio(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopen_propagate_append_and_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopen_condition(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_new_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_opath(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_vet_accmode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_writable(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_safe_flags_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା extra_flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_safe_flags(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଫେରନ୍ତୁ fd_verify_safe_flags_full(fd, 0);
}

unsigned read_nr_open(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_get_diskseq(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_ମୂଳ_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path) {
        ଫେରନ୍ତୁ path_is_ମୂଳ_at(AT_FDCWD, path);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd_is_ମୂଳ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd) {
        ଫେରନ୍ତୁ dir_fd == XAT_FDROOT ? true : path_is_ମୂଳ_at(dir_fd, ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd_is_ମୂଳ_or_cwd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd) {
        ଫେରନ୍ତୁ IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_ମୂଳ_at(dir_fd, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fds_inode_and_mount_same(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd2);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve_xat_fdroot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd, ଅଚଳ ଅକ୍ଷର **path, ଅକ୍ଷର **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))

ଅକ୍ଷର* format_proc_fd_path(ଅକ୍ଷର buf[ସ୍ଥିର PROC_FD_PATH_MAX], ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((ଅକ୍ଷର[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))

ଅକ୍ଷର* format_proc_pid_fd_path(ଅକ୍ଷର buf[ସ୍ଥିର PROC_PID_FD_PATH_MAX], pid_t pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((ଅକ୍ଷର[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_fd_enoent_errno(ଶୂନ୍ୟ);

ଅଚଳ ଅକ୍ଷର* accmode_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _fd_ = (fd);                \
                ପ୍ରତିଜ୍ଞା(_fd_ >= 0);              \
                _fd_;                           \
        })
