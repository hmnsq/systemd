/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <threads.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "parse-util.h"
#ضمن "psi-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"

صحيح read_resource_pressure(ثابت حرف *مسیر, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ حرف *سطر = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        بیعلامت فیلد_filled = 0;
        ResourcePressure rp = {};
        ثابت حرف *cline;
        حرف *کلمه;
        صحيح r;

        أكد(مسیر);
        أكد(درون_مجموعه(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_پر));
        أكد(ret);

        ثابت حرف *t = ASSERT_PTR(pressure_type_مقصد_string(type));

        r = fopen_unlocked(مسیر, "re", &f);
        إذا (r < 0)
                أرجع r;

        لكل (;;) {
                _cleanup_free_ حرف *l = عدم;
                حرف *w;

                r = read_سطر(f, LONG_سطر_MAX, &l);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                w = اولی_کلمه(l, t);
                إذا (w) {
                        سطر = TAKE_PTR(l);
                        cline = w;
                        اكسر;
                }
        }

        إذا (!سطر)
                أرجع -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        طالما ((r = استخراج_اولی_کلمه(&cline, &کلمه, عدم, 0)) > 0) {
                _cleanup_free_ حرف *w = کلمه;
                ثابت حرف *v;

                إذا ((v = startswith(w, "avg10="))) {
                        إذا (فیلد_filled & (1U << 0))
                                أرجع -EINVAL;

                        فیلد_filled |= 1U << 0;
                        r = parse_loadavg_fixed_نقطه(v, &rp.avg10);
                } وإلا إذا ((v = startswith(w, "avg60="))) {
                        إذا (فیلد_filled & (1U << 1))
                                أرجع -EINVAL;

                        فیلد_filled |= 1U << 1;
                        r = parse_loadavg_fixed_نقطه(v, &rp.avg60);
                } وإلا إذا ((v = startswith(w, "avg300="))) {
                        إذا (فیلد_filled & (1U << 2))
                                أرجع -EINVAL;

                        فیلد_filled |= 1U << 2;
                        r = parse_loadavg_fixed_نقطه(v, &rp.avg300);
                } وإلا إذا ((v = startswith(w, "total="))) {
                        إذا (فیلد_filled & (1U << 3))
                                أرجع -EINVAL;

                        فیلد_filled |= 1U << 3;
                        r = امن_atou64(v, &rp.مجموع);
                } وإلا
                        استمر;

                إذا (r < 0)
                        أرجع r;
        }

        إذا (r < 0)
                أرجع r;

        إذا (فیلد_filled != 15U)
                أرجع -EINVAL;

        *ret = rp;
        أرجع 0;
}

ثابت PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_حافظه] = {
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

ساكن ثابت حرف* ثابت pressure_resource_جدول[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_حافظه] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_جدول_LOOKUP(pressure_resource, PressureResource);

ساكن ثابت حرف* ثابت pressure_type_جدول[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_پر] = "full",
};

DEFINE_STRING_جدول_LOOKUP(pressure_type, PressureType);

صحيح is_pressure_supported(فراغ) {
        ساكن thread_local صحيح cached = -1;
        صحيح r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        إذا (cached >= 0)
                أرجع cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, عدم, عدم);
                إذا (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        أرجع (cached = false);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع (cached = true);
}
