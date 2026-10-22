/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "capsule-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "user-util.h"

পূর্ণাঙ্ক capsule_name_is_valid(ধ্রুব বর্ণ *name) {

        যদি (!filename_is_valid(name))
                প্রত্যাগম false;

        _cleanup_free_ বর্ণ *prefixed = strjoin("c-", name);
        যদি (!prefixed)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম valid_user_group_name(prefixed, /* flags= */ 0);
}
