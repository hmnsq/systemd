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

पूर्णाङ्क read_resource_pressure(ध्रुव वर्ण *मार्गः, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        _cleanup_fclose_ FILE *f = रिक्त;
        अचिह्नित क्षेत्रम्_filled = 0;
        ResourcePressure rp = {};
        ध्रुव वर्ण *cline;
        वर्ण *word;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(अन्तर्_समुच्चयः(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_पूर्णम्));
        प्रतिज्ञा(ret);

        ध्रुव वर्ण *t = ASSERT_PTR(pressure_type_गन्तव्यम्_string(type));

        r = fopen_unlocked(मार्गः, "re", &f);
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (;;) {
                _cleanup_free_ वर्ण *l = रिक्त;
                वर्ण *w;

                r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &l);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                w = first_word(l, t);
                यदि (w) {
                        पङ्क्तिः = TAKE_PTR(l);
                        cline = w;
                        भङ्ग;
                }
        }

        यदि (!पङ्क्तिः)
                प्रत्यागम -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        यावत् ((r = extract_first_word(&cline, &word, रिक्त, 0)) > 0) {
                _cleanup_free_ वर्ण *w = word;
                ध्रुव वर्ण *v;

                यदि ((v = startswith(w, "avg10="))) {
                        यदि (क्षेत्रम्_filled & (1U << 0))
                                प्रत्यागम -EINVAL;

                        क्षेत्रम्_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } अन्यथा यदि ((v = startswith(w, "avg60="))) {
                        यदि (क्षेत्रम्_filled & (1U << 1))
                                प्रत्यागम -EINVAL;

                        क्षेत्रम्_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } अन्यथा यदि ((v = startswith(w, "avg300="))) {
                        यदि (क्षेत्रम्_filled & (1U << 2))
                                प्रत्यागम -EINVAL;

                        क्षेत्रम्_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } अन्यथा यदि ((v = startswith(w, "total="))) {
                        यदि (क्षेत्रम्_filled & (1U << 3))
                                प्रत्यागम -EINVAL;

                        क्षेत्रम्_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.योग_2);
                } अन्यथा
                        अनुवर्त;

                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (r < 0)
                प्रत्यागम r;

        यदि (क्षेत्रम्_filled != 15U)
                प्रत्यागम -EINVAL;

        *ret = rp;
        प्रत्यागम 0;
}

ध्रुव PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_स्मृतिः] = {
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

स्थिर ध्रुव वर्ण* ध्रुव pressure_resource_सारणी[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_स्मृतिः] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_सारणी_LOOKUP(pressure_resource, PressureResource);

स्थिर ध्रुव वर्ण* ध्रुव pressure_type_सारणी[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_पूर्णम्] = "full",
};

DEFINE_STRING_सारणी_LOOKUP(pressure_type, PressureType);

पूर्णाङ्क is_pressure_supported(शून्य) {
        स्थिर thread_local पूर्णाङ्क cached = -1;
        पूर्णाङ्क r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        यदि (cached >= 0)
                प्रत्यागम cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, रिक्त, रिक्त);
                यदि (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        प्रत्यागम (cached = false);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम (cached = true);
}
