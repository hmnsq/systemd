/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

целое capsule_name_is_valid(const char *name) {

        если (!filename_is_valid(name))
                вернуть false;

        _cleanup_free_ char *prefixed = strjoin("c-", name);
        если (!prefixed)
                вернуть -ENOMEM;

        вернуть valid_user_group_name(prefixed, /* flags= */ 0);
}
