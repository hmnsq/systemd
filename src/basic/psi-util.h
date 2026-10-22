/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "parse-util.h"

प्रकारनिर्देश गणना PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_पूर्णम्,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

प्रकारनिर्देश गणना PressureResource {
        PRESSURE_स्मृतिः,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
प्रकारनिर्देश संरचना ResourcePressure {
        loadavg_t avg10;
        loadavg_t avg60;
        loadavg_t avg300;
        usec_t योग_2;
} ResourcePressure;

/** Upstream 4.20+ format
 *
 *  some avg10=0.22 avg60=0.17 avg300=1.11 total=58761459
 *  full avg10=0.23 avg60=0.16 avg300=1.08 total=58464525
 */
पूर्णाङ्क read_resource_pressure(ध्रुव वर्ण *मार्गः, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
पूर्णाङ्क is_pressure_supported(शून्य);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
प्रकारनिर्देश संरचना PressureResourceInfo {
        ध्रुव वर्ण *name;       /* "memory", "cpu", "io" */
        ध्रुव वर्ण *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        ध्रुव वर्ण *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

बाह्य ध्रुव PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

स्थिर अन्तःस्थ ध्रुव PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        प्रतिज्ञा(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        प्रत्यागम &pressure_resource_info[resource];
}

DECLARE_STRING_सारणी_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_सारणी_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#परिभाषय PRESSURE_DEFAULT_TYPE "some"
#परिभाषय PRESSURE_DEFAULT_देहली_USEC (200 * USEC_प्रत्येकम्_MSEC)
#परिभाषय PRESSURE_DEFAULT_गवाक्षः_USEC (2 * USEC_प्रत्येकम्_SEC)
