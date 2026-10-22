/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <glob.h>       /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

రకనిర్వచనం DIR* (*opendir_t)(స్థిరరాశి అక్షరం *);

పూర్ణాంకం సురక్షిత_glob_నిండిన(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, opendir_t opendir_func, అక్షరం ***ret);
స్థిర అంతఃవరుస పూర్ణాంకం సురక్షిత_glob(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, అక్షరం ***ret) {
        తిరిగి సురక్షిత_glob_నిండిన(మార్గం, flags, ఖాళీ, ret);
}

/* Note: which match is returned depends on the implementation/system and not guaranteed to be stable */
పూర్ణాంకం glob_మొదటి(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret);
#నిర్వచించు glob_exists(మార్గం) glob_మొదటి(మార్గం, ఖాళీ)
పూర్ణాంకం glob_extend(అక్షరం ***strv, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags);

పూర్ణాంకం glob_non_glob_ఉపసర్గ(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret);

bool string_is_glob(స్థిరరాశి అక్షరం *p) శుద్ధ;
