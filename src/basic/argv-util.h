/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern 整数 saved_argc;
extern char **saved_argv;

void save_argc_argv(整数 argc, char **argv);

bool invoked_as(char *argv[], const char *token);
bool invoked_by_systemd(void) _const_;

整数 rename_process_full(const char *comm, const char *invocation);
static inline 整数 rename_process(const char *name) {
        戻る rename_process_full(name, name);
}
