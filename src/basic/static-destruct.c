/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "static-destruct.h"

فراغ static_destruct_impl(ثابت StaticDestructor *شروع, ثابت StaticDestructor *انتها) {
        إذا (!شروع)
                أرجع;

        لكل (ثابت StaticDestructor *d = شروع; d < انتها; d++)
                بدل (d->type) {
                حالة STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.داده);
                        اكسر;

                حالة STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        اكسر;

                افتراضي:
                        assert_not_reached();
                }
}
