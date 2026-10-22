/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <dirent.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "forward.h"

/* maximum length of fdname */
#నిర్వచించు FDNAME_MAX 255

/* Make sure we can distinguish fd 0 and NULL */
#నిర్వచించు FD_గమ్యం_PTR(fd) INT_గమ్యం_PTR((fd)+1)
#నిర్వచించు PTR_గమ్యం_FD(p) (PTR_గమ్యం_INT(p)-1)

/* Useful helpers for initializing pipe(), socketpair() or stdio fd arrays */
#నిర్వచించు EBADF_PAIR { -EBADF, -EBADF }
#నిర్వచించు EBADF_TRIPLET { -EBADF, -EBADF, -EBADF }

/* So O_LARGEFILE is generally implied by glibc, and defined to zero hence, because we only build in LFS
 * mode. However, when invoking fcntl(F_GETFL) the flag is ORed into the result anyway — glibc does not mask
 * it away. Which sucks. Let's define the actual value here, so that we can mask it ourselves.
 *
 * The precise definition is arch specific, so we use the values defined in the kernel (note that some
 * are hexa and others are octal; duplicated as-is from the kernel definitions):
 * - alpha, arm, arm64, m68k, mips, parisc, powerpc, sparc: each has a specific value;
 * - others: they use the "generic" value (defined in include/uapi/asm-generic/fcntl.h) */
#ఒకవేళ O_LARGEFILE != 0
#  define RAW_O_LARGEFILE O_LARGEFILE
#లేకపోతే
#  ఒకవేళ defined(__alpha__) || defined(__arm__) || defined(__aarch64__) || defined(__m68k__)
#    define RAW_O_LARGEFILE 0400000
#  elif defined(__mips__)
#    define RAW_O_LARGEFILE 0x2000
#  elif defined(__parisc__) || defined(__hppa__)
#    define RAW_O_LARGEFILE 000004000
#  elif defined(__powerpc__)
#    define RAW_O_LARGEFILE 0200000
#  elif defined(__sparc__)
#    define RAW_O_LARGEFILE 0x40000
#  లేకపోతే
#    define RAW_O_LARGEFILE 00100000
#  endif
#అయితే_ముగింపు

/* On musl, O_ACCMODE is defined as (03|O_SEARCH), unlike glibc which defines it as
 * (O_RDONLY|O_WRONLY|O_RDWR). Additionally, O_SEARCH is simply defined as O_PATH. This changes the behaviour
 * of O_ACCMODE in certain situations, which we don't want. This definition is copied from glibc and works
 * around the problems with musl's definition. */
#నిర్వచించు O_ACCMODE_STRICT (O_RDONLY|O_WRONLY|O_RDWR)

/* The default, minimum, and maximum values of /proc/sys/fs/nr_open. See kernel's fs/file.c.
 * These values have been unchanged since kernel-2.6.26:
 * https://github.com/torvalds/linux/commit/eceea0b3df05ed262ae32e0c6340cc7a3626632d */
#నిర్వచించు NR_OPEN_DEFAULT ((గుర్తులేని) (1024 * 1024))
#నిర్వచించు NR_OPEN_MINIMUM ((గుర్తులేని) (పరిమాణం(దీర్ఘ) * 8))
#నిర్వచించు NR_OPEN_గరిష్ఠ ((గుర్తులేని) (CONST_MIN((size_t) గరిష్ఠ_పూర్ణాంకం, SIZE_MAX / __SIZEOF_POINTER__) & ~(పరిమాణం(దీర్ఘ) * 8 - 1)))

/* A special fd that can be passed in various helpers instead of an fd indicating the root dir. Inspired by,
 * and an alternative to AT_FDCWD. We use specific negative value that is outside of the negative errno
 * range, to avoid any potential ambiguities. */
#నిర్వచించు XAT_FDROOT -8192
assert_cc(XAT_FDROOT != AT_FDCWD);
assert_cc(XAT_FDROOT < -ERRNO_MAX);

/* Checks whether the specified fd is acceptable as a *at() directory fd that supports the two "wildcard"
 * values: it's either a regular, valid fd (i.e. >= 0), or one of the special AT_FDCWD/XAT_FDROOT values. */
స్థిర అంతఃవరుస bool wildcard_fd_is_valid(పూర్ణాంకం fd) {
        తిరిగి fd >= 0 || లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT);
}

పూర్ణాంకం close_nointr(పూర్ణాంకం fd);
పూర్ణాంకం సురక్షిత_close(పూర్ణాంకం fd);
శూన్య సురక్షిత_close_pair(పూర్ణాంకం p[స్థిర 2]);

