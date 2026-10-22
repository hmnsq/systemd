/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(تهی);

bool env_name_is_valid(ثابت نویسه *e);
bool env_مقدار_is_valid(ثابت نویسه *e);
bool env_assignment_is_valid(ثابت نویسه *e);

تعریفنوع شمارش ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

صحیح replace_env_full(ثابت نویسه *format, size_t length, نویسه **env, ReplaceEnvFlags flags, نویسه **ret, نویسه ***ret_unset_variables, نویسه ***ret_bad_variables);
ایستا درونخطی صحیح replace_env(ثابت نویسه *format, نویسه **env, ReplaceEnvFlags flags, نویسه **ret) {
        بازگشت replace_env_full(format, SIZE_MAX, env, flags, ret, پوچ, پوچ);
}

صحیح replace_env_argv(نویسه **argv, نویسه **env, نویسه ***ret, نویسه ***ret_unset_variables, نویسه ***ret_bad_variables);

bool strv_env_is_valid(نویسه * ثابت *e);
bool strv_env_name_is_valid(نویسه * ثابت *l);
bool strv_env_name_or_assignment_is_valid(نویسه * ثابت *l);

نویسه** strv_env_clean_with_callback(نویسه **l, تهی (*invalid_callback)(ثابت نویسه *p, تهی *userdata), تهی *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, پوچ, پوچ)

نویسه** _strv_env_merge(نویسه **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
نویسه** strv_env_delete(نویسه **x, size_t n_lists, ...); /* New copy */

نویسه** strv_env_unset(نویسه **l, ثابت نویسه *p); /* In place ... */
نویسه** strv_env_unset_many_internal(نویسه **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, پوچ)
صحیح strv_env_replace_consume(نویسه ***l, نویسه *p); /* In place ... */
صحیح strv_env_replace_strdup(نویسه ***l, ثابت نویسه *assignment);
صحیح strv_env_replace_strdup_passthrough(نویسه ***l, ثابت نویسه *assignment);
صحیح strv_env_assign(نویسه ***l, ثابت نویسه *کلید, ثابت نویسه *مقدار);
صحیح strv_env_assignf(نویسه ***l, ثابت نویسه *کلید, ثابت نویسه *valuef, ...) _printf_(3, 4);
صحیح _strv_env_assign_many(نویسه ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, پوچ)

نویسه* strv_env_get_n(نویسه * ثابت *l, ثابت نویسه *name, size_t k, ReplaceEnvFlags flags);
ایستا درونخطی نویسه* strv_env_get(نویسه * ثابت *x, ثابت نویسه *n) {
        بازگشت strv_env_get_n(x, n, SIZE_MAX, 0);
}

نویسه* strv_env_pairs_get(نویسه **l, ثابت نویسه *name) _pure_;
صحیح strv_env_get_merged(نویسه **l, نویسه ***ret);

صحیح getenv_bool(ثابت نویسه *p);
صحیح secure_getenv_bool(ثابت نویسه *p);

صحیح secure_getenv_uint64(ثابت نویسه *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
صحیح set_unset_env(ثابت نویسه *name, ثابت نویسه *مقدار, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
صحیح putenv_dup(ثابت نویسه *assignment, bool override);

صحیح setenv_systemd_exec_pid(bool update_only);
صحیح setenv_systemd_log_level(تهی);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
صحیح getenv_path_list(ثابت نویسه *name, نویسه ***ret_paths);

صحیح getenv_steal_erase(ثابت نویسه *name, نویسه **ret);

صحیح setenvf(ثابت نویسه *name, bool overwrite, ثابت نویسه *valuef, ...) _printf_(3,4);
