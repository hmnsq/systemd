/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "psi-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಪೂರ್ಣಾಂಕ read_resource_pressure(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, PressureType type, ResourcePressure *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ಕ್ಷೇತ್ರ_filled = 0;
        ResourcePressure rp = {};
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cline;
        ಅಕ್ಷರ *ಶಬ್ದ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(type, PRESSURE_TYPE_SOME, PRESSURE_TYPE_ತುಂಬಿದ));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = ASSERT_PTR(pressure_type_ಗಮ್ಯ_string(type));

        r = fopen_unlocked(ಮಾರ್ಗ, "re", &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *l = ಖಾಲಿ;
                ಅಕ್ಷರ *w;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &l);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                w = ಮೊದಲ_ಶಬ್ದ(l, t);
                ಆದರೆ (w) {
                        ಸಾಲು = TAKE_PTR(l);
                        cline = w;
                        ನಿಲ್ಲಿಸು;
                }
        }

        ಆದರೆ (!ಸಾಲು)
                ಹಿಂತಿರುಗಿ -ENODATA;

        /* extracts either avgX=Y.Z or total=X */
        ಆಗ ((r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&cline, &ಶಬ್ದ, ಖಾಲಿ, 0)) > 0) {
                _cleanup_free_ ಅಕ್ಷರ *w = ಶಬ್ದ;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v;

                ಆದರೆ ((v = startswith(w, "avg10="))) {
                        ಆದರೆ (ಕ್ಷೇತ್ರ_filled & (1U << 0))
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಕ್ಷೇತ್ರ_filled |= 1U << 0;
                        r = parse_loadavg_fixed_ಬಿಂದು(v, &rp.avg10);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((v = startswith(w, "avg60="))) {
                        ಆದರೆ (ಕ್ಷೇತ್ರ_filled & (1U << 1))
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಕ್ಷೇತ್ರ_filled |= 1U << 1;
                        r = parse_loadavg_fixed_ಬಿಂದು(v, &rp.avg60);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((v = startswith(w, "avg300="))) {
                        ಆದರೆ (ಕ್ಷೇತ್ರ_filled & (1U << 2))
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಕ್ಷೇತ್ರ_filled |= 1U << 2;
                        r = parse_loadavg_fixed_ಬಿಂದು(v, &rp.avg300);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((v = startswith(w, "total="))) {
                        ಆದರೆ (ಕ್ಷೇತ್ರ_filled & (1U << 3))
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಕ್ಷೇತ್ರ_filled |= 1U << 3;
                        r = ಸುರಕ್ಷಿತ_atou64(v, &rp.ಒಟ್ಟು);
                } ಇಲ್ಲದಿದ್ದರೆ
                        ಮುಂದುವರಿ;

                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಕ್ಷೇತ್ರ_filled != 15U)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = rp;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_ಸ್ಮರಣೆ] = {
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ pressure_resource_ಕೋಷ್ಟಕ[_PRESSURE_RESOURCE_MAX] = {
        [PRESSURE_ಸ್ಮರಣೆ] = "memory",
        [PRESSURE_CPU]    = "cpu",
        [PRESSURE_IO]     = "io",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(pressure_resource, PressureResource);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ pressure_type_ಕೋಷ್ಟಕ[_PRESSURE_TYPE_MAX] = {
        [PRESSURE_TYPE_SOME] = "some",
        [PRESSURE_TYPE_ತುಂಬಿದ] = "full",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(pressure_type, PressureType);

ಪೂರ್ಣಾಂಕ is_pressure_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಂಕ cached = -1;
        ಪೂರ್ಣಾಂಕ r;

        /* The pressure files, both under /proc/ and in cgroups, will exist even if the kernel has PSI
         * support disabled; we have to read the file to make sure it doesn't return -EOPNOTSUPP */

        ಆದರೆ (cached >= 0)
                ಹಿಂತಿರುಗಿ cached;

        FOREACH_STRING(p, "/proc/pressure/cpu", "/proc/pressure/io", "/proc/pressure/memory") {
                r = read_virtual_file(p, 0, ಖಾಲಿ, ಖಾಲಿ);
                ಆದರೆ (r == -ENOENT || ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಹಿಂತಿರುಗಿ (cached = false);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ (cached = true);
}
