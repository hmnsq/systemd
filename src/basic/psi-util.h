/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "parse-util.h"

प्रकार क्रमागत PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_FULL,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

प्रकार क्रमागत PressureResource {
        PRESSURE_MEMORY,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
प्रकार काष्ठा ResourcePressure {
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
पूर्णांक read_resource_pressure(स्थिर अक्षर *path, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
पूर्णांक is_pressure_supported(व्योम);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
प्रकार काष्ठा PressureResourceInfo {
        स्थिर अक्षर *name;       /* "memory", "cpu", "io" */
        स्थिर अक्षर *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        स्थिर अक्षर *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

बाह्य स्थिर PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

अटल अंतरभूत स्थिर PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        निश्चित(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        वापस &pressure_resource_info[resource];
}

DECLARE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#घोषणा PRESSURE_DEFAULT_TYPE "some"
#घोषणा PRESSURE_DEFAULT_THRESHOLD_USEC (200 * USEC_PER_MSEC)
#घोषणा PRESSURE_DEFAULT_WINDOW_USEC (2 * USEC_PER_SEC)
