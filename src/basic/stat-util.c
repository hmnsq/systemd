/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <linux/magic.h>
#شامل <sys/statvfs.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "bitfield.h"
#شامل "chase.h"
#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "filesystems.h"
#شامل "fs-util.h"
#شامل "hash-funcs.h"
#شامل "log.h"
#شامل "mountpoint-util.h"
#شامل "path-util.h"
#شامل "siphash24.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "time-util.h"

ساکن صحیح verify_stat_at(
                صحیح fd,
                ثابت حرف *راستہ,
                bool follow,
                صحیح (*verify_func)(ثابت ساخت stat *st),
                bool verify) {

        ساخت stat st;
        صحیح r;

        أكد(wildcard_fd_is_valid(fd));
        أكد(!isempty(راستہ) || !follow);
        أكد(verify_func);

        _cleanup_free_ حرف *p = تہی;
        r = resolve_xat_fdroot(&fd, &راستہ, &p);
        اگر (r < 0)
                واپس r;

        اگر (fstatat(fd, strempty(راستہ), &st,
                    (isempty(راستہ) ? AT_خالی_خانے_راستہ : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                واپس -errno;

        r = verify_func(&st);
        واپس verify ? r : r >= 0;
}

ساکن صحیح منوال_verify_regular(mode_t منوال) {
        اگر (S_ISDIR(منوال))
                واپس -EISDIR;

        اگر (S_ISLNK(منوال))
                واپس -ELOOP;

        اگر (!S_ISREG(منوال))
                واپس -EBADFD;

        واپس 0;
}

صحیح stat_verify_regular(ثابت ساخت stat *st) {
        أكد(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        واپس منوال_verify_regular(st->st_mode);
}

صحیح statx_verify_regular(ثابت ساخت statx *stx) {
        أكد(stx);

        اگر (!FLAGS_سیٹ(stx->stx_نقاب, STATX_TYPE))
                واپس -ENODATA;

        واپس منوال_verify_regular(stx->stx_منوال);
}

صحیح verify_regular_at(صحیح fd, ثابت حرف *راستہ, bool follow) {
        واپس verify_stat_at(fd, راستہ, follow, stat_verify_regular, true);
}

صحیح fd_verify_regular(صحیح fd) {
        اگر (اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT))
                واپس -EISDIR;

        واپس verify_regular_at(fd, /* path= */ تہی, /* follow= */ false);
}

ساکن صحیح منوال_verify_directory(mode_t منوال) {
        اگر (S_ISLNK(منوال))
                واپس -ELOOP;

        اگر (!S_ISDIR(منوال))
                واپس -ENOTDIR;

        واپس 0;
}

صحیح stat_verify_directory(ثابت ساخت stat *st) {
        أكد(st);

        واپس منوال_verify_directory(st->st_mode);
}

صحیح statx_verify_directory(ثابت ساخت statx *stx) {
        أكد(stx);

        اگر (!FLAGS_سیٹ(stx->stx_نقاب, STATX_TYPE))
                واپس -ENODATA;

        واپس منوال_verify_directory(stx->stx_منوال);
}

صحیح fd_verify_directory(صحیح fd) {
        اگر (اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT))
                واپس 0;

        واپس verify_stat_at(fd, تہی, false, stat_verify_directory, true);
}

صحیح is_dir_at(صحیح fd, ثابت حرف *راستہ, bool follow) {
        واپس verify_stat_at(fd, راستہ, follow, stat_verify_directory, false);
}

صحیح is_dir(ثابت حرف *راستہ, bool follow) {
        أكد(!isempty(راستہ));
        واپس is_dir_at(AT_FDCWD, راستہ, follow);
}

صحیح stat_verify_symlink(ثابت ساخت stat *st) {
        أكد(st);

        اگر (S_ISDIR(st->st_mode))
                واپس -EISDIR;

        اگر (!S_ISLNK(st->st_mode))
                واپس -ENOLINK;

        واپس 0;
}

صحیح fd_verify_symlink(صحیح fd) {
        اگر (اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT))
                واپس -EISDIR;

        واپس verify_stat_at(fd, /* path= */ تہی, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

صحیح is_symlink(ثابت حرف *راستہ) {
        أكد(!isempty(راستہ));
        واپس verify_stat_at(AT_FDCWD, راستہ, false, stat_verify_symlink, false);
}

ساکن صحیح منوال_verify_socket(mode_t منوال) {
        اگر (S_ISDIR(منوال))
                واپس -EISDIR;

        اگر (S_ISLNK(منوال))
                واپس -ELOOP;

        اگر (!S_ISSOCK(منوال))
                واپس -ENOTSOCK;

        واپس 0;
}

صحیح stat_verify_socket(ثابت ساخت stat *st) {
        أكد(st);

        واپس منوال_verify_socket(st->st_mode);
}

صحیح statx_verify_socket(ثابت ساخت statx *stx) {
        أكد(stx);

        واپس منوال_verify_socket(stx->stx_منوال);
}

صحیح fd_verify_socket(صحیح fd) {
        اگر (اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT))
                واپس -EISDIR;

        واپس verify_stat_at(fd, /* path= */ تہی, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

صحیح is_socket(ثابت حرف *راستہ) {
        أكد(!isempty(راستہ));
        واپس verify_stat_at(AT_FDCWD, راستہ, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

صحیح stat_verify_linked(ثابت ساخت stat *st) {
        أكد(st);

        اگر (st->st_nlink <= 0)
                واپس -EIDRM; /* recognizable error. */

        واپس 0;
}

صحیح fd_verify_linked(صحیح fd) {

        اگر (fd == XAT_FDROOT)
                واپس 0;

        واپس verify_stat_at(fd, تہی, false, stat_verify_linked, true);
}

صحیح stat_verify_بلاک(ثابت ساخت stat *st) {
        أكد(st);

        اگر (S_ISDIR(st->st_mode))
                واپس -EISDIR;

        اگر (S_ISLNK(st->st_mode))
                واپس -ELOOP;

        اگر (!S_ISBLK(st->st_mode))
                واپس -ENOTBLK;

        واپس 0;
}

صحیح fd_verify_بلاک(صحیح fd) {
        اگر (اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT))
                واپس -EISDIR;

        واپس verify_stat_at(fd, /* path= */ تہی, /* follow= */ false, stat_verify_بلاک, /* verify= */ true);
}

صحیح stat_verify_char(ثابت ساخت stat *st) {
        أكد(st);

        اگر (S_ISDIR(st->st_mode))
                واپس -EISDIR;

        اگر (S_ISLNK(st->st_mode))
                واپس -ELOOP;

        اگر (!S_ISCHR(st->st_mode))
                واپس -EBADFD;

        واپس 0;
}

صحیح stat_verify_device_گرہ(ثابت ساخت stat *st) {
        أكد(st);

        اگر (S_ISDIR(st->st_mode))
                واپس -EISDIR;

        اگر (S_ISLNK(st->st_mode))
                واپس -ELOOP;

        اگر (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                واپس -ENOTTY;

        واپس 0;
}

صحیح is_device_گرہ(ثابت حرف *راستہ) {
        أكد(!isempty(راستہ));
        واپس verify_stat_at(AT_FDCWD, راستہ, false, stat_verify_device_گرہ, false);
}

صحیح stat_verify_regular_or_بلاک(ثابت ساخت stat *st) {
        أكد(st);

        اگر (S_ISDIR(st->st_mode))
                واپس -EISDIR;

        اگر (S_ISLNK(st->st_mode))
                واپس -ELOOP;

        اگر (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                واپس -EBADFD;

        واپس 0;
}

صحیح fd_verify_regular_or_بلاک(صحیح fd) {
        اگر (اندر_سیٹ(fd, AT_FDCWD, XAT_FDROOT))
                واپس -EISDIR;

        واپس verify_stat_at(fd, /* path= */ تہی, /* follow= */ false, stat_verify_regular_or_بلاک, /* verify= */ true);
}

صحیح dir_is_خالی_خانے_at(صحیح dir_fd, ثابت حرف *راستہ, bool ignore_hidden_or_backup) {
        _cleanup_close_ صحیح fd = -EBADF;
        ساخت dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, راستہ, O_DIRECTORY|O_CLOEXEC);
        اگر (fd < 0)
                واپس fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_سائز_MAX;
        buf = alloca(m);

        برائے (;;) {
                ساخت dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                اگر (n < 0)
                        واپس -errno;
                اگر (n == 0)
                        توڑو;

                أكد((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_اندر_بفر(de, buf, n)
                        اگر (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                واپس 0;
        }

        واپس 1;
}

bool stat_may_be_dev_null(ساخت stat *st) {
        أكد(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        واپس S_ISCHR(st->st_mode);
}

bool stat_is_خالی_خانے(ساخت stat *st) {
        أكد(st);

        واپس S_ISREG(st->st_mode) && st->st_size <= 0;
}

صحیح null_or_خالی_خانے_راستہ_with_جڑ(ثابت حرف *fn, ثابت حرف *جڑ) {
        _cleanup_free_ حرف *resolved = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        ساخت stat st;
        صحیح r;

        أكد(fn);

        /* fn includes the root prefix. */

        اگر (راستہ_equal(راستہ_startswith(fn, جڑ ?: "/"), "dev/null"))
                واپس true;

        اگر (خالی_خانے_or_جڑ(جڑ)) {
                اگر (stat(fn, &st) < 0)
                        واپس -errno;

                واپس null_or_خالی_خانے(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, جڑ, CHASE_NONEXISTENT, &resolved, &fd);
        اگر (r < 0)
                واپس r;

        اگر (راستہ_equal(راستہ_startswith(resolved, جڑ), "dev/null"))
                واپس true;

        اگر (r == 0)
                واپس -ENOENT;

        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        واپس null_or_خالی_خانے(&st);
}

ساکن ثابت حرف* statx_نقاب_one_مقصد_name(غیرعلامتی نقاب_3);
ساکن ثابت حرف* statx_attribute_مقصد_name(uint64_t attr);

#شامل "statx-attribute-to-name.inc"
#شامل "statx-mask-to-name.inc"

#تعریف_کرو DEFINE_STATX_بٹ_مقصد_STRING(سابقہ, type, func, format_str)             \
        ساکن حرف* سابقہ##_to_string(type v) {                               \
                اگر (v == 0)                                                     \
                        واپس strdup("");                                      \
                                                                                \
                _cleanup_free_ حرف *s = تہی;                                  \
                                                                                \
                ثنائی_ہندسہ_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ثابت حرف *n = func(f);                                \
                        اگر (!n)                                                 \
                                جاری;                                       \
                                                                                \
                        اگر (!strextend_with_separator(&s, "|", n))              \
                                واپس تہی;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                اگر (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        واپس تہی;                                            \
                                                                                \
                واپس TAKE_PTR(s);                                             \
        }

DEFINE_STATX_بٹ_مقصد_STRING(statx_نقاب,       غیرعلامتی, statx_نقاب_one_مقصد_name,  "0x%x");
DEFINE_STATX_بٹ_مقصد_STRING(statx_اوصاف, uint64_t, statx_attribute_مقصد_name, "0x%" PRIx64);

صحیح xstatx_بھرا(صحیح fd,
                ثابت حرف *راستہ,
                صحیح statx_flags,
                XStatXFlags xstatx_flags,
                غیرعلامتی mandatory_نقاب,
                غیرعلامتی optional_نقاب,
                uint64_t mandatory_اوصاف,
                ساخت statx *ret) {

        ساخت statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        صحیح r;

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
        أكد((mandatory_نقاب & optional_نقاب) == 0);
        أكد(!FLAGS_سیٹ(xstatx_flags, XSTATX_MNT_ID_بہترین) || !((mandatory_نقاب|optional_نقاب) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        أكد(ret);

        _cleanup_free_ حرف *p = تہی;
        r = resolve_xat_fdroot(&fd, &راستہ, &p);
        اگر (r < 0)
                واپس r;

        غیرعلامتی request_نقاب = mandatory_نقاب|optional_نقاب;
        اگر (FLAGS_سیٹ(xstatx_flags, XSTATX_MNT_ID_بہترین))
                request_نقاب |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        اگر (statx(fd,
                  strempty(راستہ),
                  statx_flags|(isempty(راستہ) ? AT_خالی_خانے_راستہ : 0),
                  request_نقاب,
                  &sx) < 0)
                واپس negative_errno();

        اگر (FLAGS_سیٹ(xstatx_flags, XSTATX_MNT_ID_بہترین) &&
            !(sx.stx_نقاب & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        اگر (!FLAGS_سیٹ(sx.stx_نقاب, mandatory_نقاب)) {
                اگر (DEBUG_LOGGING) {
                        _cleanup_free_ حرف *نقاب_str = statx_نقاب_مقصد_string(mandatory_نقاب & ~sx.stx_نقاب);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(نقاب_str));
                }

                واپس -EUNATCH;
        }

        اگر (!FLAGS_سیٹ(sx.stx_اوصاف_نقاب, mandatory_اوصاف)) {
                اگر (DEBUG_LOGGING) {
                        _cleanup_free_ حرف *attr_str = statx_اوصاف_مقصد_string(mandatory_اوصاف & ~sx.stx_اوصاف_نقاب);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                واپس -EUNATCH;
        }

        *ret = sx;
        واپس FLAGS_سیٹ(sx.stx_نقاب, optional_نقاب);
}

ساکن صحیح xfstatfs(صحیح fd, ساخت statfs *ret) {
        أكد(ret);

        اگر (fd == AT_FDCWD)
                واپس RET_NERRNO(statfs(".", ret));
        اگر (fd == XAT_FDROOT)
                واپس RET_NERRNO(statfs("/", ret));

        أكد(fd >= 0);
        واپس RET_NERRNO(fstatfs(fd, ret));
}

صحیح xstatfsat(صحیح dir_fd, ثابت حرف *راستہ, ساخت statfs *ret) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        اگر (!isempty(راستہ)) {
                fd = xopenat(dir_fd, راستہ, O_راستہ|O_CLOEXEC);
                اگر (fd < 0)
                        واپس fd;
                dir_fd = fd;
        }

        واپس xfstatfs(dir_fd, ret);
}

صحیح fd_is_read_only_fs(صحیح fd) {
        صحیح r;

        ساخت statfs st;
        r = xfstatfs(fd, &st);
        اگر (r < 0)
                واپس r;

        اگر (st.f_flags & ST_RDONLY)
                واپس true;

        اگر (is_جال_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                واپس access_fd(fd, W_OK) == -EROFS;

        واپس false;
}

صحیح راستہ_is_read_only_fs(ثابت حرف *راستہ) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(راستہ);

        fd = open(راستہ, O_CLOEXEC | O_راستہ);
        اگر (fd < 0)
                واپس -errno;

        واپس fd_is_read_only_fs(fd);
}

صحیح inode_same_at(صحیح fda, ثابت حرف *filea, صحیح fdb, ثابت حرف *fileb, صحیح flags) {
        ساخت stat sta, stb;
        صحیح r;

        أكد(fda >= 0 || fda == AT_FDCWD);
        أكد(fdb >= 0 || fdb == AT_FDCWD);
        أكد((flags & ~(AT_خالی_خانے_راستہ|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        اگر ((isempty(filea) || isempty(fileb)) && !FLAGS_سیٹ(flags, AT_خالی_خانے_راستہ))
                واپس -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        اگر (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW))
                واپس true;

        _cleanup_close_ صحیح pin_a = -EBADF, pin_b = -EBADF;
        اگر (!FLAGS_سیٹ(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                اگر (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_راستہ|O_CLOEXEC|(FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        اگر (pin_a < 0)
                                واپس -errno;

                        fda = pin_a;
                        filea = تہی;
                        flags |= AT_خالی_خانے_راستہ;
                }

                اگر (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_راستہ|O_CLOEXEC|(FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        اگر (pin_b < 0)
                                واپس -errno;

                        fdb = pin_b;
                        fileb = تہی;
                        flags |= AT_خالی_خانے_راستہ;
                }

                صحیح ntha_flags = at_flags_normalize_follow(flags) & (AT_خالی_خانے_راستہ|AT_SYMLINK_FOLLOW);
                _cleanup_free_ ساخت file_handle *ha = تہی, *hb = تہی;
                uint64_t mntida, mntidb;
                صحیح _mntida, _mntidb;

                r = name_مقصد_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                اگر (r < 0) {
                        اگر (is_name_مقصد_handle_at_fatal_error(r))
                                واپس r;

                        جاؤ fallback;
                }
                bool have_unique_mntid = r > 0;

                اگر (!have_unique_mntid)
                        mntida = _mntida;

                r = name_مقصد_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? تہی : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : تہی,
                                ntha_flags);
                اگر (r < 0) {
                        اگر (is_name_مقصد_handle_at_fatal_error(r))
                                واپس r;

                        جاؤ fallback;
                }
                اگر (r == 0) {
                        أكد(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                اگر (!file_handle_equal(ha, hb))
                        واپس false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                اگر (mntida == mntidb)
                        واپس true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        اگر (fstatat(fda, strempty(filea), &sta, flags) < 0)
                واپس log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        اگر (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                واپس log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        واپس stat_inode_same(&sta, &stb);
}

bool is_fs_type(ثابت ساخت statfs *s, statfs_f_type_t magic_قدر) {
        أكد(s);
        assert_cc(حجم(statfs_f_type_t) >= حجم(s->f_type));

        واپس F_TYPE_EQUAL(s->f_type, magic_قدر);
}

صحیح is_fs_type_at(صحیح dir_fd, ثابت حرف *راستہ, statfs_f_type_t magic_قدر) {
        صحیح r;

        ساخت statfs s;
        r = xstatfsat(dir_fd, راستہ, &s);
        اگر (r < 0)
                واپس r;

        واپس is_fs_type(&s, magic_قدر);
}

bool is_temporary_fs(ثابت ساخت statfs *s) {
        واپس fs_اندر_گروہ(s, FILESYSTEM_سیٹ_TEMPORARY);
}

bool is_جال_fs(ثابت ساخت statfs *s) {
        واپس fs_اندر_گروہ(s, FILESYSTEM_سیٹ_جال);
}

صحیح fd_is_temporary_fs(صحیح fd) {
        صحیح r;

        ساخت statfs s;
        r = xfstatfs(fd, &s);
        اگر (r < 0)
                واپس r;

        واپس is_temporary_fs(&s);
}

صحیح fd_is_جال_fs(صحیح fd) {
        صحیح r;

        ساخت statfs s;
        r = xfstatfs(fd, &s);
        اگر (r < 0)
                واپس r;

        واپس is_جال_fs(&s);
}

صحیح راستہ_is_temporary_fs(ثابت حرف *راستہ) {
        ساخت statfs s;

        اگر (statfs(راستہ, &s) < 0)
                واپس -errno;

        واپس is_temporary_fs(&s);
}

صحیح راستہ_is_جال_fs(ثابت حرف *راستہ) {
        ساخت statfs s;

        اگر (statfs(راستہ, &s) < 0)
                واپس -errno;

        واپس is_جال_fs(&s);
}

صحیح proc_mounted(خالی) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        صحیح r;

        /* A quick check of procfs is properly mounted */

        r = راستہ_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        اگر (r == -ENOENT) /* not mounted at all */
                واپس false;

        واپس r;
}

bool stat_inode_same(ثابت ساخت stat *a, ثابت ساخت stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        واپس stat_is_سیٹ(a) && stat_is_سیٹ(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ثابت ساخت stat *a, ثابت ساخت stat *b) {

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

        واپس stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ثابت ساخت statx *a, ثابت ساخت statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        اگر (!statx_is_سیٹ(a) || !statx_is_سیٹ(b))
                واپس false;

        أكد(FLAGS_سیٹ(a->stx_نقاب, STATX_TYPE|STATX_INO));
        أكد(FLAGS_سیٹ(b->stx_نقاب, STATX_TYPE|STATX_INO));

        واپس
                ((a->stx_منوال ^ b->stx_منوال) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

صحیح statx_mount_same(ثابت ساخت statx *a, ثابت ساخت statx *b) {
        اگر (!statx_is_سیٹ(a) || !statx_is_سیٹ(b))
                واپس false;

        اگر ((FLAGS_سیٹ(a->stx_نقاب, STATX_MNT_ID) && FLAGS_سیٹ(b->stx_نقاب, STATX_MNT_ID)) ||
            (FLAGS_سیٹ(a->stx_نقاب, STATX_MNT_ID_UNIQUE) && FLAGS_سیٹ(b->stx_نقاب, STATX_MNT_ID_UNIQUE)))
                واپس a->stx_mnt_id == b->stx_mnt_id;

        واپس -ENODATA;
}

usec_t statx_timestamp_بھرو(ثابت ساخت statx_timestamp *ts) {
        أكد(ts);
        واپس timespec_بھرو(&(ثابت ساخت timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_بھرو_nsec(ثابت ساخت statx_timestamp *ts) {
        أكد(ts);
        واپس timespec_بھرو_nsec(&(ثابت ساخت timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

خالی inode_hash_func(ثابت ساخت stat *q, ساخت siphash *state) {
        أكد(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

صحیح inode_compare_func(ثابت ساخت stat *a, ثابت ساخت stat *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(a->st_dev, b->st_dev);
        اگر (r != 0)
                واپس r;

        r = CMP(a->st_ino, b->st_ino);
        اگر (r != 0)
                واپس r;

        واپس CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(inode_hash_ops, ساخت stat, inode_hash_func, inode_compare_func, آزاد);

خالی inode_unmodified_hash_func(ثابت ساخت stat *q, ساخت siphash *state) {
        أكد(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        اگر (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ورنہ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        اگر (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ورنہ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

صحیح inode_unmodified_compare_func(ثابت ساخت stat *a, ثابت ساخت stat *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = inode_compare_func(a, b);
        اگر (r != 0)
                واپس r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        اگر (r != 0)
                واپس r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        اگر (r != 0)
                واپس r;

        اگر (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                اگر (r != 0)
                        واپس r;
        }

        اگر (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                اگر (r != 0)
                        واپس r;
        }

        واپس 0;
}

DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(inode_unmodified_hash_ops, ساخت stat, inode_unmodified_hash_func, inode_unmodified_compare_func, آزاد);

ثابت حرف* inode_type_مقصد_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        انتخاب (m & S_IFMT) {
        حالت S_IFREG:
                واپس "reg";
        حالت S_IFDIR:
                واپس "dir";
        حالت S_IFLNK:
                واپس "lnk";
        حالت S_IFCHR:
                واپس "chr";
        حالت S_IFBLK:
                واپس "blk";
        حالت S_IFIFO:
                واپس "fifo";
        حالت S_IFSOCK:
                واپس "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        واپس تہی;
}

mode_t inode_type_from_string(ثابت حرف *s) {
        اگر (!s)
                واپس منوال_INVALID;

        اگر (streq(s, "reg"))
                واپس S_IFREG;
        اگر (streq(s, "dir"))
                واپس S_IFDIR;
        اگر (streq(s, "lnk"))
                واپس S_IFLNK;
        اگر (streq(s, "chr"))
                واپس S_IFCHR;
        اگر (streq(s, "blk"))
                واپس S_IFBLK;
        اگر (streq(s, "fifo"))
                واپس S_IFIFO;
        اگر (streq(s, "sock"))
                واپس S_IFSOCK;

        واپس منوال_INVALID;
}

صحیح vfs_free_بائٹیں(صحیح fd, uint64_t *ret) {
        أكد(fd >= 0);
        أكد(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        ساخت statvfs sv;
        اگر (fstatvfs(fd, &sv) < 0)
                واپس -errno;

        اگر (!MUL_محفوظ(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                واپس -ERANGE;

        واپس 0;
}
