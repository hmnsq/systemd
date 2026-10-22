/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "static-destruct.h"

خالی static_destruct_impl(ثابت StaticDestructor *آغاز, ثابت StaticDestructor *انتہا) {
        اگر (!آغاز)
                واپس;

        برائے (ثابت StaticDestructor *d = آغاز; d < انتہا; d++)
                انتخاب (d->type) {
                حالت STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.ڈیٹا);
                        توڑو;

                حالت STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        توڑو;

                طےشدہ:
                        assert_not_reached();
                }
}
