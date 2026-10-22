/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "forward.h"

స్థిరరాశి అక్షరం* string_పట్టిక_lookup_గమ్యం_string(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, ssize_t i) _const_;

ssize_t string_పట్టిక_lookup_from_string(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, స్థిరరాశి అక్షరం *కీలకం) శుద్ధ;
ssize_t string_పట్టిక_lookup_from_string_with_boolean(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, స్థిరరాశి అక్షరం *కీలకం, ssize_t yes) శుద్ధ;

పూర్ణాంకం string_పట్టిక_lookup_గమ్యం_string_fallback(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, ssize_t i, size_t max, అక్షరం **ret);
ssize_t string_పట్టిక_lookup_from_string_fallback(స్థిరరాశి అక్షరం * స్థిరరాశి *పట్టిక, size_t len, స్థిరరాశి అక్షరం *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_2(name, type, scope)                                        \
        scope స్థిరరాశి అక్షరం* name##_to_string(type i) {                                                    \
                తిరిగి string_పట్టిక_lookup_గమ్యం_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(స్థిరరాశి అక్షరం *s) {             \
                తిరిగి (type) string_పట్టిక_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(స్థిరరాశి అక్షరం *s) {                               \
                తిరిగి (type) string_పట్టిక_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_FALLBACK(name, type, max, scope)                          \
        scope పూర్ణాంకం name##_to_string_alloc(type i, అక్షరం **ret) {                                          \
                తిరిగి string_పట్టిక_lookup_గమ్యం_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(స్థిరరాశి అక్షరం *s) {                           \
                తిరిగి (type) string_పట్టిక_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_2(name, type, scope)                    \
        DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_2(name, type, scope)          \
        DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING(name, type, scope)

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_WITH_BOOLEAN_2(name, type, yes, scope)  \
        DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_2(name, type, scope)          \
        DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP(name, type) DEFINE_STRING_పట్టిక_LOOKUP_2(name, type,)
#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(name, type) DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_2(name, type,)
#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_2(name, type) DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING(name, type,)
#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP(name, type) DEFINE_STRING_పట్టిక_LOOKUP_2(name, type, స్థిర)
#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(name, type) DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_2(name, type, స్థిర)
#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_FROM_STRING(name, type) DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING(name, type, స్థిర)

#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_పట్టిక_LOOKUP_WITH_BOOLEAN_2(name, type, yes,)
#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_పట్టిక_LOOKUP_WITH_BOOLEAN_2(name, type, yes, స్థిర)
#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, స్థిర)

/* For string conversions where numbers are also acceptable */
#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(name, type, max)         \
        DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_FALLBACK(name, type, max,)  \
        DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#నిర్వచించు DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING_FALLBACK(name, type, max, స్థిర)
#నిర్వచించు DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_పట్టిక_LOOKUP_FROM_STRING_FALLBACK(name, type, max, స్థిర)

#నిర్వచించు DUMP_STRING_పట్టిక(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                కొరకు (type _k = 0; _k < (max); _k++) {                   \
                        స్థిరరాశి అక్షరం *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        ఒకవేళ (!_t)                                        \
                                కొనసాగు;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
