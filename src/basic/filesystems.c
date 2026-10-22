/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "filesystems-gperf.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"

صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret) {
        ثابت بنية FilesystemMagic *fs_magic;

        أكد(name);
        أكد(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        אם (!fs_magic)
                החזר -EINVAL;

        *ret = fs_magic->magic;
        החזר 0;
}

ثابت FilesystemSet* filesystem_set_find(ثابت حرف *name) {
        אם (isempty(name) || name[0] != '@')
                החזר عدم;

        עבור (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                אם (streq(filesystem_sets[i].name, name))
                        החזר filesystem_sets + i;

        החזר عدم;
}
