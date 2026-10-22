/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "filesystems-gperf.h"
#شامل "stat-util.h"
#شامل "string-util.h"

صحیح fs_type_from_string(ثابت نویسه *name, ثابت statfs_f_type_t **ret) {
        ثابت ساختار FilesystemMagic *fs_magic;

        أكد(name);
        أكد(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        اگر (!fs_magic)
                بازگشت -EINVAL;

        *ret = fs_magic->magic;
        بازگشت 0;
}

ثابت FilesystemSet* filesystem_set_find(ثابت نویسه *name) {
        اگر (isempty(name) || name[0] != '@')
                بازگشت پوچ;

        برای (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                اگر (streq(filesystem_sets[i].name, name))
                        بازگشت filesystem_sets + i;

        بازگشت پوچ;
}
