/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

extern puur_naa_nka saved_argc;
extern var_na **saved_argv;

void save_argc_argv(puur_naa_nka argc, var_na **argv);

bool invoked_as(var_na *argv[], const var_na *token);
bool invoked_by_systemd(void) _const_;

puur_naa_nka rename_process_full(const var_na *comm, const var_na *invocation);
static anta_hstha puur_naa_nka rename_process(const var_na *name) {
        ꦮꦭꦶ rename_process_full(name, name);
}
