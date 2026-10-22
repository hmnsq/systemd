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

ចំនួនគត់ memfd_create_wrapper(const var_na *name, unsigned mode) {
        unsigned mode_compat;
        ចំនួនគត់ mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ប្រសិនបើ (mfd != -EINVAL)
                ត្រឡប់ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ប្រសិនបើ (mode == mode_compat)
                ត្រឡប់ mfd;

        ត្រឡប់ RET_NERRNO(memfd_create(name, mode_compat));
}

ចំនួនគត់ memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        ចំនួនគត់ r;

        ប្រសិនបើ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (isempty(comm))
                        name = "sd";
                បើមិនដូច្នេះ {
                        g = strjoin("sd-", comm);
                        ប្រសិនបើ (!g)
                                ត្រឡប់ -ENOMEM;

                        name = g;
                }
        }

        ត្រឡប់ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static ចំនួនគត់ memfd_add_seals(ចំនួនគត់ fd, unsigned seals) {
        assert(fd >= 0);

        ត្រឡប់ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static ចំនួនគត់ memfd_get_seals(ចំនួនគត់ fd, unsigned *ret_seals) {
        ចំនួនគត់ r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (ret_seals)
                *ret_seals = r;
        ត្រឡប់ 0;
}

ចំនួនគត់ memfd_set_sealed(ចំនួនគត់ fd) {
        ត្រឡប់ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ចំនួនគត់ memfd_get_sealed(ចំនួនគត់ fd) {
        unsigned seals;
        ចំនួនគត់ r;

        r = memfd_get_seals(fd, &seals);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ត្រឡប់ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ចំនួនគត់ memfd_set_sealed_gracefully(ចំនួនគត់ fd) {
        ចំនួនគត់ r;

        r = memfd_set_sealed(fd);
        ប្រសិនបើ (r != -EPERM)
                ត្រឡប់ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (r == 0)
                ត្រឡប់ -EPERM; /* Propagate the original error. */
        ត្រឡប់ 0;
}

ចំនួនគត់ memfd_get_size(ចំនួនគត់ fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        ប្រសិនបើ (fstat(fd, &stat) < 0)
                ត្រឡប់ -errno;

        *ret = stat.st_size;
        ត្រឡប់ 0;
}

ចំនួនគត់ memfd_set_size(ចំនួនគត់ fd, uint64_t sz) {
        assert(fd >= 0);

        ត្រឡប់ RET_NERRNO(ftruncate(fd, sz));
}

ចំនួនគត់ memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        ចំនួនគត់ r;

        assert(data || sz == 0);

        ប្រសិនបើ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd;

        ប្រសិនបើ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        r = memfd_set_sealed(fd);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ TAKE_FD(fd);
}
