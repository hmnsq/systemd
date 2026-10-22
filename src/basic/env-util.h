/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(خالی);

bool env_name_is_valid(ثابت حرف *e);
bool env_قدر_is_valid(ثابت حرف *e);
bool env_assignment_is_valid(ثابت حرف *e);

قسمتعریف شمار ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

صحیح replace_env_بھرا(ثابت حرف *format, size_t length, حرف **env, ReplaceEnvFlags flags, حرف **ret, حرف ***ret_unset_variables, حرف ***ret_bad_variables);
ساکن اندرونی صحیح replace_env(ثابت حرف *format, حرف **env, ReplaceEnvFlags flags, حرف **ret) {
        واپس replace_env_بھرا(format, SIZE_MAX, env, flags, ret, تہی, تہی);
}

صحیح replace_env_argv(حرف **argv, حرف **env, حرف ***ret, حرف ***ret_unset_variables, حرف ***ret_bad_variables);

bool strv_env_is_valid(حرف * ثابت *e);
bool strv_env_name_is_valid(حرف * ثابت *l);
bool strv_env_name_or_assignment_is_valid(حرف * ثابت *l);

حرف** strv_env_clean_with_callback(حرف **l, خالی (*invalid_callback)(ثابت حرف *p, خالی *userdata), خالی *userdata);
#تعریف_کرو strv_env_clean(l) strv_env_clean_with_callback(l, تہی, تہی)

حرف** strv_env_ملاؤ(حرف **پہلا, ...);
#تعریف_کرو strv_env_ملاؤ_2(پہلا, ...) strv_env_ملاؤ(پہلا, __VA_ARGS__, اشارہ_گر_MAX)
حرف** strv_env_delete(حرف **x, size_t n_lists, ...); /* New copy */

حرف** strv_env_unset(حرف **l, ثابت حرف *p); /* In place ... */
حرف** strv_env_unset_many_اندرونی(حرف **l, ...) _sentinel_;
#تعریف_کرو strv_env_unset_many(l, ...) strv_env_unset_many_اندرونی(l, __VA_ARGS__, تہی)
صحیح strv_env_replace_consume(حرف ***l, حرف *p); /* In place ... */
صحیح strv_env_replace_strdup(حرف ***l, ثابت حرف *assignment);
صحیح strv_env_replace_strdup_passthrough(حرف ***l, ثابت حرف *assignment);
صحیح strv_env_تفویض(حرف ***l, ثابت حرف *کلید, ثابت حرف *قدر);
صحیح strv_env_assignf(حرف ***l, ثابت حرف *کلید, ثابت حرف *valuef, ...) _printf_(3, 4);
صحیح strv_env_تفویض_many(حرف ***l, ...) _sentinel_;
#تعریف_کرو strv_env_تفویض_many_2(l, ...) strv_env_تفویض_many(l, __VA_ARGS__, تہی)

حرف* strv_env_get_n(حرف * ثابت *l, ثابت حرف *name, size_t k, ReplaceEnvFlags flags);
ساکن اندرونی حرف* strv_env_get(حرف * ثابت *x, ثابت حرف *n) {
        واپس strv_env_get_n(x, n, SIZE_MAX, 0);
}

حرف* strv_env_جوڑے_get(حرف **l, ثابت حرف *name) خالص;
صحیح strv_env_get_merged(حرف **l, حرف ***ret);

صحیح getenv_bool(ثابت حرف *p);
صحیح secure_getenv_bool(ثابت حرف *p);

صحیح secure_getenv_uint64(ثابت حرف *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
صحیح سیٹ_unset_env(ثابت حرف *name, ثابت حرف *قدر, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
صحیح putenv_dup(ثابت حرف *assignment, bool override);

صحیح setenv_systemd_exec_pid(bool update_only);
صحیح setenv_systemd_log_level(خالی);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
صحیح getenv_راستہ_list(ثابت حرف *name, حرف ***ret_paths);

صحیح getenv_steal_erase(ثابت حرف *name, حرف **ret);

صحیح setenvf(ثابت حرف *name, bool overwrite, ثابت حرف *valuef, ...) _printf_(3,4);
