/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <elf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/random.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/auxv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/random.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidfd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
ਸਥਿਰ ਖਾਲੀ fallback_random_ਬਾਈਟਾਂ(ਖਾਲੀ *p, size_t n) {
        ਸਥਿਰ thread_local uint64_t fallback_counter = 0;
        ਬਣਤਰ {
                ਅੱਖਰ label[32];
                uint64_t call_id, ਖੰਡ_id;
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

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(state.label, "systemd fallback random bytes v1", ਆਕਾਰ(state.label));
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(state.auxval, ULONG_ਮੰਜ਼ਿਲ_PTR(getauxval(AT_RANDOM)), ਆਕਾਰ(state.auxval));
        (ਖਾਲੀ) pidfd_get_inode_id_self_cached(&state.pidfdid);

        ਜਦੋਂ (n > 0) {
                ਬਣਤਰ sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_ਬਾਈਟਾਂ(&state, ਆਕਾਰ(state), &ctx);
                ਜੇ (n < SHA256_DIGEST_ਮਾਪ) {
                        uint8_t partial[SHA256_DIGEST_ਮਾਪ];
                        sha256_ਅੰਤ_ctx(&ctx, partial);
                        ਸਮ੍ਰਿਤੀ_ਨਕਲ(p, partial, n);
                        ਰੋਕੋ;
                }
                sha256_ਅੰਤ_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_ਮਾਪ;
                n -= SHA256_DIGEST_ਮਾਪ;
                ++state.ਖੰਡ_id;
        }
}

ਖਾਲੀ random_ਬਾਈਟਾਂ(ਖਾਲੀ *p, size_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p || n == 0);

        ਜੇ (n == 0)
                ਵਾਪਸ;

        ਲਈ (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                ਜੇ (l <= 0)
                        ਰੋਕੋ; /* Unexpected error. Give up and fallback to /dev/urandom. */

                ਜੇ ((size_t) l == n)
                        ਵਾਪਸ; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ ਪੂਰਨ fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ਜੇ (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                ਵਾਪਸ;

        /* This is a terrible fallback. Oh well. */
        fallback_random_ਬਾਈਟਾਂ(p, n);
}

ਪੂਰਨ random_ਬਾਈਟਾਂ_allocate_iovec(size_t n, ਬਣਤਰ iovec *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਖਾਲੀ *p = ਸਮ੍ਰਿਤੀ_ਦਿਓ(MAX(n, 1U));
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        random_ਬਾਈਟਾਂ(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ਵਾਪਸ 0;
}

ਪੂਰਨ crypto_random_ਬਾਈਟਾਂ(ਖਾਲੀ *p, size_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p || n == 0);

        ਜੇ (n == 0)
                ਵਾਪਸ 0;

        ਲਈ (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                ਜੇ (l < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (l == 0)
                        ਵਾਪਸ -EIO; /* Weird, should never happen. */

                ਜੇ ((size_t) l == n)
                        ਵਾਪਸ 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

ਪੂਰਨ crypto_random_ਬਾਈਟਾਂ_allocate_iovec(size_t n, ਬਣਤਰ iovec *ret) {
        _cleanup_free_ ਖਾਲੀ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        p = ਸਮ੍ਰਿਤੀ_ਦਿਓ(MAX(n, 1U));
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        r = crypto_random_ਬਾਈਟਾਂ(p, n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ਵਾਪਸ 0;
}

size_t random_pool_ਮਾਪ(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Read pool size, if possible */
        r = read_one_ਸਤਰ_file("/proc/sys/kernel/random/poolsize", &s);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        ਨਹੀਂਤਾਂ {
                ਬਿਨਾਨਿਸ਼ਾਨੀ sz;

                r = ਸੁਰੱਖਿਅਤ_atou(s, &sz);
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                ਨਹੀਂਤਾਂ
                        /* poolsize is in bits on 2.6, but we want bytes */
                        ਵਾਪਸ CLAMP(sz / 8, RANDOM_POOL_ਮਾਪ_MIN, RANDOM_POOL_ਮਾਪ_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        ਵਾਪਸ RANDOM_POOL_ਮਾਪ_MIN;
}

ਪੂਰਨ random_write_entropy(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *ਬੀਜ, size_t ਮਾਪ, bool credit) {
        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਬੀਜ || ਮਾਪ == 0);

        ਜੇ (ਮਾਪ == 0)
                ਵਾਪਸ 0;

        ਜੇ (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                ਜੇ (opened_fd < 0)
                        ਵਾਪਸ -errno;

                fd = opened_fd;
        }

        ਜੇ (credit) {
                _cleanup_free_ ਬਣਤਰ rand_pool_info *info = ਨਿਰਾ;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                ਜੇ (ਮਾਪ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ / 8)
                        ਵਾਪਸ -EOVERFLOW;

                info = ਸਮ੍ਰਿਤੀ_ਦਿਓ(offsetof(ਬਣਤਰ rand_pool_info, buf) + ਮਾਪ);
                ਜੇ (!info)
                        ਵਾਪਸ -ENOMEM;

                info->entropy_count = ਮਾਪ * 8;
                info->buf_size = ਮਾਪ;
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(info->buf, ਬੀਜ, ਮਾਪ);

                ਜੇ (ioctl(fd, RNDADDENTROPY, info) < 0)
                        ਵਾਪਸ -errno;
        } ਨਹੀਂਤਾਂ {
                r = loop_write(fd, ਬੀਜ, ਮਾਪ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        ਜੇ (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                ਵਾਪਸ random_u64();
        ਜੇ (m == 1)
                ਵਾਪਸ 0;

        remainder = UINT64_MAX % m;

        ਕਰੋ {
                x = random_u64();
        } ਜਦੋਂ (x >= UINT64_MAX - remainder);

        ਵਾਪਸ x % m;
}
