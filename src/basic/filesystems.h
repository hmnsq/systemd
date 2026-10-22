/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FILESYSTEM_MAGIC_MAX 10

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_ਸੈੱਟ_BASIC_API,
        FILESYSTEM_ਸੈੱਟ_ANONYMOUS,
        FILESYSTEM_ਸੈੱਟ_APPLICATION,
        FILESYSTEM_ਸੈੱਟ_AUXILIARY_API,
        FILESYSTEM_ਸੈੱਟ_COMMON_ਖੰਡ,
        FILESYSTEM_ਸੈੱਟ_HISTORICAL_ਖੰਡ,
        FILESYSTEM_ਸੈੱਟ_ਜਾਲ,
        FILESYSTEM_ਸੈੱਟ_PRIVILEGED_API,
        FILESYSTEM_ਸੈੱਟ_SECURITY,
        FILESYSTEM_ਸੈੱਟ_TEMPORARY,
        FILESYSTEM_ਸੈੱਟ_ਗਿਆਤ,
        FILESYSTEM_ਸੈੱਟ_MAX,
        FILESYSTEM_ਸੈੱਟ_INVALID = -EINVAL,
} FilesystemGroups;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Filesystem_ਸੈੱਟ {
        ਅਚਲ ਅੱਖਰ *name;
        ਅਚਲ ਅੱਖਰ *help;
        ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;
} Filesystem_ਸੈੱਟ;

ਬਾਹਰੀ ਅਚਲ Filesystem_ਸੈੱਟ filesystem_sets[];

ਅਚਲ Filesystem_ਸੈੱਟ *filesystem_ਸੈੱਟ_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ *name);

ਅਚਲ ਅੱਖਰ* fs_type_ਮੰਜ਼ਿਲ_string(statfs_f_type_t magic);
ਪੂਰਨ fs_type_from_string(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ statfs_f_type_t **ret);
bool fs_ਵਿੱਚ_ਸਮੂਹ(ਅਚਲ ਬਣਤਰ statfs *s, ਗਿਣਤੀ FilesystemGroups fs_ਸਮੂਹ);

/* gperf prototypes */
ਅਚਲ ਬਣਤਰ FilesystemMagic* filesystems_gperf_lookup(ਅਚਲ ਅੱਖਰ *str, GPERF_LEN_TYPE length);
