/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "static-destruct.h"

व्योम static_destruct_impl(स्थिर StaticDestructor *start, स्थिर StaticDestructor *end) {
        अगर (!start)
                वापस;

        क्रम (स्थिर StaticDestructor *d = start; d < end; d++)
                चयन (d->type) {
                हाल STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.तथ्य);
                        अवरोध;

                हाल STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        अवरोध;

                शेष:
                        assert_not_reached();
                }
}
