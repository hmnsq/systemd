/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਬਾਹਰੀ ਪੂਰਨ saved_argc;
ਬਾਹਰੀ ਅੱਖਰ **saved_argv;

ਖਾਲੀ save_argc_argv(ਪੂਰਨ argc, ਅੱਖਰ **argv);

bool invoked_as(ਅੱਖਰ *argv[], ਅਚਲ ਅੱਖਰ *token);
bool invoked_by_systemd(ਖਾਲੀ) _const_;

ਪੂਰਨ rename_process_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *comm, ਅਚਲ ਅੱਖਰ *invocation);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ rename_process(ਅਚਲ ਅੱਖਰ *name) {
        ਵਾਪਸ rename_process_ਭਰਿਆ(name, name);
}
