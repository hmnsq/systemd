/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/mman.h>           /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

পূর্ণাঙ্ক memfd_create_wrapper(ধ্রুব বর্ণ *name, unsigned mode);

পূর্ণাঙ্ক memfd_new_full(ধ্রুব বর্ণ *name, unsigned extra_flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক memfd_new(ধ্রুব বর্ণ *name) {
        প্রত্যাগম memfd_new_full(name, 0);
}

পূর্ণাঙ্ক memfd_new_and_seal(ধ্রুব বর্ণ *name, ধ্রুব শূন্য *data, size_t sz) _nonnull_if_nonzero_(2, 3);
স্থির অন্তঃস্থ পূর্ণাঙ্ক memfd_new_and_seal_string(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *s) {
        প্রত্যাগম memfd_new_and_seal(name, s, SIZE_MAX);
}

পূর্ণাঙ্ক memfd_set_sealed(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক memfd_get_sealed(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক memfd_set_sealed_gracefully(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক memfd_get_size(পূর্ণাঙ্ক fd, uint64_t *ret);
পূর্ণাঙ্ক memfd_set_size(পূর্ণাঙ্ক fd, uint64_t sz);
