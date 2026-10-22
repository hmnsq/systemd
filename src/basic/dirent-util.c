/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ "dirent-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক dirent_ensure_type(পূর্ণাঙ্ক dir_fd, সংরচনা dirent *de) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0);
        প্রতিজ্ঞা(de);

        যদি (de->d_type != DT_UNKNOWN)
                ঘূৰাই 0;

        যদি (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ঘূৰাই 0;
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
                ঘূৰাই r;

        প্রতিজ্ঞা(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        যদি (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ঘূৰাই 0;
}

bool dirent_is_file(ধ্রুৱ সংরচনা dirent *de) {
        প্রতিজ্ঞা(de);

        যদি (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ঘূৰাই false;

        যদি (hidden_or_backup_file(de->d_name))
                ঘূৰাই false;

        ঘূৰাই true;
}

bool dirent_is_file_with_suffix(ধ্রুৱ সংরচনা dirent *de, ধ্রুৱ ৱর্ণ *suffix) {
        প্রতিজ্ঞা(de);

        যদি (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ঘূৰাই false;

        যদি (de->d_name[0] == '.')
                ঘূৰাই false;

        যদি (!suffix)
                ঘূৰাই true;

        ঘূৰাই endswith(de->d_name, suffix);
}

সংরচনা dirent* readdir_ensure_type(DIR *d) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        বাবে (;;) {
                সংরচনা dirent *de;

                errno = 0;
                de = readdir(d);
                যদি (!de)
                        ঘূৰাই রিক্ত;

                r = dirent_ensure_type(dirfd(d), de);
                যদি (r >= 0)
                        ঘূৰাই de;
                যদি (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ঘূৰাই রিক্ত;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

সংরচনা dirent* readdir_no_dot(DIR *d) {
        প্রতিজ্ঞা(d);

        বাবে (;;) {
                সংরচনা dirent *de;

                de = readdir_ensure_type(d);
                যদি (!de || !dot_or_dot_dot(de->d_name))
                        ঘূৰাই de;
        }
}
