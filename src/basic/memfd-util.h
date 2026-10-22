/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

целое memfd_create_wrapper(const char *name, unsigned mode);

целое memfd_new_full(const char *name, unsigned extra_flags);
static inline целое memfd_new(const char *name) {
        вернуть memfd_new_full(name, 0);
}

целое memfd_new_and_seal(const char *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static inline целое memfd_new_and_seal_string(const char *name, const char *s) {
        вернуть memfd_new_and_seal(name, s, SIZE_MAX);
}

целое memfd_set_sealed(целое fd);
целое memfd_get_sealed(целое fd);
целое memfd_set_sealed_gracefully(целое fd);

целое memfd_get_size(целое fd, uint64_t *ret);
целое memfd_set_size(целое fd, uint64_t sz);
