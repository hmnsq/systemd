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

ကိန်းပြည့် memfd_create_wrapper(const var_na *name, unsigned mode) {
        unsigned mode_compat;
        ကိန်းပြည့် mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        အကယ်၍ (mfd != -EINVAL)
                ပြန်ပေး mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        အကယ်၍ (mode == mode_compat)
                ပြန်ပေး mfd;

        ပြန်ပေး RET_NERRNO(memfd_create(name, mode_compat));
}

ကိန်းပြည့် memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        ကိန်းပြည့် r;

        အကယ်၍ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (isempty(comm))
                        name = "sd";
                မဟုတ်ပါက {
                        g = strjoin("sd-", comm);
                        အကယ်၍ (!g)
                                ပြန်ပေး -ENOMEM;

                        name = g;
                }
        }

        ပြန်ပေး memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static ကိန်းပြည့် memfd_add_seals(ကိန်းပြည့် fd, unsigned seals) {
        assert(fd >= 0);

        ပြန်ပေး RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static ကိန်းပြည့် memfd_get_seals(ကိန်းပြည့် fd, unsigned *ret_seals) {
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (ret_seals)
                *ret_seals = r;
        ပြန်ပေး 0;
}

ကိန်းပြည့် memfd_set_sealed(ကိန်းပြည့် fd) {
        ပြန်ပေး memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ကိန်းပြည့် memfd_get_sealed(ကိန်းပြည့် fd) {
        unsigned seals;
        ကိန်းပြည့် r;

        r = memfd_get_seals(fd, &seals);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ပြန်ပေး FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ကိန်းပြည့် memfd_set_sealed_gracefully(ကိန်းပြည့် fd) {
        ကိန်းပြည့် r;

        r = memfd_set_sealed(fd);
        အကယ်၍ (r != -EPERM)
                ပြန်ပေး r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0)
                ပြန်ပေး -EPERM; /* Propagate the original error. */
        ပြန်ပေး 0;
}

ကိန်းပြည့် memfd_get_size(ကိန်းပြည့် fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        အကယ်၍ (fstat(fd, &stat) < 0)
                ပြန်ပေး -errno;

        *ret = stat.st_size;
        ပြန်ပေး 0;
}

ကိန်းပြည့် memfd_set_size(ကိန်းပြည့် fd, uint64_t sz) {
        assert(fd >= 0);

        ပြန်ပေး RET_NERRNO(ftruncate(fd, sz));
}

ကိန်းပြည့် memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;
        ကိန်းပြည့် r;

        assert(data || sz == 0);

        အကယ်၍ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        အကယ်၍ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        r = memfd_set_sealed(fd);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး TAKE_FD(fd);
}
