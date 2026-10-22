/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "stat-util.h"

#నిర్వచించు FILESYSTEM_MAGIC_MAX 10

రకనిర్వచనం గణన FilesystemGroups {
        /* Please leave BASIC_API first and KNOWN last, but sort the rest alphabetically */
        FILESYSTEM_సమితి_BASIC_API,
        FILESYSTEM_సమితి_ANONYMOUS,
        FILESYSTEM_సమితి_APPLICATION,
        FILESYSTEM_సమితి_AUXILIARY_API,
        FILESYSTEM_సమితి_COMMON_ఖండం,
        FILESYSTEM_సమితి_HISTORICAL_ఖండం,
        FILESYSTEM_సమితి_జాలం,
        FILESYSTEM_సమితి_PRIVILEGED_API,
        FILESYSTEM_సమితి_SECURITY,
        FILESYSTEM_సమితి_TEMPORARY,
        FILESYSTEM_సమితి_తెలిసిన,
        FILESYSTEM_సమితి_MAX,
        FILESYSTEM_సమితి_INVALID = -EINVAL,
} FilesystemGroups;

రకనిర్వచనం నిర్మాణం Filesystem_సమితి {
        స్థిరరాశి అక్షరం *name;
        స్థిరరాశి అక్షరం *help;
        స్థిరరాశి అక్షరం *విలువ;
} Filesystem_సమితి;

బాహ్య స్థిరరాశి Filesystem_సమితి filesystem_sets[];

స్థిరరాశి Filesystem_సమితి *filesystem_సమితి_కనుగొను(స్థిరరాశి అక్షరం *name);

స్థిరరాశి అక్షరం* fs_type_గమ్యం_string(statfs_f_type_t magic);
పూర్ణాంకం fs_type_from_string(స్థిరరాశి అక్షరం *name, స్థిరరాశి statfs_f_type_t **ret);
bool fs_లోపల_సమూహం(స్థిరరాశి నిర్మాణం statfs *s, గణన FilesystemGroups fs_సమూహం);

/* gperf prototypes */
స్థిరరాశి నిర్మాణం FilesystemMagic* filesystems_gperf_lookup(స్థిరరాశి అక్షరం *str, GPERF_LEN_TYPE length);
