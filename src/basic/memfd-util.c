/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "memfd-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક memfd_create_wrapper(અચલ અક્ષર *name, unsigned mode) {
        unsigned mode_compat;
        પૂર્ણાંક mfd;

        પ્રતિજ્ઞા(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        જો (mfd != -EINVAL)
                પાછા mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        જો (mode == mode_compat)
                પાછા mfd;

        પાછા RET_NERRNO(memfd_create(name, mode_compat));
}

પૂર્ણાંક memfd_new_full(અચલ અક્ષર *name, unsigned extra_flags) {
        _cleanup_free_ અક્ષર *g = ખાલી;
        પૂર્ણાંક r;

        જો (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ અક્ષર *comm = ખાલી;
                r = pid_get_comm(/* pid= */ 0, &comm);
                જો (r < 0)
                        પાછા r;

                જો (isempty(comm))
                        name = "sd";
                નહીંતર {
                        g = strjoin("sd-", comm);
                        જો (!g)
                                પાછા -ENOMEM;

                        name = g;
                }
        }

        પાછા memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

સ્થિર પૂર્ણાંક memfd_add_seals(પૂર્ણાંક fd, unsigned seals) {
        પ્રતિજ્ઞા(fd >= 0);

        પાછા RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

સ્થિર પૂર્ણાંક memfd_get_seals(પૂર્ણાંક fd, unsigned *ret_seals) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        જો (r < 0)
                પાછા r;

        જો (ret_seals)
                *ret_seals = r;
        પાછા 0;
}

પૂર્ણાંક memfd_set_sealed(પૂર્ણાંક fd) {
        પાછા memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

પૂર્ણાંક memfd_get_sealed(પૂર્ણાંક fd) {
        unsigned seals;
        પૂર્ણાંક r;

        r = memfd_get_seals(fd, &seals);
        જો (r < 0)
                પાછા r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        પાછા FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

પૂર્ણાંક memfd_set_sealed_gracefully(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        r = memfd_set_sealed(fd);
        જો (r != -EPERM)
                પાછા r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EPERM; /* Propagate the original error. */
        પાછા 0;
}

પૂર્ણાંક memfd_get_size(પૂર્ણાંક fd, uint64_t *ret) {
        સંરચના stat stat;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (fstat(fd, &stat) < 0)
                પાછા -errno;

        *ret = stat.st_size;
        પાછા 0;
}

પૂર્ણાંક memfd_set_size(પૂર્ણાંક fd, uint64_t sz) {
        પ્રતિજ્ઞા(fd >= 0);

        પાછા RET_NERRNO(ftruncate(fd, sz));
}

પૂર્ણાંક memfd_new_and_seal(અચલ અક્ષર *name, અચલ શૂન્ય *data, size_t sz) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(data || sz == 0);

        જો (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        જો (fd < 0)
                પાછા fd;

        જો (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                જો (r < 0)
                        પાછા r;
        }

        r = memfd_set_sealed(fd);
        જો (r < 0)
                પાછા r;

        પાછા TAKE_FD(fd);
}
