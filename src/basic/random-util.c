/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <elf.h>
#include <fcntl.h>
#include <linux/random.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/ioctl.h>
#include <sys/random.h>
#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "io-util.h"
#include "iovec-util.h"
#include "log.h"
#include "parse-util.h"
#include "pidfd-util.h"
#include "process-util.h"
#include "random-util.h"
#include "sha256.h"
#include "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
static void fallback_random_bytes(void *p, size_t n) {
        static thread_local uint64_t fallback_counter = 0;
        struct {
                char label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", sizeof(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), sizeof(state.auxval));
        (void) pidfd_get_inode_id_self_cached(&state.pidfdid);

        동안 (n > 0) {
                struct sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, sizeof(state), &ctx);
                만약 (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        break;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

void random_bytes(void *p, size_t n) {
        assert(p || n == 0);

        만약 (n == 0)
                반환;

        위해 (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                만약 (l <= 0)
                        break; /* Unexpected error. Give up and fallback to /dev/urandom. */

                만약 ((size_t) l == n)
                        반환; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ 정수 fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        만약 (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                반환;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

정수 random_bytes_allocate_iovec(size_t n, struct iovec *ret) {
        assert(ret);

        void *p = malloc(MAX(n, 1U));
        만약 (!p)
                반환 -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        반환 0;
}

정수 crypto_random_bytes(void *p, size_t n) {
        assert(p || n == 0);

        만약 (n == 0)
                반환 0;

        위해 (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                만약 (l < 0)
                        반환 -errno;
                만약 (l == 0)
                        반환 -EIO; /* Weird, should never happen. */

                만약 ((size_t) l == n)
                        반환 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

정수 crypto_random_bytes_allocate_iovec(size_t n, struct iovec *ret) {
        _cleanup_free_ void *p = NULL;
        정수 r;

        assert(ret);

        p = malloc(MAX(n, 1U));
        만약 (!p)
                반환 -ENOMEM;

        r = crypto_random_bytes(p, n);
        만약 (r < 0)
                반환 r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        반환 0;
}

size_t random_pool_size(void) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        /* Read pool size, if possible */
        r = read_one_line_file("/proc/sys/kernel/random/poolsize", &s);
        만약 (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        아니면 {
                unsigned sz;

                r = safe_atou(s, &sz);
                만약 (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                아니면
                        /* poolsize is in bits on 2.6, but we want bytes */
                        반환 CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        반환 RANDOM_POOL_SIZE_MIN;
}

정수 random_write_entropy(정수 fd, const void *seed, size_t size, bool credit) {
        _cleanup_close_ 정수 opened_fd = -EBADF;
        정수 r;

        assert(seed || size == 0);

        만약 (size == 0)
                반환 0;

        만약 (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                만약 (opened_fd < 0)
                        반환 -errno;

                fd = opened_fd;
        }

        만약 (credit) {
                _cleanup_free_ struct rand_pool_info *info = NULL;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                만약 (size > INT_MAX / 8)
                        반환 -EOVERFLOW;

                info = malloc(offsetof(struct rand_pool_info, buf) + size);
                만약 (!info)
                        반환 -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                만약 (ioctl(fd, RNDADDENTROPY, info) < 0)
                        반환 -errno;
        } 아니면 {
                r = loop_write(fd, seed, size);
                만약 (r < 0)
                        반환 r;
        }

        반환 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        만약 (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                반환 random_u64();
        만약 (m == 1)
                반환 0;

        remainder = UINT64_MAX % m;

        do {
                x = random_u64();
        } 동안 (x >= UINT64_MAX - remainder);

        반환 x % m;
}
