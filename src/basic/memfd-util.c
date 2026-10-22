/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "memfd-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం memfd_create_wrapper(స్థిరరాశి అక్షరం *name, గుర్తులేని బహుళకం) {
        గుర్తులేని బహుళకం_compat;
        పూర్ణాంకం mfd;

        ప్రతిజ్ఞా(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, బహుళకం));
        ఒకవేళ (mfd != -EINVAL)
                తిరిగి mfd;

        బహుళకం_compat = బహుళకం & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ఒకవేళ (బహుళకం == బహుళకం_compat)
                తిరిగి mfd;

        తిరిగి RET_NERRNO(memfd_create(name, బహుళకం_compat));
}

పూర్ణాంకం memfd_new_నిండిన(స్థిరరాశి అక్షరం *name, గుర్తులేని extra_flags) {
        _cleanup_free_ అక్షరం *g = ఖాళీ;
        పూర్ణాంకం r;

        ఒకవేళ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ అక్షరం *comm = ఖాళీ;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (isempty(comm))
                        name = "sd";
                లేకపోతే {
                        g = strjoin("sd-", comm);
                        ఒకవేళ (!g)
                                తిరిగి -ENOMEM;

                        name = g;
                }
        }

        తిరిగి memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

స్థిర పూర్ణాంకం memfd_కూడు_seals(పూర్ణాంకం fd, గుర్తులేని seals) {
        ప్రతిజ్ఞా(fd >= 0);

        తిరిగి RET_NERRNO(fcntl(fd, F_కూడు_SEALS, seals));
}

స్థిర పూర్ణాంకం memfd_get_seals(పూర్ణాంకం fd, గుర్తులేని *ret_seals) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_seals)
                *ret_seals = r;
        తిరిగి 0;
}

పూర్ణాంకం memfd_సమితి_sealed(పూర్ణాంకం fd) {
        తిరిగి memfd_కూడు_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

పూర్ణాంకం memfd_get_sealed(పూర్ణాంకం fd) {
        గుర్తులేని seals;
        పూర్ణాంకం r;

        r = memfd_get_seals(fd, &seals);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        తిరిగి FLAGS_సమితి(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

పూర్ణాంకం memfd_సమితి_sealed_gracefully(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        r = memfd_సమితి_sealed(fd);
        ఒకవేళ (r != -EPERM)
                తిరిగి r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EPERM; /* Propagate the original error. */
        తిరిగి 0;
}

పూర్ణాంకం memfd_get_కొలత(పూర్ణాంకం fd, uint64_t *ret) {
        నిర్మాణం stat stat;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (fstat(fd, &stat) < 0)
                తిరిగి -errno;

        *ret = stat.st_size;
        తిరిగి 0;
}

పూర్ణాంకం memfd_సమితి_కొలత(పూర్ణాంకం fd, uint64_t sz) {
        ప్రతిజ్ఞా(fd >= 0);

        తిరిగి RET_NERRNO(ftruncate(fd, sz));
}

పూర్ణాంకం memfd_new_and_seal(స్థిరరాశి అక్షరం *name, స్థిరరాశి శూన్య *దత్తాంశం, size_t sz) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(దత్తాంశం || sz == 0);

        ఒకవేళ (sz == SIZE_MAX)
                sz = అక్షరశ్రేణి_పొడవు(దత్తాంశం);

        fd = memfd_new_నిండిన(name, MFD_ALLOW_SEALING);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        ఒకవేళ (sz > 0) {
                r = pwrite_నిండిన(fd, దత్తాంశం, sz, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        r = memfd_సమితి_sealed(fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి TAKE_FD(fd);
}
