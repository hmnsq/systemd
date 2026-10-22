/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "filesystems-gperf.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"

पूर्णांक fs_type_from_string(स्थिरांक अक्षर *name, स्थिरांक statfs_f_type_t **ret) {
        स्थिरांक संरचना FilesystemMagic *fs_magic;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret);

        fs_magic = filesystems_gperf_lookup(name, म_माप(name));
        जर (!fs_magic)
                परत -EINVAL;

        *ret = fs_magic->magic;
        परत 0;
}

स्थिरांक FilesystemSet* filesystem_set_find(स्थिरांक अक्षर *name) {
        जर (isempty(name) || name[0] != '@')
                परत रिक्त;

        साठी (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                जर (streq(filesystem_sets[i].name, name))
                        परत filesystem_sets + i;

        परत रिक्त;
}
