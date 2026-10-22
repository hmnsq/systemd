/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <threads.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "parse-util.h"
#समावेश "psi-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"

पूर्णांक read_resource_pressure(स्थिर अक्षर *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        _cleanup_fclose_ खाता *f = शून्य;
        अचिन्हित field_filled = 0;
        ResourcePressure rp = {};
        स्थिर अक्षर *cline;
        अक्षर *word;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        निश्चित(ret);

        स्थिर अक्षर *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *l = शून्य;
                अक्षर *w;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &l);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                w = first_word(l, t);
                अगर (w) {
                        पंक्ति = TAKE_PTR(l);
                        cline = w;
                        अवरोध;
                }
        }

        अगर (!पंक्ति)
                वापस -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        जबतक ((r = extract_first_word(&cline, &word, शून्य, 0)) > 0) {
                _cleanup_free_ अक्षर *w = word;
                स्थिर अक्षर *v;

                अगर ((v = startswith(w, "avg10="))) {
                        अगर (field_filled & (1U << 0))
                                वापस -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } अन्यथा अगर ((v = startswith(w, "avg60="))) {
                        अगर (field_filled & (1U << 1))
                                वापस -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } अन्यथा अगर ((v = startswith(w, "avg300="))) {
                        अगर (field_filled & (1U << 2))
                                वापस -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } अन्यथा अगर ((v = startswith(w, "total="))) {
                        अगर (field_filled & (1U << 3))
                                वापस -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.योग_2);
                } अन्यथा
                        जारी;

                अगर (r < 0)
                        वापस r;
        }

        अगर (r < 0)
                वापस r;

        अगर (field_filled != 15U)
                वापस -EINVAL;

        *ret = rp;
        वापस 0;
}

स्थिर PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

अटल स्थिर अक्षर* स्थिर pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

अटल स्थिर अक्षर* स्थिर pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

पूर्णांक is_pressure_supported(व्योम) {
        अटल thread_local पूर्णांक cached = -1;
        पूर्णांक r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        अगर (cached >= 0)
                वापस cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, शून्य, शून्य);
                अगर (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        वापस (cached = false);
                अगर (r < 0)
                        वापस r;
        }

        वापस (cached = true);
}
