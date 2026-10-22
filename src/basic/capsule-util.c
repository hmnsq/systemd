/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

ကိန်းပြည့် capsule_name_is_valid(const var_na *name) {

        အကယ်၍ (!filename_is_valid(name))
                ပြန်ပေး false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        အကယ်၍ (!prefixed)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး valid_user_group_name(prefixed, /* flags= */ 0);
}
