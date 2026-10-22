/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "capsule-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "user-util.h"

পূর্ণাঙ্ক capsule_name_is_valid(ধ্রুৱ ৱর্ণ *name) {

        যদি (!filename_is_valid(name))
                ঘূৰাই false;

        _cleanup_free_ ৱর্ণ *prefixed = strjoin("c-", name);
        যদি (!prefixed)
                ঘূৰাই -ENOMEM;

        ঘূৰাই valid_user_group_name(prefixed, /* flags= */ 0);
}
