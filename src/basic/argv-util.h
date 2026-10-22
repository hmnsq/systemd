/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

बाह्य पूर्णांक saved_argc;
बाह्य अक्षर **saved_argv;

व्योम save_argc_argv(पूर्णांक argc, अक्षर **argv);

bool invoked_as(अक्षर *argv[], स्थिर अक्षर *token);
bool invoked_by_systemd(व्योम) _const_;

पूर्णांक rename_process_full(स्थिर अक्षर *comm, स्थिर अक्षर *invocation);
अटल अंतरभूत पूर्णांक rename_process(स्थिर अक्षर *name) {
        वापस rename_process_full(name, name);
}
