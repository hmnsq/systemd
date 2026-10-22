/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

ціле fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        якщо (!fs_magic)
                повернути -EINVAL;

        *ret = fs_magic->magic;
        повернути 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        якщо (isempty(name) || name[0] != '@')
                повернути NULL;

        для (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                якщо (streq(filesystem_sets[i].name, name))
                        повернути filesystem_sets + i;

        повернути NULL;
}
