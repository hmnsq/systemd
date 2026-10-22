/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

số_nguyên capsule_name_is_valid(const char *name) {

        nếu (!filename_is_valid(name))
                trả_về false;

        _cleanup_free_ char *prefixed = strjoin("c-", name);
        nếu (!prefixed)
                trả_về -ENOMEM;

        trả_về valid_user_group_name(prefixed, /* flags= */ 0);
}
