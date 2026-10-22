/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_ತುಂಬಿದ,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ PressureResource {
        PRESSURE_ಸ್ಮರಣೆ,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ResourcePressure {
        loadavg_t avg10;
        loadavg_t avg60;
        loadavg_t avg300;
        usec_t ಒಟ್ಟು;
} ResourcePressure;

/** Upstream 4.20+ format
 *
 *  some avg10=0.22 avg60=0.17 avg300=1.11 total=58761459
 *  full avg10=0.23 avg60=0.16 avg300=1.08 total=58464525
 */
ಪೂರ್ಣಾಂಕ read_resource_pressure(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
ಪೂರ್ಣಾಂಕ is_pressure_supported(ಶೂನ್ಯ);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ PressureResourceInfo {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;       /* "memory", "cpu", "io" */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        ಪ್ರತಿಜ್ಞಾ(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        ಹಿಂತಿರುಗಿ &pressure_resource_info[resource];
}

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#ವ್ಯಾಖ್ಯಾನಿಸು PRESSURE_DEFAULT_TYPE "some"
#ವ್ಯಾಖ್ಯಾನಿಸು PRESSURE_DEFAULT_ಮಿತಿ_USEC (200 * USEC_ಪ್ರತಿ_MSEC)
#ವ್ಯಾಖ್ಯಾನಿಸು PRESSURE_DEFAULT_ಕಿಟಕಿ_USEC (2 * USEC_ಪ್ರತಿ_SEC)
