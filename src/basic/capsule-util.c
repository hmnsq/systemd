/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "capsule-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "user-util.h"

पूर्णांक capsule_name_is_valid(स्थिरांक अक्षर *name) {

        जर (!filename_is_valid(name))
                परत false;

        _cleanup_free_ अक्षर *prefixed = strjoin("c-", name);
        जर (!prefixed)
                परत -ENOMEM;

        परत valid_user_group_name(prefixed, /* flags= */ 0);
}
