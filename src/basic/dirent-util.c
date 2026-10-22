/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ dirent_ensure_type(ਪੂਰਨ dir_fd, ਬਣਤਰ dirent *de) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(de);

        ਜੇ (de->d_type != DT_UNKNOWN)
                ਵਾਪਸ 0;

        ਜੇ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ਵਾਪਸ 0;
        }

        /* Let's ask only for the type, nothing else. */
        ਬਣਤਰ statx sx;
        r = xstatx_ਭਰਿਆ(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(sx.stx_ਨਕਾਬ, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_ਬਹੁਲਕ);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ਜੇ (FLAGS_ਸੈੱਟ(sx.stx_ਨਕਾਬ, STATX_INO))
                de->d_ino = sx.stx_ino;

        ਵਾਪਸ 0;
}

bool dirent_is_file(ਅਚਲ ਬਣਤਰ dirent *de) {
        ਪ੍ਰਤਿਜ੍ਞਾ(de);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ਵਾਪਸ false;

        ਜੇ (hidden_or_backup_file(de->d_name))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool dirent_is_file_with_suffix(ਅਚਲ ਬਣਤਰ dirent *de, ਅਚਲ ਅੱਖਰ *suffix) {
        ਪ੍ਰਤਿਜ੍ਞਾ(de);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ਵਾਪਸ false;

        ਜੇ (de->d_name[0] == '.')
                ਵਾਪਸ false;

        ਜੇ (!suffix)
                ਵਾਪਸ true;

        ਵਾਪਸ endswith(de->d_name, suffix);
}

ਬਣਤਰ dirent* readdir_ensure_type(DIR *d) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        ਲਈ (;;) {
                ਬਣਤਰ dirent *de;

                errno = 0;
                de = readdir(d);
                ਜੇ (!de)
                        ਵਾਪਸ ਨਿਰਾ;

                r = dirent_ensure_type(dirfd(d), de);
                ਜੇ (r >= 0)
                        ਵਾਪਸ de;
                ਜੇ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ਵਾਪਸ ਨਿਰਾ;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ਬਣਤਰ dirent* readdir_no_dot(DIR *d) {
        ਪ੍ਰਤਿਜ੍ਞਾ(d);

        ਲਈ (;;) {
                ਬਣਤਰ dirent *de;

                de = readdir_ensure_type(d);
                ਜੇ (!de || !dot_or_dot_dot(de->d_name))
                        ਵਾਪਸ de;
        }
}
