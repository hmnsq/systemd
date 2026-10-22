/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "capsule-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಪೂರ್ಣಾಙ್ಕ capsule_name_is_valid(ಧ್ರುವ ವರ್ಣ *name) {

        ಯದಿ (!filename_is_valid(name))
                ಪ್ರತ್ಯಾಗಮ false;

        _cleanup_free_ ವರ್ಣ *prefixed = strjoin("c-", name);
        ಯದಿ (!prefixed)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ valid_user_group_name(prefixed, /* flags= */ 0);
}
