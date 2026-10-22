/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा ENVIRONMENT_ASSIGNMENTS_MAX 16384U

माप_प्रकार sc_arg_max(व्योम);

bool env_name_is_valid(स्थिर अक्षर *e);
bool env_मूल्य_is_valid(स्थिर अक्षर *e);
bool env_assignment_is_valid(स्थिर अक्षर *e);

प्रकार क्रमागत ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

पूर्णांक replace_env_full(स्थिर अक्षर *format, माप_प्रकार length, अक्षर **env, ReplaceEnvFlags flags, अक्षर **ret, अक्षर ***ret_unset_variables, अक्षर ***ret_bad_variables);
अटल अंतरभूत पूर्णांक replace_env(स्थिर अक्षर *format, अक्षर **env, ReplaceEnvFlags flags, अक्षर **ret) {
        वापस replace_env_full(format, SIZE_MAX, env, flags, ret, शून्य, शून्य);
}

पूर्णांक replace_env_argv(अक्षर **argv, अक्षर **env, अक्षर ***ret, अक्षर ***ret_unset_variables, अक्षर ***ret_bad_variables);

bool strv_env_is_valid(अक्षर * स्थिर *e);
bool strv_env_name_is_valid(अक्षर * स्थिर *l);
bool strv_env_name_or_assignment_is_valid(अक्षर * स्थिर *l);

अक्षर** strv_env_clean_with_callback(अक्षर **l, व्योम (*invalid_callback)(स्थिर अक्षर *p, व्योम *userdata), व्योम *userdata);
#घोषणा strv_env_clean(l) strv_env_clean_with_callback(l, शून्य, शून्य)

अक्षर** _strv_env_merge(अक्षर **first, ...);
#घोषणा strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
अक्षर** strv_env_delete(अक्षर **x, माप_प्रकार n_lists, ...); /* New copy */

अक्षर** strv_env_unset(अक्षर **l, स्थिर अक्षर *p); /* In place ... */
अक्षर** strv_env_unset_many_internal(अक्षर **l, ...) _sentinel_;
#घोषणा strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, शून्य)
पूर्णांक strv_env_replace_consume(अक्षर ***l, अक्षर *p); /* In place ... */
पूर्णांक strv_env_replace_strdup(अक्षर ***l, स्थिर अक्षर *assignment);
पूर्णांक strv_env_replace_strdup_passthrough(अक्षर ***l, स्थिर अक्षर *assignment);
पूर्णांक strv_env_assign(अक्षर ***l, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);
पूर्णांक strv_env_assignf(अक्षर ***l, स्थिर अक्षर *कुंजी, स्थिर अक्षर *valuef, ...) _printf_(3, 4);
पूर्णांक _strv_env_assign_many(अक्षर ***l, ...) _sentinel_;
#घोषणा strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, शून्य)

अक्षर* strv_env_get_n(अक्षर * स्थिर *l, स्थिर अक्षर *name, माप_प्रकार k, ReplaceEnvFlags flags);
अटल अंतरभूत अक्षर* strv_env_get(अक्षर * स्थिर *x, स्थिर अक्षर *n) {
        वापस strv_env_get_n(x, n, SIZE_MAX, 0);
}

अक्षर* strv_env_pairs_get(अक्षर **l, स्थिर अक्षर *name) _pure_;
पूर्णांक strv_env_get_merged(अक्षर **l, अक्षर ***ret);

पूर्णांक getenv_bool(स्थिर अक्षर *p);
पूर्णांक secure_getenv_bool(स्थिर अक्षर *p);

पूर्णांक secure_getenv_uint64(स्थिर अक्षर *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
पूर्णांक set_unset_env(स्थिर अक्षर *name, स्थिर अक्षर *मूल्य, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
पूर्णांक putenv_dup(स्थिर अक्षर *assignment, bool override);

पूर्णांक setenv_systemd_exec_pid(bool update_only);
पूर्णांक setenv_systemd_log_level(व्योम);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
पूर्णांक getenv_path_list(स्थिर अक्षर *name, अक्षर ***ret_paths);

पूर्णांक getenv_steal_erase(स्थिर अक्षर *name, अक्षर **ret);

पूर्णांक setenvf(स्थिर अक्षर *name, bool overwrite, स्थिर अक्षर *valuef, ...) _printf_(3,4);
