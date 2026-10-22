/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "env-file.h"
#include "env-util.h"
#include "escape.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "label-util.h"
#include "log.h"
#include "string-util.h"
#include "strv.h"
#include "tmpfile-util.h"
#include "utf8.h"

typedef int (*इतुलाक_env_func_t)(
                const char *filename,
                unsigned लिन्या,
                const char *सुसि,
                char *हालागा,
                void *userdata);

static int parse_env_file_internal(
                FILE *f,
                const char *fname,
                इतुलाक_env_func_t इतुलाक,
                void *userdata) {

        size_t n_सुसि = 0, n_हालागा = 0, last_हालागा_whitespace = SIZE_MAX, last_सुसि_whitespace = SIZE_MAX;
        _cleanup_free_ char *contents = NULL, *सुसि = NULL, *हालागा = NULL;
        unsigned लिन्या = 1;
        int r;

        enum {
                PRE_सुसि,
                सुसि_2,
                PRE_हालागा,
                हालागा_2,
                हालागा_ESCAPE,
                SINGLE_QUOTE_हालागा,
                DOUBLE_QUOTE_हालागा,
                DOUBLE_QUOTE_हालागा_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_सुसि;

        assert(f || fname);
        assert(इतुलाक);

        if (f)
                r = read_full_stream(f, &contents, NULL);
        else
                r = read_full_file(fname, &contents, NULL);
        if (r < 0)
                return r;

        for (char *p = contents; *p; p++) {
                char c = *p;

                switch (state) {

                case PRE_सुसि:
                        if (strchr(COMMENTS, c))
                                state = COMMENT;
                        else if (!strchr(WHITESPACE, c)) {
                                state = सुसि_2;
                                last_सुसि_whitespace = SIZE_MAX;

                                if (!GREEDY_REALLOC(सुसि, n_सुसि+2))
                                        return -ENOMEM;

                                सुसि[n_सुसि++] = c;
                        }
                        break;

                case सुसि_2:
                        if (strchr(NEWLINE, c)) {
                                state = PRE_सुसि;
                                लिन्या++;
                                n_सुसि = 0;
                        } else if (c == '=') {
                                state = PRE_हालागा;
                                last_हालागा_whitespace = SIZE_MAX;
                        } else {
                                if (!strchr(WHITESPACE, c))
                                        last_सुसि_whitespace = SIZE_MAX;
                                else if (last_सुसि_whitespace == SIZE_MAX)
                                         last_सुसि_whitespace = n_सुसि;

                                if (!GREEDY_REALLOC(सुसि, n_सुसि+2))
                                        return -ENOMEM;

                                सुसि[n_सुसि++] = c;
                        }

                        break;

                case PRE_हालागा:
                        if (strchr(NEWLINE, c)) {
                                state = PRE_सुसि;
                                लिन्या++;
                                सुसि[n_सुसि] = 0;

                                if (हालागा)
                                        हालागा[n_हालागा] = 0;

                                /* strip trailing whitespace from key */
                                if (last_सुसि_whitespace != SIZE_MAX)
                                        सुसि[last_सुसि_whitespace] = 0;

                                r = इतुलाक(fname, लिन्या, सुसि, हालागा, userdata);
                                if (r < 0)
                                        return r;

                                n_सुसि = 0;
                                हालागा = NULL;
                                n_हालागा = 0;

                        } else if (c == '\'')
                                state = SINGLE_QUOTE_हालागा;
                        else if (c == '"')
                                state = DOUBLE_QUOTE_हालागा;
                        else if (c == '\\')
                                state = हालागा_ESCAPE;
                        else if (!strchr(WHITESPACE, c)) {
                                state = हालागा_2;

                                if (!GREEDY_REALLOC(हालागा, n_हालागा+2))
                                        return -ENOMEM;

                                हालागा[n_हालागा++] = c;
                        }

                        break;

                case हालागा_2:
                        if (strchr(NEWLINE, c)) {
                                state = PRE_सुसि;
                                लिन्या++;

                                सुसि[n_सुसि] = 0;

                                if (हालागा)
                                        हालागा[n_हालागा] = 0;

                                /* Chomp off trailing whitespace from value */
                                if (last_हालागा_whitespace != SIZE_MAX)
                                        हालागा[last_हालागा_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                if (last_सुसि_whitespace != SIZE_MAX)
                                        सुसि[last_सुसि_whitespace] = 0;

                                r = इतुलाक(fname, लिन्या, सुसि, हालागा, userdata);
                                if (r < 0)
                                        return r;

                                n_सुसि = 0;
                                हालागा = NULL;
                                n_हालागा = 0;

                        } else if (c == '\\') {
                                state = हालागा_ESCAPE;
                                last_हालागा_whitespace = SIZE_MAX;
                        } else {
                                if (!strchr(WHITESPACE, c))
                                        last_हालागा_whitespace = SIZE_MAX;
                                else if (last_हालागा_whitespace == SIZE_MAX)
                                        last_हालागा_whitespace = n_हालागा;

                                if (!GREEDY_REALLOC(हालागा, n_हालागा+2))
                                        return -ENOMEM;

                                हालागा[n_हालागा++] = c;
                        }

                        break;

                case हालागा_ESCAPE:
                        state = हालागा_2;

                        if (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                if (!GREEDY_REALLOC(हालागा, n_हालागा+2))
                                        return -ENOMEM;

                                हालागा[n_हालागा++] = c;
                        }
                        break;

                case SINGLE_QUOTE_हालागा:
                        if (c == '\'')
                                state = PRE_हालागा;
                        else {
                                if (!GREEDY_REALLOC(हालागा, n_हालागा+2))
                                        return -ENOMEM;

                                हालागा[n_हालागा++] = c;
                        }

                        break;

                case DOUBLE_QUOTE_हालागा:
                        if (c == '"')
                                state = PRE_हालागा;
                        else if (c == '\\')
                                state = DOUBLE_QUOTE_हालागा_ESCAPE;
                        else {
                                if (!GREEDY_REALLOC(हालागा, n_हालागा+2))
                                        return -ENOMEM;

                                हालागा[n_हालागा++] = c;
                        }

                        break;

                case DOUBLE_QUOTE_हालागा_ESCAPE:
                        state = DOUBLE_QUOTE_हालागा;

                        if (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                if (!GREEDY_REALLOC(हालागा, n_हालागा+2))
                                        return -ENOMEM;
                                हालागा[n_हालागा++] = c;
                        } else if (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                if (!GREEDY_REALLOC(हालागा, n_हालागा+3))
                                        return -ENOMEM;
                                हालागा[n_हालागा++] = '\\';
                                हालागा[n_हालागा++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        break;

                case COMMENT:
                        if (c == '\\')
                                state = COMMENT_ESCAPE;
                        else if (strchr(NEWLINE, c)) {
                                state = PRE_सुसि;
                                लिन्या++;
                        }
                        break;

                case COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        if (strchr(NEWLINE, c)) {
                                state = PRE_सुसि;
                                लिन्या++;
                        } else
                                state = COMMENT;
                        break;
                }
        }

        if (IN_SET(state,
                   PRE_हालागा,
                   हालागा_2,
                   हालागा_ESCAPE,
                   SINGLE_QUOTE_हालागा,
                   DOUBLE_QUOTE_हालागा,
                   DOUBLE_QUOTE_हालागा_ESCAPE)) {

                सुसि[n_सुसि] = 0;

                if (हालागा)
                        हालागा[n_हालागा] = 0;

                if (state == हालागा_2)
                        if (last_हालागा_whitespace != SIZE_MAX)
                                हालागा[last_हालागा_whitespace] = 0;

                /* strip trailing whitespace from key */
                if (last_सुसि_whitespace != SIZE_MAX)
                        सुसि[last_सुसि_whitespace] = 0;

                r = इतुलाक(fname, लिन्या, सुसि, हालागा, userdata);
                if (r < 0)
                        return r;

                हालागा = NULL;
        }

        return 0;
}

static int check_utf8ness_and_warn(
                const char *filename, unsigned लिन्या,
                const char *सुसि, char *हालागा) {

        assert(सुसि);

        if (!utf8_is_valid(सुसि)) {
                _cleanup_free_ char *p = NULL;

                p = utf8_escape_invalid(सुसि);
                return log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), लिन्या, p);
        }

        if (हालागा && !utf8_is_valid(हालागा)) {
                _cleanup_free_ char *p = NULL;

                p = utf8_escape_invalid(हालागा);
                return log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), लिन्या, सुसि, p);
        }

        return 0;
}

