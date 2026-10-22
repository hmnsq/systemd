/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "stat-util.h"

#घोषणा FILESYSTEM_MAGIC_MAX 10

प्रकार क्रमागत FilesystemGroups {
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

प्रकार काष्ठा FilesystemSet {
        स्थिर अक्षर *name;
        स्थिर अक्षर *help;
        स्थिर अक्षर *मूल्य;
} FilesystemSet;

बाह्य स्थिर FilesystemSet filesystem_sets[];

स्थिर FilesystemSet *filesystem_set_find(स्थिर अक्षर *name);

स्थिर अक्षर* fs_type_to_string(statfs_f_type_t magic);
पूर्णांक fs_type_from_string(स्थिर अक्षर *name, स्थिर statfs_f_type_t **ret);
bool fs_in_group(स्थिर काष्ठा statfs *s, क्रमागत FilesystemGroups fs_group);

/* gperf prototypes */
स्थिर काष्ठा FilesystemMagic* filesystems_gperf_lookup(स्थिर अक्षर *str, GPERF_LEN_TYPE length);
