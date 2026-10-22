/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

خارجی صحیح saved_argc;
خارجی حرف **saved_argv;

خالی save_argc_argv(صحیح argc, حرف **argv);

bool invoked_as(حرف *argv[], ثابت حرف *token);
bool invoked_by_systemd(خالی) _const_;

صحیح rename_process_بھرا(ثابت حرف *comm, ثابت حرف *invocation);
ساکن اندرونی صحیح rename_process(ثابت حرف *name) {
        واپس rename_process_بھرا(name, name);
}
