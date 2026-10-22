/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <linux/magic.h>
#అన్తర్భావ <sys/statvfs.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "bitfield.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "filesystems.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "hash-funcs.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mountpoint-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "time-util.h"

స్థిర పూర్ణాంకం verify_stat_at(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                bool follow,
                పూర్ణాంకం (*verify_func)(స్థిరరాశి నిర్మాణం stat *st),
                bool verify) {

        నిర్మాణం stat st;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(fd));
        ప్రతిజ్ఞా(!isempty(మార్గం) || !follow);
        ప్రతిజ్ఞా(verify_func);

        _cleanup_free_ అక్షరం *p = ఖాళీ;
        r = resolve_xat_fdroot(&fd, &మార్గం, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (fstatat(fd, strempty(మార్గం), &st,
                    (isempty(మార్గం) ? AT_ఖాళీలు_మార్గం : 0) | (follow ? 0 : AT_SYMLINK_NOFOLLOW)) < 0)
                తిరిగి -errno;

        r = verify_func(&st);
        తిరిగి verify ? r : r >= 0;
}

స్థిర పూర్ణాంకం బహుళకం_verify_regular(mode_t బహుళకం) {
        ఒకవేళ (S_ISDIR(బహుళకం))
                తిరిగి -EISDIR;

        ఒకవేళ (S_ISLNK(బహుళకం))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISREG(బహుళకం))
                తిరిగి -EBADFD;

        తిరిగి 0;
}

పూర్ణాంకం stat_verify_regular(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        /* Checks whether the specified stat() structure refers to a regular file. If not returns an
         * appropriate error code. */

        తిరిగి బహుళకం_verify_regular(st->st_mode);
}

పూర్ణాంకం statx_verify_regular(స్థిరరాశి నిర్మాణం statx *stx) {
        ప్రతిజ్ఞా(stx);

        ఒకవేళ (!FLAGS_సమితి(stx->stx_ముసుగు, STATX_TYPE))
                తిరిగి -ENODATA;

        తిరిగి బహుళకం_verify_regular(stx->stx_బహుళకం);
}

పూర్ణాంకం verify_regular_at(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, bool follow) {
        తిరిగి verify_stat_at(fd, మార్గం, follow, stat_verify_regular, true);
}

పూర్ణాంకం fd_verify_regular(పూర్ణాంకం fd) {
        ఒకవేళ (లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT))
                తిరిగి -EISDIR;

        తిరిగి verify_regular_at(fd, /* path= */ ఖాళీ, /* follow= */ false);
}

స్థిర పూర్ణాంకం బహుళకం_verify_directory(mode_t బహుళకం) {
        ఒకవేళ (S_ISLNK(బహుళకం))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISDIR(బహుళకం))
                తిరిగి -ENOTDIR;

        తిరిగి 0;
}

పూర్ణాంకం stat_verify_directory(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        తిరిగి బహుళకం_verify_directory(st->st_mode);
}

పూర్ణాంకం statx_verify_directory(స్థిరరాశి నిర్మాణం statx *stx) {
        ప్రతిజ్ఞా(stx);

        ఒకవేళ (!FLAGS_సమితి(stx->stx_ముసుగు, STATX_TYPE))
                తిరిగి -ENODATA;

        తిరిగి బహుళకం_verify_directory(stx->stx_బహుళకం);
}

పూర్ణాంకం fd_verify_directory(పూర్ణాంకం fd) {
        ఒకవేళ (లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT))
                తిరిగి 0;

        తిరిగి verify_stat_at(fd, ఖాళీ, false, stat_verify_directory, true);
}

పూర్ణాంకం is_dir_at(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, bool follow) {
        తిరిగి verify_stat_at(fd, మార్గం, follow, stat_verify_directory, false);
}

పూర్ణాంకం is_dir(స్థిరరాశి అక్షరం *మార్గం, bool follow) {
        ప్రతిజ్ఞా(!isempty(మార్గం));
        తిరిగి is_dir_at(AT_FDCWD, మార్గం, follow);
}

పూర్ణాంకం stat_verify_symlink(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        ఒకవేళ (S_ISDIR(st->st_mode))
                తిరిగి -EISDIR;

        ఒకవేళ (!S_ISLNK(st->st_mode))
                తిరిగి -ENOLINK;

        తిరిగి 0;
}

