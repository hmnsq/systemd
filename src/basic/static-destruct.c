/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "static-destruct.h"

શૂન્ય static_destruct_impl(અચલ StaticDestructor *start, અચલ StaticDestructor *end) {
        જો (!start)
                પાછા;

        માટે (અચલ StaticDestructor *d = start; d < end; d++)
                પસંદગી (d->type) {
                સ્થિતિ STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        તોડો;

                સ્થિતિ STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        તોડો;

                મૂળભૂત:
                        assert_not_reached();
                }
}
