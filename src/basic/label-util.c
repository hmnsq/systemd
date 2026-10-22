/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "label-util.h"

స్థిర స్థిరరాశి LabelOps *label_ops = ఖాళీ;

పూర్ణాంకం label_ops_సమితి(స్థిరరాశి LabelOps *ops) {
        ప్రతిజ్ఞా(ops);

        ఒకవేళ (label_ops)
                తిరిగి -EBUSY;

        label_ops = ops;
        తిరిగి 0;
}

శూన్య label_ops_reset(శూన్య) {
        label_ops = ఖాళీ;
}

పూర్ణాంకం label_ops_pre(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, LabelContext *label_context) {
        ఒకవేళ (!label_ops || !label_ops->pre)
                తిరిగి 0;

        తిరిగి label_ops->pre(dir_fd, మార్గం, బహుళకం, label_context);
}

పూర్ణాంకం label_ops_post(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, bool created, LabelContext *label_context) {
        ఒకవేళ (!label_ops || !label_ops->post)
                తిరిగి 0;

        తిరిగి label_ops->post(dir_fd, మార్గం, created, label_context);
}
