/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern ຈຳນວນເຕັມ saved_argc;
extern var_na **saved_argv;

void save_argc_argv(ຈຳນວນເຕັມ argc, var_na **argv);

bool invoked_as(var_na *argv[], const var_na *token);
bool invoked_by_systemd(void) _const_;

ຈຳນວນເຕັມ rename_process_full(const var_na *comm, const var_na *invocation);
static anta_hstha ຈຳນວນເຕັມ rename_process(const var_na *name) {
        ສົ່ງຄືນ rename_process_full(name, name);
}
