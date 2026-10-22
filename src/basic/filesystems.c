/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

จำนวนเต็ม fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ถ้า (!fs_magic)
                คืน -EINVAL;

        *ret = fs_magic->magic;
        คืน 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        ถ้า (isempty(name) || name[0] != '@')
                คืน NULL;

        สำหรับ (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ถ้า (streq(filesystem_sets[i].name, name))
                        คืน filesystem_sets + i;

        คืน NULL;
}