పూర్ణాంకం fd_verify_symlink(పూర్ణాంకం fd) {
        ఒకవేళ (లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT))
                తిరిగి -EISDIR;

        తిరిగి verify_stat_at(fd, /* path= */ ఖాళీ, /* follow= */ false, stat_verify_symlink, /* verify= */ true);
}

పూర్ణాంకం is_symlink(స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(!isempty(మార్గం));
        తిరిగి verify_stat_at(AT_FDCWD, మార్గం, false, stat_verify_symlink, false);
}

స్థిర పూర్ణాంకం బహుళకం_verify_socket(mode_t బహుళకం) {
        ఒకవేళ (S_ISDIR(బహుళకం))
                తిరిగి -EISDIR;

        ఒకవేళ (S_ISLNK(బహుళకం))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISSOCK(బహుళకం))
                తిరిగి -ENOTSOCK;

        తిరిగి 0;
}

పూర్ణాంకం stat_verify_socket(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        తిరిగి బహుళకం_verify_socket(st->st_mode);
}

పూర్ణాంకం statx_verify_socket(స్థిరరాశి నిర్మాణం statx *stx) {
        ప్రతిజ్ఞా(stx);

        తిరిగి బహుళకం_verify_socket(stx->stx_బహుళకం);
}

పూర్ణాంకం fd_verify_socket(పూర్ణాంకం fd) {
        ఒకవేళ (లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT))
                తిరిగి -EISDIR;

        తిరిగి verify_stat_at(fd, /* path= */ ఖాళీ, /* follow= */ false, stat_verify_socket, /* verify= */ true);
}

పూర్ణాంకం is_socket(స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(!isempty(మార్గం));
        తిరిగి verify_stat_at(AT_FDCWD, మార్గం, /* follow= */ true, stat_verify_socket, /* verify= */ false);
}

పూర్ణాంకం stat_verify_linked(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        ఒకవేళ (st->st_nlink <= 0)
                తిరిగి -EIDRM; /* recognizable error. */

        తిరిగి 0;
}

పూర్ణాంకం fd_verify_linked(పూర్ణాంకం fd) {

        ఒకవేళ (fd == XAT_FDROOT)
                తిరిగి 0;

        తిరిగి verify_stat_at(fd, ఖాళీ, false, stat_verify_linked, true);
}

పూర్ణాంకం stat_verify_ఖండం(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        ఒకవేళ (S_ISDIR(st->st_mode))
                తిరిగి -EISDIR;

        ఒకవేళ (S_ISLNK(st->st_mode))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISBLK(st->st_mode))
                తిరిగి -ENOTBLK;

        తిరిగి 0;
}

పూర్ణాంకం fd_verify_ఖండం(పూర్ణాంకం fd) {
        ఒకవేళ (లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT))
                తిరిగి -EISDIR;

        తిరిగి verify_stat_at(fd, /* path= */ ఖాళీ, /* follow= */ false, stat_verify_ఖండం, /* verify= */ true);
}

పూర్ణాంకం stat_verify_char(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        ఒకవేళ (S_ISDIR(st->st_mode))
                తిరిగి -EISDIR;

        ఒకవేళ (S_ISLNK(st->st_mode))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISCHR(st->st_mode))
                తిరిగి -EBADFD;

        తిరిగి 0;
}

పూర్ణాంకం stat_verify_device_కణుపు(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        ఒకవేళ (S_ISDIR(st->st_mode))
                తిరిగి -EISDIR;

        ఒకవేళ (S_ISLNK(st->st_mode))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISBLK(st->st_mode) && !S_ISCHR(st->st_mode))
                తిరిగి -ENOTTY;

        తిరిగి 0;
}

పూర్ణాంకం is_device_కణుపు(స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(!isempty(మార్గం));
        తిరిగి verify_stat_at(AT_FDCWD, మార్గం, false, stat_verify_device_కణుపు, false);
}

పూర్ణాంకం stat_verify_regular_or_ఖండం(స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        ఒకవేళ (S_ISDIR(st->st_mode))
                తిరిగి -EISDIR;

        ఒకవేళ (S_ISLNK(st->st_mode))
                తిరిగి -ELOOP;

        ఒకవేళ (!S_ISREG(st->st_mode) && !S_ISBLK(st->st_mode))
                తిరిగి -EBADFD;

        తిరిగి 0;
}

పూర్ణాంకం fd_verify_regular_or_ఖండం(పూర్ణాంకం fd) {
        ఒకవేళ (లోపల_సమితి(fd, AT_FDCWD, XAT_FDROOT))
                తిరిగి -EISDIR;

        తిరిగి verify_stat_at(fd, /* path= */ ఖాళీ, /* follow= */ false, stat_verify_regular_or_ఖండం, /* verify= */ true);
}

