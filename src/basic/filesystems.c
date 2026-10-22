/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "filesystems-gperf.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం fs_type_from_string(స్థిరరాశి అక్షరం *name, స్థిరరాశి statfs_f_type_t **ret) {
        స్థిరరాశి నిర్మాణం FilesystemMagic *fs_magic;

        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(ret);

        fs_magic = filesystems_gperf_lookup(name, అక్షరశ్రేణి_పొడవు(name));
        ఒకవేళ (!fs_magic)
                తిరిగి -EINVAL;

        *ret = fs_magic->magic;
        తిరిగి 0;
}

స్థిరరాశి Filesystem_సమితి* filesystem_సమితి_కనుగొను(స్థిరరాశి అక్షరం *name) {
        ఒకవేళ (isempty(name) || name[0] != '@')
                తిరిగి ఖాళీ;

        కొరకు (FilesystemGroups i = 0; i < FILESYSTEM_సమితి_MAX; i++)
                ఒకవేళ (streq(filesystem_sets[i].name, name))
                        తిరిగి filesystem_sets + i;

        తిరిగి ఖాళీ;
}
