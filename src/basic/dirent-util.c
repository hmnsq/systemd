/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"

முழு dirent_ensure_type(முழு dir_fd, கட்டமைப்பு dirent *de) {
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0);
        ப்ரதிஜ்ஞா(de);

        எனில் (de->d_type != DT_UNKNOWN)
                திரும்பு 0;

        எனில் (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                திரும்பு 0;
        }

        /* Let's ask only for the type, nothing else. */
        கட்டமைப்பு statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        எனில் (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        திரும்பு 0;
}

bool dirent_is_file(மாறிலி கட்டமைப்பு dirent *de) {
        ப்ரதிஜ்ஞா(de);

        எனில் (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                திரும்பு false;

        எனில் (hidden_or_backup_file(de->d_name))
                திரும்பு false;

        திரும்பு true;
}

bool dirent_is_file_with_suffix(மாறிலி கட்டமைப்பு dirent *de, மாறிலி எழுத்து *suffix) {
        ப்ரதிஜ்ஞா(de);

        எனில் (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                திரும்பு false;

        எனில் (de->d_name[0] == '.')
                திரும்பு false;

        எனில் (!suffix)
                திரும்பு true;

        திரும்பு endswith(de->d_name, suffix);
}

கட்டமைப்பு dirent* readdir_ensure_type(DIR *d) {
        முழு r;

        ப்ரதிஜ்ஞா(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        க்கு (;;) {
                கட்டமைப்பு dirent *de;

                errno = 0;
                de = readdir(d);
                எனில் (!de)
                        திரும்பு வெற்று;

                r = dirent_ensure_type(dirfd(d), de);
                எனில் (r >= 0)
                        திரும்பு de;
                எனில் (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        திரும்பு வெற்று;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

கட்டமைப்பு dirent* readdir_no_dot(DIR *d) {
        ப்ரதிஜ்ஞா(d);

        க்கு (;;) {
                கட்டமைப்பு dirent *de;

                de = readdir_ensure_type(d);
                எனில் (!de || !dot_or_dot_dot(de->d_name))
                        திரும்பு de;
        }
}
