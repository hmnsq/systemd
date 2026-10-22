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

정수 memfd_create_wrapper(const char *name, unsigned mode) {
        unsigned mode_compat;
        정수 mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        만약 (mfd != -EINVAL)
                반환 mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        만약 (mode == mode_compat)
                반환 mfd;

        반환 RET_NERRNO(memfd_create(name, mode_compat));
}

정수 memfd_new_full(const char *name, unsigned extra_flags) {
        _cleanup_free_ char *g = NULL;
        정수 r;

        만약 (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ char *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                만약 (r < 0)
                        반환 r;

                만약 (isempty(comm))
                        name = "sd";
                아니면 {
                        g = strjoin("sd-", comm);
                        만약 (!g)
                                반환 -ENOMEM;

                        name = g;
                }
        }

        반환 memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static 정수 memfd_add_seals(정수 fd, unsigned seals) {
        assert(fd >= 0);

        반환 RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static 정수 memfd_get_seals(정수 fd, unsigned *ret_seals) {
        정수 r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        만약 (r < 0)
                반환 r;

        만약 (ret_seals)
                *ret_seals = r;
        반환 0;
}

정수 memfd_set_sealed(정수 fd) {
        반환 memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

정수 memfd_get_sealed(정수 fd) {
        unsigned seals;
        정수 r;

        r = memfd_get_seals(fd, &seals);
        만약 (r < 0)
                반환 r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        반환 FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

정수 memfd_set_sealed_gracefully(정수 fd) {
        정수 r;

        r = memfd_set_sealed(fd);
        만약 (r != -EPERM)
                반환 r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        만약 (r < 0)
                반환 r;
        만약 (r == 0)
                반환 -EPERM; /* Propagate the original error. */
        반환 0;
}

정수 memfd_get_size(정수 fd, uint64_t *ret) {
        struct stat stat;

        assert(fd >= 0);
        assert(ret);

        만약 (fstat(fd, &stat) < 0)
                반환 -errno;

        *ret = stat.st_size;
        반환 0;
}

정수 memfd_set_size(정수 fd, uint64_t sz) {
        assert(fd >= 0);

        반환 RET_NERRNO(ftruncate(fd, sz));
}

정수 memfd_new_and_seal(const char *name, const void *data, size_t sz) {
        _cleanup_close_ 정수 fd = -EBADF;
        정수 r;

        assert(data || sz == 0);

        만약 (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        만약 (fd < 0)
                반환 fd;

        만약 (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                만약 (r < 0)
                        반환 r;
        }

        r = memfd_set_sealed(fd);
        만약 (r < 0)
                반환 r;

        반환 TAKE_FD(fd);
}
