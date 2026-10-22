/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

خارجي صحيح saved_argc;
خارجي حرف **saved_argv;

فراغ save_argc_argv(صحيح argc, حرف **argv);

bool invoked_as(حرف *argv[], ثابت حرف *token);
bool invoked_by_systemd(فراغ) _const_;

صحيح rename_process_ډک(ثابت حرف *comm, ثابت حرف *invocation);
ساكن مضمن صحيح rename_process(ثابت حرف *name) {
        أرجع rename_process_ډک(name, name);
}
