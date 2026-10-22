/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(શૂન્ય);

bool env_name_is_valid(અચલ અક્ષર *e);
bool env_કિંમત_is_valid(અચલ અક્ષર *e);
bool env_assignment_is_valid(અચલ અક્ષર *e);

પ્રકારવ્યાખ્યા ગણના ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

પૂર્ણાંક replace_env_full(અચલ અક્ષર *format, size_t length, અક્ષર **env, ReplaceEnvFlags flags, અક્ષર **ret, અક્ષર ***ret_unset_variables, અક્ષર ***ret_bad_variables);
સ્થિર અંતઃરેખ પૂર્ણાંક replace_env(અચલ અક્ષર *format, અક્ષર **env, ReplaceEnvFlags flags, અક્ષર **ret) {
        પાછા replace_env_full(format, SIZE_MAX, env, flags, ret, ખાલી, ખાલી);
}

પૂર્ણાંક replace_env_argv(અક્ષર **argv, અક્ષર **env, અક્ષર ***ret, અક્ષર ***ret_unset_variables, અક્ષર ***ret_bad_variables);

bool strv_env_is_valid(અક્ષર * અચલ *e);
bool strv_env_name_is_valid(અક્ષર * અચલ *l);
bool strv_env_name_or_assignment_is_valid(અક્ષર * અચલ *l);

અક્ષર** strv_env_clean_with_callback(અક્ષર **l, શૂન્ય (*invalid_callback)(અચલ અક્ષર *p, શૂન્ય *userdata), શૂન્ય *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, ખાલી, ખાલી)

અક્ષર** _strv_env_merge(અક્ષર **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
અક્ષર** strv_env_delete(અક્ષર **x, size_t n_lists, ...); /* New copy */

અક્ષર** strv_env_unset(અક્ષર **l, અચલ અક્ષર *p); /* In place ... */
અક્ષર** strv_env_unset_many_internal(અક્ષર **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, ખાલી)
પૂર્ણાંક strv_env_replace_consume(અક્ષર ***l, અક્ષર *p); /* In place ... */
પૂર્ણાંક strv_env_replace_strdup(અક્ષર ***l, અચલ અક્ષર *assignment);
પૂર્ણાંક strv_env_replace_strdup_passthrough(અક્ષર ***l, અચલ અક્ષર *assignment);
પૂર્ણાંક strv_env_assign(અક્ષર ***l, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);
પૂર્ણાંક strv_env_assignf(અક્ષર ***l, અચલ અક્ષર *ચાવી, અચલ અક્ષર *valuef, ...) _printf_(3, 4);
પૂર્ણાંક _strv_env_assign_many(અક્ષર ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, ખાલી)

અક્ષર* strv_env_get_n(અક્ષર * અચલ *l, અચલ અક્ષર *name, size_t k, ReplaceEnvFlags flags);
સ્થિર અંતઃરેખ અક્ષર* strv_env_get(અક્ષર * અચલ *x, અચલ અક્ષર *n) {
        પાછા strv_env_get_n(x, n, SIZE_MAX, 0);
}

અક્ષર* strv_env_pairs_get(અક્ષર **l, અચલ અક્ષર *name) _pure_;
પૂર્ણાંક strv_env_get_merged(અક્ષર **l, અક્ષર ***ret);

પૂર્ણાંક getenv_bool(અચલ અક્ષર *p);
પૂર્ણાંક secure_getenv_bool(અચલ અક્ષર *p);

પૂર્ણાંક secure_getenv_uint64(અચલ અક્ષર *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
પૂર્ણાંક set_unset_env(અચલ અક્ષર *name, અચલ અક્ષર *કિંમત, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
પૂર્ણાંક putenv_dup(અચલ અક્ષર *assignment, bool override);

પૂર્ણાંક setenv_systemd_exec_pid(bool update_only);
પૂર્ણાંક setenv_systemd_log_level(શૂન્ય);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
પૂર્ણાંક getenv_path_list(અચલ અક્ષર *name, અક્ષર ***ret_paths);

પૂર્ણાંક getenv_steal_erase(અચલ અક્ષર *name, અક્ષર **ret);

પૂર્ણાંક setenvf(અચલ અક્ષર *name, bool overwrite, અચલ અક્ષર *valuef, ...) _printf_(3,4);
