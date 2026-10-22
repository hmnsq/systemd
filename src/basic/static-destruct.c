/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "static-destruct.h"

تهی static_destruct_impl(ثابت StaticDestructor *start, ثابت StaticDestructor *end) {
        اگر (!start)
                بازگشت;

        برای (ثابت StaticDestructor *d = start; d < end; d++)
                انتخاب (d->type) {
                حالت STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        بشکن;

                حالت STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        بشکن;

                پیشفرض:
                        assert_not_reached();
                }
}
