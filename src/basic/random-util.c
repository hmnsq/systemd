/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <elf.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <linux/random.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/auxv.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/random.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "pidfd-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "random-util.h"
#అన్తర్భావ "sha256.h"
#అన్తర్భావ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
స్థిర శూన్య fallback_random_బైట్లు(శూన్య *p, size_t n) {
        స్థిర thread_local uint64_t fallback_counter = 0;
        నిర్మాణం {
                అక్షరం label[32];
                uint64_t call_id, ఖండం_id;
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

        జ్ఞాపకం_నకలు(state.label, "systemd fallback random bytes v1", పరిమాణం(state.label));
        జ్ఞాపకం_నకలు(state.auxval, ULONG_గమ్యం_PTR(getauxval(AT_RANDOM)), పరిమాణం(state.auxval));
        (శూన్య) pidfd_get_inode_id_self_cached(&state.pidfdid);

        అయితే (n > 0) {
                నిర్మాణం sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_బైట్లు(&state, పరిమాణం(state), &ctx);
                ఒకవేళ (n < SHA256_DIGEST_కొలత) {
                        uint8_t partial[SHA256_DIGEST_కొలత];
                        sha256_ముగింపు_ctx(&ctx, partial);
                        జ్ఞాపకం_నకలు(p, partial, n);
                        ఆపు;
                }
                sha256_ముగింపు_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_కొలత;
                n -= SHA256_DIGEST_కొలత;
                ++state.ఖండం_id;
        }
}

శూన్య random_బైట్లు(శూన్య *p, size_t n) {
        ప్రతిజ్ఞా(p || n == 0);

        ఒకవేళ (n == 0)
                తిరిగి;

        కొరకు (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                ఒకవేళ (l <= 0)
                        ఆపు; /* Unexpected error. Give up and fallback to /dev/urandom. */

                ఒకవేళ ((size_t) l == n)
                        తిరిగి; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ పూర్ణాంకం fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ఒకవేళ (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                తిరిగి;

        /* This is a terrible fallback. Oh well. */
        fallback_random_బైట్లు(p, n);
}

పూర్ణాంకం random_బైట్లు_allocate_iovec(size_t n, నిర్మాణం iovec *ret) {
        ప్రతిజ్ఞా(ret);

        శూన్య *p = జ్ఞాపకం_ఇవ్వు(MAX(n, 1U));
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        random_బైట్లు(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        తిరిగి 0;
}

పూర్ణాంకం crypto_random_బైట్లు(శూన్య *p, size_t n) {
        ప్రతిజ్ఞా(p || n == 0);

        ఒకవేళ (n == 0)
                తిరిగి 0;

        కొరకు (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                ఒకవేళ (l < 0)
                        తిరిగి -errno;
                ఒకవేళ (l == 0)
                        తిరిగి -EIO; /* Weird, should never happen. */

                ఒకవేళ ((size_t) l == n)
                        తిరిగి 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

పూర్ణాంకం crypto_random_బైట్లు_allocate_iovec(size_t n, నిర్మాణం iovec *ret) {
        _cleanup_free_ శూన్య *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        p = జ్ఞాపకం_ఇవ్వు(MAX(n, 1U));
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        r = crypto_random_బైట్లు(p, n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        తిరిగి 0;
}

size_t random_pool_కొలత(శూన్య) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        /* Read pool size, if possible */
        r = read_one_పంక్తి_file("/proc/sys/kernel/random/poolsize", &s);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        లేకపోతే {
                గుర్తులేని sz;

                r = సురక్షిత_atou(s, &sz);
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                లేకపోతే
                        /* poolsize is in bits on 2.6, but we want bytes */
                        తిరిగి CLAMP(sz / 8, RANDOM_POOL_కొలత_MIN, RANDOM_POOL_కొలత_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        తిరిగి RANDOM_POOL_కొలత_MIN;
}

పూర్ణాంకం random_write_entropy(పూర్ణాంకం fd, స్థిరరాశి శూన్య *విత్తనం, size_t కొలత, bool credit) {
        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(విత్తనం || కొలత == 0);

        ఒకవేళ (కొలత == 0)
                తిరిగి 0;

        ఒకవేళ (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                ఒకవేళ (opened_fd < 0)
                        తిరిగి -errno;

                fd = opened_fd;
        }

        ఒకవేళ (credit) {
                _cleanup_free_ నిర్మాణం rand_pool_info *info = ఖాళీ;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                ఒకవేళ (కొలత > గరిష్ఠ_పూర్ణాంకం / 8)
                        తిరిగి -EOVERFLOW;

                info = జ్ఞాపకం_ఇవ్వు(offsetof(నిర్మాణం rand_pool_info, buf) + కొలత);
                ఒకవేళ (!info)
                        తిరిగి -ENOMEM;

                info->entropy_count = కొలత * 8;
                info->buf_size = కొలత;
                జ్ఞాపకం_నకలు(info->buf, విత్తనం, కొలత);

                ఒకవేళ (ioctl(fd, RNDADDENTROPY, info) < 0)
                        తిరిగి -errno;
        } లేకపోతే {
                r = loop_write(fd, విత్తనం, కొలత);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        ఒకవేళ (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                తిరిగి random_u64();
        ఒకవేళ (m == 1)
                తిరిగి 0;

        remainder = UINT64_MAX % m;

        చేయి {
                x = random_u64();
        } అయితే (x >= UINT64_MAX - remainder);

        తిరిగి x % m;
}
