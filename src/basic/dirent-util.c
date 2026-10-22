/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ dirent_ensure_type(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಸಂರಚನಾ dirent *de) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(de);

        ಯದಿ (de->d_type != DT_UNKNOWN)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* Let's ask only for the type, nothing else. */
        ಸಂರಚನಾ statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ಯದಿ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool dirent_is_file(ಧ್ರುವ ಸಂರಚನಾ dirent *de) {
        ಪ್ರತಿಜ್ಞಾ(de);

        ಯದಿ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (hidden_or_backup_file(de->d_name))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool dirent_is_file_with_suffix(ಧ್ರುವ ಸಂರಚನಾ dirent *de, ಧ್ರುವ ವರ್ಣ *suffix) {
        ಪ್ರತಿಜ್ಞಾ(de);

        ಯದಿ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (de->d_name[0] == '.')
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!suffix)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ endswith(de->d_name, suffix);
}

ಸಂರಚನಾ dirent* readdir_ensure_type(DIR *d) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        ಪ್ರತಿ (;;) {
                ಸಂರಚನಾ dirent *de;

                errno = 0;
                de = readdir(d);
                ಯದಿ (!de)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                r = dirent_ensure_type(dirfd(d), de);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ de;
                ಯದಿ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ಸಂರಚನಾ dirent* readdir_no_dot(DIR *d) {
        ಪ್ರತಿಜ್ಞಾ(d);

        ಪ್ರತಿ (;;) {
                ಸಂರಚನಾ dirent *de;

                de = readdir_ensure_type(d);
                ಯದಿ (!de || !dot_or_dot_dot(de->d_name))
                        ಪ್ರತ್ಯಾಗಮ de;
        }
}
