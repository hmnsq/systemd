/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>

#include "dirent-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"

ចំនួនគត់ dirent_ensure_type(ចំនួនគត់ dir_fd, sa_nracanaa dirent *de) {
        ចំនួនគត់ r;

        assert(dir_fd >= 0);
        assert(de);

        ប្រសិនបើ (de->d_type != DT_UNKNOWN)
                ត្រឡប់ 0;

        ប្រសិនបើ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ត្រឡប់ 0;
        }

        /* Let's ask only for the type, nothing else. */
        sa_nracanaa statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        assert(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ប្រសិនបើ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ត្រឡប់ 0;
}

bool dirent_is_file(const sa_nracanaa dirent *de) {
        assert(de);

        ប្រសិនបើ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ត្រឡប់ false;

        ប្រសិនបើ (hidden_or_backup_file(de->d_name))
                ត្រឡប់ false;

        ត្រឡប់ true;
}

bool dirent_is_file_with_suffix(const sa_nracanaa dirent *de, const var_na *suffix) {
        assert(de);

        ប្រសិនបើ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ត្រឡប់ false;

        ប្រសិនបើ (de->d_name[0] == '.')
                ត្រឡប់ false;

        ប្រសិនបើ (!suffix)
                ត្រឡប់ true;

        ត្រឡប់ endswith(de->d_name, suffix);
}

sa_nracanaa dirent* readdir_ensure_type(DIR *d) {
        ចំនួនគត់ r;

        assert(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        សម្រាប់ (;;) {
                sa_nracanaa dirent *de;

                errno = 0;
                de = readdir(d);
                ប្រសិនបើ (!de)
                        ត្រឡប់ NULL;

                r = dirent_ensure_type(dirfd(d), de);
                ប្រសិនបើ (r >= 0)
                        ត្រឡប់ de;
                ប្រសិនបើ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ត្រឡប់ NULL;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

sa_nracanaa dirent* readdir_no_dot(DIR *d) {
        assert(d);

        សម្រាប់ (;;) {
                sa_nracanaa dirent *de;

                de = readdir_ensure_type(d);
                ប្រសិនបើ (!de || !dot_or_dot_dot(de->d_name))
                        ត្រឡប់ de;
        }
}