స్థిర అంతఃవరుస పూర్ణాంకం సురక్షిత_close_above_stdio(పూర్ణాంకం fd) {
        ఒకవేళ (fd < 3) /* Don't close stdin/stdout/stderr, but still invalidate the fd by returning -EBADF. */
                తిరిగి -EBADF;

        తిరిగి సురక్షిత_close(fd);
}

స్థిర అంతఃవరుస శూన్య* close_fd_ptr(శూన్య *p) {
        సురక్షిత_close(PTR_గమ్యం_FD(p));
        తిరిగి ఖాళీ;
}

శూన్య close_many(స్థిరరాశి పూర్ణాంకం fds[], size_t n_fds);
శూన్య close_many_unset(పూర్ణాంకం fds[], size_t n_fds);
శూన్య close_many_and_free(పూర్ణాంకం *fds, size_t n_fds);

పూర్ణాంకం fclose_nointr(FILE *f);
FILE* సురక్షిత_fclose(FILE *f);
DIR* సురక్షిత_closedir(DIR *d);

స్థిర అంతఃవరుస శూన్య closep(పూర్ణాంకం *fd) {
        సురక్షిత_close(*fd);
}

స్థిర అంతఃవరుస శూన్య close_pairp(పూర్ణాంకం (*p)[2]) {
        సురక్షిత_close_pair(*p);
}

స్థిర అంతఃవరుస శూన్య fclosep(FILE **f) {
        సురక్షిత_fclose(*f);
}

DEFINE_TRIVIAL_CLEANUP_FUNC_నిండిన(FILE*, pclose, ఖాళీ);
DEFINE_TRIVIAL_CLEANUP_FUNC_నిండిన(DIR*, closedir, ఖాళీ);

#నిర్వచించు _cleanup_close_ _cleanup_(closep)
#నిర్వచించు _cleanup_fclose_ _cleanup_(fclosep)
#నిర్వచించు _cleanup_pclose_ _cleanup_(pclosep)
#నిర్వచించు _cleanup_closedir_ _cleanup_(closedirp)
#నిర్వచించు _cleanup_close_pair_ _cleanup_(close_pairp)

పూర్ణాంకం fd_nonblock(పూర్ణాంకం fd, bool nonblock);
పూర్ణాంకం stdio_disable_nonblock(శూన్య);

శూన్య nonblock_resetp(పూర్ణాంకం *fd);

పూర్ణాంకం fd_cloexec(పూర్ణాంకం fd, bool cloexec);
పూర్ణాంకం fd_cloexec_many(స్థిరరాశి పూర్ణాంకం fds[], size_t n_fds, bool cloexec);

పూర్ణాంకం get_max_fd(శూన్య);

పూర్ణాంకం close_all_fds(స్థిరరాశి పూర్ణాంకం except[], size_t n_except);
పూర్ణాంకం close_all_fds_without_malloc(స్థిరరాశి పూర్ణాంకం except[], size_t n_except);
పూర్ణాంకం close_all_fds_frugal(స్థిరరాశి పూర్ణాంకం except[], size_t n_except);

పూర్ణాంకం pack_fds(పూర్ణాంకం fds[], size_t n);

పూర్ణాంకం fd_validate(పూర్ణాంకం fd);
పూర్ణాంకం same_fd(పూర్ణాంకం a, పూర్ణాంకం b);

bool fdname_is_valid(స్థిరరాశి అక్షరం *s);

పూర్ణాంకం fd_get_మార్గం(పూర్ణాంకం fd, అక్షరం **ret);

పూర్ణాంకం move_fd(పూర్ణాంకం from, పూర్ణాంకం to, పూర్ణాంకం cloexec);

పూర్ణాంకం fd_move_above_stdio(పూర్ణాంకం fd);

పూర్ణాంకం rearrange_stdio(పూర్ణాంకం original_ఆదానం_fd, పూర్ణాంకం original_output_fd, పూర్ణాంకం original_error_fd);

స్థిర అంతఃవరుస పూర్ణాంకం make_null_stdio(శూన్య) {
        తిరిగి rearrange_stdio(-EBADF, -EBADF, -EBADF);
}

/* Like TAKE_PTR() but for file descriptors, resetting them to -EBADF */
#నిర్వచించు TAKE_FD(fd) TAKE_GENERIC(fd, పూర్ణాంకం, -EBADF)

/* Like free_and_replace(), but for file descriptors */
#నిర్వచించు close_and_replace(a, b)                 \
        ({                                      \
                పూర్ణాంకం *_fdp_ = &(a);              \
                సురక్షిత_close(*_fdp_);             \
                *_fdp_ = TAKE_FD(b);            \
                0;                              \
        })

