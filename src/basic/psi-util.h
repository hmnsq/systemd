/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "parse-util.h"

प्रकारव्याख्या गणना PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_FULL,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

प्रकारव्याख्या गणना PressureResource {
        PRESSURE_MEMORY,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
प्रकारव्याख्या संरचना ResourcePressure {
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
पूर्णांक read_resource_pressure(स्थिरांक अक्षर *path, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
पूर्णांक is_pressure_supported(शून्य);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
प्रकारव्याख्या संरचना PressureResourceInfo {
        स्थिरांक अक्षर *name;       /* "memory", "cpu", "io" */
        स्थिरांक अक्षर *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        स्थिरांक अक्षर *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

बाह्य स्थिरांक PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

स्थिर ओळीत स्थिरांक PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        प्रतिज्ञा(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        परत &pressure_resource_info[resource];
}

DECLARE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#define PRESSURE_DEFAULT_TYPE "some"
#define PRESSURE_DEFAULT_THRESHOLD_USEC (200 * USEC_PER_MSEC)
#define PRESSURE_DEFAULT_WINDOW_USEC (2 * USEC_PER_SEC)
