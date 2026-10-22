/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

પ્રકારવ્યાખ્યા ગણના FilesystemGroups {
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

પ્રકારવ્યાખ્યા સંરચના FilesystemSet {
        અચલ અક્ષર *name;
        અચલ અક્ષર *help;
        અચલ અક્ષર *કિંમત;
} FilesystemSet;

બાહ્ય અચલ FilesystemSet filesystem_sets[];

અચલ FilesystemSet *filesystem_set_find(અચલ અક્ષર *name);

અચલ અક્ષર* fs_type_to_string(statfs_f_type_t magic);
પૂર્ણાંક fs_type_from_string(અચલ અક્ષર *name, અચલ statfs_f_type_t **ret);
bool fs_in_group(અચલ સંરચના statfs *s, ગણના FilesystemGroups fs_group);

/* gperf prototypes */
અચલ સંરચના FilesystemMagic* filesystems_gperf_lookup(અચલ અક્ષર *str, GPERF_LEN_TYPE length);
