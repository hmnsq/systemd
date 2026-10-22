/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"
#ضمن "stat-util.h"

#تعریف_کړه FILESYSTEM_MAGIC_MAX 10

تعريفنوع تعداد FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_سېټ_BASIC_API,
        FILESYSTEM_سېټ_ANONYMOUS,
        FILESYSTEM_سېټ_APPLICATION,
        FILESYSTEM_سېټ_AUXILIARY_API,
        FILESYSTEM_سېټ_COMMON_بلاک,
        FILESYSTEM_سېټ_HISTORICAL_بلاک,
        FILESYSTEM_سېټ_شبکه,
        FILESYSTEM_سېټ_PRIVILEGED_API,
        FILESYSTEM_سېټ_SECURITY,
        FILESYSTEM_سېټ_TEMPORARY,
        FILESYSTEM_سېټ_معلوم,
        FILESYSTEM_سېټ_MAX,
        FILESYSTEM_سېټ_INVALID = -EINVAL,
} FilesystemGroups;

تعريفنوع بنية Filesystem_سېټ {
        ثابت حرف *name;
        ثابت حرف *help;
        ثابت حرف *ارزښت;
} Filesystem_سېټ;

خارجي ثابت Filesystem_سېټ filesystem_sets[];

ثابت Filesystem_سېټ *filesystem_سېټ_پیدا_کړه(ثابت حرف *name);

ثابت حرف* fs_type_منزل_string(statfs_f_type_t magic);
صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret);
bool fs_دننه_ډله(ثابت بنية statfs *s, تعداد FilesystemGroups fs_ډله);

/* gperf prototypes */
ثابت بنية FilesystemMagic* filesystems_gperf_lookup(ثابت حرف *str, GPERF_LEN_TYPE length);
