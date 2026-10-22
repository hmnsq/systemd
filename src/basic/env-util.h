/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(শূন্য);

bool env_name_is_valid(ধ্রুবক অক্ষর *e);
bool env_মান_is_valid(ধ্রুবক অক্ষর *e);
bool env_assignment_is_valid(ধ্রুবক অক্ষর *e);

প্রকারলেখ গণনা ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

পূর্ণসংখ্যা replace_env_full(ধ্রুবক অক্ষর *format, size_t length, অক্ষর **env, ReplaceEnvFlags flags, অক্ষর **ret, অক্ষর ***ret_unset_variables, অক্ষর ***ret_bad_variables);
স্থির অন্তঃরেখ পূর্ণসংখ্যা replace_env(ধ্রুবক অক্ষর *format, অক্ষর **env, ReplaceEnvFlags flags, অক্ষর **ret) {
        ফেরত replace_env_full(format, SIZE_MAX, env, flags, ret, ফাঁকা, ফাঁকা);
}

পূর্ণসংখ্যা replace_env_argv(অক্ষর **argv, অক্ষর **env, অক্ষর ***ret, অক্ষর ***ret_unset_variables, অক্ষর ***ret_bad_variables);

bool strv_env_is_valid(অক্ষর * ধ্রুবক *e);
bool strv_env_name_is_valid(অক্ষর * ধ্রুবক *l);
bool strv_env_name_or_assignment_is_valid(অক্ষর * ধ্রুবক *l);

অক্ষর** strv_env_clean_with_callback(অক্ষর **l, শূন্য (*invalid_callback)(ধ্রুবক অক্ষর *p, শূন্য *userdata), শূন্য *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, ফাঁকা, ফাঁকা)

অক্ষর** _strv_env_merge(অক্ষর **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
অক্ষর** strv_env_delete(অক্ষর **x, size_t n_lists, ...); /* New copy */

অক্ষর** strv_env_unset(অক্ষর **l, ধ্রুবক অক্ষর *p); /* In place ... */
অক্ষর** strv_env_unset_many_internal(অক্ষর **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, ফাঁকা)
পূর্ণসংখ্যা strv_env_replace_consume(অক্ষর ***l, অক্ষর *p); /* In place ... */
পূর্ণসংখ্যা strv_env_replace_strdup(অক্ষর ***l, ধ্রুবক অক্ষর *assignment);
পূর্ণসংখ্যা strv_env_replace_strdup_passthrough(অক্ষর ***l, ধ্রুবক অক্ষর *assignment);
পূর্ণসংখ্যা strv_env_assign(অক্ষর ***l, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);
পূর্ণসংখ্যা strv_env_assignf(অক্ষর ***l, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *valuef, ...) _printf_(3, 4);
পূর্ণসংখ্যা _strv_env_assign_many(অক্ষর ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, ফাঁকা)

অক্ষর* strv_env_get_n(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name, size_t k, ReplaceEnvFlags flags);
স্থির অন্তঃরেখ অক্ষর* strv_env_get(অক্ষর * ধ্রুবক *x, ধ্রুবক অক্ষর *n) {
        ফেরত strv_env_get_n(x, n, SIZE_MAX, 0);
}

অক্ষর* strv_env_pairs_get(অক্ষর **l, ধ্রুবক অক্ষর *name) _pure_;
পূর্ণসংখ্যা strv_env_get_merged(অক্ষর **l, অক্ষর ***ret);

পূর্ণসংখ্যা getenv_bool(ধ্রুবক অক্ষর *p);
পূর্ণসংখ্যা secure_getenv_bool(ধ্রুবক অক্ষর *p);

পূর্ণসংখ্যা secure_getenv_uint64(ধ্রুবক অক্ষর *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
পূর্ণসংখ্যা set_unset_env(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *মান, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
পূর্ণসংখ্যা putenv_dup(ধ্রুবক অক্ষর *assignment, bool override);

পূর্ণসংখ্যা setenv_systemd_exec_pid(bool update_only);
পূর্ণসংখ্যা setenv_systemd_log_level(শূন্য);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
পূর্ণসংখ্যা getenv_path_list(ধ্রুবক অক্ষর *name, অক্ষর ***ret_paths);

পূর্ণসংখ্যা getenv_steal_erase(ধ্রুবক অক্ষর *name, অক্ষর **ret);

পূর্ণসংখ্যা setenvf(ধ্রুবক অক্ষর *name, bool overwrite, ধ্রুবক অক্ষর *valuef, ...) _printf_(3,4);
