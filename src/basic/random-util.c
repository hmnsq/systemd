/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <elf.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <linux/random.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/auxv.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/random.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "pidfd-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"
#ଅନ୍ତର୍ଭାଵ "sha256.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
ସ୍ଥିର ଶୂନ୍ୟ fallback_random_bytes(ଶୂନ୍ୟ *p, size_t n) {
        ସ୍ଥିର thread_local uint64_t fallback_counter = 0;
        ସଂରଚନା {
                ଅକ୍ଷର label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", ଆକାର(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), ଆକାର(state.auxval));
        (ଶୂନ୍ୟ) pidfd_get_inode_id_self_cached(&state.pidfdid);

        ଯେତେବେଳେ (n > 0) {
                ସଂରଚନା sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, ଆକାର(state), &ctx);
                ଯଦି (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        ଭାଙ୍ଗ;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

ଶୂନ୍ୟ random_bytes(ଶୂନ୍ୟ *p, size_t n) {
        ପ୍ରତିଜ୍ଞା(p || n == 0);

        ଯଦି (n == 0)
                ଫେରନ୍ତୁ;

        ପାଇଁ (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                ଯଦି (l <= 0)
                        ଭାଙ୍ଗ; /* Unexpected error. Give up and fallback to /dev/urandom. */

                ଯଦି ((size_t) l == n)
                        ଫେରନ୍ତୁ; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ଯଦି (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                ଫେରନ୍ତୁ;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା random_bytes_allocate_iovec(size_t n, ସଂରଚନା iovec *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଶୂନ୍ୟ *p = malloc(MAX(n, 1U));
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା crypto_random_bytes(ଶୂନ୍ୟ *p, size_t n) {
        ପ୍ରତିଜ୍ଞା(p || n == 0);

        ଯଦି (n == 0)
                ଫେରନ୍ତୁ 0;

        ପାଇଁ (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                ଯଦି (l < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (l == 0)
                        ଫେରନ୍ତୁ -EIO; /* Weird, should never happen. */

                ଯଦି ((size_t) l == n)
                        ଫେରନ୍ତୁ 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା crypto_random_bytes_allocate_iovec(size_t n, ସଂରଚନା iovec *ret) {
        _cleanup_free_ ଶୂନ୍ୟ *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        p = malloc(MAX(n, 1U));
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        r = crypto_random_bytes(p, n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        ଫେରନ୍ତୁ 0;
}

size_t random_pool_size(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Read pool size, if possible */
        r = read_one_ଧାଡ଼ି_file("/proc/sys/kernel/random/poolsize", &s);
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        ଅନ୍ୟଥା {
                unsigned sz;

                r = safe_atou(s, &sz);
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                ଅନ୍ୟଥା
                        /* poolsize is in bits on 2.6, but we want bytes */
                        ଫେରନ୍ତୁ CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        ଫେରନ୍ତୁ RANDOM_POOL_SIZE_MIN;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା random_write_entropy(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *seed, size_t size, bool credit) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(seed || size == 0);

        ଯଦି (size == 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                ଯଦି (opened_fd < 0)
                        ଫେରନ୍ତୁ -errno;

                fd = opened_fd;
        }

        ଯଦି (credit) {
                _cleanup_free_ ସଂରଚନା rand_pool_info *info = ଖାଲି;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                ଯଦି (size > INT_MAX / 8)
                        ଫେରନ୍ତୁ -EOVERFLOW;

                info = malloc(offsetof(ସଂରଚନା rand_pool_info, buf) + size);
                ଯଦି (!info)
                        ଫେରନ୍ତୁ -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                ଯଦି (ioctl(fd, RNDADDENTROPY, info) < 0)
                        ଫେରନ୍ତୁ -errno;
        } ଅନ୍ୟଥା {
                r = loop_write(fd, seed, size);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        ଯଦି (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                ଫେରନ୍ତୁ random_u64();
        ଯଦି (m == 1)
                ଫେରନ୍ତୁ 0;

        remainder = UINT64_MAX % m;

        କର {
                x = random_u64();
        } ଯେତେବେଳେ (x >= UINT64_MAX - remainder);

        ଫେରନ୍ତୁ x % m;
}
