/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <sys/mman.h>           /* IWYU pragma: export */

#ضمن "forward.h"

صحيح memfd_create_wrapper(ثابت حرف *name, بینښه ډېرپېښ);

صحيح memfd_new_ډک(ثابت حرف *name, بینښه extra_flags);
ساكن مضمن صحيح memfd_new(ثابت حرف *name) {
        أرجع memfd_new_ډک(name, 0);
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *معلومات, size_t sz) _nonnull_if_nonzero_(2, 3);
ساكن مضمن صحيح memfd_new_and_seal_string(ثابت حرف *name, ثابت حرف *s) {
        أرجع memfd_new_and_seal(name, s, SIZE_MAX);
}

صحيح memfd_سېټ_sealed(صحيح fd);
صحيح memfd_get_sealed(صحيح fd);
صحيح memfd_سېټ_sealed_gracefully(صحيح fd);

صحيح memfd_get_اندازه(صحيح fd, uint64_t *ret);
صحيح memfd_سېټ_اندازه(صحيح fd, uint64_t sz);
