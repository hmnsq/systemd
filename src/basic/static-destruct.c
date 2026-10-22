/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "static-destruct.h"

शून्य static_destruct_impl(ध्रुव StaticDestructor *start, ध्रुव StaticDestructor *अन्तः) {
        यदि (!start)
                प्रत्यागम;

        प्रति (ध्रुव StaticDestructor *d = start; d < अन्तः; d++)
                विकल्प (d->type) {
                पक्ष STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.दत्त);
                        भङ्ग;

                पक्ष STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        भङ्ग;

                स्वभाव:
                        assert_not_reached();
                }
}
