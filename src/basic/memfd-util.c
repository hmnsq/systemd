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

පූර්ණ_සංඛ්‍යා memfd_create_wrapper(const var_na *name, unsigned mode) {
        unsigned mode_compat;
        පූර්ණ_සංඛ්‍යා mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        නම් (mfd != -EINVAL)
                ආපසු mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        නම් (mode == mode_compat)
                ආපසු mfd;

        ආපසු RET_NERRNO(memfd_create(name, mode_compat));
}

පූර්ණ_සංඛ්‍යා memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        පූර්ණ_සංඛ්‍යා r;

        නම් (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                නම් (r < 0)
                        ආපසු r;

                නම් (isempty(comm))
                        name = "sd";
                නැතහොත් {
                        g = strjoin("sd-", comm);
                        නම් (!g)
                                ආපසු -ENOMEM;

                        name = g;
                }
        }

        ආපසු memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static පූර්ණ_සංඛ්‍යා memfd_add_seals(පූර්ණ_සංඛ්‍යා fd, unsigned seals) {
        assert(fd >= 0);

        ආපසු RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static පූර්ණ_සංඛ්‍යා memfd_get_seals(පූර්ණ_සංඛ්‍යා fd, unsigned *ret_seals) {
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        නම් (r < 0)
                ආපසු r;

        නම් (ret_seals)
                *ret_seals = r;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා memfd_set_sealed(පූර්ණ_සංඛ්‍යා fd) {
        ආපසු memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

පූර්ණ_සංඛ්‍යා memfd_get_sealed(පූර්ණ_සංඛ්‍යා fd) {
        unsigned seals;
        පූර්ණ_සංඛ්‍යා r;

        r = memfd_get_seals(fd, &seals);
        නම් (r < 0)
                ආපසු r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ආපසු FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

පූර්ණ_සංඛ්‍යා memfd_set_sealed_gracefully(පූර්ණ_සංඛ්‍යා fd) {
        පූර්ණ_සංඛ්‍යා r;

        r = memfd_set_sealed(fd);
        නම් (r != -EPERM)
                ආපසු r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -EPERM; /* Propagate the original error. */
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා memfd_get_size(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        නම් (fstat(fd, &stat) < 0)
                ආපසු -errno;

        *ret = stat.st_size;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා memfd_set_size(පූර්ණ_සංඛ්‍යා fd, uint64_t sz) {
        assert(fd >= 0);

        ආපසු RET_NERRNO(ftruncate(fd, sz));
}

පූර්ණ_සංඛ්‍යා memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        පූර්ණ_සංඛ්‍යා r;

        assert(data || sz == 0);

        නම් (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        නම් (fd < 0)
                ආපසු fd;

        නම් (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                නම් (r < 0)
                        ආපසු r;
        }

        r = memfd_set_sealed(fd);
        නම් (r < 0)
                ආපසු r;

        ආපසු TAKE_FD(fd);
}
