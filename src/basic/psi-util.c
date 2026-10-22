/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <threads.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "psi-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

పూర్ణాంకం read_resource_pressure(స్థిరరాశి అక్షరం *మార్గం, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        _cleanup_fclose_ FILE *f = ఖాళీ;
        గుర్తులేని క్షేత్రం_filled = 0;
        ResourcePressure rp = {};
        స్థిరరాశి అక్షరం *cline;
        అక్షరం *మాట;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(లోపల_సమితి(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_నిండిన));
        ప్రతిజ్ఞా(ret);

        స్థిరరాశి అక్షరం *t = ASSERT_PTR(pressure_type_గమ్యం_string(type));

        r = fopen_unlocked(మార్గం, "re", &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *l = ఖాళీ;
                అక్షరం *w;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &l);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                w = మొదటి_మాట(l, t);
                ఒకవేళ (w) {
                        పంక్తి = TAKE_PTR(l);
                        cline = w;
                        ఆపు;
                }
        }

        ఒకవేళ (!పంక్తి)
                తిరిగి -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        అయితే ((r = వెలికితీయి_మొదటి_మాట(&cline, &మాట, ఖాళీ, 0)) > 0) {
                _cleanup_free_ అక్షరం *w = మాట;
                స్థిరరాశి అక్షరం *v;

                ఒకవేళ ((v = startswith(w, "avg10="))) {
                        ఒకవేళ (క్షేత్రం_filled & (1U << 0))
                                తిరిగి -EINVAL;

                        క్షేత్రం_filled |= 1U << 0;
                        r = parse_loadavg_fixed_బిందువు(v, &rp.avg10);
                } లేకపోతే ఒకవేళ ((v = startswith(w, "avg60="))) {
                        ఒకవేళ (క్షేత్రం_filled & (1U << 1))
                                తిరిగి -EINVAL;

                        క్షేత్రం_filled |= 1U << 1;
                        r = parse_loadavg_fixed_బిందువు(v, &rp.avg60);
                } లేకపోతే ఒకవేళ ((v = startswith(w, "avg300="))) {
                        ఒకవేళ (క్షేత్రం_filled & (1U << 2))
                                తిరిగి -EINVAL;

                        క్షేత్రం_filled |= 1U << 2;
                        r = parse_loadavg_fixed_బిందువు(v, &rp.avg300);
                } లేకపోతే ఒకవేళ ((v = startswith(w, "total="))) {
                        ఒకవేళ (క్షేత్రం_filled & (1U << 3))
                                తిరిగి -EINVAL;

                        క్షేత్రం_filled |= 1U << 3;
                        r = సురక్షిత_atou64(v, &rp.మొత్తం);
                } లేకపోతే
                        కొనసాగు;

                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (క్షేత్రం_filled != 15U)
                తిరిగి -EINVAL;

        *ret = rp;
        తిరిగి 0;
}

స్థిరరాశి PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_జ్ఞాపకం] = {
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

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి pressure_resource_పట్టిక[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_జ్ఞాపకం] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_పట్టిక_LOOKUP(pressure_resource, PressureResource);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి pressure_type_పట్టిక[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_నిండిన] = "full",
};

DEFINE_STRING_పట్టిక_LOOKUP(pressure_type, PressureType);

పూర్ణాంకం is_pressure_supported(శూన్య) {
        స్థిర thread_local పూర్ణాంకం cached = -1;
        పూర్ణాంకం r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ఒకవేళ (cached >= 0)
                తిరిగి cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ఖాళీ, ఖాళీ);
                ఒకవేళ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        తిరిగి (cached = false);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి (cached = true);
}
