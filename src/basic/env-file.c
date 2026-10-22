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

प्रकारनिर्देश पूर्णाङ्क (*धकेलो_env_func_t)(
                ध्रुव वर्ण *filename,
                unsigned पंक्ति,
                ध्रुव वर्ण *कुंजी,
                वर्ण *मूल्य,
                शून्य *userdata);

स्थिर पूर्णाङ्क parse_env_file_internal(
                FILE *f,
                ध्रुव वर्ण *fname,
                धकेलो_env_func_t धकेलो,
                शून्य *userdata) {

        size_t n_कुंजी = 0, n_मूल्य = 0, last_मूल्य_whitespace = SIZE_MAX, last_कुंजी_whitespace = SIZE_MAX;
        _cleanup_free_ वर्ण *contents = रिक्त, *कुंजी = रिक्त, *मूल्य = रिक्त;
        unsigned पंक्ति = 1;
        पूर्णाङ्क r;

        गणना {
                PRE_कुंजी,
                कुंजी_2,
                PRE_मूल्य,
                मूल्य_2,
                मूल्य_ESCAPE,
                SINGLE_QUOTE_मूल्य,
                DOUBLE_QUOTE_मूल्य,
                DOUBLE_QUOTE_मूल्य_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_कुंजी;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(धकेलो);

        यदि (f)
                r = read_full_stream(f, &contents, रिक्त);
        अन्यथा
                r = read_full_file(fname, &contents, रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (वर्ण *p = contents; *p; p++) {
                वर्ण c = *p;

                विकल्प (state) {

                पक्ष PRE_कुंजी:
                        यदि (strchr(COMMENTS, c))
                                state = COMMENT;
                        अन्यथा यदि (!strchr(WHITESPACE, c)) {
                                state = कुंजी_2;
                                last_कुंजी_whitespace = SIZE_MAX;

                                यदि (!GREEDY_REALLOC(कुंजी, n_कुंजी+2))
                                        प्रत्यागम -ENOMEM;

                                कुंजी[n_कुंजी++] = c;
                        }
                        भङ्ग;

                पक्ष कुंजी_2:
                        यदि (strchr(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                                n_कुंजी = 0;
                        } अन्यथा यदि (c == '=') {
                                state = PRE_मूल्य;
                                last_मूल्य_whitespace = SIZE_MAX;
                        } अन्यथा {
                                यदि (!strchr(WHITESPACE, c))
                                        last_कुंजी_whitespace = SIZE_MAX;
                                अन्यथा यदि (last_कुंजी_whitespace == SIZE_MAX)
                                         last_कुंजी_whitespace = n_कुंजी;

                                यदि (!GREEDY_REALLOC(कुंजी, n_कुंजी+2))
                                        प्रत्यागम -ENOMEM;

                                कुंजी[n_कुंजी++] = c;
                        }

                        भङ्ग;

                पक्ष PRE_मूल्य:
                        यदि (strchr(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                                कुंजी[n_कुंजी] = 0;

                                यदि (मूल्य)
                                        मूल्य[n_मूल्य] = 0;

                                /* strip trailing whitespace from key */
                                यदि (last_कुंजी_whitespace != SIZE_MAX)
                                        कुंजी[last_कुंजी_whitespace] = 0;

                                r = धकेलो(fname, पंक्ति, कुंजी, मूल्य, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                n_कुंजी = 0;
                                मूल्य = रिक्त;
                                n_मूल्य = 0;

                        } अन्यथा यदि (c == '\'')
                                state = SINGLE_QUOTE_मूल्य;
                        अन्यथा यदि (c == '"')
                                state = DOUBLE_QUOTE_मूल्य;
                        अन्यथा यदि (c == '\\')
                                state = मूल्य_ESCAPE;
                        अन्यथा यदि (!strchr(WHITESPACE, c)) {
                                state = मूल्य_2;

                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        प्रत्यागम -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        भङ्ग;

                पक्ष मूल्य_2:
                        यदि (strchr(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;

                                कुंजी[n_कुंजी] = 0;

                                यदि (मूल्य)
                                        मूल्य[n_मूल्य] = 0;

                                /* Chomp off trailing whitespace from value */
                                यदि (last_मूल्य_whitespace != SIZE_MAX)
                                        मूल्य[last_मूल्य_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                यदि (last_कुंजी_whitespace != SIZE_MAX)
                                        कुंजी[last_कुंजी_whitespace] = 0;

                                r = धकेलो(fname, पंक्ति, कुंजी, मूल्य, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                n_कुंजी = 0;
                                मूल्य = रिक्त;
                                n_मूल्य = 0;

                        } अन्यथा यदि (c == '\\') {
                                state = मूल्य_ESCAPE;
                                last_मूल्य_whitespace = SIZE_MAX;
                        } अन्यथा {
                                यदि (!strchr(WHITESPACE, c))
                                        last_मूल्य_whitespace = SIZE_MAX;
                                अन्यथा यदि (last_मूल्य_whitespace == SIZE_MAX)
                                        last_मूल्य_whitespace = n_मूल्य;

                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        प्रत्यागम -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        भङ्ग;

                पक्ष मूल्य_ESCAPE:
                        state = मूल्य_2;

                        यदि (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        प्रत्यागम -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }
                        भङ्ग;

                पक्ष SINGLE_QUOTE_मूल्य:
                        यदि (c == '\'')
                                state = PRE_मूल्य;
                        अन्यथा {
                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        प्रत्यागम -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        भङ्ग;

                पक्ष DOUBLE_QUOTE_मूल्य:
                        यदि (c == '"')
                                state = PRE_मूल्य;
                        अन्यथा यदि (c == '\\')
                                state = DOUBLE_QUOTE_मूल्य_ESCAPE;
                        अन्यथा {
                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        प्रत्यागम -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        भङ्ग;

                पक्ष DOUBLE_QUOTE_मूल्य_ESCAPE:
                        state = DOUBLE_QUOTE_मूल्य;

                        यदि (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        प्रत्यागम -ENOMEM;
                                मूल्य[n_मूल्य++] = c;
                        } अन्यथा यदि (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                यदि (!GREEDY_REALLOC(मूल्य, n_मूल्य+3))
                                        प्रत्यागम -ENOMEM;
                                मूल्य[n_मूल्य++] = '\\';
                                मूल्य[n_मूल्य++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        भङ्ग;

                पक्ष COMMENT:
                        यदि (c == '\\')
                                state = COMMENT_ESCAPE;
                        अन्यथा यदि (strchr(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                        }
                        भङ्ग;

                पक्ष COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        यदि (strchr(NEWLINE, c)) {
                                state = PRE_कुंजी;
                                पंक्ति++;
                        } अन्यथा
                                state = COMMENT;
                        भङ्ग;
                }
        }

        यदि (IN_SET(state,
                   PRE_मूल्य,
                   मूल्य_2,
                   मूल्य_ESCAPE,
                   SINGLE_QUOTE_मूल्य,
                   DOUBLE_QUOTE_मूल्य,
                   DOUBLE_QUOTE_मूल्य_ESCAPE)) {

                कुंजी[n_कुंजी] = 0;

                यदि (मूल्य)
                        मूल्य[n_मूल्य] = 0;

                यदि (state == मूल्य_2)
                        यदि (last_मूल्य_whitespace != SIZE_MAX)
                                मूल्य[last_मूल्य_whitespace] = 0;

                /* strip trailing whitespace from key */
                यदि (last_कुंजी_whitespace != SIZE_MAX)
                        कुंजी[last_कुंजी_whitespace] = 0;

                r = धकेलो(fname, पंक्ति, कुंजी, मूल्य, userdata);
                यदि (r < 0)
                        प्रत्यागम r;

                मूल्य = रिक्त;
        }

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क check_utf8ness_and_warn(
                ध्रुव वर्ण *filename, unsigned पंक्ति,
                ध्रुव वर्ण *कुंजी, वर्ण *मूल्य) {

        प्रतिज्ञा(कुंजी);

        यदि (!utf8_is_valid(कुंजी)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                p = utf8_escape_invalid(कुंजी);
                प्रत्यागम log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), पंक्ति, p);
        }

        यदि (मूल्य && !utf8_is_valid(मूल्य)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                p = utf8_escape_invalid(मूल्य);
                प्रत्यागम log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), पंक्ति, कुंजी, p);
        }

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क parse_env_file_धकेलो(
                ध्रुव वर्ण *filename, unsigned पंक्ति,
                ध्रुव वर्ण *कुंजी, वर्ण *मूल्य,
                शून्य *userdata) {

        ध्रुव वर्ण *k;
        va_list aq, *ap = userdata;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुंजी);

        r = check_utf8ness_and_warn(filename, पंक्ति, कुंजी, मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        va_copy(aq, *ap);

        यावत् ((k = va_arg(aq, ध्रुव वर्ण *))) {
                वर्ण **v;

                v = va_arg(aq, वर्ण **);

                यदि (streq(कुंजी, k)) {
                        va_end(aq);
                        free_and_replace(*v, मूल्य);

                        प्रत्यागम 1;
                }
        }

        va_end(aq);
        मुक्त(मूल्य);

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
        r = parse_env_file_internal(f, fname, parse_env_file_धकेलो, &aq);
        va_end(aq);
        प्रत्यागम r;
}

पूर्णाङ्क parse_env_file_fdv(पूर्णाङ्क fd, ध्रुव वर्ण *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = रिक्त;
        va_list aq;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        यदि (r < 0)
                प्रत्यागम r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_धकेलो, &aq);
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

        यदि (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((शून्य*) data, size, "r");
        यदि (!f)
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

स्थिर पूर्णाङ्क load_env_file_धकेलो(
                ध्रुव वर्ण *filename, unsigned पंक्ति,
                ध्रुव वर्ण *कुंजी, वर्ण *मूल्य,
                शून्य *userdata) {

        वर्ण ***m = userdata;
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुंजी);

        r = check_utf8ness_and_warn(filename, पंक्ति, कुंजी, मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        p = strjoin(कुंजी, "=", मूल्य);
        यदि (!p)
                प्रत्यागम -ENOMEM;

        r = strv_env_replace_consume(m, p);
        यदि (r < 0)
                प्रत्यागम r;

        मुक्त(मूल्य);
        प्रत्यागम 0;
}

पूर्णाङ्क load_env_file(FILE *f, ध्रुव वर्ण *fname, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_धकेलो, &m);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(m);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क load_env_file_धकेलो_pairs(
                ध्रुव वर्ण *filename, unsigned पंक्ति,
                ध्रुव वर्ण *कुंजी, वर्ण *मूल्य,
                शून्य *userdata) {

        वर्ण ***m = ASSERT_PTR(userdata);
        पूर्णाङ्क r;

        प्रतिज्ञा(कुंजी);

        r = check_utf8ness_and_warn(filename, पंक्ति, कुंजी, मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        /* Check if the key is present */
        प्रति (वर्ण **t = *m; t && *t; t += 2)
                यदि (streq(t[0], कुंजी)) {
                        यदि (मूल्य)
                                प्रत्यागम free_and_replace(t[1], मूल्य);
                        अन्यथा
                                प्रत्यागम free_and_strdup(t+1, "");
                }

        r = strv_extend(m, कुंजी);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (मूल्य)
                प्रत्यागम strv_धकेलो(m, मूल्य);
        अन्यथा
                प्रत्यागम strv_extend(m, "");
}

पूर्णाङ्क load_env_file_pairs(FILE *f, ध्रुव वर्ण *fname, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **m = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_धकेलो_pairs, &m);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(m);
        प्रत्यागम 0;
}

पूर्णाङ्क load_env_file_pairs_fd(पूर्णाङ्क fd, ध्रुव वर्ण *fname, वर्ण ***ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम load_env_file_pairs(f, fname, ret);
}

स्थिर पूर्णाङ्क merge_env_file_धकेलो(
                ध्रुव वर्ण *filename, unsigned पंक्ति,
                ध्रुव वर्ण *कुंजी, वर्ण *मूल्य,
                शून्य *userdata) {

        वर्ण ***env = ASSERT_PTR(userdata);
        वर्ण *expanded_मूल्य;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुंजी);

        यदि (!मूल्य) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), पंक्ति, कुंजी);
                प्रत्यागम 0;
        }

        यदि (!env_name_is_valid(कुंजी)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), पंक्ति, कुंजी);
                मुक्त(मूल्य);
                प्रत्यागम 0;
        }

        r = replace_env(मूल्य,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_मूल्य);
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), पंक्ति, मूल्य);

        free_and_replace(मूल्य, expanded_मूल्य);

        log_debug("%s:%u: setting %s=%s", filename, पंक्ति, कुंजी, मूल्य);

        प्रत्यागम load_env_file_धकेलो(filename, पंक्ति, कुंजी, मूल्य, env);
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

        प्रत्यागम parse_env_file_internal(f, fname, merge_env_file_धकेलो, env);
}

स्थिर शून्य env_file_fputs_escaped(FILE *f, ध्रुव वर्ण *p) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(p);

        flockfile(f);

        यदि (string_has_cc(p, रिक्त) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                प्रति (; *p; p++) {
                        यदि (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } अन्यथा
                fputs_unlocked(p, f);

        funlockfile(f);
}

शून्य env_file_fputs_assignment(FILE *f, ध्रुव वर्ण *k, ध्रुव वर्ण *v) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(k);

        यदि (!v)
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
        यदि (!p) {
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

        यदि (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_free_ वर्ण *p = रिक्त;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        यदि (r < 0) {
                यदि (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (शून्य) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                प्रत्यागम r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        यदि (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ रिक्त, /* created= */ true, label_context);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        r = fchmod_umask(fileno(f), 0644);
        यदि (r < 0)
                प्रत्यागम r;

        STRV_FOREACH(i, headers) {
                प्रतिज्ञा(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        यदि (r < 0)
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
