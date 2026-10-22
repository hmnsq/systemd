/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "static-destruct.h"

শূন্য static_destruct_impl(ধ্রুব StaticDestructor *start, ধ্রুব StaticDestructor *end) {
        যদি (!start)
                প্রত্যাগম;

        প্রতি (ধ্রুব StaticDestructor *d = start; d < end; d++)
                বিকল্প (d->type) {
                পক্ষ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        ভঙ্গ;

                পক্ষ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ভঙ্গ;

                স্বভাব:
                        assert_not_reached();
                }
}
