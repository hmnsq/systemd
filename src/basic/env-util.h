/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(শূন্য);

bool env_name_is_valid(ধ্রুব বর্ণ *e);
bool env_মান_is_valid(ধ্রুব বর্ণ *e);
bool env_assignment_is_valid(ধ্রুব বর্ণ *e);

প্রকারনির্দেশ গণনা ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

পূর্ণাঙ্ক replace_env_full(ধ্রুব বর্ণ *format, size_t length, বর্ণ **env, ReplaceEnvFlags flags, বর্ণ **ret, বর্ণ ***ret_unset_variables, বর্ণ ***ret_bad_variables);
স্থির অন্তঃস্থ পূর্ণাঙ্ক replace_env(ধ্রুব বর্ণ *format, বর্ণ **env, ReplaceEnvFlags flags, বর্ণ **ret) {
        প্রত্যাগম replace_env_full(format, SIZE_MAX, env, flags, ret, রিক্ত, রিক্ত);
}

পূর্ণাঙ্ক replace_env_argv(বর্ণ **argv, বর্ণ **env, বর্ণ ***ret, বর্ণ ***ret_unset_variables, বর্ণ ***ret_bad_variables);

bool strv_env_is_valid(বর্ণ * ধ্রুব *e);
bool strv_env_name_is_valid(বর্ণ * ধ্রুব *l);
bool strv_env_name_or_assignment_is_valid(বর্ণ * ধ্রুব *l);

বর্ণ** strv_env_clean_with_callback(বর্ণ **l, শূন্য (*invalid_callback)(ধ্রুব বর্ণ *p, শূন্য *userdata), শূন্য *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, রিক্ত, রিক্ত)

বর্ণ** _strv_env_merge(বর্ণ **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
বর্ণ** strv_env_delete(বর্ণ **x, size_t n_lists, ...); /* New copy */

বর্ণ** strv_env_unset(বর্ণ **l, ধ্রুব বর্ণ *p); /* In place ... */
বর্ণ** strv_env_unset_many_internal(বর্ণ **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, রিক্ত)
পূর্ণাঙ্ক strv_env_replace_consume(বর্ণ ***l, বর্ণ *p); /* In place ... */
পূর্ণাঙ্ক strv_env_replace_strdup(বর্ণ ***l, ধ্রুব বর্ণ *assignment);
পূর্ণাঙ্ক strv_env_replace_strdup_passthrough(বর্ণ ***l, ধ্রুব বর্ণ *assignment);
পূর্ণাঙ্ক strv_env_assign(বর্ণ ***l, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান);
পূর্ণাঙ্ক strv_env_assignf(বর্ণ ***l, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *valuef, ...) _printf_(3, 4);
পূর্ণাঙ্ক _strv_env_assign_many(বর্ণ ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, রিক্ত)

বর্ণ* strv_env_get_n(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name, size_t k, ReplaceEnvFlags flags);
স্থির অন্তঃস্থ বর্ণ* strv_env_get(বর্ণ * ধ্রুব *x, ধ্রুব বর্ণ *n) {
        প্রত্যাগম strv_env_get_n(x, n, SIZE_MAX, 0);
}

বর্ণ* strv_env_pairs_get(বর্ণ **l, ধ্রুব বর্ণ *name) _pure_;
পূর্ণাঙ্ক strv_env_get_merged(বর্ণ **l, বর্ণ ***ret);

পূর্ণাঙ্ক getenv_bool(ধ্রুব বর্ণ *p);
পূর্ণাঙ্ক secure_getenv_bool(ধ্রুব বর্ণ *p);

পূর্ণাঙ্ক secure_getenv_uint64(ধ্রুব বর্ণ *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
পূর্ণাঙ্ক set_unset_env(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *মান, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
পূর্ণাঙ্ক putenv_dup(ধ্রুব বর্ণ *assignment, bool override);

পূর্ণাঙ্ক setenv_systemd_exec_pid(bool update_only);
পূর্ণাঙ্ক setenv_systemd_log_level(শূন্য);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
পূর্ণাঙ্ক getenv_path_list(ধ্রুব বর্ণ *name, বর্ণ ***ret_paths);

পূর্ণাঙ্ক getenv_steal_erase(ধ্রুব বর্ণ *name, বর্ণ **ret);

পূর্ণাঙ্ক setenvf(ধ্রুব বর্ণ *name, bool overwrite, ধ্রুব বর্ণ *valuef, ...) _printf_(3,4);
