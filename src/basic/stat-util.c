/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <linux/magic.h>
#समावेश <sys/statvfs.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "bitfield.h"
#समावेश "chase.h"
#समावेश "dirent-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "filesystems.h"
#समावेश "fs-util.h"
#समावेश "hash-funcs.h"
#समावेश "log.h"
#समावेश "mountpoint-util.h"
#समावेश "path-util.h"
#समावेश "siphash24.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "time-util.h"

अटल पूर्णांक verify_stat_at(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                bool follow,
                पूर्णांक (*verify_func)(स्थिर काष्ठा stat *st),
                bool verify) {

        काष्ठा stat st;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(fd));
        निश्चित(!isempty(path) || !follow);
        निश्चित(verify_func);

        _cleanup_free_ अक्षर *p = शून्य;
        r = resolve_xat_fdroot(&fd, &path, &p);
        अगर (r < 0)
                वापस r;

        अगर (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                वापस -त्रुटि_सं;

        r = verify_func(&st);
        वापस verify ? r : r >= 0;
}

अटल पूर्णांक mode_verify_regular(mode_t mode) {
        अगर (S_ISDIR(mode))
                वापस -EISDIR;

        अगर (S_ISLNK(mode))
                वापस -ELOOP;

        अगर (!S_ISREG(mode))
                वापस -EBADFD;

        वापस 0;
}

पूर्णांक stat_verify_regular(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        वापस mode_verify_regular(st->st_mode);
}

