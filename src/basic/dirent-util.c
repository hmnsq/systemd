/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/stat.h>

#شامل "dirent-util.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"

صحیح dirent_ensure_type(صحیح dir_fd, ساخت dirent *de) {
        صحیح r;

        أكد(dir_fd >= 0);
        أكد(de);

        اگر (de->d_type != DT_UNKNOWN)
                واپس 0;

        اگر (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                واپس 0;
        }

        /* Let's ask only for the type, nothing else. */
        ساخت statx sx;
        r = xstatx_بھرا(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        اگر (r < 0)
                واپس r;

        أكد(FLAGS_سیٹ(sx.stx_نقاب, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_منوال);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        اگر (FLAGS_سیٹ(sx.stx_نقاب, STATX_INO))
                de->d_ino = sx.stx_ino;

        واپس 0;
}

bool dirent_is_file(ثابت ساخت dirent *de) {
        أكد(de);

        اگر (!اندر_سیٹ(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                واپس false;

        اگر (hidden_or_backup_file(de->d_name))
                واپس false;

        واپس true;
}

bool dirent_is_file_with_suffix(ثابت ساخت dirent *de, ثابت حرف *suffix) {
        أكد(de);

        اگر (!اندر_سیٹ(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                واپس false;

        اگر (de->d_name[0] == '.')
                واپس false;

        اگر (!suffix)
                واپس true;

        واپس endswith(de->d_name, suffix);
}

ساخت dirent* readdir_ensure_type(DIR *d) {
        صحیح r;

        أكد(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        برائے (;;) {
                ساخت dirent *de;

                errno = 0;
                de = readdir(d);
                اگر (!de)
                        واپس تہی;

                r = dirent_ensure_type(dirfd(d), de);
                اگر (r >= 0)
                        واپس de;
                اگر (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        واپس تہی;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ساخت dirent* readdir_no_dot(DIR *d) {
        أكد(d);

        برائے (;;) {
                ساخت dirent *de;

                de = readdir_ensure_type(d);
                اگر (!de || !dot_or_dot_dot(de->d_name))
                        واپس de;
        }
}
