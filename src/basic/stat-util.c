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

static ចំនួនគត់ verify_stat_at(
                ចំនួនគត់ fd,
                const var_na *path,
                bool follow,
                ចំនួនគត់ (*verify_func)(const sa_nracanaa stat *st),
                bool verify) {

        sa_nracanaa stat st;
        ចំនួនគត់ r;

        assert(wildcard_fd_is_valid(fd));
        assert(!isempty(path) || !follow);
        assert(verify_func);

        _cleanup_free_ var_na *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ត្រឡប់ -errno;

        r = verify_func(&st);
        ត្រឡប់ verify ? r : r >= 0;
}

static ចំនួនគត់ mode_verify_regular(mode_t mode) {
        ប្រសិនបើ (S_ISDIR(mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (S_ISLNK(mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISREG(mode))
                ត្រឡប់ -EBADFD;

        ត្រឡប់ 0;
}

ចំនួនគត់ stat_verify_regular(const sa_nracanaa stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ត្រឡប់ mode_verify_regular(st->st_mode);
}

ចំនួនគត់ statx_verify_regular(const sa_nracanaa statx *stx) {
        assert(stx);

        ប្រសិនបើ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ត្រឡប់ -ENODATA;

        ត្រឡប់ mode_verify_regular(stx->stx_mode);
}

ចំនួនគត់ verify_regular_at(ចំនួនគត់ fd, const var_na *path, bool follow) {
        ត្រឡប់ verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

ចំនួនគត់ fd_verify_regular(ចំនួនគត់ fd) {
        ប្រសិនបើ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ត្រឡប់ -EISDIR;

        ត្រឡប់ verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static ចំនួនគត់ mode_verify_directory(mode_t mode) {
        ប្រសិនបើ (S_ISLNK(mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISDIR(mode))
                ត្រឡប់ -ENOTDIR;

        ត្រឡប់ 0;
}

ចំនួនគត់ stat_verify_directory(const sa_nracanaa stat *st) {
        assert(st);

        ត្រឡប់ mode_verify_directory(st->st_mode);
}

ចំនួនគត់ statx_verify_directory(const sa_nracanaa statx *stx) {
        assert(stx);

        ប្រសិនបើ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ត្រឡប់ -ENODATA;

        ត្រឡប់ mode_verify_directory(stx->stx_mode);
}

ចំនួនគត់ fd_verify_directory(ចំនួនគត់ fd) {
        ប្រសិនបើ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ត្រឡប់ 0;

        ត្រឡប់ verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

ចំនួនគត់ is_dir_at(ចំនួនគត់ fd, const var_na *path, bool follow) {
        ត្រឡប់ verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

ចំនួនគត់ is_dir(const var_na *path, bool follow) {
        assert(!isempty(path));
        ត្រឡប់ is_dir_at(AT_FDCWD, path, follow);
}

ចំនួនគត់ stat_verify_symlink(const sa_nracanaa stat *st) {
        assert(st);

        ប្រសិនបើ (S_ISDIR(st->st_mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (!S_ISLNK(st->st_mode))
                ត្រឡប់ -ENOLINK;

        ត្រឡប់ 0;
}

ចំនួនគត់ fd_verify_symlink(ចំនួនគត់ fd) {
        ប្រសិនបើ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ត្រឡប់ -EISDIR;

        ត្រឡប់ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

ចំនួនគត់ is_symlink(const var_na *path) {
        assert(!isempty(path));
        ត្រឡប់ verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static ចំនួនគត់ mode_verify_socket(mode_t mode) {
        ប្រសិនបើ (S_ISDIR(mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (S_ISLNK(mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISSOCK(mode))
                ត្រឡប់ -ENOTSOCK;

        ត្រឡប់ 0;
}

ចំនួនគត់ stat_verify_socket(const sa_nracanaa stat *st) {
        assert(st);

        ត្រឡប់ mode_verify_socket(st->st_mode);
}

ចំនួនគត់ statx_verify_socket(const sa_nracanaa statx *stx) {
        assert(stx);

        ត្រឡប់ mode_verify_socket(stx->stx_mode);
}

ចំនួនគត់ fd_verify_socket(ចំនួនគត់ fd) {
        ប្រសិនបើ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ត្រឡប់ -EISDIR;

        ត្រឡប់ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

ចំនួនគត់ is_socket(const var_na *path) {
        assert(!isempty(path));
        ត្រឡប់ verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

ចំនួនគត់ stat_verify_linked(const sa_nracanaa stat *st) {
        assert(st);

        ប្រសិនបើ (st->st_nlink <= 0)
                ត្រឡប់ -EIDRM; /* recognizable error. */

        ត្រឡប់ 0;
}

ចំនួនគត់ fd_verify_linked(ចំនួនគត់ fd) {

        ប្រសិនបើ (fd == XAT_FDROOT)
                ត្រឡប់ 0;

        ត្រឡប់ verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

ចំនួនគត់ stat_verify_block(const sa_nracanaa stat *st) {
        assert(st);

        ប្រសិនបើ (S_ISDIR(st->st_mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (S_ISLNK(st->st_mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISBLK(st->st_mode))
                ត្រឡប់ -ENOTBLK;

        ត្រឡប់ 0;
}

ចំនួនគត់ fd_verify_block(ចំនួនគត់ fd) {
        ប្រសិនបើ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ត្រឡប់ -EISDIR;

        ត្រឡប់ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

ចំនួនគត់ stat_verify_char(const sa_nracanaa stat *st) {
        assert(st);

        ប្រសិនបើ (S_ISDIR(st->st_mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (S_ISLNK(st->st_mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISCHR(st->st_mode))
                ត្រឡប់ -EBADFD;

        ត្រឡប់ 0;
}

ចំនួនគត់ stat_verify_device_node(const sa_nracanaa stat *st) {
        assert(st);

        ប្រសិនបើ (S_ISDIR(st->st_mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (S_ISLNK(st->st_mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ត្រឡប់ -ENOTTY;

        ត្រឡប់ 0;
}

ចំនួនគត់ is_device_node(const var_na *path) {
        assert(!isempty(path));
        ត្រឡប់ verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

ចំនួនគត់ stat_verify_regular_or_block(const sa_nracanaa stat *st) {
        assert(st);

        ប្រសិនបើ (S_ISDIR(st->st_mode))
                ត្រឡប់ -EISDIR;

        ប្រសិនបើ (S_ISLNK(st->st_mode))
                ត្រឡប់ -ELOOP;

        ប្រសិនបើ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ត្រឡប់ -EBADFD;

        ត្រឡប់ 0;
}

ចំនួនគត់ fd_verify_regular_or_block(ចំនួនគត់ fd) {
        ប្រសិនបើ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ត្រឡប់ -EISDIR;

        ត្រឡប់ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

ចំនួនគត់ dir_is_empty_at(ចំនួនគត់ dir_fd, const var_na *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        sa_nracanaa dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        សម្រាប់ (;;) {
                sa_nracanaa dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ប្រសិនបើ (n < 0)
                        ត្រឡប់ -errno;
                ប្រសិនបើ (n == 0)
                        bha_nga;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ប្រសិនបើ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ត្រឡប់ 0;
        }

        ត្រឡប់ 1;
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ត្រឡប់ S_ISCHR(st->st_mode);
}

bool stat_is_empty(sa_nracanaa stat *st) {
        assert(st);

        ត្រឡប់ S_ISREG(st->st_mode) && st->st_size <= 0;
}

ចំនួនគត់ null_or_empty_path_with_root(const var_na *fn, const var_na *root) {
        _cleanup_free_ var_na *resolved = NULL;
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        sa_nracanaa stat st;
        ចំនួនគត់ r;

        assert(fn);

        /* fn includes the root prefix. */

        ប្រសិនបើ (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                ត្រឡប់ true;

        ប្រសិនបើ (empty_or_root(root)) {
                ប្រសិនបើ (stat(fn, &st) < 0)
                        ត្រឡប់ -errno;

                ត្រឡប់ null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (path_equal(path_startswith(resolved, root), "dev/null"))
                ត្រឡប់ true;

        ប្រសិនបើ (r == 0)
                ត្រឡប់ -ENOENT;

        ប្រសិនបើ (fstat(fd, &st) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ null_or_empty(&st);
}

static const var_na* statx_mask_one_to_name(unsigned mask);
static const var_na* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static var_na* prefix##_to_string(type v) {                               \
                ប្រសិនបើ (v == 0)                                                     \
                        ត្រឡប់ strdup("");                                      \
                                                                                \
                _cleanup_free_ var_na *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const var_na *n = func(f);                                \
                        ប្រសិនបើ (!n)                                                 \
                                continue;                                       \
                                                                                \
                        ប្រសិនបើ (!strextend_with_separator(&s, "|", n))              \
                                ត្រឡប់ NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ប្រសិនបើ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ត្រឡប់ NULL;                                            \
                                                                                \
                ត្រឡប់ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

ចំនួនគត់ xstatx_full(ចំនួនគត់ fd,
                const var_na *path,
                ចំនួនគត់ statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                sa_nracanaa statx *ret) {

        sa_nracanaa statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        ចំនួនគត់ r;

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
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ប្រសិនបើ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ប្រសិនបើ (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                ត្រឡប់ negative_errno();

        ប្រសិនបើ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ត្រឡប់ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ប្រសិនបើ (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ប្រសិនបើ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                ត្រឡប់ -EUNATCH;
        }

        ប្រសិនបើ (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ប្រសិនបើ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ត្រឡប់ -EUNATCH;
        }

        *ret = sx;
        ត្រឡប់ FLAGS_SET(sx.stx_mask, optional_mask);
}

static ចំនួនគត់ xfstatfs(ចំនួនគត់ fd, sa_nracanaa statfs *ret) {
        assert(ret);

        ប្រសិនបើ (fd == AT_FDCWD)
                ត្រឡប់ RET_NERRNO(statfs(".", ret));
        ប្រសិនបើ (fd == XAT_FDROOT)
                ត្រឡប់ RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        ត្រឡប់ RET_NERRNO(fstatfs(fd, ret));
}

ចំនួនគត់ xstatfsat(ចំនួនគត់ dir_fd, const var_na *path, sa_nracanaa statfs *ret) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        ប្រសិនបើ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ប្រសិនបើ (fd < 0)
                        ត្រឡប់ fd;
                dir_fd = fd;
        }

        ត្រឡប់ xfstatfs(dir_fd, ret);
}

ចំនួនគត់ fd_is_read_only_fs(ចំនួនគត់ fd) {
        ចំនួនគត់ r;

        sa_nracanaa statfs st;
        r = xfstatfs(fd, &st);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (st.f_flags & ST_RDONLY)
                ត្រឡប់ true;

        ប្រសិនបើ (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ត្រឡប់ access_fd(fd, W_OK) == -EROFS;

        ត្រឡប់ false;
}

ចំនួនគត់ path_is_read_only_fs(const var_na *path) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ fd_is_read_only_fs(fd);
}

ចំនួនគត់ inode_same_at(ចំនួនគត់ fda, const var_na *filea, ចំនួនគត់ fdb, const var_na *fileb, ចំនួនគត់ flags) {
        sa_nracanaa stat sta, stb;
        ចំនួនគត់ r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ប្រសិនបើ ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                ត្រឡប់ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ប្រសិនបើ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                ត្រឡប់ true;

        _cleanup_close_ ចំនួនគត់ pin_a = -EBADF, pin_b = -EBADF;
        ប្រសិនបើ (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ប្រសិនបើ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ប្រសិនបើ (pin_a < 0)
                                ត្រឡប់ -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ប្រសិនបើ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ប្រសិនបើ (pin_b < 0)
                                ត្រឡប់ -errno;

                        fdb = pin_b;
                        fileb = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ចំនួនគត់ ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ sa_nracanaa file_handle *ha = NULL, *hb = NULL;
                uint64_t mntida, mntidb;
                ចំនួនគត់ _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ប្រសិនបើ (r < 0) {
                        ប្រសិនបើ (is_name_to_handle_at_fatal_error(r))
                                ត្រឡប់ r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                ប្រសិនបើ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                ប្រសិនបើ (r < 0) {
                        ប្រសិនបើ (is_name_to_handle_at_fatal_error(r))
                                ត្រឡប់ r;

                        goto fallback;
                }
                ប្រសិនបើ (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ប្រសិនបើ (!file_handle_equal(ha, hb))
                        ត្រឡប់ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ប្រសិនបើ (mntida == mntidb)
                        ត្រឡប់ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ប្រសិនបើ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ត្រឡប់ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ប្រសិនបើ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ត្រឡប់ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ត្រឡប់ stat_inode_same(&sta, &stb);
}

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(parimaa_na(statfs_f_type_t) >= parimaa_na(s->f_type));

        ត្រឡប់ F_TYPE_EQUAL(s->f_type, magic_value);
}

ចំនួនគត់ is_fs_type_at(ចំនួនគត់ dir_fd, const var_na *path, statfs_f_type_t magic_value) {
        ចំនួនគត់ r;

        sa_nracanaa statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) {
        ត្រឡប់ fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const sa_nracanaa statfs *s) {
        ត្រឡប់ fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

ចំនួនគត់ fd_is_temporary_fs(ចំនួនគត់ fd) {
        ចំនួនគត់ r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ is_temporary_fs(&s);
}

ចំនួនគត់ fd_is_network_fs(ចំនួនគត់ fd) {
        ចំនួនគត់ r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ is_network_fs(&s);
}

ចំនួនគត់ path_is_temporary_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ប្រសិនបើ (statfs(path, &s) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ is_temporary_fs(&s);
}

ចំនួនគត់ path_is_network_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ប្រសិនបើ (statfs(path, &s) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ is_network_fs(&s);
}

ចំនួនគត់ proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        ចំនួនគត់ r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ប្រសិនបើ (r == -ENOENT) /* not mounted at all */
                ត្រឡប់ false;

        ត្រឡប់ r;
}

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ត្រឡប់ stat_is_set(a) && stat_is_set(b) &&
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

        ត្រឡប់ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ប្រសិនបើ (!statx_is_set(a) || !statx_is_set(b))
                ត្រឡប់ false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        ត្រឡប់
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

ចំនួនគត់ statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {
        ប្រសិនបើ (!statx_is_set(a) || !statx_is_set(b))
                ត្រឡប់ false;

        ប្រសិនបើ ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                ត្រឡប់ a->stx_mnt_id == b->stx_mnt_id;

        ត្រឡប់ -ENODATA;
}

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ត្រឡប់ timespec_load(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ត្រឡប់ timespec_load_nsec(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

void inode_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

ចំនួនគត់ inode_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        ចំនួនគត់ r;

        assert(a);
        assert(b);

        r = CMP(a->st_dev, b->st_dev);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        r = CMP(a->st_ino, b->st_ino);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        ត្រឡប់ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, sa_nracanaa stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ប្រសិនបើ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        បើមិនដូច្នេះ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ប្រសិនបើ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        បើមិនដូច្នេះ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

ចំនួនគត់ inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        ចំនួនគត់ r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ r;

        ប្រសិនបើ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ប្រសិនបើ (r != 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ប្រសិនបើ (r != 0)
                        ត្រឡប់ r;
        }

        ត្រឡប់ 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, sa_nracanaa stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const var_na* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        pak_sa S_IFREG:
                ត្រឡប់ "reg";
        pak_sa S_IFDIR:
                ត្រឡប់ "dir";
        pak_sa S_IFLNK:
                ត្រឡប់ "lnk";
        pak_sa S_IFCHR:
                ត្រឡប់ "chr";
        pak_sa S_IFBLK:
                ត្រឡប់ "blk";
        pak_sa S_IFIFO:
                ត្រឡប់ "fifo";
        pak_sa S_IFSOCK:
                ត្រឡប់ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ត្រឡប់ NULL;
}

mode_t inode_type_from_string(const var_na *s) {
        ប្រសិនបើ (!s)
                ត្រឡប់ MODE_INVALID;

        ប្រសិនបើ (streq(s, "reg"))
                ត្រឡប់ S_IFREG;
        ប្រសិនបើ (streq(s, "dir"))
                ត្រឡប់ S_IFDIR;
        ប្រសិនបើ (streq(s, "lnk"))
                ត្រឡប់ S_IFLNK;
        ប្រសិនបើ (streq(s, "chr"))
                ត្រឡប់ S_IFCHR;
        ប្រសិនបើ (streq(s, "blk"))
                ត្រឡប់ S_IFBLK;
        ប្រសិនបើ (streq(s, "fifo"))
                ត្រឡប់ S_IFIFO;
        ប្រសិនបើ (streq(s, "sock"))
                ត្រឡប់ S_IFSOCK;

        ត្រឡប់ MODE_INVALID;
}

ចំនួនគត់ vfs_free_bytes(ចំនួនគត់ fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        sa_nracanaa statvfs sv;
        ប្រសិនបើ (fstatvfs(fd, &sv) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ត្រឡប់ -ERANGE;

        ត្រឡប់ 0;
}
