/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "psi-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_resource_pressure(ଅଚଳ ଅକ୍ଷର *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        _cleanup_fclose_ FILE *f = ଖାଲି;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        ଅଚଳ ଅକ୍ଷର *cline;
        ଅକ୍ଷର *word;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        ପ୍ରତିଜ୍ଞା(ret);

        ଅଚଳ ଅକ୍ଷର *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *l = ଖାଲି;
                ଅକ୍ଷର *w;

                r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &l);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                w = first_word(l, t);
                ଯଦି (w) {
                        ଧାଡ଼ି = TAKE_PTR(l);
                        cline = w;
                        ଭାଙ୍ଗ;
                }
        }

        ଯଦି (!ଧାଡ଼ି)
                ଫେରନ୍ତୁ -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        ଯେତେବେଳେ ((r = extract_first_word(&cline, &word, ଖାଲି, 0)) > 0) {
                _cleanup_free_ ଅକ୍ଷର *w = word;
                ଅଚଳ ଅକ୍ଷର *v;

                ଯଦି ((v = startswith(w, "avg10="))) {
                        ଯଦି (field_filled & (1U << 0))
                                ଫେରନ୍ତୁ -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } ଅନ୍ୟଥା ଯଦି ((v = startswith(w, "avg60="))) {
                        ଯଦି (field_filled & (1U << 1))
                                ଫେରନ୍ତୁ -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } ଅନ୍ୟଥା ଯଦି ((v = startswith(w, "avg300="))) {
                        ଯଦି (field_filled & (1U << 2))
                                ଫେରନ୍ତୁ -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } ଅନ୍ୟଥା ଯଦି ((v = startswith(w, "total="))) {
                        ଯଦି (field_filled & (1U << 3))
                                ଫେରନ୍ତୁ -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } ଅନ୍ୟଥା
                        ଜାରି;

                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (field_filled != 15U)
                ଫେରନ୍ତୁ -EINVAL;

        *ret = rp;
        ଫେରନ୍ତୁ 0;
}

ଅଚଳ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_pressure_supported(ଶୂନ୍ୟ) {
        ସ୍ଥିର thread_local ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached = -1;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ଯଦି (cached >= 0)
                ଫେରନ୍ତୁ cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ଖାଲି, ଖାଲି);
                ଯଦି (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ଫେରନ୍ତୁ (cached = false);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ (cached = true);
}
