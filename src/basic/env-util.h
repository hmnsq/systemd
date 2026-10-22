/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#परिभाषय ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(शून्य);

bool env_name_is_valid(ध्रुव वर्ण *e);
bool env_मानम्_is_valid(ध्रुव वर्ण *e);
bool env_assignment_is_valid(ध्रुव वर्ण *e);

प्रकारनिर्देश गणना ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

पूर्णाङ्क replace_env_पूर्णम्(ध्रुव वर्ण *format, size_t length, वर्ण **env, ReplaceEnvFlags flags, वर्ण **ret, वर्ण ***ret_unset_variables, वर्ण ***ret_bad_variables);
स्थिर अन्तःस्थ पूर्णाङ्क replace_env(ध्रुव वर्ण *format, वर्ण **env, ReplaceEnvFlags flags, वर्ण **ret) {
        प्रत्यागम replace_env_पूर्णम्(format, SIZE_MAX, env, flags, ret, रिक्त, रिक्त);
}

पूर्णाङ्क replace_env_argv(वर्ण **argv, वर्ण **env, वर्ण ***ret, वर्ण ***ret_unset_variables, वर्ण ***ret_bad_variables);

bool strv_env_is_valid(वर्ण * ध्रुव *e);
bool strv_env_name_is_valid(वर्ण * ध्रुव *l);
bool strv_env_name_or_assignment_is_valid(वर्ण * ध्रुव *l);

वर्ण** strv_env_clean_with_callback(वर्ण **l, शून्य (*invalid_callback)(ध्रुव वर्ण *p, शून्य *userdata), शून्य *userdata);
#परिभाषय strv_env_clean(l) strv_env_clean_with_callback(l, रिक्त, रिक्त)

वर्ण** _strv_env_merge(वर्ण **first, ...);
#परिभाषय strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
वर्ण** strv_env_delete(वर्ण **x, size_t n_lists, ...); /* New copy */

वर्ण** strv_env_unset(वर्ण **l, ध्रुव वर्ण *p); /* In place ... */
वर्ण** strv_env_unset_many_आन्तरिकम्(वर्ण **l, ...) _sentinel_;
#परिभाषय strv_env_unset_many(l, ...) strv_env_unset_many_आन्तरिकम्(l, __VA_ARGS__, रिक्त)
पूर्णाङ्क strv_env_replace_consume(वर्ण ***l, वर्ण *p); /* In place ... */
पूर्णाङ्क strv_env_replace_strdup(वर्ण ***l, ध्रुव वर्ण *assignment);
पूर्णाङ्क strv_env_replace_strdup_passthrough(वर्ण ***l, ध्रुव वर्ण *assignment);
पूर्णाङ्क strv_env_नियोजय(वर्ण ***l, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);
पूर्णाङ्क strv_env_assignf(वर्ण ***l, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *valuef, ...) _printf_(3, 4);
पूर्णाङ्क strv_env_नियोजय_many(वर्ण ***l, ...) _sentinel_;
#परिभाषय strv_env_नियोजय_many_2(l, ...) strv_env_नियोजय_many(l, __VA_ARGS__, रिक्त)

वर्ण* strv_env_get_n(वर्ण * ध्रुव *l, ध्रुव वर्ण *name, size_t k, ReplaceEnvFlags flags);
स्थिर अन्तःस्थ वर्ण* strv_env_get(वर्ण * ध्रुव *x, ध्रुव वर्ण *n) {
        प्रत्यागम strv_env_get_n(x, n, SIZE_MAX, 0);
}

वर्ण* strv_env_युग्मानि_get(वर्ण **l, ध्रुव वर्ण *name) विशुद्धम्;
पूर्णाङ्क strv_env_get_merged(वर्ण **l, वर्ण ***ret);

पूर्णाङ्क getenv_bool(ध्रुव वर्ण *p);
पूर्णाङ्क secure_getenv_bool(ध्रुव वर्ण *p);

पूर्णाङ्क secure_getenv_uint64(ध्रुव वर्ण *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
पूर्णाङ्क समुच्चयः_unset_env(ध्रुव वर्ण *name, ध्रुव वर्ण *मानम्, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
पूर्णाङ्क putenv_dup(ध्रुव वर्ण *assignment, bool override);

पूर्णाङ्क setenv_systemd_exec_pid(bool update_only);
पूर्णाङ्क setenv_systemd_log_level(शून्य);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
पूर्णाङ्क getenv_मार्गः_list(ध्रुव वर्ण *name, वर्ण ***ret_paths);

पूर्णाङ्क getenv_steal_erase(ध्रुव वर्ण *name, वर्ण **ret);

पूर्णाङ्क setenvf(ध्रुव वर्ण *name, bool overwrite, ध्रुव वर्ण *valuef, ...) _printf_(3,4);
