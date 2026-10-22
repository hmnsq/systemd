/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"
#ضمن "stat-util.h"

#وصف_ڪريو FILESYSTEM_MAGIC_MAX 10

تعريفنوع تعداد FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_سيٽ_BASIC_API,
        FILESYSTEM_سيٽ_ANONYMOUS,
        FILESYSTEM_سيٽ_APPLICATION,
        FILESYSTEM_سيٽ_AUXILIARY_API,
        FILESYSTEM_سيٽ_COMMON_بلاڪ,
        FILESYSTEM_سيٽ_HISTORICAL_بلاڪ,
        FILESYSTEM_سيٽ_ڄار,
        FILESYSTEM_سيٽ_PRIVILEGED_API,
        FILESYSTEM_سيٽ_SECURITY,
        FILESYSTEM_سيٽ_TEMPORARY,
        FILESYSTEM_سيٽ_معلوم,
        FILESYSTEM_سيٽ_MAX,
        FILESYSTEM_سيٽ_INVALID = -EINVAL,
} FilesystemGroups;

تعريفنوع بنية Filesystem_سيٽ {
        ثابت حرف *name;
        ثابت حرف *help;
        ثابت حرف *قدر;
} Filesystem_سيٽ;

خارجي ثابت Filesystem_سيٽ filesystem_sets[];

ثابت Filesystem_سيٽ *filesystem_سيٽ_ڳوليو(ثابت حرف *name);

ثابت حرف* fs_type_منزل_string(statfs_f_type_t magic);
صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret);
bool fs_اندريون_گروهه(ثابت بنية statfs *s, تعداد FilesystemGroups fs_گروهه);

/* gperf prototypes */
ثابت بنية FilesystemMagic* filesystems_gperf_lookup(ثابت حرف *str, GPERF_LEN_TYPE length);
