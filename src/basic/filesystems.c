/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

整数 fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        もし (!fs_magic)
                戻る -EINVAL;

        *ret = fs_magic->magic;
        戻る 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        もし (isempty(name) || name[0] != '@')
                戻る NULL;

        ために (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                もし (streq(filesystem_sets[i].name, name))
                        戻る filesystem_sets + i;

        戻る NULL;
}
