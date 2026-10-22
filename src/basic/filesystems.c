/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "filesystems-gperf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ fs_type_from_string(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ statfs_f_type_t **ret) {
        ਅਚਲ ਬਣਤਰ FilesystemMagic *fs_magic;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        fs_magic = filesystems_gperf_lookup(name, ਲੜੀ_ਲੰਬਾਈ(name));
        ਜੇ (!fs_magic)
                ਵਾਪਸ -EINVAL;

        *ret = fs_magic->magic;
        ਵਾਪਸ 0;
}

ਅਚਲ Filesystem_ਸੈੱਟ* filesystem_ਸੈੱਟ_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ *name) {
        ਜੇ (isempty(name) || name[0] != '@')
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (FilesystemGroups i = 0; i < FILESYSTEM_ਸੈੱਟ_MAX; i++)
                ਜੇ (streq(filesystem_sets[i].name, name))
                        ਵਾਪਸ filesystem_sets + i;

        ਵਾਪਸ ਨਿਰਾ;
}
