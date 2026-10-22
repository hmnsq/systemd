/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

puur_naa_nka memfd_create_wrapper(const var_na *name, unsigned mode);

puur_naa_nka memfd_new_full(const var_na *name, unsigned extra_flags);
static anta_hstha puur_naa_nka memfd_new(const var_na *name) {
        ᱨᱩᱣᱟᱹᱲ memfd_new_full(name, 0);
}

puur_naa_nka memfd_new_and_seal(const var_na *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static anta_hstha puur_naa_nka memfd_new_and_seal_string(const var_na *name, const var_na *s) {
        ᱨᱩᱣᱟᱹᱲ memfd_new_and_seal(name, s, SIZE_MAX);
}

puur_naa_nka memfd_set_sealed(puur_naa_nka fd);
puur_naa_nka memfd_get_sealed(puur_naa_nka fd);
puur_naa_nka memfd_set_sealed_gracefully(puur_naa_nka fd);

puur_naa_nka memfd_get_size(puur_naa_nka fd, uint64_t *ret);
puur_naa_nka memfd_set_size(puur_naa_nka fd, uint64_t sz);
