/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "filesystems-gperf.h"
#شامل "stat-util.h"
#شامل "string-util.h"

صحیح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret) {
        ثابت ساخت FilesystemMagic *fs_magic;

        أكد(name);
        أكد(ret);

        fs_magic = filesystems_gperf_lookup(name, لڑی_لمبائی(name));
        اگر (!fs_magic)
                واپس -EINVAL;

        *ret = fs_magic->magic;
        واپس 0;
}

ثابت Filesystem_سیٹ* filesystem_سیٹ_ڈھونڈو(ثابت حرف *name) {
        اگر (isempty(name) || name[0] != '@')
                واپس تہی;

        برائے (FilesystemGroups i = 0; i < FILESYSTEM_سیٹ_MAX; i++)
                اگر (streq(filesystem_sets[i].name, name))
                        واپس filesystem_sets + i;

        واپس تہی;
}
