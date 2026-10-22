/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa LabelOps {
        පූර්ණ_සංඛ්‍යා (*pre)(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
        පූර්ණ_සංඛ්‍යා (*post)(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, bool created, LabelContext *label_context);
} LabelOps;

පූර්ණ_සංඛ්‍යා label_ops_set(const LabelOps *label_ops);
void label_ops_reset(void);

පූර්ණ_සංඛ්‍යා label_ops_pre(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode, LabelContext *label_context);
පූර්ණ_සංඛ්‍යා label_ops_post(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, bool created, LabelContext *label_context);
