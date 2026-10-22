/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/mman.h>           /* IWYU pragma: export */

#ضمن "forward.h"

صحيح memfd_create_wrapper(ثابت حرف *name, unsigned mode);

صحيح memfd_new_full(ثابت حرف *name, unsigned extra_flags);
ساكن مضمن صحيح memfd_new(ثابت حرف *name) {
        ܦܢܐ memfd_new_full(name, 0);
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *data, size_t sz) _nonnull_if_nonzero_(2, 3);
ساكن مضمن صحيح memfd_new_and_seal_string(ثابت حرف *name, ثابت حرف *s) {
        ܦܢܐ memfd_new_and_seal(name, s, SIZE_MAX);
}

صحيح memfd_set_sealed(صحيح fd);
صحيح memfd_get_sealed(صحيح fd);
صحيح memfd_set_sealed_gracefully(صحيح fd);

صحيح memfd_get_size(صحيح fd, uint64_t *ret);
صحيح memfd_set_size(صحيح fd, uint64_t sz);
