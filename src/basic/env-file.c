/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "env-file.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "label-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "tmpfile-util.h"
#અન્તર્ભાવ "utf8.h"

પ્રકારવ્યાખ્યા પૂર્ણાંક (*ધકેલો_env_func_t)(
                અચલ અક્ષર *filename,
                unsigned લીટી,
                અચલ અક્ષર *ચાવી,
                અક્ષર *કિંમત,
                શૂન્ય *userdata);

સ્થિર પૂર્ણાંક parse_env_file_internal(
                FILE *f,
                અચલ અક્ષર *fname,
                ધકેલો_env_func_t ધકેલો,
                શૂન્ય *userdata) {

        size_t n_ચાવી = 0, n_કિંમત = 0, last_કિંમત_whitespace = SIZE_MAX, last_ચાવી_whitespace = SIZE_MAX;
        _cleanup_free_ અક્ષર *contents = ખાલી, *ચાવી = ખાલી, *કિંમત = ખાલી;
        unsigned લીટી = 1;
        પૂર્ણાંક r;

        ગણના {
                PRE_ચાવી,
                ચાવી_2,
                PRE_કિંમત,
                કિંમત_2,
                કિંમત_ESCAPE,
                SINGLE_QUOTE_કિંમત,
                DOUBLE_QUOTE_કિંમત,
                DOUBLE_QUOTE_કિંમત_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_ચાવી;

        પ્રતિજ્ઞા(f || fname);
        પ્રતિજ્ઞા(ધકેલો);

        જો (f)
                r = read_full_stream(f, &contents, ખાલી);
        નહીંતર
                r = read_full_file(fname, &contents, ખાલી);
        જો (r < 0)
                પાછા r;

        માટે (અક્ષર *p = contents; *p; p++) {
                અક્ષર c = *p;

                પસંદગી (state) {

                સ્થિતિ PRE_ચાવી:
                        જો (strchr(COMMENTS, c))
                                state = COMMENT;
                        નહીંતર જો (!strchr(WHITESPACE, c)) {
                                state = ચાવી_2;
                                last_ચાવી_whitespace = SIZE_MAX;

                                જો (!GREEDY_REALLOC(ચાવી, n_ચાવી+2))
                                        પાછા -ENOMEM;

                                ચાવી[n_ચાવી++] = c;
                        }
                        તોડો;

                સ્થિતિ ચાવી_2:
                        જો (strchr(NEWLINE, c)) {
                                state = PRE_ચાવી;
                                લીટી++;
                                n_ચાવી = 0;
                        } નહીંતર જો (c == '=') {
                                state = PRE_કિંમત;
                                last_કિંમત_whitespace = SIZE_MAX;
                        } નહીંતર {
                                જો (!strchr(WHITESPACE, c))
                                        last_ચાવી_whitespace = SIZE_MAX;
                                નહીંતર જો (last_ચાવી_whitespace == SIZE_MAX)
                                         last_ચાવી_whitespace = n_ચાવી;

                                જો (!GREEDY_REALLOC(ચાવી, n_ચાવી+2))
                                        પાછા -ENOMEM;

                                ચાવી[n_ચાવી++] = c;
                        }

                        તોડો;

                સ્થિતિ PRE_કિંમત:
                        જો (strchr(NEWLINE, c)) {
                                state = PRE_ચાવી;
                                લીટી++;
                                ચાવી[n_ચાવી] = 0;

                                જો (કિંમત)
                                        કિંમત[n_કિંમત] = 0;

                                /* strip trailing whitespace from key */
                                જો (last_ચાવી_whitespace != SIZE_MAX)
                                        ચાવી[last_ચાવી_whitespace] = 0;

                                r = ધકેલો(fname, લીટી, ચાવી, કિંમત, userdata);
                                જો (r < 0)
                                        પાછા r;

                                n_ચાવી = 0;
                                કિંમત = ખાલી;
                                n_કિંમત = 0;

                        } નહીંતર જો (c == '\'')
                                state = SINGLE_QUOTE_કિંમત;
                        નહીંતર જો (c == '"')
                                state = DOUBLE_QUOTE_કિંમત;
                        નહીંતર જો (c == '\\')
                                state = કિંમત_ESCAPE;
                        નહીંતર જો (!strchr(WHITESPACE, c)) {
                                state = કિંમત_2;

                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+2))
                                        પાછા -ENOMEM;

                                કિંમત[n_કિંમત++] = c;
                        }

                        તોડો;

                સ્થિતિ કિંમત_2:
                        જો (strchr(NEWLINE, c)) {
                                state = PRE_ચાવી;
                                લીટી++;

                                ચાવી[n_ચાવી] = 0;

                                જો (કિંમત)
                                        કિંમત[n_કિંમત] = 0;

                                /* Chomp off trailing whitespace from value */
                                જો (last_કિંમત_whitespace != SIZE_MAX)
                                        કિંમત[last_કિંમત_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                જો (last_ચાવી_whitespace != SIZE_MAX)
                                        ચાવી[last_ચાવી_whitespace] = 0;

                                r = ધકેલો(fname, લીટી, ચાવી, કિંમત, userdata);
                                જો (r < 0)
                                        પાછા r;

                                n_ચાવી = 0;
                                કિંમત = ખાલી;
                                n_કિંમત = 0;

                        } નહીંતર જો (c == '\\') {
                                state = કિંમત_ESCAPE;
                                last_કિંમત_whitespace = SIZE_MAX;
                        } નહીંતર {
                                જો (!strchr(WHITESPACE, c))
                                        last_કિંમત_whitespace = SIZE_MAX;
                                નહીંતર જો (last_કિંમત_whitespace == SIZE_MAX)
                                        last_કિંમત_whitespace = n_કિંમત;

                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+2))
                                        પાછા -ENOMEM;

                                કિંમત[n_કિંમત++] = c;
                        }

                        તોડો;

                સ્થિતિ કિંમત_ESCAPE:
                        state = કિંમત_2;

                        જો (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+2))
                                        પાછા -ENOMEM;

                                કિંમત[n_કિંમત++] = c;
                        }
                        તોડો;

                સ્થિતિ SINGLE_QUOTE_કિંમત:
                        જો (c == '\'')
                                state = PRE_કિંમત;
                        નહીંતર {
                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+2))
                                        પાછા -ENOMEM;

                                કિંમત[n_કિંમત++] = c;
                        }

                        તોડો;

                સ્થિતિ DOUBLE_QUOTE_કિંમત:
                        જો (c == '"')
                                state = PRE_કિંમત;
                        નહીંતર જો (c == '\\')
                                state = DOUBLE_QUOTE_કિંમત_ESCAPE;
                        નહીંતર {
                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+2))
                                        પાછા -ENOMEM;

                                કિંમત[n_કિંમત++] = c;
                        }

                        તોડો;

                સ્થિતિ DOUBLE_QUOTE_કિંમત_ESCAPE:
                        state = DOUBLE_QUOTE_કિંમત;

                        જો (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+2))
                                        પાછા -ENOMEM;
                                કિંમત[n_કિંમત++] = c;
                        } નહીંતર જો (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                જો (!GREEDY_REALLOC(કિંમત, n_કિંમત+3))
                                        પાછા -ENOMEM;
                                કિંમત[n_કિંમત++] = '\\';
                                કિંમત[n_કિંમત++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        તોડો;

                સ્થિતિ COMMENT:
                        જો (c == '\\')
                                state = COMMENT_ESCAPE;
                        નહીંતર જો (strchr(NEWLINE, c)) {
                                state = PRE_ચાવી;
                                લીટી++;
                        }
                        તોડો;

                સ્થિતિ COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        જો (strchr(NEWLINE, c)) {
                                state = PRE_ચાવી;
                                લીટી++;
                        } નહીંતર
                                state = COMMENT;
                        તોડો;
                }
        }

        જો (IN_SET(state,
                   PRE_કિંમત,
                   કિંમત_2,
                   કિંમત_ESCAPE,
                   SINGLE_QUOTE_કિંમત,
                   DOUBLE_QUOTE_કિંમત,
                   DOUBLE_QUOTE_કિંમત_ESCAPE)) {

                ચાવી[n_ચાવી] = 0;

                જો (કિંમત)
                        કિંમત[n_કિંમત] = 0;

                જો (state == કિંમત_2)
                        જો (last_કિંમત_whitespace != SIZE_MAX)
                                કિંમત[last_કિંમત_whitespace] = 0;

                /* strip trailing whitespace from key */
                જો (last_ચાવી_whitespace != SIZE_MAX)
                        ચાવી[last_ચાવી_whitespace] = 0;

                r = ધકેલો(fname, લીટી, ચાવી, કિંમત, userdata);
                જો (r < 0)
                        પાછા r;

                કિંમત = ખાલી;
        }

        પાછા 0;
}

