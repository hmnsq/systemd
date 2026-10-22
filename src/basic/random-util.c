/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <elf.h>
#ضمن <fcntl.h>
#ضمن <linux/random.h>
#ضمن <string.h>
#ضمن <sys/auxv.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/random.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "io-util.h"
#ضمن "iovec-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "pidfd-util.h"
#ضمن "process-util.h"
#ضمن "random-util.h"
#ضمن "sha256.h"
#ضمن "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
ساكن فراغ fallback_random_bytes(فراغ *p, size_t n) {
        ساكن thread_local uint64_t fallback_counter = 0;
        بنية {
                حرف label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", حجم(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), حجم(state.auxval));
        (فراغ) pidfd_get_inode_id_self_cached(&state.pidfdid);

        طالما (n > 0) {
                بنية sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, حجم(state), &ctx);
                𐡄𐡍 (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        اكسر;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

فراغ random_bytes(فراغ *p, size_t n) {
        أكد(p || n == 0);

        𐡄𐡍 (n == 0)
                أرجع;

        لكل (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                𐡄𐡍 (l <= 0)
                        اكسر; /* Unexpected error. Give up and fallback to /dev/urandom. */

                𐡄𐡍 ((size_t) l == n)
                        أرجع; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ صحيح fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        𐡄𐡍 (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                أرجع;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

صحيح random_bytes_allocate_iovec(size_t n, بنية iovec *ret) {
        أكد(ret);

        فراغ *p = malloc(MAX(n, 1U));
        𐡄𐡍 (!p)
                أرجع -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        أرجع 0;
}

صحيح crypto_random_bytes(فراغ *p, size_t n) {
        أكد(p || n == 0);

        𐡄𐡍 (n == 0)
                أرجع 0;

        لكل (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                𐡄𐡍 (l < 0)
                        أرجع -errno;
                𐡄𐡍 (l == 0)
                        أرجع -EIO; /* Weird, should never happen. */

                𐡄𐡍 ((size_t) l == n)
                        أرجع 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

صحيح crypto_random_bytes_allocate_iovec(size_t n, بنية iovec *ret) {
        _cleanup_free_ فراغ *p = عدم;
        صحيح r;

        أكد(ret);

        p = malloc(MAX(n, 1U));
        𐡄𐡍 (!p)
                أرجع -ENOMEM;

        r = crypto_random_bytes(p, n);
        𐡄𐡍 (r < 0)
                أرجع r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        أرجع 0;
}

size_t random_pool_size(فراغ) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        /* Read pool size, if possible */
        r = read_one_line_file("/proc/sys/kernel/random/poolsize", &s);
        𐡄𐡍 (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        وإلا {
                unsigned sz;

                r = safe_atou(s, &sz);
                𐡄𐡍 (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                وإلا
                        /* poolsize is in bits on 2.6, but we want bytes */
                        أرجع CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        أرجع RANDOM_POOL_SIZE_MIN;
}

صحيح random_write_entropy(صحيح fd, ثابت فراغ *seed, size_t size, bool credit) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        صحيح r;

        أكد(seed || size == 0);

        𐡄𐡍 (size == 0)
                أرجع 0;

        𐡄𐡍 (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                𐡄𐡍 (opened_fd < 0)
                        أرجع -errno;

                fd = opened_fd;
        }

        𐡄𐡍 (credit) {
                _cleanup_free_ بنية rand_pool_info *info = عدم;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                𐡄𐡍 (size > INT_MAX / 8)
                        أرجع -EOVERFLOW;

                info = malloc(offsetof(بنية rand_pool_info, buf) + size);
                𐡄𐡍 (!info)
                        أرجع -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                𐡄𐡍 (ioctl(fd, RNDADDENTROPY, info) < 0)
                        أرجع -errno;
        } وإلا {
                r = loop_write(fd, seed, size);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        أرجع 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        𐡄𐡍 (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                أرجع random_u64();
        𐡄𐡍 (m == 1)
                أرجع 0;

        remainder = UINT64_MAX % m;

        𐡏𐡁𐡃 {
                x = random_u64();
        } طالما (x >= UINT64_MAX - remainder);

        أرجع x % m;
}
