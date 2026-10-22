/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

माप_प्रकार sc_arg_max(शून्य);

bool env_name_is_valid(स्थिरांक अक्षर *e);
bool env_मूल्य_is_valid(स्थिरांक अक्षर *e);
bool env_assignment_is_valid(स्थिरांक अक्षर *e);

प्रकारव्याख्या गणना ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

पूर्णांक replace_env_full(स्थिरांक अक्षर *format, माप_प्रकार length, अक्षर **env, ReplaceEnvFlags flags, अक्षर **ret, अक्षर ***ret_unset_variables, अक्षर ***ret_bad_variables);
स्थिर ओळीत पूर्णांक replace_env(स्थिरांक अक्षर *format, अक्षर **env, ReplaceEnvFlags flags, अक्षर **ret) {
        परत replace_env_full(format, SIZE_MAX, env, flags, ret, रिक्त, रिक्त);
}

पूर्णांक replace_env_argv(अक्षर **argv, अक्षर **env, अक्षर ***ret, अक्षर ***ret_unset_variables, अक्षर ***ret_bad_variables);

bool strv_env_is_valid(अक्षर * स्थिरांक *e);
bool strv_env_name_is_valid(अक्षर * स्थिरांक *l);
bool strv_env_name_or_assignment_is_valid(अक्षर * स्थिरांक *l);

अक्षर** strv_env_clean_with_callback(अक्षर **l, शून्य (*invalid_callback)(स्थिरांक अक्षर *p, शून्य *userdata), शून्य *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, रिक्त, रिक्त)

अक्षर** _strv_env_merge(अक्षर **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
अक्षर** strv_env_delete(अक्षर **x, माप_प्रकार n_lists, ...); /* New copy */

अक्षर** strv_env_unset(अक्षर **l, स्थिरांक अक्षर *p); /* In place ... */
अक्षर** strv_env_unset_many_internal(अक्षर **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, रिक्त)
पूर्णांक strv_env_replace_consume(अक्षर ***l, अक्षर *p); /* In place ... */
पूर्णांक strv_env_replace_strdup(अक्षर ***l, स्थिरांक अक्षर *assignment);
पूर्णांक strv_env_replace_strdup_passthrough(अक्षर ***l, स्थिरांक अक्षर *assignment);
पूर्णांक strv_env_assign(अक्षर ***l, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य);
पूर्णांक strv_env_assignf(अक्षर ***l, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *valuef, ...) _printf_(3, 4);
पूर्णांक _strv_env_assign_many(अक्षर ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, रिक्त)

अक्षर* strv_env_get_n(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name, माप_प्रकार k, ReplaceEnvFlags flags);
स्थिर ओळीत अक्षर* strv_env_get(अक्षर * स्थिरांक *x, स्थिरांक अक्षर *n) {
        परत strv_env_get_n(x, n, SIZE_MAX, 0);
}

अक्षर* strv_env_pairs_get(अक्षर **l, स्थिरांक अक्षर *name) _pure_;
पूर्णांक strv_env_get_merged(अक्षर **l, अक्षर ***ret);

पूर्णांक getenv_bool(स्थिरांक अक्षर *p);
पूर्णांक secure_getenv_bool(स्थिरांक अक्षर *p);

पूर्णांक secure_getenv_uint64(स्थिरांक अक्षर *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
पूर्णांक set_unset_env(स्थिरांक अक्षर *name, स्थिरांक अक्षर *मूल्य, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
पूर्णांक putenv_dup(स्थिरांक अक्षर *assignment, bool override);

पूर्णांक setenv_systemd_exec_pid(bool update_only);
पूर्णांक setenv_systemd_log_level(शून्य);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
पूर्णांक getenv_path_list(स्थिरांक अक्षर *name, अक्षर ***ret_paths);

पूर्णांक getenv_steal_erase(स्थिरांक अक्षर *name, अक्षर **ret);

पूर्णांक setenvf(स्थिरांक अक्षर *name, bool overwrite, स्थिरांक अक्षर *valuef, ...) _printf_(3,4);
