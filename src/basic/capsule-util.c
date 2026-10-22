/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

整数 capsule_name_is_valid(const char *name) {

        如果 (!filename_is_valid(name))
                返回 false;

        _cleanup_free_ char *prefixed = strjoin("c-", name);
        如果 (!prefixed)
                返回 -ENOMEM;

        返回 valid_user_group_name(prefixed, /* flags= */ 0);
}
