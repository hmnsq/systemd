/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "filesystems-gperf.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ fs_type_from_string(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം statfs_f_type_t **ret) {
        സ്ഥിരം ഘടന FilesystemMagic *fs_magic;

        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(ret);

        fs_magic = filesystems_gperf_lookup(name, strlen(name));
        എങ്കിൽ (!fs_magic)
                തിരികെ -EINVAL;

        *ret = fs_magic->magic;
        തിരികെ 0;
}

സ്ഥിരം FilesystemSet* filesystem_set_find(സ്ഥിരം അക്ഷരം *name) {
        എങ്കിൽ (isempty(name) || name[0] != '@')
                തിരികെ ശൂന്യ;

        വേണ്ടി (FilesystemGroups i = 0; i < _FILESYSTEM_SET_MAX; i++)
                എങ്കിൽ (streq(filesystem_sets[i].name, name))
                        തിരികെ filesystem_sets + i;

        തിരികെ ശൂന്യ;
}
