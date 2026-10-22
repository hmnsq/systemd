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

static puur_naa_nka verify_stat_at(
                puur_naa_nka fd,
                const var_na *path,
                bool follow,
                puur_naa_nka (*verify_func)(const sa_nracanaa stat *st),
                bool verify) {

        sa_nracanaa stat st;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(fd));
        assert(!isempty(path) || !follow);
        assert(verify_func);

        _cleanup_free_ var_na *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ᤀᤡᤴ -errno;

        r = verify_func(&st);
        ᤀᤡᤴ verify ? r : r >= 0;
}

static puur_naa_nka mode_verify_regular(mode_t mode) {
        ᤀᤡ᤺ (S_ISDIR(mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (S_ISLNK(mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISREG(mode))
                ᤀᤡᤴ -EBADFD;

        ᤀᤡᤴ 0;
}

puur_naa_nka stat_verify_regular(const sa_nracanaa stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ᤀᤡᤴ mode_verify_regular(st->st_mode);
}

puur_naa_nka statx_verify_regular(const sa_nracanaa statx *stx) {
        assert(stx);

        ᤀᤡ᤺ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ᤀᤡᤴ -ENODATA;

        ᤀᤡᤴ mode_verify_regular(stx->stx_mode);
}

puur_naa_nka verify_regular_at(puur_naa_nka fd, const var_na *path, bool follow) {
        ᤀᤡᤴ verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

puur_naa_nka fd_verify_regular(puur_naa_nka fd) {
        ᤀᤡ᤺ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡᤴ verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static puur_naa_nka mode_verify_directory(mode_t mode) {
        ᤀᤡ᤺ (S_ISLNK(mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISDIR(mode))
                ᤀᤡᤴ -ENOTDIR;

        ᤀᤡᤴ 0;
}

puur_naa_nka stat_verify_directory(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡᤴ mode_verify_directory(st->st_mode);
}

puur_naa_nka statx_verify_directory(const sa_nracanaa statx *stx) {
        assert(stx);

        ᤀᤡ᤺ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ᤀᤡᤴ -ENODATA;

        ᤀᤡᤴ mode_verify_directory(stx->stx_mode);
}

puur_naa_nka fd_verify_directory(puur_naa_nka fd) {
        ᤀᤡ᤺ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᤀᤡᤴ 0;

        ᤀᤡᤴ verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

puur_naa_nka is_dir_at(puur_naa_nka fd, const var_na *path, bool follow) {
        ᤀᤡᤴ verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

puur_naa_nka is_dir(const var_na *path, bool follow) {
        assert(!isempty(path));
        ᤀᤡᤴ is_dir_at(AT_FDCWD, path, follow);
}

puur_naa_nka stat_verify_symlink(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡ᤺ (S_ISDIR(st->st_mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (!S_ISLNK(st->st_mode))
                ᤀᤡᤴ -ENOLINK;

        ᤀᤡᤴ 0;
}

puur_naa_nka fd_verify_symlink(puur_naa_nka fd) {
        ᤀᤡ᤺ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡᤴ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

puur_naa_nka is_symlink(const var_na *path) {
        assert(!isempty(path));
        ᤀᤡᤴ verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static puur_naa_nka mode_verify_socket(mode_t mode) {
        ᤀᤡ᤺ (S_ISDIR(mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (S_ISLNK(mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISSOCK(mode))
                ᤀᤡᤴ -ENOTSOCK;

        ᤀᤡᤴ 0;
}

puur_naa_nka stat_verify_socket(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡᤴ mode_verify_socket(st->st_mode);
}

puur_naa_nka statx_verify_socket(const sa_nracanaa statx *stx) {
        assert(stx);

        ᤀᤡᤴ mode_verify_socket(stx->stx_mode);
}

puur_naa_nka fd_verify_socket(puur_naa_nka fd) {
        ᤀᤡ᤺ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡᤴ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

puur_naa_nka is_socket(const var_na *path) {
        assert(!isempty(path));
        ᤀᤡᤴ verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

puur_naa_nka stat_verify_linked(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡ᤺ (st->st_nlink <= 0)
                ᤀᤡᤴ -EIDRM; /* recognizable error. */

        ᤀᤡᤴ 0;
}

puur_naa_nka fd_verify_linked(puur_naa_nka fd) {

        ᤀᤡ᤺ (fd == XAT_FDROOT)
                ᤀᤡᤴ 0;

        ᤀᤡᤴ verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

puur_naa_nka stat_verify_block(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡ᤺ (S_ISDIR(st->st_mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (S_ISLNK(st->st_mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISBLK(st->st_mode))
                ᤀᤡᤴ -ENOTBLK;

        ᤀᤡᤴ 0;
}

puur_naa_nka fd_verify_block(puur_naa_nka fd) {
        ᤀᤡ᤺ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡᤴ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

puur_naa_nka stat_verify_char(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡ᤺ (S_ISDIR(st->st_mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (S_ISLNK(st->st_mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISCHR(st->st_mode))
                ᤀᤡᤴ -EBADFD;

        ᤀᤡᤴ 0;
}

puur_naa_nka stat_verify_device_node(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡ᤺ (S_ISDIR(st->st_mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (S_ISLNK(st->st_mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ᤀᤡᤴ -ENOTTY;

        ᤀᤡᤴ 0;
}

puur_naa_nka is_device_node(const var_na *path) {
        assert(!isempty(path));
        ᤀᤡᤴ verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

puur_naa_nka stat_verify_regular_or_block(const sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡ᤺ (S_ISDIR(st->st_mode))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡ᤺ (S_ISLNK(st->st_mode))
                ᤀᤡᤴ -ELOOP;

        ᤀᤡ᤺ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ᤀᤡᤴ -EBADFD;

        ᤀᤡᤴ 0;
}

puur_naa_nka fd_verify_regular_or_block(puur_naa_nka fd) {
        ᤀᤡ᤺ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᤀᤡᤴ -EISDIR;

        ᤀᤡᤴ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

puur_naa_nka dir_is_empty_at(puur_naa_nka dir_fd, const var_na *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        sa_nracanaa dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ᤀᤡ᤺ (fd < 0)
                ᤀᤡᤴ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        for (;;) {
                sa_nracanaa dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ᤀᤡ᤺ (n < 0)
                        ᤀᤡᤴ -errno;
                ᤀᤡ᤺ (n == 0)
                        bha_nga;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ᤀᤡ᤺ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ᤀᤡᤴ 0;
        }

        ᤀᤡᤴ 1;
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ᤀᤡᤴ S_ISCHR(st->st_mode);
}

bool stat_is_empty(sa_nracanaa stat *st) {
        assert(st);

        ᤀᤡᤴ S_ISREG(st->st_mode) && st->st_size <= 0;
}

puur_naa_nka null_or_empty_path_with_root(const var_na *fn, const var_na *root) {
        _cleanup_free_ var_na *resolved = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        sa_nracanaa stat st;
        puur_naa_nka r;

        assert(fn);

        /* fn includes the root prefix. */

        ᤀᤡ᤺ (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                ᤀᤡᤴ true;

        ᤀᤡ᤺ (empty_or_root(root)) {
                ᤀᤡ᤺ (stat(fn, &st) < 0)
                        ᤀᤡᤴ -errno;

                ᤀᤡᤴ null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (path_equal(path_startswith(resolved, root), "dev/null"))
                ᤀᤡᤴ true;

        ᤀᤡ᤺ (r == 0)
                ᤀᤡᤴ -ENOENT;

        ᤀᤡ᤺ (fstat(fd, &st) < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡᤴ null_or_empty(&st);
}

static const var_na* statx_mask_one_to_name(unsigned mask);
static const var_na* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static var_na* prefix##_to_string(type v) {                               \
                ᤀᤡ᤺ (v == 0)                                                     \
                        ᤀᤡᤴ strdup("");                                      \
                                                                                \
                _cleanup_free_ var_na *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const var_na *n = func(f);                                \
                        ᤀᤡ᤺ (!n)                                                 \
                                continue;                                       \
                                                                                \
                        ᤀᤡ᤺ (!strextend_with_separator(&s, "|", n))              \
                                ᤀᤡᤴ NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ᤀᤡ᤺ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ᤀᤡᤴ NULL;                                            \
                                                                                \
                ᤀᤡᤴ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

puur_naa_nka xstatx_full(puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                sa_nracanaa statx *ret) {

        sa_nracanaa statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        puur_naa_nka r;

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
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ᤀᤡ᤺ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ᤀᤡ᤺ (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                ᤀᤡᤴ negative_errno();

        ᤀᤡ᤺ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ᤀᤡᤴ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ᤀᤡ᤺ (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ᤀᤡ᤺ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                ᤀᤡᤴ -EUNATCH;
        }

        ᤀᤡ᤺ (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ᤀᤡ᤺ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ᤀᤡᤴ -EUNATCH;
        }

        *ret = sx;
        ᤀᤡᤴ FLAGS_SET(sx.stx_mask, optional_mask);
}

static puur_naa_nka xfstatfs(puur_naa_nka fd, sa_nracanaa statfs *ret) {
        assert(ret);

        ᤀᤡ᤺ (fd == AT_FDCWD)
                ᤀᤡᤴ RET_NERRNO(statfs(".", ret));
        ᤀᤡ᤺ (fd == XAT_FDROOT)
                ᤀᤡᤴ RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        ᤀᤡᤴ RET_NERRNO(fstatfs(fd, ret));
}

puur_naa_nka xstatfsat(puur_naa_nka dir_fd, const var_na *path, sa_nracanaa statfs *ret) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        ᤀᤡ᤺ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ᤀᤡ᤺ (fd < 0)
                        ᤀᤡᤴ fd;
                dir_fd = fd;
        }

        ᤀᤡᤴ xfstatfs(dir_fd, ret);
}

puur_naa_nka fd_is_read_only_fs(puur_naa_nka fd) {
        puur_naa_nka r;

        sa_nracanaa statfs st;
        r = xfstatfs(fd, &st);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (st.f_flags & ST_RDONLY)
                ᤀᤡᤴ true;

        ᤀᤡ᤺ (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ᤀᤡᤴ access_fd(fd, W_OK) == -EROFS;

        ᤀᤡᤴ false;
}

puur_naa_nka path_is_read_only_fs(const var_na *path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ᤀᤡ᤺ (fd < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡᤴ fd_is_read_only_fs(fd);
}

puur_naa_nka inode_same_at(puur_naa_nka fda, const var_na *filea, puur_naa_nka fdb, const var_na *fileb, puur_naa_nka flags) {
        sa_nracanaa stat sta, stb;
        puur_naa_nka r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ᤀᤡ᤺ ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                ᤀᤡᤴ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ᤀᤡ᤺ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                ᤀᤡᤴ true;

        _cleanup_close_ puur_naa_nka pin_a = -EBADF, pin_b = -EBADF;
        ᤀᤡ᤺ (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ᤀᤡ᤺ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ᤀᤡ᤺ (pin_a < 0)
                                ᤀᤡᤴ -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ᤀᤡ᤺ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ᤀᤡ᤺ (pin_b < 0)
                                ᤀᤡᤴ -errno;

                        fdb = pin_b;
                        fileb = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                puur_naa_nka ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ sa_nracanaa file_handle *ha = NULL, *hb = NULL;
                uint64_t mntida, mntidb;
                puur_naa_nka _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ᤀᤡ᤺ (r < 0) {
                        ᤀᤡ᤺ (is_name_to_handle_at_fatal_error(r))
                                ᤀᤡᤴ r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                ᤀᤡ᤺ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                ᤀᤡ᤺ (r < 0) {
                        ᤀᤡ᤺ (is_name_to_handle_at_fatal_error(r))
                                ᤀᤡᤴ r;

                        goto fallback;
                }
                ᤀᤡ᤺ (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ᤀᤡ᤺ (!file_handle_equal(ha, hb))
                        ᤀᤡᤴ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ᤀᤡ᤺ (mntida == mntidb)
                        ᤀᤡᤴ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ᤀᤡ᤺ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ᤀᤡᤴ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ᤀᤡ᤺ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ᤀᤡᤴ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ᤀᤡᤴ stat_inode_same(&sta, &stb);
}

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(parimaa_na(statfs_f_type_t) >= parimaa_na(s->f_type));

        ᤀᤡᤴ F_TYPE_EQUAL(s->f_type, magic_value);
}

puur_naa_nka is_fs_type_at(puur_naa_nka dir_fd, const var_na *path, statfs_f_type_t magic_value) {
        puur_naa_nka r;

        sa_nracanaa statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) {
        ᤀᤡᤴ fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const sa_nracanaa statfs *s) {
        ᤀᤡᤴ fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

puur_naa_nka fd_is_temporary_fs(puur_naa_nka fd) {
        puur_naa_nka r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ is_temporary_fs(&s);
}

puur_naa_nka fd_is_network_fs(puur_naa_nka fd) {
        puur_naa_nka r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ is_network_fs(&s);
}

puur_naa_nka path_is_temporary_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ᤀᤡ᤺ (statfs(path, &s) < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡᤴ is_temporary_fs(&s);
}

puur_naa_nka path_is_network_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ᤀᤡ᤺ (statfs(path, &s) < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡᤴ is_network_fs(&s);
}

puur_naa_nka proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        puur_naa_nka r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ᤀᤡ᤺ (r == -ENOENT) /* not mounted at all */
                ᤀᤡᤴ false;

        ᤀᤡᤴ r;
}

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ᤀᤡᤴ stat_is_set(a) && stat_is_set(b) &&
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

        ᤀᤡᤴ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ᤀᤡ᤺ (!statx_is_set(a) || !statx_is_set(b))
                ᤀᤡᤴ false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        ᤀᤡᤴ
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

puur_naa_nka statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {
        ᤀᤡ᤺ (!statx_is_set(a) || !statx_is_set(b))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                ᤀᤡᤴ a->stx_mnt_id == b->stx_mnt_id;

        ᤀᤡᤴ -ENODATA;
}

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ᤀᤡᤴ timespec_load(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ᤀᤡᤴ timespec_load_nsec(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

void inode_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

puur_naa_nka inode_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        puur_naa_nka r;

        assert(a);
        assert(b);

        r = CMP(a->st_dev, b->st_dev);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        r = CMP(a->st_ino, b->st_ino);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, sa_nracanaa stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ᤀᤡ᤺ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ᤀᤡ᤺ᤆᤡ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ᤀᤡ᤺ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ᤀᤡ᤺ᤆᤡ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

puur_naa_nka inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        puur_naa_nka r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ᤀᤡ᤺ (r != 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ᤀᤡ᤺ (r != 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ᤀᤡ᤺ (r != 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡᤴ 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, sa_nracanaa stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const var_na* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        pak_sa S_IFREG:
                ᤀᤡᤴ "reg";
        pak_sa S_IFDIR:
                ᤀᤡᤴ "dir";
        pak_sa S_IFLNK:
                ᤀᤡᤴ "lnk";
        pak_sa S_IFCHR:
                ᤀᤡᤴ "chr";
        pak_sa S_IFBLK:
                ᤀᤡᤴ "blk";
        pak_sa S_IFIFO:
                ᤀᤡᤴ "fifo";
        pak_sa S_IFSOCK:
                ᤀᤡᤴ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ᤀᤡᤴ NULL;
}

mode_t inode_type_from_string(const var_na *s) {
        ᤀᤡ᤺ (!s)
                ᤀᤡᤴ MODE_INVALID;

        ᤀᤡ᤺ (streq(s, "reg"))
                ᤀᤡᤴ S_IFREG;
        ᤀᤡ᤺ (streq(s, "dir"))
                ᤀᤡᤴ S_IFDIR;
        ᤀᤡ᤺ (streq(s, "lnk"))
                ᤀᤡᤴ S_IFLNK;
        ᤀᤡ᤺ (streq(s, "chr"))
                ᤀᤡᤴ S_IFCHR;
        ᤀᤡ᤺ (streq(s, "blk"))
                ᤀᤡᤴ S_IFBLK;
        ᤀᤡ᤺ (streq(s, "fifo"))
                ᤀᤡᤴ S_IFIFO;
        ᤀᤡ᤺ (streq(s, "sock"))
                ᤀᤡᤴ S_IFSOCK;

        ᤀᤡᤴ MODE_INVALID;
}

puur_naa_nka vfs_free_bytes(puur_naa_nka fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        sa_nracanaa statvfs sv;
        ᤀᤡ᤺ (fstatvfs(fd, &sv) < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡ᤺ (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ᤀᤡᤴ -ERANGE;

        ᤀᤡᤴ 0;
}
