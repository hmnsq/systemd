/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_FULL,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା PressureResource {
        PRESSURE_MEMORY,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ResourcePressure {
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_resource_pressure(ଅଚଳ ଅକ୍ଷର *path, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_pressure_supported(ଶୂନ୍ୟ);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା PressureResourceInfo {
        ଅଚଳ ଅକ୍ଷର *name;       /* "memory", "cpu", "io" */
        ଅଚଳ ଅକ୍ଷର *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        ଅଚଳ ଅକ୍ଷର *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

ବାହ୍ୟ ଅଚଳ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        ପ୍ରତିଜ୍ଞା(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        ଫେରନ୍ତୁ &pressure_resource_info[resource];
}

DECLARE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#define PRESSURE_DEFAULT_TYPE "some"
#define PRESSURE_DEFAULT_THRESHOLD_USEC (200 * USEC_PER_MSEC)
#define PRESSURE_DEFAULT_WINDOW_USEC (2 * USEC_PER_SEC)
