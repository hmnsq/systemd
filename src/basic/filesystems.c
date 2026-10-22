/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "filesystems-gperf.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क fs_type_from_string(ध्रुव वर्ण *name, ध्रुव statfs_f_type_t **ret) {
        ध्रुव संरचना FilesystemMagic *fs_magic;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret);

        fs_magic = filesystems_gperf_lookup(name, वाक्यदैर्घ्यम्(name));
        यदि (!fs_magic)
                प्रत्यागम -EINVAL;

        *ret = fs_magic->magic;
        प्रत्यागम 0;
}

ध्रुव Filesystem_समुच्चयः* filesystem_समुच्चयः_find(ध्रुव वर्ण *name) {
        यदि (isempty(name) || name[0] != '@')
                प्रत्यागम रिक्त;

        प्रति (FilesystemGroups i = 0; i < FILESYSTEM_समुच्चयः_MAX; i++)
                यदि (streq(filesystem_sets[i].name, name))
                        प्रत्यागम filesystem_sets + i;

        प्रत्यागम रिक्त;
}
