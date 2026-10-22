/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

ຈຳນວນເຕັມ capsule_name_is_valid(const var_na *name) {

        ຖ້າ (!filename_is_valid(name))
                ສົ່ງຄືນ false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        ຖ້າ (!prefixed)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ valid_user_group_name(prefixed, /* flags= */ 0);
}
