/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

வகைவரை எண்ணிக்கை FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_SET_BASIC_API,
        FILESYSTEM_SET_ANONYMOUS,
        FILESYSTEM_SET_APPLICATION,
        FILESYSTEM_SET_AUXILIARY_API,
        FILESYSTEM_SET_COMMON_BLOCK,
        FILESYSTEM_SET_HISTORICAL_BLOCK,
        FILESYSTEM_SET_NETWORK,
        FILESYSTEM_SET_PRIVILEGED_API,
        FILESYSTEM_SET_SECURITY,
        FILESYSTEM_SET_TEMPORARY,
        FILESYSTEM_SET_KNOWN,
        _FILESYSTEM_SET_MAX,
        _FILESYSTEM_SET_INVALID = -EINVAL,
} FilesystemGroups;

வகைவரை கட்டமைப்பு FilesystemSet {
        மாறிலி எழுத்து *name;
        மாறிலி எழுத்து *help;
        மாறிலி எழுத்து *மதிப்பு;
} FilesystemSet;

வெளி மாறிலி FilesystemSet filesystem_sets[];

மாறிலி FilesystemSet *filesystem_set_find(மாறிலி எழுத்து *name);

மாறிலி எழுத்து* fs_type_to_string(statfs_f_type_t magic);
முழு fs_type_from_string(மாறிலி எழுத்து *name, மாறிலி statfs_f_type_t **ret);
bool fs_in_group(மாறிலி கட்டமைப்பு statfs *s, எண்ணிக்கை FilesystemGroups fs_group);

/* gperf prototypes */
மாறிலி கட்டமைப்பு FilesystemMagic* filesystems_gperf_lookup(மாறிலி எழுத்து *str, GPERF_LEN_TYPE length);
