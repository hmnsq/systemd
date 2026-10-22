/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <libintl.h>    /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <locale.h>     /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ LocaleVariable {
        /* We don't list LC_ALL here on purpose. People should be
         * using LANG instead. */

        VARIABLE_LANG,
        VARIABLE_LANGUAGE,
        VARIABLE_LC_CTYPE,
        VARIABLE_LC_NUMERIC,
        VARIABLE_LC_ಸಮಯ,
        VARIABLE_LC_COLLATE,
        VARIABLE_LC_MONETARY,
        VARIABLE_LC_MESSAGES,
        VARIABLE_LC_PAPER,
        VARIABLE_LC_NAME,
        VARIABLE_LC_ವಿಳಾಸ,
        VARIABLE_LC_TELEPHONE,
        VARIABLE_LC_MEASUREMENT,
        VARIABLE_LC_IDENTIFICATION,
        _VARIABLE_LC_MAX,
        _VARIABLE_LC_INVALID = -EINVAL,
} LocaleVariable;

ಪೂರ್ಣಾಂಕ get_locales(ಅಕ್ಷರ ***ret);
bool locale_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಪೂರ್ಣಾಂಕ locale_is_installed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

#ವ್ಯಾಖ್ಯಾನಿಸು _(String) dgettext(GETTEXT_PACKAGE, String)
#ವ್ಯಾಖ್ಯಾನಿಸು N_(String) String

bool is_locale_utf8(ಶೂನ್ಯ);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(locale_variable, LocaleVariable);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ freelocalep(locale_t *p) {
        ಆದರೆ (*p == (locale_t) 0)
                ಹಿಂತಿರುಗಿ;

        freelocale(*p);
}

ಶೂನ್ಯ locale_variables_free(ಅಕ್ಷರ* l[_VARIABLE_LC_MAX]);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ locale_variables_freep(ಅಕ್ಷರ*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
ಶೂನ್ಯ locale_variables_simplify(ಅಕ್ಷರ *l[_VARIABLE_LC_MAX]);
