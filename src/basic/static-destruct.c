/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "static-destruct.h"

فراغ static_destruct_impl(ثابت StaticDestructor *پیل, ثابت StaticDestructor *پای_ټکی) {
        إذا (!پیل)
                أرجع;

        لكل (ثابت StaticDestructor *d = پیل; d < پای_ټکی; d++)
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
