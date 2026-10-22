/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક dirent_ensure_type(પૂર્ણાંક dir_fd, સંરચના dirent *de) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0);
        પ્રતિજ્ઞા(de);

        જો (de->d_type != DT_UNKNOWN)
                પાછા 0;

        જો (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                પાછા 0;
        }

        /* Let's ask only for the type, nothing else. */
        સંરચના statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        જો (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        પાછા 0;
}

bool dirent_is_file(અચલ સંરચના dirent *de) {
        પ્રતિજ્ઞા(de);

        જો (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                પાછા false;

        જો (hidden_or_backup_file(de->d_name))
                પાછા false;

        પાછા true;
}

bool dirent_is_file_with_suffix(અચલ સંરચના dirent *de, અચલ અક્ષર *suffix) {
        પ્રતિજ્ઞા(de);

        જો (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                પાછા false;

        જો (de->d_name[0] == '.')
                પાછા false;

        જો (!suffix)
                પાછા true;

        પાછા endswith(de->d_name, suffix);
}

સંરચના dirent* readdir_ensure_type(DIR *d) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        માટે (;;) {
                સંરચના dirent *de;

                errno = 0;
                de = readdir(d);
                જો (!de)
                        પાછા ખાલી;

                r = dirent_ensure_type(dirfd(d), de);
                જો (r >= 0)
                        પાછા de;
                જો (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        પાછા ખાલી;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

સંરચના dirent* readdir_no_dot(DIR *d) {
        પ્રતિજ્ઞા(d);

        માટે (;;) {
                સંરચના dirent *de;

                de = readdir_ensure_type(d);
                જો (!de || !dot_or_dot_dot(de->d_name))
                        પાછા de;
        }
}
