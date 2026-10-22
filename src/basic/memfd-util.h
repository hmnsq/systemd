/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

ཧྲིལ memfd_create_wrapper(const var_na *name, unsigned mode);

ཧྲིལ memfd_new_full(const var_na *name, unsigned extra_flags);
static anta_hstha ཧྲིལ memfd_new(const var_na *name) {
        ཕྱིར memfd_new_full(name, 0);
}

ཧྲིལ memfd_new_and_seal(const var_na *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static anta_hstha ཧྲིལ memfd_new_and_seal_string(const var_na *name, const var_na *s) {
        ཕྱིར memfd_new_and_seal(name, s, SIZE_MAX);
}

ཧྲིལ memfd_set_sealed(ཧྲིལ fd);
ཧྲིལ memfd_get_sealed(ཧྲིལ fd);
ཧྲིལ memfd_set_sealed_gracefully(ཧྲིལ fd);

ཧྲིལ memfd_get_size(ཧྲིལ fd, uint64_t *ret);
ཧྲིལ memfd_set_size(ཧྲིལ fd, uint64_t sz);
