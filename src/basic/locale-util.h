/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <libintl.h>    /* IWYU pragma: export */
#அந்தர்பாவ <locale.h>     /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை LocaleVariable {
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

முழு get_locales(எழுத்து ***ret);
bool locale_is_valid(மாறிலி எழுத்து *name);
முழு locale_is_installed(மாறிலி எழுத்து *name);

#define _(String) dgettext(GETTEXT_PACKAGE, String)
#define N_(String) String

bool is_locale_utf8(வெறுமை);

DECLARE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);

நிலையான உள்வரி வெறுமை freelocalep(locale_t *p) {
        எனில் (*p == (locale_t) 0)
                திரும்பு;

        freelocale(*p);
}

வெறுமை locale_variables_free(எழுத்து* l[_VARIABLE_LC_MAX]);
நிலையான உள்வரி வெறுமை locale_variables_freep(எழுத்து*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
வெறுமை locale_variables_simplify(எழுத்து *l[_VARIABLE_LC_MAX]);
