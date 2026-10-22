/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/magic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/statvfs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bitfield.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "filesystems.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash-funcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mountpoint-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

ਸਥਿਰ ਪੂਰਨ verify_stat_at(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                bool follow,
                ਪੂਰਨ (*verify_func)(ਅਚਲ ਬਣਤਰ stat *st),
                bool verify) {

        ਬਣਤਰ stat st;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(ਰਸਤਾ) || !follow);
        ਪ੍ਰਤਿਜ੍ਞਾ(verify_func);

        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        r = resolve_xat_fdroot(&fd, &ਰਸਤਾ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (fstatat(fd, strempty(ਰਸਤਾ), &st,
                    (isempty(ਰਸਤਾ) ? AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ਵਾਪਸ -errno;

        r = verify_func(&st);
        ਵਾਪਸ verify ? r : r >= 0;
}

ਸਥਿਰ ਪੂਰਨ ਬਹੁਲਕ_verify_regular(mode_t ਬਹੁਲਕ) {
        ਜੇ (S_ISDIR(ਬਹੁਲਕ))
                ਵਾਪਸ -EISDIR;

        ਜੇ (S_ISLNK(ਬਹੁਲਕ))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISREG(ਬਹੁਲਕ))
                ਵਾਪਸ -EBADFD;

        ਵਾਪਸ 0;
}

ਪੂਰਨ stat_verify_regular(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ਵਾਪਸ ਬਹੁਲਕ_verify_regular(st->st_mode);
}

ਪੂਰਨ statx_verify_regular(ਅਚਲ ਬਣਤਰ statx *stx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(stx);

        ਜੇ (!FLAGS_ਸੈੱਟ(stx->stx_ਨਕਾਬ, STATX_TYPE))
                ਵਾਪਸ -ENODATA;

        ਵਾਪਸ ਬਹੁਲਕ_verify_regular(stx->stx_ਬਹੁਲਕ);
}

ਪੂਰਨ verify_regular_at(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool follow) {
        ਵਾਪਸ verify_stat_at(fd, ਰਸਤਾ, follow, stat_verify_regular, true);
}

ਪੂਰਨ fd_verify_regular(ਪੂਰਨ fd) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT))
                ਵਾਪਸ -EISDIR;

        ਵਾਪਸ verify_regular_at(fd, /* path= */ ਨਿਰਾ, /* follow= */ false);
}

ਸਥਿਰ ਪੂਰਨ ਬਹੁਲਕ_verify_directory(mode_t ਬਹੁਲਕ) {
        ਜੇ (S_ISLNK(ਬਹੁਲਕ))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISDIR(ਬਹੁਲਕ))
                ਵਾਪਸ -ENOTDIR;

        ਵਾਪਸ 0;
}

ਪੂਰਨ stat_verify_directory(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਵਾਪਸ ਬਹੁਲਕ_verify_directory(st->st_mode);
}

ਪੂਰਨ statx_verify_directory(ਅਚਲ ਬਣਤਰ statx *stx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(stx);

        ਜੇ (!FLAGS_ਸੈੱਟ(stx->stx_ਨਕਾਬ, STATX_TYPE))
                ਵਾਪਸ -ENODATA;

        ਵਾਪਸ ਬਹੁਲਕ_verify_directory(stx->stx_ਬਹੁਲਕ);
}

ਪੂਰਨ fd_verify_directory(ਪੂਰਨ fd) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT))
                ਵਾਪਸ 0;

        ਵਾਪਸ verify_stat_at(fd, ਨਿਰਾ, false, stat_verify_directory, true);
}

ਪੂਰਨ is_dir_at(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool follow) {
        ਵਾਪਸ verify_stat_at(fd, ਰਸਤਾ, follow, stat_verify_directory, false);
}

ਪੂਰਨ is_dir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool follow) {
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(ਰਸਤਾ));
        ਵਾਪਸ is_dir_at(AT_FDCWD, ਰਸਤਾ, follow);
}

