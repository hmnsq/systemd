/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "filesystems-gperf.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"

صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret) {
        ثابت بنية FilesystemMagic *fs_magic;

        أكد(name);
        أكد(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        𐤀𐤌 (!fs_magic)
                أرجع -EINVAL;

        *ret = fs_magic->magic;
        أرجع 0;
}

ثابت FilesystemSet* filesystem_set_find(ثابت حرف *name) {
        𐤀𐤌 (isempty(name) || name[0] != '@')
                أرجع عدم;

        لكل (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                𐤀𐤌 (streq(filesystem_sets[i].name, name))
                        أرجع filesystem_sets + i;

        أرجع عدم;
}
