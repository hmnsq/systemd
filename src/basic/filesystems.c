/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

整数 fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        如果 (!fs_magic)
                返回 -EINVAL;

        *ret = fs_magic->magic;
        返回 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        如果 (isempty(name) || name[0] != '@')
                返回 NULL;

        为 (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                如果 (streq(filesystem_sets[i].name, name))
                        返回 filesystem_sets + i;

        返回 NULL;
}
