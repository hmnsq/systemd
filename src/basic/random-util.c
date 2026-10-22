/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <elf.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <linux/random.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/auxv.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/random.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "pidfd-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "random-util.h"
#अन्तर्भाव "sha256.h"
#अन्तर्भाव "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
स्थिर शून्य fallback_random_अष्टकानि(शून्य *p, size_t n) {
        स्थिर thread_local uint64_t fallback_गणक = 0;
        संरचना {
                वर्ण label[32];
                uint64_t call_id, खण्डम्_id;
                usec_t stamp_mono, stamp_real;
                pid_t pid, tid;
                uint64_t pidfdid;
                uint8_t auxval[16];
        } state = {
                /* Arbitrary domain separation to prevent other usage of AT_RANDOM from clashing. */
                .call_id = fallback_गणक++,
                .stamp_mono = now(CLOCK_MONOTONIC),
                .stamp_real = now(CLOCK_REALTIME),
                .pid = getpid_cached(),
                .tid = gettid(),
        };

        स्मृतिं_प्रतिलिख(state.label, "systemd fallback random bytes v1", परिमाण(state.label));
        स्मृतिं_प्रतिलिख(state.auxval, ULONG_गन्तव्यम्_PTR(getauxval(AT_RANDOM)), परिमाण(state.auxval));
        (शून्य) pidfd_get_inode_id_self_cached(&state.pidfdid);

        यावत् (n > 0) {
                संरचना sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_अष्टकानि(&state, परिमाण(state), &ctx);
                यदि (n < SHA256_DIGEST_आकार) {
                        uint8_t partial[SHA256_DIGEST_आकार];
                        sha256_finish_ctx(&ctx, partial);
                        स्मृतिं_प्रतिलिख(p, partial, n);
                        भङ्ग;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_आकार;
                n -= SHA256_DIGEST_आकार;
                ++state.खण्डम्_id;
        }
}

शून्य random_अष्टकानि(शून्य *p, size_t n) {
        प्रतिज्ञा(p || n == 0);

        यदि (n == 0)
                प्रत्यागम;

        प्रति (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                यदि (l <= 0)
                        भङ्ग; /* Unexpected error. Give up and fallback to /dev/urandom. */

                यदि ((size_t) l == n)
                        प्रत्यागम; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ पूर्णाङ्क fd = उद्घाटय("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        यदि (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                प्रत्यागम;

        /* This is a terrible fallback. Oh well. */
        fallback_random_अष्टकानि(p, n);
}

पूर्णाङ्क random_अष्टकानि_allocate_iovec(size_t n, संरचना iovec *ret) {
        प्रतिज्ञा(ret);

        शून्य *p = स्मृतिं_देहि(MAX(n, 1U));
        यदि (!p)
                प्रत्यागम -ENOMEM;

        random_अष्टकानि(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        प्रत्यागम 0;
}

पूर्णाङ्क crypto_random_अष्टकानि(शून्य *p, size_t n) {
        प्रतिज्ञा(p || n == 0);

        यदि (n == 0)
                प्रत्यागम 0;

        प्रति (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                यदि (l < 0)
                        प्रत्यागम -errno;
                यदि (l == 0)
                        प्रत्यागम -EIO; /* Weird, should never happen. */

                यदि ((size_t) l == n)
                        प्रत्यागम 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

पूर्णाङ्क crypto_random_अष्टकानि_allocate_iovec(size_t n, संरचना iovec *ret) {
        _cleanup_free_ शून्य *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        p = स्मृतिं_देहि(MAX(n, 1U));
        यदि (!p)
                प्रत्यागम -ENOMEM;

        r = crypto_random_अष्टकानि(p, n);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        प्रत्यागम 0;
}

size_t random_pool_आकार(शून्य) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        /* Read pool size, if possible */
        r = read_one_पङ्क्तिः_file("/proc/sys/kernel/random/poolsize", &s);
        यदि (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        अन्यथा {
                अचिह्नित sz;

                r = safe_atou(s, &sz);
                यदि (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                अन्यथा
                        /* poolsize is in bits on 2.6, but we want bytes */
                        प्रत्यागम CLAMP(sz / 8, RANDOM_POOL_आकार_MIN, RANDOM_POOL_आकार_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        प्रत्यागम RANDOM_POOL_आकार_MIN;
}

पूर्णाङ्क random_write_entropy(पूर्णाङ्क fd, ध्रुव शून्य *बीजम्, size_t आकार, bool credit) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(बीजम् || आकार == 0);

        यदि (आकार == 0)
                प्रत्यागम 0;

        यदि (fd < 0) {
                opened_fd = उद्घाटय("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                यदि (opened_fd < 0)
                        प्रत्यागम -errno;

                fd = opened_fd;
        }

        यदि (credit) {
                _cleanup_free_ संरचना rand_pool_info *info = रिक्त;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                यदि (आकार > अधिकतम_पूर्णाङ्कः / 8)
                        प्रत्यागम -EOVERFLOW;

                info = स्मृतिं_देहि(offsetof(संरचना rand_pool_info, buf) + आकार);
                यदि (!info)
                        प्रत्यागम -ENOMEM;

                info->entropy_count = आकार * 8;
                info->buf_size = आकार;
                स्मृतिं_प्रतिलिख(info->buf, बीजम्, आकार);

                यदि (ioctl(fd, RNDADDENTROPY, info) < 0)
                        प्रत्यागम -errno;
        } अन्यथा {
                r = loop_write(fd, बीजम्, आकार);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        यदि (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                प्रत्यागम random_u64();
        यदि (m == 1)
                प्रत्यागम 0;

        remainder = UINT64_MAX % m;

        कुरु {
                x = random_u64();
        } यावत् (x >= UINT64_MAX - remainder);

        प्रत्यागम x % m;
}
