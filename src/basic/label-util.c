/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "label-util.h"

സ്ഥിതം സ്ഥിരം LabelOps *label_ops = ശൂന്യ;

പൂർണ്ണസംഖ്യ label_ops_set(സ്ഥിരം LabelOps *ops) {
        പ്രതിജ്ഞാ(ops);

        എങ്കിൽ (label_ops)
                തിരികെ -EBUSY;

        label_ops = ops;
        തിരികെ 0;
}

ശൂന്യം label_ops_reset(ശൂന്യം) {
        label_ops = ശൂന്യ;
}

പൂർണ്ണസംഖ്യ label_ops_pre(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, LabelContext *label_context) {
        എങ്കിൽ (!label_ops || !label_ops->pre)
                തിരികെ 0;

        തിരികെ label_ops->pre(dir_fd, path, mode, label_context);
}

പൂർണ്ണസംഖ്യ label_ops_post(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, bool created, LabelContext *label_context) {
        എങ്കിൽ (!label_ops || !label_ops->post)
                തിരികെ 0;

        തിരികെ label_ops->post(dir_fd, path, created, label_context);
}
