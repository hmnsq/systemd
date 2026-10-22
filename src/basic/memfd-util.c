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
        གལ་སྲིད (mfd != -EINVAL)
                ལོག mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        གལ་སྲིད (mode == mode_compat)
                ལོག mfd;

        ལོག RET_NERRNO(memfd_create(name, mode_compat));
}

ཧྲིལ memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        ཧྲིལ r;

        གལ་སྲིད (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (isempty(comm))
                        name = "sd";
                དེ་མིན {
                        g = strjoin("sd-", comm);
                        གལ་སྲིད (!g)
                                ལོག -ENOMEM;

                        name = g;
                }
        }

        ལོག memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static ཧྲིལ memfd_add_seals(ཧྲིལ fd, unsigned seals) {
        assert(fd >= 0);

        ལོག RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static ཧྲིལ memfd_get_seals(ཧྲིལ fd, unsigned *ret_seals) {
        ཧྲིལ r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (ret_seals)
                *ret_seals = r;
        ལོག 0;
}

ཧྲིལ memfd_set_sealed(ཧྲིལ fd) {
        ལོག memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ཧྲིལ memfd_get_sealed(ཧྲིལ fd) {
        unsigned seals;
        ཧྲིལ r;

        r = memfd_get_seals(fd, &seals);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ལོག FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ཧྲིལ memfd_set_sealed_gracefully(ཧྲིལ fd) {
        ཧྲིལ r;

        r = memfd_set_sealed(fd);
        གལ་སྲིད (r != -EPERM)
                ལོག r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (r == 0)
                ལོག -EPERM; /* Propagate the original error. */
        ལོག 0;
}

ཧྲིལ memfd_get_size(ཧྲིལ fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        གལ་སྲིད (fstat(fd, &stat) < 0)
                ལོག -errno;

        *ret = stat.st_size;
        ལོག 0;
}

ཧྲིལ memfd_set_size(ཧྲིལ fd, uint64_t sz) {
        assert(fd >= 0);

        ལོག RET_NERRNO(ftruncate(fd, sz));
}

ཧྲིལ memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ r;

        assert(data || sz == 0);

        གལ་སྲིད (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        གལ་སྲིད (fd < 0)
                ལོག fd;

        གལ་སྲིད (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        r = memfd_set_sealed(fd);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག TAKE_FD(fd);
}
