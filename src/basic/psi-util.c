/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <threads.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "parse-util.h"
#include "psi-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"

ཧྲིལ read_resource_pressure(const var_na *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ var_na *line = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        const var_na *cline;
        var_na *word;
        ཧྲིལ r;

        assert(path);
        assert(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        assert(ret);

        const var_na *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *l = NULL;
                var_na *w;

                r = read_line(f, LONG_LINE_MAX, &l);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;

                w = first_word(l, t);
                གལ་ཏེ (w) {
                        line = TAKE_PTR(l);
                        cline = w;
                        bha_nga;
                }
        }

        གལ་ཏེ (!line)
                ཕྱིར -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        སྐབས ((r = extract_first_word(&cline, &word, NULL, 0)) > 0) {
                _cleanup_free_ var_na *w = word;
                const var_na *v;

                གལ་ཏེ ((v = startswith(w, "avg10="))) {
                        གལ་ཏེ (field_filled & (1U << 0))
                                ཕྱིར -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } དེ་མིན གལ་ཏེ ((v = startswith(w, "avg60="))) {
                        གལ་ཏེ (field_filled & (1U << 1))
                                ཕྱིར -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } དེ་མིན གལ་ཏེ ((v = startswith(w, "avg300="))) {
                        གལ་ཏེ (field_filled & (1U << 2))
                                ཕྱིར -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } དེ་མིན གལ་ཏེ ((v = startswith(w, "total="))) {
                        གལ་ཏེ (field_filled & (1U << 3))
                                ཕྱིར -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } དེ་མིན
                        continue;

                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (field_filled != 15U)
                ཕྱིར -EINVAL;

        *ret = rp;
        ཕྱིར 0;
}

const PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

static const var_na* const pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

static const var_na* const pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

ཧྲིལ is_pressure_supported(void) {
        static thread_local ཧྲིལ cached = -1;
        ཧྲིལ r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        གལ་ཏེ (cached >= 0)
                ཕྱིར cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, NULL, NULL);
                གལ་ཏེ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ཕྱིར (cached = false);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        ཕྱིར (cached = true);
}
