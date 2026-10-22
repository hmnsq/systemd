/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define ENVIRONMENT_ASSIGNMENTS_MAX 16384U

size_t sc_arg_max(ଶୂନ୍ୟ);

bool env_name_is_valid(ଅଚଳ ଅକ୍ଷର *e);
bool env_ମୂଲ୍ୟ_is_valid(ଅଚଳ ଅକ୍ଷର *e);
bool env_assignment_is_valid(ଅଚଳ ଅକ୍ଷର *e);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ReplaceEnvFlags {
        REPLACE_ENV_USE_ENVIRONMENT = 1 << 0,
        REPLACE_ENV_ALLOW_BRACELESS = 1 << 1,
        REPLACE_ENV_ALLOW_EXTENDED  = 1 << 2,
} ReplaceEnvFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା replace_env_full(ଅଚଳ ଅକ୍ଷର *format, size_t length, ଅକ୍ଷର **env, ReplaceEnvFlags flags, ଅକ୍ଷର **ret, ଅକ୍ଷର ***ret_unset_variables, ଅକ୍ଷର ***ret_bad_variables);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା replace_env(ଅଚଳ ଅକ୍ଷର *format, ଅକ୍ଷର **env, ReplaceEnvFlags flags, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ replace_env_full(format, SIZE_MAX, env, flags, ret, ଖାଲି, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା replace_env_argv(ଅକ୍ଷର **argv, ଅକ୍ଷର **env, ଅକ୍ଷର ***ret, ଅକ୍ଷର ***ret_unset_variables, ଅକ୍ଷର ***ret_bad_variables);

bool strv_env_is_valid(ଅକ୍ଷର * ଅଚଳ *e);
bool strv_env_name_is_valid(ଅକ୍ଷର * ଅଚଳ *l);
bool strv_env_name_or_assignment_is_valid(ଅକ୍ଷର * ଅଚଳ *l);

ଅକ୍ଷର** strv_env_clean_with_callback(ଅକ୍ଷର **l, ଶୂନ୍ୟ (*invalid_callback)(ଅଚଳ ଅକ୍ଷର *p, ଶୂନ୍ୟ *userdata), ଶୂନ୍ୟ *userdata);
#define strv_env_clean(l) strv_env_clean_with_callback(l, ଖାଲି, ଖାଲି)

ଅକ୍ଷର** _strv_env_merge(ଅକ୍ଷର **first, ...);
#define strv_env_merge(first, ...) _strv_env_merge(first, __VA_ARGS__, POINTER_MAX)
ଅକ୍ଷର** strv_env_delete(ଅକ୍ଷର **x, size_t n_lists, ...); /* New copy */

ଅକ୍ଷର** strv_env_unset(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *p); /* In place ... */
ଅକ୍ଷର** strv_env_unset_many_internal(ଅକ୍ଷର **l, ...) _sentinel_;
#define strv_env_unset_many(l, ...) strv_env_unset_many_internal(l, __VA_ARGS__, ଖାଲି)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_replace_consume(ଅକ୍ଷର ***l, ଅକ୍ଷର *p); /* In place ... */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_replace_strdup(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *assignment);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_replace_strdup_passthrough(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *assignment);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_assign(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_assignf(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *valuef, ...) _printf_(3, 4);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _strv_env_assign_many(ଅକ୍ଷର ***l, ...) _sentinel_;
#define strv_env_assign_many(l, ...) _strv_env_assign_many(l, __VA_ARGS__, ଖାଲି)

ଅକ୍ଷର* strv_env_get_n(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name, size_t k, ReplaceEnvFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* strv_env_get(ଅକ୍ଷର * ଅଚଳ *x, ଅଚଳ ଅକ୍ଷର *n) {
        ଫେରନ୍ତୁ strv_env_get_n(x, n, SIZE_MAX, 0);
}

ଅକ୍ଷର* strv_env_pairs_get(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *name) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_get_merged(ଅକ୍ଷର **l, ଅକ୍ଷର ***ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_bool(ଅଚଳ ଅକ୍ଷର *p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା secure_getenv_bool(ଅଚଳ ଅକ୍ଷର *p);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା secure_getenv_uint64(ଅଚଳ ଅକ୍ଷର *p, uint64_t *ret);

/* Like setenv, but calls unsetenv if value == NULL. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_unset_env(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, bool overwrite);

/* Like putenv, but duplicates the memory like setenv. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା putenv_dup(ଅଚଳ ଅକ୍ଷର *assignment, bool override);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setenv_systemd_exec_pid(bool update_only);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା setenv_systemd_log_level(ଶୂନ୍ୟ);

/* Parses and does sanity checks on an environment variable containing
 * PATH-like colon-separated absolute paths */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_path_list(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର ***ret_paths);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_steal_erase(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setenvf(ଅଚଳ ଅକ୍ଷର *name, bool overwrite, ଅଚଳ ଅକ୍ଷର *valuef, ...) _printf_(3,4);
