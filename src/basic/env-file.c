/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "env-file.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "label-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "tmpfile-util.h"
#അന്തര്ഭാവ "utf8.h"

തരനിർവചനം പൂർണ്ണസംഖ്യ (*തള്ളുക_env_func_t)(
                സ്ഥിരം അക്ഷരം *filename,
                unsigned വരി,
                സ്ഥിരം അക്ഷരം *താക്കോൽ,
                അക്ഷരം *മൂല്യം,
                ശൂന്യം *userdata);

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_env_file_internal(
                FILE *f,
                സ്ഥിരം അക്ഷരം *fname,
                തള്ളുക_env_func_t തള്ളുക,
                ശൂന്യം *userdata) {

        size_t n_താക്കോൽ = 0, n_മൂല്യം = 0, last_മൂല്യം_whitespace = SIZE_MAX, last_താക്കോൽ_whitespace = SIZE_MAX;
        _cleanup_free_ അക്ഷരം *contents = ശൂന്യ, *താക്കോൽ = ശൂന്യ, *മൂല്യം = ശൂന്യ;
        unsigned വരി = 1;
        പൂർണ്ണസംഖ്യ r;

        എണ്ണൽ {
                PRE_താക്കോൽ,
                താക്കോൽ_2,
                PRE_മൂല്യം,
                മൂല്യം_2,
                മൂല്യം_ESCAPE,
                SINGLE_QUOTE_മൂല്യം,
                DOUBLE_QUOTE_മൂല്യം,
                DOUBLE_QUOTE_മൂല്യം_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_താക്കോൽ;

        പ്രതിജ്ഞാ(f || fname);
        പ്രതിജ്ഞാ(തള്ളുക);

        എങ്കിൽ (f)
                r = read_full_stream(f, &contents, ശൂന്യ);
        അല്ലെങ്കിൽ
                r = read_full_file(fname, &contents, ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (അക്ഷരം *p = contents; *p; p++) {
                അക്ഷരം c = *p;

                തിരഞ്ഞെടുപ്പ് (state) {

                അവസ്ഥ PRE_താക്കോൽ:
                        എങ്കിൽ (strchr(COMMENTS, c))
                                state = COMMENT;
                        അല്ലെങ്കിൽ എങ്കിൽ (!strchr(WHITESPACE, c)) {
                                state = താക്കോൽ_2;
                                last_താക്കോൽ_whitespace = SIZE_MAX;

                                എങ്കിൽ (!GREEDY_REALLOC(താക്കോൽ, n_താക്കോൽ+2))
                                        തിരികെ -ENOMEM;

                                താക്കോൽ[n_താക്കോൽ++] = c;
                        }
                        നിർത്തുക;

                അവസ്ഥ താക്കോൽ_2:
                        എങ്കിൽ (strchr(NEWLINE, c)) {
                                state = PRE_താക്കോൽ;
                                വരി++;
                                n_താക്കോൽ = 0;
                        } അല്ലെങ്കിൽ എങ്കിൽ (c == '=') {
                                state = PRE_മൂല്യം;
                                last_മൂല്യം_whitespace = SIZE_MAX;
                        } അല്ലെങ്കിൽ {
                                എങ്കിൽ (!strchr(WHITESPACE, c))
                                        last_താക്കോൽ_whitespace = SIZE_MAX;
                                അല്ലെങ്കിൽ എങ്കിൽ (last_താക്കോൽ_whitespace == SIZE_MAX)
                                         last_താക്കോൽ_whitespace = n_താക്കോൽ;

                                എങ്കിൽ (!GREEDY_REALLOC(താക്കോൽ, n_താക്കോൽ+2))
                                        തിരികെ -ENOMEM;

                                താക്കോൽ[n_താക്കോൽ++] = c;
                        }

                        നിർത്തുക;

                അവസ്ഥ PRE_മൂല്യം:
                        എങ്കിൽ (strchr(NEWLINE, c)) {
                                state = PRE_താക്കോൽ;
                                വരി++;
                                താക്കോൽ[n_താക്കോൽ] = 0;

                                എങ്കിൽ (മൂല്യം)
                                        മൂല്യം[n_മൂല്യം] = 0;

                                /* strip trailing whitespace from key */
                                എങ്കിൽ (last_താക്കോൽ_whitespace != SIZE_MAX)
                                        താക്കോൽ[last_താക്കോൽ_whitespace] = 0;

                                r = തള്ളുക(fname, വരി, താക്കോൽ, മൂല്യം, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                n_താക്കോൽ = 0;
                                മൂല്യം = ശൂന്യ;
                                n_മൂല്യം = 0;

                        } അല്ലെങ്കിൽ എങ്കിൽ (c == '\'')
                                state = SINGLE_QUOTE_മൂല്യം;
                        അല്ലെങ്കിൽ എങ്കിൽ (c == '"')
                                state = DOUBLE_QUOTE_മൂല്യം;
                        അല്ലെങ്കിൽ എങ്കിൽ (c == '\\')
                                state = മൂല്യം_ESCAPE;
                        അല്ലെങ്കിൽ എങ്കിൽ (!strchr(WHITESPACE, c)) {
                                state = മൂല്യം_2;

                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+2))
                                        തിരികെ -ENOMEM;

                                മൂല്യം[n_മൂല്യം++] = c;
                        }

                        നിർത്തുക;

                അവസ്ഥ മൂല്യം_2:
                        എങ്കിൽ (strchr(NEWLINE, c)) {
                                state = PRE_താക്കോൽ;
                                വരി++;

                                താക്കോൽ[n_താക്കോൽ] = 0;

                                എങ്കിൽ (മൂല്യം)
                                        മൂല്യം[n_മൂല്യം] = 0;

                                /* Chomp off trailing whitespace from value */
                                എങ്കിൽ (last_മൂല്യം_whitespace != SIZE_MAX)
                                        മൂല്യം[last_മൂല്യം_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                എങ്കിൽ (last_താക്കോൽ_whitespace != SIZE_MAX)
                                        താക്കോൽ[last_താക്കോൽ_whitespace] = 0;

                                r = തള്ളുക(fname, വരി, താക്കോൽ, മൂല്യം, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                n_താക്കോൽ = 0;
                                മൂല്യം = ശൂന്യ;
                                n_മൂല്യം = 0;

                        } അല്ലെങ്കിൽ എങ്കിൽ (c == '\\') {
                                state = മൂല്യം_ESCAPE;
                                last_മൂല്യം_whitespace = SIZE_MAX;
                        } അല്ലെങ്കിൽ {
                                എങ്കിൽ (!strchr(WHITESPACE, c))
                                        last_മൂല്യം_whitespace = SIZE_MAX;
                                അല്ലെങ്കിൽ എങ്കിൽ (last_മൂല്യം_whitespace == SIZE_MAX)
                                        last_മൂല്യം_whitespace = n_മൂല്യം;

                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+2))
                                        തിരികെ -ENOMEM;

                                മൂല്യം[n_മൂല്യം++] = c;
                        }

                        നിർത്തുക;

                അവസ്ഥ മൂല്യം_ESCAPE:
                        state = മൂല്യം_2;

                        എങ്കിൽ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+2))
                                        തിരികെ -ENOMEM;

                                മൂല്യം[n_മൂല്യം++] = c;
                        }
                        നിർത്തുക;

                അവസ്ഥ SINGLE_QUOTE_മൂല്യം:
                        എങ്കിൽ (c == '\'')
                                state = PRE_മൂല്യം;
                        അല്ലെങ്കിൽ {
                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+2))
                                        തിരികെ -ENOMEM;

                                മൂല്യം[n_മൂല്യം++] = c;
                        }

                        നിർത്തുക;

                അവസ്ഥ DOUBLE_QUOTE_മൂല്യം:
                        എങ്കിൽ (c == '"')
                                state = PRE_മൂല്യം;
                        അല്ലെങ്കിൽ എങ്കിൽ (c == '\\')
                                state = DOUBLE_QUOTE_മൂല്യം_ESCAPE;
                        അല്ലെങ്കിൽ {
                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+2))
                                        തിരികെ -ENOMEM;

                                മൂല്യം[n_മൂല്യം++] = c;
                        }

                        നിർത്തുക;

                അവസ്ഥ DOUBLE_QUOTE_മൂല്യം_ESCAPE:
                        state = DOUBLE_QUOTE_മൂല്യം;

                        എങ്കിൽ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+2))
                                        തിരികെ -ENOMEM;
                                മൂല്യം[n_മൂല്യം++] = c;
                        } അല്ലെങ്കിൽ എങ്കിൽ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                എങ്കിൽ (!GREEDY_REALLOC(മൂല്യം, n_മൂല്യം+3))
                                        തിരികെ -ENOMEM;
                                മൂല്യം[n_മൂല്യം++] = '\\';
                                മൂല്യം[n_മൂല്യം++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        നിർത്തുക;

                അവസ്ഥ COMMENT:
                        എങ്കിൽ (c == '\\')
                                state = COMMENT_ESCAPE;
                        അല്ലെങ്കിൽ എങ്കിൽ (strchr(NEWLINE, c)) {
                                state = PRE_താക്കോൽ;
                                വരി++;
                        }
                        നിർത്തുക;

                അവസ്ഥ COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        എങ്കിൽ (strchr(NEWLINE, c)) {
                                state = PRE_താക്കോൽ;
                                വരി++;
                        } അല്ലെങ്കിൽ
                                state = COMMENT;
                        നിർത്തുക;
                }
        }

        എങ്കിൽ (IN_SET(state,
                   PRE_മൂല്യം,
                   മൂല്യം_2,
                   മൂല്യം_ESCAPE,
                   SINGLE_QUOTE_മൂല്യം,
                   DOUBLE_QUOTE_മൂല്യം,
                   DOUBLE_QUOTE_മൂല്യം_ESCAPE)) {

                താക്കോൽ[n_താക്കോൽ] = 0;

                എങ്കിൽ (മൂല്യം)
                        മൂല്യം[n_മൂല്യം] = 0;

                എങ്കിൽ (state == മൂല്യം_2)
                        എങ്കിൽ (last_മൂല്യം_whitespace != SIZE_MAX)
                                മൂല്യം[last_മൂല്യം_whitespace] = 0;

                /* strip trailing whitespace from key */
                എങ്കിൽ (last_താക്കോൽ_whitespace != SIZE_MAX)
                        താക്കോൽ[last_താക്കോൽ_whitespace] = 0;

                r = തള്ളുക(fname, വരി, താക്കോൽ, മൂല്യം, userdata);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                മൂല്യം = ശൂന്യ;
        }

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ check_utf8ness_and_warn(
                സ്ഥിരം അക്ഷരം *filename, unsigned വരി,
                സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം *മൂല്യം) {

        പ്രതിജ്ഞാ(താക്കോൽ);

        എങ്കിൽ (!utf8_is_valid(താക്കോൽ)) {
                _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                p = utf8_escape_invalid(താക്കോൽ);
                തിരികെ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), വരി, p);
        }

        എങ്കിൽ (മൂല്യം && !utf8_is_valid(മൂല്യം)) {
                _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                p = utf8_escape_invalid(മൂല്യം);
                തിരികെ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), വരി, താക്കോൽ, p);
        }

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_env_file_തള്ളുക(
                സ്ഥിരം അക്ഷരം *filename, unsigned വരി,
                സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം *മൂല്യം,
                ശൂന്യം *userdata) {

        സ്ഥിരം അക്ഷരം *k;
        va_list aq, *ap = userdata;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(താക്കോൽ);

        r = check_utf8ness_and_warn(filename, വരി, താക്കോൽ, മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        va_copy(aq, *ap);

        ആകുമ്പോൾ ((k = va_arg(aq, സ്ഥിരം അക്ഷരം *))) {
                അക്ഷരം **v;

                v = va_arg(aq, അക്ഷരം **);

                എങ്കിൽ (streq(താക്കോൽ, k)) {
                        va_end(aq);
                        free_and_replace(*v, മൂല്യം);

                        തിരികെ 1;
                }
        }

        va_end(aq);
        സ്വതന്ത്രം(മൂല്യം);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_env_filev(
                FILE *f,
                സ്ഥിരം അക്ഷരം *fname,
                va_list ap) {

        പൂർണ്ണസംഖ്യ r;
        va_list aq;

        പ്രതിജ്ഞാ(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_തള്ളുക, &aq);
        va_end(aq);
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ parse_env_file_fdv(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        va_list aq;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_തള്ളുക, &aq);
        va_end(aq);
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ parse_env_file_sentinel(
                FILE *f,
                സ്ഥിരം അക്ഷരം *fname,
                ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ parse_env_file_fd_sentinel(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *fname, /* only used for logging */
                ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ parse_env_datav(
                സ്ഥിരം അക്ഷരം *data,
                size_t size,
                സ്ഥിരം അക്ഷരം *fname, /* only used for logging */
                va_list ap) {

        പ്രതിജ്ഞാ(data);

        എങ്കിൽ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((ശൂന്യം*) data, size, "r");
        എങ്കിൽ (!f)
                തിരികെ -ENOMEM;

        തിരികെ parse_env_filev(f, fname, ap);
}

പൂർണ്ണസംഖ്യ parse_env_data_sentinel(
                സ്ഥിരം അക്ഷരം *data,
                size_t size,
                സ്ഥിരം അക്ഷരം *fname, /* only used for logging */
                ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ load_env_file_തള്ളുക(
                സ്ഥിരം അക്ഷരം *filename, unsigned വരി,
                സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം *മൂല്യം,
                ശൂന്യം *userdata) {

        അക്ഷരം ***m = userdata;
        അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(താക്കോൽ);

        r = check_utf8ness_and_warn(filename, വരി, താക്കോൽ, മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        p = strjoin(താക്കോൽ, "=", മൂല്യം);
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        സ്വതന്ത്രം(മൂല്യം);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ load_env_file(FILE *f, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **m = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f || fname);
        പ്രതിജ്ഞാ(ret);

        r = parse_env_file_internal(f, fname, load_env_file_തള്ളുക, &m);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(m);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ load_env_file_തള്ളുക_pairs(
                സ്ഥിരം അക്ഷരം *filename, unsigned വരി,
                സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം *മൂല്യം,
                ശൂന്യം *userdata) {

        അക്ഷരം ***m = ASSERT_PTR(userdata);
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(താക്കോൽ);

        r = check_utf8ness_and_warn(filename, വരി, താക്കോൽ, മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Check if the key is present */
        വേണ്ടി (അക്ഷരം **t = *m; t && *t; t += 2)
                എങ്കിൽ (streq(t[0], താക്കോൽ)) {
                        എങ്കിൽ (മൂല്യം)
                                തിരികെ free_and_replace(t[1], മൂല്യം);
                        അല്ലെങ്കിൽ
                                തിരികെ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, താക്കോൽ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (മൂല്യം)
                തിരികെ strv_തള്ളുക(m, മൂല്യം);
        അല്ലെങ്കിൽ
                തിരികെ strv_extend(m, "");
}

പൂർണ്ണസംഖ്യ load_env_file_pairs(FILE *f, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **m = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f || fname);
        പ്രതിജ്ഞാ(ret);

        r = parse_env_file_internal(f, fname, load_env_file_തള്ളുക_pairs, &m);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(m);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ load_env_file_pairs_fd(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം ***ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ load_env_file_pairs(f, fname, ret);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ merge_env_file_തള്ളുക(
                സ്ഥിരം അക്ഷരം *filename, unsigned വരി,
                സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം *മൂല്യം,
                ശൂന്യം *userdata) {

        അക്ഷരം ***env = ASSERT_PTR(userdata);
        അക്ഷരം *expanded_മൂല്യം;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(താക്കോൽ);

        എങ്കിൽ (!മൂല്യം) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), വരി, താക്കോൽ);
                തിരികെ 0;
        }

        എങ്കിൽ (!env_name_is_valid(താക്കോൽ)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), വരി, താക്കോൽ);
                സ്വതന്ത്രം(മൂല്യം);
                തിരികെ 0;
        }

        r = replace_env(മൂല്യം,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), വരി, മൂല്യം);

        free_and_replace(മൂല്യം, expanded_മൂല്യം);

        log_debug("%s:%u: setting %s=%s", filename, വരി, താക്കോൽ, മൂല്യം);

        തിരികെ load_env_file_തള്ളുക(filename, വരി, താക്കോൽ, മൂല്യം, env);
}

പൂർണ്ണസംഖ്യ merge_env_file(
                അക്ഷരം ***env,
                FILE *f,
                സ്ഥിരം അക്ഷരം *fname) {

        പ്രതിജ്ഞാ(env);
        പ്രതിജ്ഞാ(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        തിരികെ parse_env_file_internal(f, fname, merge_env_file_തള്ളുക, env);
}

സ്ഥിതം ശൂന്യം env_file_fputs_escaped(FILE *f, സ്ഥിരം അക്ഷരം *p) {
        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(p);

        flockfile(f);

        എങ്കിൽ (string_has_cc(p, ശൂന്യ) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                വേണ്ടി (; *p; p++) {
                        എങ്കിൽ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } അല്ലെങ്കിൽ
                fputs_unlocked(p, f);

        funlockfile(f);
}

ശൂന്യം env_file_fputs_assignment(FILE *f, സ്ഥിരം അക്ഷരം *k, സ്ഥിരം അക്ഷരം *v) {
        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(k);

        എങ്കിൽ (!v)
                തിരികെ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

സ്ഥിതം ശൂന്യം write_env_var(FILE *f, സ്ഥിരം അക്ഷരം *v) {
        സ്ഥിരം അക്ഷരം *p;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(v);

        p = strchr(v, '=');
        എങ്കിൽ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                തിരികെ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

പൂർണ്ണസംഖ്യ write_env_file_label(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം **headers, അക്ഷരം **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(fname);

        എങ്കിൽ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (ശൂന്യം) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                തിരികെ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        എങ്കിൽ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ശൂന്യ, /* created= */ true, label_context);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        STRV_FOREACH(i, headers) {
                പ്രതിജ്ഞാ(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ write_vconsole_conf(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fname, അക്ഷരം **l) {
        അക്ഷരം **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        തിരികെ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ശൂന്യ);
}
