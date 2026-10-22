/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <elf.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <linux/random.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/auxv.h>
#অন্তর্ভাৱ <sys/ioctl.h>
#অন্তর্ভাৱ <sys/random.h>
#অন্তর্ভাৱ <threads.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "io-util.h"
#অন্তর্ভাৱ "iovec-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "pidfd-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "random-util.h"
#অন্তর্ভাৱ "sha256.h"
#অন্তর্ভাৱ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
স্থির শূন্য fallback_random_bytes(শূন্য *p, size_t n) {
        স্থির thread_local uint64_t fallback_counter = 0;
        সংরচনা {
                ৱর্ণ label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", পরিমাণ(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), পরিমাণ(state.auxval));
        (শূন্য) pidfd_get_inode_id_self_cached(&state.pidfdid);

        যেতিয়ালৈ (n > 0) {
                সংরচনা sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, পরিমাণ(state), &ctx);
                যদি (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        ভঙ্গ;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

শূন্য random_bytes(শূন্য *p, size_t n) {
        প্রতিজ্ঞা(p || n == 0);

        যদি (n == 0)
                ঘূৰাই;

        বাবে (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                যদি (l <= 0)
                        ভঙ্গ; /* Unexpected error. Give up and fallback to /dev/urandom. */

                যদি ((size_t) l == n)
                        ঘূৰাই; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ পূর্ণাঙ্ক fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        যদি (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                ঘূৰাই;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

পূর্ণাঙ্ক random_bytes_allocate_iovec(size_t n, সংরচনা iovec *ret) {
        প্রতিজ্ঞা(ret);

        শূন্য *p = malloc(MAX(n, 1U));
        যদি (!p)
                ঘূৰাই -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক crypto_random_bytes(শূন্য *p, size_t n) {
        প্রতিজ্ঞা(p || n == 0);

        যদি (n == 0)
                ঘূৰাই 0;

        বাবে (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                যদি (l < 0)
                        ঘূৰাই -errno;
                যদি (l == 0)
                        ঘূৰাই -EIO; /* Weird, should never happen. */

                যদি ((size_t) l == n)
                        ঘূৰাই 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

পূর্ণাঙ্ক crypto_random_bytes_allocate_iovec(size_t n, সংরচনা iovec *ret) {
        _cleanup_free_ শূন্য *p = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        p = malloc(MAX(n, 1U));
        যদি (!p)
                ঘূৰাই -ENOMEM;

        r = crypto_random_bytes(p, n);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ঘূৰাই 0;
}

size_t random_pool_size(শূন্য) {
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        /* Read pool size, if possible */
        r = read_one_সাৰি_file("/proc/sys/kernel/random/poolsize", &s);
        যদি (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        অন্যথা {
                unsigned sz;

                r = safe_atou(s, &sz);
                যদি (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                অন্যথা
                        /* poolsize is in bits on 2.6, but we want bytes */
                        ঘূৰাই CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        ঘূৰাই RANDOM_POOL_SIZE_MIN;
}

পূর্ণাঙ্ক random_write_entropy(পূর্ণাঙ্ক fd, ধ্রুৱ শূন্য *seed, size_t size, bool credit) {
        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(seed || size == 0);

        যদি (size == 0)
                ঘূৰাই 0;

        যদি (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                যদি (opened_fd < 0)
                        ঘূৰাই -errno;

                fd = opened_fd;
        }

        যদি (credit) {
                _cleanup_free_ সংরচনা rand_pool_info *info = রিক্ত;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                যদি (size > INT_MAX / 8)
                        ঘূৰাই -EOVERFLOW;

                info = malloc(offsetof(সংরচনা rand_pool_info, buf) + size);
                যদি (!info)
                        ঘূৰাই -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                যদি (ioctl(fd, RNDADDENTROPY, info) < 0)
                        ঘূৰাই -errno;
        } অন্যথা {
                r = loop_write(fd, seed, size);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        যদি (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                ঘূৰাই random_u64();
        যদি (m == 1)
                ঘূৰাই 0;

        remainder = UINT64_MAX % m;

        কৰা {
                x = random_u64();
        } যেতিয়ালৈ (x >= UINT64_MAX - remainder);

        ঘূৰাই x % m;
}
