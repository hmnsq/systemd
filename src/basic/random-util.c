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
ساكن فراغ fallback_random_بائيٽون(فراغ *p, size_t n) {
        ساكن thread_local uint64_t fallback_counter = 0;
        بنية {
                حرف label[32];
                uint64_t call_id, بلاڪ_id;
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

        يادگيري_نقل(state.label, "systemd fallback random bytes v1", حجم(state.label));
        يادگيري_نقل(state.auxval, ULONG_منزل_PTR(getauxval(AT_RANDOM)), حجم(state.auxval));
        (فراغ) pidfd_get_inode_id_self_cached(&state.pidfdid);

        طالما (n > 0) {
                بنية sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_بائيٽون(&state, حجم(state), &ctx);
                إذا (n < SHA256_DIGEST_سائيز) {
                        uint8_t partial[SHA256_DIGEST_سائيز];
                        sha256_پڄاڻي_ctx(&ctx, partial);
                        يادگيري_نقل(p, partial, n);
                        اكسر;
                }
                sha256_پڄاڻي_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_سائيز;
                n -= SHA256_DIGEST_سائيز;
                ++state.بلاڪ_id;
        }
}

فراغ random_بائيٽون(فراغ *p, size_t n) {
        أكد(p || n == 0);

        إذا (n == 0)
                أرجع;

        لكل (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                إذا (l <= 0)
                        اكسر; /* Unexpected error. Give up and fallback to /dev/urandom. */

                إذا ((size_t) l == n)
                        أرجع; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ صحيح fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        إذا (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                أرجع;

        /* This is a terrible fallback. Oh well. */
        fallback_random_بائيٽون(p, n);
}

صحيح random_بائيٽون_allocate_iovec(size_t n, بنية iovec *ret) {
        أكد(ret);

        فراغ *p = يادگيري_ڏيو(MAX(n, 1U));
        إذا (!p)
                أرجع -ENOMEM;

        random_بائيٽون(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        أرجع 0;
}

صحيح crypto_random_بائيٽون(فراغ *p, size_t n) {
        أكد(p || n == 0);

        إذا (n == 0)
                أرجع 0;

        لكل (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                إذا (l < 0)
                        أرجع -errno;
                إذا (l == 0)
                        أرجع -EIO; /* Weird, should never happen. */

                إذا ((size_t) l == n)
                        أرجع 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

صحيح crypto_random_بائيٽون_allocate_iovec(size_t n, بنية iovec *ret) {
        _cleanup_free_ فراغ *p = عدم;
        صحيح r;

        أكد(ret);

        p = يادگيري_ڏيو(MAX(n, 1U));
        إذا (!p)
                أرجع -ENOMEM;

        r = crypto_random_بائيٽون(p, n);
        إذا (r < 0)
                أرجع r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        أرجع 0;
}

size_t random_pool_سائيز(فراغ) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        /* Read pool size, if possible */
        r = read_one_سٽ_file("/proc/sys/kernel/random/poolsize", &s);
        إذا (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        وإلا {
                بنانشان sz;

                r = محفوظ_atou(s, &sz);
                إذا (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                وإلا
                        /* poolsize is in bits on 2.6, but we want bytes */
                        أرجع CLAMP(sz / 8, RANDOM_POOL_سائيز_MIN, RANDOM_POOL_سائيز_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        أرجع RANDOM_POOL_سائيز_MIN;
}

صحيح random_write_entropy(صحيح fd, ثابت فراغ *ٻج, size_t سائيز, bool credit) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        صحيح r;

        أكد(ٻج || سائيز == 0);

        إذا (سائيز == 0)
                أرجع 0;

        إذا (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                إذا (opened_fd < 0)
                        أرجع -errno;

                fd = opened_fd;
        }

        إذا (credit) {
                _cleanup_free_ بنية rand_pool_info *info = عدم;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                إذا (سائيز > وڌ_کان_وڌ_عدد / 8)
                        أرجع -EOVERFLOW;

                info = يادگيري_ڏيو(offsetof(بنية rand_pool_info, buf) + سائيز);
                إذا (!info)
                        أرجع -ENOMEM;

                info->entropy_count = سائيز * 8;
                info->buf_size = سائيز;
                يادگيري_نقل(info->buf, ٻج, سائيز);

                إذا (ioctl(fd, RNDADDENTROPY, info) < 0)
                        أرجع -errno;
        } وإلا {
                r = loop_write(fd, ٻج, سائيز);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        إذا (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                أرجع random_u64();
        إذا (m == 1)
                أرجع 0;

        remainder = UINT64_MAX % m;

        افعل {
                x = random_u64();
        } طالما (x >= UINT64_MAX - remainder);

        أرجع x % m;
}
