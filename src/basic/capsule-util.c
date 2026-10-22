/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "capsule-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "user-util.h"

পূর্ণসংখ্যা capsule_name_is_valid(ধ্রুবক অক্ষর *name) {

        যদি (!filename_is_valid(name))
                ফেরত false;

        _cleanup_free_ অক্ষর *prefixed = strjoin("c-", name);
        যদি (!prefixed)
                ফেরত -ENOMEM;

        ফেরত valid_user_group_name(prefixed, /* flags= */ 0);
}
