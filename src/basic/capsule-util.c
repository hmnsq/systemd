/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

puur_naa_nka capsule_name_is_valid(const var_na *name) {

        ᯀᯮᯝ (!filename_is_valid(name))
                ᯀᯮᯞ false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        ᯀᯮᯝ (!prefixed)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ valid_user_group_name(prefixed, /* flags= */ 0);
}
