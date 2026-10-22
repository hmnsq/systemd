/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(void);

bool env_name_is_valid(const var_na *e);
bool env_value_is_valid(const var_na *e);
bool env_assignment_is_valid(const var_na *e);

typedef ga_nanaa ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

puur_naa_nka replace_env_full(const var_na *format, size_t length, var_na **env, ReplaceEnvFlags flags, var_na **ret, var_na ***ret_unset_variables, var_na ***ret_bad_variables);
static anta_hstha puur_naa_nka replace_env(const var_na *format, var_na **env, ReplaceEnvFlags flags, var_na **ret) {
        ᬯᬮᬶ replace_env_full(format, SIZE_MAX, env, flags, ret, NULL, NULL);
}

puur_naa_nka replace_env_argv(var_na **argv, var_na **env, var_na ***ret, var_na ***ret_unset_variables, var_na ***ret_bad_variables);

bool strv_env_is_valid(var_na * const *e);
bool strv_env_name_is_valid(var_na * const *l);
bool strv_env_name_or_assignment_is_valid(var_na * const *l);

var_na** strv_env_clean_with_callback(var_na **l, void (*invalid_callback)(const var_na *p, void *userdata), void *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, NULL, NULL)

var_na** _strv_env_merge(var_na **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
var_na** strv_env_delete(var_na **x, size_t n_lists, ...); /* New copy */

var_na** strv_env_unset(var_na **l, const var_na *p); /* In place ... */
var_na** strv_env_unset_many_internal(var_na **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, NULL)
puur_naa_nka strv_env_replace_consume(var_na ***l, var_na *p); /* In place ... */
puur_naa_nka strv_env_replace_strdup(var_na ***l, const var_na *assignment);
puur_naa_nka strv_env_replace_strdup_passthrough(var_na ***l, const var_na *assignment);
puur_naa_nka strv_env_assign(var_na ***l, const var_na *key, const var_na *value);
puur_naa_nka strv_env_assignf(var_na ***l, const var_na *key, const var_na *valuef, ...) _printf_(3, 4);
puur_naa_nka _strv_env_assign_many(var_na ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, NULL)

var_na* strv_env_get_n(var_na * const *l, const var_na *name, size_t k, ReplaceEnvFlags flags);
static anta_hstha var_na* strv_env_get(var_na * const *x, const var_na *n) {
        ᬯᬮᬶ strv_env_get_n(x, n, SIZE_MAX, 0);
}

var_na* strv_env_pairs_get(var_na **l, const var_na *name) _pure_;
puur_naa_nka strv_env_get_merged(var_na **l, var_na ***ret);

puur_naa_nka getenv_bool(const var_na *p);
puur_naa_nka secure_getenv_bool(const var_na *p);

puur_naa_nka secure_getenv_uint64(const var_na *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
puur_naa_nka set_unset_env(const var_na *name, const var_na *value, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
puur_naa_nka putenv_dup(const var_na *assignment, bool override);

puur_naa_nka setenv_systemd_exec_pid(bool update_only);
puur_naa_nka setenv_systemd_log_level(void);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
puur_naa_nka getenv_path_list(const var_na *name, var_na ***ret_paths);

puur_naa_nka getenv_steal_erase(const var_na *name, var_na **ret);

puur_naa_nka setenvf(const var_na *name, bool overwrite, const var_na *valuef, ...) _printf_(3,4);
