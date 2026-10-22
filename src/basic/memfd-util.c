/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "io-util.h"
#समावेश "memfd-util.h"
#समावेश "process-util.h"
#समावेश "string-util.h"

पूर्णांक memfd_create_wrapper(स्थिर अक्षर *name, अचिन्हित mode) {
        अचिन्हित mode_compat;
        पूर्णांक mfd;

        निश्चित(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        अगर (mfd != -EINVAL)
                वापस mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        अगर (mode == mode_compat)
                वापस mfd;

        वापस RET_NERRNO(memfd_create(name, mode_compat));
}

पूर्णांक memfd_new_full(स्थिर अक्षर *name, अचिन्हित extra_flags) {
        _cleanup_free_ अक्षर *g = शून्य;
        पूर्णांक r;

        अगर (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ अक्षर *comm = शून्य;
                r = pid_get_comm(/* pid= */ 0, &comm);
                अगर (r < 0)
                        वापस r;

                अगर (isempty(comm))
                        name = "sd";
                अन्यथा {
                        g = strjoin("sd-", comm);
                        अगर (!g)
                                वापस -ENOMEM;

                        name = g;
                }
        }

        वापस memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

अटल पूर्णांक memfd_add_seals(पूर्णांक fd, अचिन्हित seals) {
        निश्चित(fd >= 0);

        वापस RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

अटल पूर्णांक memfd_get_seals(पूर्णांक fd, अचिन्हित *ret_seals) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        अगर (r < 0)
                वापस r;

        अगर (ret_seals)
                *ret_seals = r;
        वापस 0;
}

पूर्णांक memfd_set_sealed(पूर्णांक fd) {
        वापस memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णांक memfd_get_sealed(पूर्णांक fd) {
        अचिन्हित seals;
        पूर्णांक r;

        r = memfd_get_seals(fd, &seals);
        अगर (r < 0)
                वापस r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        वापस FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

पूर्णांक memfd_set_sealed_gracefully(पूर्णांक fd) {
        पूर्णांक r;

        r = memfd_set_sealed(fd);
        अगर (r != -EPERM)
                वापस r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EPERM; /* Propagate the original error. */
        वापस 0;
}

पूर्णांक memfd_get_आकार(पूर्णांक fd, uint64_t *ret) {
        काष्ठा stat stat;

        निश्चित(fd >= 0);
        निश्चित(ret);

        अगर (ख_स्थिति(fd, &stat) < 0)
                वापस -त्रुटि_सं;

        *ret = stat.st_size;
        वापस 0;
}

पूर्णांक memfd_set_आकार(पूर्णांक fd, uint64_t sz) {
        निश्चित(fd >= 0);

        वापस RET_NERRNO(ftruncate(fd, sz));
}

पूर्णांक memfd_new_and_seal(स्थिर अक्षर *name, स्थिर व्योम *तथ्य, माप_प्रकार sz) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(तथ्य || sz == 0);

        अगर (sz == SIZE_MAX)
                sz = म_माप(तथ्य);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        अगर (fd < 0)
                वापस fd;

        अगर (sz > 0) {
                r = pwrite_full(fd, तथ्य, sz, 0);
                अगर (r < 0)
                        वापस r;
        }

        r = memfd_set_sealed(fd);
        अगर (r < 0)
                वापस r;

        वापस TAKE_FD(fd);
}
