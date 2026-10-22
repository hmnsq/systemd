/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ LabelOps {
        ਪੂਰਨ (*pre)(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, LabelContext *label_context);
        ਪੂਰਨ (*post)(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool created, LabelContext *label_context);
} LabelOps;

ਪੂਰਨ label_ops_ਸੈੱਟ(ਅਚਲ LabelOps *label_ops);
ਖਾਲੀ label_ops_reset(ਖਾਲੀ);

ਪੂਰਨ label_ops_pre(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, LabelContext *label_context);
ਪੂਰਨ label_ops_post(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool created, LabelContext *label_context);
