/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "static-destruct.h"

void static_destruct_impl(const StaticDestructor *start, const StaticDestructor *end) {
        གལ་སྲིད (!start)
                ལོག;

        དོན་ལུ (const StaticDestructor *d = start; d < end; d++)
                switch (d->type) {
                pak_sa STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.data);
                        bha_nga;

                pak_sa STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        bha_nga;

                default:
                        assert_not_reached();
                }
}
