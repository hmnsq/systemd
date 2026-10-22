/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "capsule-util.h"
#समावेश "path-util.h"
#समावेश "string-util.h"
#समावेश "user-util.h"

पूर्णांक capsule_name_is_valid(स्थिर अक्षर *name) {

        अगर (!filename_is_valid(name))
                वापस false;

        _cleanup_free_ अक्षर *prefixed = strjoin("c-", name);
        अगर (!prefixed)
                वापस -ENOMEM;

        वापस valid_user_group_name(prefixed, /* flags= */ 0);
}
