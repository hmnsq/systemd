/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

ཧྲིལ capsule_name_is_valid(const var_na *name) {

        གལ་སྲིད (!filename_is_valid(name))
                ལོག false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        གལ་སྲིད (!prefixed)
                ལོག -ENOMEM;

        ལོག valid_user_group_name(prefixed, /* flags= */ 0);
}
