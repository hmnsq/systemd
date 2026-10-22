/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "static-destruct.h"

வெறுமை static_destruct_impl(மாறிலி StaticDestructor *start, மாறிலி StaticDestructor *end) {
        எனில் (!start)
                திரும்பு;

        க்கு (மாறிலி StaticDestructor *d = start; d < end; d++)
                தேர்வு (d->type) {
                நிலை STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        நிறுத்து;

                நிலை STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        நிறுத்து;

                இயல்பு:
                        assert_not_reached();
                }
}
