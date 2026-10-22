/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণাঙ্ক dirent_ensure_type(পূর্ণাঙ্ক dir_fd, সংরচনা dirent *de) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0);
        প্রতিজ্ঞা(de);

        যদি (de->d_type != DT_UNKNOWN)
                প্রত্যাগম 0;

        যদি (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                প্রত্যাগম 0;
        }

        /* Let's ask only for the type, nothing else. */
        সংরচনা statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতিজ্ঞা(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        যদি (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        প্রত্যাগম 0;
}

bool dirent_is_file(ধ্রুব সংরচনা dirent *de) {
        প্রতিজ্ঞা(de);

        যদি (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                প্রত্যাগম false;

        যদি (hidden_or_backup_file(de->d_name))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool dirent_is_file_with_suffix(ধ্রুব সংরচনা dirent *de, ধ্রুব বর্ণ *suffix) {
        প্রতিজ্ঞা(de);

        যদি (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                প্রত্যাগম false;

        যদি (de->d_name[0] == '.')
                প্রত্যাগম false;

        যদি (!suffix)
                প্রত্যাগম true;

        প্রত্যাগম endswith(de->d_name, suffix);
}

সংরচনা dirent* readdir_ensure_type(DIR *d) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        প্রতি (;;) {
                সংরচনা dirent *de;

                errno = 0;
                de = readdir(d);
                যদি (!de)
                        প্রত্যাগম রিক্ত;

                r = dirent_ensure_type(dirfd(d), de);
                যদি (r >= 0)
                        প্রত্যাগম de;
                যদি (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        প্রত্যাগম রিক্ত;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

সংরচনা dirent* readdir_no_dot(DIR *d) {
        প্রতিজ্ঞা(d);

        প্রতি (;;) {
                সংরচনা dirent *de;

                de = readdir_ensure_type(d);
                যদি (!de || !dot_or_dot_dot(de->d_name))
                        প্রত্যাগম de;
        }
}
