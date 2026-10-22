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
        अचर वर्ण *name;
        अचर वर्ण *help;
        अचर वर्ण *मान;
} FilesystemSet;

बाह्य अचर FilesystemSet filesystem_sets[];

अचर FilesystemSet *filesystem_set_find(अचर वर्ण *name);

अचर वर्ण* fs_type_to_string(statfs_f_type_t magic);
पूर्णाङ्क fs_type_from_string(अचर वर्ण *name, अचर statfs_f_type_t **ret);
bool fs_in_group(अचर संरचना statfs *s, गणना FilesystemGroups fs_group);

/* gperf prototypes */
अचर संरचना FilesystemMagic* filesystems_gperf_lookup(अचर वर्ण *str, GPERF_LEN_TYPE length);
