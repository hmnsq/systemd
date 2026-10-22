/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

प्रकारव्याख्या गणना FilesystemGroups {
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

प्रकारव्याख्या संरचना FilesystemSet {
        स्थिरांक अक्षर *name;
        स्थिरांक अक्षर *help;
        स्थिरांक अक्षर *मूल्य;
} FilesystemSet;

बाह्य स्थिरांक FilesystemSet filesystem_sets[];

स्थिरांक FilesystemSet *filesystem_set_find(स्थिरांक अक्षर *name);

स्थिरांक अक्षर* fs_type_to_string(statfs_f_type_t magic);
पूर्णांक fs_type_from_string(स्थिरांक अक्षर *name, स्थिरांक statfs_f_type_t **ret);
bool fs_in_group(स्थिरांक संरचना statfs *s, गणना FilesystemGroups fs_group);

/* gperf prototypes */
स्थिरांक संरचना FilesystemMagic* filesystems_gperf_lookup(स्थिरांक अक्षर *str, GPERF_LEN_TYPE length);
