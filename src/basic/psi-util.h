/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "parse-util.h"

প্রকারলেখ গণনা PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_FULL,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

প্রকারলেখ গণনা PressureResource {
        PRESSURE_MEMORY,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
প্রকারলেখ কাঠামো ResourcePressure {
        loadavg_t avg10;
        loadavg_t avg60;
        loadavg_t avg300;
        usec_t যোগ_2;
} ResourcePressure;

/** Upstream 4.20+ format
 *
 *  some avg10=0.22 avg60=0.17 avg300=1.11 total=58761459
 *  full avg10=0.23 avg60=0.16 avg300=1.08 total=58464525
 */
পূর্ণসংখ্যা read_resource_pressure(ধ্রুবক অক্ষর *path, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
পূর্ণসংখ্যা is_pressure_supported(শূন্য);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
প্রকারলেখ কাঠামো PressureResourceInfo {
        ধ্রুবক অক্ষর *name;       /* "memory", "cpu", "io" */
        ধ্রুবক অক্ষর *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        ধ্রুবক অক্ষর *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

বাহ্যিক ধ্রুবক PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

স্থির অন্তঃরেখ ধ্রুবক PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        প্রতিজ্ঞা(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        ফেরত &pressure_resource_info[resource];
}

DECLARE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#define PRESSURE_DEFAULT_TYPE "some"
#define PRESSURE_DEFAULT_THRESHOLD_USEC (200 * USEC_PER_MSEC)
#define PRESSURE_DEFAULT_WINDOW_USEC (2 * USEC_PER_SEC)
