/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

цео fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ако (!fs_magic)
                врати -EINVAL;

        *ret = fs_magic->magic;
        врати 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        ако (isempty(name) || name[0] != '@')
                врати NULL;

        за (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ако (streq(filesystem_sets[i].name, name))
                        врати filesystem_sets + i;

        врати NULL;
}