పూర్ణాంకం dir_is_ఖాళీలు_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, bool ignore_hidden_or_backup) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        నిర్మాణం dirent *buf;
        size_t m;

        fd = xopenat(dir_fd, మార్గం, O_DIRECTORY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        /* Allocate space for at least 3 full dirents, since every dir has at least two entries ("."  +
         * ".."), and only once we have seen if there's a third we know whether the dir is empty or not. If
         * 'ignore_hidden_or_backup' is true we'll allocate a bit more, since we might skip over a bunch of
         * entries that we end up ignoring. */
        m = (ignore_hidden_or_backup ? 16 : 3) * DIRENT_కొలత_MAX;
        buf = alloca(m);

        కొరకు (;;) {
                నిర్మాణం dirent *de;
                ssize_t n;

                n = getdents64(fd, buf, m);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;
                ఒకవేళ (n == 0)
                        ఆపు;

                ప్రతిజ్ఞా((size_t) n <= m);
                msan_unpoison(buf, n);

                FOREACH_DIRENT_లోపల_బఫరు(de, buf, n)
                        ఒకవేళ (!(ignore_hidden_or_backup ? hidden_or_backup_file(de->d_name) : dot_or_dot_dot(de->d_name)))
                                తిరిగి 0;
        }

        తిరిగి 1;
}

bool stat_may_be_dev_null(నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        /* We don't want to hardcode the major/minor of /dev/null, hence we do a simpler "is this a character
         * device node?" check. */

        తిరిగి S_ISCHR(st->st_mode);
}

bool stat_is_ఖాళీలు(నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(st);

        తిరిగి S_ISREG(st->st_mode) && st->st_size <= 0;
}

పూర్ణాంకం null_or_ఖాళీలు_మార్గం_with_మూలం(స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *మూలం) {
        _cleanup_free_ అక్షరం *resolved = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        నిర్మాణం stat st;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fn);

        /* fn includes the root prefix. */

        ఒకవేళ (మార్గం_equal(మార్గం_startswith(fn, మూలం ?: "/"), "dev/null"))
                తిరిగి true;

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం)) {
                ఒకవేళ (stat(fn, &st) < 0)
                        తిరిగి -errno;

                తిరిగి null_or_ఖాళీలు(&st);
        }

        /* An image below a root directory usually has no /dev/null, so a symlink to /dev/null dangles there.
         * CHASE_NONEXISTENT resolves such a symlink without requiring the target to exist. */
        r = chase(fn, మూలం, CHASE_NONEXISTENT, &resolved, &fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (మార్గం_equal(మార్గం_startswith(resolved, మూలం), "dev/null"))
                తిరిగి true;

        ఒకవేళ (r == 0)
                తిరిగి -ENOENT;

        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        తిరిగి null_or_ఖాళీలు(&st);
}

స్థిర స్థిరరాశి అక్షరం* statx_ముసుగు_one_గమ్యం_name(గుర్తులేని ముసుగు_3);
స్థిర స్థిరరాశి అక్షరం* statx_attribute_గమ్యం_name(uint64_t attr);

#అన్తర్భావ "statx-attribute-to-name.inc"
#అన్తర్భావ "statx-mask-to-name.inc"

#నిర్వచించు DEFINE_STATX_బిట్లు_గమ్యం_STRING(ఉపసర్గ, type, func, format_str)             \
        స్థిర అక్షరం* ఉపసర్గ##_to_string(type v) {                               \
                ఒకవేళ (v == 0)                                                     \
                        తిరిగి strdup("");                                      \
                                                                                \
                _cleanup_free_ అక్షరం *s = ఖాళీ;                                  \
                                                                                \
                బిట్టు_FOREACH(i, v) {                                             \
                        type f = 1 << i;                                        \
                                                                                \
                        స్థిరరాశి అక్షరం *n = func(f);                                \
                        ఒకవేళ (!n)                                                 \
                                కొనసాగు;                                       \
                                                                                \
                        ఒకవేళ (!strextend_with_separator(&s, "|", n))              \
                                తిరిగి ఖాళీ;                                    \
                        v &= ~f;                                                \
                }                                                               \
                                                                                \
                ఒకవేళ (v != 0 && strextendf_with_separator(&s, "|", format_str, v) < 0) \
                        తిరిగి ఖాళీ;                                            \
                                                                                \
                తిరిగి TAKE_PTR(s);                                             \
        }

