/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/mman.h>           /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

முழு memfd_create_wrapper(மாறிலி எழுத்து *name, unsigned mode);

முழு memfd_new_full(மாறிலி எழுத்து *name, unsigned extra_flags);
நிலையான உள்வரி முழு memfd_new(மாறிலி எழுத்து *name) {
        திரும்பு memfd_new_full(name, 0);
}

முழு memfd_new_and_seal(மாறிலி எழுத்து *name, மாறிலி வெறுமை *data, size_t sz) _nonnull_if_nonzero_(2, 3);
நிலையான உள்வரி முழு memfd_new_and_seal_string(மாறிலி எழுத்து *name, மாறிலி எழுத்து *s) {
        திரும்பு memfd_new_and_seal(name, s, SIZE_MAX);
}

முழு memfd_set_sealed(முழு fd);
முழு memfd_get_sealed(முழு fd);
முழு memfd_set_sealed_gracefully(முழு fd);

முழு memfd_get_size(முழு fd, uint64_t *ret);
முழு memfd_set_size(முழு fd, uint64_t sz);
