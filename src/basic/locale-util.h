/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <libintl.h>    /* IWYU pragma: export */
#شامل <locale.h>     /* IWYU pragma: export */

#شامل "forward.h"

قسمتعریف شمار LocaleVariable {
        /* We don't list LC_ALL here on purpose. People should be
         * using LANG instead. */

        VARIABLE_LANG,
        VARIABLE_LANGUAGE,
        VARIABLE_LC_CTYPE,
        VARIABLE_LC_NUMERIC,
        VARIABLE_LC_وقت,
        VARIABLE_LC_COLLATE,
        VARIABLE_LC_MONETARY,
        VARIABLE_LC_MESSAGES,
        VARIABLE_LC_PAPER,
        VARIABLE_LC_NAME,
        VARIABLE_LC_پتہ,
        VARIABLE_LC_TELEPHONE,
        VARIABLE_LC_MEASUREMENT,
        VARIABLE_LC_IDENTIFICATION,
        _VARIABLE_LC_MAX,
        _VARIABLE_LC_INVALID = -EINVAL,
} LocaleVariable;

صحیح get_locales(حرف ***ret);
bool locale_is_valid(ثابت حرف *name);
صحیح locale_is_installed(ثابت حرف *name);

#تعریف_کرو _(String) dgettext(GETTEXT_PACKAGE, String)
#تعریف_کرو N_(String) String

bool is_locale_utf8(خالی);

DECLARE_STRING_جدول_LOOKUP(locale_variable, LocaleVariable);

ساکن اندرونی خالی freelocalep(locale_t *p) {
        اگر (*p == (locale_t) 0)
                واپس;

        freelocale(*p);
}

خالی locale_variables_free(حرف* l[_VARIABLE_LC_MAX]);
ساکن اندرونی خالی locale_variables_freep(حرف*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
خالی locale_variables_simplify(حرف *l[_VARIABLE_LC_MAX]);
