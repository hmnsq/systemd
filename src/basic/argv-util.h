/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

बाह्य पूर्णाङ्क saved_argc;
बाह्य वर्ण **saved_argv;

शून्य save_argc_argv(पूर्णाङ्क argc, वर्ण **argv);

bool invoked_as(वर्ण *argv[], ध्रुव वर्ण *token);
bool invoked_by_systemd(शून्य) _const_;

पूर्णाङ्क rename_process_full(ध्रुव वर्ण *comm, ध्रुव वर्ण *invocation);
स्थिर अन्तःस्थ पूर्णाङ्क rename_process(ध्रुव वर्ण *name) {
        पटिनिवत्त rename_process_full(name, name);
}
