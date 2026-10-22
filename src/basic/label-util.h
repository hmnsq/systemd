/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం LabelOps {
        పూర్ణాంకం (*pre)(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, LabelContext *label_context);
        పూర్ణాంకం (*post)(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, bool created, LabelContext *label_context);
} LabelOps;

పూర్ణాంకం label_ops_సమితి(స్థిరరాశి LabelOps *label_ops);
శూన్య label_ops_reset(శూన్య);

పూర్ణాంకం label_ops_pre(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, LabelContext *label_context);
పూర్ణాంకం label_ops_post(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, bool created, LabelContext *label_context);
