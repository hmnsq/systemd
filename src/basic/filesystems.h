/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "stat-util.h"

#परिभाषय FILESYSTEM_MAGIC_MAX 10

प्रकारनिर्देश गणना FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_समुच्चयः_BASIC_API,
        FILESYSTEM_समुच्चयः_ANONYMOUS,
        FILESYSTEM_समुच्चयः_APPLICATION,
        FILESYSTEM_समुच्चयः_AUXILIARY_API,
        FILESYSTEM_समुच्चयः_COMMON_खण्डम्,
        FILESYSTEM_समुच्चयः_HISTORICAL_खण्डम्,
        FILESYSTEM_समुच्चयः_जालम्,
        FILESYSTEM_समुच्चयः_PRIVILEGED_API,
        FILESYSTEM_समुच्चयः_SECURITY,
        FILESYSTEM_समुच्चयः_TEMPORARY,
        FILESYSTEM_समुच्चयः_ज्ञातम्,
        FILESYSTEM_समुच्चयः_MAX,
        FILESYSTEM_समुच्चयः_INVALID = -EINVAL,
} FilesystemGroups;

प्रकारनिर्देश संरचना Filesystem_समुच्चयः {
        ध्रुव वर्ण *name;
        ध्रुव वर्ण *help;
        ध्रुव वर्ण *मानम्;
} Filesystem_समुच्चयः;

बाह्य ध्रुव Filesystem_समुच्चयः filesystem_sets[];

ध्रुव Filesystem_समुच्चयः *filesystem_समुच्चयः_find(ध्रुव वर्ण *name);

ध्रुव वर्ण* fs_type_गन्तव्यम्_string(statfs_f_type_t magic);
पूर्णाङ्क fs_type_from_string(ध्रुव वर्ण *name, ध्रुव statfs_f_type_t **ret);
bool fs_अन्तर्_गणः(ध्रुव संरचना statfs *s, गणना FilesystemGroups fs_गणः);

/* gperf prototypes */
ध्रुव संरचना FilesystemMagic* filesystems_gperf_lookup(ध्रुव वर्ण *str, GPERF_LEN_TYPE length);
