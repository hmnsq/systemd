/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <elf.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <linux/random.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/auxv.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/random.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "pidfd-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "random-util.h"
#અન્તર્ભાવ "sha256.h"
#અન્તર્ભાવ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
સ્થિર શૂન્ય fallback_random_bytes(શૂન્ય *p, size_t n) {
        સ્થિર thread_local uint64_t fallback_counter = 0;
        સંરચના {
                અક્ષર label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", કદ(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), કદ(state.auxval));
        (શૂન્ય) pidfd_get_inode_id_self_cached(&state.pidfdid);

        જ્યારે (n > 0) {
                સંરચના sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, કદ(state), &ctx);
                જો (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        તોડો;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

શૂન્ય random_bytes(શૂન્ય *p, size_t n) {
        પ્રતિજ્ઞા(p || n == 0);

        જો (n == 0)
                પાછા;

        માટે (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                જો (l <= 0)
                        તોડો; /* Unexpected error. Give up and fallback to /dev/urandom. */

                જો ((size_t) l == n)
                        પાછા; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ પૂર્ણાંક fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        જો (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                પાછા;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

પૂર્ણાંક random_bytes_allocate_iovec(size_t n, સંરચના iovec *ret) {
        પ્રતિજ્ઞા(ret);

        શૂન્ય *p = malloc(MAX(n, 1U));
        જો (!p)
                પાછા -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        પાછા 0;
}

પૂર્ણાંક crypto_random_bytes(શૂન્ય *p, size_t n) {
        પ્રતિજ્ઞા(p || n == 0);

        જો (n == 0)
                પાછા 0;

        માટે (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                જો (l < 0)
                        પાછા -errno;
                જો (l == 0)
                        પાછા -EIO; /* Weird, should never happen. */

                જો ((size_t) l == n)
                        પાછા 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

પૂર્ણાંક crypto_random_bytes_allocate_iovec(size_t n, સંરચના iovec *ret) {
        _cleanup_free_ શૂન્ય *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        p = malloc(MAX(n, 1U));
        જો (!p)
                પાછા -ENOMEM;

        r = crypto_random_bytes(p, n);
        જો (r < 0)
                પાછા r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        પાછા 0;
}

size_t random_pool_size(શૂન્ય) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        /* Read pool size, if possible */
        r = read_one_લીટી_file("/proc/sys/kernel/random/poolsize", &s);
        જો (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        નહીંતર {
                unsigned sz;

                r = safe_atou(s, &sz);
                જો (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                નહીંતર
                        /* poolsize is in bits on 2.6, but we want bytes */
                        પાછા CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        પાછા RANDOM_POOL_SIZE_MIN;
}

પૂર્ણાંક random_write_entropy(પૂર્ણાંક fd, અચલ શૂન્ય *seed, size_t size, bool credit) {
        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(seed || size == 0);

        જો (size == 0)
                પાછા 0;

        જો (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                જો (opened_fd < 0)
                        પાછા -errno;

                fd = opened_fd;
        }

        જો (credit) {
                _cleanup_free_ સંરચના rand_pool_info *info = ખાલી;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                જો (size > INT_MAX / 8)
                        પાછા -EOVERFLOW;

                info = malloc(offsetof(સંરચના rand_pool_info, buf) + size);
                જો (!info)
                        પાછા -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                જો (ioctl(fd, RNDADDENTROPY, info) < 0)
                        પાછા -errno;
        } નહીંતર {
                r = loop_write(fd, seed, size);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        જો (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                પાછા random_u64();
        જો (m == 1)
                પાછા 0;

        remainder = UINT64_MAX % m;

        કરો {
                x = random_u64();
        } જ્યારે (x >= UINT64_MAX - remainder);

        પાછા x % m;
}
