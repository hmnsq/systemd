/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <linux/magic.h>
#ضمن <sys/statvfs.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "bitfield.h"
#ضمن "chase.h"
#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "filesystems.h"
#ضمن "fs-util.h"
#ضمن "hash-funcs.h"
#ضمن "log.h"
#ضمن "mountpoint-util.h"
#ضمن "path-util.h"
#ضمن "siphash24.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "time-util.h"

ساكن صحيح verify_stat_at(
                صحيح fd,
                ثابت حرف *path,
                bool follow,
                صحيح (*verify_func)(ثابت بنية stat *st),
                bool verify) {

        بنية stat st;
        صحيح r;

        أكد(wildcard_fd_is_valid(fd));
        أكد(!isempty(path) || !follow);
        أكد(verify_func);

        _cleanup_free_ حرف *p = عدم;
        r = resolve_xat_fdroot(&fd, &path, &p);
        אם (r < 0)
                החזר r;

        אם (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                החזר -errno;

        r = verify_func(&st);
        החזר verify ? r : r >= 0;
}

ساكن صحيح mode_verify_regular(mode_t mode) {
        אם (S_ISDIR(mode))
                החזר -EISDIR;

        אם (S_ISLNK(mode))
                החזר -ELOOP;

        אם (!S_ISREG(mode))
                החזר -EBADFD;

        החזר 0;
}

صحيح stat_verify_regular(ثابت بنية stat *st) {
        أكد(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        החזר mode_verify_regular(st->st_mode);
}

صحيح statx_verify_regular(ثابت بنية statx *stx) {
        أكد(stx);

        אם (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                החזר -ENODATA;

        החזר mode_verify_regular(stx->stx_mode);
}

صحيح verify_regular_at(صحيح fd, ثابت حرف *path, bool follow) {
        החזר verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

صحيح fd_verify_regular(صحيح fd) {
        אם (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                החזר -EISDIR;

        החזר verify_regular_at(fd, /* path= */ عدم, /* follow= */ false);
}

ساكن صحيح mode_verify_directory(mode_t mode) {
        אם (S_ISLNK(mode))
                החזר -ELOOP;

        אם (!S_ISDIR(mode))
                החזר -ENOTDIR;

        החזר 0;
}

صحيح stat_verify_directory(ثابت بنية stat *st) {
        أكد(st);

        החזר mode_verify_directory(st->st_mode);
}

صحيح statx_verify_directory(ثابت بنية statx *stx) {
        أكد(stx);

        אם (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                החזר -ENODATA;

        החזר mode_verify_directory(stx->stx_mode);
}

صحيح fd_verify_directory(صحيح fd) {
        אם (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                החזר 0;

        החזר verify_stat_at(fd, عدم, false, stat_verify_directory, true);
}

صحيح is_dir_at(صحيح fd, ثابت حرف *path, bool follow) {
        החזר verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

صحيح is_dir(ثابت حرف *path, bool follow) {
        أكد(!isempty(path));
        החזר is_dir_at(AT_FDCWD, path, follow);
}

صحيح stat_verify_symlink(ثابت بنية stat *st) {
        أكد(st);

        אם (S_ISDIR(st->st_mode))
                החזר -EISDIR;

        אם (!S_ISLNK(st->st_mode))
                החזר -ENOLINK;

        החזר 0;
}

صحيح fd_verify_symlink(صحيح fd) {
        אם (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                החזר -EISDIR;

        החזר verify_stat_at(fd, /* path= */ عدم, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

صحيح is_symlink(ثابت حرف *path) {
        أكد(!isempty(path));
        החזר verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

ساكن صحيح mode_verify_socket(mode_t mode) {
        אם (S_ISDIR(mode))
                החזר -EISDIR;

        אם (S_ISLNK(mode))
                החזר -ELOOP;

        אם (!S_ISSOCK(mode))
                החזר -ENOTSOCK;

        החזר 0;
}

صحيح stat_verify_socket(ثابت بنية stat *st) {
        أكد(st);

        החזר mode_verify_socket(st->st_mode);
}

صحيح statx_verify_socket(ثابت بنية statx *stx) {
        أكد(stx);

        החזר mode_verify_socket(stx->stx_mode);
}

صحيح fd_verify_socket(صحيح fd) {
        אם (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                החזר -EISDIR;

        החזר verify_stat_at(fd, /* path= */ عدم, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

صحيح is_socket(ثابت حرف *path) {
        أكد(!isempty(path));
        החזר verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

صحيح stat_verify_linked(ثابت بنية stat *st) {
        أكد(st);

        אם (st->st_nlink <= 0)
                החזר -EIDRM; /* recognizable error. */

        החזר 0;
}

صحيح fd_verify_linked(صحيح fd) {

        אם (fd == XAT_FDROOT)
                החזר 0;

        החזר verify_stat_at(fd, عدم, false, stat_verify_linked, true);
}

صحيح stat_verify_block(ثابت بنية stat *st) {
        أكد(st);

        אם (S_ISDIR(st->st_mode))
                החזר -EISDIR;

        אם (S_ISLNK(st->st_mode))
                החזר -ELOOP;

        אם (!S_ISBLK(st->st_mode))
                החזר -ENOTBLK;

        החזר 0;
}

صحيح fd_verify_block(صحيح fd) {
        אם (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                החזר -EISDIR;

        החזר verify_stat_at(fd, /* path= */ عدم, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

صحيح stat_verify_char(ثابت بنية stat *st) {
        أكد(st);

        אם (S_ISDIR(st->st_mode))
                החזר -EISDIR;

        אם (S_ISLNK(st->st_mode))
                החזר -ELOOP;

        אם (!S_ISCHR(st->st_mode))
                החזר -EBADFD;

        החזר 0;
}

صحيح stat_verify_device_node(ثابت بنية stat *st) {
        أكد(st);

        אם (S_ISDIR(st->st_mode))
                החזר -EISDIR;

        אם (S_ISLNK(st->st_mode))
                החזר -ELOOP;

        אם (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                החזר -ENOTTY;

        החזר 0;
}

صحيح is_device_node(ثابت حرف *path) {
        أكد(!isempty(path));
        החזר verify_stat_at(AT_FDCWD, path, false, stat_verify_device_node, false);
}

صحيح stat_verify_regular_or_block(ثابت بنية stat *st) {
        أكد(st);

        אם (S_ISDIR(st->st_mode))
                החזר -EISDIR;

        אם (S_ISLNK(st->st_mode))
                החזר -ELOOP;

        אם (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                החזר -EBADFD;

        החזר 0;
}

صحيح fd_verify_regular_or_block(صحيح fd) {
        אם (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                החזר -EISDIR;

        החזר verify_stat_at(fd, /* path= */ عدم, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

صحيح dir_is_empty_at(صحيح dir_fd, ثابت حرف *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ صحيح fd = -EBADF;
        بنية dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        אם (fd < 0)
                החזר fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        עבור (;;) {
                بنية dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                אם (n < 0)
                        החזר -errno;
                אם (n == 0)
                        اكسر;

                أكد((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        אם (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                החזר 0;
        }

        החזר 1;
}

bool stat_may_be_dev_null(بنية stat *st) {
        أكد(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        החזר S_ISCHR(st->st_mode);
}

bool stat_is_empty(بنية stat *st) {
        أكد(st);

        החזר S_ISREG(st->st_mode) && st->st_size <= 0;
}

صحيح null_or_empty_path_with_root(ثابت حرف *fn, ثابت حرف *root) {
        _cleanup_free_ حرف *resolved = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        بنية stat st;
        صحيح r;

        أكد(fn);

        /* fn includes the root prefix. */

        אם (path_equal(path_startswith(fn, root ?: "/"), "dev/null"))
                החזר true;

        אם (empty_or_root(root)) {
                אם (stat(fn, &st) < 0)
                        החזר -errno;

                החזר null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, root, CHASE_NONEXISTENT, &resolved, &fd);
        אם (r < 0)
                החזר r;

        אם (path_equal(path_startswith(resolved, root), "dev/null"))
                החזר true;

        אם (r == 0)
                החזר -ENOENT;

        אם (fstat(fd, &st) < 0)
                החזר -errno;

        החזר null_or_empty(&st);
}

ساكن ثابت حرف* statx_mask_one_to_name(unsigned mask);
ساكن ثابت حرف* statx_attribute_to_name(uint64_t attr);

#ضمن "statx-attribute-to-name.inc"
#ضمن "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        ساكن حرف* prefix##_to_string(type v) {                               \
                אם (v == 0)                                                     \
                        החזר strdup("");                                      \
                                                                                \
                _cleanup_free_ حرف *s = عدم;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ثابت حرف *n = func(f);                                \
                        אם (!n)                                                 \
                                استمر;                                       \
                                                                                \
                        אם (!strextend_with_separator(&s, "|", n))              \
                                החזר عدم;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                אם (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        החזר عدم;                                            \
                                                                                \
                החזר TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

صحيح xstatx_full(صحيح fd,
                ثابت حرف *path,
                صحيح statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                بنية statx *ret) {

        بنية statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        صحيح r;

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

        أكد(wildcard_fd_is_valid(fd));
        أكد((mandatory_mask & optional_mask) == 0);
        أكد(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        أكد(ret);

        _cleanup_free_ حرف *p = عدم;
        r = resolve_xat_fdroot(&fd, &path, &p);
        אם (r < 0)
                החזר r;

        unsigned request_mask = mandatory_mask|optional_mask;
        אם (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        אם (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                החזר negative_errno();

        אם (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                החזר log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        אם (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                אם (DEBUG_LOGGING) {
                        _cleanup_free_ حرف *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                החזר -EUNATCH;
        }

        אם (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                אם (DEBUG_LOGGING) {
                        _cleanup_free_ حرف *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                החזר -EUNATCH;
        }

        *ret = sx;
        החזר FLAGS_SET(sx.stx_mask, optional_mask);
}

ساكن صحيح xfstatfs(صحيح fd, بنية statfs *ret) {
        أكد(ret);

        אם (fd == AT_FDCWD)
                החזר RET_NERRNO(statfs(".", ret));
        אם (fd == XAT_FDROOT)
                החזר RET_NERRNO(statfs("/", ret));

        أكد(fd >= 0);
        החזר RET_NERRNO(fstatfs(fd, ret));
}

صحيح xstatfsat(صحيح dir_fd, ثابت حرف *path, بنية statfs *ret) {
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        אם (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                אם (fd < 0)
                        החזר fd;
                dir_fd = fd;
        }

        החזר xfstatfs(dir_fd, ret);
}

صحيح fd_is_read_only_fs(صحيح fd) {
        صحيح r;

        بنية statfs st;
        r = xfstatfs(fd, &st);
        אם (r < 0)
                החזר r;

        אם (st.f_flags & ST_RDONLY)
                החזר true;

        אם (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                החזר access_fd(fd, W_OK) == -EROFS;

        החזר false;
}

صحيح path_is_read_only_fs(ثابت حرف *path) {
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        אם (fd < 0)
                החזר -errno;

        החזר fd_is_read_only_fs(fd);
}

صحيح inode_same_at(صحيح fda, ثابت حرف *filea, صحيح fdb, ثابت حرف *fileb, صحيح flags) {
        بنية stat sta, stb;
        صحيح r;

        أكد(fda >= 0 || fda == AT_FDCWD);
        أكد(fdb >= 0 || fdb == AT_FDCWD);
        أكد((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        אם ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                החזר -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        אם (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                החזר true;

        _cleanup_close_ صحيح pin_a = -EBADF, pin_b = -EBADF;
        אם (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                אם (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        אם (pin_a < 0)
                                החזר -errno;

                        fda = pin_a;
                        filea = عدم;
                        flags |= AT_EMPTY_PATH;
                }

                אם (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        אם (pin_b < 0)
                                החזר -errno;

                        fdb = pin_b;
                        fileb = عدم;
                        flags |= AT_EMPTY_PATH;
                }

                صحيح ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ بنية file_handle *ha = عدم, *hb = عدم;
                uint64_t mntida, mntidb;
                صحيح _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                אם (r < 0) {
                        אם (is_name_to_handle_at_fatal_error(r))
                                החזר r;

                        اذهب fallback;
                }
                bool have_unique_mntid = r > 0;

                אם (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? عدم : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : عدم,
                                ntha_flags);
                אם (r < 0) {
                        אם (is_name_to_handle_at_fatal_error(r))
                                החזר r;

                        اذهب fallback;
                }
                אם (r == 0) {
                        أكد(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                אם (!file_handle_equal(ha, hb))
                        החזר false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                אם (mntida == mntidb)
                        החזר true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        אם (fstatat(fda, strempty(filea), &sta, flags) < 0)
                החזר log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        אם (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                החזר log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        החזר stat_inode_same(&sta, &stb);
}

bool is_fs_type(ثابت بنية statfs *s, statfs_f_type_t magic_value) {
        أكد(s);
        assert_cc(حجم(statfs_f_type_t) >= حجم(s->f_type));

        החזר F_TYPE_EQUAL(s->f_type, magic_value);
}

صحيح is_fs_type_at(صحيح dir_fd, ثابت حرف *path, statfs_f_type_t magic_value) {
        صحيح r;

        بنية statfs s;
        r = xstatfsat(dir_fd, path, &s);
        אם (r < 0)
                החזר r;

        החזר is_fs_type(&s, magic_value);
}

bool is_temporary_fs(ثابت بنية statfs *s) {
        החזר fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(ثابت بنية statfs *s) {
        החזר fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

صحيح fd_is_temporary_fs(صحيح fd) {
        صحيح r;

        بنية statfs s;
        r = xfstatfs(fd, &s);
        אם (r < 0)
                החזר r;

        החזר is_temporary_fs(&s);
}

صحيح fd_is_network_fs(صحيح fd) {
        صحيح r;

        بنية statfs s;
        r = xfstatfs(fd, &s);
        אם (r < 0)
                החזר r;

        החזר is_network_fs(&s);
}

صحيح path_is_temporary_fs(ثابت حرف *path) {
        بنية statfs s;

        אם (statfs(path, &s) < 0)
                החזר -errno;

        החזר is_temporary_fs(&s);
}

صحيح path_is_network_fs(ثابت حرف *path) {
        بنية statfs s;

        אם (statfs(path, &s) < 0)
                החזר -errno;

        החזר is_network_fs(&s);
}

صحيح proc_mounted(فراغ) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        صحيح r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        אם (r == -ENOENT) /* not mounted at all */
                החזר false;

        החזר r;
}

bool stat_inode_same(ثابت بنية stat *a, ثابت بنية stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        החזר stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ثابت بنية stat *a, ثابت بنية stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        أكد(a);
        أكد(b);

        החזר stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ثابت بنية statx *a, ثابت بنية statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        אם (!statx_is_set(a) || !statx_is_set(b))
                החזר false;

        أكد(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        أكد(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        החזר
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

صحيح statx_mount_same(ثابت بنية statx *a, ثابت بنية statx *b) {
        אם (!statx_is_set(a) || !statx_is_set(b))
                החזר false;

        אם ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                החזר a->stx_mnt_id == b->stx_mnt_id;

        החזר -ENODATA;
}

usec_t statx_timestamp_load(ثابت بنية statx_timestamp *ts) {
        أكد(ts);
        החזר timespec_load(&(ثابت بنية timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(ثابت بنية statx_timestamp *ts) {
        أكد(ts);
        החזר timespec_load_nsec(&(ثابت بنية timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

فراغ inode_hash_func(ثابت بنية stat *q, بنية siphash *state) {
        أكد(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

صحيح inode_compare_func(ثابت بنية stat *a, ثابت بنية stat *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(a->st_dev, b->st_dev);
        אם (r != 0)
                החזר r;

        r = CMP(a->st_ino, b->st_ino);
        אם (r != 0)
                החזר r;

        החזר CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_hash_ops, بنية stat, inode_hash_func, inode_compare_func, حرر);

فراغ inode_unmodified_hash_func(ثابت بنية stat *q, بنية siphash *state) {
        أكد(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        אם (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        אחרת {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        אם (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        אחרת {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

صحيح inode_unmodified_compare_func(ثابت بنية stat *a, ثابت بنية stat *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = inode_compare_func(a, b);
        אם (r != 0)
                החזר r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        אם (r != 0)
                החזר r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        אם (r != 0)
                החזר r;

        אם (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                אם (r != 0)
                        החזר r;
        }

        אם (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                אם (r != 0)
                        החזר r;
        }

        החזר 0;
}

DEFINE_HASH_OPS_WITH_KEY_DESTRUCTOR(inode_unmodified_hash_ops, بنية stat, inode_unmodified_hash_func, inode_unmodified_compare_func, حرر);

ثابت حرف* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        بدل (m & S_IFMT) {
        حالة S_IFREG:
                החזר "reg";
        حالة S_IFDIR:
                החזר "dir";
        حالة S_IFLNK:
                החזר "lnk";
        حالة S_IFCHR:
                החזר "chr";
        حالة S_IFBLK:
                החזר "blk";
        حالة S_IFIFO:
                החזר "fifo";
        حالة S_IFSOCK:
                החזר "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        החזר عدم;
}

mode_t inode_type_from_string(ثابت حرف *s) {
        אם (!s)
                החזר MODE_INVALID;

        אם (streq(s, "reg"))
                החזר S_IFREG;
        אם (streq(s, "dir"))
                החזר S_IFDIR;
        אם (streq(s, "lnk"))
                החזר S_IFLNK;
        אם (streq(s, "chr"))
                החזר S_IFCHR;
        אם (streq(s, "blk"))
                החזר S_IFBLK;
        אם (streq(s, "fifo"))
                החזר S_IFIFO;
        אם (streq(s, "sock"))
                החזר S_IFSOCK;

        החזר MODE_INVALID;
}

صحيح vfs_free_bytes(صحيح fd, uint64_t *ret) {
        أكد(fd >= 0);
        أكد(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        بنية statvfs sv;
        אם (fstatvfs(fd, &sv) < 0)
                החזר -errno;

        אם (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                החזר -ERANGE;

        החזר 0;
}
