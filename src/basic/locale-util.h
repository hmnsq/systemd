/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <libintl.h>    /* IWYU pragma: export */
#ضمن <locale.h>     /* IWYU pragma: export */

#ضمن "forward.h"

تعريفنوع تعداد LocaleVariable {
        /* We don't list LC_ALL here on purpose. People should be
         * using LANG instead. */

        VARIABLE_LANG,
        VARIABLE_LANGUAGE,
        VARIABLE_LC_CTYPE,
        VARIABLE_LC_NUMERIC,
        VARIABLE_LC_زمان,
        VARIABLE_LC_COLLATE,
        VARIABLE_LC_MONETARY,
        VARIABLE_LC_MESSAGES,
        VARIABLE_LC_PAPER,
        VARIABLE_LC_NAME,
        VARIABLE_LC_آدرس,
        VARIABLE_LC_TELEPHONE,
        VARIABLE_LC_MEASUREMENT,
        VARIABLE_LC_IDENTIFICATION,
        _VARIABLE_LC_MAX,
        _VARIABLE_LC_INVALID = -EINVAL,
} LocaleVariable;

صحيح get_locales(حرف ***ret);
bool locale_is_valid(ثابت حرف *name);
صحيح locale_is_installed(ثابت حرف *name);

#تعریف_کن _(String) dgettext(GETTEXT_PACKAGE, String)
#تعریف_کن N_(String) String

bool is_locale_utf8(فراغ);

DECLARE_STRING_جدول_LOOKUP(locale_variable, LocaleVariable);

ساكن مضمن فراغ freelocalep(locale_t *p) {
        إذا (*p == (locale_t) 0)
                أرجع;

        freelocale(*p);
}

فراغ locale_variables_free(حرف* l[_VARIABLE_LC_MAX]);
ساكن مضمن فراغ locale_variables_freep(حرف*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
فراغ locale_variables_simplify(حرف *l[_VARIABLE_LC_MAX]);
