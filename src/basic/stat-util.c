/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <linux/magic.h>
#अन्तर्भाव <sys/statvfs.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bitfield.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "filesystems.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "hash-funcs.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mountpoint-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "time-util.h"

स्थिर पूर्णाङ्क verify_stat_at(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                bool follow,
                पूर्णाङ्क (*verify_func)(ध्रुव संरचना stat *st),
                bool verify) {

        संरचना stat st;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(fd));
        प्रतिज्ञा(!isempty(path) || !follow);
        प्रतिज्ञा(verify_func);

        _cleanup_free_ वर्ण *p = रिक्त;
        r = resolve_xat_fdroot(&fd, &path, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                पटिनिवत्त -errno;

        r = verify_func(&st);
        पटिनिवत्त verify ? r : r >= 0;
}

स्थिर पूर्णाङ्क mode_verify_regular(mode_t mode) {
        सचे (S_ISDIR(mode))
                पटिनिवत्त -EISDIR;

        सचे (S_ISLNK(mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISREG(mode))
                पटिनिवत्त -EBADFD;

        पटिनिवत्त 0;
}

पूर्णाङ्क stat_verify_regular(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        पटिनिवत्त mode_verify_regular(st->st_mode);
}

पूर्णाङ्क statx_verify_regular(ध्रुव संरचना statx *stx) {
        प्रतिज्ञा(stx);

        सचे (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                पटिनिवत्त -ENODATA;

        पटिनिवत्त mode_verify_regular(stx->stx_mode);
}

पूर्णाङ्क verify_regular_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, bool follow) {
        पटिनिवत्त verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

पूर्णाङ्क fd_verify_regular(पूर्णाङ्क fd) {
        सचे (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                पटिनिवत्त -EISDIR;

        पटिनिवत्त verify_regular_at(fd, /* path= */ रिक्त, /* follow= */ false);
}

स्थिर पूर्णाङ्क mode_verify_directory(mode_t mode) {
        सचे (S_ISLNK(mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISDIR(mode))
                पटिनिवत्त -ENOTDIR;

        पटिनिवत्त 0;
}

पूर्णाङ्क stat_verify_directory(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        पटिनिवत्त mode_verify_directory(st->st_mode);
}

पूर्णाङ्क statx_verify_directory(ध्रुव संरचना statx *stx) {
        प्रतिज्ञा(stx);

        सचे (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                पटिनिवत्त -ENODATA;

        पटिनिवत्त mode_verify_directory(stx->stx_mode);
}

पूर्णाङ्क fd_verify_directory(पूर्णाङ्क fd) {
        सचे (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                पटिनिवत्त 0;

        पटिनिवत्त verify_stat_at(fd, रिक्त, false, stat_verify_directory, true);
}

पूर्णाङ्क is_dir_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, bool follow) {
        पटिनिवत्त verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

पूर्णाङ्क is_dir(ध्रुव वर्ण *path, bool follow) {
        प्रतिज्ञा(!isempty(path));
        पटिनिवत्त is_dir_at(AT_FDCWD, path, follow);
}

पूर्णाङ्क stat_verify_symlink(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        सचे (S_ISDIR(st->st_mode))
                पटिनिवत्त -EISDIR;

        सचे (!S_ISLNK(st->st_mode))
                पटिनिवत्त -ENOLINK;

        पटिनिवत्त 0;
}

पूर्णाङ्क fd_verify_symlink(पूर्णाङ्क fd) {
        सचे (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                पटिनिवत्त -EISDIR;

        पटिनिवत्त verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

पूर्णाङ्क is_symlink(ध्रुव वर्ण *path) {
        प्रतिज्ञा(!isempty(path));
        पटिनिवत्त verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

स्थिर पूर्णाङ्क mode_verify_socket(mode_t mode) {
        सचे (S_ISDIR(mode))
                पटिनिवत्त -EISDIR;

        सचे (S_ISLNK(mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISSOCK(mode))
                पटिनिवत्त -ENOTSOCK;

        पटिनिवत्त 0;
}

पूर्णाङ्क stat_verify_socket(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        पटिनिवत्त mode_verify_socket(st->st_mode);
}

पूर्णाङ्क statx_verify_socket(ध्रुव संरचना statx *stx) {
        प्रतिज्ञा(stx);

        पटिनिवत्त mode_verify_socket(stx->stx_mode);
}

पूर्णाङ्क fd_verify_socket(पूर्णाङ्क fd) {
        सचे (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                पटिनिवत्त -EISDIR;

        पटिनिवत्त verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

पूर्णाङ्क is_socket(ध्रुव वर्ण *path) {
        प्रतिज्ञा(!isempty(path));
        पटिनिवत्त verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

पूर्णाङ्क stat_verify_linked(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        सचे (st->st_nlink <= 0)
                पटिनिवत्त -EIDRM; /* recognizable error. */

        पटिनिवत्त 0;
}

पूर्णाङ्क fd_verify_linked(पूर्णाङ्क fd) {

        सचे (fd == XAT_FDROOT)
                पटिनिवत्त 0;

        पटिनिवत्त verify_stat_at(fd, रिक्त, false, stat_verify_linked, true);
}

पूर्णाङ्क stat_verify_block(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        सचे (S_ISDIR(st->st_mode))
                पटिनिवत्त -EISDIR;

        सचे (S_ISLNK(st->st_mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISBLK(st->st_mode))
                पटिनिवत्त -ENOTBLK;

        पटिनिवत्त 0;
}

पूर्णाङ्क fd_verify_block(पूर्णाङ्क fd) {
        सचे (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                पटिनिवत्त -EISDIR;

        पटिनिवत्त verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

पूर्णाङ्क stat_verify_char(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        सचे (S_ISDIR(st->st_mode))
                पटिनिवत्त -EISDIR;

        सचे (S_ISLNK(st->st_mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISCHR(st->st_mode))
                पटिनिवत्त -EBADFD;

        पटिनिवत्त 0;
}

पूर्णाङ्क stat_verify_device_ग्रन्थिः(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        सचे (S_ISDIR(st->st_mode))
                पटिनिवत्त -EISDIR;

        सचे (S_ISLNK(st->st_mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                पटिनिवत्त -ENOTTY;

        पटिनिवत्त 0;
}

पूर्णाङ्क is_device_ग्रन्थिः(ध्रुव वर्ण *path) {
        प्रतिज्ञा(!isempty(path));
        पटिनिवत्त verify_stat_at(AT_FDCWD, path, false, stat_verify_device_ग्रन्थिः, false);
}

पूर्णाङ्क stat_verify_regular_or_block(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        सचे (S_ISDIR(st->st_mode))
                पटिनिवत्त -EISDIR;

        सचे (S_ISLNK(st->st_mode))
                पटिनिवत्त -ELOOP;

        सचे (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                पटिनिवत्त -EBADFD;

        पटिनिवत्त 0;
}

पूर्णाङ्क fd_verify_regular_or_block(पूर्णाङ्क fd) {
        सचे (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                पटिनिवत्त -EISDIR;

        पटिनिवत्त verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

पूर्णाङ्क dir_is_empty_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        संरचना dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        सचे (fd < 0)
                पटिनिवत्त fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        अत्थाय (;;) {
                संरचना dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                सचे (n < 0)
                        पटिनिवत्त -errno;
                सचे (n == 0)
                        भङ्ग;

                प्रतिज्ञा((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        सचे (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                पटिनिवत्त 0;
        }

        पटिनिवत्त 1;
}

bool stat_may_be_dev_null(संरचना stat *st) {
        प्रतिज्ञा(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        पटिनिवत्त S_ISCHR(st->st_mode);
}

bool stat_is_empty(संरचना stat *st) {
        प्रतिज्ञा(st);

        पटिनिवत्त S_ISREG(st->st_mode) && st->st_size <= 0;
}

पूर्णाङ्क null_or_empty_path_with_मूलम्(ध्रुव वर्ण *fn, ध्रुव वर्ण *मूलम्) {
        _cleanup_free_ वर्ण *resolved = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        संरचना stat st;
        पूर्णाङ्क r;

        प्रतिज्ञा(fn);

        /* fn includes the root prefix. */

        सचे (path_equal(path_startswith(fn, मूलम् ?: "/"), "dev/null"))
                पटिनिवत्त true;

        सचे (empty_or_मूलम्(मूलम्)) {
                सचे (stat(fn, &st) < 0)
                        पटिनिवत्त -errno;

                पटिनिवत्त null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, मूलम्, CHASE_NONEXISTENT, &resolved, &fd);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (path_equal(path_startswith(resolved, मूलम्), "dev/null"))
                पटिनिवत्त true;

        सचे (r == 0)
                पटिनिवत्त -ENOENT;

        सचे (fstat(fd, &st) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त null_or_empty(&st);
}

स्थिर ध्रुव वर्ण* statx_mask_one_to_name(unsigned mask);
स्थिर ध्रुव वर्ण* statx_attribute_to_name(uint64_t attr);

#अन्तर्भाव "statx-attribute-to-name.inc"
#अन्तर्भाव "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        स्थिर वर्ण* prefix##_to_string(type v) {                               \
                सचे (v == 0)                                                     \
                        पटिनिवत्त strdup("");                                      \
                                                                                \
                _cleanup_free_ वर्ण *s = रिक्त;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ध्रुव वर्ण *n = func(f);                                \
                        सचे (!n)                                                 \
                                अनुवर्त;                                       \
                                                                                \
                        सचे (!strextend_with_separator(&s, "|", n))              \
                                पटिनिवत्त रिक्त;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                सचे (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        पटिनिवत्त रिक्त;                                            \
                                                                                \
                पटिनिवत्त TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

पूर्णाङ्क xstatx_full(पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                संरचना statx *ret) {

        संरचना statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        पूर्णाङ्क r;

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

        प्रतिज्ञा(wildcard_fd_is_valid(fd));
        प्रतिज्ञा((mandatory_mask & optional_mask) == 0);
        प्रतिज्ञा(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        प्रतिज्ञा(ret);

        _cleanup_free_ वर्ण *p = रिक्त;
        r = resolve_xat_fdroot(&fd, &path, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        unsigned request_mask = mandatory_mask|optional_mask;
        सचे (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        सचे (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                पटिनिवत्त negative_errno();

        सचे (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        सचे (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                सचे (DEBUG_LOGGING) {
                        _cleanup_free_ वर्ण *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                पटिनिवत्त -EUNATCH;
        }

        सचे (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                सचे (DEBUG_LOGGING) {
                        _cleanup_free_ वर्ण *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                पटिनिवत्त -EUNATCH;
        }

        *ret = sx;
        पटिनिवत्त FLAGS_SET(sx.stx_mask, optional_mask);
}

स्थिर पूर्णाङ्क xfstatfs(पूर्णाङ्क fd, संरचना statfs *ret) {
        प्रतिज्ञा(ret);

        सचे (fd == AT_FDCWD)
                पटिनिवत्त RET_NERRNO(statfs(".", ret));
        सचे (fd == XAT_FDROOT)
                पटिनिवत्त RET_NERRNO(statfs("/", ret));

        प्रतिज्ञा(fd >= 0);
        पटिनिवत्त RET_NERRNO(fstatfs(fd, ret));
}

पूर्णाङ्क xstatfsat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, संरचना statfs *ret) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(ret);

        सचे (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                सचे (fd < 0)
                        पटिनिवत्त fd;
                dir_fd = fd;
        }

        पटिनिवत्त xfstatfs(dir_fd, ret);
}

पूर्णाङ्क fd_is_read_only_fs(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        संरचना statfs st;
        r = xfstatfs(fd, &st);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (st.f_flags & ST_RDONLY)
                पटिनिवत्त true;

        सचे (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                पटिनिवत्त access_fd(fd, W_OK) == -EROFS;

        पटिनिवत्त false;
}

पूर्णाङ्क path_is_read_only_fs(ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त fd_is_read_only_fs(fd);
}

पूर्णाङ्क inode_same_at(पूर्णाङ्क fda, ध्रुव वर्ण *filea, पूर्णाङ्क fdb, ध्रुव वर्ण *fileb, पूर्णाङ्क flags) {
        संरचना stat sta, stb;
        पूर्णाङ्क r;

        प्रतिज्ञा(fda >= 0 || fda == AT_FDCWD);
        प्रतिज्ञा(fdb >= 0 || fdb == AT_FDCWD);
        प्रतिज्ञा((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        सचे ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                पटिनिवत्त -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        सचे (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                पटिनिवत्त true;

        _cleanup_close_ पूर्णाङ्क pin_a = -EBADF, pin_b = -EBADF;
        सचे (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                सचे (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        सचे (pin_a < 0)
                                पटिनिवत्त -errno;

                        fda = pin_a;
                        filea = रिक्त;
                        flags |= AT_EMPTY_PATH;
                }

                सचे (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        सचे (pin_b < 0)
                                पटिनिवत्त -errno;

                        fdb = pin_b;
                        fileb = रिक्त;
                        flags |= AT_EMPTY_PATH;
                }

                पूर्णाङ्क ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ संरचना file_handle *ha = रिक्त, *hb = रिक्त;
                uint64_t mntida, mntidb;
                पूर्णाङ्क _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                सचे (r < 0) {
                        सचे (is_name_to_handle_at_fatal_error(r))
                                पटिनिवत्त r;

                        गच्छ fallback;
                }
                bool have_unique_mntid = r > 0;

                सचे (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? रिक्त : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : रिक्त,
                                ntha_flags);
                सचे (r < 0) {
                        सचे (is_name_to_handle_at_fatal_error(r))
                                पटिनिवत्त r;

                        गच्छ fallback;
                }
                सचे (r == 0) {
                        प्रतिज्ञा(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                सचे (!file_handle_equal(ha, hb))
                        पटिनिवत्त false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                सचे (mntida == mntidb)
                        पटिनिवत्त true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        सचे (fstatat(fda, strempty(filea), &sta, flags) < 0)
                पटिनिवत्त log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        सचे (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                पटिनिवत्त log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        पटिनिवत्त stat_inode_same(&sta, &stb);
}

bool is_fs_type(ध्रुव संरचना statfs *s, statfs_f_type_t magic_मानम्) {
        प्रतिज्ञा(s);
        assert_cc(परिमाण(statfs_f_type_t) >= परिमाण(s->f_type));

        पटिनिवत्त F_TYPE_EQUAL(s->f_type, magic_मानम्);
}

पूर्णाङ्क is_fs_type_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, statfs_f_type_t magic_मानम्) {
        पूर्णाङ्क r;

        संरचना statfs s;
        r = xstatfsat(dir_fd, path, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त is_fs_type(&s, magic_मानम्);
}

bool is_temporary_fs(ध्रुव संरचना statfs *s) {
        पटिनिवत्त fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(ध्रुव संरचना statfs *s) {
        पटिनिवत्त fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

पूर्णाङ्क fd_is_temporary_fs(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        संरचना statfs s;
        r = xfstatfs(fd, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त is_temporary_fs(&s);
}

पूर्णाङ्क fd_is_network_fs(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        संरचना statfs s;
        r = xfstatfs(fd, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त is_network_fs(&s);
}

पूर्णाङ्क path_is_temporary_fs(ध्रुव वर्ण *path) {
        संरचना statfs s;

        सचे (statfs(path, &s) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त is_temporary_fs(&s);
}

पूर्णाङ्क path_is_network_fs(ध्रुव वर्ण *path) {
        संरचना statfs s;

        सचे (statfs(path, &s) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त is_network_fs(&s);
}

पूर्णाङ्क proc_mounted(शून्य) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        पूर्णाङ्क r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        सचे (r == -ENOENT) /* not mounted at all */
                पटिनिवत्त false;

        पटिनिवत्त r;
}

bool stat_inode_same(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        पटिनिवत्त stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        सचे (!statx_is_set(a) || !statx_is_set(b))
                पटिनिवत्त false;

        प्रतिज्ञा(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        प्रतिज्ञा(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        पटिनिवत्त
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

पूर्णाङ्क statx_mount_same(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b) {
        सचे (!statx_is_set(a) || !statx_is_set(b))
                पटिनिवत्त false;

        सचे ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                पटिनिवत्त a->stx_mnt_id == b->stx_mnt_id;

        पटिनिवत्त -ENODATA;
}

usec_t statx_timestamp_load(ध्रुव संरचना statx_timestamp *ts) {
        प्रतिज्ञा(ts);
        पटिनिवत्त timespec_load(&(ध्रुव संरचना timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(ध्रुव संरचना statx_timestamp *ts) {
        प्रतिज्ञा(ts);
        पटिनिवत्त timespec_load_nsec(&(ध्रुव संरचना timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

शून्य inode_hash_func(ध्रुव संरचना stat *q, संरचना siphash *state) {
        प्रतिज्ञा(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

पूर्णाङ्क inode_compare_func(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->st_dev, b->st_dev);
        सचे (r != 0)
                पटिनिवत्त r;

        r = CMP(a->st_ino, b->st_ino);
        सचे (r != 0)
                पटिनिवत्त r;

        पटिनिवत्त CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_कुञ्चिका_DESTRUCTOR(inode_hash_ops, संरचना stat, inode_hash_func, inode_compare_func, मुक्त);

शून्य inode_unmodified_hash_func(ध्रुव संरचना stat *q, संरचना siphash *state) {
        प्रतिज्ञा(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        सचे (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        अञ्ञथा {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        सचे (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        अञ्ञथा {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

पूर्णाङ्क inode_unmodified_compare_func(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = inode_compare_func(a, b);
        सचे (r != 0)
                पटिनिवत्त r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        सचे (r != 0)
                पटिनिवत्त r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        सचे (r != 0)
                पटिनिवत्त r;

        सचे (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                सचे (r != 0)
                        पटिनिवत्त r;
        }

        सचे (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                सचे (r != 0)
                        पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

DEFINE_HASH_OPS_WITH_कुञ्चिका_DESTRUCTOR(inode_unmodified_hash_ops, संरचना stat, inode_unmodified_hash_func, inode_unmodified_compare_func, मुक्त);

ध्रुव वर्ण* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        विकल्प (m & S_IFMT) {
        पक्ष S_IFREG:
                पटिनिवत्त "reg";
        पक्ष S_IFDIR:
                पटिनिवत्त "dir";
        पक्ष S_IFLNK:
                पटिनिवत्त "lnk";
        पक्ष S_IFCHR:
                पटिनिवत्त "chr";
        पक्ष S_IFBLK:
                पटिनिवत्त "blk";
        पक्ष S_IFIFO:
                पटिनिवत्त "fifo";
        पक्ष S_IFSOCK:
                पटिनिवत्त "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        पटिनिवत्त रिक्त;
}

mode_t inode_type_from_string(ध्रुव वर्ण *s) {
        सचे (!s)
                पटिनिवत्त MODE_INVALID;

        सचे (streq(s, "reg"))
                पटिनिवत्त S_IFREG;
        सचे (streq(s, "dir"))
                पटिनिवत्त S_IFDIR;
        सचे (streq(s, "lnk"))
                पटिनिवत्त S_IFLNK;
        सचे (streq(s, "chr"))
                पटिनिवत्त S_IFCHR;
        सचे (streq(s, "blk"))
                पटिनिवत्त S_IFBLK;
        सचे (streq(s, "fifo"))
                पटिनिवत्त S_IFIFO;
        सचे (streq(s, "sock"))
                पटिनिवत्त S_IFSOCK;

        पटिनिवत्त MODE_INVALID;
}

पूर्णाङ्क vfs_free_bytes(पूर्णाङ्क fd, uint64_t *ret) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        संरचना statvfs sv;
        सचे (fstatvfs(fd, &sv) < 0)
                पटिनिवत्त -errno;

        सचे (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                पटिनिवत्त -ERANGE;

        पटिनिवत्त 0;
}
