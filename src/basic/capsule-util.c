/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

ចំនួនគត់ capsule_name_is_valid(const var_na *name) {

        ប្រសិនបើ (!filename_is_valid(name))
                ត្រឡប់ false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        ប្រសិនបើ (!prefixed)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ valid_user_group_name(prefixed, /* flags= */ 0);
}
