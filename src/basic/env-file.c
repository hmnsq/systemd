/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-file.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "label-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "tmpfile-util.h"
#अन्तर्भाव "utf8.h"

प्रकारनिर्देश पूर्णाङ्क (*प्रक्षिप_env_func_t)(
                ध्रुव वर्ण *filename,
                unsigned पङ्क्तिः,
                ध्रुव वर्ण *कुञ्चिका,
                वर्ण *मानम्,
                शून्य *userdata);

स्थिर पूर्णाङ्क parse_env_file_internal(
                FILE *f,
                ध्रुव वर्ण *fname,
                प्रक्षिप_env_func_t प्रक्षिप,
                शून्य *userdata) {

        size_t n_कुञ्चिका = 0, n_मानम् = 0, last_मानम्_whitespace = SIZE_MAX, last_कुञ्चिका_whitespace = SIZE_MAX;
        _cleanup_free_ वर्ण *contents = रिक्त, *कुञ्चिका = रिक्त, *मानम् = रिक्त;
        unsigned पङ्क्तिः = 1;
        पूर्णाङ्क r;

        गणना {
                PRE_कुञ्चिका,
                कुञ्चिका_2,
                PRE_मानम्,
                मानम्_2,
                मानम्_ESCAPE,
                SINGLE_QUOTE_मानम्,
                DOUBLE_QUOTE_मानम्,
                DOUBLE_QUOTE_मानम्_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_कुञ्चिका;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(प्रक्षिप);

        जइ (f)
                r = read_full_stream(f, &contents, रिक्त);
        अण्णहा
                r = read_full_file(fname, &contents, रिक्त);
        जइ (r < 0)
                प्रत्यागम r;

        प्रति (वर्ण *p = contents; *p; p++) {
                वर्ण c = *p;

                विकल्प (state) {

                पक्ष PRE_कुञ्चिका:
                        जइ (strchr(COMMENTS, c))
                                state = COMMENT;
                        अण्णहा जइ (!strchr(WHITESPACE, c)) {
                                state = कुञ्चिका_2;
                                last_कुञ्चिका_whitespace = SIZE_MAX;

                                जइ (!GREEDY_REALLOC(कुञ्चिका, n_कुञ्चिका+2))
                                        प्रत्यागम -ENOMEM;

                                कुञ्चिका[n_कुञ्चिका++] = c;
                        }
                        भङ्ग;

                पक्ष कुञ्चिका_2:
                        जइ (strchr(NEWLINE, c)) {
                                state = PRE_कुञ्चिका;
                                पङ्क्तिः++;
                                n_कुञ्चिका = 0;
                        } अण्णहा जइ (c == '=') {
                                state = PRE_मानम्;
                                last_मानम्_whitespace = SIZE_MAX;
                        } अण्णहा {
                                जइ (!strchr(WHITESPACE, c))
                                        last_कुञ्चिका_whitespace = SIZE_MAX;
                                अण्णहा जइ (last_कुञ्चिका_whitespace == SIZE_MAX)
                                         last_कुञ्चिका_whitespace = n_कुञ्चिका;

                                जइ (!GREEDY_REALLOC(कुञ्चिका, n_कुञ्चिका+2))
                                        प्रत्यागम -ENOMEM;

                                कुञ्चिका[n_कुञ्चिका++] = c;
                        }

                        भङ्ग;

                पक्ष PRE_मानम्:
                        जइ (strchr(NEWLINE, c)) {
                                state = PRE_कुञ्चिका;
                                पङ्क्तिः++;
                                कुञ्चिका[n_कुञ्चिका] = 0;

                                जइ (मानम्)
                                        मानम्[n_मानम्] = 0;

                                /* strip trailing whitespace from key */
                                जइ (last_कुञ्चिका_whitespace != SIZE_MAX)
                                        कुञ्चिका[last_कुञ्चिका_whitespace] = 0;

                                r = प्रक्षिप(fname, पङ्क्तिः, कुञ्चिका, मानम्, userdata);
                                जइ (r < 0)
                                        प्रत्यागम r;

                                n_कुञ्चिका = 0;
                                मानम् = रिक्त;
                                n_मानम् = 0;

                        } अण्णहा जइ (c == '\'')
                                state = SINGLE_QUOTE_मानम्;
                        अण्णहा जइ (c == '"')
                                state = DOUBLE_QUOTE_मानम्;
                        अण्णहा जइ (c == '\\')
                                state = मानम्_ESCAPE;
                        अण्णहा जइ (!strchr(WHITESPACE, c)) {
                                state = मानम्_2;

                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+2))
                                        प्रत्यागम -ENOMEM;

                                मानम्[n_मानम्++] = c;
                        }

                        भङ्ग;

                पक्ष मानम्_2:
                        जइ (strchr(NEWLINE, c)) {
                                state = PRE_कुञ्चिका;
                                पङ्क्तिः++;

                                कुञ्चिका[n_कुञ्चिका] = 0;

                                जइ (मानम्)
                                        मानम्[n_मानम्] = 0;

                                /* Chomp off trailing whitespace from value */
                                जइ (last_मानम्_whitespace != SIZE_MAX)
                                        मानम्[last_मानम्_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                जइ (last_कुञ्चिका_whitespace != SIZE_MAX)
                                        कुञ्चिका[last_कुञ्चिका_whitespace] = 0;

                                r = प्रक्षिप(fname, पङ्क्तिः, कुञ्चिका, मानम्, userdata);
                                जइ (r < 0)
                                        प्रत्यागम r;

                                n_कुञ्चिका = 0;
                                मानम् = रिक्त;
                                n_मानम् = 0;

                        } अण्णहा जइ (c == '\\') {
                                state = मानम्_ESCAPE;
                                last_मानम्_whitespace = SIZE_MAX;
                        } अण्णहा {
                                जइ (!strchr(WHITESPACE, c))
                                        last_मानम्_whitespace = SIZE_MAX;
                                अण्णहा जइ (last_मानम्_whitespace == SIZE_MAX)
                                        last_मानम्_whitespace = n_मानम्;

                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+2))
                                        प्रत्यागम -ENOMEM;

                                मानम्[n_मानम्++] = c;
                        }

                        भङ्ग;

                पक्ष मानम्_ESCAPE:
                        state = मानम्_2;

                        जइ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+2))
                                        प्रत्यागम -ENOMEM;

                                मानम्[n_मानम्++] = c;
                        }
                        भङ्ग;

                पक्ष SINGLE_QUOTE_मानम्:
                        जइ (c == '\'')
                                state = PRE_मानम्;
                        अण्णहा {
                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+2))
                                        प्रत्यागम -ENOMEM;

                                मानम्[n_मानम्++] = c;
                        }

                        भङ्ग;

                पक्ष DOUBLE_QUOTE_मानम्:
                        जइ (c == '"')
                                state = PRE_मानम्;
                        अण्णहा जइ (c == '\\')
                                state = DOUBLE_QUOTE_मानम्_ESCAPE;
                        अण्णहा {
                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+2))
                                        प्रत्यागम -ENOMEM;

                                मानम्[n_मानम्++] = c;
                        }

                        भङ्ग;

                पक्ष DOUBLE_QUOTE_मानम्_ESCAPE:
                        state = DOUBLE_QUOTE_मानम्;

                        जइ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+2))
                                        प्रत्यागम -ENOMEM;
                                मानम्[n_मानम्++] = c;
                        } अण्णहा जइ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                जइ (!GREEDY_REALLOC(मानम्, n_मानम्+3))
                                        प्रत्यागम -ENOMEM;
                                मानम्[n_मानम्++] = '\\';
                                मानम्[n_मानम्++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        भङ्ग;

                पक्ष COMMENT:
                        जइ (c == '\\')
                                state = COMMENT_ESCAPE;
                        अण्णहा जइ (strchr(NEWLINE, c)) {
                                state = PRE_कुञ्चिका;
                                पङ्क्तिः++;
                        }
                        भङ्ग;

                पक्ष COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        जइ (strchr(NEWLINE, c)) {
                                state = PRE_कुञ्चिका;
                                पङ्क्तिः++;
                        } अण्णहा
                                state = COMMENT;
                        भङ्ग;
                }
        }

        जइ (IN_SET(state,
                   PRE_मानम्,
                   मानम्_2,
                   मानम्_ESCAPE,
                   SINGLE_QUOTE_मानम्,
                   DOUBLE_QUOTE_मानम्,
                   DOUBLE_QUOTE_मानम्_ESCAPE)) {

                कुञ्चिका[n_कुञ्चिका] = 0;

                जइ (मानम्)
                        मानम्[n_मानम्] = 0;

                जइ (state == मानम्_2)
                        जइ (last_मानम्_whitespace != SIZE_MAX)
                                मानम्[last_मानम्_whitespace] = 0;

                /* strip trailing whitespace from key */
                जइ (last_कुञ्चिका_whitespace != SIZE_MAX)
                        कुञ्चिका[last_कुञ्चिका_whitespace] = 0;

                r = प्रक्षिप(fname, पङ्क्तिः, कुञ्चिका, मानम्, userdata);
                जइ (r < 0)
                        प्रत्यागम r;

                मानम् = रिक्त;
        }

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क check_utf8ness_and_warn(
                ध्रुव वर्ण *filename, unsigned पङ्क्तिः,
                ध्रुव वर्ण *कुञ्चिका, वर्ण *मानम्) {

        प्रतिज्ञा(कुञ्चिका);

        जइ (!utf8_is_valid(कुञ्चिका)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                p = utf8_escape_invalid(कुञ्चिका);
                प्रत्यागम log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), पङ्क्तिः, p);
        }

        जइ (मानम् && !utf8_is_valid(मानम्)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                p = utf8_escape_invalid(मानम्);
                प्रत्यागम log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), पङ्क्तिः, कुञ्चिका, p);
        }

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क parse_env_file_प्रक्षिप(
                ध्रुव वर्ण *filename, unsigned पङ्क्तिः,
                ध्रुव वर्ण *कुञ्चिका, वर्ण *मानम्,
                शून्य *userdata) {

        ध्रुव वर्ण *k;
        va_list aq, *ap = userdata;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुञ्चिका);

        r = check_utf8ness_and_warn(filename, पङ्क्तिः, कुञ्चिका, मानम्);
        जइ (r < 0)
                प्रत्यागम r;

        va_copy(aq, *ap);

        जाव ((k = va_arg(aq, ध्रुव वर्ण *))) {
                वर्ण **v;

                v = va_arg(aq, वर्ण **);

                जइ (streq(कुञ्चिका, k)) {
                        va_end(aq);
                        free_and_replace(*v, मानम्);

                        प्रत्यागम 1;
                }
        }

        va_end(aq);
        मुक्त(मानम्);

        प्रत्यागम 0;
}

