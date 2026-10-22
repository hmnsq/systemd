/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

ចំនួនគត់ fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ប្រសិនបើ (!fs_magic)
                ត្រឡប់ -EINVAL;

        *ret = fs_magic->magic;
        ត្រឡប់ 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        ប្រសិនបើ (isempty(name) || name[0] != '@')
                ត្រឡប់ NULL;

        សម្រាប់ (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ប្រសិនបើ (streq(filesystem_sets[i].name, name))
                        ត្រឡប់ filesystem_sets + i;

        ត្រឡប់ NULL;
}
