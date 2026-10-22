/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <dirent.h>
#समावेश <fcntl.h>
#समावेश <stdio.h>

#समावेश "forward.h"

/* maximum length of fdname */
#घोषणा FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#घोषणा FD_TO_PTR(fd) INT_TO_PTR((fd)+1)
#घोषणा PTR_TO_FD(p) (PTR_TO_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#घोषणा EBADF_PAIR { -EBADF, -EBADF }
#घोषणा EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#अगर O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#अन्यथा
#  अगर defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
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
#पूर्ण_अगर

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#घोषणा O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#घोषणा NR_OPEN_DEFAULT ((अचिन्हित) (1024 * 1024))
#घोषणा NR_OPEN_MINIMUM ((अचिन्हित) (माप(दीर्घ) * 8))
#घोषणा NR_OPEN_MAXIMUM ((अचिन्हित) (CONST_MIN((माप_प्रकार) पूर्णांक_उच्च, SIZE_MAX / __SIZEOF_POINTER__) & ~(माप(दीर्घ) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#घोषणा XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
अटल अंतरभूत bool wildcard_fd_is_valid(पूर्णांक fd) {
        वापस fd >= 0 || IN_SET(fd, AT_FDCWD, XAT_FDROOT);
}

पूर्णांक close_nointr(पूर्णांक fd);
पूर्णांक safe_close(पूर्णांक fd);
व्योम safe_close_pair(पूर्णांक p[अटल 2]);

अटल अंतरभूत पूर्णांक safe_close_above_stdio(पूर्णांक fd) {
        अगर (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                वापस -EBADF;

        वापस safe_close(fd);
}

अटल अंतरभूत व्योम* close_fd_ptr(व्योम *p) {
        safe_close(PTR_TO_FD(p));
        वापस शून्य;
}

व्योम close_many(स्थिर पूर्णांक fds[], माप_प्रकार n_fds);
व्योम close_many_unset(पूर्णांक fds[], माप_प्रकार n_fds);
व्योम close_many_and_free(पूर्णांक *fds, माप_प्रकार n_fds);

पूर्णांक fclose_nointr(खाता *f);
खाता* safe_fclose(खाता *f);
सूची* safe_closedir(सूची *d);

अटल अंतरभूत व्योम closep(पूर्णांक *fd) {
        safe_close(*fd);
}

अटल अंतरभूत व्योम close_pairp(पूर्णांक (*p)[2]) {
        safe_close_pair(*p);
}

अटल अंतरभूत व्योम fclosep(खाता **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(खाता*, pclose, शून्य);
DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(सूची*, बंद_सूची, शून्य);

#घोषणा _cleanup_close_ _cleanup_(closep)
#घोषणा _cleanup_fclose_ _cleanup_(fclosep)
#घोषणा _cleanup_pclose_ _cleanup_(pclosep)
#घोषणा _cleanup_closedir_ _cleanup_(closedirp)
#घोषणा _cleanup_close_pair_ _cleanup_(close_pairp)

पूर्णांक fd_nonblock(पूर्णांक fd, bool nonblock);
पूर्णांक stdio_disable_nonblock(व्योम);

व्योम nonblock_resetp(पूर्णांक *fd);

पूर्णांक fd_cloexec(पूर्णांक fd, bool cloexec);
पूर्णांक fd_cloexec_many(स्थिर पूर्णांक fds[], माप_प्रकार n_fds, bool cloexec);

पूर्णांक get_max_fd(व्योम);

पूर्णांक close_all_fds(स्थिर पूर्णांक except[], माप_प्रकार n_except);
पूर्णांक close_all_fds_without_malloc(स्थिर पूर्णांक except[], माप_प्रकार n_except);
पूर्णांक close_all_fds_frugal(स्थिर पूर्णांक except[], माप_प्रकार n_except);

पूर्णांक pack_fds(पूर्णांक fds[], माप_प्रकार n);

पूर्णांक fd_validate(पूर्णांक fd);
पूर्णांक same_fd(पूर्णांक a, पूर्णांक b);

bool fdname_is_valid(स्थिर अक्षर *s);

पूर्णांक fd_get_path(पूर्णांक fd, अक्षर **ret);

पूर्णांक move_fd(पूर्णांक from, पूर्णांक to, पूर्णांक cloexec);

पूर्णांक fd_move_above_stdio(पूर्णांक fd);

पूर्णांक rearrange_stdio(पूर्णांक original_निवेश_fd, पूर्णांक original_output_fd, पूर्णांक original_error_fd);

अटल अंतरभूत पूर्णांक make_null_stdio(व्योम) {
        वापस rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#घोषणा TAKE_FD(fd) TAKE_GENERIC(fd, पूर्णांक, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#घोषणा close_and_replace(a, b)                 \
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
अटल अंतरभूत पूर्णांक fd_verify_safe_flags(पूर्णांक fd) {
        वापस fd_verify_safe_flags_full(fd, 0);
}

अचिन्हित read_nr_open(व्योम);
पूर्णांक fd_get_diskseq(पूर्णांक fd, uint64_t *ret);

पूर्णांक path_is_मूल_at(पूर्णांक dir_fd, स्थिर अक्षर *path);
अटल अंतरभूत पूर्णांक path_is_मूल(स्थिर अक्षर *path) {
        वापस path_is_मूल_at(AT_FDCWD, path);
}
अटल अंतरभूत पूर्णांक dir_fd_is_मूल(पूर्णांक dir_fd) {
        वापस dir_fd == XAT_FDROOT ? true : path_is_मूल_at(dir_fd, शून्य);
}
अटल अंतरभूत पूर्णांक dir_fd_is_मूल_or_cwd(पूर्णांक dir_fd) {
        वापस IN_SET(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : path_is_मूल_at(dir_fd, शून्य);
}

पूर्णांक fds_inode_and_mount_same(पूर्णांक fd1, पूर्णांक fd2);

पूर्णांक resolve_xat_fdroot(पूर्णांक *fd, स्थिर अक्षर **path, अक्षर **ret_बफर);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#घोषणा PROC_FD_PATH_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(पूर्णांक))

अक्षर* format_proc_fd_path(अक्षर buf[अटल PROC_FD_PATH_MAX], पूर्णांक fd);

#घोषणा FORMAT_PROC_FD_PATH(fd) \
        format_proc_fd_path((अक्षर[PROC_FD_PATH_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#घोषणा PROC_PID_FD_PATH_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(पूर्णांक))

अक्षर* format_proc_pid_fd_path(अक्षर buf[अटल PROC_PID_FD_PATH_MAX], pid_t pid, पूर्णांक fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#घोषणा FORMAT_PROC_PID_FD_PATH(pid, fd)                                \
        format_proc_pid_fd_path((अक्षर[PROC_PID_FD_PATH_MAX]) {}, (pid), (fd))

पूर्णांक proc_fd_enoent_errno(व्योम);

स्थिर अक्षर* accmode_to_string(पूर्णांक flags);

/* Like ASSERT_PTR, but for fds */
#घोषणा ASSERT_FD(fd)                           \
        ({                                      \
                पूर्णांक _fd_ = (fd);                \
                निश्चित(_fd_ >= 0);              \
                _fd_;                           \
        })
