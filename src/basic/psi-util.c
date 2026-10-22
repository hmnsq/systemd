/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <threads.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "psi-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

पूर्णांक read_resource_pressure(स्थिरांक अक्षर *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ अक्षर *ओळ = रिक्त;
        _cleanup_fclose_ खाता *f = रिक्त;
        अचिन्हित field_filled = 0;
        ResourcePressure rp = {};
        स्थिरांक अक्षर *cline;
        अक्षर *word;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        प्रतिज्ञा(ret);

        स्थिरांक अक्षर *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        जर (r < 0)
                परत r;

        साठी (;;) {
                _cleanup_free_ अक्षर *l = रिक्त;
                अक्षर *w;

                r = read_ओळ(f, LONG_ओळ_MAX, &l);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                w = first_word(l, t);
                जर (w) {
                        ओळ = TAKE_PTR(l);
                        cline = w;
                        थांबा;
                }
        }

        जर (!ओळ)
                परत -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        पर्यंत ((r = extract_first_word(&cline, &word, रिक्त, 0)) > 0) {
                _cleanup_free_ अक्षर *w = word;
                स्थिरांक अक्षर *v;

                जर ((v = startswith(w, "avg10="))) {
                        जर (field_filled & (1U << 0))
                                परत -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } अन्यथा जर ((v = startswith(w, "avg60="))) {
                        जर (field_filled & (1U << 1))
                                परत -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } अन्यथा जर ((v = startswith(w, "avg300="))) {
                        जर (field_filled & (1U << 2))
                                परत -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } अन्यथा जर ((v = startswith(w, "total="))) {
                        जर (field_filled & (1U << 3))
                                परत -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } अन्यथा
                        सुरूठेवा;

                जर (r < 0)
                        परत r;
        }

        जर (r < 0)
                परत r;

        जर (field_filled != 15U)
                परत -EINVAL;

        *ret = rp;
        परत 0;
}

स्थिरांक PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

स्थिर स्थिरांक अक्षर* स्थिरांक pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

स्थिर स्थिरांक अक्षर* स्थिरांक pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

पूर्णांक is_pressure_supported(शून्य) {
        स्थिर thread_local पूर्णांक cached = -1;
        पूर्णांक r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        जर (cached >= 0)
                परत cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, रिक्त, रिक्त);
                जर (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        परत (cached = false);
                जर (r < 0)
                        परत r;
        }

        परत (cached = true);
}
