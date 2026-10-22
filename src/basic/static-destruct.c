/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "static-destruct.h"

ശൂന്യം static_destruct_impl(സ്ഥിരം StaticDestructor *start, സ്ഥിരം StaticDestructor *end) {
        എങ്കിൽ (!start)
                തിരികെ;

        വേണ്ടി (സ്ഥിരം StaticDestructor *d = start; d < end; d++)
                തിരഞ്ഞെടുപ്പ് (d->type) {
                അവസ്ഥ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        നിർത്തുക;

                അവസ്ഥ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        നിർത്തുക;

                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
                }
}
