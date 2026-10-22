/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "capsule-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capsule_name_is_valid(ଅଚଳ ଅକ୍ଷର *name) {

        ଯଦି (!filename_is_valid(name))
                ଫେରନ୍ତୁ false;

        _cleanup_free_ ଅକ୍ଷର *prefixed = strjoin("c-", name);
        ଯଦି (!prefixed)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ valid_user_group_name(prefixed, /* flags= */ 0);
}
