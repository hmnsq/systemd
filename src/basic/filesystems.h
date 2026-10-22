/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "stat-util.h"

#define FILESYSTEM_MAGIC_MAX 10

തരനിർവചനം എണ്ണൽ FilesystemGroups {
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

തരനിർവചനം ഘടന FilesystemSet {
        സ്ഥിരം അക്ഷരം *name;
        സ്ഥിരം അക്ഷരം *help;
        സ്ഥിരം അക്ഷരം *മൂല്യം;
} FilesystemSet;

ബാഹ്യ സ്ഥിരം FilesystemSet filesystem_sets[];

സ്ഥിരം FilesystemSet *filesystem_set_find(സ്ഥിരം അക്ഷരം *name);

സ്ഥിരം അക്ഷരം* fs_type_to_string(statfs_f_type_t magic);
പൂർണ്ണസംഖ്യ fs_type_from_string(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം statfs_f_type_t **ret);
bool fs_in_group(സ്ഥിരം ഘടന statfs *s, എണ്ണൽ FilesystemGroups fs_group);

/* gperf prototypes */
സ്ഥിരം ഘടന FilesystemMagic* filesystems_gperf_lookup(സ്ഥിരം അക്ഷരം *str, GPERF_LEN_TYPE length);