DEFINE_STATX_బిట్లు_గమ్యం_STRING(statx_ముసుగు,       గుర్తులేని, statx_ముసుగు_one_గమ్యం_name,  "0x%x");
DEFINE_STATX_బిట్లు_గమ్యం_STRING(statx_లక్షణాలు, uint64_t, statx_attribute_గమ్యం_name, "0x%" PRIx64);

పూర్ణాంకం xstatx_నిండిన(పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం statx_flags,
                XStatXFlags xstatx_flags,
                గుర్తులేని mandatory_ముసుగు,
                గుర్తులేని optional_ముసుగు,
                uint64_t mandatory_లక్షణాలు,
                నిర్మాణం statx *ret) {

        నిర్మాణం statx sx = {}; /* explicitly initialize the struct to make msan silent. */
        పూర్ణాంకం r;

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

        ప్రతిజ్ఞా(wildcard_fd_is_valid(fd));
        ప్రతిజ్ఞా((mandatory_ముసుగు & optional_ముసుగు) == 0);
        ప్రతిజ్ఞా(!FLAGS_సమితి(xstatx_flags, XSTATX_MNT_ID_ఉత్తమ) || !((mandatory_ముసుగు|optional_ముసుగు) & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)));
        ప్రతిజ్ఞా(ret);

        _cleanup_free_ అక్షరం *p = ఖాళీ;
        r = resolve_xat_fdroot(&fd, &మార్గం, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        గుర్తులేని request_ముసుగు = mandatory_ముసుగు|optional_ముసుగు;
        ఒకవేళ (FLAGS_సమితి(xstatx_flags, XSTATX_MNT_ID_ఉత్తమ))
                request_ముసుగు |= STATX_MNT_ID|STATX_MNT_ID_UNIQUE;

        ఒకవేళ (statx(fd,
                  strempty(మార్గం),
                  statx_flags|(isempty(మార్గం) ? AT_ఖాళీలు_మార్గం : 0),
                  request_ముసుగు,
                  &sx) < 0)
                తిరిగి negative_errno();

        ఒకవేళ (FLAGS_సమితి(xstatx_flags, XSTATX_MNT_ID_ఉత్తమ) &&
            !(sx.stx_ముసుగు & (STATX_MNT_ID|STATX_MNT_ID_UNIQUE)))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EUNATCH), "statx() did not return either STATX_MNT_ID or STATX_MNT_ID_UNIQUE.");

        ఒకవేళ (!FLAGS_సమితి(sx.stx_ముసుగు, mandatory_ముసుగు)) {
                ఒకవేళ (DEBUG_LOGGING) {
                        _cleanup_free_ అక్షరం *ముసుగు_str = statx_ముసుగు_గమ్యం_string(mandatory_ముసుగు & ~sx.stx_ముసుగు);
                        log_debug("statx() does not support '%s' mask (running on an old kernel?)", strnull(ముసుగు_str));
                }

                తిరిగి -EUNATCH;
        }

        ఒకవేళ (!FLAGS_సమితి(sx.stx_లక్షణాలు_ముసుగు, mandatory_లక్షణాలు)) {
                ఒకవేళ (DEBUG_LOGGING) {
                        _cleanup_free_ అక్షరం *attr_str = statx_లక్షణాలు_గమ్యం_string(mandatory_లక్షణాలు & ~sx.stx_లక్షణాలు_ముసుగు);
                        log_debug("statx() does not support '%s' attribute (running on an old kernel?)", strnull(attr_str));
                }

                తిరిగి -EUNATCH;
        }

        *ret = sx;
        తిరిగి FLAGS_సమితి(sx.stx_ముసుగు, optional_ముసుగు);
}

స్థిర పూర్ణాంకం xfstatfs(పూర్ణాంకం fd, నిర్మాణం statfs *ret) {
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (fd == AT_FDCWD)
                తిరిగి RET_NERRNO(statfs(".", ret));
        ఒకవేళ (fd == XAT_FDROOT)
                తిరిగి RET_NERRNO(statfs("/", ret));

        ప్రతిజ్ఞా(fd >= 0);
        తిరిగి RET_NERRNO(fstatfs(fd, ret));
}