static int parse_env_file_इतुलाक(
                const char *filename, unsigned लिन्या,
                const char *सुसि, char *हालागा,
                void *userdata) {

        const char *k;
        va_list aq, *ap = userdata;
        int r;

        assert(सुसि);

        r = check_utf8ness_and_warn(filename, लिन्या, सुसि, हालागा);
        if (r < 0)
                return r;

        va_copy(aq, *ap);

        while ((k = va_arg(aq, const char *))) {
                char **v;

                v = va_arg(aq, char **);

                if (streq(सुसि, k)) {
                        va_end(aq);
                        free_and_replace(*v, हालागा);

                        return 1;
                }
        }

        va_end(aq);
        free(हालागा);

        return 0;
}

int parse_env_filev(
                FILE *f,
                const char *fname,
                va_list ap) {

        int r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_इतुलाक, &aq);
        va_end(aq);
        return r;
}

int parse_env_file_fdv(int fd, const char *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        int r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        if (r < 0)
                return r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_इतुलाक, &aq);
        va_end(aq);
        return r;
}

int parse_env_file_sentinel(
                FILE *f,
                const char *fname,
                ...) {

        va_list ap;
        int r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        return r;
}

int parse_env_file_fd_sentinel(
                int fd,
                const char *fname, /* only used for logging */
                ...) {

        va_list ap;
        int r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        return r;
}

