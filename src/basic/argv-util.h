/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

بیرونی صحیح saved_argc;
بیرونی نویسه **saved_argv;

تهی save_argc_argv(صحیح argc, نویسه **argv);

bool invoked_as(نویسه *argv[], ثابت نویسه *token);
bool invoked_by_systemd(تهی) _const_;

صحیح rename_process_full(ثابت نویسه *comm, ثابت نویسه *invocation);
ایستا درونخطی صحیح rename_process(ثابت نویسه *name) {
        بازگشت rename_process_full(name, name);
}