पूर्णाङ्क parse_env_filev(
                FILE *f,
                ध्रुव वर्ण *fname,
                va_list ap) {

        पूर्णाङ्क r;
        va_list aq;

        प्रतिज्ञा(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_प्रक्षिप, &aq);
        va_end(aq);
        प्रत्यागम r;
}

पूर्णाङ्क parse_env_file_fdv(पूर्णाङ्क fd, ध्रुव वर्ण *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = रिक्त;
        va_list aq;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        जइ (r < 0)
                प्रत्यागम r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_प्रक्षिप, &aq);
        va_end(aq);
        प्रत्यागम r;
}

पूर्णाङ्क parse_env_file_sentinel(
                FILE *f,
                ध्रुव वर्ण *fname,
                ...) {

        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        प्रत्यागम r;
}

पूर्णाङ्क parse_env_file_fd_sentinel(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *fname, /* only used for logging */
                ...) {

        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        प्रत्यागम r;
}

पूर्णाङ्क parse_env_datav(
                ध्रुव वर्ण *data,
                size_t size,
                ध्रुव वर्ण *fname, /* only used for logging */
                va_list ap) {

        प्रतिज्ञा(data);

        जइ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((शून्य*) data, size, "r");
        जइ (!f)
                प्रत्यागम -ENOMEM;

        प्रत्यागम parse_env_filev(f, fname, ap);
}

