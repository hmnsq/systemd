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
ಸ್ಥಿರ ಶೂನ್ಯ fallback_random_bytes(ಶೂನ್ಯ *p, size_t n) {
        ಸ್ಥಿರ thread_local uint64_t fallback_counter = 0;
        ಸಂರಚನಾ {
                ವರ್ಣ label[32];
                uint64_t call_id, block_id;
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

        memcpy(state.label, "systemd fallback random bytes v1", ಪರಿಮಾಣ(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), ಪರಿಮಾಣ(state.auxval));
        (ಶೂನ್ಯ) pidfd_get_inode_id_self_cached(&state.pidfdid);

        ಯಾವತ್ (n > 0) {
                ಸಂರಚನಾ sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, ಪರಿಮಾಣ(state), &ctx);
                ಯದಿ (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        ಭಙ್ಗ;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

ಶೂನ್ಯ random_bytes(ಶೂನ್ಯ *p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ;

        ಪ್ರತಿ (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                ಯದಿ (l <= 0)
                        ಭಙ್ಗ; /* Unexpected error. Give up and fallback to /dev/urandom. */

                ಯದಿ ((size_t) l == n)
                        ಪ್ರತ್ಯಾಗಮ; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ಯದಿ (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                ಪ್ರತ್ಯಾಗಮ;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

ಪೂರ್ಣಾಙ್ಕ random_bytes_allocate_iovec(size_t n, ಸಂರಚನಾ iovec *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಶೂನ್ಯ *p = malloc(MAX(n, 1U));
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ crypto_random_bytes(ಶೂನ್ಯ *p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತಿ (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                ಯದಿ (l < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (l == 0)
                        ಪ್ರತ್ಯಾಗಮ -EIO; /* Weird, should never happen. */

                ಯದಿ ((size_t) l == n)
                        ಪ್ರತ್ಯಾಗಮ 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

ಪೂರ್ಣಾಙ್ಕ crypto_random_bytes_allocate_iovec(size_t n, ಸಂರಚನಾ iovec *ret) {
        _cleanup_free_ ಶೂನ್ಯ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        p = malloc(MAX(n, 1U));
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = crypto_random_bytes(p, n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

size_t random_pool_size(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Read pool size, if possible */
        r = read_one_ಸಾಲು_file("/proc/sys/kernel/random/poolsize", &s);
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        ಅನ್ಯಥಾ {
                unsigned sz;

                r = safe_atou(s, &sz);
                ಯದಿ (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                ಅನ್ಯಥಾ
                        /* poolsize is in bits on 2.6, but we want bytes */
                        ಪ್ರತ್ಯಾಗಮ CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        ಪ್ರತ್ಯಾಗಮ RANDOM_POOL_SIZE_MIN;
}

ಪೂರ್ಣಾಙ್ಕ random_write_entropy(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *seed, size_t size, bool credit) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(seed || size == 0);

        ಯದಿ (size == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                ಯದಿ (opened_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                fd = opened_fd;
        }

        ಯದಿ (credit) {
                _cleanup_free_ ಸಂರಚನಾ rand_pool_info *info = ರಿಕ್ತ;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                ಯದಿ (size > INT_MAX / 8)
                        ಪ್ರತ್ಯಾಗಮ -EOVERFLOW;

                info = malloc(offsetof(ಸಂರಚನಾ rand_pool_info, buf) + size);
                ಯದಿ (!info)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                ಯದಿ (ioctl(fd, RNDADDENTROPY, info) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        } ಅನ್ಯಥಾ {
                r = loop_write(fd, seed, size);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        ಯದಿ (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                ಪ್ರತ್ಯಾಗಮ random_u64();
        ಯದಿ (m == 1)
                ಪ್ರತ್ಯಾಗಮ 0;

        remainder = UINT64_MAX % m;

        ಕುರು {
                x = random_u64();
        } ಯಾವತ್ (x >= UINT64_MAX - remainder);

        ಪ್ರತ್ಯಾಗಮ x % m;
}
