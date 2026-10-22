/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "stat-util.h"

#تعریف_کرو FILESYSTEM_MAGIC_MAX 10

قسمتعریف شمار FilesystemGroups {
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

قسمتعریف ساخت Filesystem_سیٹ {
        ثابت حرف *name;
        ثابت حرف *help;
        ثابت حرف *قدر;
} Filesystem_سیٹ;

خارجی ثابت Filesystem_سیٹ filesystem_sets[];

ثابت Filesystem_سیٹ *filesystem_سیٹ_ڈھونڈو(ثابت حرف *name);

ثابت حرف* fs_type_مقصد_string(statfs_f_type_t magic);
صحیح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret);
bool fs_اندر_گروہ(ثابت ساخت statfs *s, شمار FilesystemGroups fs_گروہ);

/* gperf prototypes */
ثابت ساخت FilesystemMagic* filesystems_gperf_lookup(ثابت حرف *str, GPERF_LEN_TYPE length);