పూర్ణాంకం xstatfsat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, నిర్మాణం statfs *ret) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!isempty(మార్గం)) {
                fd = xopenat(dir_fd, మార్గం, O_మార్గం|O_CLOEXEC);
                ఒకవేళ (fd < 0)
                        తిరిగి fd;
                dir_fd = fd;
        }

        తిరిగి xfstatfs(dir_fd, ret);
}

పూర్ణాంకం fd_is_read_only_fs(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        నిర్మాణం statfs st;
        r = xfstatfs(fd, &st);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (st.f_flags & ST_RDONLY)
                తిరిగి true;

        ఒకవేళ (is_జాలం_fs(&st))
                /* On NFS, fstatfs() might not reflect whether we can actually write to the remote share.
                 * Let's try again with access(W_OK) which is more reliable, at least sometimes. */
                తిరిగి access_fd(fd, W_OK) == -EROFS;

        తిరిగి false;
}

పూర్ణాంకం మార్గం_is_read_only_fs(స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(మార్గం);

        fd = open(మార్గం, O_CLOEXEC | O_మార్గం);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        తిరిగి fd_is_read_only_fs(fd);
}

పూర్ణాంకం inode_same_at(పూర్ణాంకం fda, స్థిరరాశి అక్షరం *filea, పూర్ణాంకం fdb, స్థిరరాశి అక్షరం *fileb, పూర్ణాంకం flags) {
        నిర్మాణం stat sta, stb;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fda >= 0 || fda == AT_FDCWD);
        ప్రతిజ్ఞా(fdb >= 0 || fdb == AT_FDCWD);
        ప్రతిజ్ఞా((flags & ~(AT_ఖాళీలు_మార్గం|AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT)) == 0);

        /* Refuse an unset filea or fileb early unless AT_EMPTY_PATH is set */
        ఒకవేళ ((isempty(filea) || isempty(fileb)) && !FLAGS_సమితి(flags, AT_ఖాళీలు_మార్గం))
                తిరిగి -EINVAL;

        /* Shortcut: comparing the same fd with itself means we can return true */
        ఒకవేళ (fda >= 0 && fda == fdb && isempty(filea) && isempty(fileb) && FLAGS_సమితి(flags, AT_SYMLINK_NOFOLLOW))
                తిరిగి true;

        _cleanup_close_ పూర్ణాంకం pin_a = -EBADF, pin_b = -EBADF;
        ఒకవేళ (!FLAGS_సమితి(flags, AT_NO_AUTOMOUNT)) {
                /* Let's try to use the name_to_handle_at() AT_HANDLE_FID API to identify identical
                 * inodes. We have to issue multiple calls on the same file for that (first, to acquire the
                 * FID, and then to check if .st_dev is actually the same). Hence let's pin the inode in
                 * between via O_PATH, unless we already have an fd for it. */

                ఒకవేళ (!isempty(filea)) {
                        pin_a = openat(fda, filea, O_మార్గం|O_CLOEXEC|(FLAGS_సమితి(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ఒకవేళ (pin_a < 0)
                                తిరిగి -errno;

                        fda = pin_a;
                        filea = ఖాళీ;
                        flags |= AT_ఖాళీలు_మార్గం;
                }

                ఒకవేళ (!isempty(fileb)) {
                        pin_b = openat(fdb, fileb, O_మార్గం|O_CLOEXEC|(FLAGS_సమితి(flags, AT_SYMLINK_NOFOLLOW) ? O_NOFOLLOW : 0));
                        ఒకవేళ (pin_b < 0)
                                తిరిగి -errno;

                        fdb = pin_b;
                        fileb = ఖాళీ;
                        flags |= AT_ఖాళీలు_మార్గం;
                }

                పూర్ణాంకం ntha_flags = at_flags_normalize_follow(flags) & (AT_ఖాళీలు_మార్గం|AT_SYMLINK_FOLLOW);
                _cleanup_free_ నిర్మాణం file_handle *ha = ఖాళీ, *hb = ఖాళీ;
                uint64_t mntida, mntidb;
                పూర్ణాంకం _mntida, _mntidb;

                r = name_గమ్యం_handle_at_try_fid(
                                fda,
                                filea,
                                &ha,
                                &_mntida,
                                &mntida,
                                ntha_flags);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (is_name_గమ్యం_handle_at_fatal_error(r))
                                తిరిగి r;

                        వెళ్ళు fallback;
                }
                bool have_unique_mntid = r > 0;

                ఒకవేళ (!have_unique_mntid)
                        mntida = _mntida;

                r = name_గమ్యం_handle_at_try_fid(
                                fdb,
                                fileb,
                                &hb,
                                have_unique_mntid ? ఖాళీ : &_mntidb, /* if we managed to get unique mnt id for a, insist on that for b */
                                have_unique_mntid ? &mntidb : ఖాళీ,
                                ntha_flags);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (is_name_గమ్యం_handle_at_fatal_error(r))
                                తిరిగి r;

                        వెళ్ళు fallback;
                }
                ఒకవేళ (r == 0) {
                        ప్రతిజ్ఞా(!have_unique_mntid); /* _mntidb was initialized by name_to_handle_at_try_fid() */
                        mntidb = _mntidb;
                }

                /* Now compare the two file handles */
                ఒకవేళ (!file_handle_equal(ha, hb))
                        తిరిగి false;

                /* If the file handles are the same and they come from the same mount ID? Great, then we are
                 * good, they are definitely the same */
                ఒకవేళ (mntida == mntidb)
                        తిరిగి true;

                /* File handles are the same, they are not on the same mount id. This might either be because
                 * they are on two entirely different file systems, that just happen to have the same FIDs
                 * (because they originally where created off the same disk images), or it could be because
                 * they are located on two distinct bind mounts of the same fs. To check that, let's look at
                 * .st_rdev of the inode. We simply reuse the fallback codepath for that, since it checks
                 * exactly that (it checks slightly more, but we don't care.) */
        }

fallback:
        ఒకవేళ (fstatat(fda, strempty(filea), &sta, flags) < 0)
                తిరిగి log_debug_errno(errno, "Cannot stat %s: %m", strna(filea));

        ఒకవేళ (fstatat(fdb, strempty(fileb), &stb, flags) < 0)
                తిరిగి log_debug_errno(errno, "Cannot stat %s: %m", strna(fileb));

        తిరిగి stat_inode_same(&sta, &stb);
}