पूर्णाङ्क parse_env_data_sentinel(
                ध्रुव वर्ण *data,
                size_t size,
                ध्रुव वर्ण *fname, /* only used for logging */
                ...) {

        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        प्रत्यागम r;
}

स्थिर पूर्णाङ्क load_env_file_प्रक्षिप(
                ध्रुव वर्ण *filename, unsigned पङ्क्तिः,
                ध्रुव वर्ण *कुञ्चिका, वर्ण *मानम्,
                शून्य *userdata) {

        वर्ण ***m = userdata;
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुञ्चिका);

        r = check_utf8ness_and_warn(filename, पङ्क्तिः, कुञ्चिका, मानम्);
        जइ (r < 0)
                प्रत्यागम r;

        p = strjoin(कुञ्चिका, "=", मानम्);
        जइ (!p)
                प्रत्यागम -ENOMEM;

        r = strv_env_replace_consume(m, p);
        जइ (r < 0)
                प्रत्यागम r;

        मुक्त(मानम्);
        प्रत्यागम 0;
}

पूर्णाङ्क load_env_file(FILE *f, ध्रुव वर्ण *fname, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_प्रक्षिप, &m);
        जइ (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(m);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क load_env_file_प्रक्षिप_pairs(
                ध्रुव वर्ण *filename, unsigned पङ्क्तिः,
                ध्रुव वर्ण *कुञ्चिका, वर्ण *मानम्,
                शून्य *userdata) {

        वर्ण ***m = ASSERT_PTR(userdata);
        पूर्णाङ्क r;

        प्रतिज्ञा(कुञ्चिका);

        r = check_utf8ness_and_warn(filename, पङ्क्तिः, कुञ्चिका, मानम्);
        जइ (r < 0)
                प्रत्यागम r;

        /* Check if the key is present */
        प्रति (वर्ण **t = *m; t && *t; t += 2)
                जइ (streq(t[0], कुञ्चिका)) {
                        जइ (मानम्)
                                प्रत्यागम free_and_replace(t[1], मानम्);
                        अण्णहा
                                प्रत्यागम free_and_strdup(t+1, "");
                }

        r = strv_extend(m, कुञ्चिका);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (मानम्)
                प्रत्यागम strv_प्रक्षिप(m, मानम्);
        अण्णहा
                प्रत्यागम strv_extend(m, "");
}

पूर्णाङ्क load_env_file_pairs(FILE *f, ध्रुव वर्ण *fname, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_प्रक्षिप_pairs, &m);
        जइ (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(m);
        प्रत्यागम 0;
}

पूर्णाङ्क load_env_file_pairs_fd(पूर्णाङ्क fd, ध्रुव वर्ण *fname, वर्ण ***ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम load_env_file_pairs(f, fname, ret);
}

स्थिर पूर्णाङ्क merge_env_file_प्रक्षिप(
                ध्रुव वर्ण *filename, unsigned पङ्क्तिः,
                ध्रुव वर्ण *कुञ्चिका, वर्ण *मानम्,
                शून्य *userdata) {

        वर्ण ***env = ASSERT_PTR(userdata);
        वर्ण *expanded_मानम्;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुञ्चिका);

        जइ (!मानम्) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), पङ्क्तिः, कुञ्चिका);
                प्रत्यागम 0;
        }

        जइ (!env_name_is_valid(कुञ्चिका)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), पङ्क्तिः, कुञ्चिका);
                मुक्त(मानम्);
                प्रत्यागम 0;
        }

        r = replace_env(मानम्,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_मानम्);
        जइ (r < 0)
                प्रत्यागम log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), पङ्क्तिः, मानम्);

        free_and_replace(मानम्, expanded_मानम्);

        log_debug("%s:%u: setting %s=%s", filename, पङ्क्तिः, कुञ्चिका, मानम्);

        प्रत्यागम load_env_file_प्रक्षिप(filename, पङ्क्तिः, कुञ्चिका, मानम्, env);
}

