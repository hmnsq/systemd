/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <libintl.h>    /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <locale.h>     /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ LocaleVariable {
        /* We don't list LC_ALL here on purpose. People should be
         * using LANG instead. */

        VARIABLE_LANG,
        VARIABLE_LANGUAGE,
        VARIABLE_LC_CTYPE,
        VARIABLE_LC_NUMERIC,
        VARIABLE_LC_ਸਮਾਂ,
        VARIABLE_LC_COLLATE,
        VARIABLE_LC_MONETARY,
        VARIABLE_LC_MESSAGES,
        VARIABLE_LC_PAPER,
        VARIABLE_LC_NAME,
        VARIABLE_LC_ਪਤਾ,
        VARIABLE_LC_TELEPHONE,
        VARIABLE_LC_MEASUREMENT,
        VARIABLE_LC_IDENTIFICATION,
        _VARIABLE_LC_MAX,
        _VARIABLE_LC_INVALID = -EINVAL,
} LocaleVariable;

ਪੂਰਨ get_locales(ਅੱਖਰ ***ret);
bool locale_is_valid(ਅਚਲ ਅੱਖਰ *name);
ਪੂਰਨ locale_is_installed(ਅਚਲ ਅੱਖਰ *name);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _(String) dgettext(GETTEXT_PACKAGE, String)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ N_(String) String

bool is_locale_utf8(ਖਾਲੀ);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(locale_variable, LocaleVariable);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ freelocalep(locale_t *p) {
        ਜੇ (*p == (locale_t) 0)
                ਵਾਪਸ;

        freelocale(*p);
}

ਖਾਲੀ locale_variables_free(ਅੱਖਰ* l[_VARIABLE_LC_MAX]);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ locale_variables_freep(ਅੱਖਰ*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
ਖਾਲੀ locale_variables_simplify(ਅੱਖਰ *l[_VARIABLE_LC_MAX]);
