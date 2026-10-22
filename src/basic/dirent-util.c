/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ dirent_ensure_type(ಪೂರ್ಣಾಂಕ dir_fd, ರಚನೆ dirent *de) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(de);

        ಆದರೆ (de->d_type != DT_UNKNOWN)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ಹಿಂತಿರುಗಿ 0;
        }

        /* Let's ask only for the type, nothing else. */
        ರಚನೆ statx sx;
        r = xstatx_ತುಂಬಿದ(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(sx.stx_ಮುಖವಾಡ, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_ಬಹುಲಕ);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ಆದರೆ (FLAGS_ಗಣ(sx.stx_ಮುಖವಾಡ, STATX_INO))
                de->d_ino = sx.stx_ino;

        ಹಿಂತಿರುಗಿ 0;
}

bool dirent_is_file(ಸ್ಥಿರಾಂಕ ರಚನೆ dirent *de) {
        ಪ್ರತಿಜ್ಞಾ(de);

        ಆದರೆ (!ಒಳ_ಗಣ(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (hidden_or_backup_file(de->d_name))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool dirent_is_file_with_suffix(ಸ್ಥಿರಾಂಕ ರಚನೆ dirent *de, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix) {
        ಪ್ರತಿಜ್ಞಾ(de);

        ಆದರೆ (!ಒಳ_ಗಣ(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (de->d_name[0] == '.')
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!suffix)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ endswith(de->d_name, suffix);
}

ರಚನೆ dirent* readdir_ensure_type(DIR *d) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        ಗಾಗಿ (;;) {
                ರಚನೆ dirent *de;

                errno = 0;
                de = readdir(d);
                ಆದರೆ (!de)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                r = dirent_ensure_type(dirfd(d), de);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ de;
                ಆದರೆ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ರಚನೆ dirent* readdir_no_dot(DIR *d) {
        ಪ್ರತಿಜ್ಞಾ(d);

        ಗಾಗಿ (;;) {
                ರಚನೆ dirent *de;

                de = readdir_ensure_type(d);
                ಆದರೆ (!de || !dot_or_dot_dot(de->d_name))
                        ಹಿಂತಿರುಗಿ de;
        }
}
