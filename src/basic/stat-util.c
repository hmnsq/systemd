/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <linux/magic.h>
#ଅନ୍ତର୍ଭାଵ <sys/statvfs.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "bitfield.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "filesystems.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "hash-funcs.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mountpoint-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_stat_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                bool follow,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*verify_func)(ଅଚଳ ସଂରଚନା stat *st),
                bool verify) {

        ସଂରଚନା stat st;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(fd));
        ପ୍ରତିଜ୍ଞା(!isempty(path) || !follow);
        ପ୍ରତିଜ୍ଞା(verify_func);

        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (fstatat(fd, strempty(path), &st,
                    (isempty(path) ? AT_EMPTY_PATH : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                ଫେରନ୍ତୁ -errno;

        r = verify_func(&st);
        ଫେରନ୍ତୁ verify ? r : r >= 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_verify_regular(mode_t mode) {
        ଯଦି (S_ISDIR(mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (S_ISLNK(mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISREG(mode))
                ଫେରନ୍ତୁ -EBADFD;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_regular(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        ଫେରନ୍ତୁ mode_verify_regular(st->st_mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_verify_regular(ଅଚଳ ସଂରଚନା statx *stx) {
        ପ୍ରତିଜ୍ଞା(stx);

        ଯଦି (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ଫେରନ୍ତୁ -ENODATA;

        ଫେରନ୍ତୁ mode_verify_regular(stx->stx_mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_regular_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, bool follow) {
        ଫେରନ୍ତୁ verify_stat_at(fd, path, follow, stat_verify_regular, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_regular(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ଫେରନ୍ତୁ -EISDIR;

        ଫେରନ୍ତୁ verify_regular_at(fd, /* path= */ ଖାଲି, /* follow= */ false);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_verify_directory(mode_t mode) {
        ଯଦି (S_ISLNK(mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISDIR(mode))
                ଫେରନ୍ତୁ -ENOTDIR;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_directory(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଫେରନ୍ତୁ mode_verify_directory(st->st_mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_verify_directory(ଅଚଳ ସଂରଚନା statx *stx) {
        ପ୍ରତିଜ୍ଞା(stx);

        ଯଦି (!FLAGS_SET(stx->stx_mask, STATX_TYPE))
                ଫେରନ୍ତୁ -ENODATA;

        ଫେରନ୍ତୁ mode_verify_directory(stx->stx_mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_directory(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ verify_stat_at(fd, ଖାଲି, false, stat_verify_directory, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_dir_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, bool follow) {
        ଫେରନ୍ତୁ verify_stat_at(fd, path, follow, stat_verify_directory, false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_dir(ଅଚଳ ଅକ୍ଷର *path, bool follow) {
        ପ୍ରତିଜ୍ଞା(!isempty(path));
        ଫେରନ୍ତୁ is_dir_at(AT_FDCWD, path, follow);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_symlink(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଯଦି (S_ISDIR(st->st_mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (!S_ISLNK(st->st_mode))
                ଫେରନ୍ତୁ -ENOLINK;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_symlink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ଫେରନ୍ତୁ -EISDIR;

        ଫେରନ୍ତୁ verify_stat_at(fd, /* path= */ ଖାଲି, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_symlink(ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(!isempty(path));
        ଫେରନ୍ତୁ verify_stat_at(AT_FDCWD, path, false, stat_verify_symlink, false);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode_verify_socket(mode_t mode) {
        ଯଦି (S_ISDIR(mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (S_ISLNK(mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISSOCK(mode))
                ଫେରନ୍ତୁ -ENOTSOCK;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_socket(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଫେରନ୍ତୁ mode_verify_socket(st->st_mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_verify_socket(ଅଚଳ ସଂରଚନା statx *stx) {
        ପ୍ରତିଜ୍ଞା(stx);

        ଫେରନ୍ତୁ mode_verify_socket(stx->stx_mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_socket(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ଫେରନ୍ତୁ -EISDIR;

        ଫେରନ୍ତୁ verify_stat_at(fd, /* path= */ ଖାଲି, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_socket(ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(!isempty(path));
        ଫେରନ୍ତୁ verify_stat_at(AT_FDCWD, path, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_linked(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଯଦି (st->st_nlink <= 0)
                ଫେରନ୍ତୁ -EIDRM; /* recognizable error. */

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_linked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {

        ଯଦି (fd == XAT_FDROOT)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ verify_stat_at(fd, ଖାଲି, false, stat_verify_linked, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_block(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଯଦି (S_ISDIR(st->st_mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (S_ISLNK(st->st_mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISBLK(st->st_mode))
                ଫେରନ୍ତୁ -ENOTBLK;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_block(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ଫେରନ୍ତୁ -EISDIR;

        ଫେରନ୍ତୁ verify_stat_at(fd, /* path= */ ଖାଲି, /* follow= */ false, stat_verify_block, /* verify= */ true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_char(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଯଦି (S_ISDIR(st->st_mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (S_ISLNK(st->st_mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISCHR(st->st_mode))
                ଫେରନ୍ତୁ -EBADFD;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_device_ଗଣ୍ଠି(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଯଦି (S_ISDIR(st->st_mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (S_ISLNK(st->st_mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                ଫେରନ୍ତୁ -ENOTTY;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_device_ଗଣ୍ଠି(ଅଚଳ ଅକ୍ଷର *path) {
        ପ୍ରତିଜ୍ଞା(!isempty(path));
        ଫେରନ୍ତୁ verify_stat_at(AT_FDCWD, path, false, stat_verify_device_ଗଣ୍ଠି, false);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_regular_or_block(ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଯଦି (S_ISDIR(st->st_mode))
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (S_ISLNK(st->st_mode))
                ଫେରନ୍ତୁ -ELOOP;

        ଯଦି (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                ଫେରନ୍ତୁ -EBADFD;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_regular_or_block(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଯଦି (IN_SET(fd, AT_FDCWD, XAT_FDROOT))
                ଫେରନ୍ତୁ -EISDIR;

        ଫେରନ୍ତୁ verify_stat_at(fd, /* path= */ ଖାଲି, /* follow= */ false, stat_verify_regular_or_block, /* verify= */ true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_is_empty_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, bool ignore_hidden_or_backup) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ସଂରଚନା dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, path, O_DIRECTORY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_SIZE_MAX;
        buf = alloca(m);

        ପାଇଁ (;;) {
                ସଂରଚନା dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (n == 0)
                        ଭାଙ୍ଗ;

                ପ୍ରତିଜ୍ଞା((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_IN_BUFFER(de, buf, n)
                        ଯଦି (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ 1;
}

bool stat_may_be_dev_null(ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        ଫେରନ୍ତୁ S_ISCHR(st->st_mode);
}

bool stat_is_empty(ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(st);

        ଫେରନ୍ତୁ S_ISREG(st->st_mode) && st->st_size <= 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା null_or_empty_path_with_ମୂଳ(ଅଚଳ ଅକ୍ଷର *fn, ଅଚଳ ଅକ୍ଷର *ମୂଳ) {
        _cleanup_free_ ଅକ୍ଷର *resolved = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ସଂରଚନା stat st;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fn);

        /* fn includes the root prefix. */

        ଯଦି (path_equal(path_startswith(fn, ମୂଳ ?: "/"), "dev/null"))
                ଫେରନ୍ତୁ true;

        ଯଦି (empty_or_ମୂଳ(ମୂଳ)) {
                ଯଦି (stat(fn, &st) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ null_or_empty(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, ମୂଳ, CHASE_NONEXISTENT, &resolved, &fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (path_equal(path_startswith(resolved, ମୂଳ), "dev/null"))
                ଫେରନ୍ତୁ true;

        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -ENOENT;

        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ null_or_empty(&st);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* statx_mask_one_to_name(unsigned mask);
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* statx_attribute_to_name(uint64_t attr);

#ଅନ୍ତର୍ଭାଵ "statx-attribute-to-name.inc"
#ଅନ୍ତର୍ଭାଵ "statx-mask-to-name.inc"

#define DEFINE_STATX_BITS_TO_STRING(prefix, type, func, format_str)             \
        ସ୍ଥିର ଅକ୍ଷର* prefix##_to_string(type v) {                               \
                ଯଦି (v == 0)                                                     \
                        ଫେରନ୍ତୁ strdup("");                                      \
                                                                                \
                _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;                                  \
                                                                                \
                BIT_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        ଅଚଳ ଅକ୍ଷର *n = func(f);                                \
                        ଯଦି (!n)                                                 \
                                ଜାରି;                                       \
                                                                                \
                        ଯଦି (!strextend_with_separator(&s, "|", n))              \
                                ଫେରନ୍ତୁ ଖାଲି;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ଯଦି (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        ଫେରନ୍ତୁ ଖାଲି;                                            \
                                                                                \
                ଫେରନ୍ତୁ TAKE_PTR(s);                                             \
        }

DEFINE_STATX_BITS_TO_STRING(statx_mask,       unsigned, statx_mask_one_to_name,  "0x%x");
DEFINE_STATX_BITS_TO_STRING(statx_attributes, uint64_t, statx_attribute_to_name, "0x%" PRIx64);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatx_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ସଂରଚନା statx *ret) {

        ସଂରଚନା statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(fd));
        ପ୍ରତିଜ୍ଞା((mandatory_mask & optional_mask) == 0);
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) || !((mandatory_mask|optional_mask) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        ପ୍ରତିଜ୍ଞା(ret);

        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        r = resolve_xat_fdroot(&fd, &path, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        unsigned request_mask = mandatory_mask|optional_mask;
        ଯଦି (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST))
                request_mask |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ଯଦି (statx(fd,
                  strempty(path),
                  statx_flags|(isempty(path) ? AT_EMPTY_PATH : 0),
                  request_mask,
                  &sx) < 0)
                ଫେରନ୍ତୁ negative_errno();

        ଯଦି (FLAGS_SET(xstatx_flags, XSTATX_MNT_ID_BEST) &&
            !(sx.stx_mask & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ଯଦି (!FLAGS_SET(sx.stx_mask, mandatory_mask)) {
                ଯଦି (DEBUG_LOGGING) {
                        _cleanup_free_ ଅକ୍ଷର *mask_str = statx_mask_to_string(mandatory_mask & ~sx.stx_mask);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(mask_str));
                }

                ଫେରନ୍ତୁ -EUNATCH;
        }

        ଯଦି (!FLAGS_SET(sx.stx_attributes_mask, mandatory_attributes)) {
                ଯଦି (DEBUG_LOGGING) {
                        _cleanup_free_ ଅକ୍ଷର *attr_str = statx_attributes_to_string(mandatory_attributes & ~sx.stx_attributes_mask);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                ଫେରନ୍ତୁ -EUNATCH;
        }

        *ret = sx;
        ଫେରନ୍ତୁ FLAGS_SET(sx.stx_mask, optional_mask);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xfstatfs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା statfs *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (fd == AT_FDCWD)
                ଫେରନ୍ତୁ RET_NERRNO(statfs(".", ret));
        ଯଦି (fd == XAT_FDROOT)
                ଫେରନ୍ତୁ RET_NERRNO(statfs("/", ret));

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ଫେରନ୍ତୁ RET_NERRNO(fstatfs(fd, ret));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatfsat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ସଂରଚନା statfs *ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!isempty(path)) {
                fd = xopenat(dir_fd, path, O_PATH|O_CLOEXEC);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ fd;
                dir_fd = fd;
        }

        ଫେରନ୍ତୁ xfstatfs(dir_fd, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_read_only_fs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ସଂରଚନା statfs st;
        r = xfstatfs(fd, &st);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (st.f_flags & ST_RDONLY)
                ଫେରନ୍ତୁ true;

        ଯଦି (is_network_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                ଫେରନ୍ତୁ access_fd(fd, W_OK) == -EROFS;

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_read_only_fs(ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(path);

        fd = open(path, O_CLOEXEC | O_PATH);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ fd_is_read_only_fs(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_same_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fda, ଅଚଳ ଅକ୍ଷର *filea, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdb, ଅଚଳ ଅକ୍ଷର *fileb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ସଂରଚନା stat sta, stb;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fda >= 0 || fda == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(fdb >= 0 || fdb == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା((flags & ~(AT_EMPTY_PATH|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ଯଦି ((isempty(filea) || isempty(fileb)) && !FLAGS_SET(flags, AT_EMPTY_PATH))
                ଫେରନ୍ତୁ -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ଯଦି (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW))
                ଫେରନ୍ତୁ true;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pin_a = -EBADF, pin_b = -EBADF;
        ଯଦି (!FLAGS_SET(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ଯଦି (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ଯଦି (pin_a < 0)
                                ଫେରନ୍ତୁ -errno;

                        fda = pin_a;
                        filea = ଖାଲି;
                        flags |= AT_EMPTY_PATH;
                }

                ଯଦି (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_PATH|O_CLOEXEC|(FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ଯଦି (pin_b < 0)
                                ଫେରନ୍ତୁ -errno;

                        fdb = pin_b;
                        fileb = ଖାଲି;
                        flags |= AT_EMPTY_PATH;
                }

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ntha_flags = at_flags_normalize_follow(flags) & (AT_EMPTY_PATH|AT_SYMLINK_FOLLOW);
                _cleanup_free_ ସଂରଚନା file_handle *ha = ଖାଲି, *hb = ଖାଲି;
                uint64_t mntida, mntidb;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _mntida, _mntidb;

                r = name_to_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ଯଦି (r < 0) {
                        ଯଦି (is_name_to_handle_at_fatal_error(r))
                                ଫେରନ୍ତୁ r;

                        ଯାଅ fallback;
                }
                bool have_unique_mntid = r > 0;

                ଯଦି (!have_unique_mntid)
                        mntida = _mntida;

                r = name_to_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ଖାଲି : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ଖାଲି,
                                ntha_flags);
                ଯଦି (r < 0) {
                        ଯଦି (is_name_to_handle_at_fatal_error(r))
                                ଫେରନ୍ତୁ r;

                        ଯାଅ fallback;
                }
                ଯଦି (r == 0) {
                        ପ୍ରତିଜ୍ଞା(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ଯଦି (!file_handle_equal(ha, hb))
                        ଫେରନ୍ତୁ false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ଯଦି (mntida == mntidb)
                        ଫେରନ୍ତୁ true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ଯଦି (fstatat(fda, strempty(filea), &sta, flags) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ଯଦି (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        ଫେରନ୍ତୁ stat_inode_same(&sta, &stb);
}

bool is_fs_type(ଅଚଳ ସଂରଚନା statfs *s, statfs_f_type_t magic_ମୂଲ୍ୟ) {
        ପ୍ରତିଜ୍ଞା(s);
        assert_cc(ଆକାର(statfs_f_type_t) >= ଆକାର(s->f_type));

        ଫେରନ୍ତୁ F_TYPE_EQUAL(s->f_type, magic_ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_fs_type_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, statfs_f_type_t magic_ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ସଂରଚନା statfs s;
        r = xstatfsat(dir_fd, path, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ is_fs_type(&s, magic_ମୂଲ୍ୟ);
}

bool is_temporary_fs(ଅଚଳ ସଂରଚନା statfs *s) {
        ଫେରନ୍ତୁ fs_in_group(s, FILESYSTEM_SET_TEMPORARY);
}

bool is_network_fs(ଅଚଳ ସଂରଚନା statfs *s) {
        ଫେରନ୍ତୁ fs_in_group(s, FILESYSTEM_SET_NETWORK);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_temporary_fs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ସଂରଚନା statfs s;
        r = xfstatfs(fd, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ is_temporary_fs(&s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_network_fs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ସଂରଚନା statfs s;
        r = xfstatfs(fd, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ is_network_fs(&s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_temporary_fs(ଅଚଳ ଅକ୍ଷର *path) {
        ସଂରଚନା statfs s;

        ଯଦି (statfs(path, &s) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ is_temporary_fs(&s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_network_fs(ଅଚଳ ଅକ୍ଷର *path) {
        ସଂରଚନା statfs s;

        ଯଦି (statfs(path, &s) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ is_network_fs(&s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_mounted(ଶୂନ୍ୟ) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* A quick check of procfs is properly mounted */

        r = path_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ଯଦି (r == -ENOENT) /* not mounted at all */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ r;
}

bool stat_inode_same(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        ଫେରନ୍ତୁ stat_is_set(a) && stat_is_set(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(ଅଚଳ ସଂରଚନା statx *a, ଅଚଳ ସଂରଚନା statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ଯଦି (!statx_is_set(a) || !statx_is_set(b))
                ଫେରନ୍ତୁ false;

        ପ୍ରତିଜ୍ଞା(FLAGS_SET(a->stx_mask, STATX_TYPE|STATX_INO));
        ପ୍ରତିଜ୍ଞା(FLAGS_SET(b->stx_mask, STATX_TYPE|STATX_INO));

        ଫେରନ୍ତୁ
                ((a->stx_mode ^ b->stx_mode) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_mount_same(ଅଚଳ ସଂରଚନା statx *a, ଅଚଳ ସଂରଚନା statx *b) {
        ଯଦି (!statx_is_set(a) || !statx_is_set(b))
                ଫେରନ୍ତୁ false;

        ଯଦି ((FLAGS_SET(a->stx_mask, STATX_MNT_ID) && FLAGS_SET(b->stx_mask, STATX_MNT_ID)) ||
            (FLAGS_SET(a->stx_mask, STATX_MNT_ID_UNIQUE) && FLAGS_SET(b->stx_mask, STATX_MNT_ID_UNIQUE)))
                ଫେରନ୍ତୁ a->stx_mnt_id == b->stx_mnt_id;

        ଫେରନ୍ତୁ -ENODATA;
}

usec_t statx_timestamp_load(ଅଚଳ ସଂରଚନା statx_timestamp *ts) {
        ପ୍ରତିଜ୍ଞା(ts);
        ଫେରନ୍ତୁ timespec_load(&(ଅଚଳ ସଂରଚନା timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_load_nsec(ଅଚଳ ସଂରଚନା statx_timestamp *ts) {
        ପ୍ରତିଜ୍ଞା(ts);
        ଫେରନ୍ତୁ timespec_load_nsec(&(ଅଚଳ ସଂରଚନା timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

ଶୂନ୍ୟ inode_hash_func(ଅଚଳ ସଂରଚନା stat *q, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_compare_func(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = CMP(a->st_dev, b->st_dev);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        r = CMP(a->st_ino, b->st_ino);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(inode_hash_ops, ସଂରଚନା stat, inode_hash_func, inode_compare_func, ମୁକ୍ତ);

ଶୂନ୍ୟ inode_unmodified_hash_func(ଅଚଳ ସଂରଚନା stat *q, ସଂରଚନା siphash *state) {
        ପ୍ରତିଜ୍ଞା(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ଯଦି (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        ଅନ୍ୟଥା {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ଯଦି (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        ଅନ୍ୟଥା {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_unmodified_compare_func(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = inode_compare_func(a, b);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(inode_unmodified_hash_ops, ସଂରଚନା stat, inode_unmodified_hash_func, inode_unmodified_compare_func, ମୁକ୍ତ);

ଅଚଳ ଅକ୍ଷର* inode_type_to_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        ବାଛ (m & S_IFMT) {
        ସ୍ଥିତି S_IFREG:
                ଫେରନ୍ତୁ "reg";
        ସ୍ଥିତି S_IFDIR:
                ଫେରନ୍ତୁ "dir";
        ସ୍ଥିତି S_IFLNK:
                ଫେରନ୍ତୁ "lnk";
        ସ୍ଥିତି S_IFCHR:
                ଫେରନ୍ତୁ "chr";
        ସ୍ଥିତି S_IFBLK:
                ଫେରନ୍ତୁ "blk";
        ସ୍ଥିତି S_IFIFO:
                ଫେରନ୍ତୁ "fifo";
        ସ୍ଥିତି S_IFSOCK:
                ଫେରନ୍ତୁ "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        ଫେରନ୍ତୁ ଖାଲି;
}

mode_t inode_type_from_string(ଅଚଳ ଅକ୍ଷର *s) {
        ଯଦି (!s)
                ଫେରନ୍ତୁ MODE_INVALID;

        ଯଦି (streq(s, "reg"))
                ଫେରନ୍ତୁ S_IFREG;
        ଯଦି (streq(s, "dir"))
                ଫେରନ୍ତୁ S_IFDIR;
        ଯଦି (streq(s, "lnk"))
                ଫେରନ୍ତୁ S_IFLNK;
        ଯଦି (streq(s, "chr"))
                ଫେରନ୍ତୁ S_IFCHR;
        ଯଦି (streq(s, "blk"))
                ଫେରନ୍ତୁ S_IFBLK;
        ଯଦି (streq(s, "fifo"))
                ଫେରନ୍ତୁ S_IFIFO;
        ଯଦି (streq(s, "sock"))
                ଫେରନ୍ତୁ S_IFSOCK;

        ଫେରନ୍ତୁ MODE_INVALID;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vfs_free_bytes(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        ସଂରଚନା statvfs sv;
        ଯଦି (fstatvfs(fd, &sv) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (!MUL_SAFE(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ 0;
}
