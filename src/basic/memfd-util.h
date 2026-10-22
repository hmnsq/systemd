/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

цео memfd_create_wrapper(const char *name, unsigned mode);

цео memfd_new_full(const char *name, unsigned extra_flags);
static inline цео memfd_new(const char *name) {
        врати memfd_new_full(name, 0);
}

цео memfd_new_and_seal(const char *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static inline цео memfd_new_and_seal_string(const char *name, const char *s) {
        врати memfd_new_and_seal(name, s, SIZE_MAX);
}

цео memfd_set_sealed(цео fd);
цео memfd_get_sealed(цео fd);
цео memfd_set_sealed_gracefully(цео fd);

цео memfd_get_size(цео fd, uint64_t *ret);
цео memfd_set_size(цео fd, uint64_t sz);
