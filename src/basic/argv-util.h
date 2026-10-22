/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern số_nguyên saved_argc;
extern char **saved_argv;

void save_argc_argv(số_nguyên argc, char **argv);

bool invoked_as(char *argv[], const char *token);
bool invoked_by_systemd(void) _const_;

số_nguyên rename_process_full(const char *comm, const char *invocation);
static inline số_nguyên rename_process(const char *name) {
        trả_về rename_process_full(name, name);
}
