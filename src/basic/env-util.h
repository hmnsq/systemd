/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(வெறுமை);

bool env_name_is_valid(மாறிலி எழுத்து *e);
bool env_மதிப்பு_is_valid(மாறிலி எழுத்து *e);
bool env_assignment_is_valid(மாறிலி எழுத்து *e);

வகைவரை எண்ணிக்கை ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

முழு replace_env_full(மாறிலி எழுத்து *format, size_t length, எழுத்து **env, ReplaceEnvFlags flags, எழுத்து **ret, எழுத்து ***ret_unset_variables, எழுத்து ***ret_bad_variables);
நிலையான உள்வரி முழு replace_env(மாறிலி எழுத்து *format, எழுத்து **env, ReplaceEnvFlags flags, எழுத்து **ret) {
        திரும்பு replace_env_full(format, SIZE_MAX, env, flags, ret, வெற்று, வெற்று);
}

முழு replace_env_argv(எழுத்து **argv, எழுத்து **env, எழுத்து ***ret, எழுத்து ***ret_unset_variables, எழுத்து ***ret_bad_variables);

bool strv_env_is_valid(எழுத்து * மாறிலி *e);
bool strv_env_name_is_valid(எழுத்து * மாறிலி *l);
bool strv_env_name_or_assignment_is_valid(எழுத்து * மாறிலி *l);

எழுத்து** strv_env_clean_with_callback(எழுத்து **l, வெறுமை (*invalid_callback)(மாறிலி எழுத்து *p, வெறுமை *userdata), வெறுமை *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, வெற்று, வெற்று)

எழுத்து** _strv_env_merge(எழுத்து **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
எழுத்து** strv_env_delete(எழுத்து **x, size_t n_lists, ...); /* New copy */

எழுத்து** strv_env_unset(எழுத்து **l, மாறிலி எழுத்து *p); /* In place ... */
எழுத்து** strv_env_unset_many_internal(எழுத்து **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, வெற்று)
முழு strv_env_replace_consume(எழுத்து ***l, எழுத்து *p); /* In place ... */
முழு strv_env_replace_strdup(எழுத்து ***l, மாறிலி எழுத்து *assignment);
முழு strv_env_replace_strdup_passthrough(எழுத்து ***l, மாறிலி எழுத்து *assignment);
முழு strv_env_assign(எழுத்து ***l, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);
முழு strv_env_assignf(எழுத்து ***l, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *valuef, ...) _printf_(3, 4);
முழு _strv_env_assign_many(எழுத்து ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, வெற்று)

எழுத்து* strv_env_get_n(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name, size_t k, ReplaceEnvFlags flags);
நிலையான உள்வரி எழுத்து* strv_env_get(எழுத்து * மாறிலி *x, மாறிலி எழுத்து *n) {
        திரும்பு strv_env_get_n(x, n, SIZE_MAX, 0);
}

எழுத்து* strv_env_pairs_get(எழுத்து **l, மாறிலி எழுத்து *name) _pure_;
முழு strv_env_get_merged(எழுத்து **l, எழுத்து ***ret);

முழு getenv_bool(மாறிலி எழுத்து *p);
முழு secure_getenv_bool(மாறிலி எழுத்து *p);

முழு secure_getenv_uint64(மாறிலி எழுத்து *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
முழு set_unset_env(மாறிலி எழுத்து *name, மாறிலி எழுத்து *மதிப்பு, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
முழு putenv_dup(மாறிலி எழுத்து *assignment, bool override);

முழு setenv_systemd_exec_pid(bool update_only);
முழு setenv_systemd_log_level(வெறுமை);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
முழு getenv_path_list(மாறிலி எழுத்து *name, எழுத்து ***ret_paths);

முழு getenv_steal_erase(மாறிலி எழுத்து *name, எழுத்து **ret);

முழு setenvf(மாறிலி எழுத்து *name, bool overwrite, மாறிலி எழுத்து *valuef, ...) _printf_(3,4);
