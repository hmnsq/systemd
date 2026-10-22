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

ціле memfd_create_wrapper(const char *name, unsigned mode) {
        unsigned mode_compat;
        ціле mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        якщо (mfd != -EINVAL)
                повернути mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        якщо (mode == mode_compat)
                повернути mfd;

        повернути RET_NERRNO(memfd_create(name, mode_compat));
}

ціле memfd_new_full(const char *name, unsigned extra_flags) {
        _cleanup_free_ char *g = NULL;
        ціле r;

        якщо (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ char *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                якщо (r < 0)
                        повернути r;

                якщо (isempty(comm))
                        name = "sd";
                інакше {
                        g = strjoin("sd-", comm);
                        якщо (!g)
                                повернути -ENOMEM;

                        name = g;
                }
        }

        повернути memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static ціле memfd_add_seals(ціле fd, unsigned seals) {
        assert(fd >= 0);

        повернути RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static ціле memfd_get_seals(ціле fd, unsigned *ret_seals) {
        ціле r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        якщо (r < 0)
                повернути r;

        якщо (ret_seals)
                *ret_seals = r;
        повернути 0;
}

ціле memfd_set_sealed(ціле fd) {
        повернути memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ціле memfd_get_sealed(ціле fd) {
        unsigned seals;
        ціле r;

        r = memfd_get_seals(fd, &seals);
        якщо (r < 0)
                повернути r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        повернути FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ціле memfd_set_sealed_gracefully(ціле fd) {
        ціле r;

        r = memfd_set_sealed(fd);
        якщо (r != -EPERM)
                повернути r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EPERM; /* Propagate the original error. */
        повернути 0;
}

ціле memfd_get_size(ціле fd, uint64_t *ret) {
        struct stat stat;

        assert(fd >= 0);
        assert(ret);

        якщо (fstat(fd, &stat) < 0)
                повернути -errno;

        *ret = stat.st_size;
        повернути 0;
}

ціле memfd_set_size(ціле fd, uint64_t sz) {
        assert(fd >= 0);

        повернути RET_NERRNO(ftruncate(fd, sz));
}

ціле memfd_new_and_seal(const char *name, const void *data, size_t sz) {
        _cleanup_close_ ціле fd = -EBADF;
        ціле r;

        assert(data || sz == 0);

        якщо (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        якщо (fd < 0)
                повернути fd;

        якщо (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                якщо (r < 0)
                        повернути r;
        }

        r = memfd_set_sealed(fd);
        якщо (r < 0)
                повернути r;

        повернути TAKE_FD(fd);
}