સ્થિર પૂર્ણાંક check_utf8ness_and_warn(
                અચલ અક્ષર *filename, unsigned લીટી,
                અચલ અક્ષર *ચાવી, અક્ષર *કિંમત) {

        પ્રતિજ્ઞા(ચાવી);

        જો (!utf8_is_valid(ચાવી)) {
                _cleanup_free_ અક્ષર *p = ખાલી;

                p = utf8_escape_invalid(ચાવી);
                પાછા log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), લીટી, p);
        }

        જો (કિંમત && !utf8_is_valid(કિંમત)) {
                _cleanup_free_ અક્ષર *p = ખાલી;

                p = utf8_escape_invalid(કિંમત);
                પાછા log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), લીટી, ચાવી, p);
        }

        પાછા 0;
}

સ્થિર પૂર્ણાંક parse_env_file_ધકેલો(
                અચલ અક્ષર *filename, unsigned લીટી,
                અચલ અક્ષર *ચાવી, અક્ષર *કિંમત,
                શૂન્ય *userdata) {

        અચલ અક્ષર *k;
        va_list aq, *ap = userdata;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ચાવી);

        r = check_utf8ness_and_warn(filename, લીટી, ચાવી, કિંમત);
        જો (r < 0)
                પાછા r;

        va_copy(aq, *ap);

        જ્યારે ((k = va_arg(aq, અચલ અક્ષર *))) {
                અક્ષર **v;

                v = va_arg(aq, અક્ષર **);

                જો (streq(ચાવી, k)) {
                        va_end(aq);
                        free_and_replace(*v, કિંમત);

                        પાછા 1;
                }
        }

        va_end(aq);
        મુક્ત(કિંમત);

        પાછા 0;
}

