/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_argc;
ବାହ୍ୟ ଅକ୍ଷର **saved_argv;

ଶୂନ୍ୟ save_argc_argv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv);

bool invoked_as(ଅକ୍ଷର *argv[], ଅଚଳ ଅକ୍ଷର *token);
bool invoked_by_systemd(ଶୂନ୍ୟ) _const_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rename_process_full(ଅଚଳ ଅକ୍ଷର *comm, ଅଚଳ ଅକ୍ଷର *invocation);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rename_process(ଅଚଳ ଅକ୍ଷର *name) {
        ଫେରନ୍ତୁ rename_process_full(name, name);
}
