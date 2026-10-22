/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା LabelOps {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*pre)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, LabelContext *label_context);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*post)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, bool created, LabelContext *label_context);
} LabelOps;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_ops_set(ଅଚଳ LabelOps *label_ops);
ଶୂନ୍ୟ label_ops_reset(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_ops_pre(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, LabelContext *label_context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା label_ops_post(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, bool created, LabelContext *label_context);
