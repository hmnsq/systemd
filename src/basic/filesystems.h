/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

प्रकारनिर्देश गणना FilesystemGroups {
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

प्रकारनिर्देश संरचना FilesystemSet {
        ध्रुव वर्ण *name;
        ध्रुव वर्ण *help;
        ध्रुव वर्ण *मानम्;
} FilesystemSet;

बाह्य ध्रुव FilesystemSet filesystem_sets[];

ध्रुव FilesystemSet *filesystem_set_find(ध्रुव वर्ण *name);

ध्रुव वर्ण* fs_type_to_string(statfs_f_type_t magic);
पूर्णाङ्क fs_type_from_string(ध्रुव वर्ण *name, ध्रुव statfs_f_type_t **ret);
bool fs_in_group(ध्रुव संरचना statfs *s, गणना FilesystemGroups fs_group);

/* gperf prototypes */
ध्रुव संरचना FilesystemMagic* filesystems_gperf_lookup(ध्रुव वर्ण *str, GPERF_LEN_TYPE length);
