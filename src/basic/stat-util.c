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

static จำนวนเต็ม verify_stat_at(
                จำนวนเต็ม fd,
                const var_na *path,
                bool follow,
                จำนวนเต็ม (*verify_func)(const sa_nracanaa stat *st),
                bool verify) {

        sa_nracanaa stat st;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(fd));
        assert(!isempty(path) || !follow);
        assert(verify_func);

        _cleanup_free_ var_na *p = NULL;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ถ้า (r < 0)
                คืน r;

        ถ้า (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                คืน -errno;

        r = verify_func(&st);
        คืน verify ? r : r >= 0;
}

static จำนวนเต็ม mode_verify_regular(mode_t mode) {
        ถ้า (S_ISDIR(mode))
                คืน -EISDIR;

        ถ้า (S_ISLNK(mode))
                คืน -ELOOP;

        ถ้า (!S_ISREG(mode))
                คืน -EBADFD;

        คืน 0;
}

จำนวนเต็ม stat_verify_regular(const sa_nracanaa stat *st) {
        assert(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        คืน mode_verify_regular(st->st_mode);
}

จำนวนเต็ม statx_verify_regular(const sa_nracanaa statx *stx) {
        assert(stx);

        ถ้า (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                คืน -ENODATA;

        คืน mode_verify_regular(stx->stx_mode);
}

จำนวนเต็ม verify_regular_at(จำนวนเต็ม fd, const var_na *path, bool follow) {
        คืน verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

จำนวนเต็ม fd_verify_regular(จำนวนเต็ม fd) {
        ถ้า (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                คืน -EISDIR;

        คืน verify_regular_at(fd, /* path= */ NULL, /* follow= */ false);
}

static จำนวนเต็ม mode_verify_directory(mode_t mode) {
        ถ้า (S_ISLNK(mode))
                คืน -ELOOP;

        ถ้า (!S_ISDIR(mode))
                คืน -ENOTDIR;

        คืน 0;
}

จำนวนเต็ม stat_verify_directory(const sa_nracanaa stat *st) {
        assert(st);

        คืน mode_verify_directory(st->st_mode);
}

จำนวนเต็ม statx_verify_directory(const sa_nracanaa statx *stx) {
        assert(stx);

        ถ้า (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                คืน -ENODATA;

        คืน mode_verify_directory(stx->stx_mode);
}

จำนวนเต็ม fd_verify_directory(จำนวนเต็ม fd) {
        ถ้า (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                คืน 0;

        คืน verify_stat_at(fd, NULL, false, stat_verify_directory, true);
}

จำนวนเต็ม is_dir_at(จำนวนเต็ม fd, const var_na *path, bool follow) {
        คืน verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

จำนวนเต็ม is_dir(const var_na *path, bool follow) {
        assert(!isempty(path));
        คืน is_dir_at(AT_FDCWD, path, follow);
}

จำนวนเต็ม stat_verify_symlink(const sa_nracanaa stat *st) {
        assert(st);

        ถ้า (S_ISDIR(st->st_mode))
                คืน -EISDIR;

        ถ้า (!S_ISLNK(st->st_mode))
                คืน -ENOLINK;

        คืน 0;
}

จำนวนเต็ม fd_verify_symlink(จำนวนเต็ม fd) {
        ถ้า (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                คืน -EISDIR;

        คืน verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

จำนวนเต็ม is_symlink(const var_na *path) {
        assert(!isempty(path));
        คืน verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

static จำนวนเต็ม mode_verify_socket(mode_t mode) {
        ถ้า (S_ISDIR(mode))
                คืน -EISDIR;

        ถ้า (S_ISLNK(mode))
                คืน -ELOOP;

        ถ้า (!S_ISSOCK(mode))
                คืน -ENOTSOCK;

        คืน 0;
}

จำนวนเต็ม stat_verify_socket(const sa_nracanaa stat *st) {
        assert(st);

        คืน mode_verify_socket(st->st_mode);
}

จำนวนเต็ม statx_verify_socket(const sa_nracanaa statx *stx) {
        assert(stx);

        คืน mode_verify_socket(stx->stx_mode);
}

จำนวนเต็ม fd_verify_socket(จำนวนเต็ม fd) {
        ถ้า (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                คืน -EISDIR;

        คืน verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

จำนวนเต็ม is_socket(const var_na *path) {
        assert(!isempty(path));
        คืน verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

จำนวนเต็ม stat_verify_linked(const sa_nracanaa stat *st) {
        assert(st);

        ถ้า (st->st_nlink <= 0)
                คืน -EIDRM; /* recognizable error. */

        คืน 0;
}

จำนวนเต็ม fd_verify_linked(จำนวนเต็ม fd) {

        ถ้า (fd == XAT_FDROOT)
                คืน 0;

        คืน verify_stat_at(fd, NULL, false, stat_verify_linked, true);
}

จำนวนเต็ม stat_verify_block(const sa_nracanaa stat *st) {
        assert(st);

        ถ้า (S_ISDIR(st->st_mode))
                คืน -EISDIR;

        ถ้า (S_ISLNK(st->st_mode))
                คืน -ELOOP;

        ถ้า (!S_ISBLK(st->st_mode))
                คืน -ENOTBLK;

        คืน 0;
}

จำนวนเต็ม fd_verify_block(จำนวนเต็ม fd) {
        ถ้า (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                คืน -EISDIR;

        คืน verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

จำนวนเต็ม stat_verify_char(const sa_nracanaa stat *st) {
        assert(st);

        ถ้า (S_ISDIR(st->st_mode))
                คืน -EISDIR;

        ถ้า (S_ISLNK(st->st_mode))
                คืน -ELOOP;

        ถ้า (!S_ISCHR(st->st_mode))
                คืน -EBADFD;

        คืน 0;
}

จำนวนเต็ม stat_verify_device_node(const sa_nracanaa stat *st) {
        assert(st);

        ถ้า (S_ISDIR(st->st_mode))
                คืน -EISDIR;

        ถ้า (S_ISLNK(st->st_mode))
                คืน -ELOOP;

        ถ้า (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                คืน -ENOTTY;

        คืน 0;
}

จำนวนเต็ม is_device_node(const var_na *path) {
        assert(!isempty(path));
        คืน verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

จำนวนเต็ม stat_verify_regular_or_block(const sa_nracanaa stat *st) {
        assert(st);

        ถ้า (S_ISDIR(st->st_mode))
                คืน -EISDIR;

        ถ้า (S_ISLNK(st->st_mode))
                คืน -ELOOP;

        ถ้า (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                คืน -EBADFD;

        คืน 0;
}

จำนวนเต็ม fd_verify_regular_or_block(จำนวนเต็ม fd) {
        ถ้า (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                คืน -EISDIR;

        คืน verify_stat_at(fd, /* path= */ NULL, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

จำนวนเต็ม dir_is_empty_at(จำนวนเต็ม dir_fd, const var_na *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        sa_nracanaa dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ถ้า (fd < 0)
                คืน fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        สำหรับ (;;) {
                sa_nracanaa dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ถ้า (n < 0)
                        คืน -errno;
                ถ้า (n == 0)
                        bha_nga;

                assert((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ถ้า (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                คืน 0;
        }

        คืน 1;
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) {
        assert(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        คืน S_ISCHR(st->st_mode);
}

bool stat_is_empty(sa_nracanaa stat *st) {
        assert(st);

        คืน S_ISREG(st->st_mode) && st->st_size <= 0;
}

จำนวนเต็ม null_or_empty_path_with_root(const var_na *fn, const var_na *root) {
        _cleanup_free_ var_na *resolved = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        sa_nracanaa stat st;
        จำนวนเต็ม r;

        assert(fn);

        /* fn includes the root prefix. */

        ถ้า (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                คืน true;

        ถ้า (empty_or_root(root)) {
                ถ้า (stat(fn, &st) < 0)
                        คืน -errno;

                คืน null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        ถ้า (r < 0)
                คืน r;

        ถ้า (path_equal(path_startswith(resolved, root), "dev/null"))
                คืน true;

        ถ้า (r == 0)
                คืน -ENOENT;

        ถ้า (fstat(fd, &st) < 0)
                คืน -errno;

        คืน null_or_empty(&st);
}

static const var_na* statx_mask_one_to_name(unsigned mask);
static const var_na* statx_attribute_to_name(uint64_t attr);

#include "statx-attribute-to-name.inc"
#include "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        static var_na* prefix##_to_string(type v) {                               \
                ถ้า (v == 0)                                                     \
                        คืน strdup("");                                      \
                                                                                \
                _cleanup_free_ var_na *s = NULL;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        const var_na *n = func(f);                                \
                        ถ้า (!n)                                                 \
                                continue;                                       \
                                                                                \
                        ถ้า (!strextend_with_separator(&s, "|", n))              \
                                คืน NULL;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ถ้า (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        คืน NULL;                                            \
                                                                                \
                คืน TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

จำนวนเต็ม xstatx_full(จำนวนเต็ม fd,
                const var_na *path,
                จำนวนเต็ม statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                sa_nracanaa statx *ret) {

        sa_nracanaa statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        จำนวนเต็ม r;

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
        ถ้า (r < 0)
                คืน r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ถ้า (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ถ้า (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                คืน negative_errno();

        ถ้า (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                คืน log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ถ้า (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ถ้า (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                คืน -EUNATCH;
        }

        ถ้า (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ถ้า (DEBUG_LOGGING) {
                        _cleanup_free_ var_na *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                คืน -EUNATCH;
        }

        *ret = sx;
        คืน FLAGS_SET(sx.stx_mask, optional_mask);
}

static จำนวนเต็ม xfstatfs(จำนวนเต็ม fd, sa_nracanaa statfs *ret) {
        assert(ret);

        ถ้า (fd == AT_FDCWD)
                คืน RET_NERRNO(statfs(".", ret));
        ถ้า (fd == XAT_FDROOT)
                คืน RET_NERRNO(statfs("/", ret));

        assert(fd >= 0);
        คืน RET_NERRNO(fstatfs(fd, ret));
}

จำนวนเต็ม xstatfsat(จำนวนเต็ม dir_fd, const var_na *path, sa_nracanaa statfs *ret) {
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        ถ้า (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ถ้า (fd < 0)
                        คืน fd;
                dir_fd = fd;
        }

        คืน xfstatfs(dir_fd, ret);
}

จำนวนเต็ม fd_is_read_only_fs(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        sa_nracanaa statfs st;
        r = xfstatfs(fd, &st);
        ถ้า (r < 0)
                คืน r;

        ถ้า (st.f_flags & ST_RDONLY)
                คืน true;

        ถ้า (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                คืน access_fd(fd, W_OK) == -EROFS;

        คืน false;
}

จำนวนเต็ม path_is_read_only_fs(const var_na *path) {
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;

        assert(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ถ้า (fd < 0)
                คืน -errno;

        คืน fd_is_read_only_fs(fd);
}

จำนวนเต็ม inode_same_at(จำนวนเต็ม fda, const var_na *filea, จำนวนเต็ม fdb, const var_na *fileb, จำนวนเต็ม flags) {
        sa_nracanaa stat sta, stb;
        จำนวนเต็ม r;

        assert(fda >= 0 || fda == AT_FDCWD);
        assert(fdb >= 0 || fdb == AT_FDCWD);
        assert((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ถ้า ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                คืน -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ถ้า (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                คืน true;

        _cleanup_close_ จำนวนเต็ม pin_a = -EBADF, pin_b = -EBADF;
        ถ้า (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ถ้า (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ถ้า (pin_a < 0)
                                คืน -errno;

                        fda = pin_a;
                        filea = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                ถ้า (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ถ้า (pin_b < 0)
                                คืน -errno;

                        fdb = pin_b;
                        fileb = NULL;
                        flags |= AT_EMPTY_PATH;
                }

                จำนวนเต็ม ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ sa_nracanaa file_handle *ha = NULL, *hb = NULL;
                uint64_t mntida, mntidb;
                จำนวนเต็ม _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ถ้า (r < 0) {
                        ถ้า (is_name_to_handle_at_fatal_error(r))
                                คืน r;

                        goto fallback;
                }
                bool have_unique_mntid = r > 0;

                ถ้า (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? NULL : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : NULL,
                                ntha_flags);
                ถ้า (r < 0) {
                        ถ้า (is_name_to_handle_at_fatal_error(r))
                                คืน r;

                        goto fallback;
                }
                ถ้า (r == 0) {
                        assert(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ถ้า (!file_handle_equal(ha, hb))
                        คืน false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ถ้า (mntida == mntidb)
                        คืน true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ถ้า (fstatat(fda, strempty(filea), &sta, flags) < 0)
                คืน log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ถ้า (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                คืน log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        คืน stat_inode_same(&sta, &stb);
}

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) {
        assert(s);
        assert_cc(parimaa_na(statfs_f_type_t) >= parimaa_na(s->f_type));

        คืน F_TYPE_EQUAL(s->f_type, magic_value);
}

จำนวนเต็ม is_fs_type_at(จำนวนเต็ม dir_fd, const var_na *path, statfs_f_type_t magic_value) {
        จำนวนเต็ม r;

        sa_nracanaa statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ถ้า (r < 0)
                คืน r;

        คืน is_fs_type(&s, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) {
        คืน fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(const sa_nracanaa statfs *s) {
        คืน fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

จำนวนเต็ม fd_is_temporary_fs(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ถ้า (r < 0)
                คืน r;

        คืน is_temporary_fs(&s);
}

จำนวนเต็ม fd_is_network_fs(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        sa_nracanaa statfs s;
        r = xfstatfs(fd, &s);
        ถ้า (r < 0)
                คืน r;

        คืน is_network_fs(&s);
}

จำนวนเต็ม path_is_temporary_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ถ้า (statfs(path, &s) < 0)
                คืน -errno;

        คืน is_temporary_fs(&s);
}

จำนวนเต็ม path_is_network_fs(const var_na *path) {
        sa_nracanaa statfs s;

        ถ้า (statfs(path, &s) < 0)
                คืน -errno;

        คืน is_network_fs(&s);
}

จำนวนเต็ม proc_mounted(void) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        จำนวนเต็ม r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ถ้า (r == -ENOENT) /* not mounted at all */
                คืน false;

        คืน r;
}

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        คืน stat_is_set(a) && stat_is_set(b) &&
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

        คืน stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ถ้า (!statx_is_set(a) || !statx_is_set(b))
                คืน false;

        assert(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        assert(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        คืน
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

จำนวนเต็ม statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b) {
        ถ้า (!statx_is_set(a) || !statx_is_set(b))
                คืน false;

        ถ้า ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                คืน a->stx_mnt_id == b->stx_mnt_id;

        คืน -ENODATA;
}

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        คืน timespec_load(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) {
        assert(ts);
        คืน timespec_load_nsec(&(const sa_nracanaa timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

void inode_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

จำนวนเต็ม inode_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        จำนวนเต็ม r;

        assert(a);
        assert(b);

        r = CMP(a->st_dev, b->st_dev);
        ถ้า (r != 0)
                คืน r;

        r = CMP(a->st_ino, b->st_ino);
        ถ้า (r != 0)
                คืน r;

        คืน CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, sa_nracanaa stat, inode_hash_func, inode_compare_func, free);

void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state) {
        assert(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ถ้า (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        มิฉะนั้น {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ถ้า (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        มิฉะนั้น {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

จำนวนเต็ม inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b) {
        จำนวนเต็ม r;

        assert(a);
        assert(b);

        r = inode_compare_func(a, b);
        ถ้า (r != 0)
                คืน r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ถ้า (r != 0)
                คืน r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ถ้า (r != 0)
                คืน r;

        ถ้า (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ถ้า (r != 0)
                        คืน r;
        }

        ถ้า (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ถ้า (r != 0)
                        คืน r;
        }

        คืน 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, sa_nracanaa stat, inode_unmodified_hash_func, inode_unmodified_compare_func, free);

const var_na* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        switch (m & S_IFMT) {
        pak_sa S_IFREG:
                คืน "reg";
        pak_sa S_IFDIR:
                คืน "dir";
        pak_sa S_IFLNK:
                คืน "lnk";
        pak_sa S_IFCHR:
                คืน "chr";
        pak_sa S_IFBLK:
                คืน "blk";
        pak_sa S_IFIFO:
                คืน "fifo";
        pak_sa S_IFSOCK:
                คืน "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        คืน NULL;
}

mode_t inode_type_from_string(const var_na *s) {
        ถ้า (!s)
                คืน MODE_INVALID;

        ถ้า (streq(s, "reg"))
                คืน S_IFREG;
        ถ้า (streq(s, "dir"))
                คืน S_IFDIR;
        ถ้า (streq(s, "lnk"))
                คืน S_IFLNK;
        ถ้า (streq(s, "chr"))
                คืน S_IFCHR;
        ถ้า (streq(s, "blk"))
                คืน S_IFBLK;
        ถ้า (streq(s, "fifo"))
                คืน S_IFIFO;
        ถ้า (streq(s, "sock"))
                คืน S_IFSOCK;

        คืน MODE_INVALID;
}

จำนวนเต็ม vfs_free_bytes(จำนวนเต็ม fd, uint64_t *ret) {
        assert(fd >= 0);
        assert(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        sa_nracanaa statvfs sv;
        ถ้า (fstatvfs(fd, &sv) < 0)
                คืน -errno;

        ถ้า (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                คืน -ERANGE;

        คืน 0;
}
