/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/mman.h>           /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ memfd_create_wrapper(സ്ഥിരം അക്ഷരം *name, unsigned mode);

പൂർണ്ണസംഖ്യ memfd_new_full(സ്ഥിരം അക്ഷരം *name, unsigned extra_flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ memfd_new(സ്ഥിരം അക്ഷരം *name) {
        തിരികെ memfd_new_full(name, 0);
}

പൂർണ്ണസംഖ്യ memfd_new_and_seal(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം ശൂന്യം *data, size_t sz) _nonnull_if_nonzero_(2, 3);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ memfd_new_and_seal_string(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *s) {
        തിരികെ memfd_new_and_seal(name, s, SIZE_MAX);
}

പൂർണ്ണസംഖ്യ memfd_set_sealed(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ memfd_get_sealed(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ memfd_set_sealed_gracefully(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ memfd_get_size(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);
പൂർണ്ണസംഖ്യ memfd_set_size(പൂർണ്ണസംഖ്യ fd, uint64_t sz);
