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

ཧྲིལ memfd_create_wrapper(const var_na *name, unsigned mode) {
        unsigned mode_compat;
        ཧྲིལ mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        གལ་ཏེ (mfd != -EINVAL)
                ཕྱིར mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        གལ་ཏེ (mode == mode_compat)
                ཕྱིར mfd;

        ཕྱིར RET_NERRNO(memfd_create(name, mode_compat));
}

ཧྲིལ memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        ཧྲིལ r;

        གལ་ཏེ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                གལ་ཏེ (isempty(comm))
                        name = "sd";
                དེ་མིན {
                        g = strjoin("sd-", comm);
                        གལ་ཏེ (!g)
                                ཕྱིར -ENOMEM;

                        name = g;
                }
        }

        ཕྱིར memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static ཧྲིལ memfd_add_seals(ཧྲིལ fd, unsigned seals) {
        assert(fd >= 0);

        ཕྱིར RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static ཧྲིལ memfd_get_seals(ཧྲིལ fd, unsigned *ret_seals) {
        ཧྲིལ r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (ret_seals)
                *ret_seals = r;
        ཕྱིར 0;
}

ཧྲིལ memfd_set_sealed(ཧྲིལ fd) {
        ཕྱིར memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ཧྲིལ memfd_get_sealed(ཧྲིལ fd) {
        unsigned seals;
        ཧྲིལ r;

        r = memfd_get_seals(fd, &seals);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ཕྱིར FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ཧྲིལ memfd_set_sealed_gracefully(ཧྲིལ fd) {
        ཧྲིལ r;

        r = memfd_set_sealed(fd);
        གལ་ཏེ (r != -EPERM)
                ཕྱིར r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (r == 0)
                ཕྱིར -EPERM; /* Propagate the original error. */
        ཕྱིར 0;
}

ཧྲིལ memfd_get_size(ཧྲིལ fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        གལ་ཏེ (fstat(fd, &stat) < 0)
                ཕྱིར -errno;

        *ret = stat.st_size;
        ཕྱིར 0;
}

ཧྲིལ memfd_set_size(ཧྲིལ fd, uint64_t sz) {
        assert(fd >= 0);

        ཕྱིར RET_NERRNO(ftruncate(fd, sz));
}

ཧྲིལ memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ r;

        assert(data || sz == 0);

        གལ་ཏེ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        གལ་ཏེ (fd < 0)
                ཕྱིར fd;

        གལ་ཏེ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        r = memfd_set_sealed(fd);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར TAKE_FD(fd);
}
