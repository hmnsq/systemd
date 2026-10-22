/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <elf.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <linux/random.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/auxv.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/random.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "pidfd-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "random-util.h"
#அந்தர்பாவ "sha256.h"
#அந்தர்பாவ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
நிலையான வெறுமை fallback_random_bytes(வெறுமை *p, size_t n) {
        நிலையான thread_local uint64_t fallback_counter = 0;
        கட்டமைப்பு {
                எழுத்து label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", அளவு(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), அளவு(state.auxval));
        (வெறுமை) pidfd_get_inode_id_self_cached(&state.pidfdid);

        போது (n > 0) {
                கட்டமைப்பு sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, அளவு(state), &ctx);
                எனில் (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        நிறுத்து;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

வெறுமை random_bytes(வெறுமை *p, size_t n) {
        ப்ரதிஜ்ஞா(p || n == 0);

        எனில் (n == 0)
                திரும்பு;

        க்கு (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                எனில் (l <= 0)
                        நிறுத்து; /* Unexpected error. Give up and fallback to /dev/urandom. */

                எனில் ((size_t) l == n)
                        திரும்பு; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ முழு fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        எனில் (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                திரும்பு;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

முழு random_bytes_allocate_iovec(size_t n, கட்டமைப்பு iovec *ret) {
        ப்ரதிஜ்ஞா(ret);

        வெறுமை *p = malloc(MAX(n, 1U));
        எனில் (!p)
                திரும்பு -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        திரும்பு 0;
}

முழு crypto_random_bytes(வெறுமை *p, size_t n) {
        ப்ரதிஜ்ஞா(p || n == 0);

        எனில் (n == 0)
                திரும்பு 0;

        க்கு (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                எனில் (l < 0)
                        திரும்பு -errno;
                எனில் (l == 0)
                        திரும்பு -EIO; /* Weird, should never happen. */

                எனில் ((size_t) l == n)
                        திரும்பு 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

முழு crypto_random_bytes_allocate_iovec(size_t n, கட்டமைப்பு iovec *ret) {
        _cleanup_free_ வெறுமை *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        p = malloc(MAX(n, 1U));
        எனில் (!p)
                திரும்பு -ENOMEM;

        r = crypto_random_bytes(p, n);
        எனில் (r < 0)
                திரும்பு r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        திரும்பு 0;
}

size_t random_pool_size(வெறுமை) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        /* Read pool size, if possible */
        r = read_one_வரி_file("/proc/sys/kernel/random/poolsize", &s);
        எனில் (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        இல்லையெனில் {
                unsigned sz;

                r = safe_atou(s, &sz);
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                இல்லையெனில்
                        /* poolsize is in bits on 2.6, but we want bytes */
                        திரும்பு CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        திரும்பு RANDOM_POOL_SIZE_MIN;
}

முழு random_write_entropy(முழு fd, மாறிலி வெறுமை *seed, size_t size, bool credit) {
        _cleanup_close_ முழு opened_fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(seed || size == 0);

        எனில் (size == 0)
                திரும்பு 0;

        எனில் (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                எனில் (opened_fd < 0)
                        திரும்பு -errno;

                fd = opened_fd;
        }

        எனில் (credit) {
                _cleanup_free_ கட்டமைப்பு rand_pool_info *info = வெற்று;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                எனில் (size > INT_MAX / 8)
                        திரும்பு -EOVERFLOW;

                info = malloc(offsetof(கட்டமைப்பு rand_pool_info, buf) + size);
                எனில் (!info)
                        திரும்பு -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                எனில் (ioctl(fd, RNDADDENTROPY, info) < 0)
                        திரும்பு -errno;
        } இல்லையெனில் {
                r = loop_write(fd, seed, size);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        எனில் (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                திரும்பு random_u64();
        எனில் (m == 1)
                திரும்பு 0;

        remainder = UINT64_MAX % m;

        செய் {
                x = random_u64();
        } போது (x >= UINT64_MAX - remainder);

        திரும்பு x % m;
}
