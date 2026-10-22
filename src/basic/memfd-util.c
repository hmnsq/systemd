/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "memfd-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"

முழு memfd_create_wrapper(மாறிலி எழுத்து *name, unsigned mode) {
        unsigned mode_compat;
        முழு mfd;

        ப்ரதிஜ்ஞா(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        எனில் (mfd != -EINVAL)
                திரும்பு mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        எனில் (mode == mode_compat)
                திரும்பு mfd;

        திரும்பு RET_NERRNO(memfd_create(name, mode_compat));
}

முழு memfd_new_full(மாறிலி எழுத்து *name, unsigned extra_flags) {
        _cleanup_free_ எழுத்து *g = வெற்று;
        முழு r;

        எனில் (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ எழுத்து *comm = வெற்று;
                r = pid_get_comm(/* pid= */ 0, &comm);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (isempty(comm))
                        name = "sd";
                இல்லையெனில் {
                        g = strjoin("sd-", comm);
                        எனில் (!g)
                                திரும்பு -ENOMEM;

                        name = g;
                }
        }

        திரும்பு memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

நிலையான முழு memfd_add_seals(முழு fd, unsigned seals) {
        ப்ரதிஜ்ஞா(fd >= 0);

        திரும்பு RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

நிலையான முழு memfd_get_seals(முழு fd, unsigned *ret_seals) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_seals)
                *ret_seals = r;
        திரும்பு 0;
}

முழு memfd_set_sealed(முழு fd) {
        திரும்பு memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

முழு memfd_get_sealed(முழு fd) {
        unsigned seals;
        முழு r;

        r = memfd_get_seals(fd, &seals);
        எனில் (r < 0)
                திரும்பு r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        திரும்பு FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

முழு memfd_set_sealed_gracefully(முழு fd) {
        முழு r;

        r = memfd_set_sealed(fd);
        எனில் (r != -EPERM)
                திரும்பு r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EPERM; /* Propagate the original error. */
        திரும்பு 0;
}

முழு memfd_get_size(முழு fd, uint64_t *ret) {
        கட்டமைப்பு stat stat;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (fstat(fd, &stat) < 0)
                திரும்பு -errno;

        *ret = stat.st_size;
        திரும்பு 0;
}

முழு memfd_set_size(முழு fd, uint64_t sz) {
        ப்ரதிஜ்ஞா(fd >= 0);

        திரும்பு RET_NERRNO(ftruncate(fd, sz));
}

முழு memfd_new_and_seal(மாறிலி எழுத்து *name, மாறிலி வெறுமை *data, size_t sz) {
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(data || sz == 0);

        எனில் (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        எனில் (fd < 0)
                திரும்பு fd;

        எனில் (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                எனில் (r < 0)
                        திரும்பு r;
        }

        r = memfd_set_sealed(fd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு TAKE_FD(fd);
}
