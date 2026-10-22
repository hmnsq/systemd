/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "static-destruct.h"

శూన్య static_destruct_impl(స్థిరరాశి StaticDestructor *ప్రారంభం, స్థిరరాశి StaticDestructor *చివర) {
        ఒకవేళ (!ప్రారంభం)
                తిరిగి;

        కొరకు (స్థిరరాశి StaticDestructor *d = ప్రారంభం; d < చివర; d++)
                ఎంపిక (d->type) {
                స్థితి STATIC_DESTRUCTOR_SIMPLE:
                        d->simple.destroy(d->simple.దత్తాంశం);
                        ఆపు;

                స్థితి STATIC_DESTRUCTOR_ARRAY:
                        array_cleanup(&d->array);
                        ఆపు;

                అప్రమేయ:
                        assert_not_reached();
                }
}
