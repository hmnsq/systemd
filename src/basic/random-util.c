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
ساکن خالی fallback_random_بائٹیں(خالی *p, size_t n) {
        ساکن thread_local uint64_t fallback_شمار = 0;
        ساخت {
                حرف label[32];
                uint64_t call_id, بلاک_id;
                usec_t stamp_mono, stamp_real;
                pid_t pid, tid;
                uint64_t pidfdid;
                uint8_t auxval[16];
        } state = {
                /* Arbitrary domain separation to prevent other usage of AT_RANDOM from clashing. */
                .call_id = fallback_شمار++,
                .stamp_mono = now(CLOCK_MONOTONIC),
                .stamp_real = now(CLOCK_REALTIME),
                .pid = getpid_cached(),
                .tid = gettid(),
        };

        حافظہ_نقل(state.label, "systemd fallback random bytes v1", حجم(state.label));
        حافظہ_نقل(state.auxval, ULONG_مقصد_PTR(getauxval(AT_RANDOM)), حجم(state.auxval));
        (خالی) pidfd_get_inode_id_self_cached(&state.pidfdid);

        جب (n > 0) {
                ساخت sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_بائٹیں(&state, حجم(state), &ctx);
                اگر (n < SHA256_DIGEST_سائز) {
                        uint8_t partial[SHA256_DIGEST_سائز];
                        sha256_اختتام_ctx(&ctx, partial);
                        حافظہ_نقل(p, partial, n);
                        توڑو;
                }
                sha256_اختتام_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_سائز;
                n -= SHA256_DIGEST_سائز;
                ++state.بلاک_id;
        }
}

خالی random_بائٹیں(خالی *p, size_t n) {
        أكد(p || n == 0);

        اگر (n == 0)
                واپس;

        برائے (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                اگر (l <= 0)
                        توڑو; /* Unexpected error. Give up and fallback to /dev/urandom. */

                اگر ((size_t) l == n)
                        واپس; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ صحیح fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        اگر (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                واپس;

        /* This is a terrible fallback. Oh well. */
        fallback_random_بائٹیں(p, n);
}

صحیح random_بائٹیں_allocate_iovec(size_t n, ساخت iovec *ret) {
        أكد(ret);

        خالی *p = حافظہ_دو(MAX(n, 1U));
        اگر (!p)
                واپس -ENOMEM;

        random_بائٹیں(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        واپس 0;
}

صحیح crypto_random_بائٹیں(خالی *p, size_t n) {
        أكد(p || n == 0);

        اگر (n == 0)
                واپس 0;

        برائے (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                اگر (l < 0)
                        واپس -errno;
                اگر (l == 0)
                        واپس -EIO; /* Weird, should never happen. */

                اگر ((size_t) l == n)
                        واپس 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

صحیح crypto_random_بائٹیں_allocate_iovec(size_t n, ساخت iovec *ret) {
        _cleanup_free_ خالی *p = تہی;
        صحیح r;

        أكد(ret);

        p = حافظہ_دو(MAX(n, 1U));
        اگر (!p)
                واپس -ENOMEM;

        r = crypto_random_بائٹیں(p, n);
        اگر (r < 0)
                واپس r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        واپس 0;
}

size_t random_pool_سائز(خالی) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        /* Read pool size, if possible */
        r = read_one_سطر_file("/proc/sys/kernel/random/poolsize", &s);
        اگر (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        ورنہ {
                غیرعلامتی sz;

                r = محفوظ_atou(s, &sz);
                اگر (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                ورنہ
                        /* poolsize is in bits on 2.6, but we want bytes */
                        واپس CLAMP(sz / 8, RANDOM_POOL_سائز_MIN, RANDOM_POOL_سائز_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        واپس RANDOM_POOL_سائز_MIN;
}

صحیح random_write_entropy(صحیح fd, ثابت خالی *بیج, size_t سائز, bool credit) {
        _cleanup_close_ صحیح opened_fd = -EBADF;
        صحیح r;

        أكد(بیج || سائز == 0);

        اگر (سائز == 0)
                واپس 0;

        اگر (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                اگر (opened_fd < 0)
                        واپس -errno;

                fd = opened_fd;
        }

        اگر (credit) {
                _cleanup_free_ ساخت rand_pool_info *info = تہی;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                اگر (سائز > زیادہ_سے_زیادہ_عدد / 8)
                        واپس -EOVERFLOW;

                info = حافظہ_دو(offsetof(ساخت rand_pool_info, buf) + سائز);
                اگر (!info)
                        واپس -ENOMEM;

                info->entropy_count = سائز * 8;
                info->buf_size = سائز;
                حافظہ_نقل(info->buf, بیج, سائز);

                اگر (ioctl(fd, RNDADDENTROPY, info) < 0)
                        واپس -errno;
        } ورنہ {
                r = loop_write(fd, بیج, سائز);
                اگر (r < 0)
                        واپس r;
        }

        واپس 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        اگر (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                واپس random_u64();
        اگر (m == 1)
                واپس 0;

        remainder = UINT64_MAX % m;

        کرو {
                x = random_u64();
        } جب (x >= UINT64_MAX - remainder);

        واپس x % m;
}
