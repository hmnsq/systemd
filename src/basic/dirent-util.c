/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা dirent_ensure_type(পূর্ণসংখ্যা dir_fd, কাঠামো dirent *de) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(dir_fd >= 0);
        প্রতিজ্ঞা(de);

        যদি (de->d_type != DT_UNKNOWN)
                ফেরত 0;

        যদি (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ফেরত 0;
        }

        /* Let's ask only for the type, nothing else. */
        কাঠামো statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        যদি (r < 0)
                ফেরত r;

        প্রতিজ্ঞা(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        যদি (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ফেরত 0;
}

bool dirent_is_file(ধ্রুবক কাঠামো dirent *de) {
        প্রতিজ্ঞা(de);

        যদি (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ফেরত false;

        যদি (hidden_or_backup_file(de->d_name))
                ফেরত false;

        ফেরত true;
}

bool dirent_is_file_with_suffix(ধ্রুবক কাঠামো dirent *de, ধ্রুবক অক্ষর *suffix) {
        প্রতিজ্ঞা(de);

        যদি (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ফেরত false;

        যদি (de->d_name[0] == '.')
                ফেরত false;

        যদি (!suffix)
                ফেরত true;

        ফেরত endswith(de->d_name, suffix);
}

কাঠামো dirent* readdir_ensure_type(DIR *d) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        জন্য (;;) {
                কাঠামো dirent *de;

                errno = 0;
                de = readdir(d);
                যদি (!de)
                        ফেরত ফাঁকা;

                r = dirent_ensure_type(dirfd(d), de);
                যদি (r >= 0)
                        ফেরত de;
                যদি (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ফেরত ফাঁকা;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

কাঠামো dirent* readdir_no_dot(DIR *d) {
        প্রতিজ্ঞা(d);

        জন্য (;;) {
                কাঠামো dirent *de;

                de = readdir_ensure_type(d);
                যদি (!de || !dot_or_dot_dot(de->d_name))
                        ফেরত de;
        }
}
