/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "filesystems-gperf.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ fs_type_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ statfs_f_type_t **ret) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ FilesystemMagic *fs_magic;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        fs_magic = filesystems_gperf_lookup(name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
        ಆದರೆ (!fs_magic)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = fs_magic->magic;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ Filesystem_ಗಣ* filesystem_ಗಣ_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಆದರೆ (isempty(name) || name[0] != '@')
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (FilesystemGroups i = 0; i < FILESYSTEM_ಗಣ_MAX; i++)
                ಆದರೆ (streq(filesystem_sets[i].name, name))
                        ಹಿಂತಿರುಗಿ filesystem_sets + i;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
