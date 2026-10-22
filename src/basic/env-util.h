/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(ಶೂನ್ಯ);

bool env_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
bool env_ಮೌಲ್ಯ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
bool env_assignment_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

ಪೂರ್ಣಾಂಕ replace_env_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, size_t length, ಅಕ್ಷರ **env, ReplaceEnvFlags flags, ಅಕ್ಷರ **ret, ಅಕ್ಷರ ***ret_unset_variables, ಅಕ್ಷರ ***ret_bad_variables);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ replace_env(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ಅಕ್ಷರ **env, ReplaceEnvFlags flags, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ replace_env_ತುಂಬಿದ(format, SIZE_MAX, env, flags, ret, ಖಾಲಿ, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ replace_env_argv(ಅಕ್ಷರ **argv, ಅಕ್ಷರ **env, ಅಕ್ಷರ ***ret, ಅಕ್ಷರ ***ret_unset_variables, ಅಕ್ಷರ ***ret_bad_variables);

bool strv_env_is_valid(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *e);
bool strv_env_name_is_valid(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l);
bool strv_env_name_or_assignment_is_valid(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l);

ಅಕ್ಷರ** strv_env_clean_with_callback(ಅಕ್ಷರ **l, ಶೂನ್ಯ (*invalid_callback)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಶೂನ್ಯ *userdata), ಶೂನ್ಯ *userdata);
#ವ್ಯಾಖ್ಯಾನಿಸು strv_env_clean(l) strv_env_clean_with_callback(l, ಖಾಲಿ, ಖಾಲಿ)

ಅಕ್ಷರ** strv_env_ವಿಲೀನ(ಅಕ್ಷರ **ಮೊದಲ, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು strv_env_ವಿಲೀನ_2(ಮೊದಲ, ...) strv_env_ವಿಲೀನ(ಮೊದಲ, __VA_ARGS__, ಸೂಚಕ_MAX)
ಅಕ್ಷರ** strv_env_delete(ಅಕ್ಷರ **x, size_t n_lists, ...); /* New copy */

ಅಕ್ಷರ** strv_env_unset(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p); /* In place ... */
ಅಕ್ಷರ** strv_env_unset_many_ಆಂತರಿಕ(ಅಕ್ಷರ **l, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು strv_env_unset_many(l, ...) strv_env_unset_many_ಆಂತರಿಕ(l, __VA_ARGS__, ಖಾಲಿ)
ಪೂರ್ಣಾಂಕ strv_env_replace_consume(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *p); /* In place ... */
ಪೂರ್ಣಾಂಕ strv_env_replace_strdup(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *assignment);
ಪೂರ್ಣಾಂಕ strv_env_replace_strdup_passthrough(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *assignment);
ಪೂರ್ಣಾಂಕ strv_env_ನಿಯೋಜಿಸು(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ strv_env_assignf(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *valuef, ...) _printf_(3, 4);
ಪೂರ್ಣಾಂಕ strv_env_ನಿಯೋಜಿಸು_many(ಅಕ್ಷರ ***l, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು strv_env_ನಿಯೋಜಿಸು_many_2(l, ...) strv_env_ನಿಯೋಜಿಸು_many(l, __VA_ARGS__, ಖಾಲಿ)

ಅಕ್ಷರ* strv_env_get_n(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t k, ReplaceEnvFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* strv_env_get(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n) {
        ಹಿಂತಿರುಗಿ strv_env_get_n(x, n, SIZE_MAX, 0);
}

ಅಕ್ಷರ* strv_env_ಜೋಡಿಗಳು_get(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ strv_env_get_merged(ಅಕ್ಷರ **l, ಅಕ್ಷರ ***ret);

ಪೂರ್ಣಾಂಕ getenv_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
ಪೂರ್ಣಾಂಕ secure_getenv_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);

ಪೂರ್ಣಾಂಕ secure_getenv_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
ಪೂರ್ಣಾಂಕ ಗಣ_unset_env(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
ಪೂರ್ಣಾಂಕ putenv_dup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *assignment, bool override);

ಪೂರ್ಣಾಂಕ setenv_systemd_exec_pid(bool update_only);
ಪೂರ್ಣಾಂಕ setenv_systemd_log_level(ಶೂನ್ಯ);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
ಪೂರ್ಣಾಂಕ getenv_ಮಾರ್ಗ_list(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ ***ret_paths);

ಪೂರ್ಣಾಂಕ getenv_steal_erase(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ setenvf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, bool overwrite, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *valuef, ...) _printf_(3,4);
