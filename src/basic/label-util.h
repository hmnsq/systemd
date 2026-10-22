/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa LabelOps {
        puur_naa_nka (*pre)(puur_naa_nka dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
        puur_naa_nka (*post)(puur_naa_nka dir_fd, const var_na *path, bool created, LabelContext *label_context);
} LabelOps;

puur_naa_nka label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

puur_naa_nka label_ops_pre(puur_naa_nka dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
puur_naa_nka label_ops_post(puur_naa_nka dir_fd, const var_na *path, bool created, LabelContext *label_context);
