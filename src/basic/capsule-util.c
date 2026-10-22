/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "capsule-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "user-util.h"

முழு capsule_name_is_valid(மாறிலி எழுத்து *name) {

        எனில் (!filename_is_valid(name))
                திரும்பு false;

        _cleanup_free_ எழுத்து *prefixed = strjoin("c-", name);
        எனில் (!prefixed)
                திரும்பு -ENOMEM;

        திரும்பு valid_user_group_name(prefixed, /* flags= */ 0);
}
