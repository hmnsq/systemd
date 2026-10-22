/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ dirent_ensure_type(പൂർണ്ണസംഖ്യ dir_fd, ഘടന dirent *de) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0);
        പ്രതിജ്ഞാ(de);

        എങ്കിൽ (de->d_type != DT_UNKNOWN)
                തിരികെ 0;

        എങ്കിൽ (dot_or_dot_dot(de->d_name)) {
                de->d_type = DT_DIR;
                തിരികെ 0;
        }

        /* Let's ask only for the type, nothing else. */
        ഘടന statx sx;
        r = xstatx_full(dir_fd,
                        de->d_name,
                        AT_SYMLINK_NOFOLLOW|AT_NO_AUTOMOUNT,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ STATX_TYPE,
                        /* optional_mask= */ STATX_INO,
                        /* mandatory_attributes= */ 0,
                        &sx);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(FLAGS_SET(sx.stx_mask, STATX_TYPE));
        de->d_type = IFTODT(sx.stx_mode);

        /* If the inode is passed too, update the field, i.e. report most recent data */
        എങ്കിൽ (FLAGS_SET(sx.stx_mask, STATX_INO))
                de->d_ino = sx.stx_ino;

        തിരികെ 0;
}

bool dirent_is_file(സ്ഥിരം ഘടന dirent *de) {
        പ്രതിജ്ഞാ(de);

        എങ്കിൽ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                തിരികെ false;

        എങ്കിൽ (hidden_or_backup_file(de->d_name))
                തിരികെ false;

        തിരികെ true;
}

bool dirent_is_file_with_suffix(സ്ഥിരം ഘടന dirent *de, സ്ഥിരം അക്ഷരം *suffix) {
        പ്രതിജ്ഞാ(de);

        എങ്കിൽ (!IN_SET(de->d_type, DT_REG, DT_LNK, DT_UNKNOWN))
                തിരികെ false;

        എങ്കിൽ (de->d_name[0] == '.')
                തിരികെ false;

        എങ്കിൽ (!suffix)
                തിരികെ true;

        തിരികെ endswith(de->d_name, suffix);
}

ഘടന dirent* readdir_ensure_type(DIR *d) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(d);

        /* Like readdir(), but fills in .d_type if it is DT_UNKNOWN */

        വേണ്ടി (;;) {
                ഘടന dirent *de;

                errno = 0;
                de = readdir(d);
                എങ്കിൽ (!de)
                        തിരികെ ശൂന്യ;

                r = dirent_ensure_type(dirfd(d), de);
                എങ്കിൽ (r >= 0)
                        തിരികെ de;
                എങ്കിൽ (r != -ENOENT) {
                        errno = -r; /* We want to be compatible with readdir(), hence propagate error via errno here */
                        തിരികെ ശൂന്യ;
                }

                /* Vanished by now? Then skip immediately to next */
        }
}

ഘടന dirent* readdir_no_dot(DIR *d) {
        പ്രതിജ്ഞാ(d);

        വേണ്ടി (;;) {
                ഘടന dirent *de;

                de = readdir_ensure_type(d);
                എങ്കിൽ (!de || !dot_or_dot_dot(de->d_name))
                        തിരികെ de;
        }
}
