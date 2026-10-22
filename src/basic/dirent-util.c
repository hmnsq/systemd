/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"

पूर्णांक dirent_ensure_type(पूर्णांक dir_fd, संरचना dirent *de) {
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0);
        प्रतिज्ञा(de);

        जर (de->d_type != DT_UNKNOWN)
                परत 0;

        जर (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                परत 0;
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
        जर (r < 0)
                परत r;

        प्रतिज्ञा(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        जर (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        परत 0;
}

bool dirent_is_file(स्थिरांक संरचना dirent *de) {
        प्रतिज्ञा(de);

        जर (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                परत false;

        जर (hidden_or_backup_file(de->d_name))
                परत false;

        परत true;
}

bool dirent_is_file_with_suffix(स्थिरांक संरचना dirent *de, स्थिरांक अक्षर *suffix) {
        प्रतिज्ञा(de);

        जर (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                परत false;

        जर (de->d_name[0] == '.')
                परत false;

        जर (!suffix)
                परत true;

        परत endswith(de->d_name, suffix);
}

संरचना dirent* readdir_ensure_type(सूची *d) {
        पूर्णांक r;

        प्रतिज्ञा(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        साठी (;;) {
                संरचना dirent *de;

                त्रुटि_सं = 0;
                de = सूची_पढ़ो(d);
                जर (!de)
                        परत रिक्त;

                r = dirent_ensure_type(dirfd(d), de);
                जर (r >= 0)
                        परत de;
                जर (r != -ENOENT) {
                        त्रुटि_सं = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        परत रिक्त;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

संरचना dirent* readdir_no_dot(सूची *d) {
        प्रतिज्ञा(d);

        साठी (;;) {
                संरचना dirent *de;

                de = readdir_ensure_type(d);
                जर (!de || !dot_or_dot_dot(de->d_name))
                        परत de;
        }
}
