/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

माप_प्रकार sc_arg_max(शून्य);

bool env_name_is_valid(अचर वर्ण *e);
bool env_मान_is_valid(अचर वर्ण *e);
bool env_assignment_is_valid(अचर वर्ण *e);

प्रकारनिर्देश गणना ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

पूर्णाङ्क replace_env_full(अचर वर्ण *format, माप_प्रकार length, वर्ण **env, ReplaceEnvFlags flags, वर्ण **ret, वर्ण ***ret_unset_variables, वर्ण ***ret_bad_variables);
स्थिर अन्तःस्थ पूर्णाङ्क replace_env(अचर वर्ण *format, वर्ण **env, ReplaceEnvFlags flags, वर्ण **ret) {
        फर्काऊ replace_env_full(format, SIZE_MAX, env, flags, ret, रिक्त, रिक्त);
}

पूर्णाङ्क replace_env_argv(वर्ण **argv, वर्ण **env, वर्ण ***ret, वर्ण ***ret_unset_variables, वर्ण ***ret_bad_variables);

bool strv_env_is_valid(वर्ण * अचर *e);
bool strv_env_name_is_valid(वर्ण * अचर *l);
bool strv_env_name_or_assignment_is_valid(वर्ण * अचर *l);

वर्ण** strv_env_clean_with_callback(वर्ण **l, शून्य (*invalid_callback)(अचर वर्ण *p, शून्य *userdata), शून्य *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, रिक्त, रिक्त)

वर्ण** _strv_env_merge(वर्ण **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
वर्ण** strv_env_delete(वर्ण **x, माप_प्रकार n_lists, ...); /* New copy */

वर्ण** strv_env_unset(वर्ण **l, अचर वर्ण *p); /* In place ... */
वर्ण** strv_env_unset_many_internal(वर्ण **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, रिक्त)
पूर्णाङ्क strv_env_replace_consume(वर्ण ***l, वर्ण *p); /* In place ... */
पूर्णाङ्क strv_env_replace_strdup(वर्ण ***l, अचर वर्ण *assignment);
पूर्णाङ्क strv_env_replace_strdup_passthrough(वर्ण ***l, अचर वर्ण *assignment);
पूर्णाङ्क strv_env_assign(वर्ण ***l, अचर वर्ण *साँचो, अचर वर्ण *मान);
पूर्णाङ्क strv_env_assignf(वर्ण ***l, अचर वर्ण *साँचो, अचर वर्ण *valuef, ...) _printf_(3, 4);
पूर्णाङ्क _strv_env_assign_many(वर्ण ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, रिक्त)

वर्ण* strv_env_get_n(वर्ण * अचर *l, अचर वर्ण *name, माप_प्रकार k, ReplaceEnvFlags flags);
स्थिर अन्तःस्थ वर्ण* strv_env_get(वर्ण * अचर *x, अचर वर्ण *n) {
        फर्काऊ strv_env_get_n(x, n, SIZE_MAX, 0);
}

वर्ण* strv_env_pairs_get(वर्ण **l, अचर वर्ण *name) _pure_;
पूर्णाङ्क strv_env_get_merged(वर्ण **l, वर्ण ***ret);

पूर्णाङ्क getenv_bool(अचर वर्ण *p);
पूर्णाङ्क secure_getenv_bool(अचर वर्ण *p);

पूर्णाङ्क secure_getenv_uint64(अचर वर्ण *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
पूर्णाङ्क set_unset_env(अचर वर्ण *name, अचर वर्ण *मान, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
पूर्णाङ्क putenv_dup(अचर वर्ण *assignment, bool override);

पूर्णाङ्क setenv_systemd_exec_pid(bool update_only);
पूर्णाङ्क setenv_systemd_log_level(शून्य);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
पूर्णाङ्क getenv_path_list(अचर वर्ण *name, वर्ण ***ret_paths);

पूर्णाङ्क getenv_steal_erase(अचर वर्ण *name, वर्ण **ret);

पूर्णाङ्क setenvf(अचर वर्ण *name, bool overwrite, अचर वर्ण *valuef, ...) _printf_(3,4);
