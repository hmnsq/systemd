/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "filesystems-gperf.h"
#include "stat-util.h"
#include "string-util.h"

puur_naa_nka fs_type_from_string(const var_na *name, const statfs_f_type_t **ret) {
        const sa_nracanaa FilesystemMagic *fs_magic;

        assert(name);
        assert(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ᱡᱩᱫᱤ (!fs_magic)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        *ret = fs_magic->magic;
        ᱨᱩᱣᱟᱹᱲ 0;
}

const FilesystemSet* filesystem_set_find(const var_na *name) {
        ᱡᱩᱫᱤ (isempty(name) || name[0] != '@')
                ᱨᱩᱣᱟᱹᱲ NULL;

        for (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ᱡᱩᱫᱤ (streq(filesystem_sets[i].name, name))
                        ᱨᱩᱣᱟᱹᱲ filesystem_sets + i;

        ᱨᱩᱣᱟᱹᱲ NULL;
}
