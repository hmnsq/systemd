/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

පූර්ණ_සංඛ්‍යා capsule_name_is_valid(const var_na *name) {

        නම් (!filename_is_valid(name))
                ආපසු false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        නම් (!prefixed)
                ආපසු -ENOMEM;

        ආපසු valid_user_group_name(prefixed, /* flags= */ 0);
}
