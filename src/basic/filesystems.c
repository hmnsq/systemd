/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "filesystems-gperf.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণাঙ্ক fs_type_from_string(ধ্রুব বর্ণ *name, ধ্রুব statfs_f_type_t **ret) {
        ধ্রুব সংরচনা FilesystemMagic *fs_magic;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        যদি (!fs_magic)
                প্রত্যাগম -EINVAL;

        *ret = fs_magic->magic;
        প্রত্যাগম 0;
}

ধ্রুব FilesystemSet* filesystem_set_find(ধ্রুব বর্ণ *name) {
        যদি (isempty(name) || name[0] != '@')
                প্রত্যাগম রিক্ত;

        প্রতি (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                যদি (streq(filesystem_sets[i].name, name))
                        প্রত্যাগম filesystem_sets + i;

        প্রত্যাগম রিক্ত;
}
