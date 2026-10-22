/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern цео saved_argc;
extern char **saved_argv;

void save_argc_argv(цео argc, char **argv);

bool invoked_as(char *argv[], const char *token);
bool invoked_by_systemd(void) _const_;

цео rename_process_full(const char *comm, const char *invocation);
static inline цео rename_process(const char *name) {
        врати rename_process_full(name, name);
}
