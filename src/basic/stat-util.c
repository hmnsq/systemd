/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <linux/magic.h>
#અન્તર્ભાવ <sys/statvfs.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "bitfield.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "filesystems.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "hash-funcs.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mountpoint-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "time-util.h"

સ્થિર પૂર્ણાંક verify_stat_at(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                bool follow,
                પૂર્ણાંક (*verify_func)(અચલ સંરચના stat *st),
                bool verify) {

        સંરચના stat st;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(fd));
        પ્રતિજ્ઞા(!isempty(path) || !follow);
        પ્રતિજ્ઞા(verify_func);

        _cleanup_free_ અક્ષર *p = ખાલી;
        r = resolve_xat_fdroot(&fd, &path, &p);
        જો (r < 0)
                પાછા r;

        જો (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                પાછા -errno;

        r = verify_func(&st);
        પાછા verify ? r : r >= 0;
}

સ્થિર પૂર્ણાંક mode_verify_regular(mode_t mode) {
        જો (S_ISDIR(mode))
                પાછા -EISDIR;

        જો (S_ISLNK(mode))
                પાછા -ELOOP;

        જો (!S_ISREG(mode))
                પાછા -EBADFD;

        પાછા 0;
}

પૂર્ણાંક stat_verify_regular(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        પાછા mode_verify_regular(st->st_mode);
}

