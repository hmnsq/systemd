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

صحیح read_resource_pressure(ثابت نویسه *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ نویسه *سطر = پوچ;
        _cleanup_fclose_ FILE *f = پوچ;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        ثابت نویسه *cline;
        نویسه *word;
        صحیح r;

        أكد(path);
        أكد(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        أكد(ret);

        ثابت نویسه *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        اگر (r < 0)
                بازگشت r;

        برای (;;) {
                _cleanup_free_ نویسه *l = پوچ;
                نویسه *w;

                r = read_سطر(f, LONG_سطر_MAX, &l);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                w = first_word(l, t);
                اگر (w) {
                        سطر = TAKE_PTR(l);
                        cline = w;
                        بشکن;
                }
        }

        اگر (!سطر)
                بازگشت -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        وقتی ((r = extract_first_word(&cline, &word, پوچ, 0)) > 0) {
                _cleanup_free_ نویسه *w = word;
                ثابت نویسه *v;

                اگر ((v = startswith(w, "avg10="))) {
                        اگر (field_filled & (1U << 0))
                                بازگشت -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } وگرنه اگر ((v = startswith(w, "avg60="))) {
                        اگر (field_filled & (1U << 1))
                                بازگشت -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } وگرنه اگر ((v = startswith(w, "avg300="))) {
                        اگر (field_filled & (1U << 2))
                                بازگشت -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } وگرنه اگر ((v = startswith(w, "total="))) {
                        اگر (field_filled & (1U << 3))
                                بازگشت -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } وگرنه
                        ادامه;

                اگر (r < 0)
                        بازگشت r;
        }

        اگر (r < 0)
                بازگشت r;

        اگر (field_filled != 15U)
                بازگشت -EINVAL;

        *ret = rp;
        بازگشت 0;
}

ثابت PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = {
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

ایستا ثابت نویسه* ثابت pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

ایستا ثابت نویسه* ثابت pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

صحیح is_pressure_supported(تهی) {
        ایستا thread_local صحیح cached = -1;
        صحیح r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        اگر (cached >= 0)
                بازگشت cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, پوچ, پوچ);
                اگر (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        بازگشت (cached = false);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت (cached = true);
}
