/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <elf.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <linux/random.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/auxv.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/random.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "pidfd-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "random-util.h"
#അന്തര്ഭാവ "sha256.h"
#അന്തര്ഭാവ "time-util.h"

/* This is a "best effort" kind of thing, but has no real security value. So, this should only be used by
 * random_bytes(), which is not meant for crypto. This could be made better, but we're *not* trying to roll a
 * userspace prng here, or even have forward secrecy, but rather just do the shortest thing that is at least
 * better than libc rand(). */
സ്ഥിതം ശൂന്യം fallback_random_bytes(ശൂന്യം *p, size_t n) {
        സ്ഥിതം thread_local uint64_t fallback_counter = 0;
        ഘടന {
                അക്ഷരം label[32];
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

        memcpy(state.label, "systemd fallback random bytes v1", വലുപ്പം(state.label));
        memcpy(state.auxval, ULONG_TO_PTR(getauxval(AT_RANDOM)), വലുപ്പം(state.auxval));
        (ശൂന്യം) pidfd_get_inode_id_self_cached(&state.pidfdid);

        ആകുമ്പോൾ (n > 0) {
                ഘടന sha256_ctx ctx;

                sha256_init_ctx(&ctx);
                sha256_process_bytes(&state, വലുപ്പം(state), &ctx);
                എങ്കിൽ (n < SHA256_DIGEST_SIZE) {
                        uint8_t partial[SHA256_DIGEST_SIZE];
                        sha256_finish_ctx(&ctx, partial);
                        memcpy(p, partial, n);
                        നിർത്തുക;
                }
                sha256_finish_ctx(&ctx, p);
                p = (uint8_t *) p + SHA256_DIGEST_SIZE;
                n -= SHA256_DIGEST_SIZE;
                ++state.block_id;
        }
}

ശൂന്യം random_bytes(ശൂന്യം *p, size_t n) {
        പ്രതിജ്ഞാ(p || n == 0);

        എങ്കിൽ (n == 0)
                തിരികെ;

        വേണ്ടി (;;) {
                ssize_t l;

                l = getrandom(p, n, GRND_INSECURE);
                എങ്കിൽ (l <= 0)
                        നിർത്തുക; /* Unexpected error. Give up and fallback to /dev/urandom. */

                എങ്കിൽ ((size_t) l == n)
                        തിരികെ; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = open("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        എങ്കിൽ (fd >= 0 && loop_read_exact(fd, p, n, false) >= 0)
                തിരികെ;

        /* This is a terrible fallback. Oh well. */
        fallback_random_bytes(p, n);
}

പൂർണ്ണസംഖ്യ random_bytes_allocate_iovec(size_t n, ഘടന iovec *ret) {
        പ്രതിജ്ഞാ(ret);

        ശൂന്യം *p = malloc(MAX(n, 1U));
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        random_bytes(p, n);

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ crypto_random_bytes(ശൂന്യം *p, size_t n) {
        പ്രതിജ്ഞാ(p || n == 0);

        എങ്കിൽ (n == 0)
                തിരികെ 0;

        വേണ്ടി (;;) {
                ssize_t l;

                l = getrandom(p, n, 0);
                എങ്കിൽ (l < 0)
                        തിരികെ -errno;
                എങ്കിൽ (l == 0)
                        തിരികെ -EIO; /* Weird, should never happen. */

                എങ്കിൽ ((size_t) l == n)
                        തിരികെ 0; /* Done reading, success. */

                p = (uint8_t *) p + l;
                n -= l;
                /* Interrupted by a signal; keep going. */
        }
}

പൂർണ്ണസംഖ്യ crypto_random_bytes_allocate_iovec(size_t n, ഘടന iovec *ret) {
        _cleanup_free_ ശൂന്യം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        p = malloc(MAX(n, 1U));
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        r = crypto_random_bytes(p, n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = IOVEC_MAKE(TAKE_PTR(p), n);
        തിരികെ 0;
}

size_t random_pool_size(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Read pool size, if possible */
        r = read_one_വരി_file("/proc/sys/kernel/random/poolsize", &s);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to read pool size from kernel: %m");
        അല്ലെങ്കിൽ {
                unsigned sz;

                r = safe_atou(s, &sz);
                എങ്കിൽ (r < 0)
                        log_debug_errno(r, "Failed to parse pool size: %s", s);
                അല്ലെങ്കിൽ
                        /* poolsize is in bits on 2.6, but we want bytes */
                        തിരികെ CLAMP(sz / 8, RANDOM_POOL_SIZE_MIN, RANDOM_POOL_SIZE_MAX);
        }

        /* Use the minimum as default, if we can't retrieve the correct value */
        തിരികെ RANDOM_POOL_SIZE_MIN;
}

പൂർണ്ണസംഖ്യ random_write_entropy(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *seed, size_t size, bool credit) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(seed || size == 0);

        എങ്കിൽ (size == 0)
                തിരികെ 0;

        എങ്കിൽ (fd < 0) {
                opened_fd = open("/dev/urandom", O_WRONLY|O_CLOEXEC|O_NOCTTY);
                എങ്കിൽ (opened_fd < 0)
                        തിരികെ -errno;

                fd = opened_fd;
        }

        എങ്കിൽ (credit) {
                _cleanup_free_ ഘടന rand_pool_info *info = ശൂന്യ;

                /* The kernel API only accepts "int" as entropy count (which is in bits), let's avoid any
                 * chance for confusion here. */
                എങ്കിൽ (size > INT_MAX / 8)
                        തിരികെ -EOVERFLOW;

                info = malloc(offsetof(ഘടന rand_pool_info, buf) + size);
                എങ്കിൽ (!info)
                        തിരികെ -ENOMEM;

                info->entropy_count = size * 8;
                info->buf_size = size;
                memcpy(info->buf, seed, size);

                എങ്കിൽ (ioctl(fd, RNDADDENTROPY, info) < 0)
                        തിരികെ -errno;
        } അല്ലെങ്കിൽ {
                r = loop_write(fd, seed, size);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 1;
}

uint64_t random_u64_range(uint64_t m) {
        uint64_t x, remainder;

        /* Generates a random number in the range 0…m-1, unbiased. (Java's algorithm) */

        എങ്കിൽ (m == 0) /* Let's take m == 0 as special case to return an integer from the full range */
                തിരികെ random_u64();
        എങ്കിൽ (m == 1)
                തിരികെ 0;

        remainder = UINT64_MAX % m;

        ചെയ്യുക {
                x = random_u64();
        } ആകുമ്പോൾ (x >= UINT64_MAX - remainder);

        തിരികെ x % m;
}
