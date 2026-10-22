/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/mman.h>           /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_create_wrapper(ଅଚଳ ଅକ୍ଷର *name, unsigned mode);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_new_full(ଅଚଳ ଅକ୍ଷର *name, unsigned extra_flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_new(ଅଚଳ ଅକ୍ଷର *name) {
        ଫେରନ୍ତୁ memfd_new_full(name, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_new_and_seal(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଶୂନ୍ୟ *data, size_t sz) _nonnull_if_nonzero_(2, 3);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_new_and_seal_string(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ memfd_new_and_seal(name, s, SIZE_MAX);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_set_sealed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_get_sealed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_set_sealed_gracefully(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_get_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_set_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t sz);
