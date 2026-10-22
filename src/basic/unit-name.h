/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు UNIT_NAME_MAX 256

రకనిర్వచనం గణన UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(స్థిరరాశి అక్షరం *n, UnitNameFlags flags) శుద్ధ;
bool unit_ఉపసర్గ_is_valid(స్థిరరాశి అక్షరం *p) శుద్ధ;
bool unit_instance_is_valid(స్థిరరాశి అక్షరం *i) శుద్ధ;
bool unit_suffix_is_valid(స్థిరరాశి అక్షరం *s) శుద్ధ;

పూర్ణాంకం unit_name_గమ్యం_ఉపసర్గ(స్థిరరాశి అక్షరం *n, అక్షరం **ret);
UnitNameFlags unit_name_గమ్యం_instance(స్థిరరాశి అక్షరం *n, అక్షరం **ret);
స్థిర అంతఃవరుస UnitNameFlags unit_name_classify(స్థిరరాశి అక్షరం *n) {
        తిరిగి unit_name_గమ్యం_instance(n, ఖాళీ);
}
పూర్ణాంకం unit_name_గమ్యం_ఉపసర్గ_and_instance(స్థిరరాశి అక్షరం *n, అక్షరం **ret);

UnitType unit_name_గమ్యం_type(స్థిరరాశి అక్షరం *n) శుద్ధ;

పూర్ణాంకం unit_name_change_suffix(స్థిరరాశి అక్షరం *n, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret);

పూర్ణాంకం unit_name_build(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *instance, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret);
పూర్ణాంకం unit_name_build_from_type(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *instance, UnitType type, అక్షరం **ret);

అక్షరం* unit_name_escape(స్థిరరాశి అక్షరం *f);
పూర్ణాంకం unit_name_unescape(స్థిరరాశి అక్షరం *f, అక్షరం **ret);
పూర్ణాంకం unit_name_మార్గం_escape(స్థిరరాశి అక్షరం *f, అక్షరం **ret);
పూర్ణాంకం unit_name_మార్గం_unescape(స్థిరరాశి అక్షరం *f, అక్షరం **ret);

పూర్ణాంకం unit_name_replace_instance_నిండిన(
                స్థిరరాశి అక్షరం *original,
                స్థిరరాశి అక్షరం *instance,
                bool accept_glob,
                అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం unit_name_replace_instance(స్థిరరాశి అక్షరం *original, స్థిరరాశి అక్షరం *instance, అక్షరం **ret) {
        తిరిగి unit_name_replace_instance_నిండిన(original, instance, false, ret);
}

పూర్ణాంకం unit_name_template(స్థిరరాశి అక్షరం *f, అక్షరం **ret);

పూర్ణాంకం unit_name_hash_long(స్థిరరాశి అక్షరం *name, అక్షరం **ret);
bool unit_name_is_hashed(స్థిరరాశి అక్షరం *name);

పూర్ణాంకం unit_name_from_మార్గం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret);
పూర్ణాంకం unit_name_from_మార్గం_instance(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret);
పూర్ణాంకం unit_name_గమ్యం_మార్గం(స్థిరరాశి అక్షరం *name, అక్షరం **ret);

రకనిర్వచనం గణన UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

పూర్ణాంకం unit_name_mangle_with_suffix(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *operation, UnitNameMangle flags, స్థిరరాశి అక్షరం *suffix, అక్షరం **ret);

స్థిర అంతఃవరుస పూర్ణాంకం unit_name_mangle(స్థిరరాశి అక్షరం *name, UnitNameMangle flags, అక్షరం **ret) {
        తిరిగి unit_name_mangle_with_suffix(name, ఖాళీ, flags, ".service", ret);
}

పూర్ణాంకం ముక్క_build_మాతృ_ముక్క(స్థిరరాశి అక్షరం *ముక్క, అక్షరం **ret);
పూర్ణాంకం ముక్క_build_subslice(స్థిరరాశి అక్షరం *ముక్క, స్థిరరాశి అక్షరం *name, అక్షరం **ret);
bool ముక్క_name_is_valid(స్థిరరాశి అక్షరం *name);

bool unit_name_ఉపసర్గ_equal(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b);