પૂર્ણાંક statx_verify_regular(અચલ સંરચના statx *stx) {
        પ્રતિજ્ઞા(stx);

        જો (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                પાછા -ENODATA;

        પાછા mode_verify_regular(stx->stx_mode);
}

પૂર્ણાંક verify_regular_at(પૂર્ણાંક fd, અચલ અક્ષર *path, bool follow) {
        પાછા verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

પૂર્ણાંક fd_verify_regular(પૂર્ણાંક fd) {
        જો (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                પાછા -EISDIR;

        પાછા verify_regular_at(fd, /* path= */ ખાલી, /* follow= */ false);
}

સ્થિર પૂર્ણાંક mode_verify_directory(mode_t mode) {
        જો (S_ISLNK(mode))
                પાછા -ELOOP;

        જો (!S_ISDIR(mode))
                પાછા -ENOTDIR;

        પાછા 0;
}

પૂર્ણાંક stat_verify_directory(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        પાછા mode_verify_directory(st->st_mode);
}

પૂર્ણાંક statx_verify_directory(અચલ સંરચના statx *stx) {
        પ્રતિજ્ઞા(stx);

        જો (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                પાછા -ENODATA;

        પાછા mode_verify_directory(stx->stx_mode);
}

પૂર્ણાંક fd_verify_directory(પૂર્ણાંક fd) {
        જો (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                પાછા 0;

        પાછા verify_stat_at(fd, ખાલી, false, stat_verify_directory, true);
}

પૂર્ણાંક is_dir_at(પૂર્ણાંક fd, અચલ અક્ષર *path, bool follow) {
        પાછા verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

પૂર્ણાંક is_dir(અચલ અક્ષર *path, bool follow) {
        પ્રતિજ્ઞા(!isempty(path));
        પાછા is_dir_at(AT_FDCWD, path, follow);
}

પૂર્ણાંક stat_verify_symlink(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        જો (S_ISDIR(st->st_mode))
                પાછા -EISDIR;

        જો (!S_ISLNK(st->st_mode))
                પાછા -ENOLINK;

        પાછા 0;
}

પૂર્ણાંક fd_verify_symlink(પૂર્ણાંક fd) {
        જો (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                પાછા -EISDIR;

        પાછા verify_stat_at(fd, /* path= */ ખાલી, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

પૂર્ણાંક is_symlink(અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(!isempty(path));
        પાછા verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

સ્થિર પૂર્ણાંક mode_verify_socket(mode_t mode) {
        જો (S_ISDIR(mode))
                પાછા -EISDIR;

        જો (S_ISLNK(mode))
                પાછા -ELOOP;

        જો (!S_ISSOCK(mode))
                પાછા -ENOTSOCK;

        પાછા 0;
}

પૂર્ણાંક stat_verify_socket(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        પાછા mode_verify_socket(st->st_mode);
}

પૂર્ણાંક statx_verify_socket(અચલ સંરચના statx *stx) {
        પ્રતિજ્ઞા(stx);

        પાછા mode_verify_socket(stx->stx_mode);
}

પૂર્ણાંક fd_verify_socket(પૂર્ણાંક fd) {
        જો (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                પાછા -EISDIR;

        પાછા verify_stat_at(fd, /* path= */ ખાલી, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

પૂર્ણાંક is_socket(અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(!isempty(path));
        પાછા verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

પૂર્ણાંક stat_verify_linked(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        જો (st->st_nlink <= 0)
                પાછા -EIDRM; /* recognizable error. */

        પાછા 0;
}

પૂર્ણાંક fd_verify_linked(પૂર્ણાંક fd) {

        જો (fd == XAT_FDROOT)
                પાછા 0;

        પાછા verify_stat_at(fd, ખાલી, false, stat_verify_linked, true);
}

પૂર્ણાંક stat_verify_block(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        જો (S_ISDIR(st->st_mode))
                પાછા -EISDIR;

        જો (S_ISLNK(st->st_mode))
                પાછા -ELOOP;

        જો (!S_ISBLK(st->st_mode))
                પાછા -ENOTBLK;

        પાછા 0;
}

પૂર્ણાંક fd_verify_block(પૂર્ણાંક fd) {
        જો (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                પાછા -EISDIR;

        પાછા verify_stat_at(fd, /* path= */ ખાલી, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

પૂર્ણાંક stat_verify_char(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        જો (S_ISDIR(st->st_mode))
                પાછા -EISDIR;

        જો (S_ISLNK(st->st_mode))
                પાછા -ELOOP;

        જો (!S_ISCHR(st->st_mode))
                પાછા -EBADFD;

        પાછા 0;
}

પૂર્ણાંક stat_verify_device_ગાંઠ(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        જો (S_ISDIR(st->st_mode))
                પાછા -EISDIR;

        જો (S_ISLNK(st->st_mode))
                પાછા -ELOOP;

        જો (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                પાછા -ENOTTY;

        પાછા 0;
}

પૂર્ણાંક is_device_ગાંઠ(અચલ અક્ષર *path) {
        પ્રતિજ્ઞા(!isempty(path));
        પાછા verify_stat_at(AT_FDCWD, path, false, stat_verify_device_ગાંઠ, false);
}

પૂર્ણાંક stat_verify_regular_or_block(અચલ સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        જો (S_ISDIR(st->st_mode))
                પાછા -EISDIR;

        જો (S_ISLNK(st->st_mode))
                પાછા -ELOOP;

        જો (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                પાછા -EBADFD;

        પાછા 0;
}

પૂર્ણાંક fd_verify_regular_or_block(પૂર્ણાંક fd) {
        જો (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                પાછા -EISDIR;

        પાછા verify_stat_at(fd, /* path= */ ખાલી, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

પૂર્ણાંક dir_is_empty_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        સંરચના dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        જો (fd < 0)
                પાછા fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        માટે (;;) {
                સંરચના dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                જો (n < 0)
                        પાછા -errno;
                જો (n == 0)
                        તોડો;

                પ્રતિજ્ઞા((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        જો (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                પાછા 0;
        }

        પાછા 1;
}

bool stat_may_be_dev_null(સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        પાછા S_ISCHR(st->st_mode);
}

bool stat_is_empty(સંરચના stat *st) {
        પ્રતિજ્ઞા(st);

        પાછા S_ISREG(st->st_mode) && st->st_size <= 0;
}

પૂર્ણાંક null_or_empty_path_with_મૂળ(અચલ અક્ષર *fn, અચલ અક્ષર *મૂળ) {
        _cleanup_free_ અક્ષર *resolved = ખાલી;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        સંરચના stat st;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fn);

        /* fn includes the root prefix. */

        જો (path_equal(path_startswith(fn, મૂળ ?: "/"), "dev/null"))
                પાછા true;

        જો (empty_or_મૂળ(મૂળ)) {
                જો (stat(fn, &st) < 0)
                        પાછા -errno;

                પાછા null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, મૂળ, CHASE_NONEXISTENT, &resolved, &fd);
        જો (r < 0)
                પાછા r;

        જો (path_equal(path_startswith(resolved, મૂળ), "dev/null"))
                પાછા true;

        જો (r == 0)
                પાછા -ENOENT;

        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        પાછા null_or_empty(&st);
}

સ્થિર અચલ અક્ષર* statx_mask_one_to_name(unsigned mask);
સ્થિર અચલ અક્ષર* statx_attribute_to_name(uint64_t attr);

#અન્તર્ભાવ "statx-attribute-to-name.inc"
#અન્તર્ભાવ "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        સ્થિર અક્ષર* prefix##_to_string(type v) {                               \
                જો (v == 0)                                                     \
                        પાછા strdup("");                                      \
                                                                                \
                _cleanup_free_ અક્ષર *s = ખાલી;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        અચલ અક્ષર *n = func(f);                                \
                        જો (!n)                                                 \
                                ચાલુ;                                       \
                                                                                \
                        જો (!strextend_with_separator(&s, "|", n))              \
                                પાછા ખાલી;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                જો (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        પાછા ખાલી;                                            \
                                                                                \
                પાછા TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

પૂર્ણાંક xstatx_full(પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                સંરચના statx *ret) {

        સંરચના statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        પૂર્ણાંક r;

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

        પ્રતિજ્ઞા(wildcard_fd_is_valid(fd));
        પ્રતિજ્ઞા((mandatory_mask & optional_mask) == 0);
        પ્રતિજ્ઞા(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        પ્રતિજ્ઞા(ret);

        _cleanup_free_ અક્ષર *p = ખાલી;
        r = resolve_xat_fdroot(&fd, &path, &p);
        જો (r < 0)
                પાછા r;

        unsigned request_mask = mandatory_mask|optional_mask;
        જો (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        જો (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                પાછા negative_errno();

        જો (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        જો (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                જો (DEBUG_LOGGING) {
                        _cleanup_free_ અક્ષર *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                પાછા -EUNATCH;
        }

        જો (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                જો (DEBUG_LOGGING) {
                        _cleanup_free_ અક્ષર *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                પાછા -EUNATCH;
        }

        *ret = sx;
        પાછા FLAGS_SET(sx.stx_mask, optional_mask);
}

સ્થિર પૂર્ણાંક xfstatfs(પૂર્ણાંક fd, સંરચના statfs *ret) {
        પ્રતિજ્ઞા(ret);

        જો (fd == AT_FDCWD)
                પાછા RET_NERRNO(statfs(".", ret));
        જો (fd == XAT_FDROOT)
                પાછા RET_NERRNO(statfs("/", ret));

        પ્રતિજ્ઞા(fd >= 0);
        પાછા RET_NERRNO(fstatfs(fd, ret));
}

પૂર્ણાંક xstatfsat(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, સંરચના statfs *ret) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(ret);

        જો (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                જો (fd < 0)
                        પાછા fd;
                dir_fd = fd;
        }

        પાછા xfstatfs(dir_fd, ret);
}

પૂર્ણાંક fd_is_read_only_fs(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        સંરચના statfs st;
        r = xfstatfs(fd, &st);
        જો (r < 0)
                પાછા r;

        જો (st.f_flags & ST_RDONLY)
                પાછા true;

        જો (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                પાછા access_fd(fd, W_OK) == -EROFS;

        પાછા false;
}

પૂર્ણાંક path_is_read_only_fs(અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        જો (fd < 0)
                પાછા -errno;

        પાછા fd_is_read_only_fs(fd);
}

પૂર્ણાંક inode_same_at(પૂર્ણાંક fda, અચલ અક્ષર *filea, પૂર્ણાંક fdb, અચલ અક્ષર *fileb, પૂર્ણાંક flags) {
        સંરચના stat sta, stb;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fda >= 0 || fda == AT_FDCWD);
        પ્રતિજ્ઞા(fdb >= 0 || fdb == AT_FDCWD);
        પ્રતિજ્ઞા((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        જો ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                પાછા -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        જો (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                પાછા true;

        _cleanup_close_ પૂર્ણાંક pin_a = -EBADF, pin_b = -EBADF;
        જો (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                જો (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        જો (pin_a < 0)
                                પાછા -errno;

                        fda = pin_a;
                        filea = ખાલી;
                        flags |= AT_EMPTY_PATH;
                }

                જો (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        જો (pin_b < 0)
                                પાછા -errno;

                        fdb = pin_b;
                        fileb = ખાલી;
                        flags |= AT_EMPTY_PATH;
                }

                પૂર્ણાંક ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ સંરચના file_handle *ha = ખાલી, *hb = ખાલી;
                uint64_t mntida, mntidb;
                પૂર્ણાંક _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                જો (r < 0) {
                        જો (is_name_to_handle_at_fatal_error(r))
                                પાછા r;

                        જાઓ fallback;
                }
                bool have_unique_mntid = r > 0;

                જો (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ખાલી : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ખાલી,
                                ntha_flags);
                જો (r < 0) {
                        જો (is_name_to_handle_at_fatal_error(r))
                                પાછા r;

                        જાઓ fallback;
                }
                જો (r == 0) {
                        પ્રતિજ્ઞા(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                જો (!file_handle_equal(ha, hb))
                        પાછા false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                જો (mntida == mntidb)
                        પાછા true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        જો (fstatat(fda, strempty(filea), &sta, flags) < 0)
                પાછા log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        જો (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                પાછા log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        પાછા stat_inode_same(&sta, &stb);
}

bool is_fs_type(અચલ સંરચના statfs *s, statfs_f_type_t magic_કિંમત) {
        પ્રતિજ્ઞા(s);
        assert_cc(કદ(statfs_f_type_t) >= કદ(s->f_type));

        પાછા F_TYPE_EQUAL(s->f_type, magic_કિંમત);
}

પૂર્ણાંક is_fs_type_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, statfs_f_type_t magic_કિંમત) {
        પૂર્ણાંક r;

        સંરચના statfs s;
        r = xstatfsat(dir_fd, path, &s);
        જો (r < 0)
                પાછા r;

        પાછા is_fs_type(&s, magic_કિંમત);
}

bool is_temporary_fs(અચલ સંરચના statfs *s) {
        પાછા fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(અચલ સંરચના statfs *s) {
        પાછા fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

પૂર્ણાંક fd_is_temporary_fs(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        સંરચના statfs s;
        r = xfstatfs(fd, &s);
        જો (r < 0)
                પાછા r;

        પાછા is_temporary_fs(&s);
}

પૂર્ણાંક fd_is_network_fs(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        સંરચના statfs s;
        r = xfstatfs(fd, &s);
        જો (r < 0)
                પાછા r;

        પાછા is_network_fs(&s);
}

પૂર્ણાંક path_is_temporary_fs(અચલ અક્ષર *path) {
        સંરચના statfs s;

        જો (statfs(path, &s) < 0)
                પાછા -errno;

        પાછા is_temporary_fs(&s);
}

પૂર્ણાંક path_is_network_fs(અચલ અક્ષર *path) {
        સંરચના statfs s;

        જો (statfs(path, &s) < 0)
                પાછા -errno;

        પાછા is_network_fs(&s);
}

પૂર્ણાંક proc_mounted(શૂન્ય) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        પૂર્ણાંક r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        જો (r == -ENOENT) /* not mounted at all */
                પાછા false;

        પાછા r;
}

bool stat_inode_same(અચલ સંરચના stat *a, અચલ સંરચના stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        પાછા stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(અચલ સંરચના stat *a, અચલ સંરચના stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(અચલ સંરચના statx *a, અચલ સંરચના statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        જો (!statx_is_set(a) || !statx_is_set(b))
                પાછા false;

        પ્રતિજ્ઞા(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        પ્રતિજ્ઞા(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        પાછા
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

પૂર્ણાંક statx_mount_same(અચલ સંરચના statx *a, અચલ સંરચના statx *b) {
        જો (!statx_is_set(a) || !statx_is_set(b))
                પાછા false;

        જો ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                પાછા a->stx_mnt_id == b->stx_mnt_id;

        પાછા -ENODATA;
}

usec_t statx_timestamp_load(અચલ સંરચના statx_timestamp *ts) {
        પ્રતિજ્ઞા(ts);
        પાછા timespec_load(&(અચલ સંરચના timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(અચલ સંરચના statx_timestamp *ts) {
        પ્રતિજ્ઞા(ts);
        પાછા timespec_load_nsec(&(અચલ સંરચના timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

શૂન્ય inode_hash_func(અચલ સંરચના stat *q, સંરચના siphash *state) {
        પ્રતિજ્ઞા(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

પૂર્ણાંક inode_compare_func(અચલ સંરચના stat *a, અચલ સંરચના stat *b) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = CMP(a->st_dev, b->st_dev);
        જો (r != 0)
                પાછા r;

        r = CMP(a->st_ino, b->st_ino);
        જો (r != 0)
                પાછા r;

        પાછા CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(inode_hash_ops, સંરચના stat, inode_hash_func, inode_compare_func, મુક્ત);

શૂન્ય inode_unmodified_hash_func(અચલ સંરચના stat *q, સંરચના siphash *state) {
        પ્રતિજ્ઞા(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        જો (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        નહીંતર {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        જો (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        નહીંતર {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

પૂર્ણાંક inode_unmodified_compare_func(અચલ સંરચના stat *a, અચલ સંરચના stat *b) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = inode_compare_func(a, b);
        જો (r != 0)
                પાછા r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        જો (r != 0)
                પાછા r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        જો (r != 0)
                પાછા r;

        જો (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                જો (r != 0)
                        પાછા r;
        }

        જો (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                જો (r != 0)
                        પાછા r;
        }

        પાછા 0;
}

DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(inode_unmodified_hash_ops, સંરચના stat, inode_unmodified_hash_func, inode_unmodified_compare_func, મુક્ત);

અચલ અક્ષર* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        પસંદગી (m & S_IFMT) {
        સ્થિતિ S_IFREG:
                પાછા "reg";
        સ્થિતિ S_IFDIR:
                પાછા "dir";
        સ્થિતિ S_IFLNK:
                પાછા "lnk";
        સ્થિતિ S_IFCHR:
                પાછા "chr";
        સ્થિતિ S_IFBLK:
                પાછા "blk";
        સ્થિતિ S_IFIFO:
                પાછા "fifo";
        સ્થિતિ S_IFSOCK:
                પાછા "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        પાછા ખાલી;
}

mode_t inode_type_from_string(અચલ અક્ષર *s) {
        જો (!s)
                પાછા MODE_INVALID;

        જો (streq(s, "reg"))
                પાછા S_IFREG;
        જો (streq(s, "dir"))
                પાછા S_IFDIR;
        જો (streq(s, "lnk"))
                પાછા S_IFLNK;
        જો (streq(s, "chr"))
                પાછા S_IFCHR;
        જો (streq(s, "blk"))
                પાછા S_IFBLK;
        જો (streq(s, "fifo"))
                પાછા S_IFIFO;
        જો (streq(s, "sock"))
                પાછા S_IFSOCK;

        પાછા MODE_INVALID;
}

પૂર્ણાંક vfs_free_bytes(પૂર્ણાંક fd, uint64_t *ret) {
        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        સંરચના statvfs sv;
        જો (fstatvfs(fd, &sv) < 0)
                પાછા -errno;

        જો (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                પાછા -ERANGE;

        પાછા 0;
}
