/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం dirent_ensure_type(పూర్ణాంకం dir_fd, నిర్మాణం dirent *de) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0);
        ప్రతిజ్ఞా(de);

        ఒకవేళ (de->d_type != DT_UNKNOWN)
                తిరిగి 0;

        ఒకవేళ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                తిరిగి 0;
        }

        /* Let's ask only for the type, nothing else. */
        నిర్మాణం statx sx;
        r = xstatx_నిండిన(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(FLAGS_సమితి(sx.stx_ముసుగు, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_బహుళకం);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ఒకవేళ (FLAGS_సమితి(sx.stx_ముసుగు, STATX_INO))
                de->d_ino = sx.stx_ino;

        తిరిగి 0;
}

bool dirent_is_file(స్థిరరాశి నిర్మాణం dirent *de) {
        ప్రతిజ్ఞా(de);

        ఒకవేళ (!లోపల_సమితి(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                తిరిగి false;

        ఒకవేళ (hidden_or_backup_file(de->d_name))
                తిరిగి false;

        తిరిగి true;
}

bool dirent_is_file_with_suffix(స్థిరరాశి నిర్మాణం dirent *de, స్థిరరాశి అక్షరం *suffix) {
        ప్రతిజ్ఞా(de);

        ఒకవేళ (!లోపల_సమితి(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                తిరిగి false;

        ఒకవేళ (de->d_name[0] == '.')
                తిరిగి false;

        ఒకవేళ (!suffix)
                తిరిగి true;

        తిరిగి endswith(de->d_name, suffix);
}

నిర్మాణం dirent* readdir_ensure_type(DIR *d) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        కొరకు (;;) {
                నిర్మాణం dirent *de;

                errno = 0;
                de = readdir(d);
                ఒకవేళ (!de)
                        తిరిగి ఖాళీ;

                r = dirent_ensure_type(dirfd(d), de);
                ఒకవేళ (r >= 0)
                        తిరిగి de;
                ఒకవేళ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        తిరిగి ఖాళీ;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

నిర్మాణం dirent* readdir_no_dot(DIR *d) {
        ప్రతిజ్ఞా(d);

        కొరకు (;;) {
                నిర్మాణం dirent *de;

                de = readdir_ensure_type(d);
                ఒకవేళ (!de || !dot_or_dot_dot(de->d_name))
                        తిరిగి de;
        }
}
