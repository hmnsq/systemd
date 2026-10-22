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

ຈຳນວນເຕັມ memfd_create_wrapper(const var_na *name, unsigned mode) {
        unsigned mode_compat;
        ຈຳນວນເຕັມ mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ຖ້າ (mfd != -EINVAL)
                ສົ່ງຄືນ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ຖ້າ (mode == mode_compat)
                ສົ່ງຄືນ mfd;

        ສົ່ງຄືນ RET_NERRNO(memfd_create(name, mode_compat));
}

ຈຳນວນເຕັມ memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        ຈຳນວນເຕັມ r;

        ຖ້າ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (isempty(comm))
                        name = "sd";
                ບໍ່ດັ່ງນັ້ນ {
                        g = strjoin("sd-", comm);
                        ຖ້າ (!g)
                                ສົ່ງຄືນ -ENOMEM;

                        name = g;
                }
        }

        ສົ່ງຄືນ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static ຈຳນວນເຕັມ memfd_add_seals(ຈຳນວນເຕັມ fd, unsigned seals) {
        assert(fd >= 0);

        ສົ່ງຄືນ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static ຈຳນວນເຕັມ memfd_get_seals(ຈຳນວນເຕັມ fd, unsigned *ret_seals) {
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (ret_seals)
                *ret_seals = r;
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ memfd_set_sealed(ຈຳນວນເຕັມ fd) {
        ສົ່ງຄືນ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ຈຳນວນເຕັມ memfd_get_sealed(ຈຳນວນເຕັມ fd) {
        unsigned seals;
        ຈຳນວນເຕັມ r;

        r = memfd_get_seals(fd, &seals);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ສົ່ງຄືນ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ຈຳນວນເຕັມ memfd_set_sealed_gracefully(ຈຳນວນເຕັມ fd) {
        ຈຳນວນເຕັມ r;

        r = memfd_set_sealed(fd);
        ຖ້າ (r != -EPERM)
                ສົ່ງຄືນ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0)
                ສົ່ງຄືນ -EPERM; /* Propagate the original error. */
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ memfd_get_size(ຈຳນວນເຕັມ fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        ຖ້າ (fstat(fd, &stat) < 0)
                ສົ່ງຄືນ -errno;

        *ret = stat.st_size;
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ memfd_set_size(ຈຳນວນເຕັມ fd, uint64_t sz) {
        assert(fd >= 0);

        ສົ່ງຄືນ RET_NERRNO(ftruncate(fd, sz));
}

ຈຳນວນເຕັມ memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF;
        ຈຳນວນເຕັມ r;

        assert(data || sz == 0);

        ຖ້າ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ fd;

        ຖ້າ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        r = memfd_set_sealed(fd);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ TAKE_FD(fd);
}
