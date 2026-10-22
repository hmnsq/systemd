/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை கட்டமைப்பு LabelOps {
        முழு (*pre)(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, LabelContext *label_context);
        முழு (*post)(முழு dir_fd, மாறிலி எழுத்து *path, bool created, LabelContext *label_context);
} LabelOps;

முழு label_ops_set(மாறிலி LabelOps *label_ops);
வெறுமை label_ops_reset(வெறுமை);

முழு label_ops_pre(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, LabelContext *label_context);
முழு label_ops_post(முழு dir_fd, மாறிலி எழுத்து *path, bool created, LabelContext *label_context);
