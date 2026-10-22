/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

ціле capsule_name_is_valid(const char *name) {

        якщо (!filename_is_valid(name))
                повернути false;

        _cleanup_free_ char *prefixed = strjoin("c-", name);
        якщо (!prefixed)
                повернути -ENOMEM;

        повернути valid_user_group_name(prefixed, /* flags= */ 0);
}
