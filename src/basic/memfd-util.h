/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

정수 memfd_create_wrapper(const char *name, unsigned mode);

정수 memfd_new_full(const char *name, unsigned extra_flags);
static inline 정수 memfd_new(const char *name) {
        반환 memfd_new_full(name, 0);
}

정수 memfd_new_and_seal(const char *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static inline 정수 memfd_new_and_seal_string(const char *name, const char *s) {
        반환 memfd_new_and_seal(name, s, SIZE_MAX);
}

정수 memfd_set_sealed(정수 fd);
정수 memfd_get_sealed(정수 fd);
정수 memfd_set_sealed_gracefully(정수 fd);

정수 memfd_get_size(정수 fd, uint64_t *ret);
정수 memfd_set_size(정수 fd, uint64_t sz);
