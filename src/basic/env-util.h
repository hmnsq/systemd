/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(శూన్య);

bool env_name_is_valid(స్థిరరాశి అక్షరం *e);
bool env_విలువ_is_valid(స్థిరరాశి అక్షరం *e);
bool env_assignment_is_valid(స్థిరరాశి అక్షరం *e);

రకనిర్వచనం గణన ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

పూర్ణాంకం replace_env_నిండిన(స్థిరరాశి అక్షరం *format, size_t length, అక్షరం **env, ReplaceEnvFlags flags, అక్షరం **ret, అక్షరం ***ret_unset_variables, అక్షరం ***ret_bad_variables);
స్థిర అంతఃవరుస పూర్ణాంకం replace_env(స్థిరరాశి అక్షరం *format, అక్షరం **env, ReplaceEnvFlags flags, అక్షరం **ret) {
        తిరిగి replace_env_నిండిన(format, SIZE_MAX, env, flags, ret, ఖాళీ, ఖాళీ);
}

పూర్ణాంకం replace_env_argv(అక్షరం **argv, అక్షరం **env, అక్షరం ***ret, అక్షరం ***ret_unset_variables, అక్షరం ***ret_bad_variables);

bool strv_env_is_valid(అక్షరం * స్థిరరాశి *e);
bool strv_env_name_is_valid(అక్షరం * స్థిరరాశి *l);
bool strv_env_name_or_assignment_is_valid(అక్షరం * స్థిరరాశి *l);

అక్షరం** strv_env_clean_with_callback(అక్షరం **l, శూన్య (*invalid_callback)(స్థిరరాశి అక్షరం *p, శూన్య *userdata), శూన్య *userdata);
#నిర్వచించు strv_env_clean(l) strv_env_clean_with_callback(l, ఖాళీ, ఖాళీ)

అక్షరం** strv_env_విలీనం(అక్షరం **మొదటి, ...);
#నిర్వచించు strv_env_విలీనం_2(మొదటి, ...) strv_env_విలీనం(మొదటి, __VA_ARGS__, సూచిక_MAX)
అక్షరం** strv_env_delete(అక్షరం **x, size_t n_lists, ...); /* New copy */

అక్షరం** strv_env_unset(అక్షరం **l, స్థిరరాశి అక్షరం *p); /* In place ... */
అక్షరం** strv_env_unset_many_అంతర్గత(అక్షరం **l, ...) _sentinel_;
#నిర్వచించు strv_env_unset_many(l, ...) strv_env_unset_many_అంతర్గత(l, __VA_ARGS__, ఖాళీ)
పూర్ణాంకం strv_env_replace_consume(అక్షరం ***l, అక్షరం *p); /* In place ... */
పూర్ణాంకం strv_env_replace_strdup(అక్షరం ***l, స్థిరరాశి అక్షరం *assignment);
పూర్ణాంకం strv_env_replace_strdup_passthrough(అక్షరం ***l, స్థిరరాశి అక్షరం *assignment);
పూర్ణాంకం strv_env_కేటాయించు(అక్షరం ***l, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);
పూర్ణాంకం strv_env_assignf(అక్షరం ***l, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *valuef, ...) _printf_(3, 4);
పూర్ణాంకం strv_env_కేటాయించు_many(అక్షరం ***l, ...) _sentinel_;
#నిర్వచించు strv_env_కేటాయించు_many_2(l, ...) strv_env_కేటాయించు_many(l, __VA_ARGS__, ఖాళీ)

అక్షరం* strv_env_get_n(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name, size_t k, ReplaceEnvFlags flags);
స్థిర అంతఃవరుస అక్షరం* strv_env_get(అక్షరం * స్థిరరాశి *x, స్థిరరాశి అక్షరం *n) {
        తిరిగి strv_env_get_n(x, n, SIZE_MAX, 0);
}

అక్షరం* strv_env_జతలు_get(అక్షరం **l, స్థిరరాశి అక్షరం *name) శుద్ధ;
పూర్ణాంకం strv_env_get_merged(అక్షరం **l, అక్షరం ***ret);

పూర్ణాంకం getenv_bool(స్థిరరాశి అక్షరం *p);
పూర్ణాంకం secure_getenv_bool(స్థిరరాశి అక్షరం *p);

పూర్ణాంకం secure_getenv_uint64(స్థిరరాశి అక్షరం *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
పూర్ణాంకం సమితి_unset_env(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *విలువ, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
పూర్ణాంకం putenv_dup(స్థిరరాశి అక్షరం *assignment, bool override);

పూర్ణాంకం setenv_systemd_exec_pid(bool update_only);
పూర్ణాంకం setenv_systemd_log_level(శూన్య);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
పూర్ణాంకం getenv_మార్గం_list(స్థిరరాశి అక్షరం *name, అక్షరం ***ret_paths);

పూర్ణాంకం getenv_steal_erase(స్థిరరాశి అక్షరం *name, అక్షరం **ret);

పూర్ణాంకం setenvf(స్థిరరాశి అక్షరం *name, bool overwrite, స్థిరరాశి అక్షరం *valuef, ...) _printf_(3,4);
