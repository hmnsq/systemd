/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

বাহ্য পূর্ণাঙ্ক saved_argc;
বাহ্য বর্ণ **saved_argv;

শূন্য save_argc_argv(পূর্ণাঙ্ক argc, বর্ণ **argv);

bool invoked_as(বর্ণ *argv[], ধ্রুব বর্ণ *token);
bool invoked_by_systemd(শূন্য) _const_;

পূর্ণাঙ্ক rename_process_full(ধ্রুব বর্ণ *comm, ধ্রুব বর্ণ *invocation);
স্থির অন্তঃস্থ পূর্ণাঙ্ক rename_process(ধ্রুব বর্ণ *name) {
        প্রত্যাগম rename_process_full(name, name);
}
