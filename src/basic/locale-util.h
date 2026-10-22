/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <libintl.h>    /* IWYU pragma: export */
#અન્તર્ભાવ <locale.h>     /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના LocaleVariable {
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

પૂર્ણાંક get_locales(અક્ષર ***ret);
bool locale_is_valid(અચલ અક્ષર *name);
પૂર્ણાંક locale_is_installed(અચલ અક્ષર *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(શૂન્ય);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

સ્થિર અંતઃરેખ શૂન્ય freelocalep(locale_t *p) {
        જો (*p == (locale_t) 0)
                પાછા;

        freelocale(*p);
}

શૂન્ય locale_variables_free(અક્ષર* l[_VARIABLE_LC_MAX]);
સ્થિર અંતઃરેખ શૂન્ય locale_variables_freep(અક્ષર*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
શૂન્ય locale_variables_simplify(અક્ષર *l[_VARIABLE_LC_MAX]);
