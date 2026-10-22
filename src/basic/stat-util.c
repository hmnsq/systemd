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

static 整数 verify_stat_at(
                整数 fd,
                const char *path,
                bool follow,
                整数 (*verify_func)(const struct stat *st),
                bool verify) {

        struct stat st;
        整数 r;

        assert(wildcard_fd_is_valid(fd));
        assert(!isempty(path) || !follow);
        assert(verify_func);

        _cleanup_free_ char *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        もし (r < 0)
                戻る r;

        もし (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                戻る -errno;

        r = verify_func(&st);
        戻る verify ? r : r >= 0;
}

static 整数 mode_verify_regular(mode_t mode) {
        もし (S_ISDIR(mode))
                戻る -EISDIR;

        もし (S_ISLNK(mode))
                戻る -ELOOP;

        もし (!S_ISREG(mode))
                戻る -EBADFD;

        戻る 0;
}

整数 stat_verify_regular(const struct stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        戻る mode_verify_regular(st->st_mode);
}

整数 statx_verify_regular(const struct statx *stx) {
        assert(stx);

        もし (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                戻る -ENODATA;

        戻る mode_verify_regular(stx->stx_mode);
}

整数 verify_regular_at(整数 fd, const char *path, bool follow) {
        戻る verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

整数 fd_verify_regular(整数 fd) {
        もし (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                戻る -EISDIR;

        戻る verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static 整数 mode_verify_directory(mode_t mode) {
        もし (S_ISLNK(mode))
                戻る -ELOOP;

        もし (!S_ISDIR(mode))
                戻る -ENOTDIR;

        戻る 0;
}

整数 stat_verify_directory(const struct stat *st) {
        assert(st);

        戻る mode_verify_directory(st->st_mode);
}

整数 statx_verify_directory(const struct statx *stx) {
        assert(stx);

        もし (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                戻る -ENODATA;

        戻る mode_verify_directory(stx->stx_mode);
}

整数 fd_verify_directory(整数 fd) {
        もし (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                戻る 0;

        戻る verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

整数 is_dir_at(整数 fd, const char *path, bool follow) {
        戻る verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

整数 is_dir(const char *path, bool follow) {
        assert(!isempty(path));
        戻る is_dir_at(AT_FDCWD, path, follow);
}

整数 stat_verify_symlink(const struct stat *st) {
        assert(st);

        もし (S_ISDIR(st->st_mode))
                戻る -EISDIR;

        もし (!S_ISLNK(st->st_mode))
                戻る -ENOLINK;

        戻る 0;
}

整数 fd_verify_symlink(整数 fd) {
        もし (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                戻る -EISDIR;

        戻る verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

整数 is_symlink(const char *path) {
        assert(!isempty(path));
        戻る verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static 整数 mode_verify_socket(mode_t mode) {
        もし (S_ISDIR(mode))
                戻る -EISDIR;

        もし (S_ISLNK(mode))
                戻る -ELOOP;

        もし (!S_ISSOCK(mode))
                戻る -ENOTSOCK;

        戻る 0;
}

整数 stat_verify_socket(const struct stat *st) {
        assert(st);

        戻る mode_verify_socket(st->st_mode);
}

整数 statx_verify_socket(const struct statx *stx) {
        assert(stx);

        戻る mode_verify_socket(stx->stx_mode);
}

整数 fd_verify_socket(整数 fd) {
        もし (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                戻る -EISDIR;

        戻る verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

整数 is_socket(const char *path) {
        assert(!isempty(path));
        戻る verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

整数 stat_verify_linked(const struct stat *st) {
        assert(st);

        もし (st->st_nlink <= 0)
                戻る -EIDRM; /* recognizable error. */

        戻る 0;
}

整数 fd_verify_linked(整数 fd) {

        もし (fd == XAT_FDROOT)
                戻る 0;

        戻る verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

整数 stat_verify_block(const struct stat *st) {
        assert(st);

        もし (S_ISDIR(st->st_mode))
                戻る -EISDIR;

        もし (S_ISLNK(st->st_mode))
                戻る -ELOOP;

        もし (!S_ISBLK(st->st_mode))
                戻る -ENOTBLK;

        戻る 0;
}

整数 fd_verify_block(整数 fd) {
        もし (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                戻る -EISDIR;

        戻る verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

整数 stat_verify_char(const struct stat *st) {
        assert(st);

        もし (S_ISDIR(st->st_mode))
                戻る -EISDIR;

        もし (S_ISLNK(st->st_mode))
                戻る -ELOOP;

        もし (!S_ISCHR(st->st_mode))
                戻る -EBADFD;

        戻る 0;
}

整数 stat_verify_device_node(const struct stat *st) {
        assert(st);

        もし (S_ISDIR(st->st_mode))
                戻る -EISDIR;

        もし (S_ISLNK(st->st_mode))
                戻る -ELOOP;

        もし (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                戻る -ENOTTY;

        戻る 0;
}

整数 is_device_node(const char *path) {
        assert(!isempty(path));
        戻る verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

整数 stat_verify_regular_or_block(const struct stat *st) {
        assert(st);

        もし (S_ISDIR(st->st_mode))
                戻る -EISDIR;

        もし (S_ISLNK(st->st_mode))
                戻る -ELOOP;

        もし (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                戻る -EBADFD;

        戻る 0;
}

整数 fd_verify_regular_or_block(整数 fd) {
        もし (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                戻る -EISDIR;

        戻る verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

整数 dir_is_empty_at(整数 dir_fd, const char *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ 整数 fd = -EBADF;
        struct dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        もし (fd < 0)
                戻る fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        ために (;;) {
                struct dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                もし (n < 0)
                        戻る -errno;
                もし (n == 0)
                        break;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        もし (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                戻る 0;
        }

        戻る 1;
}

bool stat_may_be_dev_null(struct stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        戻る S_ISCHR(st->st_mode);
}

bool stat_is_empty(struct stat *st) {
        assert(st);

        戻る S_ISREG(st->st_mode) && st->st_size <= 0;
}

整数 null_or_empty_path_with_root(const char *fn, const char *root) {
        _cleanup_free_ char *resolved = NULL;
        _cleanup_close_ 整数 fd = -EBADF;
        struct stat st;
        整数 r;

        assert(fn);

        /* fn includes the root prefix. */

        もし (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                戻る true;

        もし (empty_or_root(root)) {
                もし (stat(fn, &st) < 0)
                        戻る -errno;

                戻る null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        もし (r < 0)
                戻る r;

        もし (path_equal(path_startswith(resolved, root), "dev/null"))
                戻る true;

        もし (r == 0)
                戻る -ENOENT;

        もし (fstat(fd, &st) < 0)
                戻る -errno;

        戻る null_or_empty(&st);
}

static const char* statx_mask_one_to_name(unsigned mask);
static const char* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static char* prefix##_to_string(type v) {                               \
                もし (v == 0)                                                     \
                        戻る strdup("");                                      \
                                                                                \
                _cleanup_free_ char *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const char *n = func(f);                                \
                        もし (!n)                                                 \
                                continue;                                       \
                                                                                \
                        もし (!strextend_with_separator(&s, "|", n))              \
                                戻る NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                もし (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        戻る NULL;                                            \
                                                                                \
                戻る TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

整数 xstatx_full(整数 fd,
                const char *path,
                整数 statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                struct statx *ret) {

        struct statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        整数 r;

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
        もし (r < 0)
                戻る r;

        unsigned request_mask = mandatory_mask|optional_mask;
        もし (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        もし (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                戻る negative_errno();

        もし (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                戻る log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        もし (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                もし (DEBUG_LOGGING) {
                        _cleanup_free_ char *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                戻る -EUNATCH;
        }

        もし (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                もし (DEBUG_LOGGING) {
                        _cleanup_free_ char *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                戻る -EUNATCH;
        }

        *ret = sx;
        戻る FLAGS_SET(sx.stx_mask, optional_mask);
}

static 整数 xfstatfs(整数 fd, struct statfs *ret) {
        assert(ret);

        もし (fd == AT_FDCWD)
                戻る RET_NERRNO(statfs(".", ret));
        もし (fd == XAT_FDROOT)
                戻る RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        戻る RET_NERRNO(fstatfs(fd, ret));
}

整数 xstatfsat(整数 dir_fd, const char *path, struct statfs *ret) {
        _cleanup_close_ 整数 fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        もし (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                もし (fd < 0)
                        戻る fd;
                dir_fd = fd;
        }

        戻る xfstatfs(dir_fd, ret);
}

整数 fd_is_read_only_fs(整数 fd) {
        整数 r;

        struct statfs st;
        r = xfstatfs(fd, &st);
        もし (r < 0)
                戻る r;

        もし (st.f_flags & ST_RDONLY)
                戻る true;

        もし (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                戻る access_fd(fd, W_OK) == -EROFS;

        戻る false;
}

整数 path_is_read_only_fs(const char *path) {
        _cleanup_close_ 整数 fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        もし (fd < 0)
                戻る -errno;

        戻る fd_is_read_only_fs(fd);
}

整数 inode_same_at(整数 fda, const char *filea, 整数 fdb, const char *fileb, 整数 flags) {
        struct stat sta, stb;
        整数 r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        もし ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                戻る -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        もし (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                戻る true;

        _cleanup_close_ 整数 pin_a = -EBADF, pin_b = -EBADF;
        もし (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                もし (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        もし (pin_a < 0)
                                戻る -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                もし (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        もし (pin_b < 0)
                                戻る -errno;

                        fdb = pin_b;
                        fileb = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                整数 ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ struct file_handle *ha = NULL, *hb = NULL;
                uint64_t mntida, mntidb;
                整数 _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                もし (r < 0) {
                        もし (is_name_to_handle_at_fatal_error(r))
                                戻る r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                もし (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                もし (r < 0) {
                        もし (is_name_to_handle_at_fatal_error(r))
                                戻る r;

                        goto fallback;
                }
                もし (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                もし (!file_handle_equal(ha, hb))
                        戻る false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                もし (mntida == mntidb)
                        戻る true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        もし (fstatat(fda, strempty(filea), &sta, flags) < 0)
                戻る log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        もし (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                戻る log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        戻る stat_inode_same(&sta, &stb);
}

bool is_fs_type(const struct statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(sizeof(statfs_f_type_t) >= sizeof(s->f_type));

        戻る F_TYPE_EQUAL(s->f_type, magic_value);
}

整数 is_fs_type_at(整数 dir_fd, const char *path, statfs_f_type_t magic_value) {
        整数 r;

        struct statfs s;
        r = xstatfsat(dir_fd, path, &s);
        もし (r < 0)
                戻る r;

        戻る is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const struct statfs *s) {
        戻る fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const struct statfs *s) {
        戻る fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

整数 fd_is_temporary_fs(整数 fd) {
        整数 r;

        struct statfs s;
        r = xfstatfs(fd, &s);
        もし (r < 0)
                戻る r;

        戻る is_temporary_fs(&s);
}

整数 fd_is_network_fs(整数 fd) {
        整数 r;

        struct statfs s;
        r = xfstatfs(fd, &s);
        もし (r < 0)
                戻る r;

        戻る is_network_fs(&s);
}

整数 path_is_temporary_fs(const char *path) {
        struct statfs s;

        もし (statfs(path, &s) < 0)
                戻る -errno;

        戻る is_temporary_fs(&s);
}

整数 path_is_network_fs(const char *path) {
        struct statfs s;

        もし (statfs(path, &s) < 0)
                戻る -errno;

        戻る is_network_fs(&s);
}

整数 proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        整数 r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        もし (r == -ENOENT) /* not mounted at all */
                戻る false;

        戻る r;
}

bool stat_inode_same(const struct stat *a, const struct stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        戻る stat_is_set(a) && stat_is_set(b) &&
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

        戻る stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const struct statx *a, const struct statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        もし (!statx_is_set(a) || !statx_is_set(b))
                戻る false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        戻る
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

整数 statx_mount_same(const struct statx *a, const struct statx *b) {
        もし (!statx_is_set(a) || !statx_is_set(b))
                戻る false;

        もし ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                戻る a->stx_mnt_id == b->stx_mnt_id;

        戻る -ENODATA;
}

usec_t statx_timestamp_load(const struct statx_timestamp *ts) {
        assert(ts);
        戻る timespec_load(&(const struct timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const struct statx_timestamp *ts) {
        assert(ts);
        戻る timespec_load_nsec(&(const struct timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

void inode_hash_func(const struct stat *q, struct siphash *state) {
        assert(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

整数 inode_compare_func(const struct stat *a, const struct stat *b) {
        整数 r;

        assert(a);
        assert(b);

        r = CMP(a->st_dev, b->st_dev);
        もし (r != 0)
                戻る r;

        r = CMP(a->st_ino, b->st_ino);
        もし (r != 0)
                戻る r;

        戻る CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, struct stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const struct stat *q, struct siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        もし (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        否则 {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        もし (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        否则 {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

整数 inode_unmodified_compare_func(const struct stat *a, const struct stat *b) {
        整数 r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        もし (r != 0)
                戻る r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        もし (r != 0)
                戻る r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        もし (r != 0)
                戻る r;

        もし (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                もし (r != 0)
                        戻る r;
        }

        もし (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                もし (r != 0)
                        戻る r;
        }

        戻る 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, struct stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const char* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        case S_IFREG:
                戻る "reg";
        case S_IFDIR:
                戻る "dir";
        case S_IFLNK:
                戻る "lnk";
        case S_IFCHR:
                戻る "chr";
        case S_IFBLK:
                戻る "blk";
        case S_IFIFO:
                戻る "fifo";
        case S_IFSOCK:
                戻る "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        戻る NULL;
}

mode_t inode_type_from_string(const char *s) {
        もし (!s)
                戻る MODE_INVALID;

        もし (streq(s, "reg"))
                戻る S_IFREG;
        もし (streq(s, "dir"))
                戻る S_IFDIR;
        もし (streq(s, "lnk"))
                戻る S_IFLNK;
        もし (streq(s, "chr"))
                戻る S_IFCHR;
        もし (streq(s, "blk"))
                戻る S_IFBLK;
        もし (streq(s, "fifo"))
                戻る S_IFIFO;
        もし (streq(s, "sock"))
                戻る S_IFSOCK;

        戻る MODE_INVALID;
}

整数 vfs_free_bytes(整数 fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        struct statvfs sv;
        もし (fstatvfs(fd, &sv) < 0)
                戻る -errno;

        もし (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                戻る -ERANGE;

        戻る 0;
}
