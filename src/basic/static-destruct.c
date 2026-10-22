/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "static-destruct.h"

শূন্য static_destruct_impl(ধ্রুবক StaticDestructor *start, ধ্রুবক StaticDestructor *end) {
        যদি (!start)
                ফেরত;

        জন্য (ধ্রুবক StaticDestructor *d = start; d < end; d++)
                নির্বাচন (d->type) {
                ক্ষেত্র STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.তথ্য);
                        বিরতি;

                ক্ষেত্র STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        বিরতি;

                পূর্বনির্ধারিত:
                        assert_not_reached();
                }
}