પૂર્ણાંક parse_env_filev(
                FILE *f,
                અચલ અક્ષર *fname,
                va_list ap) {

        પૂર્ણાંક r;
        va_list aq;

        પ્રતિજ્ઞા(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ધકેલો, &aq);
        va_end(aq);
        પાછા r;
}

પૂર્ણાંક parse_env_file_fdv(પૂર્ણાંક fd, અચલ અક્ષર *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ખાલી;
        va_list aq;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        જો (r < 0)
                પાછા r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ધકેલો, &aq);
        va_end(aq);
        પાછા r;
}

પૂર્ણાંક parse_env_file_sentinel(
                FILE *f,
                અચલ અક્ષર *fname,
                ...) {

        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક parse_env_file_fd_sentinel(
                પૂર્ણાંક fd,
                અચલ અક્ષર *fname, /* only used for logging */
                ...) {

        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક parse_env_datav(
                અચલ અક્ષર *data,
                size_t size,
                અચલ અક્ષર *fname, /* only used for logging */
                va_list ap) {

        પ્રતિજ્ઞા(data);

        જો (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((શૂન્ય*) data, size, "r");
        જો (!f)
                પાછા -ENOMEM;

        પાછા parse_env_filev(f, fname, ap);
}

પૂર્ણાંક parse_env_data_sentinel(
                અચલ અક્ષર *data,
                size_t size,
                અચલ અક્ષર *fname, /* only used for logging */
                ...) {

        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        પાછા r;
}

સ્થિર પૂર્ણાંક load_env_file_ધકેલો(
                અચલ અક્ષર *filename, unsigned લીટી,
                અચલ અક્ષર *ચાવી, અક્ષર *કિંમત,
                શૂન્ય *userdata) {

        અક્ષર ***m = userdata;
        અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ચાવી);

        r = check_utf8ness_and_warn(filename, લીટી, ચાવી, કિંમત);
        જો (r < 0)
                પાછા r;

        p = strjoin(ચાવી, "=", કિંમત);
        જો (!p)
                પાછા -ENOMEM;

        r = strv_env_replace_consume(m, p);
        જો (r < 0)
                પાછા r;

        મુક્ત(કિંમત);
        પાછા 0;
}

પૂર્ણાંક load_env_file(FILE *f, અચલ અક્ષર *fname, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **m = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f || fname);
        પ્રતિજ્ઞા(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ધકેલો, &m);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(m);
        પાછા 0;
}

સ્થિર પૂર્ણાંક load_env_file_ધકેલો_pairs(
                અચલ અક્ષર *filename, unsigned લીટી,
                અચલ અક્ષર *ચાવી, અક્ષર *કિંમત,
                શૂન્ય *userdata) {

        અક્ષર ***m = ASSERT_PTR(userdata);
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ચાવી);

        r = check_utf8ness_and_warn(filename, લીટી, ચાવી, કિંમત);
        જો (r < 0)
                પાછા r;

        /* Check if the key is present */
        માટે (અક્ષર **t = *m; t && *t; t += 2)
                જો (streq(t[0], ચાવી)) {
                        જો (કિંમત)
                                પાછા free_and_replace(t[1], કિંમત);
                        નહીંતર
                                પાછા free_and_strdup(t+1, "");
                }

        r = strv_extend(m, ચાવી);
        જો (r < 0)
                પાછા r;

        જો (કિંમત)
                પાછા strv_ધકેલો(m, કિંમત);
        નહીંતર
                પાછા strv_extend(m, "");
}

