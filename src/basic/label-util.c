/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "label-util.h"

સ્થિર અચલ LabelOps *label_ops = ખાલી;

પૂર્ણાંક label_ops_set(અચલ LabelOps *ops) {
        પ્રતિજ્ઞા(ops);

        જો (label_ops)
                પાછા -EBUSY;

        label_ops = ops;
        પાછા 0;
}

શૂન્ય label_ops_reset(શૂન્ય) {
        label_ops = ખાલી;
}

પૂર્ણાંક label_ops_pre(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, LabelContext *label_context) {
        જો (!label_ops || !label_ops->pre)
                પાછા 0;

        પાછા label_ops->pre(dir_fd, path, mode, label_context);
}

પૂર્ણાંક label_ops_post(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, bool created, LabelContext *label_context) {
        જો (!label_ops || !label_ops->post)
                પાછા 0;

        પાછા label_ops->post(dir_fd, path, created, label_context);
}
