/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern целое saved_argc;
extern char **saved_argv;

void save_argc_argv(целое argc, char **argv);

bool invoked_as(char *argv[], const char *token);
bool invoked_by_systemd(void) _const_;

целое rename_process_full(const char *comm, const char *invocation);
static inline целое rename_process(const char *name) {
        вернуть rename_process_full(name, name);
}