पूर्णांक statx_verify_regular(स्थिर काष्ठा statx *stx) {
        निश्चित(stx);

        अगर (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                वापस -ENODATA;

        वापस mode_verify_regular(stx->stx_mode);
}

पूर्णांक verify_regular_at(पूर्णांक fd, स्थिर अक्षर *path, bool follow) {
        वापस verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

पूर्णांक fd_verify_regular(पूर्णांक fd) {
        अगर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                वापस -EISDIR;

        वापस verify_regular_at(fd, /* path= */ शून्य, /* follow= */ false);
}

अटल पूर्णांक mode_verify_directory(mode_t mode) {
        अगर (S_ISLNK(mode))
                वापस -ELOOP;

        अगर (!S_ISDIR(mode))
                वापस -ENOTDIR;

        वापस 0;
}

पूर्णांक stat_verify_directory(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        वापस mode_verify_directory(st->st_mode);
}

पूर्णांक statx_verify_directory(स्थिर काष्ठा statx *stx) {
        निश्चित(stx);

        अगर (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                वापस -ENODATA;

        वापस mode_verify_directory(stx->stx_mode);
}

पूर्णांक fd_verify_directory(पूर्णांक fd) {
        अगर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                वापस 0;

        वापस verify_stat_at(fd, शून्य, false, stat_verify_directory, true);
}

पूर्णांक is_dir_at(पूर्णांक fd, स्थिर अक्षर *path, bool follow) {
        वापस verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

पूर्णांक is_dir(स्थिर अक्षर *path, bool follow) {
        निश्चित(!isempty(path));
        वापस is_dir_at(AT_FDCWD, path, follow);
}

पूर्णांक stat_verify_symlink(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        अगर (S_ISDIR(st->st_mode))
                वापस -EISDIR;

        अगर (!S_ISLNK(st->st_mode))
                वापस -ENOLINK;

        वापस 0;
}

पूर्णांक fd_verify_symlink(पूर्णांक fd) {
        अगर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                वापस -EISDIR;

        वापस verify_stat_at(fd, /* path= */ शून्य, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

पूर्णांक is_symlink(स्थिर अक्षर *path) {
        निश्चित(!isempty(path));
        वापस verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

अटल पूर्णांक mode_verify_socket(mode_t mode) {
        अगर (S_ISDIR(mode))
                वापस -EISDIR;

        अगर (S_ISLNK(mode))
                वापस -ELOOP;

        अगर (!S_ISSOCK(mode))
                वापस -ENOTSOCK;

        वापस 0;
}

पूर्णांक stat_verify_socket(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        वापस mode_verify_socket(st->st_mode);
}

पूर्णांक statx_verify_socket(स्थिर काष्ठा statx *stx) {
        निश्चित(stx);

        वापस mode_verify_socket(stx->stx_mode);
}

पूर्णांक fd_verify_socket(पूर्णांक fd) {
        अगर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                वापस -EISDIR;

        वापस verify_stat_at(fd, /* path= */ शून्य, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

पूर्णांक is_socket(स्थिर अक्षर *path) {
        निश्चित(!isempty(path));
        वापस verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

पूर्णांक stat_verify_linked(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        अगर (st->st_nlink <= 0)
                वापस -EIDRM; /* recognizable error. */

        वापस 0;
}

पूर्णांक fd_verify_linked(पूर्णांक fd) {

        अगर (fd == XAT_FDROOT)
                वापस 0;

        वापस verify_stat_at(fd, शून्य, false, stat_verify_linked, true);
}

पूर्णांक stat_verify_block(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        अगर (S_ISDIR(st->st_mode))
                वापस -EISDIR;

        अगर (S_ISLNK(st->st_mode))
                वापस -ELOOP;

        अगर (!S_ISBLK(st->st_mode))
                वापस -ENOTBLK;

        वापस 0;
}

पूर्णांक fd_verify_block(पूर्णांक fd) {
        अगर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                वापस -EISDIR;

        वापस verify_stat_at(fd, /* path= */ शून्य, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

पूर्णांक stat_verify_char(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        अगर (S_ISDIR(st->st_mode))
                वापस -EISDIR;

        अगर (S_ISLNK(st->st_mode))
                वापस -ELOOP;

        अगर (!S_ISCHR(st->st_mode))
                वापस -EBADFD;

        वापस 0;
}

पूर्णांक stat_verify_device_गाँठ(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        अगर (S_ISDIR(st->st_mode))
                वापस -EISDIR;

        अगर (S_ISLNK(st->st_mode))
                वापस -ELOOP;

        अगर (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                वापस -ENOTTY;

        वापस 0;
}

पूर्णांक is_device_गाँठ(स्थिर अक्षर *path) {
        निश्चित(!isempty(path));
        वापस verify_stat_at(AT_FDCWD, path, false, stat_verify_device_गाँठ, false);
}

पूर्णांक stat_verify_regular_or_block(स्थिर काष्ठा stat *st) {
        निश्चित(st);

        अगर (S_ISDIR(st->st_mode))
                वापस -EISDIR;

        अगर (S_ISLNK(st->st_mode))
                वापस -ELOOP;

        अगर (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                वापस -EBADFD;

        वापस 0;
}

पूर्णांक fd_verify_regular_or_block(पूर्णांक fd) {
        अगर (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                वापस -EISDIR;

        वापस verify_stat_at(fd, /* path= */ शून्य, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

पूर्णांक dir_is_empty_at(पूर्णांक dir_fd, स्थिर अक्षर *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        काष्ठा dirent *buf;
        माप_प्रकार m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        अगर (fd < 0)
                वापस fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_आकार_MAX;
        buf = alloca(m);

        क्रम (;;) {
                काष्ठा dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                अगर (n < 0)
                        वापस -त्रुटि_सं;
                अगर (n == 0)
                        अवरोध;

                निश्चित((माप_प्रकार) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_बफर(de, buf, n)
                        अगर (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                वापस 0;
        }

        वापस 1;
}

bool stat_may_be_dev_null(काष्ठा stat *st) {
        निश्चित(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        वापस S_ISCHR(st->st_mode);
}

bool stat_is_empty(काष्ठा stat *st) {
        निश्चित(st);

        वापस S_ISREG(st->st_mode) && st->st_size <= 0;
}

पूर्णांक null_or_empty_path_with_मूल(स्थिर अक्षर *fn, स्थिर अक्षर *मूल) {
        _cleanup_free_ अक्षर *resolved = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        काष्ठा stat st;
        पूर्णांक r;

        निश्चित(fn);

        /* fn includes the root prefix. */

        अगर (path_equal(path_startswith(fn, मूल ?: "/"), "dev/null"))
                वापस true;

        अगर (empty_or_मूल(मूल)) {
                अगर (stat(fn, &st) < 0)
                        वापस -त्रुटि_सं;

                वापस null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, मूल, CHASE_NONEXISTENT, &resolved, &fd);
        अगर (r < 0)
                वापस r;

        अगर (path_equal(path_startswith(resolved, मूल), "dev/null"))
                वापस true;

        अगर (r == 0)
                वापस -ENOENT;

        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        वापस null_or_empty(&st);
}

अटल स्थिर अक्षर* statx_mask_one_to_name(अचिन्हित mask);
अटल स्थिर अक्षर* statx_attribute_to_name(uint64_t attr);

#समावेश "statx-attribute-to-name.inc"
#समावेश "statx-mask-to-name.inc"

#घोषणा DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        अटल अक्षर* prefix##_to_string(type v) {                               \
                अगर (v == 0)                                                     \
                        वापस strdup("");                                      \
                                                                                \
                _cleanup_free_ अक्षर *s = शून्य;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        स्थिर अक्षर *n = func(f);                                \
                        अगर (!n)                                                 \
                                जारी;                                       \
                                                                                \
                        अगर (!strextend_with_separator(&s, "|", n))              \
                                वापस शून्य;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                अगर (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        वापस शून्य;                                            \
                                                                                \
                वापस TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       अचिन्हित, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

पूर्णांक xstatx_full(पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक statx_flags,
                XStatXFlags xstatx_flags,
                अचिन्हित mandatory_mask,
                अचिन्हित optional_mask,
                uint64_t mandatory_attributes,
                काष्ठा statx *ret) {

        काष्ठा statx sx = {}; /* explicitly initialize the struct to make msan silent. */
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

        निश्चित(wildcard_fd_is_valid(fd));
        निश्चित((mandatory_mask & optional_mask) == 0);
        निश्चित(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        निश्चित(ret);

        _cleanup_free_ अक्षर *p = शून्य;
        r = resolve_xat_fdroot(&fd, &path, &p);
        अगर (r < 0)
                वापस r;

        अचिन्हित request_mask = mandatory_mask|optional_mask;
        अगर (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        अगर (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                वापस negative_errno();

        अगर (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        अगर (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                अगर (DEBUG_LOGGING) {
                        _cleanup_free_ अक्षर *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                वापस -EUNATCH;
        }

        अगर (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                अगर (DEBUG_LOGGING) {
                        _cleanup_free_ अक्षर *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                वापस -EUNATCH;
        }

        *ret = sx;
        वापस FLAGS_SET(sx.stx_mask, optional_mask);
}

अटल पूर्णांक xfstatfs(पूर्णांक fd, काष्ठा statfs *ret) {
        निश्चित(ret);

        अगर (fd == AT_FDCWD)
                वापस RET_NERRNO(statfs(".", ret));
        अगर (fd == XAT_FDROOT)
                वापस RET_NERRNO(statfs("/", ret));

        निश्चित(fd >= 0);
        वापस RET_NERRNO(fstatfs(fd, ret));
}

पूर्णांक xstatfsat(पूर्णांक dir_fd, स्थिर अक्षर *path, काष्ठा statfs *ret) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(ret);

        अगर (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                अगर (fd < 0)
                        वापस fd;
                dir_fd = fd;
        }

        वापस xfstatfs(dir_fd, ret);
}

पूर्णांक fd_is_read_only_fs(पूर्णांक fd) {
        पूर्णांक r;

        काष्ठा statfs st;
        r = xfstatfs(fd, &st);
        अगर (r < 0)
                वापस r;

        अगर (st.f_flags & ST_RDONLY)
                वापस true;

        अगर (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                वापस access_fd(fd, W_OK) == -EROFS;

        वापस false;
}

पूर्णांक path_is_read_only_fs(स्थिर अक्षर *path) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(path);

        fd = खोलो(path, O_CLOEXEC | O_PATH);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        वापस fd_is_read_only_fs(fd);
}

पूर्णांक inode_same_at(पूर्णांक fda, स्थिर अक्षर *filea, पूर्णांक fdb, स्थिर अक्षर *fileb, पूर्णांक flags) {
        काष्ठा stat sta, stb;
        पूर्णांक r;

        निश्चित(fda >= 0 || fda == AT_FDCWD);
        निश्चित(fdb >= 0 || fdb == AT_FDCWD);
        निश्चित((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        अगर ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                वापस -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        अगर (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                वापस true;

        _cleanup_close_ पूर्णांक pin_a = -EBADF, pin_b = -EBADF;
        अगर (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                अगर (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        अगर (pin_a < 0)
                                वापस -त्रुटि_सं;

                        fda = pin_a;
                        filea = शून्य;
                        flags |= AT_EMPTY_PATH;
                }

                अगर (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        अगर (pin_b < 0)
                                वापस -त्रुटि_सं;

                        fdb = pin_b;
                        fileb = शून्य;
                        flags |= AT_EMPTY_PATH;
                }

                पूर्णांक ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ काष्ठा file_handle *ha = शून्य, *hb = शून्य;
                uint64_t mntida, mntidb;
                पूर्णांक _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                अगर (r < 0) {
                        अगर (is_name_to_handle_at_fatal_error(r))
                                वापस r;

                        जाओ fallback;
                }
                bool have_unique_mntid = r > 0;

                अगर (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? शून्य : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : शून्य,
                                ntha_flags);
                अगर (r < 0) {
                        अगर (is_name_to_handle_at_fatal_error(r))
                                वापस r;

                        जाओ fallback;
                }
                अगर (r == 0) {
                        निश्चित(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                अगर (!file_handle_equal(ha, hb))
                        वापस false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                अगर (mntida == mntidb)
                        वापस true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        अगर (fstatat(fda, strempty(filea), &sta, flags) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Cannot stat %s: %m", strna(filea));

        अगर (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Cannot stat %s: %m", strna(fileb));

        वापस stat_inode_same(&sta, &stb);
}

bool is_fs_type(स्थिर काष्ठा statfs *s, statfs_f_type_t magic_मूल्य) {
        निश्चित(s);
        assert_cc(माप(statfs_f_type_t) >= माप(s->f_type));

        वापस F_TYPE_EQUAL(s->f_type, magic_मूल्य);
}

पूर्णांक is_fs_type_at(पूर्णांक dir_fd, स्थिर अक्षर *path, statfs_f_type_t magic_मूल्य) {
        पूर्णांक r;

        काष्ठा statfs s;
        r = xstatfsat(dir_fd, path, &s);
        अगर (r < 0)
                वापस r;

        वापस is_fs_type(&s, magic_मूल्य);
}

bool is_temporary_fs(स्थिर काष्ठा statfs *s) {
        वापस fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(स्थिर काष्ठा statfs *s) {
        वापस fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

पूर्णांक fd_is_temporary_fs(पूर्णांक fd) {
        पूर्णांक r;

        काष्ठा statfs s;
        r = xfstatfs(fd, &s);
        अगर (r < 0)
                वापस r;

        वापस is_temporary_fs(&s);
}

पूर्णांक fd_is_network_fs(पूर्णांक fd) {
        पूर्णांक r;

        काष्ठा statfs s;
        r = xfstatfs(fd, &s);
        अगर (r < 0)
                वापस r;

        वापस is_network_fs(&s);
}

पूर्णांक path_is_temporary_fs(स्थिर अक्षर *path) {
        काष्ठा statfs s;

        अगर (statfs(path, &s) < 0)
                वापस -त्रुटि_सं;

        वापस is_temporary_fs(&s);
}

पूर्णांक path_is_network_fs(स्थिर अक्षर *path) {
        काष्ठा statfs s;

        अगर (statfs(path, &s) < 0)
                वापस -त्रुटि_सं;

        वापस is_network_fs(&s);
}

पूर्णांक proc_mounted(व्योम) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        पूर्णांक r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        अगर (r == -ENOENT) /* not mounted at all */
                वापस false;

        वापस r;
}

bool stat_inode_same(स्थिर काष्ठा stat *a, स्थिर काष्ठा stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        वापस stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(स्थिर काष्ठा stat *a, स्थिर काष्ठा stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        निश्चित(a);
        निश्चित(b);

        वापस stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(स्थिर काष्ठा statx *a, स्थिर काष्ठा statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        अगर (!statx_is_set(a) || !statx_is_set(b))
                वापस false;

        निश्चित(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        निश्चित(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        वापस
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

पूर्णांक statx_mount_same(स्थिर काष्ठा statx *a, स्थिर काष्ठा statx *b) {
        अगर (!statx_is_set(a) || !statx_is_set(b))
                वापस false;

        अगर ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                वापस a->stx_mnt_id == b->stx_mnt_id;

        वापस -ENODATA;
}

usec_t statx_timestamp_load(स्थिर काष्ठा statx_timestamp *ts) {
        निश्चित(ts);
        वापस timespec_load(&(स्थिर काष्ठा timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(स्थिर काष्ठा statx_timestamp *ts) {
        निश्चित(ts);
        वापस timespec_load_nsec(&(स्थिर काष्ठा timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

व्योम inode_hash_func(स्थिर काष्ठा stat *q, काष्ठा siphash *state) {
        निश्चित(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

पूर्णांक inode_compare_func(स्थिर काष्ठा stat *a, स्थिर काष्ठा stat *b) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = CMP(a->st_dev, b->st_dev);
        अगर (r != 0)
                वापस r;

        r = CMP(a->st_ino, b->st_ino);
        अगर (r != 0)
                वापस r;

        वापस CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(inode_hash_ops, काष्ठा stat, inode_hash_func, inode_compare_func, मुक्त);

व्योम inode_unmodified_hash_func(स्थिर काष्ठा stat *q, काष्ठा siphash *state) {
        निश्चित(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        अगर (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        अन्यथा {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        अगर (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        अन्यथा {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

पूर्णांक inode_unmodified_compare_func(स्थिर काष्ठा stat *a, स्थिर काष्ठा stat *b) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = inode_compare_func(a, b);
        अगर (r != 0)
                वापस r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        अगर (r != 0)
                वापस r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        अगर (r != 0)
                वापस r;

        अगर (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                अगर (r != 0)
                        वापस r;
        }

        अगर (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                अगर (r != 0)
                        वापस r;
        }

        वापस 0;
}

DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(inode_unmodified_hash_ops, काष्ठा stat, inode_unmodified_hash_func, inode_unmodified_compare_func, मुक्त);

स्थिर अक्षर* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        चयन (m & S_IFMT) {
        हाल S_IFREG:
                वापस "reg";
        हाल S_IFDIR:
                वापस "dir";
        हाल S_IFLNK:
                वापस "lnk";
        हाल S_IFCHR:
                वापस "chr";
        हाल S_IFBLK:
                वापस "blk";
        हाल S_IFIFO:
                वापस "fifo";
        हाल S_IFSOCK:
                वापस "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        वापस शून्य;
}

mode_t inode_type_from_string(स्थिर अक्षर *s) {
        अगर (!s)
                वापस MODE_INVALID;

        अगर (streq(s, "reg"))
                वापस S_IFREG;
        अगर (streq(s, "dir"))
                वापस S_IFDIR;
        अगर (streq(s, "lnk"))
                वापस S_IFLNK;
        अगर (streq(s, "chr"))
                वापस S_IFCHR;
        अगर (streq(s, "blk"))
                वापस S_IFBLK;
        अगर (streq(s, "fifo"))
                वापस S_IFIFO;
        अगर (streq(s, "sock"))
                वापस S_IFSOCK;

        वापस MODE_INVALID;
}

पूर्णांक vfs_free_bytes(पूर्णांक fd, uint64_t *ret) {
        निश्चित(fd >= 0);
        निश्चित(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        काष्ठा statvfs sv;
        अगर (fstatvfs(fd, &sv) < 0)
                वापस -त्रुटि_सं;

        अगर (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                वापस -दुस्फल;

        वापस 0;
}
