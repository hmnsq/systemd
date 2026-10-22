/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "static-destruct.h"

শূন্য static_destruct_impl(ধ্রুৱ StaticDestructor *start, ধ্রুৱ StaticDestructor *end) {
        যদি (!start)
                ঘূৰাই;

        বাবে (ধ্রুৱ StaticDestructor *d = start; d < end; d++)
                ৱিকল্প (d->type) {
                পক্ষ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        ভঙ্গ;

                পক্ষ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ভঙ্গ;

                স্ৱভাৱ:
                        assert_not_reached();
                }
}
