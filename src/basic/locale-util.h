/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <libintl.h>    /* IWYU pragma: export */
#अन्तर्भाव <locale.h>     /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना LocaleVariable {
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

पूर्णाङ्क get_locales(वर्ण ***ret);
bool locale_is_valid(अचर वर्ण *name);
पूर्णाङ्क locale_is_installed(अचर वर्ण *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(शून्य);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

स्थिर अन्तःस्थ शून्य freelocalep(locale_t *p) {
        यदि (*p == (locale_t) 0)
                फर्काऊ;

        freelocale(*p);
}

शून्य locale_variables_free(वर्ण* l[_VARIABLE_LC_MAX]);
स्थिर अन्तःस्थ शून्य locale_variables_freep(वर्ण*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
शून्य locale_variables_simplify(वर्ण *l[_VARIABLE_LC_MAX]);
