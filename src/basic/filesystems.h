/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_SET_BASIC_API,
        FILESYSTEM_SET_ANONYMOUS,
        FILESYSTEM_SET_APPLICATION,
        FILESYSTEM_SET_AUXILIARY_API,
        FILESYSTEM_SET_COMMON_BLOCK,
        FILESYSTEM_SET_HISTORICAL_BLOCK,
        FILESYSTEM_SET_NETWORK,
        FILESYSTEM_SET_PRIVILEGED_API,
        FILESYSTEM_SET_SECURITY,
        FILESYSTEM_SET_TEMPORARY,
        FILESYSTEM_SET_KNOWN,
        _FILESYSTEM_SET_MAX,
        _FILESYSTEM_SET_INVALID = -EINVAL,
} FilesystemGroups;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା FilesystemSet {
        ଅଚଳ ଅକ୍ଷର *name;
        ଅଚଳ ଅକ୍ଷର *help;
        ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;
} FilesystemSet;

ବାହ୍ୟ ଅଚଳ FilesystemSet filesystem_sets[];

ଅଚଳ FilesystemSet *filesystem_set_find(ଅଚଳ ଅକ୍ଷର *name);

ଅଚଳ ଅକ୍ଷର* fs_type_to_string(statfs_f_type_t magic);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fs_type_from_string(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ statfs_f_type_t **ret);
bool fs_in_group(ଅଚଳ ସଂରଚନା statfs *s, ଗଣନା FilesystemGroups fs_group);

/* gperf prototypes */
ଅଚଳ ସଂରଚନା FilesystemMagic* filesystems_gperf_lookup(ଅଚଳ ଅକ୍ଷର *str, GPERF_LEN_TYPE length);
