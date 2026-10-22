/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <libintl.h>    /* IWYU pragma: export */
#അന്തര്ഭാവ <locale.h>     /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ LocaleVariable {
        /* We don't list LC_ALL here on purpose. People should be
         * using LANG instead. */

        VARIABLE_LANG,
        VARIABLE_LANGUAGE,
        VARIABLE_LC_CTYPE,
        VARIABLE_LC_NUMERIC,
        VARIABLE_LC_TIME,
        VARIABLE_LC_COLLATE,
        VARIABLE_LC_MONETARY,
        VARIABLE_LC_MESSAGES,
        VARIABLE_LC_PAPER,
        VARIABLE_LC_NAME,
        VARIABLE_LC_ADDRESS,
        VARIABLE_LC_TELEPHONE,
        VARIABLE_LC_MEASUREMENT,
        VARIABLE_LC_IDENTIFICATION,
        _VARIABLE_LC_MAX,
        _VARIABLE_LC_INVALID = -EINVAL,
} LocaleVariable;

പൂർണ്ണസംഖ്യ get_locales(അക്ഷരം ***ret);
bool locale_is_valid(സ്ഥിരം അക്ഷരം *name);
പൂർണ്ണസംഖ്യ locale_is_installed(സ്ഥിരം അക്ഷരം *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(ശൂന്യം);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

സ്ഥിതം വരിയിൽ ശൂന്യം freelocalep(locale_t *p) {
        എങ്കിൽ (*p == (locale_t) 0)
                തിരികെ;

        freelocale(*p);
}

ശൂന്യം locale_variables_free(അക്ഷരം* l[_VARIABLE_LC_MAX]);
സ്ഥിതം വരിയിൽ ശൂന്യം locale_variables_freep(അക്ഷരം*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
ശൂന്യം locale_variables_simplify(അക്ഷരം *l[_VARIABLE_LC_MAX]);
