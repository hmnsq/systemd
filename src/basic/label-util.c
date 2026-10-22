/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "label-util.h"

static const LabelOps *label_ops = NULL;

puur_naa_nka label_ops_set(const LabelOps *ops) {
        assert(ops);

        ᨀᨘ (label_ops)
                ᨀᨘᨒ -EBUSY;

        label_ops = ops;
        ᨀᨘᨒ 0;
}

void label_ops_reset(void) {
        label_ops = NULL;
}

puur_naa_nka label_ops_pre(puur_naa_nka dir_fd, const var_na *path, mode_t mode, LabelContext *label_context) {
        ᨀᨘ (!label_ops || !label_ops->pre)
                ᨀᨘᨒ 0;

        ᨀᨘᨒ label_ops->pre(dir_fd, path, mode, label_context);
}

puur_naa_nka label_ops_post(puur_naa_nka dir_fd, const var_na *path, bool created, LabelContext *label_context) {
        ᨀᨘ (!label_ops || !label_ops->post)
                ᨀᨘᨒ 0;

        ᨀᨘᨒ label_ops->post(dir_fd, path, created, label_context);
}
