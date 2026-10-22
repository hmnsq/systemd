/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

അക്ഷരം octchar(പൂർണ്ണസംഖ്യ x) _const_;
പൂർണ്ണസംഖ്യ unoctchar(അക്ഷരം c) _const_;

അക്ഷരം decchar(പൂർണ്ണസംഖ്യ x) _const_;
പൂർണ്ണസംഖ്യ undecchar(അക്ഷരം c) _const_;

അക്ഷരം hexchar(പൂർണ്ണസംഖ്യ x) _const_;
പൂർണ്ണസംഖ്യ unhexchar(അക്ഷരം c) _const_;

അക്ഷരം* hexmem(സ്ഥിരം ശൂന്യം *p, size_t l) _nonnull_if_nonzero_(1, 2);
പൂർണ്ണസംഖ്യ unhexmem_full(സ്ഥിരം അക്ഷരം *p, size_t l, bool secure, ശൂന്യം **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ unhexmem(സ്ഥിരം അക്ഷരം *p, ശൂന്യം **ret_data, size_t *ret_size) {
        തിരികെ unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

അക്ഷരം base32hexchar(പൂർണ്ണസംഖ്യ x) _const_;
പൂർണ്ണസംഖ്യ unbase32hexchar(അക്ഷരം c) _const_;

അക്ഷരം base64char(പൂർണ്ണസംഖ്യ x) _const_;
അക്ഷരം urlsafe_base64char(പൂർണ്ണസംഖ്യ x) _const_;
പൂർണ്ണസംഖ്യ unbase64char(അക്ഷരം c) _const_;

അക്ഷരം* base32hexmem(സ്ഥിരം ശൂന്യം *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
പൂർണ്ണസംഖ്യ unbase32hexmem(സ്ഥിരം അക്ഷരം *p, size_t l, bool padding, ശൂന്യം **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(സ്ഥിരം ശൂന്യം *p, size_t l, size_t വരി_break, അക്ഷരം **ret) _nonnull_if_nonzero_(1, 2);
സ്ഥിതം വരിയിൽ ssize_t base64mem(സ്ഥിരം ശൂന്യം *p, size_t l, അക്ഷരം **ret) {
        തിരികെ base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                അക്ഷരം **prefix,
                size_t plen,
                സ്ഥിരം ശൂന്യം *p,
                size_t l,
                size_t indent,
                size_t width);
പൂർണ്ണസംഖ്യ unbase64mem_full(സ്ഥിരം അക്ഷരം *p, size_t l, bool secure, ശൂന്യം **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ unbase64mem(സ്ഥിരം അക്ഷരം *p, ശൂന്യം **ret_data, size_t *ret_size) {
        തിരികെ unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

ശൂന്യം hexdump(FILE *f, സ്ഥിരം ശൂന്യം *p, size_t s) _nonnull_if_nonzero_(2, 3);
