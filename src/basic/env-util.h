/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(শূন্য);

bool env_name_is_valid(ধ্রুৱ ৱর্ণ *e);
bool env_মান_is_valid(ধ্রুৱ ৱর্ণ *e);
bool env_assignment_is_valid(ধ্রুৱ ৱর্ণ *e);

প্রকারনির্দেশ গণনা ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

পূর্ণাঙ্ক replace_env_full(ধ্রুৱ ৱর্ণ *format, size_t length, ৱর্ণ **env, ReplaceEnvFlags flags, ৱর্ণ **ret, ৱর্ণ ***ret_unset_variables, ৱর্ণ ***ret_bad_variables);
স্থির অন্তঃস্থ পূর্ণাঙ্ক replace_env(ধ্রুৱ ৱর্ণ *format, ৱর্ণ **env, ReplaceEnvFlags flags, ৱর্ণ **ret) {
        ঘূৰাই replace_env_full(format, SIZE_MAX, env, flags, ret, রিক্ত, রিক্ত);
}

পূর্ণাঙ্ক replace_env_argv(ৱর্ণ **argv, ৱর্ণ **env, ৱর্ণ ***ret, ৱর্ণ ***ret_unset_variables, ৱর্ণ ***ret_bad_variables);

bool strv_env_is_valid(ৱর্ণ * ধ্রুৱ *e);
bool strv_env_name_is_valid(ৱর্ণ * ধ্রুৱ *l);
bool strv_env_name_or_assignment_is_valid(ৱর্ণ * ধ্রুৱ *l);

ৱর্ণ** strv_env_clean_with_callback(ৱর্ণ **l, শূন্য (*invalid_callback)(ধ্রুৱ ৱর্ণ *p, শূন্য *userdata), শূন্য *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, রিক্ত, রিক্ত)

ৱর্ণ** _strv_env_merge(ৱর্ণ **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
ৱর্ণ** strv_env_delete(ৱর্ণ **x, size_t n_lists, ...); /* New copy */

ৱর্ণ** strv_env_unset(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *p); /* In place ... */
ৱর্ণ** strv_env_unset_many_internal(ৱর্ণ **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, রিক্ত)
পূর্ণাঙ্ক strv_env_replace_consume(ৱর্ণ ***l, ৱর্ণ *p); /* In place ... */
পূর্ণাঙ্ক strv_env_replace_strdup(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *assignment);
পূর্ণাঙ্ক strv_env_replace_strdup_passthrough(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *assignment);
পূর্ণাঙ্ক strv_env_assign(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);
পূর্ণাঙ্ক strv_env_assignf(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *valuef, ...) _printf_(3, 4);
পূর্ণাঙ্ক _strv_env_assign_many(ৱর্ণ ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, রিক্ত)

ৱর্ণ* strv_env_get_n(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name, size_t k, ReplaceEnvFlags flags);
স্থির অন্তঃস্থ ৱর্ণ* strv_env_get(ৱর্ণ * ধ্রুৱ *x, ধ্রুৱ ৱর্ণ *n) {
        ঘূৰাই strv_env_get_n(x, n, SIZE_MAX, 0);
}

ৱর্ণ* strv_env_pairs_get(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *name) _pure_;
পূর্ণাঙ্ক strv_env_get_merged(ৱর্ণ **l, ৱর্ণ ***ret);

পূর্ণাঙ্ক getenv_bool(ধ্রুৱ ৱর্ণ *p);
পূর্ণাঙ্ক secure_getenv_bool(ধ্রুৱ ৱর্ণ *p);

পূর্ণাঙ্ক secure_getenv_uint64(ধ্রুৱ ৱর্ণ *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
পূর্ণাঙ্ক set_unset_env(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *মান, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
পূর্ণাঙ্ক putenv_dup(ধ্রুৱ ৱর্ণ *assignment, bool override);

পূর্ণাঙ্ক setenv_systemd_exec_pid(bool update_only);
পূর্ণাঙ্ক setenv_systemd_log_level(শূন্য);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
পূর্ণাঙ্ক getenv_path_list(ধ্রুৱ ৱর্ণ *name, ৱর্ণ ***ret_paths);

পূর্ণাঙ্ক getenv_steal_erase(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret);

পূর্ণাঙ্ক setenvf(ধ্রুৱ ৱর্ণ *name, bool overwrite, ধ্রুৱ ৱর্ণ *valuef, ...) _printf_(3,4);
