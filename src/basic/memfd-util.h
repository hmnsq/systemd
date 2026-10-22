/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <sys/mman.h>           /* IWYU pragma: export */

#ضمن "forward.h"

صحيح memfd_create_wrapper(ثابت حرف *name, بنانشان گهڻائي);

صحيح memfd_new_ڀريل(ثابت حرف *name, بنانشان extra_flags);
ساكن مضمن صحيح memfd_new(ثابت حرف *name) {
        أرجع memfd_new_ڀريل(name, 0);
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *معلومات, size_t sz) _nonnull_if_nonzero_(2, 3);
ساكن مضمن صحيح memfd_new_and_seal_string(ثابت حرف *name, ثابت حرف *s) {
        أرجع memfd_new_and_seal(name, s, SIZE_MAX);
}

صحيح memfd_سيٽ_sealed(صحيح fd);
صحيح memfd_get_sealed(صحيح fd);
صحيح memfd_سيٽ_sealed_gracefully(صحيح fd);

صحيح memfd_get_سائيز(صحيح fd, uint64_t *ret);
صحيح memfd_سيٽ_سائيز(صحيح fd, uint64_t sz);
