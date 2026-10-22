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

static цео verify_stat_at(
                цео fd,
                const char *path,
                bool follow,
                цео (*verify_func)(const struct stat *st),
                bool verify) {

        struct stat st;
        цео r;

        assert(wildcard_fd_is_valid(fd));
        assert(!isempty(path) || !follow);
        assert(verify_func);

        _cleanup_free_ char *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ако (r < 0)
                врати r;

        ако (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                врати -errno;

        r = verify_func(&st);
        врати verify ? r : r >= 0;
}

static цео mode_verify_regular(mode_t mode) {
        ако (S_ISDIR(mode))
                врати -EISDIR;

        ако (S_ISLNK(mode))
                врати -ELOOP;

        ако (!S_ISREG(mode))
                врати -EBADFD;

        врати 0;
}

цео stat_verify_regular(const struct stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        врати mode_verify_regular(st->st_mode);
}

цео statx_verify_regular(const struct statx *stx) {
        assert(stx);

        ако (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                врати -ENODATA;

        врати mode_verify_regular(stx->stx_mode);
}

цео verify_regular_at(цео fd, const char *path, bool follow) {
        врати verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

цео fd_verify_regular(цео fd) {
        ако (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                врати -EISDIR;

        врати verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static цео mode_verify_directory(mode_t mode) {
        ако (S_ISLNK(mode))
                врати -ELOOP;

        ако (!S_ISDIR(mode))
                врати -ENOTDIR;

        врати 0;
}

цео stat_verify_directory(const struct stat *st) {
        assert(st);

        врати mode_verify_directory(st->st_mode);
}

цео statx_verify_directory(const struct statx *stx) {
        assert(stx);

        ако (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                врати -ENODATA;

        врати mode_verify_directory(stx->stx_mode);
}

цео fd_verify_directory(цео fd) {
        ако (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                врати 0;

        врати verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

цео is_dir_at(цео fd, const char *path, bool follow) {
        врати verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

цео is_dir(const char *path, bool follow) {
        assert(!isempty(path));
        врати is_dir_at(AT_FDCWD, path, follow);
}

цео stat_verify_symlink(const struct stat *st) {
        assert(st);

        ако (S_ISDIR(st->st_mode))
                врати -EISDIR;

        ако (!S_ISLNK(st->st_mode))
                врати -ENOLINK;

        врати 0;
}

цео fd_verify_symlink(цео fd) {
        ако (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                врати -EISDIR;

        врати verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

цео is_symlink(const char *path) {
        assert(!isempty(path));
        врати verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static цео mode_verify_socket(mode_t mode) {
        ако (S_ISDIR(mode))
                врати -EISDIR;

        ако (S_ISLNK(mode))
                врати -ELOOP;

        ако (!S_ISSOCK(mode))
                врати -ENOTSOCK;

        врати 0;
}

цео stat_verify_socket(const struct stat *st) {
        assert(st);

        врати mode_verify_socket(st->st_mode);
}

цео statx_verify_socket(const struct statx *stx) {
        assert(stx);

        врати mode_verify_socket(stx->stx_mode);
}

цео fd_verify_socket(цео fd) {
        ако (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                врати -EISDIR;

        врати verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

цео is_socket(const char *path) {
        assert(!isempty(path));
        врати verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

цео stat_verify_linked(const struct stat *st) {
        assert(st);

        ако (st->st_nlink <= 0)
                врати -EIDRM; /* recognizable error. */

        врати 0;
}

цео fd_verify_linked(цео fd) {

        ако (fd == XAT_FDROOT)
                врати 0;

        врати verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

цео stat_verify_block(const struct stat *st) {
        assert(st);

        ако (S_ISDIR(st->st_mode))
                врати -EISDIR;

        ако (S_ISLNK(st->st_mode))
                врати -ELOOP;

        ако (!S_ISBLK(st->st_mode))
                врати -ENOTBLK;

        врати 0;
}

цео fd_verify_block(цео fd) {
        ако (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                врати -EISDIR;

        врати verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

цео stat_verify_char(const struct stat *st) {
        assert(st);

        ако (S_ISDIR(st->st_mode))
                врати -EISDIR;

        ако (S_ISLNK(st->st_mode))
                врати -ELOOP;

        ако (!S_ISCHR(st->st_mode))
                врати -EBADFD;

        врати 0;
}

цео stat_verify_device_node(const struct stat *st) {
        assert(st);

        ако (S_ISDIR(st->st_mode))
                врати -EISDIR;

        ако (S_ISLNK(st->st_mode))
                врати -ELOOP;

        ако (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                врати -ENOTTY;

        врати 0;
}

цео is_device_node(const char *path) {
        assert(!isempty(path));
        врати verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

цео stat_verify_regular_or_block(const struct stat *st) {
        assert(st);

        ако (S_ISDIR(st->st_mode))
                врати -EISDIR;

        ако (S_ISLNK(st->st_mode))
                врати -ELOOP;

        ако (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                врати -EBADFD;

        врати 0;
}

цео fd_verify_regular_or_block(цео fd) {
        ако (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                врати -EISDIR;

        врати verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

цео dir_is_empty_at(цео dir_fd, const char *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ цео fd = -EBADF;
        struct dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ако (fd < 0)
                врати fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        за (;;) {
                struct dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ако (n < 0)
                        врати -errno;
                ако (n == 0)
                        break;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ако (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                врати 0;
        }

        врати 1;
}

bool stat_may_be_dev_null(struct stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        врати S_ISCHR(st->st_mode);
}

bool stat_is_empty(struct stat *st) {
        assert(st);

        врати S_ISREG(st->st_mode) && st->st_size <= 0;
}

цео null_or_empty_path_with_root(const char *fn, const char *root) {
        _cleanup_free_ char *resolved = NULL;
        _cleanup_close_ цео fd = -EBADF;
        struct stat st;
        цео r;

        assert(fn);

        /* fn includes the root prefix. */

        ако (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                врати true;

        ако (empty_or_root(root)) {
                ако (stat(fn, &st) < 0)
                        врати -errno;

                врати null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        ако (r < 0)
                врати r;

        ако (path_equal(path_startswith(resolved, root), "dev/null"))
                врати true;

        ако (r == 0)
                врати -ENOENT;

        ако (fstat(fd, &st) < 0)
                врати -errno;

        врати null_or_empty(&st);
}

static const char* statx_mask_one_to_name(unsigned mask);
static const char* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static char* prefix##_to_string(type v) {                               \
                ако (v == 0)                                                     \
                        врати strdup("");                                      \
                                                                                \
                _cleanup_free_ char *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const char *n = func(f);                                \
                        ако (!n)                                                 \
                                continue;                                       \
                                                                                \
                        ако (!strextend_with_separator(&s, "|", n))              \
                                врати NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ако (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        врати NULL;                                            \
                                                                                \
                врати TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

цео xstatx_full(цео fd,
                const char *path,
                цео statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                struct statx *ret) {

        struct statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        цео r;

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

        _cleanup_free_ char *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ако (r < 0)
                врати r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ако (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ако (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                врати negative_errno();

        ако (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                врати log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ако (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ако (DEBUG_LOGGING) {
                        _cleanup_free_ char *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                врати -EUNATCH;
        }

        ако (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ако (DEBUG_LOGGING) {
                        _cleanup_free_ char *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                врати -EUNATCH;
        }

        *ret = sx;
        врати FLAGS_SET(sx.stx_mask, optional_mask);
}

static цео xfstatfs(цео fd, struct statfs *ret) {
        assert(ret);

        ако (fd == AT_FDCWD)
                врати RET_NERRNO(statfs(".", ret));
        ако (fd == XAT_FDROOT)
                врати RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        врати RET_NERRNO(fstatfs(fd, ret));
}

цео xstatfsat(цео dir_fd, const char *path, struct statfs *ret) {
        _cleanup_close_ цео fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        ако (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ако (fd < 0)
                        врати fd;
                dir_fd = fd;
        }

        врати xfstatfs(dir_fd, ret);
}

цео fd_is_read_only_fs(цео fd) {
        цео r;

        struct statfs st;
        r = xfstatfs(fd, &st);
        ако (r < 0)
                врати r;

        ако (st.f_flags & ST_RDONLY)
                врати true;

        ако (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                врати access_fd(fd, W_OK) == -EROFS;

        врати false;
}

цео path_is_read_only_fs(const char *path) {
        _cleanup_close_ цео fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ако (fd < 0)
                врати -errno;

        врати fd_is_read_only_fs(fd);
}

цео inode_same_at(цео fda, const char *filea, цео fdb, const char *fileb, цео flags) {
        struct stat sta, stb;
        цео r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ако ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                врати -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ако (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                врати true;

        _cleanup_close_ цео pin_a = -EBADF, pin_b = -EBADF;
        ако (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ако (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ако (pin_a < 0)
                                врати -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ако (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ако (pin_b < 0)
                                врати -errno;

                        fdb = pin_b;
                        fileb = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                цео ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ struct file_handle *ha = NULL, *hb = NULL;
                uint64_t mntida, mntidb;
                цео _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ако (r < 0) {
                        ако (is_name_to_handle_at_fatal_error(r))
                                врати r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                ако (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                ако (r < 0) {
                        ако (is_name_to_handle_at_fatal_error(r))
                                врати r;

                        goto fallback;
                }
                ако (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ако (!file_handle_equal(ha, hb))
                        врати false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ако (mntida == mntidb)
                        врати true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ако (fstatat(fda, strempty(filea), &sta, flags) < 0)
                врати log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ако (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                врати log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        врати stat_inode_same(&sta, &stb);
}

bool is_fs_type(const struct statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(sizeof(statfs_f_type_t) >= sizeof(s->f_type));

        врати F_TYPE_EQUAL(s->f_type, magic_value);
}

цео is_fs_type_at(цео dir_fd, const char *path, statfs_f_type_t magic_value) {
        цео r;

        struct statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ако (r < 0)
                врати r;

        врати is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const struct statfs *s) {
        врати fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const struct statfs *s) {
        врати fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

цео fd_is_temporary_fs(цео fd) {
        цео r;

        struct statfs s;
        r = xfstatfs(fd, &s);
        ако (r < 0)
                врати r;

        врати is_temporary_fs(&s);
}

цео fd_is_network_fs(цео fd) {
        цео r;

        struct statfs s;
        r = xfstatfs(fd, &s);
        ако (r < 0)
                врати r;

        врати is_network_fs(&s);
}

цео path_is_temporary_fs(const char *path) {
        struct statfs s;

        ако (statfs(path, &s) < 0)
                врати -errno;

        врати is_temporary_fs(&s);
}

цео path_is_network_fs(const char *path) {
        struct statfs s;

        ако (statfs(path, &s) < 0)
                врати -errno;

        врати is_network_fs(&s);
}

цео proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        цео r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ако (r == -ENOENT) /* not mounted at all */
                врати false;

        врати r;
}

bool stat_inode_same(const struct stat *a, const struct stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        врати stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(const struct stat *a, const struct stat *b) {

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

        врати stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const struct statx *a, const struct statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ако (!statx_is_set(a) || !statx_is_set(b))
                врати false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        врати
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

цео statx_mount_same(const struct statx *a, const struct statx *b) {
        ако (!statx_is_set(a) || !statx_is_set(b))
                врати false;

        ако ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                врати a->stx_mnt_id == b->stx_mnt_id;

        врати -ENODATA;
}

usec_t statx_timestamp_load(const struct statx_timestamp *ts) {
        assert(ts);
        врати timespec_load(&(const struct timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const struct statx_timestamp *ts) {
        assert(ts);
        врати timespec_load_nsec(&(const struct timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

void inode_hash_func(const struct stat *q, struct siphash *state) {
        assert(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

цео inode_compare_func(const struct stat *a, const struct stat *b) {
        цео r;

        assert(a);
        assert(b);

        r = CMP(a->st_dev, b->st_dev);
        ако (r != 0)
                врати r;

        r = CMP(a->st_ino, b->st_ino);
        ако (r != 0)
                врати r;

        врати CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, struct stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const struct stat *q, struct siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ако (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        инако {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ако (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        инако {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

цео inode_unmodified_compare_func(const struct stat *a, const struct stat *b) {
        цео r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        ако (r != 0)
                врати r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ако (r != 0)
                врати r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ако (r != 0)
                врати r;

        ако (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ако (r != 0)
                        врати r;
        }

        ако (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ако (r != 0)
                        врати r;
        }

        врати 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, struct stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const char* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        case S_IFREG:
                врати "reg";
        case S_IFDIR:
                врати "dir";
        case S_IFLNK:
                врати "lnk";
        case S_IFCHR:
                врати "chr";
        case S_IFBLK:
                врати "blk";
        case S_IFIFO:
                врати "fifo";
        case S_IFSOCK:
                врати "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        врати NULL;
}

mode_t inode_type_from_string(const char *s) {
        ако (!s)
                врати MODE_INVALID;

        ако (streq(s, "reg"))
                врати S_IFREG;
        ако (streq(s, "dir"))
                врати S_IFDIR;
        ако (streq(s, "lnk"))
                врати S_IFLNK;
        ако (streq(s, "chr"))
                врати S_IFCHR;
        ако (streq(s, "blk"))
                врати S_IFBLK;
        ако (streq(s, "fifo"))
                врати S_IFIFO;
        ако (streq(s, "sock"))
                врати S_IFSOCK;

        врати MODE_INVALID;
}

цео vfs_free_bytes(цео fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        struct statvfs sv;
        ако (fstatvfs(fd, &sv) < 0)
                врати -errno;

        ако (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                врати -ERANGE;

        врати 0;
}
