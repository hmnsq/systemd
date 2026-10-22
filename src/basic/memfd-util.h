/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/mman.h>           /* IWYU pragma: export */

#شامل "forward.h"

صحیح memfd_create_wrapper(ثابت حرف *name, غیرعلامتی منوال);

صحیح memfd_new_بھرا(ثابت حرف *name, غیرعلامتی extra_flags);
ساکن اندرونی صحیح memfd_new(ثابت حرف *name) {
        واپس memfd_new_بھرا(name, 0);
}

صحیح memfd_new_and_seal(ثابت حرف *name, ثابت خالی *ڈیٹا, size_t sz) _nonnull_if_nonzero_(2, 3);
ساکن اندرونی صحیح memfd_new_and_seal_string(ثابت حرف *name, ثابت حرف *s) {
        واپس memfd_new_and_seal(name, s, SIZE_MAX);
}

صحیح memfd_سیٹ_sealed(صحیح fd);
صحیح memfd_get_sealed(صحیح fd);
صحیح memfd_سیٹ_sealed_gracefully(صحیح fd);

صحیح memfd_get_سائز(صحیح fd, uint64_t *ret);
صحیح memfd_سیٹ_سائز(صحیح fd, uint64_t sz);
