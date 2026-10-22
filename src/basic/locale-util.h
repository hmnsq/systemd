/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <libintl.h>    /* IWYU pragma: export */
#समावेश <locale.h>     /* IWYU pragma: export */

#समावेश "forward.h"

प्रकार क्रमागत LocaleVariable {
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

पूर्णांक get_locales(अक्षर ***ret);
bool locale_is_valid(स्थिर अक्षर *name);
पूर्णांक locale_is_installed(स्थिर अक्षर *name);

#घोषणा _(String) dgettext(GETTEXT_PACKAGE, String)
#घोषणा N_(String) String

bool is_locale_utf8(व्योम);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

अटल अंतरभूत व्योम freelocalep(locale_t *p) {
        अगर (*p == (locale_t) 0)
                वापस;

        freelocale(*p);
}

व्योम locale_variables_free(अक्षर* l[_VARIABLE_LC_MAX]);
अटल अंतरभूत व्योम locale_variables_freep(अक्षर*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
व्योम locale_variables_simplify(अक्षर *l[_VARIABLE_LC_MAX]);
