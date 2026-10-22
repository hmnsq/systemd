/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(void);

bool env_name_is_valid(const char *e);
bool env_value_is_valid(const char *e);
bool env_assignment_is_valid(const char *e);

typedef enum ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

整数 replace_env_full(const char *format, size_t length, char **env, ReplaceEnvFlags flags, char **ret, char ***ret_unset_variables, char ***ret_bad_variables);
static inline 整数 replace_env(const char *format, char **env, ReplaceEnvFlags flags, char **ret) {
        返回 replace_env_full(format, SIZE_MAX, env, flags, ret, NULL, NULL);
}

整数 replace_env_argv(char **argv, char **env, char ***ret, char ***ret_unset_variables, char ***ret_bad_variables);

bool strv_env_is_valid(char * const *e);
bool strv_env_name_is_valid(char * const *l);
bool strv_env_name_or_assignment_is_valid(char * const *l);

char** strv_env_clean_with_callback(char **l, void (*invalid_callback)(const char *p, void *userdata), void *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, NULL, NULL)

char** _strv_env_merge(char **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
char** strv_env_delete(char **x, size_t n_lists, ...); /* New copy */

char** strv_env_unset(char **l, const char *p); /* In place ... */
char** strv_env_unset_many_internal(char **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, NULL)
整数 strv_env_replace_consume(char ***l, char *p); /* In place ... */
整数 strv_env_replace_strdup(char ***l, const char *assignment);
整数 strv_env_replace_strdup_passthrough(char ***l, const char *assignment);
整数 strv_env_assign(char ***l, const char *key, const char *value);
整数 strv_env_assignf(char ***l, const char *key, const char *valuef, ...) _printf_(3, 4);
整数 _strv_env_assign_many(char ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, NULL)

char* strv_env_get_n(char * const *l, const char *name, size_t k, ReplaceEnvFlags flags);
static inline char* strv_env_get(char * const *x, const char *n) {
        返回 strv_env_get_n(x, n, SIZE_MAX, 0);
}

char* strv_env_pairs_get(char **l, const char *name) _pure_;
整数 strv_env_get_merged(char **l, char ***ret);

整数 getenv_bool(const char *p);
整数 secure_getenv_bool(const char *p);

整数 secure_getenv_uint64(const char *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
整数 set_unset_env(const char *name, const char *value, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
整数 putenv_dup(const char *assignment, bool override);

整数 setenv_systemd_exec_pid(bool update_only);
整数 setenv_systemd_log_level(void);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
整数 getenv_path_list(const char *name, char ***ret_paths);

整数 getenv_steal_erase(const char *name, char **ret);

整数 setenvf(const char *name, bool overwrite, const char *valuef, ...) _printf_(3,4);
