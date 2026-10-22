/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <linux/magic.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include "alloc-util.h"
#include "bitfield.h"
#include "chase.h"
#include "dirent-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "filesystems.h"
#include "fs-util.h"
#include "hash-funcs.h"
#include "log.h"
#include "mountpoint-util.h"
#include "path-util.h"
#include "siphash24.h"
#include "stat-util.h"
#include "string-util.h"
#include "time-util.h"

static ကိန်းပြည့် verify_stat_at(
                ကိန်းပြည့် fd,
                const var_na *path,
                bool follow,
                ကိန်းပြည့် (*verify_func)(const sa_nracanaa stat *st),
                bool verify) {

        sa_nracanaa stat st;
        ကိန်းပြည့် r;

        assert(wildcard_fd_is_valid(fd));
        assert(!isempty(path) || !follow);
        assert(verify_func);

        _cleanup_free_ var_na *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ပြန်ပေး -errno;

        r = verify_func(&st);
        ပြန်ပေး verify ? r : r >= 0;
}

static ကိန်းပြည့် mode_verify_regular(mode_t mode) {
        အကယ်၍ (S_ISDIR(mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (S_ISLNK(mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISREG(mode))
                ပြန်ပေး -EBADFD;

        ပြန်ပေး 0;
}

ကိန်းပြည့် stat_verify_regular(const sa_nracanaa stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ပြန်ပေး mode_verify_regular(st->st_mode);
}

ကိန်းပြည့် statx_verify_regular(const sa_nracanaa statx *stx) {
        assert(stx);

        အကယ်၍ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ပြန်ပေး -ENODATA;

        ပြန်ပေး mode_verify_regular(stx->stx_mode);
}

ကိန်းပြည့် verify_regular_at(ကိန်းပြည့် fd, const var_na *path, bool follow) {
        ပြန်ပေး verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

ကိန်းပြည့် fd_verify_regular(ကိန်းပြည့် fd) {
        အကယ်၍ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ပြန်ပေး -EISDIR;

        ပြန်ပေး verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static ကိန်းပြည့် mode_verify_directory(mode_t mode) {
        အကယ်၍ (S_ISLNK(mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISDIR(mode))
                ပြန်ပေး -ENOTDIR;

        ပြန်ပေး 0;
}

ကိန်းပြည့် stat_verify_directory(const sa_nracanaa stat *st) {
        assert(st);

        ပြန်ပေး mode_verify_directory(st->st_mode);
}

ကိန်းပြည့် statx_verify_directory(const sa_nracanaa statx *stx) {
        assert(stx);

        အကယ်၍ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ပြန်ပေး -ENODATA;

        ပြန်ပေး mode_verify_directory(stx->stx_mode);
}

ကိန်းပြည့် fd_verify_directory(ကိန်းပြည့် fd) {
        အကယ်၍ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ပြန်ပေး 0;

        ပြန်ပေး verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

ကိန်းပြည့် is_dir_at(ကိန်းပြည့် fd, const var_na *path, bool follow) {
        ပြန်ပေး verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

ကိန်းပြည့် is_dir(const var_na *path, bool follow) {
        assert(!isempty(path));
        ပြန်ပေး is_dir_at(AT_FDCWD, path, follow);
}

ကိန်းပြည့် stat_verify_symlink(const sa_nracanaa stat *st) {
        assert(st);

        အကယ်၍ (S_ISDIR(st->st_mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (!S_ISLNK(st->st_mode))
                ပြန်ပေး -ENOLINK;

        ပြန်ပေး 0;
}

ကိန်းပြည့် fd_verify_symlink(ကိန်းပြည့် fd) {
        အကယ်၍ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ပြန်ပေး -EISDIR;

        ပြန်ပေး verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

ကိန်းပြည့် is_symlink(const var_na *path) {
        assert(!isempty(path));
        ပြန်ပေး verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static ကိန်းပြည့် mode_verify_socket(mode_t mode) {
        အကယ်၍ (S_ISDIR(mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (S_ISLNK(mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISSOCK(mode))
                ပြန်ပေး -ENOTSOCK;

        ပြန်ပေး 0;
}

ကိန်းပြည့် stat_verify_socket(const sa_nracanaa stat *st) {
        assert(st);

        ပြန်ပေး mode_verify_socket(st->st_mode);
}

ကိန်းပြည့် statx_verify_socket(const sa_nracanaa statx *stx) {
        assert(stx);

        ပြန်ပေး mode_verify_socket(stx->stx_mode);
}

ကိန်းပြည့် fd_verify_socket(ကိန်းပြည့် fd) {
        အကယ်၍ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ပြန်ပေး -EISDIR;

        ပြန်ပေး verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

ကိန်းပြည့် is_socket(const var_na *path) {
        assert(!isempty(path));
        ပြန်ပေး verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

ကိန်းပြည့် stat_verify_linked(const sa_nracanaa stat *st) {
        assert(st);

        အကယ်၍ (st->st_nlink <= 0)
                ပြန်ပေး -EIDRM; /* recognizable error. */

        ပြန်ပေး 0;
}

ကိန်းပြည့် fd_verify_linked(ကိန်းပြည့် fd) {

        အကယ်၍ (fd == XAT_FDROOT)
                ပြန်ပေး 0;

        ပြန်ပေး verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

ကိန်းပြည့် stat_verify_block(const sa_nracanaa stat *st) {
        assert(st);

        အကယ်၍ (S_ISDIR(st->st_mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (S_ISLNK(st->st_mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISBLK(st->st_mode))
                ပြန်ပေး -ENOTBLK;

        ပြန်ပေး 0;
}

ကိန်းပြည့် fd_verify_block(ကိန်းပြည့် fd) {
        အကယ်၍ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ပြန်ပေး -EISDIR;

        ပြန်ပေး verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

ကိန်းပြည့် stat_verify_char(const sa_nracanaa stat *st) {
        assert(st);

        အကယ်၍ (S_ISDIR(st->st_mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (S_ISLNK(st->st_mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISCHR(st->st_mode))
                ပြန်ပေး -EBADFD;

        ပြန်ပေး 0;
}

ကိန်းပြည့် stat_verify_device_node(const sa_nracanaa stat *st) {
        assert(st);

        အကယ်၍ (S_ISDIR(st->st_mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (S_ISLNK(st->st_mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ပြန်ပေး -ENOTTY;

        ပြန်ပေး 0;
}

ကိန်းပြည့် is_device_node(const var_na *path) {
        assert(!isempty(path));
        ပြန်ပေး verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

ကိန်းပြည့် stat_verify_regular_or_block(const sa_nracanaa stat *st) {
        assert(st);

        အကယ်၍ (S_ISDIR(st->st_mode))
                ပြန်ပေး -EISDIR;

        အကယ်၍ (S_ISLNK(st->st_mode))
                ပြန်ပေး -ELOOP;

        အကယ်၍ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ပြန်ပေး -EBADFD;

        ပြန်ပေး 0;
}

ကိန်းပြည့် fd_verify_regular_or_block(ကိန်းပြည့် fd) {
        အကယ်၍ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ပြန်ပေး -EISDIR;

        ပြန်ပေး verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

ကိန်းပြည့် dir_is_empty_at(ကိန်းပြည့် dir_fd, const var_na *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;
        sa_nracanaa dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        အတွက် (;;) {
                sa_nracanaa dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                အကယ်၍ (n < 0)
                        ပြန်ပေး -errno;
                အကယ်၍ (n == 0)
                        bha_nga;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        အကယ်၍ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ပြန်ပေး 0;
        }

        ပြန်ပေး 1;
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ပြန်ပေး S_ISCHR(st->st_mode);
}

bool stat_is_empty(sa_nracanaa stat *st) {
        assert(st);

        ပြန်ပေး S_ISREG(st->st_mode) && st->st_size <= 0;
}

ကိန်းပြည့် null_or_empty_path_with_root(const var_na *fn, const var_na *root) {
        _cleanup_free_ var_na *resolved = NULL;
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;
        sa_nracanaa stat st;
        ကိန်းပြည့် r;

        assert(fn);

        /* fn includes the root prefix. */

        အကယ်၍ (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                ပြန်ပေး true;

        အကယ်၍ (empty_or_root(root)) {
                အကယ်၍ (stat(fn, &st) < 0)
                        ပြန်ပေး -errno;

                ပြန်ပေး null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (path_equal(path_startswith(resolved, root), "dev/null"))
                ပြန်ပေး true;

        အကယ်၍ (r == 0)
                ပြန်ပေး -ENOENT;

        အကယ်၍ (fstat(fd, &st) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး null_or_empty(&st);
}

static const var_na* statx_mask_one_to_name(unsigned mask);
static const var_na* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static var_na* prefix##_to_string(type v) {                               \
                အကယ်၍ (v == 0)                                                     \
                        ပြန်ပေး strdup("");                                      \
                                                                                \
                _cleanup_free_ var_na *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const var_na *n = func(f);                                \
                        အကယ်၍ (!n)                                                 \
                                continue;                                       \
                                                                                \
                        အကယ်၍ (!strextend_with_separator(&s, "|", n))              \
                                ပြန်ပေး NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                အကယ်၍ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ပြန်ပေး NULL;                                            \
                                                                                \
                ပြန်ပေး TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

ကိန်းပြည့် xstatx_full(ကိန်းပြည့် fd,
                const var_na *path,
                ကိန်းပြည့် statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                sa_nracanaa statx *ret) {

        sa_nracanaa statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        ကိန်းပြည့် r;

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

        assert(wildcard_fd_is_valid(fd));
        assert((mandatory_mask & optional_mask) == 0);
        assert(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        assert(ret);

        _cleanup_free_ var_na *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        unsigned request_mask = mandatory_mask|optional_mask;
        အကယ်၍ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        အကယ်၍ (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                ပြန်ပေး negative_errno();

        အကယ်၍ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        အကယ်၍ (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                အကယ်၍ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                ပြန်ပေး -EUNATCH;
        }

        အကယ်၍ (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                အကယ်၍ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ပြန်ပေး -EUNATCH;
        }

        *ret = sx;
        ပြန်ပေး FLAGS_SET(sx.stx_mask, optional_mask);
}

static ကိန်းပြည့် xfstatfs(ကိန်းပြည့် fd, sa_nracanaa statfs *ret) {
        assert(ret);

        အကယ်၍ (fd == AT_FDCWD)
                ပြန်ပေး RET_NERRNO(statfs(".", ret));
        အကယ်၍ (fd == XAT_FDROOT)
                ပြန်ပေး RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        ပြန်ပေး RET_NERRNO(fstatfs(fd, ret));
}

ကိန်းပြည့် xstatfsat(ကိန်းပြည့် dir_fd, const var_na *path, sa_nracanaa statfs *ret) {
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        အကယ်၍ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                အကယ်၍ (fd < 0)
                        ပြန်ပေး fd;
                dir_fd = fd;
        }

        ပြန်ပေး xfstatfs(dir_fd, ret);
}

ကိန်းပြည့် fd_is_read_only_fs(ကိန်းပြည့် fd) {
        ကိန်းပြည့် r;

        sa_nracanaa statfs st;
        r = xfstatfs(fd, &st);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (st.f_flags & ST_RDONLY)
                ပြန်ပေး true;

        အကယ်၍ (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ပြန်ပေး access_fd(fd, W_OK) == -EROFS;

        ပြန်ပေး false;
}

ကိန်းပြည့် path_is_read_only_fs(const var_na *path) {
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        အကယ်၍ (fd < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး fd_is_read_only_fs(fd);
}

ကိန်းပြည့် inode_same_at(ကိန်းပြည့် fda, const var_na *filea, ကိန်းပြည့် fdb, const var_na *fileb, ကိန်းပြည့် flags) {
        sa_nracanaa stat sta, stb;
        ကိန်းပြည့် r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        အကယ်၍ ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                ပြန်ပေး -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        အကယ်၍ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                ပြန်ပေး true;

        _cleanup_close_ ကိန်းပြည့် pin_a = -EBADF, pin_b = -EBADF;
        အကယ်၍ (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                အကယ်၍ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        အကယ်၍ (pin_a < 0)
                                ပြန်ပေး -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                အကယ်၍ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        အကယ်၍ (pin_b < 0)
                                ပြန်ပေး -errno;

                        fdb = pin_b;
                        fileb = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ကိန်းပြည့် ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ sa_nracanaa file_handle *ha = NULL, *hb = NULL;
                uint64_t mntida, mntidb;
                ကိန်းပြည့် _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                အကယ်၍ (r < 0) {
                        အကယ်၍ (is_name_to_handle_at_fatal_error(r))
                                ပြန်ပေး r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                အကယ်၍ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                အကယ်၍ (r < 0) {
                        အကယ်၍ (is_name_to_handle_at_fatal_error(r))
                                ပြန်ပေး r;

                        goto fallback;
                }
                အကယ်၍ (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                အကယ်၍ (!file_handle_equal(ha, hb))
                        ပြန်ပေး false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                အကယ်၍ (mntida == mntidb)
                        ပြန်ပေး true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        အကယ်၍ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ပြန်ပေး log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        အကယ်၍ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ပြန်ပေး log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ပြန်ပေး stat_inode_same(&sta, &stb);
}

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(parimaa_na(statfs_f_type_t) >= parimaa_na(s->f_type));

        ပြန်ပေး F_TYPE_EQUAL(s->f_type, magic_value);
}

ကိန်းပြည့် is_fs_type_at(ကိန်းပြည့် dir_fd, const var_na *path, statfs_f_type_t magic_value) {
        ကိန်းပြည့် r;

        sa_nracanaa statfs s;
        r = xstatfsat(dir_fd, path, &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) {
        ပြန်ပေး fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const sa_nracanaa statfs *s) {
        ပြန်ပေး fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

ကိန်းပြည့် fd_is_temporary_fs(ကိန်းပြည့် fd) {
        ကိန်းပြည့် r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး is_temporary_fs(&s);
}

ကိန်းပြည့် fd_is_network_fs(ကိန်းပြည့် fd) {
        ကိန်းပြည့် r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး is_network_fs(&s);
}

ကိန်းပြည့် path_is_temporary_fs(const var_na *path) {
        sa_nracanaa statfs s;

        အကယ်၍ (statfs(path, &s) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး is_temporary_fs(&s);
}

ကိန်းပြည့် path_is_network_fs(const var_na *path) {
        sa_nracanaa statfs s;

        အကယ်၍ (statfs(path, &s) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး is_network_fs(&s);
}

ကိန်းပြည့် proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        ကိန်းပြည့် r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        အကယ်၍ (r == -ENOENT) /* not mounted at all */
                ပြန်ပေး false;

        ပြန်ပေး r;
}

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ပြန်ပေး stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        assert(a);
        assert(b);

        ပြန်ပေး stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        အကယ်၍ (!statx_is_set(a) || !statx_is_set(b))
                ပြန်ပေး false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        ပြန်ပေး
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

ကိန်းပြည့် statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {
        အကယ်၍ (!statx_is_set(a) || !statx_is_set(b))
                ပြန်ပေး false;

        အကယ်၍ ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                ပြန်ပေး a->stx_mnt_id == b->stx_mnt_id;

        ပြန်ပေး -ENODATA;
}

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ပြန်ပေး timespec_load(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ပြန်ပေး timespec_load_nsec(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

void inode_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

ကိန်းပြည့် inode_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        r = CMP(a->st_dev, b->st_dev);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        r = CMP(a->st_ino, b->st_ino);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        ပြန်ပေး CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, sa_nracanaa stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        အကယ်၍ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        မဟုတ်ပါက {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        အကယ်၍ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        မဟုတ်ပါက {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

ကိန်းပြည့် inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        အကယ်၍ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                အကယ်၍ (r != 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                အကယ်၍ (r != 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, sa_nracanaa stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const var_na* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        pak_sa S_IFREG:
                ပြန်ပေး "reg";
        pak_sa S_IFDIR:
                ပြန်ပေး "dir";
        pak_sa S_IFLNK:
                ပြန်ပေး "lnk";
        pak_sa S_IFCHR:
                ပြန်ပေး "chr";
        pak_sa S_IFBLK:
                ပြန်ပေး "blk";
        pak_sa S_IFIFO:
                ပြန်ပေး "fifo";
        pak_sa S_IFSOCK:
                ပြန်ပေး "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ပြန်ပေး NULL;
}

mode_t inode_type_from_string(const var_na *s) {
        အကယ်၍ (!s)
                ပြန်ပေး MODE_INVALID;

        အကယ်၍ (streq(s, "reg"))
                ပြန်ပေး S_IFREG;
        အကယ်၍ (streq(s, "dir"))
                ပြန်ပေး S_IFDIR;
        အကယ်၍ (streq(s, "lnk"))
                ပြန်ပေး S_IFLNK;
        အကယ်၍ (streq(s, "chr"))
                ပြန်ပေး S_IFCHR;
        အကယ်၍ (streq(s, "blk"))
                ပြန်ပေး S_IFBLK;
        အကယ်၍ (streq(s, "fifo"))
                ပြန်ပေး S_IFIFO;
        အကယ်၍ (streq(s, "sock"))
                ပြန်ပေး S_IFSOCK;

        ပြန်ပေး MODE_INVALID;
}

ကိန်းပြည့် vfs_free_bytes(ကိန်းပြည့် fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        sa_nracanaa statvfs sv;
        အကယ်၍ (fstatvfs(fd, &sv) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ပြန်ပေး -ERANGE;

        ပြန်ပေး 0;
}
