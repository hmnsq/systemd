/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "filesystems-gperf.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક fs_type_from_string(અચલ અક્ષર *name, અચલ statfs_f_type_t **ret) {
        અચલ સંરચના FilesystemMagic *fs_magic;

        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        જો (!fs_magic)
                પાછા -EINVAL;

        *ret = fs_magic->magic;
        પાછા 0;
}

અચલ FilesystemSet* filesystem_set_find(અચલ અક્ષર *name) {
        જો (isempty(name) || name[0] != '@')
                પાછા ખાલી;

        માટે (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                જો (streq(filesystem_sets[i].name, name))
                        પાછા filesystem_sets + i;

        પાછા ખાલી;
}
