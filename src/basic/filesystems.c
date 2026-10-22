/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "filesystems-gperf.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"

पूर्णांक fs_type_from_string(स्थिर अक्षर *name, स्थिर statfs_f_type_t **ret) {
        स्थिर काष्ठा FilesystemMagic *fs_magic;

        निश्चित(name);
        निश्चित(ret);

        fs_magic = filesystems_gperf_lookup(name, म_माप(name));
        अगर (!fs_magic)
                वापस -EINVAL;

        *ret = fs_magic->magic;
        वापस 0;
}

स्थिर FilesystemSet* filesystem_set_find(स्थिर अक्षर *name) {
        अगर (isempty(name) || name[0] != '@')
                वापस शून्य;

        क्रम (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                अगर (streq(filesystem_sets[i].name, name))
                        वापस filesystem_sets + i;

        वापस शून्य;
}
