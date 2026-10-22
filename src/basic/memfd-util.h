/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/mman.h>           /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা memfd_create_wrapper(ধ্রুবক অক্ষর *name, unsigned mode);

পূর্ণসংখ্যা memfd_new_full(ধ্রুবক অক্ষর *name, unsigned extra_flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা memfd_new(ধ্রুবক অক্ষর *name) {
        ফেরত memfd_new_full(name, 0);
}

পূর্ণসংখ্যা memfd_new_and_seal(ধ্রুবক অক্ষর *name, ধ্রুবক শূন্য *তথ্য, size_t sz) _nonnull_if_nonzero_(2, 3);
স্থির অন্তঃরেখ পূর্ণসংখ্যা memfd_new_and_seal_string(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *s) {
        ফেরত memfd_new_and_seal(name, s, SIZE_MAX);
}

পূর্ণসংখ্যা memfd_set_sealed(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা memfd_get_sealed(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা memfd_set_sealed_gracefully(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা memfd_get_আকার(পূর্ণসংখ্যা fd, uint64_t *ret);
পূর্ণসংখ্যা memfd_set_আকার(পূর্ণসংখ্যা fd, uint64_t sz);
