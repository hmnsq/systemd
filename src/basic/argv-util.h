/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வெளி முழு saved_argc;
வெளி எழுத்து **saved_argv;

வெறுமை save_argc_argv(முழு argc, எழுத்து **argv);

bool invoked_as(எழுத்து *argv[], மாறிலி எழுத்து *token);
bool invoked_by_systemd(வெறுமை) _const_;

முழு rename_process_full(மாறிலி எழுத்து *comm, மாறிலி எழுத்து *invocation);
நிலையான உள்வரி முழு rename_process(மாறிலி எழுத்து *name) {
        திரும்பு rename_process_full(name, name);
}
