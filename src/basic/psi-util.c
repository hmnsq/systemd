/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <threads.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "psi-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

പൂർണ്ണസംഖ്യ read_resource_pressure(സ്ഥിരം അക്ഷരം *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        സ്ഥിരം അക്ഷരം *cline;
        അക്ഷരം *word;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        പ്രതിജ്ഞാ(ret);

        സ്ഥിരം അക്ഷരം *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *l = ശൂന്യ;
                അക്ഷരം *w;

                r = read_വരി(f, LONG_വരി_MAX, &l);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                w = first_word(l, t);
                എങ്കിൽ (w) {
                        വരി = TAKE_PTR(l);
                        cline = w;
                        നിർത്തുക;
                }
        }

        എങ്കിൽ (!വരി)
                തിരികെ -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        ആകുമ്പോൾ ((r = extract_first_word(&cline, &word, ശൂന്യ, 0)) > 0) {
                _cleanup_free_ അക്ഷരം *w = word;
                സ്ഥിരം അക്ഷരം *v;

                എങ്കിൽ ((v = startswith(w, "avg10="))) {
                        എങ്കിൽ (field_filled & (1U << 0))
                                തിരികെ -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } അല്ലെങ്കിൽ എങ്കിൽ ((v = startswith(w, "avg60="))) {
                        എങ്കിൽ (field_filled & (1U << 1))
                                തിരികെ -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } അല്ലെങ്കിൽ എങ്കിൽ ((v = startswith(w, "avg300="))) {
                        എങ്കിൽ (field_filled & (1U << 2))
                                തിരികെ -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } അല്ലെങ്കിൽ എങ്കിൽ ((v = startswith(w, "total="))) {
                        എങ്കിൽ (field_filled & (1U << 3))
                                തിരികെ -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } അല്ലെങ്കിൽ
                        തുടരുക;

                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (field_filled != 15U)
                തിരികെ -EINVAL;

        *ret = rp;
        തിരികെ 0;
}

സ്ഥിരം PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

പൂർണ്ണസംഖ്യ is_pressure_supported(ശൂന്യം) {
        സ്ഥിതം thread_local പൂർണ്ണസംഖ്യ cached = -1;
        പൂർണ്ണസംഖ്യ r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        എങ്കിൽ (cached >= 0)
                തിരികെ cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ശൂന്യ, ശൂന്യ);
                എങ്കിൽ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        തിരികെ (cached = false);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ (cached = true);
}
