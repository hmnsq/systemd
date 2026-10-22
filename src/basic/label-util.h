/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന LabelOps {
        പൂർണ്ണസംഖ്യ (*pre)(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, LabelContext *label_context);
        പൂർണ്ണസംഖ്യ (*post)(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, bool created, LabelContext *label_context);
} LabelOps;

പൂർണ്ണസംഖ്യ label_ops_set(സ്ഥിരം LabelOps *label_ops);
ശൂന്യം label_ops_reset(ശൂന്യം);

പൂർണ്ണസംഖ്യ label_ops_pre(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, LabelContext *label_context);
പൂർണ്ണസംഖ്യ label_ops_post(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, bool created, LabelContext *label_context);
