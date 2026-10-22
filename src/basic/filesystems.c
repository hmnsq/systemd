/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "filesystems-gperf.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ fs_type_from_string(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ statfs_f_type_t **ret) {
        ಧ್ರುವ ಸಂರಚನಾ FilesystemMagic *fs_magic;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ಯದಿ (!fs_magic)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = fs_magic->magic;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ FilesystemSet* filesystem_set_find(ಧ್ರುವ ವರ್ಣ *name) {
        ಯದಿ (isempty(name) || name[0] != '@')
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ಯದಿ (streq(filesystem_sets[i].name, name))
                        ಪ್ರತ್ಯಾಗಮ filesystem_sets + i;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
