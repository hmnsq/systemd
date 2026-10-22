/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "capsule-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "user-util.h"

పూర్ణాంకం capsule_name_is_valid(స్థిరరాశి అక్షరం *name) {

        ఒకవేళ (!filename_is_valid(name))
                తిరిగి false;

        _cleanup_free_ అక్షరం *prefixed = strjoin("c-", name);
        ఒకవేళ (!prefixed)
                తిరిగి -ENOMEM;

        తిరిగి valid_user_సమూహం_name(prefixed, /* flags= */ 0);
}
