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

पूर्णाङ्क memfd_create_wrapper(ध्रुव वर्ण *name, unsigned mode) {
        unsigned mode_compat;
        पूर्णाङ्क mfd;

        प्रतिज्ञा(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        सचे (mfd != -EINVAL)
                पटिनिवत्त mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        सचे (mode == mode_compat)
                पटिनिवत्त mfd;

        पटिनिवत्त RET_NERRNO(memfd_create(name, mode_compat));
}

पूर्णाङ्क memfd_new_full(ध्रुव वर्ण *name, unsigned extra_flags) {
        _cleanup_free_ वर्ण *g = रिक्त;
        पूर्णाङ्क r;

        सचे (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ वर्ण *comm = रिक्त;
                r = pid_get_comm(/* pid= */ 0, &comm);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (isempty(comm))
                        name = "sd";
                अञ्ञथा {
                        g = strjoin("sd-", comm);
                        सचे (!g)
                                पटिनिवत्त -ENOMEM;

                        name = g;
                }
        }

        पटिनिवत्त memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

स्थिर पूर्णाङ्क memfd_add_seals(पूर्णाङ्क fd, unsigned seals) {
        प्रतिज्ञा(fd >= 0);

        पटिनिवत्त RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

स्थिर पूर्णाङ्क memfd_get_seals(पूर्णाङ्क fd, unsigned *ret_seals) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret_seals)
                *ret_seals = r;
        पटिनिवत्त 0;
}

पूर्णाङ्क memfd_set_sealed(पूर्णाङ्क fd) {
        पटिनिवत्त memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णाङ्क memfd_get_sealed(पूर्णाङ्क fd) {
        unsigned seals;
        पूर्णाङ्क r;

        r = memfd_get_seals(fd, &seals);
        सचे (r < 0)
                पटिनिवत्त r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        पटिनिवत्त FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णाङ्क memfd_set_sealed_gracefully(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        r = memfd_set_sealed(fd);
        सचे (r != -EPERM)
                पटिनिवत्त r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0)
                पटिनिवत्त -EPERM; /* Propagate the original error. */
        पटिनिवत्त 0;
}

पूर्णाङ्क memfd_get_size(पूर्णाङ्क fd, uint64_t *ret) {
        संरचना stat stat;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        सचे (fstat(fd, &stat) < 0)
                पटिनिवत्त -errno;

        *ret = stat.st_size;
        पटिनिवत्त 0;
}

पूर्णाङ्क memfd_set_size(पूर्णाङ्क fd, uint64_t sz) {
        प्रतिज्ञा(fd >= 0);

        पटिनिवत्त RET_NERRNO(ftruncate(fd, sz));
}

पूर्णाङ्क memfd_new_and_seal(ध्रुव वर्ण *name, ध्रुव शून्य *data, size_t sz) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(data || sz == 0);

        सचे (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        सचे (fd < 0)
                पटिनिवत्त fd;

        सचे (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        r = memfd_set_sealed(fd);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त TAKE_FD(fd);
}
