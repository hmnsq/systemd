/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

ཧྲིལ capsule_name_is_valid(const var_na *name) {

        གལ་ཏེ (!filename_is_valid(name))
                ཕྱིར false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        གལ་ཏེ (!prefixed)
                ཕྱིར -ENOMEM;

        ཕྱིར valid_user_group_name(prefixed, /* flags= */ 0);
}