bool is_fs_type(స్థిరరాశి నిర్మాణం statfs *s, statfs_f_type_t magic_విలువ) {
        ప్రతిజ్ఞా(s);
        assert_cc(పరిమాణం(statfs_f_type_t) >= పరిమాణం(s->f_type));

        తిరిగి F_TYPE_EQUAL(s->f_type, magic_విలువ);
}

పూర్ణాంకం is_fs_type_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, statfs_f_type_t magic_విలువ) {
        పూర్ణాంకం r;

        నిర్మాణం statfs s;
        r = xstatfsat(dir_fd, మార్గం, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి is_fs_type(&s, magic_విలువ);
}

bool is_temporary_fs(స్థిరరాశి నిర్మాణం statfs *s) {
        తిరిగి fs_లోపల_సమూహం(s, FILESYSTEM_సమితి_TEMPORARY);
}

bool is_జాలం_fs(స్థిరరాశి నిర్మాణం statfs *s) {
        తిరిగి fs_లోపల_సమూహం(s, FILESYSTEM_సమితి_జాలం);
}

పూర్ణాంకం fd_is_temporary_fs(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        నిర్మాణం statfs s;
        r = xfstatfs(fd, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి is_temporary_fs(&s);
}

పూర్ణాంకం fd_is_జాలం_fs(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        నిర్మాణం statfs s;
        r = xfstatfs(fd, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి is_జాలం_fs(&s);
}

పూర్ణాంకం మార్గం_is_temporary_fs(స్థిరరాశి అక్షరం *మార్గం) {
        నిర్మాణం statfs s;

        ఒకవేళ (statfs(మార్గం, &s) < 0)
                తిరిగి -errno;

        తిరిగి is_temporary_fs(&s);
}

పూర్ణాంకం మార్గం_is_జాలం_fs(స్థిరరాశి అక్షరం *మార్గం) {
        నిర్మాణం statfs s;

        ఒకవేళ (statfs(మార్గం, &s) < 0)
                తిరిగి -errno;

        తిరిగి is_జాలం_fs(&s);
}

పూర్ణాంకం proc_mounted(శూన్య) {
        /* This is typically used in error path. So, it is better to not overwrite the original errno. */
        PROTECT_ERRNO;
        పూర్ణాంకం r;

        /* A quick check of procfs is properly mounted */

        r = మార్గం_is_fs_type("/proc/", PROC_SUPER_MAGIC);
        ఒకవేళ (r == -ENOENT) /* not mounted at all */
                తిరిగి false;

        తిరిగి r;
}

bool stat_inode_same(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b) {

        /* Returns if the specified stat structure references the same (though possibly modified) inode. Does
         * a thorough check, comparing inode nr, backing device and if the inode is still of the same type. */

        తిరిగి stat_is_సమితి(a) && stat_is_సమితి(b) &&
                ((a->st_mode ^ b->st_mode) & S_IFMT) == 0 &&  /* same inode type */
                a->st_dev == b->st_dev &&
                a->st_ino == b->st_ino;
}

bool stat_inode_unmodified(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b) {

        /* Returns if the specified stat structures reference the same, unmodified inode. This check tries to
         * be reasonably careful when detecting changes: we check both inode and mtime, to cater for file
         * systems where mtimes are fixed to 0 (think: ostree/nixos type installations). We also check file
         * size, backing device, inode type and if this refers to a device not the major/minor.
         *
         * Note that we don't care if file attributes such as ownership or access mode change, this here is
         * about contents of the file. The purpose here is to detect file contents changes, and nothing
         * else. */

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి stat_inode_same(a, b) &&
                a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
                a->st_mtim.tv_nsec == b->st_mtim.tv_nsec &&
                (!S_ISREG(a->st_mode) || a->st_size == b->st_size) && /* if regular file, compare file size */
                (!(S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) || a->st_rdev == b->st_rdev); /* if device node, also compare major/minor, because we can */
}

bool statx_inode_same(స్థిరరాశి నిర్మాణం statx *a, స్థిరరాశి నిర్మాణం statx *b) {

        /* Same as stat_inode_same() but for struct statx */

        ఒకవేళ (!statx_is_సమితి(a) || !statx_is_సమితి(b))
                తిరిగి false;

        ప్రతిజ్ఞా(FLAGS_సమితి(a->stx_ముసుగు, STATX_TYPE|STATX_INO));
        ప్రతిజ్ఞా(FLAGS_సమితి(b->stx_ముసుగు, STATX_TYPE|STATX_INO));

        తిరిగి
                ((a->stx_బహుళకం ^ b->stx_బహుళకం) & S_IFMT) == 0 &&
                a->stx_dev_major == b->stx_dev_major &&
                a->stx_dev_minor == b->stx_dev_minor &&
                a->stx_ino == b->stx_ino;
}

పూర్ణాంకం statx_mount_same(స్థిరరాశి నిర్మాణం statx *a, స్థిరరాశి నిర్మాణం statx *b) {
        ఒకవేళ (!statx_is_సమితి(a) || !statx_is_సమితి(b))
                తిరిగి false;

        ఒకవేళ ((FLAGS_సమితి(a->stx_ముసుగు, STATX_MNT_ID) && FLAGS_సమితి(b->stx_ముసుగు, STATX_MNT_ID)) ||
            (FLAGS_సమితి(a->stx_ముసుగు, STATX_MNT_ID_UNIQUE) && FLAGS_సమితి(b->stx_ముసుగు, STATX_MNT_ID_UNIQUE)))
                తిరిగి a->stx_mnt_id == b->stx_mnt_id;

        తిరిగి -ENODATA;
}

usec_t statx_timestamp_ఎక్కించు(స్థిరరాశి నిర్మాణం statx_timestamp *ts) {
        ప్రతిజ్ఞా(ts);
        తిరిగి timespec_ఎక్కించు(&(స్థిరరాశి నిర్మాణం timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}
nsec_t statx_timestamp_ఎక్కించు_nsec(స్థిరరాశి నిర్మాణం statx_timestamp *ts) {
        ప్రతిజ్ఞా(ts);
        తిరిగి timespec_ఎక్కించు_nsec(&(స్థిరరాశి నిర్మాణం timespec) { .tv_sec = ts->tv_sec, .tv_nsec = ts->tv_nsec });
}

శూన్య inode_hash_func(స్థిరరాశి నిర్మాణం stat *q, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(q);

        siphash24_compress_typesafe(q->st_dev, state);
        siphash24_compress_typesafe(q->st_ino, state);

        /* Also include inode type, to mirror stat_inode_same() */
        mode_t type = q->st_mode & S_IFMT;
        siphash24_compress_typesafe(type, state);
}

పూర్ణాంకం inode_compare_func(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = CMP(a->st_dev, b->st_dev);
        ఒకవేళ (r != 0)
                తిరిగి r;

        r = CMP(a->st_ino, b->st_ino);
        ఒకవేళ (r != 0)
                తిరిగి r;

        తిరిగి CMP(a->st_mode & S_IFMT, b->st_mode & S_IFMT);
}

DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(inode_hash_ops, నిర్మాణం stat, inode_hash_func, inode_compare_func, విడుదల);

శూన్య inode_unmodified_hash_func(స్థిరరాశి నిర్మాణం stat *q, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(q);

        inode_hash_func(q, state);

        siphash24_compress_typesafe(q->st_mtim.tv_sec, state);
        siphash24_compress_typesafe(q->st_mtim.tv_nsec, state);

        ఒకవేళ (S_ISREG(q->st_mode))
                siphash24_compress_typesafe(q->st_size, state);
        లేకపోతే {
                uint64_t invalid = UINT64_MAX;
                siphash24_compress_typesafe(invalid, state);
        }

        ఒకవేళ (S_ISCHR(q->st_mode) || S_ISBLK(q->st_mode))
                siphash24_compress_typesafe(q->st_rdev, state);
        లేకపోతే {
                dev_t invalid = (dev_t) -1;
                siphash24_compress_typesafe(invalid, state);
        }
}

పూర్ణాంకం inode_unmodified_compare_func(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = inode_compare_func(a, b);
        ఒకవేళ (r != 0)
                తిరిగి r;

        r = CMP(a->st_mtim.tv_sec, b->st_mtim.tv_sec);
        ఒకవేళ (r != 0)
                తిరిగి r;

        r = CMP(a->st_mtim.tv_nsec, b->st_mtim.tv_nsec);
        ఒకవేళ (r != 0)
                తిరిగి r;

        ఒకవేళ (S_ISREG(a->st_mode)) {
                r = CMP(a->st_size, b->st_size);
                ఒకవేళ (r != 0)
                        తిరిగి r;
        }

        ఒకవేళ (S_ISCHR(a->st_mode) || S_ISBLK(a->st_mode)) {
                r = CMP(a->st_rdev, b->st_rdev);
                ఒకవేళ (r != 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(inode_unmodified_hash_ops, నిర్మాణం stat, inode_unmodified_hash_func, inode_unmodified_compare_func, విడుదల);

స్థిరరాశి అక్షరం* inode_type_గమ్యం_string(mode_t m) {

        /* Returns a short string for the inode type. We use the same name as the underlying macros for each
         * inode type. */

        ఎంపిక (m & S_IFMT) {
        స్థితి S_IFREG:
                తిరిగి "reg";
        స్థితి S_IFDIR:
                తిరిగి "dir";
        స్థితి S_IFLNK:
                తిరిగి "lnk";
        స్థితి S_IFCHR:
                తిరిగి "chr";
        స్థితి S_IFBLK:
                తిరిగి "blk";
        స్థితి S_IFIFO:
                తిరిగి "fifo";
        స్థితి S_IFSOCK:
                తిరిగి "sock";
        }

        /* Note anonymous inodes in the kernel will have a zero type. Hence fstat() of an eventfd() will
         * return an .st_mode where we'll return NULL here! */
        తిరిగి ఖాళీ;
}

mode_t inode_type_from_string(స్థిరరాశి అక్షరం *s) {
        ఒకవేళ (!s)
                తిరిగి బహుళకం_INVALID;

        ఒకవేళ (streq(s, "reg"))
                తిరిగి S_IFREG;
        ఒకవేళ (streq(s, "dir"))
                తిరిగి S_IFDIR;
        ఒకవేళ (streq(s, "lnk"))
                తిరిగి S_IFLNK;
        ఒకవేళ (streq(s, "chr"))
                తిరిగి S_IFCHR;
        ఒకవేళ (streq(s, "blk"))
                తిరిగి S_IFBLK;
        ఒకవేళ (streq(s, "fifo"))
                తిరిగి S_IFIFO;
        ఒకవేళ (streq(s, "sock"))
                తిరిగి S_IFSOCK;

        తిరిగి బహుళకం_INVALID;
}

పూర్ణాంకం vfs_free_బైట్లు(పూర్ణాంకం fd, uint64_t *ret) {
        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        /* Safely returns the current available disk space (for root, i.e. including any space reserved for
         * root) of the disk referenced by the fd, converted to bytes. */

        నిర్మాణం statvfs sv;
        ఒకవేళ (fstatvfs(fd, &sv) < 0)
                తిరిగి -errno;

        ఒకవేళ (!MUL_సురక్షిత(ret, (uint64_t) sv.f_frsize, (uint64_t) sv.f_bfree))
                తిరిగి -ERANGE;

        తిరిగి 0;
}
