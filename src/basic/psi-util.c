/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <threads.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "psi-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

পূর্ণসংখ্যা read_resource_pressure(ধ্রুবক অক্ষর *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ অক্ষর *সারি = ফাঁকা;
        _cleanup_fclose_ FILE *f = ফাঁকা;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        ধ্রুবক অক্ষর *cline;
        অক্ষর *word;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        প্রতিজ্ঞা(ret);

        ধ্রুবক অক্ষর *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        যদি (r < 0)
                ফেরত r;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *l = ফাঁকা;
                অক্ষর *w;

                r = read_সারি(f, LONG_সারি_MAX, &l);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                w = first_word(l, t);
                যদি (w) {
                        সারি = TAKE_PTR(l);
                        cline = w;
                        বিরতি;
                }
        }

        যদি (!সারি)
                ফেরত -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        যখন ((r = extract_first_word(&cline, &word, ফাঁকা, 0)) > 0) {
                _cleanup_free_ অক্ষর *w = word;
                ধ্রুবক অক্ষর *v;

                যদি ((v = startswith(w, "avg10="))) {
                        যদি (field_filled & (1U << 0))
                                ফেরত -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } অন্যথা যদি ((v = startswith(w, "avg60="))) {
                        যদি (field_filled & (1U << 1))
                                ফেরত -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } অন্যথা যদি ((v = startswith(w, "avg300="))) {
                        যদি (field_filled & (1U << 2))
                                ফেরত -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } অন্যথা যদি ((v = startswith(w, "total="))) {
                        যদি (field_filled & (1U << 3))
                                ফেরত -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.যোগ_2);
                } অন্যথা
                        চালু;

                যদি (r < 0)
                        ফেরত r;
        }

        যদি (r < 0)
                ফেরত r;

        যদি (field_filled != 15U)
                ফেরত -EINVAL;

        *ret = rp;
        ফেরত 0;
}

ধ্রুবক PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

স্থির ধ্রুবক অক্ষর* ধ্রুবক pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

স্থির ধ্রুবক অক্ষর* ধ্রুবক pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

পূর্ণসংখ্যা is_pressure_supported(শূন্য) {
        স্থির thread_local পূর্ণসংখ্যা cached = -1;
        পূর্ণসংখ্যা r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        যদি (cached >= 0)
                ফেরত cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ফাঁকা, ফাঁকা);
                যদি (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ফেরত (cached = false);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত (cached = true);
}
