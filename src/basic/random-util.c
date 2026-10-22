/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <elf.h>
#समावेश <fcntl.h>
#समावेश <linux/random.h>
#समावेश <string.h>
#समावेश <sys/auxv.h>
#समावेश <sys/ioctl.h>
#समावेश <sys/random.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "io-util.h"
#समावेश "iovec-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "pidfd-util.h"
#समावेश "process-util.h"
#समावेश "random-util.h"
#समावेश "sha256.h"
#समावेश "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
अटल व्योम fallback_random_bytes(व्योम *p, माप_प्रकार n) {
        अटल thread_local uint64_t fallback_गणक = 0;
        काष्ठा {
                अक्षर label[32];
                uint64_t call_id, block_id;
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

        स_नकल(state.label, "systemd fallback random bytes v1", माप(state.label));
        स_नकल(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), माप(state.auxval));
        (व्योम) pidfd_get_inode_id_self_cached(&state.pidfdid);

        जबतक (n > 0) {
                काष्ठा sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, माप(state), &ctx);
                अगर (n < SHA256_DIGEST_आकार) {
                        uint8_t partial[SHA256_DIGEST_आकार];
                        sha256_finish_ctx(&ctx, partial);
                        स_नकल(p, partial, n);
                        अवरोध;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_आकार;
                n -= SHA256_DIGEST_आकार;
                ++state.block_id;
        }
}

व्योम random_bytes(व्योम *p, माप_प्रकार n) {
        निश्चित(p || n == 0);

        अगर (n == 0)
                वापस;

        क्रम (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                अगर (l <= 0)
                        अवरोध; /* Unexpected error. Give up and fallback to /dev/urandom. */

                अगर ((माप_प्रकार) l == n)
                        वापस; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ पूर्णांक fd = खोलो("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        अगर (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                वापस;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

पूर्णांक random_bytes_allocate_iovec(माप_प्रकार n, काष्ठा iovec *ret) {
        निश्चित(ret);

        व्योम *p = दो_स्मृति(MAX(n, 1U));
        अगर (!p)
                वापस -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        वापस 0;
}

पूर्णांक crypto_random_bytes(व्योम *p, माप_प्रकार n) {
        निश्चित(p || n == 0);

        अगर (n == 0)
                वापस 0;

        क्रम (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                अगर (l < 0)
                        वापस -त्रुटि_सं;
                अगर (l == 0)
                        वापस -EIO; /* Weird, should never happen. */

                अगर ((माप_प्रकार) l == n)
                        वापस 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

पूर्णांक crypto_random_bytes_allocate_iovec(माप_प्रकार n, काष्ठा iovec *ret) {
        _cleanup_free_ व्योम *p = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        p = दो_स्मृति(MAX(n, 1U));
        अगर (!p)
                वापस -ENOMEM;

        r = crypto_random_bytes(p, n);
        अगर (r < 0)
                वापस r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        वापस 0;
}

माप_प्रकार random_pool_आकार(व्योम) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        /* Read pool size, if possible */
        r = read_one_पंक्ति_file("/proc/sys/kernel/random/poolsize", &s);
        अगर (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        अन्यथा {
                अचिन्हित sz;

                r = safe_atou(s, &sz);
                अगर (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                अन्यथा
                        /* poolsize is in bits on 2.6, but we want bytes */
                        वापस CLAMP(sz / 8, RANDOM_POOL_आकार_MIN, RANDOM_POOL_आकार_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        वापस RANDOM_POOL_आकार_MIN;
}

पूर्णांक random_write_entropy(पूर्णांक fd, स्थिर व्योम *seed, माप_प्रकार आकार, bool credit) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        पूर्णांक r;

        निश्चित(seed || आकार == 0);

        अगर (आकार == 0)
                वापस 0;

        अगर (fd < 0) {
                opened_fd = खोलो("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                अगर (opened_fd < 0)
                        वापस -त्रुटि_सं;

                fd = opened_fd;
        }

        अगर (credit) {
                _cleanup_free_ काष्ठा rand_pool_info *info = शून्य;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                अगर (आकार > पूर्णांक_उच्च / 8)
                        वापस -EOVERFLOW;

                info = दो_स्मृति(दुरत्व(काष्ठा rand_pool_info, buf) + आकार);
                अगर (!info)
                        वापस -ENOMEM;

                info->entropy_count = आकार * 8;
                info->buf_size = आकार;
                स_नकल(info->buf, seed, आकार);

                अगर (ioctl(fd, RNDADDENTROPY, info) < 0)
                        वापस -त्रुटि_सं;
        } अन्यथा {
                r = loop_write(fd, seed, आकार);
                अगर (r < 0)
                        वापस r;
        }

        वापस 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        अगर (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                वापस random_u64();
        अगर (m == 1)
                वापस 0;

        remainder = UINT64_MAX % m;

        करो {
                x = random_u64();
        } जबतक (x >= UINT64_MAX - remainder);

        वापस x % m;
}
