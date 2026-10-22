/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirent_ensure_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ସଂରଚନା dirent *de) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0);
        ପ୍ରତିଜ୍ଞା(de);

        ଯଦି (de->d_type != DT_UNKNOWN)
                ଫେରନ୍ତୁ 0;

        ଯଦି (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                ଫେରନ୍ତୁ 0;
        }

        /* Let's ask only for the type, nothing else. */
        ସଂରଚନା statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        ଯଦି (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        ଫେରନ୍ତୁ 0;
}

bool dirent_is_file(ଅଚଳ ସଂରଚନା dirent *de) {
        ପ୍ରତିଜ୍ଞା(de);

        ଯଦି (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ଫେରନ୍ତୁ false;

        ଯଦି (hidden_or_backup_file(de->d_name))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool dirent_is_file_with_suffix(ଅଚଳ ସଂରଚନା dirent *de, ଅଚଳ ଅକ୍ଷର *suffix) {
        ପ୍ରତିଜ୍ଞା(de);

        ଯଦି (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                ଫେରନ୍ତୁ false;

        ଯଦି (de->d_name[0] == '.')
                ଫେରନ୍ତୁ false;

        ଯଦି (!suffix)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ endswith(de->d_name, suffix);
}

ସଂରଚନା dirent* readdir_ensure_type(DIR *d) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        ପାଇଁ (;;) {
                ସଂରଚନା dirent *de;

                errno = 0;
                de = readdir(d);
                ଯଦି (!de)
                        ଫେରନ୍ତୁ ଖାଲି;

                r = dirent_ensure_type(dirfd(d), de);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ de;
                ଯଦି (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        ଫେରନ୍ତୁ ଖାଲି;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ସଂରଚନା dirent* readdir_no_dot(DIR *d) {
        ପ୍ରତିଜ୍ଞା(d);

        ପାଇଁ (;;) {
                ସଂରଚନା dirent *de;

                de = readdir_ensure_type(d);
                ଯଦି (!de || !dot_or_dot_dot(de->d_name))
                        ଫେରନ୍ତୁ de;
        }
}
