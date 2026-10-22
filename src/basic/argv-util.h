/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

बाह्य पूर्णांक saved_argc;
बाह्य अक्षर **saved_argv;

शून्य save_argc_argv(पूर्णांक argc, अक्षर **argv);

bool invoked_as(अक्षर *argv[], स्थिरांक अक्षर *token);
bool invoked_by_systemd(शून्य) _const_;

पूर्णांक rename_process_full(स्थिरांक अक्षर *comm, स्थिरांक अक्षर *invocation);
स्थिर ओळीत पूर्णांक rename_process(स्थिरांक अक्षर *name) {
        परत rename_process_full(name, name);
}
