/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "static-destruct.h"

ಶೂನ್ಯ static_destruct_impl(ಸ್ಥಿರಾಂಕ StaticDestructor *ಆರಂಭ, ಸ್ಥಿರಾಂಕ StaticDestructor *ಕೊನೆ) {
        ಆದರೆ (!ಆರಂಭ)
                ಹಿಂತಿರುಗಿ;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ StaticDestructor *d = ಆರಂಭ; d < ಕೊನೆ; d++)
                ಆಯ್ಕೆ (d->type) {
                ಸ್ಥಿತಿ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.ದತ್ತಾಂಶ);
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ನಿಲ್ಲಿಸು;

                ಪೂರ್ವನಿಯೋಜಿತ:
                        assert_not_reached();
                }
}
