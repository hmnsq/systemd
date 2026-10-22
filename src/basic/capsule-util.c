/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "capsule-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "user-util.h"

صحيح capsule_name_is_valid(ثابت حرف *name) {

        𐤀𐤌 (!filename_is_valid(name))
                أرجع false;

        _cleanup_free_ حرف *prefixed = strjoin("c-", name);
        𐤀𐤌 (!prefixed)
                أرجع -ENOMEM;

        أرجع valid_user_group_name(prefixed, /* flags= */ 0);
}
