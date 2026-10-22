/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "static-destruct.h"

ಶೂನ್ಯ static_destruct_impl(ಧ್ರುವ StaticDestructor *start, ಧ್ರುವ StaticDestructor *end) {
        ಯದಿ (!start)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿ (ಧ್ರುವ StaticDestructor *d = start; d < end; d++)
                ವಿಕಲ್ಪ (d->type) {
                ಪಕ್ಷ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        ಭಙ್ಗ;

                ಪಕ್ಷ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ಭಙ್ಗ;

                ಸ್ವಭಾವ:
                        assert_not_reached();
                }
}
