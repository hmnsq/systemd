/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <sys/statvfs.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "filesystems.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "hash-funcs.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mountpoint-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ verify_stat_at(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                bool follow,
                ಪೂರ್ಣಾಙ್ಕ (*verify_func)(ಧ್ರುವ ಸಂರಚನಾ stat *st),
                bool verify) {

        ಸಂರಚನಾ stat st;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));
        ಪ್ರತಿಜ್ಞಾ(!isempty(path) || !follow);
        ಪ್ರತಿಜ್ಞಾ(verify_func);

        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = verify_func(&st);
        ಪ್ರತ್ಯಾಗಮ verify ? r : r >= 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mode_verify_regular(mode_t mode) {
        ಯದಿ (S_ISDIR(mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (S_ISLNK(mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISREG(mode))
                ಪ್ರತ್ಯಾಗಮ -EBADFD;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_regular(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ಪ್ರತ್ಯಾಗಮ mode_verify_regular(st->st_mode);
}

ಪೂರ್ಣಾಙ್ಕ statx_verify_regular(ಧ್ರುವ ಸಂರಚನಾ statx *stx) {
        ಪ್ರತಿಜ್ಞಾ(stx);

        ಯದಿ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಪ್ರತ್ಯಾಗಮ mode_verify_regular(stx->stx_mode);
}

ಪೂರ್ಣಾಙ್ಕ verify_regular_at(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, bool follow) {
        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_regular(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಪ್ರತ್ಯಾಗಮ verify_regular_at(fd, /* path= */ ರಿಕ್ತ, /* follow= */ false);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mode_verify_directory(mode_t mode) {
        ಯದಿ (S_ISLNK(mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISDIR(mode))
                ಪ್ರತ್ಯಾಗಮ -ENOTDIR;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_directory(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಪ್ರತ್ಯಾಗಮ mode_verify_directory(st->st_mode);
}

ಪೂರ್ಣಾಙ್ಕ statx_verify_directory(ಧ್ರುವ ಸಂರಚನಾ statx *stx) {
        ಪ್ರತಿಜ್ಞಾ(stx);

        ಯದಿ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        ಪ್ರತ್ಯಾಗಮ mode_verify_directory(stx->stx_mode);
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_directory(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, ರಿಕ್ತ, false, stat_verify_directory, true);
}

ಪೂರ್ಣಾಙ್ಕ is_dir_at(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, bool follow) {
        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

ಪೂರ್ಣಾಙ್ಕ is_dir(ಧ್ರುವ ವರ್ಣ *path, bool follow) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(path));
        ಪ್ರತ್ಯಾಗಮ is_dir_at(AT_FDCWD, path, follow);
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_symlink(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಯದಿ (S_ISDIR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (!S_ISLNK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ENOLINK;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_symlink(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, /* path= */ ರಿಕ್ತ, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

ಪೂರ್ಣಾಙ್ಕ is_symlink(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(path));
        ಪ್ರತ್ಯಾಗಮ verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mode_verify_socket(mode_t mode) {
        ಯದಿ (S_ISDIR(mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (S_ISLNK(mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISSOCK(mode))
                ಪ್ರತ್ಯಾಗಮ -ENOTSOCK;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_socket(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಪ್ರತ್ಯಾಗಮ mode_verify_socket(st->st_mode);
}

ಪೂರ್ಣಾಙ್ಕ statx_verify_socket(ಧ್ರುವ ಸಂರಚನಾ statx *stx) {
        ಪ್ರತಿಜ್ಞಾ(stx);

        ಪ್ರತ್ಯಾಗಮ mode_verify_socket(stx->stx_mode);
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_socket(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, /* path= */ ರಿಕ್ತ, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

ಪೂರ್ಣಾಙ್ಕ is_socket(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(path));
        ಪ್ರತ್ಯಾಗಮ verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_linked(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಯದಿ (st->st_nlink <= 0)
                ಪ್ರತ್ಯಾಗಮ -EIDRM; /* recognizable error. */

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_linked(ಪೂರ್ಣಾಙ್ಕ fd) {

        ಯದಿ (fd == XAT_FDROOT)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, ರಿಕ್ತ, false, stat_verify_linked, true);
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_block(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಯದಿ (S_ISDIR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (S_ISLNK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISBLK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ENOTBLK;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_block(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, /* path= */ ರಿಕ್ತ, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_char(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಯದಿ (S_ISDIR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (S_ISLNK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISCHR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EBADFD;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_device_ಗಂಟು(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಯದಿ (S_ISDIR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (S_ISLNK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ is_device_ಗಂಟು(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(path));
        ಪ್ರತ್ಯಾಗಮ verify_stat_at(AT_FDCWD, path, false, stat_verify_device_ಗಂಟು, false);
}

ಪೂರ್ಣಾಙ್ಕ stat_verify_regular_or_block(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಯದಿ (S_ISDIR(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (S_ISLNK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -ELOOP;

        ಯದಿ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ -EBADFD;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_verify_regular_or_block(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಯದಿ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಪ್ರತ್ಯಾಗಮ verify_stat_at(fd, /* path= */ ರಿಕ್ತ, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

ಪೂರ್ಣಾಙ್ಕ dir_is_empty_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಸಂರಚನಾ dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        ಪ್ರತಿ (;;) {
                ಸಂರಚನಾ dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (n == 0)
                        ಭಙ್ಗ;

                ಪ್ರತಿಜ್ಞಾ((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ಯದಿ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ 1;
}

bool stat_may_be_dev_null(ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ಪ್ರತ್ಯಾಗಮ S_ISCHR(st->st_mode);
}

bool stat_is_empty(ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಪ್ರತ್ಯಾಗಮ S_ISREG(st->st_mode) && st->st_size <= 0;
}

ಪೂರ್ಣಾಙ್ಕ null_or_empty_path_with_ಬೇರು(ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *ಬೇರು) {
        _cleanup_free_ ವರ್ಣ *resolved = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಸಂರಚನಾ stat st;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fn);

        /* fn includes the root prefix. */

        ಯದಿ (path_equal(path_startswith(fn, ಬೇರು ?: "/"), "dev/null"))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು)) {
                ಯದಿ (stat(fn, &st) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, ಬೇರು, CHASE_NONEXISTENT, &resolved, &fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (path_equal(path_startswith(resolved, ಬೇರು), "dev/null"))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ null_or_empty(&st);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* statx_mask_one_to_name(unsigned mask);
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* statx_attribute_to_name(uint64_t attr);

#ಅನ್ತರ್ಭಾವ "statx-attribute-to-name.inc"
#ಅನ್ತರ್ಭಾವ "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        ಸ್ಥಿರ ವರ್ಣ* prefix##_to_string(type v) {                               \
                ಯದಿ (v == 0)                                                     \
                        ಪ್ರತ್ಯಾಗಮ strdup("");                                      \
                                                                                \
                _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ಧ್ರುವ ವರ್ಣ *n = func(f);                                \
                        ಯದಿ (!n)                                                 \
                                ಅನುವರ್ತ;                                       \
                                                                                \
                        ಯದಿ (!strextend_with_separator(&s, "|", n))              \
                                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ಯದಿ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;                                            \
                                                                                \
                ಪ್ರತ್ಯಾಗಮ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

ಪೂರ್ಣಾಙ್ಕ xstatx_full(ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ಸಂರಚನಾ statx *ret) {

        ಸಂರಚನಾ statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Wrapper around statx(), with additional bells and whistles:
         *
         * 1. AT_EMPTY_PATH is implied on empty path
         * 2. Supports XAT_FDROOT
         * 3. Takes separate mandatory and optional mask params, plus mandatory attributes.
         *    Returns -EUNATCH if statx() does not return all masks specified as mandatory,
         *    > 0 if all optional masks are supported, 0 otherwise.
         * 4. Supports a new flag XSTATX_MNT_ID_BEST which acquires STATX_MNT_ID_UNIQUE if available and
         *    STATX_MNT_ID if not.
         */

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));
        ಪ್ರತಿಜ್ಞಾ((mandatory_mask & optional_mask) == 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ಯದಿ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ಯದಿ (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                ಪ್ರತ್ಯಾಗಮ negative_errno();

        ಯದಿ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ಯದಿ (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ಯದಿ (DEBUG_LOGGING) {
                        _cleanup_free_ ವರ್ಣ *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                ಪ್ರತ್ಯಾಗಮ -EUNATCH;
        }

        ಯದಿ (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ಯದಿ (DEBUG_LOGGING) {
                        _cleanup_free_ ವರ್ಣ *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ಪ್ರತ್ಯಾಗಮ -EUNATCH;
        }

        *ret = sx;
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(sx.stx_mask, optional_mask);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xfstatfs(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ statfs *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (fd == AT_FDCWD)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(statfs(".", ret));
        ಯದಿ (fd == XAT_FDROOT)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(statfs("/", ret));

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fstatfs(fd, ret));
}

ಪೂರ್ಣಾಙ್ಕ xstatfsat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಸಂರಚನಾ statfs *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ fd;
                dir_fd = fd;
        }

        ಪ್ರತ್ಯಾಗಮ xfstatfs(dir_fd, ret);
}

ಪೂರ್ಣಾಙ್ಕ fd_is_read_only_fs(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಸಂರಚನಾ statfs st;
        r = xfstatfs(fd, &st);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (st.f_flags & ST_RDONLY)
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ಪ್ರತ್ಯಾಗಮ access_fd(fd, W_OK) == -EROFS;

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ path_is_read_only_fs(ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ fd_is_read_only_fs(fd);
}

ಪೂರ್ಣಾಙ್ಕ inode_same_at(ಪೂರ್ಣಾಙ್ಕ fda, ಧ್ರುವ ವರ್ಣ *filea, ಪೂರ್ಣಾಙ್ಕ fdb, ಧ್ರುವ ವರ್ಣ *fileb, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಸಂರಚನಾ stat sta, stb;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fda >= 0 || fda == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(fdb >= 0 || fdb == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ಯದಿ ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ಯದಿ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                ಪ್ರತ್ಯಾಗಮ true;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ pin_a = -EBADF, pin_b = -EBADF;
        ಯದಿ (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ಯದಿ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ಯದಿ (pin_a < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        fda = pin_a;
                        filea = ರಿಕ್ತ;
                        flags |= AT_EMPTY_PATH;
                }

                ಯದಿ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ಯದಿ (pin_b < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        fdb = pin_b;
                        fileb = ರಿಕ್ತ;
                        flags |= AT_EMPTY_PATH;
                }

                ಪೂರ್ಣಾಙ್ಕ ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ ಸಂರಚನಾ file_handle *ha = ರಿಕ್ತ, *hb = ರಿಕ್ತ;
                uint64_t mntida, mntidb;
                ಪೂರ್ಣಾಙ್ಕ _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ಯದಿ (r < 0) {
                        ಯದಿ (is_name_to_handle_at_fatal_error(r))
                                ಪ್ರತ್ಯಾಗಮ r;

                        ಗಚ್ಛ fallback;
                }
                bool have_unique_mntid = r > 0;

                ಯದಿ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ರಿಕ್ತ : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ರಿಕ್ತ,
                                ntha_flags);
                ಯದಿ (r < 0) {
                        ಯದಿ (is_name_to_handle_at_fatal_error(r))
                                ಪ್ರತ್ಯಾಗಮ r;

                        ಗಚ್ಛ fallback;
                }
                ಯದಿ (r == 0) {
                        ಪ್ರತಿಜ್ಞಾ(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ಯದಿ (!file_handle_equal(ha, hb))
                        ಪ್ರತ್ಯಾಗಮ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ಯದಿ (mntida == mntidb)
                        ಪ್ರತ್ಯಾಗಮ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ಯದಿ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ಯದಿ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ಪ್ರತ್ಯಾಗಮ stat_inode_same(&sta, &stb);
}

bool is_fs_type(ಧ್ರುವ ಸಂರಚನಾ statfs *s, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(s);
        assert_cc(ಪರಿಮಾಣ(statfs_f_type_t) >= ಪರಿಮಾಣ(s->f_type));

        ಪ್ರತ್ಯಾಗಮ F_TYPE_EQUAL(s->f_type, magic_ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ is_fs_type_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಸಂರಚನಾ statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ is_fs_type(&s, magic_ಮೌಲ್ಯ);
}

bool is_temporary_fs(ಧ್ರುವ ಸಂರಚನಾ statfs *s) {
        ಪ್ರತ್ಯಾಗಮ fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(ಧ್ರುವ ಸಂರಚನಾ statfs *s) {
        ಪ್ರತ್ಯಾಗಮ fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

ಪೂರ್ಣಾಙ್ಕ fd_is_temporary_fs(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಸಂರಚನಾ statfs s;
        r = xfstatfs(fd, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ is_temporary_fs(&s);
}

ಪೂರ್ಣಾಙ್ಕ fd_is_network_fs(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಸಂರಚನಾ statfs s;
        r = xfstatfs(fd, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ is_network_fs(&s);
}

ಪೂರ್ಣಾಙ್ಕ path_is_temporary_fs(ಧ್ರುವ ವರ್ಣ *path) {
        ಸಂರಚನಾ statfs s;

        ಯದಿ (statfs(path, &s) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ is_temporary_fs(&s);
}

ಪೂರ್ಣಾಙ್ಕ path_is_network_fs(ಧ್ರುವ ವರ್ಣ *path) {
        ಸಂರಚನಾ statfs s;

        ಯದಿ (statfs(path, &s) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ is_network_fs(&s);
}

ಪೂರ್ಣಾಙ್ಕ proc_mounted(ಶೂನ್ಯ) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ಯದಿ (r == -ENOENT) /* not mounted at all */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ r;
}

bool stat_inode_same(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ಪ್ರತ್ಯಾಗಮ stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ಧ್ರುವ ಸಂರಚನಾ statx *a, ಧ್ರುವ ಸಂರಚನಾ statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ಯದಿ (!statx_is_set(a) || !statx_is_set(b))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        ಪ್ರತ್ಯಾಗಮ
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

ಪೂರ್ಣಾಙ್ಕ statx_mount_same(ಧ್ರುವ ಸಂರಚನಾ statx *a, ಧ್ರುವ ಸಂರಚನಾ statx *b) {
        ಯದಿ (!statx_is_set(a) || !statx_is_set(b))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                ಪ್ರತ್ಯಾಗಮ a->stx_mnt_id == b->stx_mnt_id;

        ಪ್ರತ್ಯಾಗಮ -ENODATA;
}

usec_t statx_timestamp_load(ಧ್ರುವ ಸಂರಚನಾ statx_timestamp *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);
        ಪ್ರತ್ಯಾಗಮ timespec_load(&(ಧ್ರುವ ಸಂರಚನಾ timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(ಧ್ರುವ ಸಂರಚನಾ statx_timestamp *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);
        ಪ್ರತ್ಯಾಗಮ timespec_load_nsec(&(ಧ್ರುವ ಸಂರಚನಾ timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

ಶೂನ್ಯ inode_hash_func(ಧ್ರುವ ಸಂರಚನಾ stat *q, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

ಪೂರ್ಣಾಙ್ಕ inode_compare_func(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(a->st_dev, b->st_dev);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = CMP(a->st_ino, b->st_ino);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(inode_hash_ops, ಸಂರಚನಾ stat, inode_hash_func, inode_compare_func, ಮುಕ್ತ);

ಶೂನ್ಯ inode_unmodified_hash_func(ಧ್ರುವ ಸಂರಚನಾ stat *q, ಸಂರಚನಾ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ಯದಿ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ಅನ್ಯಥಾ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ಯದಿ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ಅನ್ಯಥಾ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

ಪೂರ್ಣಾಙ್ಕ inode_unmodified_compare_func(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = inode_compare_func(a, b);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(inode_unmodified_hash_ops, ಸಂರಚನಾ stat, inode_unmodified_hash_func, inode_unmodified_compare_func, ಮುಕ್ತ);

ಧ್ರುವ ವರ್ಣ* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        ವಿಕಲ್ಪ (m & S_IFMT) {
        ಪಕ್ಷ S_IFREG:
                ಪ್ರತ್ಯಾಗಮ "reg";
        ಪಕ್ಷ S_IFDIR:
                ಪ್ರತ್ಯಾಗಮ "dir";
        ಪಕ್ಷ S_IFLNK:
                ಪ್ರತ್ಯಾಗಮ "lnk";
        ಪಕ್ಷ S_IFCHR:
                ಪ್ರತ್ಯಾಗಮ "chr";
        ಪಕ್ಷ S_IFBLK:
                ಪ್ರತ್ಯಾಗಮ "blk";
        ಪಕ್ಷ S_IFIFO:
                ಪ್ರತ್ಯಾಗಮ "fifo";
        ಪಕ್ಷ S_IFSOCK:
                ಪ್ರತ್ಯಾಗಮ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

mode_t inode_type_from_string(ಧ್ರುವ ವರ್ಣ *s) {
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ MODE_INVALID;

        ಯದಿ (streq(s, "reg"))
                ಪ್ರತ್ಯಾಗಮ S_IFREG;
        ಯದಿ (streq(s, "dir"))
                ಪ್ರತ್ಯಾಗಮ S_IFDIR;
        ಯದಿ (streq(s, "lnk"))
                ಪ್ರತ್ಯಾಗಮ S_IFLNK;
        ಯದಿ (streq(s, "chr"))
                ಪ್ರತ್ಯಾಗಮ S_IFCHR;
        ಯದಿ (streq(s, "blk"))
                ಪ್ರತ್ಯಾಗಮ S_IFBLK;
        ಯದಿ (streq(s, "fifo"))
                ಪ್ರತ್ಯಾಗಮ S_IFIFO;
        ಯದಿ (streq(s, "sock"))
                ಪ್ರತ್ಯಾಗಮ S_IFSOCK;

        ಪ್ರತ್ಯಾಗಮ MODE_INVALID;
}

ಪೂರ್ಣಾಙ್ಕ vfs_free_bytes(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        ಸಂರಚನಾ statvfs sv;
        ಯದಿ (fstatvfs(fd, &sv) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ 0;
}
