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

puur_naa_nka memfd_create_wrapper(const var_na *name, unsigned mode) {
        unsigned mode_compat;
        puur_naa_nka mfd;

        assert(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ᨀᨘ (mfd != -EINVAL)
                ᨀᨘᨒ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ᨀᨘ (mode == mode_compat)
                ᨀᨘᨒ mfd;

        ᨀᨘᨒ RET_NERRNO(memfd_create(name, mode_compat));
}

puur_naa_nka memfd_new_full(const var_na *name, unsigned extra_flags) {
        _cleanup_free_ var_na *g = NULL;
        puur_naa_nka r;

        ᨀᨘ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ var_na *comm = NULL;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                ᨀᨘ (isempty(comm))
                        name = "sd";
                ᨀᨘᨁ {
                        g = strjoin("sd-", comm);
                        ᨀᨘ (!g)
                                ᨀᨘᨒ -ENOMEM;

                        name = g;
                }
        }

        ᨀᨘᨒ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

static puur_naa_nka memfd_add_seals(puur_naa_nka fd, unsigned seals) {
        assert(fd >= 0);

        ᨀᨘᨒ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

static puur_naa_nka memfd_get_seals(puur_naa_nka fd, unsigned *ret_seals) {
        puur_naa_nka r;

        assert(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘ (ret_seals)
                *ret_seals = r;
        ᨀᨘᨒ 0;
}

puur_naa_nka memfd_set_sealed(puur_naa_nka fd) {
        ᨀᨘᨒ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

puur_naa_nka memfd_get_sealed(puur_naa_nka fd) {
        unsigned seals;
        puur_naa_nka r;

        r = memfd_get_seals(fd, &seals);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ᨀᨘᨒ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

puur_naa_nka memfd_set_sealed_gracefully(puur_naa_nka fd) {
        puur_naa_nka r;

        r = memfd_set_sealed(fd);
        ᨀᨘ (r != -EPERM)
                ᨀᨘᨒ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (r == 0)
                ᨀᨘᨒ -EPERM; /* Propagate the original error. */
        ᨀᨘᨒ 0;
}

puur_naa_nka memfd_get_size(puur_naa_nka fd, uint64_t *ret) {
        sa_nracanaa stat stat;

        assert(fd >= 0);
        assert(ret);

        ᨀᨘ (fstat(fd, &stat) < 0)
                ᨀᨘᨒ -errno;

        *ret = stat.st_size;
        ᨀᨘᨒ 0;
}

puur_naa_nka memfd_set_size(puur_naa_nka fd, uint64_t sz) {
        assert(fd >= 0);

        ᨀᨘᨒ RET_NERRNO(ftruncate(fd, sz));
}

puur_naa_nka memfd_new_and_seal(const var_na *name, const void *data, size_t sz) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        assert(data || sz == 0);

        ᨀᨘ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ fd;

        ᨀᨘ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
        }

        r = memfd_set_sealed(fd);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ TAKE_FD(fd);
}
