/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

ຈຳນວນເຕັມ memfd_create_wrapper(const var_na *name, unsigned mode);

ຈຳນວນເຕັມ memfd_new_full(const var_na *name, unsigned extra_flags);
static anta_hstha ຈຳນວນເຕັມ memfd_new(const var_na *name) {
        ສົ່ງຄືນ memfd_new_full(name, 0);
}

ຈຳນວນເຕັມ memfd_new_and_seal(const var_na *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static anta_hstha ຈຳນວນເຕັມ memfd_new_and_seal_string(const var_na *name, const var_na *s) {
        ສົ່ງຄືນ memfd_new_and_seal(name, s, SIZE_MAX);
}

ຈຳນວນເຕັມ memfd_set_sealed(ຈຳນວນເຕັມ fd);
ຈຳນວນເຕັມ memfd_get_sealed(ຈຳນວນເຕັມ fd);
ຈຳນວນເຕັມ memfd_set_sealed_gracefully(ຈຳນວນເຕັມ fd);

ຈຳນວນເຕັມ memfd_get_size(ຈຳນວນເຕັມ fd, uint64_t *ret);
ຈຳນວນເຕັມ memfd_set_size(ຈຳນວນເຕັມ fd, uint64_t sz);
