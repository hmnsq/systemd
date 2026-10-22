/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern ціле saved_argc;
extern char **saved_argv;

void save_argc_argv(ціле argc, char **argv);

bool invoked_as(char *argv[], const char *token);
bool invoked_by_systemd(void) _const_;

ціле rename_process_full(const char *comm, const char *invocation);
static inline ціле rename_process(const char *name) {
        повернути rename_process_full(name, name);
}
