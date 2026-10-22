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

ကိန်းပြည့် read_resource_pressure(const var_na *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ var_na *line = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        const var_na *cline;
        var_na *word;
        ကိန်းပြည့် r;

        assert(path);
        assert(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        assert(ret);

        const var_na *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အတွက် (;;) {
                _cleanup_free_ var_na *l = NULL;
                var_na *w;

                r = read_line(f, LONG_LINE_MAX, &l);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        bha_nga;

                w = first_word(l, t);
                အကယ်၍ (w) {
                        line = TAKE_PTR(l);
                        cline = w;
                        bha_nga;
                }
        }

        အကယ်၍ (!line)
                ပြန်ပေး -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        နေစဉ် ((r = extract_first_word(&cline, &word, NULL, 0)) > 0) {
                _cleanup_free_ var_na *w = word;
                const var_na *v;

                အကယ်၍ ((v = startswith(w, "avg10="))) {
                        အကယ်၍ (field_filled & (1U << 0))
                                ပြန်ပေး -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } မဟုတ်ပါက အကယ်၍ ((v = startswith(w, "avg60="))) {
                        အကယ်၍ (field_filled & (1U << 1))
                                ပြန်ပေး -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } မဟုတ်ပါက အကယ်၍ ((v = startswith(w, "avg300="))) {
                        အကယ်၍ (field_filled & (1U << 2))
                                ပြန်ပေး -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } မဟုတ်ပါက အကယ်၍ ((v = startswith(w, "total="))) {
                        အကယ်၍ (field_filled & (1U << 3))
                                ပြန်ပေး -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } မဟုတ်ပါက
                        continue;

                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (field_filled != 15U)
                ပြန်ပေး -EINVAL;

        *ret = rp;
        ပြန်ပေး 0;
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

ကိန်းပြည့် is_pressure_supported(void) {
        static thread_local ကိန်းပြည့် cached = -1;
        ကိန်းပြည့် r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        အကယ်၍ (cached >= 0)
                ပြန်ပေး cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, NULL, NULL);
                အကယ်၍ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ပြန်ပေး (cached = false);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး (cached = true);
}
