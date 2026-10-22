/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

정수 capsule_name_is_valid(const char *name) {

        만약 (!filename_is_valid(name))
                반환 false;

        _cleanup_free_ char *prefixed = strjoin("c-", name);
        만약 (!prefixed)
                반환 -ENOMEM;

        반환 valid_user_group_name(prefixed, /* flags= */ 0);
}
