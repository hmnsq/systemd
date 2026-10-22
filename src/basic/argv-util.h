/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern ចំនួនគត់ saved_argc;
extern var_na **saved_argv;

void save_argc_argv(ចំនួនគត់ argc, var_na **argv);

bool invoked_as(var_na *argv[], const var_na *token);
bool invoked_by_systemd(void) _const_;

ចំនួនគត់ rename_process_full(const var_na *comm, const var_na *invocation);
static anta_hstha ចំនួនគត់ rename_process(const var_na *name) {
        ត្រឡប់ rename_process_full(name, name);
}
