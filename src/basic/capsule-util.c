/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "capsule-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "user-util.h"

પૂર્ણાંક capsule_name_is_valid(અચલ અક્ષર *name) {

        જો (!filename_is_valid(name))
                પાછા false;

        _cleanup_free_ અક્ષર *prefixed = strjoin("c-", name);
        જો (!prefixed)
                પાછા -ENOMEM;

        પાછા valid_user_group_name(prefixed, /* flags= */ 0);
}
