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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ verify_stat_at(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                bool follow,
                ಪೂರ್ಣಾಂಕ (*verify_func)(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st),
                bool verify) {

        ರಚನೆ stat st;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(fd));
        ಪ್ರತಿಜ್ಞಾ(!isempty(ಮಾರ್ಗ) || !follow);
        ಪ್ರತಿಜ್ಞಾ(verify_func);

        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        r = resolve_xat_fdroot(&fd, &ಮಾರ್ಗ, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (fstatat(fd, strempty(ಮಾರ್ಗ), &st,
                    (isempty(ಮಾರ್ಗ) ? AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = verify_func(&st);
        ಹಿಂತಿರುಗಿ verify ? r : r >= 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_verify_regular(mode_t ಬಹುಲಕ) {
        ಆದರೆ (S_ISDIR(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (S_ISLNK(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISREG(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -EBADFD;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ stat_verify_regular(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_verify_regular(st->st_mode);
}

ಪೂರ್ಣಾಂಕ statx_verify_regular(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *stx) {
        ಪ್ರತಿಜ್ಞಾ(stx);

        ಆದರೆ (!FLAGS_ಗಣ(stx->stx_ಮುಖವಾಡ, STATX_TYPE))
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_verify_regular(stx->stx_ಬಹುಲಕ);
}

ಪೂರ್ಣಾಂಕ verify_regular_at(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool follow) {
        ಹಿಂತಿರುಗಿ verify_stat_at(fd, ಮಾರ್ಗ, follow, stat_verify_regular, true);
}

ಪೂರ್ಣಾಂಕ fd_verify_regular(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಹಿಂತಿರುಗಿ verify_regular_at(fd, /* path= */ ಖಾಲಿ, /* follow= */ false);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_verify_directory(mode_t ಬಹುಲಕ) {
        ಆದರೆ (S_ISLNK(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISDIR(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -ENOTDIR;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ stat_verify_directory(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_verify_directory(st->st_mode);
}

ಪೂರ್ಣಾಂಕ statx_verify_directory(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *stx) {
        ಪ್ರತಿಜ್ಞಾ(stx);

        ಆದರೆ (!FLAGS_ಗಣ(stx->stx_ಮುಖವಾಡ, STATX_TYPE))
                ಹಿಂತಿರುಗಿ -ENODATA;

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_verify_directory(stx->stx_ಬಹುಲಕ);
}

ಪೂರ್ಣಾಂಕ fd_verify_directory(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT))
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ verify_stat_at(fd, ಖಾಲಿ, false, stat_verify_directory, true);
}

ಪೂರ್ಣಾಂಕ is_dir_at(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool follow) {
        ಹಿಂತಿರುಗಿ verify_stat_at(fd, ಮಾರ್ಗ, follow, stat_verify_directory, false);
}

ಪೂರ್ಣಾಂಕ is_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool follow) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(ಮಾರ್ಗ));
        ಹಿಂತಿರುಗಿ is_dir_at(AT_FDCWD, ಮಾರ್ಗ, follow);
}

ಪೂರ್ಣಾಂಕ stat_verify_symlink(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಆದರೆ (S_ISDIR(st->st_mode))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (!S_ISLNK(st->st_mode))
                ಹಿಂತಿರುಗಿ -ENOLINK;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_verify_symlink(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಹಿಂತಿರುಗಿ verify_stat_at(fd, /* path= */ ಖಾಲಿ, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

ಪೂರ್ಣಾಂಕ is_symlink(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(ಮಾರ್ಗ));
        ಹಿಂತಿರುಗಿ verify_stat_at(AT_FDCWD, ಮಾರ್ಗ, false, stat_verify_symlink, false);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಹುಲಕ_verify_socket(mode_t ಬಹುಲಕ) {
        ಆದರೆ (S_ISDIR(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (S_ISLNK(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISSOCK(ಬಹುಲಕ))
                ಹಿಂತಿರುಗಿ -ENOTSOCK;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ stat_verify_socket(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_verify_socket(st->st_mode);
}

ಪೂರ್ಣಾಂಕ statx_verify_socket(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *stx) {
        ಪ್ರತಿಜ್ಞಾ(stx);

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_verify_socket(stx->stx_ಬಹುಲಕ);
}

ಪೂರ್ಣಾಂಕ fd_verify_socket(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಹಿಂತಿರುಗಿ verify_stat_at(fd, /* path= */ ಖಾಲಿ, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

ಪೂರ್ಣಾಂಕ is_socket(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(ಮಾರ್ಗ));
        ಹಿಂತಿರುಗಿ verify_stat_at(AT_FDCWD, ಮಾರ್ಗ, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

ಪೂರ್ಣಾಂಕ stat_verify_linked(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಆದರೆ (st->st_nlink <= 0)
                ಹಿಂತಿರುಗಿ -EIDRM; /* recognizable error. */

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_verify_linked(ಪೂರ್ಣಾಂಕ fd) {

        ಆದರೆ (fd == XAT_FDROOT)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ verify_stat_at(fd, ಖಾಲಿ, false, stat_verify_linked, true);
}

ಪೂರ್ಣಾಂಕ stat_verify_ಖಂಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಆದರೆ (S_ISDIR(st->st_mode))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (S_ISLNK(st->st_mode))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISBLK(st->st_mode))
                ಹಿಂತಿರುಗಿ -ENOTBLK;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_verify_ಖಂಡ(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಹಿಂತಿರುಗಿ verify_stat_at(fd, /* path= */ ಖಾಲಿ, /* follow= */ false, stat_verify_ಖಂಡ, /* verify= */ true);
}

ಪೂರ್ಣಾಂಕ stat_verify_char(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಆದರೆ (S_ISDIR(st->st_mode))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (S_ISLNK(st->st_mode))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISCHR(st->st_mode))
                ಹಿಂತಿರುಗಿ -EBADFD;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ stat_verify_device_ಗಂಟು(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಆದರೆ (S_ISDIR(st->st_mode))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (S_ISLNK(st->st_mode))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ಹಿಂತಿರುಗಿ -ENOTTY;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ is_device_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(!isempty(ಮಾರ್ಗ));
        ಹಿಂತಿರುಗಿ verify_stat_at(AT_FDCWD, ಮಾರ್ಗ, false, stat_verify_device_ಗಂಟು, false);
}

ಪೂರ್ಣಾಂಕ stat_verify_regular_or_ಖಂಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಆದರೆ (S_ISDIR(st->st_mode))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (S_ISLNK(st->st_mode))
                ಹಿಂತಿರುಗಿ -ELOOP;

        ಆದರೆ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ಹಿಂತಿರುಗಿ -EBADFD;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_verify_regular_or_ಖಂಡ(ಪೂರ್ಣಾಂಕ fd) {
        ಆದರೆ (ಒಳ_ಗಣ(fd, AT_FDCWD, XAT_FDROOT))
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಹಿಂತಿರುಗಿ verify_stat_at(fd, /* path= */ ಖಾಲಿ, /* follow= */ false, stat_verify_regular_or_ಖಂಡ, /* verify= */ true);
}

ಪೂರ್ಣಾಂಕ dir_is_ಖಾಲಿಗಳು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool ignore_hidden_or_backup) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ರಚನೆ dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, ಮಾರ್ಗ, O_DIRECTORY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_ಗಾತ್ರ_MAX;
        buf = alloca(m);

        ಗಾಗಿ (;;) {
                ರಚನೆ dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (n == 0)
                        ನಿಲ್ಲಿಸು;

                ಪ್ರತಿಜ್ಞಾ((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_ಒಳ_ಬಫರ್(de, buf, n)
                        ಆದರೆ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ 1;
}

bool stat_may_be_dev_null(ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ಹಿಂತಿರುಗಿ S_ISCHR(st->st_mode);
}

bool stat_is_ಖಾಲಿಗಳು(ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(st);

        ಹಿಂತಿರುಗಿ S_ISREG(st->st_mode) && st->st_size <= 0;
}

ಪೂರ್ಣಾಂಕ null_or_ಖಾಲಿಗಳು_ಮಾರ್ಗ_with_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು) {
        _cleanup_free_ ಅಕ್ಷರ *resolved = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ರಚನೆ stat st;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fn);

        /* fn includes the root prefix. */

        ಆದರೆ (ಮಾರ್ಗ_equal(ಮಾರ್ಗ_startswith(fn, ಬೇರು ?: "/"), "dev/null"))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು)) {
                ಆದರೆ (stat(fn, &st) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ null_or_ಖಾಲಿಗಳು(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, ಬೇರು, CHASE_NONEXISTENT, &resolved, &fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಮಾರ್ಗ_equal(ಮಾರ್ಗ_startswith(resolved, ಬೇರು), "dev/null"))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -ENOENT;

        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ null_or_ಖಾಲಿಗಳು(&st);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* statx_ಮುಖವಾಡ_one_ಗಮ್ಯ_name(ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ_3);
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* statx_attribute_ಗಮ್ಯ_name(uint64_t attr);

#ಅನ್ತರ್ಭಾವ "statx-attribute-to-name.inc"
#ಅನ್ತರ್ಭಾವ "statx-mask-to-name.inc"

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STATX_ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_STRING(ಪೂರ್ವಪ್ರತ್ಯಯ, type, func, format_str)             \
        ಸ್ಥಿರ ಅಕ್ಷರ* ಪೂರ್ವಪ್ರತ್ಯಯ##_to_string(type v) {                               \
                ಆದರೆ (v == 0)                                                     \
                        ಹಿಂತಿರುಗಿ strdup("");                                      \
                                                                                \
                _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;                                  \
                                                                                \
                ಬಿಟ್_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n = func(f);                                \
                        ಆದರೆ (!n)                                                 \
                                ಮುಂದುವರಿ;                                       \
                                                                                \
                        ಆದರೆ (!strextend_with_separator(&s, "|", n))              \
                                ಹಿಂತಿರುಗಿ ಖಾಲಿ;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ಆದರೆ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;                                            \
                                                                                \
                ಹಿಂತಿರುಗಿ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_STRING(statx_ಮುಖವಾಡ,       ಚಿಹ್ನೆರಹಿತ, statx_ಮುಖವಾಡ_one_ಗಮ್ಯ_name,  "0x%x");
DEFINE_STATX_ಬಿಟ್ಟುಗಳು_ಗಮ್ಯ_STRING(statx_ಗುಣಲಕ್ಷಣಗಳು, uint64_t, statx_attribute_ಗಮ್ಯ_name, "0x%" PRIx64);

ಪೂರ್ಣಾಂಕ xstatx_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ statx_flags,
                XStatXFlags xstatx_flags,
                ಚಿಹ್ನೆರಹಿತ mandatory_ಮುಖವಾಡ,
                ಚಿಹ್ನೆರಹಿತ optional_ಮುಖವಾಡ,
                uint64_t mandatory_ಗುಣಲಕ್ಷಣಗಳು,
                ರಚನೆ statx *ret) {

        ರಚನೆ statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        ಪೂರ್ಣಾಂಕ r;

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
        ಪ್ರತಿಜ್ಞಾ((mandatory_ಮುಖವಾಡ & optional_ಮುಖವಾಡ) == 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(xstatx_flags, XSTATX_MNT_ID_ಉತ್ತಮ) || !((mandatory_ಮುಖವಾಡ|optional_ಮುಖವಾಡ) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        r = resolve_xat_fdroot(&fd, &ಮಾರ್ಗ, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಚಿಹ್ನೆರಹಿತ request_ಮುಖವಾಡ = mandatory_ಮುಖವಾಡ|optional_ಮುಖವಾಡ;
        ಆದರೆ (FLAGS_ಗಣ(xstatx_flags, XSTATX_MNT_ID_ಉತ್ತಮ))
                request_ಮುಖವಾಡ |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ಆದರೆ (statx(fd,
                  strempty(ಮಾರ್ಗ),
                  statx_flags|(isempty(ಮಾರ್ಗ) ? AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ : 0),
                  request_ಮುಖವಾಡ,
                  &sx) < 0)
                ಹಿಂತಿರುಗಿ negative_errno();

        ಆದರೆ (FLAGS_ಗಣ(xstatx_flags, XSTATX_MNT_ID_ಉತ್ತಮ) &&
            !(sx.stx_ಮುಖವಾಡ & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ಆದರೆ (!FLAGS_ಗಣ(sx.stx_ಮುಖವಾಡ, mandatory_ಮುಖವಾಡ)) {
                ಆದರೆ (DEBUG_LOGGING) {
                        _cleanup_free_ ಅಕ್ಷರ *ಮುಖವಾಡ_str = statx_ಮುಖವಾಡ_ಗಮ್ಯ_string(mandatory_ಮುಖವಾಡ & ~sx.stx_ಮುಖವಾಡ);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(ಮುಖವಾಡ_str));
                }

                ಹಿಂತಿರುಗಿ -EUNATCH;
        }

        ಆದರೆ (!FLAGS_ಗಣ(sx.stx_ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ, mandatory_ಗುಣಲಕ್ಷಣಗಳು)) {
                ಆದರೆ (DEBUG_LOGGING) {
                        _cleanup_free_ ಅಕ್ಷರ *attr_str = statx_ಗುಣಲಕ್ಷಣಗಳು_ಗಮ್ಯ_string(mandatory_ಗುಣಲಕ್ಷಣಗಳು & ~sx.stx_ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ಹಿಂತಿರುಗಿ -EUNATCH;
        }

        *ret = sx;
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(sx.stx_ಮುಖವಾಡ, optional_ಮುಖವಾಡ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xfstatfs(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ statfs *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (fd == AT_FDCWD)
                ಹಿಂತಿರುಗಿ RET_NERRNO(statfs(".", ret));
        ಆದರೆ (fd == XAT_FDROOT)
                ಹಿಂತಿರುಗಿ RET_NERRNO(statfs("/", ret));

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಹಿಂತಿರುಗಿ RET_NERRNO(fstatfs(fd, ret));
}

ಪೂರ್ಣಾಂಕ xstatfsat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ರಚನೆ statfs *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!isempty(ಮಾರ್ಗ)) {
                fd = xopenat(dir_fd, ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ fd;
                dir_fd = fd;
        }

        ಹಿಂತಿರುಗಿ xfstatfs(dir_fd, ret);
}

ಪೂರ್ಣಾಂಕ fd_is_read_only_fs(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ರಚನೆ statfs st;
        r = xfstatfs(fd, &st);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (st.f_flags & ST_RDONLY)
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (is_ಜಾಲ_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ಹಿಂತಿರುಗಿ access_fd(fd, W_OK) == -EROFS;

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_read_only_fs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        fd = open(ಮಾರ್ಗ, O_CLOEXEC | O_ಮಾರ್ಗ);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ fd_is_read_only_fs(fd);
}

ಪೂರ್ಣಾಂಕ inode_same_at(ಪೂರ್ಣಾಂಕ fda, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filea, ಪೂರ್ಣಾಂಕ fdb, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fileb, ಪೂರ್ಣಾಂಕ flags) {
        ರಚನೆ stat sta, stb;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fda >= 0 || fda == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(fdb >= 0 || fdb == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((flags & ~(AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ಆದರೆ ((isempty(filea) || isempty(fileb)) && !FLAGS_ಗಣ(flags, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ಆದರೆ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_ಗಣ(flags, AT_SYMLINK_NOFOLLOW))
                ಹಿಂತಿರುಗಿ true;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ pin_a = -EBADF, pin_b = -EBADF;
        ಆದರೆ (!FLAGS_ಗಣ(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ಆದರೆ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_ಮಾರ್ಗ|O_CLOEXEC|(FLAGS_ಗಣ(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ಆದರೆ (pin_a < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        fda = pin_a;
                        filea = ಖಾಲಿ;
                        flags |= AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ;
                }

                ಆದರೆ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_ಮಾರ್ಗ|O_CLOEXEC|(FLAGS_ಗಣ(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ಆದರೆ (pin_b < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        fdb = pin_b;
                        fileb = ಖಾಲಿ;
                        flags |= AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ;
                }

                ಪೂರ್ಣಾಂಕ ntha_flags = at_flags_normalize_follow(flags) & (AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ|AT_SYMLINK_FOLLOW);
                _cleanup_free_ ರಚನೆ file_handle *ha = ಖಾಲಿ, *hb = ಖಾಲಿ;
                uint64_t mntida, mntidb;
                ಪೂರ್ಣಾಂಕ _mntida, _mntidb;

                r = name_ಗಮ್ಯ_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (is_name_ಗಮ್ಯ_handle_at_fatal_error(r))
                                ಹಿಂತಿರುಗಿ r;

                        ಹೋಗು fallback;
                }
                bool have_unique_mntid = r > 0;

                ಆದರೆ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_ಗಮ್ಯ_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ಖಾಲಿ : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ಖಾಲಿ,
                                ntha_flags);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (is_name_ಗಮ್ಯ_handle_at_fatal_error(r))
                                ಹಿಂತಿರುಗಿ r;

                        ಹೋಗು fallback;
                }
                ಆದರೆ (r == 0) {
                        ಪ್ರತಿಜ್ಞಾ(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ಆದರೆ (!file_handle_equal(ha, hb))
                        ಹಿಂತಿರುಗಿ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ಆದರೆ (mntida == mntidb)
                        ಹಿಂತಿರುಗಿ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ಆದರೆ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ಆದರೆ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ಹಿಂತಿರುಗಿ stat_inode_same(&sta, &stb);
}

bool is_fs_type(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(s);
        assert_cc(ಗಾತ್ರ(statfs_f_type_t) >= ಗಾತ್ರ(s->f_type));

        ಹಿಂತಿರುಗಿ F_TYPE_EQUAL(s->f_type, magic_ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ is_fs_type_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ರಚನೆ statfs s;
        r = xstatfsat(dir_fd, ಮಾರ್ಗ, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ is_fs_type(&s, magic_ಮೌಲ್ಯ);
}

bool is_temporary_fs(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s) {
        ಹಿಂತಿರುಗಿ fs_ಒಳ_ಗುಂಪು(s, FILESYSTEM_ಗಣ_TEMPORARY);
}

bool is_ಜಾಲ_fs(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s) {
        ಹಿಂತಿರುಗಿ fs_ಒಳ_ಗುಂಪು(s, FILESYSTEM_ಗಣ_ಜಾಲ);
}

ಪೂರ್ಣಾಂಕ fd_is_temporary_fs(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ರಚನೆ statfs s;
        r = xfstatfs(fd, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ is_temporary_fs(&s);
}

ಪೂರ್ಣಾಂಕ fd_is_ಜಾಲ_fs(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ರಚನೆ statfs s;
        r = xfstatfs(fd, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ is_ಜಾಲ_fs(&s);
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_temporary_fs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ರಚನೆ statfs s;

        ಆದರೆ (statfs(ಮಾರ್ಗ, &s) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ is_temporary_fs(&s);
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_ಜಾಲ_fs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ರಚನೆ statfs s;

        ಆದರೆ (statfs(ಮಾರ್ಗ, &s) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ is_ಜಾಲ_fs(&s);
}

ಪೂರ್ಣಾಂಕ proc_mounted(ಶೂನ್ಯ) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        ಪೂರ್ಣಾಂಕ r;

        /* A quick check of procfs is properly mounted */

        r = ಮಾರ್ಗ_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ಆದರೆ (r == -ENOENT) /* not mounted at all */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ r;
}

bool stat_inode_same(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ಹಿಂತಿರುಗಿ stat_is_ಗಣ(a) && stat_is_ಗಣ(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b) {

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

        ಹಿಂತಿರುಗಿ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *a, ಸ್ಥಿರಾಂಕ ರಚನೆ statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ಆದರೆ (!statx_is_ಗಣ(a) || !statx_is_ಗಣ(b))
                ಹಿಂತಿರುಗಿ false;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(a->stx_ಮುಖವಾಡ, STATX_TYPE|STATX_INO));
        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(b->stx_ಮುಖವಾಡ, STATX_TYPE|STATX_INO));

        ಹಿಂತಿರುಗಿ
                ((a->stx_ಬಹುಲಕ ^ b->stx_ಬಹುಲಕ) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

ಪೂರ್ಣಾಂಕ statx_mount_same(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *a, ಸ್ಥಿರಾಂಕ ರಚನೆ statx *b) {
        ಆದರೆ (!statx_is_ಗಣ(a) || !statx_is_ಗಣ(b))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ ((FLAGS_ಗಣ(a->stx_ಮುಖವಾಡ, STATX_MNT_ID) && FLAGS_ಗಣ(b->stx_ಮುಖವಾಡ, STATX_MNT_ID)) ||
            (FLAGS_ಗಣ(a->stx_ಮುಖವಾಡ, STATX_MNT_ID_UNIQUE) && FLAGS_ಗಣ(b->stx_ಮುಖವಾಡ, STATX_MNT_ID_UNIQUE)))
                ಹಿಂತಿರುಗಿ a->stx_mnt_id == b->stx_mnt_id;

        ಹಿಂತಿರುಗಿ -ENODATA;
}

usec_t statx_timestamp_ತುಂಬು(ಸ್ಥಿರಾಂಕ ರಚನೆ statx_timestamp *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);
        ಹಿಂತಿರುಗಿ timespec_ತುಂಬು(&(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_ತುಂಬು_nsec(ಸ್ಥಿರಾಂಕ ರಚನೆ statx_timestamp *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);
        ಹಿಂತಿರುಗಿ timespec_ತುಂಬು_nsec(&(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

ಶೂನ್ಯ inode_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *q, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

ಪೂರ್ಣಾಂಕ inode_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(a->st_dev, b->st_dev);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        r = CMP(a->st_ino, b->st_ino);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(inode_hash_ops, ರಚನೆ stat, inode_hash_func, inode_compare_func, ಬಿಡುಗಡೆ);

ಶೂನ್ಯ inode_unmodified_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *q, ರಚನೆ siphash *state) {
        ಪ್ರತಿಜ್ಞಾ(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ಆದರೆ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ಇಲ್ಲದಿದ್ದರೆ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ಆದರೆ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ಇಲ್ಲದಿದ್ದರೆ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

ಪೂರ್ಣಾಂಕ inode_unmodified_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = inode_compare_func(a, b);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(inode_unmodified_hash_ops, ರಚನೆ stat, inode_unmodified_hash_func, inode_unmodified_compare_func, ಬಿಡುಗಡೆ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* inode_type_ಗಮ್ಯ_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        ಆಯ್ಕೆ (m & S_IFMT) {
        ಸ್ಥಿತಿ S_IFREG:
                ಹಿಂತಿರುಗಿ "reg";
        ಸ್ಥಿತಿ S_IFDIR:
                ಹಿಂತಿರುಗಿ "dir";
        ಸ್ಥಿತಿ S_IFLNK:
                ಹಿಂತಿರುಗಿ "lnk";
        ಸ್ಥಿತಿ S_IFCHR:
                ಹಿಂತಿರುಗಿ "chr";
        ಸ್ಥಿತಿ S_IFBLK:
                ಹಿಂತಿರುಗಿ "blk";
        ಸ್ಥಿತಿ S_IFIFO:
                ಹಿಂತಿರುಗಿ "fifo";
        ಸ್ಥಿತಿ S_IFSOCK:
                ಹಿಂತಿರುಗಿ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

mode_t inode_type_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ ಬಹುಲಕ_INVALID;

        ಆದರೆ (streq(s, "reg"))
                ಹಿಂತಿರುಗಿ S_IFREG;
        ಆದರೆ (streq(s, "dir"))
                ಹಿಂತಿರುಗಿ S_IFDIR;
        ಆದರೆ (streq(s, "lnk"))
                ಹಿಂತಿರುಗಿ S_IFLNK;
        ಆದರೆ (streq(s, "chr"))
                ಹಿಂತಿರುಗಿ S_IFCHR;
        ಆದರೆ (streq(s, "blk"))
                ಹಿಂತಿರುಗಿ S_IFBLK;
        ಆದರೆ (streq(s, "fifo"))
                ಹಿಂತಿರುಗಿ S_IFIFO;
        ಆದರೆ (streq(s, "sock"))
                ಹಿಂತಿರುಗಿ S_IFSOCK;

        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_INVALID;
}

ಪೂರ್ಣಾಂಕ vfs_free_ಬೈಟುಗಳು(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        ರಚನೆ statvfs sv;
        ಆದರೆ (fstatvfs(fd, &sv) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (!MUL_ಸುರಕ್ಷಿತ(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ 0;
}
