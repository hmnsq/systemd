/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "capsule-util.h"
#شامل "path-util.h"
#شامل "string-util.h"
#شامل "user-util.h"

صحیح capsule_name_is_valid(ثابت نویسه *name) {

        اگر (!filename_is_valid(name))
                بازگشت false;

        _cleanup_free_ نویسه *prefixed = strjoin("c-", name);
        اگر (!prefixed)
                بازگشت -ENOMEM;

        بازگشت valid_user_group_name(prefixed, /* flags= */ 0);
}
