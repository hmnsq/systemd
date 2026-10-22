/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

цяло memfd_create_wrapper(const char *name, unsigned mode);

цяло memfd_new_full(const char *name, unsigned extra_flags);
static inline цяло memfd_new(const char *name) {
        върни memfd_new_full(name, 0);
}

цяло memfd_new_and_seal(const char *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static inline цяло memfd_new_and_seal_string(const char *name, const char *s) {
        върни memfd_new_and_seal(name, s, SIZE_MAX);
}

цяло memfd_set_sealed(цяло fd);
цяло memfd_get_sealed(цяло fd);
цяло memfd_set_sealed_gracefully(цяло fd);

цяло memfd_get_size(цяло fd, uint64_t *ret);
цяло memfd_set_size(цяло fd, uint64_t sz);
