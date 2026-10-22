/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସଂରଚନା iovec_wrapper {
        ସଂରଚନା iovec *iovec;
        size_t ଗଣନା_2;
};

ଶୂନ୍ୟ iovw_done_free(ସଂରଚନା iovec_wrapper *iovw);
ଶୂନ୍ୟ iovw_done(ସଂରଚନା iovec_wrapper *iovw);

ସଂରଚନା iovec_wrapper* iovw_free(ସଂରଚନା iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ସଂରଚନା iovec_wrapper*, iovw_free);

ସଂରଚନା iovec_wrapper* iovw_free_free(ସଂରଚନା iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ସଂରଚନା iovec_wrapper*, iovw_free_free);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_compare(ଅଚଳ ସଂରଚନା iovec_wrapper *a, ଅଚଳ ସଂରଚନା iovec_wrapper *b) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool iovw_equal(ଅଚଳ ସଂରଚନା iovec_wrapper *a, ଅଚଳ ସଂରଚନା iovec_wrapper *b) {
        ଫେରନ୍ତୁ iovw_compare(a, b) == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଶୂନ୍ୟ *data, size_t len);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put(ସଂରଚନା iovec_wrapper *iovw, ଶୂନ୍ୟ *data, size_t len) {
        ଫେରନ୍ତୁ iovw_put_full(iovw, false, data, len);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_iov_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec *iov);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_iov(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ସଂରଚନା iovec *iov) {
        ଫେରନ୍ତୁ iovw_put_iov_full(iovw, false, iov);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_iovw_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec_wrapper *source);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_iovw(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ସଂରଚନା iovec_wrapper *source) {
        ଫେରନ୍ତୁ iovw_put_iovw_full(iovw, false, source);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_consume_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଶୂନ୍ୟ *data, size_t len);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_consume(ସଂରଚନା iovec_wrapper *iovw, ଶୂନ୍ୟ *data, size_t len) {
        ଫେରନ୍ତୁ iovw_consume_full(iovw, false, data, len);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_consume_iov_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ସଂରଚନା iovec *iov);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_consume_iov(ସଂରଚନା iovec_wrapper *iovw, ସଂରଚନା iovec *iov) {
        ଫେରନ୍ତୁ iovw_consume_iov_full(iovw, false, iov);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ଶୂନ୍ୟ *data, size_t len);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ଶୂନ୍ୟ *data, size_t len) {
        ଫେରନ୍ତୁ iovw_extend_full(iovw, false, data, len);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_iov_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec *iov);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_iov(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ସଂରଚନା iovec *iov) {
        ଫେରନ୍ତୁ iovw_extend_iov_full(iovw, false, iov);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_iovw_full(ସଂରଚନା iovec_wrapper *iovw, bool accept_zero, ଅଚଳ ସଂରଚନା iovec_wrapper *source);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_extend_iovw(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ସଂରଚନା iovec_wrapper *source) {
        ଫେରନ୍ତୁ iovw_extend_iovw_full(iovw, false, source);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool iovw_isempty(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw) {
        ଫେରନ୍ତୁ !iovw || iovw->ଗଣନା_2 == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_field_full(ସଂରଚନା iovec_wrapper *iovw, bool replace, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_field(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ iovw_put_string_field_full(iovw, false, field, ମୂଲ୍ୟ);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_replace_string_field(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ iovw_put_string_field_full(iovw, true, field, ମୂଲ୍ୟ);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_fieldf_full(ସଂରଚନା iovec_wrapper *iovw, bool replace, ଅଚଳ ଅକ୍ଷର *field, ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_put_string_field_free(ସଂରଚନା iovec_wrapper *iovw, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର *ମୂଲ୍ୟ);
ଶୂନ୍ୟ iovw_rebase(ସଂରଚନା iovec_wrapper *iovw, ଶୂନ୍ୟ *old, ଶୂନ୍ୟ *new);
size_t iovw_size(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_concat(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw, ସଂରଚନା iovec *ret);
ଅକ୍ଷର* iovw_to_cstring(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovec_split(ଅଚଳ ସଂରଚନା iovec *iov, size_t length_size, ସଂରଚନା iovec_wrapper *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା iovw_merge(ଅଚଳ ସଂରଚନା iovec_wrapper *iovw, size_t length_size, ସଂରଚନା iovec *ret);
