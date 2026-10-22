/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

จำนวนเต็ม capsule_name_is_valid(const var_na *name) {

        ถ้า (!filename_is_valid(name))
                คืน false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        ถ้า (!prefixed)
                คืน -ENOMEM;

        คืน valid_user_group_name(prefixed, /* flags= */ 0);
}
