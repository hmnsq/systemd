/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

정수 fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        만약 (!fs_magic)
                반환 -EINVAL;

        *ret = fs_magic->magic;
        반환 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        만약 (isempty(name) || name[0] != '@')
                반환 NULL;

        위해 (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                만약 (streq(filesystem_sets[i].name, name))
                        반환 filesystem_sets + i;

        반환 NULL;
}
