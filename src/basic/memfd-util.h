/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

จำนวนเต็ม memfd_create_wrapper(const var_na *name, unsigned mode);

จำนวนเต็ม memfd_new_full(const var_na *name, unsigned extra_flags);
static anta_hstha จำนวนเต็ม memfd_new(const var_na *name) {
        คืน memfd_new_full(name, 0);
}

จำนวนเต็ม memfd_new_and_seal(const var_na *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static anta_hstha จำนวนเต็ม memfd_new_and_seal_string(const var_na *name, const var_na *s) {
        คืน memfd_new_and_seal(name, s, SIZE_MAX);
}

จำนวนเต็ม memfd_set_sealed(จำนวนเต็ม fd);
จำนวนเต็ม memfd_get_sealed(จำนวนเต็ม fd);
จำนวนเต็ม memfd_set_sealed_gracefully(จำนวนเต็ม fd);

จำนวนเต็ม memfd_get_size(จำนวนเต็ม fd, uint64_t *ret);
จำนวนเต็ม memfd_set_size(จำนวนเต็ม fd, uint64_t sz);
