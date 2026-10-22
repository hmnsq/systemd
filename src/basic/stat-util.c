/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <linux/magic.h>
#অন্তর্ভাব <sys/statvfs.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "bitfield.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "filesystems.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "hash-funcs.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mountpoint-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "time-util.h"

স্থির পূর্ণাঙ্ক verify_stat_at(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                bool follow,
                পূর্ণাঙ্ক (*verify_func)(ধ্রুব সংরচনা stat *st),
                bool verify) {

        সংরচনা stat st;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(fd));
        প্রতিজ্ঞা(!isempty(path) || !follow);
        প্রতিজ্ঞা(verify_func);

        _cleanup_free_ বর্ণ *p = রিক্ত;
        r = resolve_xat_fdroot(&fd, &path, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                প্রত্যাগম -errno;

        r = verify_func(&st);
        প্রত্যাগম verify ? r : r >= 0;
}

স্থির পূর্ণাঙ্ক mode_verify_regular(mode_t mode) {
        যদি (S_ISDIR(mode))
                প্রত্যাগম -EISDIR;

        যদি (S_ISLNK(mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISREG(mode))
                প্রত্যাগম -EBADFD;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক stat_verify_regular(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        প্রত্যাগম mode_verify_regular(st->st_mode);
}

পূর্ণাঙ্ক statx_verify_regular(ধ্রুব সংরচনা statx *stx) {
        প্রতিজ্ঞা(stx);

        যদি (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                প্রত্যাগম -ENODATA;

        প্রত্যাগম mode_verify_regular(stx->stx_mode);
}

পূর্ণাঙ্ক verify_regular_at(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, bool follow) {
        প্রত্যাগম verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

পূর্ণাঙ্ক fd_verify_regular(পূর্ণাঙ্ক fd) {
        যদি (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                প্রত্যাগম -EISDIR;

        প্রত্যাগম verify_regular_at(fd, /* path= */ রিক্ত, /* follow= */ false);
}

স্থির পূর্ণাঙ্ক mode_verify_directory(mode_t mode) {
        যদি (S_ISLNK(mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISDIR(mode))
                প্রত্যাগম -ENOTDIR;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক stat_verify_directory(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        প্রত্যাগম mode_verify_directory(st->st_mode);
}

পূর্ণাঙ্ক statx_verify_directory(ধ্রুব সংরচনা statx *stx) {
        প্রতিজ্ঞা(stx);

        যদি (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                প্রত্যাগম -ENODATA;

        প্রত্যাগম mode_verify_directory(stx->stx_mode);
}

পূর্ণাঙ্ক fd_verify_directory(পূর্ণাঙ্ক fd) {
        যদি (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                প্রত্যাগম 0;

        প্রত্যাগম verify_stat_at(fd, রিক্ত, false, stat_verify_directory, true);
}

পূর্ণাঙ্ক is_dir_at(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, bool follow) {
        প্রত্যাগম verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

পূর্ণাঙ্ক is_dir(ধ্রুব বর্ণ *path, bool follow) {
        প্রতিজ্ঞা(!isempty(path));
        প্রত্যাগম is_dir_at(AT_FDCWD, path, follow);
}

পূর্ণাঙ্ক stat_verify_symlink(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        যদি (S_ISDIR(st->st_mode))
                প্রত্যাগম -EISDIR;

        যদি (!S_ISLNK(st->st_mode))
                প্রত্যাগম -ENOLINK;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fd_verify_symlink(পূর্ণাঙ্ক fd) {
        যদি (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                প্রত্যাগম -EISDIR;

        প্রত্যাগম verify_stat_at(fd, /* path= */ রিক্ত, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

পূর্ণাঙ্ক is_symlink(ধ্রুব বর্ণ *path) {
        প্রতিজ্ঞা(!isempty(path));
        প্রত্যাগম verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

স্থির পূর্ণাঙ্ক mode_verify_socket(mode_t mode) {
        যদি (S_ISDIR(mode))
                প্রত্যাগম -EISDIR;

        যদি (S_ISLNK(mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISSOCK(mode))
                প্রত্যাগম -ENOTSOCK;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক stat_verify_socket(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        প্রত্যাগম mode_verify_socket(st->st_mode);
}

পূর্ণাঙ্ক statx_verify_socket(ধ্রুব সংরচনা statx *stx) {
        প্রতিজ্ঞা(stx);

        প্রত্যাগম mode_verify_socket(stx->stx_mode);
}

পূর্ণাঙ্ক fd_verify_socket(পূর্ণাঙ্ক fd) {
        যদি (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                প্রত্যাগম -EISDIR;

        প্রত্যাগম verify_stat_at(fd, /* path= */ রিক্ত, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

পূর্ণাঙ্ক is_socket(ধ্রুব বর্ণ *path) {
        প্রতিজ্ঞা(!isempty(path));
        প্রত্যাগম verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

পূর্ণাঙ্ক stat_verify_linked(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        যদি (st->st_nlink <= 0)
                প্রত্যাগম -EIDRM; /* recognizable error. */

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fd_verify_linked(পূর্ণাঙ্ক fd) {

        যদি (fd == XAT_FDROOT)
                প্রত্যাগম 0;

        প্রত্যাগম verify_stat_at(fd, রিক্ত, false, stat_verify_linked, true);
}

পূর্ণাঙ্ক stat_verify_block(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        যদি (S_ISDIR(st->st_mode))
                প্রত্যাগম -EISDIR;

        যদি (S_ISLNK(st->st_mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISBLK(st->st_mode))
                প্রত্যাগম -ENOTBLK;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fd_verify_block(পূর্ণাঙ্ক fd) {
        যদি (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                প্রত্যাগম -EISDIR;

        প্রত্যাগম verify_stat_at(fd, /* path= */ রিক্ত, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

পূর্ণাঙ্ক stat_verify_char(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        যদি (S_ISDIR(st->st_mode))
                প্রত্যাগম -EISDIR;

        যদি (S_ISLNK(st->st_mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISCHR(st->st_mode))
                প্রত্যাগম -EBADFD;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক stat_verify_device_গ্রন্থি(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        যদি (S_ISDIR(st->st_mode))
                প্রত্যাগম -EISDIR;

        যদি (S_ISLNK(st->st_mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                প্রত্যাগম -ENOTTY;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক is_device_গ্রন্থি(ধ্রুব বর্ণ *path) {
        প্রতিজ্ঞা(!isempty(path));
        প্রত্যাগম verify_stat_at(AT_FDCWD, path, false, stat_verify_device_গ্রন্থি, false);
}

পূর্ণাঙ্ক stat_verify_regular_or_block(ধ্রুব সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        যদি (S_ISDIR(st->st_mode))
                প্রত্যাগম -EISDIR;

        যদি (S_ISLNK(st->st_mode))
                প্রত্যাগম -ELOOP;

        যদি (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                প্রত্যাগম -EBADFD;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fd_verify_regular_or_block(পূর্ণাঙ্ক fd) {
        যদি (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                প্রত্যাগম -EISDIR;

        প্রত্যাগম verify_stat_at(fd, /* path= */ রিক্ত, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

পূর্ণাঙ্ক dir_is_empty_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        সংরচনা dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        যদি (fd < 0)
                প্রত্যাগম fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        প্রতি (;;) {
                সংরচনা dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                যদি (n < 0)
                        প্রত্যাগম -errno;
                যদি (n == 0)
                        ভঙ্গ;

                প্রতিজ্ঞা((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        যদি (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                প্রত্যাগম 0;
        }

        প্রত্যাগম 1;
}

bool stat_may_be_dev_null(সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        প্রত্যাগম S_ISCHR(st->st_mode);
}

bool stat_is_empty(সংরচনা stat *st) {
        প্রতিজ্ঞা(st);

        প্রত্যাগম S_ISREG(st->st_mode) && st->st_size <= 0;
}

পূর্ণাঙ্ক null_or_empty_path_with_মূল(ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *মূল) {
        _cleanup_free_ বর্ণ *resolved = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        সংরচনা stat st;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fn);

        /* fn includes the root prefix. */

        যদি (path_equal(path_startswith(fn, মূল ?: "/"), "dev/null"))
                প্রত্যাগম true;

        যদি (empty_or_মূল(মূল)) {
                যদি (stat(fn, &st) < 0)
                        প্রত্যাগম -errno;

                প্রত্যাগম null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, মূল, CHASE_NONEXISTENT, &resolved, &fd);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (path_equal(path_startswith(resolved, মূল), "dev/null"))
                প্রত্যাগম true;

        যদি (r == 0)
                প্রত্যাগম -ENOENT;

        যদি (fstat(fd, &st) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম null_or_empty(&st);
}

স্থির ধ্রুব বর্ণ* statx_mask_one_to_name(unsigned mask);
স্থির ধ্রুব বর্ণ* statx_attribute_to_name(uint64_t attr);

#অন্তর্ভাব "statx-attribute-to-name.inc"
#অন্তর্ভাব "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        স্থির বর্ণ* prefix##_to_string(type v) {                               \
                যদি (v == 0)                                                     \
                        প্রত্যাগম strdup("");                                      \
                                                                                \
                _cleanup_free_ বর্ণ *s = রিক্ত;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ধ্রুব বর্ণ *n = func(f);                                \
                        যদি (!n)                                                 \
                                অনুবর্ত;                                       \
                                                                                \
                        যদি (!strextend_with_separator(&s, "|", n))              \
                                প্রত্যাগম রিক্ত;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                যদি (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        প্রত্যাগম রিক্ত;                                            \
                                                                                \
                প্রত্যাগম TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

পূর্ণাঙ্ক xstatx_full(পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                সংরচনা statx *ret) {

        সংরচনা statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        পূর্ণাঙ্ক r;

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

        প্রতিজ্ঞা(wildcard_fd_is_valid(fd));
        প্রতিজ্ঞা((mandatory_mask & optional_mask) == 0);
        প্রতিজ্ঞা(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        প্রতিজ্ঞা(ret);

        _cleanup_free_ বর্ণ *p = রিক্ত;
        r = resolve_xat_fdroot(&fd, &path, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        unsigned request_mask = mandatory_mask|optional_mask;
        যদি (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        যদি (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                প্রত্যাগম negative_errno();

        যদি (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        যদি (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                যদি (DEBUG_LOGGING) {
                        _cleanup_free_ বর্ণ *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                প্রত্যাগম -EUNATCH;
        }

        যদি (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                যদি (DEBUG_LOGGING) {
                        _cleanup_free_ বর্ণ *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                প্রত্যাগম -EUNATCH;
        }

        *ret = sx;
        প্রত্যাগম FLAGS_SET(sx.stx_mask, optional_mask);
}

স্থির পূর্ণাঙ্ক xfstatfs(পূর্ণাঙ্ক fd, সংরচনা statfs *ret) {
        প্রতিজ্ঞা(ret);

        যদি (fd == AT_FDCWD)
                প্রত্যাগম RET_NERRNO(statfs(".", ret));
        যদি (fd == XAT_FDROOT)
                প্রত্যাগম RET_NERRNO(statfs("/", ret));

        প্রতিজ্ঞা(fd >= 0);
        প্রত্যাগম RET_NERRNO(fstatfs(fd, ret));
}

পূর্ণাঙ্ক xstatfsat(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, সংরচনা statfs *ret) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(ret);

        যদি (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                যদি (fd < 0)
                        প্রত্যাগম fd;
                dir_fd = fd;
        }

        প্রত্যাগম xfstatfs(dir_fd, ret);
}

পূর্ণাঙ্ক fd_is_read_only_fs(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r;

        সংরচনা statfs st;
        r = xfstatfs(fd, &st);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (st.f_flags & ST_RDONLY)
                প্রত্যাগম true;

        যদি (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                প্রত্যাগম access_fd(fd, W_OK) == -EROFS;

        প্রত্যাগম false;
}

পূর্ণাঙ্ক path_is_read_only_fs(ধ্রুব বর্ণ *path) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম fd_is_read_only_fs(fd);
}

পূর্ণাঙ্ক inode_same_at(পূর্ণাঙ্ক fda, ধ্রুব বর্ণ *filea, পূর্ণাঙ্ক fdb, ধ্রুব বর্ণ *fileb, পূর্ণাঙ্ক flags) {
        সংরচনা stat sta, stb;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fda >= 0 || fda == AT_FDCWD);
        প্রতিজ্ঞা(fdb >= 0 || fdb == AT_FDCWD);
        প্রতিজ্ঞা((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        যদি ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                প্রত্যাগম -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        যদি (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                প্রত্যাগম true;

        _cleanup_close_ পূর্ণাঙ্ক pin_a = -EBADF, pin_b = -EBADF;
        যদি (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                যদি (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        যদি (pin_a < 0)
                                প্রত্যাগম -errno;

                        fda = pin_a;
                        filea = রিক্ত;
                        flags |= AT_EMPTY_PATH;
                }

                যদি (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        যদি (pin_b < 0)
                                প্রত্যাগম -errno;

                        fdb = pin_b;
                        fileb = রিক্ত;
                        flags |= AT_EMPTY_PATH;
                }

                পূর্ণাঙ্ক ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ সংরচনা file_handle *ha = রিক্ত, *hb = রিক্ত;
                uint64_t mntida, mntidb;
                পূর্ণাঙ্ক _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                যদি (r < 0) {
                        যদি (is_name_to_handle_at_fatal_error(r))
                                প্রত্যাগম r;

                        গচ্ছ fallback;
                }
                bool have_unique_mntid = r > 0;

                যদি (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? রিক্ত : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : রিক্ত,
                                ntha_flags);
                যদি (r < 0) {
                        যদি (is_name_to_handle_at_fatal_error(r))
                                প্রত্যাগম r;

                        গচ্ছ fallback;
                }
                যদি (r == 0) {
                        প্রতিজ্ঞা(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                যদি (!file_handle_equal(ha, hb))
                        প্রত্যাগম false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                যদি (mntida == mntidb)
                        প্রত্যাগম true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        যদি (fstatat(fda, strempty(filea), &sta, flags) < 0)
                প্রত্যাগম log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        যদি (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                প্রত্যাগম log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        প্রত্যাগম stat_inode_same(&sta, &stb);
}

bool is_fs_type(ধ্রুব সংরচনা statfs *s, statfs_f_type_t magic_মান) {
        প্রতিজ্ঞা(s);
        assert_cc(পরিমাণ(statfs_f_type_t) >= পরিমাণ(s->f_type));

        প্রত্যাগম F_TYPE_EQUAL(s->f_type, magic_মান);
}

পূর্ণাঙ্ক is_fs_type_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, statfs_f_type_t magic_মান) {
        পূর্ণাঙ্ক r;

        সংরচনা statfs s;
        r = xstatfsat(dir_fd, path, &s);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম is_fs_type(&s, magic_মান);
}

bool is_temporary_fs(ধ্রুব সংরচনা statfs *s) {
        প্রত্যাগম fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(ধ্রুব সংরচনা statfs *s) {
        প্রত্যাগম fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

পূর্ণাঙ্ক fd_is_temporary_fs(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r;

        সংরচনা statfs s;
        r = xfstatfs(fd, &s);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম is_temporary_fs(&s);
}

পূর্ণাঙ্ক fd_is_network_fs(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r;

        সংরচনা statfs s;
        r = xfstatfs(fd, &s);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম is_network_fs(&s);
}

পূর্ণাঙ্ক path_is_temporary_fs(ধ্রুব বর্ণ *path) {
        সংরচনা statfs s;

        যদি (statfs(path, &s) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম is_temporary_fs(&s);
}

পূর্ণাঙ্ক path_is_network_fs(ধ্রুব বর্ণ *path) {
        সংরচনা statfs s;

        যদি (statfs(path, &s) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম is_network_fs(&s);
}

পূর্ণাঙ্ক proc_mounted(শূন্য) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        পূর্ণাঙ্ক r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        যদি (r == -ENOENT) /* not mounted at all */
                প্রত্যাগম false;

        প্রত্যাগম r;
}

bool stat_inode_same(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        প্রত্যাগম stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        প্রত্যাগম stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ধ্রুব সংরচনা statx *a, ধ্রুব সংরচনা statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        যদি (!statx_is_set(a) || !statx_is_set(b))
                প্রত্যাগম false;

        প্রতিজ্ঞা(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        প্রতিজ্ঞা(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        প্রত্যাগম
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

পূর্ণাঙ্ক statx_mount_same(ধ্রুব সংরচনা statx *a, ধ্রুব সংরচনা statx *b) {
        যদি (!statx_is_set(a) || !statx_is_set(b))
                প্রত্যাগম false;

        যদি ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                প্রত্যাগম a->stx_mnt_id == b->stx_mnt_id;

        প্রত্যাগম -ENODATA;
}

usec_t statx_timestamp_load(ধ্রুব সংরচনা statx_timestamp *ts) {
        প্রতিজ্ঞা(ts);
        প্রত্যাগম timespec_load(&(ধ্রুব সংরচনা timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(ধ্রুব সংরচনা statx_timestamp *ts) {
        প্রতিজ্ঞা(ts);
        প্রত্যাগম timespec_load_nsec(&(ধ্রুব সংরচনা timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

শূন্য inode_hash_func(ধ্রুব সংরচনা stat *q, সংরচনা siphash *state) {
        প্রতিজ্ঞা(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

পূর্ণাঙ্ক inode_compare_func(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(a->st_dev, b->st_dev);
        যদি (r != 0)
                প্রত্যাগম r;

        r = CMP(a->st_ino, b->st_ino);
        যদি (r != 0)
                প্রত্যাগম r;

        প্রত্যাগম CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(inode_hash_ops, সংরচনা stat, inode_hash_func, inode_compare_func, মুক্ত);

শূন্য inode_unmodified_hash_func(ধ্রুব সংরচনা stat *q, সংরচনা siphash *state) {
        প্রতিজ্ঞা(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        যদি (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        অন্যথা {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        যদি (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        অন্যথা {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

পূর্ণাঙ্ক inode_unmodified_compare_func(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = inode_compare_func(a, b);
        যদি (r != 0)
                প্রত্যাগম r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        যদি (r != 0)
                প্রত্যাগম r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        যদি (r != 0)
                প্রত্যাগম r;

        যদি (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                যদি (r != 0)
                        প্রত্যাগম r;
        }

        যদি (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                যদি (r != 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(inode_unmodified_hash_ops, সংরচনা stat, inode_unmodified_hash_func, inode_unmodified_compare_func, মুক্ত);

ধ্রুব বর্ণ* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        বিকল্প (m & S_IFMT) {
        পক্ষ S_IFREG:
                প্রত্যাগম "reg";
        পক্ষ S_IFDIR:
                প্রত্যাগম "dir";
        পক্ষ S_IFLNK:
                প্রত্যাগম "lnk";
        পক্ষ S_IFCHR:
                প্রত্যাগম "chr";
        পক্ষ S_IFBLK:
                প্রত্যাগম "blk";
        পক্ষ S_IFIFO:
                প্রত্যাগম "fifo";
        পক্ষ S_IFSOCK:
                প্রত্যাগম "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        প্রত্যাগম রিক্ত;
}

mode_t inode_type_from_string(ধ্রুব বর্ণ *s) {
        যদি (!s)
                প্রত্যাগম MODE_INVALID;

        যদি (streq(s, "reg"))
                প্রত্যাগম S_IFREG;
        যদি (streq(s, "dir"))
                প্রত্যাগম S_IFDIR;
        যদি (streq(s, "lnk"))
                প্রত্যাগম S_IFLNK;
        যদি (streq(s, "chr"))
                প্রত্যাগম S_IFCHR;
        যদি (streq(s, "blk"))
                প্রত্যাগম S_IFBLK;
        যদি (streq(s, "fifo"))
                প্রত্যাগম S_IFIFO;
        যদি (streq(s, "sock"))
                প্রত্যাগম S_IFSOCK;

        প্রত্যাগম MODE_INVALID;
}

পূর্ণাঙ্ক vfs_free_bytes(পূর্ণাঙ্ক fd, uint64_t *ret) {
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        সংরচনা statvfs sv;
        যদি (fstatvfs(fd, &sv) < 0)
                প্রত্যাগম -errno;

        যদি (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                প্রত্যাগম -ERANGE;

        প্রত্যাগম 0;
}
