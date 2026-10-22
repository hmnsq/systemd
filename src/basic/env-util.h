/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(فراغ);

bool env_name_is_valid(ثابت حرف *e);
bool env_قىممەت_is_valid(ثابت حرف *e);
bool env_assignment_is_valid(ثابت حرف *e);

تعريفنوع تعداد ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

صحيح replace_env_full(ثابت حرف *format, size_t length, حرف **env, ReplaceEnvFlags flags, حرف **ret, حرف ***ret_unset_variables, حرف ***ret_bad_variables);
ساكن مضمن صحيح replace_env(ثابت حرف *format, حرف **env, ReplaceEnvFlags flags, حرف **ret) {
        أرجع replace_env_full(format, SIZE_MAX, env, flags, ret, عدم, عدم);
}

صحيح replace_env_argv(حرف **argv, حرف **env, حرف ***ret, حرف ***ret_unset_variables, حرف ***ret_bad_variables);

bool strv_env_is_valid(حرف * ثابت *e);
bool strv_env_name_is_valid(حرف * ثابت *l);
bool strv_env_name_or_assignment_is_valid(حرف * ثابت *l);

حرف** strv_env_clean_with_callback(حرف **l, فراغ (*invalid_callback)(ثابت حرف *p, فراغ *userdata), فراغ *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, عدم, عدم)

حرف** _strv_env_merge(حرف **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
حرف** strv_env_delete(حرف **x, size_t n_lists, ...); /* New copy */

حرف** strv_env_unset(حرف **l, ثابت حرف *p); /* In place ... */
حرف** strv_env_unset_many_internal(حرف **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, عدم)
صحيح strv_env_replace_consume(حرف ***l, حرف *p); /* In place ... */
صحيح strv_env_replace_strdup(حرف ***l, ثابت حرف *assignment);
صحيح strv_env_replace_strdup_passthrough(حرف ***l, ثابت حرف *assignment);
صحيح strv_env_assign(حرف ***l, ثابت حرف *ئاچقۇچ, ثابت حرف *قىممەت);
صحيح strv_env_assignf(حرف ***l, ثابت حرف *ئاچقۇچ, ثابت حرف *valuef, ...) _printf_(3, 4);
صحيح _strv_env_assign_many(حرف ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, عدم)

حرف* strv_env_get_n(حرف * ثابت *l, ثابت حرف *name, size_t k, ReplaceEnvFlags flags);
ساكن مضمن حرف* strv_env_get(حرف * ثابت *x, ثابت حرف *n) {
        أرجع strv_env_get_n(x, n, SIZE_MAX, 0);
}

حرف* strv_env_pairs_get(حرف **l, ثابت حرف *name) _pure_;
صحيح strv_env_get_merged(حرف **l, حرف ***ret);

صحيح getenv_bool(ثابت حرف *p);
صحيح secure_getenv_bool(ثابت حرف *p);

صحيح secure_getenv_uint64(ثابت حرف *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
صحيح set_unset_env(ثابت حرف *name, ثابت حرف *قىممەت, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
صحيح putenv_dup(ثابت حرف *assignment, bool override);

صحيح setenv_systemd_exec_pid(bool update_only);
صحيح setenv_systemd_log_level(فراغ);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
صحيح getenv_path_list(ثابت حرف *name, حرف ***ret_paths);

صحيح getenv_steal_erase(ثابت حرف *name, حرف **ret);

صحيح setenvf(ثابت حرف *name, bool overwrite, ثابت حرف *valuef, ...) _printf_(3,4);
