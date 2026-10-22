/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

整数 memfd_create_wrapper(const char *name, unsigned mode);

整数 memfd_new_full(const char *name, unsigned extra_flags);
static inline 整数 memfd_new(const char *name) {
        返回 memfd_new_full(name, 0);
}

整数 memfd_new_and_seal(const char *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static inline 整数 memfd_new_and_seal_string(const char *name, const char *s) {
        返回 memfd_new_and_seal(name, s, SIZE_MAX);
}

整数 memfd_set_sealed(整数 fd);
整数 memfd_get_sealed(整数 fd);
整数 memfd_set_sealed_gracefully(整数 fd);

整数 memfd_get_size(整数 fd, uint64_t *ret);
整数 memfd_set_size(整数 fd, uint64_t sz);
