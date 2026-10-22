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

số_nguyên read_resource_pressure(const char *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ char *line = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        const char *cline;
        char *word;
        số_nguyên r;

        assert(path);
        assert(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        assert(ret);

        const char *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        nếu (r < 0)
                trả_về r;

        cho (;;) {
                _cleanup_free_ char *l = NULL;
                char *w;

                r = read_line(f, LONG_LINE_MAX, &l);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;

                w = first_word(l, t);
                nếu (w) {
                        line = TAKE_PTR(l);
                        cline = w;
                        break;
                }
        }

        nếu (!line)
                trả_về -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        trong_khi ((r = extract_first_word(&cline, &word, NULL, 0)) > 0) {
                _cleanup_free_ char *w = word;
                const char *v;

                nếu ((v = startswith(w, "avg10="))) {
                        nếu (field_filled & (1U << 0))
                                trả_về -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } không_thì nếu ((v = startswith(w, "avg60="))) {
                        nếu (field_filled & (1U << 1))
                                trả_về -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } không_thì nếu ((v = startswith(w, "avg300="))) {
                        nếu (field_filled & (1U << 2))
                                trả_về -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } không_thì nếu ((v = startswith(w, "total="))) {
                        nếu (field_filled & (1U << 3))
                                trả_về -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } không_thì
                        continue;

                nếu (r < 0)
                        trả_về r;
        }

        nếu (r < 0)
                trả_về r;

        nếu (field_filled != 15U)
                trả_về -EINVAL;

        *ret = rp;
        trả_về 0;
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

static const char* const pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

static const char* const pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

số_nguyên is_pressure_supported(void) {
        static thread_local số_nguyên cached = -1;
        số_nguyên r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        nếu (cached >= 0)
                trả_về cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, NULL, NULL);
                nếu (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        trả_về (cached = false);
                nếu (r < 0)
                        trả_về r;
        }

        trả_về (cached = true);
}
