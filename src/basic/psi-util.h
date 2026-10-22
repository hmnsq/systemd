/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "parse-util.h"

પ્રકારવ્યાખ્યા ગણના PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_FULL,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

પ્રકારવ્યાખ્યા ગણના PressureResource {
        PRESSURE_MEMORY,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
પ્રકારવ્યાખ્યા સંરચના ResourcePressure {
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
પૂર્ણાંક read_resource_pressure(અચલ અક્ષર *path, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
પૂર્ણાંક is_pressure_supported(શૂન્ય);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
પ્રકારવ્યાખ્યા સંરચના PressureResourceInfo {
        અચલ અક્ષર *name;       /* "memory", "cpu", "io" */
        અચલ અક્ષર *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        અચલ અક્ષર *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

બાહ્ય અચલ PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

સ્થિર અંતઃરેખ અચલ PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        પ્રતિજ્ઞા(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        પાછા &pressure_resource_info[resource];
}

DECLARE_STRING_TABLE_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_TABLE_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#define PRESSURE_DEFAULT_TYPE "some"
#define PRESSURE_DEFAULT_THRESHOLD_USEC (200 * USEC_PER_MSEC)
#define PRESSURE_DEFAULT_WINDOW_USEC (2 * USEC_PER_SEC)