ਪੂਰਨ stat_verify_symlink(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਜੇ (S_ISDIR(st->st_mode))
                ਵਾਪਸ -EISDIR;

        ਜੇ (!S_ISLNK(st->st_mode))
                ਵਾਪਸ -ENOLINK;

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_verify_symlink(ਪੂਰਨ fd) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT))
                ਵਾਪਸ -EISDIR;

        ਵਾਪਸ verify_stat_at(fd, /* path= */ ਨਿਰਾ, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

ਪੂਰਨ is_symlink(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(ਰਸਤਾ));
        ਵਾਪਸ verify_stat_at(AT_FDCWD, ਰਸਤਾ, false, stat_verify_symlink, false);
}

ਸਥਿਰ ਪੂਰਨ ਬਹੁਲਕ_verify_socket(mode_t ਬਹੁਲਕ) {
        ਜੇ (S_ISDIR(ਬਹੁਲਕ))
                ਵਾਪਸ -EISDIR;

        ਜੇ (S_ISLNK(ਬਹੁਲਕ))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISSOCK(ਬਹੁਲਕ))
                ਵਾਪਸ -ENOTSOCK;

        ਵਾਪਸ 0;
}

ਪੂਰਨ stat_verify_socket(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਵਾਪਸ ਬਹੁਲਕ_verify_socket(st->st_mode);
}

ਪੂਰਨ statx_verify_socket(ਅਚਲ ਬਣਤਰ statx *stx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(stx);

        ਵਾਪਸ ਬਹੁਲਕ_verify_socket(stx->stx_ਬਹੁਲਕ);
}

ਪੂਰਨ fd_verify_socket(ਪੂਰਨ fd) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT))
                ਵਾਪਸ -EISDIR;

        ਵਾਪਸ verify_stat_at(fd, /* path= */ ਨਿਰਾ, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

ਪੂਰਨ is_socket(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(ਰਸਤਾ));
        ਵਾਪਸ verify_stat_at(AT_FDCWD, ਰਸਤਾ, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

ਪੂਰਨ stat_verify_linked(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਜੇ (st->st_nlink <= 0)
                ਵਾਪਸ -EIDRM; /* recognizable error. */

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_verify_linked(ਪੂਰਨ fd) {

        ਜੇ (fd == XAT_FDROOT)
                ਵਾਪਸ 0;

        ਵਾਪਸ verify_stat_at(fd, ਨਿਰਾ, false, stat_verify_linked, true);
}

ਪੂਰਨ stat_verify_ਖੰਡ(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਜੇ (S_ISDIR(st->st_mode))
                ਵਾਪਸ -EISDIR;

        ਜੇ (S_ISLNK(st->st_mode))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISBLK(st->st_mode))
                ਵਾਪਸ -ENOTBLK;

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_verify_ਖੰਡ(ਪੂਰਨ fd) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT))
                ਵਾਪਸ -EISDIR;

        ਵਾਪਸ verify_stat_at(fd, /* path= */ ਨਿਰਾ, /* follow= */ false, stat_verify_ਖੰਡ, /* verify= */ true);
}

ਪੂਰਨ stat_verify_char(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਜੇ (S_ISDIR(st->st_mode))
                ਵਾਪਸ -EISDIR;

        ਜੇ (S_ISLNK(st->st_mode))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISCHR(st->st_mode))
                ਵਾਪਸ -EBADFD;

        ਵਾਪਸ 0;
}

ਪੂਰਨ stat_verify_device_ਗੰਢ(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਜੇ (S_ISDIR(st->st_mode))
                ਵਾਪਸ -EISDIR;

        ਜੇ (S_ISLNK(st->st_mode))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ਵਾਪਸ -ENOTTY;

        ਵਾਪਸ 0;
}

