/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa LabelOps {
        ຈຳນວນເຕັມ (*pre)(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
        ຈຳນວນເຕັມ (*post)(ຈຳນວນເຕັມ dir_fd, const var_na *path, bool created, LabelContext *label_context);
} LabelOps;

ຈຳນວນເຕັມ label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

ຈຳນວນເຕັມ label_ops_pre(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
ຈຳນວນເຕັມ label_ops_post(ຈຳນວນເຕັມ dir_fd, const var_na *path, bool created, LabelContext *label_context);
