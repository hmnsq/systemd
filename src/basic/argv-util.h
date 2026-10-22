/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

బాహ్య పూర్ణాంకం saved_argc;
బాహ్య అక్షరం **saved_argv;

శూన్య save_argc_argv(పూర్ణాంకం argc, అక్షరం **argv);

bool invoked_as(అక్షరం *argv[], స్థిరరాశి అక్షరం *token);
bool invoked_by_systemd(శూన్య) _const_;

పూర్ణాంకం rename_process_నిండిన(స్థిరరాశి అక్షరం *comm, స్థిరరాశి అక్షరం *invocation);
స్థిర అంతఃవరుస పూర్ణాంకం rename_process(స్థిరరాశి అక్షరం *name) {
        తిరిగి rename_process_నిండిన(name, name);
}