ਪੂਰਨ is_device_ਗੰਢ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(ਰਸਤਾ));
        ਵਾਪਸ verify_stat_at(AT_FDCWD, ਰਸਤਾ, false, stat_verify_device_ਗੰਢ, false);
}

ਪੂਰਨ stat_verify_regular_or_ਖੰਡ(ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਜੇ (S_ISDIR(st->st_mode))
                ਵਾਪਸ -EISDIR;

        ਜੇ (S_ISLNK(st->st_mode))
                ਵਾਪਸ -ELOOP;

        ਜੇ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ਵਾਪਸ -EBADFD;

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_verify_regular_or_ਖੰਡ(ਪੂਰਨ fd) {
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, AT_FDCWD, XAT_FDROOT))
                ਵਾਪਸ -EISDIR;

        ਵਾਪਸ verify_stat_at(fd, /* path= */ ਨਿਰਾ, /* follow= */ false, stat_verify_regular_or_ਖੰਡ, /* verify= */ true);
}

ਪੂਰਨ dir_is_ਖਾਲੀ_ਥਾਵਾਂ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool ignore_hidden_or_backup) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਬਣਤਰ dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, ਰਸਤਾ, O_DIRECTORY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_ਮਾਪ_MAX;
        buf = alloca(m);

        ਲਈ (;;) {
                ਬਣਤਰ dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (n == 0)
                        ਰੋਕੋ;

                ਪ੍ਰਤਿਜ੍ਞਾ((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_ਵਿੱਚ_ਬਫਰ(de, buf, n)
                        ਜੇ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ਵਾਪਸ 0;
        }

        ਵਾਪਸ 1;
}

bool stat_may_be_dev_null(ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ਵਾਪਸ S_ISCHR(st->st_mode);
}

bool stat_is_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        ਵਾਪਸ S_ISREG(st->st_mode) && st->st_size <= 0;
}

