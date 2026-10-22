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
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ᰀᰧᰌ -errno;

        r = verify_func(&st);
        ᰀᰧᰌ verify ? r : r >= 0;
}

static puur_naa_nka mode_verify_regular(mode_t mode) {
        ᰀᰧ (S_ISDIR(mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (S_ISLNK(mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISREG(mode))
                ᰀᰧᰌ -EBADFD;

        ᰀᰧᰌ 0;
}

puur_naa_nka stat_verify_regular(const sa_nracanaa stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ᰀᰧᰌ mode_verify_regular(st->st_mode);
}

puur_naa_nka statx_verify_regular(const sa_nracanaa statx *stx) {
        assert(stx);

        ᰀᰧ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ᰀᰧᰌ -ENODATA;

        ᰀᰧᰌ mode_verify_regular(stx->stx_mode);
}

puur_naa_nka verify_regular_at(puur_naa_nka fd, const var_na *path, bool follow) {
        ᰀᰧᰌ verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

puur_naa_nka fd_verify_regular(puur_naa_nka fd) {
        ᰀᰧ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧᰌ verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static puur_naa_nka mode_verify_directory(mode_t mode) {
        ᰀᰧ (S_ISLNK(mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISDIR(mode))
                ᰀᰧᰌ -ENOTDIR;

        ᰀᰧᰌ 0;
}

puur_naa_nka stat_verify_directory(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧᰌ mode_verify_directory(st->st_mode);
}

puur_naa_nka statx_verify_directory(const sa_nracanaa statx *stx) {
        assert(stx);

        ᰀᰧ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ᰀᰧᰌ -ENODATA;

        ᰀᰧᰌ mode_verify_directory(stx->stx_mode);
}

puur_naa_nka fd_verify_directory(puur_naa_nka fd) {
        ᰀᰧ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᰀᰧᰌ 0;

        ᰀᰧᰌ verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

puur_naa_nka is_dir_at(puur_naa_nka fd, const var_na *path, bool follow) {
        ᰀᰧᰌ verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

puur_naa_nka is_dir(const var_na *path, bool follow) {
        assert(!isempty(path));
        ᰀᰧᰌ is_dir_at(AT_FDCWD, path, follow);
}

puur_naa_nka stat_verify_symlink(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧ (S_ISDIR(st->st_mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (!S_ISLNK(st->st_mode))
                ᰀᰧᰌ -ENOLINK;

        ᰀᰧᰌ 0;
}

puur_naa_nka fd_verify_symlink(puur_naa_nka fd) {
        ᰀᰧ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧᰌ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

puur_naa_nka is_symlink(const var_na *path) {
        assert(!isempty(path));
        ᰀᰧᰌ verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static puur_naa_nka mode_verify_socket(mode_t mode) {
        ᰀᰧ (S_ISDIR(mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (S_ISLNK(mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISSOCK(mode))
                ᰀᰧᰌ -ENOTSOCK;

        ᰀᰧᰌ 0;
}

puur_naa_nka stat_verify_socket(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧᰌ mode_verify_socket(st->st_mode);
}

puur_naa_nka statx_verify_socket(const sa_nracanaa statx *stx) {
        assert(stx);

        ᰀᰧᰌ mode_verify_socket(stx->stx_mode);
}

puur_naa_nka fd_verify_socket(puur_naa_nka fd) {
        ᰀᰧ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧᰌ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

puur_naa_nka is_socket(const var_na *path) {
        assert(!isempty(path));
        ᰀᰧᰌ verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

puur_naa_nka stat_verify_linked(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧ (st->st_nlink <= 0)
                ᰀᰧᰌ -EIDRM; /* recognizable error. */

        ᰀᰧᰌ 0;
}

puur_naa_nka fd_verify_linked(puur_naa_nka fd) {

        ᰀᰧ (fd == XAT_FDROOT)
                ᰀᰧᰌ 0;

        ᰀᰧᰌ verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

puur_naa_nka stat_verify_block(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧ (S_ISDIR(st->st_mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (S_ISLNK(st->st_mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISBLK(st->st_mode))
                ᰀᰧᰌ -ENOTBLK;

        ᰀᰧᰌ 0;
}

puur_naa_nka fd_verify_block(puur_naa_nka fd) {
        ᰀᰧ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧᰌ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

puur_naa_nka stat_verify_char(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧ (S_ISDIR(st->st_mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (S_ISLNK(st->st_mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISCHR(st->st_mode))
                ᰀᰧᰌ -EBADFD;

        ᰀᰧᰌ 0;
}

puur_naa_nka stat_verify_device_node(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧ (S_ISDIR(st->st_mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (S_ISLNK(st->st_mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ᰀᰧᰌ -ENOTTY;

        ᰀᰧᰌ 0;
}

puur_naa_nka is_device_node(const var_na *path) {
        assert(!isempty(path));
        ᰀᰧᰌ verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

puur_naa_nka stat_verify_regular_or_block(const sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧ (S_ISDIR(st->st_mode))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧ (S_ISLNK(st->st_mode))
                ᰀᰧᰌ -ELOOP;

        ᰀᰧ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ᰀᰧᰌ -EBADFD;

        ᰀᰧᰌ 0;
}

puur_naa_nka fd_verify_regular_or_block(puur_naa_nka fd) {
        ᰀᰧ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ᰀᰧᰌ -EISDIR;

        ᰀᰧᰌ verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

puur_naa_nka dir_is_empty_at(puur_naa_nka dir_fd, const var_na *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        sa_nracanaa dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ fd;

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
                ᰀᰧ (n < 0)
                        ᰀᰧᰌ -errno;
                ᰀᰧ (n == 0)
                        bha_nga;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ᰀᰧ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ᰀᰧᰌ 0;
        }

        ᰀᰧᰌ 1;
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ᰀᰧᰌ S_ISCHR(st->st_mode);
}

bool stat_is_empty(sa_nracanaa stat *st) {
        assert(st);

        ᰀᰧᰌ S_ISREG(st->st_mode) && st->st_size <= 0;
}

puur_naa_nka null_or_empty_path_with_root(const var_na *fn, const var_na *root) {
        _cleanup_free_ var_na *resolved = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        sa_nracanaa stat st;
        puur_naa_nka r;

        assert(fn);

        /* fn includes the root prefix. */

        ᰀᰧ (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                ᰀᰧᰌ true;

        ᰀᰧ (empty_or_root(root)) {
                ᰀᰧ (stat(fn, &st) < 0)
                        ᰀᰧᰌ -errno;

                ᰀᰧᰌ null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (path_equal(path_startswith(resolved, root), "dev/null"))
                ᰀᰧᰌ true;

        ᰀᰧ (r == 0)
                ᰀᰧᰌ -ENOENT;

        ᰀᰧ (fstat(fd, &st) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ null_or_empty(&st);
}

static const var_na* statx_mask_one_to_name(unsigned mask);
static const var_na* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static var_na* prefix##_to_string(type v) {                               \
                ᰀᰧ (v == 0)                                                     \
                        ᰀᰧᰌ strdup("");                                      \
                                                                                \
                _cleanup_free_ var_na *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const var_na *n = func(f);                                \
                        ᰀᰧ (!n)                                                 \
                                continue;                                       \
                                                                                \
                        ᰀᰧ (!strextend_with_separator(&s, "|", n))              \
                                ᰀᰧᰌ NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ᰀᰧ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ᰀᰧᰌ NULL;                                            \
                                                                                \
                ᰀᰧᰌ TAKE_PTR(s);                                             \
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
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ᰀᰧ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ᰀᰧ (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                ᰀᰧᰌ negative_errno();

        ᰀᰧ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ᰀᰧᰌ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ᰀᰧ (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ᰀᰧ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                ᰀᰧᰌ -EUNATCH;
        }

        ᰀᰧ (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ᰀᰧ (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ᰀᰧᰌ -EUNATCH;
        }

        *ret = sx;
        ᰀᰧᰌ FLAGS_SET(sx.stx_mask, optional_mask);
}

static puur_naa_nka xfstatfs(puur_naa_nka fd, sa_nracanaa statfs *ret) {
        assert(ret);

        ᰀᰧ (fd == AT_FDCWD)
                ᰀᰧᰌ RET_NERRNO(statfs(".", ret));
        ᰀᰧ (fd == XAT_FDROOT)
                ᰀᰧᰌ RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        ᰀᰧᰌ RET_NERRNO(fstatfs(fd, ret));
}

puur_naa_nka xstatfsat(puur_naa_nka dir_fd, const var_na *path, sa_nracanaa statfs *ret) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        ᰀᰧ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ᰀᰧ (fd < 0)
                        ᰀᰧᰌ fd;
                dir_fd = fd;
        }

        ᰀᰧᰌ xfstatfs(dir_fd, ret);
}

puur_naa_nka fd_is_read_only_fs(puur_naa_nka fd) {
        puur_naa_nka r;

        sa_nracanaa statfs st;
        r = xfstatfs(fd, &st);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (st.f_flags & ST_RDONLY)
                ᰀᰧᰌ true;

        ᰀᰧ (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ᰀᰧᰌ access_fd(fd, W_OK) == -EROFS;

        ᰀᰧᰌ false;
}

puur_naa_nka path_is_read_only_fs(const var_na *path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ fd_is_read_only_fs(fd);
}

puur_naa_nka inode_same_at(puur_naa_nka fda, const var_na *filea, puur_naa_nka fdb, const var_na *fileb, puur_naa_nka flags) {
        sa_nracanaa stat sta, stb;
        puur_naa_nka r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ᰀᰧ ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                ᰀᰧᰌ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ᰀᰧ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                ᰀᰧᰌ true;

        _cleanup_close_ puur_naa_nka pin_a = -EBADF, pin_b = -EBADF;
        ᰀᰧ (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ᰀᰧ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ᰀᰧ (pin_a < 0)
                                ᰀᰧᰌ -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ᰀᰧ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ᰀᰧ (pin_b < 0)
                                ᰀᰧᰌ -errno;

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
                ᰀᰧ (r < 0) {
                        ᰀᰧ (is_name_to_handle_at_fatal_error(r))
                                ᰀᰧᰌ r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                ᰀᰧ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                ᰀᰧ (r < 0) {
                        ᰀᰧ (is_name_to_handle_at_fatal_error(r))
                                ᰀᰧᰌ r;

                        goto fallback;
                }
                ᰀᰧ (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ᰀᰧ (!file_handle_equal(ha, hb))
                        ᰀᰧᰌ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ᰀᰧ (mntida == mntidb)
                        ᰀᰧᰌ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ᰀᰧ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ᰀᰧᰌ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ᰀᰧ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ᰀᰧᰌ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ᰀᰧᰌ stat_inode_same(&sta, &stb);
}

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(parimaa_na(statfs_f_type_t) >= parimaa_na(s->f_type));

        ᰀᰧᰌ F_TYPE_EQUAL(s->f_type, magic_value);
}

puur_naa_nka is_fs_type_at(puur_naa_nka dir_fd, const var_na *path, statfs_f_type_t magic_value) {
        puur_naa_nka r;

        sa_nracanaa statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) {
        ᰀᰧᰌ fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const sa_nracanaa statfs *s) {
        ᰀᰧᰌ fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

puur_naa_nka fd_is_temporary_fs(puur_naa_nka fd) {
        puur_naa_nka r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ is_temporary_fs(&s);
}

puur_naa_nka fd_is_network_fs(puur_naa_nka fd) {
        puur_naa_nka r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ is_network_fs(&s);
}

puur_naa_nka path_is_temporary_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ᰀᰧ (statfs(path, &s) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ is_temporary_fs(&s);
}

puur_naa_nka path_is_network_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ᰀᰧ (statfs(path, &s) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ is_network_fs(&s);
}

puur_naa_nka proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        puur_naa_nka r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ᰀᰧ (r == -ENOENT) /* not mounted at all */
                ᰀᰧᰌ false;

        ᰀᰧᰌ r;
}

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ᰀᰧᰌ stat_is_set(a) && stat_is_set(b) &&
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

        ᰀᰧᰌ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ᰀᰧ (!statx_is_set(a) || !statx_is_set(b))
                ᰀᰧᰌ false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        ᰀᰧᰌ
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

puur_naa_nka statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {
        ᰀᰧ (!statx_is_set(a) || !statx_is_set(b))
                ᰀᰧᰌ false;

        ᰀᰧ ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                ᰀᰧᰌ a->stx_mnt_id == b->stx_mnt_id;

        ᰀᰧᰌ -ENODATA;
}

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ᰀᰧᰌ timespec_load(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        ᰀᰧᰌ timespec_load_nsec(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
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
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        r = CMP(a->st_ino, b->st_ino);
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, sa_nracanaa stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ᰀᰧ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ᰀᰧᰀ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ᰀᰧ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ᰀᰧᰀ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

puur_naa_nka inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        puur_naa_nka r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ᰀᰧ (r != 0)
                ᰀᰧᰌ r;

        ᰀᰧ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ᰀᰧ (r != 0)
                        ᰀᰧᰌ r;
        }

        ᰀᰧ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ᰀᰧ (r != 0)
                        ᰀᰧᰌ r;
        }

        ᰀᰧᰌ 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, sa_nracanaa stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const var_na* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        pak_sa S_IFREG:
                ᰀᰧᰌ "reg";
        pak_sa S_IFDIR:
                ᰀᰧᰌ "dir";
        pak_sa S_IFLNK:
                ᰀᰧᰌ "lnk";
        pak_sa S_IFCHR:
                ᰀᰧᰌ "chr";
        pak_sa S_IFBLK:
                ᰀᰧᰌ "blk";
        pak_sa S_IFIFO:
                ᰀᰧᰌ "fifo";
        pak_sa S_IFSOCK:
                ᰀᰧᰌ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ᰀᰧᰌ NULL;
}

mode_t inode_type_from_string(const var_na *s) {
        ᰀᰧ (!s)
                ᰀᰧᰌ MODE_INVALID;

        ᰀᰧ (streq(s, "reg"))
                ᰀᰧᰌ S_IFREG;
        ᰀᰧ (streq(s, "dir"))
                ᰀᰧᰌ S_IFDIR;
        ᰀᰧ (streq(s, "lnk"))
                ᰀᰧᰌ S_IFLNK;
        ᰀᰧ (streq(s, "chr"))
                ᰀᰧᰌ S_IFCHR;
        ᰀᰧ (streq(s, "blk"))
                ᰀᰧᰌ S_IFBLK;
        ᰀᰧ (streq(s, "fifo"))
                ᰀᰧᰌ S_IFIFO;
        ᰀᰧ (streq(s, "sock"))
                ᰀᰧᰌ S_IFSOCK;

        ᰀᰧᰌ MODE_INVALID;
}

puur_naa_nka vfs_free_bytes(puur_naa_nka fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        sa_nracanaa statvfs sv;
        ᰀᰧ (fstatvfs(fd, &sv) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ᰀᰧᰌ -ERANGE;

        ᰀᰧᰌ 0;
}
