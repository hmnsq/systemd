/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <elf.h>
#شامل <fcntl.h>
#شامل <linux/random.h>
#شامل <string.h>
#شامل <sys/auxv.h>
#شامل <sys/ioctl.h>
#شامل <sys/random.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "io-util.h"
#شامل "iovec-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "pidfd-util.h"
#شامل "process-util.h"
#شامل "random-util.h"
#شامل "sha256.h"
#شامل "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
ایستا تهی fallback_random_bytes(تهی *p, size_t n) {
        ایستا thread_local uint64_t fallback_counter = 0;
        ساختار {
                نویسه label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", اندازه(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), اندازه(state.auxval));
        (تهی) pidfd_get_inode_id_self_cached(&state.pidfdid);

        وقتی (n > 0) {
                ساختار sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, اندازه(state), &ctx);
                اگر (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        بشکن;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

تهی random_bytes(تهی *p, size_t n) {
        أكد(p || n == 0);

        اگر (n == 0)
                بازگشت;

        برای (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                اگر (l <= 0)
                        بشکن; /* Unexpected error. Give up and fallback to /dev/urandom. */

                اگر ((size_t) l == n)
                        بازگشت; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ صحیح fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        اگر (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                بازگشت;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

صحیح random_bytes_allocate_iovec(size_t n, ساختار iovec *ret) {
        أكد(ret);

        تهی *p = malloc(MAX(n, 1U));
        اگر (!p)
                بازگشت -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        بازگشت 0;
}

صحیح crypto_random_bytes(تهی *p, size_t n) {
        أكد(p || n == 0);

        اگر (n == 0)
                بازگشت 0;

        برای (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                اگر (l < 0)
                        بازگشت -errno;
                اگر (l == 0)
                        بازگشت -EIO; /* Weird, should never happen. */

                اگر ((size_t) l == n)
                        بازگشت 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

صحیح crypto_random_bytes_allocate_iovec(size_t n, ساختار iovec *ret) {
        _cleanup_free_ تهی *p = پوچ;
        صحیح r;

        أكد(ret);

        p = malloc(MAX(n, 1U));
        اگر (!p)
                بازگشت -ENOMEM;

        r = crypto_random_bytes(p, n);
        اگر (r < 0)
                بازگشت r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        بازگشت 0;
}

size_t random_pool_size(تهی) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        /* Read pool size, if possible */
        r = read_one_سطر_file("/proc/sys/kernel/random/poolsize", &s);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        وگرنه {
                unsigned sz;

                r = safe_atou(s, &sz);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                وگرنه
                        /* poolsize is in bits on 2.6, but we want bytes */
                        بازگشت CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        بازگشت RANDOM_POOL_SIZE_MIN;
}

صحیح random_write_entropy(صحیح fd, ثابت تهی *seed, size_t size, bool credit) {
        _cleanup_close_ صحیح opened_fd = -EBADF;
        صحیح r;

        أكد(seed || size == 0);

        اگر (size == 0)
                بازگشت 0;

        اگر (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                اگر (opened_fd < 0)
                        بازگشت -errno;

                fd = opened_fd;
        }

        اگر (credit) {
                _cleanup_free_ ساختار rand_pool_info *info = پوچ;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                اگر (size > INT_MAX / 8)
                        بازگشت -EOVERFLOW;

                info = malloc(offsetof(ساختار rand_pool_info, buf) + size);
                اگر (!info)
                        بازگشت -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                اگر (ioctl(fd, RNDADDENTROPY, info) < 0)
                        بازگشت -errno;
        } وگرنه {
                r = loop_write(fd, seed, size);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        اگر (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                بازگشت random_u64();
        اگر (m == 1)
                بازگشت 0;

        remainder = UINT64_MAX % m;

        انجامبده {
                x = random_u64();
        } وقتی (x >= UINT64_MAX - remainder);

        بازگشت x % m;
}