ਪੂਰਨ null_or_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ_with_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ) {
        _cleanup_free_ ਅੱਖਰ *resolved = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਬਣਤਰ stat st;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fn);

        /* fn includes the root prefix. */

        ਜੇ (ਰਸਤਾ_equal(ਰਸਤਾ_startswith(fn, ਜੜ੍ਹ ?: "/"), "dev/null"))
                ਵਾਪਸ true;

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ)) {
                ਜੇ (stat(fn, &st) < 0)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ null_or_ਖਾਲੀ_ਥਾਵਾਂ(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, ਜੜ੍ਹ, CHASE_NONEXISTENT, &resolved, &fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਰਸਤਾ_equal(ਰਸਤਾ_startswith(resolved, ਜੜ੍ਹ), "dev/null"))
                ਵਾਪਸ true;

        ਜੇ (r == 0)
                ਵਾਪਸ -ENOENT;

        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ null_or_ਖਾਲੀ_ਥਾਵਾਂ(&st);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* statx_ਨਕਾਬ_one_ਮੰਜ਼ਿਲ_name(ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ_3);
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* statx_attribute_ਮੰਜ਼ਿਲ_name(uint64_t attr);

#ਅਨ੍ਤਰ੍ਭਾਵ "statx-attribute-to-name.inc"
#ਅਨ੍ਤਰ੍ਭਾਵ "statx-mask-to-name.inc"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STATX_ਬਿੱਟ_ਮੰਜ਼ਿਲ_STRING(ਅਗੇਤਰ, type, func, format_str)             \
        ਸਥਿਰ ਅੱਖਰ* ਅਗੇਤਰ##_to_string(type v) {                               \
                ਜੇ (v == 0)                                                     \
                        ਵਾਪਸ strdup("");                                      \
                                                                                \
                _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;                                  \
                                                                                \
                ਦੋਆਧਾਰੀ_ਅੰਕ_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ਅਚਲ ਅੱਖਰ *n = func(f);                                \
                        ਜੇ (!n)                                                 \
                                ਜਾਰੀ;                                       \
                                                                                \
                        ਜੇ (!strextend_with_separator(&s, "|", n))              \
                                ਵਾਪਸ ਨਿਰਾ;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ਜੇ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ਵਾਪਸ ਨਿਰਾ;                                            \
                                                                                \
                ਵਾਪਸ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_ਬਿੱਟ_ਮੰਜ਼ਿਲ_STRING(statx_ਨਕਾਬ,       ਬਿਨਾਨਿਸ਼ਾਨੀ, statx_ਨਕਾਬ_one_ਮੰਜ਼ਿਲ_name,  "0x%x");
DEFINE_STATX_ਬਿੱਟ_ਮੰਜ਼ਿਲ_STRING(statx_ਗੁਣ, uint64_t, statx_attribute_ਮੰਜ਼ਿਲ_name, "0x%" PRIx64);

ਪੂਰਨ xstatx_ਭਰਿਆ(ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ statx_flags,
                XStatXFlags xstatx_flags,
                ਬਿਨਾਨਿਸ਼ਾਨੀ mandatory_ਨਕਾਬ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ optional_ਨਕਾਬ,
                uint64_t mandatory_ਗੁਣ,
                ਬਣਤਰ statx *ret) {

        ਬਣਤਰ statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        ਪੂਰਨ r;

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

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(fd));
        ਪ੍ਰਤਿਜ੍ਞਾ((mandatory_ਨਕਾਬ & optional_ਨਕਾਬ) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(xstatx_flags, XSTATX_MNT_ID_ਉੱਤਮ) || !((mandatory_ਨਕਾਬ|optional_ਨਕਾਬ) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        r = resolve_xat_fdroot(&fd, &ਰਸਤਾ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਬਿਨਾਨਿਸ਼ਾਨੀ request_ਨਕਾਬ = mandatory_ਨਕਾਬ|optional_ਨਕਾਬ;
        ਜੇ (FLAGS_ਸੈੱਟ(xstatx_flags, XSTATX_MNT_ID_ਉੱਤਮ))
                request_ਨਕਾਬ |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ਜੇ (statx(fd,
                  strempty(ਰਸਤਾ),
                  statx_flags|(isempty(ਰਸਤਾ) ? AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ : 0),
                  request_ਨਕਾਬ,
                  &sx) < 0)
                ਵਾਪਸ negative_errno();

        ਜੇ (FLAGS_ਸੈੱਟ(xstatx_flags, XSTATX_MNT_ID_ਉੱਤਮ) &&
            !(sx.stx_ਨਕਾਬ & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ਜੇ (!FLAGS_ਸੈੱਟ(sx.stx_ਨਕਾਬ, mandatory_ਨਕਾਬ)) {
                ਜੇ (DEBUG_LOGGING) {
                        _cleanup_free_ ਅੱਖਰ *ਨਕਾਬ_str = statx_ਨਕਾਬ_ਮੰਜ਼ਿਲ_string(mandatory_ਨਕਾਬ & ~sx.stx_ਨਕਾਬ);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(ਨਕਾਬ_str));
                }

                ਵਾਪਸ -EUNATCH;
        }

        ਜੇ (!FLAGS_ਸੈੱਟ(sx.stx_ਗੁਣ_ਨਕਾਬ, mandatory_ਗੁਣ)) {
                ਜੇ (DEBUG_LOGGING) {
                        _cleanup_free_ ਅੱਖਰ *attr_str = statx_ਗੁਣ_ਮੰਜ਼ਿਲ_string(mandatory_ਗੁਣ & ~sx.stx_ਗੁਣ_ਨਕਾਬ);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ਵਾਪਸ -EUNATCH;
        }

        *ret = sx;
        ਵਾਪਸ FLAGS_ਸੈੱਟ(sx.stx_ਨਕਾਬ, optional_ਨਕਾਬ);
}

ਸਥਿਰ ਪੂਰਨ xfstatfs(ਪੂਰਨ fd, ਬਣਤਰ statfs *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (fd == AT_FDCWD)
                ਵਾਪਸ RET_NERRNO(statfs(".", ret));
        ਜੇ (fd == XAT_FDROOT)
                ਵਾਪਸ RET_NERRNO(statfs("/", ret));

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਵਾਪਸ RET_NERRNO(fstatfs(fd, ret));
}

ਪੂਰਨ xstatfsat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਣਤਰ statfs *ret) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!isempty(ਰਸਤਾ)) {
                fd = xopenat(dir_fd, ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC);
                ਜੇ (fd < 0)
                        ਵਾਪਸ fd;
                dir_fd = fd;
        }

        ਵਾਪਸ xfstatfs(dir_fd, ret);
}

ਪੂਰਨ fd_is_read_only_fs(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਬਣਤਰ statfs st;
        r = xfstatfs(fd, &st);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (st.f_flags & ST_RDONLY)
                ਵਾਪਸ true;

        ਜੇ (is_ਜਾਲ_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ਵਾਪਸ access_fd(fd, W_OK) == -EROFS;

        ਵਾਪਸ false;
}

ਪੂਰਨ ਰਸਤਾ_is_read_only_fs(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        fd = open(ਰਸਤਾ, O_CLOEXEC | O_ਰਸਤਾ);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ fd_is_read_only_fs(fd);
}

ਪੂਰਨ inode_same_at(ਪੂਰਨ fda, ਅਚਲ ਅੱਖਰ *filea, ਪੂਰਨ fdb, ਅਚਲ ਅੱਖਰ *fileb, ਪੂਰਨ flags) {
        ਬਣਤਰ stat sta, stb;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fda >= 0 || fda == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(fdb >= 0 || fdb == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~(AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ਜੇ ((isempty(filea) || isempty(fileb)) && !FLAGS_ਸੈੱਟ(flags, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ਜੇ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_NOFOLLOW))
                ਵਾਪਸ true;

        _cleanup_close_ ਪੂਰਨ pin_a = -EBADF, pin_b = -EBADF;
        ਜੇ (!FLAGS_ਸੈੱਟ(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ਜੇ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_ਰਸਤਾ|O_CLOEXEC|(FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ਜੇ (pin_a < 0)
                                ਵਾਪਸ -errno;

                        fda = pin_a;
                        filea = ਨਿਰਾ;
                        flags |= AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ;
                }

                ਜੇ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_ਰਸਤਾ|O_CLOEXEC|(FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ਜੇ (pin_b < 0)
                                ਵਾਪਸ -errno;

                        fdb = pin_b;
                        fileb = ਨਿਰਾ;
                        flags |= AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ;
                }

                ਪੂਰਨ ntha_flags = at_flags_normalize_follow(flags) & (AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ|AT_SYMLINK_FOLLOW);
                _cleanup_free_ ਬਣਤਰ file_handle *ha = ਨਿਰਾ, *hb = ਨਿਰਾ;
                uint64_t mntida, mntidb;
                ਪੂਰਨ _mntida, _mntidb;

                r = name_ਮੰਜ਼ਿਲ_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ਜੇ (r < 0) {
                        ਜੇ (is_name_ਮੰਜ਼ਿਲ_handle_at_fatal_error(r))
                                ਵਾਪਸ r;

                        ਜਾਓ fallback;
                }
                bool have_unique_mntid = r > 0;

                ਜੇ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_ਮੰਜ਼ਿਲ_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ਨਿਰਾ : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ਨਿਰਾ,
                                ntha_flags);
                ਜੇ (r < 0) {
                        ਜੇ (is_name_ਮੰਜ਼ਿਲ_handle_at_fatal_error(r))
                                ਵਾਪਸ r;

                        ਜਾਓ fallback;
                }
                ਜੇ (r == 0) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ਜੇ (!file_handle_equal(ha, hb))
                        ਵਾਪਸ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ਜੇ (mntida == mntidb)
                        ਵਾਪਸ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ਜੇ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ਜੇ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ਵਾਪਸ stat_inode_same(&sta, &stb);
}

bool is_fs_type(ਅਚਲ ਬਣਤਰ statfs *s, statfs_f_type_t magic_ਮੁੱਲ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        assert_cc(ਆਕਾਰ(statfs_f_type_t) >= ਆਕਾਰ(s->f_type));

        ਵਾਪਸ F_TYPE_EQUAL(s->f_type, magic_ਮੁੱਲ);
}

ਪੂਰਨ is_fs_type_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, statfs_f_type_t magic_ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਬਣਤਰ statfs s;
        r = xstatfsat(dir_fd, ਰਸਤਾ, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ is_fs_type(&s, magic_ਮੁੱਲ);
}

bool is_temporary_fs(ਅਚਲ ਬਣਤਰ statfs *s) {
        ਵਾਪਸ fs_ਵਿੱਚ_ਸਮੂਹ(s, FILESYSTEM_ਸੈੱਟ_TEMPORARY);
}

bool is_ਜਾਲ_fs(ਅਚਲ ਬਣਤਰ statfs *s) {
        ਵਾਪਸ fs_ਵਿੱਚ_ਸਮੂਹ(s, FILESYSTEM_ਸੈੱਟ_ਜਾਲ);
}

ਪੂਰਨ fd_is_temporary_fs(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਬਣਤਰ statfs s;
        r = xfstatfs(fd, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ is_temporary_fs(&s);
}

ਪੂਰਨ fd_is_ਜਾਲ_fs(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਬਣਤਰ statfs s;
        r = xfstatfs(fd, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ is_ਜਾਲ_fs(&s);
}

ਪੂਰਨ ਰਸਤਾ_is_temporary_fs(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਬਣਤਰ statfs s;

        ਜੇ (statfs(ਰਸਤਾ, &s) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ is_temporary_fs(&s);
}

ਪੂਰਨ ਰਸਤਾ_is_ਜਾਲ_fs(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਬਣਤਰ statfs s;

        ਜੇ (statfs(ਰਸਤਾ, &s) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ is_ਜਾਲ_fs(&s);
}

ਪੂਰਨ proc_mounted(ਖਾਲੀ) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        ਪੂਰਨ r;

        /* A quick check of procfs is properly mounted */

        r = ਰਸਤਾ_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ਜੇ (r == -ENOENT) /* not mounted at all */
                ਵਾਪਸ false;

        ਵਾਪਸ r;
}

bool stat_inode_same(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ਵਾਪਸ stat_is_ਸੈੱਟ(a) && stat_is_ਸੈੱਟ(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ਅਚਲ ਬਣਤਰ statx *a, ਅਚਲ ਬਣਤਰ statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ਜੇ (!statx_is_ਸੈੱਟ(a) || !statx_is_ਸੈੱਟ(b))
                ਵਾਪਸ false;

        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(a->stx_ਨਕਾਬ, STATX_TYPE|STATX_INO));
        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(b->stx_ਨਕਾਬ, STATX_TYPE|STATX_INO));

        ਵਾਪਸ
                ((a->stx_ਬਹੁਲਕ ^ b->stx_ਬਹੁਲਕ) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

ਪੂਰਨ statx_mount_same(ਅਚਲ ਬਣਤਰ statx *a, ਅਚਲ ਬਣਤਰ statx *b) {
        ਜੇ (!statx_is_ਸੈੱਟ(a) || !statx_is_ਸੈੱਟ(b))
                ਵਾਪਸ false;

        ਜੇ ((FLAGS_ਸੈੱਟ(a->stx_ਨਕਾਬ, STATX_MNT_ID) && FLAGS_ਸੈੱਟ(b->stx_ਨਕਾਬ, STATX_MNT_ID)) ||
            (FLAGS_ਸੈੱਟ(a->stx_ਨਕਾਬ, STATX_MNT_ID_UNIQUE) && FLAGS_ਸੈੱਟ(b->stx_ਨਕਾਬ, STATX_MNT_ID_UNIQUE)))
                ਵਾਪਸ a->stx_mnt_id == b->stx_mnt_id;

        ਵਾਪਸ -ENODATA;
}

usec_t statx_timestamp_ਲੱਦੋ(ਅਚਲ ਬਣਤਰ statx_timestamp *ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);
        ਵਾਪਸ timespec_ਲੱਦੋ(&(ਅਚਲ ਬਣਤਰ timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_ਲੱਦੋ_nsec(ਅਚਲ ਬਣਤਰ statx_timestamp *ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);
        ਵਾਪਸ timespec_ਲੱਦੋ_nsec(&(ਅਚਲ ਬਣਤਰ timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

ਖਾਲੀ inode_hash_func(ਅਚਲ ਬਣਤਰ stat *q, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

ਪੂਰਨ inode_compare_func(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = CMP(a->st_dev, b->st_dev);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        r = CMP(a->st_ino, b->st_ino);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਵਾਪਸ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(inode_hash_ops, ਬਣਤਰ stat, inode_hash_func, inode_compare_func, ਮੁਕਤ);

ਖਾਲੀ inode_unmodified_hash_func(ਅਚਲ ਬਣਤਰ stat *q, ਬਣਤਰ siphash *state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ਜੇ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ਨਹੀਂਤਾਂ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ਜੇ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ਨਹੀਂਤਾਂ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

ਪੂਰਨ inode_unmodified_compare_func(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = inode_compare_func(a, b);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਜੇ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ਜੇ (r != 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ਜੇ (r != 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(inode_unmodified_hash_ops, ਬਣਤਰ stat, inode_unmodified_hash_func, inode_unmodified_compare_func, ਮੁਕਤ);

ਅਚਲ ਅੱਖਰ* inode_type_ਮੰਜ਼ਿਲ_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        ਚੋਣ (m & S_IFMT) {
        ਹਾਲਤ S_IFREG:
                ਵਾਪਸ "reg";
        ਹਾਲਤ S_IFDIR:
                ਵਾਪਸ "dir";
        ਹਾਲਤ S_IFLNK:
                ਵਾਪਸ "lnk";
        ਹਾਲਤ S_IFCHR:
                ਵਾਪਸ "chr";
        ਹਾਲਤ S_IFBLK:
                ਵਾਪਸ "blk";
        ਹਾਲਤ S_IFIFO:
                ਵਾਪਸ "fifo";
        ਹਾਲਤ S_IFSOCK:
                ਵਾਪਸ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ਵਾਪਸ ਨਿਰਾ;
}

mode_t inode_type_from_string(ਅਚਲ ਅੱਖਰ *s) {
        ਜੇ (!s)
                ਵਾਪਸ ਬਹੁਲਕ_INVALID;

        ਜੇ (streq(s, "reg"))
                ਵਾਪਸ S_IFREG;
        ਜੇ (streq(s, "dir"))
                ਵਾਪਸ S_IFDIR;
        ਜੇ (streq(s, "lnk"))
                ਵਾਪਸ S_IFLNK;
        ਜੇ (streq(s, "chr"))
                ਵਾਪਸ S_IFCHR;
        ਜੇ (streq(s, "blk"))
                ਵਾਪਸ S_IFBLK;
        ਜੇ (streq(s, "fifo"))
                ਵਾਪਸ S_IFIFO;
        ਜੇ (streq(s, "sock"))
                ਵਾਪਸ S_IFSOCK;

        ਵਾਪਸ ਬਹੁਲਕ_INVALID;
}

ਪੂਰਨ vfs_free_ਬਾਈਟਾਂ(ਪੂਰਨ fd, uint64_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        ਬਣਤਰ statvfs sv;
        ਜੇ (fstatvfs(fd, &sv) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (!MUL_ਸੁਰੱਖਿਅਤ(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ 0;
}
