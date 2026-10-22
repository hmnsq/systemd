/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memfd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ memfd_create_wrapper(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਹੁਲਕ) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਹੁਲਕ_compat;
        ਪੂਰਨ mfd;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, ਬਹੁਲਕ));
        ਜੇ (mfd != -EINVAL)
                ਵਾਪਸ mfd;

        ਬਹੁਲਕ_compat = ਬਹੁਲਕ & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ਜੇ (ਬਹੁਲਕ == ਬਹੁਲਕ_compat)
                ਵਾਪਸ mfd;

        ਵਾਪਸ RET_NERRNO(memfd_create(name, ਬਹੁਲਕ_compat));
}

ਪੂਰਨ memfd_new_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ extra_flags) {
        _cleanup_free_ ਅੱਖਰ *g = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ ਅੱਖਰ *comm = ਨਿਰਾ;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (isempty(comm))
                        name = "sd";
                ਨਹੀਂਤਾਂ {
                        g = strjoin("sd-", comm);
                        ਜੇ (!g)
                                ਵਾਪਸ -ENOMEM;

                        name = g;
                }
        }

        ਵਾਪਸ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ਸਥਿਰ ਪੂਰਨ memfd_ਜੋੜੋ_seals(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ seals) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਵਾਪਸ RET_NERRNO(fcntl(fd, F_ਜੋੜੋ_SEALS, seals));
}

ਸਥਿਰ ਪੂਰਨ memfd_get_seals(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_seals) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_seals)
                *ret_seals = r;
        ਵਾਪਸ 0;
}

ਪੂਰਨ memfd_ਸੈੱਟ_sealed(ਪੂਰਨ fd) {
        ਵਾਪਸ memfd_ਜੋੜੋ_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ਪੂਰਨ memfd_get_sealed(ਪੂਰਨ fd) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ seals;
        ਪੂਰਨ r;

        r = memfd_get_seals(fd, &seals);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ਵਾਪਸ FLAGS_ਸੈੱਟ(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ਪੂਰਨ memfd_ਸੈੱਟ_sealed_gracefully(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        r = memfd_ਸੈੱਟ_sealed(fd);
        ਜੇ (r != -EPERM)
                ਵਾਪਸ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EPERM; /* Propagate the original error. */
        ਵਾਪਸ 0;
}

ਪੂਰਨ memfd_get_ਮਾਪ(ਪੂਰਨ fd, uint64_t *ret) {
        ਬਣਤਰ stat stat;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (fstat(fd, &stat) < 0)
                ਵਾਪਸ -errno;

        *ret = stat.st_size;
        ਵਾਪਸ 0;
}

ਪੂਰਨ memfd_ਸੈੱਟ_ਮਾਪ(ਪੂਰਨ fd, uint64_t sz) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਵਾਪਸ RET_NERRNO(ftruncate(fd, sz));
}

ਪੂਰਨ memfd_new_and_seal(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t sz) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ || sz == 0);

        ਜੇ (sz == SIZE_MAX)
                sz = ਲੜੀ_ਲੰਬਾਈ(ਅੰਕੜੇ);

        fd = memfd_new_ਭਰਿਆ(name, MFD_ALLOW_SEALING);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਜੇ (sz > 0) {
                r = pwrite_ਭਰਿਆ(fd, ਅੰਕੜੇ, sz, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        r = memfd_ਸੈੱਟ_sealed(fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ TAKE_FD(fd);
}
