/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <libintl.h>    /* IWYU pragma: export */
#అన్తర్భావ <locale.h>     /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన LocaleVariable {
        /* We don't list LC_ALL here on purpose. People should be
         * using LANG instead. */

        VARIABLE_LANG,
        VARIABLE_LANGUAGE,
        VARIABLE_LC_CTYPE,
        VARIABLE_LC_NUMERIC,
        VARIABLE_LC_సమయం,
        VARIABLE_LC_COLLATE,
        VARIABLE_LC_MONETARY,
        VARIABLE_LC_MESSAGES,
        VARIABLE_LC_PAPER,
        VARIABLE_LC_NAME,
        VARIABLE_LC_చిరునామా,
        VARIABLE_LC_TELEPHONE,
        VARIABLE_LC_MEASUREMENT,
        VARIABLE_LC_IDENTIFICATION,
        _VARIABLE_LC_MAX,
        _VARIABLE_LC_INVALID = -EINVAL,
} LocaleVariable;

పూర్ణాంకం get_locales(అక్షరం ***ret);
bool locale_is_valid(స్థిరరాశి అక్షరం *name);
పూర్ణాంకం locale_is_installed(స్థిరరాశి అక్షరం *name);

#నిర్వచించు _(String) dgettext(GETTEXT_PACKAGE, String)
#నిర్వచించు N_(String) String

bool is_locale_utf8(శూన్య);

DECLARE_STRING_పట్టిక_LOOKUP(locale_variable, LocaleVariable);

స్థిర అంతఃవరుస శూన్య freelocalep(locale_t *p) {
        ఒకవేళ (*p == (locale_t) 0)
                తిరిగి;

        freelocale(*p);
}

శూన్య locale_variables_free(అక్షరం* l[_VARIABLE_LC_MAX]);
స్థిర అంతఃవరుస శూన్య locale_variables_freep(అక్షరం*(*l)[_VARIABLE_LC_MAX]) {
        locale_variables_free(*l);
}
శూన్య locale_variables_simplify(అక్షరం *l[_VARIABLE_LC_MAX]);
