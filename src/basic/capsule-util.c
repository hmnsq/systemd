/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "capsule-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "user-util.h"

पूर्णाङ्क capsule_name_is_valid(ध्रुव वर्ण *name) {

        सचे (!filename_is_valid(name))
                पटिनिवत्त false;

        _cleanup_free_ वर्ण *prefixed = strjoin("c-", name);
        सचे (!prefixed)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त valid_user_group_name(prefixed, /* flags= */ 0);
}