પૂર્ણાંક load_env_file_pairs(FILE *f, અચલ અક્ષર *fname, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **m = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f || fname);
        પ્રતિજ્ઞા(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ધકેલો_pairs, &m);
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(m);
        પાછા 0;
}

પૂર્ણાંક load_env_file_pairs_fd(પૂર્ણાંક fd, અચલ અક્ષર *fname, અક્ષર ***ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        જો (r < 0)
                પાછા r;

        પાછા load_env_file_pairs(f, fname, ret);
}

સ્થિર પૂર્ણાંક merge_env_file_ધકેલો(
                અચલ અક્ષર *filename, unsigned લીટી,
                અચલ અક્ષર *ચાવી, અક્ષર *કિંમત,
                શૂન્ય *userdata) {

        અક્ષર ***env = ASSERT_PTR(userdata);
        અક્ષર *expanded_કિંમત;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ચાવી);

        જો (!કિંમત) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), લીટી, ચાવી);
                પાછા 0;
        }

        જો (!env_name_is_valid(ચાવી)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), લીટી, ચાવી);
                મુક્ત(કિંમત);
                પાછા 0;
        }

        r = replace_env(કિંમત,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_કિંમત);
        જો (r < 0)
                પાછા log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), લીટી, કિંમત);

        free_and_replace(કિંમત, expanded_કિંમત);

        log_debug("%s:%u: setting %s=%s", filename, લીટી, ચાવી, કિંમત);

        પાછા load_env_file_ધકેલો(filename, લીટી, ચાવી, કિંમત, env);
}

પૂર્ણાંક merge_env_file(
                અક્ષર ***env,
                FILE *f,
                અચલ અક્ષર *fname) {

        પ્રતિજ્ઞા(env);
        પ્રતિજ્ઞા(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        પાછા parse_env_file_internal(f, fname, merge_env_file_ધકેલો, env);
}

સ્થિર શૂન્ય env_file_fputs_escaped(FILE *f, અચલ અક્ષર *p) {
        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(p);

        flockfile(f);

        જો (string_has_cc(p, ખાલી) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                માટે (; *p; p++) {
                        જો (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } નહીંતર
                fputs_unlocked(p, f);

        funlockfile(f);
}

શૂન્ય env_file_fputs_assignment(FILE *f, અચલ અક્ષર *k, અચલ અક્ષર *v) {
        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(k);

        જો (!v)
                પાછા;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

સ્થિર શૂન્ય write_env_var(FILE *f, અચલ અક્ષર *v) {
        અચલ અક્ષર *p;

        પ્રતિજ્ઞા(f);
        પ્રતિજ્ઞા(v);

        p = strchr(v, '=');
        જો (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                પાછા;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

પૂર્ણાંક write_env_file_label(પૂર્ણાંક dir_fd, અચલ અક્ષર *fname, અક્ષર **headers, અક્ષર **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(fname);

        જો (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                જો (r < 0)
                        પાછા r;
        }

        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_free_ અક્ષર *p = ખાલી;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        જો (r < 0) {
                જો (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (શૂન્ય) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                પાછા r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        જો (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ખાલી, /* created= */ true, label_context);
                જો (r < 0)
                        પાછા r;
        }

        r = fchmod_umask(fileno(f), 0644);
        જો (r < 0)
                પાછા r;

        STRV_FOREACH(i, headers) {
                પ્રતિજ્ઞા(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        જો (r < 0)
                પાછા r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        પાછા 0;
}

પૂર્ણાંક write_vconsole_conf(પૂર્ણાંક dir_fd, અચલ અક્ષર *fname, અક્ષર **l) {
        અક્ષર **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        પાછા write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ખાલી);
}
