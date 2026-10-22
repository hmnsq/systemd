/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <threads.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "psi-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

முழு read_resource_pressure(மாறிலி எழுத்து *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        _cleanup_fclose_ FILE *f = வெற்று;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        மாறிலி எழுத்து *cline;
        எழுத்து *word;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        ப்ரதிஜ்ஞா(ret);

        மாறிலி எழுத்து *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *l = வெற்று;
                எழுத்து *w;

                r = read_வரி(f, LONG_வரி_MAX, &l);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                w = first_word(l, t);
                எனில் (w) {
                        வரி = TAKE_PTR(l);
                        cline = w;
                        நிறுத்து;
                }
        }

        எனில் (!வரி)
                திரும்பு -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        போது ((r = extract_first_word(&cline, &word, வெற்று, 0)) > 0) {
                _cleanup_free_ எழுத்து *w = word;
                மாறிலி எழுத்து *v;

                எனில் ((v = startswith(w, "avg10="))) {
                        எனில் (field_filled & (1U << 0))
                                திரும்பு -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } இல்லையெனில் எனில் ((v = startswith(w, "avg60="))) {
                        எனில் (field_filled & (1U << 1))
                                திரும்பு -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } இல்லையெனில் எனில் ((v = startswith(w, "avg300="))) {
                        எனில் (field_filled & (1U << 2))
                                திரும்பு -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } இல்லையெனில் எனில் ((v = startswith(w, "total="))) {
                        எனில் (field_filled & (1U << 3))
                                திரும்பு -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } இல்லையெனில்
                        தொடர்;

                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (r < 0)
                திரும்பு r;

        எனில் (field_filled != 15U)
                திரும்பு -EINVAL;

        *ret = rp;
        திரும்பு 0;
}

மாறிலி PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

நிலையான மாறிலி எழுத்து* மாறிலி pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

நிலையான மாறிலி எழுத்து* மாறிலி pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

முழு is_pressure_supported(வெறுமை) {
        நிலையான thread_local முழு cached = -1;
        முழு r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        எனில் (cached >= 0)
                திரும்பு cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, வெற்று, வெற்று);
                எனில் (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        திரும்பு (cached = false);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு (cached = true);
}
