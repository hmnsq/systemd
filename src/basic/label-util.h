/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના LabelOps {
        પૂર્ણાંક (*pre)(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, LabelContext *label_context);
        પૂર્ણાંક (*post)(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, bool created, LabelContext *label_context);
} LabelOps;

પૂર્ણાંક label_ops_set(અચલ LabelOps *label_ops);
શૂન્ય label_ops_reset(શૂન્ય);

પૂર્ણાંક label_ops_pre(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, LabelContext *label_context);
પૂર્ણાંક label_ops_post(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, bool created, LabelContext *label_context);
