/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ saved_argc;
ಬಾಹ್ಯ ಅಕ್ಷರ **saved_argv;

ಶೂನ್ಯ save_argc_argv(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv);

bool invoked_as(ಅಕ್ಷರ *argv[], ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token);
bool invoked_by_systemd(ಶೂನ್ಯ) _const_;

ಪೂರ್ಣಾಂಕ rename_process_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *comm, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *invocation);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ rename_process(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಹಿಂತಿರುಗಿ rename_process_ತುಂಬಿದ(name, name);
}
