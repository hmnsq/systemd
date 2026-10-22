/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "memfd-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क memfd_create_wrapper(अचर वर्ण *name, अचिन्हित mode) {
        अचिन्हित mode_compat;
        पूर्णाङ्क mfd;

        प्रतिज्ञा(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        यदि (mfd != -EINVAL)
                फर्काऊ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        यदि (mode == mode_compat)
                फर्काऊ mfd;

        फर्काऊ RET_NERRNO(memfd_create(name, mode_compat));
}

पूर्णाङ्क memfd_new_full(अचर वर्ण *name, अचिन्हित extra_flags) {
        _cleanup_free_ वर्ण *g = रिक्त;
        पूर्णाङ्क r;

        यदि (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ वर्ण *comm = रिक्त;
                r = pid_get_comm(/* pid= */ 0, &comm);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (isempty(comm))
                        name = "sd";
                अन्यथा {
                        g = strjoin("sd-", comm);
                        यदि (!g)
                                फर्काऊ -ENOMEM;

                        name = g;
                }
        }

        फर्काऊ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

स्थिर पूर्णाङ्क memfd_add_seals(पूर्णाङ्क fd, अचिन्हित seals) {
        प्रतिज्ञा(fd >= 0);

        फर्काऊ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

स्थिर पूर्णाङ्क memfd_get_seals(पूर्णाङ्क fd, अचिन्हित *ret_seals) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        यदि (r < 0)
                फर्काऊ r;

        यदि (ret_seals)
                *ret_seals = r;
        फर्काऊ 0;
}

पूर्णाङ्क memfd_set_sealed(पूर्णाङ्क fd) {
        फर्काऊ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णाङ्क memfd_get_sealed(पूर्णाङ्क fd) {
        अचिन्हित seals;
        पूर्णाङ्क r;

        r = memfd_get_seals(fd, &seals);
        यदि (r < 0)
                फर्काऊ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        फर्काऊ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णाङ्क memfd_set_sealed_gracefully(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        r = memfd_set_sealed(fd);
        यदि (r != -EPERM)
                फर्काऊ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        यदि (r < 0)
                फर्काऊ r;
        यदि (r == 0)
                फर्काऊ -EPERM; /* Propagate the original error. */
        फर्काऊ 0;
}

पूर्णाङ्क memfd_get_size(पूर्णाङ्क fd, uint64_t *ret) {
        संरचना stat stat;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        यदि (ख_स्थिति(fd, &stat) < 0)
                फर्काऊ -त्रुटि_सं;

        *ret = stat.st_size;
        फर्काऊ 0;
}

पूर्णाङ्क memfd_set_size(पूर्णाङ्क fd, uint64_t sz) {
        प्रतिज्ञा(fd >= 0);

        फर्काऊ RET_NERRNO(ftruncate(fd, sz));
}

पूर्णाङ्क memfd_new_and_seal(अचर वर्ण *name, अचर शून्य *data, माप_प्रकार sz) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(data || sz == 0);

        यदि (sz == SIZE_MAX)
                sz = म_माप(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        यदि (fd < 0)
                फर्काऊ fd;

        यदि (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                यदि (r < 0)
                        फर्काऊ r;
        }

        r = memfd_set_sealed(fd);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ TAKE_FD(fd);
}
