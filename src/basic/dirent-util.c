/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/stat.h>

#شامل "dirent-util.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"

صحیح dirent_ensure_type(صحیح dir_fd, ساختار dirent *de) {
        صحیح r;

        أكد(dir_fd >= 0);
        أكد(de);

        اگر (de->d_type != DT_UNKNOWN)
                بازگشت 0;

        اگر (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                بازگشت 0;
        }

        /* Let's ask only for the type, nothing else. */
        ساختار statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        اگر (r < 0)
                بازگشت r;

        أكد(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        اگر (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        بازگشت 0;
}

bool dirent_is_file(ثابت ساختار dirent *de) {
        أكد(de);

        اگر (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                بازگشت false;

        اگر (hidden_or_backup_file(de->d_name))
                بازگشت false;

        بازگشت true;
}

bool dirent_is_file_with_suffix(ثابت ساختار dirent *de, ثابت نویسه *suffix) {
        أكد(de);

        اگر (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                بازگشت false;

        اگر (de->d_name[0] == '.')
                بازگشت false;

        اگر (!suffix)
                بازگشت true;

        بازگشت endswith(de->d_name, suffix);
}

ساختار dirent* readdir_ensure_type(DIR *d) {
        صحیح r;

        أكد(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        برای (;;) {
                ساختار dirent *de;

                errno = 0;
                de = readdir(d);
                اگر (!de)
                        بازگشت پوچ;

                r = dirent_ensure_type(dirfd(d), de);
                اگر (r >= 0)
                        بازگشت de;
                اگر (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        بازگشت پوچ;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ساختار dirent* readdir_no_dot(DIR *d) {
        أكد(d);

        برای (;;) {
                ساختار dirent *de;

                de = readdir_ensure_type(d);
                اگر (!de || !dot_or_dot_dot(de->d_name))
                        بازگشت de;
        }
}
