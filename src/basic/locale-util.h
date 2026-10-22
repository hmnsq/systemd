/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <libintl.h>    /* IWYU pragma: export */
#অন্তর্ভাব <locale.h>     /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা LocaleVariable {
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

পূর্ণসংখ্যা get_locales(অক্ষর ***ret);
bool locale_is_valid(ধ্রুবক অক্ষর *name);
পূর্ণসংখ্যা locale_is_installed(ধ্রুবক অক্ষর *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(শূন্য);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

স্থির অন্তঃরেখ শূন্য freelocalep(locale_t *p) {
        যদি (*p == (locale_t) 0)
                ফেরত;

        freelocale(*p);
}

শূন্য locale_variables_free(অক্ষর* l[_VARIABLE_LC_MAX]);
স্থির অন্তঃরেখ শূন্য locale_variables_freep(অক্ষর*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
শূন্য locale_variables_simplify(অক্ষর *l[_VARIABLE_LC_MAX]);
