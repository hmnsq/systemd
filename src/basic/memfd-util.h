/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <sys/mman.h>           /* IWYU pragma: export */

#ضمن "forward.h"

صحيح memfd_create_wrapper(ثابت حرف *name, بیعلامت نما);

صحيح memfd_new_پر(ثابت حرف *name, بیعلامت extra_flags);
ساكن مضمن صحيح memfd_new(ثابت حرف *name) {
        أرجع memfd_new_پر(name, 0);
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *داده, size_t sz) _nonnull_if_nonzero_(2, 3);
ساكن مضمن صحيح memfd_new_and_seal_string(ثابت حرف *name, ثابت حرف *s) {
        أرجع memfd_new_and_seal(name, s, SIZE_MAX);
}

صحيح memfd_مجموعه_sealed(صحيح fd);
صحيح memfd_get_sealed(صحيح fd);
صحيح memfd_مجموعه_sealed_gracefully(صحيح fd);

صحيح memfd_get_اندازه(صحيح fd, uint64_t *ret);
صحيح memfd_مجموعه_اندازه(صحيح fd, uint64_t sz);
