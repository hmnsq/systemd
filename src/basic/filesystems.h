/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "stat-util.h"

#تعریف_کن FILESYSTEM_MAGIC_MAX 10

تعريفنوع تعداد FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_مجموعه_BASIC_API,
        FILESYSTEM_مجموعه_ANONYMOUS,
        FILESYSTEM_مجموعه_APPLICATION,
        FILESYSTEM_مجموعه_AUXILIARY_API,
        FILESYSTEM_مجموعه_COMMON_بلوک,
        FILESYSTEM_مجموعه_HISTORICAL_بلوک,
        FILESYSTEM_مجموعه_شبکه,
        FILESYSTEM_مجموعه_PRIVILEGED_API,
        FILESYSTEM_مجموعه_SECURITY,
        FILESYSTEM_مجموعه_TEMPORARY,
        FILESYSTEM_مجموعه_معلوم,
        FILESYSTEM_مجموعه_MAX,
        FILESYSTEM_مجموعه_INVALID = -EINVAL,
} FilesystemGroups;

تعريفنوع بنية Filesystem_مجموعه {
        ثابت حرف *name;
        ثابت حرف *help;
        ثابت حرف *مقدار;
} Filesystem_مجموعه;

خارجي ثابت Filesystem_مجموعه filesystem_sets[];

ثابت Filesystem_مجموعه *filesystem_مجموعه_پیدا_کن(ثابت حرف *name);

ثابت حرف* fs_type_مقصد_string(statfs_f_type_t magic);
صحيح fs_type_from_string(ثابت حرف *name, ثابت statfs_f_type_t **ret);
bool fs_درون_گروه(ثابت بنية statfs *s, تعداد FilesystemGroups fs_گروه);

/* gperf prototypes */
ثابت بنية FilesystemMagic* filesystems_gperf_lookup(ثابت حرف *str, GPERF_LEN_TYPE length);
