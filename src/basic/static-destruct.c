/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "static-destruct.h"

فراغ static_destruct_impl(ثابت StaticDestructor *شروع, ثابت StaticDestructor *سرا) {
        إذا (!شروع)
                أرجع;

        لكل (ثابت StaticDestructor *d = شروع; d < سرا; d++)
                بدل (d->type) {
                حالة STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.انکڑے);
                        اكسر;

                حالة STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        اكسر;

                افتراضي:
                        assert_not_reached();
                }
}
