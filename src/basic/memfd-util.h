/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

ціле memfd_create_wrapper(const char *name, unsigned mode);

ціле memfd_new_full(const char *name, unsigned extra_flags);
static inline ціле memfd_new(const char *name) {
        повернути memfd_new_full(name, 0);
}

ціле memfd_new_and_seal(const char *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static inline ціле memfd_new_and_seal_string(const char *name, const char *s) {
        повернути memfd_new_and_seal(name, s, SIZE_MAX);
}

ціле memfd_set_sealed(ціле fd);
ціле memfd_get_sealed(ціле fd);
ціле memfd_set_sealed_gracefully(ціле fd);

ціле memfd_get_size(ціле fd, uint64_t *ret);
ціле memfd_set_size(ціле fd, uint64_t sz);
