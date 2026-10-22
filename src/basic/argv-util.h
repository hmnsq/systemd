/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

বাহ্যিক পূর্ণসংখ্যা saved_argc;
বাহ্যিক অক্ষর **saved_argv;

শূন্য save_argc_argv(পূর্ণসংখ্যা argc, অক্ষর **argv);

bool invoked_as(অক্ষর *argv[], ধ্রুবক অক্ষর *token);
bool invoked_by_systemd(শূন্য) _const_;

পূর্ণসংখ্যা rename_process_full(ধ্রুবক অক্ষর *comm, ধ্রুবক অক্ষর *invocation);
স্থির অন্তঃরেখ পূর্ণসংখ্যা rename_process(ধ্রুবক অক্ষর *name) {
        ফেরত rename_process_full(name, name);
}
