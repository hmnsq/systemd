/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <dirent.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "forward.h"

/* maximum length of fdname */
#परिभाषय FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#परिभाषय FD_गन्तव्यम्_PTR(fd) INT_गन्तव्यम्_PTR((fd)+1)
#परिभाषय PTR_गन्तव्यम्_FD(p) (PTR_गन्तव्यम्_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#परिभाषय EBADF_PAIR { -EBADF, -EBADF }
#परिभाषय EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#यदि O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#अन्यथा
#  यदि defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
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
#यदि_अन्तः

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#परिभाषय O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#परिभाषय NR_OPEN_DEFAULT ((अचिह्नित) (1024 * 1024))
#परिभाषय NR_OPEN_MINIMUM ((अचिह्नित) (परिमाण(दीर्घ) * 8))
#परिभाषय NR_OPEN_अधिकतमम् ((अचिह्नित) (CONST_MIN((size_t) अधिकतम_पूर्णाङ्कः, SIZE_MAX / __SIZEOF_POINTER__) & ~(परिमाण(दीर्घ) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#परिभाषय XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
स्थिर अन्तःस्थ bool wildcard_fd_is_valid(पूर्णाङ्क fd) {
        प्रत्यागम fd >= 0 || अन्तर्_समुच्चयः(fd, AT_FDCWD, XAT_FDROOT);
}

पूर्णाङ्क close_nointr(पूर्णाङ्क fd);
पूर्णाङ्क safe_close(पूर्णाङ्क fd);
शून्य safe_close_pair(पूर्णाङ्क p[स्थिर 2]);

