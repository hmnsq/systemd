/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <libintl.h>    /* IWYU pragma: export */
#شامل <locale.h>     /* IWYU pragma: export */

#شامل "forward.h"

تعریفنوع شمارش LocaleVariable {
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

صحیح get_locales(نویسه ***ret);
bool locale_is_valid(ثابت نویسه *name);
صحیح locale_is_installed(ثابت نویسه *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(تهی);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

ایستا درونخطی تهی freelocalep(locale_t *p) {
        اگر (*p == (locale_t) 0)
                بازگشت;

        freelocale(*p);
}

تهی locale_variables_free(نویسه* l[_VARIABLE_LC_MAX]);
ایستا درونخطی تهی locale_variables_freep(نویسه*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
تهی locale_variables_simplify(نویسه *l[_VARIABLE_LC_MAX]);
