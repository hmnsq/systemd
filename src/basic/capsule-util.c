/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "capsule-util.h"
#include "path-util.h"
#include "string-util.h"
#include "user-util.h"

puur_naa_nka capsule_name_is_valid(const var_na *name) {

        ꦪꦼꦤ꧀ (!filename_is_valid(name))
                ꦮꦭꦶ false;

        _cleanup_free_ var_na *prefixed = strjoin("c-", name);
        ꦪꦼꦤ꧀ (!prefixed)
                ꦮꦭꦶ -ENOMEM;

        ꦮꦭꦶ valid_user_group_name(prefixed, /* flags= */ 0);
}