पूर्णाङ्क merge_env_file(
                वर्ण ***env,
                FILE *f,
                ध्रुव वर्ण *fname) {

        प्रतिज्ञा(env);
        प्रतिज्ञा(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        प्रत्यागम parse_env_file_internal(f, fname, merge_env_file_प्रक्षिप, env);
}

स्थिर शून्य env_file_fputs_escaped(FILE *f, ध्रुव वर्ण *p) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(p);

        flockfile(f);

        जइ (string_has_cc(p, रिक्त) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                प्रति (; *p; p++) {
                        जइ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } अण्णहा
                fputs_unlocked(p, f);

        funlockfile(f);
}

शून्य env_file_fputs_assignment(FILE *f, ध्रुव वर्ण *k, ध्रुव वर्ण *v) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(k);

        जइ (!v)
                प्रत्यागम;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

स्थिर शून्य write_env_var(FILE *f, ध्रुव वर्ण *v) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(v);

        p = strchr(v, '=');
        जइ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                प्रत्यागम;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

पूर्णाङ्क write_env_file_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fname, वर्ण **headers, वर्ण **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(fname);

        जइ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_free_ वर्ण *p = रिक्त;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        जइ (r < 0) {
                जइ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (शून्य) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                प्रत्यागम r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        जइ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ रिक्त, /* created= */ true, label_context);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        r = fchmod_umask(fileno(f), 0644);
        जइ (r < 0)
                प्रत्यागम r;

        STRV_FOREACH(i, headers) {
                प्रतिज्ञा(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        जइ (r < 0)
                प्रत्यागम r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        प्रत्यागम 0;
}

पूर्णाङ्क write_vconsole_conf(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fname, वर्ण **l) {
        वर्ण **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        प्रत्यागम write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ रिक्त);
}
