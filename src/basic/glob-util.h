/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <glob.h>       /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા DIR* (*opendir_t)(અચલ અક્ષર *);

પૂર્ણાંક safe_glob_full(અચલ અક્ષર *path, પૂર્ણાંક flags, opendir_t opendir_func, અક્ષર ***ret);
સ્થિર અંતઃરેખ પૂર્ણાંક safe_glob(અચલ અક્ષર *path, પૂર્ણાંક flags, અક્ષર ***ret) {
        પાછા safe_glob_full(path, flags, ખાલી, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
પૂર્ણાંક glob_first(અચલ અક્ષર *path, અક્ષર **ret);
#define glob_exists(path) glob_first(path, ખાલી)
પૂર્ણાંક glob_extend(અક્ષર ***strv, અચલ અક્ષર *path, પૂર્ણાંક flags);

પૂર્ણાંક glob_non_glob_prefix(અચલ અક્ષર *path, અક્ષર **ret);

bool string_is_glob(અચલ અક્ષર *p) _pure_;
