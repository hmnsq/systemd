/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

બાહ્ય પૂર્ણાંક saved_argc;
બાહ્ય અક્ષર **saved_argv;

શૂન્ય save_argc_argv(પૂર્ણાંક argc, અક્ષર **argv);

bool invoked_as(અક્ષર *argv[], અચલ અક્ષર *token);
bool invoked_by_systemd(શૂન્ય) _const_;

પૂર્ણાંક rename_process_full(અચલ અક્ષર *comm, અચલ અક્ષર *invocation);
સ્થિર અંતઃરેખ પૂર્ણાંક rename_process(અચલ અક્ષર *name) {
        પાછા rename_process_full(name, name);
}
