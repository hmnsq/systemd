/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "label-util.h"

நிலையான மாறிலி LabelOps *label_ops = வெற்று;

முழு label_ops_set(மாறிலி LabelOps *ops) {
        ப்ரதிஜ்ஞா(ops);

        எனில் (label_ops)
                திரும்பு -EBUSY;

        label_ops = ops;
        திரும்பு 0;
}

வெறுமை label_ops_reset(வெறுமை) {
        label_ops = வெற்று;
}

முழு label_ops_pre(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, LabelContext *label_context) {
        எனில் (!label_ops || !label_ops->pre)
                திரும்பு 0;

        திரும்பு label_ops->pre(dir_fd, path, mode, label_context);
}

முழு label_ops_post(முழு dir_fd, மாறிலி எழுத்து *path, bool created, LabelContext *label_context) {
        எனில் (!label_ops || !label_ops->post)
                திரும்பு 0;

        திரும்பு label_ops->post(dir_fd, path, created, label_context);
}
