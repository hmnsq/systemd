/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_ਭਰਿਆ,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ PressureResource {
        PRESSURE_ਸਮ੍ਰਿਤੀ,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ResourcePressure {
        loadavg_t avg10;
        loadavg_t avg60;
        loadavg_t avg300;
        usec_t ਕੁੱਲ;
} ResourcePressure;

/** Upstream 4.20+ format
 *
 *  some avg10=0.22 avg60=0.17 avg300=1.11 total=58761459
 *  full avg10=0.23 avg60=0.16 avg300=1.08 total=58464525
 */
ਪੂਰਨ read_resource_pressure(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
ਪੂਰਨ is_pressure_supported(ਖਾਲੀ);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ PressureResourceInfo {
        ਅਚਲ ਅੱਖਰ *name;       /* "memory", "cpu", "io" */
        ਅਚਲ ਅੱਖਰ *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        ਅਚਲ ਅੱਖਰ *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

ਬਾਹਰੀ ਅਚਲ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        ਪ੍ਰਤਿਜ੍ਞਾ(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        ਵਾਪਸ &pressure_resource_info[resource];
}

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESSURE_DEFAULT_TYPE "some"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESSURE_DEFAULT_ਹੱਦ_USEC (200 * USEC_ਪ੍ਰਤੀ_MSEC)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRESSURE_DEFAULT_ਖਿੜਕੀ_USEC (2 * USEC_ਪ੍ਰਤੀ_SEC)
