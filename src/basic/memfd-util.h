/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/mman.h>           /* IWYU pragma: export */

#شامل "forward.h"

صحیح memfd_create_wrapper(ثابت نویسه *name, unsigned mode);

صحیح memfd_new_full(ثابت نویسه *name, unsigned extra_flags);
ایستا درونخطی صحیح memfd_new(ثابت نویسه *name) {
        بازگشت memfd_new_full(name, 0);
}

صحیح memfd_new_and_seal(ثابت نویسه *name, ثابت تهی *data, size_t sz) _nonnull_if_nonzero_(2, 3);
ایستا درونخطی صحیح memfd_new_and_seal_string(ثابت نویسه *name, ثابت نویسه *s) {
        بازگشت memfd_new_and_seal(name, s, SIZE_MAX);
}

صحیح memfd_set_sealed(صحیح fd);
صحیح memfd_get_sealed(صحیح fd);
صحیح memfd_set_sealed_gracefully(صحیح fd);

صحیح memfd_get_size(صحیح fd, uint64_t *ret);
صحیح memfd_set_size(صحیح fd, uint64_t sz);
