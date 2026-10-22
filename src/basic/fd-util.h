/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <dirent.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "forward.h"

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
#जर O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#अन्यथा
#  जर defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  अन्यथा
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
#define NR_OPEN_DEFAULT ((अचिन्हित) (1024 * 1024))
#define NR_OPEN_MINIMUM ((अचिन्हित) (आकार(दीर्घ) * 8))
#define NR_OPEN_MAXIMUM ((अचिन्हित) (CONST_MIN((माप_प्रकार) पूर्णांक_उच्च, SIZE_MAX / __SIZEOF_POINTER__) & ~(आकार(दीर्घ) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#define XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
स्थिर ओळीत bool wildcard_fd_is_valid(पूर्णांक fd) {
        परत fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

पूर्णांक close_nointr(पूर्णांक fd);
पूर्णांक safe_close(पूर्णांक fd);
शून्य safe_close_pair(पूर्णांक p[स्थिर 2]);

स्थिर ओळीत पूर्णांक safe_close_above_stdio(पूर्णांक fd) {
        जर (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                परत -EBADF;

        परत safe_close(fd);
}

स्थिर ओळीत शून्य* close_fd_ptr(शून्य *p) {
        safe_close(PTR_TO_FD(p));
        परत रिक्त;
}

शून्य close_many(स्थिरांक पूर्णांक fds[], माप_प्रकार n_fds);
शून्य close_many_unset(पूर्णांक fds[], माप_प्रकार n_fds);
शून्य close_many_and_free(पूर्णांक *fds, माप_प्रकार n_fds);

पूर्णांक fclose_nointr(खाता *f);
खाता* safe_fclose(खाता *f);
सूची* safe_closedir(सूची *d);

स्थिर ओळीत शून्य closep(पूर्णांक *fd) {
        safe_close(*fd);
}

स्थिर ओळीत शून्य close_pairp(पूर्णांक (*p)[2]) {
        safe_close_pair(*p);
}

स्थिर ओळीत शून्य fclosep(खाता **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(खाता*, pclose, रिक्त);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(सूची*, बंद_सूची, रिक्त);

#define _cleanup_close_ _cleanup_(closep)
#define _cleanup_fclose_ _cleanup_(fclosep)
#define _cleanup_pclose_ _cleanup_(pclosep)
#define _cleanup_closedir_ _cleanup_(closedirp)
#define _cleanup_close_pair_ _cleanup_(close_pairp)

पूर्णांक fd_nonblock(पूर्णांक fd, bool nonblock);
पूर्णांक stdio_disable_nonblock(शून्य);

शून्य nonblock_resetp(पूर्णांक *fd);

पूर्णांक fd_cloexec(पूर्णांक fd, bool cloexec);
पूर्णांक fd_cloexec_many(स्थिरांक पूर्णांक fds[], माप_प्रकार n_fds, bool cloexec);

पूर्णांक get_max_fd(शून्य);

पूर्णांक close_all_fds(स्थिरांक पूर्णांक except[], माप_प्रकार n_except);
पूर्णांक close_all_fds_without_malloc(स्थिरांक पूर्णांक except[], माप_प्रकार n_except);
पूर्णांक close_all_fds_frugal(स्थिरांक पूर्णांक except[], माप_प्रकार n_except);

पूर्णांक pack_fds(पूर्णांक fds[], माप_प्रकार n);

पूर्णांक fd_validate(पूर्णांक fd);
पूर्णांक same_fd(पूर्णांक a, पूर्णांक b);

bool fdname_is_valid(स्थिरांक अक्षर *s);

पूर्णांक fd_get_path(पूर्णांक fd, अक्षर **ret);

पूर्णांक move_fd(पूर्णांक from, पूर्णांक to, पूर्णांक cloexec);

पूर्णांक fd_move_above_stdio(पूर्णांक fd);

पूर्णांक rearrange_stdio(पूर्णांक original_आदान_fd, पूर्णांक original_output_fd, पूर्णांक original_error_fd);

स्थिर ओळीत पूर्णांक make_null_stdio(शून्य) {
        परत rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#define TAKE_FD(fd) TAKE_GENERIC(fd, पूर्णांक, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#define close_and_replace(a, b)                 \
        ({                                      \
                पूर्णांक *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

पूर्णांक fd_reopen(पूर्णांक fd, पूर्णांक flags);
पूर्णांक fd_reopen_propagate_append_and_position(पूर्णांक fd, पूर्णांक flags);
पूर्णांक fd_reopen_condition(पूर्णांक fd, पूर्णांक flags, पूर्णांक mask, पूर्णांक *ret_new_fd);

पूर्णांक fd_is_opath(पूर्णांक fd);
पूर्णांक fd_vet_accmode(पूर्णांक fd, पूर्णांक mode);
पूर्णांक fd_is_writable(पूर्णांक fd);

पूर्णांक fd_verify_safe_flags_full(पूर्णांक fd, पूर्णांक extra_flags);
स्थिर ओळीत पूर्णांक fd_verify_safe_flags(पूर्णांक fd) {
        परत fd_verify_safe_flags_full(fd, 0);
}

अचिन्हित read_nr_open(शून्य);
पूर्णांक fd_get_diskseq(पूर्णांक fd, uint64_t *ret);

पूर्णांक path_is_मूळ_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path);
स्थिर ओळीत पूर्णांक path_is_मूळ(स्थिरांक अक्षर *path) {
        परत path_is_मूळ_at(AT_FDCWD, path);
}
स्थिर ओळीत पूर्णांक dir_fd_is_मूळ(पूर्णांक dir_fd) {
        परत dir_fd == XAT_FDROOT ? true : path_is_मूळ_at(dir_fd, रिक्त);
}
स्थिर ओळीत पूर्णांक dir_fd_is_मूळ_or_cwd(पूर्णांक dir_fd) {
        परत IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_मूळ_at(dir_fd, रिक्त);
}

पूर्णांक fds_inode_and_mount_same(पूर्णांक fd1, पूर्णांक fd2);

पूर्णांक resolve_xat_fdroot(पूर्णांक *fd, स्थिरांक अक्षर **path, अक्षर **ret_buffer);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#define PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(पूर्णांक))

अक्षर* format_proc_fd_path(अक्षर buf[स्थिर PROC_FD_PATH_MAX], पूर्णांक fd);

#define FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((अक्षर[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#define PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(पूर्णांक))

अक्षर* format_proc_pid_fd_path(अक्षर buf[स्थिर PROC_PID_FD_PATH_MAX], pid_t pid, पूर्णांक fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#define FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((अक्षर[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

पूर्णांक proc_fd_enoent_errno(शून्य);

स्थिरांक अक्षर* accmode_to_string(पूर्णांक flags);

/* Like ASSERT_PTR, but for fds */
#define ASSERT_FD(fd)                           \
        ({                                      \
                पूर्णांक _fd_ = (fd);                \
                प्रतिज्ञा(_fd_ >= 0);              \
                _fd_;                           \
        })
