/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "filesystems-gperf.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fs_type_from_string(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ statfs_f_type_t **ret) {
        ଅଚଳ ସଂରଚନା FilesystemMagic *fs_magic;

        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        ଯଦି (!fs_magic)
                ଫେରନ୍ତୁ -EINVAL;

        *ret = fs_magic->magic;
        ଫେରନ୍ତୁ 0;
}

ଅଚଳ FilesystemSet* filesystem_set_find(ଅଚଳ ଅକ୍ଷର *name) {
        ଯଦି (isempty(name) || name[0] != '@')
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                ଯଦି (streq(filesystem_sets[i].name, name))
                        ଫେରନ୍ତୁ filesystem_sets + i;

        ଫେରନ୍ତୁ ଖାଲି;
}
