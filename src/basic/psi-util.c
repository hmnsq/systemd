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

صحيح read_resource_pressure(ثابت حرف *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ حرف *line = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        ثابت حرف *cline;
        حرف *word;
        صحيح r;

        أكد(path);
        أكد(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        أكد(ret);

        ثابت حرف *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        ܐܢ (r < 0)
                ܦܢܐ r;

        لكل (;;) {
                _cleanup_free_ حرف *l = عدم;
                حرف *w;

                r = read_line(f, LONG_LINE_MAX, &l);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;

                w = first_word(l, t);
                ܐܢ (w) {
                        line = TAKE_PTR(l);
                        cline = w;
                        اكسر;
                }
        }

        ܐܢ (!line)
                ܦܢܐ -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        ܟܡܐ ((r = extract_first_word(&cline, &word, عدم, 0)) > 0) {
                _cleanup_free_ حرف *w = word;
                ثابت حرف *v;

                ܐܢ ((v = startswith(w, "avg10="))) {
                        ܐܢ (field_filled & (1U << 0))
                                ܦܢܐ -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } ܐܚܪܢܐ ܐܢ ((v = startswith(w, "avg60="))) {
                        ܐܢ (field_filled & (1U << 1))
                                ܦܢܐ -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } ܐܚܪܢܐ ܐܢ ((v = startswith(w, "avg300="))) {
                        ܐܢ (field_filled & (1U << 2))
                                ܦܢܐ -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } ܐܚܪܢܐ ܐܢ ((v = startswith(w, "total="))) {
                        ܐܢ (field_filled & (1U << 3))
                                ܦܢܐ -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } ܐܚܪܢܐ
                        استمر;

                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (field_filled != 15U)
                ܦܢܐ -EINVAL;

        *ret = rp;
        ܦܢܐ 0;
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

ساكن ثابت حرف* ثابت pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

ساكن ثابت حرف* ثابت pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

صحيح is_pressure_supported(فراغ) {
        ساكن thread_local صحيح cached = -1;
        صحيح r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ܐܢ (cached >= 0)
                ܦܢܐ cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, عدم, عدم);
                ܐܢ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ܦܢܐ (cached = false);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܦܢܐ (cached = true);
}
