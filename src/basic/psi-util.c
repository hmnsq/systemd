/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "psi-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਪੂਰਨ read_resource_pressure(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਖੇਤਰ_filled = 0;
        ResourcePressure rp = {};
        ਅਚਲ ਅੱਖਰ *cline;
        ਅੱਖਰ *ਸ਼ਬਦ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_ਭਰਿਆ));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਅਚਲ ਅੱਖਰ *t = ASSERT_PTR(pressure_type_ਮੰਜ਼ਿਲ_string(type));

        r = fopen_unlocked(ਰਸਤਾ, "re", &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *l = ਨਿਰਾ;
                ਅੱਖਰ *w;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &l);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                w = ਪਹਿਲਾ_ਸ਼ਬਦ(l, t);
                ਜੇ (w) {
                        ਸਤਰ = TAKE_PTR(l);
                        cline = w;
                        ਰੋਕੋ;
                }
        }

        ਜੇ (!ਸਤਰ)
                ਵਾਪਸ -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        ਜਦੋਂ ((r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&cline, &ਸ਼ਬਦ, ਨਿਰਾ, 0)) > 0) {
                _cleanup_free_ ਅੱਖਰ *w = ਸ਼ਬਦ;
                ਅਚਲ ਅੱਖਰ *v;

                ਜੇ ((v = startswith(w, "avg10="))) {
                        ਜੇ (ਖੇਤਰ_filled & (1U << 0))
                                ਵਾਪਸ -EINVAL;

                        ਖੇਤਰ_filled |= 1U << 0;
                        r = parse_loadavg_fixed_ਬਿੰਦੂ(v, &rp.avg10);
                } ਨਹੀਂਤਾਂ ਜੇ ((v = startswith(w, "avg60="))) {
                        ਜੇ (ਖੇਤਰ_filled & (1U << 1))
                                ਵਾਪਸ -EINVAL;

                        ਖੇਤਰ_filled |= 1U << 1;
                        r = parse_loadavg_fixed_ਬਿੰਦੂ(v, &rp.avg60);
                } ਨਹੀਂਤਾਂ ਜੇ ((v = startswith(w, "avg300="))) {
                        ਜੇ (ਖੇਤਰ_filled & (1U << 2))
                                ਵਾਪਸ -EINVAL;

                        ਖੇਤਰ_filled |= 1U << 2;
                        r = parse_loadavg_fixed_ਬਿੰਦੂ(v, &rp.avg300);
                } ਨਹੀਂਤਾਂ ਜੇ ((v = startswith(w, "total="))) {
                        ਜੇ (ਖੇਤਰ_filled & (1U << 3))
                                ਵਾਪਸ -EINVAL;

                        ਖੇਤਰ_filled |= 1U << 3;
                        r = ਸੁਰੱਖਿਅਤ_atou64(v, &rp.ਕੁੱਲ);
                } ਨਹੀਂਤਾਂ
                        ਜਾਰੀ;

                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਖੇਤਰ_filled != 15U)
                ਵਾਪਸ -EINVAL;

        *ret = rp;
        ਵਾਪਸ 0;
}

ਅਚਲ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_ਸਮ੍ਰਿਤੀ] = {
                .name      = "memory",
                .env_watch = "MEMORY_PRESSURE_WATCH",
                .env_write = "MEMORY_PRESSURE_WRITE",
        },
        [PRESSURE_CPU] = {
                .name      = "cpu",
                .env_watch = "CPU_PRESSURE_WATCH",
                .env_write = "CPU_PRESSURE_WRITE",
        },
        [PRESSURE_IO] = {
                .name      = "io",
                .env_watch = "IO_PRESSURE_WATCH",
                .env_write = "IO_PRESSURE_WRITE",
        },
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ pressure_resource_ਸਾਰਣੀ[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_ਸਮ੍ਰਿਤੀ] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(pressure_resource, PressureResource);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ pressure_type_ਸਾਰਣੀ[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_ਭਰਿਆ] = "full",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(pressure_type, PressureType);

ਪੂਰਨ is_pressure_supported(ਖਾਲੀ) {
        ਸਥਿਰ thread_local ਪੂਰਨ cached = -1;
        ਪੂਰਨ r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ਜੇ (cached >= 0)
                ਵਾਪਸ cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ਨਿਰਾ, ਨਿਰਾ);
                ਜੇ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ਵਾਪਸ (cached = false);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ (cached = true);
}
