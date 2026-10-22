/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"
#ضمن "stat-util.h"

#تعریف_کرو FILESYSTEM_MAGIC_MAX 10

تعريفنوع تعداد FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_سیٹ_BASIC_API,
        FILESYSTEM_سیٹ_ANONYMOUS,
        FILESYSTEM_سیٹ_APPLICATION,
        FILESYSTEM_سیٹ_AUXILIARY_API,
        FILESYSTEM_سیٹ_COMMON_بلاک,
        FILESYSTEM_سیٹ_HISTORICAL_بلاک,
        FILESYSTEM_سیٹ_جال,
        FILESYSTEM_سیٹ_PRIVILEGED_API,
        FILESYSTEM_سیٹ_SECURITY,
        FILESYSTEM_سیٹ_TEMPORARY,
        FILESYSTEM_سیٹ_معلوم,
        FILESYSTEM_سیٹ_MAX,
        FILESYSTEM_سیٹ_INVALID = -EINVAL,
} FilesystemGroups;

تعريفنوع بنية Filesystem_سیٹ {
        ثابت حرف *name;
        ثابت حرف *help;
        ثابت حرف *قدر;
} Filesystem_سیٹ;

خارجي ثابت Filesystem_سیٹ filesystem_sets[];

ثابت Filesystem_سیٹ *filesystem_سیٹ_لبھو(ثابت حرف *name);

ثابت حرف* fs_type_منزل_string(statfs_f_type_t magic);
صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret);
bool fs_وچ_گروہ(ثابت بنية statfs *s, تعداد FilesystemGroups fs_گروہ);

/* gperf prototypes */
ثابت بنية FilesystemMagic* filesystems_gperf_lookup(ثابت حرف *str, GPERF_LEN_TYPE length);
