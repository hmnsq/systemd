/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern 정수 saved_argc;
extern char **saved_argv;

void save_argc_argv(정수 argc, char **argv);

bool invoked_as(char *argv[], const char *token);
bool invoked_by_systemd(void) _const_;

정수 rename_process_full(const char *comm, const char *invocation);
static inline 정수 rename_process(const char *name) {
        반환 rename_process_full(name, name);
}
