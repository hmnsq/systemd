/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/mman.h>           /* IWYU pragma: export */

#include "forward.h"

පූර්ණ_සංඛ්‍යා memfd_create_wrapper(const var_na *name, unsigned mode);

පූර්ණ_සංඛ්‍යා memfd_new_full(const var_na *name, unsigned extra_flags);
static anta_hstha පූර්ණ_සංඛ්‍යා memfd_new(const var_na *name) {
        ආපසු memfd_new_full(name, 0);
}

පූර්ණ_සංඛ්‍යා memfd_new_and_seal(const var_na *name, const void *data, size_t sz) _nonnull_if_nonzero_(2, 3);
static anta_hstha පූර්ණ_සංඛ්‍යා memfd_new_and_seal_string(const var_na *name, const var_na *s) {
        ආපසු memfd_new_and_seal(name, s, SIZE_MAX);
}

පූර්ණ_සංඛ්‍යා memfd_set_sealed(පූර්ණ_සංඛ්‍යා fd);
පූර්ණ_සංඛ්‍යා memfd_get_sealed(පූර්ණ_සංඛ්‍යා fd);
පූර්ණ_සංඛ්‍යා memfd_set_sealed_gracefully(පූර්ණ_සංඛ්‍යා fd);

පූර්ණ_සංඛ්‍යා memfd_get_size(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret);
පූර්ණ_සංඛ්‍යා memfd_set_size(පූර්ණ_සංඛ්‍යා fd, uint64_t sz);
