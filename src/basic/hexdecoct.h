/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

અક્ષર octchar(પૂર્ણાંક x) _const_;
પૂર્ણાંક unoctchar(અક્ષર c) _const_;

અક્ષર decchar(પૂર્ણાંક x) _const_;
પૂર્ણાંક undecchar(અક્ષર c) _const_;

અક્ષર hexchar(પૂર્ણાંક x) _const_;
પૂર્ણાંક unhexchar(અક્ષર c) _const_;

અક્ષર* hexmem(અચલ શૂન્ય *p, size_t l) _nonnull_if_nonzero_(1, 2);
પૂર્ણાંક unhexmem_full(અચલ અક્ષર *p, size_t l, bool secure, શૂન્ય **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
સ્થિર અંતઃરેખ પૂર્ણાંક unhexmem(અચલ અક્ષર *p, શૂન્ય **ret_data, size_t *ret_size) {
        પાછા unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

અક્ષર base32hexchar(પૂર્ણાંક x) _const_;
પૂર્ણાંક unbase32hexchar(અક્ષર c) _const_;

અક્ષર base64char(પૂર્ણાંક x) _const_;
અક્ષર urlsafe_base64char(પૂર્ણાંક x) _const_;
પૂર્ણાંક unbase64char(અક્ષર c) _const_;

અક્ષર* base32hexmem(અચલ શૂન્ય *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
પૂર્ણાંક unbase32hexmem(અચલ અક્ષર *p, size_t l, bool padding, શૂન્ય **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(અચલ શૂન્ય *p, size_t l, size_t લીટી_break, અક્ષર **ret) _nonnull_if_nonzero_(1, 2);
સ્થિર અંતઃરેખ ssize_t base64mem(અચલ શૂન્ય *p, size_t l, અક્ષર **ret) {
        પાછા base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                અક્ષર **prefix,
                size_t plen,
                અચલ શૂન્ય *p,
                size_t l,
                size_t indent,
                size_t width);
પૂર્ણાંક unbase64mem_full(અચલ અક્ષર *p, size_t l, bool secure, શૂન્ય **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
સ્થિર અંતઃરેખ પૂર્ણાંક unbase64mem(અચલ અક્ષર *p, શૂન્ય **ret_data, size_t *ret_size) {
        પાછા unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

શૂન્ય hexdump(FILE *f, અચલ શૂન્ય *p, size_t s) _nonnull_if_nonzero_(2, 3);
