/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "capsule-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಪೂರ್ಣಾಂಕ capsule_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {

        ಆದರೆ (!filename_is_valid(name))
                ಹಿಂತಿರುಗಿ false;

        _cleanup_free_ ಅಕ್ಷರ *prefixed = strjoin("c-", name);
        ಆದರೆ (!prefixed)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ valid_user_ಗುಂಪು_name(prefixed, /* flags= */ 0);
}
