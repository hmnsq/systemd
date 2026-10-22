/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

ຈຳນວນເຕັມ fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ຖ້າ (!fs_magic)
                ສົ່ງຄືນ -EINVAL;

        *ret = fs_magic->magic;
        ສົ່ງຄືນ 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        ຖ້າ (isempty(name) || name[0] != '@')
                ສົ່ງຄືນ NULL;

        ສຳລັບ (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ຖ້າ (streq(filesystem_sets[i].name, name))
                        ສົ່ງຄືນ filesystem_sets + i;

        ສົ່ງຄືນ NULL;
}
