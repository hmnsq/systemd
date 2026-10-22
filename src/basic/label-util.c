/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "label-util.h"

ਸਥਿਰ ਅਚਲ LabelOps *label_ops = ਨਿਰਾ;

ਪੂਰਨ label_ops_ਸੈੱਟ(ਅਚਲ LabelOps *ops) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ops);

        ਜੇ (label_ops)
                ਵਾਪਸ -EBUSY;

        label_ops = ops;
        ਵਾਪਸ 0;
}

ਖਾਲੀ label_ops_reset(ਖਾਲੀ) {
        label_ops = ਨਿਰਾ;
}

ਪੂਰਨ label_ops_pre(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, LabelContext *label_context) {
        ਜੇ (!label_ops || !label_ops->pre)
                ਵਾਪਸ 0;

        ਵਾਪਸ label_ops->pre(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, label_context);
}

ਪੂਰਨ label_ops_post(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool created, LabelContext *label_context) {
        ਜੇ (!label_ops || !label_ops->post)
                ਵਾਪਸ 0;

        ਵਾਪਸ label_ops->post(dir_fd, ਰਸਤਾ, created, label_context);
}
