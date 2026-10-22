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

цяло read_resource_pressure(const char *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ char *line = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        const char *cline;
        char *word;
        цяло r;

        assert(path);
        assert(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        assert(ret);

        const char *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        ако (r < 0)
                върни r;

        за (;;) {
                _cleanup_free_ char *l = NULL;
                char *w;

                r = read_line(f, LONG_LINE_MAX, &l);
                ако (r < 0)
                        върни r;
                ако (r == 0)
                        break;

                w = first_word(l, t);
                ако (w) {
                        line = TAKE_PTR(l);
                        cline = w;
                        break;
                }
        }

        ако (!line)
                върни -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        докато ((r = extract_first_word(&cline, &word, NULL, 0)) > 0) {
                _cleanup_free_ char *w = word;
                const char *v;

                ако ((v = startswith(w, "avg10="))) {
                        ако (field_filled & (1U << 0))
                                върни -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } иначе ако ((v = startswith(w, "avg60="))) {
                        ако (field_filled & (1U << 1))
                                върни -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } иначе ако ((v = startswith(w, "avg300="))) {
                        ако (field_filled & (1U << 2))
                                върни -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } иначе ако ((v = startswith(w, "total="))) {
                        ако (field_filled & (1U << 3))
                                върни -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } иначе
                        continue;

                ако (r < 0)
                        върни r;
        }

        ако (r < 0)
                върни r;

        ако (field_filled != 15U)
                върни -EINVAL;

        *ret = rp;
        върни 0;
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

цяло is_pressure_supported(void) {
        static thread_local цяло cached = -1;
        цяло r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ако (cached >= 0)
                върни cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, NULL, NULL);
                ако (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        върни (cached = false);
                ако (r < 0)
                        върни r;
        }

        върни (cached = true);
}
