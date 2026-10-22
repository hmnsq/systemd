/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "filesystems-gperf.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক fs_type_from_string(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ statfs_f_type_t **ret) {
        ধ্রুৱ সংরচনা FilesystemMagic *fs_magic;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        যদি (!fs_magic)
                ঘূৰাই -EINVAL;

        *ret = fs_magic->magic;
        ঘূৰাই 0;
}

ধ্রুৱ FilesystemSet* filesystem_set_find(ধ্রুৱ ৱর্ণ *name) {
        যদি (isempty(name) || name[0] != '@')
                ঘূৰাই রিক্ত;

        বাবে (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                যদি (streq(filesystem_sets[i].name, name))
                        ঘূৰাই filesystem_sets + i;

        ঘূৰাই রিক্ত;
}
