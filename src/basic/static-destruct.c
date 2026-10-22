/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "static-destruct.h"

शून्य static_destruct_impl(स्थिरांक StaticDestructor *start, स्थिरांक StaticDestructor *end) {
        जर (!start)
                परत;

        साठी (स्थिरांक StaticDestructor *d = start; d < end; d++)
                निवड (d->type) {
                स्थिती STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        थांबा;

                स्थिती STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        थांबा;

                पूर्वनिर्धारित:
                        assert_not_reached();
                }
}
