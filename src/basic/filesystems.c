/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

ကိန်းပြည့် fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        အကယ်၍ (!fs_magic)
                ပြန်ပေး -EINVAL;

        *ret = fs_magic->magic;
        ပြန်ပေး 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        အကယ်၍ (isempty(name) || name[0] != '@')
                ပြန်ပေး NULL;

        အတွက် (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                အကယ်၍ (streq(filesystem_sets[i].name, name))
                        ပြန်ပေး filesystem_sets + i;

        ပြန်ပေး NULL;
}
