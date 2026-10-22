/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

বাহ্য পূর্ণাঙ্ক saved_argc;
বাহ্য ৱর্ণ **saved_argv;

শূন্য save_argc_argv(পূর্ণাঙ্ক argc, ৱর্ণ **argv);

bool invoked_as(ৱর্ণ *argv[], ধ্রুৱ ৱর্ণ *token);
bool invoked_by_systemd(শূন্য) _const_;

পূর্ণাঙ্ক rename_process_full(ধ্রুৱ ৱর্ণ *comm, ধ্রুৱ ৱর্ণ *invocation);
স্থির অন্তঃস্থ পূর্ণাঙ্ক rename_process(ধ্রুৱ ৱর্ণ *name) {
        ঘূৰাই rename_process_full(name, name);
}
