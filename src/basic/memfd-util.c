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

पूर्णांक memfd_create_wrapper(स्थिरांक अक्षर *name, अचिन्हित mode) {
        अचिन्हित mode_compat;
        पूर्णांक mfd;

        प्रतिज्ञा(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        जर (mfd != -EINVAL)
                परत mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        जर (mode == mode_compat)
                परत mfd;

        परत RET_NERRNO(memfd_create(name, mode_compat));
}

पूर्णांक memfd_new_full(स्थिरांक अक्षर *name, अचिन्हित extra_flags) {
        _cleanup_free_ अक्षर *g = रिक्त;
        पूर्णांक r;

        जर (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ अक्षर *comm = रिक्त;
                r = pid_get_comm(/* pid= */ 0, &comm);
                जर (r < 0)
                        परत r;

                जर (isempty(comm))
                        name = "sd";
                अन्यथा {
                        g = strjoin("sd-", comm);
                        जर (!g)
                                परत -ENOMEM;

                        name = g;
                }
        }

        परत memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

स्थिर पूर्णांक memfd_add_seals(पूर्णांक fd, अचिन्हित seals) {
        प्रतिज्ञा(fd >= 0);

        परत RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

स्थिर पूर्णांक memfd_get_seals(पूर्णांक fd, अचिन्हित *ret_seals) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        जर (r < 0)
                परत r;

        जर (ret_seals)
                *ret_seals = r;
        परत 0;
}

पूर्णांक memfd_set_sealed(पूर्णांक fd) {
        परत memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णांक memfd_get_sealed(पूर्णांक fd) {
        अचिन्हित seals;
        पूर्णांक r;

        r = memfd_get_seals(fd, &seals);
        जर (r < 0)
                परत r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        परत FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णांक memfd_set_sealed_gracefully(पूर्णांक fd) {
        पूर्णांक r;

        r = memfd_set_sealed(fd);
        जर (r != -EPERM)
                परत r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EPERM; /* Propagate the original error. */
        परत 0;
}

पूर्णांक memfd_get_size(पूर्णांक fd, uint64_t *ret) {
        संरचना stat stat;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जर (ख_स्थिति(fd, &stat) < 0)
                परत -त्रुटि_सं;

        *ret = stat.st_size;
        परत 0;
}

पूर्णांक memfd_set_size(पूर्णांक fd, uint64_t sz) {
        प्रतिज्ञा(fd >= 0);

        परत RET_NERRNO(ftruncate(fd, sz));
}

पूर्णांक memfd_new_and_seal(स्थिरांक अक्षर *name, स्थिरांक शून्य *data, माप_प्रकार sz) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(data || sz == 0);

        जर (sz == SIZE_MAX)
                sz = म_माप(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        जर (fd < 0)
                परत fd;

        जर (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                जर (r < 0)
                        परत r;
        }

        r = memfd_set_sealed(fd);
        जर (r < 0)
                परत r;

        परत TAKE_FD(fd);
}
