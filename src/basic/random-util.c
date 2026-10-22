/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <elf.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/random.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/auxv.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/random.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "pidfd-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "sha256.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
ಸ್ಥಿರ ಶೂನ್ಯ fallback_random_ಬೈಟುಗಳು(ಶೂನ್ಯ *p, size_t n) {
        ಸ್ಥಿರ thread_local uint64_t fallback_counter = 0;
        ರಚನೆ {
                ಅಕ್ಷರ label[32];
                uint64_t call_id, ಖಂಡ_id;
                usec_t stamp_mono, stamp_real;
                pid_t pid, tid;
                uint64_t pidfdid;
                uint8_t auxval[16];
        } state = {
                /* Arbitrary domain separation to prevent other usage of AT_RANDOM from clashing. */
                .call_id = fallback_counter++,
                .stamp_mono = now(CLOCK_MONOTONIC),
                .stamp_real = now(CLOCK_REALTIME),
                .pid = getpid_cached(),
                .tid = gettid(),
        };

        ಸ್ಮರಣೆ_ನಕಲು(state.label, "systemd fallback random bytes v1", ಗಾತ್ರ(state.label));
        ಸ್ಮರಣೆ_ನಕಲು(state.auxval, ULONG_ಗಮ್ಯ_PTR(getauxval(AT_RANDOM)), ಗಾತ್ರ(state.auxval));
        (ಶೂನ್ಯ) pidfd_get_inode_id_self_cached(&state.pidfdid);

        ಆಗ (n > 0) {
                ರಚನೆ sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_ಬೈಟುಗಳು(&state, ಗಾತ್ರ(state), &ctx);
                ಆದರೆ (n < SHA256_DIGEST_ಗಾತ್ರ) {
                        uint8_t partial[SHA256_DIGEST_ಗಾತ್ರ];
                        sha256_ಮುಕ್ತಾಯ_ctx(&ctx, partial);
                        ಸ್ಮರಣೆ_ನಕಲು(p, partial, n);
                        ನಿಲ್ಲಿಸು;
                }
                sha256_ಮುಕ್ತಾಯ_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_ಗಾತ್ರ;
                n -= SHA256_DIGEST_ಗಾತ್ರ;
                ++state.ಖಂಡ_id;
        }
}

ಶೂನ್ಯ random_ಬೈಟುಗಳು(ಶೂನ್ಯ *p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ;

        ಗಾಗಿ (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                ಆದರೆ (l <= 0)
                        ನಿಲ್ಲಿಸು; /* Unexpected error. Give up and fallback to /dev/urandom. */

                ಆದರೆ ((size_t) l == n)
                        ಹಿಂತಿರುಗಿ; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ಆದರೆ (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                ಹಿಂತಿರುಗಿ;

        /* This is a terrible fallback. Oh well. */
        fallback_random_ಬೈಟುಗಳು(p, n);
}

ಪೂರ್ಣಾಂಕ random_ಬೈಟುಗಳು_allocate_iovec(size_t n, ರಚನೆ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಶೂನ್ಯ *p = ಸ್ಮರಣೆ_ಕೊಡು(MAX(n, 1U));
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        random_ಬೈಟುಗಳು(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ crypto_random_ಬೈಟುಗಳು(ಶೂನ್ಯ *p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಗಾಗಿ (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                ಆದರೆ (l < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (l == 0)
                        ಹಿಂತಿರುಗಿ -EIO; /* Weird, should never happen. */

                ಆದರೆ ((size_t) l == n)
                        ಹಿಂತಿರುಗಿ 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

ಪೂರ್ಣಾಂಕ crypto_random_ಬೈಟುಗಳು_allocate_iovec(size_t n, ರಚನೆ iovec *ret) {
        _cleanup_free_ ಶೂನ್ಯ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        p = ಸ್ಮರಣೆ_ಕೊಡು(MAX(n, 1U));
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = crypto_random_ಬೈಟುಗಳು(p, n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ಹಿಂತಿರುಗಿ 0;
}

size_t random_pool_ಗಾತ್ರ(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Read pool size, if possible */
        r = read_one_ಸಾಲು_file("/proc/sys/kernel/random/poolsize", &s);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        ಇಲ್ಲದಿದ್ದರೆ {
                ಚಿಹ್ನೆರಹಿತ sz;

                r = ಸುರಕ್ಷಿತ_atou(s, &sz);
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                ಇಲ್ಲದಿದ್ದರೆ
                        /* poolsize is in bits on 2.6, but we want bytes */
                        ಹಿಂತಿರುಗಿ CLAMP(sz / 8, RANDOM_POOL_ಗಾತ್ರ_MIN, RANDOM_POOL_ಗಾತ್ರ_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        ಹಿಂತಿರುಗಿ RANDOM_POOL_ಗಾತ್ರ_MIN;
}

ಪೂರ್ಣಾಂಕ random_write_entropy(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬೀಜ, size_t ಗಾತ್ರ_2, bool credit) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಬೀಜ || ಗಾತ್ರ_2 == 0);

        ಆದರೆ (ಗಾತ್ರ_2 == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                ಆದರೆ (opened_fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                fd = opened_fd;
        }

        ಆದರೆ (credit) {
                _cleanup_free_ ರಚನೆ rand_pool_info *info = ಖಾಲಿ;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                ಆದರೆ (ಗಾತ್ರ_2 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / 8)
                        ಹಿಂತಿರುಗಿ -EOVERFLOW;

                info = ಸ್ಮರಣೆ_ಕೊಡು(offsetof(ರಚನೆ rand_pool_info, buf) + ಗಾತ್ರ_2);
                ಆದರೆ (!info)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                info->entropy_count = ಗಾತ್ರ_2 * 8;
                info->buf_size = ಗಾತ್ರ_2;
                ಸ್ಮರಣೆ_ನಕಲು(info->buf, ಬೀಜ, ಗಾತ್ರ_2);

                ಆದರೆ (ioctl(fd, RNDADDENTROPY, info) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = loop_write(fd, ಬೀಜ, ಗಾತ್ರ_2);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        ಆದರೆ (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                ಹಿಂತಿರುಗಿ random_u64();
        ಆದರೆ (m == 1)
                ಹಿಂತಿರುಗಿ 0;

        remainder = UINT64_MAX % m;

        ಮಾಡು {
                x = random_u64();
        } ಆಗ (x >= UINT64_MAX - remainder);

        ಹಿಂತಿರುಗಿ x % m;
}
