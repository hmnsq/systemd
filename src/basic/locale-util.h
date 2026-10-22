/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <libintl.h>    /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <locale.h>     /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା LocaleVariable {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_locales(ଅକ୍ଷର ***ret);
bool locale_is_valid(ଅଚଳ ଅକ୍ଷର *name);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା locale_is_installed(ଅଚଳ ଅକ୍ଷର *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(ଶୂନ୍ୟ);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ freelocalep(locale_t *p) {
        ଯଦି (*p == (locale_t) 0)
                ଫେରନ୍ତୁ;

        freelocale(*p);
}

ଶୂନ୍ୟ locale_variables_free(ଅକ୍ଷର* l[_VARIABLE_LC_MAX]);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ locale_variables_freep(ଅକ୍ଷର*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
ଶୂନ୍ୟ locale_variables_simplify(ଅକ୍ଷର *l[_VARIABLE_LC_MAX]);
