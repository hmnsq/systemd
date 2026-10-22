/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa LabelOps {
        ចំនួនគត់ (*pre)(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
        ចំនួនគត់ (*post)(ចំនួនគត់ dir_fd, const var_na *path, bool created, LabelContext *label_context);
} LabelOps;

ចំនួនគត់ label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

ចំនួនគត់ label_ops_pre(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
ចំនួនគត់ label_ops_post(ចំនួនគត់ dir_fd, const var_na *path, bool created, LabelContext *label_context);
