/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "filesystems-gperf.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা fs_type_from_string(ধ্রুবক অক্ষর *name, ধ্রুবক statfs_f_type_t **ret) {
        ধ্রুবক কাঠামো FilesystemMagic *fs_magic;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        যদি (!fs_magic)
                ফেরত -EINVAL;

        *ret = fs_magic->magic;
        ফেরত 0;
}

ধ্রুবক FilesystemSet* filesystem_set_find(ধ্রুবক অক্ষর *name) {
        যদি (isempty(name) || name[0] != '@')
                ফেরত ফাঁকা;

        জন্য (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                যদি (streq(filesystem_sets[i].name, name))
                        ফেরত filesystem_sets + i;

        ফেরত ফাঁকা;
}
