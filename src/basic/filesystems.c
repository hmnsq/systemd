/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

целое fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        если (!fs_magic)
                вернуть -EINVAL;

        *ret = fs_magic->magic;
        вернуть 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        если (isempty(name) || name[0] != '@')
                вернуть NULL;

        для (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                если (streq(filesystem_sets[i].name, name))
                        вернуть filesystem_sets + i;

        вернуть NULL;
}
