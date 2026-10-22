/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_FULL,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ PressureResource {
        PRESSURE_MEMORY,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ResourcePressure {
        loadavg_t avg10;
        loadavg_t avg60;
        loadavg_t avg300;
        usec_t total;
} ResourcePressure;

/** Upstream 4.20+ format
 *
 *  some avg10=0.22 avg60=0.17 avg300=1.11 total=58761459
 *  full avg10=0.23 avg60=0.16 avg300=1.08 total=58464525
 */
ಪೂರ್ಣಾಙ್ಕ read_resource_pressure(ಧ್ರುವ ವರ್ಣ *path, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
ಪೂರ್ಣಾಙ್ಕ is_pressure_supported(ಶೂನ್ಯ);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ PressureResourceInfo {
        ಧ್ರುವ ವರ್ಣ *name;       /* "memory", "cpu", "io" */
        ಧ್ರುವ ವರ್ಣ *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        ಧ್ರುವ ವರ್ಣ *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

ಬಾಹ್ಯ ಧ್ರುವ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        ಪ್ರತಿಜ್ಞಾ(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        ಪ್ರತ್ಯಾಗಮ &pressure_resource_info[resource];
}

DECLARE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#define PRESSURE_DEFAULT_TYPE "some"
#define PRESSURE_DEFAULT_THRESHOLD_USEC (200 * USEC_PER_MSEC)
#define PRESSURE_DEFAULT_WINDOW_USEC (2 * USEC_PER_SEC)
