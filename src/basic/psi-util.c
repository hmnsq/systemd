/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <threads.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "psi-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

પૂર્ણાંક read_resource_pressure(અચલ અક્ષર *path, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        _cleanup_fclose_ FILE *f = ખાલી;
        unsigned field_filled = 0;
        ResourcePressure rp = {};
        અચલ અક્ષર *cline;
        અક્ષર *word;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(IN_SET(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_FULL));
        પ્રતિજ્ઞા(ret);

        અચલ અક્ષર *t = ASSERT_PTR(pressure_type_to_string(type));

        r = fopen_unlocked(path, "re", &f);
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *l = ખાલી;
                અક્ષર *w;

                r = read_લીટી(f, LONG_લીટી_MAX, &l);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                w = first_word(l, t);
                જો (w) {
                        લીટી = TAKE_PTR(l);
                        cline = w;
                        તોડો;
                }
        }

        જો (!લીટી)
                પાછા -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        જ્યારે ((r = extract_first_word(&cline, &word, ખાલી, 0)) > 0) {
                _cleanup_free_ અક્ષર *w = word;
                અચલ અક્ષર *v;

                જો ((v = startswith(w, "avg10="))) {
                        જો (field_filled & (1U << 0))
                                પાછા -EINVAL;

                        field_filled |= 1U << 0;
                        r = parse_loadavg_fixed_point(v, &rp.avg10);
                } નહીંતર જો ((v = startswith(w, "avg60="))) {
                        જો (field_filled & (1U << 1))
                                પાછા -EINVAL;

                        field_filled |= 1U << 1;
                        r = parse_loadavg_fixed_point(v, &rp.avg60);
                } નહીંતર જો ((v = startswith(w, "avg300="))) {
                        જો (field_filled & (1U << 2))
                                પાછા -EINVAL;

                        field_filled |= 1U << 2;
                        r = parse_loadavg_fixed_point(v, &rp.avg300);
                } નહીંતર જો ((v = startswith(w, "total="))) {
                        જો (field_filled & (1U << 3))
                                પાછા -EINVAL;

                        field_filled |= 1U << 3;
                        r = safe_atou64(v, &rp.total);
                } નહીંતર
                        ચાલુ;

                જો (r < 0)
                        પાછા r;
        }

        જો (r < 0)
                પાછા r;

        જો (field_filled != 15U)
                પાછા -EINVAL;

        *ret = rp;
        પાછા 0;
}

અચલ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
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

સ્થિર અચલ અક્ષર* અચલ pressure_resource_table[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_MEMORY] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);

સ્થિર અચલ અક્ષર* અચલ pressure_type_table[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_FULL] = "full",
};

DEFINE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

પૂર્ણાંક is_pressure_supported(શૂન્ય) {
        સ્થિર thread_local પૂર્ણાંક cached = -1;
        પૂર્ણાંક r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        જો (cached >= 0)
                પાછા cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ખાલી, ખાલી);
                જો (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        પાછા (cached = false);
                જો (r < 0)
                        પાછા r;
        }

        પાછા (cached = true);
}
