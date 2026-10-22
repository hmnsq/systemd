/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <sys/stat.h>

#समावेश "dirent-util.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"

पूर्णांक dirent_ensure_type(पूर्णांक dir_fd, काष्ठा dirent *de) {
        पूर्णांक r;

        निश्चित(dir_fd >= 0);
        निश्चित(de);

        अगर (de->d_type != DT_UNKNOWN)
                वापस 0;

        अगर (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                वापस 0;
        }

        /* Let's ask only for the type, nothing else. */
        काष्ठा statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        अगर (r < 0)
                वापस r;

        निश्चित(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        अगर (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        वापस 0;
}

bool dirent_is_file(स्थिर काष्ठा dirent *de) {
        निश्चित(de);

        अगर (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                वापस false;

        अगर (hidden_or_backup_file(de->d_name))
                वापस false;

        वापस true;
}

bool dirent_is_file_with_suffix(स्थिर काष्ठा dirent *de, स्थिर अक्षर *suffix) {
        निश्चित(de);

        अगर (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                वापस false;

        अगर (de->d_name[0] == '.')
                वापस false;

        अगर (!suffix)
                वापस true;

        वापस endswith(de->d_name, suffix);
}

काष्ठा dirent* readdir_ensure_type(सूची *d) {
        पूर्णांक r;

        निश्चित(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        क्रम (;;) {
                काष्ठा dirent *de;

                त्रुटि_सं = 0;
                de = सूची_पढ़ो(d);
                अगर (!de)
                        वापस शून्य;

                r = dirent_ensure_type(dirfd(d), de);
                अगर (r >= 0)
                        वापस de;
                अगर (r != -ENOENT) {
                        त्रुटि_सं = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        वापस शून्य;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

काष्ठा dirent* readdir_no_dot(सूची *d) {
        निश्चित(d);

        क्रम (;;) {
                काष्ठा dirent *de;

                de = readdir_ensure_type(d);
                अगर (!de || !dot_or_dot_dot(de->d_name))
                        वापस de;
        }
}
