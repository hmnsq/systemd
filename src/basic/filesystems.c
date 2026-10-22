/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "filesystems-gperf.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क fs_type_from_string(ध्रुव वर्ण *name, ध्रुव statfs_f_type_t **ret) {
        ध्रुव संरचना FilesystemMagic *fs_magic;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        सचे (!fs_magic)
                पटिनिवत्त -EINVAL;

        *ret = fs_magic->magic;
        पटिनिवत्त 0;
}

ध्रुव FilesystemSet* filesystem_set_find(ध्रुव वर्ण *name) {
        सचे (isempty(name) || name[0] != '@')
                पटिनिवत्त रिक्त;

        अत्थाय (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                सचे (streq(filesystem_sets[i].name, name))
                        पटिनिवत्त filesystem_sets + i;

        पटिनिवत्त रिक्त;
}
