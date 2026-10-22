/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "static-destruct.h"

فراغ static_destruct_impl(ثابت StaticDestructor *شروعات, ثابت StaticDestructor *پڇاڙي) {
        إذا (!شروعات)
                أرجع;

        لكل (ثابت StaticDestructor *d = شروعات; d < پڇاڙي; d++)
                بدل (d->type) {
                حالة STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.معلومات);
                        اكسر;

                حالة STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        اكسر;

                افتراضي:
                        assert_not_reached();
                }
}
