/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "static-destruct.h"

शून्य static_destruct_impl(अचर StaticDestructor *start, अचर StaticDestructor *end) {
        यदि (!start)
                फर्काऊ;

        लागि (अचर StaticDestructor *d = start; d < end; d++)
                विकल्प (d->type) {
                पक्ष STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        रोक;

                पक्ष STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        रोक;

                स्वभाव:
                        assert_not_reached();
                }
}
