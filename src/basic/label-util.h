/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa LabelOps {
        จำนวนเต็ม (*pre)(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
        จำนวนเต็ม (*post)(จำนวนเต็ม dir_fd, const var_na *path, bool created, LabelContext *label_context);
} LabelOps;

จำนวนเต็ม label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

จำนวนเต็ม label_ops_pre(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
จำนวนเต็ม label_ops_post(จำนวนเต็ม dir_fd, const var_na *path, bool created, LabelContext *label_context);
