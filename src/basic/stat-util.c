/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <linux/magic.h>
#அந்தர்பாவ <sys/statvfs.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "bitfield.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "filesystems.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "hash-funcs.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mountpoint-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "siphash24.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "time-util.h"

நிலையான முழு verify_stat_at(
                முழு fd,
                மாறிலி எழுத்து *path,
                bool follow,
                முழு (*verify_func)(மாறிலி கட்டமைப்பு stat *st),
                bool verify) {

        கட்டமைப்பு stat st;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(fd));
        ப்ரதிஜ்ஞா(!isempty(path) || !follow);
        ப்ரதிஜ்ஞா(verify_func);

        _cleanup_free_ எழுத்து *p = வெற்று;
        r = resolve_xat_fdroot(&fd, &path, &p);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                திரும்பு -errno;

        r = verify_func(&st);
        திரும்பு verify ? r : r >= 0;
}

நிலையான முழு mode_verify_regular(mode_t mode) {
        எனில் (S_ISDIR(mode))
                திரும்பு -EISDIR;

        எனில் (S_ISLNK(mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISREG(mode))
                திரும்பு -EBADFD;

        திரும்பு 0;
}

முழு stat_verify_regular(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        திரும்பு mode_verify_regular(st->st_mode);
}

முழு statx_verify_regular(மாறிலி கட்டமைப்பு statx *stx) {
        ப்ரதிஜ்ஞா(stx);

        எனில் (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                திரும்பு -ENODATA;

        திரும்பு mode_verify_regular(stx->stx_mode);
}

முழு verify_regular_at(முழு fd, மாறிலி எழுத்து *path, bool follow) {
        திரும்பு verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

முழு fd_verify_regular(முழு fd) {
        எனில் (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                திரும்பு -EISDIR;

        திரும்பு verify_regular_at(fd, /* path= */ வெற்று, /* follow= */ false);
}

நிலையான முழு mode_verify_directory(mode_t mode) {
        எனில் (S_ISLNK(mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISDIR(mode))
                திரும்பு -ENOTDIR;

        திரும்பு 0;
}

முழு stat_verify_directory(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        திரும்பு mode_verify_directory(st->st_mode);
}

முழு statx_verify_directory(மாறிலி கட்டமைப்பு statx *stx) {
        ப்ரதிஜ்ஞா(stx);

        எனில் (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                திரும்பு -ENODATA;

        திரும்பு mode_verify_directory(stx->stx_mode);
}

முழு fd_verify_directory(முழு fd) {
        எனில் (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                திரும்பு 0;

        திரும்பு verify_stat_at(fd, வெற்று, false, stat_verify_directory, true);
}

முழு is_dir_at(முழு fd, மாறிலி எழுத்து *path, bool follow) {
        திரும்பு verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

முழு is_dir(மாறிலி எழுத்து *path, bool follow) {
        ப்ரதிஜ்ஞா(!isempty(path));
        திரும்பு is_dir_at(AT_FDCWD, path, follow);
}

முழு stat_verify_symlink(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        எனில் (S_ISDIR(st->st_mode))
                திரும்பு -EISDIR;

        எனில் (!S_ISLNK(st->st_mode))
                திரும்பு -ENOLINK;

        திரும்பு 0;
}

முழு fd_verify_symlink(முழு fd) {
        எனில் (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                திரும்பு -EISDIR;

        திரும்பு verify_stat_at(fd, /* path= */ வெற்று, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

முழு is_symlink(மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(!isempty(path));
        திரும்பு verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

நிலையான முழு mode_verify_socket(mode_t mode) {
        எனில் (S_ISDIR(mode))
                திரும்பு -EISDIR;

        எனில் (S_ISLNK(mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISSOCK(mode))
                திரும்பு -ENOTSOCK;

        திரும்பு 0;
}

முழு stat_verify_socket(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        திரும்பு mode_verify_socket(st->st_mode);
}

முழு statx_verify_socket(மாறிலி கட்டமைப்பு statx *stx) {
        ப்ரதிஜ்ஞா(stx);

        திரும்பு mode_verify_socket(stx->stx_mode);
}

முழு fd_verify_socket(முழு fd) {
        எனில் (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                திரும்பு -EISDIR;

        திரும்பு verify_stat_at(fd, /* path= */ வெற்று, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

முழு is_socket(மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(!isempty(path));
        திரும்பு verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

முழு stat_verify_linked(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        எனில் (st->st_nlink <= 0)
                திரும்பு -EIDRM; /* recognizable error. */

        திரும்பு 0;
}

முழு fd_verify_linked(முழு fd) {

        எனில் (fd == XAT_FDROOT)
                திரும்பு 0;

        திரும்பு verify_stat_at(fd, வெற்று, false, stat_verify_linked, true);
}

முழு stat_verify_block(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        எனில் (S_ISDIR(st->st_mode))
                திரும்பு -EISDIR;

        எனில் (S_ISLNK(st->st_mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISBLK(st->st_mode))
                திரும்பு -ENOTBLK;

        திரும்பு 0;
}

முழு fd_verify_block(முழு fd) {
        எனில் (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                திரும்பு -EISDIR;

        திரும்பு verify_stat_at(fd, /* path= */ வெற்று, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

முழு stat_verify_char(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        எனில் (S_ISDIR(st->st_mode))
                திரும்பு -EISDIR;

        எனில் (S_ISLNK(st->st_mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISCHR(st->st_mode))
                திரும்பு -EBADFD;

        திரும்பு 0;
}

முழு stat_verify_device_முனை(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        எனில் (S_ISDIR(st->st_mode))
                திரும்பு -EISDIR;

        எனில் (S_ISLNK(st->st_mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                திரும்பு -ENOTTY;

        திரும்பு 0;
}

முழு is_device_முனை(மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(!isempty(path));
        திரும்பு verify_stat_at(AT_FDCWD, path, false, stat_verify_device_முனை, false);
}

முழு stat_verify_regular_or_block(மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        எனில் (S_ISDIR(st->st_mode))
                திரும்பு -EISDIR;

        எனில் (S_ISLNK(st->st_mode))
                திரும்பு -ELOOP;

        எனில் (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                திரும்பு -EBADFD;

        திரும்பு 0;
}

முழு fd_verify_regular_or_block(முழு fd) {
        எனில் (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                திரும்பு -EISDIR;

        திரும்பு verify_stat_at(fd, /* path= */ வெற்று, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

முழு dir_is_empty_at(முழு dir_fd, மாறிலி எழுத்து *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ முழு fd = -EBADF;
        கட்டமைப்பு dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        க்கு (;;) {
                கட்டமைப்பு dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                எனில் (n < 0)
                        திரும்பு -errno;
                எனில் (n == 0)
                        நிறுத்து;

                ப்ரதிஜ்ஞா((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        எனில் (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                திரும்பு 0;
        }

        திரும்பு 1;
}

bool stat_may_be_dev_null(கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        திரும்பு S_ISCHR(st->st_mode);
}

bool stat_is_empty(கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(st);

        திரும்பு S_ISREG(st->st_mode) && st->st_size <= 0;
}

முழு null_or_empty_path_with_வேர்(மாறிலி எழுத்து *fn, மாறிலி எழுத்து *வேர்) {
        _cleanup_free_ எழுத்து *resolved = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        கட்டமைப்பு stat st;
        முழு r;

        ப்ரதிஜ்ஞா(fn);

        /* fn includes the root prefix. */

        எனில் (path_equal(path_startswith(fn, வேர் ?: "/"), "dev/null"))
                திரும்பு true;

        எனில் (empty_or_வேர்(வேர்)) {
                எனில் (stat(fn, &st) < 0)
                        திரும்பு -errno;

                திரும்பு null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, வேர், CHASE_NONEXISTENT, &resolved, &fd);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (path_equal(path_startswith(resolved, வேர்), "dev/null"))
                திரும்பு true;

        எனில் (r == 0)
                திரும்பு -ENOENT;

        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        திரும்பு null_or_empty(&st);
}

நிலையான மாறிலி எழுத்து* statx_mask_one_to_name(unsigned mask);
நிலையான மாறிலி எழுத்து* statx_attribute_to_name(uint64_t attr);

#அந்தர்பாவ "statx-attribute-to-name.inc"
#அந்தர்பாவ "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        நிலையான எழுத்து* prefix##_to_string(type v) {                               \
                எனில் (v == 0)                                                     \
                        திரும்பு strdup("");                                      \
                                                                                \
                _cleanup_free_ எழுத்து *s = வெற்று;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        மாறிலி எழுத்து *n = func(f);                                \
                        எனில் (!n)                                                 \
                                தொடர்;                                       \
                                                                                \
                        எனில் (!strextend_with_separator(&s, "|", n))              \
                                திரும்பு வெற்று;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                எனில் (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        திரும்பு வெற்று;                                            \
                                                                                \
                திரும்பு TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

முழு xstatx_full(முழு fd,
                மாறிலி எழுத்து *path,
                முழு statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                கட்டமைப்பு statx *ret) {

        கட்டமைப்பு statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        முழு r;

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

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(fd));
        ப்ரதிஜ்ஞா((mandatory_mask & optional_mask) == 0);
        ப்ரதிஜ்ஞா(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        ப்ரதிஜ்ஞா(ret);

        _cleanup_free_ எழுத்து *p = வெற்று;
        r = resolve_xat_fdroot(&fd, &path, &p);
        எனில் (r < 0)
                திரும்பு r;

        unsigned request_mask = mandatory_mask|optional_mask;
        எனில் (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        எனில் (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                திரும்பு negative_errno();

        எனில் (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        எனில் (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                எனில் (DEBUG_LOGGING) {
                        _cleanup_free_ எழுத்து *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                திரும்பு -EUNATCH;
        }

        எனில் (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                எனில் (DEBUG_LOGGING) {
                        _cleanup_free_ எழுத்து *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                திரும்பு -EUNATCH;
        }

        *ret = sx;
        திரும்பு FLAGS_SET(sx.stx_mask, optional_mask);
}

நிலையான முழு xfstatfs(முழு fd, கட்டமைப்பு statfs *ret) {
        ப்ரதிஜ்ஞா(ret);

        எனில் (fd == AT_FDCWD)
                திரும்பு RET_NERRNO(statfs(".", ret));
        எனில் (fd == XAT_FDROOT)
                திரும்பு RET_NERRNO(statfs("/", ret));

        ப்ரதிஜ்ஞா(fd >= 0);
        திரும்பு RET_NERRNO(fstatfs(fd, ret));
}

முழு xstatfsat(முழு dir_fd, மாறிலி எழுத்து *path, கட்டமைப்பு statfs *ret) {
        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(ret);

        எனில் (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                எனில் (fd < 0)
                        திரும்பு fd;
                dir_fd = fd;
        }

        திரும்பு xfstatfs(dir_fd, ret);
}

முழு fd_is_read_only_fs(முழு fd) {
        முழு r;

        கட்டமைப்பு statfs st;
        r = xfstatfs(fd, &st);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (st.f_flags & ST_RDONLY)
                திரும்பு true;

        எனில் (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                திரும்பு access_fd(fd, W_OK) == -EROFS;

        திரும்பு false;
}

முழு path_is_read_only_fs(மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        எனில் (fd < 0)
                திரும்பு -errno;

        திரும்பு fd_is_read_only_fs(fd);
}

முழு inode_same_at(முழு fda, மாறிலி எழுத்து *filea, முழு fdb, மாறிலி எழுத்து *fileb, முழு flags) {
        கட்டமைப்பு stat sta, stb;
        முழு r;

        ப்ரதிஜ்ஞா(fda >= 0 || fda == AT_FDCWD);
        ப்ரதிஜ்ஞா(fdb >= 0 || fdb == AT_FDCWD);
        ப்ரதிஜ்ஞா((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        எனில் ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                திரும்பு -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        எனில் (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                திரும்பு true;

        _cleanup_close_ முழு pin_a = -EBADF, pin_b = -EBADF;
        எனில் (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                எனில் (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        எனில் (pin_a < 0)
                                திரும்பு -errno;

                        fda = pin_a;
                        filea = வெற்று;
                        flags |= AT_EMPTY_PATH;
                }

                எனில் (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        எனில் (pin_b < 0)
                                திரும்பு -errno;

                        fdb = pin_b;
                        fileb = வெற்று;
                        flags |= AT_EMPTY_PATH;
                }

                முழு ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ கட்டமைப்பு file_handle *ha = வெற்று, *hb = வெற்று;
                uint64_t mntida, mntidb;
                முழு _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                எனில் (r < 0) {
                        எனில் (is_name_to_handle_at_fatal_error(r))
                                திரும்பு r;

                        செல் fallback;
                }
                bool have_unique_mntid = r > 0;

                எனில் (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? வெற்று : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : வெற்று,
                                ntha_flags);
                எனில் (r < 0) {
                        எனில் (is_name_to_handle_at_fatal_error(r))
                                திரும்பு r;

                        செல் fallback;
                }
                எனில் (r == 0) {
                        ப்ரதிஜ்ஞா(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                எனில் (!file_handle_equal(ha, hb))
                        திரும்பு false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                எனில் (mntida == mntidb)
                        திரும்பு true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        எனில் (fstatat(fda, strempty(filea), &sta, flags) < 0)
                திரும்பு log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        எனில் (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                திரும்பு log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        திரும்பு stat_inode_same(&sta, &stb);
}

bool is_fs_type(மாறிலி கட்டமைப்பு statfs *s, statfs_f_type_t magic_மதிப்பு) {
        ப்ரதிஜ்ஞா(s);
        assert_cc(அளவு(statfs_f_type_t) >= அளவு(s->f_type));

        திரும்பு F_TYPE_EQUAL(s->f_type, magic_மதிப்பு);
}

முழு is_fs_type_at(முழு dir_fd, மாறிலி எழுத்து *path, statfs_f_type_t magic_மதிப்பு) {
        முழு r;

        கட்டமைப்பு statfs s;
        r = xstatfsat(dir_fd, path, &s);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு is_fs_type(&s, magic_மதிப்பு);
}

bool is_temporary_fs(மாறிலி கட்டமைப்பு statfs *s) {
        திரும்பு fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(மாறிலி கட்டமைப்பு statfs *s) {
        திரும்பு fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

முழு fd_is_temporary_fs(முழு fd) {
        முழு r;

        கட்டமைப்பு statfs s;
        r = xfstatfs(fd, &s);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு is_temporary_fs(&s);
}

முழு fd_is_network_fs(முழு fd) {
        முழு r;

        கட்டமைப்பு statfs s;
        r = xfstatfs(fd, &s);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு is_network_fs(&s);
}

முழு path_is_temporary_fs(மாறிலி எழுத்து *path) {
        கட்டமைப்பு statfs s;

        எனில் (statfs(path, &s) < 0)
                திரும்பு -errno;

        திரும்பு is_temporary_fs(&s);
}

முழு path_is_network_fs(மாறிலி எழுத்து *path) {
        கட்டமைப்பு statfs s;

        எனில் (statfs(path, &s) < 0)
                திரும்பு -errno;

        திரும்பு is_network_fs(&s);
}

முழு proc_mounted(வெறுமை) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        முழு r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        எனில் (r == -ENOENT) /* not mounted at all */
                திரும்பு false;

        திரும்பு r;
}

bool stat_inode_same(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        திரும்பு stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(மாறிலி கட்டமைப்பு statx *a, மாறிலி கட்டமைப்பு statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        எனில் (!statx_is_set(a) || !statx_is_set(b))
                திரும்பு false;

        ப்ரதிஜ்ஞா(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        ப்ரதிஜ்ஞா(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        திரும்பு
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

முழு statx_mount_same(மாறிலி கட்டமைப்பு statx *a, மாறிலி கட்டமைப்பு statx *b) {
        எனில் (!statx_is_set(a) || !statx_is_set(b))
                திரும்பு false;

        எனில் ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                திரும்பு a->stx_mnt_id == b->stx_mnt_id;

        திரும்பு -ENODATA;
}

usec_t statx_timestamp_load(மாறிலி கட்டமைப்பு statx_timestamp *ts) {
        ப்ரதிஜ்ஞா(ts);
        திரும்பு timespec_load(&(மாறிலி கட்டமைப்பு timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(மாறிலி கட்டமைப்பு statx_timestamp *ts) {
        ப்ரதிஜ்ஞா(ts);
        திரும்பு timespec_load_nsec(&(மாறிலி கட்டமைப்பு timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

வெறுமை inode_hash_func(மாறிலி கட்டமைப்பு stat *q, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

முழு inode_compare_func(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = CMP(a->st_dev, b->st_dev);
        எனில் (r != 0)
                திரும்பு r;

        r = CMP(a->st_ino, b->st_ino);
        எனில் (r != 0)
                திரும்பு r;

        திரும்பு CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(inode_hash_ops, கட்டமைப்பு stat, inode_hash_func, inode_compare_func, விடுவி);

வெறுமை inode_unmodified_hash_func(மாறிலி கட்டமைப்பு stat *q, கட்டமைப்பு siphash *state) {
        ப்ரதிஜ்ஞா(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        எனில் (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        இல்லையெனில் {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        எனில் (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        இல்லையெனில் {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

முழு inode_unmodified_compare_func(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = inode_compare_func(a, b);
        எனில் (r != 0)
                திரும்பு r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        எனில் (r != 0)
                திரும்பு r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        எனில் (r != 0)
                திரும்பு r;

        எனில் (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                எனில் (r != 0)
                        திரும்பு r;
        }

        எனில் (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                எனில் (r != 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(inode_unmodified_hash_ops, கட்டமைப்பு stat, inode_unmodified_hash_func, inode_unmodified_compare_func, விடுவி);

மாறிலி எழுத்து* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        தேர்வு (m & S_IFMT) {
        நிலை S_IFREG:
                திரும்பு "reg";
        நிலை S_IFDIR:
                திரும்பு "dir";
        நிலை S_IFLNK:
                திரும்பு "lnk";
        நிலை S_IFCHR:
                திரும்பு "chr";
        நிலை S_IFBLK:
                திரும்பு "blk";
        நிலை S_IFIFO:
                திரும்பு "fifo";
        நிலை S_IFSOCK:
                திரும்பு "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        திரும்பு வெற்று;
}

mode_t inode_type_from_string(மாறிலி எழுத்து *s) {
        எனில் (!s)
                திரும்பு MODE_INVALID;

        எனில் (streq(s, "reg"))
                திரும்பு S_IFREG;
        எனில் (streq(s, "dir"))
                திரும்பு S_IFDIR;
        எனில் (streq(s, "lnk"))
                திரும்பு S_IFLNK;
        எனில் (streq(s, "chr"))
                திரும்பு S_IFCHR;
        எனில் (streq(s, "blk"))
                திரும்பு S_IFBLK;
        எனில் (streq(s, "fifo"))
                திரும்பு S_IFIFO;
        எனில் (streq(s, "sock"))
                திரும்பு S_IFSOCK;

        திரும்பு MODE_INVALID;
}

முழு vfs_free_bytes(முழு fd, uint64_t *ret) {
        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        கட்டமைப்பு statvfs sv;
        எனில் (fstatvfs(fd, &sv) < 0)
                திரும்பு -errno;

        எனில் (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                திரும்பு -ERANGE;

        திரும்பு 0;
}
