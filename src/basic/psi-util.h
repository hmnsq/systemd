/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "parse-util.h"

రకనిర్వచనం గణన PressureType {
        PRESSURE_TYPE_SOME,
        PRESSURE_TYPE_నిండిన,
        _PRESSURE_TYPE_MAX,
        _PRESSURE_TYPE_INVALID = -EINVAL,
} PressureType;

రకనిర్వచనం గణన PressureResource {
        PRESSURE_జ్ఞాపకం,
        PRESSURE_CPU,
        PRESSURE_IO,
        _PRESSURE_RESOURCE_MAX,
        _PRESSURE_RESOURCE_INVALID = -EINVAL,
} PressureResource;

/* Averages are stored in fixed-point with 11 bit fractions */
రకనిర్వచనం నిర్మాణం ResourcePressure {
        loadavg_t avg10;
        loadavg_t avg60;
        loadavg_t avg300;
        usec_t మొత్తం;
} ResourcePressure;

/** Upstream 4.20+ format
 *
 *  some avg10=0.22 avg60=0.17 avg300=1.11 total=58761459
 *  full avg10=0.23 avg60=0.16 avg300=1.08 total=58464525
 */
పూర్ణాంకం read_resource_pressure(స్థిరరాశి అక్షరం *మార్గం, PressureType type, ResourcePressure *ret);

/* Was the kernel compiled with CONFIG_PSI=y? 1 if yes, 0 if not, negative on error. */
పూర్ణాంకం is_pressure_supported(శూన్య);

/* Metadata for each pressure resource type, for use in sd-event and PID 1 */
రకనిర్వచనం నిర్మాణం PressureResourceInfo {
        స్థిరరాశి అక్షరం *name;       /* "memory", "cpu", "io" */
        స్థిరరాశి అక్షరం *env_watch;  /* "MEMORY_PRESSURE_WATCH", etc. */
        స్థిరరాశి అక్షరం *env_write;  /* "MEMORY_PRESSURE_WRITE", etc. */
} PressureResourceInfo;

బాహ్య స్థిరరాశి PressureResourceInfo pressure_resource_info[_PRESSURE_RESOURCE_MAX];

స్థిర అంతఃవరుస స్థిరరాశి PressureResourceInfo* pressure_resource_get_info(PressureResource resource) {
        ప్రతిజ్ఞా(resource >= 0 && resource < _PRESSURE_RESOURCE_MAX);
        తిరిగి &pressure_resource_info[resource];
}

DECLARE_STRING_పట్టిక_LOOKUP(pressure_resource, PressureResource);
DECLARE_STRING_పట్టిక_LOOKUP(pressure_type, PressureType);

/* Default parameters for pressure watch logic in sd-event and PID 1 */
#నిర్వచించు PRESSURE_DEFAULT_TYPE "some"
#నిర్వచించు PRESSURE_DEFAULT_పరిమితి_USEC (200 * USEC_చొప్పున_MSEC)
#నిర్వచించు PRESSURE_DEFAULT_కిటికీ_USEC (2 * USEC_చొప్పున_SEC)
