/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "filesystems-gperf.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"

முழு fs_type_from_string(மாறிலி எழுத்து *name, மாறிலி statfs_f_type_t **ret) {
        மாறிலி கட்டமைப்பு FilesystemMagic *fs_magic;

        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        எனில் (!fs_magic)
                திரும்பு -EINVAL;

        *ret = fs_magic->magic;
        திரும்பு 0;
}

மாறிலி FilesystemSet* filesystem_set_find(மாறிலி எழுத்து *name) {
        எனில் (isempty(name) || name[0] != '@')
                திரும்பு வெற்று;

        க்கு (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                எனில் (streq(filesystem_sets[i].name, name))
                        திரும்பு filesystem_sets + i;

        திரும்பு வெற்று;
}
