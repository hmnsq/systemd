/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ কাঠামো LabelOps {
        পূর্ণসংখ্যা (*pre)(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, LabelContext *label_context);
        পূর্ণসংখ্যা (*post)(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, bool created, LabelContext *label_context);
} LabelOps;

পূর্ণসংখ্যা label_ops_set(ধ্রুবক LabelOps *label_ops);
শূন্য label_ops_reset(শূন্য);

পূর্ণসংখ্যা label_ops_pre(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, LabelContext *label_context);
পূর্ণসংখ্যা label_ops_post(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, bool created, LabelContext *label_context);
