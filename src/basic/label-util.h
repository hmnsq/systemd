/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa LabelOps {
        ကိန်းပြည့် (*pre)(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
        ကိန်းပြည့် (*post)(ကိန်းပြည့် dir_fd, const var_na *path, bool created, LabelContext *label_context);
} LabelOps;

ကိန်းပြည့် label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

ကိန်းပြည့် label_ops_pre(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
ကိန်းပြည့် label_ops_post(ကိန်းပြည့် dir_fd, const var_na *path, bool created, LabelContext *label_context);
