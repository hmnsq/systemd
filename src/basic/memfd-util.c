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

целое memfd_create_wrapper(const char *name, unsigned mode) {
        unsigned mode_compat;
        целое mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        если (mfd != -EINVAL)
                вернуть mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        если (mode == mode_compat)
                вернуть mfd;

        вернуть RET_NERRNO(memfd_create(name, mode_compat));
}

целое memfd_new_full(const char *name, unsigned extra_flags) {
        _cleanup_free_ char *g = NULL;
        целое r;

        если (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ char *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                если (r < 0)
                        вернуть r;

                если (isempty(comm))
                        name = "sd";
                иначе {
                        g = strjoin("sd-", comm);
                        если (!g)
                                вернуть -ENOMEM;

                        name = g;
                }
        }

        вернуть memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static целое memfd_add_seals(целое fd, unsigned seals) {
        assert(fd >= 0);

        вернуть RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static целое memfd_get_seals(целое fd, unsigned *ret_seals) {
        целое r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        если (r < 0)
                вернуть r;

        если (ret_seals)
                *ret_seals = r;
        вернуть 0;
}

целое memfd_set_sealed(целое fd) {
        вернуть memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

целое memfd_get_sealed(целое fd) {
        unsigned seals;
        целое r;

        r = memfd_get_seals(fd, &seals);
        если (r < 0)
                вернуть r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        вернуть FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

целое memfd_set_sealed_gracefully(целое fd) {
        целое r;

        r = memfd_set_sealed(fd);
        если (r != -EPERM)
                вернуть r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        если (r < 0)
                вернуть r;
        если (r == 0)
                вернуть -EPERM; /* Propagate the original error. */
        вернуть 0;
}

целое memfd_get_size(целое fd, uint64_t *ret) {
        struct stat stat;

        assert(fd >= 0);
        assert(ret);

        если (fstat(fd, &stat) < 0)
                вернуть -errno;

        *ret = stat.st_size;
        вернуть 0;
}

целое memfd_set_size(целое fd, uint64_t sz) {
        assert(fd >= 0);

        вернуть RET_NERRNO(ftruncate(fd, sz));
}

целое memfd_new_and_seal(const char *name, const void *data, size_t sz) {
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        assert(data || sz == 0);

        если (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        если (fd < 0)
                вернуть fd;

        если (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                если (r < 0)
                        вернуть r;
        }

        r = memfd_set_sealed(fd);
        если (r < 0)
                вернуть r;

        вернуть TAKE_FD(fd);
}
