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
        यदि (r < 0)
                प्रत्यागम r;

        यदि (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                प्रत्यागम -errno;

        r = verify_func(&st);
        प्रत्यागम verify ? r : r >= 0;
}

स्थिर पूर्णाङ्क mode_verify_regular(mode_t mode) {
        यदि (S_ISDIR(mode))
                प्रत्यागम -EISDIR;

        यदि (S_ISLNK(mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISREG(mode))
                प्रत्यागम -EBADFD;

        प्रत्यागम 0;
}

पूर्णाङ्क stat_verify_regular(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        प्रत्यागम mode_verify_regular(st->st_mode);
}

पूर्णाङ्क statx_verify_regular(ध्रुव संरचना statx *stx) {
        प्रतिज्ञा(stx);

        यदि (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                प्रत्यागम -ENODATA;

        प्रत्यागम mode_verify_regular(stx->stx_mode);
}

पूर्णाङ्क verify_regular_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, bool follow) {
        प्रत्यागम verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

पूर्णाङ्क fd_verify_regular(पूर्णाङ्क fd) {
        यदि (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                प्रत्यागम -EISDIR;

        प्रत्यागम verify_regular_at(fd, /* path= */ रिक्त, /* follow= */ false);
}

स्थिर पूर्णाङ्क mode_verify_directory(mode_t mode) {
        यदि (S_ISLNK(mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISDIR(mode))
                प्रत्यागम -ENOTDIR;

        प्रत्यागम 0;
}

पूर्णाङ्क stat_verify_directory(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        प्रत्यागम mode_verify_directory(st->st_mode);
}

पूर्णाङ्क statx_verify_directory(ध्रुव संरचना statx *stx) {
        प्रतिज्ञा(stx);

        यदि (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                प्रत्यागम -ENODATA;

        प्रत्यागम mode_verify_directory(stx->stx_mode);
}

पूर्णाङ्क fd_verify_directory(पूर्णाङ्क fd) {
        यदि (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                प्रत्यागम 0;

        प्रत्यागम verify_stat_at(fd, रिक्त, false, stat_verify_directory, true);
}

पूर्णाङ्क is_dir_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, bool follow) {
        प्रत्यागम verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

पूर्णाङ्क is_dir(ध्रुव वर्ण *path, bool follow) {
        प्रतिज्ञा(!isempty(path));
        प्रत्यागम is_dir_at(AT_FDCWD, path, follow);
}

पूर्णाङ्क stat_verify_symlink(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        यदि (S_ISDIR(st->st_mode))
                प्रत्यागम -EISDIR;

        यदि (!S_ISLNK(st->st_mode))
                प्रत्यागम -ENOLINK;

        प्रत्यागम 0;
}

पूर्णाङ्क fd_verify_symlink(पूर्णाङ्क fd) {
        यदि (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                प्रत्यागम -EISDIR;

        प्रत्यागम verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

पूर्णाङ्क is_symlink(ध्रुव वर्ण *path) {
        प्रतिज्ञा(!isempty(path));
        प्रत्यागम verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

स्थिर पूर्णाङ्क mode_verify_socket(mode_t mode) {
        यदि (S_ISDIR(mode))
                प्रत्यागम -EISDIR;

        यदि (S_ISLNK(mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISSOCK(mode))
                प्रत्यागम -ENOTSOCK;

        प्रत्यागम 0;
}

पूर्णाङ्क stat_verify_socket(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        प्रत्यागम mode_verify_socket(st->st_mode);
}

पूर्णाङ्क statx_verify_socket(ध्रुव संरचना statx *stx) {
        प्रतिज्ञा(stx);

        प्रत्यागम mode_verify_socket(stx->stx_mode);
}

पूर्णाङ्क fd_verify_socket(पूर्णाङ्क fd) {
        यदि (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                प्रत्यागम -EISDIR;

        प्रत्यागम verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

पूर्णाङ्क is_socket(ध्रुव वर्ण *path) {
        प्रतिज्ञा(!isempty(path));
        प्रत्यागम verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

पूर्णाङ्क stat_verify_linked(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        यदि (st->st_nlink <= 0)
                प्रत्यागम -EIDRM; /* recognizable error. */

        प्रत्यागम 0;
}

पूर्णाङ्क fd_verify_linked(पूर्णाङ्क fd) {

        यदि (fd == XAT_FDROOT)
                प्रत्यागम 0;

        प्रत्यागम verify_stat_at(fd, रिक्त, false, stat_verify_linked, true);
}

पूर्णाङ्क stat_verify_block(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        यदि (S_ISDIR(st->st_mode))
                प्रत्यागम -EISDIR;

        यदि (S_ISLNK(st->st_mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISBLK(st->st_mode))
                प्रत्यागम -ENOTBLK;

        प्रत्यागम 0;
}

पूर्णाङ्क fd_verify_block(पूर्णाङ्क fd) {
        यदि (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                प्रत्यागम -EISDIR;

        प्रत्यागम verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

पूर्णाङ्क stat_verify_char(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        यदि (S_ISDIR(st->st_mode))
                प्रत्यागम -EISDIR;

        यदि (S_ISLNK(st->st_mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISCHR(st->st_mode))
                प्रत्यागम -EBADFD;

        प्रत्यागम 0;
}

पूर्णाङ्क stat_verify_device_गाँठ(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        यदि (S_ISDIR(st->st_mode))
                प्रत्यागम -EISDIR;

        यदि (S_ISLNK(st->st_mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                प्रत्यागम -ENOTTY;

        प्रत्यागम 0;
}

पूर्णाङ्क is_device_गाँठ(ध्रुव वर्ण *path) {
        प्रतिज्ञा(!isempty(path));
        प्रत्यागम verify_stat_at(AT_FDCWD, path, false, stat_verify_device_गाँठ, false);
}

पूर्णाङ्क stat_verify_regular_or_block(ध्रुव संरचना stat *st) {
        प्रतिज्ञा(st);

        यदि (S_ISDIR(st->st_mode))
                प्रत्यागम -EISDIR;

        यदि (S_ISLNK(st->st_mode))
                प्रत्यागम -ELOOP;

        यदि (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                प्रत्यागम -EBADFD;

        प्रत्यागम 0;
}

पूर्णाङ्क fd_verify_regular_or_block(पूर्णाङ्क fd) {
        यदि (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                प्रत्यागम -EISDIR;

        प्रत्यागम verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

पूर्णाङ्क dir_is_empty_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        संरचना dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        प्रति (;;) {
                संरचना dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                यदि (n < 0)
                        प्रत्यागम -errno;
                यदि (n == 0)
                        भङ्ग;

                प्रतिज्ञा((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        यदि (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                प्रत्यागम 0;
        }

        प्रत्यागम 1;
}

bool stat_may_be_dev_null(संरचना stat *st) {
        प्रतिज्ञा(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        प्रत्यागम S_ISCHR(st->st_mode);
}

bool stat_is_empty(संरचना stat *st) {
        प्रतिज्ञा(st);

        प्रत्यागम S_ISREG(st->st_mode) && st->st_size <= 0;
}

पूर्णाङ्क null_or_empty_path_with_मूल(ध्रुव वर्ण *fn, ध्रुव वर्ण *मूल) {
        _cleanup_free_ वर्ण *resolved = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        संरचना stat st;
        पूर्णाङ्क r;

        प्रतिज्ञा(fn);

        /* fn includes the root prefix. */

        यदि (path_equal(path_startswith(fn, मूल ?: "/"), "dev/null"))
                प्रत्यागम true;

        यदि (empty_or_मूल(मूल)) {
                यदि (stat(fn, &st) < 0)
                        प्रत्यागम -errno;

                प्रत्यागम null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, मूल, CHASE_NONEXISTENT, &resolved, &fd);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (path_equal(path_startswith(resolved, मूल), "dev/null"))
                प्रत्यागम true;

        यदि (r == 0)
                प्रत्यागम -ENOENT;

        यदि (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        प्रत्यागम null_or_empty(&st);
}

स्थिर ध्रुव वर्ण* statx_mask_one_to_name(unsigned mask);
स्थिर ध्रुव वर्ण* statx_attribute_to_name(uint64_t attr);

#अन्तर्भाव "statx-attribute-to-name.inc"
#अन्तर्भाव "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        स्थिर वर्ण* prefix##_to_string(type v) {                               \
                यदि (v == 0)                                                     \
                        प्रत्यागम strdup("");                                      \
                                                                                \
                _cleanup_free_ वर्ण *s = रिक्त;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ध्रुव वर्ण *n = func(f);                                \
                        यदि (!n)                                                 \
                                अनुवर्त;                                       \
                                                                                \
                        यदि (!strextend_with_separator(&s, "|", n))              \
                                प्रत्यागम रिक्त;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                यदि (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        प्रत्यागम रिक्त;                                            \
                                                                                \
                प्रत्यागम TAKE_PTR(s);                                             \
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
        यदि (r < 0)
                प्रत्यागम r;

        unsigned request_mask = mandatory_mask|optional_mask;
        यदि (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        यदि (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                प्रत्यागम negative_errno();

        यदि (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        यदि (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                यदि (DEBUG_LOGGING) {
                        _cleanup_free_ वर्ण *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                प्रत्यागम -EUNATCH;
        }

        यदि (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                यदि (DEBUG_LOGGING) {
                        _cleanup_free_ वर्ण *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                प्रत्यागम -EUNATCH;
        }

        *ret = sx;
        प्रत्यागम FLAGS_SET(sx.stx_mask, optional_mask);
}

स्थिर पूर्णाङ्क xfstatfs(पूर्णाङ्क fd, संरचना statfs *ret) {
        प्रतिज्ञा(ret);

        यदि (fd == AT_FDCWD)
                प्रत्यागम RET_NERRNO(statfs(".", ret));
        यदि (fd == XAT_FDROOT)
                प्रत्यागम RET_NERRNO(statfs("/", ret));

        प्रतिज्ञा(fd >= 0);
        प्रत्यागम RET_NERRNO(fstatfs(fd, ret));
}

पूर्णाङ्क xstatfsat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, संरचना statfs *ret) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(ret);

        यदि (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                यदि (fd < 0)
                        प्रत्यागम fd;
                dir_fd = fd;
        }

        प्रत्यागम xfstatfs(dir_fd, ret);
}

पूर्णाङ्क fd_is_read_only_fs(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        संरचना statfs st;
        r = xfstatfs(fd, &st);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (st.f_flags & ST_RDONLY)
                प्रत्यागम true;

        यदि (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                प्रत्यागम access_fd(fd, W_OK) == -EROFS;

        प्रत्यागम false;
}

पूर्णाङ्क path_is_read_only_fs(ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        यदि (fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम fd_is_read_only_fs(fd);
}

पूर्णाङ्क inode_same_at(पूर्णाङ्क fda, ध्रुव वर्ण *filea, पूर्णाङ्क fdb, ध्रुव वर्ण *fileb, पूर्णाङ्क flags) {
        संरचना stat sta, stb;
        पूर्णाङ्क r;

        प्रतिज्ञा(fda >= 0 || fda == AT_FDCWD);
        प्रतिज्ञा(fdb >= 0 || fdb == AT_FDCWD);
        प्रतिज्ञा((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        यदि ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                प्रत्यागम -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        यदि (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                प्रत्यागम true;

        _cleanup_close_ पूर्णाङ्क pin_a = -EBADF, pin_b = -EBADF;
        यदि (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                यदि (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        यदि (pin_a < 0)
                                प्रत्यागम -errno;

                        fda = pin_a;
                        filea = रिक्त;
                        flags |= AT_EMPTY_PATH;
                }

                यदि (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        यदि (pin_b < 0)
                                प्रत्यागम -errno;

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
                यदि (r < 0) {
                        यदि (is_name_to_handle_at_fatal_error(r))
                                प्रत्यागम r;

                        गच्छ fallback;
                }
                bool have_unique_mntid = r > 0;

                यदि (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? रिक्त : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : रिक्त,
                                ntha_flags);
                यदि (r < 0) {
                        यदि (is_name_to_handle_at_fatal_error(r))
                                प्रत्यागम r;

                        गच्छ fallback;
                }
                यदि (r == 0) {
                        प्रतिज्ञा(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                यदि (!file_handle_equal(ha, hb))
                        प्रत्यागम false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                यदि (mntida == mntidb)
                        प्रत्यागम true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        यदि (fstatat(fda, strempty(filea), &sta, flags) < 0)
                प्रत्यागम log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        यदि (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                प्रत्यागम log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        प्रत्यागम stat_inode_same(&sta, &stb);
}

bool is_fs_type(ध्रुव संरचना statfs *s, statfs_f_type_t magic_मूल्य) {
        प्रतिज्ञा(s);
        assert_cc(परिमाण(statfs_f_type_t) >= परिमाण(s->f_type));

        प्रत्यागम F_TYPE_EQUAL(s->f_type, magic_मूल्य);
}

पूर्णाङ्क is_fs_type_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, statfs_f_type_t magic_मूल्य) {
        पूर्णाङ्क r;

        संरचना statfs s;
        r = xstatfsat(dir_fd, path, &s);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम is_fs_type(&s, magic_मूल्य);
}

bool is_temporary_fs(ध्रुव संरचना statfs *s) {
        प्रत्यागम fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(ध्रुव संरचना statfs *s) {
        प्रत्यागम fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

पूर्णाङ्क fd_is_temporary_fs(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        संरचना statfs s;
        r = xfstatfs(fd, &s);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम is_temporary_fs(&s);
}

पूर्णाङ्क fd_is_network_fs(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        संरचना statfs s;
        r = xfstatfs(fd, &s);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम is_network_fs(&s);
}

पूर्णाङ्क path_is_temporary_fs(ध्रुव वर्ण *path) {
        संरचना statfs s;

        यदि (statfs(path, &s) < 0)
                प्रत्यागम -errno;

        प्रत्यागम is_temporary_fs(&s);
}

पूर्णाङ्क path_is_network_fs(ध्रुव वर्ण *path) {
        संरचना statfs s;

        यदि (statfs(path, &s) < 0)
                प्रत्यागम -errno;

        प्रत्यागम is_network_fs(&s);
}

पूर्णाङ्क proc_mounted(शून्य) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        पूर्णाङ्क r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        यदि (r == -ENOENT) /* not mounted at all */
                प्रत्यागम false;

        प्रत्यागम r;
}

bool stat_inode_same(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        प्रत्यागम stat_is_set(a) && stat_is_set(b) &&
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

        प्रत्यागम stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        यदि (!statx_is_set(a) || !statx_is_set(b))
                प्रत्यागम false;

        प्रतिज्ञा(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        प्रतिज्ञा(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        प्रत्यागम
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

पूर्णाङ्क statx_mount_same(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b) {
        यदि (!statx_is_set(a) || !statx_is_set(b))
                प्रत्यागम false;

        यदि ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                प्रत्यागम a->stx_mnt_id == b->stx_mnt_id;

        प्रत्यागम -ENODATA;
}

usec_t statx_timestamp_load(ध्रुव संरचना statx_timestamp *ts) {
        प्रतिज्ञा(ts);
        प्रत्यागम timespec_load(&(ध्रुव संरचना timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(ध्रुव संरचना statx_timestamp *ts) {
        प्रतिज्ञा(ts);
        प्रत्यागम timespec_load_nsec(&(ध्रुव संरचना timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
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
        यदि (r != 0)
                प्रत्यागम r;

        r = CMP(a->st_ino, b->st_ino);
        यदि (r != 0)
                प्रत्यागम r;

        प्रत्यागम CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(inode_hash_ops, संरचना stat, inode_hash_func, inode_compare_func, मुक्त);

शून्य inode_unmodified_hash_func(ध्रुव संरचना stat *q, संरचना siphash *state) {
        प्रतिज्ञा(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        यदि (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        अन्यथा {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        यदि (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        अन्यथा {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

पूर्णाङ्क inode_unmodified_compare_func(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = inode_compare_func(a, b);
        यदि (r != 0)
                प्रत्यागम r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        यदि (r != 0)
                प्रत्यागम r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        यदि (r != 0)
                प्रत्यागम r;

        यदि (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                यदि (r != 0)
                        प्रत्यागम r;
        }

        यदि (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                यदि (r != 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(inode_unmodified_hash_ops, संरचना stat, inode_unmodified_hash_func, inode_unmodified_compare_func, मुक्त);

ध्रुव वर्ण* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        विकल्प (m & S_IFMT) {
        पक्ष S_IFREG:
                प्रत्यागम "reg";
        पक्ष S_IFDIR:
                प्रत्यागम "dir";
        पक्ष S_IFLNK:
                प्रत्यागम "lnk";
        पक्ष S_IFCHR:
                प्रत्यागम "chr";
        पक्ष S_IFBLK:
                प्रत्यागम "blk";
        पक्ष S_IFIFO:
                प्रत्यागम "fifo";
        पक्ष S_IFSOCK:
                प्रत्यागम "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        प्रत्यागम रिक्त;
}

mode_t inode_type_from_string(ध्रुव वर्ण *s) {
        यदि (!s)
                प्रत्यागम MODE_INVALID;

        यदि (streq(s, "reg"))
                प्रत्यागम S_IFREG;
        यदि (streq(s, "dir"))
                प्रत्यागम S_IFDIR;
        यदि (streq(s, "lnk"))
                प्रत्यागम S_IFLNK;
        यदि (streq(s, "chr"))
                प्रत्यागम S_IFCHR;
        यदि (streq(s, "blk"))
                प्रत्यागम S_IFBLK;
        यदि (streq(s, "fifo"))
                प्रत्यागम S_IFIFO;
        यदि (streq(s, "sock"))
                प्रत्यागम S_IFSOCK;

        प्रत्यागम MODE_INVALID;
}

पूर्णाङ्क vfs_free_bytes(पूर्णाङ्क fd, uint64_t *ret) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        संरचना statvfs sv;
        यदि (fstatvfs(fd, &sv) < 0)
                प्रत्यागम -errno;

        यदि (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                प्रत्यागम -ERANGE;

        प्रत्यागम 0;
}
