/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

কাঠামো iovec_wrapper {
        কাঠামো iovec *iovec;
        size_t গুনতি;
};

শূন্য iovw_done_free(কাঠামো iovec_wrapper *iovw);
শূন্য iovw_done(কাঠামো iovec_wrapper *iovw);

কাঠামো iovec_wrapper* iovw_free(কাঠামো iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(কাঠামো iovec_wrapper*, iovw_free);

কাঠামো iovec_wrapper* iovw_free_free(কাঠামো iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(কাঠামো iovec_wrapper*, iovw_free_free);

পূর্ণসংখ্যা iovw_compare(ধ্রুবক কাঠামো iovec_wrapper *a, ধ্রুবক কাঠামো iovec_wrapper *b) _pure_;
স্থির অন্তঃরেখ bool iovw_equal(ধ্রুবক কাঠামো iovec_wrapper *a, ধ্রুবক কাঠামো iovec_wrapper *b) {
        ফেরত iovw_compare(a, b) == 0;
}

পূর্ণসংখ্যা iovw_put_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, শূন্য *তথ্য, size_t len);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_put(কাঠামো iovec_wrapper *iovw, শূন্য *তথ্য, size_t len) {
        ফেরত iovw_put_full(iovw, false, তথ্য, len);
}
পূর্ণসংখ্যা iovw_put_iov_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec *iov);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_put_iov(কাঠামো iovec_wrapper *iovw, ধ্রুবক কাঠামো iovec *iov) {
        ফেরত iovw_put_iov_full(iovw, false, iov);
}
পূর্ণসংখ্যা iovw_put_iovw_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec_wrapper *source);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_put_iovw(কাঠামো iovec_wrapper *iovw, ধ্রুবক কাঠামো iovec_wrapper *source) {
        ফেরত iovw_put_iovw_full(iovw, false, source);
}
পূর্ণসংখ্যা iovw_consume_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, শূন্য *তথ্য, size_t len);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_consume(কাঠামো iovec_wrapper *iovw, শূন্য *তথ্য, size_t len) {
        ফেরত iovw_consume_full(iovw, false, তথ্য, len);
}
পূর্ণসংখ্যা iovw_consume_iov_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, কাঠামো iovec *iov);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_consume_iov(কাঠামো iovec_wrapper *iovw, কাঠামো iovec *iov) {
        ফেরত iovw_consume_iov_full(iovw, false, iov);
}
পূর্ণসংখ্যা iovw_extend_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক শূন্য *তথ্য, size_t len);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_extend(কাঠামো iovec_wrapper *iovw, ধ্রুবক শূন্য *তথ্য, size_t len) {
        ফেরত iovw_extend_full(iovw, false, তথ্য, len);
}
পূর্ণসংখ্যা iovw_extend_iov_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec *iov);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_extend_iov(কাঠামো iovec_wrapper *iovw, ধ্রুবক কাঠামো iovec *iov) {
        ফেরত iovw_extend_iov_full(iovw, false, iov);
}
পূর্ণসংখ্যা iovw_extend_iovw_full(কাঠামো iovec_wrapper *iovw, bool accept_zero, ধ্রুবক কাঠামো iovec_wrapper *source);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_extend_iovw(কাঠামো iovec_wrapper *iovw, ধ্রুবক কাঠামো iovec_wrapper *source) {
        ফেরত iovw_extend_iovw_full(iovw, false, source);
}

স্থির অন্তঃরেখ bool iovw_isempty(ধ্রুবক কাঠামো iovec_wrapper *iovw) {
        ফেরত !iovw || iovw->গুনতি == 0;
}

পূর্ণসংখ্যা iovw_put_string_field_full(কাঠামো iovec_wrapper *iovw, bool replace, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *মান);
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_put_string_field(কাঠামো iovec_wrapper *iovw, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *মান) {
        ফেরত iovw_put_string_field_full(iovw, false, field, মান);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা iovw_replace_string_field(কাঠামো iovec_wrapper *iovw, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *মান) {
        ফেরত iovw_put_string_field_full(iovw, true, field, মান);
}
পূর্ণসংখ্যা iovw_put_string_fieldf_full(কাঠামো iovec_wrapper *iovw, bool replace, ধ্রুবক অক্ষর *field, ধ্রুবক অক্ষর *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
পূর্ণসংখ্যা iovw_put_string_field_free(কাঠামো iovec_wrapper *iovw, ধ্রুবক অক্ষর *field, অক্ষর *মান);
শূন্য iovw_rebase(কাঠামো iovec_wrapper *iovw, শূন্য *old, শূন্য *new);
size_t iovw_আকার(ধ্রুবক কাঠামো iovec_wrapper *iovw);
পূর্ণসংখ্যা iovw_concat(ধ্রুবক কাঠামো iovec_wrapper *iovw, কাঠামো iovec *ret);
অক্ষর* iovw_to_cstring(ধ্রুবক কাঠামো iovec_wrapper *iovw);

পূর্ণসংখ্যা iovec_split(ধ্রুবক কাঠামো iovec *iov, size_t দৈর্ঘ্য_আকার, কাঠামো iovec_wrapper *ret);
পূর্ণসংখ্যা iovw_merge(ধ্রুবক কাঠামো iovec_wrapper *iovw, size_t দৈর্ঘ্য_আকার, কাঠামো iovec *ret);