పూర్ణాంకం fd_reopen(పూర్ణాంకం fd, పూర్ణాంకం flags);
పూర్ణాంకం fd_reopen_propagate_append_and_position(పూర్ణాంకం fd, పూర్ణాంకం flags);
పూర్ణాంకం fd_reopen_condition(పూర్ణాంకం fd, పూర్ణాంకం flags, పూర్ణాంకం ముసుగు_3, పూర్ణాంకం *ret_new_fd);

పూర్ణాంకం fd_is_opath(పూర్ణాంకం fd);
పూర్ణాంకం fd_vet_accmode(పూర్ణాంకం fd, పూర్ణాంకం బహుళకం);
పూర్ణాంకం fd_is_writable(పూర్ణాంకం fd);

పూర్ణాంకం fd_verify_సురక్షిత_flags_నిండిన(పూర్ణాంకం fd, పూర్ణాంకం extra_flags);
స్థిర అంతఃవరుస పూర్ణాంకం fd_verify_సురక్షిత_flags(పూర్ణాంకం fd) {
        తిరిగి fd_verify_సురక్షిత_flags_నిండిన(fd, 0);
}

గుర్తులేని read_nr_open(శూన్య);
పూర్ణాంకం fd_get_diskseq(పూర్ణాంకం fd, uint64_t *ret);

పూర్ణాంకం మార్గం_is_మూలం_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం);
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_is_మూలం(స్థిరరాశి అక్షరం *మార్గం) {
        తిరిగి మార్గం_is_మూలం_at(AT_FDCWD, మార్గం);
}
స్థిర అంతఃవరుస పూర్ణాంకం dir_fd_is_మూలం(పూర్ణాంకం dir_fd) {
        తిరిగి dir_fd == XAT_FDROOT ? true : మార్గం_is_మూలం_at(dir_fd, ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం dir_fd_is_మూలం_or_cwd(పూర్ణాంకం dir_fd) {
        తిరిగి లోపల_సమితి(dir_fd, AT_FDCWD, XAT_FDROOT) ? true : మార్గం_is_మూలం_at(dir_fd, ఖాళీ);
}

పూర్ణాంకం fds_inode_and_mount_same(పూర్ణాంకం fd1, పూర్ణాంకం fd2);

పూర్ణాంకం resolve_xat_fdroot(పూర్ణాంకం *fd, స్థిరరాశి అక్షరం **మార్గం, అక్షరం **ret_బఫరు);

/* The maximum length a buffer for a /proc/self/fd/<fd> path needs */
#నిర్వచించు PROC_FD_మార్గం_MAX \
        (STRLEN("/proc/self/fd/") + DECIMAL_STR_MAX(పూర్ణాంకం))

అక్షరం* format_proc_fd_మార్గం(అక్షరం buf[స్థిర PROC_FD_మార్గం_MAX], పూర్ణాంకం fd);

#నిర్వచించు FORMAT_PROC_FD_మార్గం(fd) \
        format_proc_fd_మార్గం((అక్షరం[PROC_FD_మార్గం_MAX]) {}, (fd))

/* The maximum length a buffer for a /proc/<pid>/fd/<fd> path needs */
#నిర్వచించు PROC_PID_FD_మార్గం_MAX \
        (STRLEN("/proc//fd/") + DECIMAL_STR_MAX(pid_t) + DECIMAL_STR_MAX(పూర్ణాంకం))

అక్షరం* format_proc_pid_fd_మార్గం(అక్షరం buf[స్థిర PROC_PID_FD_మార్గం_MAX], pid_t pid, పూర్ణాంకం fd);

/* Kinda the same as FORMAT_PROC_FD_PATH(), but goes by PID rather than "self" symlink */
#నిర్వచించు FORMAT_PROC_PID_FD_మార్గం(pid, fd)                                \
        format_proc_pid_fd_మార్గం((అక్షరం[PROC_PID_FD_మార్గం_MAX]) {}, (pid), (fd))

పూర్ణాంకం proc_fd_enoent_errno(శూన్య);

స్థిరరాశి అక్షరం* accmode_గమ్యం_string(పూర్ణాంకం flags);

/* Like ASSERT_PTR, but for fds */
#నిర్వచించు ASSERT_FD(fd)                           \
        ({                                      \
                పూర్ణాంకం _fd_ = (fd);                \
                ప్రతిజ్ఞా(_fd_ >= 0);              \
                _fd_;                           \
        })
