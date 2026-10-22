/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct LabelOps {
        ціле (*pre)(ціле dir_fd, const char *path, mode_t mode, LabelContext *label_context);
        ціле (*post)(ціле dir_fd, const char *path, bool created, LabelContext *label_context);
} LabelOps;

ціле label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

ціле label_ops_pre(ціле dir_fd, const char *path, mode_t mode, LabelContext *label_context);
ціле label_ops_post(ціле dir_fd, const char *path, bool created, LabelContext *label_context);
