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

स्थिर पूर्णांक verify_stat_at(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                bool follow,
                पूर्णांक (*verify_func)(स्थिरांक संरचना stat *st),
                bool verify) {

        संरचना stat st;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(fd));
        प्रतिज्ञा(!isempty(path) || !follow);
        प्रतिज्ञा(verify_func);

        _cleanup_free_ अक्षर *p = रिक्त;
        r = resolve_xat_fdroot(&fd, &path, &p);
        जर (r < 0)
                परत r;

        जर (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                परत -त्रुटि_सं;

        r = verify_func(&st);
        परत verify ? r : r >= 0;
}

स्थिर पूर्णांक mode_verify_regular(mode_t mode) {
        जर (S_ISDIR(mode))
                परत -EISDIR;

        जर (S_ISLNK(mode))
                परत -ELOOP;

        जर (!S_ISREG(mode))
                परत -EBADFD;

        परत 0;
}

पूर्णांक stat_verify_regular(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        परत mode_verify_regular(st->st_mode);
}

पूर्णांक statx_verify_regular(स्थिरांक संरचना statx *stx) {
        प्रतिज्ञा(stx);

        जर (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                परत -ENODATA;

        परत mode_verify_regular(stx->stx_mode);
}

पूर्णांक verify_regular_at(पूर्णांक fd, स्थिरांक अक्षर *path, bool follow) {
        परत verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

पूर्णांक fd_verify_regular(पूर्णांक fd) {
        जर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                परत -EISDIR;

        परत verify_regular_at(fd, /* path= */ रिक्त, /* follow= */ false);
}

स्थिर पूर्णांक mode_verify_directory(mode_t mode) {
        जर (S_ISLNK(mode))
                परत -ELOOP;

        जर (!S_ISDIR(mode))
                परत -ENOTDIR;

        परत 0;
}

पूर्णांक stat_verify_directory(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        परत mode_verify_directory(st->st_mode);
}

पूर्णांक statx_verify_directory(स्थिरांक संरचना statx *stx) {
        प्रतिज्ञा(stx);

        जर (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                परत -ENODATA;

        परत mode_verify_directory(stx->stx_mode);
}

पूर्णांक fd_verify_directory(पूर्णांक fd) {
        जर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                परत 0;

        परत verify_stat_at(fd, रिक्त, false, stat_verify_directory, true);
}

पूर्णांक is_dir_at(पूर्णांक fd, स्थिरांक अक्षर *path, bool follow) {
        परत verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

पूर्णांक is_dir(स्थिरांक अक्षर *path, bool follow) {
        प्रतिज्ञा(!isempty(path));
        परत is_dir_at(AT_FDCWD, path, follow);
}

पूर्णांक stat_verify_symlink(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        जर (S_ISDIR(st->st_mode))
                परत -EISDIR;

        जर (!S_ISLNK(st->st_mode))
                परत -ENOLINK;

        परत 0;
}

पूर्णांक fd_verify_symlink(पूर्णांक fd) {
        जर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                परत -EISDIR;

        परत verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

पूर्णांक is_symlink(स्थिरांक अक्षर *path) {
        प्रतिज्ञा(!isempty(path));
        परत verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

स्थिर पूर्णांक mode_verify_socket(mode_t mode) {
        जर (S_ISDIR(mode))
                परत -EISDIR;

        जर (S_ISLNK(mode))
                परत -ELOOP;

        जर (!S_ISSOCK(mode))
                परत -ENOTSOCK;

        परत 0;
}

पूर्णांक stat_verify_socket(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        परत mode_verify_socket(st->st_mode);
}

पूर्णांक statx_verify_socket(स्थिरांक संरचना statx *stx) {
        प्रतिज्ञा(stx);

        परत mode_verify_socket(stx->stx_mode);
}

पूर्णांक fd_verify_socket(पूर्णांक fd) {
        जर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                परत -EISDIR;

        परत verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

पूर्णांक is_socket(स्थिरांक अक्षर *path) {
        प्रतिज्ञा(!isempty(path));
        परत verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

पूर्णांक stat_verify_linked(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        जर (st->st_nlink <= 0)
                परत -EIDRM; /* recognizable error. */

        परत 0;
}

पूर्णांक fd_verify_linked(पूर्णांक fd) {

        जर (fd == XAT_FDROOT)
                परत 0;

        परत verify_stat_at(fd, रिक्त, false, stat_verify_linked, true);
}

पूर्णांक stat_verify_block(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        जर (S_ISDIR(st->st_mode))
                परत -EISDIR;

        जर (S_ISLNK(st->st_mode))
                परत -ELOOP;

        जर (!S_ISBLK(st->st_mode))
                परत -ENOTBLK;

        परत 0;
}

पूर्णांक fd_verify_block(पूर्णांक fd) {
        जर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                परत -EISDIR;

        परत verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

पूर्णांक stat_verify_char(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        जर (S_ISDIR(st->st_mode))
                परत -EISDIR;

        जर (S_ISLNK(st->st_mode))
                परत -ELOOP;

        जर (!S_ISCHR(st->st_mode))
                परत -EBADFD;

        परत 0;
}

पूर्णांक stat_verify_device_गाठ(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        जर (S_ISDIR(st->st_mode))
                परत -EISDIR;

        जर (S_ISLNK(st->st_mode))
                परत -ELOOP;

        जर (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                परत -ENOTTY;

        परत 0;
}

पूर्णांक is_device_गाठ(स्थिरांक अक्षर *path) {
        प्रतिज्ञा(!isempty(path));
        परत verify_stat_at(AT_FDCWD, path, false, stat_verify_device_गाठ, false);
}

पूर्णांक stat_verify_regular_or_block(स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(st);

        जर (S_ISDIR(st->st_mode))
                परत -EISDIR;

        जर (S_ISLNK(st->st_mode))
                परत -ELOOP;

        जर (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                परत -EBADFD;

        परत 0;
}

पूर्णांक fd_verify_regular_or_block(पूर्णांक fd) {
        जर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                परत -EISDIR;

        परत verify_stat_at(fd, /* path= */ रिक्त, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

पूर्णांक dir_is_empty_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        संरचना dirent *buf;
        माप_प्रकार m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        जर (fd < 0)
                परत fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        साठी (;;) {
                संरचना dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                जर (n < 0)
                        परत -त्रुटि_सं;
                जर (n == 0)
                        थांबा;

                प्रतिज्ञा((माप_प्रकार) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        जर (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                परत 0;
        }

        परत 1;
}

bool stat_may_be_dev_null(संरचना stat *st) {
        प्रतिज्ञा(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        परत S_ISCHR(st->st_mode);
}

bool stat_is_empty(संरचना stat *st) {
        प्रतिज्ञा(st);

        परत S_ISREG(st->st_mode) && st->st_size <= 0;
}

पूर्णांक null_or_empty_path_with_मूळ(स्थिरांक अक्षर *fn, स्थिरांक अक्षर *मूळ) {
        _cleanup_free_ अक्षर *resolved = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        संरचना stat st;
        पूर्णांक r;

        प्रतिज्ञा(fn);

        /* fn includes the root prefix. */

        जर (path_equal(path_startswith(fn, मूळ ?: "/"), "dev/null"))
                परत true;

        जर (empty_or_मूळ(मूळ)) {
                जर (stat(fn, &st) < 0)
                        परत -त्रुटि_सं;

                परत null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, मूळ, CHASE_NONEXISTENT, &resolved, &fd);
        जर (r < 0)
                परत r;

        जर (path_equal(path_startswith(resolved, मूळ), "dev/null"))
                परत true;

        जर (r == 0)
                परत -ENOENT;

        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        परत null_or_empty(&st);
}

स्थिर स्थिरांक अक्षर* statx_mask_one_to_name(अचिन्हित mask);
स्थिर स्थिरांक अक्षर* statx_attribute_to_name(uint64_t attr);

#अन्तर्भाव "statx-attribute-to-name.inc"
#अन्तर्भाव "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        स्थिर अक्षर* prefix##_to_string(type v) {                               \
                जर (v == 0)                                                     \
                        परत strdup("");                                      \
                                                                                \
                _cleanup_free_ अक्षर *s = रिक्त;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        स्थिरांक अक्षर *n = func(f);                                \
                        जर (!n)                                                 \
                                सुरूठेवा;                                       \
                                                                                \
                        जर (!strextend_with_separator(&s, "|", n))              \
                                परत रिक्त;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                जर (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        परत रिक्त;                                            \
                                                                                \
                परत TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       अचिन्हित, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

पूर्णांक xstatx_full(पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक statx_flags,
                XStatXFlags xstatx_flags,
                अचिन्हित mandatory_mask,
                अचिन्हित optional_mask,
                uint64_t mandatory_attributes,
                संरचना statx *ret) {

        संरचना statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        पूर्णांक r;

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

        _cleanup_free_ अक्षर *p = रिक्त;
        r = resolve_xat_fdroot(&fd, &path, &p);
        जर (r < 0)
                परत r;

        अचिन्हित request_mask = mandatory_mask|optional_mask;
        जर (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        जर (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                परत negative_errno();

        जर (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                परत log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        जर (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                जर (DEBUG_LOGGING) {
                        _cleanup_free_ अक्षर *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                परत -EUNATCH;
        }

        जर (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                जर (DEBUG_LOGGING) {
                        _cleanup_free_ अक्षर *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                परत -EUNATCH;
        }

        *ret = sx;
        परत FLAGS_SET(sx.stx_mask, optional_mask);
}

स्थिर पूर्णांक xfstatfs(पूर्णांक fd, संरचना statfs *ret) {
        प्रतिज्ञा(ret);

        जर (fd == AT_FDCWD)
                परत RET_NERRNO(statfs(".", ret));
        जर (fd == XAT_FDROOT)
                परत RET_NERRNO(statfs("/", ret));

        प्रतिज्ञा(fd >= 0);
        परत RET_NERRNO(fstatfs(fd, ret));
}

पूर्णांक xstatfsat(पूर्णांक dir_fd, स्थिरांक अक्षर *path, संरचना statfs *ret) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(ret);

        जर (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                जर (fd < 0)
                        परत fd;
                dir_fd = fd;
        }

        परत xfstatfs(dir_fd, ret);
}

पूर्णांक fd_is_read_only_fs(पूर्णांक fd) {
        पूर्णांक r;

        संरचना statfs st;
        r = xfstatfs(fd, &st);
        जर (r < 0)
                परत r;

        जर (st.f_flags & ST_RDONLY)
                परत true;

        जर (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                परत access_fd(fd, W_OK) == -EROFS;

        परत false;
}

पूर्णांक path_is_read_only_fs(स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        प्रतिज्ञा(path);

        fd = खोलो(path, O_CLOEXEC | O_PATH);
        जर (fd < 0)
                परत -त्रुटि_सं;

        परत fd_is_read_only_fs(fd);
}

पूर्णांक inode_same_at(पूर्णांक fda, स्थिरांक अक्षर *filea, पूर्णांक fdb, स्थिरांक अक्षर *fileb, पूर्णांक flags) {
        संरचना stat sta, stb;
        पूर्णांक r;

        प्रतिज्ञा(fda >= 0 || fda == AT_FDCWD);
        प्रतिज्ञा(fdb >= 0 || fdb == AT_FDCWD);
        प्रतिज्ञा((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        जर ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                परत -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        जर (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                परत true;

        _cleanup_close_ पूर्णांक pin_a = -EBADF, pin_b = -EBADF;
        जर (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                जर (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        जर (pin_a < 0)
                                परत -त्रुटि_सं;

                        fda = pin_a;
                        filea = रिक्त;
                        flags |= AT_EMPTY_PATH;
                }

                जर (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        जर (pin_b < 0)
                                परत -त्रुटि_सं;

                        fdb = pin_b;
                        fileb = रिक्त;
                        flags |= AT_EMPTY_PATH;
                }

                पूर्णांक ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ संरचना file_handle *ha = रिक्त, *hb = रिक्त;
                uint64_t mntida, mntidb;
                पूर्णांक _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                जर (r < 0) {
                        जर (is_name_to_handle_at_fatal_error(r))
                                परत r;

                        जा fallback;
                }
                bool have_unique_mntid = r > 0;

                जर (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? रिक्त : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : रिक्त,
                                ntha_flags);
                जर (r < 0) {
                        जर (is_name_to_handle_at_fatal_error(r))
                                परत r;

                        जा fallback;
                }
                जर (r == 0) {
                        प्रतिज्ञा(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                जर (!file_handle_equal(ha, hb))
                        परत false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                जर (mntida == mntidb)
                        परत true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        जर (fstatat(fda, strempty(filea), &sta, flags) < 0)
                परत log_debug_errno(त्रुटि_सं, "Cannot stat %s: %m", strna(filea));

        जर (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                परत log_debug_errno(त्रुटि_सं, "Cannot stat %s: %m", strna(fileb));

        परत stat_inode_same(&sta, &stb);
}

bool is_fs_type(स्थिरांक संरचना statfs *s, statfs_f_type_t magic_मूल्य) {
        प्रतिज्ञा(s);
        assert_cc(आकार(statfs_f_type_t) >= आकार(s->f_type));

        परत F_TYPE_EQUAL(s->f_type, magic_मूल्य);
}

पूर्णांक is_fs_type_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, statfs_f_type_t magic_मूल्य) {
        पूर्णांक r;

        संरचना statfs s;
        r = xstatfsat(dir_fd, path, &s);
        जर (r < 0)
                परत r;

        परत is_fs_type(&s, magic_मूल्य);
}

bool is_temporary_fs(स्थिरांक संरचना statfs *s) {
        परत fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(स्थिरांक संरचना statfs *s) {
        परत fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

पूर्णांक fd_is_temporary_fs(पूर्णांक fd) {
        पूर्णांक r;

        संरचना statfs s;
        r = xfstatfs(fd, &s);
        जर (r < 0)
                परत r;

        परत is_temporary_fs(&s);
}

पूर्णांक fd_is_network_fs(पूर्णांक fd) {
        पूर्णांक r;

        संरचना statfs s;
        r = xfstatfs(fd, &s);
        जर (r < 0)
                परत r;

        परत is_network_fs(&s);
}

पूर्णांक path_is_temporary_fs(स्थिरांक अक्षर *path) {
        संरचना statfs s;

        जर (statfs(path, &s) < 0)
                परत -त्रुटि_सं;

        परत is_temporary_fs(&s);
}

पूर्णांक path_is_network_fs(स्थिरांक अक्षर *path) {
        संरचना statfs s;

        जर (statfs(path, &s) < 0)
                परत -त्रुटि_सं;

        परत is_network_fs(&s);
}

पूर्णांक proc_mounted(शून्य) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        पूर्णांक r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        जर (r == -ENOENT) /* not mounted at all */
                परत false;

        परत r;
}

bool stat_inode_same(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        परत stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b) {

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

        परत stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(स्थिरांक संरचना statx *a, स्थिरांक संरचना statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        जर (!statx_is_set(a) || !statx_is_set(b))
                परत false;

        प्रतिज्ञा(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        प्रतिज्ञा(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        परत
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

पूर्णांक statx_mount_same(स्थिरांक संरचना statx *a, स्थिरांक संरचना statx *b) {
        जर (!statx_is_set(a) || !statx_is_set(b))
                परत false;

        जर ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                परत a->stx_mnt_id == b->stx_mnt_id;

        परत -ENODATA;
}

usec_t statx_timestamp_load(स्थिरांक संरचना statx_timestamp *ts) {
        प्रतिज्ञा(ts);
        परत timespec_load(&(स्थिरांक संरचना timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(स्थिरांक संरचना statx_timestamp *ts) {
        प्रतिज्ञा(ts);
        परत timespec_load_nsec(&(स्थिरांक संरचना timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

शून्य inode_hash_func(स्थिरांक संरचना stat *q, संरचना siphash *state) {
        प्रतिज्ञा(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

पूर्णांक inode_compare_func(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b) {
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->st_dev, b->st_dev);
        जर (r != 0)
                परत r;

        r = CMP(a->st_ino, b->st_ino);
        जर (r != 0)
                परत r;

        परत CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(inode_hash_ops, संरचना stat, inode_hash_func, inode_compare_func, मुक्त);

शून्य inode_unmodified_hash_func(स्थिरांक संरचना stat *q, संरचना siphash *state) {
        प्रतिज्ञा(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        जर (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        अन्यथा {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        जर (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        अन्यथा {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

पूर्णांक inode_unmodified_compare_func(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b) {
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = inode_compare_func(a, b);
        जर (r != 0)
                परत r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        जर (r != 0)
                परत r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        जर (r != 0)
                परत r;

        जर (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                जर (r != 0)
                        परत r;
        }

        जर (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                जर (r != 0)
                        परत r;
        }

        परत 0;
}

DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(inode_unmodified_hash_ops, संरचना stat, inode_unmodified_hash_func, inode_unmodified_compare_func, मुक्त);

स्थिरांक अक्षर* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        निवड (m & S_IFMT) {
        स्थिती S_IFREG:
                परत "reg";
        स्थिती S_IFDIR:
                परत "dir";
        स्थिती S_IFLNK:
                परत "lnk";
        स्थिती S_IFCHR:
                परत "chr";
        स्थिती S_IFBLK:
                परत "blk";
        स्थिती S_IFIFO:
                परत "fifo";
        स्थिती S_IFSOCK:
                परत "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        परत रिक्त;
}

mode_t inode_type_from_string(स्थिरांक अक्षर *s) {
        जर (!s)
                परत MODE_INVALID;

        जर (streq(s, "reg"))
                परत S_IFREG;
        जर (streq(s, "dir"))
                परत S_IFDIR;
        जर (streq(s, "lnk"))
                परत S_IFLNK;
        जर (streq(s, "chr"))
                परत S_IFCHR;
        जर (streq(s, "blk"))
                परत S_IFBLK;
        जर (streq(s, "fifo"))
                परत S_IFIFO;
        जर (streq(s, "sock"))
                परत S_IFSOCK;

        परत MODE_INVALID;
}

पूर्णांक vfs_free_bytes(पूर्णांक fd, uint64_t *ret) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        संरचना statvfs sv;
        जर (fstatvfs(fd, &sv) < 0)
                परत -त्रुटि_सं;

        जर (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                परत -दुस्फल;

        परत 0;
}
