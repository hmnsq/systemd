/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <threads.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "parse-util.h"
#شامل "psi-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"

صحیح read_resource_pressure(ثابت حرف *راستہ, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ حرف *سطر = تہی;
        _cleanup_fclose_ FILE *f = تہی;
        غیرعلامتی میدان_filled = 0;
        ResourcePressure rp = {};
        ثابت حرف *cline;
        حرف *لفظ;
        صحیح r;

        أكد(راستہ);
        أكد(اندر_سیٹ(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_بھرا));
        أكد(ret);

        ثابت حرف *t = ASSERT_PTR(pressure_type_مقصد_string(type));

        r = fopen_unlocked(راستہ, "re", &f);
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                _cleanup_free_ حرف *l = تہی;
                حرف *w;

                r = read_سطر(f, LONG_سطر_MAX, &l);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                w = پہلا_لفظ(l, t);
                اگر (w) {
                        سطر = TAKE_PTR(l);
                        cline = w;
                        توڑو;
                }
        }

        اگر (!سطر)
                واپس -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        جب ((r = نکالو_پہلا_لفظ(&cline, &لفظ, تہی, 0)) > 0) {
                _cleanup_free_ حرف *w = لفظ;
                ثابت حرف *v;

                اگر ((v = startswith(w, "avg10="))) {
                        اگر (میدان_filled & (1U << 0))
                                واپس -EINVAL;

                        میدان_filled |= 1U << 0;
                        r = parse_loadavg_fixed_نقطہ(v, &rp.avg10);
                } ورنہ اگر ((v = startswith(w, "avg60="))) {
                        اگر (میدان_filled & (1U << 1))
                                واپس -EINVAL;

                        میدان_filled |= 1U << 1;
                        r = parse_loadavg_fixed_نقطہ(v, &rp.avg60);
                } ورنہ اگر ((v = startswith(w, "avg300="))) {
                        اگر (میدان_filled & (1U << 2))
                                واپس -EINVAL;

                        میدان_filled |= 1U << 2;
                        r = parse_loadavg_fixed_نقطہ(v, &rp.avg300);
                } ورنہ اگر ((v = startswith(w, "total="))) {
                        اگر (میدان_filled & (1U << 3))
                                واپس -EINVAL;

                        میدان_filled |= 1U << 3;
                        r = محفوظ_atou64(v, &rp.کل);
                } ورنہ
                        جاری;

                اگر (r < 0)
                        واپس r;
        }

        اگر (r < 0)
                واپس r;

        اگر (میدان_filled != 15U)
                واپس -EINVAL;

        *ret = rp;
        واپس 0;
}

ثابت PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_حافظہ] = {
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

ساکن ثابت حرف* ثابت pressure_resource_جدول[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_حافظہ] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_جدول_LOOKUP(pressure_resource, PressureResource);

ساکن ثابت حرف* ثابت pressure_type_جدول[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_بھرا] = "full",
};

DEFINE_STRING_جدول_LOOKUP(pressure_type, PressureType);

صحیح is_pressure_supported(خالی) {
        ساکن thread_local صحیح cached = -1;
        صحیح r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        اگر (cached >= 0)
                واپس cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, تہی, تہی);
                اگر (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        واپس (cached = false);
                اگر (r < 0)
                        واپس r;
        }

        واپس (cached = true);
}
