/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "static-destruct.h"

ଶୂନ୍ୟ static_destruct_impl(ଅଚଳ StaticDestructor *start, ଅଚଳ StaticDestructor *end) {
        ଯଦି (!start)
                ଫେରନ୍ତୁ;

        ପାଇଁ (ଅଚଳ StaticDestructor *d = start; d < end; d++)
                ବାଛ (d->type) {
                ସ୍ଥିତି STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ଭାଙ୍ଗ;

                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        assert_not_reached();
                }
}
