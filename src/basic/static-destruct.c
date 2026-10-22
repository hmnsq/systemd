/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "static-destruct.h"

ਖਾਲੀ static_destruct_impl(ਅਚਲ StaticDestructor *ਸ਼ੁਰੂ, ਅਚਲ StaticDestructor *ਸਿਰਾ) {
        ਜੇ (!ਸ਼ੁਰੂ)
                ਵਾਪਸ;

        ਲਈ (ਅਚਲ StaticDestructor *d = ਸ਼ੁਰੂ; d < ਸਿਰਾ; d++)
                ਚੋਣ (d->type) {
                ਹਾਲਤ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.ਅੰਕੜੇ);
                        ਰੋਕੋ;

                ਹਾਲਤ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ਰੋਕੋ;

                ਮੂਲ:
                        assert_not_reached();
                }
}
