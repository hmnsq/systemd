/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(ਖਾਲੀ);

bool env_name_is_valid(ਅਚਲ ਅੱਖਰ *e);
bool env_ਮੁੱਲ_is_valid(ਅਚਲ ਅੱਖਰ *e);
bool env_assignment_is_valid(ਅਚਲ ਅੱਖਰ *e);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

ਪੂਰਨ replace_env_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *format, size_t length, ਅੱਖਰ **env, ReplaceEnvFlags flags, ਅੱਖਰ **ret, ਅੱਖਰ ***ret_unset_variables, ਅੱਖਰ ***ret_bad_variables);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ replace_env(ਅਚਲ ਅੱਖਰ *format, ਅੱਖਰ **env, ReplaceEnvFlags flags, ਅੱਖਰ **ret) {
        ਵਾਪਸ replace_env_ਭਰਿਆ(format, SIZE_MAX, env, flags, ret, ਨਿਰਾ, ਨਿਰਾ);
}

ਪੂਰਨ replace_env_argv(ਅੱਖਰ **argv, ਅੱਖਰ **env, ਅੱਖਰ ***ret, ਅੱਖਰ ***ret_unset_variables, ਅੱਖਰ ***ret_bad_variables);

bool strv_env_is_valid(ਅੱਖਰ * ਅਚਲ *e);
bool strv_env_name_is_valid(ਅੱਖਰ * ਅਚਲ *l);
bool strv_env_name_or_assignment_is_valid(ਅੱਖਰ * ਅਚਲ *l);

ਅੱਖਰ** strv_env_clean_with_callback(ਅੱਖਰ **l, ਖਾਲੀ (*invalid_callback)(ਅਚਲ ਅੱਖਰ *p, ਖਾਲੀ *userdata), ਖਾਲੀ *userdata);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_env_clean(l) strv_env_clean_with_callback(l, ਨਿਰਾ, ਨਿਰਾ)

ਅੱਖਰ** strv_env_ਮਿਲਾਓ(ਅੱਖਰ **ਪਹਿਲਾ, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_env_ਮਿਲਾਓ_2(ਪਹਿਲਾ, ...) strv_env_ਮਿਲਾਓ(ਪਹਿਲਾ, __VA_ARGS__, ਸੂਚਕ_MAX)
ਅੱਖਰ** strv_env_delete(ਅੱਖਰ **x, size_t n_lists, ...); /* New copy */

ਅੱਖਰ** strv_env_unset(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *p); /* In place ... */
ਅੱਖਰ** strv_env_unset_many_ਅੰਦਰੂਨੀ(ਅੱਖਰ **l, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_env_unset_many(l, ...) strv_env_unset_many_ਅੰਦਰੂਨੀ(l, __VA_ARGS__, ਨਿਰਾ)
ਪੂਰਨ strv_env_replace_consume(ਅੱਖਰ ***l, ਅੱਖਰ *p); /* In place ... */
ਪੂਰਨ strv_env_replace_strdup(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *assignment);
ਪੂਰਨ strv_env_replace_strdup_passthrough(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *assignment);
ਪੂਰਨ strv_env_ਸੌਂਪੋ(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਪੂਰਨ strv_env_assignf(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *valuef, ...) _printf_(3, 4);
ਪੂਰਨ strv_env_ਸੌਂਪੋ_many(ਅੱਖਰ ***l, ...) _sentinel_;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strv_env_ਸੌਂਪੋ_many_2(l, ...) strv_env_ਸੌਂਪੋ_many(l, __VA_ARGS__, ਨਿਰਾ)

ਅੱਖਰ* strv_env_get_n(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name, size_t k, ReplaceEnvFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* strv_env_get(ਅੱਖਰ * ਅਚਲ *x, ਅਚਲ ਅੱਖਰ *n) {
        ਵਾਪਸ strv_env_get_n(x, n, SIZE_MAX, 0);
}

ਅੱਖਰ* strv_env_ਜੋੜੇ_get(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *name) ਸ਼ੁੱਧ;
ਪੂਰਨ strv_env_get_merged(ਅੱਖਰ **l, ਅੱਖਰ ***ret);

ਪੂਰਨ getenv_bool(ਅਚਲ ਅੱਖਰ *p);
ਪੂਰਨ secure_getenv_bool(ਅਚਲ ਅੱਖਰ *p);

ਪੂਰਨ secure_getenv_uint64(ਅਚਲ ਅੱਖਰ *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
ਪੂਰਨ ਸੈੱਟ_unset_env(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
ਪੂਰਨ putenv_dup(ਅਚਲ ਅੱਖਰ *assignment, bool override);

ਪੂਰਨ setenv_systemd_exec_pid(bool update_only);
ਪੂਰਨ setenv_systemd_log_level(ਖਾਲੀ);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
ਪੂਰਨ getenv_ਰਸਤਾ_list(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ ***ret_paths);

ਪੂਰਨ getenv_steal_erase(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret);

ਪੂਰਨ setenvf(ਅਚਲ ਅੱਖਰ *name, bool overwrite, ਅਚਲ ਅੱਖਰ *valuef, ...) _printf_(3,4);