स्थिर अन्तःस्थ पूर्णाङ्क safe_close_above_stdio(पूर्णाङ्क fd) {
        यदि (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                प्रत्यागम -EBADF;

        प्रत्यागम safe_close(fd);
}

स्थिर अन्तःस्थ शून्य* close_fd_ptr(शून्य *p) {
        safe_close(PTR_गन्तव्यम्_FD(p));
        प्रत्यागम रिक्त;
}

शून्य close_many(ध्रुव पूर्णाङ्क fds[], size_t n_fds);
शून्य close_many_unset(पूर्णाङ्क fds[], size_t n_fds);
शून्य close_many_and_free(पूर्णाङ्क *fds, size_t n_fds);

पूर्णाङ्क fclose_nointr(FILE *f);
FILE* safe_fclose(FILE *f);
DIR* safe_closedir(DIR *d);

स्थिर अन्तःस्थ शून्य closep(पूर्णाङ्क *fd) {
        safe_close(*fd);
}

स्थिर अन्तःस्थ शून्य close_pairp(पूर्णाङ्क (*p)[2]) {
        safe_close_pair(*p);
}

स्थिर अन्तःस्थ शून्य fclosep(FILE **f) {
        safe_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_पूर्णम्(FILE*, pclose, रिक्त);
DEFINE_TRIVIAL_CLEANUP_FUNC_पूर्णम्(DIR*, closedir, रिक्त);

#परिभाषय _cleanup_close_ _cleanup_(closep)
#परिभाषय _cleanup_fclose_ _cleanup_(fclosep)
#परिभाषय _cleanup_pclose_ _cleanup_(pclosep)
#परिभाषय _cleanup_closedir_ _cleanup_(closedirp)
#परिभाषय _cleanup_close_pair_ _cleanup_(close_pairp)

पूर्णाङ्क fd_nonblock(पूर्णाङ्क fd, bool nonblock);
पूर्णाङ्क stdio_disable_nonblock(शून्य);

शून्य nonblock_resetp(पूर्णाङ्क *fd);

पूर्णाङ्क fd_cloexec(पूर्णाङ्क fd, bool cloexec);
पूर्णाङ्क fd_cloexec_many(ध्रुव पूर्णाङ्क fds[], size_t n_fds, bool cloexec);

पूर्णाङ्क get_max_fd(शून्य);

पूर्णाङ्क close_all_fds(ध्रुव पूर्णाङ्क except[], size_t n_except);
पूर्णाङ्क close_all_fds_without_malloc(ध्रुव पूर्णाङ्क except[], size_t n_except);
पूर्णाङ्क close_all_fds_frugal(ध्रुव पूर्णाङ्क except[], size_t n_except);

पूर्णाङ्क pack_fds(पूर्णाङ्क fds[], size_t n);

पूर्णाङ्क fd_validate(पूर्णाङ्क fd);
पूर्णाङ्क same_fd(पूर्णाङ्क a, पूर्णाङ्क b);

bool fdname_is_valid(ध्रुव वर्ण *s);

पूर्णाङ्क fd_get_मार्गः(पूर्णाङ्क fd, वर्ण **ret);

पूर्णाङ्क move_fd(पूर्णाङ्क from, पूर्णाङ्क to, पूर्णाङ्क cloexec);

पूर्णाङ्क fd_move_above_stdio(पूर्णाङ्क fd);

पूर्णाङ्क rearrange_stdio(पूर्णाङ्क original_निवेशः_fd, पूर्णाङ्क original_output_fd, पूर्णाङ्क original_error_fd);

स्थिर अन्तःस्थ पूर्णाङ्क make_null_stdio(शून्य) {
        प्रत्यागम rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#परिभाषय TAKE_FD(fd) TAKE_GENERIC(fd, पूर्णाङ्क, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#परिभाषय close_and_replace(a, b)                 \
        ({                                      \
                पूर्णाङ्क *_fdp_ = &(a);              \
                safe_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

पूर्णाङ्क fd_reopen(पूर्णाङ्क fd, पूर्णाङ्क flags);
पूर्णाङ्क fd_reopen_propagate_append_and_position(पूर्णाङ्क fd, पूर्णाङ्क flags);
पूर्णाङ्क fd_reopen_condition(पूर्णाङ्क fd, पूर्णाङ्क flags, पूर्णाङ्क आवरणम्_3, पूर्णाङ्क *ret_new_fd);

पूर्णाङ्क fd_is_opath(पूर्णाङ्क fd);
पूर्णाङ्क fd_vet_accmode(पूर्णाङ्क fd, पूर्णाङ्क बहुलकम्);
पूर्णाङ्क fd_is_writable(पूर्णाङ्क fd);

पूर्णाङ्क fd_verify_safe_flags_पूर्णम्(पूर्णाङ्क fd, पूर्णाङ्क extra_flags);
स्थिर अन्तःस्थ पूर्णाङ्क fd_verify_safe_flags(पूर्णाङ्क fd) {
        प्रत्यागम fd_verify_safe_flags_पूर्णम्(fd, 0);
}

अचिह्नित read_nr_open(शून्य);
पूर्णाङ्क fd_get_diskseq(पूर्णाङ्क fd, uint64_t *ret);

पूर्णाङ्क मार्गः_is_मूलम्_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः);
स्थिर अन्तःस्थ पूर्णाङ्क मार्गः_is_मूलम्(ध्रुव वर्ण *मार्गः) {
        प्रत्यागम मार्गः_is_मूलम्_at(AT_FDCWD, मार्गः);
}
स्थिर अन्तःस्थ पूर्णाङ्क dir_fd_is_मूलम्(पूर्णाङ्क dir_fd) {
        प्रत्यागम dir_fd == XAT_FDROOT ? true : मार्गः_is_मूलम्_at(dir_fd, रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क dir_fd_is_मूलम्_or_cwd(पूर्णाङ्क dir_fd) {
        प्रत्यागम अन्तर्_समुच्चयः(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : मार्गः_is_मूलम्_at(dir_fd, रिक्त);
}

पूर्णाङ्क fds_inode_and_mount_same(पूर्णाङ्क fd1, पूर्णाङ्क fd2);

पूर्णाङ्क resolve_xat_fdroot(पूर्णाङ्क *fd, ध्रुव वर्ण **मार्गः, वर्ण **ret_बफर);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#परिभाषय PROC_FD_मार्गः_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(पूर्णाङ्क))

वर्ण* format_proc_fd_मार्गः(वर्ण buf[स्थिर PROC_FD_मार्गः_MAX], पूर्णाङ्क fd);

#परिभाषय FORMAT_PROC_FD_मार्गः(fd) \
        format_proc_fd_मार्गः((वर्ण[PROC_FD_मार्गः_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#परिभाषय PROC_PID_FD_मार्गः_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(पूर्णाङ्क))

वर्ण* format_proc_pid_fd_मार्गः(वर्ण buf[स्थिर PROC_PID_FD_मार्गः_MAX], pid_t pid, पूर्णाङ्क fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#परिभाषय FORMAT_PROC_PID_FD_मार्गः(pid, fd)                                \
        format_proc_pid_fd_मार्गः((वर्ण[PROC_PID_FD_मार्गः_MAX]) {}, (pid), (fd))

पूर्णाङ्क proc_fd_enoent_errno(शून्य);

ध्रुव वर्ण* accmode_गन्तव्यम्_string(पूर्णाङ्क flags);

/* Like ASSERT_PTR, but for fds */
#परिभाषय ASSERT_FD(fd)                           \
        ({                                      \
                पूर्णाङ्क _fd_ = (fd);                \
                प्रतिज्ञा(_fd_ >= 0);              \
                _fd_;                           \
        })
