/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "capsule-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "user-util.h"

പൂർണ്ണസംഖ്യ capsule_name_is_valid(സ്ഥിരം അക്ഷരം *name) {

        എങ്കിൽ (!filename_is_valid(name))
                തിരികെ false;

        _cleanup_free_ അക്ഷരം *prefixed = strjoin("c-", name);
        എങ്കിൽ (!prefixed)
                തിരികെ -ENOMEM;

        തിരികെ valid_user_group_name(prefixed, /* flags= */ 0);
}
