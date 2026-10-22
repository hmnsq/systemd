/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

صحيح encode_devnode_name(ثابت حرف *str, حرف *str_enc, size_t len);
صحيح allow_listed_char_for_devnode(حرف c, ثابت حرف *additional);

صحيح devnode_same(ثابت حرف *a, ثابت حرف *b);
