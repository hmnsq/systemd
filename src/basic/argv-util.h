/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

ബാഹ്യ പൂർണ്ണസംഖ്യ saved_argc;
ബാഹ്യ അക്ഷരം **saved_argv;

ശൂന്യം save_argc_argv(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv);

bool invoked_as(അക്ഷരം *argv[], സ്ഥിരം അക്ഷരം *token);
bool invoked_by_systemd(ശൂന്യം) _const_;

പൂർണ്ണസംഖ്യ rename_process_full(സ്ഥിരം അക്ഷരം *comm, സ്ഥിരം അക്ഷരം *invocation);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ rename_process(സ്ഥിരം അക്ഷരം *name) {
        തിരികെ rename_process_full(name, name);
}
