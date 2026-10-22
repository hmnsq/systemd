/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"

#ವ್ಯಾಖ್ಯಾನಿಸು FILESYSTEM_MAGIC_MAX 10

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_ಗಣ_BASIC_API,
        FILESYSTEM_ಗಣ_ANONYMOUS,
        FILESYSTEM_ಗಣ_APPLICATION,
        FILESYSTEM_ಗಣ_AUXILIARY_API,
        FILESYSTEM_ಗಣ_COMMON_ಖಂಡ,
        FILESYSTEM_ಗಣ_HISTORICAL_ಖಂಡ,
        FILESYSTEM_ಗಣ_ಜಾಲ,
        FILESYSTEM_ಗಣ_PRIVILEGED_API,
        FILESYSTEM_ಗಣ_SECURITY,
        FILESYSTEM_ಗಣ_TEMPORARY,
        FILESYSTEM_ಗಣ_ತಿಳಿದ,
        FILESYSTEM_ಗಣ_MAX,
        FILESYSTEM_ಗಣ_INVALID = -EINVAL,
} FilesystemGroups;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Filesystem_ಗಣ {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *help;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;
} Filesystem_ಗಣ;

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ Filesystem_ಗಣ filesystem_sets[];

ಸ್ಥಿರಾಂಕ Filesystem_ಗಣ *filesystem_ಗಣ_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* fs_type_ಗಮ್ಯ_string(statfs_f_type_t magic);
ಪೂರ್ಣಾಂಕ fs_type_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ statfs_f_type_t **ret);
bool fs_ಒಳ_ಗುಂಪು(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s, ಎಣಿಕೆ FilesystemGroups fs_ಗುಂಪು);

/* gperf prototypes */
ಸ್ಥಿರಾಂಕ ರಚನೆ FilesystemMagic* filesystems_gperf_lookup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, GPERF_LEN_TYPE length);
