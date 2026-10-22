/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ সংরচনা LabelOps {
        পূর্ণাঙ্ক (*pre)(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, LabelContext *label_context);
        পূর্ণাঙ্ক (*post)(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, bool created, LabelContext *label_context);
} LabelOps;

পূর্ণাঙ্ক label_ops_set(ধ্রুৱ LabelOps *label_ops);
শূন্য label_ops_reset(শূন্য);

পূর্ণাঙ্ক label_ops_pre(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, LabelContext *label_context);
পূর্ণাঙ্ক label_ops_post(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, bool created, LabelContext *label_context);
