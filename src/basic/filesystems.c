/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

ཧྲིལ fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        གལ་ཏེ (!fs_magic)
                ཕྱིར -EINVAL;

        *ret = fs_magic->magic;
        ཕྱིར 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        གལ་ཏེ (isempty(name) || name[0] != '@')
                ཕྱིར NULL;

        ཆེད་དུ (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                གལ་ཏེ (streq(filesystem_sets[i].name, name))
                        ཕྱིར filesystem_sets + i;

        ཕྱིར NULL;
}
