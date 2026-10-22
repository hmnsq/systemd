/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <sys/stat.h>

#ضمن "dirent-util.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"

صحيح dirent_ensure_type(صحيح dir_fd, بنية dirent *de) {
        صحيح r;

        أكد(dir_fd >= 0);
        أكد(de);

        𐤀𐤌 (de->d_type != DT_UNKNOWN)
                أرجع 0;

        𐤀𐤌 (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                أرجع 0;
        }

        /* Let's ask only for the type, nothing else. */
        بنية statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        𐤀𐤌 (r < 0)
                أرجع r;

        أكد(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        𐤀𐤌 (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        أرجع 0;
}

bool dirent_is_file(ثابت بنية dirent *de) {
        أكد(de);

        𐤀𐤌 (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                أرجع false;

        𐤀𐤌 (hidden_or_backup_file(de->d_name))
                أرجع false;

        أرجع true;
}

bool dirent_is_file_with_suffix(ثابت بنية dirent *de, ثابت حرف *suffix) {
        أكد(de);

        𐤀𐤌 (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                أرجع false;

        𐤀𐤌 (de->d_name[0] == '.')
                أرجع false;

        𐤀𐤌 (!suffix)
                أرجع true;

        أرجع endswith(de->d_name, suffix);
}

بنية dirent* readdir_ensure_type(DIR *d) {
        صحيح r;

        أكد(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        لكل (;;) {
                بنية dirent *de;

                errno = 0;
                de = readdir(d);
                𐤀𐤌 (!de)
                        أرجع عدم;

                r = dirent_ensure_type(dirfd(d), de);
                𐤀𐤌 (r >= 0)
                        أرجع de;
                𐤀𐤌 (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        أرجع عدم;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

بنية dirent* readdir_no_dot(DIR *d) {
        أكد(d);

        لكل (;;) {
                بنية dirent *de;

                de = readdir_ensure_type(d);
                𐤀𐤌 (!de || !dot_or_dot_dot(de->d_name))
                        أرجع de;
        }
}
