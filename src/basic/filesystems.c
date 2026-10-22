/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

số_nguyên fs_type_from_string(const char *name, const statfs_f_type_t **ret) {
        const struct FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        nếu (!fs_magic)
                trả_về -EINVAL;

        *ret = fs_magic->magic;
        trả_về 0;
}

const FilesystemSet* filesystem_set_find(const char *name) {
        nếu (isempty(name) || name[0] != '@')
                trả_về NULL;

        cho (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                nếu (streq(filesystem_sets[i].name, name))
                        trả_về filesystem_sets + i;

        trả_về NULL;
}
