/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "static-destruct.h"

فراغ static_destruct_impl(ثابت StaticDestructor *start, ثابت StaticDestructor *end) {
        אם (!start)
                החזר;

        עבור (ثابت StaticDestructor *d = start; d < end; d++)
                بدل (d->type) {
                حالة STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        اكسر;

                حالة STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        اكسر;

                افتراضي:
                        assert_not_reached();
                }
}
