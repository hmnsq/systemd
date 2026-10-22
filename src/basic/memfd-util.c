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

số_nguyên memfd_create_wrapper(const char *name, unsigned mode) {
        unsigned mode_compat;
        số_nguyên mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        nếu (mfd != -EINVAL)
                trả_về mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        nếu (mode == mode_compat)
                trả_về mfd;

        trả_về RET_NERRNO(memfd_create(name, mode_compat));
}

số_nguyên memfd_new_full(const char *name, unsigned extra_flags) {
        _cleanup_free_ char *g = NULL;
        số_nguyên r;

        nếu (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ char *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                nếu (r < 0)
                        trả_về r;

                nếu (isempty(comm))
                        name = "sd";
                không_thì {
                        g = strjoin("sd-", comm);
                        nếu (!g)
                                trả_về -ENOMEM;

                        name = g;
                }
        }

        trả_về memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static số_nguyên memfd_add_seals(số_nguyên fd, unsigned seals) {
        assert(fd >= 0);

        trả_về RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static số_nguyên memfd_get_seals(số_nguyên fd, unsigned *ret_seals) {
        số_nguyên r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        nếu (r < 0)
                trả_về r;

        nếu (ret_seals)
                *ret_seals = r;
        trả_về 0;
}

số_nguyên memfd_set_sealed(số_nguyên fd) {
        trả_về memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

số_nguyên memfd_get_sealed(số_nguyên fd) {
        unsigned seals;
        số_nguyên r;

        r = memfd_get_seals(fd, &seals);
        nếu (r < 0)
                trả_về r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        trả_về FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

số_nguyên memfd_set_sealed_gracefully(số_nguyên fd) {
        số_nguyên r;

        r = memfd_set_sealed(fd);
        nếu (r != -EPERM)
                trả_về r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EPERM; /* Propagate the original error. */
        trả_về 0;
}

số_nguyên memfd_get_size(số_nguyên fd, uint64_t *ret) {
        struct stat stat;

        assert(fd >= 0);
        assert(ret);

        nếu (fstat(fd, &stat) < 0)
                trả_về -errno;

        *ret = stat.st_size;
        trả_về 0;
}

số_nguyên memfd_set_size(số_nguyên fd, uint64_t sz) {
        assert(fd >= 0);

        trả_về RET_NERRNO(ftruncate(fd, sz));
}

số_nguyên memfd_new_and_seal(const char *name, const void *data, size_t sz) {
        _cleanup_close_ số_nguyên fd = -EBADF;
        số_nguyên r;

        assert(data || sz == 0);

        nếu (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        nếu (fd < 0)
                trả_về fd;

        nếu (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                nếu (r < 0)
                        trả_về r;
        }

        r = memfd_set_sealed(fd);
        nếu (r < 0)
                trả_về r;

        trả_về TAKE_FD(fd);
}
