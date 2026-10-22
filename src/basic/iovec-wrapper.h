/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

સંરચના iovec_wrapper {
        સંરચના iovec *iovec;
        size_t ગણતરી;
};

શૂન્ય iovw_done_free(સંરચના iovec_wrapper *iovw);
શૂન્ય iovw_done(સંરચના iovec_wrapper *iovw);

સંરચના iovec_wrapper* iovw_free(સંરચના iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(સંરચના iovec_wrapper*, iovw_free);

સંરચના iovec_wrapper* iovw_free_free(સંરચના iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(સંરચના iovec_wrapper*, iovw_free_free);

પૂર્ણાંક iovw_compare(અચલ સંરચના iovec_wrapper *a, અચલ સંરચના iovec_wrapper *b) _pure_;
સ્થિર અંતઃરેખ bool iovw_equal(અચલ સંરચના iovec_wrapper *a, અચલ સંરચના iovec_wrapper *b) {
        પાછા iovw_compare(a, b) == 0;
}

પૂર્ણાંક iovw_put_full(સંરચના iovec_wrapper *iovw, bool accept_zero, શૂન્ય *data, size_t len);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_put(સંરચના iovec_wrapper *iovw, શૂન્ય *data, size_t len) {
        પાછા iovw_put_full(iovw, false, data, len);
}
પૂર્ણાંક iovw_put_iov_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec *iov);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_put_iov(સંરચના iovec_wrapper *iovw, અચલ સંરચના iovec *iov) {
        પાછા iovw_put_iov_full(iovw, false, iov);
}
પૂર્ણાંક iovw_put_iovw_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec_wrapper *source);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_put_iovw(સંરચના iovec_wrapper *iovw, અચલ સંરચના iovec_wrapper *source) {
        પાછા iovw_put_iovw_full(iovw, false, source);
}
પૂર્ણાંક iovw_consume_full(સંરચના iovec_wrapper *iovw, bool accept_zero, શૂન્ય *data, size_t len);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_consume(સંરચના iovec_wrapper *iovw, શૂન્ય *data, size_t len) {
        પાછા iovw_consume_full(iovw, false, data, len);
}
પૂર્ણાંક iovw_consume_iov_full(સંરચના iovec_wrapper *iovw, bool accept_zero, સંરચના iovec *iov);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_consume_iov(સંરચના iovec_wrapper *iovw, સંરચના iovec *iov) {
        પાછા iovw_consume_iov_full(iovw, false, iov);
}
પૂર્ણાંક iovw_extend_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ શૂન્ય *data, size_t len);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_extend(સંરચના iovec_wrapper *iovw, અચલ શૂન્ય *data, size_t len) {
        પાછા iovw_extend_full(iovw, false, data, len);
}
પૂર્ણાંક iovw_extend_iov_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec *iov);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_extend_iov(સંરચના iovec_wrapper *iovw, અચલ સંરચના iovec *iov) {
        પાછા iovw_extend_iov_full(iovw, false, iov);
}
પૂર્ણાંક iovw_extend_iovw_full(સંરચના iovec_wrapper *iovw, bool accept_zero, અચલ સંરચના iovec_wrapper *source);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_extend_iovw(સંરચના iovec_wrapper *iovw, અચલ સંરચના iovec_wrapper *source) {
        પાછા iovw_extend_iovw_full(iovw, false, source);
}

સ્થિર અંતઃરેખ bool iovw_isempty(અચલ સંરચના iovec_wrapper *iovw) {
        પાછા !iovw || iovw->ગણતરી == 0;
}

પૂર્ણાંક iovw_put_string_field_full(સંરચના iovec_wrapper *iovw, bool replace, અચલ અક્ષર *field, અચલ અક્ષર *કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_put_string_field(સંરચના iovec_wrapper *iovw, અચલ અક્ષર *field, અચલ અક્ષર *કિંમત) {
        પાછા iovw_put_string_field_full(iovw, false, field, કિંમત);
}
સ્થિર અંતઃરેખ પૂર્ણાંક iovw_replace_string_field(સંરચના iovec_wrapper *iovw, અચલ અક્ષર *field, અચલ અક્ષર *કિંમત) {
        પાછા iovw_put_string_field_full(iovw, true, field, કિંમત);
}
પૂર્ણાંક iovw_put_string_fieldf_full(સંરચના iovec_wrapper *iovw, bool replace, અચલ અક્ષર *field, અચલ અક્ષર *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
પૂર્ણાંક iovw_put_string_field_free(સંરચના iovec_wrapper *iovw, અચલ અક્ષર *field, અક્ષર *કિંમત);
શૂન્ય iovw_rebase(સંરચના iovec_wrapper *iovw, શૂન્ય *old, શૂન્ય *new);
size_t iovw_size(અચલ સંરચના iovec_wrapper *iovw);
પૂર્ણાંક iovw_concat(અચલ સંરચના iovec_wrapper *iovw, સંરચના iovec *ret);
અક્ષર* iovw_to_cstring(અચલ સંરચના iovec_wrapper *iovw);

પૂર્ણાંક iovec_split(અચલ સંરચના iovec *iov, size_t length_size, સંરચના iovec_wrapper *ret);
પૂર્ણાંક iovw_merge(અચલ સંરચના iovec_wrapper *iovw, size_t length_size, સંરચના iovec *ret);
