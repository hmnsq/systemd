/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

puur_naa_nka fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ᬬᬾᬦ᭄ (!fs_magic)
                ᬯᬮᬶ -EINVAL;

        *ret = fs_magic->magic;
        ᬯᬮᬶ 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        ᬬᬾᬦ᭄ (isempty(name) || name[0] != '@')
                ᬯᬮᬶ NULL;

        for (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ᬬᬾᬦ᭄ (streq(filesystem_sets[i].name, name))
                        ᬯᬮᬶ filesystem_sets + i;

        ᬯᬮᬶ NULL;
}
