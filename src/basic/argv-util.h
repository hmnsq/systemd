/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ saved_argc;
ಬಾಹ್ಯ ವರ್ಣ **saved_argv;

ಶೂನ್ಯ save_argc_argv(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv);

bool invoked_as(ವರ್ಣ *argv[], ಧ್ರುವ ವರ್ಣ *token);
bool invoked_by_systemd(ಶೂನ್ಯ) _const_;

ಪೂರ್ಣಾಙ್ಕ rename_process_full(ಧ್ರುವ ವರ್ಣ *comm, ಧ್ರುವ ವರ್ಣ *invocation);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ rename_process(ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತ್ಯಾಗಮ rename_process_full(name, name);
}
