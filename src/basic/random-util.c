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
स्थिर शून्य fallback_random_bytes(शून्य *p, माप_प्रकार n) {
        स्थिर thread_local uint64_t fallback_counter = 0;
        संरचना {
                वर्ण label[32];
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

        स_नकल(state.label, "systemd fallback random bytes v1", परिमाण(state.label));
        स_नकल(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), परिमाण(state.auxval));
        (शून्य) pidfd_get_inode_id_self_cached(&state.pidfdid);

        जबसम्म (n > 0) {
                संरचना sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, परिमाण(state), &ctx);
                यदि (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        स_नकल(p, partial, n);
                        रोक;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

शून्य random_bytes(शून्य *p, माप_प्रकार n) {
        प्रतिज्ञा(p || n == 0);

        यदि (n == 0)
                फर्काऊ;

        लागि (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                यदि (l <= 0)
                        रोक; /* Unexpected error. Give up and fallback to /dev/urandom. */

                यदि ((माप_प्रकार) l == n)
                        फर्काऊ; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ पूर्णाङ्क fd = खोलो("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        यदि (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                फर्काऊ;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

पूर्णाङ्क random_bytes_allocate_iovec(माप_प्रकार n, संरचना iovec *ret) {
        प्रतिज्ञा(ret);

        शून्य *p = दो_स्मृति(MAX(n, 1U));
        यदि (!p)
                फर्काऊ -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        फर्काऊ 0;
}

पूर्णाङ्क crypto_random_bytes(शून्य *p, माप_प्रकार n) {
        प्रतिज्ञा(p || n == 0);

        यदि (n == 0)
                फर्काऊ 0;

        लागि (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                यदि (l < 0)
                        फर्काऊ -त्रुटि_सं;
                यदि (l == 0)
                        फर्काऊ -EIO; /* Weird, should never happen. */

                यदि ((माप_प्रकार) l == n)
                        फर्काऊ 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

पूर्णाङ्क crypto_random_bytes_allocate_iovec(माप_प्रकार n, संरचना iovec *ret) {
        _cleanup_free_ शून्य *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        p = दो_स्मृति(MAX(n, 1U));
        यदि (!p)
                फर्काऊ -ENOMEM;

        r = crypto_random_bytes(p, n);
        यदि (r < 0)
                फर्काऊ r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        फर्काऊ 0;
}

माप_प्रकार random_pool_size(शून्य) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        /* Read pool size, if possible */
        r = read_one_पङ्क्ति_file("/proc/sys/kernel/random/poolsize", &s);
        यदि (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        अन्यथा {
                अचिन्हित sz;

                r = safe_atou(s, &sz);
                यदि (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                अन्यथा
                        /* poolsize is in bits on 2.6, but we want bytes */
                        फर्काऊ CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        फर्काऊ RANDOM_POOL_SIZE_MIN;
}

पूर्णाङ्क random_write_entropy(पूर्णाङ्क fd, अचर शून्य *seed, माप_प्रकार size, bool credit) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(seed || size == 0);

        यदि (size == 0)
                फर्काऊ 0;

        यदि (fd < 0) {
                opened_fd = खोलो("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                यदि (opened_fd < 0)
                        फर्काऊ -त्रुटि_सं;

                fd = opened_fd;
        }

        यदि (credit) {
                _cleanup_free_ संरचना rand_pool_info *info = रिक्त;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                यदि (size > पूर्णांक_उच्च / 8)
                        फर्काऊ -EOVERFLOW;

                info = दो_स्मृति(दुरत्व(संरचना rand_pool_info, buf) + size);
                यदि (!info)
                        फर्काऊ -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                स_नकल(info->buf, seed, size);

                यदि (ioctl(fd, RNDADDENTROPY, info) < 0)
                        फर्काऊ -त्रुटि_सं;
        } अन्यथा {
                r = loop_write(fd, seed, size);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        यदि (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                फर्काऊ random_u64();
        यदि (m == 1)
                फर्काऊ 0;

        remainder = UINT64_MAX % m;

        गर {
                x = random_u64();
        } जबसम्म (x >= UINT64_MAX - remainder);

        फर्काऊ x % m;
}
