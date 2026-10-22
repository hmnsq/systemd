/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

පූර්ණ_සංඛ්‍යා fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        නම් (!fs_magic)
                ආපසු -EINVAL;

        *ret = fs_magic->magic;
        ආපසු 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        නම් (isempty(name) || name[0] != '@')
                ආපසු NULL;

        සඳහා (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                නම් (streq(filesystem_sets[i].name, name))
                        ආපසු filesystem_sets + i;

        ආපසු NULL;
}
