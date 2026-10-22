/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(ಶೂನ್ಯ);

bool env_name_is_valid(ಧ್ರುವ ವರ್ಣ *e);
bool env_ಮೌಲ್ಯ_is_valid(ಧ್ರುವ ವರ್ಣ *e);
bool env_assignment_is_valid(ಧ್ರುವ ವರ್ಣ *e);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

ಪೂರ್ಣಾಙ್ಕ replace_env_full(ಧ್ರುವ ವರ್ಣ *format, size_t length, ವರ್ಣ **env, ReplaceEnvFlags flags, ವರ್ಣ **ret, ವರ್ಣ ***ret_unset_variables, ವರ್ಣ ***ret_bad_variables);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ replace_env(ಧ್ರುವ ವರ್ಣ *format, ವರ್ಣ **env, ReplaceEnvFlags flags, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ replace_env_full(format, SIZE_MAX, env, flags, ret, ರಿಕ್ತ, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ replace_env_argv(ವರ್ಣ **argv, ವರ್ಣ **env, ವರ್ಣ ***ret, ವರ್ಣ ***ret_unset_variables, ವರ್ಣ ***ret_bad_variables);

bool strv_env_is_valid(ವರ್ಣ * ಧ್ರುವ *e);
bool strv_env_name_is_valid(ವರ್ಣ * ಧ್ರುವ *l);
bool strv_env_name_or_assignment_is_valid(ವರ್ಣ * ಧ್ರುವ *l);

ವರ್ಣ** strv_env_clean_with_callback(ವರ್ಣ **l, ಶೂನ್ಯ (*invalid_callback)(ಧ್ರುವ ವರ್ಣ *p, ಶೂನ್ಯ *userdata), ಶೂನ್ಯ *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, ರಿಕ್ತ, ರಿಕ್ತ)

ವರ್ಣ** _strv_env_merge(ವರ್ಣ **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
ವರ್ಣ** strv_env_delete(ವರ್ಣ **x, size_t n_lists, ...); /* New copy */

ವರ್ಣ** strv_env_unset(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *p); /* In place ... */
ವರ್ಣ** strv_env_unset_many_internal(ವರ್ಣ **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, ರಿಕ್ತ)
ಪೂರ್ಣಾಙ್ಕ strv_env_replace_consume(ವರ್ಣ ***l, ವರ್ಣ *p); /* In place ... */
ಪೂರ್ಣಾಙ್ಕ strv_env_replace_strdup(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *assignment);
ಪೂರ್ಣಾಙ್ಕ strv_env_replace_strdup_passthrough(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *assignment);
ಪೂರ್ಣಾಙ್ಕ strv_env_assign(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ strv_env_assignf(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *valuef, ...) _printf_(3, 4);
ಪೂರ್ಣಾಙ್ಕ _strv_env_assign_many(ವರ್ಣ ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, ರಿಕ್ತ)

ವರ್ಣ* strv_env_get_n(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name, size_t k, ReplaceEnvFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* strv_env_get(ವರ್ಣ * ಧ್ರುವ *x, ಧ್ರುವ ವರ್ಣ *n) {
        ಪ್ರತ್ಯಾಗಮ strv_env_get_n(x, n, SIZE_MAX, 0);
}

ವರ್ಣ* strv_env_pairs_get(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *name) _pure_;
ಪೂರ್ಣಾಙ್ಕ strv_env_get_merged(ವರ್ಣ **l, ವರ್ಣ ***ret);

ಪೂರ್ಣಾಙ್ಕ getenv_bool(ಧ್ರುವ ವರ್ಣ *p);
ಪೂರ್ಣಾಙ್ಕ secure_getenv_bool(ಧ್ರುವ ವರ್ಣ *p);

ಪೂರ್ಣಾಙ್ಕ secure_getenv_uint64(ಧ್ರುವ ವರ್ಣ *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
ಪೂರ್ಣಾಙ್ಕ set_unset_env(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
ಪೂರ್ಣಾಙ್ಕ putenv_dup(ಧ್ರುವ ವರ್ಣ *assignment, bool override);

ಪೂರ್ಣಾಙ್ಕ setenv_systemd_exec_pid(bool update_only);
ಪೂರ್ಣಾಙ್ಕ setenv_systemd_log_level(ಶೂನ್ಯ);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
ಪೂರ್ಣಾಙ್ಕ getenv_path_list(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ ***ret_paths);

ಪೂರ್ಣಾಙ್ಕ getenv_steal_erase(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ setenvf(ಧ್ರುವ ವರ್ಣ *name, bool overwrite, ಧ್ರುವ ವರ್ಣ *valuef, ...) _printf_(3,4);
