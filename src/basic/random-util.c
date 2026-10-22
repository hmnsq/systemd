/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <elf.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <linux/random.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/auxv.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/random.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "pidfd-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "random-util.h"
#অন্তর্ভাব "sha256.h"
#অন্তর্ভাব "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
স্থির শূন্য fallback_random_bytes(শূন্য *p, size_t n) {
        স্থির thread_local uint64_t fallback_গণক = 0;
        কাঠামো {
                অক্ষর label[32];
                uint64_t call_id, block_id;
                usec_t stamp_mono, stamp_real;
                pid_t pid, tid;
                uint64_t pidfdid;
                uint8_t auxval[16];
        } state = {
                /* Arbitrary domain separation to prevent other usage of AT_RANDOM from clashing. */
                .call_id = fallback_গণক++,
                .stamp_mono = now(CLOCK_MONOTONIC),
                .stamp_real = now(CLOCK_REALTIME),
                .pid = getpid_cached(),
                .tid = gettid(),
        };

        memcpy(state.label, "systemd fallback random bytes v1", আকার(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), আকার(state.auxval));
        (শূন্য) pidfd_get_inode_id_self_cached(&state.pidfdid);

        যখন (n > 0) {
                কাঠামো sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, আকার(state), &ctx);
                যদি (n < SHA256_DIGEST_আকার) {
                        uint8_t partial[SHA256_DIGEST_আকার];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        বিরতি;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_আকার;
                n -= SHA256_DIGEST_আকার;
                ++state.block_id;
        }
}

শূন্য random_bytes(শূন্য *p, size_t n) {
        প্রতিজ্ঞা(p || n == 0);

        যদি (n == 0)
                ফেরত;

        জন্য (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                যদি (l <= 0)
                        বিরতি; /* Unexpected error. Give up and fallback to /dev/urandom. */

                যদি ((size_t) l == n)
                        ফেরত; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ পূর্ণসংখ্যা fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        যদি (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                ফেরত;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

পূর্ণসংখ্যা random_bytes_allocate_iovec(size_t n, কাঠামো iovec *ret) {
        প্রতিজ্ঞা(ret);

        শূন্য *p = malloc(MAX(n, 1U));
        যদি (!p)
                ফেরত -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ফেরত 0;
}

পূর্ণসংখ্যা crypto_random_bytes(শূন্য *p, size_t n) {
        প্রতিজ্ঞা(p || n == 0);

        যদি (n == 0)
                ফেরত 0;

        জন্য (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                যদি (l < 0)
                        ফেরত -errno;
                যদি (l == 0)
                        ফেরত -EIO; /* Weird, should never happen. */

                যদি ((size_t) l == n)
                        ফেরত 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

পূর্ণসংখ্যা crypto_random_bytes_allocate_iovec(size_t n, কাঠামো iovec *ret) {
        _cleanup_free_ শূন্য *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        p = malloc(MAX(n, 1U));
        যদি (!p)
                ফেরত -ENOMEM;

        r = crypto_random_bytes(p, n);
        যদি (r < 0)
                ফেরত r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ফেরত 0;
}

size_t random_pool_আকার(শূন্য) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Read pool size, if possible */
        r = read_one_সারি_file("/proc/sys/kernel/random/poolsize", &s);
        যদি (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        অন্যথা {
                unsigned sz;

                r = safe_atou(s, &sz);
                যদি (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                অন্যথা
                        /* poolsize is in bits on 2.6, but we want bytes */
                        ফেরত CLAMP(sz / 8, RANDOM_POOL_আকার_MIN, RANDOM_POOL_আকার_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        ফেরত RANDOM_POOL_আকার_MIN;
}

পূর্ণসংখ্যা random_write_entropy(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *seed, size_t আকার_2, bool credit) {
        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(seed || আকার_2 == 0);

        যদি (আকার_2 == 0)
                ফেরত 0;

        যদি (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                যদি (opened_fd < 0)
                        ফেরত -errno;

                fd = opened_fd;
        }

        যদি (credit) {
                _cleanup_free_ কাঠামো rand_pool_info *info = ফাঁকা;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                যদি (আকার_2 > INT_MAX / 8)
                        ফেরত -EOVERFLOW;

                info = malloc(offsetof(কাঠামো rand_pool_info, buf) + আকার_2);
                যদি (!info)
                        ফেরত -ENOMEM;

                info->entropy_count = আকার_2 * 8;
                info->buf_size = আকার_2;
                memcpy(info->buf, seed, আকার_2);

                যদি (ioctl(fd, RNDADDENTROPY, info) < 0)
                        ফেরত -errno;
        } অন্যথা {
                r = loop_write(fd, seed, আকার_2);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        যদি (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                ফেরত random_u64();
        যদি (m == 1)
                ফেরত 0;

        remainder = UINT64_MAX % m;

        করো {
                x = random_u64();
        } যখন (x >= UINT64_MAX - remainder);

        ফেরত x % m;
}
