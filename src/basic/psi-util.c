/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "psi-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಪೂರ್ಣಾಙ್ಕ read_resource_pressure(ಧ್ರುವ ವರ್ಣ *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        ಧ್ರುವ ವರ್ಣ *cline;
        ವರ್ಣ *word;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಧ್ರುವ ವರ್ಣ *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *l = ರಿಕ್ತ;
                ವರ್ಣ *w;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &l);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                w = first_word(l, t);
                ಯದಿ (w) {
                        ಸಾಲು = TAKE_PTR(l);
                        cline = w;
                        ಭಙ್ಗ;
                }
        }

        ಯದಿ (!ಸಾಲು)
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        ಯಾವತ್ ((r = extract_first_word(&cline, &word, ರಿಕ್ತ, 0)) > 0) {
                _cleanup_free_ ವರ್ಣ *w = word;
                ಧ್ರುವ ವರ್ಣ *v;

                ಯದಿ ((v = startswith(w, "avg10="))) {
                        ಯದಿ (field_filled & (1U << 0))
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } ಅನ್ಯಥಾ ಯದಿ ((v = startswith(w, "avg60="))) {
                        ಯದಿ (field_filled & (1U << 1))
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } ಅನ್ಯಥಾ ಯದಿ ((v = startswith(w, "avg300="))) {
                        ಯದಿ (field_filled & (1U << 2))
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } ಅನ್ಯಥಾ ಯದಿ ((v = startswith(w, "total="))) {
                        ಯದಿ (field_filled & (1U << 3))
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } ಅನ್ಯಥಾ
                        ಅನುವರ್ತ;

                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (field_filled != 15U)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = rp;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

ಪೂರ್ಣಾಙ್ಕ is_pressure_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಙ್ಕ cached = -1;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ಯದಿ (cached >= 0)
                ಪ್ರತ್ಯಾಗಮ cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ರಿಕ್ತ, ರಿಕ್ತ);
                ಯದಿ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಪ್ರತ್ಯಾಗಮ (cached = false);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ (cached = true);
}
