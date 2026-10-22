/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "capsule-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "user-util.h"

पूर्णाङ्क capsule_name_is_valid(ध्रुव वर्ण *name) {

        यदि (!filename_is_valid(name))
                प्रत्यागम false;

        _cleanup_free_ वर्ण *prefixed = strjoin("c-", name);
        यदि (!prefixed)
                प्रत्यागम -ENOMEM;

        प्रत्यागम valid_user_गणः_name(prefixed, /* flags= */ 0);
}
