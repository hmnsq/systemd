/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क dirent_ensure_type(पूर्णाङ्क dir_fd, संरचना dirent *de) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0);
        प्रतिज्ञा(de);

        यदि (de->d_type != DT_UNKNOWN)
                प्रत्यागम 0;

        यदि (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                प्रत्यागम 0;
        }

        /* Let's ask only for the type, nothing else. */
        संरचना statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        यदि (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        प्रत्यागम 0;
}

bool dirent_is_file(ध्रुव संरचना dirent *de) {
        प्रतिज्ञा(de);

        यदि (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                प्रत्यागम false;

        यदि (hidden_or_backup_file(de->d_name))
                प्रत्यागम false;

        प्रत्यागम true;
}

bool dirent_is_file_with_suffix(ध्रुव संरचना dirent *de, ध्रुव वर्ण *suffix) {
        प्रतिज्ञा(de);

        यदि (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                प्रत्यागम false;

        यदि (de->d_name[0] == '.')
                प्रत्यागम false;

        यदि (!suffix)
                प्रत्यागम true;

        प्रत्यागम endswith(de->d_name, suffix);
}

संरचना dirent* readdir_ensure_type(सूची *d) {
        पूर्णाङ्क r;

        प्रतिज्ञा(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        प्रति (;;) {
                संरचना dirent *de;

                त्रुटि_सं = 0;
                de = सूची_पढ़ो(d);
                यदि (!de)
                        प्रत्यागम रिक्त;

                r = dirent_ensure_type(dirfd(d), de);
                यदि (r >= 0)
                        प्रत्यागम de;
                यदि (r != -ENOENT) {
                        त्रुटि_सं = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        प्रत्यागम रिक्त;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

संरचना dirent* readdir_no_dot(सूची *d) {
        प्रतिज्ञा(d);

        प्रति (;;) {
                संरचना dirent *de;

                de = readdir_ensure_type(d);
                यदि (!de || !dot_or_dot_dot(de->d_name))
                        प्रत्यागम de;
        }
}
