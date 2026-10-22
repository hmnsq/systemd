/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <linux/magic.h>
#അന്തര്ഭാവ <sys/statvfs.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "bitfield.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "filesystems.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "hash-funcs.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mountpoint-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "time-util.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ verify_stat_at(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                bool follow,
                പൂർണ്ണസംഖ്യ (*verify_func)(സ്ഥിരം ഘടന stat *st),
                bool verify) {

        ഘടന stat st;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(fd));
        പ്രതിജ്ഞാ(!isempty(path) || !follow);
        പ്രതിജ്ഞാ(verify_func);

        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        r = resolve_xat_fdroot(&fd, &path, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                തിരികെ -errno;

        r = verify_func(&st);
        തിരികെ verify ? r : r >= 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mode_verify_regular(mode_t mode) {
        എങ്കിൽ (S_ISDIR(mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (S_ISLNK(mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISREG(mode))
                തിരികെ -EBADFD;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ stat_verify_regular(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        തിരികെ mode_verify_regular(st->st_mode);
}

പൂർണ്ണസംഖ്യ statx_verify_regular(സ്ഥിരം ഘടന statx *stx) {
        പ്രതിജ്ഞാ(stx);

        എങ്കിൽ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                തിരികെ -ENODATA;

        തിരികെ mode_verify_regular(stx->stx_mode);
}

പൂർണ്ണസംഖ്യ verify_regular_at(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, bool follow) {
        തിരികെ verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

പൂർണ്ണസംഖ്യ fd_verify_regular(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                തിരികെ -EISDIR;

        തിരികെ verify_regular_at(fd, /* path= */ ശൂന്യ, /* follow= */ false);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mode_verify_directory(mode_t mode) {
        എങ്കിൽ (S_ISLNK(mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISDIR(mode))
                തിരികെ -ENOTDIR;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ stat_verify_directory(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        തിരികെ mode_verify_directory(st->st_mode);
}

പൂർണ്ണസംഖ്യ statx_verify_directory(സ്ഥിരം ഘടന statx *stx) {
        പ്രതിജ്ഞാ(stx);

        എങ്കിൽ (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                തിരികെ -ENODATA;

        തിരികെ mode_verify_directory(stx->stx_mode);
}

പൂർണ്ണസംഖ്യ fd_verify_directory(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                തിരികെ 0;

        തിരികെ verify_stat_at(fd, ശൂന്യ, false, stat_verify_directory, true);
}

പൂർണ്ണസംഖ്യ is_dir_at(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, bool follow) {
        തിരികെ verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

പൂർണ്ണസംഖ്യ is_dir(സ്ഥിരം അക്ഷരം *path, bool follow) {
        പ്രതിജ്ഞാ(!isempty(path));
        തിരികെ is_dir_at(AT_FDCWD, path, follow);
}

പൂർണ്ണസംഖ്യ stat_verify_symlink(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        എങ്കിൽ (S_ISDIR(st->st_mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (!S_ISLNK(st->st_mode))
                തിരികെ -ENOLINK;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_verify_symlink(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                തിരികെ -EISDIR;

        തിരികെ verify_stat_at(fd, /* path= */ ശൂന്യ, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

പൂർണ്ണസംഖ്യ is_symlink(സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(!isempty(path));
        തിരികെ verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mode_verify_socket(mode_t mode) {
        എങ്കിൽ (S_ISDIR(mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (S_ISLNK(mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISSOCK(mode))
                തിരികെ -ENOTSOCK;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ stat_verify_socket(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        തിരികെ mode_verify_socket(st->st_mode);
}

പൂർണ്ണസംഖ്യ statx_verify_socket(സ്ഥിരം ഘടന statx *stx) {
        പ്രതിജ്ഞാ(stx);

        തിരികെ mode_verify_socket(stx->stx_mode);
}

പൂർണ്ണസംഖ്യ fd_verify_socket(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                തിരികെ -EISDIR;

        തിരികെ verify_stat_at(fd, /* path= */ ശൂന്യ, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

പൂർണ്ണസംഖ്യ is_socket(സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(!isempty(path));
        തിരികെ verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

പൂർണ്ണസംഖ്യ stat_verify_linked(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        എങ്കിൽ (st->st_nlink <= 0)
                തിരികെ -EIDRM; /* recognizable error. */

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_verify_linked(പൂർണ്ണസംഖ്യ fd) {

        എങ്കിൽ (fd == XAT_FDROOT)
                തിരികെ 0;

        തിരികെ verify_stat_at(fd, ശൂന്യ, false, stat_verify_linked, true);
}

പൂർണ്ണസംഖ്യ stat_verify_block(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        എങ്കിൽ (S_ISDIR(st->st_mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (S_ISLNK(st->st_mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISBLK(st->st_mode))
                തിരികെ -ENOTBLK;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_verify_block(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                തിരികെ -EISDIR;

        തിരികെ verify_stat_at(fd, /* path= */ ശൂന്യ, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

പൂർണ്ണസംഖ്യ stat_verify_char(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        എങ്കിൽ (S_ISDIR(st->st_mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (S_ISLNK(st->st_mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISCHR(st->st_mode))
                തിരികെ -EBADFD;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ stat_verify_device_കണ്ണി(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        എങ്കിൽ (S_ISDIR(st->st_mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (S_ISLNK(st->st_mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                തിരികെ -ENOTTY;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ is_device_കണ്ണി(സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(!isempty(path));
        തിരികെ verify_stat_at(AT_FDCWD, path, false, stat_verify_device_കണ്ണി, false);
}

പൂർണ്ണസംഖ്യ stat_verify_regular_or_block(സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        എങ്കിൽ (S_ISDIR(st->st_mode))
                തിരികെ -EISDIR;

        എങ്കിൽ (S_ISLNK(st->st_mode))
                തിരികെ -ELOOP;

        എങ്കിൽ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                തിരികെ -EBADFD;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_verify_regular_or_block(പൂർണ്ണസംഖ്യ fd) {
        എങ്കിൽ (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                തിരികെ -EISDIR;

        തിരികെ verify_stat_at(fd, /* path= */ ശൂന്യ, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

പൂർണ്ണസംഖ്യ dir_is_empty_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        ഘടന dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        വേണ്ടി (;;) {
                ഘടന dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;
                എങ്കിൽ (n == 0)
                        നിർത്തുക;

                പ്രതിജ്ഞാ((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        എങ്കിൽ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                തിരികെ 0;
        }

        തിരികെ 1;
}

bool stat_may_be_dev_null(ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        തിരികെ S_ISCHR(st->st_mode);
}

bool stat_is_empty(ഘടന stat *st) {
        പ്രതിജ്ഞാ(st);

        തിരികെ S_ISREG(st->st_mode) && st->st_size <= 0;
}

പൂർണ്ണസംഖ്യ null_or_empty_path_with_വേര്(സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *വേര്) {
        _cleanup_free_ അക്ഷരം *resolved = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        ഘടന stat st;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fn);

        /* fn includes the root prefix. */

        എങ്കിൽ (path_equal(path_startswith(fn, വേര് ?: "/"), "dev/null"))
                തിരികെ true;

        എങ്കിൽ (empty_or_വേര്(വേര്)) {
                എങ്കിൽ (stat(fn, &st) < 0)
                        തിരികെ -errno;

                തിരികെ null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, വേര്, CHASE_NONEXISTENT, &resolved, &fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (path_equal(path_startswith(resolved, വേര്), "dev/null"))
                തിരികെ true;

        എങ്കിൽ (r == 0)
                തിരികെ -ENOENT;

        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        തിരികെ null_or_empty(&st);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* statx_mask_one_to_name(unsigned mask);
സ്ഥിതം സ്ഥിരം അക്ഷരം* statx_attribute_to_name(uint64_t attr);

#അന്തര്ഭാവ "statx-attribute-to-name.inc"
#അന്തര്ഭാവ "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        സ്ഥിതം അക്ഷരം* prefix##_to_string(type v) {                               \
                എങ്കിൽ (v == 0)                                                     \
                        തിരികെ strdup("");                                      \
                                                                                \
                _cleanup_free_ അക്ഷരം *s = ശൂന്യ;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        സ്ഥിരം അക്ഷരം *n = func(f);                                \
                        എങ്കിൽ (!n)                                                 \
                                തുടരുക;                                       \
                                                                                \
                        എങ്കിൽ (!strextend_with_separator(&s, "|", n))              \
                                തിരികെ ശൂന്യ;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                എങ്കിൽ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        തിരികെ ശൂന്യ;                                            \
                                                                                \
                തിരികെ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

പൂർണ്ണസംഖ്യ xstatx_full(പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ഘടന statx *ret) {

        ഘടന statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        പൂർണ്ണസംഖ്യ r;

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

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(fd));
        പ്രതിജ്ഞാ((mandatory_mask & optional_mask) == 0);
        പ്രതിജ്ഞാ(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        പ്രതിജ്ഞാ(ret);

        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        r = resolve_xat_fdroot(&fd, &path, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        unsigned request_mask = mandatory_mask|optional_mask;
        എങ്കിൽ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        എങ്കിൽ (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                തിരികെ negative_errno();

        എങ്കിൽ (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        എങ്കിൽ (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                എങ്കിൽ (DEBUG_LOGGING) {
                        _cleanup_free_ അക്ഷരം *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                തിരികെ -EUNATCH;
        }

        എങ്കിൽ (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                എങ്കിൽ (DEBUG_LOGGING) {
                        _cleanup_free_ അക്ഷരം *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                തിരികെ -EUNATCH;
        }

        *ret = sx;
        തിരികെ FLAGS_SET(sx.stx_mask, optional_mask);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ xfstatfs(പൂർണ്ണസംഖ്യ fd, ഘടന statfs *ret) {
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (fd == AT_FDCWD)
                തിരികെ RET_NERRNO(statfs(".", ret));
        എങ്കിൽ (fd == XAT_FDROOT)
                തിരികെ RET_NERRNO(statfs("/", ret));

        പ്രതിജ്ഞാ(fd >= 0);
        തിരികെ RET_NERRNO(fstatfs(fd, ret));
}

പൂർണ്ണസംഖ്യ xstatfsat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ഘടന statfs *ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                എങ്കിൽ (fd < 0)
                        തിരികെ fd;
                dir_fd = fd;
        }

        തിരികെ xfstatfs(dir_fd, ret);
}

പൂർണ്ണസംഖ്യ fd_is_read_only_fs(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        ഘടന statfs st;
        r = xfstatfs(fd, &st);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (st.f_flags & ST_RDONLY)
                തിരികെ true;

        എങ്കിൽ (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                തിരികെ access_fd(fd, W_OK) == -EROFS;

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ path_is_read_only_fs(സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        തിരികെ fd_is_read_only_fs(fd);
}

പൂർണ്ണസംഖ്യ inode_same_at(പൂർണ്ണസംഖ്യ fda, സ്ഥിരം അക്ഷരം *filea, പൂർണ്ണസംഖ്യ fdb, സ്ഥിരം അക്ഷരം *fileb, പൂർണ്ണസംഖ്യ flags) {
        ഘടന stat sta, stb;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fda >= 0 || fda == AT_FDCWD);
        പ്രതിജ്ഞാ(fdb >= 0 || fdb == AT_FDCWD);
        പ്രതിജ്ഞാ((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        എങ്കിൽ ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                തിരികെ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        എങ്കിൽ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                തിരികെ true;

        _cleanup_close_ പൂർണ്ണസംഖ്യ pin_a = -EBADF, pin_b = -EBADF;
        എങ്കിൽ (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                എങ്കിൽ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        എങ്കിൽ (pin_a < 0)
                                തിരികെ -errno;

                        fda = pin_a;
                        filea = ശൂന്യ;
                        flags |= AT_EMPTY_PATH;
                }

                എങ്കിൽ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        എങ്കിൽ (pin_b < 0)
                                തിരികെ -errno;

                        fdb = pin_b;
                        fileb = ശൂന്യ;
                        flags |= AT_EMPTY_PATH;
                }

                പൂർണ്ണസംഖ്യ ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ ഘടന file_handle *ha = ശൂന്യ, *hb = ശൂന്യ;
                uint64_t mntida, mntidb;
                പൂർണ്ണസംഖ്യ _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (is_name_to_handle_at_fatal_error(r))
                                തിരികെ r;

                        പോകുക fallback;
                }
                bool have_unique_mntid = r > 0;

                എങ്കിൽ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ശൂന്യ : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ശൂന്യ,
                                ntha_flags);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (is_name_to_handle_at_fatal_error(r))
                                തിരികെ r;

                        പോകുക fallback;
                }
                എങ്കിൽ (r == 0) {
                        പ്രതിജ്ഞാ(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                എങ്കിൽ (!file_handle_equal(ha, hb))
                        തിരികെ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                എങ്കിൽ (mntida == mntidb)
                        തിരികെ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        എങ്കിൽ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                തിരികെ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        എങ്കിൽ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                തിരികെ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        തിരികെ stat_inode_same(&sta, &stb);
}

bool is_fs_type(സ്ഥിരം ഘടന statfs *s, statfs_f_type_t magic_മൂല്യം) {
        പ്രതിജ്ഞാ(s);
        assert_cc(വലുപ്പം(statfs_f_type_t) >= വലുപ്പം(s->f_type));

        തിരികെ F_TYPE_EQUAL(s->f_type, magic_മൂല്യം);
}

പൂർണ്ണസംഖ്യ is_fs_type_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, statfs_f_type_t magic_മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        ഘടന statfs s;
        r = xstatfsat(dir_fd, path, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ is_fs_type(&s, magic_മൂല്യം);
}

bool is_temporary_fs(സ്ഥിരം ഘടന statfs *s) {
        തിരികെ fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(സ്ഥിരം ഘടന statfs *s) {
        തിരികെ fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

പൂർണ്ണസംഖ്യ fd_is_temporary_fs(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        ഘടന statfs s;
        r = xfstatfs(fd, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ is_temporary_fs(&s);
}

പൂർണ്ണസംഖ്യ fd_is_network_fs(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        ഘടന statfs s;
        r = xfstatfs(fd, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ is_network_fs(&s);
}

പൂർണ്ണസംഖ്യ path_is_temporary_fs(സ്ഥിരം അക്ഷരം *path) {
        ഘടന statfs s;

        എങ്കിൽ (statfs(path, &s) < 0)
                തിരികെ -errno;

        തിരികെ is_temporary_fs(&s);
}

പൂർണ്ണസംഖ്യ path_is_network_fs(സ്ഥിരം അക്ഷരം *path) {
        ഘടന statfs s;

        എങ്കിൽ (statfs(path, &s) < 0)
                തിരികെ -errno;

        തിരികെ is_network_fs(&s);
}

പൂർണ്ണസംഖ്യ proc_mounted(ശൂന്യം) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        പൂർണ്ണസംഖ്യ r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        എങ്കിൽ (r == -ENOENT) /* not mounted at all */
                തിരികെ false;

        തിരികെ r;
}

bool stat_inode_same(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        തിരികെ stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(സ്ഥിരം ഘടന statx *a, സ്ഥിരം ഘടന statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        എങ്കിൽ (!statx_is_set(a) || !statx_is_set(b))
                തിരികെ false;

        പ്രതിജ്ഞാ(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        പ്രതിജ്ഞാ(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        തിരികെ
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

പൂർണ്ണസംഖ്യ statx_mount_same(സ്ഥിരം ഘടന statx *a, സ്ഥിരം ഘടന statx *b) {
        എങ്കിൽ (!statx_is_set(a) || !statx_is_set(b))
                തിരികെ false;

        എങ്കിൽ ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                തിരികെ a->stx_mnt_id == b->stx_mnt_id;

        തിരികെ -ENODATA;
}

usec_t statx_timestamp_load(സ്ഥിരം ഘടന statx_timestamp *ts) {
        പ്രതിജ്ഞാ(ts);
        തിരികെ timespec_load(&(സ്ഥിരം ഘടന timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(സ്ഥിരം ഘടന statx_timestamp *ts) {
        പ്രതിജ്ഞാ(ts);
        തിരികെ timespec_load_nsec(&(സ്ഥിരം ഘടന timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

ശൂന്യം inode_hash_func(സ്ഥിരം ഘടന stat *q, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

പൂർണ്ണസംഖ്യ inode_compare_func(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = CMP(a->st_dev, b->st_dev);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        r = CMP(a->st_ino, b->st_ino);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        തിരികെ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(inode_hash_ops, ഘടന stat, inode_hash_func, inode_compare_func, സ്വതന്ത്രം);

ശൂന്യം inode_unmodified_hash_func(സ്ഥിരം ഘടന stat *q, ഘടന siphash *state) {
        പ്രതിജ്ഞാ(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        എങ്കിൽ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        അല്ലെങ്കിൽ {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        എങ്കിൽ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        അല്ലെങ്കിൽ {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

പൂർണ്ണസംഖ്യ inode_unmodified_compare_func(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = inode_compare_func(a, b);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        എങ്കിൽ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                എങ്കിൽ (r != 0)
                        തിരികെ r;
        }

        എങ്കിൽ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                എങ്കിൽ (r != 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(inode_unmodified_hash_ops, ഘടന stat, inode_unmodified_hash_func, inode_unmodified_compare_func, സ്വതന്ത്രം);

സ്ഥിരം അക്ഷരം* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        തിരഞ്ഞെടുപ്പ് (m & S_IFMT) {
        അവസ്ഥ S_IFREG:
                തിരികെ "reg";
        അവസ്ഥ S_IFDIR:
                തിരികെ "dir";
        അവസ്ഥ S_IFLNK:
                തിരികെ "lnk";
        അവസ്ഥ S_IFCHR:
                തിരികെ "chr";
        അവസ്ഥ S_IFBLK:
                തിരികെ "blk";
        അവസ്ഥ S_IFIFO:
                തിരികെ "fifo";
        അവസ്ഥ S_IFSOCK:
                തിരികെ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        തിരികെ ശൂന്യ;
}

mode_t inode_type_from_string(സ്ഥിരം അക്ഷരം *s) {
        എങ്കിൽ (!s)
                തിരികെ MODE_INVALID;

        എങ്കിൽ (streq(s, "reg"))
                തിരികെ S_IFREG;
        എങ്കിൽ (streq(s, "dir"))
                തിരികെ S_IFDIR;
        എങ്കിൽ (streq(s, "lnk"))
                തിരികെ S_IFLNK;
        എങ്കിൽ (streq(s, "chr"))
                തിരികെ S_IFCHR;
        എങ്കിൽ (streq(s, "blk"))
                തിരികെ S_IFBLK;
        എങ്കിൽ (streq(s, "fifo"))
                തിരികെ S_IFIFO;
        എങ്കിൽ (streq(s, "sock"))
                തിരികെ S_IFSOCK;

        തിരികെ MODE_INVALID;
}

പൂർണ്ണസംഖ്യ vfs_free_bytes(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        ഘടന statvfs sv;
        എങ്കിൽ (fstatvfs(fd, &sv) < 0)
                തിരികെ -errno;

        എങ്കിൽ (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                തിരികെ -ERANGE;

        തിരികെ 0;
}
