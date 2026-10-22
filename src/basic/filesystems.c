/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "filesystems-gperf.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"

صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret) {
        ثابت بنية FilesystemMagic *fs_magic;

        أكد(name);
        أكد(ret);

        fs_magic = filesystems_gperf_lookup(name, مزي_اوږدوالی(name));
        إذا (!fs_magic)
                أرجع -EINVAL;

        *ret = fs_magic->magic;
        أرجع 0;
}

ثابت Filesystem_سېټ* filesystem_سېټ_پیدا_کړه(ثابت حرف *name) {
        إذا (isempty(name) || name[0] != '@')
                أرجع عدم;

        لكل (FilesystemGroups i = 0; i < FILESYSTEM_سېټ_MAX; i++)
                إذا (streq(filesystem_sets[i].name, name))
                        أرجع filesystem_sets + i;

        أرجع عدم;
}