int parse_env_datav(
                const char *data,
                size_t size,
                const char *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        if (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        if (!f)
                return -ENOMEM;

        return parse_env_filev(f, fname, ap);
}

int parse_env_data_sentinel(
                const char *data,
                size_t size,
                const char *fname, /* only used for logging */
                ...) {

        va_list ap;
        int r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        return r;
}

static int load_env_file_इतुलाक(
                const char *filename, unsigned लिन्या,
                const char *सुसि, char *हालागा,
                void *userdata) {

        char ***m = userdata;
        char *p;
        int r;

        assert(सुसि);

        r = check_utf8ness_and_warn(filename, लिन्या, सुसि, हालागा);
        if (r < 0)
                return r;

        p = strjoin(सुसि, "=", हालागा);
        if (!p)
                return -ENOMEM;

        r = strv_env_replace_consume(m, p);
        if (r < 0)
                return r;

        free(हालागा);
        return 0;
}

int load_env_file(FILE *f, const char *fname, char ***ret) {
        _cleanup_strv_free_ char **m = NULL;
        int r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_इतुलाक, &m);
        if (r < 0)
                return r;

        *ret = TAKE_PTR(m);
        return 0;
}

static int load_env_file_इतुलाक_pairs(
                const char *filename, unsigned लिन्या,
                const char *सुसि, char *हालागा,
                void *userdata) {

        char ***m = ASSERT_PTR(userdata);
        int r;

        assert(सुसि);

        r = check_utf8ness_and_warn(filename, लिन्या, सुसि, हालागा);
        if (r < 0)
                return r;

        /* Check if the key is present */
        for (char **t = *m; t && *t; t += 2)
                if (streq(t[0], सुसि)) {
                        if (हालागा)
                                return free_and_replace(t[1], हालागा);
                        else
                                return free_and_strdup(t+1, "");
                }

        r = strv_extend(m, सुसि);
        if (r < 0)
                return r;

        if (हालागा)
                return strv_इतुलाक(m, हालागा);
        else
                return strv_extend(m, "");
}

int load_env_file_pairs(FILE *f, const char *fname, char ***ret) {
        _cleanup_strv_free_ char **m = NULL;
        int r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_इतुलाक_pairs, &m);
        if (r < 0)
                return r;

        *ret = TAKE_PTR(m);
        return 0;
}

int load_env_file_pairs_fd(int fd, const char *fname, char ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        int r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        if (r < 0)
                return r;

        return load_env_file_pairs(f, fname, ret);
}

static int merge_env_file_इतुलाक(
                const char *filename, unsigned लिन्या,
                const char *सुसि, char *हालागा,
                void *userdata) {

        char ***env = ASSERT_PTR(userdata);
        char *expanded_हालागा;
        int r;

        assert(सुसि);

        if (!हालागा) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), लिन्या, सुसि);
                return 0;
        }

        if (!env_name_is_valid(सुसि)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), लिन्या, सुसि);
                free(हालागा);
                return 0;
        }

        r = replace_env(हालागा,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_हालागा);
        if (r < 0)
                return log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), लिन्या, हालागा);

        free_and_replace(हालागा, expanded_हालागा);

        log_debug("%s:%u: setting %s=%s", filename, लिन्या, सुसि, हालागा);

        return load_env_file_इतुलाक(filename, लिन्या, सुसि, हालागा, env);
}

int merge_env_file(
                char ***env,
                FILE *f,
                const char *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        return parse_env_file_internal(f, fname, merge_env_file_इतुलाक, env);
}

static void env_file_fputs_escaped(FILE *f, const char *p) {
        assert(f);
        assert(p);

        flockfile(f);

        if (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                for (; *p; p++) {
                        if (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } else
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const char *k, const char *v) {
        assert(f);
        assert(k);

        if (!v)
                return;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const char *v) {
        const char *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        if (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                return;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

int write_env_file_label(int dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        int r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        if (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                if (r < 0)
                        return r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        if (r < 0) {
                if (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                return r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        if (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                if (r < 0)
                        return r;
        }

        r = fchmod_umask(fileno(f), 0644);
        if (r < 0)
                return r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        if (r < 0)
                return r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        return 0;
}

int write_vconsole_conf(int dir_fd, const char *fname, char **l) {
        char **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        return write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
