/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <libintl.h>    /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <locale.h>     /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ LocaleVariable {
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

ಪೂರ್ಣಾಙ್ಕ get_locales(ವರ್ಣ ***ret);
bool locale_is_valid(ಧ್ರುವ ವರ್ಣ *name);
ಪೂರ್ಣಾಙ್ಕ locale_is_installed(ಧ್ರುವ ವರ್ಣ *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(ಶೂನ್ಯ);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ freelocalep(locale_t *p) {
        ಯದಿ (*p == (locale_t) 0)
                ಪ್ರತ್ಯಾಗಮ;

        freelocale(*p);
}

ಶೂನ್ಯ locale_variables_free(ವರ್ಣ* l[_VARIABLE_LC_MAX]);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ locale_variables_freep(ವರ್ಣ*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
ಶೂನ್ಯ locale_variables_simplify(ವರ್ಣ *l[_VARIABLE_LC_MAX]);
