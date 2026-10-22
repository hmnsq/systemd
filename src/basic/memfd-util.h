/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/mman.h>           /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક memfd_create_wrapper(અચલ અક્ષર *name, unsigned mode);

પૂર્ણાંક memfd_new_full(અચલ અક્ષર *name, unsigned extra_flags);
સ્થિર અંતઃરેખ પૂર્ણાંક memfd_new(અચલ અક્ષર *name) {
        પાછા memfd_new_full(name, 0);
}

પૂર્ણાંક memfd_new_and_seal(અચલ અક્ષર *name, અચલ શૂન્ય *data, size_t sz) _nonnull_if_nonzero_(2, 3);
સ્થિર અંતઃરેખ પૂર્ણાંક memfd_new_and_seal_string(અચલ અક્ષર *name, અચલ અક્ષર *s) {
        પાછા memfd_new_and_seal(name, s, SIZE_MAX);
}

પૂર્ણાંક memfd_set_sealed(પૂર્ણાંક fd);
પૂર્ણાંક memfd_get_sealed(પૂર્ણાંક fd);
પૂર્ણાંક memfd_set_sealed_gracefully(પૂર્ણાંક fd);

પૂર્ણાંક memfd_get_size(પૂર્ણાંક fd, uint64_t *ret);
પૂર્ણાંક memfd_set_size(પૂર્ણાંક fd, uint64_t sz);
