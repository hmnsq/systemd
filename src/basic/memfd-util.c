/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "io-util.h"
#include "memfd-util.h"
#include "process-util.h"
#include "string-util.h"

цео memfd_create_wrapper(const char *name, unsigned mode) {
        unsigned mode_compat;
        цео mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ако (mfd != -EINVAL)
                врати mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ако (mode == mode_compat)
                врати mfd;

        врати RET_NERRNO(memfd_create(name, mode_compat));
}

цео memfd_new_full(const char *name, unsigned extra_flags) {
        _cleanup_free_ char *g = NULL;
        цео r;

        ако (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ char *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ако (r < 0)
                        врати r;

                ако (isempty(comm))
                        name = "sd";
                инако {
                        g = strjoin("sd-", comm);
                        ако (!g)
                                врати -ENOMEM;

                        name = g;
                }
        }

        врати memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static цео memfd_add_seals(цео fd, unsigned seals) {
        assert(fd >= 0);

        врати RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static цео memfd_get_seals(цео fd, unsigned *ret_seals) {
        цео r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ако (r < 0)
                врати r;

        ако (ret_seals)
                *ret_seals = r;
        врати 0;
}

цео memfd_set_sealed(цео fd) {
        врати memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

цео memfd_get_sealed(цео fd) {
        unsigned seals;
        цео r;

        r = memfd_get_seals(fd, &seals);
        ако (r < 0)
                врати r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        врати FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

цео memfd_set_sealed_gracefully(цео fd) {
        цео r;

        r = memfd_set_sealed(fd);
        ако (r != -EPERM)
                врати r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ако (r < 0)
                врати r;
        ако (r == 0)
                врати -EPERM; /* Propagate the original error. */
        врати 0;
}

цео memfd_get_size(цео fd, uint64_t *ret) {
        struct stat stat;

        assert(fd >= 0);
        assert(ret);

        ако (fstat(fd, &stat) < 0)
                врати -errno;

        *ret = stat.st_size;
        врати 0;
}

цео memfd_set_size(цео fd, uint64_t sz) {
        assert(fd >= 0);

        врати RET_NERRNO(ftruncate(fd, sz));
}

цео memfd_new_and_seal(const char *name, const void *data, size_t sz) {
        _cleanup_close_ цео fd = -EBADF;
        цео r;

        assert(data || sz == 0);

        ако (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ако (fd < 0)
                врати fd;

        ако (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ако (r < 0)
                        врати r;
        }

        r = memfd_set_sealed(fd);
        ако (r < 0)
                врати r;

        врати TAKE_FD(fd);
}
