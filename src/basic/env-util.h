/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(ശൂന്യം);

bool env_name_is_valid(സ്ഥിരം അക്ഷരം *e);
bool env_മൂല്യം_is_valid(സ്ഥിരം അക്ഷരം *e);
bool env_assignment_is_valid(സ്ഥിരം അക്ഷരം *e);

തരനിർവചനം എണ്ണൽ ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

പൂർണ്ണസംഖ്യ replace_env_full(സ്ഥിരം അക്ഷരം *format, size_t length, അക്ഷരം **env, ReplaceEnvFlags flags, അക്ഷരം **ret, അക്ഷരം ***ret_unset_variables, അക്ഷരം ***ret_bad_variables);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ replace_env(സ്ഥിരം അക്ഷരം *format, അക്ഷരം **env, ReplaceEnvFlags flags, അക്ഷരം **ret) {
        തിരികെ replace_env_full(format, SIZE_MAX, env, flags, ret, ശൂന്യ, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ replace_env_argv(അക്ഷരം **argv, അക്ഷരം **env, അക്ഷരം ***ret, അക്ഷരം ***ret_unset_variables, അക്ഷരം ***ret_bad_variables);

bool strv_env_is_valid(അക്ഷരം * സ്ഥിരം *e);
bool strv_env_name_is_valid(അക്ഷരം * സ്ഥിരം *l);
bool strv_env_name_or_assignment_is_valid(അക്ഷരം * സ്ഥിരം *l);

അക്ഷരം** strv_env_clean_with_callback(അക്ഷരം **l, ശൂന്യം (*invalid_callback)(സ്ഥിരം അക്ഷരം *p, ശൂന്യം *userdata), ശൂന്യം *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, ശൂന്യ, ശൂന്യ)

അക്ഷരം** _strv_env_merge(അക്ഷരം **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
അക്ഷരം** strv_env_delete(അക്ഷരം **x, size_t n_lists, ...); /* New copy */

അക്ഷരം** strv_env_unset(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *p); /* In place ... */
അക്ഷരം** strv_env_unset_many_internal(അക്ഷരം **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, ശൂന്യ)
പൂർണ്ണസംഖ്യ strv_env_replace_consume(അക്ഷരം ***l, അക്ഷരം *p); /* In place ... */
പൂർണ്ണസംഖ്യ strv_env_replace_strdup(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *assignment);
പൂർണ്ണസംഖ്യ strv_env_replace_strdup_passthrough(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *assignment);
പൂർണ്ണസംഖ്യ strv_env_assign(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);
പൂർണ്ണസംഖ്യ strv_env_assignf(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *valuef, ...) _printf_(3, 4);
പൂർണ്ണസംഖ്യ _strv_env_assign_many(അക്ഷരം ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, ശൂന്യ)

അക്ഷരം* strv_env_get_n(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name, size_t k, ReplaceEnvFlags flags);
സ്ഥിതം വരിയിൽ അക്ഷരം* strv_env_get(അക്ഷരം * സ്ഥിരം *x, സ്ഥിരം അക്ഷരം *n) {
        തിരികെ strv_env_get_n(x, n, SIZE_MAX, 0);
}

അക്ഷരം* strv_env_pairs_get(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *name) _pure_;
പൂർണ്ണസംഖ്യ strv_env_get_merged(അക്ഷരം **l, അക്ഷരം ***ret);

പൂർണ്ണസംഖ്യ getenv_bool(സ്ഥിരം അക്ഷരം *p);
പൂർണ്ണസംഖ്യ secure_getenv_bool(സ്ഥിരം അക്ഷരം *p);

പൂർണ്ണസംഖ്യ secure_getenv_uint64(സ്ഥിരം അക്ഷരം *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
പൂർണ്ണസംഖ്യ set_unset_env(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *മൂല്യം, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
പൂർണ്ണസംഖ്യ putenv_dup(സ്ഥിരം അക്ഷരം *assignment, bool override);

പൂർണ്ണസംഖ്യ setenv_systemd_exec_pid(bool update_only);
പൂർണ്ണസംഖ്യ setenv_systemd_log_level(ശൂന്യം);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
പൂർണ്ണസംഖ്യ getenv_path_list(സ്ഥിരം അക്ഷരം *name, അക്ഷരം ***ret_paths);

പൂർണ്ണസംഖ്യ getenv_steal_erase(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ setenvf(സ്ഥിരം അക്ഷരം *name, bool overwrite, സ്ഥിരം അക്ഷരം *valuef, ...) _printf_(3,4);
